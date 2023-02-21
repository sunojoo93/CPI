#include "stdafx.h"
#include "BumpInspection.h"
#include "BumpDefineInspect.h"
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

IMPLEMENT_DYNCREATE(BumpInspection, CWinThread)

BumpInspection::BumpInspection(void)
{
	//cMemResult[0] = NULL;

	m_pThreadScanMemory = NULL;
	m_pThreadScanArea = NULL;
	m_pThreadScanAlignMark = NULL;
	m_pThreadScanDie = NULL;
	m_pThreadScanInspection = NULL;

	m_eThreadWorkMemory = E_THREAD_STOP;
	m_eThreadWorkArea = E_THREAD_STOP;
	m_eThreadWorkAlignMark = E_THREAD_STOP;
	m_eThreadWorkDie = E_THREAD_STOP;
	m_eThreadWorkInspection = E_THREAD_STOP;
}

BumpInspection::~BumpInspection(void)
{


}

bool BumpInspection::DrawDefectImage(CString strPanelID, cv::Mat(*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], ResultPanelData& resultPanelData)
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
		case E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY:
		case E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK:
		case E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB:
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


		//DC 불량 미니맵에 안그림
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_DC_START	&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_DC_END)
		{
			// 미니맵에 Rect 안그림
			resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
		}

		// PNZ 19.04.23 Mura Normal F Grade Defect Rect 안 그림
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURANORMAL_TYPE2_F_GRADE ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURANORMAL_TYPE3_F_GRADE)
		{
			// 미니맵에 Rect 안그림
			resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
		}

		// Mura - 미니맵에 Rect 안그림
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_MURA_START	&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_MURA_END)
		{
			// 미니맵에 Rect 안그림
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP)
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

			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_LEFT_TOP], ptDefectArea[E_CORNER_RIGHT_TOP], color, nThickness);
			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_RIGHT_TOP], ptDefectArea[E_CORNER_RIGHT_BOTTOM], color, nThickness);
			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_RIGHT_BOTTOM], ptDefectArea[E_CORNER_LEFT_BOTTOM], color, nThickness);
			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_LEFT_BOTTOM], ptDefectArea[E_CORNER_LEFT_TOP], color, nThickness);
		}
	}
	timerDrawRect.End();
	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Draw Rect tact time %.2f"), timerDrawRect.GetTime() / 1000.);

	return true;
}

// 외곽 부분 처리
long BumpInspection::makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio)
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

bool BumpInspection::Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
	const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio,
	ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatOrgImage_Real[][MAX_CAMERA_COUNT], bool bUseInspect, int nStageNo)
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

		// Report 삭제
		JudgementDelReport(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Del Report. Panel ID : %s"), strPanelID);

		//JudgementSpot(resultPanelData);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Spot. Panel ID : %s"), strPanelID);

		// PNZ:17.11.27 먼저 중복 부분 제거후 Line Merge
		// 파라미터 받아오기
		double* dAlignPara = theApp.GetAlignParameter(0);

		////////////////////TIANMA PARAM ////////////////////
		//Parameter
		bool	bGroupAllorEach = dAlignPara[E_PARA_GROUP_TEXT] > 0 ? true : false;  // 0이면 All Pattern, 1이면 R, G, B 각각
		/////////////////////////////////////////////////////

		// Delet Some Line Defect 판정
		// 강라인 주변 약라인 제거
		JudgementDeletLineDefect(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Del Line Defect End. Panel ID : %s - %d"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount());

		// Same Pattern Defect Merge
		JudgementSamePatternDefectMerge(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Same Pattern Defect Merge End. Panel ID : %s"), strPanelID);

		// Line Classification
		JudgementClassifyLineDefect(resultPanelData, MatOrgImage);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Line Defect Classification End. Panel ID : %s"), strPanelID);

		// 6.39QHD Notch Y Line Delete
		JudgementNotchDefect(resultPanelData, MatOrgImage, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - NotchLine Defect End. Panel ID : %s - %d"), strPanelID, resultPanelData.m_ListDefectInfo.GetCount());

		// Crack 판정
		JudgementCrack(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Crack Pattern Judge End. Panel ID : %s"), strPanelID);

		// PCD CRACK 판정 ( 비검사 영역 5.99" )
		JudgementPCDCrack(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - PCD Crack End. Panel ID : %s"), strPanelID);

		// 2018-07-31 Camera Tap에 인한 과검 제거
		JudgementCameraTapOverKill(resultPanelData, MatOrgImage, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Camera Tap OverKill Delet End. Panel ID : %s"), strPanelID);

		// DGS 판정
		/*JudgementNewDGS(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - DGS End. Panel ID : %s"), strPanelID);*/

		//JudgementDGS_Vth(resultPanelData);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - DGS Vth End. Panel ID : %s"), strPanelID);

		// Weak Plan B Test
		//JudgementSpecialGrayLineDefect(resultPanelData);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Plan B End. Panel ID : %s"), strPanelID);

		// 2018-03-16 ver. 0.0.3 PNZ
		// 기포 제거 알고리즘
		DeleteOverlapDefect_DimpleDelet(resultPanelData, MatOrgImage, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Dimple Overkill Defect End. Panel ID : %s"), strPanelID);

		// 2018-04-19 ver. 0.0.3 PNZ
		// 한도내 Spot 제거 알고리즘
		DeleteOverlapDefect_SpecInSpotDelet(resultPanelData, MatOrgImage, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Spot Overkill Defect End. Panel ID : %s"), strPanelID);

		// 2018-05-17 ver. 0.0.1 PNZ
		// 암점 확인 알고리즘
		DeleteOverlapDefect_DustDelet(resultPanelData, MatOrgImage, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Dust Defect End. Panel ID : %s"), strPanelID);

		// 2018-04-19 ver. 0.0.1 PNZ
		// 한도내 Spot 제거 알고리즘
		DeleteOverlapDefect_BlackSmallDelet(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Black Pattern Small Point Defect End. Panel ID : %s"), strPanelID);

		//// LJH 그레이만 보이는 명점 분류 
		PatternPointBrightClassify(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - PatternPointBrightClassify. Panel ID : %s"), strPanelID);

		// 중복 좌표 제거
		DeleteOverlapDefect(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Overlap Defect End. Panel ID : %s"), strPanelID);

		// Black Pattern 기준 Merge 판정
		JudgementBlackPatternMerge(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Black Pattern Merge End. Panel ID : %s"), strPanelID);

		// KSW 17.09.11 - 혼색 많이 발생 -> 개수 많은 경우 ET 판정
		// 내부에서 End 로그 생성
		//JudgementET(resultPanelData, dAlignPara, strPanelID);

		// KSW 17.09.11 - 엣지 부분 명점 -> Pad Bright 판정
		//JudgementEdgePadBright(resultPanelData, dAlignPara);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Edge Pad Bright End. Panel ID : %s"), strPanelID);


		JudgeCHoleJudgment(resultPanelData, tCHoleAlignData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - CHole Judgement End. Panel ID : %s"), strPanelID);

		////// LJH 멀티 라인////////
		JudgeMultLine(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - JudgeMultLine End. Panel ID : %s"), strPanelID);

		JudgeRGBSp3Judgment(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Group Each Pattern End. Panel ID : %s"), strPanelID);

		//20201125 LJH BRIGHT_RGB 스크래치 동일 X 또는 Y 좌표에 여러개가 잡힐경우 제거
		JudgeBrightRGBJudgment(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Bright_RGB_SCRATCH End. Panel ID : %s"), strPanelID);


		int	bBlackOverkill = (int)dAlignPara[E_PARA_AVI_BLACK_OVERKILL_TEXT];

		if (bBlackOverkill > 1)
		{
			DeleteOverlapDefect_Point_Black_RGB(resultPanelData, MatOrgImage_Real, dAlignPara);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Point_Black_RGB End. Panel ID : %s"), strPanelID);
		}
		// 설정된 Panel Size - Resolution 계산
		double dResolution = calcResolution(WrtResultInfo);

		// KSW 17.09.25 - 인접 & 군집 판정
		//JudgeGroup(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
		//JudgeGroupTEST(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);

		// All Pattern 
		if (bGroupAllorEach == 0)
		{
			JudgeGroupJudgment(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Group All Pattern End. Panel ID : %s"), strPanelID);
		}
		// Each Pattern(R, G, B 별도 카운트)
		else if (bGroupAllorEach == 1)
		{
			JudgeGroupJudgmentEach(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Group Each Pattern End. Panel ID : %s"), strPanelID);
		}


		// PNZ 19.04.15 Mura Normal 분류
		JudgementMuraNormalClassification(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - MuraNormal Classification End. Panel ID : %s"), strPanelID);

		// contact 이상시 G64 불량 다발 알람 발생 요청 건 추가
		if (JudgementContactAlram(resultPanelData, dAlignPara) == false)
		{
			theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eAutoRun, eALARMID_ALIGN_FAIL, eALARMTYPE_HEAVY);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Contact Check!. Panel ID : %s"), strPanelID);
		}

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

		// YDS - 임시 로그 추가
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Contours FilePath : %s"), strContoursPath);

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
		JudgeSaveMuraContours(resultPanelData, strContoursPath);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Mura Contours End. Panel ID : %s"), strPanelID);
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
	JudgementDefectSumGrade(resultPanelData);

	TotalDefectJudge(resultPanelData);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judgement End. Panel ID : %s (%s)"), strPanelID, resultPanelData.m_ResultPanel.Judge);

	return true;
}

bool BumpInspection::JudgementPointToLine(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, const int nImageWidth, const int nImageHeight)
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

// bool BumpInspection::JudgeGroup(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
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

// bool BumpInspection::JudgeGroupTEST(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
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

// All Pattern용
bool BumpInspection::JudgeGroupJudgment(ResultPanelData& resultPanelData, cv::Mat(*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
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


	//////////////////////////////////////////////////////////////////////////
	// Count Group Judgment

	// Defect Number Count
	bool	bGroupJudgment_Count = false;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

		switch (nDefectType)
		{
		case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1: nDefect1_Count++; break;
		case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2: nDefect2_Count++; break;
		case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3: nDefect3_Count++; break;
		}
	}

	// 설정 개수 보다 크는지 확인
	if (nDefect1_Count >= nCount_SP_1 || nDefect2_Count >= nCount_SP_2 || nDefect3_Count >= nCount_SP_3)	bGroupJudgment_Count = true;
	else																									bGroupJudgment_Count = false;

	if (bGroupJudgment_Count == true)
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

	return true;
}

// each Pattern(R, G, B 별도 카운트)
bool BumpInspection::JudgeGroupJudgmentEach(ResultPanelData& resultPanelData, cv::Mat(*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
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
//	int		nDefect1_Count		= 0;
//	int		nDefect2_Count		= 0;
//	int		nDefect3_Count		= 0;

	int 	nDefect1_Count_Non[3] = { 0,0,0 }; // R, G, B의 패턴 별 갯수 카운트
	int		nDefect2_Count_Non[3] = { 0,0,0 };
	int 	nDefect3_Count_Non[3] = { 0,0,0 };


	float	nGroupDistance_1st = 0;
	float	nGroupDistance_2st = 0;
	float	nGroupDistance_3st = 0;

	float	nGroupDistance_1nd = 0;
	float	nGroupDistance_2nd = 0;
	float	nGroupDistance_3nd = 0;

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


	//////////////////////////////////////////////////////////////////////////
	// Count Group Judgment

	// Defect Number Count
//	bool	bGroupJudgment_Count = false;
	bool    bGroupJudgment_Count_Array[3] = { false, false, false };


	//for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	//{
	//	int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
	//	int nDefectPattern = resultPanelData.m_ListDefectInfo[i].Pattern_Type;
	//	int nRealPattern = theApp.GetImageClassify(nDefectPattern);
	//	switch (nRealPattern)
	//	{
	//		case E_IMAGE_CLASSIFY_AVI_R:
	//			switch (nDefectType)
	//			{
	//				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1: nDefect1_Count_Non[E_IMAGE_CLASSIFY_AVI_R]++; break;
	//				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2: nDefect2_Count_Non[E_IMAGE_CLASSIFY_AVI_R]++; break;
	//				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3: nDefect3_Count_Non[E_IMAGE_CLASSIFY_AVI_R]++; break;
	//			}

	//		case E_IMAGE_CLASSIFY_AVI_G:
	//			switch (nDefectType)
	//			{
	//				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1: nDefect1_Count_Non[E_IMAGE_CLASSIFY_AVI_G]++; break;
	//				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2: nDefect2_Count_Non[E_IMAGE_CLASSIFY_AVI_G]++; break;
	//				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3: nDefect3_Count_Non[E_IMAGE_CLASSIFY_AVI_G]++; break;
	//			}

	//		case E_IMAGE_CLASSIFY_AVI_B:
	//			switch (nDefectType)
	//			{
	//				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1: nDefect1_Count_Non[E_IMAGE_CLASSIFY_AVI_B]++; break;
	//				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2: nDefect2_Count_Non[E_IMAGE_CLASSIFY_AVI_B]++; break;
	//				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3: nDefect3_Count_Non[E_IMAGE_CLASSIFY_AVI_B]++; break;
	//			}
	//	}
	//}

	//// 설정 개수 보다 크는지 확인
	//if (nDefect1_Count_Non[E_IMAGE_CLASSIFY_AVI_R] >= nCount_SP_1 ||
	//	nDefect2_Count_Non[E_IMAGE_CLASSIFY_AVI_R] >= nCount_SP_2 ||
	//	nDefect3_Count_Non[E_IMAGE_CLASSIFY_AVI_R] >= nCount_SP_3)	bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_R] = true;
	//else															bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_R] = false;

	//// 설정 개수 보다 크는지 확인
	//if (nDefect1_Count_Non[E_IMAGE_CLASSIFY_AVI_G] >= nCount_SP_1 ||
	//	nDefect2_Count_Non[E_IMAGE_CLASSIFY_AVI_G] >= nCount_SP_2 ||
	//	nDefect3_Count_Non[E_IMAGE_CLASSIFY_AVI_G] >= nCount_SP_3)	bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_G] = true;
	//else															bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_G] = false;

	//// 설정 개수 보다 크는지 확인
	//if (nDefect1_Count_Non[E_IMAGE_CLASSIFY_AVI_B] >= nCount_SP_1 || 
	//	nDefect2_Count_Non[E_IMAGE_CLASSIFY_AVI_B] >= nCount_SP_2 || 
	//	nDefect3_Count_Non[E_IMAGE_CLASSIFY_AVI_B] >= nCount_SP_3)	bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_B] = true;
	//else															bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_B] = false;

	////Red
	//if (bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_R] == true)
	//{
	//	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	//	{
	//		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
	//		int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;
	//		int nRealPattern = theApp.GetImageClassify(nPatternType);
	//		if (nRealPattern == E_IMAGE_CLASSIFY_AVI_R)
	//		{
	//			switch (nDefectType)
	//			{
	//			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
	//			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
	//			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:

	//				nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
	//				break;
	//			}
	//			resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;
	//		}
	//	}
	//}
	//else if (bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_G] == true)
	//{
	//	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	//	{
	//		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
	//		int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;
	//		int nRealPattern = theApp.GetImageClassify(nPatternType);
	//		if (nRealPattern == E_IMAGE_CLASSIFY_AVI_G)
	//		{
	//			switch (nDefectType)
	//			{
	//			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
	//			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
	//			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:

	//				nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
	//				break;
	//			}
	//			resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;
	//		}
	//	}
	//}
	//else if (bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_B] == true)
	//{
	//	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	//	{
	//		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
	//		int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;
	//		int nRealPattern = theApp.GetImageClassify(nPatternType);
	//		if (nRealPattern == E_IMAGE_CLASSIFY_AVI_B)
	//		{
	//			switch (nDefectType)
	//			{
	//			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
	//			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
	//			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:

	//				nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
	//				break;
	//			}
	//			resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;
	//		}
	//	}
	//}


	//////////////////////////////////////////////////////////////////////////
	// Distance Group Judgement
	// 갯수 카운팅용
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		bool bGroupJudgment_Distance = false;
		bool bGroupJudgment_Distance2 = false;
		bool bGroupJudgment_Distance3 = false;

		int nDefectCount1 = 0;
		int nDefectCount2 = 0;
		int nDefectCount3 = 0;

		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
		int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;
		int nRealPattern = theApp.GetImageClassify(nPatternType);
		// Point 아닌 불량 제외
		if (nDefectType < E_DEFECT_JUDGEMENT_POINT_DARK || nDefectType > E_DEFECT_JUDGEMENT_POINT_GROUP_DARK) continue;

		// Parameter 초기화
		nGroupDistance_1st = 0;
		nGroupDistance_2st = 0;
		nGroupDistance_3st = 0;

		// POINT Dark 불량 이면 해당 Distance 선택
		if (nDefectType == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1) nGroupDistance_1st = nDistance_SP_1;
		if (nDefectType == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2) nGroupDistance_2st = nDistance_SP_2;
		if (nDefectType == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) nGroupDistance_3st = nDistance_SP_3;
		if (nDefectType == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK) nGroupDistance_3st = nDistance_SP_3;
		// Temp Defect Center in MM
		CPoint ptTemp((int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio,
			(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
		{
			if (j == i) continue;

			bGroupJudgment_Distance = false;
			bGroupJudgment_Distance2 = false;
			bGroupJudgment_Distance3 = false;

			int nDefectType2 = resultPanelData.m_ListDefectInfo[j].Defect_Type;
			int nPatternType2 = resultPanelData.m_ListDefectInfo[j].Pattern_Type;

			if (nPatternType != nPatternType2) continue;

			// Point 아닌 불량 제외
			if (nDefectType2 < E_DEFECT_JUDGEMENT_POINT_DARK || nDefectType2 > E_DEFECT_JUDGEMENT_POINT_GROUP_DARK) continue;

			// Parameter 초기화
			nGroupDistance_1nd = 0;
			nGroupDistance_2nd = 0;
			nGroupDistance_3nd = 0;

			// POINT Dark 불량 이면 해당 Distance 선택
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1) nGroupDistance_1nd = nDistance_SP_1;
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2) nGroupDistance_2nd = nDistance_SP_2;
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) nGroupDistance_3nd = nDistance_SP_3;
			if (nDefectType == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK && nGroupDistance_1st > 0) nGroupDistance_1nd = nDistance_SP_1;
			if (nDefectType == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK && nGroupDistance_2st > 0) nGroupDistance_2nd = nDistance_SP_2;
			if (nDefectType == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK && nGroupDistance_3st > 0) nGroupDistance_3nd = nDistance_SP_3;
			// Compair Defect Center in MM
			CPoint ptTemp2((int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio,
				(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// dX, dY in MM
			float fTempX = (float)(ptTemp2.x - ptTemp.x);
			float fTempY = (float)(ptTemp2.y - ptTemp.y);

			// 불량 그룹 중심으로부터 길이 (MM)
			float fTempLength = (float)(sqrt(fTempX * fTempX + fTempY * fTempY)* dResolution);

			// 비교 불량 중 하나가 Dark SP 123 일 때
			if (nGroupDistance_1st > 0)
			{
				if (fTempLength <= nGroupDistance_1nd)	bGroupJudgment_Distance = true;
				nDefectCount1++;
			}
			else if (nGroupDistance_2st > 0)
			{
				if (fTempLength <= nGroupDistance_2nd)	bGroupJudgment_Distance = true;
				nDefectCount2++;
			}
			else if (nGroupDistance_3st > 0)
			{
				if (fTempLength <= nGroupDistance_3nd)	bGroupJudgment_Distance = true;
				nDefectCount3++;
			}
			else
				continue;
		}
		///////////////////////////// 불량 변경
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
		{
			if (j == i) continue;

			bGroupJudgment_Distance = false;
			bGroupJudgment_Distance2 = false;
			bGroupJudgment_Distance3 = false;

			int nDefectType2 = resultPanelData.m_ListDefectInfo[j].Defect_Type;
			int nPatternType2 = resultPanelData.m_ListDefectInfo[j].Pattern_Type;

			if (nPatternType != nPatternType2) continue;

			// Point 아닌 불량 제외
			if (nDefectType2 < E_DEFECT_JUDGEMENT_POINT_DARK || nDefectType2 > E_DEFECT_JUDGEMENT_POINT_GROUP_DARK) continue;

			// Parameter 초기화
			nGroupDistance_1nd = 0;
			nGroupDistance_2nd = 0;
			nGroupDistance_3nd = 0;

			// POINT Dark 불량 이면 해당 Distance 선택
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1) nGroupDistance_1nd = nDistance_SP_1;
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2) nGroupDistance_2nd = nDistance_SP_2;
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) nGroupDistance_3nd = nDistance_SP_3;
			if (nDefectType == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK && nGroupDistance_1st > 0) nGroupDistance_1nd = nDistance_SP_1;
			if (nDefectType == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK && nGroupDistance_2st > 0) nGroupDistance_2nd = nDistance_SP_2;
			if (nDefectType == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK && nGroupDistance_3st > 0) nGroupDistance_3nd = nDistance_SP_3;

			// Compair Defect Center in MM
			CPoint ptTemp2((int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio,
				(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// dX, dY in MM
			float fTempX = (float)(ptTemp2.x - ptTemp.x);
			float fTempY = (float)(ptTemp2.y - ptTemp.y);

			// 불량 그룹 중심으로부터 길이 (MM)
			float fTempLength = (float)(sqrt(fTempX * fTempX + fTempY * fTempY)* dResolution);

			// 비교 불량 중 하나가 Dark SP 123 일 때
			if (nGroupDistance_1st > 0)
			{
				if (fTempLength <= nGroupDistance_1nd)	bGroupJudgment_Distance = true;
			}
			else if (nGroupDistance_2st > 0)
			{
				if (fTempLength <= nGroupDistance_2nd)	bGroupJudgment_Distance2 = true;
			}
			else if (nGroupDistance_3st > 0)
			{
				if (fTempLength <= nGroupDistance_3nd)	bGroupJudgment_Distance3 = true;
			}
			else
				continue;

			if (bGroupJudgment_Distance == true && nDefectCount1 + 1 >= nCount_SP_1)
			{
				switch (nDefectType2)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:

					nDefectType2 = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
					break;
				}
				resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType2;
				resultPanelData.m_ListDefectInfo[j].Defect_Type = nDefectType2;
			}
			else if (bGroupJudgment_Distance2 == true && nDefectCount2 + 1 >= nCount_SP_2)
			{
				switch (nDefectType2)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:

					nDefectType2 = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
					break;
				}
				resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType2;
				resultPanelData.m_ListDefectInfo[j].Defect_Type = nDefectType2;
			}
			else if (bGroupJudgment_Distance3 == true && nDefectCount3 + 1 >= nCount_SP_3)
			{
				switch (nDefectType2)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:

					nDefectType2 = E_DEFECT_JUDGEMENT_POINT_GROUP_DARK;
					break;
				}
				resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType2;
				resultPanelData.m_ListDefectInfo[j].Defect_Type = nDefectType2;
			}
			else
				continue;
		}

	}
	//////////////////////////////////////////////////////////////////////////
	// Delete Dark POINT 123

	// 다 Delete 하지 않을 시 이름 그래도 
	if (nDeleteOnOff_SP_1 + nDeleteOnOff_SP_2 + nDeleteOnOff_SP_3 == 0) return true;

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

	return true;
}

bool BumpInspection::JudgeRGBSp3Judgment(ResultPanelData & resultPanelData, double * dAlignPara)
{
	// 예외처리
	if (dAlignPara == NULL)	return false;
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;

	int	nRedMax = (int)dAlignPara[E_PARA_AVI_DARK_SP3_RED];
	int	nGreenMax = (int)dAlignPara[E_PARA_AVI_DARK_SP3_GREEN];
	int	nBlueMax = (int)dAlignPara[E_PARA_AVI_DARK_SP3_BLUE];
	int	nWhiteMax = (int)dAlignPara[E_PARA_AVI_DARK_SP3_WHITE];
	int	nG64Max = (int)dAlignPara[E_PARA_AVI_DARK_SP3_G64];

	int nCountRedSp3 = 0;
	int nCountGreenSp3 = 0;
	int nCountBlueSp3 = 0;
	int nCountWhiteSp3 = 0;
	int nCountG64Sp3 = 0;

	//R,G,B 패턴에 대해 SP3가 몇개 있는지 카운트
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
		int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

		if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_R)
		{
			switch (nDefectType)
			{
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
				nCountRedSp3++;
				break;
			}
		}
		else if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_G)
		{
			switch (nDefectType)
			{
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
				nCountGreenSp3++;
				break;
			}
		}
		else if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_B)
		{
			switch (nDefectType)
			{
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
				nCountBlueSp3++;
				break;
			}
		}
		else if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_WHITE)
		{
			switch (nDefectType)
			{
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
				nCountWhiteSp3++;
				break;
			}
		}
		else if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_GRAY_64)
		{
			switch (nDefectType)
			{
			case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
				nCountG64Sp3++;
				break;
			}
		}
	}
	//위에서 카운트 한 갯수를 토대로 그룹 변경 고객사 요청으로 불량 카운트 한개만 남기고 나머지 전부 삭제
	//RED 패턴
	if (nCountRedSp3 >= nRedMax)
	{
		// 불량 갯수 만큼
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
			int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

			if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_R)
			{
				switch (nDefectType)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DARK_RED;
					break;
				}
			}
		}
	}
	//GREEN 패턴
	if (nCountGreenSp3 >= nGreenMax)
	{
		// 불량 갯수 만큼
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
			int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

			if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_G)
			{
				switch (nDefectType)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DARK_GREEN;
					break;
				}
			}
		}
	}
	//BLUE 패턴
	if (nCountBlueSp3 >= nBlueMax)
	{
		// 불량 갯수 만큼
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
			int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

			if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_B)
			{
				switch (nDefectType)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DARK_BLUE;
					break;
				}
			}
		}
	}
	if (nCountWhiteSp3 >= nWhiteMax)
	{
		// 불량 갯수 만큼
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
			int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

			if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_WHITE)
			{
				switch (nDefectType)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DARK_WHITE;
					break;
				}
			}
		}
	}
	if (nCountG64Sp3 >= nG64Max)
	{
		// 불량 갯수 만큼
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
			int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

			if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_GRAY_64)
			{
				switch (nDefectType)
				{
				case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DARK_G64;
					break;
				}
			}
		}
	}

	return false;
}

