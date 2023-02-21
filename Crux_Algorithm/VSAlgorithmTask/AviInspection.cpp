#include "stdafx.h"
#include "AviInspection.h"
#include "AviDefineInspect.h"
#include "DllInterface.h"

CMatBufferResultManager	cMemResultBuff;

void ImageSave(cv::Mat& MatSrcBuffer, TCHAR* strPath, ...);
void ImageAsyncSaveJPG(cv::Mat& MatSrcBuffer, const char* strPath);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

using namespace Concurrency;

IMPLEMENT_DYNCREATE(AviInspection, CWinThread)

AviInspection::AviInspection(void)
{
	//cMemResult[0] = NULL;
}

AviInspection::~AviInspection(void)
{
}

bool AviInspection::DrawDefectImage(CString strPanelID, cv::Mat(*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], ResultPanelData& resultPanelData)
{
	CCPUTimer timerDrawRect;
	timerDrawRect.Start();

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// [Draw & Save] 불량 표시
		cv::Scalar color;

		// Point & Line만 추가하면 됨 ( Mura 외각선 그림 )
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
		case E_DEFECT_JUDGEMENT_POINT_DARK:
		case E_DEFECT_JUDGEMENT_POINT_GROUP_DARK:
			color = cv::Scalar(255, 0, 0);		// Red
			break;

		case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
		case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
		case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:
		case E_DEFECT_JUDGEMENT_MURA_MULT_BP:	// Point 불량 Gray 패턴 근접인경우 ( 백점 비슷 )
			color = cv::Scalar(0, 255, 0);		// Green
			break;

		case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:
		case E_DEFECT_JUDGEMENT_RETEST_POINT_DARK:
		case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:
			color = cv::Scalar(0, 0, 255);		// Blue
			break;

		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT_MULT:
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT_MULT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK_MULT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK_MULT:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
		case E_DEFECT_JUDGEMENT_LINE_DGS:
		case E_DEFECT_JUDGEMENT_LINE_DGS_X:
		case E_DEFECT_JUDGEMENT_LINE_DGS_Y:
		case E_DEFECT_JUDGEMENT_XLINE_SPOT:
		case E_DEFECT_JUDGEMENT_YLINE_SPOT:
		case E_DEFECT_JUDGEMENT_LINE_X_EDGE_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_EDGE_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_CRACK_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_CRACK_LEFT:
		case E_DEFECT_JUDGEMENT_LINE_CRACK_BOTH:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_RETEST_LINE_BRIGHT:
		case E_DEFECT_JUDGEMENT_RETEST_LINE_DARK:
			color = cv::Scalar(255, 105, 180);	// Hot Pink
			break;

		case E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL:
		case E_DEFECT_JUDGEMENT_DISPLAY_DARK:
		case E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT:
		case E_DEFECT_JUDGEMENT_DUST_GROUP:
			color = cv::Scalar(210, 105, 30);		// Chocolate
			break;

		default:
			color = cv::Scalar(0, 0, 0);			// Black
			break;
		}

		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int nCamNum = resultPanelData.m_ListDefectInfo[i].Camera_No;

		// 17.03.29 Mura & SVI 사각형 그리지 않기 ( Alg에서 외곽선 따로 그림 )
		// 영상 잘라내는 부분은 해야 함
		int nOffSet = 5, nThickness = 1;
		// 		// 실제 Defect 영역
		// 		int nDefStartX, nDefStartY, nDefWidth, nDefHeight;
		// 		
		// 		nDefStartX = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nOffSet;
		// 		nDefStartY = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y - nOffSet;
		// 		// 절대값 임시 사용 (by CWH)
		// 		nDefWidth  = abs(resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X) + (nOffSet * 2) + 1;
		// 		nDefHeight = abs(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y) + (nOffSet * 2) + 1;
		cv::Point ptDefectArea[4] = { (0, 0), };
		ptDefectArea[E_CORNER_LEFT_TOP].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_TOP].x - nOffSet;
		ptDefectArea[E_CORNER_LEFT_TOP].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_TOP].y - nOffSet;
		ptDefectArea[E_CORNER_RIGHT_TOP].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_TOP].x + nOffSet;
		ptDefectArea[E_CORNER_RIGHT_TOP].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_TOP].y - nOffSet;
		ptDefectArea[E_CORNER_RIGHT_BOTTOM].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM].x + nOffSet;
		ptDefectArea[E_CORNER_RIGHT_BOTTOM].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM].y + nOffSet;
		ptDefectArea[E_CORNER_LEFT_BOTTOM].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_BOTTOM].x - nOffSet;
		ptDefectArea[E_CORNER_LEFT_BOTTOM].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_BOTTOM].y + nOffSet;

		// Mura - 미니맵에 Rect 안그림
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_MURA_START	&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_MURA_END)
		{
			// 미니맵에 Rect 안그림
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP 
				&& resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT
				&& resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_EMD_DARK
				&& resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT
				&& resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_NUGI)

				resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
		}
		// Mura Retest - 미니맵에 Rect 안그림
		else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_MURA ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_MURA_BRIGHT)
		{
			// 미니맵에 Rect 안그림
			resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
		}
		// APP - 미니맵에 Rect 안그림
		else if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_APP_START		&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_APP_END)
		{
			// 미니맵에 Rect 안그림
			resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
		}


		// 무라 이외의 불량 / 사각형 그리기
		if (resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect)
		{
			// 사각형 그리기
			//cv::rectangle(MatDrawBuffer[nImgNum][nCamNum], cv::Rect(nDefStartX, nDefStartY, nDefWidth, nDefHeight), color);

			//choi 리포트

// 			cv::Point pOffset_LT(-200, -200);
// 			cv::Point pOffset_RT(200, -200);
// 			cv::Point pOffset_RB(200, 200);
// 			cv::Point pOffset_LB(-200, 200);

// 			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_LEFT_TOP]+ pOffset_LT,		ptDefectArea[E_CORNER_RIGHT_TOP]+ pOffset_RT,		color, nThickness);
// 			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_RIGHT_TOP]+ pOffset_RT,		ptDefectArea[E_CORNER_RIGHT_BOTTOM]+ pOffset_RB,	color, nThickness);
// 			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_RIGHT_BOTTOM]+ pOffset_RB,	ptDefectArea[E_CORNER_LEFT_BOTTOM]+ pOffset_LB,		color, nThickness);
// 			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_LEFT_BOTTOM]+ pOffset_LB,	ptDefectArea[E_CORNER_LEFT_TOP]+ pOffset_LT,		color, nThickness);


			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_LEFT_TOP], ptDefectArea[E_CORNER_RIGHT_TOP], color, nThickness);
			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_RIGHT_TOP], ptDefectArea[E_CORNER_RIGHT_BOTTOM], color, nThickness);
			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_RIGHT_BOTTOM], ptDefectArea[E_CORNER_LEFT_BOTTOM], color, nThickness);
			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_LEFT_BOTTOM], ptDefectArea[E_CORNER_LEFT_TOP], color, nThickness);

			//choi 리포트
// 			char str[256] = { 0, };
// 			sprintf_s(str, sizeof(str), "%s%d", "DefectNum_", i+1);
// 			cv::Point ptRT(ptDefectArea[E_CORNER_RIGHT_TOP].x -220, ptDefectArea[E_CORNER_RIGHT_TOP].y -220);
// 			cv::putText(MatDrawBuffer[nImgNum][nCamNum], str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 4, cv::Scalar(255, 0, 0),4);
		}
	}



	timerDrawRect.End();
	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Draw Rect tact time %.2f"), timerDrawRect.GetTime() / 1000.);

	return true;
}

// 외곽 부분 처리
long AviInspection::makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio)
{
	// 예외 처리
	if (dAlgPara == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	wchar_t wstrID[MAX_PATH] = { 0, };
	swprintf(wstrID, _T("%s"), LogicPara.tszPanelID);

	// 장비 타입
	int nEqpType = theApp.m_Config.GetEqpType();

	// 셋팅 플래그 On인 경우
	if (dAlgPara[E_PARA_ROUND_SETTING] > 0 || dAlgPara[E_PARA_CHOLE_SETTING] > 0)
	{
		// 코너 등록
		nErrorCode = Align_SetFindContour_(LogicPara.MatOrgImage, theApp.GetRoundROI(nImageNum, nCameraNum), theApp.GetCHoleROI(nImageNum, nCameraNum),
			theApp.GetRndROICnt(nImageNum, nCameraNum), theApp.GetCHoleROICnt(nImageNum, nCameraNum), dAlgPara, nAlgImg, nCameraNum, nRatio, nEqpType, theApp.GetRoundPath());

		// 정상적으로 동작한 경우, 변경
		// 오류 발생한 경우, 오류 코드 넘기기
		if (nErrorCode == E_ERROR_CODE_TRUE)
			nErrorCode = E_ERROR_CODE_ALIGN_ROUND_SETTING;
	}

	// 셋팅 플래그 Off인 경우
	else
	{
		// Alg 로그용
		wchar_t strAlgLog[MAX_PATH] = { 0, };
		swprintf(strAlgLog, _T("ID:%s\tPat:%s"), LogicPara.tszPanelID, theApp.GetGrabStepName(nImageNum));

		// Dust 패턴이 아닌 경우
		if (nAlgImg != E_IMAGE_CLASSIFY_AVI_DUST)
		{
			nErrorCode = Align_FindFillOutArea(LogicPara.MatOrgImage, LogicPara.MatBKG, theApp.m_pGrab_Step[nImageNum].tRoundSet, theApp.m_pGrab_Step[nImageNum].tCHoleSet, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nAlgImg], stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nAlgImg], stThrdAlignInfo.tCHoleAlignData->bCHoleAD[nAlgImg],
				dAlgPara, nAlgImg, nCameraNum, nRatio, nEqpType, strAlgLog, wstrID);
		}

		// Dust 패턴인 경우
		else
		{
			nErrorCode = Align_FindFillOutAreaDust(LogicPara.MatOrgImage, stThrdAlignInfo.ptCorner, stThrdAlignInfo.dAlignTheta, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nAlgImg],
				theApp.m_pGrab_Step[nImageNum].tRoundSet, theApp.m_pGrab_Step[nImageNum].tCHoleSet, dAlgPara, nAlgImg, nRatio, strAlgLog, wstrID);
		}
	}

	return nErrorCode;
}

bool AviInspection::Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
	const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, const int nCamNum,
	ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStageNo)
{
	resultPanelData.m_ResultHeader.SetInspectEndTime();

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judgement Start. Panel ID : %s"), strPanelID);

	// UI - 검사 실행 체크
	// 검사 동작한 경우만 실행
	// Grab만 한 경우 동작 안함 ( 불량 없음 )
	if (bUseInspect)
	{
		// Point -> Line 판정
		// 추후, Line 알고리즘 안정화 시, 함수 제거
		//JudgementPointToLine(WrtResultInfo, resultPanelData, nImageW, nImageH);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge Point To Line End. Panel ID : %s"), strPanelID);

		// 2차 판정 LCP
		//
		//////////////////////////////////////////////////////////////////////////
		//검사 확인 Path 설정
		wchar_t wstrInspConfirmPath[MAX_PATH] = { 0, };

		CString				strPanelID_;
		strPanelID_.Format(_T("%s"), strPanelID);

		CString		stBasic_Path;
		stBasic_Path.Format(_T("%s\\검사확인"), theApp.m_Config.GetNetworkDrivePath() + strDrive);
		//CString		stBasic_Path = _T("D:\\NetworkDrive\\D_Drive\\검사확인");
		//if (nCommonPara[1] > 0) CreateDirectory(stBasic_Path, NULL);
		if (true) CreateDirectory(stBasic_Path, NULL); //LCP 결과이미지
		swprintf(wstrInspConfirmPath, _T("%s\\%s"), (LPCWSTR)stBasic_Path, (LPCWSTR)strPanelID_);
		//if (nCommonPara[1] > 0) CreateDirectory(wstrInspConfirmPath, NULL);
		if (true ) CreateDirectory(wstrInspConfirmPath, NULL); //LCP 결과이미지

		int nCameraNum_ = nCamNum;

		double dMoi_per = 0.0; //수분 Log 확인용
		double dVari_per = 0.0; //수분 Log 확인용
		//////////////////////////////////////////////////////////////////////////

		// Report 삭제
		JudgementDelReport(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Del Report. Panel ID : %s"), strPanelID);
		
	
		//JudgementSpot(resultPanelData);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Spot. Panel ID : %s"), strPanelID);

		// PNZ:17.11.27 먼저 중복 부분 제거후 Line Merge
		// 파라미터 받아오기
		double* dAlignPara = theApp.GetAlignParameter(0);

		DeleteOverlapDefect_LCP_Moisture_Substance(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - DeleteOverlapDefect LCP Moisture Substance End. Panel ID : %s - %d"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount());

		JudgementMoistureDefect(resultPanelData, dAlignPara, dMoi_per , m_stThrdAlignInfo.matMask_Active, m_stThrdAlignInfo.rcAlignCellROI);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Judgement Moisture Defect End. Panel ID : %s - %d [Moisture Per : %f]"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount() , dMoi_per);

		JudgementVarianceDefect(resultPanelData, dAlignPara, dVari_per,m_stThrdAlignInfo.matMask_Active, m_stThrdAlignInfo.rcAlignCellROI);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Judgement Variance Defect End. Panel ID : %s - %d [Variance Per : %f]"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount(), dVari_per);
		
		DeleteOverlapDefect_LCP_Candidate_Substance(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - DeleteOverlapDefect LCP Candidate Substance End. Panel ID : %s - %d"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount());

		

		JudgementDeleteChips(resultPanelData, dAlignPara, m_stThrdAlignInfo.matSrc_DrawMain, m_stThrdAlignInfo.rcAlignCellROI , strPanelID,strDrive);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Judgement Delete Chips End. Panel ID : %s - %d"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount());
		
		Judgement_DefectColor_Trans(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Judgement Transparency DefectColor Trans End. Panel ID : %s - %d"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount());

		/////////////////////////////////////////////0/////////////////////////////
		Report_FinalDefect(MatColorImage, wstrInspConfirmPath, strPanelID_, WrtResultInfo.GetCamResolution(nCameraNum_), nCameraNum_, resultPanelData, dAlignPara);
		//////////////////////////////////////////////////////////////////////////

		Judgement_Defect_Use_Result_Trans(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Judgement Defect Use Result Trans End. Panel ID : %s - %d"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount());

		cv::imwrite("D:\\DefectMapImage.jpg", MatColorImage[theApp.GetImageNum((int)dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_MAIN])][nCamNum]);
//		Delet Some Line Defect 판정
//		강라인 주변 약라인 제거
// 		JudgementDeletLineDefect(resultPanelData, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Del Line Defect End. Panel ID : %s - %d"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount());
// 
// 		// Same Pattern Defect Merge
// 		JudgementSamePatternDefectMerge(resultPanelData);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Same Pattern Defect Merge End. Panel ID : %s"), strPanelID);
// 
// 		// Line Classification
// 		JudgementClassifyLineDefect(resultPanelData, MatOrgImage);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Line Defect Classification End. Panel ID : %s"), strPanelID);
// 
// 		// 6.39QHD Notch Y Line Delete
// 		JudgementNotchDefect(resultPanelData, MatOrgImage, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - NotchLine Defect End. Panel ID : %s - %d"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount());
// 
// 		// Crack 판정
// 		JudgementCrack(resultPanelData);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Crack Pattern Judge End. Panel ID : %s"), strPanelID);
// 
// 		// PCD CRACK 판정 ( 비검사 영역 5.99" )
// 		JudgementPCDCrack(resultPanelData, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - PCD Crack End. Panel ID : %s"), strPanelID);
// 
// 		// 2018-07-31 Camera Tap에 인한 과검 제거
// 		JudgementCameraTapOverKill(resultPanelData, MatOrgImage, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Camera Tap OverKill Delet End. Panel ID : %s"), strPanelID);
// 
// 		// DGS 판정
// 		JudgementNewDGS(resultPanelData);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - DGS End. Panel ID : %s"), strPanelID);
// 
// 		JudgementDGS_Vth(resultPanelData);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - DGS Vth End. Panel ID : %s"), strPanelID);
// 
// 		// Weak Plan B Test
// 		//JudgementSpecialGrayLineDefect(resultPanelData);
// 		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Plan B End. Panel ID : %s"), strPanelID);
// 
// 		// 2018-03-16 ver. 0.0.3 PNZ
// 		// 기포 제거 알고리즘
// 		DeleteOverlapDefect_DimpleDelet(resultPanelData, MatOrgImage, dAlignPara);		//choikwangil boe11 임시 주석 19 08 09
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Dimple Overkill Defect End. Panel ID : %s"), strPanelID);
// 
// 		// 2018-04-19 ver. 0.0.3 PNZ
// 		// 한도내 Spot 제거 알고리즘
// 		//중복제거 뒤로 이동
// // 		DeleteOverlapDefect_SpecInSpotDelet(resultPanelData, MatOrgImage, dAlignPara);
// // 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Spot Overkill Defect End. Panel ID : %s"), strPanelID);
// 
// 		//개발중
// 		//DeleteOverlapDefect_BlackHole(resultPanelData, MatOrgImage, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Black Hole Spot Overkill Defect End. Panel ID : %s"), strPanelID);
// 
// 		// 2018-05-17 ver. 0.0.1 PNZ
// 		// 암점 확인 알고리즘
// 		DeleteOverlapDefect_DustDelet(resultPanelData, MatOrgImage, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Dust Defect End. Panel ID : %s"), strPanelID);
// 
// 		// 2018-04-19 ver. 0.0.1 PNZ
// 		// 한도내 Spot 제거 알고리즘
// 		DeleteOverlapDefect_BlackSmallDelet(resultPanelData, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Black Pattern Small Point Defect End. Panel ID : %s"), strPanelID);
// 
// 		// white spot 제거전 judge
// 		//Judge_Defect_BP_WSpot(resultPanelData, dAlignPara);
// 
// 		DeleteOverlapDefect_SpecInSpotDelet(resultPanelData, MatOrgImage, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Spot Overkill Defect End. Panel ID : %s"), strPanelID); //05_26 choi white mura 중복 판정 전으로 옮김
// 
// 	 	// 중복 좌표 제거
// 	 	DeleteOverlapDefect(resultPanelData, dAlignPara);
// 	 	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Overlap Defect End. Panel ID : %s"), strPanelID);
// 
// 		
// 		
// 		//choikwangil
// 		//Judge_DefectSize(resultPanelData, dAlignPara);
// 		// Black Pattern 기준 Merge 판정
// 		JudgementBlackPatternMerge(resultPanelData);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Black Pattern Merge End. Panel ID : %s"), strPanelID);
// 
// 		// KSW 17.09.11 - 혼색 많이 발생 -> 개수 많은 경우 ET 판정
// 		// 내부에서 End 로그 생성
// 		JudgementET(resultPanelData, dAlignPara, strPanelID);
// 
// 		// KSW 17.09.11 - 엣지 부분 명점 -> Pad Bright 판정
// 		//JudgementEdgePadBright(resultPanelData, dAlignPara);
// 		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Edge Pad Bright End. Panel ID : %s"), strPanelID);
// 
// 		JudgeCHoleJudgment(resultPanelData, tCHoleAlignData, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - CHole Judgement End. Panel ID : %s"), strPanelID);

		// 설정된 Panel Size - Resolution 계산
		double dResolution = calcResolution(WrtResultInfo);

		// KSW 17.09.25 - 인접 & 군집 판정
		//JudgeGroup(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
		//JudgeGroupTEST(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
		//JudgeGroupJudgment(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Group End. Panel ID : %s"), strPanelID);

		// PNZ 20.04.09 Point G64 GDS Judge
		//JudgePointGDStoFgarde(resultPanelData, dAlignPara);

		// PNZ 19.04.15 Mura Normal 분류
		//JudgementMuraNormalClassification(resultPanelData, dAlignPara);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - MuraNormal Classification End. Panel ID : %s"), strPanelID);

		// PNZ 19.08.15 Mura Normal Type 3 Filtering + PNZ 19.09.20 CHole 주변 T3 Filtering 추가
// 		JudgementMuraNormalT3Filter(resultPanelData, tCHoleAlignData, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - MuraNormal T3 Filtering End. Panel ID : %s"), strPanelID);

		//	김형주 18.12.06
		//	MergeTool Falg 와 상관없이 무조건 동작
		{
			resultPanelData.m_ResultHeader.MERGE_nRatio = nRatio;
			resultPanelData.m_ResultHeader.MERGE_rcAlignCellROI = m_stThrdAlignInfo.rcAlignCellROI;
			WorkCoordCrt tmpWorkCoordCrt = WrtResultInfo.GetWorkCoordCrt();
			resultPanelData.m_ResultHeader.MERGE_dPanelSizeX = (float)tmpWorkCoordCrt.dPanelSizeX;
			resultPanelData.m_ResultHeader.MERGE_dPanelSizeY = (float)tmpWorkCoordCrt.dPanelSizeY;
			resultPanelData.m_ResultHeader.MERGE_nWorkDirection = tmpWorkCoordCrt.nWorkDirection;
			resultPanelData.m_ResultHeader.MERGE_nWorkOriginPosition = tmpWorkCoordCrt.nWorkOriginPosition;
			resultPanelData.m_ResultHeader.MERGE_nWorkOffsetX = tmpWorkCoordCrt.nWorkOffsetX;
			resultPanelData.m_ResultHeader.MERGE_nWorkOffsetY = tmpWorkCoordCrt.nWorkOffsetY;
			resultPanelData.m_ResultHeader.MERGE_nDataDirection = tmpWorkCoordCrt.nDataDirection;
			resultPanelData.m_ResultHeader.MERGE_nGateDataOriginPosition = tmpWorkCoordCrt.nGateDataOriginPosition;
			resultPanelData.m_ResultHeader.MERGE_nGateDataOffsetX = tmpWorkCoordCrt.nGateDataOffsetX;
			resultPanelData.m_ResultHeader.MERGE_nGateDataOffsetY = tmpWorkCoordCrt.nGateDataOffsetY;
			resultPanelData.m_ResultHeader.MERGE_dGatePitch = (int)tmpWorkCoordCrt.dGatePitch;
			resultPanelData.m_ResultHeader.MERGE_dDataPitch = (int)tmpWorkCoordCrt.dDataPitch;
		}

		// KSW 17.11.30 - AVI & SVI 외곽 정보 저장 경로	
		wchar_t strContoursPath[MAX_PATH] = { 0 };
		CString strResultPath = RESULT_PATH;
		CString strMergeToolPath = MERGETOOL_PATH;
		wchar_t strContoursPath_Folder[MAX_PATH] = { 0 }; // JSO - Result 폴더 생성

		// Contours.Merge 파일 경로 설정 180322 YSS
		swprintf(strContoursPath, _T("%s\\%s\\Contours.Merge"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID);
		swprintf(strContoursPath_Folder, _T("%s\\%s\\"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID);

		// KSW 17.11.29 - AVI & SVI 외곽 정보 저장 ( AVI & SVI 머지 툴 )
		if (GetFileAttributes(strContoursPath_Folder) == -1)
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Can't find Folder : %s"), strContoursPath_Folder);
		else
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Found Folder : %s"), strContoursPath_Folder);

		if (GetFileAttributes(strContoursPath_Folder) == -1) // JSO - 폴더 한번 더 생성
		{
			SHCreateDirectoryEx(NULL, strContoursPath_Folder, NULL);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Created Folder : %s"), strContoursPath_Folder);
		}
		else
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Exist Folder : %s"), strContoursPath_Folder);

		JudgeSaveContours(resultPanelData, strContoursPath);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Contours End. Panel ID : %s"), strPanelID);

		if (eInspMode == eAutoRun && theApp.GetMergeToolUse())
		{
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("eInspMode = %d, MergeToolFlag = %d, Panel ID : %s"), eInspMode, theApp.GetMergeToolUse(), strPanelID); // JSO 0604 - 머지툴 파일 카피
		}
		CString strDest;
		strDest.Format(_T("%s\\%s\\"), MERGETOOL_PATH, strPanelID);

		if (GetFileAttributes(strDest) == -1) // JSO - 폴더 한번 더 생성
		{
			SHCreateDirectoryEx(NULL, strDest, NULL);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Created Folder : %s"), strDest);
		}
		else
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Exist Folder : %s"), strDest);

		CString strContoursDest = strDest + _T("Contours.Merge");
		bool nRet = CopyFile(strContoursPath, strContoursDest, FALSE);

		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("MergeTool Data Copy Result = %d. Panel ID : %s"), nRet, strPanelID); // JSO 0604 - 머지툴 파일 카피

		// Mura 외곽선 정보 저장 경로
		swprintf(strContoursPath, _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID);

		// Mura 외곽선 정보 저장
// 		JudgeSaveMuraContours(resultPanelData, strContoursPath);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Mura Contours End. Panel ID : %s"), strPanelID);
	}
	//	김형주 18.12.05
	//else
	//{
	//	wchar_t strContoursPath[MAX_PATH] = { 0 };

	//	// Contours.Merge 파일 경로 설정 180322 YSS
	//	swprintf(strContoursPath, _T("%s\\%s\\Contours.Merge"), MERGETOOL_PATH, strPanelID);

	//	JudgeSaveContours(resultPanelData, strContoursPath);
	//}

	// 리페어 보고용 좌표?
	JudgementRepair(strPanelID, resultPanelData, WrtResultInfo);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Repair End. Panel ID : %s"), strPanelID);

	// 17.07.12 Panel Grade 판정 추가
	JudgementPanelGrade(resultPanelData);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judge - PanelGrade End. Panel ID : %s"), strPanelID);

	// Work 좌표 계산 및 불량 순서대로 Numbering
	NumberingDefect(strModelID, strPanelID, strLotID, WrtResultInfo, resultPanelData, nRatio);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Numbering Defect End. Panel ID : %s"), strPanelID);

	// 17.12.19 동일 위치 반복 불량 검출 경/중알람 - CCD 불량 - Auto-Run 시에만 동작
	if (eInspMode == eAutoRun)
	{
		JudgementRepeatCount(strPanelID, resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judge - RepeatCount End. Panel ID : %s"), strPanelID);
	}

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

bool AviInspection::JudgementPointToLine(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, const int nImageWidth, const int nImageHeight)
{
	int nPointCount = 20;   // 같은 좌표에 Point 불량,  설정 갯수 이상 Line 판정
	int nOffSet = 9;        // 같은 좌표 에서 좌,우 (nOffSet/2) 씩 확인 ( 홀수 설정 )

	// ex) 9 : 좌우 4Pixel
	int nHalfnOffSet = nOffSet / 2;

	// 가로, 세로 Projection
	int* nProjectionX = new int[nImageWidth];
	int* nProjectionY = new int[nImageHeight];

	// 배열 초기화
	memset(nProjectionX, 0, sizeof(int) * nImageWidth);
	memset(nProjectionY, 0, sizeof(int) * nImageHeight);

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Point 불량 아닌것 제외
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK)
			continue;

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			continue;

		// 해당 좌표 1씩 증가
		nProjectionX[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X]++;
		nProjectionY[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y]++;
	}

	CArray<CRect> listTempX;
	listTempX.RemoveAll();

	// 동일한 x 좌표가 nPointCount개 이상 있는 경우
	for (int i = 0; i < nImageWidth; i++)
	{
		// 불량 리스트 번호 추가
		if (nProjectionX[i] > nPointCount)
		{
			// 처음인 경우 추가
			if (listTempX.GetCount() == 0)
			{
				listTempX.Add(CRect(i - nHalfnOffSet, 0, i + nHalfnOffSet, nOffSet));
			}
			else
			{
				// 리스트 가져오기
				CRect rectTemp = listTempX[listTempX.GetCount() - 1];

				// 연속으로 있는 경우 수정
				if (i - rectTemp.right < nHalfnOffSet)
				{
					// X 길이 변경
					rectTemp.right = i + nHalfnOffSet;

					// 결과 넣기
					listTempX[listTempX.GetCount() - 1] = rectTemp;
				}
				// 비연속인 경우 추가
				else
				{
					listTempX.Add(CRect(i - nHalfnOffSet, 0, i + nHalfnOffSet, nOffSet));
				}
			}
		}
	}

	CArray<CRect> listTempY;
	listTempY.RemoveAll();

	// 동일한 y 좌표가 nPointCount개 이상 있는 경우
	for (int i = 0; i < nImageHeight; i++)
	{
		// 불량 리스트 번호 추가
		if (nProjectionY[i] > nPointCount)
		{
			// 처음인경우 추가
			if (listTempY.GetCount() == 0)
			{
				listTempY.Add(CRect(0, i - nHalfnOffSet, nOffSet, i + nHalfnOffSet));
			}
			else
			{
				// 리스트 가져오기
				CRect rectTemp = listTempY[listTempY.GetCount() - 1];

				// 연속으로 있는 경우 수정
				if (i - rectTemp.bottom < nHalfnOffSet)
				{
					// Y 길이 변경
					rectTemp.bottom = i + nHalfnOffSet;

					// 결과 넣기
					listTempY[listTempY.GetCount() - 1] = rectTemp;
				}
				// 비연속인 경우 추가
				else
				{
					listTempY.Add(CRect(0, i - nHalfnOffSet, nOffSet, i + nHalfnOffSet));
				}
			}
		}
	}

	// nPointCount 이상 동일 좌표가 하나라도 있는 경우
	if (listTempX.GetCount() != 0 || listTempY.GetCount() != 0)
	{
		// 불량 갯수
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// Point 불량 아닌것 제외
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK)
			{
				i++;		continue;
			}	// 무한루프 수정

			if (resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				i++;		continue;
			}	// 무한루프 수정

			bool bDelete = false;

			// 동일한 x 좌표
			for (int j = 0; j < listTempX.GetCount(); j++)
			{
				if (listTempX[j].left > resultPanelData.m_ListDefectInfo[i].Pixel_Center_X ||
					listTempX[j].right < resultPanelData.m_ListDefectInfo[i].Pixel_Center_X)
					continue;

				// 리스트 가져오기
				CRect rectTemp = listTempX[j];

				// 초기 설정
				if (rectTemp.top == 0)
				{
					rectTemp.top = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					rectTemp.bottom = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;
				}
				// 길이 설정
				else
				{
					if (rectTemp.top > resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y)
						rectTemp.top = (LONG)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

					else if (rectTemp.bottom < resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y)
						rectTemp.bottom = (LONG)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;
				}

				// 결과 넣기
				listTempX[j] = rectTemp;

				bDelete = true;
			}

			// 동일한 y 좌표
			for (int j = 0; j < listTempY.GetCount(); j++)
			{
				if (listTempY[j].top > resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y ||
					listTempY[j].bottom < resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y)
					continue;

				// 리스트 가져오기
				CRect rectTemp = listTempY[j];

				// 초기 설정
				if (rectTemp.left == 0)
				{
					rectTemp.left = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					rectTemp.right = resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
				}
				// 길이 설정
				else
				{
					if (rectTemp.left > resultPanelData.m_ListDefectInfo[i].Pixel_Center_X)
						rectTemp.left = (LONG)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;

					else if (rectTemp.right < resultPanelData.m_ListDefectInfo[i].Pixel_Center_X)
						rectTemp.right = (LONG)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				}

				// 결과 넣기
				listTempY[j] = rectTemp;

				bDelete = true;
			}

			// Point -> Line 불량 판정인 경우
			if (bDelete)
			{
				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo.RemoveAt(i);
			}
			// 다음 불량
			else
			{
				i++;
			}
		}

		ResultDefectInfo* pResultData;

		// 동일한 x 좌표
		for (int j = 0; j < listTempX.GetCount(); j++)
		{
			pResultData = new ResultDefectInfo;
			// Judgement 에서 Defect Number 의미 없음
			//pResultData.Defect_No			= 0;	// ??
			_tcscpy_s(pResultData->Defect_Code, _T("CODE"));
			pResultData->Pixel_Start_X = (int)listTempX[j].left;
			pResultData->Pixel_Start_Y = (int)listTempX[j].top;
			pResultData->Pixel_End_X = (int)listTempX[j].right;
			pResultData->Pixel_End_Y = (int)listTempX[j].bottom;
			pResultData->Pixel_Center_X = (int)listTempX[j].left + listTempX[j].Width() / 2;
			pResultData->Pixel_Center_Y = (int)listTempX[j].top + listTempX[j].Height() / 2;
			pResultData->Defect_Size_Pixel = (int)(listTempX[j].Width() * listTempX[j].Height());
			pResultData->Defect_Size = (int)(pResultData->Defect_Size_Pixel * WrtResultInfo.GetCamResolution(0));
			pResultData->Img_Number = theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_GRAY_128);		// 임시로 G128 이미지 사용. 이미지 번호 없을 경우 조각 이미지 자를 때 에러
			pResultData->Defect_MeanGV = 0;	// 병합되서 평균GV 못구함.			
			//pResultData->Defect_Img_Name		= _T("");
			pResultData->Img_Size_X = (DOUBLE)listTempX[j].Width();		// 임시 불량 영상 가로 크기
			pResultData->Img_Size_Y = (DOUBLE)listTempX[j].Height();	// 임시 불량 영상 세로 크기
			pResultData->Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_OPEN;
			// Defect 결과 이미지에 그릴 때 어떤 이미지 사용 할지 선택해야함. 0 적용 시 에러
			pResultData->Camera_No = 0;

			resultPanelData.Add_DefectInfo(*pResultData);
			SAFE_DELETE(pResultData);
		}

		// 동일한 y 좌표
		for (int j = 0; j < listTempY.GetCount(); j++)
		{
			pResultData = new ResultDefectInfo;
			// Judgement 에서 Defect Number 의미 없음
			//pResultData.Defect_No			= 0;	// ??
			_tcscpy_s(pResultData->Defect_Code, _T("CODE"));
			pResultData->Pixel_Start_X = (int)listTempY[j].left;
			pResultData->Pixel_Start_Y = (int)listTempY[j].top;
			pResultData->Pixel_End_X = (int)listTempY[j].right;
			pResultData->Pixel_End_Y = (int)listTempY[j].bottom;
			pResultData->Pixel_Center_X = (int)listTempY[j].left + listTempY[j].Width() / 2;
			pResultData->Pixel_Center_Y = (int)listTempY[j].top + listTempY[j].Height() / 2;
			pResultData->Defect_Size_Pixel = (int)(listTempY[j].Width() * listTempY[j].Height());
			pResultData->Defect_Size = (int)(pResultData->Defect_Size_Pixel * WrtResultInfo.GetCamResolution(0));
			pResultData->Img_Number = theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_GRAY_128);		// 임시로 G128 이미지 사용. 이미지 번호 없을 경우 조각 이미지 자를 때 에러
			pResultData->Defect_MeanGV = 0;	// 병합되서 평균GV 못구함.
			//pResultData->Defect_Img_Name	= _T("");
			pResultData->Img_Size_X = 0;	// 현재 대체 변수 없음
			pResultData->Img_Size_Y = 0;	// 현재 대체 변수 없음
			pResultData->Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT;
			// Defect 결과 이미지에 그릴 때 어떤 이미지 사용 할지 선택해야함. 0 적용 시 에러
			pResultData->Camera_No = 0;

			resultPanelData.Add_DefectInfo(*pResultData);
			SAFE_DELETE(pResultData);
		}

		listTempX.RemoveAll();
		listTempY.RemoveAll();
	}
	return true;
}

// bool AviInspection::JudgeGroup(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
// {
// 	// 예외처리
// 	if ( dAlignPara == NULL )	return false;
// 
// 	// 불량 리스트 없는 경우 나가기
// 	if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )
// 		return true;
// 
// 	// 사용 유/무
// 	if( dAlignPara[E_PARA_GROUP_FLAG] <= 0 )
// 		return true;
// 
// 	int nGroupCount			= (int)(dAlignPara[E_PARA_GROUP1_COUNT]);					// 2개 이상
// 	int nGroupDiameterPixel	= (int)(dAlignPara[E_PARA_GROUP1_DIAMETER] / dResolution);	// 10mm 지름 ( 0.0192 파라미터화 해야 함 )
// 
// 	// 불량 갯수 설정 2개 이하인 경우.
// 	if (nGroupCount < 2)			return false;
// 
// 	// 범위 설정이 작은 경우.
// 	if (nGroupDiameterPixel < 10)	return false;
// 
// 	// 판정 반지름
// 	long nGroupRadius = nGroupDiameterPixel / 2;
// 
// 	// 군집 판정 중심
// 	cv::Point ptGroupCenter;
// 
// 	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
// 	{
// 		// KSW 17.08.26 - Point 불량 아닌것 제외 ( ReTest 군집 판정 제외 )
// 		if ( resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK				||
// 			 resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_POINT_PARTICLE	)
// 			continue;
// 		//if ( resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK				||
// 		//	(resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_POINT_ADJOIN_BRIGHT	&&
// 		//	resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_RETEST_POINT_DARK		)	||
// 		//	resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT		)
// 		//	continue;
// 
// 		// 1차 검사//////////////////////////////////////////////////
// 
// 		// KSW 17.07.19 - P/S 모드에 따른 좌표 비교
// 		// 불량 중심으로부터 설정 값(지름) 만큼
// 		CRect rectGroupRange(
// 			(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio - nGroupDiameterPixel)	, 
// 			(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio - nGroupDiameterPixel)	,
// 			(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio + nGroupDiameterPixel)	,
// 			(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio + nGroupDiameterPixel)	);
// 
// 		// 임시 불량 목록
// 		CArray<INT32> listTemp;
// 		listTemp.RemoveAll();
// 
// 		// 초기화
// 		ptGroupCenter.x = ptGroupCenter.y = 0;
// 
// 		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
// 		{			
// 			// 같은 불량 끼리만....
// 			//if (resultPanelData.m_ListDefectInfo[i].Defect_Type != resultPanelData.m_ListDefectInfo[j].Defect_Type) continue;
// 
// 			// KSW 17.08.26 - Point 불량 아닌것 제외 ( ReTest 군집 판정 제외 )
// 			if ( resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK			||
// 				 resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_POINT_PARTICLE	)
// 				continue;
// 
// 			// KSW 17.07.19 - 불량 종류 상관 없이 Point 불량
// 			// Point 불량 아닌것 제외
// 			//if ( resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK				||
// 			//	(resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_POINT_ADJOIN_BRIGHT	&&
// 			//	resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_RETEST_POINT_DARK		)	||
// 			//	resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT		)
// 			//	continue;
// 
// 			// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
// 			CPoint ptTemp(	(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio, 
// 							(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
// 
// 			// 범위 안에 불량 존재?
// 			if (rectGroupRange.PtInRect( ptTemp ) )
// 			{
// 				// 그룹 중심점
// 				ptGroupCenter.x += (int)ptTemp.x;
// 				ptGroupCenter.y += (int)ptTemp.y;
// 
// 				// 불량 리스트 번호 추가
// 				listTemp.Add(j);
// 			}
// 		}
// 
// 		// 불량 개수가 설정한 값 이하면 다른 범위 찾기
// 		if (listTemp.GetCount() < nGroupCount) continue;
// 
// 		// 2차 검사//////////////////////////////////////////////////
// 
// 		// 그룹 중심 구하기
// 		float fGroupCx = ptGroupCenter.x / (float)listTemp.GetCount();
// 		float fGroupCy = ptGroupCenter.y / (float)listTemp.GetCount();
// 		float fTempX, fTempY, fTempLength;
// 
// 		for (int nCount = 0; nCount < listTemp.GetCount(); )
// 		{
// 			int nDefectNum = listTemp[nCount];
// 
// 			// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
// 			fTempX = (float)(resultPanelData.m_ListDefectInfo[nDefectNum].Pixel_Center_X / resultPanelData.m_ListDefectInfo[nDefectNum].nRatio) - fGroupCx;
// 			fTempY = (float)(resultPanelData.m_ListDefectInfo[nDefectNum].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[nDefectNum].nRatio) - fGroupCy;
// 
// 			// 불량 그룹 중심으로부터 길이
// 			fTempLength = (fTempX * fTempX + fTempY * fTempY);
// 
// 			// 불량 그룹 중심에서 길이가 반지름보다 큰가?
// 			if (sqrt(fTempLength) > nGroupRadius)
// 			{
// 				listTemp.RemoveAt(nCount);
// 			}
// 			// 불량 그룹 중심에서 길이가 반지름보다 작은가?
// 			else
// 			{
// 				// 다음 불량으로...
// 				nCount++;
// 			}
// 		}
// 
// 		// 불량 개수가 설정한 값 이하면 다른 범위 찾기
// 		if (listTemp.GetCount() < nGroupCount) continue;		
// 
// 		// 그리기
// 		for (int nImgNum = 0; nImgNum < theApp.GetGrabStepCount(); nImgNum++)
// 		{
// 			for (int nCamNum = 0; nCamNum < theApp.GetCameraCount(nImgNum); nCamNum++)
// 			{
// 				// KSW 17.07.19 - 임시로 P/S 영상 대응
// 				if( MatDraw[nImgNum][nCamNum].cols > 10000 )
// 					cv::circle(MatDraw[nImgNum][nCamNum], cv::Point((int)fGroupCx*2, (int)fGroupCy*2), (int)nGroupRadius*2, cv::Scalar(255,0,255));
// 				else
// 					cv::circle(MatDraw[nImgNum][nCamNum], cv::Point((int)fGroupCx, (int)fGroupCy), (int)nGroupRadius, cv::Scalar(255,0,255));
// 			}
// 		}
// 
// 		// 불량명 그룹으로 변경
// 		for (int nCount = 0; nCount < listTemp.GetCount(); nCount++)
// 		{
// 			int nDefectNum = listTemp[nCount];
// 			ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[nDefectNum];
// 
// 			// KSW 17.07.19 - 불량 밝기에 따라 설정
// 			switch( tTemp.Defect_Type )
// 			{
// 			// 밝은 불량
// 			case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
// 			case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
// 			case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:
// 			case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:
// 				tTemp.Defect_Type = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
// 				break;
// 
// 			// 어두운 불량
// 			case E_DEFECT_JUDGEMENT_POINT_DARK:
// 			case E_DEFECT_JUDGEMENT_POINT_PARTICLE:
// 			case E_DEFECT_JUDGEMENT_RETEST_POINT_DARK:
// 				tTemp.Defect_Type = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
// 				break;
// 			}			
// 
// 			resultPanelData.m_ListDefectInfo[nDefectNum] = tTemp;
// 		}
// 	}
// 	return true;
// }

// bool AviInspection::JudgeGroupTEST(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
// {
// 	//////////////////////////////////////////////////////////////////////////
// 	// KSW 17.11.24 - Spec. 변경됨
// 	// D(크기) = ( 장변 + 단변 ) / 2
// 	// DS(거리) : 불량과 불량 사이 거리
// 	// N(개수) : 한 Panel 안에 존재하는 불량 개수
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	// 예외처리
// 	if ( dAlignPara == NULL )	return false;
// 
// 	// 불량 리스트 없는 경우 나가기
// 	if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )
// 		return true;
// 
// 	// 사용 유/무
// 	if( dAlignPara[E_PARA_GROUP_FLAG] <= 0 )
// 		return true;
// 
// 	// 총 그룹 개수
// 	const int nGroupCount	= 3;
// 
// 	// 파라미터 설정
// 	int		nGroup_Count[nGroupCount];		// 지름 안에 n개 이상
// 	int		nGroup_Diameter[nGroupCount];	// 지름 ( mm )
// 	double	dGroup_MinSize[nGroupCount];	// 최소 크기 ( mm )
// 	double	dGroup_MaxSize[nGroupCount];	// 최대 크기 ( mm )
// 	long	nGroup_Radius[nGroupCount];		// 반지름 ( pixel )
// 
// 	int nPara = E_PARA_GROUP1_COUNT;
// 	for (int i=0 ; i<nGroupCount ; i++)
// 	{
// 		nGroup_Count	[i]	= (int)dAlignPara[nPara++];					// 지름 안에 n개 이상
// 		nGroup_Diameter	[i]	= (int)(dAlignPara[nPara++] / dResolution);	// 지름 ( pixel )
// 		dGroup_MinSize	[i]	= dAlignPara[nPara++];						// 최소 크기 ( mm )
// 		dGroup_MaxSize	[i]	= dAlignPara[nPara++];						// 최대 크기 ( mm )
// 		nGroup_Radius	[i]	= nGroup_Diameter[i] / 2;					// 반지름 ( pixel )
// 	}
// 
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	if( 1 )
// 	{
// 		for (int i=0 ; i<resultPanelData.m_ListDefectInfo.GetCount() ; i++)
// 		{
// 			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
// 
// 			// Point 불량 아닌 불량 제외 ( ReTest 판정 제외 )
// 			if (	nDefectType < E_DEFECT_JUDGEMENT_POINT_START	||
// 					nDefectType > E_DEFECT_JUDGEMENT_POINT_END		)
// 				continue;
// 
// 			// P/S 모드에 따른 좌표 비교
// 			CPoint ptTemp(	(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio, 
// 							(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);
// 
// 			for (int j=i+1 ; j<resultPanelData.m_ListDefectInfo.GetCount() ; j++)
// 			{
// 				int nDefectType2 = resultPanelData.m_ListDefectInfo[j].Defect_Type;
// 
// 				// Point 불량 아닌 불량 제외 ( ReTest 판정 제외 )
// 				if (	nDefectType2 < E_DEFECT_JUDGEMENT_POINT_START	||
// 						nDefectType2 > E_DEFECT_JUDGEMENT_POINT_END		)
// 					continue;
// 
// 				// P/S 모드에 따른 좌표 비교
// 				CPoint ptTemp2(	(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio, 
// 								(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
// 
// 				// P/S 모드에 따른 좌표 비교
// 				float fTempX = (float)(ptTemp2.x - ptTemp.x);
// 				float fTempY = (float)(ptTemp2.y - ptTemp.y);
// 
// 				// 불량 그룹 중심으로부터 길이
// 				float fTempLength = (fTempX * fTempX + fTempY * fTempY);
// 
// 				// 불량 그룹 중심에서 길이가 지름보다 큰가?
// 				if (sqrt(fTempLength) <= nGroup_Diameter[0])
// 				{
// 					// 불량명 그룹으로 변경
// 					switch( nDefectType )
// 					{
// 						// 밝은 불량
// 					case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:
// 					case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:
// 						nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
// 						break;
// 
// 						// 어두운 불량
// 					case E_DEFECT_JUDGEMENT_POINT_DARK:
// 					case E_DEFECT_JUDGEMENT_POINT_PARTICLE:
// 					case E_DEFECT_JUDGEMENT_RETEST_POINT_DARK:
// 					case E_DEFECT_JUDGEMENT_POINT_PAD_DARK:
// 					case E_DEFECT_JUDGEMENT_POINT_GROUP_DARK:
// 						nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
// 						break;
// 					}
// 					resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;
// 
// 					//////////////////////////////////////////////////////////////////////////
// 
// 					// 불량명 그룹으로 변경
// 					switch( nDefectType2 )
// 					{
// 						// 밝은 불량
// 					case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:
// 					case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:
// 						nDefectType2 = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
// 						break;
// 
// 						// 어두운 불량
// 					case E_DEFECT_JUDGEMENT_POINT_DARK:
// 					case E_DEFECT_JUDGEMENT_POINT_PARTICLE:
// 					case E_DEFECT_JUDGEMENT_RETEST_POINT_DARK:
// 					case E_DEFECT_JUDGEMENT_POINT_PAD_DARK:
// 					case E_DEFECT_JUDGEMENT_POINT_GROUP_DARK:
// 						nDefectType2 = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
// 						break;
// 					}
// 					resultPanelData.m_ListDefectInfo[j].Defect_Type = nDefectType2;
// 				}
// 			}
// 		}
// 	}
// 	else
// 	{
// 		// 군집 판정 중심
// 		cv::Point ptGroupCenter;
// 
// 		for (int i=0 ; i<resultPanelData.m_ListDefectInfo.GetCount() ; i++)
// 		{
// 			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
// 
// 			// Point 불량 아닌 불량 제외 ( ReTest 판정 제외 )
// 			if (	nDefectType < E_DEFECT_JUDGEMENT_POINT_START	||
// 				nDefectType > E_DEFECT_JUDGEMENT_POINT_END		)
// 				continue;
// 
// 			// (장축 + 단축) / 2  ->  실제 길이 표현
// 			// P/S 모드에 따른 좌표 보정
// 			double dDimension = resultPanelData.m_ListDefectInfo[i].dDimension * dResolution / (double)resultPanelData.m_ListDefectInfo[i].nRatio;
// 
// 			// 크기 분류
// 			int nGroupType1 = -1;
// 			for (int m=0 ; m<nGroupCount ; m++)
// 			{
// 				if( dDimension <= dGroup_MinSize[m] )	continue;
// 				if( dDimension > dGroup_MaxSize[m]	)	continue;
// 
// 				nGroupType1 = m;
// 				break;
// 			}
// 
// 			// 해당하는 범위에 만족하는 경우가, 하나도 없으면 에러
// 			if( nGroupType1 == -1 )
// 				AfxMessageBox(_T("Error Set Para Size !!"));
// 
// 			// 명점인 경우 5mm 사용
// 			if( nDefectType == E_DEFECT_JUDGEMENT_POINT_BRIGHT			||
// 				nDefectType == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT		||
// 				nDefectType == E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK		||
// 				nDefectType == E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT		||
// 				nDefectType == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	)
// 				nGroupType1 = 1;
// 
// 			// 1차 검사//////////////////////////////////////////////////
// 
// 			// P/S 모드에 따른 좌표 비교
// 			// 불량 중심으로부터 설정 값(지름) 만큼
// 			CRect rectGroupRange(
// 				(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio - nGroup_Diameter[nGroupType1])	, 
// 				(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio - nGroup_Diameter[nGroupType1])	,
// 				(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio + nGroup_Diameter[nGroupType1])	,
// 				(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio + nGroup_Diameter[nGroupType1])	);
// 
// 			// 임시 불량 목록
// 			CArray<INT32> listTemp;
// 			listTemp.RemoveAll();
// 
// 			// 초기화
// 			ptGroupCenter.x = ptGroupCenter.y = 0;
// 
// 			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
// 			{
// 				// Point 불량 아닌것 제외 ( ReTest 군집 판정 제외 )
// 				if ( resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_POINT_START	||
// 					resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_POINT_END		)
// 					continue;
// 
// 				// P/S 모드에 따른 좌표 비교
// 				CPoint ptTemp(	(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio, 
// 					(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
// 
// 				// 범위 안에 불량 존재?
// 				if (rectGroupRange.PtInRect( ptTemp ) )
// 				{
// 					// 그룹 중심점
// 					ptGroupCenter.x += (int)ptTemp.x;
// 					ptGroupCenter.y += (int)ptTemp.y;
// 
// 					// 불량 리스트 번호 추가
// 					listTemp.Add(j);
// 				}
// 			}
// 
// 			// 불량 개수가 설정한 값 이하면 다른 범위 찾기
// 			if (listTemp.GetCount() < nGroup_Count[nGroupType1]) continue;
// 
// 			// 2차 검사//////////////////////////////////////////////////
// 
// 			// 그룹 중심 구하기
// 			float fGroupCx = ptGroupCenter.x / (float)listTemp.GetCount();
// 			float fGroupCy = ptGroupCenter.y / (float)listTemp.GetCount();
// 			float fTempX, fTempY, fTempLength;
// 
// 			for (int nCount = 0; nCount < listTemp.GetCount(); )
// 			{
// 				int nDefectNum = listTemp[nCount];
// 
// 				// P/S 모드에 따른 좌표 비교
// 				fTempX = (float)(resultPanelData.m_ListDefectInfo[nDefectNum].Pixel_Center_X / resultPanelData.m_ListDefectInfo[nDefectNum].nRatio) - fGroupCx;
// 				fTempY = (float)(resultPanelData.m_ListDefectInfo[nDefectNum].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[nDefectNum].nRatio) - fGroupCy;
// 
// 				// 불량 그룹 중심으로부터 길이
// 				fTempLength = (fTempX * fTempX + fTempY * fTempY);
// 
// 				// 불량 그룹 중심에서 길이가 반지름보다 큰가?
// 				if (sqrt(fTempLength) > nGroup_Radius[nGroupType1])
// 				{
// 					listTemp.RemoveAt(nCount);
// 				}
// 				// 불량 그룹 중심에서 길이가 반지름보다 작은가?
// 				else
// 				{
// 					// 다음 불량으로...
// 					nCount++;
// 				}
// 			}
// 
// 			// 불량 개수가 설정한 값 이하면 다른 범위 찾기
// 			if (listTemp.GetCount() < nGroup_Count[nGroupType1]) continue;
// 
// 			// 그리기
// 			for (int nImgNum = 0; nImgNum < theApp.GetGrabStepCount(); nImgNum++)
// 			{
// 				for (int nCamNum = 0; nCamNum < theApp.GetCameraCount(nImgNum); nCamNum++)
// 				{
// 					// 임시로 P/S 영상 대응
// 					if( MatDraw[nImgNum][nCamNum].cols > 10000 )
// 						cv::circle(MatDraw[nImgNum][nCamNum], cv::Point((int)fGroupCx*2, (int)fGroupCy*2), (int)nGroup_Radius[nGroupType1]*2, cv::Scalar(255,0,255));
// 					else
// 						cv::circle(MatDraw[nImgNum][nCamNum], cv::Point((int)fGroupCx, (int)fGroupCy), (int)nGroup_Radius[nGroupType1], cv::Scalar(255,0,255));
// 				}
// 			}
// 
// 			// 불량명 그룹으로 변경
// 			for (int nCount = 0; nCount < listTemp.GetCount(); nCount++)
// 			{
// 				int nDefectNum = listTemp[nCount];
// 				ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[nDefectNum];
// 
// 				// KSW 17.07.19 - 불량 밝기에 따라 설정
// 				switch( tTemp.Defect_Type )
// 				{
// 					// 밝은 불량
// 				case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
// 				case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
// 				case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:
// 				case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:
// 				case E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT:
// 				case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:
// 					tTemp.Defect_Type = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
// 					break;
// 
// 					// 어두운 불량
// 				case E_DEFECT_JUDGEMENT_POINT_DARK:
// 				case E_DEFECT_JUDGEMENT_POINT_PARTICLE:
// 				case E_DEFECT_JUDGEMENT_RETEST_POINT_DARK:
// 				case E_DEFECT_JUDGEMENT_POINT_PAD_DARK:
// 				case E_DEFECT_JUDGEMENT_POINT_GROUP_DARK:
// 					tTemp.Defect_Type = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
// 					break;
// 				}
// 
// 				resultPanelData.m_ListDefectInfo[nDefectNum] = tTemp;
// 			}
// 		}
// 	}	
// 
// 	return true;
// }

bool AviInspection::JudgeGroupJudgment(ResultPanelData& resultPanelData, cv::Mat(*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	// PNZ - 2019.01.11 최신 Spec.
	// Spect 설명 
	// 1. RGB Pattern 별 POINT Dark 불량을 Size(Area)에 따라 1,2,3으로 나눔, 검출 불량은 Dark Point로 통일
	// 2. Case 3 가지로 나눔
	// 3. Parameter 들
	//		- Count			: Pattern 내 불량 개수 관리, 초과시 모두 Group 판정
	//		- Distance		: 인접 불량 범위 ( 반경 )
	//		- Delete On/Off : 해당 Pattern Group 아닌 불량 삭제
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// 예외처리
	if (dAlignPara == NULL)	return false;
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;
	if (dAlignPara[E_PARA_GROUP_FLAG] <= 0)	return true;

	// Common Parameter 초기화
	int		nDefect1_Count = 0;
	int		nDefect2_Count = 0;
	int		nDefect3_Count = 0;
	int		nDefect_PD_Count = 0;
	/////////////////////////////////////choikwangil 추가
	// 모든 group judge 후 사용
	int		nAll_Count = 0;
	int		nAll_Dark_Count = 0;
	int		nAll_Bright_Count = 0;

	////////////////////////////////////
	float	nGroupDistance_1st = 0;
	float	nGroupDistance_2nd = 0;

	// Case Parameter
	int		nCount_SP_1 = (int)dAlignPara[E_PARA_GROUP1_COUNT];
	float	nDistance_SP_1 = (float)dAlignPara[E_PARA_GROUP1_DIAMETER];
	int		nDeleteOnOff_SP_1 = (int)dAlignPara[E_PARA_GROUP1_DELETE];
	int		nCount_SP_2 = (int)dAlignPara[E_PARA_GROUP2_COUNT];
	float	nDistance_SP_2 = (float)dAlignPara[E_PARA_GROUP2_DIAMETER];
	int		nDeleteOnOff_SP_2 = (int)dAlignPara[E_PARA_GROUP2_DELETE];
	int		nCount_SP_3 = (int)dAlignPara[E_PARA_GROUP3_COUNT];
	float	nDistance_SP_3 = (float)dAlignPara[E_PARA_GROUP3_DIAMETER];
	int		nDeleteOnOff_SP_3 = (int)dAlignPara[E_PARA_GROUP3_DELETE];

	/////////////////////////////////////////////////////////////////////////////choikwangil 추가
	//int		nAll_Dark_Judge = (int)dAlignPara[E_PARA_LASTGROUP_JUDGE_DARK_COUNT];
	int		nCount_All_Judge = (int)dAlignPara[E_PARA_LASTGROUP_JUDGE_ALL_COUNT];
	int		nAll_Bright_Judge = (int)dAlignPara[E_PARA_LASTGROUP_JUDGE_BRIGHT_COUNT];
	int		nAll_Dark_Judge = (int)dAlignPara[E_PARA_LASTGROUP_JUDGE_DARK_COUNT];
	/////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Count Group Judgment

	// Defect Number Count
	bool	bGroupJudgment_All_Count = false;
	bool	bGroupJudgment_Count = false;
	bool	bGroupJudgment_DarkBright_Count = false;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

		switch (nDefectType)
		{
		case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1: nDefect1_Count++; break;
		case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2: nDefect2_Count++; break;
		case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3: nDefect3_Count++; break;
		case E_DEFECT_JUDGEMENT_POINT_DARK:		 nDefect_PD_Count++; break;

		case E_DEFECT_JUDGEMENT_POINT_BRIGHT:		nAll_Bright_Count++; break;
		case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:	nAll_Bright_Count++; break;
		case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:	nAll_Bright_Count++; break;
		}
	}

	nAll_Dark_Count = nDefect1_Count + nDefect2_Count + nDefect3_Count + nDefect_PD_Count;
	nAll_Count = nDefect1_Count + nDefect2_Count + nDefect3_Count + nAll_Bright_Count + nDefect_PD_Count;

	if (nAll_Count >= nCount_All_Judge)		bGroupJudgment_All_Count = true;
	else								bGroupJudgment_All_Count = false;

	if (bGroupJudgment_All_Count == true)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			switch (nDefectType)
			{
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
			case E_DEFECT_JUDGEMENT_POINT_DARK:

				nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
				break;

			case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
			case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
			case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:

				nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
				break;
			}
			resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;
		}
	}
	else {

		if (nAll_Dark_Count >= nAll_Dark_Judge) {
			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

				switch (nDefectType)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
				case E_DEFECT_JUDGEMENT_POINT_DARK:

					nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
					break;

				case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
				case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
				case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:

					nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
					break;
				}
				resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;
			}
			bGroupJudgment_DarkBright_Count = true;
		}
		else if (nAll_Bright_Count >= nAll_Bright_Judge) {

			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

				switch (nDefectType)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
				case E_DEFECT_JUDGEMENT_POINT_DARK:

					nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
					break;

				case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
				case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
				case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:

					nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
					break;
				}
				resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;
			}
			bGroupJudgment_DarkBright_Count = true;
		}


	}

	///////////////////////////////////////////////////////////////////////////////////////
	// 설정 개수 보다 크는지 확인
	if (nDefect1_Count >= nCount_SP_1 || nDefect2_Count >= nCount_SP_2 || nDefect3_Count >= nCount_SP_3)	bGroupJudgment_Count = true;
	else																									bGroupJudgment_Count = false;

	if (bGroupJudgment_Count == true && bGroupJudgment_DarkBright_Count == false && bGroupJudgment_All_Count == false)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			switch (nDefectType)
			{
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:

				nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
				break;

			}
			resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// Distance Group Judgement

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

		// Point 아닌 불량 제외
		if (nDefectType < E_DEFECT_JUDGEMENT_POINT_DARK || nDefectType > E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) continue;

		// Parameter 초기화
		nGroupDistance_1st = 0;

		// POINT Dark 불량 이면 해당 Distance 선택
		if (nDefectType == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1) nGroupDistance_1st = nDistance_SP_1;
		if (nDefectType == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2) nGroupDistance_1st = nDistance_SP_2;
		if (nDefectType == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) nGroupDistance_1st = nDistance_SP_3;

		// Temp Defect Center in MM
		CPoint ptTemp((int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio,
			(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
		{
			if (j == i) continue;

			int nDefectType2 = resultPanelData.m_ListDefectInfo[j].Defect_Type;

			// Point 아닌 불량 제외
			if (nDefectType2 < E_DEFECT_JUDGEMENT_POINT_DARK || nDefectType2 > E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) continue;

			// Parameter 초기화
			nGroupDistance_2nd = 0;

			// POINT Dark 불량 이면 해당 Distance 선택
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1) nGroupDistance_2nd = nDistance_SP_1;
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2) nGroupDistance_2nd = nDistance_SP_2;
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) nGroupDistance_2nd = nDistance_SP_3;

			// Compair Defect Center in MM
			CPoint ptTemp2((int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio,
				(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// dX, dY in MM
			float fTempX = (float)(ptTemp2.x - ptTemp.x);
			float fTempY = (float)(ptTemp2.y - ptTemp.y);

			// 불량 그룹 중심으로부터 길이 (MM)
			float fTempLength = (float)(sqrt(fTempX * fTempX + fTempY * fTempY) * dResolution);

			// Group 판정 조건 들
			bool bGroupJudgment_Distance = false;

			// 비교 불량 중 하나가 Dark SP 123 일 때
			if (nGroupDistance_1st + nGroupDistance_2nd > 0)
			{
				if (fTempLength >= nGroupDistance_1st	&& fTempLength >= nGroupDistance_2nd)	bGroupJudgment_Distance = false;
				else if (fTempLength >= nGroupDistance_1st	&& fTempLength < nGroupDistance_2nd)	bGroupJudgment_Distance = true;
				else if (fTempLength < nGroupDistance_1st	&& fTempLength >= nGroupDistance_2nd)	bGroupJudgment_Distance = true;
				else if (fTempLength < nGroupDistance_1st	&& fTempLength < nGroupDistance_2nd)	bGroupJudgment_Distance = true;
			}

			// 비교 불량 모두 Dark SP 123외 불량 인 경우
			else if (nGroupDistance_1st + nGroupDistance_2nd == 0)
			{
				// 최대 Distance3을 기준으로 판정
				if (fTempLength <= nDistance_SP_3) bGroupJudgment_Distance = true;
			}

			else
				continue;

			if (bGroupJudgment_Distance == true && nGroupDistance_1st + nGroupDistance_2nd > 0)
			{
				// 불량명 그룹으로 변경
				switch (nDefectType)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:

					nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
					break;
				}
				resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;

				switch (nDefectType2)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:

					nDefectType2 = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
					break;
				}
				resultPanelData.m_ListDefectInfo[j].Defect_Type = nDefectType2;
			}

			else if (bGroupJudgment_Distance == true && nGroupDistance_1st + nGroupDistance_2nd == 0)
			{
				// 불량명 그룹으로 변경
				switch (nDefectType)
				{
				case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
				case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
				case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:

					nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
					break;
				}
				resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;

				switch (nDefectType2)
				{
				case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
				case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
				case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:

					nDefectType2 = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
					break;
				}
				resultPanelData.m_ListDefectInfo[j].Defect_Type = nDefectType2;
			}

			else
				continue;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// Delete Dark POINT 123

	// 다 Delete 하지 않을 시 이름 그래도 
	//if (nDeleteOnOff_SP_1 + nDeleteOnOff_SP_2 + nDeleteOnOff_SP_3 == 0) return true;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount();)
	{
		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

		// Only Point Dark SP1,2,3
		if (nDefectType != E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 && nDefectType != E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 && nDefectType != E_DEFECT_JUDGEMENT_POINT_DARK_SP_3)
		{
			i++;
			continue;
		}

		// Delete On/Off
		int nDeleteDefect = 0;

		if (nDefectType == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1) { nDeleteDefect = nDeleteOnOff_SP_1;	if (nDeleteDefect == 0)	nDefectType = E_DEFECT_JUDGEMENT_POINT_DARK; }		// Case 1 Defect
		else if (nDefectType == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2) { nDeleteDefect = nDeleteOnOff_SP_2;							nDefectType = E_DEFECT_JUDGEMENT_POINT_DARK; }		// Case 2 Defect
		else if (nDefectType == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) { nDeleteDefect = nDeleteOnOff_SP_3;							nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK; }		// Case 3 Defect
		else	 nDeleteDefect = 0;

		// Defect 이름 변경
		resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;

		// 불량 제거
		if (nDeleteDefect > 0) { resultPanelData.m_ListDefectInfo.RemoveAt(i); }
		else { i++; continue; }

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	return true;
}
////////////////////////////////////////////////////////////////////////////////CHOIKWANGIL추가
// 	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
// 	{
// 		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
// 
// 		switch (nDefectType)
// 		{
// // 		case E_DEFECT_JUDGEMENT_POINT_DARK:		 nAll_Dark_Count++; break;
// // 		case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1: nAll_Dark_Count++; break;
// // 		case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2: nAll_Dark_Count++; break;
// // 		case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3: nAll_Dark_Count++; break;
// 
//////////////////////////////////////////////////////////////////////////
// PNZ 2019-04-15
// Mura Normal 불량 분류
//////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementMuraNormalClassification(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// 예외처리
	if (dAlignPara == NULL)									return false;
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;

	//////////////////////////////////////////////////////////////////////////
	// Type 별 Classify Parameters
	int		nType2ClassPara = (int)dAlignPara[E_PARA_MURANORMAL_CLASSPARA_TYPE2]; // Count 5
	float	nType3ClassPara = (float)dAlignPara[E_PARA_MURANORMAL_CLASSPARA_TYPE3]; // Diff GV 23.0

	//////////////////////////////////////////////////////////////////////////
	// 예외처리 : Type 별 불량 개수 확인
	int nMuraType1_Count = 0;
	int nMuraType2_Count = 0;
	int nMuraType3_Count = 0;
	int nMuraType4_Count = 0;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

		switch (nDefectType)
		{
		case E_DEFECT_JUDGEMENT_MURA_LINEMURA_X:	nMuraType2_Count++; break;
		case E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y:	nMuraType2_Count++; break;
		case E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG:		nMuraType3_Count++; break;
		}
	}

	if (nMuraType1_Count == 0 && nMuraType2_Count == 0 && nMuraType3_Count == 0 && nMuraType4_Count == 0) return true;

	//////////////////////////////////////////////////////////////////////////
	// Type 2 불량 분류 : 설정하는 조건 보다 많으면 판정
	if ((nMuraType2_Count >= nType2ClassPara) && nType2ClassPara != 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			// Type 2 Mura 이면 Change Name
			if (nDefectType == E_DEFECT_JUDGEMENT_MURA_LINEMURA_X || nDefectType == E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y)
			{
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURANORMAL_TYPE2_F_GRADE;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Type 3 불량 분류 : 설정하는 조건 보다 많으면 판정
	if (nMuraType3_Count != 0 && nType3ClassPara != 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int	nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			if (nDefectType == E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG)
			{
				bool bFgradMura = false;

				// Check Diff GV of Defect
				double	dbDefectMeanGV = resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
				int		nDefectMinGV = resultPanelData.m_ListDefectInfo[i].Defect_MinGV;

				if (dbDefectMeanGV - nDefectMinGV > nType3ClassPara) bFgradMura = true;

				// Type 3 Mura 이면 Change Name
				if (bFgradMura == true)	resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURANORMAL_TYPE3_F_GRADE;
				else                        continue;
			}

			else
				continue;
		}
	}

	return true;

}


//////////////////////////////////////////////////////////////////////////
// PNZ 2019-08-15
// Type 3 과검 개선 방안 : 한도 Cell 검출 Merge
//
//////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementMuraNormalT3Filter(ResultPanelData& resultPanelData, tCHoleAlignInfo tCHoleAlignData, double* dAlignPara)
{
	// 예외처리
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;

	double	dbTHDiffGV_BBS = (double)dAlignPara[E_PARA_MURANORMAL_OR_TYPE3_BBS_DIFFGV];
	double	dbTHDiffGV_SBS = (double)dAlignPara[E_PARA_MURANORMAL_OR_TYPE3_SBS_DIFFGV];
	double	dbTHDiffGV_CH = (double)dAlignPara[E_PARA_MURANORMAL_OR_TYPE3_CH_DIFFGV];
	int		nBubbleMergePara = (int)dAlignPara[E_PARA_MURANORMAL_OR_TYPE3_BUBBLEMERGE];

	int		nOffSet = 200;

	// 기능 사용 안하면 Reture
	if (dbTHDiffGV_BBS == 0 && dbTHDiffGV_SBS == 0 && dbTHDiffGV_CH == 0 && nBubbleMergePara == 0) return true;

	//////////////////////////////////////////////////////////////////////////
	// Camera Hole 주변 T3 과검 제거
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount();)
	{
		// 값 다시 받아 오기
		dbTHDiffGV_CH = (double)dAlignPara[E_PARA_MURANORMAL_OR_TYPE3_CH_DIFFGV];

		// CHole Para 사용한지 확인
		if (dbTHDiffGV_CH <= 0) break;

		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

		// Check Targut Defect
		if (nDefectType == E_DEFECT_JUDGEMENT_MURA_TYPE3_RING ||
			nDefectType == E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG ||
			nDefectType == E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL ||
			nDefectType == E_DEFECT_JUDGEMENT_MURANORMAL_TYPE3_F_GRADE)
		{
			bool bOverKill_Locatoin = false;
			bool bOverKill_GVCheck = false;

			// Check GV
			double dbDefectAvgGV = resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
			double dbDefectBKGV = resultPanelData.m_ListDefectInfo[i].Defect_BKGV;
			double dbDiffGV_CHole = abs(dbDefectAvgGV - dbDefectBKGV);

			// 19.11.14 CHole 주변 불량 조건 추가, 작은 Size 불량 Diff 높게 설정
			double dbDefectArea = resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel;

			if (dbDefectArea <= 3000) dbTHDiffGV_CH = 5;

			if (dbDefectArea >= 50000) { i++; continue; }

			if (dbDiffGV_CHole <= dbTHDiffGV_CH) bOverKill_GVCheck = true;

			// CHole 주변 인지 확인
			for (int j = 0; j < MAX_MEM_SIZE_E_INSPECT_AREA; j++)
			{
				// Defect Info
				int nDefect_ImgNo = resultPanelData.m_ListDefectInfo[i].Img_Number;
				int nDefect_AlgNo = theApp.GetImageClassify(nDefect_ImgNo);

				if (tCHoleAlignData.rcCHoleROI[nDefect_AlgNo][j].empty())		continue;
				if (tCHoleAlignData.matCHoleROIBuf[nDefect_AlgNo][j].empty())	continue;

				int nX_Start = tCHoleAlignData.rcCHoleROI[nDefect_AlgNo][j].x;
				int nX_End = tCHoleAlignData.rcCHoleROI[nDefect_AlgNo][j].x + tCHoleAlignData.rcCHoleROI[nDefect_AlgNo][j].width;
				int nY_Start = tCHoleAlignData.rcCHoleROI[nDefect_AlgNo][j].y;
				int nY_End = tCHoleAlignData.rcCHoleROI[nDefect_AlgNo][j].y + tCHoleAlignData.rcCHoleROI[nDefect_AlgNo][j].height;

				int nDefect_CenterX = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nDefect_CenterY = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;
				//int nOffSet			= (int)(max(tCHoleAlignData.rcCHoleROI[nDefect_AlgNo][j].width, tCHoleAlignData.rcCHoleROI[nDefect_AlgNo][j].height) / 2);

				int nXDCheck = 0;
				int nYDCheck = 0;

				if (nDefect_CenterX >= nX_Start - nOffSet && nDefect_CenterX <= nX_End + nOffSet) nXDCheck = 1;
				if (nDefect_CenterY >= nY_Start - nOffSet && nDefect_CenterY <= nY_End + nOffSet) nYDCheck = 1;

				// CHole 주변에 있음
				if (nXDCheck == 1 && nYDCheck == 1) { bOverKill_Locatoin = true;  break; }
			}

			// 불량 제거
			if (bOverKill_GVCheck == true && bOverKill_Locatoin == true)
				resultPanelData.m_ListDefectInfo.RemoveAt(i);

			else { i++; continue; }
		}

		else { i++; continue; }
	}


	//////////////////////////////////////////////////////////////////////////
	// Bubble Merge Logic

	if (nBubbleMergePara != 0) // 사용 해야 진행
	{
		int nBubbleDefectCount = 0;

		// Check Defect Active Bubble
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			switch (nDefectType)
			{
			case E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLE:
				nBubbleDefectCount++;
				break;
			}
		}

		if (nBubbleDefectCount != 0) // Bubble 불량이 있어야 함
		{
			// Merge
			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{

				int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

				if (nDefectType != E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLE) continue;

				// Check Location
				int nBubbleX = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nBubbleY = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				int nBubbleWidth = (int)resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
				int nBubbleHeight = (int)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;

				int nBubbleRadian = (int)max(nBubbleWidth, nBubbleHeight) / 2;

				// CHole 주변 인지 확인
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
				{
					int nDefectType = resultPanelData.m_ListDefectInfo[j].Defect_Type;

					// Check Targut Defect
					if (nDefectType == E_DEFECT_JUDGEMENT_MURA_TYPE3_RING ||
						nDefectType == E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG ||
						nDefectType == E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL ||
						nDefectType == E_DEFECT_JUDGEMENT_MURANORMAL_TYPE3_F_GRADE)
					{
						int nT3CenterX = (int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;
						int nT3CenterY = (int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y;

						int dbDiffX = (int)abs(nBubbleX - nT3CenterX);
						int dbDiffY = (int)abs(nBubbleY - nT3CenterY);

						double	dbDistance = (double)sqrt(dbDiffX*dbDiffX + dbDiffY*dbDiffY);

						if (dbDistance - nBubbleRadian <= nBubbleMergePara)
							resultPanelData.m_ListDefectInfo.RemoveAt(j);

						else { j++; continue; }
					}
					else { j++; continue; }
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// T3 SBS + BBS Diff GV Check Logic

	// 한도 판정
	bool bJudgeLimiteSample = false;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

		switch (nDefectType)
		{
		case E_DEFECT_JUDGEMENT_MURA_TYPE3_RING:
			bJudgeLimiteSample = true;
			break;
		}
	}

	// Type 3 BBS 불량 Filtering ( 한도 Cell일 때 사용 )
	if (bJudgeLimiteSample == true)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount();)
		{
			double dbDefectAvgGV = resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
			double dbDefectBKGV = resultPanelData.m_ListDefectInfo[i].Defect_BKGV;

			double dbDiffGV = abs(dbDefectAvgGV - dbDefectBKGV);

			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			if (nDefectType == E_DEFECT_JUDGEMENT_MURA_TYPE3_RING)
				resultPanelData.m_ListDefectInfo.RemoveAt(i);

			// BBS Judge On/Off
			if (dbTHDiffGV_BBS > 0)
			{
				if (nDefectType == E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG			&&  dbDiffGV <= dbTHDiffGV_BBS)
					resultPanelData.m_ListDefectInfo.RemoveAt(i);

				else if (nDefectType == E_DEFECT_JUDGEMENT_MURANORMAL_TYPE3_F_GRADE &&  dbDiffGV <= dbTHDiffGV_BBS)
					resultPanelData.m_ListDefectInfo.RemoveAt(i);
			}

			// SBS Judge On/Off
			if (dbTHDiffGV_SBS > 0)
			{
				if (nDefectType == E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL			&&  dbDiffGV <= dbTHDiffGV_SBS)
					resultPanelData.m_ListDefectInfo.RemoveAt(i);
			}
			else
			{
				i++;
				continue;
			}
		}
	}

	return true;
}


//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	StartLogicAlgorithm
//	 2. 함수기능 :	Algorithm DLL
//	 3. 인    수 :	
//	 4. 반 환 값 :	완료 : true
//					실패 : false
//	 5. 작 성 일 :	16.02.29
//	 6. 작 성 자 :	KSW
//	 7. 수정기록 :
//	 8. 참    고 :  알고리즘 통합
//----------------------------------------------------------------------------------------------------

long AviInspection::StartLogicAlgorithm(const CString strDrive, const CString strLcp_Bubble_Path ,const tLogicPara& LogicPara, cv::Mat& MatColorBuffer, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
	cv::Mat& MatDrawBuffer, const int nImageNum, const int nROINumber, const int nAlgorithmNumber, tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	// 파라미터 설정
	cv::Mat				MatOriginImage_ = LogicPara.MatOrgImage;
	int					nAlgorithmNumber_ = nAlgorithmNumber;					// 현재 알고리즘	
	int					nCameraNum_ = LogicPara.nCameraNum;				// 검사 카메라
	int					nThrdIndex_ = LogicPara.nThreadLog;
	cv::Mat*			MatOriImageRGB_[3];
	cv::Mat				MatBKG_ = LogicPara.MatBKG;					// Mura에서 사용 ( Cell 이외 영역 ) / Dust, Black 패턴은 없음

	ENUM_KIND_OF_LOG	eLogCamera = (ENUM_KIND_OF_LOG)nCameraNum_;

	// 현재 패널 ID
	CString				strPanelID_;
	strPanelID_.Format(_T("%s"), LogicPara.tszPanelID);

	long lErrorCode = E_ERROR_CODE_TRUE;

	// 알고리즘 결과 저장 폴더 생성
	SYSTEMTIME time;
	::GetLocalTime(&time);

	// 개별 알고리즘 검사 파라미터 가져오기
	double* dAlgPara = theApp.GetAlgorithmParameter(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);
	double*	dAlignPara = theApp.GetAlignParameter(nCameraNum_);

	
	int nColorCode = (int)dAlignPara[E_PARA_LCP_COLORCODE_NUM];
	

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
		nRatio												// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	};

	CString strResultTime;
	strResultTime.Format(_T("%s%sARESULT"), theApp.m_Config.GetNetworkDrivePath(), strDrive);
	if (nCommonPara[1] > 0) CreateDirectory(strResultTime, NULL); //LCP 중간이미지

	// 검사 결과(불량) 정보 구조체 ( Dust 에서는 조금만 해도 될듯... )
	stDefectInfo* pResultBlob = new stDefectInfo(MAX_DEFECT_COUNT, nImageNum);

	// 엔지니어 판정 구조체 가져오기
	STRU_DEFECT_ITEM* EngineerDefectJudgment = theApp.GetDefectFilteringParam(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);

	// Alg DLL Start Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Start. PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
		theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));

	// 외곽선 저장 경로
	wchar_t fPath[MAX_PATH] = { 0 };
	CString strResultPath = RESULT_PATH;
	swprintf(fPath, _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID_);
	
	// 중간 영상 저장 경로 & 폴더 생성
	wchar_t wstrAlgPath[MAX_PATH] = { 0, };
	swprintf(wstrAlgPath, _T("%s\\%s\\"), (LPCWSTR)strResultTime, (LPCWSTR)strPanelID_);
	if (true) CreateDirectory(wstrAlgPath, NULL); //LCP 중간이미지

	// Alg 로그용
	wchar_t strAlgLog[MAX_PATH] = { 0, };
	swprintf(strAlgLog, _T("ID:%s\tPat:%s"), (LPCWSTR)strPanelID_, theApp.GetGrabStepName(nImageNum));

	//LCP Area Camera Bubble 검사용 Auto Run 시에
	wchar_t wstrInspBubblePath[MAX_PATH] = { 0, };
	swprintf(wstrInspBubblePath,_T("%s\\%s\\"), ORIGIN_PATH, (LPCWSTR)strPanelID_);

	//LCP Area Camera Bubble 검사용 Menual 시에
	CString strManualImagePath = _T("");

	if (eInspMode == eManualInspect) {
		strManualImagePath.Format(_T("%s\\"), strLcp_Bubble_Path);
	}

	wchar_t wstrInspBubblePath_Menual[MAX_PATH] = { 0, };
   	swprintf(wstrInspBubblePath_Menual, _T("%s"),(LPCWSTR)strManualImagePath);

	//단일쓰레드 중간 영상 저장 경로 폴더 생성
	wchar_t wstrFinalResultPath[MAX_PATH] = { 0, };

	CString		strFinal_Name = _T("Final Result");

	swprintf(wstrFinalResultPath, _T("%s%s"), wstrAlgPath, (LPCWSTR)strFinal_Name);
	if (true) CreateDirectory(wstrFinalResultPath, NULL); //LCP 중간이미지

	//////////////////////////////////////////////////////////////////////////
	wchar_t wstrInspConfirmPath[MAX_PATH] = { 0, };
	
	//CString		stBasic_Path = _T("D:\\NetworkDrive\\D_Drive\\검사확인");

	CString		stBasic_Path;
	stBasic_Path.Format(_T("%s\\검사확인"), theApp.m_Config.GetNetworkDrivePath() + strDrive);
	//if (nCommonPara[1] > 0) CreateDirectory(stBasic_Path, NULL);
	if (true) CreateDirectory(stBasic_Path, NULL); //LCP 결과이미지
	swprintf(wstrInspConfirmPath, _T("%s\\%s"), (LPCWSTR)stBasic_Path, (LPCWSTR)strPanelID_);
	//if (nCommonPara[1] > 0) CreateDirectory(wstrInspConfirmPath, NULL);
	if (true) CreateDirectory(wstrInspConfirmPath, NULL); //LCP 결과이미지
	//////////////////////////////////////////////////////////////////////////

	// Result Buff 생성 : LINE이 아닌 경우에 만 사용
	// 버퍼 할당 & 초기화

	cv::Mat matReusltImage[E_DEFECT_COLOR_COUNT];
	cv::Mat matDustReusltImage[E_DEFECT_COLOR_COUNT];
	CMatResultBuf* ResultBuf;


	ResultBuf = cMemResultBuff.FindFreeBuf_Result();

	SetMem_Result(ResultBuf);

	// 버퍼 할당
	matReusltImage[E_DEFECT_COLOR_DARK] = cMemResult->GetMat_Result(MatOriginImage_.size(), MatOriginImage_.type(), false);
	matReusltImage[E_DEFECT_COLOR_BRIGHT] = cMemResult->GetMat_Result(MatOriginImage_.size(), MatOriginImage_.type(), false);

	// Buff Set Start Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s BuffSet. PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
		theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));


	//////////////////////////////////////////////////////////////////////////
	// 알고리즘 DLL 실행
	//////////////////////////////////////////////////////////////////////////
	// White , Np , Gray Color 검사
	if (nColorCode != E_COLOR_CODE_TRANSPARENCY && nColorCode != E_COLOR_CODE_BLACK) {
		switch (nAlgorithmNumber_)
		{
		case E_ALGORITHM_LCP_SUBSTANCE:
		{
			//////////////////////////////////////////////////////////////////////////
			//LCP SUBSTANCE COMMON PARA
			int nInsp_Substance_Main_IMAGE = dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_MAIN];
			int nInsp_Substance_Compare_IMAGE = dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_COMPARE];
			int nInsp_Substance_Dust_IMAGE = dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_DUST];

			//////////////////////////////////////////////////////////////////////////

			// NP 이면 라벨지를 Active 평균으로 변환
			if (nColorCode == E_COLOR_CODE_MP && (nImageNum == theApp.GetImageNum(nInsp_Substance_Main_IMAGE) || nImageNum == theApp.GetImageNum(nInsp_Substance_Compare_IMAGE))) {
				cv::Mat temp;
				double sum = 0;
				double avg = 0;
				int count = 0;

				cv::subtract(MatOriginImage_, m_stThrdAlignInfo.matMask_Label, MatOriginImage_);

				for (int i = 0; i < MatOriginImage_.cols*MatOriginImage_.rows; i++)
				{
					if (MatOriginImage_.data[i] > 100)
					{
						count++;
						sum = sum + MatOriginImage_.data[i];
					}
				}

				avg = sum / count;

				for (int i = 0; i < MatOriginImage_.cols*MatOriginImage_.rows; i++)
				{
					if (m_stThrdAlignInfo.matMask_Label.data[i] > 100)
					{
						MatOriginImage_.data[i] = (int)avg;
					}
				}

				
			}

			//////////////////////////////////////////////////////////////////////////

			// C#에서 C++과 cv::Mat 구조가 달라 버퍼, 크기 따로 파라미터 넘겼었음.
			// MFC 사용하므로 cv::Mat 그대로 넘겨도 무방함
			lErrorCode = Point_FindDefect(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT],
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI ,dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode ,EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);

		

			// 여기 각 패턴별 Blob 또는 Contour 하기
			//Mask 없애기
			cv::bitwise_and(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_Active, matReusltImage[E_DEFECT_COLOR_DARK]);
			cv::bitwise_and(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_Active, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

			cv::subtract(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_Hole, matReusltImage[E_DEFECT_COLOR_DARK]);
			cv::subtract(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_Hole, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

			if (nColorCode == E_COLOR_CODE_MP) {
				cv::subtract(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_Label, matReusltImage[E_DEFECT_COLOR_DARK]);
				cv::subtract(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_Label, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

				//cv::subtract(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_VacuumHole, matReusltImage[E_DEFECT_COLOR_DARK]);
				//cv::subtract(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_VacuumHole, matReusltImage[E_DEFECT_COLOR_BRIGHT]);
			}

			MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK] = matReusltImage[E_DEFECT_COLOR_DARK].clone();
			MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] = matReusltImage[E_DEFECT_COLOR_BRIGHT].clone();

			bpInspectAlgEnd[nImageNum][nCameraNum_][nAlgorithmNumber_] = true;

			// 모든 카메라 모든 스레드 검사 종료 대기
			for (int i = 0; i < MAX_GRAB_STEP_COUNT; i++)
			{
				for (int k = 0; k < MAX_CAMERA_COUNT; k++)
				{
					while (!bpInspectAlgEnd[i][k][nAlgorithmNumber_])
					{
						Sleep(10);
					}
				}
			}

			// 검사 결과 영상 조합 부분
			if (nImageNum == theApp.GetImageNum(nInsp_Substance_Main_IMAGE) && nCameraNum_ == 0)
			{
				// 단일 쓰레드


				// 동축 영상 결과에 먼지, Back, Side 밝은 불량 제거
				MatResultImg[theApp.GetImageNum(nInsp_Substance_Compare_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK] -= (MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
					MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
					MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

				CString strFont2;
				strFont2.Format(_T("%s\\동축비교전확인.jpg"), wstrFinalResultPath ,(LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Compare_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK]);

				// 사이드 영상 결과에 먼지, Back, Side 밝은 불량 제거
				MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK] -= (MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
					MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
					MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);


				strFont2.Format(_T("%s\\사이드비교전확인.jpg"), wstrFinalResultPath ,(LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK]);


				MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] += MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
				MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT];

			
				strFont2.Format(_T("%s\\%s_Dust확인용.jpg"), wstrInspConfirmPath, (LPCWSTR)strPanelID_);

				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

				__int64 numOfLables = 0;
				cv::Mat matLabels, stats, centroids;
				numOfLables = connectedComponentsWithStats(MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK], matLabels, stats, centroids, 8, CV_32S) - 1;


				cv::Rect rcRect;

				cv::Mat temp = MatResultImg[theApp.GetImageNum(nInsp_Substance_Compare_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK].clone();

				MaximumFilter(temp, temp, 9);

				for (int idx = 1; idx <= numOfLables; idx++)
				{



					rcRect.x = stats.at<int>(idx, CC_STAT_LEFT);
					rcRect.y = stats.at<int>(idx, CC_STAT_TOP);
					rcRect.width = stats.at<int>(idx, CC_STAT_WIDTH);
					rcRect.height = stats.at<int>(idx, CC_STAT_HEIGHT);

					int nCenter_X = rcRect.x + rcRect.width / 2;

					int nCenter_Y = rcRect.y + rcRect.height / 2;


					if (temp.at<uchar>(nCenter_Y, nCenter_X) < 255)
					{

						// 0GV 로 초기화 후, 넘어가기
						cv::Mat matTempROI = MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK](rcRect);
						cv::Mat matLabelROI = matLabels(rcRect);

						for (int y = 0; y < rcRect.height; y++)
						{
							int		*ptrLabel = (int *)matLabelROI.ptr(y);
							uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

							for (int x = 0; x < rcRect.width; x++, ptrLabel++, ptrGray++)
							{
								// 해당 객체
								if (*ptrLabel == idx)	*ptrGray = 0;
							}
						}

						continue;
					}


				}
				
				strFont2.Format(_T("%s\\전체불량_더스트 제거.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK]);


				matReusltImage[E_DEFECT_COLOR_DARK] =  MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK].clone(); //  Gray 사용시.... temp_gray;
				//matReusltImage[E_DEFECT_COLOR_BRIGHT] = MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT].clone();

				

				lErrorCode = Point_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_], MatDrawBuffer, stThrdAlignInfo.ptCorner,dAlignPara ,dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);

				// 범위 설정
				CRect rectROI = CRect(
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x),
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y),
					max(stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x),
					max(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y));

				// 검사 영역이 영상 크기를 벗어난 경우
				if (rectROI.left < 0 ||
					rectROI.top < 0 ||
					rectROI.right >= MatOriginImage_.cols ||
					rectROI.bottom >= MatOriginImage_.rows)	return E_ERROR_CODE_ROI_OVER;

				if (rectROI.left >= rectROI.right)	return E_ERROR_CODE_ROI_OVER;
				if (rectROI.top >= rectROI.bottom)	return E_ERROR_CODE_ROI_OVER;


// 				cv::Mat mOk_flip = MatColorBuffer.clone(); // UI 결과 그리기용
// 				MatColorBuffer.copyTo(mOk_flip);
				//cv::imwrite("D:\\DefectMapImage.jpg", mOk_flip);

			}
		}
		break;

		case E_ALGORITHM_LCP_SCRATCH:
		{
			lErrorCode = LCPLine_FindDefect(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT], stThrdAlignInfo.matMask_Active, stThrdAlignInfo.matMask_Hole,
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);

			lErrorCode = LCPLine_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlignPara, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);

		}
		break;

		case E_ALGORITHM_LCP_MOISTURE:
		{
			lErrorCode = Moisture_FindDefect(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT], stThrdAlignInfo.matMask_Active, stThrdAlignInfo.matMask_Hole,
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);
		
			lErrorCode = Moisture_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlignPara ,dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);
		}
		break;

		case E_ALGORITHM_LCP_VARIANCE:
		{
			lErrorCode = Variance_FindDefect(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT], stThrdAlignInfo.matMask_Active,
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);

			lErrorCode = Variance_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlignPara ,dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);
		}

		break;

		case E_ALGORITHM_LCP_CONCEALMENT:
		{

			//다른 이미지 이물 Dust 검사 결과 기다림 
			for (int i = 0; i < MAX_GRAB_STEP_COUNT; i++)
			{
				for (int k = 0; k < MAX_CAMERA_COUNT; k++)
				{
					while (!bpInspectAlgEnd[i][k][E_ALGORITHM_LCP_SUBSTANCE])
					{
						Sleep(10);
					}
				}
			}
			
			lErrorCode = Concealment_FindDefect(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT], stThrdAlignInfo.matMask_Active,
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);

			matReusltImage[E_DEFECT_COLOR_DARK] -= (MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][E_ALGORITHM_LCP_SUBSTANCE][E_DEFECT_COLOR_BRIGHT] +
				MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum_][E_ALGORITHM_LCP_SUBSTANCE][E_DEFECT_COLOR_BRIGHT] +
				MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][E_ALGORITHM_LCP_SUBSTANCE][E_DEFECT_COLOR_BRIGHT]);

			CString strFont2;
			strFont2.Format(_T("%s\\은폐력_전체불량_더스트 제거.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
			cv::imwrite((cv::String)(CStringA)strFont2, matReusltImage[E_DEFECT_COLOR_DARK]);


			lErrorCode = Concealment_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatDrawBuffer, stThrdAlignInfo.ptCorner,dAlignPara , dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);
			
			
		}
		break;

		default:
			break;
		}
	}
	// Black Color 검사
	else if (nColorCode == E_COLOR_CODE_BLACK) {
		switch (nAlgorithmNumber_)
		{
		case E_ALGORITHM_LCP_SUBSTANCE:
		{
			//////////////////////////////////////////////////////////////////////////
			//LCP SUBSTANCE COMMON PARA
			int nInsp_Substance_Main_IMAGE = dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_MAIN];
			int nInsp_Substance_Compare_IMAGE = dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_COMPARE];
			int nInsp_Substance_Dust_IMAGE = dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_DUST];

			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////

			// C#에서 C++과 cv::Mat 구조가 달라 버퍼, 크기 따로 파라미터 넘겼었음.
			// MFC 사용하므로 cv::Mat 그대로 넘겨도 무방함
			lErrorCode = Point_FindDefect_Black(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT],
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);



			// 여기 각 패턴별 Blob 또는 Contour 하기
			//Mask 없애기
			cv::bitwise_and(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_Active, matReusltImage[E_DEFECT_COLOR_DARK]);
			cv::bitwise_and(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_Active, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

			cv::subtract(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_Hole, matReusltImage[E_DEFECT_COLOR_DARK]);
			cv::subtract(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_Hole, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

			MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK] = matReusltImage[E_DEFECT_COLOR_DARK].clone();
			MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] = matReusltImage[E_DEFECT_COLOR_BRIGHT].clone();

			bpInspectAlgEnd[nImageNum][nCameraNum_][nAlgorithmNumber_] = true;

			// 모든 카메라 모든 스레드 검사 종료 대기
			for (int i = 0; i < MAX_GRAB_STEP_COUNT; i++)
			{
				for (int k = 0; k < MAX_CAMERA_COUNT; k++)
				{
					while (!bpInspectAlgEnd[i][k][nAlgorithmNumber_])
					{
						Sleep(10);
					}
				}
			}

			// 검사 결과 영상 조합 부분
			if (nImageNum == theApp.GetImageNum(nInsp_Substance_Main_IMAGE) && nCameraNum_ == 0)
			{
				// 단일 쓰레드


				CString strFont2;
				strFont2.Format(_T("%s\\동축 먼지 제거 전 확인.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

				// 동축 영상 결과에 먼지, Back, Side 밝은 불량 제거
				MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] -= (MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
					MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

			
				strFont2.Format(_T("%s\\동축 먼지 제거 후 확인.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

				MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] += MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT];


				strFont2.Format(_T("%s\\%s_Dust확인용.jpg"), wstrInspConfirmPath, (LPCWSTR)strPanelID_);

				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

				matReusltImage[E_DEFECT_COLOR_BRIGHT] = MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT].clone(); 

				lErrorCode = Point_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_], MatDrawBuffer, stThrdAlignInfo.ptCorner,dAlignPara, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);

				// 범위 설정
				CRect rectROI = CRect(
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x),
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y),
					max(stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x),
					max(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y));

				// 검사 영역이 영상 크기를 벗어난 경우
				if (rectROI.left < 0 ||
					rectROI.top < 0 ||
					rectROI.right >= MatOriginImage_.cols ||
					rectROI.bottom >= MatOriginImage_.rows)	return E_ERROR_CODE_ROI_OVER;

				if (rectROI.left >= rectROI.right)	return E_ERROR_CODE_ROI_OVER;
				if (rectROI.top >= rectROI.bottom)	return E_ERROR_CODE_ROI_OVER;


				// 				cv::Mat mOk_flip = MatColorBuffer.clone(); // UI 결과 그리기용
				// 				MatColorBuffer.copyTo(mOk_flip);
				//cv::imwrite("D:\\DefectMapImage.jpg", mOk_flip);

			}
		}
		break;

		case E_ALGORITHM_LCP_SCRATCH:
		{
			lErrorCode = LCPLine_FindDefect(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT], stThrdAlignInfo.matMask_Active, stThrdAlignInfo.matMask_Hole,
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);

			lErrorCode = LCPLine_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlignPara , dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);

		}
		break;

		case E_ALGORITHM_LCP_MOISTURE:
		{
			lErrorCode = Moisture_FindDefect(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT], stThrdAlignInfo.matMask_Active, stThrdAlignInfo.matMask_Hole,
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);

			lErrorCode = Moisture_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatDrawBuffer, stThrdAlignInfo.ptCorner,dAlignPara, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);
		}
		break;

		case E_ALGORITHM_LCP_VARIANCE:
		{
			lErrorCode = Variance_FindDefect(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT], stThrdAlignInfo.matMask_Active,
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);

			lErrorCode = Variance_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlignPara ,dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);
		}

		break;

		case E_ALGORITHM_LCP_CONCEALMENT:
		{

			//다른 이미지 이물 Dust 검사 결과 기다림 
			for (int i = 0; i < MAX_GRAB_STEP_COUNT; i++)
			{
				for (int k = 0; k < MAX_CAMERA_COUNT; k++)
				{
					while (!bpInspectAlgEnd[i][k][E_ALGORITHM_LCP_SUBSTANCE])
					{
						Sleep(10);
					}
				}
			}

			lErrorCode = Concealment_FindDefect(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT], stThrdAlignInfo.matMask_Active,
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);

			matReusltImage[E_DEFECT_COLOR_DARK] -= (MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][E_ALGORITHM_LCP_SUBSTANCE][E_DEFECT_COLOR_BRIGHT] +
				MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum_][E_ALGORITHM_LCP_SUBSTANCE][E_DEFECT_COLOR_BRIGHT] +
				MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][E_ALGORITHM_LCP_SUBSTANCE][E_DEFECT_COLOR_BRIGHT]);

			CString strFont2;
			strFont2.Format(_T("%s\\은폐력_전체불량_더스트 제거.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
			cv::imwrite((cv::String)(CStringA)strFont2, matReusltImage[E_DEFECT_COLOR_DARK]);


			lErrorCode = Concealment_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlignPara ,dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);


		}
		break;

		default:
			break;
		}
	}
	// 투명 검사
	else {
		switch (nAlgorithmNumber_)
		{
		case E_ALGORITHM_LCP_SUBSTANCE:
		{
			//////////////////////////////////////////////////////////////////////////
			//LCP SUBSTANCE COMMON PARA
			int nInsp_Substance_Main_IMAGE = dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_MAIN];
			int nInsp_Substance_Compare_IMAGE = dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_COMPARE];
			int nInsp_Substance_Dust_IMAGE = dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_DUST];

			// C#에서 C++과 cv::Mat 구조가 달라 버퍼, 크기 따로 파라미터 넘겼었음.
			// MFC 사용하므로 cv::Mat 그대로 넘겨도 무방함
			cv::Mat matSrc_Stage = m_stThrdAlignInfo.matSrc_Stage.clone();

			lErrorCode = Point_FindDefect_Transparency(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT],
				stThrdAlignInfo.ptCorner, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], matSrc_Stage);

			// 여기 각 패턴별 Blob 또는 Contour 하기
			//Mask 없애기
			cv::bitwise_and(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_Active, matReusltImage[E_DEFECT_COLOR_DARK]);
			cv::bitwise_and(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_Active, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

			cv::subtract(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_Label, matReusltImage[E_DEFECT_COLOR_DARK]);
			cv::subtract(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_Label, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

			cv::subtract(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_Hole, matReusltImage[E_DEFECT_COLOR_DARK]);
			cv::subtract(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_Hole, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

			cv::subtract(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_VacuumHole, matReusltImage[E_DEFECT_COLOR_DARK]);
			cv::subtract(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_VacuumHole, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

			MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK] = matReusltImage[E_DEFECT_COLOR_DARK].clone();
			MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] = matReusltImage[E_DEFECT_COLOR_BRIGHT].clone();


			bpInspectAlgEnd[nImageNum][nCameraNum_][nAlgorithmNumber_] = true;


			// 모든 카메라 모든 스레드 검사 종료 대기
			for (int i = 0; i < MAX_GRAB_STEP_COUNT; i++)
			{
				for (int k = 0; k < MAX_CAMERA_COUNT; k++)
				{
					while (!bpInspectAlgEnd[i][k][nAlgorithmNumber_])
					{
						Sleep(10);
					}
				}
			}


			// 검사 결과 영상 조합 부분
			if (nImageNum == theApp.GetImageNum(nInsp_Substance_Main_IMAGE) && nCameraNum_ == 0) //COAXIAL에 결과 결합
			{

				__int64  numOfLables = 0;
				cv::Mat matLabels, stats, centroids;
				numOfLables = connectedComponentsWithStats(MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK], matLabels, stats, centroids, 8, CV_32S, CCL_GRANA) - 1;

				//두개 이미지 결과 비교 겹치는것만 불량으로 남김
				cv::Rect rcRect;
				for (int idx = 1; idx <= numOfLables; idx++)
				{



					rcRect.x = stats.at<int>(idx, CC_STAT_LEFT);
					rcRect.y = stats.at<int>(idx, CC_STAT_TOP);
					rcRect.width = stats.at<int>(idx, CC_STAT_WIDTH);
					rcRect.height = stats.at<int>(idx, CC_STAT_HEIGHT);

					int nCenter_X = rcRect.x + rcRect.width / 2;

					int nCenter_Y = rcRect.y + rcRect.height / 2;

					if (MatResultImg[theApp.GetImageNum(nInsp_Substance_Compare_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK].at<uchar>(nCenter_Y, nCenter_X) < 200)
					{

						// 0GV 로 초기화 후, 넘어가기
						cv::Mat matTempROI = MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK](rcRect);
						cv::Mat matLabelROI = matLabels(rcRect);

						for (int y = 0; y < rcRect.height; y++)
						{
							int		*ptrLabel = (int *)matLabelROI.ptr(y);
							uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

							for (int x = 0; x < rcRect.width; x++, ptrLabel++, ptrGray++)
							{
								// 해당 객체
								if (*ptrLabel == idx)	*ptrGray = 0;
							}
						}

						continue;
					}


				}
				CString strFont2;

				strFont2.Format(_T("%s\\동축_Back_bitwise_result.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK]);

				
				
				strFont2.Format(_T("%s\\%s_Dust확인용.jpg"), wstrInspConfirmPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);


				//더스트 제거
				MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK] -= MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT];

				strFont2.Format(_T("%s\\전체불량_더스트 제거.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK]);


				matReusltImage[E_DEFECT_COLOR_DARK] = MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK].clone();
				//matReusltImage[E_DEFECT_COLOR_BRIGHT] = MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT].clone();

				lErrorCode = Point_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_], MatDrawBuffer, stThrdAlignInfo.ptCorner,dAlignPara, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);

				// 범위 설정
				CRect rectROI = CRect(
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x),
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y),
					max(stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x),
					max(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y));

				// 검사 영역이 영상 크기를 벗어난 경우
				if (rectROI.left < 0 ||
					rectROI.top < 0 ||
					rectROI.right >= MatOriginImage_.cols ||
					rectROI.bottom >= MatOriginImage_.rows)	return E_ERROR_CODE_ROI_OVER;

				if (rectROI.left >= rectROI.right)	return E_ERROR_CODE_ROI_OVER;
				if (rectROI.top >= rectROI.bottom)	return E_ERROR_CODE_ROI_OVER;

				////////////////////////////////////////////////////
				stDefectInfo_Substance * pResultBlob_Sub = new stDefectInfo_Substance(MAX_DEFECT_COUNT, nImageNum);

			

				cv::Mat mOk_flip = MatColorBuffer.clone(); // UI 결과 그리기용

				MatColorBuffer.copyTo(mOk_flip);

				cv::imwrite("D:\\DefectMapImage.jpg", mOk_flip);

			}
		}
		break;

		case E_ALGORITHM_LCP_SCRATCH:
		{


		}
		break;

		case E_ALGORITHM_LCP_MOISTURE:
		{

		}
		break;


		default:
			break;
		}

	}
	// Memory Release
	cMemResultBuff.ReleaseFreeBuf_Result(ResultBuf);

	// Buff Release Start Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s BuffRelease. PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
		theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));

	// 에러인 경우, 에러 코드 & 로그 출력
	if (lErrorCode != E_ERROR_CODE_TRUE)
	{
		// Alg DLL Error Log
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Error (Code : %d). PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
			theApp.GetAlgorithmName(nAlgorithmNumber_), lErrorCode, strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));
		return lErrorCode;
	}

	// 검출 불량 개수
	int nTotalblob = 0;

	// Dust 사용 안함
	if (pResultBlob != NULL)
	{
		// 비검사 영역 삭제 전, 불량 갯수 ( 어느 패턴, 어느 알고리즘에서 불량 몇개 검출했는지 로그 )
		nTotalblob = pResultBlob->nDefectCount;

		// 비검사 영역 삭제 전, Alg DLL End Log
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm End. PanelID: %s, CAM: %02d ROI: %02d, Img: %s.\n\t\t\t\t( Before DefectNum: %d )"),
			theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum), nTotalblob);

		//////////////////////////////////////////////////////////////////////////

		// 파라미터 받아오기
		double* dAlignPara = theApp.GetAlignParameter(0);

		// 결과를 못받아 오는 경우 패스
		if (dAlignPara)
		{
			// 라인 불량만 동작하는 경우
			if (dAlignPara[E_PARA_NON_INSP_AREA_ONLY_LINE] > 0)
			{
				// 라인인 경우만 동작 / 나머지 불량 동작 안함
				if (nAlgorithmNumber_ == E_ALGORITHM_LINE)
				{
					// 비검사 영역 삭제 ( 원본 영상 그대로 계산하므로, Left-Top 보정 안해도 됨 : NULL )
					m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);
				}
			}
			// 모든 불량 동작하는 경우
			else
			{
				// 비검사 영역 삭제 ( 원본 영상 그대로 계산하므로, Left-Top 보정 안해도 됨 : NULL )
				m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);
			}
		}
		// 못받아 오는 경우, 모든 불량 동작
		else
		{
			// 비검사 영역 삭제 ( 원본 영상 그대로 계산하므로, Left-Top 보정 안해도 됨 : NULL )
			m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);
		}

		// 비검사 영역 삭제 후, 불량 갯수 ( 어느 패턴, 어느 알고리즘에서 불량 몇개 검출했는지 로그 )
		nTotalblob = pResultBlob->nDefectCount;

		// KSW 17.09.07 - 필요할 경우 사용
		if (theApp.GetCommonParameter()->bIFImageSaveFlag)
		{
			// 알고리즘 불량 결과 정보 저장
			// MatDrawBuffer 영상에 숫자 표시해야 알 수 있음.

			CString strResultROIFile = _T("");
			strResultROIFile.Format(_T("%s\\%s\\ROI\\IMG-%02d_CAM-%02d_ROI-%02d %s Result.csv"),
				INSP_PATH, strPanelID_, nImageNum, nCameraNum_, nROINumber,
				theApp.GetAlgorithmName(nAlgorithmNumber_));

			// 불량 정보 저장
			BlobFeatureSave(pResultBlob, strResultROIFile);
		}

		// 불량 정보 병합
		pResultBlob_Total->AddTail_ResultBlobAndAddOffset(pResultBlob, NULL);
	}

	// Alg DLL End Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm End. PanelID: %s, CAM: %02d ROI: %02d, Img: %s.\n\t\t\t\t( After DefectNum: %d )"),
		theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum), nTotalblob);

	return true;
}

// void AviInspection::DefectColorCheck(cv::Mat MatColorBuffer, cv::Mat MatOriginImage_, stDefectInfo* pResultBlob)
// {
// 	int& nDefectCount = pResultBlob->nDefectCount;
// 	cv::Mat matThresh2 = cv::Mat::zeros(MatOriginImage_.size(), CV_8UC1);
// 
// 	// 이미지 보정 RGB 평균값 비슷하게
// 	cv::Mat matRGB[3];
// 
// 	cv::Mat matDefectBuff_Color2 = MatColorBuffer.clone();
// 
// 	split(matDefectBuff_Color2, matRGB);
// 
// 	for (int i = 0; i < 3; i++)
// 	{
// 		cv::GaussianBlur(matRGB[i], matRGB[i], cv::Size(31, 31), 1.0);
// 
// 		// 이미지의 1/5 크기로 ROI 만들기
// 		int nROIWidth = matDefectBuff_Color2.cols / 5 / 2;
// 		int nROIHeight = matDefectBuff_Color2.rows / 5 / 2;
// 
// 		Rect rectRGBROI;
// 		rectRGBROI.x = matDefectBuff_Color2.cols / 2 - nROIWidth;
// 		rectRGBROI.y = matDefectBuff_Color2.rows / 2 - nROIHeight;
// 		rectRGBROI.width = nROIWidth * 2;
// 		rectRGBROI.height = nROIHeight * 2;
// 
// 		cv::Mat matMask = matRGB[i](rectRGBROI).clone();
// 		cv::Mat matMask2 = matRGB[i](rectRGBROI).clone();
// 		// 라벨 등 평균을 구하는데 방해되는 요소 있을 수 있으므로 제거
// 		cv::threshold(matMask, matMask, 200, 255, THRESH_BINARY_INV);
// 		cv::threshold(matMask2, matMask2, 100, 255, THRESH_BINARY);
// 
// 		cv::bitwise_and(matMask, matMask2, matMask);
// 
// 		double dMean = cv::mean(matRGB[i](rectRGBROI), matMask)[0];
// 
// 		int nValue = 126 - dMean;
// 
// 		matRGB[i] += nValue;
// 
// 	}
// 
// 
// 
// 
// 	cv::merge(matRGB, 3, matDefectBuff_Color2);
// 	cv::imwrite("D:\\이미지 보정.jpg", matDefectBuff_Color2);
// 
// 	// 불량 다시 이진화(좀더 작게)
// 	// 색 판정
// 	for (int i = 0; i < nDefectCount; i++)
// 	{
// 		// 검출된 Blob의 정보에서 해당 불량의 위치를 찾아 불량 크기보다 조금 더 큰 ROI 설정
// 		int nOffSet = 0;			// 불량보다 크게 ROI을 설정할때 몇 Pixel 더 크게 잡을지 설정해주는 값
// 
// 		Rect rectROI;
// 
// 		// 시작 x좌표
// 		rectROI.x = pResultBlob->ptLT[i].x - nOffSet;
// 		rectROI.y = pResultBlob->ptLT[i].y - nOffSet;
// 		rectROI.width = rectROI.width = pResultBlob->ptRT[i].x - pResultBlob->ptLT[i].x + nOffSet * 2;
// 		rectROI.height = rectROI.height = pResultBlob->ptLB[i].y - pResultBlob->ptLT[i].y + nOffSet * 2;
// 
// 
// 		// 해당 위치 이미지 잘라내기
// 		cv::Mat matDefectBuff_Color = matDefectBuff_Color2(rectROI).clone();			// 해당 부분 메모리를 선할당 해야 하는가??
// 		cv::Mat matGray = MatOriginImage_(rectROI).clone();								// Gray 영상
// 		cv::Mat matDefectBuff_Thresh = matThresh2(rectROI);
// 
// 		double dMeanGV = pResultBlob->dMeanGV[i];
// 
// 
// 		cv::threshold(matGray, matDefectBuff_Thresh, dMeanGV - 5, 255, THRESH_BINARY_INV);
// 
// 
// 
// 		// 이미지 Lab 변환 -> Vector 접근을 이용하면 matLab 사용 안해도 될수 있음 메모리를 생각해서 조사해봐야함
// 		cv::Mat matLab[3];
// 
// 		matDefectBuff_Color.convertTo(matDefectBuff_Color, CV_32FC3);			// float 형으로 변환
// 		matDefectBuff_Color *= 1.0 / 255.0;										// 소수점으로 변환
// 
// 		cv::cvtColor(matDefectBuff_Color, matDefectBuff_Color, CV_BGR2Lab);		// Lab 변환
// 		cv::split(matDefectBuff_Color, matLab);									// Lab 분리
// 
// 
// 
// 																				// 불량의 평균 Lab값 구하기
// 		double dMeanL = cv::mean(matLab[0], matDefectBuff_Thresh)[0];
// 		double dMeanA = cv::mean(matLab[1], matDefectBuff_Thresh)[0];
// 		double dMeanB = cv::mean(matLab[2], matDefectBuff_Thresh)[0];
// 
// 
// 		// 색구분
// 		double Deg = atan2f(dMeanB, dMeanA) * 180 / 3.141592;
// 		String strColorName;
// 
// 		double dLangth = sqrt(pow(dMeanA, 2) + pow(dMeanB, 2));
// 
// 		if (Deg > -15 && Deg < 85 && dLangth>10)  // -15 ~ 85
// 			strColorName = "Red";
// 
// 		else if (Deg<-50 && Deg>-180 && dLangth > 10)
// 			strColorName = "Blue";
// 
// 		// 					if (Deg > 80 && Deg < 100 && dLangth >= 8.5)
// 		// 						strColorName = "Yellow";
// 		// 	
// 		// 					if (Deg > 100 && Deg < 180 && dLangth >= 8.5)
// 		// 						strColorName = "Green";
// 
// 		//  					if (abs(dMeanA) < 6 && abs(dMeanB) < 6 /*&& dMeanL < dMean_BG_L*/)
// 		//  						strColorName = "Dark";
// 		else
// 			strColorName = "Dark";
// 
// 		string *matString;
// 
// 		//matString[1] = strColorName;
// 		pResultBlob->Lab_avg_L[i] = dMeanL;
// 		pResultBlob->Lab_avg_a[i] = dMeanA;
// 		pResultBlob->Lab_avg_b[i] = dMeanB;
// 		pResultBlob->strColor[i] = strColorName;
// 
// 
// 	}
// 
// 	cv::imwrite("D:\\이차 이진화.jpg", matThresh2);
// 
// 	matDefectBuff_Color2.release();
// 	matRGB->release();
// 	matThresh2.release();
// }



bool AviInspection::JudgementET(ResultPanelData& resultPanelData, double* dAlignPara, CString strPanelID)
{
	// 예외처리
	if (dAlignPara == NULL)	return false;

	// 설정 확인
	if (dAlignPara[E_PARA_HS_JUDGMENT_COUNT_BRIGHT] > 0)
	{
		int nCountBright = 0;
		int nIndexBright = 0;

		// 불량 갯수 만큼
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_32	&&
				theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_64	&&
				theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_87)
				continue;

			// 명점 다발
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				nCountBright++;

				if (nCountBright == 1)
					nIndexBright = i;	// 첫번째 인덱스 보유
			}
		}

		// 일정 개수 이상 넘어가는 경우, 하나만 보고
		if (dAlignPara[E_PARA_HS_JUDGMENT_COUNT_BRIGHT] < nCountBright)
		{
			// 첫번째 보고용 판정 수정 -> RETEST_POINT_BRIGHT
			resultPanelData.m_ListDefectInfo[nIndexBright].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT;

			// 불량 갯수 만큼
			for (int i = nIndexBright + 1; i < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

				// G32 명점 다발
				if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_32	&&
					theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_64	&&
					theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_87)
				{
					i++;  // 다음 불량으로...
					continue;
				}

				// 명점 다발
				if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
				{
					// 해당 불량 삭제
					resultPanelData.m_ListDefectInfo.RemoveAt(i);
				}
				else i++;  // 다음 불량으로...
			}
		}
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - ET Bright End. Panel ID : %s (Count : %d )"), strPanelID, nCountBright);
	}

	// 설정 확인
	if (dAlignPara[E_PARA_HS_JUDGMENT_COUNT_DARK] > 0)
	{
		int nCountDark = 0;
		int nIndexDark = 0;

		// 불량 갯수 만큼
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			// R, G, B 암점 다발
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
			{
				nCountDark++;

				if (nCountDark == 1)
					nIndexDark = i;	// 첫번째 인덱스 보유
			}
		}



		// 일정 개수 이상 넘어가는 경우, 하나만 보고
		if (dAlignPara[E_PARA_HS_JUDGMENT_COUNT_DARK] < nCountDark)
		{
			// 첫번째 보고용 판정 수정 -> RETEST_POINT_DARK
			//resultPanelData.m_ListDefectInfo[nIndexDark].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_POINT_DARK;

			// 불량 갯수 만큼
			for (int i = nIndexDark + 1; i < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

				// R, G, B 암점 다발
				if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
				{
					// 해당 불량 삭제
					resultPanelData.m_ListDefectInfo.RemoveAt(i);
				}
				else i++;  // 다음 불량으로...
			}
		}
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - ET Dark End. Panel ID : %s (Count : %d )"), strPanelID, nCountDark);
	}

	return true;
}


// bool AviInspection::JudgementEdgePadBright(ResultPanelData& resultPanelData, double* dAlignPara)
// {
// 	// 예외처리
// 	if ( dAlignPara == NULL )	return false;
// 
// 	if(	m_stThrdAlignInfo.nRatio			> 0	&&
// 		dAlignPara[E_PARA_PAD_BRIGHT_FLAG]	> 0	)
// 	{
// 		// Pad 부, 점등 영역 모서리 ( P/S 보정 )
// 		cv::Point ptRT = cv::Point((int)(m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x / m_stThrdAlignInfo.nRatio	), (int)(m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y / m_stThrdAlignInfo.nRatio	));
// 		cv::Point ptRB = cv::Point((int)(m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x / m_stThrdAlignInfo.nRatio	), (int)(m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y / m_stThrdAlignInfo.nRatio	));
// 
// 		// Pad 부, 영역 설정
// 		CRect rectPadT( (int)(ptRT.x - dAlignPara[E_PARA_PAD_BRIGHT_X]), (int)(ptRT.y), (int)(ptRT.x), (int)(ptRT.y + dAlignPara[E_PARA_PAD_BRIGHT_Y]));
// 		CRect rectPadB( (int)(ptRB.x - dAlignPara[E_PARA_PAD_BRIGHT_X]), (int)(ptRB.y - dAlignPara[E_PARA_PAD_BRIGHT_Y]), (int)(ptRB.x), (int)(ptRB.y));
// 
// 		// 모든 불량 갯수만큼...
// 		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
// 		{
// 			if ( resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT )
// 				continue;
// 
// 			// P/S 모드에 따른 좌표 비교
// 			CPoint ptTemp(	(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio,
// 				(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);
// 
// 			// 범위 안에 불량 존재?
// 			if (rectPadT.PtInRect( ptTemp ) )
// 			{
// 				ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[i];
// 
// 				tTemp.Defect_Type = E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT;
// 
// 				resultPanelData.m_ListDefectInfo[i] = tTemp;
// 			}
// 
// 			// 범위 안에 불량 존재?
// 			if (rectPadB.PtInRect( ptTemp ) )
// 			{
// 				ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[i];			
// 
// 				tTemp.Defect_Type = E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT;
// 
// 				resultPanelData.m_ListDefectInfo[i] = tTemp;
// 			}
// 		}
// 	}
// 
// 	return true;
// }


// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
bool AviInspection::JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
{
#if USE_ALG_CONTOURS

	// 외곽선 저장 경로 없는 경우
	if (strContourTxt == NULL)			return false;

	// 불량 없거나 검사 안할 경우 Skip
	//if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )	return true;

	// TXT 저장
	CStdioFile	fileWriter;
	CString		strContourIndex;

	// 파일 열기
	if (fileWriter.Open(strContourTxt, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		// 회전 좌표 계산 시, 사용
		double dTheta = -m_stThrdAlignInfo.dAlignTheta * PI / 180.;
		double	dSin = sin(dTheta);
		double	dCos = cos(dTheta);
		int nCx = m_stThrdAlignInfo.ptAlignCenter.x;
		int nCy = m_stThrdAlignInfo.ptAlignCenter.y;

		// 회전된 점등 영역
		cv::Point ptCorner[E_CORNER_END];

		// 회전 시, 예측 좌표 계산
		for (int m = 0; m < E_CORNER_END; m++)
		{
			ptCorner[m].x = (int)(dCos * (m_stThrdAlignInfo.ptCorner[m].x - nCx) - dSin * (m_stThrdAlignInfo.ptCorner[m].y - nCy) + nCx);
			ptCorner[m].y = (int)(dSin * (m_stThrdAlignInfo.ptCorner[m].x - nCx) + dCos * (m_stThrdAlignInfo.ptCorner[m].y - nCy) + nCy);
		}

		// 점등된 범위
		CRect rectROI = CRect(
			min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
			min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
			max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
			max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

		// P/S 모드에 따라 좌표 수정 ( 1샷 좌표로 변경 )
		rectROI.left /= m_stThrdAlignInfo.nRatio;
		rectROI.top /= m_stThrdAlignInfo.nRatio;
		rectROI.right /= m_stThrdAlignInfo.nRatio;
		rectROI.bottom /= m_stThrdAlignInfo.nRatio;

		// 최상위 헤더 작성 ( Cell 크기 )
		strContourIndex.Format(_T("Cell_X=%d, Cell_Y=%d\n"), rectROI.Width(), rectROI.Height());
		fileWriter.SeekToEnd();
		fileWriter.WriteString(strContourIndex);

		// 불량 갯수 만큼
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// UI 패턴 순서
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			// 헤더 파일 쓰기 ( No. || 패턴 정보 || 카메라 번호 || 불량명 )
			strContourIndex.Format(_T("No=%d, Pattern=%02d, Camera=%02d, Defect=%02d\n"), i + 1, theApp.GetImageClassify(nImgNum), resultPanelData.m_ListDefectInfo[i].Camera_No, resultPanelData.m_ListDefectInfo[i].Defect_Type);
			fileWriter.SeekToEnd();
			fileWriter.WriteString(strContourIndex);

			// AD 불량인 경우
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_DUST_GROUP)
			{
				// 사각형 E_CORNER_LEFT_TOP 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), 0, 0);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// 사각형 E_CORNER_RIGHT_TOP 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), rectROI.Width() - 1, 0);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// 사각형 E_CORNER_RIGHT_BOTTOM 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), rectROI.Width() - 1, rectROI.Height() - 1);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// 사각형 E_CORNER_LEFT_BOTTOM 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), 0, rectROI.Height() - 1);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				continue;
			}

			// 무라 불량 인가?
			bool bMura = false;

			// Mura
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_MURA_START	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_MURA_END)
			{
				if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP)
					bMura = true;
			}
			// Mura Retest
			else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_MURA ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_MURA_BRIGHT)
			{
				bMura = true;
			}
			// APP
			else if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_APP_START		&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_APP_END)
			{
				bMura = true;
			}

			// 외곽선 정보 넘기기
			if (bMura)
			{
				// 외곽선 정보
				for (int j = 0; j < MAX_CONTOURS; j++)
				{
					// P/S 모드에 따라 좌표 수정 ( 1샷 좌표로 변경 )
					// CFeatureExtraction::calcContours() 에서 이미 1샷 좌표로 변경되서 들어옴
					int x = resultPanelData.m_ListDefectInfo[i].nContoursX[j];
					int y = resultPanelData.m_ListDefectInfo[i].nContoursY[j];

					// 값이 없는 경우 나가기
					if (x == 0 && y == 0)	break;

					// E_CORNER_LEFT_TOP 좌표를 원점으로 사용
					x -= rectROI.left;
					y -= rectROI.top;

					// 예외처리
					if (x < 0)						x = 0;
					if (y < 0)						y = 0;
					if (x >= rectROI.Width())		x = rectROI.Width() - 1;
					if (y >= rectROI.Height())		y = rectROI.Height() - 1;

					// 외곽선 정보 파일 쓰기
					strContourIndex.Format(_T("%d, %d\n"), x, y);

					fileWriter.SeekToEnd();
					fileWriter.WriteString(strContourIndex);
				}
			}
			// 무라 이외의 불량 ( Point & Line ) / 사각형 정보 넘기기
			else
			{
				int nOffSet = 5;
				cv::Point ptDefectArea[4] = { (0, 0), };

				// 해당 불량 사각형 꼭지점 정보
				ptDefectArea[E_CORNER_LEFT_TOP].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_TOP].x - nOffSet;
				ptDefectArea[E_CORNER_LEFT_TOP].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_TOP].y - nOffSet;
				ptDefectArea[E_CORNER_RIGHT_TOP].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_TOP].x + nOffSet;
				ptDefectArea[E_CORNER_RIGHT_TOP].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_TOP].y - nOffSet;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM].x + nOffSet;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM].y + nOffSet;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_BOTTOM].x - nOffSet;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_BOTTOM].y + nOffSet;

				// P/S 모드에 따라 좌표 수정 ( 1샷 좌표로 변경 )
				int nRatio = resultPanelData.m_ListDefectInfo[i].nRatio;
				ptDefectArea[E_CORNER_LEFT_TOP].x /= nRatio;
				ptDefectArea[E_CORNER_LEFT_TOP].y /= nRatio;
				ptDefectArea[E_CORNER_RIGHT_TOP].x /= nRatio;
				ptDefectArea[E_CORNER_RIGHT_TOP].y /= nRatio;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].x /= nRatio;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].y /= nRatio;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].x /= nRatio;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].y /= nRatio;

				// E_CORNER_LEFT_TOP 좌표를 원점으로 사용
				ptDefectArea[E_CORNER_LEFT_TOP].x -= rectROI.left;
				ptDefectArea[E_CORNER_LEFT_TOP].y -= rectROI.top;
				ptDefectArea[E_CORNER_RIGHT_TOP].x -= rectROI.left;
				ptDefectArea[E_CORNER_RIGHT_TOP].y -= rectROI.top;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].x -= rectROI.left;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].y -= rectROI.top;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].x -= rectROI.left;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].y -= rectROI.top;

				// 예외처리
				for (int m = E_CORNER_LEFT_TOP; m <= E_CORNER_LEFT_BOTTOM; m++)
				{
					if (ptDefectArea[m].x < 0)						ptDefectArea[m].x = 0;
					if (ptDefectArea[m].y < 0)						ptDefectArea[m].y = 0;
					if (ptDefectArea[m].x >= rectROI.Width())		ptDefectArea[m].x = rectROI.Width() - 1;
					if (ptDefectArea[m].y >= rectROI.Height())		ptDefectArea[m].y = rectROI.Height() - 1;
				}

				// 사각형 E_CORNER_LEFT_TOP 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), ptDefectArea[E_CORNER_LEFT_TOP].x, ptDefectArea[E_CORNER_LEFT_TOP].y);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// 사각형 E_CORNER_RIGHT_TOP 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), ptDefectArea[E_CORNER_RIGHT_TOP].x, ptDefectArea[E_CORNER_RIGHT_TOP].y);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// 사각형 E_CORNER_RIGHT_BOTTOM 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), ptDefectArea[E_CORNER_RIGHT_BOTTOM].x, ptDefectArea[E_CORNER_RIGHT_BOTTOM].y);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// 사각형 E_CORNER_LEFT_BOTTOM 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), ptDefectArea[E_CORNER_LEFT_BOTTOM].x, ptDefectArea[E_CORNER_LEFT_BOTTOM].y);
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
bool AviInspection::JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
{
	// 외곽선 저장 경로 없는 경우
	if (strContourTxt == NULL)			return false;

	// 불량 없거나 검사 안할 경우 Skip
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;

	// TXT 저장
	CStdioFile	fileWriter;
	CString		strContourIndex;

	// 파일 열기
	if (fileWriter.Open(strContourTxt, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		// 회전 좌표 계산 시, 사용
		double dTheta = -m_stThrdAlignInfo.dAlignTheta * PI / 180.;
		double	dSin = sin(dTheta);
		double	dCos = cos(dTheta);
		int nCx = m_stThrdAlignInfo.ptAlignCenter.x;
		int nCy = m_stThrdAlignInfo.ptAlignCenter.y;

		// 불량 갯수 만큼
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// AD 불량인 경우
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_DUST_GROUP)
				continue;

			// 무라 불량 인가?
			bool bMura = false;

			// 불량 판정
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			// Mura
			if (nDefectType >= E_DEFECT_JUDGEMENT_MURA_START	&&
				nDefectType <= E_DEFECT_JUDGEMENT_MURA_END)
			{
				if (nDefectType != E_DEFECT_JUDGEMENT_MURA_MULT_BP)
					bMura = true;
			}
			// Mura Retest
			else if (nDefectType == E_DEFECT_JUDGEMENT_RETEST_MURA ||
				nDefectType == E_DEFECT_JUDGEMENT_RETEST_MURA_BRIGHT)
			{
				bMura = true;
			}
			// APP
			else if (nDefectType >= E_DEFECT_JUDGEMENT_APP_START		&&
				nDefectType <= E_DEFECT_JUDGEMENT_APP_END)
			{
				bMura = true;
			}

			// 무라 아닌 경우 제외
			if (!bMura) continue;

			// 외곽선 정보
			for (int j = 0; j < MAX_CONTOURS; j++)
			{
				// P/S 모드에 따라 좌표 수정 ( 1샷 좌표로 변경 )
				// CFeatureExtraction::calcContours() 에서 이미 1샷 좌표로 변경되서 들어옴
				int x = resultPanelData.m_ListDefectInfo[i].nContoursX[j];
				int y = resultPanelData.m_ListDefectInfo[i].nContoursY[j];

				// 값이 없는 경우 나가기
				if (x == 0 && y == 0)	break;

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


bool AviInspection::DeleteOverlapDefect(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.09.21 - 6.39" 귀부분 점등 안된경우, E급 판정
	// EDGE_NUGI 인 경우, EDGE_NUGI를 제외한 모든 불량 삭제
	// SCJ 20.02.18 - B11 曹文 요청 EDGE 영역 // ACTIVE 영역 NUGI 불량 분류 요청
	//////////////////////////////////////////////////////////////////////////
	//
	//bool bNugiJudgeE = false;
	//
	//for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	//{
	//	// EDGE_NUGI 찾기
	//	if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI)
	//	{
	//		// EDGE_NUGI 리스트 처음으로 복사
	//		resultPanelData.m_ListDefectInfo[0] = resultPanelData.m_ListDefectInfo[i];
	//
	//		// E급 판정
	//		bNugiJudgeE = true;
	//
	//		break;
	//	}
	//}
	//
	//// EDGE_NUGI 있는 경우...
	//if (bNugiJudgeE)
	//{
	//	// 모든 불량 제거
	//	for (int i = 1; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	//		resultPanelData.m_ListDefectInfo.RemoveAt(i);
	//
	//	// 삭제후, 나가기
	//	return true;
	//}

	//////////////////////////////////////////////////////////////////////////
	// AD인 경우, AD를 제외한 모든 불량 삭제
	// EDGE_NUGI가 없는데, AD인 경우
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// AD 불량이 아닌 경우 ( E_DEFECT_JUDGEMENT_DUST_GROUP 무시 )
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT)
			continue;

		// 첫 패턴 AD 불량인 경우
		if (resultPanelData.m_ListDefectInfo[i].Img_Number == 0)
		{
			// 불량 첫번째 리스트에 덮어 씌우기
			ResultDefectInfo tTempAD = resultPanelData.m_ListDefectInfo[i];
			resultPanelData.m_ListDefectInfo[0] = tTempAD;

			// 첫번째 리스트(AD 불량) 제외하고, 모두 삭제.
			for (int j = 1; j < resultPanelData.m_ListDefectInfo.GetCount();)
				resultPanelData.m_ListDefectInfo.RemoveAt(j);

			// 삭제후, 나가기
			return true;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// AD 아닌 경우, 중복 제거
	//////////////////////////////////////////////////////////////////////////
	// RGBBlack Bright Point - White Spot Mura 중복 제거
	//DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point(resultPanelData, dAlignPara); //임시 주석 choi 05.07 필요없어보임

	//Black mura 중복 판정 
	DeleteOverlapDefect_Black_Mura_and_Judge(resultPanelData, dAlignPara);

	// Point - Point 중복 제거
	DeleteOverlapDefect_Point_Point(resultPanelData, dAlignPara);

	// Mura - Mura 중복 제거
	DeleteOverlapDefect_Mura_Mura(resultPanelData, dAlignPara);

	// Point - Line 중복 제거
	DeleteOverlapDefect_Point_Line(resultPanelData, dAlignPara);

	// Point - Mura 중복 제거
	DeleteOverlapDefect_Point_Mura(resultPanelData, dAlignPara);

	// Line - Mura 중복 제거
	DeleteOverlapDefect_Line_Mura(resultPanelData, dAlignPara);

	//RGBBlack Bright Point - White Spot Mura 중복 제거
	//DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point(resultPanelData, dAlignPara);

	return true;
}

//choikwangil Test 미사용 Enum 추가 필요
// bool AviInspection::Judge_DefectSize(ResultPanelData& resultPanelData, double* dAlignPara)
// {
// 	double Judge_Ds_Flag = (double)(dAlignPara[E_PARA_DSJ_FLAG]);
// 	// 불량 리스트 또는 Flag 없는 경우 나가기
// 	if (Judge_Ds_Flag <= 0 || resultPanelData.m_ListDefectInfo.GetCount() <= 0)
// 		return true;
// 	
// 	//SP1
// 	double SP1_start_value = (double)(dAlignPara[E_PARA_DSJ_SP1_Start_Value]);
// 	double SP1_end_value = (double)(dAlignPara[E_PARA_DSJ_SP1_End_Value]);
// 
// 	//SP2
// 	double SP2_start_value = (double)(dAlignPara[E_PARA_DSJ_SP2_Start_Value]);
// 	double SP2_end_value = (double)(dAlignPara[E_PARA_DSJ_SP2_End_Value]);
// 
// 	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
// 	{
// 		// EDGE_NUGI 찾기
// 		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK || 
// 			(resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 && resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_POINT_DARK_SP_3))
// 		{
// 			if (resultPanelData.m_ListDefectInfo[i].Defect_Size >SP1_start_value && resultPanelData.m_ListDefectInfo[i].Defect_Size <= SP1_end_value) {
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DARK_SP_1;
// 			}
// 			else if (resultPanelData.m_ListDefectInfo[i].Defect_Size >SP2_start_value && resultPanelData.m_ListDefectInfo[i].Defect_Size <= SP2_end_value) {
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DARK_SP_2;
// 			}
// 			else {
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DARK_SP_3;
// 			}
// 		}
// 	}
// 
// 	return true;
// }


bool AviInspection::DeleteOverlapDefect_DimpleDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{
	// 예외처리 Parameter 가 없으면
	if (dAlignPara == NULL)
		return false;

	// Dimple Ratio
	double dDimpleRatio_GrayPattern_Active = (double)(dAlignPara[E_PARA_DIMPLE_RATIO_GRAY_Active]);
	double dDimpleRatio_GrayPattern_Edge = (double)(dAlignPara[E_PARA_DIMPLE_RATIO_GRAY_Edge]);
	double dDimpleRatio_RGBPattern = (double)(dAlignPara[E_PARA_DIMPLE_RATIO_RGB]);


	// 예외처리 
	if (dDimpleRatio_GrayPattern_Active <= 0 || dDimpleRatio_GrayPattern_Edge <= 0 || dDimpleRatio_RGBPattern <= 0)
		return false;

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// Point 불량 Count
	int nPointDefect = 0;
	int nRGBDefect = 0;
	int nGreenDefect = 0;
	int nGrayDefect = 0;
	int nMultiBP = 0;

	// 불량 리스트를 훌터 봄 -> Point 불량 없으면 나가 / White or G64 Pattern에 불량 없으면 직접 RGB 불량을 기준으로 하여 Data 획득
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		// Point 불량인 경우만 찾음
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
		{
			//Point불량 Count
			nPointDefect++;
		}

		// Pattern 정보
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		// Point 불량이 White Gray Pattern에 있는 경우만 찾음
		if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_64)
		{
			// G64 Pattern에 있는 Point불량 Count
			nGrayDefect++;
		}

		// Point 불량이 White Gray Pattern에 있는 경우만 찾음
		if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_R || nImgNum1 == E_IMAGE_CLASSIFY_AVI_G || nImgNum1 == E_IMAGE_CLASSIFY_AVI_B)
		{
			// White Pattern와 G64 Pattern에 있는 Point불량 Count
			nRGBDefect++;
		}

	}

	// Green Pattern에서 Retest 불량이 나오는지 확인
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Point 불량인 경우만 찾음
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
		{
			// Pattern 정보
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Point 불량이 White Gray Pattern에 있는 경우만 찾음
			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_G)
			{
				// White Pattern와 G64 Pattern에 있는 Point불량 Count
				nGreenDefect++;
			}
		}
	}

	// G64 Pattern에 있는 Multi-BP 개수 Count
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Point 불량인 경우만 찾음
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_MULT_BP)
		{
			// Pattern 정보
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Point 불량이 White Gray Pattern에 있는 경우만 찾음
			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_64)
			{
				// White Pattern와 G64 Pattern에 있는 Point불량 Count
				nMultiBP++;
			}
		}
	}

	// Point 불량이 없으면 나감
	if (nPointDefect <= 0 && nGreenDefect <= 0 && nMultiBP <= 0)
		return true;

	// Parameter 들
	int	nOffSet = 4;
	int	nRangeOffset = 10;

	// 좌표들 
	cv::Rect rectTemp;

	// 좌표 초기화
	int	PixelStartX = 0;
	int	PixelStartY = 0;
	int	PixelEndX = 0;
	int	PixelEndY = 0;

	// ROI Region
	int nXStart = m_stThrdAlignInfo.rcAlignCellROI.x;
	int nYStart = m_stThrdAlignInfo.rcAlignCellROI.y;
	int nXEnd = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
	int nYEnd = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

	// White & G64 Pattern 원본 영상
	cv::Mat mWhiteImage = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_WHITE)][0];
	cv::Mat mG64Image = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_GRAY_64)][0];

	// Gaussian Blur
	cv::GaussianBlur(mWhiteImage, mWhiteImage, cv::Size(5, 5), 3.0);
	cv::GaussianBlur(mG64Image, mG64Image, cv::Size(5, 5), 3.0);

	//////////////////////////////////////////////////////////////////////////
	// Gray Pattern 명점 불량 Count
	//////////////////////////////////////////////////////////////////////////

	if (nGrayDefect > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Point 불량인 경우만...
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				continue;
			}

			// 불량 Pattern을 찾음
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// RGB에 있는 불량 만 찾음
			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_64)
			{
				continue;
			}

			////////////////////////////////
			// Max-Mean Ratio Calculation //
			int nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
			int nDefectMeanGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
			float MaxMeanRatio = (float)nDefectMaxGV / nDefectMeanGV;

			// 약명점/Dimple이 약하여 임시로 Max/Mean Ratio && 최대 밝기 값으로 분류 함
			if (MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
				continue;

			//좌표들
			PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nRangeOffset);
			PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y - 2);
			PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_X + nRangeOffset);
			PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y + 2);

			// Ratio 초기화 Active Ratio를 선택한다
			double dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Active;

			// Edge 근처 100Pixel 이내를 Edge 영역
			int nEdgeOffset_X = 90;
			int nEdgeOffset_Y = 100;

			// X 방향 Edge 부분 Dimple 제거 Ratio를 선택함
			if ((PixelStartX - nEdgeOffset_X <= nXStart) || (PixelEndX + nEdgeOffset_X >= nXEnd))
			{
				// 수평 Profile를 수직으로 변환함
				PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X - 2);
				PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y - nRangeOffset);
				PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X + 2);
				PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y + nRangeOffset);

				dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Edge;
			}

			// Y 방향 Edge 부분 Dimple 제거 Ratio를 선택함
			if ((PixelStartY - nEdgeOffset_Y <= nYStart) || (PixelEndY + nEdgeOffset_Y >= nYEnd))
			{
				dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Edge;
			}

			//Rect 그리기
			rectTemp.x = PixelStartX;
			rectTemp.y = PixelStartY;
			rectTemp.width = PixelEndX - PixelStartX + 1;
			rectTemp.height = PixelEndY - PixelStartY + 1;

			// 불량 중심 기준으로 White/G64 Pattern 에서 조각 영상을 획득
			cv::Mat matTempBufWhite = mWhiteImage(rectTemp);
			cv::Mat matTempBufG64 = mG64Image(rectTemp);

			// 영역 최대값 평균 값을 구한다
			cv::Scalar tempMean_W, tempStd_W, tempMean_G64, tempStd_G64;

			double minvalue_W = 0;
			double maxvlaue_W = 0;
			double minvalue_G64 = 0;
			double maxvlaue_G64 = 0;

			// White Pattern 정보 구하기
			cv::meanStdDev(matTempBufWhite, tempMean_W, tempStd_W);
			cv::minMaxIdx(matTempBufWhite, &minvalue_W, &maxvlaue_W, NULL, NULL);
			double dImageavg_W = tempMean_W[0];
			double dRatio1 = maxvlaue_W / dImageavg_W;

			// G64 Pattern 정보 구하기
			cv::meanStdDev(matTempBufG64, tempMean_G64, tempStd_G64);
			cv::minMaxIdx(matTempBufG64, &minvalue_G64, &maxvlaue_G64, NULL, NULL);
			double dImageavg_G64 = tempMean_G64[0];
			double dRatio2 = maxvlaue_G64 / dImageavg_G64;

			double dRatio = dRatio2 / dRatio1;

			matTempBufWhite.release();
			matTempBufG64.release();

			// 			//////////////////////////////////////////////////////////////////////////
			// 			// New Max GV Method
			// 			double Newmaxvlaue_W;
			// 			double Newmaxvlaue_G64;
			// 			int nTopGVCount = 3;
			// 
			// 			// Top 3 GV로 Max GV 다시 구하기
			// 			NewMaxGVMethold(matTempBufWhite, maxvlaue_W, Newmaxvlaue_W, nTopGVCount);
			// 			NewMaxGVMethold(matTempBufG64, maxvlaue_G64, Newmaxvlaue_G64, nTopGVCount);
			// 			double dNewRatio = (Newmaxvlaue_G64/dImageavg_G64) / (Newmaxvlaue_W/dImageavg_W);
						//////////////////////////////////////////////////////////////////////////
						// G64와 White에서 아주 밝게 올라오면 강시인 불량으로 Continue함
			if (maxvlaue_W >= 200 && maxvlaue_G64 >= 200)
				continue;

			if (dRatio <= dDimpleRatio_GrayPattern)
				//if (dNewRatio <= dDimpleRatio_GrayPattern)
			{
				// i번 영상이 Dimple로 의심 Dimple로 Defect_Type 변경
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DIMPLE;
				continue;
			}
			else
				continue;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Case 2: White-X G64-X  -> RGB Pattern Dimple Check 기능
	// 1. White + G64 Pattern에서 Dimple로 판정 못하는 위치 존재
	// 2. RGB 불량 위치를 기준으로 하여 White&G64 Pattern에 가서 Ratio를 구하여 판정 함
	// 3. RGB는 상태적으로 선명하기에 Ratio를 크게 줘도 좋으며 Active나 Edge 나주지 않는다
	//////////////////////////////////////////////////////////////////////////

	if (nRGBDefect > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Point 불량인 경우만...
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				continue;
			}

			// 불량 Pattern을 찾음
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// RGB에 있는 불량 만 찾음
			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_R	&&	nImgNum1 != E_IMAGE_CLASSIFY_AVI_G	&&	nImgNum1 != E_IMAGE_CLASSIFY_AVI_B)
			{
				continue;
			}

			////////////////////////////////
			// Max-Mean Ratio Calculation //
			int nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_G && nDefectMaxGV > 250) continue;

			//좌표들 -> RGB Pattern의 좌표를 기준으로 함
			PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nRangeOffset);
			PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y - 2);
			PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_X + nRangeOffset);
			PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y + 2);

			// 범위 설정
			rectTemp.x = PixelStartX;
			rectTemp.y = PixelStartY;
			rectTemp.width = PixelEndX - PixelStartX + 1;
			rectTemp.height = PixelEndY - PixelStartY + 1;

			// RGB 기준으로 White/G64 Pattern 불량 찾기
			cv::Mat matTempBufWhite = mWhiteImage(rectTemp);
			cv::Mat matTempBufG64 = mG64Image(rectTemp);

			// 영역 최대값 평균 값을 구한다
			cv::Scalar tempMean_W, tempStd_W, tempMean_G64, tempStd_G64;

			double minvalue_W = 0;
			double maxvlaue_W = 0;
			double minvalue_G64 = 0;
			double maxvlaue_G64 = 0;

			// White Pattern 정보 구하기
			cv::meanStdDev(matTempBufWhite, tempMean_W, tempStd_W);
			cv::minMaxIdx(matTempBufWhite, &minvalue_W, &maxvlaue_W, NULL, NULL);
			double dImageavg_W = tempMean_W[0];
			double dRatio1 = maxvlaue_W / dImageavg_W;

			// G64 Pattern 정보 구하기
			cv::meanStdDev(matTempBufG64, tempMean_G64, tempStd_G64);
			cv::minMaxIdx(matTempBufG64, &minvalue_G64, &maxvlaue_G64, NULL, NULL);
			double dImageavg_G64 = tempMean_G64[0];
			double dRatio2 = maxvlaue_G64 / dImageavg_G64;

			double dRatio = dRatio2 / dRatio1;

			matTempBufWhite.release();
			matTempBufG64.release();

			// 			//////////////////////////////////////////////////////////////////////////
			// 			// New Max GV Method
			// 			double Newmaxvlaue_W;
			// 			double Newmaxvlaue_G64;
			// 			int nTopGVCount = 3;
			// 
			// 			// Top 3 GV로 Max GV 다시 구하기
			// 			NewMaxGVMethold(matTempBufWhite, maxvlaue_W, Newmaxvlaue_W, nTopGVCount);
			// 			NewMaxGVMethold(matTempBufG64, maxvlaue_G64, Newmaxvlaue_G64, nTopGVCount);
			// 			double dNewRatio = (Newmaxvlaue_G64/dImageavg_G64) / (Newmaxvlaue_W/dImageavg_W);
						//////////////////////////////////////////////////////////////////////////

			if (maxvlaue_W >= 200 && maxvlaue_G64 >= 200)
				continue;

			if (dRatio <= dDimpleRatio_RGBPattern)
				//if (dNewRatio <= dDimpleRatio_RGBPattern)
			{
				// i번 영상이 Dimple로 의심 Dimple로 Defect_Type 변경
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DIMPLE;
				continue;
			}
			else
				continue;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Case 3: White-X G64-X  -> RGB Pattern Dimple Check 기능
	// 1. White + G64 Pattern에서 Dimple로 판정 못하는 위치 존재
	// 2. RGB 불량 위치를 기준으로 하여 White&G64 Pattern에 가서 Ratio를 구하여 판정 함
	// 3. RGB는 상태적으로 선명하기에 Ratio를 크게 줘도 좋으며 Active나 Edge 나주지 않는다
	//////////////////////////////////////////////////////////////////////////

	if (nGreenDefect > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Point 불량인 경우만...
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
			{
				continue;
			}

			// 불량 Pattern을 찾음
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// RGB에 있는 불량 만 찾음
			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_G)
			{
				continue;
			}

			//좌표들 -> RGB Pattern의 좌표를 기준으로 함
			PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nRangeOffset);
			PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y - 2);
			PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_X + nRangeOffset);
			PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y + 2);

			// 범위 설정
			rectTemp.x = PixelStartX;
			rectTemp.y = PixelStartY;
			rectTemp.width = PixelEndX - PixelStartX + 1;
			rectTemp.height = PixelEndY - PixelStartY + 1;

			// RGB 기준으로 White/G64 Pattern 불량 찾기
			cv::Mat matTempBufWhite = mWhiteImage(rectTemp);
			cv::Mat matTempBufG64 = mG64Image(rectTemp);

			// 영역 최대값 평균 값을 구한다
			cv::Scalar tempMean_W, tempStd_W, tempMean_G64, tempStd_G64;

			double minvalue_W = 0;
			double maxvlaue_W = 0;
			double minvalue_G64 = 0;
			double maxvlaue_G64 = 0;

			// White Pattern 정보 구하기
			cv::meanStdDev(matTempBufWhite, tempMean_W, tempStd_W);
			cv::minMaxIdx(matTempBufWhite, &minvalue_W, &maxvlaue_W, NULL, NULL);
			double dImageavg_W = tempMean_W[0];
			double dRatio1 = maxvlaue_W / dImageavg_W;

			// G64 Pattern 정보 구하기
			cv::meanStdDev(matTempBufG64, tempMean_G64, tempStd_G64);
			cv::minMaxIdx(matTempBufG64, &minvalue_G64, &maxvlaue_G64, NULL, NULL);
			double dImageavg_G64 = tempMean_G64[0];
			double dRatio2 = maxvlaue_G64 / dImageavg_G64;

			double dRatio = dRatio2 / dRatio1;

			matTempBufWhite.release();
			matTempBufG64.release();

			// 			//////////////////////////////////////////////////////////////////////////
			// 			// New Max GV Method
			// 			double Newmaxvlaue_W;
			// 			double Newmaxvlaue_G64;
			// 			int nTopGVCount = 3;
			// 
			// 			// Top 3 GV로 Max GV 다시 구하기
			// 			NewMaxGVMethold(matTempBufWhite, maxvlaue_W, Newmaxvlaue_W, nTopGVCount);
			// 			NewMaxGVMethold(matTempBufG64, maxvlaue_G64, Newmaxvlaue_G64, nTopGVCount);
			// 			double dNewRatio = (Newmaxvlaue_G64/dImageavg_G64) / (Newmaxvlaue_W/dImageavg_W);
						//////////////////////////////////////////////////////////////////////////

			if (maxvlaue_W >= 200 && maxvlaue_G64 >= 200)
				continue;

			if (dRatio <= dDimpleRatio_RGBPattern)
				//if (dNewRatio <= dDimpleRatio_RGBPattern)
			{
				// i번 영상이 Dimple로 의심 Dimple로 Defect_Type 변경
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DIMPLE;
				continue;
			}
			else
				continue;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// DIMPLE이 Muti BP로 판정하는 경우 추가 
	//////////////////////////////////////////////////////////////////////////
	if (nMultiBP > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Point 불량인 경우만...
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP)
			{
				continue;
			}

			// 불량 Pattern을 찾음
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// RGB에 있는 불량 만 찾음
			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_64)
			{
				continue;
			}

			////////////////////////////////
			// Max-Mean Ratio Calculation //
			int nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
			int nDefectMeanGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
			float MaxMeanRatio = (float)nDefectMaxGV / nDefectMeanGV;

			// 약명점/Dimple이 약하여 임시로 Max/Mean Ratio && 최대 밝기 값으로 분류 함
			if (MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
				continue;

			//좌표들
			PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nRangeOffset);
			PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y - 2);
			PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_X + nRangeOffset);
			PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y + 2);

			// Ratio 초기화 Active Ratio를 선택한다
			double dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Active;

			// Edge 근처 100Pixel 이내를 Edge 영역
			int nEdgeOffset_X = 90;
			int nEdgeOffset_Y = 100;

			// X 방향 Edge 부분 Dimple 제거 Ratio를 선택함
			if ((PixelStartX - nEdgeOffset_X <= nXStart) || (PixelEndX + nEdgeOffset_X >= nXEnd))
			{
				// 수평 Profile를 수직으로 변환함
				PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X - 2);
				PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y - nRangeOffset);
				PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X + 2);
				PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y + nRangeOffset);

				dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Edge;
			}

			// Y 방향 Edge 부분 Dimple 제거 Ratio를 선택함
			if ((PixelStartY - nEdgeOffset_Y <= nYStart) || (PixelEndY + nEdgeOffset_Y >= nYEnd))
			{
				dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Edge;
			}

			//Rect 그리기
			rectTemp.x = PixelStartX;
			rectTemp.y = PixelStartY;
			rectTemp.width = PixelEndX - PixelStartX + 1;
			rectTemp.height = PixelEndY - PixelStartY + 1;

			// 불량 중심 기준으로 White/G64 Pattern 에서 조각 영상을 획득
			cv::Mat matTempBufWhite = mWhiteImage(rectTemp);
			cv::Mat matTempBufG64 = mG64Image(rectTemp);

			// 영역 최대값 평균 값을 구한다
			cv::Scalar tempMean_W, tempStd_W, tempMean_G64, tempStd_G64;

			double minvalue_W = 0;
			double maxvlaue_W = 0;
			double minvalue_G64 = 0;
			double maxvlaue_G64 = 0;

			// White Pattern 정보 구하기
			cv::meanStdDev(matTempBufWhite, tempMean_W, tempStd_W);
			cv::minMaxIdx(matTempBufWhite, &minvalue_W, &maxvlaue_W, NULL, NULL);
			double dImageavg_W = tempMean_W[0];
			double dRatio1 = maxvlaue_W / dImageavg_W;

			// G64 Pattern 정보 구하기
			cv::meanStdDev(matTempBufG64, tempMean_G64, tempStd_G64);
			cv::minMaxIdx(matTempBufG64, &minvalue_G64, &maxvlaue_G64, NULL, NULL);
			double dImageavg_G64 = tempMean_G64[0];
			double dRatio2 = maxvlaue_G64 / dImageavg_G64;

			double dRatio = dRatio2 / dRatio1;

			matTempBufWhite.release();
			matTempBufG64.release();

			// 			//////////////////////////////////////////////////////////////////////////
			// 			// New Max GV Method
			// 			double Newmaxvlaue_W;
			// 			double Newmaxvlaue_G64;
			// 			int nTopGVCount = 3;
			// 
			// 			// Top 3 GV로 Max GV 다시 구하기
			// 			NewMaxGVMethold(matTempBufWhite, maxvlaue_W, Newmaxvlaue_W, nTopGVCount);
			// 			NewMaxGVMethold(matTempBufG64, maxvlaue_G64, Newmaxvlaue_G64, nTopGVCount);
			// 			double dNewRatio = (Newmaxvlaue_G64/dImageavg_G64) / (Newmaxvlaue_W/dImageavg_W);
						//////////////////////////////////////////////////////////////////////////

						// G64와 White에서 아주 밝게 올라오면 강시인 불량으로 Continue함
			if (maxvlaue_W >= 200 && maxvlaue_G64 >= 200)
				continue;

			if (dRatio <= dDimpleRatio_GrayPattern)
				//if (dNewRatio <= dDimpleRatio_GrayPattern)
			{
				// i번 영상이 Dimple로 의심 Dimple로 Defect_Type 변경
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DIMPLE;
				continue;
			}
			else
				continue;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// Dimple과 중심 좌표가 같은 불량 삭제
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		// Point 불량 아니면 돌리고
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_DIMPLE)
		{
			continue;
		}

		// 중심 좌표
		CPoint ptSrc1;
		ptSrc1.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X);
		ptSrc1.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y);

		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// 같으면 돌리고
			if (i == j)
			{
				j++;
				continue;
			}

			// Point 불량 아니면 돌리고
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP)
			{
				j++;
				continue;
			}

			// 불량 Pattern을 찾음
			int	nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Black Pattern 불량은 건들지 않다
			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_BLACK)
			{
				j++;
				continue;
			}

			// 불량 중심 좌표를 구한다
			CPoint ptSrc2;
			ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X);
			ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y);

			// 불량 중심점이 같다면...
			if (abs(ptSrc1.x - ptSrc2.x) < nOffSet && abs(ptSrc1.y - ptSrc2.y) < nOffSet)
			{
				// 리스트 작은거 삭제시
				if (i > j)	 i--;

				// 다른 Pattern의 Line 제거
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			else j++;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Dimple로 되어 있는 불량들을 제거
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		// Dimple이 아니면 돌리기
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE)
		{
			// Dimple이면 제거
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
		}
		else i++;
	}


	//////////////////////////////////////////////////////////////////////////
	//Green Pattern Retest Point 제거
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		// Retest Point 인지 확인 한다
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
		{
			// 불량 Pattern을 찾음
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Green Pattern에 있는 Retest Point 면 제거
			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_G)
			{
				resultPanelData.m_ListDefectInfo.RemoveAt(i);
			}
			else i++;

		}
		else i++;
	}

	mWhiteImage.release();
	mG64Image.release();

	return true;
}


bool AviInspection::DeleteOverlapDefect_SpecInSpotDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{
	// 예외처리 Parameter 가 없으면
	if (dAlignPara == NULL)
		return false;

	// 약명점 -S급 백점무라 PARAMETER
	double	dblSpotThRatio_Active = (double)(dAlignPara[E_PARA_SISPOT_RATIO_ACTIVE]);	//현 1로 설정
	double	dblSpotThRatio_Edge = (double)(dAlignPara[E_PARA_SISPOT_RATIO_EDGE]);		//현 1.04로 설정
	int		nGVCount = (int)(dAlignPara[E_PARA_SISPOT_NUMBER_GVCOUNT]); //GV Count: 7000
	int		nPoint_Area = (int)(dAlignPara[E_PARA_SISPOT_AREA]);  // Point Area 불량 면적 설정


	// 강명점 -F급 백점무라 PARAMETER
	double	dStrong_blSpotThRatio_Active = (double)(dAlignPara[E_PARA_STRONG_SISPOT_RATIO_ACTIVE]);	//현 1로 설정
	double	dStrong_blSpotThRatio_Edge = (double)(dAlignPara[E_PARA_STRONG_SISPOT_RATIO_EDGE]);		//현 1.04로 설정
	int		nStrong_GVCount = (int)(dAlignPara[E_PARA_STRONG_SISPOT_NUMBER_GVCOUNT]); //GV Count: 7000
	int		nStrong_Point_Area = (int)(dAlignPara[E_PARA_STRONG_SISPOT_AREA]);  // Point Area 불량 면적 설정




	// 예외처리 Dimpe Ratio가 없으면
	if (dblSpotThRatio_Active <= 0 || dblSpotThRatio_Edge <= 0 || nGVCount <= 0)
		return false;

	// 예외처리 Defect가 없으면
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// Gray Pattern Point Count
	int nGrayDefect = 0;
	int nStrong_GrayDefect = 0;

	//
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		// 약명점 Point 불량인 경우만 찾음
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT &&  nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_64 && resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel >= nPoint_Area)
		{
			nGrayDefect++;
		}
		//강명점
		if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT) &&
			nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_64 && resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel >= nStrong_Point_Area)
		{
			nStrong_GrayDefect++;
		}

	}


	//////////////////////////////////////////////////////////////////////////
	// 명점 불량 확인 작업 ver. 0.0ver
	// 정보: PNZ/2018/04/19
	// 내용: 불량에 대해 진가성을 확인, 불량 좌표영역의 
	//////////////////////////////////////////////////////////////////////////
	if (nGrayDefect > 0 && nPoint_Area >= 0)
	{
		// G64 Pattern 원본 영상 Load
		cv::Mat mG64Image = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_GRAY_64)][0];

		// ROI Region
		int nXStart = m_stThrdAlignInfo.rcAlignCellROI.x;
		int nYStart = m_stThrdAlignInfo.rcAlignCellROI.y;
		int nXEnd = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
		int nYEnd = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// Pattern 확인 작업
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Point 불량인 경우만 찾음
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT  && resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel >= nPoint_Area
				//|| resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	
				)
			{
				if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_64)
				{
					i++;
					continue;
				}

				////////////////////////////////
				// Max-Mean Ratio Calculation //
// 				int nDefectMaxGV	= (int)	resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
// 				int nDefectMeanGV	= (int) resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
// 				float MaxMeanRatio	= (float) nDefectMaxGV / nDefectMeanGV;
// 
// 				// 임시 Max/Mean Ratio의 최대 값으로 분류 함
// 				if ( MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
// 				{
// 					i++;
// 					continue;
// 				}

				/////////////////////////////////
				// 위치 확인 Threshold Ratio 선택//
				double	dblSpotThRatio = 0;
				int		nEdgeOffset = 100;

				// Center 좌표
				int nDefectCenter_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nDefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				// Edge부터 100Pixel에서 Edge Ratio 사용
				if ((nXStart <= nDefectCenter_X - nEdgeOffset) ||
					(nXEnd >= nDefectCenter_X + nEdgeOffset) ||
					(nYStart <= nDefectCenter_Y - nEdgeOffset) ||
					(nYEnd >= nDefectCenter_Y + nEdgeOffset))
				{
					dblSpotThRatio = dblSpotThRatio_Edge;
				}

				else
					dblSpotThRatio = dblSpotThRatio_Active;

				////////////////////////////
				// Region Image Selection //

				// Image Offset
				int nOffset = 100;

				// 시작 좌표
				int nCal_X = nDefectCenter_X - nOffset;
				int nCal_Y = nDefectCenter_Y - nOffset;

				// 영역 설정 200x200
				cv::Rect rectTemp;

				rectTemp.x = nCal_X;
				rectTemp.y = nCal_Y;
				rectTemp.width = nOffset * 2;
				rectTemp.height = nOffset * 2;

				cv::Mat matRegion = mG64Image(rectTemp);

				////////////////////////////
				// Shift Copy Enhancement //

				int nShiftUnit = 5;

				cv::Mat matBuff, matDstBuff, matTempBuff1, matTempBuff2;
				matRegion.copyTo(matBuff);
				matRegion.copyTo(matDstBuff);

				// size
				int nImageSizeX = matRegion.cols;
				int nImageSizeY = matRegion.rows;

				matTempBuff1 = matDstBuff(cv::Rect(0, 0, nImageSizeX - nShiftUnit, nImageSizeY));
				matTempBuff2 = matBuff(cv::Rect(nShiftUnit, 0, nImageSizeX - nShiftUnit, nImageSizeY));

				cv::add(matTempBuff1, matTempBuff2, matTempBuff1);

				nShiftUnit /= 2;

				cv::Mat matDstImage = cv::Mat::zeros(matRegion.size(), matRegion.type());
				matDstBuff(cv::Rect(0, 0, matDstBuff.cols - nShiftUnit, matDstBuff.rows)).copyTo(matDstImage(cv::Rect(nShiftUnit, 0, matDstBuff.cols - nShiftUnit, matDstBuff.rows)));

				/////////////////////////////////////////
				// Region Image Mean Value Calculation //

				int nRegionUnit = 50;

				cv::Mat matSubRegion = matDstImage(cv::Rect(nRegionUnit, nRegionUnit, matDstImage.cols - nRegionUnit * 2, matDstImage.rows - nRegionUnit * 2));
				cv::GaussianBlur(matSubRegion, matSubRegion, cv::Size(19, 19), 3.0, 3.0);

				cv::Scalar m, s;
				cv::meanStdDev(matSubRegion, m, s);
				double Imagemean = m[0];

				// 조각 영상 추출
				nRegionUnit = 70;
				cv::Mat matResult = matDstImage(cv::Rect(nRegionUnit, nRegionUnit, matDstImage.cols - nRegionUnit * 2, matDstImage.rows - nRegionUnit * 2));

				///////////////////////////
				// Histogram Calculation //

				cv::Mat matHisto;
				int nHistSize = 256;
				float fHistRange[] = { 0.0f, (float)(nHistSize - 1) };
				const float *ranges[] = { fHistRange };
				cv::calcHist(&matResult, 1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);
				int ImageImageMean = (int)(Imagemean * dblSpotThRatio);
				float *pVal = (float*)matHisto.data;

				// Diff x GV Calculation
				__int64 nPixelSum = 0;
				__int64 nPixelCount = 0;

				pVal = (float*)matHisto.ptr(0) + ImageImageMean;

				for (int m = ImageImageMean; m <= 255; m++, pVal++)
				{
					int nDiff = m - ImageImageMean;
					nPixelSum += (long)(nDiff * *pVal);
				}

				double dblDiffxGVCount = (double)nPixelSum;

				// Memory Release
				matBuff.release();
				matDstBuff.release();
				matTempBuff1.release();
				matTempBuff2.release();

				matDstImage.release();
				matSubRegion.release();
				matResult.release();

				matRegion.release();

				// 설정 값보다 큰 불량을 제거
				if (dblDiffxGVCount > nGVCount)
				{
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT;
				}

				//resultPanelData.m_ListDefectInfo.RemoveAt(i);



				else i++;
			}

			else i++;
		}

		mG64Image.release();
	}




	//강시인
	if (nStrong_GrayDefect > 0 && nStrong_Point_Area >= 0)
	{
		// G64 Pattern 원본 영상 Load
		cv::Mat mG64Image = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_GRAY_64)][0];

		// ROI Region
		int nXStart = m_stThrdAlignInfo.rcAlignCellROI.x;
		int nYStart = m_stThrdAlignInfo.rcAlignCellROI.y;
		int nXEnd = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
		int nYEnd = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// Pattern 확인 작업
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Point 불량인 경우만 찾음
			if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT) &&
				resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel >= nStrong_Point_Area)
			{
				if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_64)
				{
					i++;
					continue;
				}

				////////////////////////////////
				// Max-Mean Ratio Calculation //
				// 				int nDefectMaxGV	= (int)	resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
				// 				int nDefectMeanGV	= (int) resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
				// 				float MaxMeanRatio	= (float) nDefectMaxGV / nDefectMeanGV;
				// 
				// 				// 임시 Max/Mean Ratio의 최대 값으로 분류 함
				// 				if ( MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
				// 				{
				// 					i++;
				// 					continue;
				// 				}

				/////////////////////////////////
				// 위치 확인 Threshold Ratio 선택//
				double	dblSpotThRatio = 0;
				int		nEdgeOffset = 100;

				// Center 좌표
				int nDefectCenter_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nDefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				// Edge부터 100Pixel에서 Edge Ratio 사용
				if ((nXStart <= nDefectCenter_X - nEdgeOffset) ||
					(nXEnd >= nDefectCenter_X + nEdgeOffset) ||
					(nYStart <= nDefectCenter_Y - nEdgeOffset) ||
					(nYEnd >= nDefectCenter_Y + nEdgeOffset))
				{
					dblSpotThRatio = dStrong_blSpotThRatio_Edge;
				}

				else
					dblSpotThRatio = dStrong_blSpotThRatio_Active;

				////////////////////////////
				// Region Image Selection //

				// Image Offset
				int nOffset = 100;

				// 시작 좌표
				int nCal_X = nDefectCenter_X - nOffset;
				int nCal_Y = nDefectCenter_Y - nOffset;

				// 영역 설정 200x200
				cv::Rect rectTemp;

				rectTemp.x = nCal_X;
				rectTemp.y = nCal_Y;
				rectTemp.width = nOffset * 2;
				rectTemp.height = nOffset * 2;

				cv::Mat matRegion = mG64Image(rectTemp);

				////////////////////////////
				// Shift Copy Enhancement //

				int nShiftUnit = 5;

				cv::Mat matBuff, matDstBuff, matTempBuff1, matTempBuff2;
				matRegion.copyTo(matBuff);
				matRegion.copyTo(matDstBuff);

				// size
				int nImageSizeX = matRegion.cols;
				int nImageSizeY = matRegion.rows;

				matTempBuff1 = matDstBuff(cv::Rect(0, 0, nImageSizeX - nShiftUnit, nImageSizeY));
				matTempBuff2 = matBuff(cv::Rect(nShiftUnit, 0, nImageSizeX - nShiftUnit, nImageSizeY));

				cv::add(matTempBuff1, matTempBuff2, matTempBuff1);

				nShiftUnit /= 2;

				cv::Mat matDstImage = cv::Mat::zeros(matRegion.size(), matRegion.type());
				matDstBuff(cv::Rect(0, 0, matDstBuff.cols - nShiftUnit, matDstBuff.rows)).copyTo(matDstImage(cv::Rect(nShiftUnit, 0, matDstBuff.cols - nShiftUnit, matDstBuff.rows)));

				/////////////////////////////////////////
				// Region Image Mean Value Calculation //

				int nRegionUnit = 50;

				cv::Mat matSubRegion = matDstImage(cv::Rect(nRegionUnit, nRegionUnit, matDstImage.cols - nRegionUnit * 2, matDstImage.rows - nRegionUnit * 2));
				cv::GaussianBlur(matSubRegion, matSubRegion, cv::Size(19, 19), 3.0, 3.0);

				cv::Scalar m, s;
				cv::meanStdDev(matSubRegion, m, s);
				double Imagemean = m[0];

				// 조각 영상 추출
				nRegionUnit = 70;
				cv::Mat matResult = matDstImage(cv::Rect(nRegionUnit, nRegionUnit, matDstImage.cols - nRegionUnit * 2, matDstImage.rows - nRegionUnit * 2));

				///////////////////////////
				// Histogram Calculation //

				cv::Mat matHisto;
				int nHistSize = 256;
				float fHistRange[] = { 0.0f, (float)(nHistSize - 1) };
				const float *ranges[] = { fHistRange };
				cv::calcHist(&matResult, 1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);
				int ImageImageMean = (int)(Imagemean * dblSpotThRatio);
				float *pVal = (float*)matHisto.data;

				// Diff x GV Calculation
				__int64 nPixelSum = 0;
				__int64 nPixelCount = 0;

				pVal = (float*)matHisto.ptr(0) + ImageImageMean;

				for (int m = ImageImageMean; m <= 255; m++, pVal++)
				{
					int nDiff = m - ImageImageMean;
					nPixelSum += (long)(nDiff * *pVal);
				}

				double dblDiffxGVCount = (double)nPixelSum;

				// Memory Release
				matBuff.release();
				matDstBuff.release();
				matTempBuff1.release();
				matTempBuff2.release();

				matDstImage.release();
				matSubRegion.release();
				matResult.release();

				matRegion.release();

				// 설정 값보다 큰 불량을 제거
				if (dblDiffxGVCount > nStrong_GVCount)
				{
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}

				else i++;
			}

			else i++;
		}

		mG64Image.release();
	}








	// white pattern (Black hole Mura 판정)



	return true;
}

bool AviInspection::DeleteOverlapDefect_BlackHole(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{
	// 예외처리 Parameter 가 없으면
	if (dAlignPara == NULL)
		return false;

	// PARAMETER
	double	dblSpotThRatio_Active = (double)(dAlignPara[E_PARA_SISPOT_RATIO_ACTIVE]);	//현 1로 설정
	double	dblSpotThRatio_Edge = (double)(dAlignPara[E_PARA_SISPOT_RATIO_EDGE]);		//현 1.04로 설정
	int		nGVCount = (int)(dAlignPara[E_PARA_SISPOT_NUMBER_GVCOUNT]); //GV Count: 7000

																		// 예외처리 Dimpe Ratio가 없으면
	if (dblSpotThRatio_Active <= 0 || dblSpotThRatio_Edge <= 0 || nGVCount <= 0)
		return false;

	// 예외처리 Defect가 없으면
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// Gray Pattern Point Count
	int nGrayDefect = 0;

	//
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		// Point 불량인 경우만 찾음
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT
			)
		{
			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_64)
			{
				nGrayDefect++;
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// 명점 불량 확인 작업 ver. 0.0ver
	// 정보: PNZ/2018/04/19
	// 내용: 불량에 대해 진가성을 확인, 불량 좌표영역의 
	//////////////////////////////////////////////////////////////////////////
	if (nGrayDefect > 0)
	{
		// G64 Pattern 원본 영상 Load
		cv::Mat mWhiteImage = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_WHITE)][0];

		// ROI Region
		int nXStart = m_stThrdAlignInfo.rcAlignCellROI.x;
		int nYStart = m_stThrdAlignInfo.rcAlignCellROI.y;
		int nXEnd = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
		int nYEnd = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// Pattern 확인 작업
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Point 불량인 경우만 찾음
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT
				)
			{
				if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_64)
				{
					i++;
					continue;
				}

				////////////////////////////////
				// Max-Mean Ratio Calculation //
				// 				int nDefectMaxGV	= (int)	resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
				// 				int nDefectMeanGV	= (int) resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
				// 				float MaxMeanRatio	= (float) nDefectMaxGV / nDefectMeanGV;
				// 
				// 				// 임시 Max/Mean Ratio의 최대 값으로 분류 함
				// 				if ( MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
				// 				{
				// 					i++;
				// 					continue;
				// 				}

				/////////////////////////////////
				// 위치 확인 Threshold Ratio 선택//
				double	dblSpotThRatio = 0;
				int		nEdgeOffset = 100;

				// Center 좌표
				int nDefectCenter_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nDefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				// Edge부터 100Pixel에서 Edge Ratio 사용
				if ((nXStart <= nDefectCenter_X - nEdgeOffset) ||
					(nXEnd >= nDefectCenter_X + nEdgeOffset) ||
					(nYStart <= nDefectCenter_Y - nEdgeOffset) ||
					(nYEnd >= nDefectCenter_Y + nEdgeOffset))
				{
					dblSpotThRatio = dblSpotThRatio_Edge;
				}

				else
					dblSpotThRatio = dblSpotThRatio_Active;

				////////////////////////////
				// Region Image Selection //

				// Image Offset
				int nOffset = 100;

				// 시작 좌표
				int nCal_X = nDefectCenter_X - nOffset;
				int nCal_Y = nDefectCenter_Y - nOffset;

				// 영역 설정 200x200
				cv::Rect rectTemp;

				rectTemp.x = nCal_X;
				rectTemp.y = nCal_Y;
				rectTemp.width = nOffset * 2;
				rectTemp.height = nOffset * 2;

				cv::Mat matRegion = mWhiteImage(rectTemp);

				////////////////////////////
				// Shift Copy Enhancement //

				int nShiftUnit = 5;

				cv::Mat matBuff, matDstBuff, matTempBuff1, matTempBuff2;
				matRegion.copyTo(matBuff);
				matRegion.copyTo(matDstBuff);

				// size
				int nImageSizeX = matRegion.cols;
				int nImageSizeY = matRegion.rows;

				matTempBuff1 = matDstBuff(cv::Rect(0, 0, nImageSizeX - nShiftUnit, nImageSizeY));
				matTempBuff2 = matBuff(cv::Rect(nShiftUnit, 0, nImageSizeX - nShiftUnit, nImageSizeY));

				cv::add(matTempBuff1, matTempBuff2, matTempBuff1);

				nShiftUnit /= 2;

				cv::Mat matDstImage = cv::Mat::zeros(matRegion.size(), matRegion.type());
				matDstBuff(cv::Rect(0, 0, matDstBuff.cols - nShiftUnit, matDstBuff.rows)).copyTo(matDstImage(cv::Rect(nShiftUnit, 0, matDstBuff.cols - nShiftUnit, matDstBuff.rows)));

				/////////////////////////////////////////
				// Region Image Mean Value Calculation //

				int nRegionUnit = 50;

				cv::Mat matSubRegion = matDstImage(cv::Rect(nRegionUnit, nRegionUnit, matDstImage.cols - nRegionUnit * 2, matDstImage.rows - nRegionUnit * 2));
				cv::GaussianBlur(matSubRegion, matSubRegion, cv::Size(19, 19), 3.0, 3.0);

				cv::Scalar m, s;
				cv::meanStdDev(matSubRegion, m, s);
				double Imagemean = m[0];

				// 조각 영상 추출
				nRegionUnit = 70;
				cv::Mat matResult = matDstImage(cv::Rect(nRegionUnit, nRegionUnit, matDstImage.cols - nRegionUnit * 2, matDstImage.rows - nRegionUnit * 2));

				///////////////////////////
				// Histogram Calculation //

				cv::Mat matHisto;
				int nHistSize = 256;
				float fHistRange[] = { 0.0f, (float)(nHistSize - 1) };
				const float *ranges[] = { fHistRange };
				cv::calcHist(&matResult, 1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);
				int ImageImageMean = (int)(Imagemean * dblSpotThRatio);
				float *pVal = (float*)matHisto.data;

				// Diff x GV Calculation
				__int64 nPixelSum = 0;
				__int64 nPixelCount = 0;

				pVal = (float*)matHisto.ptr(0) + ImageImageMean;

				for (int m = ImageImageMean; m <= 255; m++, pVal++)
				{
					int nDiff = m - ImageImageMean;
					nPixelSum += (long)(nDiff * *pVal);
				}

				double dblDiffxGVCount = (double)nPixelSum;

				// Memory Release
				matBuff.release();
				matDstBuff.release();
				matTempBuff1.release();
				matTempBuff2.release();

				matDstImage.release();
				matSubRegion.release();
				matResult.release();

				matRegion.release();

				// 설정 값보다 큰 불량을 제거
				if (dblDiffxGVCount > nGVCount)
					//resultPanelData.m_ListDefectInfo.RemoveAt(i);

					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT;

				else i++;
			}

			else i++;
		}

		mWhiteImage.release();
	}




	// white pattern (Black hole Mura 판정)



	return true;
}

// Dust Pattern을 이용하여 암점 불량이 불량인지를 확인
bool AviInspection::DeleteOverlapDefect_DustDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{
	// 예외처리 Parameter 가 없으면
	if (dAlignPara == NULL)
		return false;

	// PARAMETER
	int nOnOffKey = (int)(dAlignPara[E_PARA_DUST_DARKPOINT_ONOFF]);	//현 1로 설정

	// 0이면 사용 안함
	if (nOnOffKey == 0)
		return true;

	// 예외처리 Defect가 없으면
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// ALL Pattern Dark Point Count
	int nDarkDefect = 0;

	//
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		// Point 불량인 경우만 찾음
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
		{
			nDarkDefect++;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// 암점 불량 진가성 확인 작업
	// 정보: PNZ/2018/05/17

	if (nDarkDefect > 0)
	{
		// G64 Pattern 원본 영상 Load
		cv::Mat mDustImage = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][0];

		// ROI Region
		int nXStart = m_stThrdAlignInfo.rcAlignCellROI.x;
		int nYStart = m_stThrdAlignInfo.rcAlignCellROI.y;
		int nXEnd = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
		int nYEnd = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{

			// Point 불량인 경우만 찾음
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
			{

				////////////////////////////
				// Region Image Selection //

				// Image Offset
				int nOffset = 100;

				// Center 좌표
				int nDefectCenter_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nDefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				// 시작 좌표
				int nCal_X = nDefectCenter_X - nOffset;
				int nCal_Y = nDefectCenter_Y - nOffset;

				// 영역 설정 200x200
				cv::Rect rectTemp;

				rectTemp.x = nCal_X;
				rectTemp.y = nCal_Y;
				rectTemp.width = nOffset * 2;
				rectTemp.height = nOffset * 2;

				cv::Mat matRegion = mDustImage(rectTemp);

				////////////////////////////
				// Shift Copy Enhancement //

				int nShiftUnit = 5;

				cv::Mat matBuff, matDstBuff, matTempBuff1, matTempBuff2;
				matRegion.copyTo(matBuff);
				matRegion.copyTo(matDstBuff);

				// size
				int nImageSizeX = matRegion.cols;
				int nImageSizeY = matRegion.rows;

				matTempBuff1 = matDstBuff(cv::Rect(0, 0, nImageSizeX - nShiftUnit, nImageSizeY));
				matTempBuff2 = matBuff(cv::Rect(nShiftUnit, 0, nImageSizeX - nShiftUnit, nImageSizeY));

				cv::add(matTempBuff1, matTempBuff2, matTempBuff1);

				nShiftUnit /= 2;

				cv::Mat matDstImage = cv::Mat::zeros(matRegion.size(), matRegion.type());
				matDstBuff(cv::Rect(0, 0, matDstBuff.cols - nShiftUnit, matDstBuff.rows)).copyTo(matDstImage(cv::Rect(nShiftUnit, 0, matDstBuff.cols - nShiftUnit, matDstBuff.rows)));

				//////////////////////////////////////////////////////////////////////////
				// Big Region Image MIN/Mean/MAX Value Calculation

				// 변두리 영역을 삭제한다
				int nBigRegionUnit = 10;

				// Enhance된 영상을 얻은다
				cv::Mat matBigSubRegion = matDstImage(cv::Rect(nBigRegionUnit, nBigRegionUnit, matDstImage.cols - nBigRegionUnit * 2, matDstImage.rows - nBigRegionUnit * 2));

				cv::GaussianBlur(matBigSubRegion, matBigSubRegion, cv::Size(11, 11), 3.0, 3.0);

				cv::Scalar sBigmean, sBigSdt;
				double dbBigminvalue;
				double dbBigmaxvalue;

				cv::meanStdDev(matBigSubRegion, sBigmean, sBigSdt);
				cv::minMaxIdx(matBigSubRegion, &dbBigminvalue, &dbBigmaxvalue, NULL, NULL);

				double dbBigImagemean = sBigmean[0];
				double dbBigImageStdDev = sBigSdt[0];
				double dbBigImageSub = dbBigmaxvalue - dbBigminvalue;

				//////////////////////////////////////////////////////////////////////////
				// Small Region Image MIN/Mean/MAX Value Calculation

				// 변두리 영역을 삭제한다
				int nSmallRegionUnit = 70;

				// Enhance된 영상을 얻은다
				cv::Mat matSmallSubRegion = matBigSubRegion(cv::Rect(nSmallRegionUnit, nSmallRegionUnit, matBigSubRegion.cols - nSmallRegionUnit * 2, matBigSubRegion.rows - nSmallRegionUnit * 2));

				cv::Scalar sSmallmean, sSmallSdt;
				double dbSmallminvalue;
				double dbSmallmaxvalue;

				cv::meanStdDev(matSmallSubRegion, sSmallmean, sSmallSdt);
				cv::minMaxIdx(matSmallSubRegion, &dbSmallminvalue, &dbSmallmaxvalue, NULL, NULL);

				double dbSmallImagemean = sSmallmean[0];
				double dbSmallImageStdDev = sSmallSdt[0];
				double dbSmallImageSub = dbSmallmaxvalue - dbSmallminvalue;

				//////////////////////////////////////////////////////////////////////////
				// Dust 판정 Logic

				// 판정 결과 초기화
				bool	bDustDefect = false;

				double	dbIniStdDev = 1.0;
				double	dbSubStdDev = 0.35;

				if ((dbBigImageStdDev <= dbIniStdDev) && (dbSmallImageStdDev <= dbIniStdDev))
				{
					if ((dbBigImageStdDev < dbSmallImageStdDev) && (dbSmallImageStdDev - dbBigImageStdDev > dbSubStdDev)) bDustDefect = true;
					else bDustDefect = false;
				}

				else if ((dbBigImageStdDev > dbIniStdDev) && (dbSmallImageStdDev > dbIniStdDev))
					bDustDefect = true;

				else if ((dbBigImageStdDev <= dbIniStdDev) && (dbSmallImageStdDev > dbIniStdDev))
				{
					if (abs(dbBigImageStdDev - dbSmallImageStdDev) <= dbSubStdDev) bDustDefect = false;
					else bDustDefect = true;
				}

				else if ((dbBigImageStdDev > dbIniStdDev) && (dbSmallImageStdDev < dbIniStdDev))
					bDustDefect = false;

				// Memory Release
				matRegion.release();
				matBuff.release();
				matDstBuff.release();
				matTempBuff1.release();
				matTempBuff2.release();
				matDstImage.release();
				matBigSubRegion.release();
				matSmallSubRegion.release();

				// 설정 값보다 큰 불량을 제거
				if (bDustDefect == true)
					resultPanelData.m_ListDefectInfo.RemoveAt(i);

				else i++;

			}

			else i++;

		}
		mDustImage.release();
	}

	return true;
}


bool AviInspection::DeleteOverlapDefect_BlackSmallDelet(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// 예외처리 Parameter 가 없으면
	if (dAlignPara == NULL)
		return false;



	// 예외처리 Defect가 없으면
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// Black Pattern Point Count
	int nBlackPointDefect = 0;

	// 수평/수직 방향 Offset를 설정
	int nBig_Offset = 40;
	int nSmall_Offset = 5;

	// 판정 초기화
	bool bSmallWeakDefect = false;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		// Point 불량인 경우만 찾음
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
		{
			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_BLACK)
			{
				nBlackPointDefect++;
			}
		}
	}

	if (nBlackPointDefect > 1)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Point 불량인 경우만 찾음
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				// Pattern 정보를 얻음 Black Pattern에 있는 명점 불량만 찾음
				int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
				int	nImgNum1 = theApp.GetImageClassify(nImgNum);

				if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_BLACK) continue;

				// 불량의 밝기 정보를 얻음
				int nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
				int nDefectArea = (int)resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel;

				// 강하지 않고 작은 불량은 제외
				if ((nDefectMaxGV <= 200) && (nDefectArea <= 100)) continue;

				// 강불량의 Center 좌표
				int nBig_DefectCenter_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nBig_DefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j)
					{
						j++;
						continue;
					}

					// Point 불량인 경우만 찾음
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
					{
						j++;
						continue;
					}

					// Pattern 정보를 얻음 Black Pattern에 있는 명점 불량만 찾음
					nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
					nImgNum1 = theApp.GetImageClassify(nImgNum);

					if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_BLACK)
					{
						j++;
						continue;
					}

					// 불량의 밝기 정보를 얻음
					nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[j].Defect_MaxGV;
					nDefectArea = (int)resultPanelData.m_ListDefectInfo[j].Defect_Size_Pixel;

					// 약하지 않고 큰 불량은 제외
					if ((nDefectMaxGV >= 20) && (nDefectArea >= 10))
					{
						j++;
						continue;
					}

					// 판정 초기화
					bSmallWeakDefect = false;

					// 약불량의 Center 좌표
					int nSmall_DefectCenter_X = (int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;
					int nSmall_DefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y;

					// X/Y 방향 한 방향이 같은 현상이 있음, 조건에 만족하면 Small Weak Defect으로 판정
					if ((abs(nBig_DefectCenter_X - nSmall_DefectCenter_X) <= 5) && (abs(nBig_DefectCenter_Y - nSmall_DefectCenter_Y) <= 40) ||
						(abs(nBig_DefectCenter_X - nSmall_DefectCenter_X) <= 40) && (abs(nBig_DefectCenter_Y - nSmall_DefectCenter_Y) <= 5))
					{
						bSmallWeakDefect = true;
					}

					// Small Weak 명점이면 제거
					if (bSmallWeakDefect == true)
					{
						if (i > j)	 i--;
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					else  j++;
				}
			}
		}
	}

	return true;
}



bool AviInspection::DeleteOverlapDefect_Point_Point(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;


	int nOffSet = (int)(dAlignPara[E_PARA_Duplicate_offset]);

	//int nOffSet = 6;

	//////////////////////////////////////////////////////////////////////////
	// KSW 17.10.24 - White에서 명점 시인 / Black에서 명점 미시인인 경우 => 기포
	//////////////////////////////////////////////////////////////////////////

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		// Point 불량인 경우만...
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
		{
			i++;
			continue;
		}

		// White 패턴인 경우만...
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_WHITE)
		{
			i++;
			continue;
		}

		// 불량 중심 좌표
		CPoint ptSrc1;
		ptSrc1.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio);
		ptSrc1.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

		// White 불량 밝기
		int nWhiteGV = resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;

		// 기포 판정 확인
		bool bBubble = true;

		// 비교할 불량
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// 같은 불량 항목 비교 금지
			if (i == j)
			{
				j++;
				continue;
			}

			// Point 불량인 경우만...
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP			&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT)
			{
				j++;
				continue;
			}

			// 			// Black 패턴인 경우만...
			// 			nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
			// 			if( theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_BLACK )
			// 			{
			// 				j++;
			// 				continue;
			// 			}

						// 불량 중심 좌표
			CPoint ptSrc2;
			ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// 불량 중심점이 같다면...
			if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&
				abs(ptSrc1.y - ptSrc2.y) < nOffSet)
			{
				// 명점으로 판정 ( White - Black 중복 )
				bBubble = false;

				// 나가기
				break;
			}
			// 중복 안돼면... 다음 불량으로...
			else j++;
		}

		// White & Black 중복되는 경우이므로, 명점 판정
		if (!bBubble)
		{
			// White 패턴 불량 삭제
			resultPanelData.m_ListDefectInfo.RemoveAt(i);

			continue;
		}
		// 중복되는 경우가 아닌 경우, 기포 판정
		else
		{
			//////////////////////////////////////////////////////////////////////////
			// KSW 17.12.14 - 2차 기포 확인 start
			// 기포로 인한 경우, 빛이 모여서 확인 되는 경우
			// 점등 빛이 어두우면 잘 안보임
			// 불량 GV값이 White < Gray 인 경우 진짜 불량일 확률 높음

			// 비교할 불량
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// 같은 불량 항목 비교 금지
				if (i == j)
				{
					j++;
					continue;
				}

				// Point 불량인 경우만...
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP			&&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT	&&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK		&&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT)
				{
					j++;
					continue;
				}

				// Gray 패턴인 경우만...
				nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
				if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_32	&&
					theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_64	&&
					theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_87	&&
					theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_128)
				{
					j++;
					continue;
				}

				// 불량 중심 좌표
				CPoint ptSrc2;
				ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				// 불량 중심점이 같다면...
				if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&
					abs(ptSrc1.y - ptSrc2.y) < nOffSet)
				{
					// Gray 불량 밝기
					int nGrayGV = resultPanelData.m_ListDefectInfo[j].Defect_MaxGV;

					// White보다 Gray 밝기가 밝다면...
					if (nWhiteGV <= nGrayGV)
					{
						// 명점으로 판정 ( White - Black 중복 )
						bBubble = false;

						// 나가기
						break;
					}
					// White보다 Gray 밝기가 어두우면... 다음 불량으로...
					else j++;
				}
				// 중복 안돼면... 다음 불량으로...
				else j++;
			}

			// White & Black 중복되는 경우이므로, 명점 판정
			if (!bBubble)
			{
				// White 패턴 불량 삭제
				resultPanelData.m_ListDefectInfo.RemoveAt(i);

				continue;
			}

			// KSW 17.12.14 - 2차 기포 확인 end
			//////////////////////////////////////////////////////////////////////////
		}

		// 기포와 같은 좌표 모두 삭제
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// 같은 불량 항목 비교 금지
			if (i == j)
			{
				j++;
				continue;
			}

			// Point 불량인 경우만...
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP			&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT)
			{
				j++;
				continue;
			}

			// 불량 중심 좌표
			CPoint ptSrc2;
			ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// 불량 중심점이 같다면...
			if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&
				abs(ptSrc1.y - ptSrc2.y) < nOffSet)
			{
				if (i > j)   i--;

				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// 중복 안돼면... 다음 불량으로...
			else j++;
		}

		// White 패턴 불량 삭제
		resultPanelData.m_ListDefectInfo.RemoveAt(i);
	}

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.01.15 - White에서 암점 시인 / 암점 크기 비교하여 과검 요인 제외
	//////////////////////////////////////////////////////////////////////////

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		// Point 불량인 경우만...
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_DARK			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
		{
			i++;
			continue;
		}

		// White 패턴인 경우만...
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_WHITE)
		{
			i++;
			continue;
		}

		// White 불량 범위
		CRect rectTemp = CRect(
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - 40,
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - 40,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + 40,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + 40);

		// 비교할 불량
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// 같은 불량 항목 비교 금지
			if (i == j)
			{
				j++;
				continue;
			}

			// Point 불량인 경우만...
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_DARK			&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_DARK		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_RETEST_POINT_DARK		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK)
			{
				j++;
				continue;
			}

			// 불량 중심 좌표
			CPoint ptSrc;
			ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// 범위 안에 불량 존재?
			if (rectTemp.PtInRect(ptSrc))
			{
				if (i > j)   i--;

				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// 중복 안돼면... 다음 불량으로...
			else j++;
		}

		// White 패턴 불량 삭제
		resultPanelData.m_ListDefectInfo.RemoveAt(i);
	}


	//////////////////////////////////////////////////////////////////////////
	// 명점 & 암점 & 파티클 비교
	//////////////////////////////////////////////////////////////////////////

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Point 불량인 경우만...
		if ((resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_POINT_DARK			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_MULT_BP)	// 17.09.28 추가
		{
			// 불량 중심 좌표
			// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
			CPoint ptSrc1;
			ptSrc1.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio);
			ptSrc1.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

			// 비교할 불량
			for (int j = i + 1; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// Point 불량 아닌것 제외
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK ||
					(resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_MURA_MULT_BP) ||	// 17.09.28 추가
						(resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_MURA_MULT_BP			&&
							resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_RETEST_POINT_DARK) ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
				{
					j++;
					continue;
				}

				if ((resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK) ||
					(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK))
				{
					j++;
					continue;
				}

				// 불량 중심 좌표
				// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
				CPoint ptSrc2;
				ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				// 불량 중심점이 같다면...
				if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&	abs(ptSrc1.y - ptSrc2.y) < nOffSet)
				{
					int nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;


					// KSW 17.09.11 - 중복 될 시, 다른 패턴보다 Black 패턴 우선 보고 ( 고객사 요청 )
					if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_BLACK)
					{
						ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
						ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp2;
						resultPanelData.m_ListDefectInfo[j] = tTemp1;
					}

					// 					// KSW 17.12.14 - 중복 될 시, 다른 패턴보다 Gray 패턴 우선 보고
					// 					if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_GRAY_64)
					// 					{
					// 						ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
					// 						ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
					// 						resultPanelData.m_ListDefectInfo[i] = tTemp2;
					// 						resultPanelData.m_ListDefectInfo[j] = tTemp1;
					// 					}



										// KSW 17.10.12 - 멀티인경우 우선 보고 ( 고객사 요청 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_MULT_BP)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// 					// 명/암점인 경우, 판정 수정
					// 					else if(	resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT	&&
					// 								resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK	)
					// 					{
					// 						resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK;
					// 					}
					// 					// 명/암점인 경우, 판정 수정
					// 					else if(resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK	&&
					// 							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT	)
					// 					{
					// 						resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK;
					// 					}
										//////////////////////////////////////////////////////////////////////////
										// PNZ 2018-05-08 고객사 요청: Point_Bright_Dark 불량 안 나오게

					else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK)
					{
					}
					// 명/암점인 경우, 판정 수정
					else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}

					//// 약명/암점인 경우, 판정 수정
					//else if(	resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT	&&
					//			resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK		)
					//{
					//	resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK;
					//}
					//// 약명/암점인 경우, 판정 수정
					//else if(resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK		&&
					//		resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT	)
					//{
					//	resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK;
					//}
					// 명/암점인 경우, 다른 불량 지우기
					else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// Retest인 경우 삭제
					else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// Retest인 경우 삭제
					else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// choi 20.06.23
					else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// KSW 17.07.19 - 군집 우선순위
					else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// KSW 17.07.19 - 군집 우선순위
					else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// 					// 파티클인 경우 남기고, 다른 불량 지우기
					// 					// 암정 - 파티클 인 경우 : 암점삭제
					// 					else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_PARTICLE)
					// 					{
					// 						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
					// 						resultPanelData.m_ListDefectInfo[i] = tTemp;
					// 					}

										// 해당 불량 삭제
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
				}
				else j++;  // 불량 중심점이 같지 않다면...다음 불량으로...
			}
		}
	}

	// 	////////////////////////////////////////////////////////////////////////
	// 		// 파티클 - 명점 & 암점 비교
	// 		//////////////////////////////////////////////////////////////////////////
	// 	
	// 		// 불량 갯수 만큼
	// 		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	// 		{
	// 			// 파티클 불량 크기가 크므로 중심점만으로 제거 안되는경우 발생
	// 			// 파티클 내부에 중심 있는 경우 제거
	// 			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_PARTICLE)
	// 			{
	// 				// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
	// 				CRect rectTemp = CRect(
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	/ resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_End_X	/ resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);
	// 	
	// 				// 비교할 불량
	// 				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
	// 				{
	// 					// 같은 불량 항목 비교 금지
	// 					if (i == j)
	// 					{
	// 						j++;
	// 						continue;
	// 					}
	// 	
	// 					// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
	// 					// 불량 중심 좌표
	// 					CPoint ptSrc;
	// 					ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
	// 					ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
	// 	
	// 					// 범위 안에 불량 존재?
	// 					if ( rectTemp.PtInRect(ptSrc) )
	// 					{
	// 						if (i > j)   i--;
	// 	
	// 						// 해당 불량 삭제
	// 						resultPanelData.m_ListDefectInfo.RemoveAt(j);
	// 					}
	// 					// 중복 안돼면... 다음 불량으로...
	// 					else j++;
	// 				}
	// 			}
	// 		}
	// 	
	// 		//////////////////////////////////////////////////////////////////////////
	// 		// 파티클 - 명점 & 암점 비교
	// 		//////////////////////////////////////////////////////////////////////////
	// 	
	// 		// 불량 갯수 만큼
	// 		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	// 		{
	// 			// 파티클 불량 크기가 크므로 중심점만으로 제거 안되는경우 발생
	// 			// 파티클 내부에 중심 있는 경우 제거
	// 			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_PARTICLE)
	// 			{
	// 				// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
	// 				CRect rectTemp = CRect(
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	/ resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_End_X	/ resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);
	// 	
	// 				// 비교할 불량
	// 				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
	// 				{
	// 					// 같은 불량 항목 비교 금지
	// 					if (i == j)
	// 					{
	// 						j++;
	// 						continue;
	// 					}
	// 	
	// 					// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
	// 					// 불량 중심 좌표
	// 					CPoint ptSrc;
	// 					ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
	// 					ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
	// 	
	// 					// 범위 안에 불량 존재?
	// 					if ( rectTemp.PtInRect(ptSrc) )
	// 					{
	// 						if (i > j)   i--;
	// 	
	// 						// 해당 불량 삭제
	// 						resultPanelData.m_ListDefectInfo.RemoveAt(j);
	// 					}
	// 					// 중복 안돼면... 다음 불량으로...
	// 					else j++;
	// 				}
	// 			}
	// 		}

	return true;
}


bool AviInspection::DeleteOverlapDefect_Point_Line(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOffSet = 8; //choi 05.21 4->8로 변경

	//////////////////////////////////////////////////////////////////////////
	// 라인 - 명점 & 암점 & 파티클 비교
	//////////////////////////////////////////////////////////////////////////

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// KSW 17.07.17 - Line 불량 안에 Point 불량 있는 경우 -> Line Alg에서 하기로 함
		// 17.05.09 Line 불량 안에 Point 불량 있는 경우, 보고 해야 함
		// Line 불량인 경우
		if ((resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_LINE_X_BRIGHT			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_LINE_Y_EDGE_BRIGHT) ||
			(resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_RETEST_LINE_BRIGHT		&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_RETEST_LINE_DARK) ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_PCD_CRACK)
		{
			CRect rectTemp = CRect(
				(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
				(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
				(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
				(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);

			// 비교할 불량
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// 같은 불량 항목 비교 금지
				if (i == j)
				{
					j++;
					continue;
				}

				// Point 불량 아닌것 제외
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK ||
					(resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_MURA_MULT_BP) ||	// 17.09.28 추가
						(resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_MURA_MULT_BP			&&
							resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_RETEST_POINT_DARK) ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
				{
					j++;
					continue;
				}

				// 불량 중심 좌표
				CPoint ptSrc;
				ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				// Line - Point 불량만 중복 제거
				// 범위 안에 불량 존재?
				if (rectTemp.PtInRect(ptSrc))
				{
					// 현재 리스트보다 낮은 리스트 삭제 시...
					if (i > j)	i--;

					// 해당 불량 삭제
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
				}
				// 중복 안돼면... 다음 불량으로...
				else j++;
			}
		}
	}

	return true;
}


bool AviInspection::DeleteOverlapDefect_Point_Mura(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// KJY 17.09.18 - 누기 근처 포인트 제거
	// 불량이 많은 경우 속도가 느려 질 수 있음으로 위쪽에서 포인트 군집 관련 제거 후 실행 
	// 어차피 누기나 EMD등은 확실한 불량임으로 해당 불량 보다 좀더 크게 포인트를 제거 하여 준다.
	//////////////////////////////////////////////////////////////////////////

	if (dAlignPara != NULL)
	{
		int		nDefectTypeM, nDefectTypeS;
		int		nAdjustRange = (int)dAlignPara[E_PARA_STRONG_DEFECT_NOISE_ADJUST_RANGE];
		CRect	rcDeleteArea;
		CPoint	ptCenterSub;

		//픽셀 불량일때 무라를 제거
		if (nAdjustRange > 0)
		{
			// 불량 갯수 만큼
			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				nDefectTypeM = resultPanelData.m_ListDefectInfo[i].Defect_Type;

				// 대상 불량을 찾는다.
				if (nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_NUGI ||		// 누기
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI ||		// 외곽 수직 수평 누기
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_ ||		// 외곽 수직 수평 누기
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_EMP ||		// EMP
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_EMD_BRIGHT ||		// EMD
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_EMD_DARK ||		// EMD					
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_CORNER_CM ||		// 혼색 
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_UP_CM ||		// 혼색
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_FINGER_CM ||		// 혼색
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH ||		// 스크레치 ( F급임... )
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT)		// 2018-05-08 PNZ Mura 불량이다
				{
					// P/S 모드 좌표 보정
					// 크기는 박스로 설정 - 누기가 원형도 있지만 길죽한 형태도 다수 존재
					rcDeleteArea.SetRect(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

					// 좌표계 정규화
					rcDeleteArea.NormalizeRect();

					// 범위 확장
					rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);

					// 불량 갯수 만큼
					for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
					{
						// 같은 불량 항목 비교 금지
						if (i == j)
						{
							j++;
							continue;
						}

						nDefectTypeS = resultPanelData.m_ListDefectInfo[j].Defect_Type;

						// 해당 불량이 포인트 불량중 하나인지 확인
						if (nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_DARK ||		// 암점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||		// 명점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT ||		// 약명점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK ||		// 명암점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||		// 그룹암점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT ||		// 그룹명점
							nDefectTypeS == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK ||		// 암점 재검
							nDefectTypeS == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT ||		// 명점 재검
							nDefectTypeS == E_DEFECT_JUDGEMENT_MURA_MULT_BP)
						{
							//해당 불량이 black pattern 에서 명점이면 삭제하지 않음. Mura와 같이 보고 pwj
							int nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
							if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_BLACK)
							{
								j++;
								continue;
							}
							// P/S 모드 좌표 보정
							ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
							ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

							// 범위 안에 들어오면 삭제
							if (rcDeleteArea.PtInRect(ptCenterSub))
							{
								// Point 불량 삭제
								resultPanelData.m_ListDefectInfo.RemoveAt(j);

								// 현재 리스트보다 낮은 리스트 삭제 시...
								if (i > j)	i--;
							}
							else
							{
								j++; // 지우지 않았을 경우만 다음불량으로
								continue;
							}
						}
						else j++; // 포인트 불량이 아닐 경우 다음불량으로
					}
				}
			}
		}
	}

	//19-08-20 choikwangil 주석 ->  choi 05.01 주석 해제
	//////////////////////////////////////////////////////////////////////////
// 	// Mura Spot & 명점인 경우 -> 명점 제거 & Mura 보고
// 	//////////////////////////////////////////////////////////////////////////
// 
	int nOffSet = 5;

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Mura 밝은 Spot 불량 아닌 경우 제외
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT &&
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT)   // mura 삭제로 변경 boe11 choikwangil -> 05.01 choi 원복
// 			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT		&&
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK		&&
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP)

continue;

		CRect rectTemp = CRect(
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);

		// 비교할 불량
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// 같은 불량 항목 비교 금지
			if (i == j)
			{
				j++;
				continue;
			}

			// Point 불량 아닌것 제외
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP)
				//			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)
			{
				j++;
				continue;
			}
			// 

			// 불량 중심 좌표
			CPoint ptSrc;
			ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// 범위 안에 불량 존재?
			if (rectTemp.PtInRect(ptSrc))
			{

				// 블랙 패턴 명점과 S급 무라 동시 발견 시 중복 제거하지 않음
				int nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
				if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_BLACK && resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT) // s급 스팟일 경우 블랙패턴 명점과 같이 보고 pwj 20.08.25 (강한 스팟은 정상 판정)
				{
					j++;
					continue;
				}

				double meanRatio = resultPanelData.m_ListDefectInfo[j].Defect_MeanGV / (double)resultPanelData.m_ListDefectInfo[j].Defect_Size_Pixel;

				// 현재 리스트보다 낮은 리스트 삭제 시...
				if (i > j)	i--;

				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo.RemoveAt(j);

			}

			// 중복 안돼면... 다음 불량으로...
			else j++;
		}
	}

	return true;
}


//choikwangil Test
bool AviInspection::DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// B11
	// CKI 19.08.20
	// 백점 무라와 명점 간의 구분을 위해서 R,G,B,Black 에서 나온 명점으로 G64 백점 무라 제거
	//////////////////////////////////////////////////////////////////////////

	if (dAlignPara != NULL)
	{
		int		nDefectTypeM, nDefectTypeS;
		int		nAdjustRange = (int)dAlignPara[E_PARA_STRONG_DEFECT_NOISE_ADJUST_RANGE];
		CRect	rcDeleteArea;
		CPoint	ptCenterSub;

		int	nImgNum = 0;
		int	nImgNum1 = 0;

		//픽셀 불량일때 무라를 제거
		if (nAdjustRange > 0)
		{
			// 불량 갯수 만큼
			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				// Pattern 정보
				nImgNum = 0;
				nImgNum1 = 0;
				nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
				nImgNum1 = theApp.GetImageClassify(nImgNum);



				// Point 불량이 R , G , B 에 있는 경우만 찾음
				if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_R || nImgNum1 == E_IMAGE_CLASSIFY_AVI_G || nImgNum1 == E_IMAGE_CLASSIFY_AVI_B) // 04.09 choikwangil Black 패턴 제외
				{


					//Defect_Type
					nDefectTypeM = resultPanelData.m_ListDefectInfo[i].Defect_Type;

					if (		// 암점
						nDefectTypeM == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||		// 명점
						nDefectTypeM == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT ||		// 약명점
						nDefectTypeM == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT ||		// 그룹명점
						nDefectTypeM == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT ||		// 명점 재검
						nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_MULT_BP)
					{

						// P/S 모드 좌표 보정
						// 크기는 박스로 설정 - 누기가 원형도 있지만 길죽한 형태도 다수 존재
// 						rcDeleteArea.SetRect(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio,
// 							resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio,
// 							resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio,
// 							resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio);
// 
// 						// 좌표계 정규화
// 						rcDeleteArea.NormalizeRect();
// 
// 						// 범위 확장
// 						rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);

						// P/S 모드 좌표 보정
						ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio); //choikwangil 04.10 수정
						ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

						// 불량 갯수 만큼
						for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
						{
							// 같은 불량 항목 비교 금지
							if (i == j)
							{
								j++;
								continue;
							}

							nDefectTypeS = resultPanelData.m_ListDefectInfo[j].Defect_Type;

							if (nDefectTypeS == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT || nDefectTypeS == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT)	// 백점무라				//04.16 choi		
							{
								// P/S 모드 좌표 보정
// 								ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
// 								ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

								rcDeleteArea.SetRect(resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio,	//choikwangil 04.10 수정
									resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio,
									resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio,
									resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

								// 좌표계 정규화
								rcDeleteArea.NormalizeRect();

								// 범위 확장
								rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);

								// 범위 안에 들어오면 삭제
								if (rcDeleteArea.PtInRect(ptCenterSub))
								{
									// White Spot 불량 삭제
									resultPanelData.m_ListDefectInfo.RemoveAt(j);

									// 현재 리스트보다 낮은 리스트 삭제 시...
									if (i > j)	i--;
								}
								else
								{
									j++; // 지우지 않았을 경우만 다음불량으로
									continue;
								}
							}
							else j++; // 포인트 불량이 아닐 경우 다음불량으로
						}
					}
				}
			}
		}
	}


	return true;
}
//choikwangil Test 임시 주석
// int AviInspection::Judge_Defect_BP_WSpot(ResultPanelData& resultPanelData, int index ,int Defect_Judge,double* dAlignPara)
// {
// 	// P/S 모드에 따른 좌표 비교
// 	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
// 
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// B11
// 	// CKI 19.08.20
// 	// G64 패턴에서 명점과 백점무라를 MaxGV/MeanGV 이용해 재판정
// 	//////////////////////////////////////////////////////////////////////////
// 	if ((int)dAlignPara[E_PARA_WHITESPOT_JUDGE_MAXRATIO_ONOFF] == 0) {
// 		return Defect_Judge;
// 	}
// 
// 	if (dAlignPara != NULL)
// 	{
// 		int		nDefectTypeM;
// 		int		nAdjustRange = (int)dAlignPara[E_PARA_STRONG_DEFECT_NOISE_ADJUST_RANGE]; //추후 수정해야함 MaxGV_Judge 변수 param화
// 		CRect	rcDeleteArea;
// 		CPoint	ptCenterSub;
// 
// 		int	nImgNum = 0;
// 		int	nImgNum1 = 0;
// 
// 		double MaxGV_Ratio = 0.0;
// 		double MaxGV_Judge = (int)dAlignPara[E_PARA_WHITESPOT_JUDGE_MAXRATIO_VALUE]; //추후 수정해야함 MaxGV_Judge 변수 param화
// 
// 		if (MaxGV_Judge > 0)
// 		{
// 			// 불량 갯수 만큼
// 
// 				// Pattern 정보
// 			nImgNum = 0;
// 			nImgNum1 = 0;
// 			MaxGV_Ratio = 0.0;
// 
// 			nImgNum = resultPanelData.m_ListDefectInfo[index].Img_Number;
// 			nImgNum1 = theApp.GetImageClassify(nImgNum);
// 
// 			//Defect_Type
// 			nDefectTypeM = resultPanelData.m_ListDefectInfo[index].Defect_Type;
// 
// 		
// 				MaxGV_Ratio = resultPanelData.m_ListDefectInfo[index].Defect_MaxGV / resultPanelData.m_ListDefectInfo[index].Defect_MeanGV;
// 
// 				if (MaxGV_Ratio <= MaxGV_Judge) {
// 					return E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
// 				}
// 				else {
// 					return Defect_Judge;
// 				}
// 			
// 		}
// 	}
// 
// 	return Defect_Judge;
// }


bool AviInspection::DeleteOverlapDefect_Line_Mura(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOffSet = 4;

	// 라인 무라 삭제위한 변수 선언 ( 범위 확장 )
	int nAdjustRange = 15;

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// 대상 불량을 찾는다.
		if ((resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_LINE_X_BRIGHT			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_LINE_Y_EDGE_BRIGHT) ||
			(resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_RETEST_LINE_BRIGHT		&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_RETEST_LINE_DARK) ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
		{
			CRect rectLine = CRect(
				(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
				(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
				(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
				(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);

			// 좌표계 정규화
			rectLine.NormalizeRect();

			rectLine.InflateRect(nAdjustRange, nAdjustRange);

			// 비교할 불량
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// 같은 불량 항목 비교 금지
				if (i == j)
				{
					j++;
					continue;
				}

				// 해당 불량이 포인트 불량중 하나인지 확인
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT || //04.16 choi

					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EMD_BRIGHT ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EMD_DARK ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_UP_CM ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_CORNER_CM ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_FINGER_CM ||
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_NUGI			||	// KSW 18.05.21 - 고객사 요청 : 누기 & 라인 불량 따로 보고
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BN_CORNER ||
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_STAMPED		||	// KSW 18.05.21 - 고객사 요청 : 누기 & 라인 불량 따로 보고
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI		||	// KSW 18.05.21 - 고객사 요청 : 누기 & 라인 불량 따로 보고
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_		||	// KSW 18.05.21 - 고객사 요청 : 누기 & 라인 불량 따로 보고
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT	||  // choi 06.04
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_DARK ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_MURA)
				{
					CRect rectMura = CRect(
						(resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio) - nOffSet,
						(resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio) - nOffSet,
						(resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio) + nOffSet,
						(resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio) + nOffSet);

					CRect rectRes;
					IntersectRect(rectRes, rectLine, rectMura);

					if ((rectRes.Width() + rectRes.Height()) > 0) // 범위 안에 들어오면 삭제
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(j);

						// 리스트 작은거 삭제시..
						if (i > j)	 i--;
					}
					else
					{
						j++; // 지우지 않았을 경우만 다음불량으로
						continue;
					}
				}
				// 중복 안돼면... 다음 불량으로...
				else j++;
			}
		}
	}

	return true;
}

bool AviInspection::DeleteOverlapDefect_Black_Mura_and_Judge(ResultPanelData& resultPanelData, double* dAlignPara) //choikwangil 04.07 함수 따로 뺌
{
	// P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//pwj
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 불량 갯수 만큼

	int	nImgNum = 0;

	int nImgNum1 = 0;


	int		nAdjustRange = (int)dAlignPara[E_PARA_STRONG_DEFECT_NOISE_ADJUST_RANGE];

	CRect	rcDeleteArea;
	CPoint	ptCenterSub;

	int nOffSet = 20; // choi 05.07

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Mura 불량 아닌것 제외 ( 멀티BP 제외 )
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT);
		else continue;

		CRect rectM = CRect(
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);

		// 비교할 불량
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// 같은 불량 항목 비교 금지
			if (i == j)
			{
				j++;
				continue;
			}

			if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT || resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT) { //04.16 choi

				CRect rectS = CRect(
					(resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio),
					(resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio),
					(resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio),
					(resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio));

				// 중복 영역 확인
				CRect rectRes;
				IntersectRect(rectRes, rectM, rectS);

				// 범위 안에 들어오면 이름 임시 변경
				if ((rectRes.Width() + rectRes.Height()) > 0)
				{

					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_CM;



					// 현재 리스트보다 낮은 리스트 삭제 시...
					if (i > j)	i--;

					// 해당 불량 삭제
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
				}
				// 중복 안돼면... 다음 불량으로...
				else
				{
					j++;
					continue;
				}
			}


			///////////////////////////////////////////////////////////////////////////
			//Point
			///////////////////////////////////////////////////////////////////////////

// 			else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT ||
// 				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
// 				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT ||
// 				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT) {
// 				nImgNum = 0;
// 				nImgNum1 = 0;
// 				nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
// 				nImgNum1 = theApp.GetImageClassify(nImgNum);
// 				if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_64) {
// 					j++;
// 					continue;
// 				}
// 				
// 				else {
// 				
// 					// P/S 모드 좌표 보정
// 					// 크기는 박스로 설정 - 누기가 원형도 있지만 길죽한 형태도 다수 존재
// 					rcDeleteArea.SetRect(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio,
// 						resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio,
// 						resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio,
// 						resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio);
// 
// 					// 좌표계 정규화
// 					rcDeleteArea.NormalizeRect();
// 
// 					// 범위 확장
// 					rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);
// 
// 					ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
// 					ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
// 
// 					// 범위 안에 들어오면 삭제
// 					if (rcDeleteArea.PtInRect(ptCenterSub))
// 					{
// 
// 						resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_CM;
// 
// 
// 
// 						// 현재 리스트보다 낮은 리스트 삭제 시...
// 						if (i > j)	i--;
// 
// 						// 해당 불량 삭제
// 						resultPanelData.m_ListDefectInfo.RemoveAt(j);
// 					}
// 					// 중복 안돼면... 다음 불량으로...
// 					else
// 					{
// 						j++;
// 						continue;
// 					}
// 				}
// 			}
			else
			{
				j++;
				continue;
			}

		}
	}

	// black mura 와 백점 무라가 겹치지 않는 것들은 모두 제거
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT)
		{
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
			i--;
		}
	}


	// 임시로 변경했던 이름을 다시 원래대로 복구
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_CM)
		{
			resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT;
		}
	}

	return true;

}

bool AviInspection::DeleteOverlapDefect_Mura_Mura(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// 중복되는 Mura 불량 제거
	//////////////////////////////////////////////////////////////////////////

	int nOffSet = 0;




	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Mura 불량 아닌것 제외 ( 멀티BP 제외 )
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_MURA_START ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_MURA_END ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_X ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_X ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_Y ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH) //choi 06.04
			continue;

		CRect rectM = CRect(
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);

		// 비교할 불량
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// 같은 불량 항목 비교 금지
			if (i == j)
			{
				j++;
				continue;
			}

			// Mura 불량 아닌것 제외 ( 멀티BP 제외 )
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type <= E_DEFECT_JUDGEMENT_MURA_START ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_MURA_END ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_X ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_X ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_Y ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH) //choi 06.04
			{
				j++;
				continue;
			}

			//choikwangil 12.03 black hole
			if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT&&resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT) ||
				(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT&&resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT) ||
				(resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT&&resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT) || //04.16 choi
				(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT&&resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT) ||
				(resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT&&resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT) ||
				(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT&&resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT) ||
				(resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT&&resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT) || //05.26 choi
				(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT&&resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)
				)
			{
				j++;
				continue;
			}

			/////////////////////////////////////
// 
// 			//choikwangil 12.03 black hole
// 			if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT&&resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT) ||
// 				(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT&&resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT) 
// 				)
// 			{
// 				j++;
// 				continue;
// 			}
			/////////////////////////////////////

			CRect rectS = CRect(
				(resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio),
				(resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio),
				(resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio),
				(resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio));

			// 중복 영역 확인
			CRect rectRes;
			IntersectRect(rectRes, rectM, rectS);

			// 범위 안에 들어오면 삭제
			if ((rectRes.Width() + rectRes.Height()) > 0)
			{
				// 백점 & 누기 & 하부찍힘 우선순위로 남기기
				if (//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT || //choi 06.04
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||	// 백점
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT ||	// 04.16 choi
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_NUGI ||	// 누기
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI ||	// 엣지 누기
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_ ||	// 엣지 누기
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_STAMPED)	// 하부 찍힘
				{



					ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
					ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
					resultPanelData.m_ListDefectInfo[i] = tTemp2;
					resultPanelData.m_ListDefectInfo[j] = tTemp1;


					// 범위 재 수정
					rectM = CRect(
						(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
						(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
						(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
						(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);
				}


				// 현재 리스트보다 낮은 리스트 삭제 시...
				if (i > j)	i--;

				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// 중복 안돼면... 다음 불량으로...
			else j++;
		}
	}

	//pwj
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 불량 갯수 만큼
	/*for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Mura 불량 아닌것 제외 ( 멀티BP 제외 )
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_MURA_START ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_MURA_END ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_X ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_X ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_Y)
			continue;

		CRect rectM = CRect(
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);

		// 비교할 불량
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// 같은 불량 항목 비교 금지
			if (i == j)
			{
				j++;
				continue;
			}

			// Mura 불량 아닌것 제외 ( 멀티BP 제외 )
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type <= E_DEFECT_JUDGEMENT_MURA_START ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_MURA_END ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_X ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_X ||
				resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_Y)
			{
				j++;
				continue;
			}

			//choikwangil 12.03 black hole
			// 			if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT&&resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT) ||
			// 				(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT&&resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT))
			// 			{
			// 				j++;
			// 				continue;
			// 			}
			/////////////////////////////////////

			CRect rectS = CRect(
				(resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio),
				(resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio),
				(resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio),
				(resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio));

			// 중복 영역 확인
			CRect rectRes;
			IntersectRect(rectRes, rectM, rectS);

			// 범위 안에 들어오면 삭제
			if ((rectRes.Width() + rectRes.Height()) > 0)
			{
				// 백점 & 누기 & 하부찍힘 우선순위로 남기기
				// 				if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT ||
				// 					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||	// 백점
				// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_NUGI ||	// 누기
				// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI ||	// 엣지 누기
				// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_ ||	// 엣지 누기
				// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_STAMPED)	// 하부 찍힘
				// 				{

				if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT && resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)
				{
					resultPanelData.m_ListDefectInfo[j].Defect_Type = E_DEFECT_JUDGEMENT_MURA_CM; // 진짜 불량을 임시로 다른 이름으로 변경



					ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
					ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
					resultPanelData.m_ListDefectInfo[i] = tTemp2;
					resultPanelData.m_ListDefectInfo[j] = tTemp1;


					// 범위 재 수정
					rectM = CRect(
						(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
						(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
						(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
						(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);
				}
				//}

				if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT && resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_CM;



				// 현재 리스트보다 낮은 리스트 삭제 시...
				if (i > j)	i--;

				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// 중복 안돼면... 다음 불량으로...
			else j++;
		}
	}

	// black mura 와 백점 무라가 겹치지 않는 것들은 모두 제거
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT)
		{
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
			i--;
		}
	}


	// 임시로 변경했던 이름을 다시 원래대로 복구
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_CM)
		{
			resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT;
		}
	}*/

	return true;
}


// Casting - stDefectInfo에서 필요한 부분 추출하여 ResultPanelData에 적재
bool AviInspection::GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio)
{
	// AVI는 회전하지 않은 원본 영상 좌표를 사용하므로 상위 보고 시 워크좌표 계산 등을 위해 좌표를 회전해서 넘겨줘야 함
	_tcscpy_s(pResultDefectInfo->Defect_Code, _T("CODE"));
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_TOP] = cv::Point(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_TOP] = cv::Point(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM] = cv::Point(pResultBlob->ptRB[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_BOTTOM] = cv::Point(pResultBlob->ptLB[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].y);

	// choi 결과 각도
// 	vector<Point> ptActiveContour;
// 
// 	for (int i = 0; i < 4; i++) {
// 		cv::Point ptTemp(pResultDefectInfo->Defect_Rect_Point[i].x, pResultDefectInfo->Defect_Rect_Point[i].y);
// 		Align_DoRotatePoint(ptTemp, pResultDefectInfo->Defect_Rect_Point[i], m_stThrdAlignInfo.ptAlignCenter, m_stThrdAlignInfo.dAlignTheta);
// 
// 		ptActiveContour.push_back(pResultDefectInfo->Defect_Rect_Point[i]);
// 	}
// 
// 	cv::Rect rtDefect = boundingRect(ptActiveContour);
// 
// 	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_TOP].x = rtDefect.x;
// 	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_TOP].y = rtDefect.y;
// 
// 	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_TOP].x = rtDefect.x + rtDefect.width;
// 	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_TOP].y = rtDefect.y;
// 
// 	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM].x = rtDefect.x + rtDefect.width;
// 	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM].y = rtDefect.y + rtDefect.height;
// 
// 	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_BOTTOM].x = rtDefect.x;
// 	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_BOTTOM].y = rtDefect.y + rtDefect.height;

	cv::Point ptRotate(0, 0);

	//choi 결과 각도
	// AD 아닌 불량들...
// 	if (pResultBlob->nDefectJudge[nBlobCnt] > E_DEFECT_JUDGEMENT_DUST_GROUP)
// 	{
// 		cv::Point ptTemp(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLT[nBlobCnt].y);
// 		Align_DoRotatePoint(ptTemp, ptRotate, m_stThrdAlignInfo.ptAlignCenter, m_stThrdAlignInfo.dAlignTheta);
// 		pResultDefectInfo->Pixel_Start_X		= ptRotate.x;
// 		pResultDefectInfo->Pixel_Start_Y		= ptRotate.y;
// 
// 		ptTemp.x = pResultBlob->ptRB[nBlobCnt].x;
// 		ptTemp.y = pResultBlob->ptRB[nBlobCnt].y;
// 		Align_DoRotatePoint(ptTemp, ptRotate, m_stThrdAlignInfo.ptAlignCenter, m_stThrdAlignInfo.dAlignTheta);
// 		pResultDefectInfo->Pixel_End_X			= ptRotate.x;
// 		pResultDefectInfo->Pixel_End_Y			= ptRotate.y;
// 	
// 		
// 		
// 	}
// 	// AD 불량일 경우 회전하지 않고 전체 좌표 그대로 반환
// 	else
// 	{

	pResultDefectInfo->Pixel_Start_X = pResultBlob->ptLT[nBlobCnt].x;
	pResultDefectInfo->Pixel_Start_Y = pResultBlob->ptLT[nBlobCnt].y;
	pResultDefectInfo->Pixel_End_X = pResultBlob->ptRB[nBlobCnt].x;
	pResultDefectInfo->Pixel_End_Y = pResultBlob->ptRB[nBlobCnt].y;
	//}

	pResultDefectInfo->Pixel_Center_X = (int)(pResultDefectInfo->Pixel_Start_X + pResultDefectInfo->Pixel_End_X) / 2;
	pResultDefectInfo->Pixel_Center_Y = (int)(pResultDefectInfo->Pixel_Start_Y + pResultDefectInfo->Pixel_End_Y) / 2;

	// 기울어진 원본 이미지 에서 Crop 할 불량 영역 지정
	pResultDefectInfo->Pixel_Crop_Start_X = min(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].x);
	pResultDefectInfo->Pixel_Crop_Start_Y = min(pResultBlob->ptLT[nBlobCnt].y, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Pixel_Crop_End_X = max(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].x);
	pResultDefectInfo->Pixel_Crop_End_Y = max(pResultBlob->ptLB[nBlobCnt].y, pResultBlob->ptRB[nBlobCnt].y);

	//choi 결과 각도
// 	pResultDefectInfo->Pixel_Crop_Start_X = pResultDefectInfo->Pixel_Start_X;
// 	pResultDefectInfo->Pixel_Crop_Start_Y = pResultDefectInfo->Pixel_Start_Y;
// 	pResultDefectInfo->Pixel_Crop_End_X = pResultDefectInfo->Pixel_End_X;
// 	pResultDefectInfo->Pixel_Crop_End_Y = pResultDefectInfo->Pixel_End_Y;

	// BOE Eng'r 요청 Defect Size
	//pResultDefectInfo->Defect_Size			= (int)(pResultBlob->nArea[nBlobCnt] * WrtResultInfo.GetCamResolution(nCameraNum));
	pResultDefectInfo->Defect_Size = (int)WrtResultInfo.CalcDistancePixelToUm(sqrt(pow((double)(pResultDefectInfo->Pixel_End_X - pResultDefectInfo->Pixel_Start_X), 2) +
		pow((double)(pResultDefectInfo->Pixel_End_Y - pResultDefectInfo->Pixel_Start_Y), 2)),
		nCameraNum, nRatio);
	pResultDefectInfo->Defect_BKGV = pResultBlob->dBackGroundGV[nBlobCnt];
	pResultDefectInfo->Defect_MeanGV = pResultBlob->dMeanGV[nBlobCnt];
	pResultDefectInfo->Defect_MinGV = pResultBlob->nMinGV[nBlobCnt];
	pResultDefectInfo->Defect_MaxGV = pResultBlob->nMaxGV[nBlobCnt];
	pResultDefectInfo->Defect_Size_Pixel = (int)pResultBlob->nArea[nBlobCnt];
	pResultDefectInfo->Img_Number = nImageNum;
	pResultDefectInfo->Img_Size_X = (DOUBLE)(pResultDefectInfo->Pixel_Crop_End_X - pResultDefectInfo->Pixel_Crop_Start_X);		// 임시 불량 영상 가로 크기
	pResultDefectInfo->Img_Size_Y = (DOUBLE)(pResultDefectInfo->Pixel_Crop_End_Y - pResultDefectInfo->Pixel_Crop_Start_Y);		// 임시 불량 영상 세로 크기
	pResultDefectInfo->Defect_Type = pResultBlob->nDefectJudge[nBlobCnt];
	pResultDefectInfo->Pattern_Type = pResultBlob->nPatternClassify[nBlobCnt];
	pResultDefectInfo->Camera_No = nCameraNum;
	pResultDefectInfo->nRatio = nRatio;
	pResultDefectInfo->strColor = pResultBlob->strColor[nBlobCnt]; //220216 choi
	// KSW 17.12.04 - (장축 + 단축) / 2 -> Spec 추가 ( 고객사 요청 )
	pResultDefectInfo->dDimension = (pResultBlob->dF_Max[nBlobCnt] + pResultBlob->dF_Min[nBlobCnt]) / 2.0;

	////////////////////////////////////////////////////////////////////////// 0607 choi 판정 위치 변경으로 인한 Data 추가
	pResultDefectInfo->d_L = pResultBlob->Lab_avg_L[nBlobCnt];
	pResultDefectInfo->d_Degree = pResultBlob->Lab_Degree[nBlobCnt];
	pResultDefectInfo->d_Lenght = pResultBlob->Lab_Lenght[nBlobCnt];
	//////////////////////////////////////////////////////////////////////////
	
	// 2021.12.08~ MDJ LCP 엑셀에 값을 넣기 위한 변수 추가
	//std::string strTemp = pResultBlob->strColor[nBlobCnt];
	//CString cstrTemp;
	//cstrTemp.Format(_T("%s"), strTemp);
	//_tcscpy_s(pResultDefectInfo->Color_Code, cstrTemp);

	std::string strTemp = pResultBlob->strColor[nBlobCnt];

	const char* all = strTemp.c_str();
	int len = 1 + strlen(all);
	wchar_t* t = new wchar_t[len];
	if (NULL == t) throw std::bad_alloc();
	mbstowcs(t, all, len);

	_tcscpy_s(pResultDefectInfo->Color_Code, t);


#if USE_ALG_CONTOURS
	// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
	memcpy(pResultDefectInfo->nContoursX, pResultBlob->nContoursX[nBlobCnt], sizeof(int) * MAX_CONTOURS);
	memcpy(pResultDefectInfo->nContoursY, pResultBlob->nContoursY[nBlobCnt], sizeof(int) * MAX_CONTOURS);
#endif

	return true;
}

bool AviInspection::AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner)
{
	// 회전한 경우 Left-Top 기준점 좌표 보정
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP], ptAdjCorner[E_CORNER_LEFT_TOP], pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP], ptAdjCorner[E_CORNER_RIGHT_TOP], pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM], ptAdjCorner[E_CORNER_RIGHT_BOTTOM], pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM], ptAdjCorner[E_CORNER_LEFT_BOTTOM], pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Align 하기 전, AVI & SVI 점등 이상 확인
//////////////////////////////////////////////////////////////////////////
long AviInspection::CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio)
{
	// test
	CCPUTimer tact;
	tact.Start();

	// 개별 알고리즘 검사 파라미터 가져오기
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// 장비 타입
	int nEqpType = theApp.m_Config.GetEqpType();

	// 정상 밝기와 틀리거나, 점등이 안된경우 AD 불량이므로 검사 안해도 됨. ( Align 하지 못함 )
	long nErrorCode = Align_FindDefectAD(MatOrgImage, dAlgPara, dResult, nRatio, nCameraNum, nEqpType);
	//long nErrorCode = E_ERROR_CODE_TRUE;

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Inspect AD : %.2f"), tact.Stop(false) / 1000.);

	return nErrorCode;
}

// Repair 설비에서 사용하는 좌표값 및 코드 판정
bool AviInspection::JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo)
{
	//Line이 외각에 붙어있는지 확인용
	//Cell Corner 받아오기 
	//[0] = LeftTop
	//[1] = RightTop			안씀 주석처리
	//[2] = RightBottom
	//[3] = LeftBottom			안씀 주석처리

	int		nWorkOriginPosition = -1;			// 0 : LT, 1 : RT, 2 : RB, 3 : LB
	int		nWorkDirection = -1;				// 0 : X = Width, 1 : Y = Width

	WrtResultInfo.GetWorkCoordUsingRepair(nWorkOriginPosition, nWorkDirection);

	cv::Point ptCornerTemp[4];  // Cell Corner 저장용 Align 전
	cv::Point ptRotate(0, 0);   // Align 결과 받을 변수
	cv::Point ptAlignCorner[4]; // Align 후 Cell Corner


	ptAlignCorner[0].x = m_stThrdAlignInfo.rcAlignCellROI.x;
	ptAlignCorner[0].y = m_stThrdAlignInfo.rcAlignCellROI.y;

	ptAlignCorner[2].x = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
	ptAlignCorner[2].y = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

	int nDefectCount = (int)resultPanelData.m_ListDefectInfo.GetCount();

	// 오차 보정용
	int nRepairOffSet = 20;

	// 라인 결함이 실제 결함보다 크게나옴
	// 알고리즘 수정 필요
	// 임시로 offset
	int nDefectRectOffSet = 6;

	// 돌려보자 for문 defect  순서대로
	for (int i = 0; i < nDefectCount; i++)
	{
		int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_POINT_DARK			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK)	// POINT 인 경우
		{
			resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
			resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
			resultPanelData.m_ListDefectInfo[i].bUseReport = true;
		}

		// Line Defect repair 조건 확인용
		// Line Defect인 경우
		else if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_LINE_X_BRIGHT			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
		{
			int nStart_X = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
			int nStart_Y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
			int nEnd_X = resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
			int nEnd_Y = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;

			// Cell 크기보다 크게 나와 바깥에 있는 경우 전부 최대 최소값 제한
			if (nStart_X < ptAlignCorner[0].x)
			{
				resultPanelData.m_ListDefectInfo[i].Pixel_Start_X = ptAlignCorner[0].x;
			}
			if (nStart_Y < ptAlignCorner[0].y)
			{
				resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y = ptAlignCorner[0].y;
			}
			if (nEnd_X > ptAlignCorner[2].x)
			{
				resultPanelData.m_ListDefectInfo[i].Pixel_End_X = ptAlignCorner[2].x;
			}
			if (nEnd_Y > ptAlignCorner[2].y)
			{
				resultPanelData.m_ListDefectInfo[i].Pixel_End_Y = ptAlignCorner[2].y;
			}

			// DGS와 Line Point 아닌 경우
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS		&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_XLINE_SPOT	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_YLINE_SPOT)
			{
				// X Line Defect인 경우		
				if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN)
				{
					// 한쪽만 붙어 있는경우 DO, L-GO, R-GO
					// X Line 이면서 한쪽이라도 Cell 끝쪽에 붙어있는 경우
					if (ptAlignCorner[0].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_X && resultPanelData.m_ListDefectInfo[i].Pixel_Start_X <= ptAlignCorner[0].x + nRepairOffSet ||
						ptAlignCorner[2].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_X   && resultPanelData.m_ListDefectInfo[i].Pixel_End_X <= ptAlignCorner[2].x + nRepairOffSet)
					{
						// X-Line, Y-Line
						// X Line 이면서 양쪽 다 Cell 끝쪽에 붙어있는 경우
						if (ptAlignCorner[0].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_X && resultPanelData.m_ListDefectInfo[i].Pixel_Start_X <= (ptAlignCorner[0].x + nRepairOffSet) &&
							ptAlignCorner[2].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_X && resultPanelData.m_ListDefectInfo[i].Pixel_End_X <= ptAlignCorner[2].x + nRepairOffSet)
						{
							// Work 원점이 바뀌면 보고해야하는 위치가 달라짐
							// 
							switch (nWorkOriginPosition)
							{
							case E_CORNER_LEFT_TOP:
							case E_CORNER_LEFT_BOTTOM:
								// Defect 왼쪽 좌표 입력
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = ptAlignCorner[0].x; // 라인 처음 x 좌표
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
								break;
							case E_CORNER_RIGHT_TOP:
							case E_CORNER_RIGHT_BOTTOM:
								// Defect 오른쪽 좌표 입력
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = ptAlignCorner[2].x; // 라인 끝 x 좌표
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
								break;
							}
						}
						else
						{
							// DO, L-GO
							// X Line 이면서 Line이 Cell 왼쪽에 붙어있는 경우
							// Left Corner X 좌표에서 OffSet 안쪽으로 있을 때
							if (ptAlignCorner[0].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_X			&&
								resultPanelData.m_ListDefectInfo[i].Pixel_Start_X <= ptAlignCorner[0].x + nRepairOffSet)
							{
								// Defect 오른쪽 좌표 입력
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = resultPanelData.m_ListDefectInfo[i].Pixel_End_X - nDefectRectOffSet; // 라인 끝 x 좌표
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0; // y 중간 좌표
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
							}
							// DO, R-GO
							// X Line 이면서 Line이 Cell 오른쪽에 붙어있는 경우
							// Right Corner X 좌표에서 OffSet 안쪽으로 있을 때
							if (ptAlignCorner[2].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_X	 &&
								resultPanelData.m_ListDefectInfo[i].Pixel_End_X <= ptAlignCorner[2].x + nRepairOffSet)
							{
								// Defect 왼쪽 좌표 입력
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + nDefectRectOffSet; // 라인 시작 좌표
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0; // y 중간 좌표
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
							}
						}
					}
					// Cell 끝쪽에 어느 곳도 붙어있지 않는경우 Defect 중심 좌표
					else
					{
						resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
						resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
						resultPanelData.m_ListDefectInfo[i].bUseReport = true;

					}
				}
				//Y Line인 경우
				else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT)
				{
					// 한쪽만 붙어 있는경우 DO, L-GO, R-GO
					// Y Line 이면서 한쪽이라도 Cell 끝쪽에 붙어있는 경우
					if (ptAlignCorner[0].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y && resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y <= ptAlignCorner[0].y + nRepairOffSet ||
						ptAlignCorner[2].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_Y   && resultPanelData.m_ListDefectInfo[i].Pixel_End_Y <= ptAlignCorner[2].y + nRepairOffSet)
					{
						// Y-Line 
						// Y Line 이면서 양쪽 다 Cell 끝쪽에 붙어있는 경우
						if (ptAlignCorner[0].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y && resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y <= ptAlignCorner[0].y + nRepairOffSet &&
							ptAlignCorner[2].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_Y  && resultPanelData.m_ListDefectInfo[i].Pixel_End_Y <= ptAlignCorner[2].y + nRepairOffSet)
						{
							// Work 원점이 바뀌면 보고해야하는 위치가 달라짐
							// 
							switch (nWorkOriginPosition)
							{
							case E_CORNER_LEFT_TOP:
							case E_CORNER_RIGHT_TOP:
								// Defect 위쪽 좌표 입력
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = ptAlignCorner[0].y; // 라인 끝 y 좌표
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
								break;
							case E_CORNER_LEFT_BOTTOM:
							case E_CORNER_RIGHT_BOTTOM:
								// Defect 아래쪽 좌표 입력
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;;
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = ptAlignCorner[2].y; // 라인 끝 y 좌표
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
								break;
							}
						}
						else
						{
							// DO, L-GO
							// Y Line 이면서 Line이 Cell 위쪽에 붙어있는 경우
							if (ptAlignCorner[0].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y			&&
								resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y <= ptAlignCorner[0].y + nRepairOffSet)
							{
								// Defect 아래쪽 좌표 입력
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0; // x 중간 좌표
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - nDefectRectOffSet; // 라인 끝 y 좌표
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
							}
							// DO, R-GO
							// Y Line 이면서 Line이 Cell 아래쪽에 붙어있는 경우
							if (ptAlignCorner[2].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_Y	 &&
								resultPanelData.m_ListDefectInfo[i].Pixel_End_Y <= ptAlignCorner[2].y + nRepairOffSet)
							{
								// Cell 위쪽 좌표 입력
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0; // x 중간 좌표
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + nDefectRectOffSet; // 라인 시작 Y 좌표
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
							}
						}
					}
					else// Line이 외각쪽에 라인 끝선이 붙어있지 않는 경우
					{
						resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
						resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
						resultPanelData.m_ListDefectInfo[i].bUseReport = true;
					}
				}
				else //Line이 아닌 경우 // 들어올 수 있는 경우가 없긴함.
				{
					resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
					resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
					resultPanelData.m_ListDefectInfo[i].bUseReport = true;
				}
			}
			// LinePoint
			else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_XLINE_SPOT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_YLINE_SPOT)
			{
				resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
				resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
				resultPanelData.m_ListDefectInfo[i].bUseReport = true;
			}
			// DGS인 경우
			else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS)
			{
				resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
				resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
				resultPanelData.m_ListDefectInfo[i].bUseReport = true;
			}

			// Sub 판정
			else
			{
				resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
				resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
				resultPanelData.m_ListDefectInfo[i].bUseReport = true;
			}
		}// Line Defect Repair 끝

		// MURA 및 기타 불량
		else
		{
			resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
			resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
			resultPanelData.m_ListDefectInfo[i].bUseReport = true;
		}
	}
	return true;
}

bool AviInspection::JudgementSpot(ResultPanelData& resultPanelData)
{
	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// 주변 비교
	int nOffSet = 4;

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.03.30 - G64에서 백점 판정한 결과 재확인하여 판정 적용
	//////////////////////////////////////////////////////////////////////////

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// 명점 & 백점 불량인 경우만...
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT		&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT		&& //04.16 choi
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT)
			continue;

		// G87 패턴인 경우만... 백점 검사
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_87)
			continue;

		// 불량 중심 좌표
		CPoint ptSrc1;
		ptSrc1.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio);
		ptSrc1.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

		// 비교할 불량
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// 같은 불량 항목 비교 금지
			if (i == j)
			{
				j++;
				continue;
			}

			// G64 패턴인 경우만...
			nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
			if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_64)
			{
				j++;
				continue;
			}

			// Mura 검사 불량인 경우만...
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG)
			{
				j++;
				continue;
			}

			// 불량 중심 좌표
			CPoint ptSrc2;
			ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// 불량 중심점이 같다면...
			if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&
				abs(ptSrc1.y - ptSrc2.y) < nOffSet)
			{
				// 백점인 경우, 백점으로
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL)
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				// 명점인 경우 제거
				else
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL;

				break;
			}
			// 중복 안돼면... 다음 불량으로...
			else j++;
		}
	}

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		// Mura 검사 불량인 경우만...
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL		&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG)
		{
			i++;
			continue;
		}

		// 해당 불량 삭제
		resultPanelData.m_ListDefectInfo.RemoveAt(i);
	}

	return true;
}

bool AviInspection::JudgementDelReport(ResultPanelData& resultPanelData)
{
	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		if (resultPanelData.m_ListDefectInfo[i].bUseReport)
		{
			i++;
			continue;
		}

		resultPanelData.m_ListDefectInfo.RemoveAt(i);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//제	 목: 동일 Pattern 내 Line 주변 Line Defect 제거												//  
//작업일: 2017-09-18																				// 
//작업자: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementDeletLineDefect(ResultPanelData& resultPanelData, double* dAlignPara)
{
	int nLineNumber = 0;	// 진짜 라인 불량
	int nLineDefect = 0;	// 약한 라인
	int nOffSet = 30;	// 중심점 거리 차이
	int nWidthofLine = 45;	// 약라인중에서 해당 굵기보다 높으면 삭제

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// 진짜 Line성 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
			nLineNumber++;	// 진짜 라인
			break;

			// Line 불량인 경우 변경
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
			nLineDefect++;	// 약라인
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 강라인 주변 약라인 삭제
	//////////////////////////////////////////////////////////////////////////

	// Delete the line defect around the Strong Line
	// 둘다 모두 존재하는 경우
	if (nLineNumber > 0 && nLineDefect > 0)
	{
		//////////////////////////////////////////////////////////////////////////
		// x 방향
		//////////////////////////////////////////////////////////////////////////

		// 불량 개수
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 ( 강라인 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인만... )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
					{
						j++; continue;
					}


					CPoint	X2St, X2End, X2Center;
					int		X2DefectImageType;

					X2St.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2End.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					// 같은 패턴 아니면 다른거
					if (X1DefectImageType != X2DefectImageType) { j++; continue; }

					// 강라인 주변에 약라인 있는 경우
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// 리스트 작은거 삭제시..
						if (i > j)	 i--;

						// 약라인 삭제
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
					// 강라인 주변에 약라인 없는 경우
					else  j++;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// y 방향
		//////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Y 방향 ( 강라인 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT)
			{
				CPoint Y1St, Y1End, Y1Center;
				int Y1DefectImageType;
				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;

				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// Y 방향 불량 아닌것 제외 ( 약라인만... )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
					{
						j++; continue;
					}


					CPoint	Y2St, Y2End, Y2Center;
					int		Y2DefectImageType;
					Y2St.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2Center.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;

					// 같은 패턴 아니면 다른거
					if (Y1DefectImageType != Y2DefectImageType) { j++; continue; }

					// 강라인 주변에 약라인 있는 경우
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// 리스트 작은거 삭제시..
						if (i > j) i--;

						// 약라인 삭제
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
					// 강라인 주변에 약라인 없는 경우
					else  j++;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// B라인 주변 D라인 양쪽 삭제 기능 ( x방향 )
	// D라인 주변 B라인 양쪽 삭제 기능 ( x방향 )
	//////////////////////////////////////////////////////////////////////////

	// y방향 구현 필요

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// X 방향
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
		{
			CPoint	X1St, X1End, X1Center;
			int		X1DefectImageType;
			X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
			X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
			X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
			X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// 같은 불량 비교 방지
				if (i == j)
				{
					j++; continue;
				}

				// X 방향 아닌것
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_BRIGHT  &&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK)
				{
					j++; continue;
				}


				if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT &&		// i : 밝은것
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK) ||		// j : 어두운것

					(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT &&		// j : 밝은것
						resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK))		// i : 어두운것
				{
					CPoint	X2St, X2End, X2Center;
					int		X2DefectImageType;
					X2St.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2End.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;

					// 같은 패턴 아니면 다른거
					if (X1DefectImageType != X2DefectImageType) { j++; continue; }

					if ((abs(X2St.y - X2End.y) >= nWidthofLine) &&		// 선 두께까 설정보다 크고
						(abs(X1Center.y - X2Center.y) >= nOffSet))		// 중심 좌표가 비슷한경우
					{
						// 리스트 작은거 삭제시..
						if (i > j) i--;

						// 라인 삭제
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
					else j++;
				}
				else j++;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 강라인 & 약라인 재카운트
	//////////////////////////////////////////////////////////////////////////

	// 이전 데이터 초기화
	nLineNumber = 0;
	nLineDefect = 0;

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// 진짜 Line성 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
			nLineNumber++;
			break;

			// Line 불량인 경우 변경
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
		{
			// Norch 사용 한 지 확인
			int NORCH_ONOFF = (int)(dAlignPara[E_PARA_NORCH_OVERKILL_ONOFF]);

			if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT) &&
				NORCH_ONOFF == 1)
			{
				// 설정 된 Norch 번위 값을 받아 오기
				int NORCH_UNIT = (int)(dAlignPara[E_PARA_NORCH_OVERKILL_UNIT_X]);

				if (NORCH_UNIT == NULL) NORCH_UNIT = 400;

				if (abs(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X - m_stThrdAlignInfo.rcAlignCellROI.x) < NORCH_UNIT) continue;
				else nLineDefect++;
			}

			else
				nLineDefect++;
		}
		break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 약라인 불량 15개 이상인 경우 : Retest 판정
	//////////////////////////////////////////////////////////////////////////

	// 약라인 15개 이상인 경우
	if (nLineDefect > 15)
	{
		int nBuff = 0;

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// 약라인 일때
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
			{
				// 약라인 개수
				nBuff++;

				// 첫번째 약라인 : Retest 판정 수정
				if (nBuff == 1) { resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_LINE_BRIGHT; }

				// 나머지 약라인 삭제
				if (nBuff > 1) { resultPanelData.m_ListDefectInfo.RemoveAt(i); }
			}
			else i++;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//제	 목: 동일 Pattern 내 동일 방향의 불량을 한 불량으로 Merge											//  
//작업일: 2017-11-14																				//
//작업자: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementSamePatternDefectMerge(ResultPanelData& resultPanelData)
{
	int nLineNumber = 0;	// 진짜 라인 불량
	int nSSLineNumber = 0;	// 동일 Pattern, 동일 방향에 불량
	int nOffSet = 10;	// 중심점 거리 차이

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// 진짜 Line성 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
			nLineNumber++;	// 진짜 라인
			break;
		}
	}

	// 동일 Pattern 한방향에 있는 불량이 많을 때
	if (nLineNumber > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
			{
				CPoint	X1Center, X1Length;
				int		X1DefectImageType, X1DefectLength;

				X1Length.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;	// 결함 시작점
				X1Length.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X;	// 결함 끝점
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y; // Center위치
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴
				X1DefectLength = abs(X1Length.x - X1Length.y);								// 결함 길이

				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_BRIGHT	 &&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_OPEN	 &&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK)
					{
						j++; continue;
					}

					CPoint	X2Center, X2Length;
					int		X2DefectImageType, X2DefectLength;

					X2Length.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X;	// 결함 시작점
					X2Length.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X;	// 결함 끝점
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y; // Center위치
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴
					X2DefectLength = abs(X2Length.x - X2Length.y);								// 결함 길이

					// 동일 Pattern의 결함
					if (X1DefectImageType != X2DefectImageType) { j++; continue; }

					// 같은 방향
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// 						if ( X1DefectLength < X2DefectLength)
						// 						{
						// 							resultPanelData.m_ListDefectInfo[j].Pixel_Start_X = min(X1Length.x, X2Length.x);
						// 							resultPanelData.m_ListDefectInfo[j].Pixel_End_X   = max(X1Length.y, X2Length.y);
						// 
						// 							// 리스트 작은거 삭제시
						// 							if( i < j )	 j--;
						// 
						// 							// 다른 Pattern의 Line 제거
						// 							resultPanelData.m_ListDefectInfo.RemoveAt(i);
						// 						}
						// 
						// 						if ( X1DefectLength > X2DefectLength)
						// 						{
						resultPanelData.m_ListDefectInfo[i].Pixel_Start_X = min(X1Length.x, X2Length.x);
						resultPanelData.m_ListDefectInfo[i].Pixel_End_X = max(X1Length.y, X2Length.y);

						// 리스트 작은거 삭제시
						if (i > j)	 i--;

						// 다른 Pattern의 Line 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
						//}
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}
	}
	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//제	 목: Black Pattern 기준 주변 Bright Line Merge												//  
//작업일: 2017-10-21																				//
//       17.10.30 협의 결과: Black Pattern 폭기준 Dark Line 제거									//
//작업자: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementBlackPatternMerge(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// 진짜 라인 불량
	int nYLineNumber = 0;	// 진짜 라인 불량
	int nOffSet = 20;	// 중심점 거리 차이

	// 불량 갯수를 세는 부분
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// 진짜 X Line 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_DGS_X:
		case E_DEFECT_JUDGEMENT_LINE_PCD_CRACK:
			nXLineNumber++;	// 진짜 라인
			break;

			// 진짜 Y Line 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
		case E_DEFECT_JUDGEMENT_LINE_DGS_Y:
			nYLineNumber++;	// 진짜 라인
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// x 방향
	//////////////////////////////////////////////////////////////////////////

	if (nXLineNumber > 0) // X Line이 있으면 일해야 됨
	{
		//비교함
		int	X1DefectPattern = 0;
		int X1FistPattern = 0; // 제일 먼저 나오는 Pattern 수~
		int X1InitialPattern = 20;

		// 명선 불량 발생 Pattern 우선 순위 Black > Red > Green > Blue
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 명선이 제일 먼저 나온 Pattern을 기준 Pattern임
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT)
			{
				CPoint	X1St, X1End, X1Center;

				X1DefectPattern = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴
				X1FistPattern = min(X1InitialPattern, X1DefectPattern);
				X1InitialPattern = X1FistPattern;
			}
		}

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

				// 비교하고 얻은 Pattern 이여야 한다..ㅋ
				if (X1DefectImageType != X1FistPattern) continue;

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_OPEN			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
					{
						j++; continue;
					}

					CPoint	X2St, X2End, X2Center;
					int		X2DefectImageType;

					X2St.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2End.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					// 기준 Pattern과 같으면 안됨 다른 Pattern의 결함
					//if ( X1DefectImageType == X2DefectImageType ){ j++; continue; }

					// 주변에 Line이 검출 되는 경우
					//if ( (X1St.y <= X2Center.y) && (X1End.y >= X2Center.y) )
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// 리스트 작은거 삭제시
						if (i > j)	 i--;

						// 다른 Pattern의 모든 모든 모든 Line을 모두 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}

		//DGS
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_X)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_OPEN			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS_X)
					{
						j++; continue;
					}

					CPoint	X2St, X2End, X2Center;
					int		X2DefectImageType;

					X2St.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2End.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					//if (X1DefectImageType == X2DefectImageType) j++; continue;

					// 주변에 Line이 검출 되는 경우
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// 리스트 작은거 삭제시
						if (i > j)	 i--;

						// 다른 Pattern의 모든 모든 모든 Line을 모두 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}

		// PCD Crack 부분 삭제
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_PCD_CRACK)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					//if (resultPanelData.m_ListDefectInfo[j].Defect_Type != 	E_DEFECT_JUDGEMENT_LINE_PCD_CRACK)
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_OPEN			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS_X			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_PCD_CRACK)
					{
						j++; continue;
					}

					CPoint	X2St, X2End, X2Center;
					int		X2DefectImageType;

					X2St.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2End.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					// 기준 Pattern과 같으면 안됨 다른 Pattern의 결함
					if (X1DefectImageType == X2DefectImageType) { j++; continue; }

					// 주변에 Line이 검출 되는 경우
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// 리스트 작은거 삭제시
						if (i > j)	 i--;

						// 다른 Pattern의 모든 모든 모든 Line을 모두 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// y 방향
	//////////////////////////////////////////////////////////////////////////

	if (nYLineNumber > 0) // Y Line이 있으면 일해야 됨
	{
		//비교함
		int	Y1DefectPattern = 0;
		int Y1FistPattern = 0; // 제일 먼저 나오는 Pattern 수~
		int Y1InitialPattern = 20;

		// 명선 불량 발생 Pattern 우선 순위 Black / Red / Green / Blue
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Y 방향 명선이 제일 먼저 나온 Pattern을 기준 Pattern임
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT)
			{
				CPoint	Y1St, Y1End, Y1Center;

				Y1DefectPattern = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴
				Y1FistPattern = min(Y1InitialPattern, Y1DefectPattern);
				Y1InitialPattern = Y1FistPattern;
			}
		}

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT)
			{
				CPoint	Y1St, Y1End, Y1Center;
				int		Y1DefectImageType;

				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

				// 비교하고 얻은 Pattern 이여야 한다..ㅋ
				if (Y1DefectImageType != Y1FistPattern) continue;

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DARK			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT		&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT		&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
					{
						j++; continue;
					}

					CPoint	Y2St, Y2End, Y2Center;
					int		Y2DefectImageType;

					Y2St.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2Center.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					// 기준 Pattern과 같으면 안됨 다른 Pattern의 결함
					//if ( Y1DefectImageType == Y2DefectImageType ){ j++; continue; }

					// 주변에 Line이 검출 되는 경우
					//if ( (X1St.y <= X2Center.y) && (X1End.y >= X2Center.y) )
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// 리스트 작은거 삭제시
						if (i > j)	 i--;

						// 다른 Pattern의 모든 모든 모든 Line을 모두 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}

		//DGS Y Line
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_Y)
			{
				CPoint	Y1St, Y1End, Y1Center;
				int		Y1DefectImageType;

				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DARK			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT		&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT		&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS_Y)
					{
						j++; continue;
					}

					CPoint	Y2St, Y2End, Y2Center;
					int		Y2DefectImageType;

					Y2St.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2Center.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					// 주변에 Line이 검출 되는 경우
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// 리스트 작은거 삭제시
						if (i > j)	 i--;

						// 다른 Pattern의 모든 모든 모든 Line을 모두 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 제목: Camera Tap에 인한 약 Line 불량 과검 제거
// 작업일: 2018-07-31
// 작업자: PNZ
//////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementCameraTapOverKill(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{
	// 예외처리 Parameter 가 없으면
	if (dAlignPara == NULL)
		return false;

	int		CTO_OnOff = (int)(dAlignPara[E_PARA_CAMERA_TAP_OVERKILL_ONOFF]);
	int		CTO_CameraType = (int)(dAlignPara[E_PARA_CAMERA_TAP_OVERKILL_CAMERATYPE]);
	int		CTO_Offset = (int)(dAlignPara[E_PARA_CAMERA_TAP_OVERKILL_OFFSET]);

	// On/Off
	if (CTO_OnOff == 0)
		return false;

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// 초기화
	int	nLINE_X_DEFECT_Number = 0;
	int	nLINE_Y_DEFECT_Number = 0;

	// LINE Y Defect 불량 Count
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// X 방향 LINE DEFECT
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
		{
			// LINE 불량 Count
			nLINE_X_DEFECT_Number++;
		}

		// Y 방향 LINE DEFECT
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
		{
			// LINE 불량 Count
			nLINE_Y_DEFECT_Number++;
		}
	}

	// LINE Y Defect이 없으면 나감
	if (nLINE_Y_DEFECT_Number <= 0 && nLINE_X_DEFECT_Number <= 0) return true;

	//////////////////////////////////////////////////////////////////////////
	// Camera Type Info 받아 오기
	vector<int> Direction;
	vector<int> Position;
	vector<double> Bright_Diff_GV;
	vector<double> Dark_Diff_GV;

	bool CameraTapLoad = CameraTapInfo(CTO_CameraType, Direction, Position, Bright_Diff_GV, Dark_Diff_GV);
	if (!CameraTapLoad) return false;

	if (CTO_CameraType == 0 && Direction.size() > 4) return false; // 29M
	if (CTO_CameraType == 1 && Direction.size() < 4) return false; // 47M

	//////////////////////////////////////////////////////////////////////////
	// LINE Y DEFECT 확인
	if (nLINE_Y_DEFECT_Number > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// LINE 불량인 경우만
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DARK)
			{
				i++;
				continue;
			}

			// 좌표 비교
			int nDefectLocation = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;

			// Parameter 초기 화
			double DefectMeanGV = 0;
			double LeftMeanGV = 0;
			double RightMeanGV = 0;

			// CameraType 불량 판정
			bool CameraTypeOverKill = false;

			// 좌표 비교
			for (int m = 0; m < Position.size(); m++)
			{
				// Y 방향 아니면 다음
				if (Direction[m] != 2) continue;

				// 좌표 받아 오기
				int CTO_Position_Y = Position[m];

				// Tap 경계 근처에 있는지 확인
				if (abs(nDefectLocation - CTO_Position_Y) <= CTO_Offset)
				{
					// 발생 Pattern 확인
					int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
					int	nImgNum1 = theApp.GetImageClassify(nImgNum);

					// 해당 Pattern에 가서 확인
					cv::Mat DefectImage = MatOrgImage[theApp.GetImageNum(nImgNum1)][0];

					// Defect ROI
					cv::Rect DefectRect;

					DefectRect.x = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					DefectRect.y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					DefectRect.width = resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					DefectRect.height = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;

					double MaxGV;
					DefectLDRCompair(DefectImage, DefectRect, LeftMeanGV, DefectMeanGV, RightMeanGV, MaxGV);

					// Memory 해제
					DefectImage.release();

					// 판정
					if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT)
					{
						if (DefectMeanGV > RightMeanGV && (DefectMeanGV - RightMeanGV) < Bright_Diff_GV[m] && (DefectMeanGV - RightMeanGV) > 0)
							CameraTypeOverKill = true;

						else if (LeftMeanGV <= DefectMeanGV && DefectMeanGV <= RightMeanGV)
							CameraTypeOverKill = true;

						else
							CameraTypeOverKill = false;
					}

					else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK ||
						resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
					{
						if (DefectMeanGV < LeftMeanGV && (DefectMeanGV - LeftMeanGV) > Dark_Diff_GV[m] && (DefectMeanGV - LeftMeanGV) <= 0)
							CameraTypeOverKill = true;

						else if (LeftMeanGV <= DefectMeanGV && DefectMeanGV <= RightMeanGV)
							CameraTypeOverKill = true;

						else
							CameraTypeOverKill = false;
					}

					else	CameraTypeOverKill = false;
				}
			}

			if (CameraTypeOverKill)
			{
				// Dimple이면 제거
				resultPanelData.m_ListDefectInfo.RemoveAt(i);
			}
			else { i++; continue; }
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// LINE X DEFECT 확인
	if (nLINE_X_DEFECT_Number > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// LINE 불량인 경우만
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK)
			{
				i++;
				continue;
			}

			// 좌표 비교
			int nDefectLocation = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

			// Parameter 초기 화
			double DefectMeanGV = 0;
			double LeftMeanGV = 0;
			double RightMeanGV = 0;

			// CameraType 불량 판정
			bool CameraTypeOverKill = false;

			// 좌표 비교
			for (int m = 0; m < Position.size(); m++)
			{
				// Y 방향 아니면 다음
				if (Direction[m] != 1) continue;

				// 좌표 받아 오기
				int CTO_Position_X = Position[m];

				// Tap 경계 근처에 있는지 확인
				if (abs(nDefectLocation - CTO_Position_X) <= CTO_Offset)
				{
					// 발생 Pattern 확인
					int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
					int	nImgNum1 = theApp.GetImageClassify(nImgNum);

					// 해당 Pattern에 가서 확인
					cv::Mat DefectImage = MatOrgImage[theApp.GetImageNum(nImgNum1)][0];

					// Defect ROI
					cv::Rect DefectRect;

					DefectRect.x = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					DefectRect.y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					DefectRect.width = resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					DefectRect.height = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;

					if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK && DefectRect.height <= 2)
					{
						CameraTypeOverKill = true;
						break;
					}

					double MaxGV;
					DefectLDRCompair(DefectImage, DefectRect, LeftMeanGV, DefectMeanGV, RightMeanGV, MaxGV);

					// Memory 해제
					DefectImage.release();

					// 판정
					if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT)
					{
						if (DefectMeanGV > RightMeanGV && (DefectMeanGV - RightMeanGV) < Bright_Diff_GV[m] && (DefectMeanGV - RightMeanGV) > 0)
							CameraTypeOverKill = true;

						else if (LeftMeanGV <= DefectMeanGV && DefectMeanGV <= RightMeanGV)
							CameraTypeOverKill = true;

						else
							CameraTypeOverKill = false;
					}

					else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
						resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
					{
						if (DefectMeanGV < LeftMeanGV && (DefectMeanGV - LeftMeanGV) > Dark_Diff_GV[m] && (DefectMeanGV - LeftMeanGV) <= 0)
							CameraTypeOverKill = true;

						else if (LeftMeanGV <= DefectMeanGV && DefectMeanGV <= RightMeanGV)
							CameraTypeOverKill = true;

						else
							CameraTypeOverKill = false;
					}

					else	CameraTypeOverKill = false;
				}
			}

			if (CameraTypeOverKill)
			{
				// Dimple이면 제거
				resultPanelData.m_ListDefectInfo.RemoveAt(i);
			}
			else { i++; continue; }
		}
	}

	return true;
}

bool AviInspection::DefectLDRCompair(cv::Mat matSrcImage, cv::Rect rectTemp, double& Left_MeanValue, double& Defect_MeanValue, double& Right_MeanValue, double& Defect_MaxGV)
{
	// 초기화
	cv::Scalar Left_Mean, Left_StdDev, Right_Mean, Right_StdDev, Defect_Mean, Defect_StdDev;
	cv::Rect rectTemp_Left, rectTemp_Right;
	int XShiftUnit = 0;
	int YShiftUnit = 0;
	int CutUnit_X_1 = 600;
	int CutUnit_X_2 = 150;
	int CutUnit_Y_1 = 150;
	int CutUnit_Y_2 = 150;
	int nOffset = 50;

	// Y 판정
	if (rectTemp.width / rectTemp.height < 1)
	{
		// 예외처리
		if (rectTemp.height <= CutUnit_Y_1 + CutUnit_Y_2 + nOffset) return false;

		rectTemp.y = rectTemp.y + CutUnit_Y_1;
		rectTemp.height = rectTemp.height - CutUnit_Y_1 - CutUnit_Y_2;

		XShiftUnit = 40;
		YShiftUnit = 0;
		rectTemp_Left.width = 20;
		rectTemp_Left.height = rectTemp.height;
		rectTemp_Right.width = 20;
		rectTemp_Right.height = rectTemp.height;

		// 불량 넓이가 아주 작은 불량의 폭을 증가
		if (rectTemp.width <= 4)
		{
			rectTemp.width = rectTemp.width + 10;
			rectTemp.x = rectTemp.x - 5;
		}
	}

	// X Defect
	else if (rectTemp.width / rectTemp.height >= 1)
	{
		// 예외처리
		if (rectTemp.width <= CutUnit_X_1 + CutUnit_X_2 + nOffset) return false;

		rectTemp.x = rectTemp.x + CutUnit_X_1;
		rectTemp.width = rectTemp.width - CutUnit_X_1 - CutUnit_X_2;

		XShiftUnit = 0;
		YShiftUnit = 40;
		rectTemp_Left.width = rectTemp.width;
		rectTemp_Left.height = 20;
		rectTemp_Right.width = rectTemp.width;
		rectTemp_Right.height = 20;
	}

	//////////////////////////////////////////////////////////////////////////

		// Left ROI
	rectTemp_Left.x = rectTemp.x - XShiftUnit;
	rectTemp_Left.y = rectTemp.y - YShiftUnit;

	// Right ROI
	rectTemp_Right.x = rectTemp.x + XShiftUnit;
	rectTemp_Right.y = rectTemp.y + YShiftUnit;

	// GaussianBlur
	cv::resize(matSrcImage, matSrcImage, matSrcImage.size() / 2, INTER_LINEAR);
	cv::GaussianBlur(matSrcImage, matSrcImage, cv::Size(5, 5), 3.0);
	cv::resize(matSrcImage, matSrcImage, matSrcImage.size() * 2, INTER_LINEAR);

	// 영역 Image
	cv::Mat LeftImage = matSrcImage(rectTemp_Left);
	cv::Mat RightImage = matSrcImage(rectTemp_Right);
	cv::Mat DefectImage = matSrcImage(rectTemp);

	cv::meanStdDev(LeftImage, Left_Mean, Left_StdDev);
	cv::meanStdDev(RightImage, Right_Mean, Right_StdDev);
	cv::meanStdDev(DefectImage, Defect_Mean, Defect_StdDev);

	double minvalue = 0;

	cv::minMaxIdx(DefectImage, &minvalue, &Defect_MaxGV, NULL, NULL);

	Left_MeanValue = Left_Mean[0];
	Right_MeanValue = Right_Mean[0];
	Defect_MeanValue = Defect_Mean[0];

	return true;
}

bool 	AviInspection::CameraTapInfo(int CameraType, vector<int> &Direction, vector<int> &Position, vector<double> &BrightDiffGV, vector<double> &DarkDiffGV)
{
	CString CameraTypeFile;
	CameraTypeFile.Format(_T("D:\\CRUX\\DATA\\INI\\CCD\\CameraTap.Index"));

	// 파일 존재 유/무 확인 필요
	CFileFind find;
	BOOL bFindFile = FALSE;

	bFindFile = find.FindFile(CameraTypeFile);
	find.Close();

	// 파일 없는 경우
	if (!bFindFile) return false;

	char szFileName[256] = { 0, };
	WideCharToMultiByte(CP_ACP, 0, CameraTypeFile, -1, szFileName, sizeof(szFileName), NULL, NULL);

	FILE *out = NULL;
	fopen_s(&out, szFileName, "r");

	//?
	if (!out)	return false;

	vector<int> vDirection;
	vector<int> vPosition;
	vector<double> vGV;

	int nDirectionBuf;
	int nPositionBuf;
	double dbBrightDiffGVBuf;
	double dbDarktDiffGVBuf;

	//////////////////////////////////////////////////////////////////////////
	// Camera Type 선택
	int X_Index;
	int Y_Index;
	int CalBuff;

	if (CameraType == 0) { X_Index = 3288;		Y_Index = 4384; } // 29M Camera 8  Tap / PS Image : 13152 x 8768
	else if (CameraType == 1) { X_Index = 2214;		Y_Index = 5280; } // 47M Camera 16 Tap / PS Image : 17712 x 10560
	else
		return false;

	for (int m = 0; ; m++)
	{
		// 읽어오기
		fscanf_s(out, "%d,%d,%lf,%lf\n", &nDirectionBuf, &nPositionBuf, &dbBrightDiffGVBuf, &dbDarktDiffGVBuf);

		// 위치를 계산함
		if (nDirectionBuf == 1) { CalBuff = nPositionBuf * Y_Index; } // X 방향
		if (nDirectionBuf == 2) { CalBuff = nPositionBuf * X_Index; } // Y 방향

		// 전에 있는 Data와 유사하면 Break
		if (m != 0 && nDirectionBuf == Direction[m - 1] && CalBuff == Position[m - 1] &&
			dbBrightDiffGVBuf == BrightDiffGV[m - 1] && dbDarktDiffGVBuf == DarkDiffGV[m - 1])
			break;

		// Data 저장
		Direction.push_back(nDirectionBuf);
		Position.push_back(CalBuff);
		BrightDiffGV.push_back(dbBrightDiffGVBuf);
		DarkDiffGV.push_back(dbDarktDiffGVBuf);

		if (CameraType == 0 && Direction.size() == 4) break;
	}

	fclose(out);
	out = NULL;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//제	 목: PCD CRACK 판정																			//  
//작업일: 2017-12-01																				//
//작업자: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementPCDCrack(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// 값이 없는 경우
	if (dAlignPara == NULL)	return false;

	// 사용 안하는 경우
	if (dAlignPara[E_PARA_PCD_CRACK_FLAG] <= 0)	return true;

	// Start Position Y
	cv::Point ptAlignCorner[4]; // Align 후 Cell Corner

	ptAlignCorner[0].x = m_stThrdAlignInfo.rcAlignCellROI.x;
	ptAlignCorner[0].y = m_stThrdAlignInfo.rcAlignCellROI.y;

	// Black, Red ,Blue Pattern에서
	// 고정 위치 X1 = 740~820, X2 = 2320~2400
	int nRange1Start = (740 * 2) + ptAlignCorner[0].y;
	int nRange1End = (820 * 2) + ptAlignCorner[0].y;
	int nRange2Start = (2320 * 2) + ptAlignCorner[0].y;
	int nRange2End = (2400 * 2) + ptAlignCorner[0].y;

	int nXLineNumber = 0;	// 진짜 라인 불량 
	int nOffSet = 20;	// 중심점 거리 차이

	// Merge후 남은 X Line Bright 불량
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// 진짜 X Line 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
			nXLineNumber++;	// 진짜 라인
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// x 방향에 Line 만 봄
	//////////////////////////////////////////////////////////////////////////

	if (nXLineNumber > 0) // X Line이 있으면 일해야 됨
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT)
			{
				CPoint	X1Center;

				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				//AfxMessageBox(_T("Center Distance: '%d'"), X1Center.y);

				//Ratio 만큼 기준 좌표 변한다.
				int nSubRange1_Start = nRange1Start / resultPanelData.m_ListDefectInfo[i].nRatio;
				int nSubRange1_End = nRange1End / resultPanelData.m_ListDefectInfo[i].nRatio;
				int nSubRange2_Start = nRange2Start / resultPanelData.m_ListDefectInfo[i].nRatio;
				int nSubRange2_End = nRange2End / resultPanelData.m_ListDefectInfo[i].nRatio;

				int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
				int	nImgNum1 = theApp.GetImageClassify(nImgNum);

				switch (nImgNum1)
				{
				case E_IMAGE_CLASSIFY_AVI_BLACK:
				case E_IMAGE_CLASSIFY_AVI_R:
				case E_IMAGE_CLASSIFY_AVI_B:
				{
					if ((nSubRange1_Start - nOffSet < X1Center.y) && (X1Center.y < nSubRange1_End + nOffSet))
					{
						resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_PCD_CRACK;
					}

					else if ((nSubRange2_Start - nOffSet < X1Center.y) && (X1Center.y < nSubRange2_End + nOffSet))
					{
						resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_PCD_CRACK;
					}

					else continue;
				}

				break;
				}
			}
		}
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//제	 목: DGS 판정 새기준 ver 0.3																	//  
//작업일: 2017-11-03, 2017-11-15 수정																// 
//작업자: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementNewDGS(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// X Line 라인 불량
	int nYLineNumber = 0;	// Y Line 라인 불량
	int nOffSet = 20;	// 중심점 거리 차이
	int nCenterOffSet = 30;

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// 진짜 Line성 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_VTH_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_VTH_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_DARK:
			nXLineNumber++;	// 진짜 라인
			break;

			// 진짜 Line성 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_VTH_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_VTH_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_VTH_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_VTH_DEFECT_DARK:
			nYLineNumber++;	// 진짜 라인
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 동일 Pattern에서 Cross 되면 DGS로 판정
	//////////////////////////////////////////////////////////////////////////

	int nCountDGS = 0;

	if ((nXLineNumber > 0) && (nYLineNumber > 0))
	{
		// 불량 개수
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_VTH_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_VTH_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_VTH_DGS_X)
			{
				CPoint	X1St, X1End;
				int X1DefectPattern;

				X1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectPattern = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
				{
					// 같은 불량 비교 방지
					if (i == j) { continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT				&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DARK				&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK		&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT		&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_VTH_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_VTH_DARK			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_VTH_DEFECT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_VTH_DEFECT_DARK	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS_Y)
					{
						continue;
					}

					CPoint	Y2St, Y2End;
					int Y2DefectPattern;

					Y2St.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2St.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2DefectPattern = resultPanelData.m_ListDefectInfo[j].Pattern_Type;

					if (X1DefectPattern != Y2DefectPattern) { continue; } // 동일 Pattern 아니면 안됨 이거 DGS 래...

					// 주변에 Line이 검출 되는 경우
					if ((X1St.x <= Y2St.x) && (X1End.x >= Y2End.x))
					{
						if ((X1St.y >= Y2St.y) && (X1End.y <= Y2End.y))
						{

							resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_DGS_X;
							resultPanelData.m_ListDefectInfo[j].Defect_Type = E_DEFECT_JUDGEMENT_LINE_DGS_Y;

							ResultDefectInfo SubDefect = resultPanelData.m_ListDefectInfo[i];

							SubDefect.Pixel_Start_Y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
							SubDefect.Pixel_End_Y = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;
							SubDefect.Pixel_Start_X = resultPanelData.m_ListDefectInfo[j].Pixel_Start_X;
							SubDefect.Pixel_End_X = resultPanelData.m_ListDefectInfo[j].Pixel_End_X;

							SubDefect.Defect_Type = E_DEFECT_JUDGEMENT_LINE_DGS;

							resultPanelData.m_ListDefectInfo.Add(SubDefect);

							nCountDGS++;

						}
						else continue;
					}
					else continue;
				}
			}
		}
	}

	//DGS_X, DGS_Y 주변 Line 불량 제거
	if (nCountDGS > 0)
	{
		// DGS 중복 제거
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectPattern;
				X1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				//X1Center.x		= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.x = (long)((X1St.x + X1End.y) / 2.0);
				//X1Center.y		= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)((X1St.y + X1End.y) / 2.0);
				X1DefectPattern = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS)
					{
						j++; continue;
					}

					CPoint	Y2St, Y2End, Y2Center;
					int		Y2DefectPattern;

					Y2St.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2St.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio;

					//Y2Center.x		= (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X	/ resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2Center.x = (long)((Y2St.x + Y2End.x) / 2.0);
					//Y2Center.y		= (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y	/ resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2Center.y = (long)((Y2St.y + Y2End.y) / 2.0);
					Y2DefectPattern = resultPanelData.m_ListDefectInfo[j].Pattern_Type;

					// 주변에 Line이 검출 되는 경우
					if (abs(X1Center.y - Y2Center.y) <= nCenterOffSet)
					{
						if (abs(X1Center.x - Y2Center.x) <= nCenterOffSet)
						{
							// 리스트 작은거 삭제시
							if (i > j)	 i--;

							// 다른 Pattern의 Line 제거
							resultPanelData.m_ListDefectInfo.RemoveAt(j);
						}
						else  j++;
					}
					else  j++;
				}
			}


			// X 방향 DGS X와 중복 된 DGS X만 제거
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_X)
			{
				CPoint	X1St, X1End, X1Center;
				int X1DefectPattern;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectPattern = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS_X)
					{
						j++; continue;
					}

					CPoint	Y2St, Y2End, Y2Center;
					int Y2DefectPattern;

					Y2St.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2DefectPattern = resultPanelData.m_ListDefectInfo[j].Pattern_Type;

					// 주변에 Line이 검출 되는 경우
					if (abs(X1Center.y - Y2Center.y) <= nOffSet)
					{
						// 리스트 작은거 삭제시
						if (i > j)	 i--;

						// 다른 Pattern의 Line 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
					// 주변에 결함이 없는 경우
					else  j++;
				}
			}

			// Y 방향 DGS Y와 중복 된 DGS Y만 제거
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_Y)
			{
				CPoint	Y1St, Y1End, Y1Center;
				int Y1DefectPattern;

				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectPattern = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// Y 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS_Y)
					{
						j++; continue;
					}

					CPoint	Y2St, Y2End, Y2Center;
					int Y2DefectPattern;

					Y2St.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2Center.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2DefectPattern = resultPanelData.m_ListDefectInfo[j].Pattern_Type;

					// 주변에 Line이 검출 되는 경우
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// 리스트 작은거 삭제시
						if (i > j)	 i--;

						// 다른 Pattern의 Line 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}
	}
	return true;
}
// YDS 18.05.25
// Vth Pattern DGS
bool AviInspection::JudgementDGS_Vth(ResultPanelData& resultPanelData)
{
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_X ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_Y)
		{
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_VTH)
			{
				switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
				{
				case E_DEFECT_JUDGEMENT_LINE_DGS:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_VTH_DGS;
					break;
				case E_DEFECT_JUDGEMENT_LINE_DGS_X:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_VTH_DGS_X;
					break;
				case E_DEFECT_JUDGEMENT_LINE_DGS_Y:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_VTH_DGS_Y;
					break;
				case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_VTH_BRIGHT;
					break;
				case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_VTH_BRIGHT;
					break;
				case E_DEFECT_JUDGEMENT_LINE_X_DARK:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_VTH_DARK;
					break;
				case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_VTH_DARK;
					break;
				case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_BRIGHT;
					break;
				case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_VTH_DEFECT_BRIGHT;
					break;
				case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_DARK;
					break;
				case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_VTH_DEFECT_DARK;
					break;
				}
			}
		}

		/*if(	resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_VTH_BRIGHT			||
		resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_VTH_BRIGHT			||
		resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_BRIGHT	||
		resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_VTH_DEFECT_BRIGHT	)
		{
		resultPanelData.m_ListDefectInfo.RemoveAt(i);
		i--;
		}*/
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//제	 목: Crack 판정 ver 0.2																		//  
//작업일: 2017-11-10																				// 
//작업자: PNZ																						//
//Crack Pattern: G32 기준 (Image Number 9)														//
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementCrack(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// Line 라인 불량

	int nCPLNumber_Left = 0;	// Crack Pattern Left Line 라인 불량
	int nCPLNumber_Right = 0;	// Crack Pattern Right Line 라인 불량
	int nOffSet = 10;	// 중심점 거리 차이

	// Position of Crack Defect
	int Distance1 = 1467;
	int Distance2 = 1761;
	int Distance3 = 2054;

	cv::Point ptAlignCorner[4]; // Align 후 Cell Corner

	ptAlignCorner[0].x = m_stThrdAlignInfo.rcAlignCellROI.x;
	ptAlignCorner[0].y = m_stThrdAlignInfo.rcAlignCellROI.y;

	ptAlignCorner[2].x = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
	ptAlignCorner[2].y = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

	int CenterLocation = ptAlignCorner[0].y + abs(ptAlignCorner[2].y - ptAlignCorner[0].y) / 2;
	int Distance = 0;

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_32) continue; //choikwangil 64변경 원복

		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// 진짜 Line성 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
			nXLineNumber++;	// 진짜 라인
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Black Pattern에 고정 위치 명신이 나타나면 Crack 불량
	//////////////////////////////////////////////////////////////////////////

	if (nXLineNumber > 0)
	{
		// 불량 개수
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_32) continue;

			// X 방향
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT)
			{
				CPoint	X1Center;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;
				//X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;

				if (X1Center.y <= CenterLocation) { nCPLNumber_Left++; }	//Left 고정 범위에 있는 불량 개수
				if (X1Center.y > CenterLocation) { nCPLNumber_Right++; }	//Right 고정 범위에 있는 불량 개수
			}
			else continue;
		}
	}

	if (nCPLNumber_Left >= 3)
	{
		// 불량 개수
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_32) continue;

			// X 방향
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT)
			{
				CPoint	X1Center;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				if (X1Center.y <= CenterLocation)
				{
					if (((abs(X1Center.y - ptAlignCorner[0].y) - Distance1) < nOffSet) ||
						((abs(X1Center.y - ptAlignCorner[0].y) - Distance2) < nOffSet) ||
						((abs(X1Center.y - ptAlignCorner[0].y) - Distance3) < nOffSet))
					{
						resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_CRACK_LEFT;
					}
				}
			}
			else continue;
		}
	}

	if (nCPLNumber_Right >= 3)
	{
		// 불량 개수
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_32) continue;

			// X 방향
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT)
			{
				CPoint	X1Center;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				if (X1Center.y > CenterLocation)
				{
					if (((abs(X1Center.y - ptAlignCorner[2].y) - Distance1) < nOffSet) ||
						((abs(X1Center.y - ptAlignCorner[2].y) - Distance2) < nOffSet) ||
						((abs(X1Center.y - ptAlignCorner[2].y) - Distance3) < nOffSet))
					{
						resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_CRACK_RIGHT;
					}
				}
			}
			else continue;
		}
	}

	// Left와 Right 다 3개 이상이 이면 Crack Left와 Right를 Both로 바꾼다
	if ((nCPLNumber_Left >= 3) && (nCPLNumber_Right >= 3))
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_CRACK_LEFT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_CRACK_RIGHT)
			{
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_CRACK_BOTH;
			}
		}
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//제	 목: Line 결함 분류 ver 0.2																	//  
//작업일: 2017-11-14																				// 
//작업자: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementClassifyLineDefect(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT])
{
	int nLineNumber = 0;	// 진짜 라인 불량
	int nLineDefect = 0;	// 약한 라인
	int nOffSet = 20;	// Line 불량 인접 범위

	cv::Mat ImgBK;
	if (!MatOrgImage[MAX_GRAB_STEP_COUNT - 4][0].empty())
	{
		ImgBK = MatOrgImage[MAX_GRAB_STEP_COUNT - 4][0].clone();
	}
	else return false;

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// Line성 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
			nLineNumber++;
			break;
		}
	}

	// Line 결함이 존재 하면
	if (nLineNumber > 0)
	{
		//////////////////////////////////////////////////////////////////////////
		// DO, GO 판정
		//////////////////////////////////////////////////////////////////////////

		// 불량 개수
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN)
			{
				CPoint	X1St, X1End;

				X1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
				X1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;

				int nMaxGV = resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
				int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
				int	nImgNum1 = theApp.GetImageClassify(nImgNum);

				// Black 또는 Crack Pattern 강Line이 있으면 5.5시 G32가 Crack Pattern임
				if ((nImgNum1 == E_IMAGE_CLASSIFY_AVI_BLACK) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_32))
				{
					if (nMaxGV >= 200)
					{
						nOffSet = 150; // 크게 설정
					}
				}
				// RGB Pattern Dark Line 일 때  외곽 비검사 영역으로 인한 offset 늘리기
				else if ((nImgNum1 == E_IMAGE_CLASSIFY_AVI_R) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_G) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_B))
				{
					if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK && nOffSet < 30)
						nOffSet = 30;
				}

				//예외처리

				int BKImageLenght = ImgBK.cols;
				int StartPoint = X1St.x - nOffSet;
				int EndPoint = X1End.x + nOffSet;

				if (StartPoint < 0)	StartPoint = 0;
				if (EndPoint > BKImageLenght - 1)	EndPoint = BKImageLenght - 1;

				uchar* BKGV_St = ImgBK.ptr<uchar>(X1St.y, StartPoint);
				uchar* BKGV_End = ImgBK.ptr<uchar>(X1End.y, EndPoint);
				if (*BKGV_St > 0 && *BKGV_End > 0) continue;
				else if ((*BKGV_St > 0 && *BKGV_End == 0) || (*BKGV_St == 0 && *BKGV_End > 0))
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_OPEN;
				else continue;
			}

			// Y 방향
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT)
			{
				CPoint	Y1St, Y1End;//, Y1Center;
				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
				Y1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
				Y1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;

				int nMaxGV = resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
				int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
				int	nImgNum1 = theApp.GetImageClassify(nImgNum);

				// Black 또는 Crack Pattern 강Line이 있으면 5.5시 G32가 Crack Pattern임
				if ((nImgNum1 == E_IMAGE_CLASSIFY_AVI_BLACK) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_32))
				{
					if (nMaxGV >= 200)
					{
						nOffSet = 150; // 크게 설정
					}
				}
				// RGB Pattern Dark Line 일 때  외곽 비검사 영역으로 인한 offset 늘리기
				else if ((nImgNum1 == E_IMAGE_CLASSIFY_AVI_R) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_G) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_B))
				{
					if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK && nOffSet < 30)
						nOffSet = 30;
				}

				// 예외처리

				int BKImageLenght = ImgBK.rows;
				int StartPoint = Y1St.y - nOffSet;
				int EndPoint = Y1End.y + nOffSet;

				if (StartPoint < 0)	StartPoint = 0;
				if (EndPoint > BKImageLenght - 1)	EndPoint = BKImageLenght - 1;

				uchar* BKGV_St = ImgBK.ptr<uchar>(StartPoint, Y1St.x);
				uchar* BKGV_End = ImgBK.ptr<uchar>(EndPoint, Y1End.x);
				if (*BKGV_St > 0 && *BKGV_End > 0) continue;
				else if ((*BKGV_St > 0 && *BKGV_End == 0))
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT;
				else if ((*BKGV_St == 0 && *BKGV_End > 0))
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT;
			}
		}
	}
	ImgBK.release();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//제	 목: 6.39QHD Notch Y Line 제거															//
//작업일: 2018-07.04																			//
//작업자: YDS																					//
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementNotchDefect(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{

	if (dAlignPara == NULL)
		return false;

	int		NORCH_OnOff = (int)(dAlignPara[E_PARA_NORCH_OVERKILL_ONOFF]);
	int		NORCH_UNIT_X = (int)(dAlignPara[E_PARA_NORCH_OVERKILL_UNIT_X]);
	int		NORCH_UNIT_Y = (int)(dAlignPara[E_PARA_NORCH_OVERKILL_UNIT_Y]);
	double	NORCH_THGV_GREEN = (double)(dAlignPara[E_PARA_NORCH_OVERKILL_THGV_GREEN]);
	double	NORCH_THGV_RB = (double)(dAlignPara[E_PARA_NORCH_OVERKILL_THGV_RB]);
	int		NORCH_ABS_GREEN = (int)(dAlignPara[E_PARA_NORCH_OVERKILL_ABS_GREEN]);
	int		NORCH_ABS_RB = (int)(dAlignPara[E_PARA_NORCH_OVERKILL_ABS_RB]);
	int		NORCH_OFFSET = (int)(dAlignPara[E_PARA_NORCH_OVERKILL_OFFSET]);

	// On/Off 확인 ( 거이 쓰야 됨 ) Norch 없는 거는 꺼두 됨
	if (NORCH_OnOff == 0) return false;

	int		nYLineNumber = 0;	// 진짜 라인 불량
	int		nOffSet = 0;	// Line 불량 인접 범위
	int		nLengthOffSet = 0;	// Norch 불량 길이 Offset
	bool	ImgBKEmpty = false;

	cv::Mat ImgBK;
	if (!MatOrgImage[MAX_GRAB_STEP_COUNT - 4][0].empty())
	{
		ImgBK = MatOrgImage[MAX_GRAB_STEP_COUNT - 4][0].clone();
	}
	else
		ImgBKEmpty = true;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// Line성 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_DGS_Y:
			nYLineNumber++;	// 진짜 라인
			break;
		}
	}

	if (nYLineNumber > 0 && ImgBKEmpty == false)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{

			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number; // UI ( list )		
			int	nImgNum1 = theApp.GetImageClassify(nImgNum); // Alg ( E_IMAGE_CLASSIFY_AVI_R ... )

			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_BLACK) continue;

			nOffSet = NORCH_OFFSET;
			nLengthOffSet = 150;

			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_Y ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_XLINE_SPOT)
			{
				// Norch 부분 불량인지 확인
				vector<int> NorchIndex;
				CPoint OrgIndex, DefectY_Center;
				int DefectY_XStart, NorchLocation, TopNorchLocation, BottomNorchLocation, DefectLength, DefectY_YStart, DefectY_YEnd, DefectY_MaxGV;

				// UI ( list )
				bool bNorchInfoGet = GetModelNorchInfo(theApp.m_pGrab_Step[nImgNum].tRoundSet, NorchIndex, OrgIndex);

				int NorchAvgWidth = 0;
				int NorchAvgHeight = 0;

				if (bNorchInfoGet == true)
				{
					NorchAvgWidth = (NorchIndex[0] + NorchIndex[2]) / 2;
					NorchAvgHeight = (NorchIndex[1] + NorchIndex[3]) / 2;

					NorchLocation = NorchAvgWidth + m_stThrdAlignInfo.rcAlignCellROI.x + nOffSet;
					TopNorchLocation = OrgIndex.x + m_stThrdAlignInfo.rcAlignCellROI.y;
					BottomNorchLocation = OrgIndex.y + m_stThrdAlignInfo.rcAlignCellROI.y;
				}

				// Norch 부분 좌표 계산 하기
				else if (bNorchInfoGet == false)
				{
					NorchAvgWidth = NORCH_UNIT_X;
					NorchAvgHeight = NORCH_UNIT_Y;

					NorchLocation = NORCH_UNIT_X + m_stThrdAlignInfo.rcAlignCellROI.x + nOffSet;
					TopNorchLocation = NORCH_UNIT_Y + m_stThrdAlignInfo.rcAlignCellROI.y;
					BottomNorchLocation = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height - NORCH_UNIT_Y;
				}

				// 불량 정보
				DefectY_Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				DefectY_Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;
				DefectY_YStart = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
				DefectY_YEnd = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;
				DefectY_XStart = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
				DefectY_MaxGV = (long)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;

				DefectLength = abs(DefectY_YStart - DefectY_YEnd);

				// Case1Norch 부분 외의 LINE 처리 안함
				if (DefectY_Center.x >= NorchLocation) continue;
				if (DefectY_MaxGV >= 250) continue;

				int DefectY_GV = ImgBK.at<uchar>(DefectY_Center.y, (DefectY_XStart - nOffSet));

				// Case1 : 두 Norch 사이에 있는 불량
				if ((TopNorchLocation <= DefectY_Center.y && DefectY_Center.y <= BottomNorchLocation) && DefectY_GV == 255)
				{
					// Parameter 초기 화
					double DefectMeanGV = 0;
					double LeftMeanGV = 0;
					double RightMeanGV = 0;

					// 해당 Pattern에 가서 확인
					cv::Mat DefectImage = MatOrgImage[theApp.GetImageNum(nImgNum1)][0](Rect(0, 0, NorchLocation + nLengthOffSet, MatOrgImage[theApp.GetImageNum(nImgNum1)][0].rows));

					// Defect ROI
					cv::Rect DefectRect;

					DefectRect.x = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					DefectRect.y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					DefectRect.width = resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					DefectRect.height = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;

					double MaxGV;

					DefectLDRCompair(DefectImage, DefectRect, LeftMeanGV, DefectMeanGV, RightMeanGV, MaxGV);

					double dbDiffGV = 0;

					if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_R || nImgNum1 == E_IMAGE_CLASSIFY_AVI_B) dbDiffGV = NORCH_ABS_RB;
					if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_G)										dbDiffGV = NORCH_ABS_GREEN;

					if (abs(DefectMeanGV - RightMeanGV) <= dbDiffGV)
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(i);
						i--;
					}
				}

				else if ((TopNorchLocation <= DefectY_Center.y && DefectY_Center.y <= BottomNorchLocation) && (DefectLength > NorchAvgHeight + nLengthOffSet))
				{
					int DefectY_Start_GV = ImgBK.at<uchar>(DefectY_YStart, (DefectY_XStart - nOffSet));
					int DefectY_End_GV = ImgBK.at<uchar>(DefectY_YEnd, (DefectY_XStart - nOffSet));

					if (DefectY_Start_GV + DefectY_End_GV > 0)
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(i);
						i--;
					}
				}

				else if ((DefectY_Center.y < TopNorchLocation || BottomNorchLocation < DefectY_Center.y) && (DefectLength > NorchAvgHeight + nLengthOffSet))
				{
					int DefectY_Start_GV = ImgBK.at<uchar>(DefectY_YStart, (DefectY_XStart - nOffSet));
					int DefectY_End_GV = ImgBK.at<uchar>(DefectY_YEnd, (DefectY_XStart - nOffSet));

					if (DefectY_Start_GV + DefectY_End_GV > 0)
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(i);
						i--;
					}
				}

				// 추가 Norch 부 애해한 과검 들
				else if ((DefectY_Center.y < TopNorchLocation || BottomNorchLocation < DefectY_Center.y) && (DefectLength <= NorchAvgHeight + nLengthOffSet))
				{
					// Parameter 초기 화
					double DefectMeanGV = 0;
					double LeftMeanGV = 0;
					double RightMeanGV = 0;

					// CameraType 불량 판정
					bool NorchOverKill = false;

					int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
					int	nImgNum1 = theApp.GetImageClassify(nImgNum);

					double NORCH_THGV = 0;

					// Pattern 별 Diff GV 다르게 선택
					if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_G) NORCH_THGV = NORCH_THGV_GREEN;
					if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_R || nImgNum1 == E_IMAGE_CLASSIFY_AVI_B) NORCH_THGV = NORCH_THGV_RB;

					// Norch 경계 부분에 있다고 하면
					if ((NorchLocation - nOffSet) >= DefectY_XStart)								  NORCH_THGV = NORCH_THGV_RB;

					// 해당 Pattern에 가서 확인
					cv::Mat DefectImage = MatOrgImage[theApp.GetImageNum(nImgNum1)][0](Rect(0, 0, NorchLocation + nLengthOffSet, MatOrgImage[theApp.GetImageNum(nImgNum1)][0].rows));

					// Defect ROI
					cv::Rect DefectRect;

					DefectRect.x = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					DefectRect.y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					DefectRect.width = resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					DefectRect.height = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;

					double MaxGV;

					bool CompairResult = DefectLDRCompair(DefectImage, DefectRect, LeftMeanGV, DefectMeanGV, RightMeanGV, MaxGV);

					if (CompairResult == false)															NorchOverKill = true;

					if (MaxGV >= 200)																	continue;

					if (LeftMeanGV <= DefectMeanGV && DefectMeanGV <= RightMeanGV)						NorchOverKill = true;

					else if (abs(DefectMeanGV - (LeftMeanGV + RightMeanGV) / 2) < NORCH_THGV)	NorchOverKill = true;

					else																				NorchOverKill = false;

					if (NorchOverKill == true)
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(i);
						i--;
					}
				}
			}
		}
	}

	if (nYLineNumber > 0 && ImgBKEmpty == true)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_Y)
			{
				int DefectY_Center = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;

				if (abs((m_stThrdAlignInfo.rcAlignCellROI.x + 400) - DefectY_Center) < 100)
				{
					resultPanelData.m_ListDefectInfo.RemoveAt(i);
					i--;
				}
			}
		}
	}

	ImgBK.release();
	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//제	 목: Special Pattern와 Gray Pattern 약선 검출 결과 비교 ver. 0.1					//  
//작업일: 2017-11-22																					//
//작업자: PNZ																								//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementSpecialGrayLineDefect(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// 진짜 라인 불량
	int nYLineNumber = 0;	// 진짜 라인 불량
	int nOffSet = 10;	// 중심점 거리 차이

	// 불량 갯수를 세는 부분
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
			nXLineNumber++;	// 진짜 라인
			break;

		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
			nYLineNumber++;	// 진짜 라인
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// x 방향
	//////////////////////////////////////////////////////////////////////////

	if (nXLineNumber > 1) // X Line이 있으면 일해야 됨
	{
		//비교함
		int PatternIndex;
		int	XLine_G64 = 0;
		int XLine_SP2 = 0;

		// G64 Pattern에서 검출 된 Line Defect를 Count
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 명선이 제일 먼저 나온 Pattern을 기준 Pattern임
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
			{
				PatternIndex = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;
				if (PatternIndex == 8)
				{
					XLine_G64++;
				}
			}
		}

		// Special - 2 Pattern에서 검출 된 Line Defect를 Count
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 명선이 제일 먼저 나온 Pattern을 기준 Pattern임
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
			{
				PatternIndex = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;
				if (PatternIndex == 6)
				{
					XLine_SP2++;
				}
			}
		}

		//SP2에 Line 결함이 없을 때 G64에서 불량이 있을 때
		if ((XLine_G64 > 0) && (XLine_SP2 == 0))
		{
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT		&&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK		&&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT		&&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
				{
					j++; continue;
				}

				int		X2DefectImageType;
				int		FirstLINEDefect = 0;

				X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

				// G64 Pattern에서 
				if (X2DefectImageType == 9)
				{
					//첫번째 Line Defect
					FirstLINEDefect++;
					if (FirstLINEDefect == 1)
					{
						resultPanelData.m_ListDefectInfo[j].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_LINE_BRIGHT;
					}

					if (FirstLINEDefect > 1)
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
				}
				else  j++;
			}
		}


		//G64 Pattern을 기준으로 Special Pattern에서 검출된 Line Defect과 비교한다, 만약 동일 위치면 Special Pattern의 불량을 지운다.ㅋ
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

				// 비교하고 얻은 Pattern 이여야 한다..ㅋ
				if (X1DefectImageType != 6) continue;

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT		&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
					{
						j++; continue;
					}

					CPoint	X2St, X2End, X2Center;
					int		X2DefectImageType;

					X2St.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2End.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					// 기준 Pattern과 같으면 안됨 다른 Pattern의 결함
					if (X2DefectImageType == X1DefectImageType) { j++; continue; }

					// 주변에 Line이 검출 되는 경우
					if (abs(X1Center.y - X2Center.y) >= nOffSet)
					{
						// 리스트 작은거 삭제시
						if (i > j)	 i--;

						// 다른 Pattern의 모든 모든 모든 Line을 모두 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// y 방향
	//////////////////////////////////////////////////////////////////////////

	if (nYLineNumber > 1) // X Line이 있으면 일해야 됨
	{
		//비교함
		int PatternIndex;
		int	 YLine_G64 = 0;
		int YLine_SP2 = 0;

		// G64 Pattern에서 검출 된 Line Defect를 Count
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 명선이 제일 먼저 나온 Pattern을 기준 Pattern임
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
			{
				PatternIndex = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;
				if (PatternIndex == 8)
				{
					YLine_G64++;
				}
			}
		}

		// Special - 2 Pattern에서 검출 된 Line Defect를 Count
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 명선이 제일 먼저 나온 Pattern을 기준 Pattern임
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
			{
				PatternIndex = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;
				if (PatternIndex == 6)
				{
					YLine_SP2++;
				}
			}
		}

		//G64 Pattern을 기준으로 Special Pattern에서 검출된 Line Defect과 비교한다, 만약 동일 위치면 Special Pattern의 불량을 지운다.ㅋ
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
			{
				CPoint	Y1St, Y1End, Y1Center;
				int		Y1DefectImageType;

				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

				// 비교하고 얻은 Pattern 이여야 한다..ㅋ
				if (Y1DefectImageType != 6) continue;

				// 비교할 불량
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT		&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
					{
						j++; continue;
					}

					CPoint	Y2St, Y2End, Y2Center;
					int		Y2DefectImageType;

					Y2St.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2Center.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					// 기준 Pattern과 같으면 안됨 다른 Pattern의 결함
					if (Y2DefectImageType == Y1DefectImageType) { j++; continue; }

					// 주변에 Line이 검출 되는 경우
					if (abs(Y1Center.x - Y2Center.x) >= nOffSet)
					{
						// 리스트 작은거 삭제시
						if (i > j)	 i--;

						// 다른 Pattern의 모든 모든 모든 Line을 모두 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}
	}

	// Special Pattern에 있는 Line Defect들을 지운다 ㅠㅠ 잘 나오는데 못 쓴다니... ㅈㅈ
	for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT		&&
			resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK		&&
			resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT		&&
			resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
		{
			j++; continue;
		}

		int		X2DefectImageType;

		X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

		if (X2DefectImageType == 6)
		{
			resultPanelData.m_ListDefectInfo.RemoveAt(j);
		}
		else  j++;
	}

	return true;
}




//////////////////////////////////////////////////////////////////////////
// Align 후 ROI 밝기 이상 확인
//////////////////////////////////////////////////////////////////////////
long AviInspection::CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult,
	bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode)
{
	const int DUST_RETRY_LIMIT = 1;
	// test
	CCPUTimer tact;
	tact.Start();
	long nErrorCode = E_ERROR_CODE_TRUE;

	// 장비 타입
	int nEqpType = theApp.m_Config.GetEqpType();

	// 개별 알고리즘 검사 파라미터 가져오기
	// AD Parameter 별도로 정의할 것. ToDo.
	double* dAlgPara = theApp.GetFindDefectADParameter(nImageNum, nCameraNum);

	// 알고리즘 영상 번호
	int nAlgImg = theApp.GetImageClassify(nImageNum);

	// Alg 로그용
	wchar_t strAlgLog[MAX_PATH] = { 0, };
	swprintf(strAlgLog, _T("ID:%s\tPat:%s"), (LPCWSTR)strPanelID, theApp.GetGrabStepName(nImageNum));

	// Dust 패턴인 경우
	if (nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST)
	{
		if (!theApp.GetUseFindDefectAD(nImageNum, nCameraNum))
		{
			// Dust GV 검사 하지 않을 경우 경알람 발생
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Disabled Check DUST - InspSkip !!!. PanelID: %s, Stage: %02d, CAM: %02d"),
				strPanelID, nStageNo, nCameraNum);

			theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_DIABLE_CHECK_DUST, eALARMTYPE_HEAVY, bIsHeavyAlarm);

			// Seq. Task 에 Dust 종료 및 결과 송부
			bNeedRetry = false;
			bIsNormalDust = true;
			bChkDustEnd = true;
		}
		else
		{
			// AD - 밝기값 확인
			nErrorCode = Align_FindDefectAD_GV(MatOrgImage, dAlgPara, dMeanResult, ptCorner, nEqpType, nAlgImg, strAlgLog);

			if (nErrorCode != E_ERROR_CODE_TRUE)
			{
				bIsNormalDust = false;

				if (bUseDustRetry && nDustRetryCnt < DUST_RETRY_LIMIT)
				{
					// Seq. Task 에 Dust 종료 및 결과 송부
					bNeedRetry = true;
					// 알람 발생시키지 않고 검사 진행 및 보고 안함
					bIsHeavyAlarm = true;
				}
				else
				{
					bNeedRetry = false;
					// Dust 상태 이상 알람 발생
					theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_DUST_ABNORMAL, eALARMTYPE_HEAVY, bIsHeavyAlarm);
				}

				// Alg DLL Stop Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("DUST Abnormal - InspStop !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s..\n\t\t\t\t( Avg : %.2f GV )"),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0]);

				CString strOrgFileName = _T("");
				strOrgFileName.Format(_T("%s_CAM%02d_Abnormal_Dust"), theApp.GetGrabStepName(nImageNum), nCameraNum);
				strOrgFileName = strOrgFileName + _T(".bmp");


				ImageSave(MatOrgImage, _T("%s\\%s\\%02d_%s"),
					ORIGIN_PATH, strPanelID, nImageNum, strOrgFileName);
			}
			else
			{
				// Seq. Task 에 Dust 종료 및 결과 송부
				bNeedRetry = false;
				bIsNormalDust = true;
				// Alg DLL End Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("AD GV Algorithm End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Avg : %.2f GV )"),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0]);
			}
			bChkDustEnd = true;
		}
	}
	// Dust 패턴이 아닌 경우
	else
	{
		// AVI 는 Dust 조명 상태 체크 종료시까지 대기
		while (!bChkDustEnd)
		{
			Sleep(10);
		}

		if (bIsNormalDust)
		{
			// 점등영역 AD GV값 확인 코드
			if (theApp.GetUseFindDefectAD(nImageNum, nCameraNum))
			{
				//////////////////////////////////////////////////////////////////////////
				// KSW 17.11.23
				// 5.99" 공정문제 발생 - 특정 위치 라인불량 존재
				// 라인 불량으로 인해 AD판정 ( 라인불량이 밝아서 AD판정 ) -> 검사진행 안함 : E급 판정
				// 비검사 영역 제외 후, AD 검사 진행되도록 수정
				//////////////////////////////////////////////////////////////////////////

				// 사용 설정한 경우
				if (dAlgPara[E_PARA_CHECK_NON_DEL_FLAG] > 0)
				{
					cv::Mat matTempBuf = MatOrgImage.clone();

					// 비검사 영역 ROI 갯수 받아오기
					int nFilterROICnt = theApp.GetFilterROICnt(nImageNum, nCameraNum);

					// 비검사 영역 ROI 갯수 만큼
					for (int nROICnt = 0; nROICnt < nFilterROICnt; nROICnt++)
					{
						// 사용 여부 확인
						if (theApp.GetUseFilterROI(nImageNum, nCameraNum, nROICnt))
						{
							// 비검사 영역 ROI 가져오기
							// P/S 모드인 경우, 보정
							CRect rectFilterArea = theApp.GetFilterROI(nImageNum, nCameraNum, nROICnt, nRatio);

							// 비검사 영역 - Left-Top 좌표를 원점으로 좌표값을 가지고 있음
							rectFilterArea.OffsetRect(CPoint(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_TOP].y));

							// 조금 크게설정해서 사용 ( 파라미터화 필요 )
							int nOut = (int)dAlgPara[E_PARA_CHECK_NON_DEL_OFFSET];
							rectFilterArea.InflateRect(nOut, nOut, nOut, nOut);

							// 예외처리
							if (rectFilterArea.left < 0)	rectFilterArea.left = 0;
							if (rectFilterArea.top < 0)	rectFilterArea.top = 0;
							if (rectFilterArea.right >= matTempBuf.cols)	rectFilterArea.right = matTempBuf.cols - 1;
							if (rectFilterArea.bottom >= matTempBuf.rows)	rectFilterArea.bottom = matTempBuf.rows - 1;

							// ROI
							cv::Mat matZeroBuf = matTempBuf(cv::Rect(rectFilterArea.left, rectFilterArea.top, rectFilterArea.Width(), rectFilterArea.Height()));

							// ROI 0으로 초기화
							matZeroBuf.setTo(0);

							// 해제
							matZeroBuf.release();
						}
					}

					// 결과 영상 확인
					//cv::imwrite("D:\\asd.bmp", matTempBuf);

					// AD - 밝기값 확인
					nErrorCode = Align_FindDefectAD_GV(matTempBuf, dAlgPara, dMeanResult, ptCorner, nEqpType, nAlgImg, strAlgLog);

					matTempBuf.release();
				}
				// 사용 설정 안한 경우
				else
				{
					// AD - 밝기값 확인
					nErrorCode = Align_FindDefectAD_GV(MatOrgImage, dAlgPara, dMeanResult, ptCorner, nEqpType, nAlgImg, strAlgLog);
				}

				// 에러인 경우, 에러 코드 & 로그 출력
				if (nErrorCode != E_ERROR_CODE_TRUE)
				{
					// Alg DLL Stop Log
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("AD GV Defect - InspStop !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s..\n\t\t\t\t( Avg : %.2f GV / Label : %d )"),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0], (int)dMeanResult[2]);

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
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("AD GV Algorithm End Fail. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Avg : %.2f GV / Label : %d )"),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0], (int)dMeanResult[2]);
				}
				else
				{
					// Alg DLL End Log
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("AD GV Algorithm End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Avg : %.2f GV )"),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0]);
				}
			}
		}
		else
		{
			nErrorCode = E_ERROR_CODE_FALSE;
		}
	}
	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Check AD GV (Image : %s) : %.2f"), theApp.GetGrabStepName(nImageNum), tact.Stop(false) / 1000.);

	return nErrorCode;
}

// Resolution 계산
double AviInspection::calcResolution(CWriteResultInfo WrtResultInfo)
{
	// Panel Size
	double dPanelSizeX = WrtResultInfo.GetPanelSizeX();
	double dPanelSizeY = WrtResultInfo.GetPanelSizeY();

	// 점등된 가로 길이
	double dTempX, dTempY, dImageSizeX, dImageSizeY;
	dTempX = m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x - m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x;
	dTempY = m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y - m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y;
	dImageSizeX = sqrt(dTempX*dTempX + dTempY*dTempY);

	// 점등된 세로 길이
	dTempX = m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x - m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x;
	dTempY = m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y - m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y;
	dImageSizeY = sqrt(dTempX*dTempX + dTempY*dTempY);

	// 가로, 세로 Resolution
	double dResolutionX = dPanelSizeX / dImageSizeX;
	double dResolutionY = dPanelSizeY / dImageSizeY;

	// 평균 Resolution ( Non P/S Mode 인 경우의 Resolution )
	double dResolution = (dResolutionX + dResolutionY) / 2.0 * m_stThrdAlignInfo.nRatio;

	//CString strTemp;
	//strTemp.Format(_T("%f, %f\n%f, %f\n%f, %f, %f"), dPanelSizeX, dPanelSizeY, dImageSizeX, dImageSizeY, dResolutionX, dResolutionY, dResolution);
	//AfxMessageBox(strTemp);

	return dResolution;
}

bool AviInspection::JudgementRepeatCount(CString strPanelID, ResultPanelData& resultPanelData)
{
	// 스레드 중복 접근 방지
	// 현재 결과 나오는 순서대로 반복 횟수 체크 - 패널 진행 순서 고려 안함
	EnterCriticalSection(&theApp.m_csJudgeRepeatCount);

	ListCurDefect* pStCurDefList = new ListCurDefect();
	std::list<RepeatDefectInfo>* pList = theApp.GetRepeatDefectInfo();					// 기존 불량 리스트(Pixel 좌표)
	static bool bIsAlarm = false;														// 기존 알람 발생 여부

	CCPUTimer ttRepeatCount;
	ttRepeatCount.Start();

	memcpy(pStCurDefList->bUseChkRptDefect, theApp.m_Config.GetUseCCDAlarm(), sizeof(pStCurDefList->bUseChkRptDefect));

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (0) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	// Pixel / Work 좌표 체크 둘 다 사용하지 않을 경우 기존 List Clear하고 정상 Return
	if (!pStCurDefList->bUseChkRptDefect[ePIXEL] && !pStCurDefList->bUseChkRptDefect[eWORK])
	{
		// 기존 진행 이력 있으면 삭제
		for (int nKind = 0; nKind < eMACHINE; nKind++)			// 2018. 09. 21 MDJ APP Repeat Defect 추가로 인하여 종류보다 1개 적도록 변경
		{
			if (pList[nKind].size() != 0)
				pList[nKind].clear();
		}

		m_fnSaveRepeatDefInfo(pList);
		bIsAlarm = false;
		LeaveCriticalSection(&theApp.m_csJudgeRepeatCount);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Skip Repeat Defect !!! - Not Used"));
		return true;
	}

	const int DIGIT_PNLID = 17;		// Panel ID 자릿수 설정
	CString strOrgPanelID = strPanelID;
	static CString strOldPanelID = _T("");

	// [PANELID]_1, [PANELID]_2 ... 등 Panel ID 가 실제 자릿수보다 클 경우 Panel ID 만큼만 잘라서 비교
	if (strOrgPanelID.GetLength() > DIGIT_PNLID)
		strOrgPanelID = strPanelID.Left(DIGIT_PNLID);

	// 알람 발생 안한 상태에서 동일한 Panel 연속 진행 시 체크 안함
	// 이미 알람이 발생한 상태면 동일한 Panel ID 재진행하더라도 원인이 되는 현상 해결되기 전까지 지속적으로 알람 발생
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
	// 현재 반복 불량 체크 최대 개수 1000개로 제한
	// 최대 개수 증가 필요할 경우 속도 개선 고려
	const int MAX_REPEAT_DEFECT = 1000;
	// [0] : CCD / [1] : 공정(Work좌표) 불량
	int* nRptOffset = theApp.m_Config.GetCCDOffset();
	int* nRptLArmCnt = theApp.m_Config.GetCCDLightAlarmCount();
	int* nRptHArmCnt = theApp.m_Config.GetCCDHeavyAlarmCount();

	int nTotalCount = 0;
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// 여기에 체크 필요한 항목 추가 (Pixel / Work)
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// Point Dark / Bright에 속하는 경우만 비교 list 만듦
			// Point Dark일 경우
		case E_DEFECT_JUDGEMENT_POINT_DARK:
		case E_DEFECT_JUDGEMENT_POINT_GROUP_DARK:
			// Pixel, Work 좌표 추가 - 비교 필요한 항목만 추가할 것
			pStCurDefList->Add_Tail(ePIXEL, E_DEFECT_JUDGEMENT_POINT_DARK, &resultPanelData.m_ListDefectInfo[i]);
			pStCurDefList->Add_Tail(eWORK, E_DEFECT_JUDGEMENT_POINT_DARK, &resultPanelData.m_ListDefectInfo[i]);
			nTotalCount++;
			break;

			// Point Bright일 경우
		case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
		case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
		case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:
			pStCurDefList->Add_Tail(ePIXEL, E_DEFECT_JUDGEMENT_POINT_BRIGHT, &resultPanelData.m_ListDefectInfo[i]);
			pStCurDefList->Add_Tail(eWORK, E_DEFECT_JUDGEMENT_POINT_BRIGHT, &resultPanelData.m_ListDefectInfo[i]);
			nTotalCount++;
			break;

		default:
			break;
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// 현재 불량 너무 많을 경우 Skip -
		if (nTotalCount > MAX_REPEAT_DEFECT)
		{
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Skip Repeat Defect !!! - Too Many Defect (Repeat Defect Count : Over %d)"), MAX_REPEAT_DEFECT);
			LeaveCriticalSection(&theApp.m_csJudgeRepeatCount);
			return false;
		}
	}

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (1) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	// 기존 불량 리스트와 현재 불량 리스트 비교
	// 겹치지 않는 불량 삭제
	// 겹치는 불량 평균값 취득 및 카운트 증가	
	int nMaxRepeatCount[eMACHINE] = { 0, };					// 2018. 09. 21 MDJ APP Repeat Defect 추가로 인하여 종류보다 1개 적도록 변경

#pragma omp parallel for schedule(dynamic)		// 병렬처리 추가
	for (int nKind = 0; nKind < eMACHINE; nKind++)			// 2018. 09. 21 MDJ APP Repeat Defect 추가로 인하여 종류보다 1개 적도록 변경
	{
		std::list<RepeatDefectInfo>::iterator iterDst, iterSrc;
		bool bRepeat = false;
		// 기존 불량 리스트
		for (iterDst = pList[nKind].begin(); iterDst != pList[nKind].end(); )
		{
			bRepeat = false;

			// 기존 불량 위치 Area 생성
			// CCD 불량 - 단위 Pixel / 공정 불량 - 단위 um
			CRect rcDst(iterDst->ptCenterPos.x - nRptOffset[nKind] - 1, iterDst->ptCenterPos.y - nRptOffset[nKind] - 1,			// LT
				iterDst->ptCenterPos.x + nRptOffset[nKind] + 1, iterDst->ptCenterPos.y + nRptOffset[nKind] + 1);					// RB

			// 현재 불량 리스트
			for (iterSrc = pStCurDefList->listCurDefInfo[nKind].begin(); iterSrc != pStCurDefList->listCurDefInfo[nKind].end(); )
			{
				if (iterDst->eDefType == iterSrc->eDefType)
				{
					// 반복된 위치에 새로운 불량이 있을 경우
					if (PtInRect(rcDst, iterSrc->ptCenterPos))
					{
						bRepeat = true;

						// 중심좌표 평균 취득
						iterDst->ptCenterPos += iterSrc->ptCenterPos;
						iterDst->ptCenterPos.x /= 2;
						iterDst->ptCenterPos.y /= 2;
						iterDst->nRepeatCount++;
						// 카운트 증가 후 최대 반복 발생 횟수 갱신
						if (iterDst->nRepeatCount >= nRptLArmCnt[nKind])
						{
							nMaxRepeatCount[nKind] = MAX(nMaxRepeatCount[nKind], iterDst->nRepeatCount);		// 최대 반복 발생 횟수

							// Log 용 문자열
							CString strErrMsg = _T("");
							CString strErrType = _T("");
							CString strDefType = _T("");

							if (iterDst->nRepeatCount >= nRptHArmCnt[nKind])		strErrMsg = _T("Error");
							else												strErrMsg = _T("Warning");

							if (nKind == ePIXEL)	strErrType = _T("Pixel");
							else					strErrType = _T("Work");

							switch (iterDst->eDefType)
							{
							case E_DEFECT_JUDGEMENT_POINT_DARK:
								strDefType = _T("POINT_DARK");
								break;
							case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
								strDefType = _T("POINT_BRIGHT");
								break;
							default:
								strDefType.Format(_T("%d"), (int)iterDst->eDefType);
								break;
							}
							theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("%s !! Repeat Defect(%s) %d Times !!! (DefCode <%s> : %d, %d)"),
								strErrMsg,
								strErrType,
								iterDst->nRepeatCount,
								strDefType,
								iterDst->ptCenterPos.x, iterDst->ptCenterPos.y);
						}
						// 기존 리스트 평균값 취득 및 카운트 증가 후 현재 불량 삭제
						iterSrc = pStCurDefList->listCurDefInfo[nKind].erase(iterSrc);
						// 중복 불량 삭제 후 이 후 불량 신경 안씀
						// (새로운 불량 리스트에 Offset 이내로 서로 겹치는 불량이 없다고 가정)
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

			// 하나도 겹치는게 없으면 기존 리스트의 불량 삭제
			if (!bRepeat)
				iterDst = pList[nKind].erase(iterDst);
			else
				iterDst++;
		}
	}

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (2) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	// 동일 위치 반복 불량 중 중복 횟수가 가장 많은 불량이 설정한 경/중알람 개수보다 많을 경우 -> 경알람 사용 안함
	for (int nKind = 0; nKind < eMACHINE; nKind++)			// 2018. 09. 21 MDJ APP Repeat Defect 추가로 인하여 종류보다 1개 적도록 변경
	{
		if (pStCurDefList->bUseChkRptDefect[nKind])
		{
			if (nMaxRepeatCount[nKind] >= nRptHArmCnt[nKind])
			{
				bIsAlarm = true;
				// nKind 0 : CCD 중알람 (3000) / nKind 1 : 공정불량 중알람 (3001)
				theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eAutoRun, eALARMID_CCD_DEFECT_ERROR + nKind, eALARMTYPE_HEAVY);
			}
			else
			{
				bIsAlarm = false;
			}

			// 겹치는 항목만 남은 기존 리스트에 새로 발생한 불량 추가
			// merge하기 전에 정렬하는 구문 추가
			pList[nKind].sort();
			pStCurDefList->listCurDefInfo[nKind].sort();
			pList[nKind].merge(pStCurDefList->listCurDefInfo[nKind]);
		}
	}

	// 새로운 리스트 파일로 저장
	bRet = m_fnSaveRepeatDefInfo(pList);

	SAFE_DELETE(pStCurDefList);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (3) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	LeaveCriticalSection(&theApp.m_csJudgeRepeatCount);

	return bRet;
}

bool AviInspection::m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo)
{
	// List 없을 경우
	if (pListRepeatInfo == NULL)			return false;

	// TXT 저장
	std::list<RepeatDefectInfo>::iterator iterList;
	CStdioFile	fileWriter;
	CString		strRepeatInfoHeader, strRepeatInfo;
	BOOL bRet = FALSE;

	// 기능 사용하지 않을 때는 자동으로 Clear
	for (int nKind = 0; nKind < eMACHINE; nKind++)			// 2018. 09. 21 MDJ APP Repeat Defect 추가로 인하여 종류보다 1개 적도록 변경
	{
		// 파일 열기
		switch (nKind)
		{
		case ePIXEL:
			bRet = fileWriter.Open(REPEAT_DEFECT_PIXEL_INFO_PATH, CFile::modeCreate | CFile::modeWrite);
			break;
		case eWORK:
			bRet = fileWriter.Open(REPEAT_DEFECT_WORK_INFO_PATH, CFile::modeCreate | CFile::modeWrite);
			break;
		default:
			bRet = FALSE;
			break;
		}

		if (bRet)
		{
			// 헤더 정보 - for GUI
			strRepeatInfoHeader = _T("Type,DefectX,DefectY,RepeatCount\r\n");
			fileWriter.WriteString(strRepeatInfoHeader);
			// 중복 좌표 개수 만큼
			for (iterList = pListRepeatInfo[nKind].begin(); iterList != pListRepeatInfo[nKind].end(); ++iterList)
			{
				// 동일 위치 좌표 정보
				strRepeatInfo.Format(_T("%d,%d,%d,%d\r\n"), (int)iterList->eDefType,
					iterList->ptCenterPos.x, iterList->ptCenterPos.y, iterList->nRepeatCount);

				fileWriter.SeekToEnd();
				fileWriter.WriteString(strRepeatInfo);
			}

			// 파일 열린 경우만 닫기
			fileWriter.Close();
		}
	}

	return true;
}

bool AviInspection::NewMaxGVMethold(Mat matSrcImage, double OldMaxGV, double& NewMaxGV, int nTopCountGV)
{
	// Histogram Calculation

	cv::Mat matHisto;
	int nHistSize = 256;

	float fHistRange[] = { (float)0, (float)nHistSize - 1 };
	const float *ranges[] = { fHistRange };

	cv::calcHist(&matSrcImage, 1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	float *pVal = (float*)matHisto.data;

	// Diff x GV Calculation
	__int64 nPixelSum = 0;
	__int64 nPixelCount = 0;

	//int nTopCountGV		= 3;
	int nCountNumber = 0;
	int nCountGV = 0;

	pVal = (float*)matHisto.ptr(0) + (int)OldMaxGV;

	for (int m = (int)OldMaxGV; m >= 0; m--, pVal--)
	{
		nPixelSum = (__int64)(m * *pVal);

		if (nPixelSum != 0)
		{
			nCountGV += m;
			nCountNumber++;
			if (nCountNumber == nTopCountGV) break;
			else continue;
		}

		else continue;
	}

	NewMaxGV = (double)nCountGV / nCountNumber;

	return true;
}


bool 	AviInspection::GetModelNorchInfo(ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], vector<int>& NorchIndex, CPoint& OrgIndex)
{
	if (tRoundSet == NULL) return false;

	// ROI 개수 확인
	int ROICount = 0;

	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		if (tRoundSet[i].nContourCount != 0)
			ROICount++;
	}

	if (ROICount <= 4) return false;

	int TopDataLenght = tRoundSet[6].nContourCount;
	int BottomDataLenght = tRoundSet[5].nContourCount;

	if (TopDataLenght == 0 && BottomDataLenght == 0) return false;

	// 원점 Index
	int ValueBuff1 = m_stThrdAlignInfo.rcAlignCellROI.x;
	int ValueBuff2 = m_stThrdAlignInfo.rcAlignCellROI.y;

	int ValueBuff3 = tRoundSet[6].ptContours[TopDataLenght - 1].x;
	int ValueBuff4 = tRoundSet[6].ptContours[0].y;
	int ValueBuff5 = tRoundSet[5].ptContours[0].x;
	int ValueBuff6 = tRoundSet[5].ptContours[BottomDataLenght - 1].y;

	NorchIndex.push_back(ValueBuff3); // Top Length
	NorchIndex.push_back(ValueBuff4); // Top Height
	NorchIndex.push_back(ValueBuff5); // Bottom Length
	NorchIndex.push_back(abs(ValueBuff6)); // Bottom Height

	OrgIndex.x = ValueBuff4;
	OrgIndex.y = ValueBuff6 + m_stThrdAlignInfo.rcAlignCellROI.height;

	return true;
}

bool	AviInspection::JudgeCHoleJudgment(ResultPanelData& resultPanelData, tCHoleAlignInfo tCHoleAlignData, double* dAlignPara)
{
	//////////////////////////////////////////////////////////////////////////
	// Parameter setting
	//////////////////////////////////////////////////////////////////////////

	// 스펙 받아오기
	int nDelete_Defect_Area;			// 불량 크기
	int nDelete_Defect_Offset;			// CHole Offset

										//////////////////////////////////////////////////////////////////////////

										// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nCHoleADCnt = 0;

	// CHole AD 불량 유/무 체크
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
		if (nDefect_Type == E_DEFECT_JUDGEMENT_DISPLAY_CHOLE_ABNORMAL) nCHoleADCnt++;
	}

	// CHole AD 판정
	if (nCHoleADCnt > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefect1_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
			int nDefect1_ImgNo = resultPanelData.m_ListDefectInfo[i].Img_Number;

			if (nDefect1_Type != E_DEFECT_JUDGEMENT_DISPLAY_CHOLE_ABNORMAL) continue;

			// CHole 영역
			CRect rectCHole_Rect;
			rectCHole_Rect.left = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
			rectCHole_Rect.top = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
			rectCHole_Rect.right = resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
			rectCHole_Rect.bottom = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;

			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
			{
				//같은 불량 일때 넘어가기
				if (i == j) continue;

				int nDefect2_Type = resultPanelData.m_ListDefectInfo[j].Defect_Type;
				int nDefect2_ImgNo = resultPanelData.m_ListDefectInfo[j].Img_Number;

				// CHole AD 일 때 넘어가기
				if (nDefect2_Type == E_DEFECT_JUDGEMENT_DISPLAY_CHOLE_ABNORMAL) continue;

				CRect rectDefect_Rect;
				rectDefect_Rect.left = resultPanelData.m_ListDefectInfo[j].Pixel_Start_X;
				rectDefect_Rect.top = resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y;
				rectDefect_Rect.right = resultPanelData.m_ListDefectInfo[j].Pixel_End_X;
				rectDefect_Rect.bottom = resultPanelData.m_ListDefectInfo[j].Pixel_End_Y;

				CRect aatest;

				if (nDefect1_ImgNo == nDefect2_ImgNo)
				{
					if (aatest.IntersectRect(rectCHole_Rect, rectDefect_Rect))
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
						if (i > j) i--;
						j--;
					}
				}
			}
		}
	}

	// CHole 2차 판정
	else
	{
		int nDefect_Cnt = 0;		// Point 불량
		int nDefect2_Cnt = 0;		// Line &  Type2 Y 불량

									//int nDefect3_Cnt = 0;			// 임시 Type2 Y방향

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			switch (nDefect_Type)
			{
				// 암점
			case E_DEFECT_JUDGEMENT_POINT_DARK:
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
			case E_DEFECT_JUDGEMENT_POINT_GROUP_DARK:
				nDefect_Cnt++;
				break;

				// Weak Line & Type2
			case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
			case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:

			case E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y:
				nDefect2_Cnt++;
				break;


				//nDefect3_Cnt++;
				//break;

			default:
				break;
			}
		}

		if (nDefect_Cnt > 0) // 암점 제거용
		{
			nDelete_Defect_Area = (int)dAlignPara[E_PARA_CHOLE_SMALL_DEFECT_SIZE];
			nDelete_Defect_Offset = (int)dAlignPara[E_PARA_CHOLE_SMALL_DEFECT_OFFSET];

			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
				int nDefect_ImgNo = resultPanelData.m_ListDefectInfo[i].Img_Number;

				if (nDefect_Type != E_DEFECT_JUDGEMENT_POINT_DARK		&&
					nDefect_Type != E_DEFECT_JUDGEMENT_POINT_DARK_SP_1	&&
					nDefect_Type != E_DEFECT_JUDGEMENT_POINT_DARK_SP_2	&&
					nDefect_Type != E_DEFECT_JUDGEMENT_POINT_DARK_SP_3	&&
					nDefect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_DARK) continue;

				// 암점 스펙 확인
				// 암점 크기
				int nDefect_Width = resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
				int nDefect_Height = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;

				// 작은 불량만 검사
				if (nDefect_Width * nDefect_Height > nDelete_Defect_Area) continue;

				// 암점 중심 좌표
				int nDefect_CenX = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nDefect_CenY = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				// Offset
				cv::Rect rectDefect_Center;
				rectDefect_Center.x = nDefect_CenX - nDelete_Defect_Offset;
				rectDefect_Center.y = nDefect_CenY - nDelete_Defect_Offset;
				rectDefect_Center.width = 1 + nDelete_Defect_Offset * 2;
				rectDefect_Center.height = 1 + nDelete_Defect_Offset * 2;

				for (int j = 0; j < MAX_MEM_SIZE_E_INSPECT_AREA; j++)
				{
					if (tCHoleAlignData.rcCHoleROI[theApp.GetImageClassify(nDefect_ImgNo)][j].empty()) continue;
					if (tCHoleAlignData.matCHoleROIBuf[theApp.GetImageClassify(nDefect_ImgNo)][j].empty()) continue;

					cv::Mat matCHoleCoordBuf = tCHoleAlignData.matCHoleROIBuf[theApp.GetImageClassify(nDefect_ImgNo)][j];

					rectDefect_Center.x -= tCHoleAlignData.rcCHoleROI[theApp.GetImageClassify(nDefect_ImgNo)][j].x;
					rectDefect_Center.y -= tCHoleAlignData.rcCHoleROI[theApp.GetImageClassify(nDefect_ImgNo)][j].y;

					//예외처리
					if (rectDefect_Center.x < 0) continue;
					if (rectDefect_Center.y < 0) continue;
					if (rectDefect_Center.x >= matCHoleCoordBuf.cols - rectDefect_Center.width)		continue;
					if (rectDefect_Center.y >= matCHoleCoordBuf.rows - rectDefect_Center.height)	continue;

					cv::Scalar scrMean = cv::mean(matCHoleCoordBuf(rectDefect_Center));

					if (scrMean[0] > 0)
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(i--);
						break;
					}
				}
			}
		}

		if (nDefect2_Cnt > 0) // Type2 & Line 과검 제거용
		{
			nDelete_Defect_Offset = (int)dAlignPara[E_PARA_CHOLE_BIG_DEFECT_OFFSET];
			bool bDelLineFlag = ((int)dAlignPara[E_PARA_CHOLE_LINE_DELETE_FLAG] > 0) ? 1 : 0;

			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
				int nDefect_ImgNo = resultPanelData.m_ListDefectInfo[i].Img_Number;

				if (nDefect_Type != E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y		&&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT	&&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT	&&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK	&&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK) continue;

				if ((nDefect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
					nDefect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
					nDefect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
					nDefect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK) && bDelLineFlag)
				{

					int nDefect_Start_X = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					int nDefect_Start_Y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					int nDefect_End_X = resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
					int nDefect_End_Y = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;

					int nDefect_Center_X = resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
					int nDefect_Center_Y = resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

					for (int j = 0; j < MAX_MEM_SIZE_E_INSPECT_AREA; j++)
					{
						if (tCHoleAlignData.rcCHoleROI[theApp.GetImageClassify(nDefect_ImgNo)][j].empty()) continue;

						cv::Rect rectCHoleBuf = tCHoleAlignData.rcCHoleROI[theApp.GetImageClassify(nDefect_ImgNo)][j];

						int nCHole_Start_X = rectCHoleBuf.x;
						int nCHole_Start_Y = rectCHoleBuf.y;
						int nCHole_End_X = rectCHoleBuf.width + rectCHoleBuf.x;
						int nCHole_End_Y = rectCHoleBuf.height + rectCHoleBuf.y;

						if (nDefect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
							nDefect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
						{
							if ((nDefect_Center_Y > (nCHole_Start_Y - nDelete_Defect_Offset) && nDefect_Center_Y < (nCHole_Start_Y + nDelete_Defect_Offset)) ||
								(nDefect_Center_Y > (nCHole_End_Y - nDelete_Defect_Offset) && nDefect_Center_Y < (nCHole_End_Y + nDelete_Defect_Offset)))
							{
								resultPanelData.m_ListDefectInfo.RemoveAt(i--);
								break;
							}
						}

						else if (nDefect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
							nDefect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
						{
							if ((nDefect_Center_X > (nCHole_Start_X - nDelete_Defect_Offset) && nDefect_Center_X < (nCHole_Start_X + nDelete_Defect_Offset)) ||
								(nDefect_Center_X > (nCHole_End_X - nDelete_Defect_Offset) && nDefect_Center_X < (nCHole_End_X + nDelete_Defect_Offset)))
							{
								resultPanelData.m_ListDefectInfo.RemoveAt(i--);
								break;
							}
						}
					}
				}
				// Type2 위치 및 크기
				else if (nDefect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y)
				{
					int nDefect_Start_X = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					int nDefect_Start_Y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					int nDefect_End_X = resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
					int nDefect_End_Y = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;
					int nDefect_Width = nDefect_End_X - nDefect_End_X;
					int nDefect_Height = nDefect_End_Y - nDefect_Start_Y;

					int nDefect_Area = nDefect_Width * nDefect_Height;

					for (int j = 0; j < MAX_MEM_SIZE_E_INSPECT_AREA; j++)
					{
						if (tCHoleAlignData.rcCHoleROI[theApp.GetImageClassify(nDefect_ImgNo)][j].empty()) continue;

						cv::Rect rectCHoleBuf = tCHoleAlignData.rcCHoleROI[theApp.GetImageClassify(nDefect_ImgNo)][j];

						int nCHole_Start_X = rectCHoleBuf.x - nDelete_Defect_Offset;
						int nCHole_Start_Y = rectCHoleBuf.y - nDelete_Defect_Offset;
						int nCHole_End_X = rectCHoleBuf.width + rectCHoleBuf.x + nDelete_Defect_Offset;
						int nCHole_End_Y = rectCHoleBuf.height + rectCHoleBuf.y + nDelete_Defect_Offset;

						// CHole 주변만
						if (nDefect_Start_X > nCHole_End_X)		continue;
						if (nDefect_End_X < nCHole_Start_X)		continue;
						if (nDefect_Start_Y > nCHole_End_Y)		continue;
						if (nDefect_End_Y < nCHole_Start_Y)		continue;

						// CHole Area 비교
						int nOverLap_Start_X = (nDefect_Start_X > nCHole_Start_X) ? nDefect_Start_X : nCHole_Start_X;
						int nOverLap_Start_Y = (nDefect_Start_Y > nCHole_Start_Y) ? nDefect_Start_Y : nCHole_Start_Y;
						int nOverLap_End_X = (nDefect_End_X > nCHole_End_X) ? nCHole_End_X : nDefect_End_X;
						int nOverLap_End_Y = (nDefect_End_Y > nCHole_End_Y) ? nCHole_End_Y : nDefect_End_Y;
						int nOverLap_Width = nOverLap_End_X - nOverLap_Start_X;
						int nOverLap_Height = nOverLap_End_Y - nOverLap_Start_Y;
						int nOverLap_Area = nOverLap_Width * nOverLap_Height;

						if (nOverLap_Area >= (nDefect_Area - nOverLap_Area))
						{
							resultPanelData.m_ListDefectInfo.RemoveAt(i--);
							break;
						}
					}
				}
			}
		}
	}

	return true;
}

// long AviInspection::JudgeSubStanceSpot(cv::Mat& matSrcBuffer, CRect rectROI, double* dPara, int* nCommonPara,
// 	stDefectInfo* pResultBlob)
// {
// 	// 불량이 없는 경우 나가기
// 	int nCount = pResultBlob->nDefectCount;
// 	if (nCount <= 0)		return E_ERROR_CODE_TRUE;
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// 공통 파라미터
// 	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
// 	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
// 	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
// 	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
// 	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
// 	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
// 	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
// 	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];
// 
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	long	nWidth = (long)matSrcBuffer.cols;	// 영상 가로 크기
// 	long	nHeight = (long)matSrcBuffer.rows;	// 영상 세로 크기
// 
// 	int nMorpObj = 0;
// 	int nOffset = 0;
// 	int nOffset_Defect = 0;
// 
// 
// 	
// 	cv::Mat matActMask;
// 	m_stThrdAlignInfo.matMask_Active.copyTo(matActMask);
// 
// 	cv::Scalar meanRoi, stdRoi;
// 	cv::meanStdDev(matSrcBuffer, meanRoi, stdRoi, matActMask);
// 
// 	cv::Mat matSrcRoI;
// 	cv::Mat matActMask_Temp;
// 
// 	cv::Mat matActMask_Gv = cv::Mat::zeros(matSrcBuffer.size(), matSrcBuffer.type());
// 	matActMask_Gv.setTo(meanRoi[0]);
// 
// 	cv::distanceTransform(matActMask, matActMask_Temp, CV_DIST_L2, 3);
// 
// 	cv::threshold(matActMask_Temp, matActMask_Temp, 50, 255, CV_THRESH_BINARY);
// 	matActMask_Temp.convertTo(matActMask_Temp, CV_8UC1);
// 
// 
// 	cv::bitwise_not(matActMask_Temp, matActMask_Temp);
// 	matSrcBuffer.copyTo(matSrcRoI);
// 	cv::subtract(matSrcRoI, matActMask_Temp, matSrcRoI);
// 
// 	cv::bitwise_and(matActMask_Gv, matActMask_Temp, matActMask_Gv);
// 	cv::add(matSrcRoI, matActMask_Gv, matSrcRoI);
// 
// #ifdef _DEBUG
// #else
// #pragma omp parallel for
// #endif
// 	// 불량 개수 만큼
// 	for (int i = 0; i < nCount; i++)
// 	{
// 		if( pResultBlob->bUseResult[i] == false)
// 			continue;
// 		// Spot 불량이 아닌 경우 넘어가기
// 		if (pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_POINT_DARK ) //04.16 choi
// 			continue;
// 
// 		cv::Mat matDefectMorp1Buf;
// 		//cv::Mat matDefectMorp2Buf;
// 		cv::Mat matDefectBKBuf;
// 		//cv::Mat matDefectThBuf;
// 	
// 
// 		//int nOffset =   pResultBlob->nArea[i] + 50; //old
// 		int nOffset = pResultBlob->nArea[i] + 15; //old
// 
// 		if (nOffset % 2 == 0) nOffset += 1;
// 		int nOffset_Defect_W = (pResultBlob->ptRB[i].x - pResultBlob->ptLT[i].x)+1;
// 		int nOffset_Defect_H = (pResultBlob->ptRB[i].y - pResultBlob->ptLT[i].y)+1;
// 
// // 		int nOffset_Defect_W = 5;
// // 		int nOffset_Defect_H = 5;
// 
// 		int nSegX = 10;
// 		int nSegY = 10;
// 
// 		int nRangeX = 0;
// 		int nRangeY= 0;
// 
// 		vector<int> vMedian;
// 
// 		int nSum = 0;
// 		int nThres = 0;
// 
// 		// 불량 주변 영역
// 		cv::Rect rectTempROI(pResultBlob->nCenterx[i] - nOffset , pResultBlob->nCentery[i] - nOffset  , nOffset*2, nOffset*2);
// 		if (rectTempROI.x < 0)						rectTempROI.x = 0;
// 		if (rectTempROI.y < 0)						rectTempROI.y = 0;
// 		if (rectTempROI.x + nOffset >= nWidth)		rectTempROI.x = nWidth - nOffset - 1;
// 		if (rectTempROI.y + nOffset >= nHeight)	rectTempROI.y = nHeight - nOffset - 1;
// 
// 		nRangeX = rectTempROI.width / nSegX;
// 		nRangeY = rectTempROI.height / nSegY;
// 
// 		cv::Rect rectDefectTempROI(pResultBlob->ptLT[i].x - rectTempROI.x  , pResultBlob->ptLT[i].y - rectTempROI.y, nOffset_Defect_W, nOffset_Defect_H);
// 		if (rectDefectTempROI.x < 0)						rectDefectTempROI.x = 0;
// 		if (rectDefectTempROI.y < 0)						rectDefectTempROI.y = 0;
// 		if (rectDefectTempROI.x + nOffset_Defect >= nWidth)		rectDefectTempROI.x = nWidth - nOffset_Defect_W - 1;
// 		if (rectDefectTempROI.y + nOffset_Defect >= nHeight)	rectDefectTempROI.y = nHeight - nOffset_Defect_H - 1;
// 
// 		// 불량 영역
// 	
// 
// 
// 		cv::Mat matDefectSrcBuf = matSrcRoI(rectTempROI);
// 		
// 		cv::Mat matDefectCenter = cv::Mat::zeros(matDefectSrcBuf.size(), matDefectSrcBuf.type());
// 		matDefectCenter(rectDefectTempROI).setTo(255);
// 
// 
// 		for (int y = 0; y < nSegY; y++) {
// 			for (int x = 0; x < nSegX; x++) {
// 				int nPt_X = x*nRangeX;
// 				int nPt_Y = y*nRangeY;
// 	
// 
// 				cv::Rect rectMedianROI(nPt_X, nPt_Y, nRangeX, nRangeY);
// 				
// 				cv::Scalar meanORI, stdORI;
// 				cv::meanStdDev(matDefectSrcBuf(rectMedianROI), meanORI, stdORI);
// 
// 				vMedian.push_back(meanORI[0]);
// 			}
// 		}
// 
// 		sort(vMedian.begin(), vMedian.end());
// 
// 		for (int i = 20; i < vMedian.size()-20; i++) {
// 			nSum += vMedian[i];
// 		}
// 
// 		nThres = (int)nSum / (vMedian.size()-40);
// 
// 		int nBack_Thres = nThres *0.9;
// 		//nThres *= 0.75; old
// 		nThres *= 0.9;
// 		
// 
// 		cv::Mat matDefectResBuf;
// 		cv::Mat matDefectResBuf_Temp;
// 		cv::Mat matDefectResBuf_Back;
// 
// 		cv::threshold(matDefectSrcBuf, matDefectResBuf, nThres, 255.0, CV_THRESH_BINARY_INV);
// 		cv::threshold(matDefectSrcBuf, matDefectResBuf_Back, nBack_Thres, 255.0, CV_THRESH_BINARY_INV);
// 		matDefectResBuf.copyTo(matDefectResBuf_Temp);
// 
// 		cv::bitwise_and(matDefectResBuf, matDefectCenter, matDefectResBuf);
// 
// 		double valMin, valMax;
// 		cv::minMaxLoc(matDefectResBuf, &valMin, &valMax, 0, 0);
// 		
// 		if (valMax <= 0) {
// 			//pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK_SP_1;
// 			//pResultBlob->bUseResult[i] = false;
// 			continue;
// 		}
// 
// 		// 불량 조금 크게 설정
// 		int nValue = nMorpObj * 2 + 1;
// 		if (nMorpObj >= 1)
// 			cv::morphologyEx(matDefectResBuf, matDefectMorp1Buf, MORPH_DILATE, cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(nValue, nValue)));
// 		else
// 			matDefectResBuf.copyTo(matDefectMorp1Buf);
// 
// 		// 배경 부분
// 		cv::bitwise_not(matDefectMorp1Buf, matDefectBKBuf);
// 
// 		cv::Mat matBG_Defect;
// 		int nBG_Defect_Size = 0;
// 
// 		cv::bitwise_and(matDefectResBuf_Back, matDefectBKBuf, matBG_Defect);
// 		for (int i = 0; i < matBG_Defect.rows * matBG_Defect.cols; i++) {
// 			if (matBG_Defect.data[i] == 255) { nBG_Defect_Size++; }
// 		}
// 		
// 		cv::Mat mat_Defect;
// 		int nDefect_Size = 0;
// 
// 		cv::bitwise_and(matDefectResBuf_Temp, matDefectMorp1Buf, mat_Defect);
// 		for (int i = 0; i < mat_Defect.rows * mat_Defect.cols; i++) {
// 			if (mat_Defect.data[i] == 255) { nDefect_Size++; }
// 		}
// 
// 		cv::Scalar meanBK, stdBK;
// 		cv::meanStdDev(matDefectSrcBuf, meanBK, stdBK, matDefectBKBuf);
// 
// 		cv::Scalar meanORJ, stdORJ;
// 		cv::meanStdDev(matDefectSrcBuf, meanORJ, stdORJ, matDefectMorp1Buf);
// 
// 
// 		pResultBlob->dMuraObj[i] = meanORJ[0];
// 		pResultBlob->dMuraBk[i] = meanBK[0];
// 
// 		// 무라 정보
// 		double	dArea = pResultBlob->nArea[i];
// 		double	dSub = meanORJ[0] - meanBK[0];
// 		double	dDiff = meanORJ[0] / meanBK[0];
// 
// 		// Spec1
// 		if (nDefect_Size >= 4 &&
// 			nDefect_Size < 20 &&
//  			dSub < -20 &&
//  			dDiff < 0.85)
// 		{
// 			if (nBG_Defect_Size < 10) {
// 				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK;
// 			}
// 			else {
// 				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK;
// 			}
// 		}
// 		else {
// 			if (nBG_Defect_Size < 30) {
// 				//pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK_SP_2;
// 				//pResultBlob->bUseResult[i] = false;
// 				
// 			}
// 			else {
// 				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK_SP_3;
// 				//pResultBlob->bUseResult[i] = false;
// 				
// 			}
// 		}
// 	}
// // 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// // 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// // 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// // 	}
// 
// 	return E_ERROR_CODE_TRUE;
// }

long AviInspection::JudgeSubStanceSpot(cv::Mat& matSrcBuffer, CRect rectROI, double* dPara, int* nCommonPara,
	stDefectInfo* pResultBlob, stDefectInfo_Substance* pResultBlob_Sub)
{
	// 불량이 없는 경우 나가기
	int nCount = pResultBlob->nDefectCount;
	if (nCount <= 0)		return E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	//////////////////////////////////////////////////////////////////////////
	//Align Para
	int nCrop_Offset = (int)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_CROP_OFFSET]; //15
	int nSegX = (int)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_SEG_X]; //10
	int nSegY = (int)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_SEG_Y]; //10
	double dDefect_Dark_Ratio = (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_DEFECT_DARK_TH_RATIO]; //0.9
	double dBK_Dark_Ratio = (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_BG_DARK_TH_RATIO]; //0.85

	double dJudge_Axis_Ratio = (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_AXIS_RATIO]; //3.0
	double dJudge_Area_Ratio = (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_AREA_RATIO]; //50.0
	int nJudge_BG_Defect_Size = (int)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_BG_DEFECT_SIZE]; //30

	double dJudge_Center_BG_Defect_Ratio = (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_BG_CENTER_DEFECT_RATIO]; //4.0
	double dJudge_Center_Diff = (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_CENTER_DIFF];  //-11
	double dJudge_Center_Diff_Ratio = (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_CENTER_DIFF_RATIO]; //0.9
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//int nOffset = 0;
	//////////////////////////////////////////////////////////////////////////
	long	nWidth = (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcBuffer.rows;	// 영상 세로 크기

	int nMorpObj = 0;
	int nOffset = 0;
	int nOffset_Defect = 0;


	//////////////////////////////////////////////////////////////////////////
	// Active 영역 제외한 배경 영역 평균 GV Set
	cv::Mat matSrcRoI;
	cv::Mat matActMask_Temp;

	m_stThrdAlignInfo.matMask_Active.copyTo(matActMask_Temp);

	cv::Scalar meanRoi, stdRoi;
	cv::meanStdDev(matSrcBuffer, meanRoi, stdRoi, matActMask_Temp);


	cv::Mat matActMask_Gv = cv::Mat::zeros(matSrcBuffer.size(), matSrcBuffer.type());
	matActMask_Gv.setTo(meanRoi[0]);


	cv::bitwise_not(matActMask_Temp, matActMask_Temp);
	matSrcBuffer.copyTo(matSrcRoI);
	cv::subtract(matSrcRoI, matActMask_Temp, matSrcRoI);

	cv::bitwise_and(matActMask_Gv, matActMask_Temp, matActMask_Gv);
	cv::add(matSrcRoI, matActMask_Gv, matSrcRoI);
	//////////////////////////////////////////////////////////////////////////

	/*#ifdef _DEBUG
	#else
	#pragma omp parallel for
	#endif*/
	// 불량 개수 만큼
	for (int i = 0; i < nCount; i++)
	{

		// Spot 불량이 아닌 경우 넘어가기
		if (pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_POINT_DARK &&
			pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_LINE_X_DARK) //04.16 choi
			continue;

		cv::Mat matDefectMorp1Buf;
		cv::Mat matDefectBKBuf;

		int nOffset_Defect_W = (pResultBlob->ptRB[i].x - pResultBlob->ptLT[i].x) + 1;
		int nOffset_Defect_H = (pResultBlob->ptRB[i].y - pResultBlob->ptLT[i].y) + 1;
		int nOffset = 0;
		int nBright_Line_Offset = 0;

		if (nOffset_Defect_W > nOffset_Defect_H) {
			nOffset = nOffset_Defect_W + nCrop_Offset; //old
			nBright_Line_Offset = nOffset_Defect_W;
		}
		else {
			nOffset = nOffset_Defect_H + nCrop_Offset; //old
			nBright_Line_Offset = nOffset_Defect_H;
		}

		if (nOffset % 2 == 0) nOffset += 1;


		int nSegX = 10;
		int nSegY = 10;

		int nRangeX = 0;
		int nRangeY = 0;

		
		//불량 중심 기준 Crop Rect
		cv::Rect rectTempROI(pResultBlob->nCenterx[i] - nOffset, pResultBlob->nCentery[i] - nOffset, nOffset * 2, nOffset * 2);
		if (rectTempROI.x < 0)						rectTempROI.x = 0;
		if (rectTempROI.y < 0)						rectTempROI.y = 0;
		if (rectTempROI.x + nOffset >= nWidth)		rectTempROI.x = nWidth - nOffset - 1;
		if (rectTempROI.y + nOffset >= nHeight)	rectTempROI.y = nHeight - nOffset - 1;
		//////////////////////////////////////////////////////////////////////////

		//Crop한 이미지 배경 평균 구하기 위한 Range
		nRangeX = rectTempROI.width / nSegX;
		nRangeY = rectTempROI.height / nSegY;
		//////////////////////////////////////////////////////////////////////////

		//Crop Rect 기준 불량 크기 Rect
		cv::Rect rectDefectTempROI(pResultBlob->ptLT[i].x - rectTempROI.x, pResultBlob->ptLT[i].y - rectTempROI.y, nOffset_Defect_W, nOffset_Defect_H);
		if (rectDefectTempROI.x < 0)						rectDefectTempROI.x = 0;
		if (rectDefectTempROI.y < 0)						rectDefectTempROI.y = 0;
		if (rectDefectTempROI.x + nOffset_Defect_W >= rectTempROI.width)		rectDefectTempROI.x = rectTempROI.width - nOffset_Defect_W - 1;
		if (rectDefectTempROI.y + nOffset_Defect_H >= rectTempROI.height)	rectDefectTempROI.y = rectTempROI.height - nOffset_Defect_H - 1;

		
		//////////////////////////////////////////////////////////////////////////
		//Defect 중심 Mask 생성
		cv::Mat matDefectSrcBuf = matSrcRoI(rectTempROI);
		cv::Mat matDefectCenter = cv::Mat::zeros(matDefectSrcBuf.size(), matDefectSrcBuf.type());
		cv::Mat matBrightLine_Mask = cv::Mat::zeros(matDefectSrcBuf.size(), matDefectSrcBuf.type());

		matDefectCenter(rectDefectTempROI).setTo(255);
		matDefectCenter.copyTo(matBrightLine_Mask);

		Mask_SizeUp(matBrightLine_Mask, nBright_Line_Offset);
		//matBrightLine_Mask(rectBrightLineROI).setTo(255);
		matBrightLine_Mask -= matDefectCenter;
		//////////////////////////////////////////////////////////////////////////

		//Crop 한 이미지 배경 평균 계산
		vector<int> vMedian;
		int nSum = 0;
		int nThres = 0;
		int nBack_Dark_Thres = 0;
		int nBack_Bright_Thres = 0;

		for (int y = 0; y < nSegY; y++) {
			for (int x = 0; x < nSegX; x++) {
				int nPt_X = x*nRangeX;
				int nPt_Y = y*nRangeY;


				cv::Rect rectMedianROI(nPt_X, nPt_Y, nRangeX, nRangeY);

				cv::Scalar meanORI, stdORI;
				cv::meanStdDev(matDefectSrcBuf(rectMedianROI), meanORI, stdORI);

				vMedian.push_back(meanORI[0]);
			}
		}

		sort(vMedian.begin(), vMedian.end());

		for (int i = 20; i < vMedian.size() - 20; i++) {
			nSum += vMedian[i];
		}

		nThres = (int)nSum / (vMedian.size() - 40);
		//////////////////////////////////////////////////////////////////////////


		//배경과 불량 TH값 다르게 Set
		nBack_Dark_Thres = nThres * dBK_Dark_Ratio;
		nBack_Bright_Thres = nThres * 1.1;

		nThres *= dDefect_Dark_Ratio;

		
		//////////////////////////////////////////////////////////////////////////

		cv::Mat matDefectResBuf;
		cv::Mat matDefectResBuf_Temp;
		cv::Mat matDefectResBuf_Dark_Back;
		cv::Mat matDefectResBuf_Bright_Back;

		cv::threshold(matDefectSrcBuf, matDefectResBuf, nThres, 255.0, CV_THRESH_BINARY_INV); //불량 기준 Threshold
		cv::threshold(matDefectSrcBuf, matDefectResBuf_Dark_Back, nBack_Dark_Thres, 255.0, CV_THRESH_BINARY_INV); //배경 기준 Threshold Dark
		cv::threshold(matDefectSrcBuf, matDefectResBuf_Bright_Back, nBack_Bright_Thres, 255.0, CV_THRESH_BINARY); //배경 기준 Threshold Bright

		matDefectResBuf.copyTo(matDefectResBuf_Temp);

		cv::bitwise_and(matDefectResBuf, matDefectCenter, matDefectResBuf); //불량 Center Mask로 불량만 추출

		double valMin, valMax;
		cv::minMaxLoc(matDefectResBuf, &valMin, &valMax, 0, 0);

		//Center 불량만 추출했을때 아무값도 없으면 제거
		if (valMax <= 0) {
			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_LINE_X_VTH_DARK;
			//pResultBlob->bUseResult[i] = false;

			pResultBlob_Sub->bUseResult[i] = pResultBlob->bUseResult[i];
			pResultBlob_Sub->nDefectJudge[i] = pResultBlob->nDefectJudge[i];
			pResultBlob_Sub->nDefectColor[i] = pResultBlob->nDefectColor[i];


			continue;
		}
		//////////////////////////////////////////////////////////////////////////

		// 불량 조금 크게 설정
		int nValue = nMorpObj * 2 + 1;
		if (nMorpObj >= 1)
			cv::morphologyEx(matDefectResBuf, matDefectMorp1Buf, MORPH_DILATE, cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(nValue, nValue)));
		else
			matDefectResBuf.copyTo(matDefectMorp1Buf);

		// 배경 부분
		cv::bitwise_not(matDefectMorp1Buf, matDefectBKBuf); //배경에서 불량 부분 제거

		cv::Mat matBG_Bright_Line;
		int nBG_Bright_Line_Size = 0;

		cv::bitwise_and(matDefectResBuf_Bright_Back, matBrightLine_Mask, matBG_Bright_Line); //불량 제거한 주변 배경의 올라온 후보군 개수
		for (int i = 0; i < matBG_Bright_Line.rows * matBG_Bright_Line.cols; i++) {
			if (matBG_Bright_Line.data[i] == 255) { nBG_Bright_Line_Size++; }
		}

		cv::Mat matBG_Defect;
		int nBG_Defect_Size = 0;

		cv::bitwise_and(matDefectResBuf_Dark_Back, matDefectBKBuf, matBG_Defect); //불량 제거한 주변 배경의 올라온 후보군 개수
		for (int i = 0; i < matBG_Defect.rows * matBG_Defect.cols; i++) {
			if (matBG_Defect.data[i] == 255) { nBG_Defect_Size++; }
		}

		cv::Mat mat_Defect;
		int nDefect_Size = 0;

		cv::bitwise_and(matDefectResBuf_Temp, matDefectMorp1Buf, mat_Defect); // 불량 크기
		for (int i = 0; i < mat_Defect.rows * mat_Defect.cols; i++) {
			if (mat_Defect.data[i] == 255) { nDefect_Size++; }
		}

		cv::Scalar meanBK, stdBK;
		cv::meanStdDev(matDefectSrcBuf, meanBK, stdBK, matDefectBKBuf); //불량 제거한 배경의 평균GV, 표준편차

		cv::Scalar meanORJ, stdORJ;
		cv::meanStdDev(matDefectSrcBuf, meanORJ, stdORJ, matDefectMorp1Buf); //불량의 평균GV, 표준편차


		pResultBlob->dMuraObj[i] = meanORJ[0]; 
		pResultBlob->dMuraBk[i] = meanBK[0];

		// 무라 정보
		double	dArea = pResultBlob->nArea[i];
		double	dSub = meanORJ[0] - meanBK[0]; //불량평균GV - 불량 제거한 배경의 평균GV
		double	dDiff = meanORJ[0] / meanBK[0]; //불량평균GV / 불량 제거한 배경의 평균GV


		double ratio = 0;
		double area_ratio = 0;

		//////////////////////////////////////////////////////////////////////////
		//matDefectResBuf_Dark_Back 중심 안뺀 배경
		//mat_Defect 오로지 디펙
		//matBG_Defect 디팩 뺀 배경
		//////////////////////////////////////////////////////////////////////////

		cv::Mat matLabel, matStats, matCentroid;

		__int64 nTotalLabel = 0;

		nTotalLabel = cv::connectedComponentsWithStats(matDefectResBuf_Dark_Back, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1; //Crop 영상안에서 라벨링 (불량 주변 붙어있는 어두운 배경 찾기위함)
		cv::Point2i  ptCenter(mat_Defect.cols / 2, mat_Defect.rows / 2);
		cv::Rect rcBK_Defect;
		cv::Mat matCenter_BK = cv::Mat::zeros(matDefectResBuf_Dark_Back.size(), matDefectResBuf_Dark_Back.type());

		vector<vector<cv::Point>>	contours;
		vector<vector<cv::Point>>().swap(contours);
		
		
	
		for (int idx = 1; idx <= nTotalLabel; idx++) {

			int nArea = matStats.at<int>(idx, CC_STAT_AREA);

			rcBK_Defect.x = matStats.at<int>(idx, CC_STAT_LEFT);
			rcBK_Defect.y = matStats.at<int>(idx, CC_STAT_TOP);
			rcBK_Defect.width = matStats.at<int>(idx, CC_STAT_WIDTH);
			rcBK_Defect.height = matStats.at<int>(idx, CC_STAT_HEIGHT);

			//
		
			//불량과 붙어있는 배경 장단축 비율, Area 비율 계산
			if (rcBK_Defect.contains(ptCenter)) {

				//////////////
				cv::findContours(matDefectResBuf_Dark_Back(rcBK_Defect), contours, CV_RETR_LIST, CHAIN_APPROX_NONE);
				cv::RotatedRect BoundingBox = cv::minAreaRect(contours[0]);
				//////////////
				
				/*if (rcBK_Defect.width >= rcBK_Defect.height) {
					ratio = ((double)rcBK_Defect.width / (double)rcBK_Defect.height);
				}
				else {
					ratio = ((double)rcBK_Defect.height / (double)rcBK_Defect.width);
				}*/
				if (BoundingBox.size.width >= BoundingBox.size.height) {
					ratio = ((double)BoundingBox.size.width / (double)BoundingBox.size.height);
				}
				else {
					ratio = ((double)BoundingBox.size.height / (double)BoundingBox.size.width);
				}

				area_ratio = ((double)nArea / ((double)rcBK_Defect.width *  (double)rcBK_Defect.height)) * 100.0;

				matDefectResBuf_Dark_Back(rcBK_Defect).copyTo(matCenter_BK(rcBK_Defect));

				break;
			}
		}

		//불량과 붙어있는 배경에서 불량 제거;
		matCenter_BK -= mat_Defect;

		cv::Scalar meanBK_Center, stdBK_Center;
		cv::meanStdDev(matDefectSrcBuf, meanBK_Center, stdBK_Center, matCenter_BK); //불량과 붙어있는 배경 평균GV, 표준편차 계산
		int nCenterBK_Cnt = 0;

		for (int i = 0; i < matCenter_BK.rows*matCenter_BK.cols; i++) { //불량과 붙어있는 배경 크기
			if (matCenter_BK.data[i] > 0) nCenterBK_Cnt++;
		}


		double	dSub_Center = meanORJ[0] - meanBK_Center[0]; //불량 평균GV - 불량과 붙어있는 배경 평균GV
		double	dDiff_Center = meanORJ[0] / meanBK_Center[0]; //불량 평균GV / 불량과 붙어있는 배경 평균GV

		// Spec1
		if (
			nBG_Bright_Line_Size > 400) {
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_LINE_X_VTH_DARK;
		}
		/*else if (dArea >10 &&
			ratio > 2.0 &&
			area_ratio < dJudge_Area_Ratio) {
			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_LINE_DGS;
		}
		else if (dArea >14 &&
			nCenterBK_Cnt > 20 ) 
		{
			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK_SP_1;
		}*/
/*		else if (ratio > dJudge_Axis_Ratio &&
			area_ratio < dJudge_Area_Ratio) {
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_LINE_DGS;
		}
		else if (ratio > 3.0 ) {
			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_LINE_X_OPEN;
		}*/

		// Spec1
// 		if (
// 			ratio < dJudge_Axis_Ratio &&
// 			area_ratio > dJudge_Area_Ratio &&
// 			nBG_Defect_Size < nJudge_BG_Defect_Size)
// 		{
// 
// 			if (nCenterBK_Cnt > nDefect_Size / dJudge_Center_BG_Defect_Ratio) {
// 				if (dSub_Center < dJudge_Center_Diff &&
// 					dDiff_Center < dJudge_Center_Diff_Ratio) {
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK;
// 				}
// 				else {
// 					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
// 					//pResultBlob->bUseResult[i] = false;
// 				}
// 			}
// 			else {
// 				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK;
// 			}
// 		}
// 
// 		else {
// 
// 			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK_SP_2;
// 			//pResultBlob->bUseResult[i] = false;
// 
// 		}
		//////////////////////////////////////////////////////////////////////////
		//결과 확인 저장용

		pResultBlob_Sub->bUseResult[i] = pResultBlob->bUseResult[i];
		pResultBlob_Sub->nDefectJudge[i] = pResultBlob->nDefectJudge[i];
		pResultBlob_Sub->nDefectColor[i] = pResultBlob->nDefectColor[i];
		pResultBlob_Sub->nDefect_Size[i] = nDefect_Size;
		pResultBlob_Sub->dDiff_Sub[i] = dSub;
		pResultBlob_Sub->dDiff_Ratio[i] = dDiff;
		pResultBlob_Sub->dAxis_Ratio[i] = ratio;
		pResultBlob_Sub->dArea_Ratio[i] = area_ratio;
		pResultBlob_Sub->nDefect_Box_Width[i] = rcBK_Defect.width;
		pResultBlob_Sub->nDefect_Box_Height[i] = rcBK_Defect.height;
		pResultBlob_Sub->nBG_Size[i] = nBG_Defect_Size;
		pResultBlob_Sub->nCenterBK_Size[i] = nCenterBK_Cnt;
		pResultBlob_Sub->dDiff_Center_Sub[i] = dSub_Center;
		pResultBlob_Sub->dDiff_Center_Ratio[i] = dDiff_Center;

		//////////////////////////////////////////////////////////////////////////
	}
	pResultBlob_Sub->nDefectCount = pResultBlob->nDefectCount;

	// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
	// 	}

	return E_ERROR_CODE_TRUE;
}

long AviInspection::JudgeSubStanceSpot_Transparency(cv::Mat& matSrcBuffer, CRect rectROI, double* dPara, int* nCommonPara,
	stDefectInfo* pResultBlob, stDefectInfo_Substance* pResultBlob_Sub)
{
	// 불량이 없는 경우 나가기
	int nCount = pResultBlob->nDefectCount;
	if (nCount <= 0)		return E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	//////////////////////////////////////////////////////////////////////////
	//Align Para
	int nCrop_Offset					 = (int)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_CROP_OFFSET]; //15
	int nSegX							 = (int)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_SEG_X]; //10
	int nSegY							 = (int)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_SEG_Y]; //10
	double dDefect_Dark_Ratio			 = (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_DEFECT_DARK_TH_RATIO]; //0.9
	double dBK_Dark_Ratio				 = (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_BG_DARK_TH_RATIO]; //0.85

	double dJudge_Axis_Ratio			= (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_AXIS_RATIO]; //3.0
	double dJudge_Area_Ratio			= (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_AREA_RATIO]; //50.0
	int nJudge_BG_Defect_Size			= (int)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_BG_DEFECT_SIZE]; //80

	double dJudge_Center_BG_Defect_Ratio= (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_BG_CENTER_DEFECT_RATIO]; //4.0
	double dJudge_Center_Diff			= (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_CENTER_DIFF];  //-22
	double dJudge_Center_Diff_Ratio		= (double)dPara[E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_CENTER_DIFF_RATIO]; //0.9
	//////////////////////////////////////////////////////////////////////////
	//int nOffset = 0;
	//////////////////////////////////////////////////////////////////////////
	long	nWidth = (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcBuffer.rows;	// 영상 세로 크기

	int nMorpObj = 0;
	int nOffset = 0;
	int nOffset_Defect = 0;


	//////////////////////////////////////////////////////////////////////////
	// Active 영역 제외한 배경 영역 평균 GV Set
	cv::Mat matSrcRoI;
	cv::Mat matActMask_Temp;

	m_stThrdAlignInfo.matMask_Active.copyTo(matActMask_Temp);

	cv::Scalar meanRoi, stdRoi;
	cv::meanStdDev(matSrcBuffer, meanRoi, stdRoi, matActMask_Temp);


	cv::Mat matActMask_Gv = cv::Mat::zeros(matSrcBuffer.size(), matSrcBuffer.type());
	matActMask_Gv.setTo(meanRoi[0]);


	cv::bitwise_not(matActMask_Temp, matActMask_Temp);
	matSrcBuffer.copyTo(matSrcRoI);
	cv::subtract(matSrcRoI, matActMask_Temp, matSrcRoI);

	cv::bitwise_and(matActMask_Gv, matActMask_Temp, matActMask_Gv);
	cv::add(matSrcRoI, matActMask_Gv, matSrcRoI);
	//////////////////////////////////////////////////////////////////////////

	/*#ifdef _DEBUG
	#else
	#pragma omp parallel for
	#endif*/
	// 불량 개수 만큼
	for (int i = 0; i < nCount; i++)
	{

		// Spot 불량이 아닌 경우 넘어가기
		if (pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_POINT_DARK) //04.16 choi
			continue;

		cv::Mat matDefectMorp1Buf;
		cv::Mat matDefectBKBuf;

		int nOffset_Defect_W = (pResultBlob->ptRB[i].x - pResultBlob->ptLT[i].x) + 1;
		int nOffset_Defect_H = (pResultBlob->ptRB[i].y - pResultBlob->ptLT[i].y) + 1;
		int nOffset = 0;

		if (nOffset_Defect_W > nOffset_Defect_H) {
			nOffset = nOffset_Defect_W + nCrop_Offset; //old
		}
		else {
			nOffset = nOffset_Defect_H + nCrop_Offset; //old
		}

		if (nOffset % 2 == 0) nOffset += 1;


		int nSegX = 10;
		int nSegY = 10;

		int nRangeX = 0;
		int nRangeY = 0;


		//불량 중심 기준 Crop Rect
		cv::Rect rectTempROI(pResultBlob->nCenterx[i] - nOffset, pResultBlob->nCentery[i] - nOffset, nOffset * 2, nOffset * 2);
		if (rectTempROI.x < 0)						rectTempROI.x = 0;
		if (rectTempROI.y < 0)						rectTempROI.y = 0;
		if (rectTempROI.x + nOffset >= nWidth)		rectTempROI.x = nWidth - nOffset - 1;
		if (rectTempROI.y + nOffset >= nHeight)	rectTempROI.y = nHeight - nOffset - 1;
		//////////////////////////////////////////////////////////////////////////

		//Crop한 이미지 배경 평균 구하기 위한 Range
		nRangeX = rectTempROI.width / nSegX;
		nRangeY = rectTempROI.height / nSegY;
		//////////////////////////////////////////////////////////////////////////

		//Crop Rect 기준 불량 크기 Rect
		cv::Rect rectDefectTempROI(pResultBlob->ptLT[i].x - rectTempROI.x, pResultBlob->ptLT[i].y - rectTempROI.y, nOffset_Defect_W, nOffset_Defect_H);
		if (rectDefectTempROI.x < 0)						rectDefectTempROI.x = 0;
		if (rectDefectTempROI.y < 0)						rectDefectTempROI.y = 0;
		if (rectDefectTempROI.x + nOffset_Defect >= nWidth)		rectDefectTempROI.x = nWidth - nOffset_Defect_W - 1;
		if (rectDefectTempROI.y + nOffset_Defect >= nHeight)	rectDefectTempROI.y = nHeight - nOffset_Defect_H - 1;


		//////////////////////////////////////////////////////////////////////////
		//Defect 중심 Mask 생성
		cv::Mat matDefectSrcBuf = matSrcRoI(rectTempROI);
		cv::Mat matDefectCenter = cv::Mat::zeros(matDefectSrcBuf.size(), matDefectSrcBuf.type());
		matDefectCenter(rectDefectTempROI).setTo(255);
		//////////////////////////////////////////////////////////////////////////

		//Crop 한 이미지 배경 평균 계산
		vector<int> vMedian;
		int nSum = 0;
		int nThres = 0;
		int nBack_Thres = 0;

		for (int y = 0; y < nSegY; y++) {
			for (int x = 0; x < nSegX; x++) {
				int nPt_X = x*nRangeX;
				int nPt_Y = y*nRangeY;


				cv::Rect rectMedianROI(nPt_X, nPt_Y, nRangeX, nRangeY);

				cv::Scalar meanORI, stdORI;
				cv::meanStdDev(matDefectSrcBuf(rectMedianROI), meanORI, stdORI);

				vMedian.push_back(meanORI[0]);
			}
		}

		sort(vMedian.begin(), vMedian.end());

		for (int i = 20; i < vMedian.size() - 20; i++) {
			nSum += vMedian[i];
		}

		nThres = (int)nSum / (vMedian.size() - 40);
		//////////////////////////////////////////////////////////////////////////


		//배경과 불량 TH값 다르게 Set
		nBack_Thres = nThres * dBK_Dark_Ratio;
		nThres *= dDefect_Dark_Ratio;
		//////////////////////////////////////////////////////////////////////////

		cv::Mat matDefectResBuf;
		cv::Mat matDefectResBuf_Temp;
		cv::Mat matDefectResBuf_Back;

		cv::threshold(matDefectSrcBuf, matDefectResBuf, nThres, 255.0, CV_THRESH_BINARY_INV); //불량 기준 Threshold
		cv::threshold(matDefectSrcBuf, matDefectResBuf_Back, nBack_Thres, 255.0, CV_THRESH_BINARY_INV); //배경 기준 Threshold
		matDefectResBuf.copyTo(matDefectResBuf_Temp);

		cv::bitwise_and(matDefectResBuf, matDefectCenter, matDefectResBuf); //불량 Center Mask로 불량만 추출

		double valMin, valMax;
		cv::minMaxLoc(matDefectResBuf, &valMin, &valMax, 0, 0);

		//Center 불량만 추출했을때 아무값도 없으면 제거
		if (valMax <= 0) {
			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
			//pResultBlob->bUseResult[i] = false;

			pResultBlob_Sub->bUseResult[i] = pResultBlob->bUseResult[i];
			pResultBlob_Sub->nDefectJudge[i] = pResultBlob->nDefectJudge[i];
			pResultBlob_Sub->nDefectColor[i] = pResultBlob->nDefectColor[i];


			continue;
		}
		//////////////////////////////////////////////////////////////////////////

		// 불량 조금 크게 설정
		int nValue = nMorpObj * 2 + 1;
		if (nMorpObj >= 1)
			cv::morphologyEx(matDefectResBuf, matDefectMorp1Buf, MORPH_DILATE, cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(nValue, nValue)));
		else
			matDefectResBuf.copyTo(matDefectMorp1Buf);

		// 배경 부분
		cv::bitwise_not(matDefectMorp1Buf, matDefectBKBuf); //배경에서 불량 부분 제거

		cv::Mat matBG_Defect;
		int nBG_Defect_Size = 0;

		cv::bitwise_and(matDefectResBuf_Back, matDefectBKBuf, matBG_Defect); //불량 제거한 주변 배경의 올라온 후보군 개수
		for (int i = 0; i < matBG_Defect.rows * matBG_Defect.cols; i++) {
			if (matBG_Defect.data[i] == 255) { nBG_Defect_Size++; }
		}

		cv::Mat mat_Defect;
		int nDefect_Size = 0;

		cv::bitwise_and(matDefectResBuf_Temp, matDefectMorp1Buf, mat_Defect); // 불량 크기
		for (int i = 0; i < mat_Defect.rows * mat_Defect.cols; i++) {
			if (mat_Defect.data[i] == 255) { nDefect_Size++; }
		}

		cv::Scalar meanBK, stdBK;
		cv::meanStdDev(matDefectSrcBuf, meanBK, stdBK, matDefectBKBuf); //불량 제거한 배경의 평균GV, 표준편차

		cv::Scalar meanORJ, stdORJ;
		cv::meanStdDev(matDefectSrcBuf, meanORJ, stdORJ, matDefectMorp1Buf); //불량의 평균GV, 표준편차


		pResultBlob->dMuraObj[i] = meanORJ[0];
		pResultBlob->dMuraBk[i] = meanBK[0];

		// 무라 정보
		double	dArea = pResultBlob->nArea[i];
		double	dSub = meanORJ[0] - meanBK[0]; //불량평균GV - 불량 제거한 배경의 평균GV
		double	dDiff = meanORJ[0] / meanBK[0]; //불량평균GV / 불량 제거한 배경의 평균GV


		double ratio = 0;
		double area_ratio = 0;

		//////////////////////////////////////////////////////////////////////////
		//matDefectResBuf_Back 중심 안뺀 배경
		//mat_Defect 오로지 디펙
		//matBG_Defect 디팩 뺀 배경
		//////////////////////////////////////////////////////////////////////////

		cv::Mat matLabel, matStats, matCentroid;

		__int64 nTotalLabel = 0;

		nTotalLabel = cv::connectedComponentsWithStats(matDefectResBuf_Back, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1; //Crop 영상안에서 라벨링 (불량 주변 붙어있는 어두운 배경 찾기위함)
		cv::Point2i  ptCenter(mat_Defect.cols / 2, mat_Defect.rows / 2);
		cv::Rect rcBK_Defect;
		cv::Mat matCenter_BK = cv::Mat::zeros(matDefectResBuf_Back.size(), matDefectResBuf_Back.type());



		for (int idx = 1; idx <= nTotalLabel; idx++) {

			int nArea = matStats.at<int>(idx, CC_STAT_AREA);

			rcBK_Defect.x = matStats.at<int>(idx, CC_STAT_LEFT);
			rcBK_Defect.y = matStats.at<int>(idx, CC_STAT_TOP);
			rcBK_Defect.width = matStats.at<int>(idx, CC_STAT_WIDTH);
			rcBK_Defect.height = matStats.at<int>(idx, CC_STAT_HEIGHT);

			//불량과 붙어있는 배경 장단축 비율, Area 비율 계산
			if (rcBK_Defect.contains(ptCenter)) {
				if (rcBK_Defect.width >= rcBK_Defect.height) {
					ratio = ((double)rcBK_Defect.width / (double)rcBK_Defect.height);
				}
				else {
					ratio = ((double)rcBK_Defect.height / (double)rcBK_Defect.width);
				}

				area_ratio = ((double)nArea / ((double)rcBK_Defect.width *  (double)rcBK_Defect.height)) * 100.0;

				matDefectResBuf_Back(rcBK_Defect).copyTo(matCenter_BK(rcBK_Defect));

				break;
			}
		}

		//불량과 붙어있는 배경에서 불량 제거;
		matCenter_BK -= mat_Defect;

		cv::Scalar meanBK_Center, stdBK_Center;
		cv::meanStdDev(matDefectSrcBuf, meanBK_Center, stdBK_Center, matCenter_BK); //불량과 붙어있는 배경 평균GV, 표준편차 계산
		int nCenterBK_Cnt = 0;

		for (int i = 0; i < matCenter_BK.rows*matCenter_BK.cols; i++) { //불량과 붙어있는 배경 크기
			if (matCenter_BK.data[i] > 0) nCenterBK_Cnt++;
		}


		double	dSub_Center = meanORJ[0] - meanBK_Center[0]; //불량 평균GV - 불량과 붙어있는 배경 평균GV
		double	dDiff_Center = meanORJ[0] / meanBK_Center[0]; //불량 평균GV / 불량과 붙어있는 배경 평균GV

															  // Spec1
		if (
			ratio < dJudge_Axis_Ratio &&
			area_ratio > dJudge_Area_Ratio &&
			nBG_Defect_Size < nJudge_BG_Defect_Size)
		{

			if (nCenterBK_Cnt > nDefect_Size / dJudge_Center_BG_Defect_Ratio) {
				if (dSub_Center < dJudge_Center_Diff &&
					dDiff_Center < dJudge_Center_Diff_Ratio) {
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK;
				}
				else {
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
					//pResultBlob->bUseResult[i] = false;
				}
			}
			else {
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK;
			}
		}

		else {

			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_DARK_SP_2;
			//pResultBlob->bUseResult[i] = false;

		}

		//////////////////////////////////////////////////////////////////////////
		//결과 확인 저장용

		pResultBlob_Sub->bUseResult[i] = pResultBlob->bUseResult[i];
		pResultBlob_Sub->nDefectJudge[i] = pResultBlob->nDefectJudge[i];
		pResultBlob_Sub->nDefectColor[i] = pResultBlob->nDefectColor[i];
		pResultBlob_Sub->nDefect_Size[i] = nDefect_Size;
		pResultBlob_Sub->dDiff_Sub[i] = dSub;
		pResultBlob_Sub->dDiff_Ratio[i] = dDiff;
		pResultBlob_Sub->dAxis_Ratio[i] = ratio;
		pResultBlob_Sub->dArea_Ratio[i] = area_ratio;
		pResultBlob_Sub->nDefect_Box_Width[i] = rcBK_Defect.width;
		pResultBlob_Sub->nDefect_Box_Height[i] = rcBK_Defect.height;
		pResultBlob_Sub->nBG_Size[i] = nBG_Defect_Size;
		pResultBlob_Sub->nCenterBK_Size[i] = nCenterBK_Cnt;
		pResultBlob_Sub->dDiff_Center_Sub[i] = dSub_Center;
		pResultBlob_Sub->dDiff_Center_Ratio[i] = dDiff_Center;

		//////////////////////////////////////////////////////////////////////////
	}
	pResultBlob_Sub->nDefectCount = pResultBlob->nDefectCount;

	// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
	// 	}

	return E_ERROR_CODE_TRUE;
}


void AviInspection::KalMan_Filter(cv::Mat* stack, int nStack_size, double percentvar, double gain) {
	cv::Mat ip = stack[0];
	int bitDepth = 0;
	if (ip.type() == CV_8UC1)
		bitDepth = 8;
	else if (ip.type() == CV_16UC1)
		bitDepth = 16;
	else if (ip.type() == CV_32FC1)
		bitDepth = 32;
	else
		return;

	int width = stack[0].cols;
	int height = stack[0].rows;
	int dimension = width*height;
	int stacksize = nStack_size;

	double* stackslice = new double[dimension];
	double* filteredslice = new double[dimension];
	double* noisevar = new double[dimension];
	double* average = new double[dimension];
	double* predicted = new double[dimension];
	double* predictedvar = new double[dimension];
	double* observed = new double[dimension];
	double* Kalman = new double[dimension];
	double* corrected = new double[dimension];
	double* correctedvar = new double[dimension];

	for (int i = 0; i < dimension; ++i)
		noisevar[i] = percentvar;
	predicted = toDouble(stack[0].data, dimension);
	predictedvar = noisevar;

	for (int i = 1; i < stacksize; ++i) {
		//IJ.showProgress(i, stacksize);
		stackslice = toDouble(stack[i].data, dimension);
		observed = stackslice;
		for (int k = 0; k < dimension; ++k)
			Kalman[k] = predictedvar[k] / (predictedvar[k] + noisevar[k]);
		for (int k = 0; k < dimension; ++k)
			corrected[k] = gain*predicted[k] + (1.0 - gain)*observed[k] + Kalman[k] * (observed[k] - predicted[k]);
		for (int k = 0; k < dimension; ++k)
			correctedvar[k] = predictedvar[k] * (1.0 - Kalman[k]);
		predictedvar = correctedvar;
		predicted = corrected;


		stack[0].data = (uchar*)fromDouble(corrected, dimension);
	}
}


byte* AviInspection::fromDouble(double* darray, int nlenght) {

	byte* bytes = new byte[nlenght];
	for (int i = 0; i < nlenght; i++)
		bytes[i] = (byte)darray[i];
	return bytes;
}



double* AviInspection::toDouble(byte* darray, int nlenght) {
	double* doubles = NULL;

	byte* bytes = (byte*)darray;
	doubles = new double[nlenght];
	for (int i = 0; i < nlenght; i++)
		doubles[i] = (bytes[i] & 0xff);

	return doubles;
}

long AviInspection::Image_Pow(int ImgType, double dpow, Mat& InPutImage, Mat& OutPutImage)
{
	double MaxValue = 4096;
	if (ImgType == CV_8UC1)
	{
		ushort LUT[256] = { 0 };
		double val = 0;
		for (int i = 0; i < 256; i++)
		{
			val = pow(i, dpow);

			if (val >= MaxValue) val = MaxValue - 1;
			if (val < 0) val = 0;
			LUT[i] = (ushort)val;
		}
		cv::MatIterator_<uchar> itSrc, endSrc;
		itSrc = InPutImage.begin<uchar>();
		endSrc = InPutImage.end<uchar>();
		cv::MatIterator_<ushort> itDst = OutPutImage.begin<ushort>();

		for (; itSrc != endSrc; itSrc++, itDst++)
			*itDst = LUT[(*itSrc)];
	}
	else
	{
		InPutImage.copyTo(OutPutImage);
	}

	return E_ERROR_CODE_TRUE;
}

void AviInspection::Lipschitz_Fillter(cv::Mat& ip)
{
	int slope, slope1, p, p1, p2, p3, p4, maxz;

	static boolean m_Down = false;     //  
	static boolean m_TopHat = true;  // lower Lipschitz cover  
	double m_Slope = 1.5;                 // slope

	int *destPixels = new int[ip.rows * ip.cols];
	int *srcPixels = new int[ip.rows * ip.cols];
	uchar *tmpBytePixels = new uchar[ip.rows * ip.cols];

	int ImageHeight = ip.rows;
	int ImageWidth = ip.cols;

	cv::Rect m_roi;
	m_roi.x = 0;
	m_roi.y = 0;
	m_roi.width = ip.cols;
	m_roi.height = ip.rows;
	// 	int destPixels[nChannels][ip.rows * ip.cols];
	// 	int srcPixels[ip.channels()][ip.rows * ip.cols];
	// 	uchar tmpBytePixels[ip.channels()][ip.rows * ip.cols];
	//	short tmpShortPixels[ip.channels()][ip.rows * ip.cols];



	tmpBytePixels = (uchar*)ip.data;



	int sign = (m_Down ? 1 : -1);
	int topdown = (m_Down ? 0 : 255);

	for (int ij = 0; ij < ip.rows * ip.cols; ij++)
	{
		srcPixels[ij] = sign *(tmpBytePixels[ij] & 0xff);
		destPixels[ij] = srcPixels[ij];

	}


	slope = (int)(m_Slope);
	slope1 = (int)(slope * sqrt(2.0));
	//	maxz = m_channels;


	for (int y = 0; y < ip.rows; y++)   // rows
	{


		p2 = sign * (topdown + (sign)* slope);
		p3 = sign * (topdown + (sign)* slope1);
		for (int x = m_roi.x; x < m_roi.x + m_roi.width; x++) // columns
		{
			p = (p2 - slope);
			p1 = (p3 - slope1);
			if (p1 > p) p = p1;
			p3 = destPixels[x + ImageWidth * (max(y - 1, 0))];
			p1 = p3 - slope;
			if (p1 > p) p = p1;

			p4 = destPixels[min(x + 1, ImageWidth - 1) + ImageWidth * (max(y - 1, 0))];
			p1 = p4 - slope1;
			if (p1 > p) p = p1;

			p2 = srcPixels[x + ImageWidth * y];
			if (p > p2) {
				destPixels[x + ImageWidth * y] = p;
				p2 = p;
			}
		}

	}

	for (int y = m_roi.y + m_roi.height - 1; y >= m_roi.y; y--)   // rows
	{
		p2 = sign * (topdown + (sign)* slope);
		p3 = sign * (topdown + (sign)* slope1);
		for (int x = m_roi.x + m_roi.width - 1; x >= m_roi.x; x--)  // columns
		{
			p = (p2 - slope);
			p1 = (p3 - slope1);
			if (p1 > p) p = p1;

			p3 = destPixels[x + ImageWidth * (min(y + 1, ImageHeight - 1))];
			p1 = p3 - slope;
			if (p1 > p)   p = p1;

			p4 = destPixels[max(x - 1, 0) + ImageWidth * (min(y + 1, ImageHeight - 1))];
			p1 = p4 - slope1;
			if (p1 > p) p = p1;

			p2 = destPixels[x + ImageWidth * y];
			if (p > p2)
			{
				destPixels[x + ImageWidth * y] = p;
				p2 = p;
			}
		}

	}



	for (int ij = 0; ij < ImageHeight * ImageWidth; ij++)
	{
		if (m_TopHat)
		{
			tmpBytePixels[ij] = (m_Down ? (byte)(srcPixels[ij] - destPixels[ij] + 255)
				: (byte)(destPixels[ij] - srcPixels[ij]));
		}
		else
		{

			tmpBytePixels[ij] = (byte)(sign * destPixels[ij]);
		}

	}


	// 	cv::Mat matResult = cv::Mat::zeros(ip.size(), ip.type());
	// 
	// 	for (int i = 0; i < ImageWidth * ImageHeight; i++) {
	// 		matResult.data[i] = tmpBytePixels[i];
	// 	}

	delete[] destPixels;
	delete[] srcPixels;

	// 	matResult.copyTo(ip);
	// 	matResult.release();

	//delete[] tmpBytePixels;

	// 	ByteProcessor bip = (ByteProcessor)ip;
	// 	bip.setPixels(tmpBytePixels[0]);
	// 	Mat M(2, 3, CV_32FC1); // warpAffine 연산에 사용되는 2*3 행렬
	// 	cv::Point2f center;
	// 	center.x = (float)ptCenter.x;
	// 	center.y = (float)ptCenter.y;
	// 	M = getRotationMatrix2D(center, Theta, 1.0);
	// 	cv::warpAffine(matResult, matResult, M, matResult.size()); // 회전
	// 
	// 	imwrite("D:\\Lipschitz_Fillter.bmp", matResult);


}
void AviInspection::BlobFeatureSave_Sub(stDefectInfo_Substance *resultBlob, CString strPath, int* nImageDefectCount)
{
	if (resultBlob == NULL)	return;

	int	nDefectNum = 0;

	// 결과 Log 저장
	FILE *out = NULL;

	if (nImageDefectCount == NULL)
		nImageDefectCount = &nDefectNum;

	if (*nImageDefectCount == 0)
		_wfopen_s(&out, strPath, _T("wt"));
	else
		_wfopen_s(&out, strPath, _T("at"));

	// 예외 처리.
	if (out == NULL)		return;

	// 여러개의 ROI 결과 기입 시 최초에만 Header 기입
	if (*nImageDefectCount == 0)
	{
		fprintf_s(out, "No						,\
 						Defect_Judge			,\
 						Defect_Color			,\
						Defect_Size				,\
						Diff_Sub				,\
						Diff_Ratio				,\
						Axis_Ratio				,\
						Area_Ratio				,\
						nDefect_Box_Width		,\
						nDefect_Box_Height		,\
						nBG_Size				,\
						nCenterBK_Size			,\
						dDiff_Center_Sub		,\
						dDiff_Center_Ratio		,\
						Use_Report			,,");

		fprintf_s(out, "\n");
	}
	// KSW 17.07.27 프로그램 다운 수정
	char szPath[MAX_PATH] = { 0, };
	for (int nFori = 0; nFori < resultBlob->nDefectCount; nFori++)
	{
		if (resultBlob->bUseResult[nFori] == false) continue;
		// KSW 17.07.27 프로그램 다운 수정
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, theApp.GetDefectTypeName(resultBlob->nDefectJudge[nFori]), -1, szPath, sizeof(szPath), NULL, NULL);

		// KSW 17.07.27 불량 개수 많아지면 프로그램 다운 발생
		//USES_CONVERSION;
		//char *cstrName = W2A( theApp.GetDefectTypeName(resultBlob->nDefectJudge[nFori]) );

		fprintf_s(out, "%d,%s,%d,%d,%f,%f,%f,%f,%d,%d,%d,%d,%f,%f,%s,,",
			(*nImageDefectCount)++,
			szPath,
			resultBlob->nDefectColor[nFori],
			resultBlob->nDefect_Size[nFori],
			resultBlob->dDiff_Sub[nFori],
			resultBlob->dDiff_Ratio[nFori],
			resultBlob->dAxis_Ratio[nFori],
			resultBlob->dArea_Ratio[nFori],
			resultBlob->nDefect_Box_Width[nFori],
			resultBlob->nDefect_Box_Height[nFori],
			resultBlob->nBG_Size[nFori],
			resultBlob->nCenterBK_Size[nFori],
			resultBlob->dDiff_Center_Sub[nFori],
			resultBlob->dDiff_Center_Ratio[nFori],
			resultBlob->bUseResult[nFori] ? "Y" : "N");

		fprintf_s(out, "\n");
	}
	fclose(out);
	out = NULL;


}

void AviInspection::BlobFeatureSave_LCP(stDefectInfo *resultBlob, CString strPath, CString strPanel_ID, double dResolution, int* nImageDefectCount)
{
	if (resultBlob == NULL)	return;

	int	nDefectNum = 0;

	// 결과 Log 저장
	FILE *out = NULL;

	if (nImageDefectCount == NULL)
		nImageDefectCount = &nDefectNum;

	if (*nImageDefectCount == 0)
		_wfopen_s(&out, strPath, _T("wt"));
	else
		_wfopen_s(&out, strPath, _T("at"));

	// 예외 처리.
	if (out == NULL)		return;

	// 여러개의 ROI 결과 기입 시 최초에만 Header 기입
	if (*nImageDefectCount == 0)
	{
		fprintf_s(out, "%S,,", strPanel_ID);

		fprintf_s(out, "\n");


		fprintf_s(out, ",\
 						이물 종류,,");

		fprintf_s(out, "\n");

		fprintf_s(out, "No						,\
 						흑						,\
 						청						,\
						홍						,\
						이물사이즈(um)			,\
						이물Area(pixel)			,\
						진위여부				,\
						이물사이즈(장축um)		,,");

		fprintf_s(out, "\n");
	}

	// KSW 17.07.27 프로그램 다운 수정
	char szPath[MAX_PATH] = { 0, };
	for (int nFori = 0; nFori < resultBlob->nDefectCount; nFori++)
	{
		// 		if (resultBlob->bUseResult[nFori] == false) continue;
		// 		// KSW 17.07.27 프로그램 다운 수정
		// 		memset(szPath, 0, sizeof(char) * MAX_PATH);
		// 		WideCharToMultiByte(CP_ACP, 0, theApp.GetDefectTypeName(resultBlob->nDefectJudge[nFori]), -1, szPath, sizeof(szPath), NULL, NULL);

		// KSW 17.07.27 불량 개수 많아지면 프로그램 다운 발생
		//USES_CONVERSION;
		//char *cstrName = W2A( theApp.GetDefectTypeName(resultBlob->nDefectJudge[nFori]) );
		double nDefect_distance = sqrt(pow((double)(resultBlob->ptRB[nFori].x - resultBlob->ptLT[nFori].x), 2) +
			pow((double)(resultBlob->ptRB[nFori].y - resultBlob->ptLT[nFori].y), 2));

		//장축을 실제 불량크기로 볼때..
		double dDefect_Size2 = 0;
		if ((resultBlob->ptRB[nFori].x - resultBlob->ptLT[nFori].x) > (resultBlob->ptRB[nFori].y - resultBlob->ptLT[nFori].y))
			 dDefect_Size2 = (resultBlob->ptRB[nFori].x - resultBlob->ptLT[nFori].x) * dResolution;

		else
			 dDefect_Size2 = (resultBlob->ptRB[nFori].y - resultBlob->ptLT[nFori].y) * dResolution;

		double dDefect_Size = nDefect_distance * dResolution;

		fprintf_s(out, "%d,%s,%s,%s,%f,%d,%s,%f,,",
			++(*nImageDefectCount),
			resultBlob->strColor[nFori] == "Dark" ? "O" : "",
			resultBlob->strColor[nFori] == "Blue" ? "O" : "",
			resultBlob->strColor[nFori] == "Red" ? "O" : "",
			dDefect_Size,
			resultBlob->nArea[nFori],
			resultBlob->nDefectJudge[nFori] != E_DEFECT_JUDGEMENT_RETEST_POINT_DARK ? "O" : "X",
			dDefect_Size2);

		fprintf_s(out, "\n");
	}
	fclose(out);
	out = NULL;


}

void AviInspection::BlobFeatureSave_LCP(ResultPanelData& resultPanelData, CString strPath, CString strPanel_ID, double dResolution, int* nImageDefectCount)
{
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return;

	int	nDefectNum = 0;

	// 결과 Log 저장
	FILE *out = NULL;

	if (nImageDefectCount == NULL)
		nImageDefectCount = &nDefectNum;

	if (*nImageDefectCount == 0)
		_wfopen_s(&out, strPath, _T("wt"));
	else
		_wfopen_s(&out, strPath, _T("at"));

	// 예외 처리.
	if (out == NULL)		return;

	// 여러개의 ROI 결과 기입 시 최초에만 Header 기입
	if (*nImageDefectCount == 0)
	{
		fprintf_s(out, "%S,,", strPanel_ID);

		fprintf_s(out, "\n");


		fprintf_s(out, ",\
 						이물 종류,,");

		fprintf_s(out, "\n");

		fprintf_s(out, "Index  No				        ,\
						Defect No				        ,\
 						흑						,\
 						청						,\
						홍						,\
						이물사이즈(um)			,\
						이물Area(pixel)			,\
						진위여부				,\
						이물사이즈(장축um)		,,");

		fprintf_s(out, "\n");
	}

	// KSW 17.07.27 프로그램 다운 수정
	char szPath[MAX_PATH] = { 0, };
	for (int nFori = 0; nFori < resultPanelData.m_ListDefectInfo.GetCount(); nFori++)
	{

		if (
			//////////////////////////////////////////////////////////////////////////불량군
			//어두운 불량
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||

			//////////////////////////////////////////////////////////////////////////
			//밝은 불량
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||

			////////////////////////////////////////////////////////////////////////// 후보군
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK)
		{
			double nDefect_distance = sqrt(pow((double)(resultPanelData.m_ListDefectInfo[nFori].Pixel_End_X - resultPanelData.m_ListDefectInfo[nFori].Pixel_Start_X), 2) +
				pow((double)(resultPanelData.m_ListDefectInfo[nFori].Pixel_End_Y - resultPanelData.m_ListDefectInfo[nFori].Pixel_Start_Y), 2));


			//장축을 실제 불량크기로 볼때..
			double dDefect_Size2 = 0;
			if ((resultPanelData.m_ListDefectInfo[nFori].Pixel_End_X - resultPanelData.m_ListDefectInfo[nFori].Pixel_Start_X) > (resultPanelData.m_ListDefectInfo[nFori].Pixel_End_Y - resultPanelData.m_ListDefectInfo[nFori].Pixel_Start_Y))
				dDefect_Size2 = (resultPanelData.m_ListDefectInfo[nFori].Pixel_End_X - resultPanelData.m_ListDefectInfo[nFori].Pixel_Start_X) * dResolution;

			else
				dDefect_Size2 = (resultPanelData.m_ListDefectInfo[nFori].Pixel_End_Y - resultPanelData.m_ListDefectInfo[nFori].Pixel_Start_Y) * dResolution;

			double dDefect_Size = nDefect_distance * dResolution;

			fprintf_s(out, "%d,%d,%s,%s,%s,%f,%d,%s,%f,,",
				++(*nImageDefectCount),
				nFori+1,
				resultPanelData.m_ListDefectInfo[nFori].strColor == "Dark" ? "O" : "",
				resultPanelData.m_ListDefectInfo[nFori].strColor == "Blue" ? "O" : "",
				resultPanelData.m_ListDefectInfo[nFori].strColor == "Red" ? "O" : "",
				dDefect_Size,
				resultPanelData.m_ListDefectInfo[nFori].Defect_Size_Pixel,
				resultPanelData.m_ListDefectInfo[nFori].Defect_Type != E_DEFECT_JUDGEMENT_RETEST_POINT_DARK ? "O" : "X",
				dDefect_Size2);

			fprintf_s(out, "\n");
		}

	}
	fclose(out);
	out = NULL;


}

void AviInspection::Mask_SizeUp(cv::Mat matSrc, int nDownSize)
{
	cv::Mat matTemp;
	cv::Mat Temp16;

	cv::bitwise_not(matSrc, matTemp);
	cv::distanceTransform(matTemp, Temp16, CV_DIST_L2, 3);

	cv::threshold(Temp16, Temp16, nDownSize, 255, CV_THRESH_BINARY);
	Temp16.convertTo(matTemp, CV_8UC1);
	cv::bitwise_not(matTemp, matSrc);
}


bool AviInspection::JudgementMoistureDefect(ResultPanelData& resultPanelData, double* dAlignPara ,double& dMoi_per, cv::Mat matMask_Active, cv::Rect rcAlignCellROI)
{
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	double dJudge_Area_Per = (double)dAlignPara[E_PARA_LCP_INSPECT_MOISTURE_JUDGE_AREA_RATIO];

	int nActive_Size = 0;
	int nMoisture_Size = 0;
	double dMoisture_Per = 0.0;

	vector<int> vMoisture_Index;

	for (int i = 0; i < matMask_Active.cols*matMask_Active.rows; i++) {
		if (matMask_Active.data[i] == 255) {
			nActive_Size++;
		}
	}
	
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)
		{
			nMoisture_Size += resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel;
			vMoisture_Index.push_back(i);
		}
	}

	if (nActive_Size > 0 && nMoisture_Size > 0) {
		dMoisture_Per = ((double)nMoisture_Size / (double)nActive_Size) *100.0;
	}

	dMoi_per = dMoisture_Per;

	if (vMoisture_Index.size()>0) {

		int nRemove_Cnt = 0;

		for (int j = 0; j < vMoisture_Index.size(); j++) {

			if (j == 0) {
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Draw_Defect_Rect = false;

				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_Start_X = rcAlignCellROI.x;
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_Start_Y = rcAlignCellROI.y;
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_End_X = rcAlignCellROI.x + rcAlignCellROI.width;
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_End_Y = rcAlignCellROI.y + rcAlignCellROI.height;
				
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_Center_X = (rcAlignCellROI.x + rcAlignCellROI.width) / 2;
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_Center_Y = (rcAlignCellROI.y + rcAlignCellROI.height) / 2;

				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_Crop_Start_X = resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_Start_X;
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_Crop_Start_Y = resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_Start_Y;
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_Crop_End_X = resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_End_X;
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_Crop_End_Y = resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Pixel_End_Y;

				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Defect_Size = 0;

				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Img_Size_X = rcAlignCellROI.width;
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Img_Size_Y = rcAlignCellROI.height;
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].d_MVper = dMoisture_Per;
				resultPanelData.m_ListDefectInfo[vMoisture_Index[j]].Defect_Type = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
			}
			else {
				resultPanelData.m_ListDefectInfo.RemoveAt(vMoisture_Index[j]-nRemove_Cnt);
				nRemove_Cnt++;
			}
		
		}
	}

	return true;
}

bool AviInspection::JudgementVarianceDefect(ResultPanelData& resultPanelData, double* dAlignPara, double& dVari_per, cv::Mat matMask_Active, cv::Rect rcAlignCellROI)
{
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	double dJudge_Area_Per = (double)dAlignPara[E_PARA_LCP_INSPECT_VARIANCE_JUDGE_AREA_RATIO];

	int nActive_Size = 0;
	int nVariance_Size = 0;
	double dVariance_Per = 0.0;

	vector<int> vVariance_Index;

	for (int i = 0; i < matMask_Active.cols*matMask_Active.rows; i++) {
		if (matMask_Active.data[i] == 255) {
			nActive_Size++;
		}
	}

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EMD_DARK)
		{
			nVariance_Size += resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel;
			vVariance_Index.push_back(i);
		}
	}

	if (nActive_Size > 0 && nVariance_Size > 0) {
		dVariance_Per = ((double)nVariance_Size / (double)nActive_Size) *100.0;
	}

	dVari_per = dVariance_Per;

	if (vVariance_Index.size()>0) {

		int nRemove_Cnt = 0;

		for (int j = 0; j < vVariance_Index.size(); j++) {
			if (j == 0) {
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Draw_Defect_Rect = false;

				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_Start_X = rcAlignCellROI.x;
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_Start_Y = rcAlignCellROI.y;
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_End_X = rcAlignCellROI.x + rcAlignCellROI.width;
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_End_Y = rcAlignCellROI.y + rcAlignCellROI.height;

				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_Center_X = (rcAlignCellROI.x + rcAlignCellROI.width) / 2;
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_Center_Y = (rcAlignCellROI.y + rcAlignCellROI.height) / 2;

				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_Crop_Start_X = resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_Start_X;
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_Crop_Start_Y = resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_Start_Y;
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_Crop_End_X = resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_End_X;
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_Crop_End_Y = resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Pixel_End_Y;

				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Defect_Size = 0;

				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Img_Size_X = rcAlignCellROI.width;
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Img_Size_Y = rcAlignCellROI.height;
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].d_MVper = dVariance_Per;
				resultPanelData.m_ListDefectInfo[vVariance_Index[j]].Defect_Type = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
			}
			else {
				resultPanelData.m_ListDefectInfo.RemoveAt(vVariance_Index[j]-nRemove_Cnt);
				nRemove_Cnt++;
			}

		}
	}

	return true;
}

bool AviInspection::JudgementDeleteChips(ResultPanelData& resultPanelData, double* dAlignPara, cv::Mat matSrc_Draw, cv::Rect rcAlignCellROI , CString strPanelID, CString strDrive)
{
	int nColorCode_Num = dAlignPara[E_PARA_LCP_COLORCODE_NUM];

	if (nColorCode_Num == E_COLOR_CODE_TRANSPARENCY)
		return true;

	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	cv::cvtColor(matSrc_Draw, matSrc_Draw, CV_GRAY2RGB);

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_NUGI)
		{
			vector<Point> vPt;

			//////////////////////////////////////////////////////////////////////////
			int j = 0;
			while (true)
			{
				if (resultPanelData.m_ListDefectInfo[i].nContoursX[j] != 0 || resultPanelData.m_ListDefectInfo[i].nContoursX[j] != NULL) {
					vPt.push_back(cv::Point(resultPanelData.m_ListDefectInfo[i].nContoursX[j], resultPanelData.m_ListDefectInfo[i].nContoursY[j]));
					j++;
				}
				else {
					break;
				}

			}
			////////////////////////////////////////////////////////////////////////// 분칩 Contour 좌표 Get

			
			float r = 0.f;
			cv::Point2f ptCenter;

			cv::minEnclosingCircle(vPt, ptCenter, r);
			cv::Rect BoungRect = cv::boundingRect(vPt);
			cv::RotatedRect minAreaRect = cv::minAreaRect(vPt);

			cv::Point2f vertices[E_CORNER_END];
			minAreaRect.points(vertices);

			//////////////////////////////////////////////////////////////////////////
			double dDistance_1 = sqrt(pow(vertices[0].x - vertices[1].x, 2) + pow(vertices[0].y - vertices[1].y, 2));
			double dDistance_2 = sqrt(pow(vertices[1].x - vertices[2].x, 2) + pow(vertices[1].y - vertices[2].y, 2));

			double dAngle = 0.0;
			int nVertical_mode = 0;

			cv::Point2d pt1(0.0, 0.0);
			cv::Point2d pt2(0.0, 0.0);

			if (dDistance_1 > dDistance_2) {
				if ((vertices[1].x - vertices[0].x) != 0.f) {
					dAngle = (vertices[1].y - vertices[0].y) / (vertices[1].x - vertices[0].x);

					pt1.x = (vertices[1].x + vertices[2].x) / 2;
					pt1.y = (vertices[1].y + vertices[2].y) / 2;

					pt2.x = (vertices[0].x + vertices[3].x) / 2;
					pt2.y = (vertices[0].y + vertices[3].y) / 2;

				}
				else {
					dAngle = (vertices[1].y - vertices[0].y) / (vertices[1].x - vertices[0].x);
					nVertical_mode = 1;

					pt1.x = (vertices[1].x + vertices[2].x) / 2;
					pt1.y = (vertices[1].y + vertices[2].y) / 2;

					pt2.x = (vertices[0].x + vertices[3].x) / 2;
					pt2.y = (vertices[0].y + vertices[3].y) / 2;

				}
			}
			else {
				if ((vertices[2].x - vertices[1].x) != 0.f) {
					dAngle = (vertices[2].y - vertices[1].y) / (vertices[2].x - vertices[1].x);

					pt1.x = (vertices[0].x + vertices[1].x) / 2;
					pt1.y = (vertices[0].y + vertices[1].y) / 2;

					pt2.x = (vertices[2].x + vertices[3].x) / 2;
					pt2.y = (vertices[2].y + vertices[3].y) / 2;
				}
				else {
					dAngle = (vertices[2].y - vertices[1].y) / (vertices[2].x - vertices[1].x);
					nVertical_mode = 1;

					pt1.x = (vertices[0].x + vertices[1].x) / 2;
					pt1.y = (vertices[0].y + vertices[1].y) / 2;

					pt2.x = (vertices[2].x + vertices[3].x) / 2;
					pt2.y = (vertices[2].y + vertices[3].y) / 2;
				}
			}
			////////////////////////////////////////////////////////////////////////// 분칩성 불량 끝점 좌표 및 각도 Get
			

			cv::line(matSrc_Draw, vertices[0], vertices[1], cv::Scalar(0, 255, 0), 1);//확인용
			cv::line(matSrc_Draw, vertices[1], vertices[2], cv::Scalar(0, 255, 0), 1);//확인용
			cv::line(matSrc_Draw, vertices[2], vertices[3], cv::Scalar(0, 255, 0), 1);//확인용
			cv::line(matSrc_Draw, vertices[3], vertices[0], cv::Scalar(0, 255, 0), 1);//확인용

			//cv::circle(matSrc_Draw, ptCenter, (int)r, Scalar(255, 0, 0), 1);//확인용
			//cv::rectangle(matSrc_Draw, BoungRect, Scalar(0, 0, 255), 1);//확인용
			
			cv::circle(matSrc_Draw, ptCenter, 40.0, Scalar(0, 0, 255), 5);//확인용

			cv::circle(matSrc_Draw, pt1, 1, Scalar(255, 255, 255), 1);//확인용
			cv::circle(matSrc_Draw, pt2, 1, Scalar(255, 255, 255), 1);//확인용

			////////////////////////////////////////////////////////////////////////// 이물성과 관련성 검사
			for (int k = 0; k < resultPanelData.m_ListDefectInfo.GetCount(); k++)
			{


				//			if (pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_POINT_DARK || pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 || pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 || pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
				//pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK || pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_LINE_X_DARK || pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_POINT_DIMPLE || pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_LINE_Y_DARK)

				if (resultPanelData.m_ListDefectInfo[k].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK || 
					resultPanelData.m_ListDefectInfo[k].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1	||
					resultPanelData.m_ListDefectInfo[k].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2	||
					resultPanelData.m_ListDefectInfo[k].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3	||
					resultPanelData.m_ListDefectInfo[k].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK	||
					resultPanelData.m_ListDefectInfo[k].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
					resultPanelData.m_ListDefectInfo[k].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE ||
					resultPanelData.m_ListDefectInfo[k].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK )
				{
					vector<Point> vPt_Sub;

					//////////////////////////////////////////////////////////////////////////
					int n = 0;
					while (true)
					{
						if (resultPanelData.m_ListDefectInfo[k].nContoursX[n] != 0 || resultPanelData.m_ListDefectInfo[k].nContoursX[n] != NULL) {
							vPt_Sub.push_back(cv::Point(resultPanelData.m_ListDefectInfo[k].nContoursX[n], resultPanelData.m_ListDefectInfo[k].nContoursY[n]));
							n++;
						}
						else {
							break;
						}

					}
					////////////////////////////////////////////////////////////////////////// 이물 Contour 좌표 Get
					float dSub_r = 0.f;
					cv::Point2f ptSubCenter;

					cv::minEnclosingCircle(vPt_Sub, ptSubCenter, dSub_r);

					//cv::Point2d ptSubCenter(resultPanelData.m_ListDefectInfo[k].Pixel_Center_X, resultPanelData.m_ListDefectInfo[k].Pixel_Center_Y);

					cv::circle(matSrc_Draw, ptSubCenter, 3, Scalar(255, 153, 51), 1); //확인용

					double dDistance_Main = 0.0;

					double dDistance_Sub1 = (double)sqrt(pow(pt1.x - ptSubCenter.x, 2) + pow(pt1.y - ptSubCenter.y, 2));
					double dDistance_Sub2 = (double)sqrt(pow(pt2.x - ptSubCenter.x, 2) + pow(pt2.y - ptSubCenter.y, 2));

					cv::Point2d ptMain(0.0 , 0.0);

					if (dDistance_Sub1 < dDistance_Sub2) {
						dDistance_Main = dDistance_Sub1;
						ptMain = pt1;
					}// 분칩성 불량 끝점과 이물 불량간의 거리 계산
					else {
						dDistance_Main = dDistance_Sub2;
						ptMain = pt2;
					}

					if (dDistance_Main < 40.0) {
						cv::circle(matSrc_Draw, ptSubCenter, 15, Scalar(0, 0, 255), 2); //확인용
						//cv::line(matMask_Active, ptSubCenter, ptMain, cv::Scalar(0, 0, 255), 1);//확인용
						//////////////////////////////////////////////////////////////////////////
						// 거리가 가깝다면 분칩성 불량의 각도로 그은 직선이 이물불량의 원 범위에 접하는지 Judge
						double a = 0.0;
						double b = 0.0;
						double c = 0.0;
						double d = 0.0;

						double Degree_Offset = 0.0;
						double Degree_Min = 0.0;
						double Degree_Max = 0.0;
						double Degree_Step = 0.0;

						Degree_Offset = tan(5.0 *CV_PI / 180.0);
						Degree_Step = (Degree_Offset * 2) / 10.0;

						if (nVertical_mode <= 0) {
							Degree_Min = dAngle - Degree_Offset;
							Degree_Max = dAngle + Degree_Offset;
						}
						else {
							dAngle = 0.0;
							Degree_Min = dAngle - Degree_Offset;
							Degree_Max = dAngle + Degree_Offset;
						}


						for (double dDegree = Degree_Min; dDegree < Degree_Max; dDegree += Degree_Step) {

							if (nVertical_mode <=0) {
								a = dDegree;
								b = -1;
								c = -dDegree*ptCenter.x + ptCenter.y;
								d = abs(a* (double)ptSubCenter.x + b*(double)ptSubCenter.y + c) / sqrt((a*a) + (b*b));
							}
							else {
								a = dDegree;
								b = -1;
								c = -dDegree*ptCenter.y + ptCenter.x;
								d = abs(a* (double)ptSubCenter.y + b*(double)ptSubCenter.x + c) / sqrt((a*a) + (b*b));
							}
							//////////////////////////////////////////////////////////////////////////

							

							cv::Point pt1_draw;
							cv::Point pt2_draw;

							if (nVertical_mode <= 0) {
								vector<double> vt_Y;

								for (int p = 0; p < matSrc_Draw.cols; p++) {

									vt_Y.push_back(-a*p - c);
								}

								pt1_draw.x = 0;
								pt1_draw.y = -vt_Y[0];

								pt2_draw.x = matSrc_Draw.cols - 1;
								pt2_draw.y = -vt_Y[matSrc_Draw.cols - 1];


							}
							else {
								vector<double> vt_X;
								for (int p = 0; p < matSrc_Draw.rows; p++) {

									vt_X.push_back(-a*p - c);
								}

								pt1_draw.x = -vt_X[0];
								pt1_draw.y = 0;

								pt2_draw.x = -vt_X[matSrc_Draw.rows - 1];
								pt2_draw.y = matSrc_Draw.rows - 1;
							}

							cv::line(matSrc_Draw, pt1_draw, pt2_draw, Scalar(0, 0, 255), 1);

							if (d < 3.0) { // 분칩성 불량 각도의 직선과 이물성 불량의 외접원이 겹치면 제거

								cv::circle(matSrc_Draw, ptSubCenter, 20, Scalar(0, 255, 0), 2); //확인용
								cv::line(matSrc_Draw, ptSubCenter, ptMain, cv::Scalar(0, 255, 0), 2);//확인용


								if (i > k) i--;

								resultPanelData.m_ListDefectInfo.RemoveAt(k);

								k--;

								break;
							}

						}
					}
					//////////////////////////////////////////////////////////////////////////
				}
			}

			
			resultPanelData.m_ListDefectInfo.RemoveAt(i); //Judge 쓰고난 분칩성 라인 제거
			i--;
			//////////////////////////////////////////////////////////////////////////
		}
	}

	

	if (theApp.GetCommonParameter()->bIFImageSaveFlag) {

		CString strResult = _T("");
		strResult.Format(_T("%s%sARESULT\\%s\\Final Result\\DeleteChips_Result.bmp"), theApp.m_Config.GetNetworkDrivePath(), strDrive,strPanelID);

		ImageSave(matSrc_Draw, _T("%s"),
			strResult);
	}

	return true;
}

bool AviInspection::DeleteOverlapDefect_LCP_Candidate_Substance(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// CKI 22.02.16 - 불량 후보군 주변 이물 제거
	//////////////////////////////////////////////////////////////////////////

	if (dAlignPara != NULL)
	{
		int		nDefectTypeM, nDefectTypeS;
		int		nAdjustRange = 10;
		CRect	rcDeleteArea;
		CPoint	ptCenterSub;

		//픽셀 불량일때 무라를 제거
		if (nAdjustRange > 0)
		{
			// 불량 갯수 만큼
			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				nDefectTypeM = resultPanelData.m_ListDefectInfo[i].Defect_Type;

				// 대상 불량을 찾는다.
				if (nDefectTypeM == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK)		//후보군 이물

				{
					// P/S 모드 좌표 보정

					rcDeleteArea.SetRect(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

					// 좌표계 정규화
					rcDeleteArea.NormalizeRect();

					// 범위 확장
					rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);

					// 불량 갯수 만큼
					for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
					{
						// 같은 불량 항목 비교 금지
						if (i == j)
						{
							j++;
							continue;
						}

						nDefectTypeS = resultPanelData.m_ListDefectInfo[j].Defect_Type;

						// 해당 불량이 포인트 불량중 하나인지 확인
						if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
						{

							// P/S 모드 좌표 보정
							ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
							ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

							// 범위 안에 들어오면 삭제
							if (rcDeleteArea.PtInRect(ptCenterSub))
							{
								// Point 불량 삭제
								resultPanelData.m_ListDefectInfo[j].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_POINT_DARK;
							}
							else
							{
								j++; //다음불량으로
								continue;
							}
						}
						else j++; // 후보군 이물 불량이 아닐 경우 다음불량으로
					}
				}
			}
		}
	}

	return true;
}

bool AviInspection::DeleteOverlapDefect_LCP_Moisture_Substance(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nDeleteFlag = (int)dAlignPara[E_PARA_LCP_MOISTURE_SUBSTANCE_DELETE_FLAG];

	if (nDeleteFlag <= 0) return true;
	//////////////////////////////////////////////////////////////////////////
	// CKI 22.02.16 - 불량 후보군 주변 이물 제거
	//////////////////////////////////////////////////////////////////////////

	if (dAlignPara != NULL)
	{
		int		nDefectTypeM, nDefectTypeS;
		int		nAdjustRange = 10;
		CRect	rcDeleteArea;
		CPoint	ptCenterSub;

		//픽셀 불량일때 무라를 제거
		if (nAdjustRange > 0)
		{
			// 불량 갯수 만큼
			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				nDefectTypeM = resultPanelData.m_ListDefectInfo[i].Defect_Type;

				// 대상 불량을 찾는다.
				if (nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)		//후보군 이물

				{
					// P/S 모드 좌표 보정

					rcDeleteArea.SetRect(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

					// 좌표계 정규화
					rcDeleteArea.NormalizeRect();

					// 범위 확장
					rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);

					// 불량 갯수 만큼
					for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
					{
						// 같은 불량 항목 비교 금지
						if (i == j)
						{
							j++;
							continue;
						}

						nDefectTypeS = resultPanelData.m_ListDefectInfo[j].Defect_Type;

						// 해당 불량이 포인트 불량중 하나인지 확인
						if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
						{

							// P/S 모드 좌표 보정
							ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
							ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

							// 범위 안에 들어오면 삭제
							if (rcDeleteArea.PtInRect(ptCenterSub))
							{
								// Point 불량 삭제
								resultPanelData.m_ListDefectInfo[j].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_POINT_DARK;
							}
							else
							{
								j++; //다음불량으로
								continue;
							}
						}
						else j++; // 후보군 이물 불량이 아닐 경우 다음불량으로
					}
				}
			}
		}
	}

	return true;
}


bool AviInspection::Report_FinalDefect(cv::Mat matColorImage[][MAX_CAMERA_COUNT], CString strReportPath, CString strPanelID_, double dResolution, int nCamnum, ResultPanelData& resultPanelData, double* dAlignPara) {

	int nMainImage_Num = dAlignPara[E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_MAIN];


	//////////////////////////////////////////////////////////////////////////
	//Excel Data 저장
	//////////////////////////////////////////////////////////////////////////
	int nImageDefectCount = 0;
	// 				BlobFeatureSave_Sub(pResultBlob_Sub, strResultPath, &nImageDefectCount);
	//////////////////////////////////////////////////////////////////////////

	//strResultPath.Format(_T("%s\\%s\\%d_%s_CAM%02d_LCP.csv"), INSP_PATH, strPanelID_, nImageNum, theApp.GetGrabStepName(nImageNum), nCameraNum_);
	CString strExcelPath;
	strExcelPath.Format(_T("%s\\%d_%s_CAM%02d_LCP.csv"), strReportPath, theApp.GetImageNum(nMainImage_Num), theApp.GetGrabStepName(theApp.GetImageNum(nMainImage_Num)), 0);
	//임시 결과데이터 모음
	//strResultPath.Format(_T("D:\\%s.csv"), strPanelID_);

	nImageDefectCount = 0;

	BlobFeatureSave_LCP(resultPanelData, strExcelPath, strPanelID_, dResolution, &nImageDefectCount);


	//////////////////////////////////////////////////////////////////////////
	//결과 Image 저장
	//////////////////////////////////////////////////////////////////////////
	int rect_offset = 20;
	CString strFont;

	

	cv::Mat matReport;

	matColorImage[theApp.GetImageNum(nMainImage_Num)][0].copyTo(matReport);

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (
			//////////////////////////////////////////////////////////////////////////불량군
			//어두운 불량
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||

			//////////////////////////////////////////////////////////////////////////
			//밝은 불량
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||

			////////////////////////////////////////////////////////////////////////// 후보군
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK)
		{

			cv::Rect drawrect = Rect(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - rect_offset, resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y - rect_offset, (resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X) + (rect_offset * 2), (resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y) + (rect_offset * 2));

			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT)
				cv::rectangle(matReport, drawrect, cv::Scalar(255, 0, 0), 3); // Left
			else
				cv::rectangle(matReport, drawrect, cv::Scalar(0, 0, 255), 3); // Left

			strFont.Format(_T("%d"), i + 1);

			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT)
				cv::putText(matReport, (cv::String)(CStringA)strFont, Point(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - rect_offset, resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y - rect_offset - 10), FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(255, 0, 0), 3);
			else
				cv::putText(matReport, (cv::String)(CStringA)strFont, Point(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - rect_offset, resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y - rect_offset - 10), FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(0, 0, 255), 3);

		}
	}

	strFont.Format(_T("%s\\%s_검사확인용.bmp"), strReportPath, (LPCWSTR)strPanelID_);
	cv::imwrite((cv::String)(CStringA)strFont, matReport);

	return true;
}

bool AviInspection::Judgement_DefectColor_Trans(ResultPanelData& resultPanelData, double* dAlignPara)
{
	int nColorCode_Num = dAlignPara[E_PARA_LCP_COLORCODE_NUM];

	

	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	if (nColorCode_Num == E_COLOR_CODE_TRANSPARENCY) {

// 		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
// 		{
// 			if (
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2
// 				)
// 			{
// 				resultPanelData.m_ListDefectInfo[i].strColor = "Red";
// 			}
// 			else if (
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT
// 				)
// 			{
// 				resultPanelData.m_ListDefectInfo[i].strColor = "Red";
// 			}
// 			else if (
// 				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT
// 				)
// 			{
// 				resultPanelData.m_ListDefectInfo[i].strColor = "Blue";
// 			}
// 		}
	}
	else if(nColorCode_Num == E_COLOR_CODE_BLACK){
		
	}
	else {
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			if (
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK
				)
			{
				resultPanelData.m_ListDefectInfo[i].strColor = "Red";
			}
		}
	
	}

	return true;
}

bool AviInspection::Judgement_Defect_Use_Result_Trans(ResultPanelData& resultPanelData, double* dAlignPara)
{
	int nDelete_Flag = dAlignPara[E_PARA_LCP_CANDIDATE_DELETE_FLAG];


	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0 || nDelete_Flag <= 0)
		return true;


	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EMD_DARK)
		{
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
			i--;
		}
	}

	return true;
}