bool BumpInspection::JudgeBrightRGBJudgment(ResultPanelData & resultPanelData, double * dAlignPara)
{
	int nOnOff = (int)dAlignPara[E_PARA_AVI_POINT_BRIGHT_RGB_SCRATCH_TEXT];
	int nPointCount = (int)dAlignPara[E_PARA_AVI_POINT_BRIGHT_RGB_SCRATCH_COUNT];
	int nOffset = (int)dAlignPara[E_PARA_AVI_POINT_BRIGHT_RGB_SCRATCH_OFFSET];

	int nImageWidth = m_stThrdAlignInfo.rcAlignCellROI.width;
	//int nImageHeight = m_stThrdAlignInfo.rcAlignCellROI.height;

	if (nOnOff == 0)
		return true;
	int* nProjection_R_X = new int[nImageWidth];
	int* nProjection_G_X = new int[nImageWidth];
	int* nProjection_B_X = new int[nImageWidth];
	//int* nProjectionY = new int[nImageHeight];

	// 배열 초기화
	memset(nProjection_R_X, 0, sizeof(int) * nImageWidth);
	memset(nProjection_G_X, 0, sizeof(int) * nImageWidth);
	memset(nProjection_B_X, 0, sizeof(int) * nImageWidth);
	//memset(nProjectionY, 0, sizeof(int) * nImageHeight);


	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Point 불량 아닌것 제외
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB)
			continue;

		int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;
		if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_R)
			nProjection_R_X[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X]++; // 해당 좌표 1증가

		else if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_G)
			nProjection_G_X[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X]++; // 해당 좌표 1증가

		else if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_B)
			nProjection_B_X[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X]++; // 해당 좌표 1증가
	//nProjectionY[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y]++;
	}
	for (int i = 0; i < nImageWidth; i++)
	{
		int nROpen = 0;
		int nGOpen = 0;
		int nBOpen = 0;
		int nRPointCountX = 0;
		int nGPointCountX = 0;
		int nBPointCountX = 0;
		// 불량 리스트 번호 추가
		if (nProjection_R_X[i] >= nPointCount)
		{
			nRPointCountX = i;
			nROpen = 1;
		}
		if (nProjection_G_X[i] >= nPointCount)
		{
			nGPointCountX = i;
			nGOpen = 1;
		}
		if (nProjection_B_X[i] >= nPointCount)
		{
			nBPointCountX = i;
			nBOpen = 1;
		}
		if (nROpen == 1)
		{
			// 불량 갯수 만큼
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
			{
				int nPatternType = resultPanelData.m_ListDefectInfo[j].Pattern_Type;
				if (theApp.GetImageClassify(nPatternType) != E_IMAGE_CLASSIFY_AVI_R)
					continue;
				CPoint nCenterX1;
				// POINT_BRIGHT_RGB 불량 아닌 경우 제외
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB)
				{
					continue;
				}
				nCenterX1.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;

				int nX1 = nCenterX1.x;
				// 범위 안에 불량 존재?
				if (fabs(nX1 - nRPointCountX) <= nOffset)
				{
					// 해당 불량 삭제
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
					j--;
				}
				//else j++;  // 다음 불량으로...
			}
		}
		////////////G 패턴
		if (nBOpen == 1)
		{
			// 불량 갯수 만큼
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
			{
				int nPatternType = resultPanelData.m_ListDefectInfo[j].Pattern_Type;
				if (theApp.GetImageClassify(nPatternType) != E_IMAGE_CLASSIFY_AVI_B)
					continue;
				CPoint nCenterX1;
				// POINT_BRIGHT_RGB 불량 아닌 경우 제외
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB)
				{
					continue;
				}
				nCenterX1.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;

				int nX1 = nCenterX1.x;
				// 범위 안에 불량 존재?
				if (fabs(nX1 - nBPointCountX) <= nOffset)
				{
					// 해당 불량 삭제
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
					j--;
				}
				//else j++;  // 다음 불량으로...
			}
		}
		//////////////////B 패턴
		if (nGOpen == 1)
		{
			// 불량 갯수 만큼
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
			{
				int nPatternType = resultPanelData.m_ListDefectInfo[j].Pattern_Type;
				if (theApp.GetImageClassify(nPatternType) != E_IMAGE_CLASSIFY_AVI_G)
					continue;
				CPoint nCenterX1;
				// POINT_BRIGHT_RGB 불량 아닌 경우 제외
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB)
				{
					continue;
				}
				nCenterX1.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;

				int nX1 = nCenterX1.x;
				// 범위 안에 불량 존재?
				if (fabs(nX1 - nGPointCountX) <= nOffset)
				{
					// 해당 불량 삭제
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
					j--;
				}
				//else j++;  // 다음 불량으로...
			}
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// PNZ 2019-04-15
// Mura Normal 불량 분류
//////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementMuraNormalClassification(ResultPanelData& resultPanelData, double* dAlignPara)
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
		case E_DEFECT_JUDGEMENT_MURA_LINEMURA_X:		nMuraType2_Count++; break;
		case E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y:		nMuraType2_Count++; break;
		case E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_X:	nMuraType2_Count++; break;
		case E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_Y:	nMuraType2_Count++; break;
		case E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG:			nMuraType3_Count++; break;
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
			if (nDefectType == E_DEFECT_JUDGEMENT_MURA_LINEMURA_X ||
				nDefectType == E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y ||
				nDefectType == E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_X ||
				nDefectType == E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_Y)
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
// Contact 이상시 G64 불량 다발 관련 알람 발생 
//////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementContactAlram(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// 예외처리
	if (dAlignPara == NULL)									return false;
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;

	//////////////////////////////////////////////////////////////////////////
	// Type 별 Classify Parameters
	int	nG64Defect = (int)dAlignPara[E_PARA_AVI_G64_DEFECT_COUNT];

	int nG64DefectCnt = 0;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

		switch (nDefectType)
		{
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT_MULT:		nG64DefectCnt++; break;
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK_MULT:		nG64DefectCnt++; break;
		case E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY:	nG64DefectCnt++; break;
		case E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK:	nG64DefectCnt++; break;
		}
	}

	if (nG64DefectCnt > nG64Defect)
	{
		return false;
	}

	return true;

}

long BumpInspection::StartLogicAlgorithm(const CString strDrive, const tLogicPara& LogicPara, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
	cv::Mat& MatDrawBuffer, const int nImageNum, const int nROINumber, const int nAlgorithmNumber, tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, bool bpInspectEnd[][MAX_CAMERA_COUNT], int nRatio, bool *bPattern, bool *nCount, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	// 파라미터 설정
	cv::Mat				MatOriginImage_ = LogicPara.MatOrgImage;
	int					nAlgorithmNumber_ = nAlgorithmNumber;					// 현재 알고리즘	
	int					nCameraNum_ = LogicPara.nCameraNum;				// 검사 카메라
	int					nThrdIndex_ = LogicPara.nThreadLog;
	cv::Mat				*MatOriImageRGB_[3];
	cv::Mat				MatBKG_ = LogicPara.MatBKG;					// Mura에서 사용 ( Cell 이외 영역 ) / Dust, Black 패턴은 없음

	MatOriImageRGB_[0] = LogicPara.MatOrgRGBAdd[0];
	MatOriImageRGB_[1] = LogicPara.MatOrgRGBAdd[1];
	MatOriImageRGB_[2] = LogicPara.MatOrgRGBAdd[2];

	ENUM_KIND_OF_LOG	eLogCamera = (ENUM_KIND_OF_LOG)nCameraNum_;

	// 현재 패널 ID
	CString				strPanelID_;
	strPanelID_.Format(_T("%s"), LogicPara.tszPanelID);

	CString arrStrAnalysisData[4];

	arrStrAnalysisData[0] = strPanelID_;
	arrStrAnalysisData[1] = theApp.GetCurModelID();
	arrStrAnalysisData[2] = theApp.GetGrabStepName(nImageNum);
	arrStrAnalysisData[3] = theApp.GetAlgorithmName(nAlgorithmNumber_);

	double dResolution = calcResolution(WrtResultInfo);

	float fUnif;


	long lErrorCode = E_ERROR_CODE_TRUE;

	// 알고리즘 결과 저장 폴더 생성
	SYSTEMTIME time;
	::GetLocalTime(&time);

	// 개별 알고리즘 검사 파라미터 가져오기
	double* dAlgPara = theApp.GetAlgorithmParameter(0, nCameraNum_, nROINumber, nAlgorithmNumber_);
	double*	dAlignPara = theApp.GetAlignParameter(nCameraNum_);
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

	//////////////////////////////////////////////////////////////////////////
	// 임시 Round Cell 확인 용
	//CString strTem;
	//strTem.Format(_T("D:\\%02d.bmp"), theApp.GetImageClassify(nImageNum));
	//cv::imwrite((cv::String)(CStringA)strTem, MatOriginImage_);
	//return true;
	//////////////////////////////////////////////////////////////////////////
	CString strResultTime;
	strResultTime.Format(_T("%s%sARESULT"), theApp.m_Config.GetNetworkDrivePath(), strDrive);
	if (nCommonPara[1] > 0) CreateDirectory(strResultTime, NULL);

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
	if (nCommonPara[1] > 0) CreateDirectory(wstrAlgPath, NULL);

	// Alg 로그용
	wchar_t strAlgLog[MAX_PATH] = { 0, };
	swprintf(strAlgLog, _T("ID:%s\tPat:%s"), (LPCWSTR)strPanelID_, theApp.GetGrabStepName(nImageNum));

	CString strInspPath = INSP_PATH;
	//GijunAdd 220608
	CString strWaferMapPath = WAFERMAP_PATH;
	//////////////////////////////////////////////////////////////////////////
	// Result Buff 생성 : LINE이 아닌 경우에 만 사용
	// 버퍼 할당 & 초기화

	cv::Mat matReusltImage[E_DEFECT_COLOR_COUNT];
	cv::Mat matReuslt_Mura_Normal_Image[E_DEFECT_TOTAL_COUNT];
	cv::Mat matDustReusltImage[E_DEFECT_DUST_COUNT];
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

	stInspectThreadParameter stParameter;
	int nEmptyBufferX = 136;
	int nEmptyBufferY = 136;
	stParameter.matOrgImage = MatOriginImage_;
	stParameter.vecmatAlignMark = LogicPara.vecmatAlignMark;
	stParameter.vecmatAlignMarkInvers = LogicPara.vecmatAlignMarkInvers;
	stParameter.nFindDieROI = dAlgPara[E_BUMP_SCANDIE];
	stParameter.nFristPositionThreshold = dAlgPara[E_BUMP_SCAN_AREA_BRIGHT];
	stParameter.fInspectionCheckArea = dAlgPara[E_BUMP_CHECK_AREA_RATIO];
	stParameter.nCutROIX = (int)round((dAlignPara[E_PARA_BUMP_DIE_WIDTH] * 1000 / dResolution) / 2);
	stParameter.nCutROIY = (int)round((dAlignPara[E_PARA_BUMP_DIE_HEIGHT] * 1000 / dResolution) / 2);
	stParameter.nAlignMarkDistanceX = dAlignPara[E_PARA_BUMP_DIE_WIDTH] * 1000;
	stParameter.nAlignMarkDistanceY = dAlignPara[E_PARA_BUMP_DIE_HEIGHT] * 1000;
	stParameter.nAlignSearchPitch = dAlgPara[E_BUMP_ALIGNMARK_SEARCHPITCH];
	stParameter.nDiffSearchX = dAlgPara[E_BUMP_DIFFSEARCH_X];
	stParameter.nDiffSearchY = dAlgPara[E_BUMP_DIFFSEARCH_Y];
	stParameter.bSaveImage = theApp.GetCommonParameter()->bIFImageSaveFlag;
	stParameter.nBumpMode = 1;
	stParameter.nCamScanDie = dAlignPara[E_PARA_BUMP_CAM_SCAN_COUNT];
	stParameter.nFullImageWidth = MatOriginImage_.cols;
	stParameter.nFullImageHeight = MatOriginImage_.rows;
	stParameter.nAlignMarkType = E_NOMAL_VERTEXT;
		
	stParameter.strFolderPathMark = strResultTime + _T("\\") + strPanelID_ + _T("\\Mark");
	stParameter.strFolderPathExtractDie = strResultTime + _T("\\") + strPanelID_ + _T("\\ExtractDie");
	stParameter.strFolderPathInspection = strResultTime + _T("\\") + strPanelID_ + _T("\\Inspection");
	// GijunAdd 220608
	stParameter.strFolderPathWaferMap = strWaferMapPath + _T("Sample#4.txt");

	SHCreateDirectoryEx(NULL, stParameter.strFolderPathMark, NULL);
	SHCreateDirectoryEx(NULL, stParameter.strFolderPathExtractDie, NULL);
	SHCreateDirectoryEx(NULL, stParameter.strFolderPathInspection, NULL);
	
	stParameter.vecInsideDieAlignMarkDistance.push_back(cv::Point(dAlignPara[E_PARA_BUMP_DIE_WIDTH] * 1000 - nEmptyBufferX, 0));
	stParameter.vecInsideDieAlignMarkDistance.push_back(cv::Point(dAlignPara[E_PARA_BUMP_DIE_WIDTH] * 1000 - nEmptyBufferX, dAlignPara[E_PARA_BUMP_DIE_HEIGHT] * 1000 - nEmptyBufferY));
	stParameter.vecInsideDieAlignMarkDistance.push_back(cv::Point(-(dAlignPara[E_PARA_BUMP_DIE_WIDTH] * 1000 - nEmptyBufferX), 0));

	stParameter.nInspectionScanDie = dAlgPara[E_BUMP_INSPECT_SCAN_DIE];

	// 어떻게 추가해야할지 생각...
	stParameter.nCutROICount = stParameter.nFullImageHeight / stParameter.nFindDieROI;
	stParameter.nCutROIRemainder = stParameter.nFullImageHeight % stParameter.nFindDieROI;

	stParameter.vecnMapMemory.resize(stParameter.nCutROICount);
	stParameter.vecnMapSearch.resize(stParameter.nCutROICount);

	int nWaferDieMapWidth = 0;
	int nWaferDieMapHeight = 0;

	stParameter.fDieWidth = dAlignPara[E_PARA_BUMP_DIE_WIDTH];
	stParameter.fDieHeight = dAlignPara[E_PARA_BUMP_DIE_HEIGHT];
	stParameter.fWaferDiameter_inch = dAlignPara[E_PARA_BUMP_DIAMETER_INCH];
	stParameter.fWaferDiameter = dAlignPara[E_PARA_BUMP_DIAMETER_MM];
	stParameter.fEdgeLoss = dAlignPara[E_PARA_BUMP_EDGE_LOSS];
	stParameter.nEdgeLossShift = dAlignPara[E_PARA_BUMP_EDGE_LOSS_SHIFT_USE];
	stParameter.fOffsetHeight = dAlignPara[E_PARA_BUMP_OFFSET_WIDTH];
	stParameter.fOffsetWidth = dAlignPara[E_PARA_BUMP_OFFSET_HEIGHT];
	stParameter.bImageDirection = stThrdAlignInfo.bLineDirection;

	int i = 0;
	// 처음장 촬영시에만 생성
	if (nImageNum == STANDARD_IMAGE)
	{
		if (0)
		{
			double*	dAlignPara = theApp.GetAlignParameter(0);
			int nOffset = 100;
			theApp.g_structMapDie = CreateWaferDieMap(E_CENTER_DIE,
				stParameter.fDieWidth,
				stParameter.fDieHeight,
				stParameter.fWaferDiameter_inch,
				stParameter.fWaferDiameter,
				stParameter.fEdgeLoss,
				stParameter.nEdgeLossShift,
				stParameter.fOffsetHeight,
				stParameter.fOffsetWidth,
				nOffset);
		}
		else
		{
			theApp.g_structMapDie = CreateWaferDieMap(
				stParameter.strFolderPathWaferMap,
				stParameter.fDieWidth,
				stParameter.fDieHeight);
		}
	}
	else
	{

	}

	stParameter.nMapWidth = theApp.g_structMapDie[0][0].nMapWidth;
	stParameter.nMapHeight= theApp.g_structMapDie[0][0].nMapHeight;
	stParameter.nImageNum = nImageNum;
	stParameter.structMapDie = theApp.g_structMapDie;

	// Map Y 시작지점과 끝지점 
	// 검사하는 영역의 Good Die 개수 입력
	setMapData(&stParameter);
	// memory check Thread 한개만
	StartThreadScanMemory(&stParameter);
	// Inspection Area Data check Thread 한개만 처음 영역 찾고 
	StartThreadScanArea(&stParameter);
	// Map Check Thread 한개만
	StartThreadScanAlignMark(&stParameter);
	// AlignMark로 Die 추출
	StartThreadScanDie(&stParameter);
	
	if (nImageNum == STANDARD_IMAGE)
	{
		// Inspection Thread 
		StartThreadInspectionBump(&stParameter);
	}
	
	// 끝날때까지 대기
	DWORD dwMemory;
	DWORD dwArea;
	DWORD dwAlignMark;
	DWORD dwScanDie;
	DWORD dwScanInspection;

	dwMemory = WaitForSingleObject(m_pThreadScanMemory->m_hThread, INFINITE);
	dwArea = WaitForSingleObject(m_pThreadScanArea->m_hThread, INFINITE);
	dwAlignMark = WaitForSingleObject(m_pThreadScanAlignMark->m_hThread, INFINITE);
	dwScanDie = WaitForSingleObject(m_pThreadScanDie->m_hThread, INFINITE);
	if (nImageNum == STANDARD_IMAGE)
		dwScanInspection = WaitForSingleObject(m_pThreadScanInspection->m_hThread, INFINITE);

	if (nImageNum == STANDARD_IMAGE)
	{
		// 전부다 끝났다면 Thread 종료
		if (WAIT_OBJECT_0 == dwMemory		&&
			WAIT_OBJECT_0 == dwArea			&&
			WAIT_OBJECT_0 == dwAlignMark	&&
			WAIT_OBJECT_0 == dwScanDie		&&
			WAIT_OBJECT_0 == dwScanInspection)
		{
			StopThreadScanMemory();
			StopThreadScanArea();
			StopThreadScanAlignMark();
			StopThreadScanDie();
			StopThreadInspectionBump();
		}
	}
	else
	{
		// 전부다 끝났다면 Thread 종료
		if (WAIT_OBJECT_0 == dwMemory		&&
			WAIT_OBJECT_0 == dwArea			&&
			WAIT_OBJECT_0 == dwAlignMark	&&
			WAIT_OBJECT_0 == dwScanDie		)
		{
			StopThreadScanMemory();
			StopThreadScanArea();
			StopThreadScanAlignMark();
			StopThreadScanDie();
		}
	}

	// 취합..


	switch (nAlgorithmNumber_)
	{
	case E_ALGORITHM_BUMP:
	{

	}
	break;
	default:
		break;
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
/*
long BumpInspection::StartLogicAlgorithm(const CString strDrive, const tLogicPara& LogicPara, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
	cv::Mat& MatDrawBuffer, const int nImageNum, const int nROINumber, const int nAlgorithmNumber, tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, bool bpInspectEnd[][MAX_CAMERA_COUNT], int nRatio, bool *bPattern, bool *nCount, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	// 파라미터 설정
	cv::Mat				MatOriginImage_ = LogicPara.MatOrgImage;
	int					nAlgorithmNumber_ = nAlgorithmNumber;					// 현재 알고리즘	
	int					nCameraNum_ = LogicPara.nCameraNum;				// 검사 카메라
	int					nThrdIndex_ = LogicPara.nThreadLog;
	cv::Mat				*MatOriImageRGB_[3];
	cv::Mat				MatBKG_ = LogicPara.MatBKG;					// Mura에서 사용 ( Cell 이외 영역 ) / Dust, Black 패턴은 없음

	MatOriImageRGB_[0] = LogicPara.MatOrgRGBAdd[0];
	MatOriImageRGB_[1] = LogicPara.MatOrgRGBAdd[1];
	MatOriImageRGB_[2] = LogicPara.MatOrgRGBAdd[2];

	ENUM_KIND_OF_LOG	eLogCamera = (ENUM_KIND_OF_LOG)nCameraNum_;

	// 현재 패널 ID
	CString				strPanelID_;
	strPanelID_.Format(_T("%s"), LogicPara.tszPanelID);

	CString arrStrAnalysisData[4];


	arrStrAnalysisData[0] = strPanelID_;
	arrStrAnalysisData[1] = theApp.GetCurModelID();
	arrStrAnalysisData[2] = theApp.GetGrabStepName(nImageNum);
	arrStrAnalysisData[3] = theApp.GetAlgorithmName(nAlgorithmNumber_);

	double dResolution = calcResolution(WrtResultInfo);

	float fUnif;


	long lErrorCode = E_ERROR_CODE_TRUE;

	// 알고리즘 결과 저장 폴더 생성
	SYSTEMTIME time;
	::GetLocalTime(&time);

	// 개별 알고리즘 검사 파라미터 가져오기
	double* dAlgPara = theApp.GetAlgorithmParameter(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);
	double*	dAlignPara = theApp.GetAlignParameter(nCameraNum_);
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

	//////////////////////////////////////////////////////////////////////////
	// 임시 Round Cell 확인 용
	//CString strTem;
	//strTem.Format(_T("D:\\%02d.bmp"), theApp.GetImageClassify(nImageNum));
	//cv::imwrite((cv::String)(CStringA)strTem, MatOriginImage_);
	//return true;
	//////////////////////////////////////////////////////////////////////////
	CString strResultTime;
	strResultTime.Format(_T("%s%sARESULT"), theApp.m_Config.GetNetworkDrivePath(), strDrive);
	if (nCommonPara[1] > 0) CreateDirectory(strResultTime, NULL);

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
	if (nCommonPara[1] > 0) CreateDirectory(wstrAlgPath, NULL);

	// Alg 로그용
	wchar_t strAlgLog[MAX_PATH] = { 0, };
	swprintf(strAlgLog, _T("ID:%s\tPat:%s"), (LPCWSTR)strPanelID_, theApp.GetGrabStepName(nImageNum));

	CString strInspPath = INSP_PATH;
	//////////////////////////////////////////////////////////////////////////
	// Result Buff 생성 : LINE이 아닌 경우에 만 사용
	// 버퍼 할당 & 초기화

	cv::Mat matReusltImage[E_DEFECT_COLOR_COUNT];
	cv::Mat matReuslt_Mura_Normal_Image[E_DEFECT_TOTAL_COUNT];
	cv::Mat matDustReusltImage[E_DEFECT_DUST_COUNT];
	CMatResultBuf* ResultBuf;

	if (nAlgorithmNumber_ != E_ALGORITHM_LINE)
	{
		ResultBuf = cMemResultBuff.FindFreeBuf_Result();

		SetMem_Result(ResultBuf);

		// 버퍼 할당
		matReusltImage[E_DEFECT_COLOR_DARK] = cMemResult->GetMat_Result(MatOriginImage_.size(), MatOriginImage_.type(), false);
		matReusltImage[E_DEFECT_COLOR_BRIGHT] = cMemResult->GetMat_Result(MatOriginImage_.size(), MatOriginImage_.type(), false);
		matReuslt_Mura_Normal_Image[E_DEFECT_TYPE2_WEAK_X] = cMemResult->GetMat_Result(MatOriginImage_.size(), MatOriginImage_.type(), false);
		matReuslt_Mura_Normal_Image[E_DEFECT_TYPE2_WEAK_Y] = cMemResult->GetMat_Result(MatOriginImage_.size(), MatOriginImage_.type(), false);
		matReuslt_Mura_Normal_Image[E_DEFECT_TYPE1_WEAK_X] = cMemResult->GetMat_Result(MatOriginImage_.size(), MatOriginImage_.type(), false);
		matReuslt_Mura_Normal_Image[E_DEFECT_TYPE1_WEAK_Y] = cMemResult->GetMat_Result(MatOriginImage_.size(), MatOriginImage_.type(), false);
		// Buff Set Start Log
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s BuffSet. PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
			theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));
	}

	//////////////////////////////////////////////////////////////////////////
	// 알고리즘 DLL 실행
	//////////////////////////////////////////////////////////////////////////

	switch (nAlgorithmNumber_)
	{
	case E_ALGORITHM_POINT:
	{
		// C#에서 C++과 cv::Mat 구조가 달라 버퍼, 크기 따로 파라미터 넘겼었음.
		// MFC 사용하므로 cv::Mat 그대로 넘겨도 무방함
		int nNumber = theApp.GetImageClassify(nImageNum);
		if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_DUST)
		{
			lErrorCode = Point_FindDefect(MatOriginImage_, MatOriImageRGB_, MatBKG_, matDustReusltImage,
				stThrdAlignInfo.ptCorner, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nAlgorithmNumber_], dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, strAlgLog);

			MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_DUST_DARK] = matDustReusltImage[E_DEFECT_DUST_DARK].clone();
			MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_DUST_BRIGHT] = matDustReusltImage[E_DEFECT_DUST_BRIGHT].clone();
			MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_DUST_BRIGHT_X] = matDustReusltImage[E_DEFECT_DUST_BRIGHT_X].clone();
			MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_DUST_BRIGHT_Y] = matDustReusltImage[E_DEFECT_DUST_BRIGHT_Y].clone();
		}
		else
		{
			lErrorCode = Point_FindDefect(MatOriginImage_, MatOriImageRGB_, MatBKG_, matReusltImage,
				stThrdAlignInfo.ptCorner, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nAlgorithmNumber_], dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, strAlgLog);

		}
		// Dust 불량 많은 경우, E급 판정 체크 함수
		if (lErrorCode == E_ERROR_CODE_POINT_JUDEGEMENT_E)
		{
			// 불량 E급 판정 불량 1개 추가 ( E_DEFECT_JUDGEMENT_DUST_GROUP )

			// UI에 넘겨줄 결과 넣기 ( 스테이지 번호 0으로 임시 )				
			//JudgeADDefect(nImageNum, nCameraNum_, 0, MatOriginImage_.cols, MatOriginImage_.rows, pResultBlob_Total, E_DEFECT_JUDGEMENT_DUST_GROUP, eInspMode, false);

			// nStageNo 추가
			JudgeADDefect(nImageNum, nCameraNum_, m_stThrdAlignInfo.nStageNo, MatOriginImage_.cols, MatOriginImage_.rows, pResultBlob_Total, E_DEFECT_JUDGEMENT_DUST_GROUP, eInspMode, false);

			// 에러코드 True로 변경 ( 기존 검사 로직 실행 )
			lErrorCode = E_ERROR_CODE_TRUE;
		}

		// 에러가 아닌 경우
		if (lErrorCode == E_ERROR_CODE_TRUE)
		{
			// Dust 패턴인 경우, 기포 결과
			if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_DUST)
			{
				lErrorCode = Point_GetDefectList(MatOriginImage_, MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_], MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_], MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);
			}
			// Dust 패턴 아닌 경우
			else
			{
				// Dust 검사 종료 시까지 대기
				while (!bpInspectEnd[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_])
				{
					Sleep(10);
				}
				////////////패턴 찾기 LJH//////// 
				if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_R)
					bPattern[E_POINT_DARK_RED] = true;
				else if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_G)
					bPattern[E_POINT_DARK_GREEN] = true;
				else if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_B)
					bPattern[E_POINT_DARK_BLUE] = true;
				////////////////////////////
				/////////////////////////////////////////////////////////////////////////////////////////////
				// Point_GetDefectList 함수를 타기 위해서는...
				// dust Pattern으로 Point_FindDefect() 끝난 후, Point_GetDefectList() 함수 실행 해야 함
				/////////////////////////////////////////////////////////////////////////////////////////////

				if (!matReusltImage[E_DEFECT_COLOR_DARK].empty() &&
					!matReusltImage[E_DEFECT_COLOR_BRIGHT].empty() &&
					!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_][0].empty() &&
					!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_][1].empty())
				{
					// Dust 이미지와 Point 검사하는 Pattern 이미지의 해상도가 다름.
					// DLL 내부에서 Dust 이미지 확대 / 축소하여 검사

					lErrorCode = Point_GetDefectList2(MatOriginImage_, matReusltImage, MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_], MatBKG_, MatDrawBuffer,
						stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, dAlignPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, 0, bPattern, nCount);
				}
			}
		}
	}
	break;

	case E_ALGORITHM_LINE:
	{
		vector<int> NorchIndex;
		CPoint OrgIndex;
		int nNorchPosition = (int)dAlignPara[E_PARA_NORCH_OVERKILL_TEXT];
		// UI ( list )
		if (theApp.m_pGrab_Step[nImageNum].tRoundSet != NULL)
		{
			// Norch Round 부분 정보 받아 오기
			GetModelNorchInfo(theApp.m_pGrab_Step[nImageNum].tRoundSet, NorchIndex, OrgIndex, nNorchPosition);
		}

		vector<int> CholeIndex;
		int CholeCnt = 0;

		if (theApp.m_pGrab_Step[nImageNum].tCHoleSet != NULL)
		{
			for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
			{
				if (theApp.m_pGrab_Step[nImageNum].tCHoleSet[i].nContourCount != 0)
				{
					int CholeLTX = 0;
					int CholeLTY = 0;
					int CholeRBX = 0;
					int CholeRBY = 0;

					CholeLTX = stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[theApp.GetImageClassify(nImageNum)][i].x;
					CholeLTY = stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[theApp.GetImageClassify(nImageNum)][i].y;
					CholeRBX = CholeLTX + stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[theApp.GetImageClassify(nImageNum)][i].width;
					CholeRBY = CholeLTY + stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[theApp.GetImageClassify(nImageNum)][i].height;

					CholeIndex.push_back(CholeLTX); // Top Length
					CholeIndex.push_back(CholeLTY); // Top Height
					CholeIndex.push_back(CholeRBX); // Bottom Length
					CholeIndex.push_back(CholeRBY); // Bottom Height

					CholeCnt++;
				}
				else
				{
					break;
				}
			}
		}

		lErrorCode = Line_FindDefect(MatOriginImage_, MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_], MatDrawBuffer, MatBKG_, NorchIndex, CholeIndex, CholeCnt, OrgIndex, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog);
		if (lErrorCode == E_ERROR_CODE_TRUE)
		{
			// Dust 검사 종료 시까지 대기
			while (!bpInspectEnd[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_])
			{
				Sleep(10);
			}
			// Point 알고리즘 번호 가져오기
			int nPointAlgIndex = theApp.GetAlgorithmIndex(_T("POINT"));

			if (!MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][0].empty() &&
				!MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][1].empty() &&
				!MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][2].empty() &&
				!MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][3].empty() &&
				!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex][1].empty() &&
				!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex][0].empty())
			{
				// Dust 이미지와 Point 검사하는 Pattern 이미지의 해상도가 다름.
				// DLL 내부에서 Dust 이미지 확대 / 축소하여 검사
				lErrorCode = Line_GetDefectList(MatOriginImage_, MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_], MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex], MatBKG_, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, dAlignPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog);

			}
		}
	}
	break;

	case E_ALGORITHM_MURA:
	{
		lErrorCode = Mura_FindDefect(MatOriginImage_, MatOriImageRGB_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT],
			stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog);

		// 에러가 아닌 경우
		if (lErrorCode == E_ERROR_CODE_TRUE)
		{
			// Dust 패턴 아닌 경우
			if (theApp.GetImageClassify(nImageNum) != E_IMAGE_CLASSIFY_AVI_DUST)
			{
				// Point Dust 검사(공통으로 사용) 종료 시까지 대기
				while (!bpInspectEnd[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_])
				{
					Sleep(10);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////
				// Mura_GetDefectList 함수를 타기 위해서는...
				// Dust Pattern으로 Point_FindDefect() 끝난 후, Mura_GetDefectList() 함수 실행 해야 함
				/////////////////////////////////////////////////////////////////////////////////////////////

				// Point 알고리즘 번호 가져오기
				int nPointAlgIndex = theApp.GetAlgorithmIndex(_T("POINT"));

				if (!matReusltImage[E_DEFECT_COLOR_DARK].empty() &&
					!matReusltImage[E_DEFECT_COLOR_BRIGHT].empty() &&
					!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex][0].empty() &&
					!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex][1].empty())
				{
					lErrorCode = Mura_GetDefectList(MatOriginImage_, matReusltImage, MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex], MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, fPath, strAlgLog);
				}
			}
		}
		// KSW 18.09.21 - RGB 불량이 검출되어도, 누기 검사 해야 함
		// R, G, B Line 불량 검출한 경우
		//else if ( lErrorCode == E_ERROR_CODE_MURA_RGB_LINE_DEFECT )
		//{
			// KSW 18.09.19 - Alg 안에서 추가 ( Defect-Map 표시 요청 )
			// nStageNo 추가
			//JudgeADDefect(nImageNum, nCameraNum_, m_stThrdAlignInfo.nStageNo, MatOriginImage_.cols, MatOriginImage_.rows, pResultBlob_Total, E_DEFECT_JUDGEMENT_MURA_LINE_X, eInspMode, false);				

			// 에러코드 True로 변경 ( 기존 검사 로직 실행 )
			//lErrorCode = E_ERROR_CODE_TRUE;
		//}
	}
	break;

	case E_ALGORITHM_MURA_NORMAL:
	{
		lErrorCode = MuraNormal_FindDefect(MatOriginImage_, MatOriImageRGB_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT], matReuslt_Mura_Normal_Image[E_DEFECT_TYPE1_WEAK_X], matReuslt_Mura_Normal_Image[E_DEFECT_TYPE1_WEAK_Y], matReuslt_Mura_Normal_Image[E_DEFECT_TYPE2_WEAK_X],
			matReuslt_Mura_Normal_Image[E_DEFECT_TYPE2_WEAK_Y], stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog);

		// 에러가 아닌 경우
		if (lErrorCode == E_ERROR_CODE_TRUE)
		{
			// Dust 패턴 아닌 경우
			if (theApp.GetImageClassify(nImageNum) != E_IMAGE_CLASSIFY_AVI_DUST)
			{
				// Point Dust 검사(공통으로 사용) 종료 시까지 대기
				while (!bpInspectEnd[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_])
				{
					Sleep(10);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////
				// Mura_GetDefectList 함수를 타기 위해서는...
				// Dust Pattern으로 Point_FindDefect() 끝난 후, Mura_GetDefectList() 함수 실행 해야 함
				/////////////////////////////////////////////////////////////////////////////////////////////

				// Point 알고리즘 번호 가져오기
				int nPointAlgIndex = theApp.GetAlgorithmIndex(_T("POINT"));

				if (!matReusltImage[E_DEFECT_COLOR_DARK].empty() &&
					!matReusltImage[E_DEFECT_COLOR_BRIGHT].empty() &&
					!matReuslt_Mura_Normal_Image[E_DEFECT_TYPE2_WEAK_X].empty() &&
					!matReuslt_Mura_Normal_Image[E_DEFECT_TYPE2_WEAK_Y].empty() &&
					!matReuslt_Mura_Normal_Image[E_DEFECT_TYPE1_WEAK_X].empty() &&
					!matReuslt_Mura_Normal_Image[E_DEFECT_TYPE1_WEAK_Y].empty() &&
					!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex][0].empty() &&
					!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex][1].empty())
				{
					lErrorCode = MuraNormal_GetDefectList(MatOriginImage_, matReusltImage, matReuslt_Mura_Normal_Image, MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex], MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, fPath, strAlgLog);
				}
			}
		}
	}
	break;

	case E_ALGORITHM_MURA_CHOLE:
	{
		lErrorCode = MuraChole_FindDefect(MatOriginImage_, MatOriImageRGB_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT],
			stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog);

		// 에러가 아닌 경우
		if (lErrorCode == E_ERROR_CODE_TRUE)
		{
			// Dust 패턴 아닌 경우
			if (theApp.GetImageClassify(nImageNum) != E_IMAGE_CLASSIFY_AVI_DUST)
			{
				// Point Dust 검사(공통으로 사용) 종료 시까지 대기
				while (!bpInspectEnd[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_])
				{
					Sleep(10);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////
				// Mura_GetDefectList 함수를 타기 위해서는...
				// Dust Pattern으로 Point_FindDefect() 끝난 후, Mura_GetDefectList() 함수 실행 해야 함
				/////////////////////////////////////////////////////////////////////////////////////////////

				// Point 알고리즘 번호 가져오기
				int nPointAlgIndex = theApp.GetAlgorithmIndex(_T("POINT"));

				if (!matReusltImage[E_DEFECT_COLOR_DARK].empty() &&
					!matReusltImage[E_DEFECT_COLOR_BRIGHT].empty() &&
					!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex][0].empty() &&
					!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex][1].empty())
				{
					lErrorCode = MuraChole_GetDefectList(MatOriginImage_, matReusltImage, MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex], MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, fPath, strAlgLog);
				}
			}
		}
	}
	break;
	case E_ALGORITHM_DC:
	{
		strInspPath = INSP_PATH + L"\\" + strPanelID_;
		strResultPath = RESULT_PATH + L"\\" + strPanelID_;

		int nResult = -1;
		lErrorCode = DC_FindDefect(MatOriginImage_, nResult, fUnif, MatBKG_, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, strResultPath,
			strInspPath, EngineerDefectJudgment, pResultBlob, dResolution, strAlgLog, arrStrAnalysisData);
		if (nResult != -1)
		{
			JudgeADDefect(nImageNum, nCameraNum_, m_stThrdAlignInfo.nStageNo, MatOriginImage_.cols, MatOriginImage_.rows, pResultBlob_Total, nResult, eInspMode, false);
		}
	}
	break;
	default:
		break;
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
*/

bool BumpInspection::JudgementET(ResultPanelData& resultPanelData, double* dAlignPara, CString strPanelID)
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

			//if( theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_32	&&
			//	theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_64	&&
			//	theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_87)
			//	continue;

			// 명점 다발
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK ||
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
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK ||
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
			resultPanelData.m_ListDefectInfo[nIndexDark].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_POINT_DARK;

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


// bool BumpInspection::JudgementEdgePadBright(ResultPanelData& resultPanelData, double* dAlignPara)
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
bool BumpInspection::JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
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
bool BumpInspection::JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
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

			// PNZ 19.04.23 Mura Normal F Grade Defect
			if (nDefectType == E_DEFECT_JUDGEMENT_MURANORMAL_TYPE2_F_GRADE ||
				nDefectType == E_DEFECT_JUDGEMENT_MURANORMAL_TYPE3_F_GRADE)
			{
				bMura = true;
			}

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


bool BumpInspection::DeleteOverlapDefect(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.09.21 - 6.39" 귀부분 점등 안된경우, E급 판정
	// EDGE_NUGI 인 경우, EDGE_NUGI를 제외한 모든 불량 삭제
	//////////////////////////////////////////////////////////////////////////

	bool bNugiJudgeE = false;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// EDGE_NUGI 찾기
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI)
		{
			// EDGE_NUGI 리스트 처음으로 복사
			resultPanelData.m_ListDefectInfo[0] = resultPanelData.m_ListDefectInfo[i];

			// E급 판정
			bNugiJudgeE = true;

			break;
		}
	}

	// EDGE_NUGI 있는 경우...
	if (bNugiJudgeE)
	{
		// 모든 불량 제거
		for (int i = 1; i < resultPanelData.m_ListDefectInfo.GetCount(); )
			resultPanelData.m_ListDefectInfo.RemoveAt(i);

		// 삭제후, 나가기
		return true;
	}

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

	// Point - Point 중복 제거
	DeleteOverlapDefect_Point_Point(resultPanelData, dAlignPara);

	//LJH 20201201 동일 패턴 Point - Point 중복 제거
	DeleteOverlapDefect_Point_Point_RGB(resultPanelData, dAlignPara);

	// Point - Mura 중복 제거
	DeleteOverlapDefect_Point_Mura(resultPanelData, dAlignPara);

	// Point - Line 중복 제거
	DeleteOverlapDefect_Point_Line(resultPanelData, dAlignPara);

	// Line - Mura 중복 제거
	DeleteOverlapDefect_Line_Mura(resultPanelData, dAlignPara);

	// Mura - Mura 중복 제거
//	DeleteOverlapDefect_Mura_Mura(resultPanelData, dAlignPara);

	return true;
}


bool BumpInspection::DeleteOverlapDefect_DimpleDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
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
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK ||
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
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB		&&
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


bool BumpInspection::DeleteOverlapDefect_SpecInSpotDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
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
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
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
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_64)
				{
					i++;
					continue;
				}

				////////////////////////////////
				// Max-Mean Ratio Calculation //
				int nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
				int nDefectMeanGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
				float MaxMeanRatio = (float)nDefectMaxGV / nDefectMeanGV;

				// 임시 Max/Mean Ratio의 최대 값으로 분류 함
				if (MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
				{
					i++;
					continue;
				}

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

				int nRegionUnit = 10;

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
					resultPanelData.m_ListDefectInfo.RemoveAt(i);

				else i++;
			}

			else i++;
		}

		mG64Image.release();
	}

	return true;
}

// Dust Pattern을 이용하여 암점 불량이 불량인지를 확인
bool BumpInspection::DeleteOverlapDefect_DustDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
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


bool BumpInspection::DeleteOverlapDefect_BlackSmallDelet(ResultPanelData& resultPanelData, double* dAlignPara)
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


bool BumpInspection::DeleteOverlapDefect_Point_Black_RGB(ResultPanelData& resultPanelData, cv::Mat MatOrgImage_Real[][MAX_CAMERA_COUNT], double* dAlignPara)
{
	// 예외처리 Parameter 가 없으면
	if (dAlignPara == NULL)
		return false;

	// 예외처리 Defect가 없으면
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	double dTH = dAlignPara[E_PARA_AVI_BLACK_OVERKILL_TEXT];
	int nBlackTH = (int)dAlignPara[E_PARA_AVI_BLACK_OVERKILL_BLACK_GV];
	int nRedTH = (int)dAlignPara[E_PARA_AVI_BLACK_OVERKILL_RED_GV];
	int nGreenTH = (int)dAlignPara[E_PARA_AVI_BLACK_OVERKILL_GREEN_GV];
	int nBlueTH = (int)dAlignPara[E_PARA_AVI_BLACK_OVERKILL_BLUE_GV];

	// 불량 중심에서 거리 Offset를 설정
	int nSenterOffset = 50;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		// Point 불량인 경우만...
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT) continue;

		// White 패턴인 경우만...
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_BLACK)	continue;

		// Center 좌표
		int nDefectCenter_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
		int nDefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

		// 시작 좌표
		int nCal_X = nDefectCenter_X - nSenterOffset;
		int nCal_Y = nDefectCenter_Y - nSenterOffset;

		// 영역 설정 50x50
		cv::Rect rectTemp;

		rectTemp.x = nCal_X;
		rectTemp.y = nCal_Y;
		rectTemp.width = nSenterOffset * 2;

		if (MatOrgImage_Real[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BLACK)][0].empty()) continue;
		if (MatOrgImage_Real[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_R)][0].empty()) continue;
		if (MatOrgImage_Real[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_G)][0].empty()) continue;
		if (MatOrgImage_Real[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_B)][0].empty()) continue;


		cv::Mat BlackPattern = MatOrgImage_Real[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BLACK)][0];
		cv::Mat RedPattern = MatOrgImage_Real[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_R)][0];
		cv::Mat GreenPattern = MatOrgImage_Real[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_G)][0];
		cv::Mat BluePattern = MatOrgImage_Real[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_B)][0];

		cv::Mat BlackROI = BlackPattern(rectTemp);
		cv::Mat RedROI = RedPattern(rectTemp);
		cv::Mat GreenROI = GreenPattern(rectTemp);
		cv::Mat BlueROI = BluePattern(rectTemp);

		cv::Mat BlackMask;

		BlackROI.copyTo(BlackMask);

		cv::threshold(BlackMask, BlackMask, dTH, 255.0, THRESH_BINARY);


		cv::Scalar dBlackROIGV = cv::mean(BlackROI);

		if (dBlackROIGV[0] == 0) continue;

		cv::Scalar dBlackMeanGV = cv::mean(BlackROI, BlackMask);
		cv::Scalar dRedMeanGV = cv::mean(RedROI, BlackMask);
		cv::Scalar dGreenMeanGV = cv::mean(GreenROI, BlackMask);
		cv::Scalar dBlueMeanGV = cv::mean(BlueROI, BlackMask);

		if (dBlackMeanGV[0] > nBlackTH) continue;

		if (dRedMeanGV[0] > nRedTH)
		{
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
			i--;
			continue;
		}
		if (dGreenMeanGV[0] > nGreenTH)
		{
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
			i--;
			continue;
		}
		if (dBlueMeanGV[0] > nBlackTH)
		{
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
			i--;
			continue;
		}

		BlackROI.release();
		RedROI.release();
		GreenROI.release();
		BlueROI.release();
		BlackMask.release();
	}
	return true;
}



bool BumpInspection::DeleteOverlapDefect_Point_Point(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOnOff = (int)dAlignPara[E_PARA_AVI_POINT_MERGE_ONOFF_TEXT];
	int nOffSet = (int)dAlignPara[E_PARA_AVI_POINT_MERGE_OFFSET];

	// 사용 안할경우 나가기
	if (nOnOff <= 0)
		return true;
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

			// Black 패턴인 경우만...
			nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
			if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_BLACK)
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
					resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT && resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB)
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

					// KSW 17.12.14 - 중복 될 시, 다른 패턴보다 Gray 패턴 우선 보고
					if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_GRAY_64)
					{
						ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
						ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp2;
						resultPanelData.m_ListDefectInfo[j] = tTemp1;
					}

					// KSW 17.09.11 - 중복 될 시, 다른 패턴보다 Black 패턴 우선 보고 ( 고객사 요청 )
					if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_BLACK)
					{
						ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
						ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp2;
						resultPanelData.m_ListDefectInfo[j] = tTemp1;
					}

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

bool BumpInspection::DeleteOverlapDefect_Point_Point_RGB(ResultPanelData & resultPanelData, double * dAlignPara)
{
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOnOffP_P = (int)dAlignPara[E_PARA_AVI_POINT_MERGE_ONOFF_TEXT];
	int nOffSet = (int)dAlignPara[E_PARA_AVI_POINT_MERGE_OFFSET];
	//P_P머지 사용할경우 이거 동작 안함
	if (nOnOffP_P == 1)
		return true;
	//////////////////////////////////////////////////////////////////////////
	// 동일 패턴 명, 암점 비교
	//////////////////////////////////////////////////////////////////////////

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if ((resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_POINT_START			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_POINT_END))
		{
			CRect rectTemp = CRect(
				(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
				(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
				(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
				(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);

			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			// 현재 검출한 패턴 확인 RGB 패턴만 사용할꺼임
			int nPattern = theApp.GetImageClassify(nImgNum);
			if (nPattern >= E_IMAGE_CLASSIFY_AVI_BLACK)
				continue;
			// 비교할 불량
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// 같은 불량 항목 비교 금지
				if (i == j)
				{
					j++;
					continue;
				}
				// 같은 패턴이 아닐경우 패스
				int nImgNum2 = resultPanelData.m_ListDefectInfo[j].Img_Number;
				if (nImgNum != nImgNum2)
				{
					j++;
					continue;
				}
				// Point 불량 아닌것 제외
				if ((resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_POINT_START ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_POINT_END))
				{
					j++;
					continue;
				}

				// 불량 중심 좌표
				CPoint ptSrc;
				ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				// point - Point 불량만 중복 제거
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

	return false;
}


bool BumpInspection::DeleteOverlapDefect_Point_Line(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOffSet = 4;

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


bool BumpInspection::DeleteOverlapDefect_Point_Mura(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//ljh 고객사 요구 onoff 기능 추가해 달라고 함 삭제 할꺼 같음
	int nOnOffP_M = (int)dAlignPara[E_PARA_AVI_POINT_MURA_MERGE_ONOFF_TEXT];
	if (nOnOffP_M == 0)
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
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT ||	// 2018-05-08 PNZ Mura 불량이다
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT
					)
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
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY ||		// GRAY명점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK ||		// GRAY 약 명점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB ||		//RGB 명점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT ||		// 약명점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK ||		// 명암점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||		// 그룹암점
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT ||		// 그룹명점
							nDefectTypeS == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK ||		// 암점 재검
							nDefectTypeS == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT ||		// 명점 재검
							nDefectTypeS == E_DEFECT_JUDGEMENT_MURA_MULT_BP ||
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3)

						{
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


	//////////////////////////////////////////////////////////////////////////
	// Mura Spot & 명점인 경우 -> 명점 제거 & Mura 보고
	//////////////////////////////////////////////////////////////////////////

	int nOffSet = 5;

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Mura 밝은 Spot 불량 아닌 경우 제외
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)
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


bool BumpInspection::DeleteOverlapDefect_Line_Mura(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOffSet = 8;

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
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EMD_BRIGHT ||
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EMD_DARK			||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_UP_CM ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_CORNER_CM ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_FINGER_CM ||
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_NUGI			||	// KSW 18.05.21 - 고객사 요청 : 누기 & 라인 불량 따로 보고
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BN_CORNER ||
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_STAMPED		||	// KSW 18.05.21 - 고객사 요청 : 누기 & 라인 불량 따로 보고
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI		||	// KSW 18.05.21 - 고객사 요청 : 누기 & 라인 불량 따로 보고
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_		||	// KSW 18.05.21 - 고객사 요청 : 누기 & 라인 불량 따로 보고
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_DARK ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_MURA)
					// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_X		||	// PNZ 19.04.15 Type 1234 & Line Merge
					// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y		||	// PNZ 19.04.15 Type 1234 & Line Merge
					// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_X		||	// PNZ 19.04.15 Type 1234 & Line Merge
					// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_Y		)	// PNZ 19.04.15 Type 1234 & Line Merge
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


bool BumpInspection::DeleteOverlapDefect_Mura_Mura(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S 모드에 따른 좌표 비교
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// 중복되는 Mura 불량 제거
	//////////////////////////////////////////////////////////////////////////

	int nOffSet = 5;

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
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_X ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_Y ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WEAK_TYPE1MURA_X ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WEAK_TYPE1MURA_Y)
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
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_X ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_Y ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WEAK_TYPE1MURA_X ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WEAK_TYPE1MURA_Y)
			{
				j++;
				continue;
			}

			CRect rectS = CRect(
				(resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio),
				(resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio),
				(resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio),
				(resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio));

			// 중복 영역 확인
			CRect rectRes;
			IntersectRect(rectRes, rectM, rectS);
			int nArea = 0;
			int nrectM = rectM.Width()*rectM.Height();
			int nrectS = rectS.Width() * rectS.Height();

			CRect rectRes2;
			rectRes2 = rectS & rectM;

			int nrectRes2 = rectRes2.Width() * rectRes2.Height();

			// 범위 안에 들어오면 삭제
			if ((rectRes.Width() + rectRes.Height()) > 0 && nrectM > nrectS)
			{
				// 백점 & 누기 & 하부찍힘 우선순위로 남기기
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||	// 백점
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT ||	// 백점
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

	return true;
}


// Casting - stDefectInfo에서 필요한 부분 추출하여 ResultPanelData에 적재
bool BumpInspection::GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio)
{
	// AVI는 회전하지 않은 원본 영상 좌표를 사용하므로 상위 보고 시 워크좌표 계산 등을 위해 좌표를 회전해서 넘겨줘야 함
	_tcscpy_s(pResultDefectInfo->Defect_Code, _T("CODE"));
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_TOP] = cv::Point(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_TOP] = cv::Point(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM] = cv::Point(pResultBlob->ptRB[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_BOTTOM] = cv::Point(pResultBlob->ptLB[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].y);

	cv::Point ptRotate(0, 0);

	// AD 아닌 불량들...
	if (pResultBlob->nDefectJudge[nBlobCnt] > E_DEFECT_JUDGEMENT_DUST_GROUP)
	{
		cv::Point ptTemp(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLT[nBlobCnt].y);
		Align_DoRotatePoint(ptTemp, ptRotate, m_stThrdAlignInfo.ptAlignCenter, m_stThrdAlignInfo.dAlignTheta);
		pResultDefectInfo->Pixel_Start_X = ptRotate.x;
		pResultDefectInfo->Pixel_Start_Y = ptRotate.y;

		ptTemp.x = pResultBlob->ptRB[nBlobCnt].x;
		ptTemp.y = pResultBlob->ptRB[nBlobCnt].y;
		Align_DoRotatePoint(ptTemp, ptRotate, m_stThrdAlignInfo.ptAlignCenter, m_stThrdAlignInfo.dAlignTheta);
		pResultDefectInfo->Pixel_End_X = ptRotate.x;
		pResultDefectInfo->Pixel_End_Y = ptRotate.y;
	}
	// AD 불량일 경우 회전하지 않고 전체 좌표 그대로 반환
	else
	{
		pResultDefectInfo->Pixel_Start_X = pResultBlob->ptLT[nBlobCnt].x;
		pResultDefectInfo->Pixel_Start_Y = pResultBlob->ptLT[nBlobCnt].y;
		pResultDefectInfo->Pixel_End_X = pResultBlob->ptRB[nBlobCnt].x;
		pResultDefectInfo->Pixel_End_Y = pResultBlob->ptRB[nBlobCnt].y;
	}

	pResultDefectInfo->Pixel_Center_X = (int)(pResultDefectInfo->Pixel_Start_X + pResultDefectInfo->Pixel_End_X) / 2;
	pResultDefectInfo->Pixel_Center_Y = (int)(pResultDefectInfo->Pixel_Start_Y + pResultDefectInfo->Pixel_End_Y) / 2;

	// 기울어진 원본 이미지 에서 Crop 할 불량 영역 지정
	pResultDefectInfo->Pixel_Crop_Start_X = min(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].x);
	pResultDefectInfo->Pixel_Crop_Start_Y = min(pResultBlob->ptLT[nBlobCnt].y, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Pixel_Crop_End_X = max(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].x);
	pResultDefectInfo->Pixel_Crop_End_Y = max(pResultBlob->ptLB[nBlobCnt].y, pResultBlob->ptRB[nBlobCnt].y);

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

	// KSW 17.12.04 - (장축 + 단축) / 2 -> Spec 추가 ( 고객사 요청 )
	pResultDefectInfo->dDimension = (pResultBlob->dF_Max[nBlobCnt] + pResultBlob->dF_Min[nBlobCnt]) / 2.0;

#if USE_ALG_CONTOURS
	// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
	memcpy(pResultDefectInfo->nContoursX, pResultBlob->nContoursX[nBlobCnt], sizeof(int) * MAX_CONTOURS);
	memcpy(pResultDefectInfo->nContoursY, pResultBlob->nContoursY[nBlobCnt], sizeof(int) * MAX_CONTOURS);
#endif

	return true;
}

bool BumpInspection::AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner)
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
long BumpInspection::CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio)
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
bool BumpInspection::JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo)
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

bool BumpInspection::JudgementSpot(ResultPanelData& resultPanelData)
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

bool BumpInspection::JudgementDelReport(ResultPanelData& resultPanelData)
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

bool BumpInspection::JudgementDeletLineDefect(ResultPanelData& resultPanelData, double* dAlignPara)
{
	int nLineNumber = 0;	// 진짜 라인 불량
	int nLineDefect = 0;	// 약한 라인
	int nOffSet = 50;	// 중심점 거리 차이
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

bool BumpInspection::JudgementSamePatternDefectMerge(ResultPanelData& resultPanelData)
{
	int nLineNumber = 0;	// 진짜 라인 불량
	int nSSLineNumber = 0;	// 동일 Pattern, 동일 방향에 불량
	int nOffSet = 50;	// 중심점 거리 차이

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
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
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
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
			{
				CPoint	X1Center, X1Length;
				int		X1DefectImageType, X1DefectLength;
				int nDefectCount = (int)resultPanelData.m_ListDefectInfo.GetCount();
				X1Length.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;	// 결함 시작점
				X1Length.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;	// 결함 끝점
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio; // Center위치
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴
				X1DefectLength = abs(X1Length.x - X1Length.y);								// 결함 길이

				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_OPEN			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
					{
						j++; continue;
					}

					CPoint	X2Center, X2Length;
					int		X2DefectImageType, X2DefectLength;

					X2Length.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;	// 결함 시작점
					X2Length.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;	// 결함 끝점
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio; // Center위치
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴
					X2DefectLength = abs(X2Length.x - X2Length.y);								// 결함 길이

																								// 동일 Pattern의 결함
					if (X1DefectImageType != X2DefectImageType) {
						if (abs(X1Center.y - X2Center.y) <= nOffSet) {
							resultPanelData.m_ListDefectInfo[i].Pixel_Start_X = min(X1Length.x, X2Length.x) * resultPanelData.m_ListDefectInfo[i].nRatio;
							resultPanelData.m_ListDefectInfo[i].Pixel_End_X = max(X1Length.y, X2Length.y) * resultPanelData.m_ListDefectInfo[i].nRatio;

							// 리스트 작은거 삭제시
							if (i > j)	 i--;

							// 다른 Pattern의 Line 제거
							resultPanelData.m_ListDefectInfo.RemoveAt(j);
							continue;
						}
					}

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
						resultPanelData.m_ListDefectInfo[i].Pixel_Start_X = min(X1Length.x, X2Length.x) * resultPanelData.m_ListDefectInfo[i].nRatio;
						resultPanelData.m_ListDefectInfo[i].Pixel_End_X = max(X1Length.y, X2Length.y) * resultPanelData.m_ListDefectInfo[i].nRatio;

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

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Y 방향
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
			{
				CPoint	Y1Center, Y1Length;
				int		Y1DefectImageType, Y1DefectLength;

				Y1Length.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;	// 결함 시작점
				Y1Length.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;	// 결함 끝점
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio; // Center위치
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴
				Y1DefectLength = abs(Y1Length.x - Y1Length.y);								// 결함 길이

				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// 같은 불량 비교 방지
					if (i == j) { j++; continue; }

					// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DARK			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT  &&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
					{
						j++; continue;
					}

					CPoint	Y2Center, Y2Length;
					int		Y2DefectImageType, Y2DefectLength;

					Y2Length.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;	// 결함 시작점
					Y2Length.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;	// 결함 끝점
					Y2Center.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio; // Center위치
					Y2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴
					Y2DefectLength = abs(Y2Length.x - Y2Length.y);								// 결함 길이

																								// 동일 Pattern의 결함
					if (Y1DefectImageType != Y2DefectImageType) {
						if (abs(Y1Center.x - Y2Center.x) <= nOffSet) {
							resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y = min(Y1Length.x, Y2Length.x) * resultPanelData.m_ListDefectInfo[i].nRatio;
							resultPanelData.m_ListDefectInfo[i].Pixel_End_Y = max(Y1Length.y, Y2Length.y) * resultPanelData.m_ListDefectInfo[i].nRatio;

							// 리스트 작은거 삭제시
							if (i > j)	 i--;

							// 다른 Pattern의 Line 제거
							resultPanelData.m_ListDefectInfo.RemoveAt(j);
							continue;
						}
					}

					// 같은 방향
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
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
						resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y = min(Y1Length.x, Y2Length.x) * resultPanelData.m_ListDefectInfo[i].nRatio;
						resultPanelData.m_ListDefectInfo[i].Pixel_End_Y = max(Y1Length.y, Y2Length.y) * resultPanelData.m_ListDefectInfo[i].nRatio;

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

bool BumpInspection::JudgeMultLine(ResultPanelData & resultPanelData)
{
	int nBrightLineXNumber = 0;	// 진짜 라인 불량
	int nBrightLineYNumber = 0;	// 진짜 라인 불량
	int nDarkLineXNumber = 0;	// 진짜 라인 불량
	int nDarkLineYNumber = 0;	// 진짜 라인 불량

						// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// 진짜 Line성 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
			nBrightLineXNumber++;	// 진짜 라인
			break;
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
			nBrightLineYNumber++;	// 진짜 라인
			break;
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
			nDarkLineXNumber++;	// 진짜 라인
			break;
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
			nDarkLineYNumber++;	// 진짜 라인
			break;
			//case E_DEFECT_JUDGEMENT_LINE_X_OPEN:			
			//	nBrightLineXNumber++;	// 진짜 라인
			//	nDarkLineXNumber++;	// 진짜 라인
			//	break;
			//case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
			//case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
			//	nDarkLineYNumber++;	// 진짜 라인
			//	nBrightLineYNumber++;	// 진짜 라인
			//	break;
		}
	}
	if (nBrightLineXNumber > 1)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT)
			{
				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_BRIGHT_MULT;
			}
			else i++;  // 다음 불량으로...
		}
	}
	if (nBrightLineYNumber > 1)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT)
			{
				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT_MULT;
			}
			else i++;  // 다음 불량으로...
		}
	}
	if (nDarkLineXNumber > 1)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
			{
				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_DARK_MULT;
			}
			else i++;  // 다음 불량으로...
		}
	}
	if (nDarkLineYNumber > 1)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
			{
				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_DARK_MULT;
			}
			else i++;  // 다음 불량으로...
		}
	}
	return true;
}

bool BumpInspection::PatternPointBrightClassify(ResultPanelData & resultPanelData)
{

	int nDefectNum = (int)resultPanelData.m_ListDefectInfo.GetCount();

	int nOffset = 10;

	// 불량 확인
	for (int i = 0; i < nDefectNum; i++)
	{
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		// 현재 검출한 패턴 확인
		int nPattern = theApp.GetImageClassify(nImgNum);

		//// 명점 불량 확인
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT)
		{
			int nCountOverlap = 0;
			// 불량 중심 좌표
			CPoint ptFirst, ptSecond;
			ptFirst.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio);
			ptFirst.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

			for (int j = 0; j < nDefectNum; j++)
			{
				// 같은 불량 넘어가기
				if (i == j)
				{
					continue;
				}

				// 불량 중심 좌표
				ptSecond.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSecond.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				int nTwoPointDistance = (int)sqrt(pow(ptFirst.x - ptSecond.x, 2.0) + pow(ptFirst.y - ptSecond.y, 2.0));

				// 같은위치에 불량이 존재하는지 확인
				if (nTwoPointDistance <= nOffset)
				{
					if (resultPanelData.m_ListDefectInfo[i].Img_Number != resultPanelData.m_ListDefectInfo[j].Img_Number)
						nCountOverlap++;
				}
			}
			// 같은 위치에 불량이 존재하지 않으면 패턴을 확인 후 
			// 특정 위치에만 존재하는 불량명 변경 
			if (nCountOverlap == 0)
			{
				switch (nPattern)
				{
				case E_IMAGE_CLASSIFY_AVI_GRAY_64:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY;
					break;
				default:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT;
					break;
				}
			}
		}

		//// 약 명점 불량 확인
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT)
		{
			int nCountOverlap = 0;
			// 불량 중심 좌표
			CPoint ptFirst, ptSecond;
			ptFirst.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio);
			ptFirst.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

			for (int j = 0; j < nDefectNum; j++)
			{
				// 같은 불량 넘어가기
				if (i == j)
				{
					continue;
				}

				// 불량 중심 좌표
				ptSecond.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSecond.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				int nTwoPointDistance = (int)sqrt(pow(ptFirst.x - ptSecond.x, 2.0) + pow(ptFirst.y - ptSecond.y, 2.0));

				// 같은위치에 불량이 존재하는지 확인
				if (nTwoPointDistance <= nOffset)
				{
					if (resultPanelData.m_ListDefectInfo[i].Img_Number != resultPanelData.m_ListDefectInfo[j].Img_Number)
						nCountOverlap++;
				}
			}
			// 같은 위치에 불량이 존재하지 않으면 패턴을 확인 후 
			// 특정 위치에만 존재하는 불량명 변경 
			if (nCountOverlap == 0)
			{
				switch (nPattern)
				{
				case E_IMAGE_CLASSIFY_AVI_GRAY_64:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK;
					break;
				default:
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT;
					break;
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

bool BumpInspection::JudgementBlackPatternMerge(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// 진짜 라인 불량
	int nYLineNumber = 0;	// 진짜 라인 불량
	int nOffSet = 50;	// 중심점 거리 차이

	// 불량 갯수를 세는 부분
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// 진짜 X Line 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_DGS_X:
		case E_DEFECT_JUDGEMENT_LINE_PCD_CRACK:
			nXLineNumber++;	// 진짜 라인
			break;

			// 진짜 Y Line 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
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
		/*
		//비교함
		int	X1DefectPattern  = 0;
		int X1FistPattern	 = 0; // 제일 먼저 나오는 Pattern 수~
		int X1InitialPattern = 20;
		//천마 추가 코드
		*/
		// 명선 불량 발생 Pattern 우선 순위 Black > Red > Green > Blue (Pattern Number가 순서대로 나열되지 않는 경우 존재)
		/*
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 명선이 제일 먼저 나온 Pattern을 기준 Pattern임
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT)
			{
				CPoint	X1St, X1End, X1Center;
				X1DefectPattern = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴
				X1FistPattern = min(X1InitialPattern, X1DefectPattern);
				X1InitialPattern = X1FistPattern;
			}
		}
		*/

		//명선 외 결함존재 
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

				// 비교하고 얻은 Pattern 이여야 한다..ㅋ
				//if (X1DefectImageType != X1FistPattern) continue;

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

					if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK) &&
						(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT || resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT))
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

		//천마에서 사용 안하는 기능 추후 재사용 여부 확인
		/*
		//DGS
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_X)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y		/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y			= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType	= (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

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
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS_X			)
					{ j++; continue; }

					CPoint	X2St, X2End, X2Center;
					int		X2DefectImageType;

					X2St.y				= (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[j].nRatio;
					X2End.y				= (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y		/ resultPanelData.m_ListDefectInfo[j].nRatio;
					X2Center.y			= (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y	/ resultPanelData.m_ListDefectInfo[j].nRatio;
					X2DefectImageType	= (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					//if (X1DefectImageType == X2DefectImageType) j++; continue;

					// 주변에 Line이 검출 되는 경우
					if ( abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// 리스트 작은거 삭제시
						if( i > j )	 i--;

						// 다른 Pattern의 모든 모든 모든 Line을 모두 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}
		*/

		//천마에서 사용 안하는 기능 추후 재사용 여부 확인
		/*
		// PCD Crack 부분 삭제
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_PCD_CRACK)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y		/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y			= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType	= (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

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
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_PCD_CRACK		)
					{ j++; continue; }

					CPoint	X2St, X2End, X2Center;
					int		X2DefectImageType;

					X2St.y				= (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[j].nRatio;
					X2End.y				= (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y		/ resultPanelData.m_ListDefectInfo[j].nRatio;
					X2Center.y			= (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y	/ resultPanelData.m_ListDefectInfo[j].nRatio;
					X2DefectImageType	= (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					// 기준 Pattern과 같으면 안됨 다른 Pattern의 결함
					if ( X1DefectImageType == X2DefectImageType ){ j++; continue; }

					// 주변에 Line이 검출 되는 경우
					if ( abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// 리스트 작은거 삭제시
						if( i > j )	 i--;

						// 다른 Pattern의 모든 모든 모든 Line을 모두 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}
		*/
	}



	//////////////////////////////////////////////////////////////////////////
	// y 방향
	//////////////////////////////////////////////////////////////////////////

	if (nYLineNumber > 0) // Y Line이 있으면 일해야 됨
	{
		//비교함
		/*
		int	Y1DefectPattern  = 0;
		int Y1FistPattern	 = 0; // 제일 먼저 나오는 Pattern 수~
		int Y1InitialPattern = 20;

		// 명선 불량 발생 Pattern 우선 순위 Black / Red / Green / Blue
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Y 방향 명선이 제일 먼저 나온 Pattern을 기준 Pattern임
			if(resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK )
			{
				CPoint	Y1St, Y1End, Y1Center;

				Y1DefectPattern		= (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴
				Y1FistPattern		= min(Y1InitialPattern, Y1DefectPattern);
				Y1InitialPattern	= Y1FistPattern;
			}
		}

		*/

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)

			{
				CPoint	Y1St, Y1End, Y1Center;
				int		Y1DefectImageType;

				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

				// 비교하고 얻은 Pattern 이여야 한다..ㅋ
				//if (Y1DefectImageType != Y1FistPattern) continue;

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

					if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK) &&
						(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT || resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT))
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
		/*
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X 방향 불량 아닌것 제외 ( 약라인도 나올 수 있기에 추가 했음 )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_Y)
			{
				CPoint	Y1St, Y1End, Y1Center;
				int		Y1DefectImageType;

				Y1St.x				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X		/ resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x			= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectImageType	= (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// 패턴

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
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS_Y			)
					{ j++; continue; }

					CPoint	Y2St, Y2End, Y2Center;
					int		Y2DefectImageType;

					Y2St.x				= (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X	/ resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2End.x				= (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X		/ resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2Center.x			= (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X	/ resultPanelData.m_ListDefectInfo[j].nRatio;
					Y2DefectImageType	= (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// 패턴

					// 주변에 Line이 검출 되는 경우
					if ( abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// 리스트 작은거 삭제시
						if( i > j )	 i--;

						// 다른 Pattern의 모든 모든 모든 Line을 모두 제거
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// 주변에 결함이 없는 경우
					else  j++;
				}
			}
		}

		*/
	}

	return true;
}

//Back up
/*
bool BumpInspection::JudgementBlackPatternMerge(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// 진짜 라인 불량
	int nYLineNumber = 0;	// 진짜 라인 불량
	int nOffSet = 50;	// 중심점 거리 차이

						// 불량 갯수를 세는 부분
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// 진짜 X Line 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_DGS_X:
		case E_DEFECT_JUDGEMENT_LINE_PCD_CRACK:
			nXLineNumber++;	// 진짜 라인
			break;

			// 진짜 Y Line 불량인 경우만 Count
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
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
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
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
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
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
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
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
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
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
*/

//////////////////////////////////////////////////////////////////////////
// 제목: Camera Tap에 인한 약 Line 불량 과검 제거
// 작업일: 2018-07-31
// 작업자: PNZ
//////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementCameraTapOverKill(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
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
				if (fabs(nDefectLocation - CTO_Position_X) <= CTO_Offset)
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

bool BumpInspection::DefectLDRCompair(cv::Mat matSrcImage, cv::Rect rectTemp, double& Left_MeanValue, double& Defect_MeanValue, double& Right_MeanValue, double& Defect_MaxGV)
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

bool 	BumpInspection::CameraTapInfo(int CameraType, vector<int> &Direction, vector<int> &Position, vector<double> &BrightDiffGV, vector<double> &DarkDiffGV)
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

bool BumpInspection::JudgementPCDCrack(ResultPanelData& resultPanelData, double* dAlignPara)
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

bool BumpInspection::JudgementNewDGS(ResultPanelData& resultPanelData)
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
bool BumpInspection::JudgementDGS_Vth(ResultPanelData& resultPanelData)
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

bool BumpInspection::JudgementCrack(ResultPanelData& resultPanelData)
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

		if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_32) continue;

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

bool BumpInspection::JudgementClassifyLineDefect(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT])
{
	int nLineNumber = 0;	// 진짜 라인 불량
	int nLineDefect = 0;	// 약한 라인
	int nOffSet = 50;	// Line 불량 인접 범위

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

bool BumpInspection::JudgementNotchDefect(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{

	if (dAlignPara == NULL)
		return false;
	int		nNorchPosition = (int)(dAlignPara[E_PARA_NORCH_OVERKILL_ONOFF]);
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
				bool bNorchInfoGet = GetModelNorchInfo(theApp.m_pGrab_Step[nImgNum].tRoundSet, NorchIndex, OrgIndex, nNorchPosition);

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

bool BumpInspection::JudgementSpecialGrayLineDefect(ResultPanelData& resultPanelData)
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
long BumpInspection::CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult,
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

			//theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_DIABLE_CHECK_DUST, eALARMTYPE_HEAVY, bIsHeavyAlarm);

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
					//theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_DUST_ABNORMAL, eALARMTYPE_HEAVY, bIsHeavyAlarm);
				}


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
double BumpInspection::calcResolution(CWriteResultInfo WrtResultInfo)
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

bool BumpInspection::JudgementRepeatCount(CString strPanelID, ResultPanelData& resultPanelData)
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

bool BumpInspection::m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo)
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

bool BumpInspection::NewMaxGVMethold(Mat matSrcImage, double OldMaxGV, double& NewMaxGV, int nTopCountGV)
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


bool 	BumpInspection::GetModelNorchInfo(ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], vector<int>& NorchIndex, CPoint& OrgIndex, int nNorchPosition)
{
	if (tRoundSet == NULL) return false;

	// ROI 개수 확인
	int ROICount = 0;

	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		if (tRoundSet[i].nContourCount != 0)
			ROICount++;
	}
	int nPositionT = 0;
	int nPositionB = 0;

	switch (nNorchPosition)
	{
	case 1:
		nPositionT = 6;
		nPositionB = 5;
		break;
	case 2:
		nPositionT = 2;
		nPositionB = 3;
		break;
	case 3:
		nPositionT = 3;
		nPositionB = 4;
		break;
	case 4:
		nPositionT = 8;
		nPositionB = 9;
		break;
	default:
		break;
	}
	if (ROICount <= 4) return false;

	int TopDataLenght = tRoundSet[nPositionT].nContourCount;
	int BottomDataLenght = tRoundSet[nPositionB].nContourCount;

	if (TopDataLenght == 0 && BottomDataLenght == 0) return false;

	// 원점 Index
	int ValueBuff1 = m_stThrdAlignInfo.rcAlignCellROI.x;
	int ValueBuff2 = m_stThrdAlignInfo.rcAlignCellROI.y;

	int ValueBuff3 = tRoundSet[nPositionT].ptContours[TopDataLenght - 1].x;
	int ValueBuff4 = tRoundSet[nPositionT].ptContours[0].y;
	int ValueBuff5 = tRoundSet[nPositionB].ptContours[0].x;
	int ValueBuff6 = tRoundSet[nPositionB].ptContours[BottomDataLenght - 1].y;

	NorchIndex.push_back(abs(ValueBuff3)); // Top Length
	NorchIndex.push_back(abs(ValueBuff4)); // Top Height
	NorchIndex.push_back(abs(ValueBuff5)); // Bottom Length
	NorchIndex.push_back(abs(ValueBuff6)); // Bottom Height

	OrgIndex.x = ValueBuff4;
	OrgIndex.y = ValueBuff6 + m_stThrdAlignInfo.rcAlignCellROI.height;

	return true;
}

bool	BumpInspection::JudgeCHoleJudgment(ResultPanelData& resultPanelData, tCHoleAlignInfo tCHoleAlignData, double* dAlignPara)
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
		int nDefect3_Cnt = 0;		// 무라 불량
									//int nDefect3_Cnt = 0;			// 임시 Type2 Y방향

		int nMuraStart = E_DEFECT_JUDGEMENT_MURA_START;
		int nMuraEnd = E_DEFECT_JUDGEMENT_MURA_END;
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
			case E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y:
			case E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_Y:
			case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
			case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
			case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
			case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:

			case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
			case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
			case E_DEFECT_JUDGEMENT_LINE_X_DARK:
			case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
				nDefect2_Cnt++;
				break;

			default:
				if (nMuraStart <= nDefect_Type  && nDefect_Type <= nMuraEnd)
					nDefect3_Cnt++;
				break;
			}
		}

		if (nDefect_Cnt > 0) // 암점 제거용
		{
			nDelete_Defect_Area = (int)dAlignPara[E_PARA_CHOLE_SMALL_DEFECT_SIZE];
			nDelete_Defect_Offset = (int)dAlignPara[E_PARA_CHOLE_SMALL_DEFECT_OFFSET];

			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				if (nDelete_Defect_Area == 0 && nDelete_Defect_Offset == 0)
					continue;
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
					//추후 경향 보겠음 JHK 20200805
					//if (scrMean[0] > 0)
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
			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
				int nDefect_ImgNo = resultPanelData.m_ListDefectInfo[i].Img_Number;

				if (nDefect_Type != E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y		&&
					nDefect_Type != E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_Y	&&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT	&&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT	&&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK	&&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK   &&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK          &&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DARK          &&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_X_BRIGHT		&&
					nDefect_Type != E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT)continue;

				if (nDefect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
					nDefect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
					nDefect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
					nDefect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK ||
					nDefect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
					nDefect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
					nDefect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
					nDefect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT)
				{
					//사용 예외처리 미검 발생
					if (nDelete_Defect_Offset == 0) continue;

					int nDefect_Start_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					int nDefect_Start_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					int nDefect_End_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
					int nDefect_End_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;

					int nDefect_Center_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
					int nDefect_Center_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

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

						else if (nDefect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
							nDefect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
						{
							if ((nDefect_Center_Y > (nCHole_Start_Y - nDelete_Defect_Offset) && nDefect_Center_Y < (nCHole_Start_Y + nDelete_Defect_Offset)) ||
								(nDefect_Center_Y > (nCHole_End_Y - nDelete_Defect_Offset) && nDefect_Center_Y < (nCHole_End_Y + nDelete_Defect_Offset)))
							{
								resultPanelData.m_ListDefectInfo.RemoveAt(i--);
								break;
							}
						}

						else if (nDefect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
							nDefect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
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
				else if (nDefect_Type == E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y || nDefect_Type == E_DEFECT_JUDGEMENT_MURA_WEAK_LINEMURA_Y)
				{
					int nDefect_Start_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					int nDefect_Start_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					int nDefect_End_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
					int nDefect_End_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;
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

		int nDelete_Def_Offset = (int)dAlignPara[E_PARA_CHOLE_MURA_DEFECT_OFFSET];


		if (nDefect3_Cnt > 0 && nDelete_Def_Offset > 0) // LJH 20201019 카메라 홀부분 무라 제거
		{

			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
				int nDefect_ImgNo = resultPanelData.m_ListDefectInfo[i].Img_Number;

				if (nDefect_Type < E_DEFECT_JUDGEMENT_MURA_START || nDefect_Type > E_DEFECT_JUDGEMENT_MURA_END) continue;

				// 해당 무라 불량의 크기 
				// 
				int nDefect_Start_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
				int nDefect_Start_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
				int nDefect_End_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
				int nDefect_End_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;

				cv::Rect rMuraDef;
				rMuraDef.x = nDefect_Start_X;
				rMuraDef.y = nDefect_Start_Y;
				rMuraDef.width = nDefect_End_X - nDefect_Start_X;
				rMuraDef.height = nDefect_End_Y - nDefect_Start_Y;

				if (rMuraDef.x < 0 || rMuraDef.y < 0 || rMuraDef.width < 0 || rMuraDef.height < 0) continue;

				int nDefect_Center_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nDefect_Center_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				for (int j = 0; j < MAX_MEM_SIZE_E_INSPECT_AREA; j++)
				{
					if (tCHoleAlignData.rcCHoleROI[theApp.GetImageClassify(nDefect_ImgNo)][j].empty()) continue;
					if (nDefect_Type < E_DEFECT_JUDGEMENT_MURA_START || nDefect_Type > E_DEFECT_JUDGEMENT_MURA_END) continue;
					cv::Rect rectCHoleBuf = tCHoleAlignData.rcCHoleROI[theApp.GetImageClassify(nDefect_ImgNo)][j];

					int nCHole_Start_X = rectCHoleBuf.x - nDelete_Def_Offset;
					int nCHole_Start_Y = rectCHoleBuf.y - nDelete_Def_Offset;
					int nCHole_End_X = rectCHoleBuf.width + rectCHoleBuf.x + nDelete_Def_Offset;
					int nCHole_End_Y = rectCHoleBuf.height + rectCHoleBuf.y + nDelete_Def_Offset;
					//C홀 사이즈
					cv::Rect rMuraCHole;
					rMuraCHole.x = nCHole_Start_X;
					rMuraCHole.y = nCHole_Start_Y;
					rMuraCHole.width = nCHole_End_X - nCHole_Start_X;
					rMuraCHole.height = nCHole_End_Y - nCHole_Start_Y;

					if (rMuraCHole.x < 0 || rMuraCHole.y < 0 || rMuraCHole.width < 0 || rMuraCHole.height < 0) continue;
					// 동일한 불량
					cv::Rect rAnd;
					rAnd = rMuraDef & rMuraCHole;
					double dInRate = (double)(rAnd.width*rAnd.height) / (double)(rMuraDef.width*rMuraDef.height);
					// 카메라 홀부분과 검출된 불량 부분중 겹치는 부분이 원래 불량의 60%가 넘는다면 카메라 홀에 걸쳐있다 판정 및 제거
					if (dInRate > 0.6)
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(i--);
						break;
					}
				}
			}
		}///
	}

	return true;
}

void BumpInspection::StartThreadScanMemory(stInspectThreadParameter* stParameter)
{
	if (m_pThreadScanMemory == NULL)
	{
		m_pThreadScanMemory = AfxBeginThread(threadScanMemory, stParameter);

		if (m_pThreadScanMemory == NULL)
		{
			CString str;
			str.Format(_T("Error %s"), __func__);
			AfxMessageBox(str);
		}
		m_pThreadScanMemory->m_bAutoDelete = FALSE;
		m_eThreadWorkMemory = E_THREAD_RUNNING;
	}
	else {
		if (m_eThreadWorkDie == E_THREAD_PAUSE)
		{
			m_pThreadScanMemory->ResumeThread();
			m_eThreadWorkMemory = E_THREAD_RUNNING;
		}
	}
}

void BumpInspection::PauseThreadScanMemory()
{
	if (m_pThreadScanMemory == NULL)
	{
		AfxMessageBox(L"Thread Not start");
	}
	else {
		m_pThreadScanMemory->SuspendThread();
		m_eThreadWorkMemory = E_THREAD_PAUSE;
	}
}

void BumpInspection::StopThreadScanMemory()
{
	if (m_pThreadScanMemory == NULL)
	{
		AfxMessageBox(L"Thread Not start");
	}
	else {
		m_pThreadScanMemory->SuspendThread();

		DWORD dwResult;
		::GetExitCodeThread(m_pThreadScanMemory->m_hThread, &dwResult);

		delete m_pThreadScanMemory;
		m_pThreadScanMemory = NULL;

		m_eThreadWorkMemory = E_THREAD_STOP;
	}
}

UINT BumpInspection::threadScanMemory(LPVOID _method)
{
	stInspectThreadParameter* stParam = (stInspectThreadParameter*)_method;
	

	bool bFlag = true;
	int nCount = 0;
	int nSize = stParam->vecnMapMemory.size();
	while (bFlag)
	{
		
		for (int i = 0; i < stParam->vecnMapMemory.size(); i++)
		{
			if (stParam->matOrgImage.rows >= stParam->nFindDieROI * i)
			{
				stParam->vecnMapMemory[i] = E_MEMORY_FULL;
			}

			if (stParam->vecnMapMemory[i] == E_MEMORY_FULL)
			{
				stParam->vecnMapMemory[i] = E_MEMORY_COMPLTE;
				stParam->vecnMapSearch[i] = E_SEARCH_CAN;
				nCount++;
			}
		}
		if (nCount == nSize)
		{
			bFlag = false;
		}
	}

	return 0;
}


void BumpInspection::StartThreadScanArea(stInspectThreadParameter* stParameter)
{
	if (m_pThreadScanArea == NULL)
	{
		m_pThreadScanArea = AfxBeginThread(threadScanArea, stParameter);

		if (m_pThreadScanArea == NULL)
		{
			CString str;
			str.Format(_T("Error %s"), __func__);
			AfxMessageBox(str);
		}
		m_pThreadScanArea->m_bAutoDelete = FALSE;
		m_eThreadWorkArea = E_THREAD_RUNNING;
	}
	else {
		if (m_eThreadWorkArea == E_THREAD_PAUSE)
		{
			m_pThreadScanArea->ResumeThread();
			m_eThreadWorkArea = E_THREAD_RUNNING;
		}
	}
}

void BumpInspection::PauseThreadScanArea()
{
	if (m_pThreadScanArea == NULL)
	{
		AfxMessageBox(L"Thread Not start");
	}
	else {
		m_pThreadScanArea->SuspendThread();
		m_eThreadWorkArea = E_THREAD_PAUSE;
	}
}

void BumpInspection::StopThreadScanArea()
{
	if (m_pThreadScanArea == NULL)
	{
		AfxMessageBox(L"Thread Not start");
	}
	else {
		m_pThreadScanArea->SuspendThread();

		DWORD dwResult;
		::GetExitCodeThread(m_pThreadScanArea->m_hThread, &dwResult);

		delete m_pThreadScanArea;
		m_pThreadScanArea = NULL;

		m_eThreadWorkArea = E_THREAD_STOP;
	}
}

UINT BumpInspection::threadScanArea(LPVOID _method)
{
	stInspectThreadParameter* stParam = (stInspectThreadParameter*)_method;
	BumpInspection classBump;
	bool bFlag = true;
	int nCount = 0;
	int nSize = stParam->vecnMapSearch.size();
	
	while (bFlag)
	{
		int i = 0;
		for (i = 0; i < nSize - 1; i++)
		{
			if (stParam->vecnMapSearch[i] == E_SEARCH_CAN)
			{
				stParam->vecnMapSearch[i] = E_SEARCH_SCANING;

				cv::Rect ROI(0, i * stParam->nFindDieROI, stParam->nFullImageWidth, stParam->nFindDieROI);

				if (classBump.FindWarferArea(stParam->matOrgImage(ROI), stParam->nFristPositionThreshold, stParam->fInspectionCheckArea))
				{
					stParam->vecnMapSearch[i] = E_SEARCH_EXIST;
				}
				else {
					stParam->vecnMapSearch[i] = E_SEARCH_EMPTY;
				}
				nCount++;
			}
		}

		if (i == nSize - 1)
		{
			cv::Rect ROI(0, nCount * stParam->nFindDieROI, stParam->nFullImageWidth, stParam->nCutROIRemainder);
			if (classBump.FindWarferArea(stParam->matOrgImage(ROI), stParam->nFristPositionThreshold, stParam->fInspectionCheckArea))
			{
				stParam->vecnMapSearch[nCount] = E_SEARCH_EXIST;
			}
			else {
				stParam->vecnMapSearch[nCount] = E_SEARCH_EMPTY;
			}
			nCount++;
		}
		if (nCount == nSize)
		{
			bFlag = false;
		}
	}

	return 0;
}


// 입력값으로 들어온 Wafer의 영역을 찾음
int BumpInspection::FindWarferArea(cv::Mat& matSrc, int nFristPositionThreshold, float fInspectionCheckArea)
{
	// 영역 확인을 위하여 입력 Paramter로 threshold 진행
	cv::Mat matThresholdROI;
	cv::threshold(matSrc, matThresholdROI, nFristPositionThreshold, 1, THRESH_BINARY);

	// 더할때 영역
	cv::Rect rectSumROI(0, 0, matSrc.cols, matSrc.rows);

	// bianary화 된 영상의 합으로 영역을 확인 
	size_t sizeCheck = 0;
	sizeCheck = (size_t)cv::sum(matThresholdROI(rectSumROI))[0];

	// 현재 ROI로 취득한 영역 길이 확인
	size_t sizeArea = matSrc.cols * matSrc.rows;
	float sizeAreaRatio = (float)sizeCheck / (float)sizeArea * 100;

	// 전체 면적에 % 영역 확인
	if (sizeAreaRatio > fInspectionCheckArea)
	{
		return 1;
	}

	return 0;
}

void BumpInspection::StartThreadScanAlignMark(stInspectThreadParameter* stParameter)
{
	if (m_pThreadScanAlignMark == NULL)
	{
		m_pThreadScanAlignMark = AfxBeginThread(threadScanAlignMark, stParameter);

		if (m_pThreadScanAlignMark == NULL)
		{
			CString str;
			str.Format(_T("Error %s"), __func__);
			AfxMessageBox(str);
		}
		m_pThreadScanAlignMark->m_bAutoDelete = FALSE;
		m_eThreadWorkAlignMark = E_THREAD_RUNNING;
	}
	else {
		if (m_eThreadWorkAlignMark == E_THREAD_PAUSE)
		{
			m_pThreadScanAlignMark->ResumeThread();
			m_eThreadWorkAlignMark = E_THREAD_RUNNING;
		}
	}
}

void BumpInspection::PauseThreadScanAlignMark()
{
	if (m_eThreadWorkAlignMark == NULL)
	{
		AfxMessageBox(L"Thread Not start");
	}
	else {
		m_pThreadScanAlignMark->SuspendThread();
		m_eThreadWorkAlignMark = E_THREAD_PAUSE;
	}
}

void BumpInspection::StopThreadScanAlignMark()
{
	if (m_pThreadScanAlignMark == NULL)
	{
		AfxMessageBox(L"Thread Not start");
	}
	else {
		m_pThreadScanAlignMark->SuspendThread();

		DWORD dwResult;
		::GetExitCodeThread(m_pThreadScanAlignMark->m_hThread, &dwResult);

		delete m_pThreadScanAlignMark;
		m_pThreadScanAlignMark = NULL;

		m_eThreadWorkAlignMark = E_THREAD_STOP;
	}
}

UINT BumpInspection::threadScanAlignMark(LPVOID _method)
{
	stInspectThreadParameter* stParam = (stInspectThreadParameter*)_method;
	BumpInspection classBump;
	bool bFlag = true;
	int nCount = 0;
	int nSize = stParam->vecnMapSearch.size();
	bool bFirst = true;
	int i = 0;
	// 초기 위치 찾을 때 까지 현재 영상 찾은거 돌아다니면서 찾음 
	while (bFirst)
	{
		for( i = 0; i < stParam->vecnMapSearch.size(); i++)
		{
			cv::Rect ROI(0, i * stParam->nFindDieROI, stParam->nFullImageWidth, stParam->nFindDieROI);
			if (i == stParam->vecnMapSearch.size() - 1)
			{
				ROI.height = stParam->nCutROIRemainder;
			}

			if (stParam->vecnMapSearch[i] == E_SEARCH_EXIST)
			{
				// 초기 위치 찾는... 
				if (bFirst)
				{
					if (stParam->bImageDirection)
					{
						// 처음 찾은 영역에서 Align Mark를 찾앗는지 확인
						if (classBump.SearchAlignMark2(stParam, stParam->matOrgImage(ROI), stParam->vecmatAlignMark, i, ROI.height, true) != 0)
							// 찾지 못했다면
							bFirst = true;
						else
						{
							// 찾았다면 
							bFirst = false;
							break;
						}
					}
					else
					{
						// 처음 찾은 영역에서 Align Mark를 찾앗는지 확인
						if (classBump.SearchAlignMark2(stParam, stParam->matOrgImage(ROI), stParam->vecmatAlignMarkInvers, i, ROI.height, true) != 0)
							// 찾지 못했다면
							bFirst = true;
						else
						{
							// 찾았다면 
							bFirst = false;
							break;
						}
					}

					

				}
			}
		}
	}
	// 나머지 위치 
	while (bFlag)
	{
		classBump.SearchAlignMark2(stParam, stParam->matOrgImage, stParam->vecmatAlignMark, i, 0, false);
		
		// Test
		if (stParam->bImageDirection)
		{
			// End 추가 필요
			if (stParam->cpStartPoint.y == stParam->nMapMaxY)
			{
				if (stParam->bSaveImage)
				{
					stParam->csMarkScan.Lock();
					cv::Mat matMapData;
					matMapData = cv::Mat::zeros(cv::Size(stParam->nMapWidth, stParam->nMapHeight), CV_8U);
					uchar* ucpMapData = matMapData.data;
					for (int y = 0; y < matMapData.rows; y++)
					{
						for (int x = 0; x < matMapData.cols; x++)
						{
							if (stParam->structMapDie[y][x].nDie_Class == GOOD_DIE)
								ucpMapData[y * matMapData.cols + x] += 1;
							if (stParam->structMapDie[y][x].bFoundMark == true)
								ucpMapData[y * matMapData.cols + x] += 1;
						}
					}

					CString strSave;
					CString strName;
					strName.Format(_T("\\%d.DieMap.bmp"), stParam->nImageCountMark);
					strSave = stParam->strFolderPathMark + strName;
					cv::imwrite((string)CT2CA(strSave), matMapData);
					stParam->csMarkScan.Unlock();
				}


				//for(pDieArr)
				bFlag = false;
			}
		}
		else
		{
			if (stParam->cpStartPoint.y == stParam->nMapMinY)
			{
				if (stParam->bSaveImage)
				{
					stParam->csMarkScan.Lock();
					cv::Mat matMapData;
					matMapData = cv::Mat::zeros(cv::Size(stParam->nMapWidth, stParam->nMapHeight), CV_8U);
					uchar* ucpMapData = matMapData.data;
					for (int y = 0; y < matMapData.rows; y++)
					{
						for (int x = 0; x < matMapData.cols; x++)
						{
							if (stParam->structMapDie[y][x].nDie_Class == GOOD_DIE)
								ucpMapData[y * matMapData.cols + x] += 1;
							if (stParam->structMapDie[y][x].bFoundMark == true)
								ucpMapData[y * matMapData.cols + x] += 1;
						}
					}

					CString strSave;
					CString strName;
					strName.Format(_T("\\%d.DieMap.bmp"), stParam->nImageCountMark);
					strSave = stParam->strFolderPathMark + strName;
					cv::imwrite((string)CT2CA(strSave), matMapData);
					stParam->csMarkScan.Unlock();
				}


				//for(pDieArr)
				bFlag = false;
			}
		}
		
		i++;
	}



	return 0;
}

int BumpInspection::SearchAlignMark2(stInspectThreadParameter* stParam, cv::Mat & matSrc, vector<cv::Mat>& vecmatAlignMark, int nStartYPos, int nCutLength, bool bType)
{
	// 처음위치 찾는 경우
	if (bType)
	{
		// Align Mark가 여러개 일경우 대비
		vector<cv::Point> vecCenterPoint;
		vector<cv::Mat> vecFoundMark;
		vector<cv::Rect> vecRectMark;

		cv::Mat matBilateralOrigin;
		cv::Mat matBilateralMark;
		cv::Mat matSobelOriginX;
		cv::Mat matSobelOriginY;
		cv::Mat matSobelOriginXY;
		cv::Mat matSobelMarkX;
		cv::Mat matSobelMarkY;
		cv::Mat matSobelMarkXY;

		// TemplateMatching
		cv::Mat matTemplateResult;
		// TemplateMatching 결과 영상 미리 할당
		int nTemplateResultWidth = matSrc.cols - vecmatAlignMark[0].cols + 1;
		int nTemplateResultHeight = matSrc.rows - vecmatAlignMark[0].rows + 1;
		matTemplateResult.create(nTemplateResultHeight, nTemplateResultWidth, CV_32F);
#if GPU
		// GPU
		cv::cuda::GpuMat gpumatOrigin;
		cv::cuda::GpuMat gpumatAlignMark;
		cv::cuda::GpuMat gpumatBilateralOrigin;
		cv::cuda::GpuMat gpumatBilateralMark;
		cv::cuda::GpuMat gpumatSobelOriginX;
		cv::cuda::GpuMat gpumatSobelOriginY;
		cv::cuda::GpuMat gpumatSobelOriginAbsX;
		cv::cuda::GpuMat gpumatSobelOriginAbsY;
		cv::cuda::GpuMat gpumatSobelOriginAbsXY;
		cv::cuda::GpuMat gpumatSobelOriginAbsXY8Bit;
		cv::cuda::GpuMat gpumatSobelMarkX;
		cv::cuda::GpuMat gpumatSobelMarkY;
		cv::cuda::GpuMat gpumatSobelMarkAbsX;
		cv::cuda::GpuMat gpumatSobelMarkAbsY;
		cv::cuda::GpuMat gpumatSobelMarkAbsXY;
		cv::cuda::GpuMat gpumatSobelMarkAbsXY8Bit;
		cv::cuda::GpuMat gpumatMeanFilterOrigin;
		cv::cuda::GpuMat gpumatMeanFilterMark;

		cv::cuda::Stream stream1;
		cv::cuda::Stream stream2;

		cv::cuda::GpuMat gpumatTemplateResult;
#endif 

#if GPU
		gpumatOrigin.upload(matSrc);
		gpumatAlignMark.upload(vecmatAlignMark[0]);
#endif 
#if CPU
		cv::bilateralFilter(matSrc, matBilateralOrigin, -1, 15, 15);
		cv::bilateralFilter(vecmatAlignMark[0], matBilateralMark, -1, 15, 15);

		cv::Sobel(matBilateralOrigin, matSobelOriginX, CV_32F, 1, 0);
		cv::Sobel(matBilateralOrigin, matSobelOriginY, CV_32F, 0, 1);

		cv::magnitude(matSobelOriginX, matSobelOriginY, matSobelOriginXY);

		cv::Sobel(matBilateralMark, matSobelMarkX, CV_32F, 1, 0);
		cv::Sobel(matBilateralMark, matSobelMarkY, CV_32F, 0, 1);

		cv::magnitude(matSobelMarkX, matSobelMarkY, matSobelMarkXY);


		// CPU
		cv::matchTemplate(matSobelOriginXY, matSobelMarkXY, matTemplateResult, TM_SQDIFF);
#endif
#if GPU
		// Noise 제거 
		cv::cuda::bilateralFilter(gpumatOrigin, gpumatBilateralOrigin, -1, 15, 15);
	

		// 메모리 해제
		gpumatOrigin.release();
		

		//Sobel X, Y Filter 생성
		cv::Ptr<cuda::Filter> SobelfilterX = cv::cuda::createSobelFilter(CV_8U, CV_32F, 1, 0);
		cv::Ptr<cuda::Filter> SobelfilterY = cv::cuda::createSobelFilter(CV_8U, CV_32F, 0, 1);

		// 적용
		SobelfilterX->apply(gpumatBilateralOrigin, gpumatSobelOriginX);
		SobelfilterY->apply(gpumatBilateralOrigin, gpumatSobelOriginY);

		gpumatBilateralOrigin.release();

		// 절대값으로 변환
		cv::cuda::abs(gpumatSobelOriginX, gpumatSobelOriginAbsX);
		cv::cuda::abs(gpumatSobelOriginY, gpumatSobelOriginAbsY);

		// 메모리 해제
		gpumatSobelOriginX.release();
		gpumatSobelOriginY.release();

		// 두영상 합치고 데이터 0.5배 적용 
		cv::cuda::addWeighted(gpumatSobelOriginAbsX, 0.5, gpumatSobelOriginAbsY, 0.5, 0, gpumatSobelOriginAbsXY);

		// 메모리 해제
		gpumatSobelOriginAbsX.release();
		gpumatSobelOriginAbsY.release();

		// 합친영상 8bit로 변환
		gpumatSobelOriginAbsXY.convertTo(gpumatSobelOriginAbsXY8Bit, CV_8U);

		// 메모리 해제
		gpumatSobelOriginAbsXY.release();

		SobelfilterX.release();
		SobelfilterY.release();

		cv::cuda::bilateralFilter(gpumatAlignMark, gpumatBilateralMark, -1, 15, 15);
		gpumatAlignMark.release();

		SobelfilterX = cv::cuda::createSobelFilter(CV_8U, CV_32F, 1, 0);
		SobelfilterY = cv::cuda::createSobelFilter(CV_8U, CV_32F, 0, 1);

		// Sobel 적용
		SobelfilterX->apply(gpumatBilateralMark, gpumatSobelMarkX);
		SobelfilterY->apply(gpumatBilateralMark, gpumatSobelMarkY);

		SobelfilterX.release();
		SobelfilterY.release();

		// 메모리 해제
		gpumatBilateralMark.release();

		// 절대값으로 변환
		cv::cuda::abs(gpumatSobelMarkX, gpumatSobelMarkAbsX);
		cv::cuda::abs(gpumatSobelMarkY, gpumatSobelMarkAbsY);

		// 메모리 해제
		gpumatSobelMarkX.release();
		gpumatSobelMarkY.release();

		// 두영상 합치고 데이터 0.5배 적용 
		cv::cuda::addWeighted(gpumatSobelMarkAbsX, 0.5, gpumatSobelMarkAbsY, 0.5, 0, gpumatSobelMarkAbsXY);

		// 메모리 해제
		gpumatSobelMarkAbsX.release();
		gpumatSobelMarkAbsY.release();
		// 합친영상 8bit로 변환
		gpumatSobelMarkAbsXY.convertTo(gpumatSobelMarkAbsXY8Bit, CV_8U);

		cv::Ptr<cuda::Filter> MeanFilter = cv::cuda::createBoxFilter(CV_8U, CV_8U, cv::Size(11, 11));

		MeanFilter->apply(gpumatSobelOriginAbsXY8Bit, gpumatMeanFilterOrigin);
		MeanFilter->apply(gpumatSobelMarkAbsXY8Bit, gpumatMeanFilterMark);

		MeanFilter.release();

		cv::cuda::subtract(gpumatMeanFilterOrigin, gpumatSobelOriginAbsXY8Bit, gpumatSobelOriginAbsXY8Bit);
		cv::cuda::subtract(gpumatMeanFilterMark, gpumatSobelMarkAbsXY8Bit, gpumatSobelMarkAbsXY8Bit);

		cv::cuda::threshold(gpumatSobelOriginAbsXY8Bit, gpumatMeanFilterOrigin, 3, 255, THRESH_BINARY);
		cv::cuda::threshold(gpumatSobelMarkAbsXY8Bit, gpumatMeanFilterMark, 3, 255, THRESH_BINARY);

		gpumatSobelOriginAbsXY8Bit.release();
		gpumatSobelMarkAbsXY8Bit.release();

		// 메모리 해제
		gpumatSobelMarkAbsXY.release();
		cv::Ptr<cv::cuda::TemplateMatching> pCudaTemplateMatching;
		pCudaTemplateMatching = cv::cuda::createTemplateMatching(CV_8U, TM_SQDIFF, Size(0, 0));
		pCudaTemplateMatching->match(gpumatMeanFilterOrigin, gpumatMeanFilterMark, gpumatTemplateResult);

		gpumatTemplateResult.download(matTemplateResult);
		gpumatTemplateResult.release();
		gpumatMeanFilterOrigin.download(matSobelOriginXY);
		gpumatMeanFilterMark.download(matSobelMarkXY);
		gpumatMeanFilterOrigin.release();
		gpumatMeanFilterMark.release();

#endif 
		//영상 정규화 // 이거 너무 오래걸림..
		//normalize(matTemplateResult, matTemplateResult, 0, 255, NORM_MINMAX, -1, Mat());

		if (stParam->bSaveImage)
		{
			CString strSave;
			CString strName;
			strName.Format(_T("\\%d.TemplateResult_%d.bmp"), stParam->nImageCountMark++, nStartYPos);
			strSave = stParam->strFolderPathMark + strName;
			cv::Mat matTemplateResultSave;
			cv::normalize(matTemplateResult, matTemplateResultSave, 0, 255, NORM_MINMAX, -1, Mat());
			cv::imwrite((string)CT2CA(strSave), matTemplateResultSave);
		}

		// 가장 잘 맞는부분 후보군 추출
		cv::Mat matTemplateThreshold;
		// Threshold값 파라미터로 빼야함
		float fThreshold = 2.0e+9;

		cv::threshold(matTemplateResult, matTemplateThreshold, fThreshold, 255, CV_THRESH_BINARY_INV);

		cv::Mat matThreshold8bit;
		matTemplateThreshold.convertTo(matThreshold8bit, CV_8U);

		if (stParam->bSaveImage)
		{
			CString strSave;
			CString strName;
			strName.Format(_T("\\%d.TemplatePostistion_%d.bmp"), stParam->nImageCountMark++, nStartYPos);
			strSave = stParam->strFolderPathMark + strName;
			cv::imwrite((string)CT2CA(strSave), matThreshold8bit);
		}

		cv::Mat matLabel, matStats, matCentroid;

		matLabel = cv::Mat::zeros(matTemplateThreshold.size(), CV_32SC1);

		__int64 nTotalLabel = 0;
		nTotalLabel = cv::connectedComponentsWithStats(matThreshold8bit, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

		// 우리가 찾는 데이터가 없음 다시 위로 올라가서 다시 검색
		if (nTotalLabel == 0)
		{
			return 1;
		}
		SetMapStartPosition(stParam);

		TemplateResultPositionCheck(stParam, nTotalLabel, matSrc, matTemplateResult, matLabel, matStats, vecmatAlignMark[0].cols, vecmatAlignMark[0].rows, vecFoundMark, vecRectMark, nStartYPos * nCutLength);
		//ExtractAlignMark(nAlignMarkType, vecFoundMark, vecRectMark, vecCenterPoint);
		// 위치 보정
		// 처음 Align Mark가 존재하는 Map 위치에서부터 시작해야함
		// 맵 처음위치 좌표 알려줘야함..

		CheckMarkstate(stParam);

		MapAlignMarkMatching3(stParam);

	}
	else
	{
		NextAlingMarkPosition(stParam);
	}


	return 0;
}


int BumpInspection::TemplateResultPositionCheck(stInspectThreadParameter* stParam, int nTotalLabel, cv::Mat& matSrc, cv::Mat& matTemplateResult, cv::Mat& matLabel, cv::Mat& matStats, int nAlignMarkWidth, int nAlignMarkHeight, vector<cv::Mat>& vecmatFoundMark, vector<cv::Rect>& vecRectMark, int nStartYPos)
{
#pragma omp parallel for schedule(dynamic)
	for (int idx = 1; idx <= nTotalLabel; idx++)
	{
		// blob 정보
		int* npStats = (int*)&matStats.at<int>(0, 0);
		int nStatsPos = idx * 5;
		// object Rect
		cv::Rect rectBox;
		rectBox.x = npStats[nStatsPos];
		rectBox.y = npStats[nStatsPos + 1];
		rectBox.width = npStats[nStatsPos + 2];
		rectBox.height = npStats[nStatsPos + 3];

		int nSX = rectBox.x;
		int nSY = rectBox.y;
		int nEX = rectBox.x + rectBox.width;
		int nEY = rectBox.y + rectBox.height;

		// 영역 벗어나는지 예외처리
		if (nSX < 0)	nSX = 0;
		if (nSY < 0)	nSY = 0;
		if (nSX >= matTemplateResult.cols)	continue;
		if (nSY >= matTemplateResult.rows)	continue;
		if (nEX >= matTemplateResult.cols)	nEX = matTemplateResult.cols - 1;
		if (nEY >= matTemplateResult.rows)	nEY = matTemplateResult.rows - 1;

		cv::Rect rectTemp(nSX, nSY, nEX - nSX + 1, nEY - nSY + 1);

		cv::Mat matTmp_src = matTemplateResult(rectTemp);		//	원본의 ROI
		cv::Mat matTmp_label = matLabel(rectTemp);				//	Label 의 ROI

		double dMin = 1e+10;
		int nYPos = -1;
		int nXPos = -1;
		for (int y = 0; y < rectTemp.height; y++)
		{
			int		*ptrLabel = (int *)matTmp_label.ptr(y);
			float	*ptrGray = (float *)matTmp_src.ptr(y);

			for (int x = 0; x < rectTemp.width; x++, ptrLabel++, ptrGray++)
			{
				// 해당 객체
				if (*ptrLabel == idx)
				{
					if (dMin > *ptrGray)
					{
						dMin = *ptrGray;
						nYPos = y;
						nXPos = x;
					}
				}
			}
		}

		// 대략적 위치 찾음
		if (dMin != 1e+10)
		{
			// 처음 입력 영상 위치에서 찾기
			int nStartX = rectBox.x + nXPos;
			int nStartY = rectBox.y + nYPos;
			int nEndX = nStartX + nAlignMarkWidth;
			int nEndY = nStartY + nAlignMarkHeight;
			if (nStartX < 0)
				continue;
			if (nStartY < 0)
				continue;
			if (nEndX < 0 || nEndX > matSrc.cols)
				continue;
			if (nEndY < 0 || nEndY > matSrc.rows)
				continue;

			int nRectWidth = nEndX - nStartX;
			int nRectHeight = nEndY - nStartY;

			cv::Rect rectPos(nStartX, nStartY, nRectWidth, nRectHeight);

			stParam->csMarkScan.Lock();
			cv::Mat matFoundMark;
			matSrc(rectPos).copyTo(matFoundMark);
			//vecmatFoundMark.push_back(matSrc(rectPos));
			rectPos.y += nStartYPos;

			// 원본 위치로 변경하여 넣어줌
			vecRectMark.push_back(rectPos);

			
			// 처음 위치 확인
			// 정방향
			if (stParam->bImageDirection)
			{
				int nX = rectPos.x / (stParam->fDieWidth * 1000);
				int nCameraScanPositionX = stParam->nCameraScanPositionX;
				int nCameraScanPositionY = stParam->cpStartPoint.y;
				int nWaferMapX = nCameraScanPositionX + nX;

				if (stParam->structMapDie[nCameraScanPositionY][nWaferMapX].nDie_Class == GOOD_DIE)
				{
					// 처음위치 확인 구문
					stParam->structMapDie[nCameraScanPositionY][nWaferMapX].vecPoint.push_back(cv::Point(rectPos.x, rectPos.y));
					stParam->structMapDie[nCameraScanPositionY][nWaferMapX].vecRect.push_back(rectPos);

					if (stParam->bSaveImage)
					{
						CString strSave;
						CString strName;
						strName.Format(_T("\\FoundMark_PosX%d_PosY%d_x%d_y%d.bmp"), nWaferMapX, nCameraScanPositionY, nStartX, nStartY);
						strSave = stParam->strFolderPathMark + strName;
						cv::imwrite((string)CT2CA(strSave), matFoundMark);
					}
				}
			}
			else// 역방향
			{
				int nX = rectPos.x / (stParam->fDieWidth * 1000);
				int nCameraScanPositionX = stParam->nCameraScanPositionX;
				int nCameraScanPositionY = stParam->cpStartPoint.y;
				int nCamScanDie = stParam->nCamScanDie - 1;

				int nWaferMapX = nCameraScanPositionX + (nCamScanDie - nX);

				if (stParam->structMapDie[nCameraScanPositionY][nWaferMapX].nDie_Class == GOOD_DIE)
				{
					// 처음위치 확인 구문
					stParam->structMapDie[nCameraScanPositionY][nWaferMapX].vecPoint.push_back(cv::Point(rectPos.x, rectPos.y));
					stParam->structMapDie[nCameraScanPositionY][nWaferMapX].vecRect.push_back(rectPos);

					if (stParam->bSaveImage)
					{
						CString strSave;
						CString strName;
						strName.Format(_T("\\FoundMark_PosX%d_PosY%d_x%d_y%d.bmp"), nWaferMapX, nCameraScanPositionY, nStartX, nStartY);
						strSave = stParam->strFolderPathMark + strName;
						cv::imwrite((string)CT2CA(strSave), matFoundMark);
					}
				}
			}

		
			stParam->csMarkScan.Unlock();



		}
	}

	return 0;
}

int BumpInspection::SetMapStartPosition(stInspectThreadParameter* stParam)
{
	if (stParam->bImageDirection)
	{
		stParam->cpStartPoint.x = stParam->nCameraScanPositionX;
		stParam->cpStartPoint.y = stParam->nMapMinY;
	}
	else
	{
		stParam->cpStartPoint.x = stParam->nCameraScanPositionX;
		stParam->cpStartPoint.y = stParam->nMapMaxY;
	}

	

	return 0;
}

int BumpInspection::CheckMarkstate(stInspectThreadParameter* stParam)
{
	int nStartX = stParam->cpStartPoint.x;
	int nStartY = stParam->cpStartPoint.y;
	int nSizeX = nStartX + stParam->nCamScanDie;

	for (int x = nStartX; x < nSizeX; x++)
	{
		if (stParam->structMapDie[nStartY][x].nDie_Class == GOOD_DIE)
		{
			if (stParam->structMapDie[nStartY][x].vecPoint.size() == 0)
			{
				stParam->structMapDie[nStartY][x].vecPoint.push_back(cv::Point(0, 0));
				stParam->structMapDie[nStartY][x].vecRect.push_back(cv::Rect(0, 0, 0, 0));
			}
		}
	}
		
	return 0;
}

int BumpInspection::MapAlignMarkMatching3(stInspectThreadParameter* stParam)
{
	int nAlignMarkSize = stParam->vecmatAlignMark.size();
	int nSearchSize = nAlignMarkSize - 1;

	int nStartX = stParam->cpStartPoint.x;
	int nStartY = stParam->cpStartPoint.y;
	int nSizeX = nStartX + stParam->nCamScanDie;


	// AlignMark 등록 개수가 1개 이상일 때만 사용
	if (nAlignMarkSize > 1)
	{
		bool bOffsetFirst = true;
		// 이건 병렬 해야지.
#pragma omp parallel for schedule(dynamic)
		for (int x = nStartX; x < nSizeX; x++)
		{
			// 첫번째 위치를 기준으로 나머지 위치를 전부 찾음
			// 하나라도 없으면 못 찾은 거로 처리
			// 다음 것을 찾아야하는지 확인하는 처리
			// GijunAdd 220603
			bool bFlagNext = true;
			if (stParam->structMapDie[nStartY][x].vecRect.size() == 0) {
				bFlagNext = false;
				continue;
			}

			int nNextPosX = 0;
			int nNextPosY = 0;

			int nNextStartX = 0;
			int nNextStartY = 0;
			// 임시로 위치 담을 버퍼
			vector<cv::Point> vecTemp;
			// 병렬 안됨 순차 접근해야함

			// 처음 위치 
			vecTemp.push_back(cv::Point(stParam->structMapDie[nStartY][x].vecRect[0].x, stParam->structMapDie[nStartY][x].vecRect[0].y));

			for (int m = 0; m < nSearchSize; m++)
			{
				if (!bFlagNext)
				{
					continue;
				}

				if (m == 0)
				{
					nNextPosX = stParam->structMapDie[nStartY][x].vecRect[m].x + stParam->vecInsideDieAlignMarkDistance[m].x - stParam->vecmatAlignMark[m + 1].cols;
					nNextPosY = stParam->structMapDie[nStartY][x].vecRect[m].y + stParam->vecInsideDieAlignMarkDistance[m].y;
				}
				else if (m == 1)
				{
					nNextPosX = nNextStartX + stParam->vecmatAlignMark[m].cols - stParam->vecmatAlignMark[m + 1].cols;
					nNextPosY = nNextStartY + stParam->vecInsideDieAlignMarkDistance[m].y - stParam->vecmatAlignMark[m + 1].rows;
				}
				else if (m == 2)
				{
					nNextPosX = nNextStartX + stParam->vecmatAlignMark[m].cols + stParam->vecInsideDieAlignMarkDistance[m].x;
					nNextPosY = nNextStartY + stParam->vecmatAlignMark[m].rows + stParam->vecInsideDieAlignMarkDistance[m].y - stParam->vecmatAlignMark[m + 1].rows;
				}

				// 예외 처리 필요
				cv::Rect rectROI(nNextPosX - stParam->nAlignSearchPitch,
					nNextPosY - stParam->nAlignSearchPitch,
					stParam->vecmatAlignMark[m + 1].cols + stParam->nAlignSearchPitch * 2,
					stParam->vecmatAlignMark[m + 1].rows + stParam->nAlignSearchPitch * 2);

				if (rectROI.x + rectROI.width > stParam->nFullImageWidth ||
					rectROI.y + rectROI.height > stParam->nFullImageHeight ||
					rectROI.x < 0 ||
					rectROI.y < 0)
				{
					bFlagNext = false;
					if (stParam->bSaveImage)
					{
						stParam->csMarkScan.Lock();
						cv::Mat matTemp;
						matTemp.create(cv::Size(1,1),CV_8U);
						CString strSave;
						CString strName;
						strName.Format(_T("\\x%d y%d m%d ImageX%d ImageX%d err%d AlignMark_Find.bmp"), x, nStartY, m + 1, rectROI.x, rectROI.y, rectROI.x + rectROI.width - stParam->nFullImageWidth);
						strSave = stParam->strFolderPathMark + strName;
						cv::imwrite((string)CT2CA(strSave), matTemp);
						stParam->csMarkScan.Unlock();
						matTemp.release();
					}
					continue;
				}

				// 영역 따기 전에 예외처리 필요
				
				cv::Mat matROICurrent;
				cv::Mat matROIAlign;
				cv::bilateralFilter(stParam->matOrgImage(rectROI), matROICurrent, -1, 15, 15);
				cv::bilateralFilter(stParam->vecmatAlignMark[m + 1], matROIAlign, -1, 15, 15);

				cv::Mat SobelCurrent1;
				cv::Mat SobelCurrent2;
				cv::Mat SobelAlign1;
				cv::Mat SobelAlign2;
				cv::Sobel(matROICurrent, SobelCurrent1, CV_32F, 1, 0);
				cv::Sobel(matROICurrent, SobelCurrent2, CV_32F, 0, 1);

				cv::Sobel(matROIAlign, SobelAlign1, CV_32F, 1, 0);
				cv::Sobel(matROIAlign, SobelAlign2, CV_32F, 0, 1);

				cv::magnitude(SobelCurrent1, SobelCurrent2, SobelCurrent1);
				cv::magnitude(SobelAlign1, SobelAlign2, SobelAlign1);

				SobelCurrent2.release();
				SobelAlign2.release();

				SobelCurrent1.convertTo(SobelCurrent2, CV_8U);
				SobelAlign1.convertTo(SobelAlign2, CV_8U);

				SobelCurrent1.release();
				SobelAlign1.release();

				cv::blur(SobelCurrent2, SobelCurrent1, cv::Size(11, 11));
				cv::blur(SobelAlign2, SobelAlign1, cv::Size(11, 11));

				cv::subtract(SobelCurrent2, SobelCurrent1, SobelCurrent1);
				cv::subtract(SobelAlign2, SobelAlign1, SobelAlign1);

				cv::threshold(SobelCurrent1, SobelCurrent2, 3, 255, THRESH_BINARY);
				cv::threshold(SobelAlign1, SobelAlign2, 3, 255, THRESH_BINARY);

				cv::Mat matTemplateResult;
				int nTemplateResultWidth = matROICurrent.cols - matROIAlign.cols + 1;
				int nTemplateResultHeight = matROICurrent.rows - matROIAlign.rows + 1;
				matTemplateResult.create(nTemplateResultHeight, nTemplateResultWidth, CV_32F);

				// mask 사용가능한지 확인 필요
				cv::matchTemplate(SobelCurrent2, SobelAlign2, matTemplateResult, TM_SQDIFF);

				double dMin = 0;
				cv::Point pointMin;
				cv::minMaxLoc(matTemplateResult, &dMin, NULL, &pointMin, NULL);

				matROICurrent.release();
				matROIAlign.release();
				SobelAlign1.release();
				SobelAlign2.release();
				SobelCurrent1.release();
				SobelCurrent2.release();
				matTemplateResult.release();


				double dCompareValue = 2.0e+9;

				if (dMin < dCompareValue)
				{
					if (stParam->bSaveImage)
					{
						stParam->csMarkScan.Lock();
						CString strSave;
						CString strName;
						strName.Format(_T("\\x%d y%d m%d ImageX%d ImageY%d AlignMark_Find.bmp"), x, nStartY, m + 1, rectROI.x, rectROI.y);
						strSave = stParam->strFolderPathMark + strName;
						cv::imwrite((string)CT2CA(strSave), stParam->matOrgImage(rectROI));
						stParam->csMarkScan.Unlock();
					}


					nNextStartX = rectROI.x + pointMin.x;
					nNextStartY = rectROI.y + pointMin.y;

					if (m == 0)
					{
						// 두번째 위치
						vecTemp.push_back(cv::Point(nNextStartX + stParam->vecmatAlignMark[m + 1].cols, nNextStartY));
					}
					else if (m == 1)
					{
						// 세번째 위치
						vecTemp.push_back(cv::Point(nNextStartX + stParam->vecmatAlignMark[m + 1].cols, nNextStartY + stParam->vecmatAlignMark[m + 1].rows));
					}
					else if (m == 2)
					{
						// 네번째 위치
						vecTemp.push_back(cv::Point(nNextStartX, nNextStartY + stParam->vecmatAlignMark[m + 1].rows));
					}
					else {
						vecTemp.push_back(cv::Point(-1, -1));
					}
				}
				else {
					bFlagNext = false;
					if (stParam->bSaveImage)
					{
						stParam->csMarkScan.Lock();
						CString strSave;
						CString strName;
						strName.Format(_T("\\x%d y%d m%d ImageX%d ImageY%d AlignMark_Find Fail.bmp"), /*stParam->nImageCountMark++, */x, nStartY, m + 1, rectROI.x, rectROI.y);
						strSave = stParam->strFolderPathMark + strName;
						cv::imwrite((string)CT2CA(strSave), stParam->matOrgImage(rectROI));
						stParam->csMarkScan.Unlock();
					}
					// 만약 못 찾았을때 예외처리 필요
				}
			}

			if (bFlagNext)
			{
				// 찾은 위치가 모두 존재 
				if (vecTemp.size() == nAlignMarkSize)
				{
					if (stParam->structMapDie[stParam->cpStartPoint.y][x].nDie_Class == GOOD_DIE && stParam->structMapDie[stParam->cpStartPoint.y][x].bFoundMark == false)
					{
						cv::Rect rectROI;
						int nMaxX = vecTemp[0].x;
						int nMaxY = vecTemp[0].y;
						int nMinX = vecTemp[0].x;
						int nMinY = vecTemp[0].y;
						for (int i = 1; i < stParam->vecmatAlignMark.size(); i++)
						{
							stParam->structMapDie[stParam->cpStartPoint.y][x].vecPoint.push_back(vecTemp[i]);
							if (vecTemp[i].x > nMaxX)
							{
								nMaxX = vecTemp[i].x;
							}
							if (vecTemp[i].y > nMaxY)
							{
								nMaxY = vecTemp[i].y;
							}
							if (vecTemp[i].x < nMinX)
							{
								nMinX = vecTemp[i].x;
							}
							if (vecTemp[i].y < nMinY)
							{
								nMinY = vecTemp[i].y;
							}
						}
						rectROI.x = nMinX;
						rectROI.y = nMinY;
						rectROI.width = nMaxX - nMinX + 1;
						rectROI.height = nMaxY - nMinY + 1;

						stParam->structMapDie[stParam->cpStartPoint.y][x].rectDie = rectROI;
						stParam->structMapDie[stParam->cpStartPoint.y][x].bFoundMark = true;
						stParam->structMapDie[stParam->cpStartPoint.y][x].matDie = stParam->matOrgImage(rectROI).clone();
						
						if (stParam->bSaveImage)
						{
							stParam->csExtractDie.Lock();
							CString strSave;
							CString strName;
							strName.Format(_T("\\x%d y%d_ImageX%d ImageY%d ExtractDie.bmp"), /*stParam->nImageCountExtractDie++,*/ x, stParam->cpStartPoint.y, vecTemp[0].x, vecTemp[0].y);
							strSave = stParam->strFolderPathExtractDie + strName;

							cv::Mat matSaveImage;
							cv::Mat matTempImage;
							matTempImage = cv::Mat::zeros(cv::Size(rectROI.width, rectROI.height), CV_8U);

							stParam->matOrgImage(rectROI).copyTo(matSaveImage);
							vector<cv::Point> vecTempPoint;
							for (int i = 0; i < stParam->vecmatAlignMark.size(); i++)
							{
								// ROI쪽으로 좌표계 맞춤
								cv::Point tempPoint;
								tempPoint.x = vecTemp[i].x - nMinX;
								tempPoint.y = vecTemp[i].y - nMinY;
								vecTempPoint.push_back(tempPoint);
							}

							cv::fillPoly(matTempImage, vecTempPoint, cv::Scalar(255));

							cv::bitwise_and(matSaveImage, matTempImage, matSaveImage);

							cv::imwrite((string)CT2CA(strSave), matSaveImage);
							stParam->csExtractDie.Unlock();

							vecTempPoint.clear();
							std::vector<Point>().swap(vecTempPoint);
						}

						vecTemp.clear();
						std::vector<Point>().swap(vecTemp);
					}
				}
			}
			else {
				// 못 찾았을 때
				// 
				continue;
			}
		}
	}
	else
	{

	}
	return 0;
}

// 현재 위치로 다음 위치 추정
int BumpInspection::NextAlingMarkPosition(stInspectThreadParameter* stParam)
{
	bool bFind = true;

	// X 방향 Scan하는 개수와 현재 스캔하는 위치
	int nCheckSizeX = stParam->cpStartPoint.x + stParam->nCamScanDie;
	int nCheckStartX = stParam->cpStartPoint.x;

	for (int i = nCheckStartX; i < nCheckSizeX; i++)
	{
		if (stParam->structMapDie[stParam->cpStartPoint.y][i].vecPoint.size() == 0)
		{
			return 1;
		}
	}

	int nWidth = stParam->nFullImageWidth;
	int nHeight = stParam->nFullImageHeight;

	// 일단 아래쪽으로 찾기
	vector<cv::Point> vecPoint;
	vector<cv::Rect> vecRect;

	//vecPoint.resize(nCamScanDie);
	//vecRect.resize(nCamScanDie);
#pragma omp parallel for schedule(dynamic)
	for (int x = nCheckStartX; x < nCheckSizeX; x++)
	{
		// 한개만 찾아서 옆이나 위쪽으로 확인해야되는 경우... 예외처리 필요
		cv::Mat matAlignROI;

		// 처음 위치 
		cv::Rect rectROI(stParam->structMapDie[stParam->cpStartPoint.y][x].vecPoint[0].x - stParam->nAlignSearchPitch,
			stParam->structMapDie[stParam->cpStartPoint.y][x].vecPoint[0].y - stParam->nAlignSearchPitch + stParam->nAlignMarkDistanceY,
			stParam->vecmatAlignMark[0].cols + stParam->nAlignSearchPitch * 2,
			stParam->vecmatAlignMark[0].rows + stParam->nAlignSearchPitch * 2);

		// 예외 처리
		// GijunAdd 220603
		// stParam->structMapDie[stParam->cpStartPoint.y][x].vecPoint[0].x 가 0 인경우가 발생하면, Pitch를 빼서 - 좌표가 넘어옴...
		if (rectROI.x < 0) rectROI.x = 0;
		if (rectROI.y < 0) rectROI.y = 0;
		if (rectROI.x > nWidth ||
			rectROI.y > nHeight ||
			rectROI.width > nWidth ||
			rectROI.height > nHeight)
		{
			if (stParam->bSaveImage)
			{
				stParam->csMarkScan.Lock();
				cv::Mat matTemp;
				matTemp.create(cv::Size(1, 1), CV_8U);
				CString strSave;
				CString strName;
				strName.Format(_T("\\x%d y%d m%d ImageX%d ImageX%d err%d AlignMark_Find.bmp"), x, stParam->cpStartPoint.y, 0, rectROI.x, rectROI.y, rectROI.x + rectROI.width - stParam->nFullImageWidth);
				strSave = stParam->strFolderPathMark + strName;
				cv::imwrite((string)CT2CA(strSave), matTemp);
				stParam->csMarkScan.Unlock();
			}
			// 오류 코드?
			continue;
		}

		// Align Mark 위치 취득 
		stParam->matOrgImage(rectROI).copyTo(matAlignROI);

		// TemplateMatching으로 데이터 뽑음
		cv::Mat matTemplateResult;

		cv::Mat matROICurrent;
		cv::Mat matROIAlign;

		cv::bilateralFilter(matAlignROI, matROICurrent, -1, 15, 15);
		cv::bilateralFilter(stParam->vecmatAlignMark[0], matROIAlign, -1, 15, 15);

		cv::Mat SobelCurrent1;
		cv::Mat SobelCurrent2;
		cv::Mat SobelAlign1;
		cv::Mat SobelAlign2;
		cv::Sobel(matROICurrent, SobelCurrent1, CV_32F, 1, 0);
		cv::Sobel(matROICurrent, SobelCurrent2, CV_32F, 0, 1);
		cv::Sobel(matROIAlign, SobelAlign1, CV_32F, 1, 0);
		cv::Sobel(matROIAlign, SobelAlign2, CV_32F, 0, 1);
		cv::magnitude(SobelCurrent1, SobelCurrent2, SobelCurrent1);
		cv::magnitude(SobelAlign1, SobelAlign2, SobelAlign1);
		SobelCurrent2.release();
		SobelAlign2.release();

		SobelCurrent1.convertTo(SobelCurrent2, CV_8U);
		SobelAlign1.convertTo(SobelAlign2, CV_8U);

		SobelCurrent1.release();
		SobelAlign1.release();

		cv::blur(SobelCurrent2, SobelCurrent1, cv::Size(11, 11));
		cv::blur(SobelAlign2, SobelAlign1, cv::Size(11, 11));

		cv::subtract(SobelCurrent2, SobelCurrent1, SobelCurrent1);
		cv::subtract(SobelAlign2, SobelAlign1, SobelAlign1);

		cv::threshold(SobelCurrent1, SobelCurrent2, 3, 255, THRESH_BINARY);
		cv::threshold(SobelAlign1, SobelAlign2, 3, 255, THRESH_BINARY);


		// CPU
		cv::matchTemplate(SobelCurrent2, SobelAlign2, matTemplateResult, TM_SQDIFF);

		// 가장 낮은 값의 위치 찾음
		// 위치 잘 못 찾는 경우가 생길 수 있음 예외처리 필요
		cv::Point cpMin;
		double dMin = 0;
		cv::minMaxLoc(matTemplateResult, &dMin, NULL, &cpMin, NULL);

		// AligMark를 중심으로 데이터 다시 취득
		cv::Rect rectAlignROI(rectROI.x + cpMin.x, rectROI.y + cpMin.y, stParam->vecmatAlignMark[0].cols, stParam->vecmatAlignMark[0].rows);

		float fThreshold = 2.0e+9;
		if (dMin > fThreshold)
		{
			if (stParam->bSaveImage)
			{
				if (stParam->bImageDirection)
				{
					stParam->csMarkScan.Lock();
					CString strSave;
					CString strName;
					strName.Format(_T("\\x%d y%d m%d ImageX%d ImageY%d AlignMark_Find Fail.bmp"), x, stParam->cpStartPoint.y + 1, 0, rectAlignROI.x, rectAlignROI.y);
					strSave = stParam->strFolderPathMark + strName;
					cv::imwrite((string)CT2CA(strSave), matAlignROI);
					stParam->csMarkScan.Unlock();
				}
				else
				{
					stParam->csMarkScan.Lock();
					CString strSave;
					CString strName;
					strName.Format(_T("\\x%d y%d m%d ImageX%d ImageY%d AlignMark_Find Fail.bmp"), x, stParam->cpStartPoint.y - 1, 0, rectAlignROI.x, rectAlignROI.y);
					strSave = stParam->strFolderPathMark + strName;
					cv::imwrite((string)CT2CA(strSave), matAlignROI);
					stParam->csMarkScan.Unlock();
				}
				
			}
			break;
		}
		
		if (stParam->bSaveImage)
		{
			if (stParam->bImageDirection)
			{
				stParam->csMarkScan.Lock();
				CString strSave;
				CString strName;
				strName.Format(_T("\\x%d y%d m%d ImageX%d ImageY%d AlignMark_Find.bmp"), x, stParam->cpStartPoint.y + 1, 0, rectAlignROI.x, rectAlignROI.y);
				strSave = stParam->strFolderPathMark + strName;
				cv::imwrite((string)CT2CA(strSave), matAlignROI);
				stParam->csMarkScan.Unlock();
			}
			else
			{
				stParam->csMarkScan.Lock();
				CString strSave;
				CString strName;
				strName.Format(_T("\\x%d y%d m%d ImageX%d ImageY%d AlignMark_Find.bmp"), x, stParam->cpStartPoint.y - 1, 0, rectAlignROI.x, rectAlignROI.y);
				strSave = stParam->strFolderPathMark + strName;
				cv::imwrite((string)CT2CA(strSave), matAlignROI);
				stParam->csMarkScan.Unlock();
			}
		}
		if (stParam->bImageDirection)
		{
			stParam->structMapDie[stParam->cpStartPoint.y + 1][x].vecPoint.push_back(cv::Point(rectAlignROI.x, rectAlignROI.y));
			stParam->structMapDie[stParam->cpStartPoint.y + 1][x].vecRect.push_back(rectAlignROI);
		}
		else
		{
			stParam->structMapDie[stParam->cpStartPoint.y - 1][x].vecPoint.push_back(cv::Point(rectAlignROI.x, rectAlignROI.y));
			stParam->structMapDie[stParam->cpStartPoint.y - 1][x].vecRect.push_back(rectAlignROI);
		}
		
	}
	if (stParam->bImageDirection)
	{
		stParam->cpStartPoint.y++;
	}
	else {
		stParam->cpStartPoint.y--;
	}
	
	//MapAlignMarkMatching2(vecRect, vecPoint, vecInsideDieAlignMarkDistance, nDistanceOffset, cpCountLine, g_vecmatAlign);
	MapAlignMarkMatching3(stParam);

	// 못찾은 위치가 존재 할때

	//if (vecPoint.size() != nCamScanDie)
	//{
	//	for (int i = 0; i < nCamScanDie; i++)
	//	{
	//		if (pDieArr[cpStartPoint.y][i].bFoundMark == false)
	//		{
	//			// 두번째 Align Mark를 찾아 위치를 등록해야함
	//			// 찾은 위치 정밀 AlignMark 위치 확인까지 내부에서 진행해야함..
	//			// FindAligMarkSecond();
	//			// 
	//			// 이래도 등록이 안된다면.. 다른 방법 찾아야함..
	//		}
	//	}
	//}

	return 0;
}

void BumpInspection::StartThreadScanDie(stInspectThreadParameter* stParameter)
{
	if (m_pThreadScanDie == NULL)
	{
		m_pThreadScanDie = AfxBeginThread(threadScanDie, stParameter);

		if (m_pThreadScanDie == NULL)
		{
			CString str;
			str.Format(_T("Error %s"), __func__);
			AfxMessageBox(str);
		}
		m_pThreadScanDie->m_bAutoDelete = FALSE;
		m_eThreadWorkDie = E_THREAD_RUNNING;
	}
	else {
		if (m_eThreadWorkDie == E_THREAD_PAUSE)
		{
			m_pThreadScanDie->ResumeThread();
			m_eThreadWorkDie = E_THREAD_RUNNING;
		}
	}
}

void BumpInspection::PauseThreadScanDie()
{
	if (m_pThreadScanDie == NULL)
	{
		AfxMessageBox(L"Thread Not start");
	}
	else {
		m_pThreadScanDie->SuspendThread();
		m_eThreadWorkDie = E_THREAD_PAUSE;
	}
}

void BumpInspection::StopThreadScanDie()
{
	if (m_pThreadScanDie == NULL)
	{
		AfxMessageBox(L"Thread Not start");
	}
	else {
		m_pThreadScanDie->SuspendThread();

		DWORD dwResult;
		::GetExitCodeThread(m_pThreadScanDie->m_hThread, &dwResult);

		delete m_pThreadScanDie;
		m_pThreadScanDie = NULL;

		m_eThreadWorkDie = E_THREAD_STOP;
	}
}

UINT BumpInspection::threadScanDie(LPVOID _method)
{
	stInspectThreadParameter* stParam = (stInspectThreadParameter*)_method;

	bool bFlag = true;

	// Die 검사시 현재 위치를 중심으로 필요한 개수 앞뒤로
	int nScan = stParam->nInspectionScanDie / 2; // 현재 3/2 이므로 1
	int nCheckSizeX = stParam->nCameraScanPositionX + stParam->nCamScanDie;
	int nCheckStartX = stParam->nCameraScanPositionX;
	int nMapHeight = stParam->nMapHeight;
	int nMapWidth = stParam->nMapWidth;

	int nCountInspection = 0;


	while (bFlag)
	{
		for (int x = nCheckStartX; x < nCheckSizeX; x++)
		{
			int nOffsetX = 0;
			// x 예외처리.
			// 처음 시작점이 3x3 기준 0보다 작으면,
			if (x - nScan < nCheckStartX) {
				nOffsetX = x + nScan;
			}
			else if (x + nScan > nCheckSizeX - 1) { // 마지막 지점
				nOffsetX = x - nScan;
			}
			else { // 처음도 아니고 마지막 지점도 아닌경우,
				// 그대로 두기...
				nOffsetX = x;
			}

			for (int y = 0; y < nMapHeight; y++)
			{
				int nOffsetY = 0;
				bool bInspectFlag = false;
				// y 예외처리
				// 처음부분과
				if (y - nScan < 0)
				{
					nOffsetY = y + nScan;
				}
				// 마지막 부분 버퍼
				else if (y + nScan > nMapHeight - 1)
				{
					nOffsetY = -nScan;
				}
				// 현재 위치가 0보다 큰지 확인
				else
				{
					// 시작 위치 확인
					// 이전 Die가 검사하지 않는 부분이고 시작되는 위치라면 
					if (stParam->structMapDie[y - 1][x].nDie_Class != GOOD_DIE && 
						stParam->structMapDie[y][x].nDie_Class == GOOD_DIE)
					{
						nOffsetY = nScan;
					}
					else if (stParam->structMapDie[y + 1][x].nDie_Class != GOOD_DIE && 
						stParam->structMapDie[y][x].nDie_Class == GOOD_DIE)
					{
						nOffsetY = -nScan;
					}
				}
				vector<cv::Point> vecPoint;
				// 입력한 값의 만큼 영상을 찾아서 검사로 넘겨줌
				if (stParam->structMapDie[y][x].nDie_Class == GOOD_DIE &&
					stParam->structMapDie[y][x].bFoundMark == true)
				{
					// 비교할 대상이 몇 개 인지 확인하는 Count 변수
					int nInspectCnt = 0;
					for (int nDieY = -nScan + nOffsetY; nDieY < nScan + nOffsetY; nDieY++) {
						for (int nDieX = nOffsetX - nScan; nDieX < nOffsetX + nScan + 1; nDieX++) {
							if (stParam->structMapDie[y + nDieY][nDieX].nDie_Class == GOOD_DIE &&
								stParam->structMapDie[y + nDieY][nDieX].bFoundMark == true) {
								// 조건 성립 넘어감
								vecPoint.push_back(cv::Point(nDieX, y + nDieY));
								nInspectCnt++;
								continue;
							}
							else {
								bInspectFlag = false;
							}
						}
					}
					// 일정 조건 개수 이상 검색된거면 PASS
					if (nInspectCnt > stParam->nInspectionScanDie) {
						bInspectFlag = true;
						nInspectCnt = 0;
					}						
					else nInspectCnt = 0;

					// scan 실패로 입력 다시 검사하게
					if (!bInspectFlag)
					{
						vecPoint.clear();
						std::vector<Point>().swap(vecPoint);
					}
					else {
						// 성공 (일정 조건 개수 이상 검색됨.)
						// 검사 시작
						// 
						if (stParam->structMapDie[y][x].vecInspectionPosition.size() == 0)
						{
							for (int i = 0; i < vecPoint.size(); i++)
							{
								stParam->structMapDie[y][x].vecInspectionPosition.push_back(vecPoint[i]);
							}
							nCountInspection++;

							vecPoint.clear();
							std::vector<Point>().swap(vecPoint);
						}
					}
				}
			}
		}
		if (stParam->nCountGoodDie == nCountInspection)
		{
			bFlag = false;
		}
		
	}

	return 0;
}


void BumpInspection::StartThreadInspectionBump(stInspectThreadParameter* stParameter)
{
	for (int i = 0; i < MAX_THREAD_COUNT; i++)
	{

		if (m_pThreadScanInspection == NULL)
		{
			m_pThreadScanInspection = AfxBeginThread(threadInspectionBump, (LPVOID*)stParameter);
			// 혹시 쓸일이 있을라나..
			//m_nThreadIndex = m_pThreadScanInspection->m_nThreadID;
			if (m_pThreadScanInspection == NULL)
			{
				CString str;
				str.Format(_T("Error %s"), __func__);
				AfxMessageBox(str);
			}
			else {
				m_pThreadScanInspection->m_bAutoDelete = FALSE;
				m_eThreadWorkInspection = E_THREAD_RUNNING;
			}
			// 비어있는 것중에 하나만 돌리고 나감
			break;

		}
		else {
			if (m_eThreadWorkInspection == E_THREAD_PAUSE)
			{
				m_pThreadScanInspection->ResumeThread();
				m_eThreadWorkInspection = E_THREAD_RUNNING;
			}
		}
	}

}

void BumpInspection::PauseThreadInspectionBump()
{
	if (m_pThreadScanInspection == NULL)
	{
		AfxMessageBox(L"Thread Not start");
	}
	else {
		m_pThreadScanInspection->SuspendThread();
		m_eThreadWorkInspection = E_THREAD_PAUSE;
	}
}

void BumpInspection::StopThreadInspectionBump()
{
	if (m_pThreadScanInspection == NULL)
	{
		AfxMessageBox(L"Thread Not start");
	}
	else {
		m_pThreadScanInspection->SuspendThread();

		DWORD dwResult;
		::GetExitCodeThread(m_pThreadScanInspection->m_hThread, &dwResult);

		delete m_pThreadScanInspection;
		m_pThreadScanInspection = NULL;

		m_eThreadWorkInspection = E_THREAD_STOP;
	}
}

UINT BumpInspection::threadInspectionBump(LPVOID _method)
{
	stInspectThreadParameter* stParam = (stInspectThreadParameter*)_method;
	BumpInspection classBump;

	int nMapHeight = stParam->nMapHeight;
	int nMapWidth = stParam->nMapWidth;
	bool bFlag = true;
	int nCountInspection = 0;
	
	while (bFlag)
	{
#pragma omp parallel for schedule(dynamic)
		for (int y = 0; y < nMapHeight; y++)
		{
			for (int x = 0; x < nMapWidth; x++)
			{
				if (stParam->structMapDie[y][x].vecInspectionPosition.size() > stParam->nInspectionScanDie &&// nInspectionScanDie &&
					stParam->structMapDie[y][x].nDie_Class == GOOD_DIE &&
					stParam->structMapDie[y][x].bFoundMark)
				{
					if (stParam->structMapDie[y][x].bInspectionEnd)
					{
						continue;
					}

					int nWidth = 0;
					int nHeight = 0;

					for (int i = 0; i < stParam->structMapDie[y][x].vecInspectionPosition.size(); i++)
					{
						int nDiePosX = stParam->structMapDie[y][x].vecInspectionPosition[i].x;
						int nDiePosY = stParam->structMapDie[y][x].vecInspectionPosition[i].y;
						if (stParam->structMapDie[nDiePosY][nDiePosX].rectDie.width > nWidth)
						{
							nWidth = stParam->structMapDie[nDiePosY][nDiePosX].rectDie.width;
						}
						if (stParam->structMapDie[nDiePosY][nDiePosX].rectDie.height > nHeight)
						{
							nHeight = stParam->structMapDie[nDiePosY][nDiePosX].rectDie.height;
						}
					}

					vector<cv::Mat> matData;
					cv::Mat matInspectImage = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_8U);
					cv::Rect rectDie(0, 0, stParam->structMapDie[y][x].rectDie.width, stParam->structMapDie[y][x].rectDie.height);
					stParam->structMapDie[y][x].matDie.copyTo(matInspectImage(rectDie));

					for (int i = 0; i < stParam->structMapDie[y][x].vecInspectionPosition.size(); i++)
					{
						// 크기를 맞추기 위하여 버퍼를 생성 후 데이터를 넣음
						cv::Mat matTemp = cv::Mat::zeros(cv::Size(nWidth, nHeight), CV_8U);
						matData.push_back(matTemp);

						int nDiePosX = stParam->structMapDie[y][x].vecInspectionPosition[i].x;
						int nDiePosY = stParam->structMapDie[y][x].vecInspectionPosition[i].y;

						cv::Rect rectROI(0, 0, stParam->structMapDie[nDiePosY][nDiePosX].rectDie.width, stParam->structMapDie[nDiePosY][nDiePosX].rectDie.height);
						stParam->structMapDie[nDiePosY][nDiePosX].matDie.copyTo(matData[i](rectROI));
						
					}
					
					cv::Mat matStandard;
					cv::Mat matResult;
					classBump.DieTrimmedMean(matData, matStandard);

					if (classBump.Inspection(stParam, matInspectImage, matStandard, matResult) == 0)
					{
						stParam->structMapDie[y][x].bInspectionEnd = true;
						stParam->csInspection.Lock();
						nCountInspection++;
						stParam->csInspection.Unlock();
					}

					if (stParam->bSaveImage)
					{
						stParam->csInspection.Lock();
						CString strSave;
						CString strName;
						strName.Format(_T("\\x%d y%d InspectResult .bmp"), x, y);
						strSave = stParam->strFolderPathInspection + strName;
						cv::imwrite((string)CT2CA(strSave), matResult);
						strName.Format(_T("\\x%d y%d InspectReference.bmp"),  x, y);
						strSave = stParam->strFolderPathInspection + strName;
						cv::imwrite((string)CT2CA(strSave), matStandard);
						strName.Format(_T("\\x%d y%d InspectSrc.bmp"),  x, y);
						strSave = stParam->strFolderPathInspection + strName;
						cv::imwrite((string)CT2CA(strSave), matInspectImage);
						stParam->csInspection.Unlock();
					}

					matResult.release();
					matStandard.release();
					matInspectImage.release();

					for (int i = 0; i < matData.size(); i++)
					{
						matData[i].release();
					}
					matData.clear();
					vector<cv::Mat>().swap(matData);


				}
			}
		}
		if (nCountInspection == stParam->nCountGoodDie)
		{
			stParam->csInspection.Lock();
			bFlag = false;
			stParam->csInspection.Unlock();
		}
	}




	return 0;
}

int BumpInspection::DieTrimmedMean(vector<cv::Mat> vecmatSample, cv::Mat& matResult)
{
	int nSampleSize = vecmatSample.size();

	// 들어오는 샘플의 크기가 전부 같아야 함
	size_t nImageSize = vecmatSample[0].cols * vecmatSample[0].rows;

	matResult = cv::Mat::zeros(vecmatSample[0].size(), vecmatSample[0].type());
	cv::Mat matSum = cv::Mat::zeros(vecmatSample[0].size(), CV_32FC1);
	uchar* ucpResultData = (uchar*)matResult.data;
	float* fDataSum = (float*)matSum.data;
	// 병렬 필수?
#pragma omp parallel for schedule(dynamic)
	for (int j = 0; j < nImageSize; j++)
	{
		// 합
		for (int i = 0; i < nSampleSize; i++)
		{
			uchar* ucpData = vecmatSample[i].data;
			fDataSum[j] += ucpData[j];
		}
		// 평균
		float fAvg = fDataSum[j] / (float)nSampleSize;

		// Median 을 구하는데, Cnt해서 평균 위 아래 다수 쪽의 데이터로 새로 평균을 내어보자.
		int nCntUpper = 0;
		int nCntLower = 0;
		float fNewSumValue = 0.0f;
		// 3장 중 평균보다 큰게 많은지 작은게 많은지 Count.
		for (int i = 0; i < nSampleSize; i++)
		{
			uchar* ucpData = vecmatSample[i].data;
			if (ucpData[j] >= fAvg) nCntUpper++;
			else nCntLower++;
		}
		fDataSum[j] = 0.0;
		// 평균보다 높은게 더 많은 경우,
		if (nCntUpper > nCntLower) {
			for (int i = 0; i < nSampleSize; i++)
			{
				uchar* ucpData = vecmatSample[i].data;
				if (ucpData[j] >= fAvg) {
					fDataSum[j] += ucpData[j];
				}
			}
			fAvg = fDataSum[j] / nCntUpper;
		}
		else if (nCntUpper == nCntLower) { // 평균보다 높은Cnt와 낮은 Cnt가 같은경우!!
			// 평균값 입력
			uchar ucData = (uchar)roundf(fAvg);
			ucpResultData[j] = ucData;
			continue; //아무것도 안함. Avg=avg 로 입력.
		}
		else { // 평균보다 낮은게 더 많은 경우.
			for (int i = 0; i < nSampleSize; i++)
			{
				uchar* ucpData = vecmatSample[i].data;
				if (ucpData[j] < fAvg) {
					fDataSum[j] += ucpData[j];
				}
			}
			fAvg = fDataSum[j] / nCntLower;
		}

		// 평균값 입력
		uchar ucData = (uchar)roundf(fAvg);
		ucpResultData[j] = ucData;
	}
	return 0;
}

int BumpInspection::Inspection(stInspectThreadParameter* stParam, cv::Mat Die1, cv::Mat Die2, cv::Mat& Result)
{
	// 같은 영상 크기 확인
	if (Die1.cols != Die2.cols || Die1.rows != Die2.rows)
		return 1;

	Result = cv::Mat::zeros(Die1.size(), Die1.type());

	// 같은 길이임으로 하나로 설정 
	int nWidth = Die1.cols;
	int nHeight = Die1.rows;

	// 외각처리 
	int nSearchY = stParam->nDiffSearchY;
	int nSearchX = stParam->nDiffSearchX;
	int nStartY = nSearchY;
	int nStartX = nSearchX;
	int nEndY = nHeight - 1 + nSearchY;
	int nEndX = nWidth - 1 + nSearchX;

	// 경계지역 확장
	cv::Mat matBorderSrc1;
	cv::Mat matBorderSrc2;
	cv::copyMakeBorder(Die1, matBorderSrc1, nSearchY, nSearchX, nSearchX, nSearchY, BORDER_REFLECT);
	cv::copyMakeBorder(Die2, matBorderSrc2, nSearchY, nSearchX, nSearchX, nSearchY, BORDER_REFLECT);

	cv::GaussianBlur(matBorderSrc1, matBorderSrc1, cv::Size(3, 3), 1.4);
	cv::GaussianBlur(matBorderSrc2, matBorderSrc2, cv::Size(3, 3), 1.4);

	// 데이터 접근 Pointer 넘김
	uchar* upData1 = matBorderSrc1.ptr(0);
	uchar* upData2 = matBorderSrc2.ptr(0);
	uchar* upResult = Result.data;

	int nSrcStep = matBorderSrc1.step;
	int nDstStep = Result.step;

	// SIMD 명령어로 진행? 확인 필요 
	// 각영역 확인 
	//#if defined(_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic)
	//#endif
	for (int y = nStartY; y < nEndY; y++)
	{
		for (int x = nStartX; x < nEndX; x++)
		{
			// 최소값 확인용
			int nMinDiffValue = 256;
			int nCurrentDiff = 0;
			// 차이값 위치 계산
			int nStartM = y - nSearchY;
			int nEndM = y + nSearchY;
			// 방향값 위치 계산
			int nDirection = 0;

			for (int m = nStartM; m < nEndM; m++)
			{
				int nStartN = x - nSearchX;
				int nEndN = x + nSearchX;
				for (int n = nStartN; n < nEndN; n++)
				{
					nCurrentDiff = abs(upData1[m * nSrcStep + n] - upData2[m * nSrcStep + n]);
					if (nMinDiffValue > nCurrentDiff)
					{
						nMinDiffValue = nCurrentDiff;
					}
					nDirection++;
				}
			}
			upResult[(y - nSearchY) * nDstStep + (x - nSearchX)] = nMinDiffValue;
		}
	}

	return E_ERROR_CODE_TRUE;

}

structDieArr ** BumpInspection::CreateWaferDieMap(int nType, float fDieWidth_mm, float fDieHeight_mm, float fWaferDiameter_inch1, float fWaferDiameter_mm, float fEdgeLoss_mm, int nLossShift, float fOffsetHeight_mm, float fOffsetWidth_mm, int nOffset)
{
	float fDieWidth = fDieWidth_mm * MAGNITUDE;
	float fDieHeight = fDieHeight_mm * MAGNITUDE; // 7400um -> 7.4mm
	float fWaferDiameter_inch = fWaferDiameter_inch1;
	float fWaferDiameter = fWaferDiameter_mm * MAGNITUDE;// fWaferDiameter_inch * INCH2MM * CAL10;
	float fEdgeLoss = fEdgeLoss_mm * MAGNITUDE;
	float fOffsetHeight = fOffsetHeight_mm;
	float fOffsetWidth = fOffsetWidth_mm;
	int nWidthCnt = 0;
	int nHeightCnt = 0;
	// Init members
	cv::Mat ResultImg1, ResultImg2;
	int nWidth, nHeight;
	int nCntG = 0, nCntP = 0, nCntC = 0, nCntW = 0, nCntTotal = 0;
	structDieArr** DieArr = NULL;

	switch (nType)
	{
	case E_CENTER_DIE:
	{
		////////////////////////////////////////////////////////////
		// Die Center Standard Start
		///////////////////////////////////////////////////////////
		nWidthCnt = (
			((double)((((fWaferDiameter / 2.0)) - ((fDieWidth / 2.0))) / (fDieWidth))
				+ 0.5)
			);
		nWidthCnt *= 2;
		nWidthCnt++;
		nHeightCnt = (
			((double)((((fWaferDiameter / 2.0)) - ((fDieHeight / 2.0))) / (fDieHeight))
				+ 0.5)
			);
		nHeightCnt *= 2;
		nHeightCnt++;
		nWidth = nWidthCnt * (fDieWidth);
		nHeight = nHeightCnt * (fDieHeight);
		nCntTotal = nWidthCnt * nHeightCnt;

		DieArr = new structDieArr*[nHeightCnt];
		for (int y = 0; y < nHeightCnt; y++)
		{
			DieArr[y] = new structDieArr[nWidthCnt];
			for (int x = 0; x < nWidthCnt; x++) {
				DieArr[y][x].fX = 0.0f;
				DieArr[y][x].fY = 0.0f;
				DieArr[y][x].fCenterX = 0.0f;
				DieArr[y][x].fCenterY = 0.0f;
				DieArr[y][x].nDie_Class = DEFAULT_DIE;
				DieArr[y][x].nMapWidth = nWidthCnt;
				DieArr[y][x].nMapHeight = nHeightCnt;
			}
		}
		int nAlignMarkType = 1;
		switch (nAlignMarkType)
		{
		case E_CENTER_CROSS:
		{
			for (int y = 0; y < nHeightCnt; y++) {
				for (int x = 0; x < nWidthCnt; x++) {
					cv::Rect2f rcDie(x*(fDieWidth)
						, y*(fDieHeight)
						, fDieWidth
						, fDieHeight);
					if (nLossShift == 0)
					{
						// Wafer 내부에 있다면,
						if (IsRectInCircle(nWidth / 2.0 + fOffsetWidth, nHeight / 2.0 + fOffsetHeight, ((fWaferDiameter / 2.0) - fEdgeLoss), rcDie) == 0) {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = GOOD_DIE;
							nCntG++;
						}
						else if (IsRectInCircle(nWidth / 2.0 + fOffsetWidth, nHeight / 2.0 + fOffsetHeight, fWaferDiameter / 2.0, rcDie) == 5) {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = WASTED_DIE;
							nCntW++;
						}
						else if (IsRectInCircle(nWidth / 2.0 + fOffsetWidth, nHeight / 2.0 + fOffsetHeight, ((fWaferDiameter / 2.0) - fEdgeLoss), rcDie, nOffset) < 3) {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = PARTIAL_DIE;
							nCntP++;
						}
						else {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = CUTTING_DIE;
							nCntC++;
						}
					}
					if (nLossShift == 1)
					{
						// Wafer 내부에 있다면,
						if (IsRectInCircle(nWidth / 2.0 + fOffsetWidth, nHeight / 2.0 + fOffsetHeight + fEdgeLoss, ((fWaferDiameter / 2.0) - fEdgeLoss), rcDie) == 0) {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = GOOD_DIE;
							nCntG++;
						}
						else if (IsRectInCircle(nWidth / 2.0 + fOffsetWidth, nHeight / 2.0 + fOffsetHeight, fWaferDiameter / 2.0, rcDie) == 5) {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = WASTED_DIE;
							nCntW++;
						}
						else if (IsRectInCircle(nWidth / 2.0 + fOffsetWidth, nHeight / 2.0 + fOffsetHeight + fEdgeLoss, ((fWaferDiameter / 2.0) - fEdgeLoss), rcDie, 100) < 3) {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = PARTIAL_DIE;
							nCntP++;
						}
						else {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = CUTTING_DIE;
							nCntC++;
						}
					}

				}
			}
			break;
		}
		case E_NOMAL_VERTEXT:
		{
			// Fill Rect
			for (int y = 0; y < nHeightCnt; y++) {
				for (int x = 0; x < nWidthCnt; x++) {
					cv::Rect2f rcDie(x*(fDieWidth)
						, y*(fDieHeight)
						, fDieWidth
						, fDieHeight);

					if (nLossShift == 0)
					{
						// Wafer 내부에 있다면,
						if (IsRectInCircle(nWidth / 2.0 + fOffsetWidth, nHeight / 2.0 + fOffsetHeight, ((fWaferDiameter / 2.0) - fEdgeLoss), rcDie) == 0) {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = GOOD_DIE;
							nCntG++;
						}
						else if (IsRectInCircle(nWidth / 2.0 + fOffsetWidth, nHeight / 2.0 + fOffsetHeight, fWaferDiameter / 2.0, rcDie) == 5) {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = WASTED_DIE;
							nCntW++;
						}
						else {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = CUTTING_DIE;
							nCntC++;
						}
					}

					if (nLossShift == 1)
					{
						// Wafer 내부에 있다면,
						if (IsRectInCircle(nWidth / 2.0 + fOffsetWidth, nHeight / 2.0 + fOffsetHeight + fEdgeLoss, ((fWaferDiameter / 2.0) - fEdgeLoss), rcDie) == 0) {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = GOOD_DIE;
							nCntG++;
						}
						else if (IsRectInCircle(nWidth / 2.0 + fOffsetWidth, nHeight / 2.0 + fOffsetHeight, fWaferDiameter / 2.0, rcDie) == 5) {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = WASTED_DIE;
							nCntW++;
						}
						else {
							DieArr[y][x].fX = rcDie.x;
							DieArr[y][x].fY = rcDie.y;
							DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
							DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
							DieArr[y][x].nDie_Class = CUTTING_DIE;
							nCntC++;
						}
					}
				}
			}
			break;
		}
		}

		break;
	}
	case E_CENTER_WAFER:
	{
		nWidthCnt = ((fWaferDiameter / 2.0) / fDieWidth) + 1;
		nWidthCnt *= 2;
		nHeightCnt = ((fWaferDiameter / 2.0) / fDieHeight) + 1;
		nHeightCnt *= 2;
		nWidth = nWidthCnt * (fDieWidth);
		nHeight = nHeightCnt * (fDieHeight);
		nCntTotal = nWidthCnt * nHeightCnt;

		DieArr = new structDieArr*[nHeightCnt];
		for (int y = 0; y < nHeightCnt; y++)
		{
			DieArr[y] = new structDieArr[nWidthCnt];
			for (int x = 0; x < nWidthCnt; x++) {
				DieArr[y][x].fX = 0.0f;
				DieArr[y][x].fY = 0.0f;
				DieArr[y][x].fCenterX = 0.0f;
				DieArr[y][x].fCenterY = 0.0f;
				DieArr[y][x].nDie_Class = DEFAULT_DIE;
			}
		}



		// Fill Rect
		//parallel_for(0, nHeightCnt, [&](int y) {
		for (int y = 0; y < nHeightCnt; y++) {
			for (int x = 0; x < nWidthCnt; x++) {
				cv::Rect2f rcDie(x*(fDieWidth)
					, y*(fDieHeight)
					, fDieWidth
					, fDieHeight);
				// Wafer 내부에 있다면,
				if (IsRectInCircle(nWidth / 2.0, nHeight / 2.0, ((fWaferDiameter / 2.0) - fEdgeLoss), rcDie) == 0) {
					DieArr[y][x].fX = rcDie.x;
					DieArr[y][x].fY = rcDie.y;
					DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
					DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
					DieArr[y][x].nDie_Class = GOOD_DIE;
					nCntG++;
				}
				else if (IsRectInCircle(nWidth / 2.0, nHeight / 2.0, fWaferDiameter / 2.0, rcDie) == 5) {
					DieArr[y][x].fX = rcDie.x;
					DieArr[y][x].fY = rcDie.y;
					DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
					DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
					DieArr[y][x].nDie_Class = WASTED_DIE;
					nCntW++;
				}
				else if (IsRectInCircle(nWidth / 2.0, nHeight / 2.0, ((fWaferDiameter / 2.0) - fEdgeLoss), rcDie, 100) < 3) {
					DieArr[y][x].fX = rcDie.x;
					DieArr[y][x].fY = rcDie.y;
					DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
					DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
					DieArr[y][x].nDie_Class = PARTIAL_DIE;
					nCntP++;
				}
				else {
					DieArr[y][x].fX = rcDie.x;
					DieArr[y][x].fY = rcDie.y;
					DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
					DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
					DieArr[y][x].nDie_Class = CUTTING_DIE;
					nCntC++;
				}
			}

		}

	default:
		break;
	}
	}
	return DieArr;
}

structDieArr ** BumpInspection::CreateWaferDieMap(CString strPath, float fDieWidth_mm, float fDieHeight_mm)
{
	float fDieWidth = fDieWidth_mm * MAGNITUDE;
	float fDieHeight = fDieHeight_mm * MAGNITUDE; // 7400um -> 7.4mm
	
	// Init members
	cv::Mat ResultImg1;
	int nWidth = 0, nHeight = 0;
	int nYCnt = 0, nXCnt = 0;
	structDieArr** DieArr = NULL;
	vector<int> vecWaferMap;

	// 파일 읽어오기.
	CString strMapPath = strPath;
	std::ifstream file(strMapPath);

	if (true == file.is_open()) {
		std::string strValue;
		// 우선 몇줄 있는지 확인.
		while (!file.eof()) {
			std::getline(file, strValue);
			if (!strValue.find("[CENTER]"))	break;
			else nYCnt++;
		}
		file.clear();
		file.seekg(0, std::ios_base::beg); //파일 처음으로 이동
										   // 총 몇 개의 데이터가 있는지 확인.
		while (!file.eof()) {
			std::getline(file, strValue, ',');	// 콤마 기준으로 한줄씩 데이터 뜯어내서 vector에 저장.
			if (!strValue.find("[CENTER]")) break;
			else vecWaferMap.push_back(stoi(strValue));
		}
		nXCnt = vecWaferMap.size();
		nXCnt /= nYCnt;	// X Arr Cap 계산
		file.close();
	}
	else {
		// AfxPrintInfo(_T("file open fail"));
	}
	
	////////////////////////////////////////////////////////////
	// Wafer Die Count
	///////////////////////////////////////////////////////////
	int nWidthCnt = nXCnt;
	int nHeightCnt = nYCnt;

	nWidth = nWidthCnt * fDieWidth;
	nHeight = nHeightCnt * fDieHeight;

	DieArr = new structDieArr*[nHeightCnt];
	for (int y = 0; y < nHeightCnt; y++)
	{
		DieArr[y] = new structDieArr[nWidthCnt];
		for (int x = 0; x < nWidthCnt; x++) {
			// Die Rect 좌표 설정
			cv::Rect2f rcDie(x*(fDieWidth)
				, y*(fDieHeight)
				, fDieWidth
				, fDieHeight);
			// 좌표를 이전 Vector에 저장한 Good Die와 함께 모두 Structure 에 저장.
			DieArr[y][x].fX = rcDie.x;
			DieArr[y][x].fY = rcDie.y;
			DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
			DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
			if (vecWaferMap[x + (y*nWidthCnt)] >= 1) { // Center 표시 2로 해두었음;;
				DieArr[y][x].nDie_Class = GOOD_DIE;
			}
			else {
				DieArr[y][x].nDie_Class = WASTED_DIE;
			}
			DieArr[y][x].nMapWidth = nWidthCnt;
			DieArr[y][x].nMapHeight = nHeightCnt;
		}
	}
	return DieArr;
}

int BumpInspection::IsRectInCircle(float CircleCenterX, float CircleCenterY, float CircleRadius, cv::Rect IsRect, int nOffSet)
{
	float pointX = IsRect.x;
	float pointY = IsRect.y;
	int nInCircle = 0;

	float deltaY = 0.0f;
	float deltaX = 0.0f;
	float length = 0.0f;

	// if (Math.pow(R, 2) > (Math.pow(X - TX, 2) + Math.pow(Y - TY, 2))) { 포함 = true; }
	// ■□
	// □□
	deltaY = CircleCenterY - IsRect.y;
	deltaX = CircleCenterX - IsRect.x;
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nInCircle += 1;
	}

	// □■
	// □□
	deltaY = CircleCenterY - IsRect.y;
	deltaX = CircleCenterX - (IsRect.x + IsRect.width);
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nInCircle += 1;
	}

	// □□
	// ■□
	deltaY = CircleCenterY - (IsRect.y + IsRect.height);
	deltaX = CircleCenterX - IsRect.x;
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nInCircle += 1;
	}

	// □□
	// □■
	deltaY = CircleCenterY - (IsRect.y + IsRect.height);
	deltaX = CircleCenterX - (IsRect.x + IsRect.width);
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nInCircle += 1;
	}

	// □  □
	//   ■
	// □  □
	int nCount = 0;

	deltaY = CircleCenterY - (IsRect.y + (IsRect.height / 2) - nOffSet);
	deltaX = CircleCenterX - (IsRect.x + (IsRect.width / 2) - nOffSet);
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nCount++;
	}

	deltaY = CircleCenterY - (IsRect.y + (IsRect.height / 2) - nOffSet);
	deltaX = CircleCenterX - (IsRect.x + (IsRect.width / 2) + nOffSet);
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nCount++;
	}

	deltaY = CircleCenterY - (IsRect.y + (IsRect.height / 2) + nOffSet);
	deltaX = CircleCenterX - (IsRect.x + (IsRect.width / 2) - nOffSet);
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nCount++;
	}

	deltaY = CircleCenterY - (IsRect.y + (IsRect.height / 2) + nOffSet);
	deltaX = CircleCenterX - (IsRect.x + (IsRect.width / 2) + nOffSet);
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nCount++;
	}


	if (nCount != 0)
	{
		nInCircle += 1;
	}
	
	return nInCircle;
}

void BumpInspection::setMapData(stInspectThreadParameter* stParam)
{
	int nCountGoodDie = 0;
	int nStartX = stParam->nImageNum * stParam->nCamScanDie;
	int nEndX = nStartX + stParam->nCamScanDie;

	int nMaxY = 0;
	int nMinY = stParam->nMapHeight - 1;
	for (int y = 0; y < stParam->nMapHeight; y++)
	{
		for (int x = nStartX; x < nEndX; x++)
		{
			if (stParam->structMapDie[y][x].nDie_Class == GOOD_DIE)
			{
				nCountGoodDie++;
				if (nMinY > y)
					nMinY = y;
				if (nMaxY < y)
					nMaxY = y;
			}
		}
	}

	stParam->nCountGoodDie = nCountGoodDie;
	stParam->nMapMaxY = nMaxY;
	stParam->nMapMinY = nMinY;
	stParam->nCameraScanPositionX = nStartX;
	if(stParam->bImageDirection)
		stParam->nCameraScanPositionY = nMinY;
	else
		stParam->nCameraScanPositionY = nMaxY;
	
	

}
