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

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// [Draw & Save] �ҷ� ǥ��
		cv::Scalar color;

		// Point & Line�� �߰��ϸ� �� ( Mura �ܰ��� �׸� )
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
		case E_DEFECT_JUDGEMENT_MURA_MULT_BP:	// Point �ҷ� Gray ���� �����ΰ�� ( ���� ��� )
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

		// 17.03.29 Mura & SVI �簢�� �׸��� �ʱ� ( Alg���� �ܰ��� ���� �׸� )
		// ���� �߶󳻴� �κ��� �ؾ� ��
		int nOffSet = 5, nThickness = 1;
		// 		// ���� Defect ����
		// 		int nDefStartX, nDefStartY, nDefWidth, nDefHeight;
		// 		
		// 		nDefStartX = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nOffSet;
		// 		nDefStartY = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y - nOffSet;
		// 		// ���밪 �ӽ� ��� (by CWH)
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


		//DC �ҷ� �̴ϸʿ� �ȱ׸�
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_DC_START	&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_DC_END)
		{
			// �̴ϸʿ� Rect �ȱ׸�
			resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
		}

		// PNZ 19.04.23 Mura Normal F Grade Defect Rect �� �׸�
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURANORMAL_TYPE2_F_GRADE ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURANORMAL_TYPE3_F_GRADE)
		{
			// �̴ϸʿ� Rect �ȱ׸�
			resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
		}

		// Mura - �̴ϸʿ� Rect �ȱ׸�
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_MURA_START	&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_MURA_END)
		{
			// �̴ϸʿ� Rect �ȱ׸�
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP)
				resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
		}
		// Mura Retest - �̴ϸʿ� Rect �ȱ׸�
		else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_MURA ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_MURA_BRIGHT)
		{
			// �̴ϸʿ� Rect �ȱ׸�
			resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
		}
		// APP - �̴ϸʿ� Rect �ȱ׸�
		else if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_APP_START		&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_APP_END)
		{
			// �̴ϸʿ� Rect �ȱ׸�
			resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
		}


		// ���� �̿��� �ҷ� / �簢�� �׸���
		if (resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect)
		{
			// �簢�� �׸���
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

// �ܰ� �κ� ó��
long BumpInspection::makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio)
{
	// ���� ó��
	if (dAlgPara == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	wchar_t wstrID[MAX_PATH] = { 0, };
	swprintf(wstrID, _T("%s"), LogicPara.tszPanelID);

	// ��� Ÿ��
	int nEqpType = theApp.m_Config.GetEqpType();

	// ���� �÷��� On�� ���
	if (dAlgPara[E_PARA_ROUND_SETTING] > 0 || dAlgPara[E_PARA_CHOLE_SETTING] > 0)
	{
		// �ڳ� ���
		nErrorCode = Align_SetFindContour_(LogicPara.MatOrgImage, theApp.GetRoundROI(nImageNum, nCameraNum), theApp.GetCHoleROI(nImageNum, nCameraNum),
			theApp.GetRndROICnt(nImageNum, nCameraNum), theApp.GetCHoleROICnt(nImageNum, nCameraNum), dAlgPara, nAlgImg, nCameraNum, nRatio, nEqpType, theApp.GetRoundPath());

		// ���������� ������ ���, ����
		// ���� �߻��� ���, ���� �ڵ� �ѱ��
		if (nErrorCode == E_ERROR_CODE_TRUE)
			nErrorCode = E_ERROR_CODE_ALIGN_ROUND_SETTING;
	}

	// ���� �÷��� Off�� ���
	else
	{
		// Alg �α׿�
		wchar_t strAlgLog[MAX_PATH] = { 0, };
		swprintf(strAlgLog, _T("ID:%s\tPat:%s"), LogicPara.tszPanelID, theApp.GetGrabStepName(nImageNum));

		// Dust ������ �ƴ� ���
		if (nAlgImg != E_IMAGE_CLASSIFY_AVI_DUST)
		{
			nErrorCode = Align_FindFillOutArea(LogicPara.MatOrgImage, LogicPara.MatBKG, theApp.m_pGrab_Step[nImageNum].tRoundSet, theApp.m_pGrab_Step[nImageNum].tCHoleSet, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nAlgImg], stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nAlgImg], stThrdAlignInfo.tCHoleAlignData->bCHoleAD[nAlgImg],
				dAlgPara, nAlgImg, nCameraNum, nRatio, nEqpType, strAlgLog, wstrID);
		}

		// Dust ������ ���
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

	// UI - �˻� ���� üũ
	// �˻� ������ ��츸 ����
	// Grab�� �� ��� ���� ���� ( �ҷ� ���� )
	if (bUseInspect)
	{
		// Point -> Line ����
		// ����, Line �˰��� ����ȭ ��, �Լ� ����
		//JudgementPointToLine(WrtResultInfo, resultPanelData, nImageW, nImageH);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge Point To Line End. Panel ID : %s"), strPanelID);

		// Report ����
		JudgementDelReport(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Del Report. Panel ID : %s"), strPanelID);

		//JudgementSpot(resultPanelData);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Spot. Panel ID : %s"), strPanelID);

		// PNZ:17.11.27 ���� �ߺ� �κ� ������ Line Merge
		// �Ķ���� �޾ƿ���
		double* dAlignPara = theApp.GetAlignParameter(0);

		////////////////////TIANMA PARAM ////////////////////
		//Parameter
		bool	bGroupAllorEach = dAlignPara[E_PARA_GROUP_TEXT] > 0 ? true : false;  // 0�̸� All Pattern, 1�̸� R, G, B ����
		/////////////////////////////////////////////////////

		// Delet Some Line Defect ����
		// ������ �ֺ� ����� ����
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

		// Crack ����
		JudgementCrack(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Crack Pattern Judge End. Panel ID : %s"), strPanelID);

		// PCD CRACK ���� ( ��˻� ���� 5.99" )
		JudgementPCDCrack(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - PCD Crack End. Panel ID : %s"), strPanelID);

		// 2018-07-31 Camera Tap�� ���� ���� ����
		JudgementCameraTapOverKill(resultPanelData, MatOrgImage, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Camera Tap OverKill Delet End. Panel ID : %s"), strPanelID);

		// DGS ����
		/*JudgementNewDGS(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - DGS End. Panel ID : %s"), strPanelID);*/

		//JudgementDGS_Vth(resultPanelData);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - DGS Vth End. Panel ID : %s"), strPanelID);

		// Weak Plan B Test
		//JudgementSpecialGrayLineDefect(resultPanelData);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Plan B End. Panel ID : %s"), strPanelID);

		// 2018-03-16 ver. 0.0.3 PNZ
		// ���� ���� �˰���
		DeleteOverlapDefect_DimpleDelet(resultPanelData, MatOrgImage, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Dimple Overkill Defect End. Panel ID : %s"), strPanelID);

		// 2018-04-19 ver. 0.0.3 PNZ
		// �ѵ��� Spot ���� �˰���
		DeleteOverlapDefect_SpecInSpotDelet(resultPanelData, MatOrgImage, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Spot Overkill Defect End. Panel ID : %s"), strPanelID);

		// 2018-05-17 ver. 0.0.1 PNZ
		// ���� Ȯ�� �˰���
		DeleteOverlapDefect_DustDelet(resultPanelData, MatOrgImage, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Dust Defect End. Panel ID : %s"), strPanelID);

		// 2018-04-19 ver. 0.0.1 PNZ
		// �ѵ��� Spot ���� �˰���
		DeleteOverlapDefect_BlackSmallDelet(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Black Pattern Small Point Defect End. Panel ID : %s"), strPanelID);

		//// LJH �׷��̸� ���̴� ���� �з� 
		PatternPointBrightClassify(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - PatternPointBrightClassify. Panel ID : %s"), strPanelID);

		// �ߺ� ��ǥ ����
		DeleteOverlapDefect(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Overlap Defect End. Panel ID : %s"), strPanelID);

		// Black Pattern ���� Merge ����
		JudgementBlackPatternMerge(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Black Pattern Merge End. Panel ID : %s"), strPanelID);

		// KSW 17.09.11 - ȥ�� ���� �߻� -> ���� ���� ��� ET ����
		// ���ο��� End �α� ����
		//JudgementET(resultPanelData, dAlignPara, strPanelID);

		// KSW 17.09.11 - ���� �κ� ���� -> Pad Bright ����
		//JudgementEdgePadBright(resultPanelData, dAlignPara);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Edge Pad Bright End. Panel ID : %s"), strPanelID);


		JudgeCHoleJudgment(resultPanelData, tCHoleAlignData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - CHole Judgement End. Panel ID : %s"), strPanelID);

		////// LJH ��Ƽ ����////////
		JudgeMultLine(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - JudgeMultLine End. Panel ID : %s"), strPanelID);

		JudgeRGBSp3Judgment(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Group Each Pattern End. Panel ID : %s"), strPanelID);

		//20201125 LJH BRIGHT_RGB ��ũ��ġ ���� X �Ǵ� Y ��ǥ�� �������� ������� ����
		JudgeBrightRGBJudgment(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Bright_RGB_SCRATCH End. Panel ID : %s"), strPanelID);


		int	bBlackOverkill = (int)dAlignPara[E_PARA_AVI_BLACK_OVERKILL_TEXT];

		if (bBlackOverkill > 1)
		{
			DeleteOverlapDefect_Point_Black_RGB(resultPanelData, MatOrgImage_Real, dAlignPara);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Point_Black_RGB End. Panel ID : %s"), strPanelID);
		}
		// ������ Panel Size - Resolution ���
		double dResolution = calcResolution(WrtResultInfo);

		// KSW 17.09.25 - ���� & ���� ����
		//JudgeGroup(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
		//JudgeGroupTEST(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);

		// All Pattern 
		if (bGroupAllorEach == 0)
		{
			JudgeGroupJudgment(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Group All Pattern End. Panel ID : %s"), strPanelID);
		}
		// Each Pattern(R, G, B ���� ī��Ʈ)
		else if (bGroupAllorEach == 1)
		{
			JudgeGroupJudgmentEach(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Group Each Pattern End. Panel ID : %s"), strPanelID);
		}


		// PNZ 19.04.15 Mura Normal �з�
		JudgementMuraNormalClassification(resultPanelData, dAlignPara);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - MuraNormal Classification End. Panel ID : %s"), strPanelID);

		// contact �̻�� G64 �ҷ� �ٹ� �˶� �߻� ��û �� �߰�
		if (JudgementContactAlram(resultPanelData, dAlignPara) == false)
		{
			theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eAutoRun, eALARMID_ALIGN_FAIL, eALARMTYPE_HEAVY);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Contact Check!. Panel ID : %s"), strPanelID);
		}

		//	������ 18.12.06
		//	MergeTool Falg �� ������� ������ ����
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

		// KSW 17.11.30 - AVI & SVI �ܰ� ���� ���� ���	
		wchar_t strContoursPath[MAX_PATH] = { 0 };
		CString strResultPath = RESULT_PATH;
		CString strMergeToolPath = MERGETOOL_PATH;
		wchar_t strContoursPath_Folder[MAX_PATH] = { 0 }; // JSO - Result ���� ����

		// Contours.Merge ���� ��� ���� 180322 YSS
		swprintf(strContoursPath, _T("%s\\%s\\Contours.Merge"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID);
		swprintf(strContoursPath_Folder, _T("%s\\%s\\"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID);

		// YDS - �ӽ� �α� �߰�
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Contours FilePath : %s"), strContoursPath);

		// KSW 17.11.29 - AVI & SVI �ܰ� ���� ���� ( AVI & SVI ���� �� )
		if (GetFileAttributes(strContoursPath_Folder) == -1)
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Can't find Folder : %s"), strContoursPath_Folder);
		else
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Found Folder : %s"), strContoursPath_Folder);

		if (GetFileAttributes(strContoursPath_Folder) == -1) // JSO - ���� �ѹ� �� ����
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
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("eInspMode = %d, MergeToolFlag = %d, Panel ID : %s"), eInspMode, theApp.GetMergeToolUse(), strPanelID); // JSO 0604 - ������ ���� ī��
		}
		CString strDest;
		strDest.Format(_T("%s\\%s\\"), MERGETOOL_PATH, strPanelID);

		if (GetFileAttributes(strDest) == -1) // JSO - ���� �ѹ� �� ����
		{
			SHCreateDirectoryEx(NULL, strDest, NULL);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Created Folder : %s"), strDest);
		}
		else
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Exist Folder : %s"), strDest);

		CString strContoursDest = strDest + _T("Contours.Merge");
		bool nRet = CopyFile(strContoursPath, strContoursDest, FALSE);
		
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("MergeTool Data Copy Result = %d. Panel ID : %s"), nRet, strPanelID); // JSO 0604 - ������ ���� ī��

		// Mura �ܰ��� ���� ���� ���
		swprintf(strContoursPath, _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID);

		// Mura �ܰ��� ���� ����
		JudgeSaveMuraContours(resultPanelData, strContoursPath);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Mura Contours End. Panel ID : %s"), strPanelID);
	}
	//	������ 18.12.05
	//else
	//{
	//	wchar_t strContoursPath[MAX_PATH] = { 0 };

	//	// Contours.Merge ���� ��� ���� 180322 YSS
	//	swprintf(strContoursPath, _T("%s\\%s\\Contours.Merge"), MERGETOOL_PATH, strPanelID);

	//	JudgeSaveContours(resultPanelData, strContoursPath);
	//}

	// ����� ����� ��ǥ?
	JudgementRepair(strPanelID, resultPanelData, WrtResultInfo);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Repair End. Panel ID : %s"), strPanelID);

	// 17.07.12 Panel Grade ���� �߰�
	JudgementPanelGrade(resultPanelData);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judge - PanelGrade End. Panel ID : %s"), strPanelID);

	// Work ��ǥ ��� �� �ҷ� ������� Numbering
	NumberingDefect(strModelID, strPanelID, strLotID, WrtResultInfo, resultPanelData, nRatio);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Numbering Defect End. Panel ID : %s"), strPanelID);

	// 17.12.19 ���� ��ġ �ݺ� �ҷ� ���� ��/�߾˶� - CCD �ҷ� - Auto-Run �ÿ��� ����
	if (eInspMode == eAutoRun)
	{
		JudgementRepeatCount(strPanelID, resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judge - RepeatCount End. Panel ID : %s"), strPanelID);
	}

	// ���� ���� �� ���� (Ư�� �ҷ� ���͸�, ��ǥ �ҷ� ����)
	ApplyReportRule(resultPanelData);

	// 	// ����� ���� CELL : NG / OK ����
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
	int nPointCount = 20;   // ���� ��ǥ�� Point �ҷ�,  ���� ���� �̻� Line ����
	int nOffSet = 9;        // ���� ��ǥ ���� ��,�� (nOffSet/2) �� Ȯ�� ( Ȧ�� ���� )

	// ex) 9 : �¿� 4Pixel
	int nHalfnOffSet = nOffSet / 2;

	// ����, ���� Projection
	int* nProjectionX = new int[nImageWidth];
	int* nProjectionY = new int[nImageHeight];

	// �迭 �ʱ�ȭ
	memset(nProjectionX, 0, sizeof(int) * nImageWidth);
	memset(nProjectionY, 0, sizeof(int) * nImageHeight);

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Point �ҷ� �ƴѰ� ����
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK)
			continue;

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			continue;

		// �ش� ��ǥ 1�� ����
		nProjectionX[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X]++;
		nProjectionY[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y]++;
	}

	CArray<CRect> listTempX;
	listTempX.RemoveAll();

	// ������ x ��ǥ�� nPointCount�� �̻� �ִ� ���
	for (int i = 0; i < nImageWidth; i++)
	{
		// �ҷ� ����Ʈ ��ȣ �߰�
		if (nProjectionX[i] > nPointCount)
		{
			// ó���� ��� �߰�
			if (listTempX.GetCount() == 0)
			{
				listTempX.Add(CRect(i - nHalfnOffSet, 0, i + nHalfnOffSet, nOffSet));
			}
			else
			{
				// ����Ʈ ��������
				CRect rectTemp = listTempX[listTempX.GetCount() - 1];

				// �������� �ִ� ��� ����
				if (i - rectTemp.right < nHalfnOffSet)
				{
					// X ���� ����
					rectTemp.right = i + nHalfnOffSet;

					// ��� �ֱ�
					listTempX[listTempX.GetCount() - 1] = rectTemp;
				}
				// �񿬼��� ��� �߰�
				else
				{
					listTempX.Add(CRect(i - nHalfnOffSet, 0, i + nHalfnOffSet, nOffSet));
				}
			}
		}
	}

	CArray<CRect> listTempY;
	listTempY.RemoveAll();

	// ������ y ��ǥ�� nPointCount�� �̻� �ִ� ���
	for (int i = 0; i < nImageHeight; i++)
	{
		// �ҷ� ����Ʈ ��ȣ �߰�
		if (nProjectionY[i] > nPointCount)
		{
			// ó���ΰ�� �߰�
			if (listTempY.GetCount() == 0)
			{
				listTempY.Add(CRect(0, i - nHalfnOffSet, nOffSet, i + nHalfnOffSet));
			}
			else
			{
				// ����Ʈ ��������
				CRect rectTemp = listTempY[listTempY.GetCount() - 1];

				// �������� �ִ� ��� ����
				if (i - rectTemp.bottom < nHalfnOffSet)
				{
					// Y ���� ����
					rectTemp.bottom = i + nHalfnOffSet;

					// ��� �ֱ�
					listTempY[listTempY.GetCount() - 1] = rectTemp;
				}
				// �񿬼��� ��� �߰�
				else
				{
					listTempY.Add(CRect(0, i - nHalfnOffSet, nOffSet, i + nHalfnOffSet));
				}
			}
		}
	}

	// nPointCount �̻� ���� ��ǥ�� �ϳ��� �ִ� ���
	if (listTempX.GetCount() != 0 || listTempY.GetCount() != 0)
	{
		// �ҷ� ����
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// Point �ҷ� �ƴѰ� ����
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK)
			{
				i++;		continue;
			}	// ���ѷ��� ����

			if (resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				i++;		continue;
			}	// ���ѷ��� ����

			bool bDelete = false;

			// ������ x ��ǥ
			for (int j = 0; j < listTempX.GetCount(); j++)
			{
				if (listTempX[j].left > resultPanelData.m_ListDefectInfo[i].Pixel_Center_X ||
					listTempX[j].right < resultPanelData.m_ListDefectInfo[i].Pixel_Center_X)
					continue;

				// ����Ʈ ��������
				CRect rectTemp = listTempX[j];

				// �ʱ� ����
				if (rectTemp.top == 0)
				{
					rectTemp.top = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					rectTemp.bottom = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;
				}
				// ���� ����
				else
				{
					if (rectTemp.top > resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y)
						rectTemp.top = (LONG)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

					else if (rectTemp.bottom < resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y)
						rectTemp.bottom = (LONG)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;
				}

				// ��� �ֱ�
				listTempX[j] = rectTemp;

				bDelete = true;
			}

			// ������ y ��ǥ
			for (int j = 0; j < listTempY.GetCount(); j++)
			{
				if (listTempY[j].top > resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y ||
					listTempY[j].bottom < resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y)
					continue;

				// ����Ʈ ��������
				CRect rectTemp = listTempY[j];

				// �ʱ� ����
				if (rectTemp.left == 0)
				{
					rectTemp.left = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					rectTemp.right = resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
				}
				// ���� ����
				else
				{
					if (rectTemp.left > resultPanelData.m_ListDefectInfo[i].Pixel_Center_X)
						rectTemp.left = (LONG)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;

					else if (rectTemp.right < resultPanelData.m_ListDefectInfo[i].Pixel_Center_X)
						rectTemp.right = (LONG)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				}

				// ��� �ֱ�
				listTempY[j] = rectTemp;

				bDelete = true;
			}

			// Point -> Line �ҷ� ������ ���
			if (bDelete)
			{
				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(i);
			}
			// ���� �ҷ�
			else
			{
				i++;
			}
		}

		ResultDefectInfo* pResultData;

		// ������ x ��ǥ
		for (int j = 0; j < listTempX.GetCount(); j++)
		{
			pResultData = new ResultDefectInfo;
			// Judgement ���� Defect Number �ǹ� ����
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
			pResultData->Img_Number = theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_GRAY_128);		// �ӽ÷� G128 �̹��� ���. �̹��� ��ȣ ���� ��� ���� �̹��� �ڸ� �� ����
			pResultData->Defect_MeanGV = 0;	// ���յǼ� ���GV ������.			
			//pResultData->Defect_Img_Name		= _T("");
			pResultData->Img_Size_X = (DOUBLE)listTempX[j].Width();		// �ӽ� �ҷ� ���� ���� ũ��
			pResultData->Img_Size_Y = (DOUBLE)listTempX[j].Height();	// �ӽ� �ҷ� ���� ���� ũ��
			pResultData->Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_OPEN;
			// Defect ��� �̹����� �׸� �� � �̹��� ��� ���� �����ؾ���. 0 ���� �� ����
			pResultData->Camera_No = 0;

			resultPanelData.Add_DefectInfo(*pResultData);
			SAFE_DELETE(pResultData);
		}

		// ������ y ��ǥ
		for (int j = 0; j < listTempY.GetCount(); j++)
		{
			pResultData = new ResultDefectInfo;
			// Judgement ���� Defect Number �ǹ� ����
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
			pResultData->Img_Number = theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_GRAY_128);		// �ӽ÷� G128 �̹��� ���. �̹��� ��ȣ ���� ��� ���� �̹��� �ڸ� �� ����
			pResultData->Defect_MeanGV = 0;	// ���յǼ� ���GV ������.
			//pResultData->Defect_Img_Name	= _T("");
			pResultData->Img_Size_X = 0;	// ���� ��ü ���� ����
			pResultData->Img_Size_Y = 0;	// ���� ��ü ���� ����
			pResultData->Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT;
			// Defect ��� �̹����� �׸� �� � �̹��� ��� ���� �����ؾ���. 0 ���� �� ����
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
// 	// ����ó��
// 	if ( dAlignPara == NULL )	return false;
// 
// 	// �ҷ� ����Ʈ ���� ��� ������
// 	if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )
// 		return true;
// 
// 	// ��� ��/��
// 	if( dAlignPara[E_PARA_GROUP_FLAG] <= 0 )
// 		return true;
// 
// 	int nGroupCount			= (int)(dAlignPara[E_PARA_GROUP1_COUNT]);					// 2�� �̻�
// 	int nGroupDiameterPixel	= (int)(dAlignPara[E_PARA_GROUP1_DIAMETER] / dResolution);	// 10mm ���� ( 0.0192 �Ķ����ȭ �ؾ� �� )
// 
// 	// �ҷ� ���� ���� 2�� ������ ���.
// 	if (nGroupCount < 2)			return false;
// 
// 	// ���� ������ ���� ���.
// 	if (nGroupDiameterPixel < 10)	return false;
// 
// 	// ���� ������
// 	long nGroupRadius = nGroupDiameterPixel / 2;
// 
// 	// ���� ���� �߽�
// 	cv::Point ptGroupCenter;
// 
// 	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
// 	{
// 		// KSW 17.08.26 - Point �ҷ� �ƴѰ� ���� ( ReTest ���� ���� ���� )
// 		if ( resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK				||
// 			 resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_POINT_PARTICLE	)
// 			continue;
// 		//if ( resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK				||
// 		//	(resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_POINT_ADJOIN_BRIGHT	&&
// 		//	resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_RETEST_POINT_DARK		)	||
// 		//	resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT		)
// 		//	continue;
// 
// 		// 1�� �˻�//////////////////////////////////////////////////
// 
// 		// KSW 17.07.19 - P/S ��忡 ���� ��ǥ ��
// 		// �ҷ� �߽����κ��� ���� ��(����) ��ŭ
// 		CRect rectGroupRange(
// 			(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio - nGroupDiameterPixel)	, 
// 			(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio - nGroupDiameterPixel)	,
// 			(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio + nGroupDiameterPixel)	,
// 			(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio + nGroupDiameterPixel)	);
// 
// 		// �ӽ� �ҷ� ���
// 		CArray<INT32> listTemp;
// 		listTemp.RemoveAll();
// 
// 		// �ʱ�ȭ
// 		ptGroupCenter.x = ptGroupCenter.y = 0;
// 
// 		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
// 		{			
// 			// ���� �ҷ� ������....
// 			//if (resultPanelData.m_ListDefectInfo[i].Defect_Type != resultPanelData.m_ListDefectInfo[j].Defect_Type) continue;
// 
// 			// KSW 17.08.26 - Point �ҷ� �ƴѰ� ���� ( ReTest ���� ���� ���� )
// 			if ( resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK			||
// 				 resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_POINT_PARTICLE	)
// 				continue;
// 
// 			// KSW 17.07.19 - �ҷ� ���� ��� ���� Point �ҷ�
// 			// Point �ҷ� �ƴѰ� ����
// 			//if ( resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK				||
// 			//	(resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_POINT_ADJOIN_BRIGHT	&&
// 			//	resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_RETEST_POINT_DARK		)	||
// 			//	resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT		)
// 			//	continue;
// 
// 			// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
// 			CPoint ptTemp(	(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio, 
// 							(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
// 
// 			// ���� �ȿ� �ҷ� ����?
// 			if (rectGroupRange.PtInRect( ptTemp ) )
// 			{
// 				// �׷� �߽���
// 				ptGroupCenter.x += (int)ptTemp.x;
// 				ptGroupCenter.y += (int)ptTemp.y;
// 
// 				// �ҷ� ����Ʈ ��ȣ �߰�
// 				listTemp.Add(j);
// 			}
// 		}
// 
// 		// �ҷ� ������ ������ �� ���ϸ� �ٸ� ���� ã��
// 		if (listTemp.GetCount() < nGroupCount) continue;
// 
// 		// 2�� �˻�//////////////////////////////////////////////////
// 
// 		// �׷� �߽� ���ϱ�
// 		float fGroupCx = ptGroupCenter.x / (float)listTemp.GetCount();
// 		float fGroupCy = ptGroupCenter.y / (float)listTemp.GetCount();
// 		float fTempX, fTempY, fTempLength;
// 
// 		for (int nCount = 0; nCount < listTemp.GetCount(); )
// 		{
// 			int nDefectNum = listTemp[nCount];
// 
// 			// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
// 			fTempX = (float)(resultPanelData.m_ListDefectInfo[nDefectNum].Pixel_Center_X / resultPanelData.m_ListDefectInfo[nDefectNum].nRatio) - fGroupCx;
// 			fTempY = (float)(resultPanelData.m_ListDefectInfo[nDefectNum].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[nDefectNum].nRatio) - fGroupCy;
// 
// 			// �ҷ� �׷� �߽����κ��� ����
// 			fTempLength = (fTempX * fTempX + fTempY * fTempY);
// 
// 			// �ҷ� �׷� �߽ɿ��� ���̰� ���������� ū��?
// 			if (sqrt(fTempLength) > nGroupRadius)
// 			{
// 				listTemp.RemoveAt(nCount);
// 			}
// 			// �ҷ� �׷� �߽ɿ��� ���̰� ���������� ������?
// 			else
// 			{
// 				// ���� �ҷ�����...
// 				nCount++;
// 			}
// 		}
// 
// 		// �ҷ� ������ ������ �� ���ϸ� �ٸ� ���� ã��
// 		if (listTemp.GetCount() < nGroupCount) continue;		
// 
// 		// �׸���
// 		for (int nImgNum = 0; nImgNum < theApp.GetGrabStepCount(); nImgNum++)
// 		{
// 			for (int nCamNum = 0; nCamNum < theApp.GetCameraCount(nImgNum); nCamNum++)
// 			{
// 				// KSW 17.07.19 - �ӽ÷� P/S ���� ����
// 				if( MatDraw[nImgNum][nCamNum].cols > 10000 )
// 					cv::circle(MatDraw[nImgNum][nCamNum], cv::Point((int)fGroupCx*2, (int)fGroupCy*2), (int)nGroupRadius*2, cv::Scalar(255,0,255));
// 				else
// 					cv::circle(MatDraw[nImgNum][nCamNum], cv::Point((int)fGroupCx, (int)fGroupCy), (int)nGroupRadius, cv::Scalar(255,0,255));
// 			}
// 		}
// 
// 		// �ҷ��� �׷����� ����
// 		for (int nCount = 0; nCount < listTemp.GetCount(); nCount++)
// 		{
// 			int nDefectNum = listTemp[nCount];
// 			ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[nDefectNum];
// 
// 			// KSW 17.07.19 - �ҷ� ��⿡ ���� ����
// 			switch( tTemp.Defect_Type )
// 			{
// 			// ���� �ҷ�
// 			case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
// 			case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
// 			case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:
// 			case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:
// 				tTemp.Defect_Type = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
// 				break;
// 
// 			// ��ο� �ҷ�
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
// 	// KSW 17.11.24 - Spec. �����
// 	// D(ũ��) = ( �庯 + �ܺ� ) / 2
// 	// DS(�Ÿ�) : �ҷ��� �ҷ� ���� �Ÿ�
// 	// N(����) : �� Panel �ȿ� �����ϴ� �ҷ� ����
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	// ����ó��
// 	if ( dAlignPara == NULL )	return false;
// 
// 	// �ҷ� ����Ʈ ���� ��� ������
// 	if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )
// 		return true;
// 
// 	// ��� ��/��
// 	if( dAlignPara[E_PARA_GROUP_FLAG] <= 0 )
// 		return true;
// 
// 	// �� �׷� ����
// 	const int nGroupCount	= 3;
// 
// 	// �Ķ���� ����
// 	int		nGroup_Count[nGroupCount];		// ���� �ȿ� n�� �̻�
// 	int		nGroup_Diameter[nGroupCount];	// ���� ( mm )
// 	double	dGroup_MinSize[nGroupCount];	// �ּ� ũ�� ( mm )
// 	double	dGroup_MaxSize[nGroupCount];	// �ִ� ũ�� ( mm )
// 	long	nGroup_Radius[nGroupCount];		// ������ ( pixel )
// 
// 	int nPara = E_PARA_GROUP1_COUNT;
// 	for (int i=0 ; i<nGroupCount ; i++)
// 	{
// 		nGroup_Count	[i]	= (int)dAlignPara[nPara++];					// ���� �ȿ� n�� �̻�
// 		nGroup_Diameter	[i]	= (int)(dAlignPara[nPara++] / dResolution);	// ���� ( pixel )
// 		dGroup_MinSize	[i]	= dAlignPara[nPara++];						// �ּ� ũ�� ( mm )
// 		dGroup_MaxSize	[i]	= dAlignPara[nPara++];						// �ִ� ũ�� ( mm )
// 		nGroup_Radius	[i]	= nGroup_Diameter[i] / 2;					// ������ ( pixel )
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
// 			// Point �ҷ� �ƴ� �ҷ� ���� ( ReTest ���� ���� )
// 			if (	nDefectType < E_DEFECT_JUDGEMENT_POINT_START	||
// 					nDefectType > E_DEFECT_JUDGEMENT_POINT_END		)
// 				continue;
// 
// 			// P/S ��忡 ���� ��ǥ ��
// 			CPoint ptTemp(	(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio, 
// 							(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);
// 
// 			for (int j=i+1 ; j<resultPanelData.m_ListDefectInfo.GetCount() ; j++)
// 			{
// 				int nDefectType2 = resultPanelData.m_ListDefectInfo[j].Defect_Type;
// 
// 				// Point �ҷ� �ƴ� �ҷ� ���� ( ReTest ���� ���� )
// 				if (	nDefectType2 < E_DEFECT_JUDGEMENT_POINT_START	||
// 						nDefectType2 > E_DEFECT_JUDGEMENT_POINT_END		)
// 					continue;
// 
// 				// P/S ��忡 ���� ��ǥ ��
// 				CPoint ptTemp2(	(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio, 
// 								(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
// 
// 				// P/S ��忡 ���� ��ǥ ��
// 				float fTempX = (float)(ptTemp2.x - ptTemp.x);
// 				float fTempY = (float)(ptTemp2.y - ptTemp.y);
// 
// 				// �ҷ� �׷� �߽����κ��� ����
// 				float fTempLength = (fTempX * fTempX + fTempY * fTempY);
// 
// 				// �ҷ� �׷� �߽ɿ��� ���̰� �������� ū��?
// 				if (sqrt(fTempLength) <= nGroup_Diameter[0])
// 				{
// 					// �ҷ��� �׷����� ����
// 					switch( nDefectType )
// 					{
// 						// ���� �ҷ�
// 					case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:
// 					case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:
// 						nDefectType = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
// 						break;
// 
// 						// ��ο� �ҷ�
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
// 					// �ҷ��� �׷����� ����
// 					switch( nDefectType2 )
// 					{
// 						// ���� �ҷ�
// 					case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:
// 					case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT:
// 					case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:
// 						nDefectType2 = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
// 						break;
// 
// 						// ��ο� �ҷ�
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
// 		// ���� ���� �߽�
// 		cv::Point ptGroupCenter;
// 
// 		for (int i=0 ; i<resultPanelData.m_ListDefectInfo.GetCount() ; i++)
// 		{
// 			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;
// 
// 			// Point �ҷ� �ƴ� �ҷ� ���� ( ReTest ���� ���� )
// 			if (	nDefectType < E_DEFECT_JUDGEMENT_POINT_START	||
// 				nDefectType > E_DEFECT_JUDGEMENT_POINT_END		)
// 				continue;
// 
// 			// (���� + ����) / 2  ->  ���� ���� ǥ��
// 			// P/S ��忡 ���� ��ǥ ����
// 			double dDimension = resultPanelData.m_ListDefectInfo[i].dDimension * dResolution / (double)resultPanelData.m_ListDefectInfo[i].nRatio;
// 
// 			// ũ�� �з�
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
// 			// �ش��ϴ� ������ �����ϴ� ��찡, �ϳ��� ������ ����
// 			if( nGroupType1 == -1 )
// 				AfxMessageBox(_T("Error Set Para Size !!"));
// 
// 			// ������ ��� 5mm ���
// 			if( nDefectType == E_DEFECT_JUDGEMENT_POINT_BRIGHT			||
// 				nDefectType == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT		||
// 				nDefectType == E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK		||
// 				nDefectType == E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT		||
// 				nDefectType == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	)
// 				nGroupType1 = 1;
// 
// 			// 1�� �˻�//////////////////////////////////////////////////
// 
// 			// P/S ��忡 ���� ��ǥ ��
// 			// �ҷ� �߽����κ��� ���� ��(����) ��ŭ
// 			CRect rectGroupRange(
// 				(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio - nGroup_Diameter[nGroupType1])	, 
// 				(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio - nGroup_Diameter[nGroupType1])	,
// 				(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio + nGroup_Diameter[nGroupType1])	,
// 				(int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio + nGroup_Diameter[nGroupType1])	);
// 
// 			// �ӽ� �ҷ� ���
// 			CArray<INT32> listTemp;
// 			listTemp.RemoveAll();
// 
// 			// �ʱ�ȭ
// 			ptGroupCenter.x = ptGroupCenter.y = 0;
// 
// 			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
// 			{
// 				// Point �ҷ� �ƴѰ� ���� ( ReTest ���� ���� ���� )
// 				if ( resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_POINT_START	||
// 					resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_POINT_END		)
// 					continue;
// 
// 				// P/S ��忡 ���� ��ǥ ��
// 				CPoint ptTemp(	(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio, 
// 					(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
// 
// 				// ���� �ȿ� �ҷ� ����?
// 				if (rectGroupRange.PtInRect( ptTemp ) )
// 				{
// 					// �׷� �߽���
// 					ptGroupCenter.x += (int)ptTemp.x;
// 					ptGroupCenter.y += (int)ptTemp.y;
// 
// 					// �ҷ� ����Ʈ ��ȣ �߰�
// 					listTemp.Add(j);
// 				}
// 			}
// 
// 			// �ҷ� ������ ������ �� ���ϸ� �ٸ� ���� ã��
// 			if (listTemp.GetCount() < nGroup_Count[nGroupType1]) continue;
// 
// 			// 2�� �˻�//////////////////////////////////////////////////
// 
// 			// �׷� �߽� ���ϱ�
// 			float fGroupCx = ptGroupCenter.x / (float)listTemp.GetCount();
// 			float fGroupCy = ptGroupCenter.y / (float)listTemp.GetCount();
// 			float fTempX, fTempY, fTempLength;
// 
// 			for (int nCount = 0; nCount < listTemp.GetCount(); )
// 			{
// 				int nDefectNum = listTemp[nCount];
// 
// 				// P/S ��忡 ���� ��ǥ ��
// 				fTempX = (float)(resultPanelData.m_ListDefectInfo[nDefectNum].Pixel_Center_X / resultPanelData.m_ListDefectInfo[nDefectNum].nRatio) - fGroupCx;
// 				fTempY = (float)(resultPanelData.m_ListDefectInfo[nDefectNum].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[nDefectNum].nRatio) - fGroupCy;
// 
// 				// �ҷ� �׷� �߽����κ��� ����
// 				fTempLength = (fTempX * fTempX + fTempY * fTempY);
// 
// 				// �ҷ� �׷� �߽ɿ��� ���̰� ���������� ū��?
// 				if (sqrt(fTempLength) > nGroup_Radius[nGroupType1])
// 				{
// 					listTemp.RemoveAt(nCount);
// 				}
// 				// �ҷ� �׷� �߽ɿ��� ���̰� ���������� ������?
// 				else
// 				{
// 					// ���� �ҷ�����...
// 					nCount++;
// 				}
// 			}
// 
// 			// �ҷ� ������ ������ �� ���ϸ� �ٸ� ���� ã��
// 			if (listTemp.GetCount() < nGroup_Count[nGroupType1]) continue;
// 
// 			// �׸���
// 			for (int nImgNum = 0; nImgNum < theApp.GetGrabStepCount(); nImgNum++)
// 			{
// 				for (int nCamNum = 0; nCamNum < theApp.GetCameraCount(nImgNum); nCamNum++)
// 				{
// 					// �ӽ÷� P/S ���� ����
// 					if( MatDraw[nImgNum][nCamNum].cols > 10000 )
// 						cv::circle(MatDraw[nImgNum][nCamNum], cv::Point((int)fGroupCx*2, (int)fGroupCy*2), (int)nGroup_Radius[nGroupType1]*2, cv::Scalar(255,0,255));
// 					else
// 						cv::circle(MatDraw[nImgNum][nCamNum], cv::Point((int)fGroupCx, (int)fGroupCy), (int)nGroup_Radius[nGroupType1], cv::Scalar(255,0,255));
// 				}
// 			}
// 
// 			// �ҷ��� �׷����� ����
// 			for (int nCount = 0; nCount < listTemp.GetCount(); nCount++)
// 			{
// 				int nDefectNum = listTemp[nCount];
// 				ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[nDefectNum];
// 
// 				// KSW 17.07.19 - �ҷ� ��⿡ ���� ����
// 				switch( tTemp.Defect_Type )
// 				{
// 					// ���� �ҷ�
// 				case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
// 				case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:
// 				case E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK:
// 				case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:
// 				case E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT:
// 				case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:
// 					tTemp.Defect_Type = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
// 					break;
// 
// 					// ��ο� �ҷ�
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

// All Pattern��
bool BumpInspection::JudgeGroupJudgment(ResultPanelData& resultPanelData, cv::Mat(*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	// PNZ - 2019.01.11 �ֽ� Spec.
	// Spect ���� 
	// 1. RGB Pattern �� POINT Dark �ҷ��� Size(Area)�� ���� 1,2,3���� ����, ���� �ҷ��� Dark Point�� ����
	// 2. Case 3 ������ ����
	// 3. Parameter ��
	//		- Count			: Pattern �� �ҷ� ���� ����, �ʰ��� ��� Group ����
	//		- Distance		: ���� �ҷ� ���� ( �ݰ� )
	//		- Delete On/Off : �ش� Pattern Group �ƴ� �ҷ� ����
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// ����ó��
	if (dAlignPara == NULL)	return false;
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;
	if (dAlignPara[E_PARA_GROUP_FLAG] <= 0)	return true;

	// Common Parameter �ʱ�ȭ
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

	// ���� ���� ���� ũ���� Ȯ��
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

		// Point �ƴ� �ҷ� ����
		if (nDefectType < E_DEFECT_JUDGEMENT_POINT_DARK || nDefectType > E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) continue;

		// Parameter �ʱ�ȭ
		nGroupDistance_1st = 0;

		// POINT Dark �ҷ� �̸� �ش� Distance ����
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

			// Point �ƴ� �ҷ� ����
			if (nDefectType2 < E_DEFECT_JUDGEMENT_POINT_DARK || nDefectType2 > E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) continue;

			// Parameter �ʱ�ȭ
			nGroupDistance_2nd = 0;

			// POINT Dark �ҷ� �̸� �ش� Distance ����
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1) nGroupDistance_2nd = nDistance_SP_1;
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2) nGroupDistance_2nd = nDistance_SP_2;
			if (nDefectType2 == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) nGroupDistance_2nd = nDistance_SP_3;

			// Compair Defect Center in MM
			CPoint ptTemp2((int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio,
				(int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// dX, dY in MM
			float fTempX = (float)(ptTemp2.x - ptTemp.x);
			float fTempY = (float)(ptTemp2.y - ptTemp.y);

			// �ҷ� �׷� �߽����κ��� ���� (MM)
			float fTempLength = (float)(sqrt(fTempX * fTempX + fTempY * fTempY) * dResolution);

			// Group ���� ���� ��
			bool bGroupJudgment_Distance = false;

			// �� �ҷ� �� �ϳ��� Dark SP 123 �� ��
			if (nGroupDistance_1st + nGroupDistance_2nd > 0)
			{
				if (fTempLength >= nGroupDistance_1st	&& fTempLength >= nGroupDistance_2nd)	bGroupJudgment_Distance = false;
				else if (fTempLength >= nGroupDistance_1st	&& fTempLength < nGroupDistance_2nd)	bGroupJudgment_Distance = true;
				else if (fTempLength < nGroupDistance_1st	&& fTempLength >= nGroupDistance_2nd)	bGroupJudgment_Distance = true;
				else if (fTempLength < nGroupDistance_1st	&& fTempLength < nGroupDistance_2nd)	bGroupJudgment_Distance = true;
			}

			// �� �ҷ� ��� Dark SP 123�� �ҷ� �� ���
			else if (nGroupDistance_1st + nGroupDistance_2nd == 0)
			{
				// �ִ� Distance3�� �������� ����
				if (fTempLength <= nDistance_SP_3) bGroupJudgment_Distance = true;
			}

			else
				continue;

			if (bGroupJudgment_Distance == true && nGroupDistance_1st + nGroupDistance_2nd > 0)
			{
				// �ҷ��� �׷����� ����
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
				// �ҷ��� �׷����� ����
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

	// �� Delete ���� ���� �� �̸� �׷��� 
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

		// Defect �̸� ����
		resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;

		// �ҷ� ����
		if (nDeleteDefect > 0) { resultPanelData.m_ListDefectInfo.RemoveAt(i); }
		else { i++; continue; }

	}

	return true;
}

// each Pattern(R, G, B ���� ī��Ʈ)
bool BumpInspection::JudgeGroupJudgmentEach(ResultPanelData& resultPanelData, cv::Mat(*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	// PNZ - 2019.01.11 �ֽ� Spec.
	// Spect ���� 
	// 1. RGB Pattern �� POINT Dark �ҷ��� Size(Area)�� ���� 1,2,3���� ����, ���� �ҷ��� Dark Point�� ����
	// 2. Case 3 ������ ����
	// 3. Parameter ��
	//		- Count			: Pattern �� �ҷ� ���� ����, �ʰ��� ��� Group ����
	//		- Distance		: ���� �ҷ� ���� ( �ݰ� )
	//		- Delete On/Off : �ش� Pattern Group �ƴ� �ҷ� ����
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// ����ó��
	if (dAlignPara == NULL)	return false;
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;
	if (dAlignPara[E_PARA_GROUP_FLAG] <= 0)	return true;

	// Common Parameter �ʱ�ȭ
//	int		nDefect1_Count		= 0;
//	int		nDefect2_Count		= 0;
//	int		nDefect3_Count		= 0;

	int 	nDefect1_Count_Non[3] = { 0,0,0 }; // R, G, B�� ���� �� ���� ī��Ʈ
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

	//// ���� ���� ���� ũ���� Ȯ��
	//if (nDefect1_Count_Non[E_IMAGE_CLASSIFY_AVI_R] >= nCount_SP_1 ||
	//	nDefect2_Count_Non[E_IMAGE_CLASSIFY_AVI_R] >= nCount_SP_2 ||
	//	nDefect3_Count_Non[E_IMAGE_CLASSIFY_AVI_R] >= nCount_SP_3)	bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_R] = true;
	//else															bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_R] = false;

	//// ���� ���� ���� ũ���� Ȯ��
	//if (nDefect1_Count_Non[E_IMAGE_CLASSIFY_AVI_G] >= nCount_SP_1 ||
	//	nDefect2_Count_Non[E_IMAGE_CLASSIFY_AVI_G] >= nCount_SP_2 ||
	//	nDefect3_Count_Non[E_IMAGE_CLASSIFY_AVI_G] >= nCount_SP_3)	bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_G] = true;
	//else															bGroupJudgment_Count_Array[E_IMAGE_CLASSIFY_AVI_G] = false;

	//// ���� ���� ���� ũ���� Ȯ��
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
	// ���� ī���ÿ�
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
		// Point �ƴ� �ҷ� ����
		if (nDefectType < E_DEFECT_JUDGEMENT_POINT_DARK || nDefectType > E_DEFECT_JUDGEMENT_POINT_GROUP_DARK) continue;

		// Parameter �ʱ�ȭ
		nGroupDistance_1st = 0;
		nGroupDistance_2st = 0;
		nGroupDistance_3st = 0;

		// POINT Dark �ҷ� �̸� �ش� Distance ����
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

			// Point �ƴ� �ҷ� ����
			if (nDefectType2 < E_DEFECT_JUDGEMENT_POINT_DARK || nDefectType2 > E_DEFECT_JUDGEMENT_POINT_GROUP_DARK) continue;

			// Parameter �ʱ�ȭ
			nGroupDistance_1nd = 0;
			nGroupDistance_2nd = 0;
			nGroupDistance_3nd = 0;

			// POINT Dark �ҷ� �̸� �ش� Distance ����
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

			// �ҷ� �׷� �߽����κ��� ���� (MM)
			float fTempLength = (float)(sqrt(fTempX * fTempX + fTempY * fTempY)* dResolution);

			// �� �ҷ� �� �ϳ��� Dark SP 123 �� ��
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
		///////////////////////////// �ҷ� ����
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
		{
			if (j == i) continue;

			bGroupJudgment_Distance = false;
			bGroupJudgment_Distance2 = false;
			bGroupJudgment_Distance3 = false;

			int nDefectType2 = resultPanelData.m_ListDefectInfo[j].Defect_Type;
			int nPatternType2 = resultPanelData.m_ListDefectInfo[j].Pattern_Type;

			if (nPatternType != nPatternType2) continue;

			// Point �ƴ� �ҷ� ����
			if (nDefectType2 < E_DEFECT_JUDGEMENT_POINT_DARK || nDefectType2 > E_DEFECT_JUDGEMENT_POINT_GROUP_DARK) continue;

			// Parameter �ʱ�ȭ
			nGroupDistance_1nd = 0;
			nGroupDistance_2nd = 0;
			nGroupDistance_3nd = 0;

			// POINT Dark �ҷ� �̸� �ش� Distance ����
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

			// �ҷ� �׷� �߽����κ��� ���� (MM)
			float fTempLength = (float)(sqrt(fTempX * fTempX + fTempY * fTempY)* dResolution);

			// �� �ҷ� �� �ϳ��� Dark SP 123 �� ��
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

	// �� Delete ���� ���� �� �̸� �׷��� 
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

		// Defect �̸� ����
		resultPanelData.m_ListDefectInfo[i].Defect_Type = nDefectType;

		// �ҷ� ����
		if (nDeleteDefect > 0) { resultPanelData.m_ListDefectInfo.RemoveAt(i); }
		else { i++; continue; }

	}

	return true;
}

bool BumpInspection::JudgeRGBSp3Judgment(ResultPanelData & resultPanelData, double * dAlignPara)
{
	// ����ó��
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

	//R,G,B ���Ͽ� ���� SP3�� � �ִ��� ī��Ʈ
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
	//������ ī��Ʈ �� ������ ���� �׷� ���� ���� ��û���� �ҷ� ī��Ʈ �Ѱ��� ����� ������ ���� ����
	//RED ����
	if (nCountRedSp3 >= nRedMax)
	{
		// �ҷ� ���� ��ŭ
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
	//GREEN ����
	if (nCountGreenSp3 >= nGreenMax)
	{
		// �ҷ� ���� ��ŭ
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
	//BLUE ����
	if (nCountBlueSp3 >= nBlueMax)
	{
		// �ҷ� ���� ��ŭ
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
		// �ҷ� ���� ��ŭ
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
		// �ҷ� ���� ��ŭ
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

	// �迭 �ʱ�ȭ
	memset(nProjection_R_X, 0, sizeof(int) * nImageWidth);
	memset(nProjection_G_X, 0, sizeof(int) * nImageWidth);
	memset(nProjection_B_X, 0, sizeof(int) * nImageWidth);
	//memset(nProjectionY, 0, sizeof(int) * nImageHeight);


	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Point �ҷ� �ƴѰ� ����
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB)
			continue;

		int nPatternType = resultPanelData.m_ListDefectInfo[i].Pattern_Type;
		if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_R)
			nProjection_R_X[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X]++; // �ش� ��ǥ 1����

		else if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_G)
			nProjection_G_X[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X]++; // �ش� ��ǥ 1����

		else if (theApp.GetImageClassify(nPatternType) == E_IMAGE_CLASSIFY_AVI_B)
			nProjection_B_X[(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X]++; // �ش� ��ǥ 1����
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
		// �ҷ� ����Ʈ ��ȣ �߰�
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
			// �ҷ� ���� ��ŭ
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
			{
				int nPatternType = resultPanelData.m_ListDefectInfo[j].Pattern_Type;
				if (theApp.GetImageClassify(nPatternType) != E_IMAGE_CLASSIFY_AVI_R)
					continue;
				CPoint nCenterX1;
				// POINT_BRIGHT_RGB �ҷ� �ƴ� ��� ����
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB)
				{
					continue;
				}
				nCenterX1.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;

				int nX1 = nCenterX1.x;
				// ���� �ȿ� �ҷ� ����?
				if (fabs(nX1 - nRPointCountX) <= nOffset)
				{
					// �ش� �ҷ� ����
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
					j--;
				}
				//else j++;  // ���� �ҷ�����...
			}
		}
		////////////G ����
		if (nBOpen == 1)
		{
			// �ҷ� ���� ��ŭ
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
			{
				int nPatternType = resultPanelData.m_ListDefectInfo[j].Pattern_Type;
				if (theApp.GetImageClassify(nPatternType) != E_IMAGE_CLASSIFY_AVI_B)
					continue;
				CPoint nCenterX1;
				// POINT_BRIGHT_RGB �ҷ� �ƴ� ��� ����
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB)
				{
					continue;
				}
				nCenterX1.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;

				int nX1 = nCenterX1.x;
				// ���� �ȿ� �ҷ� ����?
				if (fabs(nX1 - nBPointCountX) <= nOffset)
				{
					// �ش� �ҷ� ����
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
					j--;
				}
				//else j++;  // ���� �ҷ�����...
			}
		}
		//////////////////B ����
		if (nGOpen == 1)
		{
			// �ҷ� ���� ��ŭ
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
			{
				int nPatternType = resultPanelData.m_ListDefectInfo[j].Pattern_Type;
				if (theApp.GetImageClassify(nPatternType) != E_IMAGE_CLASSIFY_AVI_G)
					continue;
				CPoint nCenterX1;
				// POINT_BRIGHT_RGB �ҷ� �ƴ� ��� ����
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB)
				{
					continue;
				}
				nCenterX1.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;

				int nX1 = nCenterX1.x;
				// ���� �ȿ� �ҷ� ����?
				if (fabs(nX1 - nGPointCountX) <= nOffset)
				{
					// �ش� �ҷ� ����
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
					j--;
				}
				//else j++;  // ���� �ҷ�����...
			}
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// PNZ 2019-04-15
// Mura Normal �ҷ� �з�
//////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementMuraNormalClassification(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// ����ó��
	if (dAlignPara == NULL)									return false;
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;

	//////////////////////////////////////////////////////////////////////////
	// Type �� Classify Parameters
	int		nType2ClassPara = (int)dAlignPara[E_PARA_MURANORMAL_CLASSPARA_TYPE2]; // Count 5
	float	nType3ClassPara = (float)dAlignPara[E_PARA_MURANORMAL_CLASSPARA_TYPE3]; // Diff GV 23.0

	//////////////////////////////////////////////////////////////////////////
	// ����ó�� : Type �� �ҷ� ���� Ȯ��
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
	// Type 2 �ҷ� �з� : �����ϴ� ���� ���� ������ ����
	if ((nMuraType2_Count >= nType2ClassPara) && nType2ClassPara != 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			// Type 2 Mura �̸� Change Name
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
	// Type 3 �ҷ� �з� : �����ϴ� ���� ���� ������ ����
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

				// Type 3 Mura �̸� Change Name
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
// Contact �̻�� G64 �ҷ� �ٹ� ���� �˶� �߻� 
//////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementContactAlram(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// ����ó��
	if (dAlignPara == NULL)									return false;
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;

	//////////////////////////////////////////////////////////////////////////
	// Type �� Classify Parameters
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
	// �Ķ���� ����
	cv::Mat				MatOriginImage_ = LogicPara.MatOrgImage;
	int					nAlgorithmNumber_ = nAlgorithmNumber;					// ���� �˰���	
	int					nCameraNum_ = LogicPara.nCameraNum;				// �˻� ī�޶�
	int					nThrdIndex_ = LogicPara.nThreadLog;
	cv::Mat				*MatOriImageRGB_[3];
	cv::Mat				MatBKG_ = LogicPara.MatBKG;					// Mura���� ��� ( Cell �̿� ���� ) / Dust, Black ������ ����

	MatOriImageRGB_[0] = LogicPara.MatOrgRGBAdd[0];
	MatOriImageRGB_[1] = LogicPara.MatOrgRGBAdd[1];
	MatOriImageRGB_[2] = LogicPara.MatOrgRGBAdd[2];

	ENUM_KIND_OF_LOG	eLogCamera = (ENUM_KIND_OF_LOG)nCameraNum_;

	// ���� �г� ID
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

	// �˰��� ��� ���� ���� ����
	SYSTEMTIME time;
	::GetLocalTime(&time);

	// ���� �˰��� �˻� �Ķ���� ��������
	double* dAlgPara = theApp.GetAlgorithmParameter(0, nCameraNum_, nROINumber, nAlgorithmNumber_);
	double*	dAlignPara = theApp.GetAlignParameter(nCameraNum_);
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
		nRatio												// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	};

	//////////////////////////////////////////////////////////////////////////
	// �ӽ� Round Cell Ȯ�� ��
	//CString strTem;
	//strTem.Format(_T("D:\\%02d.bmp"), theApp.GetImageClassify(nImageNum));
	//cv::imwrite((cv::String)(CStringA)strTem, MatOriginImage_);
	//return true;
	//////////////////////////////////////////////////////////////////////////
	CString strResultTime;
	strResultTime.Format(_T("%s%sARESULT"), theApp.m_Config.GetNetworkDrivePath(), strDrive);
	if (nCommonPara[1] > 0) CreateDirectory(strResultTime, NULL);

	// �˻� ���(�ҷ�) ���� ����ü ( Dust ������ ���ݸ� �ص� �ɵ�... )
	stDefectInfo* pResultBlob = new stDefectInfo(MAX_DEFECT_COUNT, nImageNum);

	// �����Ͼ� ���� ����ü ��������
	STRU_DEFECT_ITEM* EngineerDefectJudgment = theApp.GetDefectFilteringParam(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);

	// Alg DLL Start Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Start. PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
		theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));

	// �ܰ��� ���� ���
	wchar_t fPath[MAX_PATH] = { 0 };
	CString strResultPath = RESULT_PATH;
	swprintf(fPath, _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID_);

	// �߰� ���� ���� ��� & ���� ����
	wchar_t wstrAlgPath[MAX_PATH] = { 0, };
	swprintf(wstrAlgPath, _T("%s\\%s\\"), (LPCWSTR)strResultTime, (LPCWSTR)strPanelID_);
	if (nCommonPara[1] > 0) CreateDirectory(wstrAlgPath, NULL);

	// Alg �α׿�
	wchar_t strAlgLog[MAX_PATH] = { 0, };
	swprintf(strAlgLog, _T("ID:%s\tPat:%s"), (LPCWSTR)strPanelID_, theApp.GetGrabStepName(nImageNum));

	CString strInspPath = INSP_PATH;
	//GijunAdd 220608
	CString strWaferMapPath = WAFERMAP_PATH;
	//////////////////////////////////////////////////////////////////////////
	// Result Buff ���� : LINE�� �ƴ� ��쿡 �� ���
	// ���� �Ҵ� & �ʱ�ȭ

	cv::Mat matReusltImage[E_DEFECT_COLOR_COUNT];
	cv::Mat matReuslt_Mura_Normal_Image[E_DEFECT_TOTAL_COUNT];
	cv::Mat matDustReusltImage[E_DEFECT_DUST_COUNT];
	CMatResultBuf* ResultBuf;

	ResultBuf = cMemResultBuff.FindFreeBuf_Result();

	SetMem_Result(ResultBuf);

	// ���� �Ҵ�
	matReusltImage[E_DEFECT_COLOR_DARK] = cMemResult->GetMat_Result(MatOriginImage_.size(), MatOriginImage_.type(), false);
	matReusltImage[E_DEFECT_COLOR_BRIGHT] = cMemResult->GetMat_Result(MatOriginImage_.size(), MatOriginImage_.type(), false);
		// Buff Set Start Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s BuffSet. PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
		theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));


	//////////////////////////////////////////////////////////////////////////
	// �˰��� DLL ����
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

	// ��� �߰��ؾ����� ����...
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
	// ó���� �Կ��ÿ��� ����
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

	// Map Y ���������� ������ 
	// �˻��ϴ� ������ Good Die ���� �Է�
	setMapData(&stParameter);
	// memory check Thread �Ѱ���
	StartThreadScanMemory(&stParameter);
	// Inspection Area Data check Thread �Ѱ��� ó�� ���� ã�� 
	StartThreadScanArea(&stParameter);
	// Map Check Thread �Ѱ���
	StartThreadScanAlignMark(&stParameter);
	// AlignMark�� Die ����
	StartThreadScanDie(&stParameter);
	
	if (nImageNum == STANDARD_IMAGE)
	{
		// Inspection Thread 
		StartThreadInspectionBump(&stParameter);
	}
	
	// ���������� ���
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
		// ���δ� �����ٸ� Thread ����
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
		// ���δ� �����ٸ� Thread ����
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

	// ����..


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

	// ������ ���, ���� �ڵ� & �α� ���
	if (lErrorCode != E_ERROR_CODE_TRUE)
	{
		// Alg DLL Error Log
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Error (Code : %d). PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
			theApp.GetAlgorithmName(nAlgorithmNumber_), lErrorCode, strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));
		return lErrorCode;
	}

	// ���� �ҷ� ����
	int nTotalblob = 0;

	// Dust ��� ����
	if (pResultBlob != NULL)
	{
		// ��˻� ���� ���� ��, �ҷ� ���� ( ��� ����, ��� �˰��򿡼� �ҷ� � �����ߴ��� �α� )
		nTotalblob = pResultBlob->nDefectCount;

		// ��˻� ���� ���� ��, Alg DLL End Log
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm End. PanelID: %s, CAM: %02d ROI: %02d, Img: %s.\n\t\t\t\t( Before DefectNum: %d )"),
			theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum), nTotalblob);

		//////////////////////////////////////////////////////////////////////////

		// �Ķ���� �޾ƿ���
		double* dAlignPara = theApp.GetAlignParameter(0);

		// ����� ���޾� ���� ��� �н�
		if (dAlignPara)
		{
			// ���� �ҷ��� �����ϴ� ���
			if (dAlignPara[E_PARA_NON_INSP_AREA_ONLY_LINE] > 0)
			{
				// ������ ��츸 ���� / ������ �ҷ� ���� ����
				if (nAlgorithmNumber_ == E_ALGORITHM_LINE)
				{
					// ��˻� ���� ���� ( ���� ���� �״�� ����ϹǷ�, Left-Top ���� ���ص� �� : NULL )
					m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);
				}
			}
			// ��� �ҷ� �����ϴ� ���
			else
			{
				// ��˻� ���� ���� ( ���� ���� �״�� ����ϹǷ�, Left-Top ���� ���ص� �� : NULL )
				m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);
			}
		}
		// ���޾� ���� ���, ��� �ҷ� ����
		else
		{
			// ��˻� ���� ���� ( ���� ���� �״�� ����ϹǷ�, Left-Top ���� ���ص� �� : NULL )
			m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);
		}

		// ��˻� ���� ���� ��, �ҷ� ���� ( ��� ����, ��� �˰��򿡼� �ҷ� � �����ߴ��� �α� )
		nTotalblob = pResultBlob->nDefectCount;

		// KSW 17.09.07 - �ʿ��� ��� ���
		if (theApp.GetCommonParameter()->bIFImageSaveFlag)
		{
			// �˰��� �ҷ� ��� ���� ����
			// MatDrawBuffer ���� ���� ǥ���ؾ� �� �� ����.

			CString strResultROIFile = _T("");
			strResultROIFile.Format(_T("%s\\%s\\ROI\\IMG-%02d_CAM-%02d_ROI-%02d %s Result.csv"),
				INSP_PATH, strPanelID_, nImageNum, nCameraNum_, nROINumber,
				theApp.GetAlgorithmName(nAlgorithmNumber_));

			// �ҷ� ���� ����
			BlobFeatureSave(pResultBlob, strResultROIFile);
		}

		// �ҷ� ���� ����
		pResultBlob_Total->AddTail_ResultBlobAndAddOffset(pResultBlob, NULL);
	}

	// Alg DLL End Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm End. PanelID: %s, CAM: %02d ROI: %02d, Img: %s.\n\t\t\t\t( After DefectNum: %d )"),
		theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum), nTotalblob);

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
/*
long BumpInspection::StartLogicAlgorithm(const CString strDrive, const tLogicPara& LogicPara, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
	cv::Mat& MatDrawBuffer, const int nImageNum, const int nROINumber, const int nAlgorithmNumber, tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, bool bpInspectEnd[][MAX_CAMERA_COUNT], int nRatio, bool *bPattern, bool *nCount, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	// �Ķ���� ����
	cv::Mat				MatOriginImage_ = LogicPara.MatOrgImage;
	int					nAlgorithmNumber_ = nAlgorithmNumber;					// ���� �˰���	
	int					nCameraNum_ = LogicPara.nCameraNum;				// �˻� ī�޶�
	int					nThrdIndex_ = LogicPara.nThreadLog;
	cv::Mat				*MatOriImageRGB_[3];
	cv::Mat				MatBKG_ = LogicPara.MatBKG;					// Mura���� ��� ( Cell �̿� ���� ) / Dust, Black ������ ����

	MatOriImageRGB_[0] = LogicPara.MatOrgRGBAdd[0];
	MatOriImageRGB_[1] = LogicPara.MatOrgRGBAdd[1];
	MatOriImageRGB_[2] = LogicPara.MatOrgRGBAdd[2];

	ENUM_KIND_OF_LOG	eLogCamera = (ENUM_KIND_OF_LOG)nCameraNum_;

	// ���� �г� ID
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

	// �˰��� ��� ���� ���� ����
	SYSTEMTIME time;
	::GetLocalTime(&time);

	// ���� �˰��� �˻� �Ķ���� ��������
	double* dAlgPara = theApp.GetAlgorithmParameter(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);
	double*	dAlignPara = theApp.GetAlignParameter(nCameraNum_);
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
		nRatio												// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	};

	//////////////////////////////////////////////////////////////////////////
	// �ӽ� Round Cell Ȯ�� ��
	//CString strTem;
	//strTem.Format(_T("D:\\%02d.bmp"), theApp.GetImageClassify(nImageNum));
	//cv::imwrite((cv::String)(CStringA)strTem, MatOriginImage_);
	//return true;
	//////////////////////////////////////////////////////////////////////////
	CString strResultTime;
	strResultTime.Format(_T("%s%sARESULT"), theApp.m_Config.GetNetworkDrivePath(), strDrive);
	if (nCommonPara[1] > 0) CreateDirectory(strResultTime, NULL);

	// �˻� ���(�ҷ�) ���� ����ü ( Dust ������ ���ݸ� �ص� �ɵ�... )
	stDefectInfo* pResultBlob = new stDefectInfo(MAX_DEFECT_COUNT, nImageNum);

	// �����Ͼ� ���� ����ü ��������
	STRU_DEFECT_ITEM* EngineerDefectJudgment = theApp.GetDefectFilteringParam(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);

	// Alg DLL Start Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Start. PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
		theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));

	// �ܰ��� ���� ���
	wchar_t fPath[MAX_PATH] = { 0 };
	CString strResultPath = RESULT_PATH;
	swprintf(fPath, _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID_);

	// �߰� ���� ���� ��� & ���� ����
	wchar_t wstrAlgPath[MAX_PATH] = { 0, };
	swprintf(wstrAlgPath, _T("%s\\%s\\"), (LPCWSTR)strResultTime, (LPCWSTR)strPanelID_);
	if (nCommonPara[1] > 0) CreateDirectory(wstrAlgPath, NULL);

	// Alg �α׿�
	wchar_t strAlgLog[MAX_PATH] = { 0, };
	swprintf(strAlgLog, _T("ID:%s\tPat:%s"), (LPCWSTR)strPanelID_, theApp.GetGrabStepName(nImageNum));

	CString strInspPath = INSP_PATH;
	//////////////////////////////////////////////////////////////////////////
	// Result Buff ���� : LINE�� �ƴ� ��쿡 �� ���
	// ���� �Ҵ� & �ʱ�ȭ

	cv::Mat matReusltImage[E_DEFECT_COLOR_COUNT];
	cv::Mat matReuslt_Mura_Normal_Image[E_DEFECT_TOTAL_COUNT];
	cv::Mat matDustReusltImage[E_DEFECT_DUST_COUNT];
	CMatResultBuf* ResultBuf;

	if (nAlgorithmNumber_ != E_ALGORITHM_LINE)
	{
		ResultBuf = cMemResultBuff.FindFreeBuf_Result();

		SetMem_Result(ResultBuf);

		// ���� �Ҵ�
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
	// �˰��� DLL ����
	//////////////////////////////////////////////////////////////////////////

	switch (nAlgorithmNumber_)
	{
	case E_ALGORITHM_POINT:
	{
		// C#���� C++�� cv::Mat ������ �޶� ����, ũ�� ���� �Ķ���� �Ѱ����.
		// MFC ����ϹǷ� cv::Mat �״�� �Ѱܵ� ������
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
		// Dust �ҷ� ���� ���, E�� ���� üũ �Լ�
		if (lErrorCode == E_ERROR_CODE_POINT_JUDEGEMENT_E)
		{
			// �ҷ� E�� ���� �ҷ� 1�� �߰� ( E_DEFECT_JUDGEMENT_DUST_GROUP )

			// UI�� �Ѱ��� ��� �ֱ� ( �������� ��ȣ 0���� �ӽ� )				
			//JudgeADDefect(nImageNum, nCameraNum_, 0, MatOriginImage_.cols, MatOriginImage_.rows, pResultBlob_Total, E_DEFECT_JUDGEMENT_DUST_GROUP, eInspMode, false);

			// nStageNo �߰�
			JudgeADDefect(nImageNum, nCameraNum_, m_stThrdAlignInfo.nStageNo, MatOriginImage_.cols, MatOriginImage_.rows, pResultBlob_Total, E_DEFECT_JUDGEMENT_DUST_GROUP, eInspMode, false);

			// �����ڵ� True�� ���� ( ���� �˻� ���� ���� )
			lErrorCode = E_ERROR_CODE_TRUE;
		}

		// ������ �ƴ� ���
		if (lErrorCode == E_ERROR_CODE_TRUE)
		{
			// Dust ������ ���, ���� ���
			if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_DUST)
			{
				lErrorCode = Point_GetDefectList(MatOriginImage_, MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_], MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_], MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);
			}
			// Dust ���� �ƴ� ���
			else
			{
				// Dust �˻� ���� �ñ��� ���
				while (!bpInspectEnd[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_])
				{
					Sleep(10);
				}
				////////////���� ã�� LJH//////// 
				if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_R)
					bPattern[E_POINT_DARK_RED] = true;
				else if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_G)
					bPattern[E_POINT_DARK_GREEN] = true;
				else if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_B)
					bPattern[E_POINT_DARK_BLUE] = true;
				////////////////////////////
				/////////////////////////////////////////////////////////////////////////////////////////////
				// Point_GetDefectList �Լ��� Ÿ�� ���ؼ���...
				// dust Pattern���� Point_FindDefect() ���� ��, Point_GetDefectList() �Լ� ���� �ؾ� ��
				/////////////////////////////////////////////////////////////////////////////////////////////

				if (!matReusltImage[E_DEFECT_COLOR_DARK].empty() &&
					!matReusltImage[E_DEFECT_COLOR_BRIGHT].empty() &&
					!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_][0].empty() &&
					!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nAlgorithmNumber_][1].empty())
				{
					// Dust �̹����� Point �˻��ϴ� Pattern �̹����� �ػ󵵰� �ٸ�.
					// DLL ���ο��� Dust �̹��� Ȯ�� / ����Ͽ� �˻�

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
			// Norch Round �κ� ���� �޾� ����
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
			// Dust �˻� ���� �ñ��� ���
			while (!bpInspectEnd[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_])
			{
				Sleep(10);
			}
			// Point �˰��� ��ȣ ��������
			int nPointAlgIndex = theApp.GetAlgorithmIndex(_T("POINT"));

			if (!MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][0].empty() &&
				!MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][1].empty() &&
				!MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][2].empty() &&
				!MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][3].empty() &&
				!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex][1].empty() &&
				!MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex][0].empty())
			{
				// Dust �̹����� Point �˻��ϴ� Pattern �̹����� �ػ󵵰� �ٸ�.
				// DLL ���ο��� Dust �̹��� Ȯ�� / ����Ͽ� �˻�
				lErrorCode = Line_GetDefectList(MatOriginImage_, MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_], MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_][nPointAlgIndex], MatBKG_, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, dAlignPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog);

			}
		}
	}
	break;

	case E_ALGORITHM_MURA:
	{
		lErrorCode = Mura_FindDefect(MatOriginImage_, MatOriImageRGB_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT],
			stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog);

		// ������ �ƴ� ���
		if (lErrorCode == E_ERROR_CODE_TRUE)
		{
			// Dust ���� �ƴ� ���
			if (theApp.GetImageClassify(nImageNum) != E_IMAGE_CLASSIFY_AVI_DUST)
			{
				// Point Dust �˻�(�������� ���) ���� �ñ��� ���
				while (!bpInspectEnd[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_])
				{
					Sleep(10);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////
				// Mura_GetDefectList �Լ��� Ÿ�� ���ؼ���...
				// Dust Pattern���� Point_FindDefect() ���� ��, Mura_GetDefectList() �Լ� ���� �ؾ� ��
				/////////////////////////////////////////////////////////////////////////////////////////////

				// Point �˰��� ��ȣ ��������
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
		// KSW 18.09.21 - RGB �ҷ��� ����Ǿ, ���� �˻� �ؾ� ��
		// R, G, B Line �ҷ� ������ ���
		//else if ( lErrorCode == E_ERROR_CODE_MURA_RGB_LINE_DEFECT )
		//{
			// KSW 18.09.19 - Alg �ȿ��� �߰� ( Defect-Map ǥ�� ��û )
			// nStageNo �߰�
			//JudgeADDefect(nImageNum, nCameraNum_, m_stThrdAlignInfo.nStageNo, MatOriginImage_.cols, MatOriginImage_.rows, pResultBlob_Total, E_DEFECT_JUDGEMENT_MURA_LINE_X, eInspMode, false);				

			// �����ڵ� True�� ���� ( ���� �˻� ���� ���� )
			//lErrorCode = E_ERROR_CODE_TRUE;
		//}
	}
	break;

	case E_ALGORITHM_MURA_NORMAL:
	{
		lErrorCode = MuraNormal_FindDefect(MatOriginImage_, MatOriImageRGB_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT], matReuslt_Mura_Normal_Image[E_DEFECT_TYPE1_WEAK_X], matReuslt_Mura_Normal_Image[E_DEFECT_TYPE1_WEAK_Y], matReuslt_Mura_Normal_Image[E_DEFECT_TYPE2_WEAK_X],
			matReuslt_Mura_Normal_Image[E_DEFECT_TYPE2_WEAK_Y], stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog);

		// ������ �ƴ� ���
		if (lErrorCode == E_ERROR_CODE_TRUE)
		{
			// Dust ���� �ƴ� ���
			if (theApp.GetImageClassify(nImageNum) != E_IMAGE_CLASSIFY_AVI_DUST)
			{
				// Point Dust �˻�(�������� ���) ���� �ñ��� ���
				while (!bpInspectEnd[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_])
				{
					Sleep(10);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////
				// Mura_GetDefectList �Լ��� Ÿ�� ���ؼ���...
				// Dust Pattern���� Point_FindDefect() ���� ��, Mura_GetDefectList() �Լ� ���� �ؾ� ��
				/////////////////////////////////////////////////////////////////////////////////////////////

				// Point �˰��� ��ȣ ��������
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

		// ������ �ƴ� ���
		if (lErrorCode == E_ERROR_CODE_TRUE)
		{
			// Dust ���� �ƴ� ���
			if (theApp.GetImageClassify(nImageNum) != E_IMAGE_CLASSIFY_AVI_DUST)
			{
				// Point Dust �˻�(�������� ���) ���� �ñ��� ���
				while (!bpInspectEnd[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum_])
				{
					Sleep(10);
				}

				/////////////////////////////////////////////////////////////////////////////////////////////
				// Mura_GetDefectList �Լ��� Ÿ�� ���ؼ���...
				// Dust Pattern���� Point_FindDefect() ���� ��, Mura_GetDefectList() �Լ� ���� �ؾ� ��
				/////////////////////////////////////////////////////////////////////////////////////////////

				// Point �˰��� ��ȣ ��������
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

	// ������ ���, ���� �ڵ� & �α� ���
	if (lErrorCode != E_ERROR_CODE_TRUE)
	{
		// Alg DLL Error Log
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Error (Code : %d). PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
			theApp.GetAlgorithmName(nAlgorithmNumber_), lErrorCode, strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));
		return lErrorCode;
	}

	// ���� �ҷ� ����
	int nTotalblob = 0;

	// Dust ��� ����
	if (pResultBlob != NULL)
	{
		// ��˻� ���� ���� ��, �ҷ� ���� ( ��� ����, ��� �˰��򿡼� �ҷ� � �����ߴ��� �α� )
		nTotalblob = pResultBlob->nDefectCount;

		// ��˻� ���� ���� ��, Alg DLL End Log
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm End. PanelID: %s, CAM: %02d ROI: %02d, Img: %s.\n\t\t\t\t( Before DefectNum: %d )"),
			theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum), nTotalblob);

		//////////////////////////////////////////////////////////////////////////

		// �Ķ���� �޾ƿ���
		double* dAlignPara = theApp.GetAlignParameter(0);

		// ����� ���޾� ���� ��� �н�
		if (dAlignPara)
		{
			// ���� �ҷ��� �����ϴ� ���
			if (dAlignPara[E_PARA_NON_INSP_AREA_ONLY_LINE] > 0)
			{
				// ������ ��츸 ���� / ������ �ҷ� ���� ����
				if (nAlgorithmNumber_ == E_ALGORITHM_LINE)
				{
					// ��˻� ���� ���� ( ���� ���� �״�� ����ϹǷ�, Left-Top ���� ���ص� �� : NULL )
					m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);
				}
			}
			// ��� �ҷ� �����ϴ� ���
			else
			{
				// ��˻� ���� ���� ( ���� ���� �״�� ����ϹǷ�, Left-Top ���� ���ص� �� : NULL )
				m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);
			}
		}
		// ���޾� ���� ���, ��� �ҷ� ����
		else
		{
			// ��˻� ���� ���� ( ���� ���� �״�� ����ϹǷ�, Left-Top ���� ���ص� �� : NULL )
			m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);
		}

		// ��˻� ���� ���� ��, �ҷ� ���� ( ��� ����, ��� �˰��򿡼� �ҷ� � �����ߴ��� �α� )
		nTotalblob = pResultBlob->nDefectCount;

		// KSW 17.09.07 - �ʿ��� ��� ���
		if (theApp.GetCommonParameter()->bIFImageSaveFlag)
		{
			// �˰��� �ҷ� ��� ���� ����
			// MatDrawBuffer ���� ���� ǥ���ؾ� �� �� ����.

			CString strResultROIFile = _T("");
			strResultROIFile.Format(_T("%s\\%s\\ROI\\IMG-%02d_CAM-%02d_ROI-%02d %s Result.csv"),
				INSP_PATH, strPanelID_, nImageNum, nCameraNum_, nROINumber,
				theApp.GetAlgorithmName(nAlgorithmNumber_));

			// �ҷ� ���� ����
			BlobFeatureSave(pResultBlob, strResultROIFile);
		}

		// �ҷ� ���� ����
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
	// ����ó��
	if (dAlignPara == NULL)	return false;

	// ���� Ȯ��
	if (dAlignPara[E_PARA_HS_JUDGMENT_COUNT_BRIGHT] > 0)
	{
		int nCountBright = 0;
		int nIndexBright = 0;

		// �ҷ� ���� ��ŭ
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			//if( theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_32	&&
			//	theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_64	&&
			//	theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_87)
			//	continue;

			// ���� �ٹ�
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				nCountBright++;

				if (nCountBright == 1)
					nIndexBright = i;	// ù��° �ε��� ����
			}
		}

		// ���� ���� �̻� �Ѿ�� ���, �ϳ��� ����
		if (dAlignPara[E_PARA_HS_JUDGMENT_COUNT_BRIGHT] < nCountBright)
		{
			// ù��° ����� ���� ���� -> RETEST_POINT_BRIGHT
			resultPanelData.m_ListDefectInfo[nIndexBright].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT;

			// �ҷ� ���� ��ŭ
			for (int i = nIndexBright + 1; i < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

				// G32 ���� �ٹ�
				if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_32	&&
					theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_64	&&
					theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_87)
				{
					i++;  // ���� �ҷ�����...
					continue;
				}

				// ���� �ٹ�
				if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
				{
					// �ش� �ҷ� ����
					resultPanelData.m_ListDefectInfo.RemoveAt(i);
				}
				else i++;  // ���� �ҷ�����...
			}
		}
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - ET Bright End. Panel ID : %s (Count : %d )"), strPanelID, nCountBright);
	}

	// ���� Ȯ��
	if (dAlignPara[E_PARA_HS_JUDGMENT_COUNT_DARK] > 0)
	{
		int nCountDark = 0;
		int nIndexDark = 0;

		// �ҷ� ���� ��ŭ
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			// R, G, B ���� �ٹ�
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
			{
				nCountDark++;

				if (nCountDark == 1)
					nIndexDark = i;	// ù��° �ε��� ����
			}
		}

		// ���� ���� �̻� �Ѿ�� ���, �ϳ��� ����
		if (dAlignPara[E_PARA_HS_JUDGMENT_COUNT_DARK] < nCountDark)
		{
			// ù��° ����� ���� ���� -> RETEST_POINT_DARK
			resultPanelData.m_ListDefectInfo[nIndexDark].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_POINT_DARK;

			// �ҷ� ���� ��ŭ
			for (int i = nIndexDark + 1; i < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

				// R, G, B ���� �ٹ�
				if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
				{
					// �ش� �ҷ� ����
					resultPanelData.m_ListDefectInfo.RemoveAt(i);
				}
				else i++;  // ���� �ҷ�����...
			}
		}
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - ET Dark End. Panel ID : %s (Count : %d )"), strPanelID, nCountDark);
	}

	return true;
}


// bool BumpInspection::JudgementEdgePadBright(ResultPanelData& resultPanelData, double* dAlignPara)
// {
// 	// ����ó��
// 	if ( dAlignPara == NULL )	return false;
// 
// 	if(	m_stThrdAlignInfo.nRatio			> 0	&&
// 		dAlignPara[E_PARA_PAD_BRIGHT_FLAG]	> 0	)
// 	{
// 		// Pad ��, ���� ���� �𼭸� ( P/S ���� )
// 		cv::Point ptRT = cv::Point((int)(m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x / m_stThrdAlignInfo.nRatio	), (int)(m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y / m_stThrdAlignInfo.nRatio	));
// 		cv::Point ptRB = cv::Point((int)(m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x / m_stThrdAlignInfo.nRatio	), (int)(m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y / m_stThrdAlignInfo.nRatio	));
// 
// 		// Pad ��, ���� ����
// 		CRect rectPadT( (int)(ptRT.x - dAlignPara[E_PARA_PAD_BRIGHT_X]), (int)(ptRT.y), (int)(ptRT.x), (int)(ptRT.y + dAlignPara[E_PARA_PAD_BRIGHT_Y]));
// 		CRect rectPadB( (int)(ptRB.x - dAlignPara[E_PARA_PAD_BRIGHT_X]), (int)(ptRB.y - dAlignPara[E_PARA_PAD_BRIGHT_Y]), (int)(ptRB.x), (int)(ptRB.y));
// 
// 		// ��� �ҷ� ������ŭ...
// 		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
// 		{
// 			if ( resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT )
// 				continue;
// 
// 			// P/S ��忡 ���� ��ǥ ��
// 			CPoint ptTemp(	(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio,
// 				(int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);
// 
// 			// ���� �ȿ� �ҷ� ����?
// 			if (rectPadT.PtInRect( ptTemp ) )
// 			{
// 				ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[i];
// 
// 				tTemp.Defect_Type = E_DEFECT_JUDGEMENT_POINT_PAD_BRIGHT;
// 
// 				resultPanelData.m_ListDefectInfo[i] = tTemp;
// 			}
// 
// 			// ���� �ȿ� �ҷ� ����?
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


// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
bool BumpInspection::JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
{
#if USE_ALG_CONTOURS

	// �ܰ��� ���� ��� ���� ���
	if (strContourTxt == NULL)			return false;

	// �ҷ� ���ų� �˻� ���� ��� Skip
	//if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )	return true;

	// TXT ����
	CStdioFile	fileWriter;
	CString		strContourIndex;

	// ���� ����
	if (fileWriter.Open(strContourTxt, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		// ȸ�� ��ǥ ��� ��, ���
		double dTheta = -m_stThrdAlignInfo.dAlignTheta * PI / 180.;
		double	dSin = sin(dTheta);
		double	dCos = cos(dTheta);
		int nCx = m_stThrdAlignInfo.ptAlignCenter.x;
		int nCy = m_stThrdAlignInfo.ptAlignCenter.y;

		// ȸ���� ���� ����
		cv::Point ptCorner[E_CORNER_END];

		// ȸ�� ��, ���� ��ǥ ���
		for (int m = 0; m < E_CORNER_END; m++)
		{
			ptCorner[m].x = (int)(dCos * (m_stThrdAlignInfo.ptCorner[m].x - nCx) - dSin * (m_stThrdAlignInfo.ptCorner[m].y - nCy) + nCx);
			ptCorner[m].y = (int)(dSin * (m_stThrdAlignInfo.ptCorner[m].x - nCx) + dCos * (m_stThrdAlignInfo.ptCorner[m].y - nCy) + nCy);
		}

		// ����� ����
		CRect rectROI = CRect(
			min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
			min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
			max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
			max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

		// P/S ��忡 ���� ��ǥ ���� ( 1�� ��ǥ�� ���� )
		rectROI.left /= m_stThrdAlignInfo.nRatio;
		rectROI.top /= m_stThrdAlignInfo.nRatio;
		rectROI.right /= m_stThrdAlignInfo.nRatio;
		rectROI.bottom /= m_stThrdAlignInfo.nRatio;

		// �ֻ��� ��� �ۼ� ( Cell ũ�� )
		strContourIndex.Format(_T("Cell_X=%d, Cell_Y=%d\n"), rectROI.Width(), rectROI.Height());
		fileWriter.SeekToEnd();
		fileWriter.WriteString(strContourIndex);

		// �ҷ� ���� ��ŭ
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// UI ���� ����
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			// ��� ���� ���� ( No. || ���� ���� || ī�޶� ��ȣ || �ҷ��� )
			strContourIndex.Format(_T("No=%d, Pattern=%02d, Camera=%02d, Defect=%02d\n"), i + 1, theApp.GetImageClassify(nImgNum), resultPanelData.m_ListDefectInfo[i].Camera_No, resultPanelData.m_ListDefectInfo[i].Defect_Type);
			fileWriter.SeekToEnd();
			fileWriter.WriteString(strContourIndex);

			// AD �ҷ��� ���
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_DUST_GROUP)
			{
				// �簢�� E_CORNER_LEFT_TOP ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), 0, 0);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// �簢�� E_CORNER_RIGHT_TOP ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), rectROI.Width() - 1, 0);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// �簢�� E_CORNER_RIGHT_BOTTOM ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), rectROI.Width() - 1, rectROI.Height() - 1);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// �簢�� E_CORNER_LEFT_BOTTOM ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), 0, rectROI.Height() - 1);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				continue;
			}

			// ���� �ҷ� �ΰ�?
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

			// �ܰ��� ���� �ѱ��
			if (bMura)
			{
				// �ܰ��� ����
				for (int j = 0; j < MAX_CONTOURS; j++)
				{
					// P/S ��忡 ���� ��ǥ ���� ( 1�� ��ǥ�� ���� )
					// CFeatureExtraction::calcContours() ���� �̹� 1�� ��ǥ�� ����Ǽ� ����
					int x = resultPanelData.m_ListDefectInfo[i].nContoursX[j];
					int y = resultPanelData.m_ListDefectInfo[i].nContoursY[j];

					// ���� ���� ��� ������
					if (x == 0 && y == 0)	break;

					// E_CORNER_LEFT_TOP ��ǥ�� �������� ���
					x -= rectROI.left;
					y -= rectROI.top;

					// ����ó��
					if (x < 0)						x = 0;
					if (y < 0)						y = 0;
					if (x >= rectROI.Width())		x = rectROI.Width() - 1;
					if (y >= rectROI.Height())		y = rectROI.Height() - 1;

					// �ܰ��� ���� ���� ����
					strContourIndex.Format(_T("%d, %d\n"), x, y);

					fileWriter.SeekToEnd();
					fileWriter.WriteString(strContourIndex);
				}
			}
			// ���� �̿��� �ҷ� ( Point & Line ) / �簢�� ���� �ѱ��
			else
			{
				int nOffSet = 5;
				cv::Point ptDefectArea[4] = { (0, 0), };

				// �ش� �ҷ� �簢�� ������ ����
				ptDefectArea[E_CORNER_LEFT_TOP].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_TOP].x - nOffSet;
				ptDefectArea[E_CORNER_LEFT_TOP].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_TOP].y - nOffSet;
				ptDefectArea[E_CORNER_RIGHT_TOP].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_TOP].x + nOffSet;
				ptDefectArea[E_CORNER_RIGHT_TOP].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_TOP].y - nOffSet;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM].x + nOffSet;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM].y + nOffSet;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].x = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_BOTTOM].x - nOffSet;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].y = resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_BOTTOM].y + nOffSet;

				// P/S ��忡 ���� ��ǥ ���� ( 1�� ��ǥ�� ���� )
				int nRatio = resultPanelData.m_ListDefectInfo[i].nRatio;
				ptDefectArea[E_CORNER_LEFT_TOP].x /= nRatio;
				ptDefectArea[E_CORNER_LEFT_TOP].y /= nRatio;
				ptDefectArea[E_CORNER_RIGHT_TOP].x /= nRatio;
				ptDefectArea[E_CORNER_RIGHT_TOP].y /= nRatio;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].x /= nRatio;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].y /= nRatio;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].x /= nRatio;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].y /= nRatio;

				// E_CORNER_LEFT_TOP ��ǥ�� �������� ���
				ptDefectArea[E_CORNER_LEFT_TOP].x -= rectROI.left;
				ptDefectArea[E_CORNER_LEFT_TOP].y -= rectROI.top;
				ptDefectArea[E_CORNER_RIGHT_TOP].x -= rectROI.left;
				ptDefectArea[E_CORNER_RIGHT_TOP].y -= rectROI.top;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].x -= rectROI.left;
				ptDefectArea[E_CORNER_RIGHT_BOTTOM].y -= rectROI.top;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].x -= rectROI.left;
				ptDefectArea[E_CORNER_LEFT_BOTTOM].y -= rectROI.top;

				// ����ó��
				for (int m = E_CORNER_LEFT_TOP; m <= E_CORNER_LEFT_BOTTOM; m++)
				{
					if (ptDefectArea[m].x < 0)						ptDefectArea[m].x = 0;
					if (ptDefectArea[m].y < 0)						ptDefectArea[m].y = 0;
					if (ptDefectArea[m].x >= rectROI.Width())		ptDefectArea[m].x = rectROI.Width() - 1;
					if (ptDefectArea[m].y >= rectROI.Height())		ptDefectArea[m].y = rectROI.Height() - 1;
				}

				// �簢�� E_CORNER_LEFT_TOP ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), ptDefectArea[E_CORNER_LEFT_TOP].x, ptDefectArea[E_CORNER_LEFT_TOP].y);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// �簢�� E_CORNER_RIGHT_TOP ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), ptDefectArea[E_CORNER_RIGHT_TOP].x, ptDefectArea[E_CORNER_RIGHT_TOP].y);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// �簢�� E_CORNER_RIGHT_BOTTOM ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), ptDefectArea[E_CORNER_RIGHT_BOTTOM].x, ptDefectArea[E_CORNER_RIGHT_BOTTOM].y);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// �簢�� E_CORNER_LEFT_BOTTOM ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), ptDefectArea[E_CORNER_LEFT_BOTTOM].x, ptDefectArea[E_CORNER_LEFT_BOTTOM].y);
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
bool BumpInspection::JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
{
	// �ܰ��� ���� ��� ���� ���
	if (strContourTxt == NULL)			return false;

	// �ҷ� ���ų� �˻� ���� ��� Skip
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;

	// TXT ����
	CStdioFile	fileWriter;
	CString		strContourIndex;

	// ���� ����
	if (fileWriter.Open(strContourTxt, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		// ȸ�� ��ǥ ��� ��, ���
		double dTheta = -m_stThrdAlignInfo.dAlignTheta * PI / 180.;
		double	dSin = sin(dTheta);
		double	dCos = cos(dTheta);
		int nCx = m_stThrdAlignInfo.ptAlignCenter.x;
		int nCy = m_stThrdAlignInfo.ptAlignCenter.y;

		// �ҷ� ���� ��ŭ
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// AD �ҷ��� ���
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_DUST_GROUP)
				continue;

			// ���� �ҷ� �ΰ�?
			bool bMura = false;

			// �ҷ� ����
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

			// ���� �ƴ� ��� ����
			if (!bMura) continue;

			// �ܰ��� ����
			for (int j = 0; j < MAX_CONTOURS; j++)
			{
				// P/S ��忡 ���� ��ǥ ���� ( 1�� ��ǥ�� ���� )
				// CFeatureExtraction::calcContours() ���� �̹� 1�� ��ǥ�� ����Ǽ� ����
				int x = resultPanelData.m_ListDefectInfo[i].nContoursX[j];
				int y = resultPanelData.m_ListDefectInfo[i].nContoursY[j];

				// ���� ���� ��� ������
				if (x == 0 && y == 0)	break;

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


bool BumpInspection::DeleteOverlapDefect(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.09.21 - 6.39" �ͺκ� ���� �ȵȰ��, E�� ����
	// EDGE_NUGI �� ���, EDGE_NUGI�� ������ ��� �ҷ� ����
	//////////////////////////////////////////////////////////////////////////

	bool bNugiJudgeE = false;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// EDGE_NUGI ã��
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI)
		{
			// EDGE_NUGI ����Ʈ ó������ ����
			resultPanelData.m_ListDefectInfo[0] = resultPanelData.m_ListDefectInfo[i];

			// E�� ����
			bNugiJudgeE = true;

			break;
		}
	}

	// EDGE_NUGI �ִ� ���...
	if (bNugiJudgeE)
	{
		// ��� �ҷ� ����
		for (int i = 1; i < resultPanelData.m_ListDefectInfo.GetCount(); )
			resultPanelData.m_ListDefectInfo.RemoveAt(i);

		// ������, ������
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// AD�� ���, AD�� ������ ��� �ҷ� ����
	// EDGE_NUGI�� ���µ�, AD�� ���
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// AD �ҷ��� �ƴ� ��� ( E_DEFECT_JUDGEMENT_DUST_GROUP ���� )
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT)
			continue;

		// ù ���� AD �ҷ��� ���
		if (resultPanelData.m_ListDefectInfo[i].Img_Number == 0)
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

	//////////////////////////////////////////////////////////////////////////
	// AD �ƴ� ���, �ߺ� ����
	//////////////////////////////////////////////////////////////////////////

	// Point - Point �ߺ� ����
	DeleteOverlapDefect_Point_Point(resultPanelData, dAlignPara);

	//LJH 20201201 ���� ���� Point - Point �ߺ� ����
	DeleteOverlapDefect_Point_Point_RGB(resultPanelData, dAlignPara);

	// Point - Mura �ߺ� ����
	DeleteOverlapDefect_Point_Mura(resultPanelData, dAlignPara);

	// Point - Line �ߺ� ����
	DeleteOverlapDefect_Point_Line(resultPanelData, dAlignPara);

	// Line - Mura �ߺ� ����
	DeleteOverlapDefect_Line_Mura(resultPanelData, dAlignPara);

	// Mura - Mura �ߺ� ����
//	DeleteOverlapDefect_Mura_Mura(resultPanelData, dAlignPara);

	return true;
}


bool BumpInspection::DeleteOverlapDefect_DimpleDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{
	// ����ó�� Parameter �� ������
	if (dAlignPara == NULL)
		return false;

	// Dimple Ratio
	double dDimpleRatio_GrayPattern_Active = (double)(dAlignPara[E_PARA_DIMPLE_RATIO_GRAY_Active]);
	double dDimpleRatio_GrayPattern_Edge = (double)(dAlignPara[E_PARA_DIMPLE_RATIO_GRAY_Edge]);
	double dDimpleRatio_RGBPattern = (double)(dAlignPara[E_PARA_DIMPLE_RATIO_RGB]);


	// ����ó�� 
	if (dDimpleRatio_GrayPattern_Active <= 0 || dDimpleRatio_GrayPattern_Edge <= 0 || dDimpleRatio_RGBPattern <= 0)
		return false;

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// Point �ҷ� Count
	int nPointDefect = 0;
	int nRGBDefect = 0;
	int nGreenDefect = 0;
	int nGrayDefect = 0;
	int nMultiBP = 0;

	// �ҷ� ����Ʈ�� ���� �� -> Point �ҷ� ������ ���� / White or G64 Pattern�� �ҷ� ������ ���� RGB �ҷ��� �������� �Ͽ� Data ȹ��
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		// Point �ҷ��� ��츸 ã��
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
		{
			//Point�ҷ� Count
			nPointDefect++;
		}

		// Pattern ����
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		// Point �ҷ��� White Gray Pattern�� �ִ� ��츸 ã��
		if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_64)
		{
			// G64 Pattern�� �ִ� Point�ҷ� Count
			nGrayDefect++;
		}

		// Point �ҷ��� White Gray Pattern�� �ִ� ��츸 ã��
		if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_R || nImgNum1 == E_IMAGE_CLASSIFY_AVI_G || nImgNum1 == E_IMAGE_CLASSIFY_AVI_B)
		{
			// White Pattern�� G64 Pattern�� �ִ� Point�ҷ� Count
			nRGBDefect++;
		}

	}

	// Green Pattern���� Retest �ҷ��� �������� Ȯ��
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Point �ҷ��� ��츸 ã��
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
		{
			// Pattern ����
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Point �ҷ��� White Gray Pattern�� �ִ� ��츸 ã��
			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_G)
			{
				// White Pattern�� G64 Pattern�� �ִ� Point�ҷ� Count
				nGreenDefect++;
			}
		}
	}

	// G64 Pattern�� �ִ� Multi-BP ���� Count
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Point �ҷ��� ��츸 ã��
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_MULT_BP)
		{
			// Pattern ����
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Point �ҷ��� White Gray Pattern�� �ִ� ��츸 ã��
			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_64)
			{
				// White Pattern�� G64 Pattern�� �ִ� Point�ҷ� Count
				nMultiBP++;
			}
		}
	}

	// Point �ҷ��� ������ ����
	if (nPointDefect <= 0 && nGreenDefect <= 0 && nMultiBP <= 0)
		return true;

	// Parameter ��
	int	nOffSet = 4;
	int	nRangeOffset = 10;

	// ��ǥ�� 
	cv::Rect rectTemp;

	// ��ǥ �ʱ�ȭ
	int	PixelStartX = 0;
	int	PixelStartY = 0;
	int	PixelEndX = 0;
	int	PixelEndY = 0;

	// ROI Region
	int nXStart = m_stThrdAlignInfo.rcAlignCellROI.x;
	int nYStart = m_stThrdAlignInfo.rcAlignCellROI.y;
	int nXEnd = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
	int nYEnd = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

	// White & G64 Pattern ���� ����
	cv::Mat mWhiteImage = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_WHITE)][0];
	cv::Mat mG64Image = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_GRAY_64)][0];

	// Gaussian Blur
	cv::GaussianBlur(mWhiteImage, mWhiteImage, cv::Size(5, 5), 3.0);
	cv::GaussianBlur(mG64Image, mG64Image, cv::Size(5, 5), 3.0);

	//////////////////////////////////////////////////////////////////////////
	// Gray Pattern ���� �ҷ� Count
	//////////////////////////////////////////////////////////////////////////

	if (nGrayDefect > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Point �ҷ��� ��츸...
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				continue;
			}

			// �ҷ� Pattern�� ã��
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// RGB�� �ִ� �ҷ� �� ã��
			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_64)
			{
				continue;
			}

			////////////////////////////////
			// Max-Mean Ratio Calculation //
			int nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
			int nDefectMeanGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
			float MaxMeanRatio = (float)nDefectMaxGV / nDefectMeanGV;

			// �����/Dimple�� ���Ͽ� �ӽ÷� Max/Mean Ratio && �ִ� ��� ������ �з� ��
			if (MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
				continue;

			//��ǥ��
			PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nRangeOffset);
			PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y - 2);
			PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_X + nRangeOffset);
			PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y + 2);

			// Ratio �ʱ�ȭ Active Ratio�� �����Ѵ�
			double dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Active;

			// Edge ��ó 100Pixel �̳��� Edge ����
			int nEdgeOffset_X = 90;
			int nEdgeOffset_Y = 100;

			// X ���� Edge �κ� Dimple ���� Ratio�� ������
			if ((PixelStartX - nEdgeOffset_X <= nXStart) || (PixelEndX + nEdgeOffset_X >= nXEnd))
			{
				// ���� Profile�� �������� ��ȯ��
				PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X - 2);
				PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y - nRangeOffset);
				PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X + 2);
				PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y + nRangeOffset);

				dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Edge;
			}

			// Y ���� Edge �κ� Dimple ���� Ratio�� ������
			if ((PixelStartY - nEdgeOffset_Y <= nYStart) || (PixelEndY + nEdgeOffset_Y >= nYEnd))
			{
				dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Edge;
			}

			//Rect �׸���
			rectTemp.x = PixelStartX;
			rectTemp.y = PixelStartY;
			rectTemp.width = PixelEndX - PixelStartX + 1;
			rectTemp.height = PixelEndY - PixelStartY + 1;

			// �ҷ� �߽� �������� White/G64 Pattern ���� ���� ������ ȹ��
			cv::Mat matTempBufWhite = mWhiteImage(rectTemp);
			cv::Mat matTempBufG64 = mG64Image(rectTemp);

			// ���� �ִ밪 ��� ���� ���Ѵ�
			cv::Scalar tempMean_W, tempStd_W, tempMean_G64, tempStd_G64;

			double minvalue_W = 0;
			double maxvlaue_W = 0;
			double minvalue_G64 = 0;
			double maxvlaue_G64 = 0;

			// White Pattern ���� ���ϱ�
			cv::meanStdDev(matTempBufWhite, tempMean_W, tempStd_W);
			cv::minMaxIdx(matTempBufWhite, &minvalue_W, &maxvlaue_W, NULL, NULL);
			double dImageavg_W = tempMean_W[0];
			double dRatio1 = maxvlaue_W / dImageavg_W;

			// G64 Pattern ���� ���ϱ�
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
			// 			// Top 3 GV�� Max GV �ٽ� ���ϱ�
			// 			NewMaxGVMethold(matTempBufWhite, maxvlaue_W, Newmaxvlaue_W, nTopGVCount);
			// 			NewMaxGVMethold(matTempBufG64, maxvlaue_G64, Newmaxvlaue_G64, nTopGVCount);
			// 			double dNewRatio = (Newmaxvlaue_G64/dImageavg_G64) / (Newmaxvlaue_W/dImageavg_W);
						//////////////////////////////////////////////////////////////////////////
						// G64�� White���� ���� ��� �ö���� ������ �ҷ����� Continue��
			if (maxvlaue_W >= 200 && maxvlaue_G64 >= 200)
				continue;

			if (dRatio <= dDimpleRatio_GrayPattern)
				//if (dNewRatio <= dDimpleRatio_GrayPattern)
			{
				// i�� ������ Dimple�� �ǽ� Dimple�� Defect_Type ����
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DIMPLE;
				continue;
			}
			else
				continue;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Case 2: White-X G64-X  -> RGB Pattern Dimple Check ���
	// 1. White + G64 Pattern���� Dimple�� ���� ���ϴ� ��ġ ����
	// 2. RGB �ҷ� ��ġ�� �������� �Ͽ� White&G64 Pattern�� ���� Ratio�� ���Ͽ� ���� ��
	// 3. RGB�� ���������� �����ϱ⿡ Ratio�� ũ�� �൵ ������ Active�� Edge ������ �ʴ´�
	//////////////////////////////////////////////////////////////////////////

	if (nRGBDefect > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Point �ҷ��� ��츸...
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				continue;
			}

			// �ҷ� Pattern�� ã��
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// RGB�� �ִ� �ҷ� �� ã��
			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_R	&&	nImgNum1 != E_IMAGE_CLASSIFY_AVI_G	&&	nImgNum1 != E_IMAGE_CLASSIFY_AVI_B)
			{
				continue;
			}

			////////////////////////////////
			// Max-Mean Ratio Calculation //
			int nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_G && nDefectMaxGV > 250) continue;

			//��ǥ�� -> RGB Pattern�� ��ǥ�� �������� ��
			PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nRangeOffset);
			PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y - 2);
			PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_X + nRangeOffset);
			PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y + 2);

			// ���� ����
			rectTemp.x = PixelStartX;
			rectTemp.y = PixelStartY;
			rectTemp.width = PixelEndX - PixelStartX + 1;
			rectTemp.height = PixelEndY - PixelStartY + 1;

			// RGB �������� White/G64 Pattern �ҷ� ã��
			cv::Mat matTempBufWhite = mWhiteImage(rectTemp);
			cv::Mat matTempBufG64 = mG64Image(rectTemp);

			// ���� �ִ밪 ��� ���� ���Ѵ�
			cv::Scalar tempMean_W, tempStd_W, tempMean_G64, tempStd_G64;

			double minvalue_W = 0;
			double maxvlaue_W = 0;
			double minvalue_G64 = 0;
			double maxvlaue_G64 = 0;

			// White Pattern ���� ���ϱ�
			cv::meanStdDev(matTempBufWhite, tempMean_W, tempStd_W);
			cv::minMaxIdx(matTempBufWhite, &minvalue_W, &maxvlaue_W, NULL, NULL);
			double dImageavg_W = tempMean_W[0];
			double dRatio1 = maxvlaue_W / dImageavg_W;

			// G64 Pattern ���� ���ϱ�
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
			// 			// Top 3 GV�� Max GV �ٽ� ���ϱ�
			// 			NewMaxGVMethold(matTempBufWhite, maxvlaue_W, Newmaxvlaue_W, nTopGVCount);
			// 			NewMaxGVMethold(matTempBufG64, maxvlaue_G64, Newmaxvlaue_G64, nTopGVCount);
			// 			double dNewRatio = (Newmaxvlaue_G64/dImageavg_G64) / (Newmaxvlaue_W/dImageavg_W);
						//////////////////////////////////////////////////////////////////////////

			if (maxvlaue_W >= 200 && maxvlaue_G64 >= 200)
				continue;

			if (dRatio <= dDimpleRatio_RGBPattern)
				//if (dNewRatio <= dDimpleRatio_RGBPattern)
			{
				// i�� ������ Dimple�� �ǽ� Dimple�� Defect_Type ����
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DIMPLE;
				continue;
			}
			else
				continue;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Case 3: White-X G64-X  -> RGB Pattern Dimple Check ���
	// 1. White + G64 Pattern���� Dimple�� ���� ���ϴ� ��ġ ����
	// 2. RGB �ҷ� ��ġ�� �������� �Ͽ� White&G64 Pattern�� ���� Ratio�� ���Ͽ� ���� ��
	// 3. RGB�� ���������� �����ϱ⿡ Ratio�� ũ�� �൵ ������ Active�� Edge ������ �ʴ´�
	//////////////////////////////////////////////////////////////////////////

	if (nGreenDefect > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Point �ҷ��� ��츸...
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
			{
				continue;
			}

			// �ҷ� Pattern�� ã��
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// RGB�� �ִ� �ҷ� �� ã��
			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_G)
			{
				continue;
			}

			//��ǥ�� -> RGB Pattern�� ��ǥ�� �������� ��
			PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nRangeOffset);
			PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y - 2);
			PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_X + nRangeOffset);
			PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y + 2);

			// ���� ����
			rectTemp.x = PixelStartX;
			rectTemp.y = PixelStartY;
			rectTemp.width = PixelEndX - PixelStartX + 1;
			rectTemp.height = PixelEndY - PixelStartY + 1;

			// RGB �������� White/G64 Pattern �ҷ� ã��
			cv::Mat matTempBufWhite = mWhiteImage(rectTemp);
			cv::Mat matTempBufG64 = mG64Image(rectTemp);

			// ���� �ִ밪 ��� ���� ���Ѵ�
			cv::Scalar tempMean_W, tempStd_W, tempMean_G64, tempStd_G64;

			double minvalue_W = 0;
			double maxvlaue_W = 0;
			double minvalue_G64 = 0;
			double maxvlaue_G64 = 0;

			// White Pattern ���� ���ϱ�
			cv::meanStdDev(matTempBufWhite, tempMean_W, tempStd_W);
			cv::minMaxIdx(matTempBufWhite, &minvalue_W, &maxvlaue_W, NULL, NULL);
			double dImageavg_W = tempMean_W[0];
			double dRatio1 = maxvlaue_W / dImageavg_W;

			// G64 Pattern ���� ���ϱ�
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
			// 			// Top 3 GV�� Max GV �ٽ� ���ϱ�
			// 			NewMaxGVMethold(matTempBufWhite, maxvlaue_W, Newmaxvlaue_W, nTopGVCount);
			// 			NewMaxGVMethold(matTempBufG64, maxvlaue_G64, Newmaxvlaue_G64, nTopGVCount);
			// 			double dNewRatio = (Newmaxvlaue_G64/dImageavg_G64) / (Newmaxvlaue_W/dImageavg_W);
						//////////////////////////////////////////////////////////////////////////

			if (maxvlaue_W >= 200 && maxvlaue_G64 >= 200)
				continue;

			if (dRatio <= dDimpleRatio_RGBPattern)
				//if (dNewRatio <= dDimpleRatio_RGBPattern)
			{
				// i�� ������ Dimple�� �ǽ� Dimple�� Defect_Type ����
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DIMPLE;
				continue;
			}
			else
				continue;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// DIMPLE�� Muti BP�� �����ϴ� ��� �߰� 
	//////////////////////////////////////////////////////////////////////////
	if (nMultiBP > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Point �ҷ��� ��츸...
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP)
			{
				continue;
			}

			// �ҷ� Pattern�� ã��
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// RGB�� �ִ� �ҷ� �� ã��
			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_64)
			{
				continue;
			}

			////////////////////////////////
			// Max-Mean Ratio Calculation //
			int nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
			int nDefectMeanGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MeanGV;
			float MaxMeanRatio = (float)nDefectMaxGV / nDefectMeanGV;

			// �����/Dimple�� ���Ͽ� �ӽ÷� Max/Mean Ratio && �ִ� ��� ������ �з� ��
			if (MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
				continue;

			//��ǥ��
			PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nRangeOffset);
			PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y - 2);
			PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_X + nRangeOffset);
			PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y + 2);

			// Ratio �ʱ�ȭ Active Ratio�� �����Ѵ�
			double dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Active;

			// Edge ��ó 100Pixel �̳��� Edge ����
			int nEdgeOffset_X = 90;
			int nEdgeOffset_Y = 100;

			// X ���� Edge �κ� Dimple ���� Ratio�� ������
			if ((PixelStartX - nEdgeOffset_X <= nXStart) || (PixelEndX + nEdgeOffset_X >= nXEnd))
			{
				// ���� Profile�� �������� ��ȯ��
				PixelStartX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X - 2);
				PixelStartY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y - nRangeOffset);
				PixelEndX = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X + 2);
				PixelEndY = (int)(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y + nRangeOffset);

				dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Edge;
			}

			// Y ���� Edge �κ� Dimple ���� Ratio�� ������
			if ((PixelStartY - nEdgeOffset_Y <= nYStart) || (PixelEndY + nEdgeOffset_Y >= nYEnd))
			{
				dDimpleRatio_GrayPattern = dDimpleRatio_GrayPattern_Edge;
			}

			//Rect �׸���
			rectTemp.x = PixelStartX;
			rectTemp.y = PixelStartY;
			rectTemp.width = PixelEndX - PixelStartX + 1;
			rectTemp.height = PixelEndY - PixelStartY + 1;

			// �ҷ� �߽� �������� White/G64 Pattern ���� ���� ������ ȹ��
			cv::Mat matTempBufWhite = mWhiteImage(rectTemp);
			cv::Mat matTempBufG64 = mG64Image(rectTemp);

			// ���� �ִ밪 ��� ���� ���Ѵ�
			cv::Scalar tempMean_W, tempStd_W, tempMean_G64, tempStd_G64;

			double minvalue_W = 0;
			double maxvlaue_W = 0;
			double minvalue_G64 = 0;
			double maxvlaue_G64 = 0;

			// White Pattern ���� ���ϱ�
			cv::meanStdDev(matTempBufWhite, tempMean_W, tempStd_W);
			cv::minMaxIdx(matTempBufWhite, &minvalue_W, &maxvlaue_W, NULL, NULL);
			double dImageavg_W = tempMean_W[0];
			double dRatio1 = maxvlaue_W / dImageavg_W;

			// G64 Pattern ���� ���ϱ�
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
			// 			// Top 3 GV�� Max GV �ٽ� ���ϱ�
			// 			NewMaxGVMethold(matTempBufWhite, maxvlaue_W, Newmaxvlaue_W, nTopGVCount);
			// 			NewMaxGVMethold(matTempBufG64, maxvlaue_G64, Newmaxvlaue_G64, nTopGVCount);
			// 			double dNewRatio = (Newmaxvlaue_G64/dImageavg_G64) / (Newmaxvlaue_W/dImageavg_W);
						//////////////////////////////////////////////////////////////////////////

						// G64�� White���� ���� ��� �ö���� ������ �ҷ����� Continue��
			if (maxvlaue_W >= 200 && maxvlaue_G64 >= 200)
				continue;

			if (dRatio <= dDimpleRatio_GrayPattern)
				//if (dNewRatio <= dDimpleRatio_GrayPattern)
			{
				// i�� ������ Dimple�� �ǽ� Dimple�� Defect_Type ����
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_DIMPLE;
				continue;
			}
			else
				continue;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// Dimple�� �߽� ��ǥ�� ���� �ҷ� ����
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		// Point �ҷ� �ƴϸ� ������
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_DIMPLE)
		{
			continue;
		}

		// �߽� ��ǥ
		CPoint ptSrc1;
		ptSrc1.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X);
		ptSrc1.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y);

		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// ������ ������
			if (i == j)
			{
				j++;
				continue;
			}

			// Point �ҷ� �ƴϸ� ������
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP)
			{
				j++;
				continue;
			}

			// �ҷ� Pattern�� ã��
			int	nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Black Pattern �ҷ��� �ǵ��� �ʴ�
			if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_BLACK)
			{
				j++;
				continue;
			}

			// �ҷ� �߽� ��ǥ�� ���Ѵ�
			CPoint ptSrc2;
			ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X);
			ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y);

			// �ҷ� �߽����� ���ٸ�...
			if (abs(ptSrc1.x - ptSrc2.x) < nOffSet && abs(ptSrc1.y - ptSrc2.y) < nOffSet)
			{
				// ����Ʈ ������ ������
				if (i > j)	 i--;

				// �ٸ� Pattern�� Line ����
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			else j++;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//Dimple�� �Ǿ� �ִ� �ҷ����� ����
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		// Dimple�� �ƴϸ� ������
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE)
		{
			// Dimple�̸� ����
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
		}
		else i++;
	}


	//////////////////////////////////////////////////////////////////////////
	//Green Pattern Retest Point ����
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		// Retest Point ���� Ȯ�� �Ѵ�
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
		{
			// �ҷ� Pattern�� ã��
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Green Pattern�� �ִ� Retest Point �� ����
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
	// ����ó�� Parameter �� ������
	if (dAlignPara == NULL)
		return false;

	// PARAMETER
	double	dblSpotThRatio_Active = (double)(dAlignPara[E_PARA_SISPOT_RATIO_ACTIVE]);	//�� 1�� ����
	double	dblSpotThRatio_Edge = (double)(dAlignPara[E_PARA_SISPOT_RATIO_EDGE]);		//�� 1.04�� ����
	int		nGVCount = (int)(dAlignPara[E_PARA_SISPOT_NUMBER_GVCOUNT]); //GV Count: 7000

	// ����ó�� Dimpe Ratio�� ������
	if (dblSpotThRatio_Active <= 0 || dblSpotThRatio_Edge <= 0 || nGVCount <= 0)
		return false;

	// ����ó�� Defect�� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// Gray Pattern Point Count
	int nGrayDefect = 0;

	//
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		// Point �ҷ��� ��츸 ã��
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
	// ���� �ҷ� Ȯ�� �۾� ver. 0.0ver
	// ����: PNZ/2018/04/19
	// ����: �ҷ��� ���� �������� Ȯ��, �ҷ� ��ǥ������ 
	//////////////////////////////////////////////////////////////////////////
	if (nGrayDefect > 0)
	{
		// G64 Pattern ���� ���� Load
		cv::Mat mG64Image = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_GRAY_64)][0];

		// ROI Region
		int nXStart = m_stThrdAlignInfo.rcAlignCellROI.x;
		int nYStart = m_stThrdAlignInfo.rcAlignCellROI.y;
		int nXEnd = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
		int nYEnd = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// Pattern Ȯ�� �۾�
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			// Point �ҷ��� ��츸 ã��
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

				// �ӽ� Max/Mean Ratio�� �ִ� ������ �з� ��
				if (MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
				{
					i++;
					continue;
				}

				/////////////////////////////////
				// ��ġ Ȯ�� Threshold Ratio ����//
				double	dblSpotThRatio = 0;
				int		nEdgeOffset = 100;

				// Center ��ǥ
				int nDefectCenter_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nDefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				// Edge���� 100Pixel���� Edge Ratio ���
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

				// ���� ��ǥ
				int nCal_X = nDefectCenter_X - nOffset;
				int nCal_Y = nDefectCenter_Y - nOffset;

				// ���� ���� 200x200
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

				// ���� ���� ����
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

				// ���� ������ ū �ҷ��� ����
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

// Dust Pattern�� �̿��Ͽ� ���� �ҷ��� �ҷ������� Ȯ��
bool BumpInspection::DeleteOverlapDefect_DustDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{
	// ����ó�� Parameter �� ������
	if (dAlignPara == NULL)
		return false;

	// PARAMETER
	int nOnOffKey = (int)(dAlignPara[E_PARA_DUST_DARKPOINT_ONOFF]);	//�� 1�� ����

	// 0�̸� ��� ����
	if (nOnOffKey == 0)
		return true;

	// ����ó�� Defect�� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// ALL Pattern Dark Point Count
	int nDarkDefect = 0;

	//
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		// Point �ҷ��� ��츸 ã��
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
		{
			nDarkDefect++;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// ���� �ҷ� ������ Ȯ�� �۾�
	// ����: PNZ/2018/05/17

	if (nDarkDefect > 0)
	{
		// G64 Pattern ���� ���� Load
		cv::Mat mDustImage = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][0];

		// ROI Region
		int nXStart = m_stThrdAlignInfo.rcAlignCellROI.x;
		int nYStart = m_stThrdAlignInfo.rcAlignCellROI.y;
		int nXEnd = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
		int nYEnd = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{

			// Point �ҷ��� ��츸 ã��
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
			{

				////////////////////////////
				// Region Image Selection //

				// Image Offset
				int nOffset = 100;

				// Center ��ǥ
				int nDefectCenter_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nDefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				// ���� ��ǥ
				int nCal_X = nDefectCenter_X - nOffset;
				int nCal_Y = nDefectCenter_Y - nOffset;

				// ���� ���� 200x200
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

				// ���θ� ������ �����Ѵ�
				int nBigRegionUnit = 10;

				// Enhance�� ������ ������
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

				// ���θ� ������ �����Ѵ�
				int nSmallRegionUnit = 70;

				// Enhance�� ������ ������
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
				// Dust ���� Logic

				// ���� ��� �ʱ�ȭ
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

				// ���� ������ ū �ҷ��� ����
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
	// ����ó�� Parameter �� ������
	if (dAlignPara == NULL)
		return false;



	// ����ó�� Defect�� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// Black Pattern Point Count
	int nBlackPointDefect = 0;

	// ����/���� ���� Offset�� ����
	int nBig_Offset = 40;
	int nSmall_Offset = 5;

	// ���� �ʱ�ȭ
	bool bSmallWeakDefect = false;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		// Point �ҷ��� ��츸 ã��
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
			// Point �ҷ��� ��츸 ã��
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			{
				// Pattern ������ ���� Black Pattern�� �ִ� ���� �ҷ��� ã��
				int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
				int	nImgNum1 = theApp.GetImageClassify(nImgNum);

				if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_BLACK) continue;

				// �ҷ��� ��� ������ ����
				int nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;
				int nDefectArea = (int)resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel;

				// ������ �ʰ� ���� �ҷ��� ����
				if ((nDefectMaxGV <= 200) && (nDefectArea <= 100)) continue;

				// ���ҷ��� Center ��ǥ
				int nBig_DefectCenter_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				int nBig_DefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j)
					{
						j++;
						continue;
					}

					// Point �ҷ��� ��츸 ã��
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
					{
						j++;
						continue;
					}

					// Pattern ������ ���� Black Pattern�� �ִ� ���� �ҷ��� ã��
					nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
					nImgNum1 = theApp.GetImageClassify(nImgNum);

					if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_BLACK)
					{
						j++;
						continue;
					}

					// �ҷ��� ��� ������ ����
					nDefectMaxGV = (int)resultPanelData.m_ListDefectInfo[j].Defect_MaxGV;
					nDefectArea = (int)resultPanelData.m_ListDefectInfo[j].Defect_Size_Pixel;

					// ������ �ʰ� ū �ҷ��� ����
					if ((nDefectMaxGV >= 20) && (nDefectArea >= 10))
					{
						j++;
						continue;
					}

					// ���� �ʱ�ȭ
					bSmallWeakDefect = false;

					// ��ҷ��� Center ��ǥ
					int nSmall_DefectCenter_X = (int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;
					int nSmall_DefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y;

					// X/Y ���� �� ������ ���� ������ ����, ���ǿ� �����ϸ� Small Weak Defect���� ����
					if ((abs(nBig_DefectCenter_X - nSmall_DefectCenter_X) <= 5) && (abs(nBig_DefectCenter_Y - nSmall_DefectCenter_Y) <= 40) ||
						(abs(nBig_DefectCenter_X - nSmall_DefectCenter_X) <= 40) && (abs(nBig_DefectCenter_Y - nSmall_DefectCenter_Y) <= 5))
					{
						bSmallWeakDefect = true;
					}

					// Small Weak �����̸� ����
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
	// ����ó�� Parameter �� ������
	if (dAlignPara == NULL)
		return false;

	// ����ó�� Defect�� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	double dTH = dAlignPara[E_PARA_AVI_BLACK_OVERKILL_TEXT];
	int nBlackTH = (int)dAlignPara[E_PARA_AVI_BLACK_OVERKILL_BLACK_GV];
	int nRedTH = (int)dAlignPara[E_PARA_AVI_BLACK_OVERKILL_RED_GV];
	int nGreenTH = (int)dAlignPara[E_PARA_AVI_BLACK_OVERKILL_GREEN_GV];
	int nBlueTH = (int)dAlignPara[E_PARA_AVI_BLACK_OVERKILL_BLUE_GV];

	// �ҷ� �߽ɿ��� �Ÿ� Offset�� ����
	int nSenterOffset = 50;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		// Point �ҷ��� ��츸...
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT) continue;

		// White ������ ��츸...
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_BLACK)	continue;

		// Center ��ǥ
		int nDefectCenter_X = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
		int nDefectCenter_Y = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

		// ���� ��ǥ
		int nCal_X = nDefectCenter_X - nSenterOffset;
		int nCal_Y = nDefectCenter_Y - nSenterOffset;

		// ���� ���� 50x50
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
	// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOnOff = (int)dAlignPara[E_PARA_AVI_POINT_MERGE_ONOFF_TEXT];
	int nOffSet = (int)dAlignPara[E_PARA_AVI_POINT_MERGE_OFFSET];

	// ��� ���Ұ�� ������
	if (nOnOff <= 0)
		return true;
	//////////////////////////////////////////////////////////////////////////
	// KSW 17.10.24 - White���� ���� ���� / Black���� ���� �̽����� ��� => ����
	//////////////////////////////////////////////////////////////////////////

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		// Point �ҷ��� ��츸...
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
		{
			i++;
			continue;
		}

		// White ������ ��츸...
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_WHITE)
		{
			i++;
			continue;
		}

		// �ҷ� �߽� ��ǥ
		CPoint ptSrc1;
		ptSrc1.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio);
		ptSrc1.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

		// White �ҷ� ���
		int nWhiteGV = resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;

		// ���� ���� Ȯ��
		bool bBubble = true;

		// ���� �ҷ�
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// ���� �ҷ� �׸� �� ����
			if (i == j)
			{
				j++;
				continue;
			}

			// Point �ҷ��� ��츸...
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

			// Black ������ ��츸...
			nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
			if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_BLACK)
			{
				j++;
				continue;
			}

			// �ҷ� �߽� ��ǥ
			CPoint ptSrc2;
			ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// �ҷ� �߽����� ���ٸ�...
			if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&
				abs(ptSrc1.y - ptSrc2.y) < nOffSet)
			{
				// �������� ���� ( White - Black �ߺ� )
				bBubble = false;

				// ������
				break;
			}
			// �ߺ� �ȵŸ�... ���� �ҷ�����...
			else j++;
		}

		// White & Black �ߺ��Ǵ� ����̹Ƿ�, ���� ����
		if (!bBubble)
		{
			// White ���� �ҷ� ����
			resultPanelData.m_ListDefectInfo.RemoveAt(i);

			continue;
		}
		// �ߺ��Ǵ� ��찡 �ƴ� ���, ���� ����
		else
		{
			//////////////////////////////////////////////////////////////////////////
			// KSW 17.12.14 - 2�� ���� Ȯ�� start
			// ������ ���� ���, ���� �𿩼� Ȯ�� �Ǵ� ���
			// ���� ���� ��ο�� �� �Ⱥ���
			// �ҷ� GV���� White < Gray �� ��� ��¥ �ҷ��� Ȯ�� ����

			// ���� �ҷ�
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// ���� �ҷ� �׸� �� ����
				if (i == j)
				{
					j++;
					continue;
				}

				// Point �ҷ��� ��츸...
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

				// Gray ������ ��츸...
				nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
				if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_32	&&
					theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_64	&&
					theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_87	&&
					theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_128)
				{
					j++;
					continue;
				}

				// �ҷ� �߽� ��ǥ
				CPoint ptSrc2;
				ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				// �ҷ� �߽����� ���ٸ�...
				if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&
					abs(ptSrc1.y - ptSrc2.y) < nOffSet)
				{
					// Gray �ҷ� ���
					int nGrayGV = resultPanelData.m_ListDefectInfo[j].Defect_MaxGV;

					// White���� Gray ��Ⱑ ��ٸ�...
					if (nWhiteGV <= nGrayGV)
					{
						// �������� ���� ( White - Black �ߺ� )
						bBubble = false;

						// ������
						break;
					}
					// White���� Gray ��Ⱑ ��ο��... ���� �ҷ�����...
					else j++;
				}
				// �ߺ� �ȵŸ�... ���� �ҷ�����...
				else j++;
			}

			// White & Black �ߺ��Ǵ� ����̹Ƿ�, ���� ����
			if (!bBubble)
			{
				// White ���� �ҷ� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(i);

				continue;
			}

			// KSW 17.12.14 - 2�� ���� Ȯ�� end
			//////////////////////////////////////////////////////////////////////////
		}

		// ������ ���� ��ǥ ��� ����
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// ���� �ҷ� �׸� �� ����
			if (i == j)
			{
				j++;
				continue;
			}

			// Point �ҷ��� ��츸...
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

			// �ҷ� �߽� ��ǥ
			CPoint ptSrc2;
			ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// �ҷ� �߽����� ���ٸ�...
			if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&
				abs(ptSrc1.y - ptSrc2.y) < nOffSet)
			{
				if (i > j)   i--;

				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// �ߺ� �ȵŸ�... ���� �ҷ�����...
			else j++;
		}

		// White ���� �ҷ� ����
		resultPanelData.m_ListDefectInfo.RemoveAt(i);
	}

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.01.15 - White���� ���� ���� / ���� ũ�� ���Ͽ� ���� ���� ����
	//////////////////////////////////////////////////////////////////////////
	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		// Point �ҷ��� ��츸...
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_DARK			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
		{
			i++;
			continue;
		}

		// White ������ ��츸...
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_WHITE)
		{
			i++;
			continue;
		}

		// White �ҷ� ����
		CRect rectTemp = CRect(
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - 40,
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - 40,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + 40,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + 40);

		// ���� �ҷ�
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// ���� �ҷ� �׸� �� ����
			if (i == j)
			{
				j++;
				continue;
			}

			// Point �ҷ��� ��츸...
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_DARK			&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_DARK		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_RETEST_POINT_DARK		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK)
			{
				j++;
				continue;
			}

			// �ҷ� �߽� ��ǥ
			CPoint ptSrc;
			ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// ���� �ȿ� �ҷ� ����?
			if (rectTemp.PtInRect(ptSrc))
			{
				if (i > j)   i--;

				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// �ߺ� �ȵŸ�... ���� �ҷ�����...
			else j++;
		}

		// White ���� �ҷ� ����
		resultPanelData.m_ListDefectInfo.RemoveAt(i);
	}


	//////////////////////////////////////////////////////////////////////////
	// ���� & ���� & ��ƼŬ ��
	//////////////////////////////////////////////////////////////////////////

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Point �ҷ��� ��츸...
		if ((resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_POINT_DARK			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_POINT_DARK_SP_3) ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_MULT_BP)	// 17.09.28 �߰�
		{
			// �ҷ� �߽� ��ǥ
			// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
			CPoint ptSrc1;
			ptSrc1.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio);
			ptSrc1.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

			// ���� �ҷ�
			for (int j = i + 1; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// Point �ҷ� �ƴѰ� ����
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK ||
					(resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_MURA_MULT_BP) ||	// 17.09.28 �߰�
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

				// �ҷ� �߽� ��ǥ
				// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
				CPoint ptSrc2;
				ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				// �ҷ� �߽����� ���ٸ�...
				if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&	abs(ptSrc1.y - ptSrc2.y) < nOffSet)
				{
					int nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;

					// KSW 17.12.14 - �ߺ� �� ��, �ٸ� ���Ϻ��� Gray ���� �켱 ����
					if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_GRAY_64)
					{
						ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
						ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp2;
						resultPanelData.m_ListDefectInfo[j] = tTemp1;
					}

					// KSW 17.09.11 - �ߺ� �� ��, �ٸ� ���Ϻ��� Black ���� �켱 ���� ( ���� ��û )
					if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_BLACK)
					{
						ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
						ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp2;
						resultPanelData.m_ListDefectInfo[j] = tTemp1;
					}

					// KSW 17.10.12 - ��Ƽ�ΰ�� �켱 ���� ( ���� ��û )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_MULT_BP)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// 					// ��/������ ���, ���� ����
					// 					else if(	resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT	&&
					// 								resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK	)
					// 					{
					// 						resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK;
					// 					}
					// 					// ��/������ ���, ���� ����
					// 					else if(resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK	&&
					// 							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT	)
					// 					{
					// 						resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK;
					// 					}
										//////////////////////////////////////////////////////////////////////////
										// PNZ 2018-05-08 ���� ��û: Point_Bright_Dark �ҷ� �� ������

					else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK)
					{
					}
					// ��/������ ���, ���� ����
					else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}

					//// ���/������ ���, ���� ����
					//else if(	resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT	&&
					//			resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK		)
					//{
					//	resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK;
					//}
					//// ���/������ ���, ���� ����
					//else if(resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK		&&
					//		resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT	)
					//{
					//	resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK;
					//}
					// ��/������ ���, �ٸ� �ҷ� �����
					else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// Retest�� ��� ����
					else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// Retest�� ��� ����
					else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// KSW 17.07.19 - ���� �켱����
					else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// KSW 17.07.19 - ���� �켱����
					else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
					{
						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp;
					}
					// 					// ��ƼŬ�� ��� �����, �ٸ� �ҷ� �����
					// 					// ���� - ��ƼŬ �� ��� : ��������
					// 					else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_PARTICLE)
					// 					{
					// 						ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
					// 						resultPanelData.m_ListDefectInfo[i] = tTemp;
					// 					}

										// �ش� �ҷ� ����
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
				}
				else j++;  // �ҷ� �߽����� ���� �ʴٸ�...���� �ҷ�����...
			}
		}
	}

	// 	////////////////////////////////////////////////////////////////////////
	// 		// ��ƼŬ - ���� & ���� ��
	// 		//////////////////////////////////////////////////////////////////////////
	// 	
	// 		// �ҷ� ���� ��ŭ
	// 		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	// 		{
	// 			// ��ƼŬ �ҷ� ũ�Ⱑ ũ�Ƿ� �߽��������� ���� �ȵǴ°�� �߻�
	// 			// ��ƼŬ ���ο� �߽� �ִ� ��� ����
	// 			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_PARTICLE)
	// 			{
	// 				// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
	// 				CRect rectTemp = CRect(
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	/ resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_End_X	/ resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);
	// 	
	// 				// ���� �ҷ�
	// 				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
	// 				{
	// 					// ���� �ҷ� �׸� �� ����
	// 					if (i == j)
	// 					{
	// 						j++;
	// 						continue;
	// 					}
	// 	
	// 					// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
	// 					// �ҷ� �߽� ��ǥ
	// 					CPoint ptSrc;
	// 					ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
	// 					ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
	// 	
	// 					// ���� �ȿ� �ҷ� ����?
	// 					if ( rectTemp.PtInRect(ptSrc) )
	// 					{
	// 						if (i > j)   i--;
	// 	
	// 						// �ش� �ҷ� ����
	// 						resultPanelData.m_ListDefectInfo.RemoveAt(j);
	// 					}
	// 					// �ߺ� �ȵŸ�... ���� �ҷ�����...
	// 					else j++;
	// 				}
	// 			}
	// 		}
	// 	
	// 		//////////////////////////////////////////////////////////////////////////
	// 		// ��ƼŬ - ���� & ���� ��
	// 		//////////////////////////////////////////////////////////////////////////
	// 	
	// 		// �ҷ� ���� ��ŭ
	// 		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	// 		{
	// 			// ��ƼŬ �ҷ� ũ�Ⱑ ũ�Ƿ� �߽��������� ���� �ȵǴ°�� �߻�
	// 			// ��ƼŬ ���ο� �߽� �ִ� ��� ����
	// 			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_PARTICLE)
	// 			{
	// 				// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
	// 				CRect rectTemp = CRect(
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	/ resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_End_X	/ resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
	// 					(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);
	// 	
	// 				// ���� �ҷ�
	// 				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
	// 				{
	// 					// ���� �ҷ� �׸� �� ����
	// 					if (i == j)
	// 					{
	// 						j++;
	// 						continue;
	// 					}
	// 	
	// 					// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
	// 					// �ҷ� �߽� ��ǥ
	// 					CPoint ptSrc;
	// 					ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
	// 					ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
	// 	
	// 					// ���� �ȿ� �ҷ� ����?
	// 					if ( rectTemp.PtInRect(ptSrc) )
	// 					{
	// 						if (i > j)   i--;
	// 	
	// 						// �ش� �ҷ� ����
	// 						resultPanelData.m_ListDefectInfo.RemoveAt(j);
	// 					}
	// 					// �ߺ� �ȵŸ�... ���� �ҷ�����...
	// 					else j++;
	// 				}
	// 			}
	// 		}

	return true;
}

bool BumpInspection::DeleteOverlapDefect_Point_Point_RGB(ResultPanelData & resultPanelData, double * dAlignPara)
{
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOnOffP_P = (int)dAlignPara[E_PARA_AVI_POINT_MERGE_ONOFF_TEXT];
	int nOffSet = (int)dAlignPara[E_PARA_AVI_POINT_MERGE_OFFSET];
	//P_P���� ����Ұ�� �̰� ���� ����
	if (nOnOffP_P == 1)
		return true;
	//////////////////////////////////////////////////////////////////////////
	// ���� ���� ��, ���� ��
	//////////////////////////////////////////////////////////////////////////

	// �ҷ� ���� ��ŭ
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
			// ���� ������ ���� Ȯ�� RGB ���ϸ� ����Ҳ���
			int nPattern = theApp.GetImageClassify(nImgNum);
			if (nPattern >= E_IMAGE_CLASSIFY_AVI_BLACK)
				continue;
			// ���� �ҷ�
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// ���� �ҷ� �׸� �� ����
				if (i == j)
				{
					j++;
					continue;
				}
				// ���� ������ �ƴҰ�� �н�
				int nImgNum2 = resultPanelData.m_ListDefectInfo[j].Img_Number;
				if (nImgNum != nImgNum2)
				{
					j++;
					continue;
				}
				// Point �ҷ� �ƴѰ� ����
				if ((resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_POINT_START ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_POINT_END))
				{
					j++;
					continue;
				}

				// �ҷ� �߽� ��ǥ
				CPoint ptSrc;
				ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				// point - Point �ҷ��� �ߺ� ����
				// ���� �ȿ� �ҷ� ����?
				if (rectTemp.PtInRect(ptSrc))
				{
					// ���� ����Ʈ���� ���� ����Ʈ ���� ��...
					if (i > j)	i--;

					// �ش� �ҷ� ����
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
				}
				// �ߺ� �ȵŸ�... ���� �ҷ�����...
				else j++;
			}
		}
	}

	return false;
}


bool BumpInspection::DeleteOverlapDefect_Point_Line(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOffSet = 4;

	//////////////////////////////////////////////////////////////////////////
	// ���� - ���� & ���� & ��ƼŬ ��
	//////////////////////////////////////////////////////////////////////////

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// KSW 17.07.17 - Line �ҷ� �ȿ� Point �ҷ� �ִ� ��� -> Line Alg���� �ϱ�� ��
		// 17.05.09 Line �ҷ� �ȿ� Point �ҷ� �ִ� ���, ���� �ؾ� ��
		// Line �ҷ��� ���
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

			// ���� �ҷ�
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// ���� �ҷ� �׸� �� ����
				if (i == j)
				{
					j++;
					continue;
				}

				// Point �ҷ� �ƴѰ� ����
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_POINT_DARK ||
					(resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_MURA_MULT_BP) ||	// 17.09.28 �߰�
						(resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_MURA_MULT_BP			&&
							resultPanelData.m_ListDefectInfo[j].Defect_Type < E_DEFECT_JUDGEMENT_RETEST_POINT_DARK) ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type > E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT)
				{
					j++;
					continue;
				}

				// �ҷ� �߽� ��ǥ
				CPoint ptSrc;
				ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				// Line - Point �ҷ��� �ߺ� ����
				// ���� �ȿ� �ҷ� ����?
				if (rectTemp.PtInRect(ptSrc))
				{
					// ���� ����Ʈ���� ���� ����Ʈ ���� ��...
					if (i > j)	i--;

					// �ش� �ҷ� ����
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
				}
				// �ߺ� �ȵŸ�... ���� �ҷ�����...
				else j++;
			}
		}
	}

	return true;
}


bool BumpInspection::DeleteOverlapDefect_Point_Mura(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//ljh ���� �䱸 onoff ��� �߰��� �޶�� �� ���� �Ҳ� ����
	int nOnOffP_M = (int)dAlignPara[E_PARA_AVI_POINT_MURA_MERGE_ONOFF_TEXT];
	if (nOnOffP_M == 0)
		return true;
	//////////////////////////////////////////////////////////////////////////
	// KJY 17.09.18 - ���� ��ó ����Ʈ ����
	// �ҷ��� ���� ��� �ӵ��� ���� �� �� �������� ���ʿ��� ����Ʈ ���� ���� ���� �� ���� 
	// ������ ���⳪ EMD���� Ȯ���� �ҷ������� �ش� �ҷ� ���� ���� ũ�� ����Ʈ�� ���� �Ͽ� �ش�.
	//////////////////////////////////////////////////////////////////////////

	if (dAlignPara != NULL)
	{
		int		nDefectTypeM, nDefectTypeS;
		int		nAdjustRange = (int)dAlignPara[E_PARA_STRONG_DEFECT_NOISE_ADJUST_RANGE];
		CRect	rcDeleteArea;
		CPoint	ptCenterSub;

		//�ȼ� �ҷ��϶� ���� ����
		if (nAdjustRange > 0)
		{
			// �ҷ� ���� ��ŭ
			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				nDefectTypeM = resultPanelData.m_ListDefectInfo[i].Defect_Type;

				// ��� �ҷ��� ã�´�.
				if (nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_NUGI ||		// ����
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI ||		// �ܰ� ���� ���� ����
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_ ||		// �ܰ� ���� ���� ����
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_EMP ||		// EMP
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_EMD_BRIGHT ||		// EMD
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_EMD_DARK ||		// EMD					
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_CORNER_CM ||		// ȥ�� 
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_UP_CM ||		// ȥ��
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_FINGER_CM ||		// ȥ��
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH ||		// ��ũ��ġ ( F����... )
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT ||	// 2018-05-08 PNZ Mura �ҷ��̴�
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT
					)
				{
					// P/S ��� ��ǥ ����
					// ũ��� �ڽ��� ���� - ���Ⱑ ������ ������ ������ ���µ� �ټ� ����
					rcDeleteArea.SetRect(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio,
						resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

					// ��ǥ�� ����ȭ
					rcDeleteArea.NormalizeRect();

					// ���� Ȯ��
					rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);

					// �ҷ� ���� ��ŭ
					for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
					{
						// ���� �ҷ� �׸� �� ����
						if (i == j)
						{
							j++;
							continue;
						}

						nDefectTypeS = resultPanelData.m_ListDefectInfo[j].Defect_Type;

						// �ش� �ҷ��� ����Ʈ �ҷ��� �ϳ����� Ȯ��
						if (nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_DARK ||		// ����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||		// ����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY ||		// GRAY����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT_GRAY_WEAK ||		// GRAY �� ����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT_RGB ||		//RGB ����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT ||		// �����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK ||		// �����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||		// �׷����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT ||		// �׷����
							nDefectTypeS == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK ||		// ���� ���
							nDefectTypeS == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT ||		// ���� ���
							nDefectTypeS == E_DEFECT_JUDGEMENT_MURA_MULT_BP ||
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3)

						{
							// P/S ��� ��ǥ ����
							ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
							ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

							// ���� �ȿ� ������ ����
							if (rcDeleteArea.PtInRect(ptCenterSub))
							{
								// Point �ҷ� ����
								resultPanelData.m_ListDefectInfo.RemoveAt(j);

								// ���� ����Ʈ���� ���� ����Ʈ ���� ��...
								if (i > j)	i--;
							}
							else
							{
								j++; // ������ �ʾ��� ��츸 �����ҷ�����
								continue;
							}
						}
						else j++; // ����Ʈ �ҷ��� �ƴ� ��� �����ҷ�����
					}
				}
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// Mura Spot & ������ ��� -> ���� ���� & Mura ����
	//////////////////////////////////////////////////////////////////////////

	int nOffSet = 5;

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Mura ���� Spot �ҷ� �ƴ� ��� ����
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)
			continue;

		CRect rectTemp = CRect(
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
			(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);

		// ���� �ҷ�
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// ���� �ҷ� �׸� �� ����
			if (i == j)
			{
				j++;
				continue;
			}

			// Point �ҷ� �ƴѰ� ����
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT			&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP)
			{
				j++;
				continue;
			}

			// �ҷ� �߽� ��ǥ
			CPoint ptSrc;
			ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// ���� �ȿ� �ҷ� ����?
			if (rectTemp.PtInRect(ptSrc))
			{
				// ���� ����Ʈ���� ���� ����Ʈ ���� ��...
				if (i > j)	i--;

				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// �ߺ� �ȵŸ�... ���� �ҷ�����...
			else j++;
		}
	}

	return true;
}


bool BumpInspection::DeleteOverlapDefect_Line_Mura(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOffSet = 8;

	// ���� ���� �������� ���� ���� ( ���� Ȯ�� )
	int nAdjustRange = 15;

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// ��� �ҷ��� ã�´�.
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

			// ��ǥ�� ����ȭ
			rectLine.NormalizeRect();

			rectLine.InflateRect(nAdjustRange, nAdjustRange);

			// ���� �ҷ�
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// ���� �ҷ� �׸� �� ����
				if (i == j)
				{
					j++;
					continue;
				}

				// �ش� �ҷ��� ����Ʈ �ҷ��� �ϳ����� Ȯ��
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EMD_BRIGHT ||
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EMD_DARK			||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_UP_CM ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_CORNER_CM ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_FINGER_CM ||
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_NUGI			||	// KSW 18.05.21 - ���� ��û : ���� & ���� �ҷ� ���� ����
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BN_CORNER ||
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_STAMPED		||	// KSW 18.05.21 - ���� ��û : ���� & ���� �ҷ� ���� ����
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI		||	// KSW 18.05.21 - ���� ��û : ���� & ���� �ҷ� ���� ����
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_		||	// KSW 18.05.21 - ���� ��û : ���� & ���� �ҷ� ���� ����
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

					if ((rectRes.Width() + rectRes.Height()) > 0) // ���� �ȿ� ������ ����
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(j);

						// ����Ʈ ������ ������..
						if (i > j)	 i--;
					}
					else
					{
						j++; // ������ �ʾ��� ��츸 �����ҷ�����
						continue;
					}
				}
				// �ߺ� �ȵŸ�... ���� �ҷ�����...
				else j++;
			}
		}
	}

	return true;
}


bool BumpInspection::DeleteOverlapDefect_Mura_Mura(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// �ߺ��Ǵ� Mura �ҷ� ����
	//////////////////////////////////////////////////////////////////////////

	int nOffSet = 5;

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Mura �ҷ� �ƴѰ� ���� ( ��ƼBP ���� )
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

		// ���� �ҷ�
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// ���� �ҷ� �׸� �� ����
			if (i == j)
			{
				j++;
				continue;
			}

			// Mura �ҷ� �ƴѰ� ���� ( ��ƼBP ���� )
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

			// �ߺ� ���� Ȯ��
			CRect rectRes;
			IntersectRect(rectRes, rectM, rectS);
			int nArea = 0;
			int nrectM = rectM.Width()*rectM.Height();
			int nrectS = rectS.Width() * rectS.Height();

			CRect rectRes2;
			rectRes2 = rectS & rectM;

			int nrectRes2 = rectRes2.Width() * rectRes2.Height();

			// ���� �ȿ� ������ ����
			if ((rectRes.Width() + rectRes.Height()) > 0 && nrectM > nrectS)
			{
				// ���� & ���� & �Ϻ����� �켱������ �����
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||	// ����
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT ||	// ����
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_NUGI ||	// ����
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI ||	// ���� ����
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_ ||	// ���� ����
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_STAMPED)	// �Ϻ� ����
				{
					ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
					ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
					resultPanelData.m_ListDefectInfo[i] = tTemp2;
					resultPanelData.m_ListDefectInfo[j] = tTemp1;

					// ���� �� ����
					rectM = CRect(
						(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
						(resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio) - nOffSet,
						(resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet,
						(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio) + nOffSet);
				}

				// ���� ����Ʈ���� ���� ����Ʈ ���� ��...
				if (i > j)	i--;

				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// �ߺ� �ȵŸ�... ���� �ҷ�����...
			else j++;
		}
	}

	return true;
}


// Casting - stDefectInfo���� �ʿ��� �κ� �����Ͽ� ResultPanelData�� ����
bool BumpInspection::GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio)
{
	// AVI�� ȸ������ ���� ���� ���� ��ǥ�� ����ϹǷ� ���� ���� �� ��ũ��ǥ ��� ���� ���� ��ǥ�� ȸ���ؼ� �Ѱ���� ��
	_tcscpy_s(pResultDefectInfo->Defect_Code, _T("CODE"));
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_TOP] = cv::Point(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_TOP] = cv::Point(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM] = cv::Point(pResultBlob->ptRB[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_BOTTOM] = cv::Point(pResultBlob->ptLB[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].y);

	cv::Point ptRotate(0, 0);

	// AD �ƴ� �ҷ���...
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
	// AD �ҷ��� ��� ȸ������ �ʰ� ��ü ��ǥ �״�� ��ȯ
	else
	{
		pResultDefectInfo->Pixel_Start_X = pResultBlob->ptLT[nBlobCnt].x;
		pResultDefectInfo->Pixel_Start_Y = pResultBlob->ptLT[nBlobCnt].y;
		pResultDefectInfo->Pixel_End_X = pResultBlob->ptRB[nBlobCnt].x;
		pResultDefectInfo->Pixel_End_Y = pResultBlob->ptRB[nBlobCnt].y;
	}

	pResultDefectInfo->Pixel_Center_X = (int)(pResultDefectInfo->Pixel_Start_X + pResultDefectInfo->Pixel_End_X) / 2;
	pResultDefectInfo->Pixel_Center_Y = (int)(pResultDefectInfo->Pixel_Start_Y + pResultDefectInfo->Pixel_End_Y) / 2;

	// ������ ���� �̹��� ���� Crop �� �ҷ� ���� ����
	pResultDefectInfo->Pixel_Crop_Start_X = min(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].x);
	pResultDefectInfo->Pixel_Crop_Start_Y = min(pResultBlob->ptLT[nBlobCnt].y, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Pixel_Crop_End_X = max(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].x);
	pResultDefectInfo->Pixel_Crop_End_Y = max(pResultBlob->ptLB[nBlobCnt].y, pResultBlob->ptRB[nBlobCnt].y);

	// BOE Eng'r ��û Defect Size
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
	pResultDefectInfo->Img_Size_X = (DOUBLE)(pResultDefectInfo->Pixel_Crop_End_X - pResultDefectInfo->Pixel_Crop_Start_X);		// �ӽ� �ҷ� ���� ���� ũ��
	pResultDefectInfo->Img_Size_Y = (DOUBLE)(pResultDefectInfo->Pixel_Crop_End_Y - pResultDefectInfo->Pixel_Crop_Start_Y);		// �ӽ� �ҷ� ���� ���� ũ��
	pResultDefectInfo->Defect_Type = pResultBlob->nDefectJudge[nBlobCnt];
	pResultDefectInfo->Pattern_Type = pResultBlob->nPatternClassify[nBlobCnt];
	pResultDefectInfo->Camera_No = nCameraNum;
	pResultDefectInfo->nRatio = nRatio;

	// KSW 17.12.04 - (���� + ����) / 2 -> Spec �߰� ( ���� ��û )
	pResultDefectInfo->dDimension = (pResultBlob->dF_Max[nBlobCnt] + pResultBlob->dF_Min[nBlobCnt]) / 2.0;

#if USE_ALG_CONTOURS
	// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
	memcpy(pResultDefectInfo->nContoursX, pResultBlob->nContoursX[nBlobCnt], sizeof(int) * MAX_CONTOURS);
	memcpy(pResultDefectInfo->nContoursY, pResultBlob->nContoursY[nBlobCnt], sizeof(int) * MAX_CONTOURS);
#endif

	return true;
}

bool BumpInspection::AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner)
{
	// ȸ���� ��� Left-Top ������ ��ǥ ����
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP], ptAdjCorner[E_CORNER_LEFT_TOP], pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP], ptAdjCorner[E_CORNER_RIGHT_TOP], pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM], ptAdjCorner[E_CORNER_RIGHT_BOTTOM], pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM], ptAdjCorner[E_CORNER_LEFT_BOTTOM], pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Align �ϱ� ��, AVI & SVI ���� �̻� Ȯ��
//////////////////////////////////////////////////////////////////////////
long BumpInspection::CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio)
{
	// test
	CCPUTimer tact;
	tact.Start();

	// ���� �˰��� �˻� �Ķ���� ��������
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// ��� Ÿ��
	int nEqpType = theApp.m_Config.GetEqpType();

	// ���� ���� Ʋ���ų�, ������ �ȵȰ�� AD �ҷ��̹Ƿ� �˻� ���ص� ��. ( Align ���� ���� )
	long nErrorCode = Align_FindDefectAD(MatOrgImage, dAlgPara, dResult, nRatio, nCameraNum, nEqpType);
	//long nErrorCode = E_ERROR_CODE_TRUE;

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Inspect AD : %.2f"), tact.Stop(false) / 1000.);

	return nErrorCode;
}

// Repair ���񿡼� ����ϴ� ��ǥ�� �� �ڵ� ����
bool BumpInspection::JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo)
{
	//Line�� �ܰ��� �پ��ִ��� Ȯ�ο�
	//Cell Corner �޾ƿ��� 
	//[0] = LeftTop
	//[1] = RightTop			�Ⱦ� �ּ�ó��
	//[2] = RightBottom
	//[3] = LeftBottom			�Ⱦ� �ּ�ó��

	int		nWorkOriginPosition = -1;			// 0 : LT, 1 : RT, 2 : RB, 3 : LB
	int		nWorkDirection = -1;				// 0 : X = Width, 1 : Y = Width

	WrtResultInfo.GetWorkCoordUsingRepair(nWorkOriginPosition, nWorkDirection);

	cv::Point ptCornerTemp[4];  // Cell Corner ����� Align ��
	cv::Point ptRotate(0, 0);   // Align ��� ���� ����
	cv::Point ptAlignCorner[4]; // Align �� Cell Corner


	ptAlignCorner[0].x = m_stThrdAlignInfo.rcAlignCellROI.x;
	ptAlignCorner[0].y = m_stThrdAlignInfo.rcAlignCellROI.y;

	ptAlignCorner[2].x = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
	ptAlignCorner[2].y = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

	int nDefectCount = (int)resultPanelData.m_ListDefectInfo.GetCount();

	// ���� ������
	int nRepairOffSet = 20;

	// ���� ������ ���� ���Ժ��� ũ�Գ���
	// �˰��� ���� �ʿ�
	// �ӽ÷� offset
	int nDefectRectOffSet = 6;

	// �������� for�� defect  �������
	for (int i = 0; i < nDefectCount; i++)
	{
		int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_POINT_DARK			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK)	// POINT �� ���
		{
			resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
			resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
			resultPanelData.m_ListDefectInfo[i].bUseReport = true;
		}

		// Line Defect repair ���� Ȯ�ο�
		// Line Defect�� ���
		else if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_LINE_X_BRIGHT			&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
		{
			int nStart_X = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
			int nStart_Y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
			int nEnd_X = resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
			int nEnd_Y = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;

			// Cell ũ�⺸�� ũ�� ���� �ٱ��� �ִ� ��� ���� �ִ� �ּҰ� ����
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

			// DGS�� Line Point �ƴ� ���
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_DGS		&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_XLINE_SPOT	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_YLINE_SPOT)
			{
				// X Line Defect�� ���		
				if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN)
				{
					// ���ʸ� �پ� �ִ°�� DO, L-GO, R-GO
					// X Line �̸鼭 �����̶� Cell ���ʿ� �پ��ִ� ���
					if (ptAlignCorner[0].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_X && resultPanelData.m_ListDefectInfo[i].Pixel_Start_X <= ptAlignCorner[0].x + nRepairOffSet ||
						ptAlignCorner[2].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_X   && resultPanelData.m_ListDefectInfo[i].Pixel_End_X <= ptAlignCorner[2].x + nRepairOffSet)
					{
						// X-Line, Y-Line
						// X Line �̸鼭 ���� �� Cell ���ʿ� �پ��ִ� ���
						if (ptAlignCorner[0].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_X && resultPanelData.m_ListDefectInfo[i].Pixel_Start_X <= (ptAlignCorner[0].x + nRepairOffSet) &&
							ptAlignCorner[2].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_X && resultPanelData.m_ListDefectInfo[i].Pixel_End_X <= ptAlignCorner[2].x + nRepairOffSet)
						{
							// Work ������ �ٲ�� �����ؾ��ϴ� ��ġ�� �޶���
							// 
							switch (nWorkOriginPosition)
							{
							case E_CORNER_LEFT_TOP:
							case E_CORNER_LEFT_BOTTOM:
								// Defect ���� ��ǥ �Է�
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = ptAlignCorner[0].x; // ���� ó�� x ��ǥ
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
								break;
							case E_CORNER_RIGHT_TOP:
							case E_CORNER_RIGHT_BOTTOM:
								// Defect ������ ��ǥ �Է�
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = ptAlignCorner[2].x; // ���� �� x ��ǥ
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
								break;
							}
						}
						else
						{
							// DO, L-GO
							// X Line �̸鼭 Line�� Cell ���ʿ� �پ��ִ� ���
							// Left Corner X ��ǥ���� OffSet �������� ���� ��
							if (ptAlignCorner[0].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_X			&&
								resultPanelData.m_ListDefectInfo[i].Pixel_Start_X <= ptAlignCorner[0].x + nRepairOffSet)
							{
								// Defect ������ ��ǥ �Է�
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = resultPanelData.m_ListDefectInfo[i].Pixel_End_X - nDefectRectOffSet; // ���� �� x ��ǥ
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0; // y �߰� ��ǥ
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
							}
							// DO, R-GO
							// X Line �̸鼭 Line�� Cell �����ʿ� �پ��ִ� ���
							// Right Corner X ��ǥ���� OffSet �������� ���� ��
							if (ptAlignCorner[2].x - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_X	 &&
								resultPanelData.m_ListDefectInfo[i].Pixel_End_X <= ptAlignCorner[2].x + nRepairOffSet)
							{
								// Defect ���� ��ǥ �Է�
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + nDefectRectOffSet; // ���� ���� ��ǥ
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0; // y �߰� ��ǥ
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
							}
						}
					}
					// Cell ���ʿ� ��� ���� �پ����� �ʴ°�� Defect �߽� ��ǥ
					else
					{
						resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
						resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
						resultPanelData.m_ListDefectInfo[i].bUseReport = true;

					}
				}
				//Y Line�� ���
				else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT ||
					resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT)
				{
					// ���ʸ� �پ� �ִ°�� DO, L-GO, R-GO
					// Y Line �̸鼭 �����̶� Cell ���ʿ� �پ��ִ� ���
					if (ptAlignCorner[0].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y && resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y <= ptAlignCorner[0].y + nRepairOffSet ||
						ptAlignCorner[2].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_Y   && resultPanelData.m_ListDefectInfo[i].Pixel_End_Y <= ptAlignCorner[2].y + nRepairOffSet)
					{
						// Y-Line 
						// Y Line �̸鼭 ���� �� Cell ���ʿ� �پ��ִ� ���
						if (ptAlignCorner[0].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y && resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y <= ptAlignCorner[0].y + nRepairOffSet &&
							ptAlignCorner[2].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_Y  && resultPanelData.m_ListDefectInfo[i].Pixel_End_Y <= ptAlignCorner[2].y + nRepairOffSet)
						{
							// Work ������ �ٲ�� �����ؾ��ϴ� ��ġ�� �޶���
							// 
							switch (nWorkOriginPosition)
							{
							case E_CORNER_LEFT_TOP:
							case E_CORNER_RIGHT_TOP:
								// Defect ���� ��ǥ �Է�
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = ptAlignCorner[0].y; // ���� �� y ��ǥ
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
								break;
							case E_CORNER_LEFT_BOTTOM:
							case E_CORNER_RIGHT_BOTTOM:
								// Defect �Ʒ��� ��ǥ �Է�
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;;
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = ptAlignCorner[2].y; // ���� �� y ��ǥ
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
								break;
							}
						}
						else
						{
							// DO, L-GO
							// Y Line �̸鼭 Line�� Cell ���ʿ� �پ��ִ� ���
							if (ptAlignCorner[0].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y			&&
								resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y <= ptAlignCorner[0].y + nRepairOffSet)
							{
								// Defect �Ʒ��� ��ǥ �Է�
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0; // x �߰� ��ǥ
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - nDefectRectOffSet; // ���� �� y ��ǥ
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
							}
							// DO, R-GO
							// Y Line �̸鼭 Line�� Cell �Ʒ��ʿ� �پ��ִ� ���
							if (ptAlignCorner[2].y - nRepairOffSet <= resultPanelData.m_ListDefectInfo[i].Pixel_End_Y	 &&
								resultPanelData.m_ListDefectInfo[i].Pixel_End_Y <= ptAlignCorner[2].y + nRepairOffSet)
							{
								// Cell ���� ��ǥ �Է�
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0; // x �߰� ��ǥ
								resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + nDefectRectOffSet; // ���� ���� Y ��ǥ
								resultPanelData.m_ListDefectInfo[i].bUseReport = true;
							}
						}
					}
					else// Line�� �ܰ��ʿ� ���� ������ �پ����� �ʴ� ���
					{
						resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
						resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
						resultPanelData.m_ListDefectInfo[i].bUseReport = true;
					}
				}
				else //Line�� �ƴ� ��� // ���� �� �ִ� ��찡 ������.
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
			// DGS�� ���
			else if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS)
			{
				resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
				resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
				resultPanelData.m_ListDefectInfo[i].bUseReport = true;
			}

			// Sub ����
			else
			{
				resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
				resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
				resultPanelData.m_ListDefectInfo[i].bUseReport = true;
			}
		}// Line Defect Repair ��

		// MURA �� ��Ÿ �ҷ�
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
	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// �ֺ� ��
	int nOffSet = 4;

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.03.30 - G64���� ���� ������ ��� ��Ȯ���Ͽ� ���� ����
	//////////////////////////////////////////////////////////////////////////

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// ���� & ���� �ҷ��� ��츸...
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT		&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_POINT_BRIGHT)
			continue;

		// G87 ������ ��츸... ���� �˻�
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_87)
			continue;

		// �ҷ� �߽� ��ǥ
		CPoint ptSrc1;
		ptSrc1.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio);
		ptSrc1.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

		// ���� �ҷ�
		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// ���� �ҷ� �׸� �� ����
			if (i == j)
			{
				j++;
				continue;
			}

			// G64 ������ ��츸...
			nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
			if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_64)
			{
				j++;
				continue;
			}

			// Mura �˻� �ҷ��� ��츸...
			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL		&&
				resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG)
			{
				j++;
				continue;
			}

			// �ҷ� �߽� ��ǥ
			CPoint ptSrc2;
			ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// �ҷ� �߽����� ���ٸ�...
			if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&
				abs(ptSrc1.y - ptSrc2.y) < nOffSet)
			{
				// ������ ���, ��������
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL)
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				// ������ ��� ����
				else
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL;

				break;
			}
			// �ߺ� �ȵŸ�... ���� �ҷ�����...
			else j++;
		}
	}

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	{
		// Mura �˻� �ҷ��� ��츸...
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL		&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG)
		{
			i++;
			continue;
		}

		// �ش� �ҷ� ����
		resultPanelData.m_ListDefectInfo.RemoveAt(i);
	}

	return true;
}

bool BumpInspection::JudgementDelReport(ResultPanelData& resultPanelData)
{
	// �ҷ� ���� ��ŭ
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
//��	 ��: ���� Pattern �� Line �ֺ� Line Defect ����												//  
//�۾���: 2017-09-18																				// 
//�۾���: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementDeletLineDefect(ResultPanelData& resultPanelData, double* dAlignPara)
{
	int nLineNumber = 0;	// ��¥ ���� �ҷ�
	int nLineDefect = 0;	// ���� ����
	int nOffSet = 50;	// �߽��� �Ÿ� ����
	int nWidthofLine = 45;	// ������߿��� �ش� ���⺸�� ������ ����

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// ��¥ Line�� �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
			nLineNumber++;	// ��¥ ����
			break;

			// Line �ҷ��� ��� ����
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
			nLineDefect++;	// �����
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// ������ �ֺ� ����� ����
	//////////////////////////////////////////////////////////////////////////

	// Delete the line defect around the Strong Line
	// �Ѵ� ��� �����ϴ� ���
	if (nLineNumber > 0 && nLineDefect > 0)
	{
		//////////////////////////////////////////////////////////////////////////
		// x ����
		//////////////////////////////////////////////////////////////////////////

		// �ҷ� ����
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� ( ������ )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����θ�... )
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
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

					// ���� ���� �ƴϸ� �ٸ���
					if (X1DefectImageType != X2DefectImageType) { j++; continue; }

					// ������ �ֺ��� ����� �ִ� ���
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// ����Ʈ ������ ������..
						if (i > j)	 i--;

						// ����� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
					// ������ �ֺ��� ����� ���� ���
					else  j++;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// y ����
		//////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Y ���� ( ������ )
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
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// Y ���� �ҷ� �ƴѰ� ���� ( ����θ�... )
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

					// ���� ���� �ƴϸ� �ٸ���
					if (Y1DefectImageType != Y2DefectImageType) { j++; continue; }

					// ������ �ֺ��� ����� �ִ� ���
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// ����Ʈ ������ ������..
						if (i > j) i--;

						// ����� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
					// ������ �ֺ��� ����� ���� ���
					else  j++;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// B���� �ֺ� D���� ���� ���� ��� ( x���� )
	// D���� �ֺ� B���� ���� ���� ��� ( x���� )
	//////////////////////////////////////////////////////////////////////////

	// y���� ���� �ʿ�

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// X ����
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
		{
			CPoint	X1St, X1End, X1Center;
			int		X1DefectImageType;
			X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
			X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
			X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
			X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// ���� �ҷ� �� ����
				if (i == j)
				{
					j++; continue;
				}

				// X ���� �ƴѰ�
				if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_BRIGHT  &&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK)
				{
					j++; continue;
				}


				if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT &&		// i : ������
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK) ||		// j : ��ο��

					(resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT &&		// j : ������
						resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK))		// i : ��ο��
				{
					CPoint	X2St, X2End, X2Center;
					int		X2DefectImageType;
					X2St.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2End.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio;
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;

					// ���� ���� �ƴϸ� �ٸ���
					if (X1DefectImageType != X2DefectImageType) { j++; continue; }

					if ((abs(X2St.y - X2End.y) >= nWidthofLine) &&		// �� �β��� �������� ũ��
						(abs(X1Center.y - X2Center.y) >= nOffSet))		// �߽� ��ǥ�� ����Ѱ��
					{
						// ����Ʈ ������ ������..
						if (i > j) i--;

						// ���� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
					else j++;
				}
				else j++;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// ������ & ����� ��ī��Ʈ
	//////////////////////////////////////////////////////////////////////////

	// ���� ������ �ʱ�ȭ
	nLineNumber = 0;
	nLineDefect = 0;

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// ��¥ Line�� �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
			nLineNumber++;
			break;

			// Line �ҷ��� ��� ����
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
		{
			// Norch ��� �� �� Ȯ��
			int NORCH_ONOFF = (int)(dAlignPara[E_PARA_NORCH_OVERKILL_ONOFF]);

			if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT) &&
				NORCH_ONOFF == 1)
			{
				// ���� �� Norch ���� ���� �޾� ����
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
	// ����� �ҷ� 15�� �̻��� ��� : Retest ����
	//////////////////////////////////////////////////////////////////////////

	// ����� 15�� �̻��� ���
	if (nLineDefect > 15)
	{
		int nBuff = 0;

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// ����� �϶�
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
			{
				// ����� ����
				nBuff++;

				// ù��° ����� : Retest ���� ����
				if (nBuff == 1) { resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_LINE_BRIGHT; }

				// ������ ����� ����
				if (nBuff > 1) { resultPanelData.m_ListDefectInfo.RemoveAt(i); }
			}
			else i++;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//��	 ��: ���� Pattern �� ���� ������ �ҷ��� �� �ҷ����� Merge											//  
//�۾���: 2017-11-14																				//
//�۾���: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementSamePatternDefectMerge(ResultPanelData& resultPanelData)
{
	int nLineNumber = 0;	// ��¥ ���� �ҷ�
	int nSSLineNumber = 0;	// ���� Pattern, ���� ���⿡ �ҷ�
	int nOffSet = 50;	// �߽��� �Ÿ� ����

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// ��¥ Line�� �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
			nLineNumber++;	// ��¥ ����
			break;
		}
	}

	// ���� Pattern �ѹ��⿡ �ִ� �ҷ��� ���� ��
	if (nLineNumber > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ����
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
			{
				CPoint	X1Center, X1Length;
				int		X1DefectImageType, X1DefectLength;
				int nDefectCount = (int)resultPanelData.m_ListDefectInfo.GetCount();
				X1Length.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;	// ���� ������
				X1Length.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;	// ���� ����
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio; // Center��ġ
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����
				X1DefectLength = abs(X1Length.x - X1Length.y);								// ���� ����

				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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

					X2Length.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;	// ���� ������
					X2Length.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;	// ���� ����
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio; // Center��ġ
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����
					X2DefectLength = abs(X2Length.x - X2Length.y);								// ���� ����

																								// ���� Pattern�� ����
					if (X1DefectImageType != X2DefectImageType) {
						if (abs(X1Center.y - X2Center.y) <= nOffSet) {
							resultPanelData.m_ListDefectInfo[i].Pixel_Start_X = min(X1Length.x, X2Length.x) * resultPanelData.m_ListDefectInfo[i].nRatio;
							resultPanelData.m_ListDefectInfo[i].Pixel_End_X = max(X1Length.y, X2Length.y) * resultPanelData.m_ListDefectInfo[i].nRatio;

							// ����Ʈ ������ ������
							if (i > j)	 i--;

							// �ٸ� Pattern�� Line ����
							resultPanelData.m_ListDefectInfo.RemoveAt(j);
							continue;
						}
					}

					// ���� ����
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// 						if ( X1DefectLength < X2DefectLength)
						// 						{
						// 							resultPanelData.m_ListDefectInfo[j].Pixel_Start_X = min(X1Length.x, X2Length.x);
						// 							resultPanelData.m_ListDefectInfo[j].Pixel_End_X   = max(X1Length.y, X2Length.y);
						// 
						// 							// ����Ʈ ������ ������
						// 							if( i < j )	 j--;
						// 
						// 							// �ٸ� Pattern�� Line ����
						// 							resultPanelData.m_ListDefectInfo.RemoveAt(i);
						// 						}
						// 
						// 						if ( X1DefectLength > X2DefectLength)
						// 						{
						resultPanelData.m_ListDefectInfo[i].Pixel_Start_X = min(X1Length.x, X2Length.x) * resultPanelData.m_ListDefectInfo[i].nRatio;
						resultPanelData.m_ListDefectInfo[i].Pixel_End_X = max(X1Length.y, X2Length.y) * resultPanelData.m_ListDefectInfo[i].nRatio;

						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� Line ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
						//}
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Y ����
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
			{
				CPoint	Y1Center, Y1Length;
				int		Y1DefectImageType, Y1DefectLength;

				Y1Length.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;	// ���� ������
				Y1Length.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;	// ���� ����
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio; // Center��ġ
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����
				Y1DefectLength = abs(Y1Length.x - Y1Length.y);								// ���� ����

				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DARK			&&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT  &&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
					{
						j++; continue;
					}

					CPoint	Y2Center, Y2Length;
					int		Y2DefectImageType, Y2DefectLength;

					Y2Length.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;	// ���� ������
					Y2Length.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;	// ���� ����
					Y2Center.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio; // Center��ġ
					Y2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����
					Y2DefectLength = abs(Y2Length.x - Y2Length.y);								// ���� ����

																								// ���� Pattern�� ����
					if (Y1DefectImageType != Y2DefectImageType) {
						if (abs(Y1Center.x - Y2Center.x) <= nOffSet) {
							resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y = min(Y1Length.x, Y2Length.x) * resultPanelData.m_ListDefectInfo[i].nRatio;
							resultPanelData.m_ListDefectInfo[i].Pixel_End_Y = max(Y1Length.y, Y2Length.y) * resultPanelData.m_ListDefectInfo[i].nRatio;

							// ����Ʈ ������ ������
							if (i > j)	 i--;

							// �ٸ� Pattern�� Line ����
							resultPanelData.m_ListDefectInfo.RemoveAt(j);
							continue;
						}
					}

					// ���� ����
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// 						if ( X1DefectLength < X2DefectLength)
						// 						{
						// 							resultPanelData.m_ListDefectInfo[j].Pixel_Start_X = min(X1Length.x, X2Length.x);
						// 							resultPanelData.m_ListDefectInfo[j].Pixel_End_X   = max(X1Length.y, X2Length.y);
						// 
						// 							// ����Ʈ ������ ������
						// 							if( i < j )	 j--;
						// 
						// 							// �ٸ� Pattern�� Line ����
						// 							resultPanelData.m_ListDefectInfo.RemoveAt(i);
						// 						}
						// 
						// 						if ( X1DefectLength > X2DefectLength)
						// 						{
						resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y = min(Y1Length.x, Y2Length.x) * resultPanelData.m_ListDefectInfo[i].nRatio;
						resultPanelData.m_ListDefectInfo[i].Pixel_End_Y = max(Y1Length.y, Y2Length.y) * resultPanelData.m_ListDefectInfo[i].nRatio;

						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� Line ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
						//}
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}
	}
	return true;
}

bool BumpInspection::JudgeMultLine(ResultPanelData & resultPanelData)
{
	int nBrightLineXNumber = 0;	// ��¥ ���� �ҷ�
	int nBrightLineYNumber = 0;	// ��¥ ���� �ҷ�
	int nDarkLineXNumber = 0;	// ��¥ ���� �ҷ�
	int nDarkLineYNumber = 0;	// ��¥ ���� �ҷ�

						// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// ��¥ Line�� �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
			nBrightLineXNumber++;	// ��¥ ����
			break;
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
			nBrightLineYNumber++;	// ��¥ ����
			break;
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
			nDarkLineXNumber++;	// ��¥ ����
			break;
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
			nDarkLineYNumber++;	// ��¥ ����
			break;
			//case E_DEFECT_JUDGEMENT_LINE_X_OPEN:			
			//	nBrightLineXNumber++;	// ��¥ ����
			//	nDarkLineXNumber++;	// ��¥ ����
			//	break;
			//case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
			//case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
			//	nDarkLineYNumber++;	// ��¥ ����
			//	nBrightLineYNumber++;	// ��¥ ����
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
				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_BRIGHT_MULT;
			}
			else i++;  // ���� �ҷ�����...
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
				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT_MULT;
			}
			else i++;  // ���� �ҷ�����...
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
				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_X_DARK_MULT;
			}
			else i++;  // ���� �ҷ�����...
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
				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_LINE_Y_DARK_MULT;
			}
			else i++;  // ���� �ҷ�����...
		}
	}
	return true;
}

bool BumpInspection::PatternPointBrightClassify(ResultPanelData & resultPanelData)
{

	int nDefectNum = (int)resultPanelData.m_ListDefectInfo.GetCount();

	int nOffset = 10;

	// �ҷ� Ȯ��
	for (int i = 0; i < nDefectNum; i++)
	{
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		// ���� ������ ���� Ȯ��
		int nPattern = theApp.GetImageClassify(nImgNum);

		//// ���� �ҷ� Ȯ��
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT)
		{
			int nCountOverlap = 0;
			// �ҷ� �߽� ��ǥ
			CPoint ptFirst, ptSecond;
			ptFirst.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio);
			ptFirst.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

			for (int j = 0; j < nDefectNum; j++)
			{
				// ���� �ҷ� �Ѿ��
				if (i == j)
				{
					continue;
				}

				// �ҷ� �߽� ��ǥ
				ptSecond.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSecond.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				int nTwoPointDistance = (int)sqrt(pow(ptFirst.x - ptSecond.x, 2.0) + pow(ptFirst.y - ptSecond.y, 2.0));

				// ������ġ�� �ҷ��� �����ϴ��� Ȯ��
				if (nTwoPointDistance <= nOffset)
				{
					if (resultPanelData.m_ListDefectInfo[i].Img_Number != resultPanelData.m_ListDefectInfo[j].Img_Number)
						nCountOverlap++;
				}
			}
			// ���� ��ġ�� �ҷ��� �������� ������ ������ Ȯ�� �� 
			// Ư�� ��ġ���� �����ϴ� �ҷ��� ���� 
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

		//// �� ���� �ҷ� Ȯ��
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT)
		{
			int nCountOverlap = 0;
			// �ҷ� �߽� ��ǥ
			CPoint ptFirst, ptSecond;
			ptFirst.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio);
			ptFirst.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

			for (int j = 0; j < nDefectNum; j++)
			{
				// ���� �ҷ� �Ѿ��
				if (i == j)
				{
					continue;
				}

				// �ҷ� �߽� ��ǥ
				ptSecond.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSecond.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				int nTwoPointDistance = (int)sqrt(pow(ptFirst.x - ptSecond.x, 2.0) + pow(ptFirst.y - ptSecond.y, 2.0));

				// ������ġ�� �ҷ��� �����ϴ��� Ȯ��
				if (nTwoPointDistance <= nOffset)
				{
					if (resultPanelData.m_ListDefectInfo[i].Img_Number != resultPanelData.m_ListDefectInfo[j].Img_Number)
						nCountOverlap++;
				}
			}
			// ���� ��ġ�� �ҷ��� �������� ������ ������ Ȯ�� �� 
			// Ư�� ��ġ���� �����ϴ� �ҷ��� ���� 
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
//��	 ��: Black Pattern ���� �ֺ� Bright Line Merge												//  
//�۾���: 2017-10-21																				//
//       17.10.30 ���� ���: Black Pattern ������ Dark Line ����									//
//�۾���: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementBlackPatternMerge(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// ��¥ ���� �ҷ�
	int nYLineNumber = 0;	// ��¥ ���� �ҷ�
	int nOffSet = 50;	// �߽��� �Ÿ� ����

	// �ҷ� ������ ���� �κ�
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// ��¥ X Line �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_DGS_X:
		case E_DEFECT_JUDGEMENT_LINE_PCD_CRACK:
			nXLineNumber++;	// ��¥ ����
			break;

			// ��¥ Y Line �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
		case E_DEFECT_JUDGEMENT_LINE_DGS_Y:
			nYLineNumber++;	// ��¥ ����
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// x ����
	//////////////////////////////////////////////////////////////////////////

	if (nXLineNumber > 0) // X Line�� ������ ���ؾ� ��
	{
		/*
		//����
		int	X1DefectPattern  = 0;
		int X1FistPattern	 = 0; // ���� ���� ������ Pattern ��~
		int X1InitialPattern = 20;
		//õ�� �߰� �ڵ�
		*/
		// �� �ҷ� �߻� Pattern �켱 ���� Black > Red > Green > Blue (Pattern Number�� ������� �������� �ʴ� ��� ����)
		/*
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� ���� ���� ���� ���� Pattern�� ���� Pattern��
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT)
			{
				CPoint	X1St, X1End, X1Center;
				X1DefectPattern = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����
				X1FistPattern = min(X1InitialPattern, X1DefectPattern);
				X1InitialPattern = X1FistPattern;
			}
		}
		*/

		//�� �� �������� 
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

				// ���ϰ� ���� Pattern �̿��� �Ѵ�..��
				//if (X1DefectImageType != X1FistPattern) continue;

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

					// ���� Pattern�� ������ �ȵ� �ٸ� Pattern�� ����
					//if ( X1DefectImageType == X2DefectImageType ){ j++; continue; }

					// �ֺ��� Line�� ���� �Ǵ� ���
					//if ( (X1St.y <= X2Center.y) && (X1End.y >= X2Center.y) )
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}

		//õ������ ��� ���ϴ� ��� ���� ���� ���� Ȯ��
		/*
		//DGS
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_X)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y		/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y			= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType	= (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					X2DefectImageType	= (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

					//if (X1DefectImageType == X2DefectImageType) j++; continue;

					// �ֺ��� Line�� ���� �Ǵ� ���
					if ( abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if( i > j )	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}
		*/

		//õ������ ��� ���ϴ� ��� ���� ���� ���� Ȯ��
		/*
		// PCD Crack �κ� ����
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_PCD_CRACK)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y		/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y			= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType	= (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					X2DefectImageType	= (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

					// ���� Pattern�� ������ �ȵ� �ٸ� Pattern�� ����
					if ( X1DefectImageType == X2DefectImageType ){ j++; continue; }

					// �ֺ��� Line�� ���� �Ǵ� ���
					if ( abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if( i > j )	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}
		*/
	}



	//////////////////////////////////////////////////////////////////////////
	// y ����
	//////////////////////////////////////////////////////////////////////////

	if (nYLineNumber > 0) // Y Line�� ������ ���ؾ� ��
	{
		//����
		/*
		int	Y1DefectPattern  = 0;
		int Y1FistPattern	 = 0; // ���� ���� ������ Pattern ��~
		int Y1InitialPattern = 20;

		// �� �ҷ� �߻� Pattern �켱 ���� Black / Red / Green / Blue
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Y ���� ���� ���� ���� ���� Pattern�� ���� Pattern��
			if(resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK )
			{
				CPoint	Y1St, Y1End, Y1Center;

				Y1DefectPattern		= (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����
				Y1FistPattern		= min(Y1InitialPattern, Y1DefectPattern);
				Y1InitialPattern	= Y1FistPattern;
			}
		}

		*/

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

				// ���ϰ� ���� Pattern �̿��� �Ѵ�..��
				//if (Y1DefectImageType != Y1FistPattern) continue;

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					Y2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

					// ���� Pattern�� ������ �ȵ� �ٸ� Pattern�� ����
					//if ( Y1DefectImageType == Y2DefectImageType ){ j++; continue; }

					// �ֺ��� Line�� ���� �Ǵ� ���
					//if ( (X1St.y <= X2Center.y) && (X1End.y >= X2Center.y) )
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}

		//DGS Y Line
		/*
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_Y)
			{
				CPoint	Y1St, Y1End, Y1Center;
				int		Y1DefectImageType;

				Y1St.x				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x				= (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X		/ resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x			= (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X	/ resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectImageType	= (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					Y2DefectImageType	= (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

					// �ֺ��� Line�� ���� �Ǵ� ���
					if ( abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if( i > j )	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
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
	int nXLineNumber = 0;	// ��¥ ���� �ҷ�
	int nYLineNumber = 0;	// ��¥ ���� �ҷ�
	int nOffSet = 50;	// �߽��� �Ÿ� ����

						// �ҷ� ������ ���� �κ�
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// ��¥ X Line �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_DGS_X:
		case E_DEFECT_JUDGEMENT_LINE_PCD_CRACK:
			nXLineNumber++;	// ��¥ ����
			break;

			// ��¥ Y Line �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
		case E_DEFECT_JUDGEMENT_LINE_DGS_Y:
			nYLineNumber++;	// ��¥ ����
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// x ����
	//////////////////////////////////////////////////////////////////////////

	if (nXLineNumber > 0) // X Line�� ������ ���ؾ� ��
	{
		//����
		int	X1DefectPattern = 0;
		int X1FistPattern = 0; // ���� ���� ������ Pattern ��~
		int X1InitialPattern = 20;

		// �� �ҷ� �߻� Pattern �켱 ���� Black > Red > Green > Blue
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� ���� ���� ���� ���� Pattern�� ���� Pattern��
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
			{
				CPoint	X1St, X1End, X1Center;

				X1DefectPattern = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����
				X1FistPattern = min(X1InitialPattern, X1DefectPattern);
				X1InitialPattern = X1FistPattern;
			}
		}

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

																							// ���ϰ� ���� Pattern �̿��� �Ѵ�..��
				if (X1DefectImageType != X1FistPattern) continue;

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

																								// ���� Pattern�� ������ �ȵ� �ٸ� Pattern�� ����
																								//if ( X1DefectImageType == X2DefectImageType ){ j++; continue; }

																								// �ֺ��� Line�� ���� �Ǵ� ���
																								//if ( (X1St.y <= X2Center.y) && (X1End.y >= X2Center.y) )
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}

		//DGS
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_X)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

																							// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

																								//if (X1DefectImageType == X2DefectImageType) j++; continue;

																								// �ֺ��� Line�� ���� �Ǵ� ���
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}

		// PCD Crack �κ� ����
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_PCD_CRACK)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

																							// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

																								// ���� Pattern�� ������ �ȵ� �ٸ� Pattern�� ����
					if (X1DefectImageType == X2DefectImageType) { j++; continue; }

					// �ֺ��� Line�� ���� �Ǵ� ���
					if (abs(X1Center.y - X2Center.y) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// y ����
	//////////////////////////////////////////////////////////////////////////

	if (nYLineNumber > 0) // Y Line�� ������ ���ؾ� ��
	{
		//����
		int	Y1DefectPattern = 0;
		int Y1FistPattern = 0; // ���� ���� ������ Pattern ��~
		int Y1InitialPattern = 20;

		// �� �ҷ� �߻� Pattern �켱 ���� Black / Red / Green / Blue
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// Y ���� ���� ���� ���� ���� Pattern�� ���� Pattern��
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
			{
				CPoint	Y1St, Y1End, Y1Center;

				Y1DefectPattern = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����
				Y1FistPattern = min(Y1InitialPattern, Y1DefectPattern);
				Y1InitialPattern = Y1FistPattern;
			}
		}

		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
			{
				CPoint	Y1St, Y1End, Y1Center;
				int		Y1DefectImageType;

				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

																							// ���ϰ� ���� Pattern �̿��� �Ѵ�..��
				if (Y1DefectImageType != Y1FistPattern) continue;

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					Y2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

																								// ���� Pattern�� ������ �ȵ� �ٸ� Pattern�� ����
																								//if ( Y1DefectImageType == Y2DefectImageType ){ j++; continue; }

																								// �ֺ��� Line�� ���� �Ǵ� ���
																								//if ( (X1St.y <= X2Center.y) && (X1End.y >= X2Center.y) )
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}

		//DGS Y Line
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_Y)
			{
				CPoint	Y1St, Y1End, Y1Center;
				int		Y1DefectImageType;

				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

																							// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					Y2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

																								// �ֺ��� Line�� ���� �Ǵ� ���
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}
	}

	return true;
}
*/

//////////////////////////////////////////////////////////////////////////
// ����: Camera Tap�� ���� �� Line �ҷ� ���� ����
// �۾���: 2018-07-31
// �۾���: PNZ
//////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementCameraTapOverKill(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{
	// ����ó�� Parameter �� ������
	if (dAlignPara == NULL)
		return false;

	int		CTO_OnOff = (int)(dAlignPara[E_PARA_CAMERA_TAP_OVERKILL_ONOFF]);
	int		CTO_CameraType = (int)(dAlignPara[E_PARA_CAMERA_TAP_OVERKILL_CAMERATYPE]);
	int		CTO_Offset = (int)(dAlignPara[E_PARA_CAMERA_TAP_OVERKILL_OFFSET]);

	// On/Off
	if (CTO_OnOff == 0)
		return false;

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	// �ʱ�ȭ
	int	nLINE_X_DEFECT_Number = 0;
	int	nLINE_Y_DEFECT_Number = 0;

	// LINE Y Defect �ҷ� Count
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// X ���� LINE DEFECT
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
		{
			// LINE �ҷ� Count
			nLINE_X_DEFECT_Number++;
		}

		// Y ���� LINE DEFECT
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
		{
			// LINE �ҷ� Count
			nLINE_Y_DEFECT_Number++;
		}
	}

	// LINE Y Defect�� ������ ����
	if (nLINE_Y_DEFECT_Number <= 0 && nLINE_X_DEFECT_Number <= 0) return true;

	//////////////////////////////////////////////////////////////////////////
	// Camera Type Info �޾� ����
	vector<int> Direction;
	vector<int> Position;
	vector<double> Bright_Diff_GV;
	vector<double> Dark_Diff_GV;

	bool CameraTapLoad = CameraTapInfo(CTO_CameraType, Direction, Position, Bright_Diff_GV, Dark_Diff_GV);
	if (!CameraTapLoad) return false;

	if (CTO_CameraType == 0 && Direction.size() > 4) return false; // 29M
	if (CTO_CameraType == 1 && Direction.size() < 4) return false; // 47M

	//////////////////////////////////////////////////////////////////////////
	// LINE Y DEFECT Ȯ��
	if (nLINE_Y_DEFECT_Number > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// LINE �ҷ��� ��츸
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_Y_DARK)
			{
				i++;
				continue;
			}

			// ��ǥ ��
			int nDefectLocation = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;

			// Parameter �ʱ� ȭ
			double DefectMeanGV = 0;
			double LeftMeanGV = 0;
			double RightMeanGV = 0;

			// CameraType �ҷ� ����
			bool CameraTypeOverKill = false;

			// ��ǥ ��
			for (int m = 0; m < Position.size(); m++)
			{
				// Y ���� �ƴϸ� ����
				if (Direction[m] != 2) continue;

				// ��ǥ �޾� ����
				int CTO_Position_Y = Position[m];

				// Tap ��� ��ó�� �ִ��� Ȯ��
				if (abs(nDefectLocation - CTO_Position_Y) <= CTO_Offset)
				{
					// �߻� Pattern Ȯ��
					int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
					int	nImgNum1 = theApp.GetImageClassify(nImgNum);

					// �ش� Pattern�� ���� Ȯ��
					cv::Mat DefectImage = MatOrgImage[theApp.GetImageNum(nImgNum1)][0];

					// Defect ROI
					cv::Rect DefectRect;

					DefectRect.x = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					DefectRect.y = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
					DefectRect.width = resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
					DefectRect.height = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;

					double MaxGV;
					DefectLDRCompair(DefectImage, DefectRect, LeftMeanGV, DefectMeanGV, RightMeanGV, MaxGV);

					// Memory ����
					DefectImage.release();

					// ����
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
				// Dimple�̸� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(i);
			}
			else { i++; continue; }
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// LINE X DEFECT Ȯ��
	if (nLINE_X_DEFECT_Number > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			// LINE �ҷ��� ��츸
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK	&&
				resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK)
			{
				i++;
				continue;
			}

			// ��ǥ ��
			int nDefectLocation = (int)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;

			// Parameter �ʱ� ȭ
			double DefectMeanGV = 0;
			double LeftMeanGV = 0;
			double RightMeanGV = 0;

			// CameraType �ҷ� ����
			bool CameraTypeOverKill = false;

			// ��ǥ ��
			for (int m = 0; m < Position.size(); m++)
			{
				// Y ���� �ƴϸ� ����
				if (Direction[m] != 1) continue;

				// ��ǥ �޾� ����
				int CTO_Position_X = Position[m];

				// Tap ��� ��ó�� �ִ��� Ȯ��
				if (fabs(nDefectLocation - CTO_Position_X) <= CTO_Offset)
				{
					// �߻� Pattern Ȯ��
					int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
					int	nImgNum1 = theApp.GetImageClassify(nImgNum);

					// �ش� Pattern�� ���� Ȯ��
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

					// Memory ����
					DefectImage.release();

					// ����
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
				// Dimple�̸� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(i);
			}
			else { i++; continue; }
		}
	}

	return true;
}

bool BumpInspection::DefectLDRCompair(cv::Mat matSrcImage, cv::Rect rectTemp, double& Left_MeanValue, double& Defect_MeanValue, double& Right_MeanValue, double& Defect_MaxGV)
{
	// �ʱ�ȭ
	cv::Scalar Left_Mean, Left_StdDev, Right_Mean, Right_StdDev, Defect_Mean, Defect_StdDev;
	cv::Rect rectTemp_Left, rectTemp_Right;
	int XShiftUnit = 0;
	int YShiftUnit = 0;
	int CutUnit_X_1 = 600;
	int CutUnit_X_2 = 150;
	int CutUnit_Y_1 = 150;
	int CutUnit_Y_2 = 150;
	int nOffset = 50;

	// Y ����
	if (rectTemp.width / rectTemp.height < 1)
	{
		// ����ó��
		if (rectTemp.height <= CutUnit_Y_1 + CutUnit_Y_2 + nOffset) return false;

		rectTemp.y = rectTemp.y + CutUnit_Y_1;
		rectTemp.height = rectTemp.height - CutUnit_Y_1 - CutUnit_Y_2;

		XShiftUnit = 40;
		YShiftUnit = 0;
		rectTemp_Left.width = 20;
		rectTemp_Left.height = rectTemp.height;
		rectTemp_Right.width = 20;
		rectTemp_Right.height = rectTemp.height;

		// �ҷ� ���̰� ���� ���� �ҷ��� ���� ����
		if (rectTemp.width <= 4)
		{
			rectTemp.width = rectTemp.width + 10;
			rectTemp.x = rectTemp.x - 5;
		}
	}

	// X Defect
	else if (rectTemp.width / rectTemp.height >= 1)
	{
		// ����ó��
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

	// ���� Image
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

	// ���� ���� ��/�� Ȯ�� �ʿ�
	CFileFind find;
	BOOL bFindFile = FALSE;

	bFindFile = find.FindFile(CameraTypeFile);
	find.Close();

	// ���� ���� ���
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
	// Camera Type ����
	int X_Index;
	int Y_Index;
	int CalBuff;

	if (CameraType == 0) { X_Index = 3288;		Y_Index = 4384; } // 29M Camera 8  Tap / PS Image : 13152 x 8768
	else if (CameraType == 1) { X_Index = 2214;		Y_Index = 5280; } // 47M Camera 16 Tap / PS Image : 17712 x 10560
	else
		return false;

	for (int m = 0; ; m++)
	{
		// �о����
		fscanf_s(out, "%d,%d,%lf,%lf\n", &nDirectionBuf, &nPositionBuf, &dbBrightDiffGVBuf, &dbDarktDiffGVBuf);

		// ��ġ�� �����
		if (nDirectionBuf == 1) { CalBuff = nPositionBuf * Y_Index; } // X ����
		if (nDirectionBuf == 2) { CalBuff = nPositionBuf * X_Index; } // Y ����

		// ���� �ִ� Data�� �����ϸ� Break
		if (m != 0 && nDirectionBuf == Direction[m - 1] && CalBuff == Position[m - 1] &&
			dbBrightDiffGVBuf == BrightDiffGV[m - 1] && dbDarktDiffGVBuf == DarkDiffGV[m - 1])
			break;

		// Data ����
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
//��	 ��: PCD CRACK ����																			//  
//�۾���: 2017-12-01																				//
//�۾���: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementPCDCrack(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// ���� ���� ���
	if (dAlignPara == NULL)	return false;

	// ��� ���ϴ� ���
	if (dAlignPara[E_PARA_PCD_CRACK_FLAG] <= 0)	return true;

	// Start Position Y
	cv::Point ptAlignCorner[4]; // Align �� Cell Corner

	ptAlignCorner[0].x = m_stThrdAlignInfo.rcAlignCellROI.x;
	ptAlignCorner[0].y = m_stThrdAlignInfo.rcAlignCellROI.y;

	// Black, Red ,Blue Pattern����
	// ���� ��ġ X1 = 740~820, X2 = 2320~2400
	int nRange1Start = (740 * 2) + ptAlignCorner[0].y;
	int nRange1End = (820 * 2) + ptAlignCorner[0].y;
	int nRange2Start = (2320 * 2) + ptAlignCorner[0].y;
	int nRange2End = (2400 * 2) + ptAlignCorner[0].y;

	int nXLineNumber = 0;	// ��¥ ���� �ҷ� 
	int nOffSet = 20;	// �߽��� �Ÿ� ����

	// Merge�� ���� X Line Bright �ҷ�
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// ��¥ X Line �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
			nXLineNumber++;	// ��¥ ����
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// x ���⿡ Line �� ��
	//////////////////////////////////////////////////////////////////////////

	if (nXLineNumber > 0) // X Line�� ������ ���ؾ� ��
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT)
			{
				CPoint	X1Center;

				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				//AfxMessageBox(_T("Center Distance: '%d'"), X1Center.y);

				//Ratio ��ŭ ���� ��ǥ ���Ѵ�.
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
//��	 ��: DGS ���� ������ ver 0.3																	//  
//�۾���: 2017-11-03, 2017-11-15 ����																// 
//�۾���: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementNewDGS(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// X Line ���� �ҷ�
	int nYLineNumber = 0;	// Y Line ���� �ҷ�
	int nOffSet = 20;	// �߽��� �Ÿ� ����
	int nCenterOffSet = 30;

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// ��¥ Line�� �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_VTH_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_VTH_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_DARK:
			nXLineNumber++;	// ��¥ ����
			break;

			// ��¥ Line�� �ҷ��� ��츸 Count
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
			nYLineNumber++;	// ��¥ ����
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// ���� Pattern���� Cross �Ǹ� DGS�� ����
	//////////////////////////////////////////////////////////////////////////

	int nCountDGS = 0;

	if ((nXLineNumber > 0) && (nYLineNumber > 0))
	{
		// �ҷ� ����
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ����
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

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
				{
					// ���� �ҷ� �� ����
					if (i == j) { continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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

					if (X1DefectPattern != Y2DefectPattern) { continue; } // ���� Pattern �ƴϸ� �ȵ� �̰� DGS ��...

					// �ֺ��� Line�� ���� �Ǵ� ���
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

	//DGS_X, DGS_Y �ֺ� Line �ҷ� ����
	if (nCountDGS > 0)
	{
		// DGS �ߺ� ����
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ����
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

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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

					// �ֺ��� Line�� ���� �Ǵ� ���
					if (abs(X1Center.y - Y2Center.y) <= nCenterOffSet)
					{
						if (abs(X1Center.x - Y2Center.x) <= nCenterOffSet)
						{
							// ����Ʈ ������ ������
							if (i > j)	 i--;

							// �ٸ� Pattern�� Line ����
							resultPanelData.m_ListDefectInfo.RemoveAt(j);
						}
						else  j++;
					}
					else  j++;
				}
			}


			// X ���� DGS X�� �ߺ� �� DGS X�� ����
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_X)
			{
				CPoint	X1St, X1End, X1Center;
				int X1DefectPattern;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectPattern = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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

					// �ֺ��� Line�� ���� �Ǵ� ���
					if (abs(X1Center.y - Y2Center.y) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� Line ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}

			// Y ���� DGS Y�� �ߺ� �� DGS Y�� ����
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_DGS_Y)
			{
				CPoint	Y1St, Y1End, Y1Center;
				int Y1DefectPattern;

				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectPattern = resultPanelData.m_ListDefectInfo[i].Pattern_Type;

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// Y ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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

					// �ֺ��� Line�� ���� �Ǵ� ���
					if (abs(Y1Center.x - Y2Center.x) <= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� Line ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}
					// �ֺ��� ������ ���� ���
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
//��	 ��: Crack ���� ver 0.2																		//  
//�۾���: 2017-11-10																				// 
//�۾���: PNZ																						//
//Crack Pattern: G32 ���� (Image Number 9)														//
//////////////////////////////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementCrack(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// Line ���� �ҷ�

	int nCPLNumber_Left = 0;	// Crack Pattern Left Line ���� �ҷ�
	int nCPLNumber_Right = 0;	// Crack Pattern Right Line ���� �ҷ�
	int nOffSet = 10;	// �߽��� �Ÿ� ����

	// Position of Crack Defect
	int Distance1 = 1467;
	int Distance2 = 1761;
	int Distance3 = 2054;

	cv::Point ptAlignCorner[4]; // Align �� Cell Corner

	ptAlignCorner[0].x = m_stThrdAlignInfo.rcAlignCellROI.x;
	ptAlignCorner[0].y = m_stThrdAlignInfo.rcAlignCellROI.y;

	ptAlignCorner[2].x = m_stThrdAlignInfo.rcAlignCellROI.x + m_stThrdAlignInfo.rcAlignCellROI.width;
	ptAlignCorner[2].y = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height;

	int CenterLocation = ptAlignCorner[0].y + abs(ptAlignCorner[2].y - ptAlignCorner[0].y) / 2;
	int Distance = 0;

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int	nImgNum1 = theApp.GetImageClassify(nImgNum);

		if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_32) continue;

		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// ��¥ Line�� �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
			nXLineNumber++;	// ��¥ ����
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Black Pattern�� ���� ��ġ ����� ��Ÿ���� Crack �ҷ�
	//////////////////////////////////////////////////////////////////////////

	if (nXLineNumber > 0)
	{
		// �ҷ� ����
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_32) continue;

			// X ����
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_OPEN ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT)
			{
				CPoint	X1Center;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;
				//X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio;

				if (X1Center.y <= CenterLocation) { nCPLNumber_Left++; }	//Left ���� ������ �ִ� �ҷ� ����
				if (X1Center.y > CenterLocation) { nCPLNumber_Right++; }	//Right ���� ������ �ִ� �ҷ� ����
			}
			else continue;
		}
	}

	if (nCPLNumber_Left >= 3)
	{
		// �ҷ� ����
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_32) continue;

			// X ����
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
		// �ҷ� ����
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
			int	nImgNum1 = theApp.GetImageClassify(nImgNum);

			if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_32) continue;

			// X ����
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

	// Left�� Right �� 3�� �̻��� �̸� Crack Left�� Right�� Both�� �ٲ۴�
	if ((nCPLNumber_Left >= 3) && (nCPLNumber_Right >= 3))
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ����
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
//��	 ��: Line ���� �з� ver 0.2																	//  
//�۾���: 2017-11-14																				// 
//�۾���: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementClassifyLineDefect(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT])
{
	int nLineNumber = 0;	// ��¥ ���� �ҷ�
	int nLineDefect = 0;	// ���� ����
	int nOffSet = 50;	// Line �ҷ� ���� ����

	cv::Mat ImgBK;
	if (!MatOrgImage[MAX_GRAB_STEP_COUNT - 4][0].empty())
	{
		ImgBK = MatOrgImage[MAX_GRAB_STEP_COUNT - 4][0].clone();
	}
	else return false;

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// Line�� �ҷ��� ��츸 Count
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

	// Line ������ ���� �ϸ�
	if (nLineNumber > 0)
	{
		//////////////////////////////////////////////////////////////////////////
		// DO, GO ����
		//////////////////////////////////////////////////////////////////////////

		// �ҷ� ����
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ����
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

				// Black �Ǵ� Crack Pattern ��Line�� ������ 5.5�� G32�� Crack Pattern��
				if ((nImgNum1 == E_IMAGE_CLASSIFY_AVI_BLACK) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_32))
				{
					if (nMaxGV >= 200)
					{
						nOffSet = 150; // ũ�� ����
					}
				}
				// RGB Pattern Dark Line �� ��  �ܰ� ��˻� �������� ���� offset �ø���
				else if ((nImgNum1 == E_IMAGE_CLASSIFY_AVI_R) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_G) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_B))
				{
					if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK && nOffSet < 30)
						nOffSet = 30;
				}

				//����ó��

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

			// Y ����
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

				// Black �Ǵ� Crack Pattern ��Line�� ������ 5.5�� G32�� Crack Pattern��
				if ((nImgNum1 == E_IMAGE_CLASSIFY_AVI_BLACK) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_32))
				{
					if (nMaxGV >= 200)
					{
						nOffSet = 150; // ũ�� ����
					}
				}
				// RGB Pattern Dark Line �� ��  �ܰ� ��˻� �������� ���� offset �ø���
				else if ((nImgNum1 == E_IMAGE_CLASSIFY_AVI_R) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_G) || (nImgNum1 == E_IMAGE_CLASSIFY_AVI_B))
				{
					if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK && nOffSet < 30)
						nOffSet = 30;
				}

				// ����ó��

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
//��	 ��: 6.39QHD Notch Y Line ����															//
//�۾���: 2018-07.04																			//
//�۾���: YDS																					//
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

	// On/Off Ȯ�� ( ���� ���� �� ) Norch ���� �Ŵ� ���� ��
	if (NORCH_OnOff == 0) return false;

	int		nYLineNumber = 0;	// ��¥ ���� �ҷ�
	int		nOffSet = 0;	// Line �ҷ� ���� ����
	int		nLengthOffSet = 0;	// Norch �ҷ� ���� Offset
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
			// Line�� �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_DGS_Y:
			nYLineNumber++;	// ��¥ ����
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
				// Norch �κ� �ҷ����� Ȯ��
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

				// Norch �κ� ��ǥ ��� �ϱ�
				else if (bNorchInfoGet == false)
				{
					NorchAvgWidth = NORCH_UNIT_X;
					NorchAvgHeight = NORCH_UNIT_Y;

					NorchLocation = NORCH_UNIT_X + m_stThrdAlignInfo.rcAlignCellROI.x + nOffSet;
					TopNorchLocation = NORCH_UNIT_Y + m_stThrdAlignInfo.rcAlignCellROI.y;
					BottomNorchLocation = m_stThrdAlignInfo.rcAlignCellROI.y + m_stThrdAlignInfo.rcAlignCellROI.height - NORCH_UNIT_Y;
				}

				// �ҷ� ����
				DefectY_Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X;
				DefectY_Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y;
				DefectY_YStart = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
				DefectY_YEnd = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;
				DefectY_XStart = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
				DefectY_MaxGV = (long)resultPanelData.m_ListDefectInfo[i].Defect_MaxGV;

				DefectLength = abs(DefectY_YStart - DefectY_YEnd);

				// Case1Norch �κ� ���� LINE ó�� ����
				if (DefectY_Center.x >= NorchLocation) continue;
				if (DefectY_MaxGV >= 250) continue;

				int DefectY_GV = ImgBK.at<uchar>(DefectY_Center.y, (DefectY_XStart - nOffSet));

				// Case1 : �� Norch ���̿� �ִ� �ҷ�
				if ((TopNorchLocation <= DefectY_Center.y && DefectY_Center.y <= BottomNorchLocation) && DefectY_GV == 255)
				{
					// Parameter �ʱ� ȭ
					double DefectMeanGV = 0;
					double LeftMeanGV = 0;
					double RightMeanGV = 0;

					// �ش� Pattern�� ���� Ȯ��
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

				// �߰� Norch �� ������ ���� ��
				else if ((DefectY_Center.y < TopNorchLocation || BottomNorchLocation < DefectY_Center.y) && (DefectLength <= NorchAvgHeight + nLengthOffSet))
				{
					// Parameter �ʱ� ȭ
					double DefectMeanGV = 0;
					double LeftMeanGV = 0;
					double RightMeanGV = 0;

					// CameraType �ҷ� ����
					bool NorchOverKill = false;

					int	nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
					int	nImgNum1 = theApp.GetImageClassify(nImgNum);

					double NORCH_THGV = 0;

					// Pattern �� Diff GV �ٸ��� ����
					if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_G) NORCH_THGV = NORCH_THGV_GREEN;
					if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_R || nImgNum1 == E_IMAGE_CLASSIFY_AVI_B) NORCH_THGV = NORCH_THGV_RB;

					// Norch ��� �κп� �ִٰ� �ϸ�
					if ((NorchLocation - nOffSet) >= DefectY_XStart)								  NORCH_THGV = NORCH_THGV_RB;

					// �ش� Pattern�� ���� Ȯ��
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
//��	 ��: Special Pattern�� Gray Pattern �༱ ���� ��� �� ver. 0.1					//  
//�۾���: 2017-11-22																					//
//�۾���: PNZ																								//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool BumpInspection::JudgementSpecialGrayLineDefect(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// ��¥ ���� �ҷ�
	int nYLineNumber = 0;	// ��¥ ���� �ҷ�
	int nOffSet = 10;	// �߽��� �Ÿ� ����

	// �ҷ� ������ ���� �κ�
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:
			nXLineNumber++;	// ��¥ ����
			break;

		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:
			nYLineNumber++;	// ��¥ ����
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// x ����
	//////////////////////////////////////////////////////////////////////////

	if (nXLineNumber > 1) // X Line�� ������ ���ؾ� ��
	{
		//����
		int PatternIndex;
		int	XLine_G64 = 0;
		int XLine_SP2 = 0;

		// G64 Pattern���� ���� �� Line Defect�� Count
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� ���� ���� ���� ���� Pattern�� ���� Pattern��
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

		// Special - 2 Pattern���� ���� �� Line Defect�� Count
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� ���� ���� ���� ���� Pattern�� ���� Pattern��
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

		//SP2�� Line ������ ���� �� G64���� �ҷ��� ���� ��
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

				X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

				// G64 Pattern���� 
				if (X2DefectImageType == 9)
				{
					//ù��° Line Defect
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


		//G64 Pattern�� �������� Special Pattern���� ����� Line Defect�� ���Ѵ�, ���� ���� ��ġ�� Special Pattern�� �ҷ��� �����.��
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK)
			{
				CPoint	X1St, X1End, X1Center;
				int		X1DefectImageType;

				X1St.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1End.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio;
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

				// ���ϰ� ���� Pattern �̿��� �Ѵ�..��
				if (X1DefectImageType != 6) continue;

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

					// ���� Pattern�� ������ �ȵ� �ٸ� Pattern�� ����
					if (X2DefectImageType == X1DefectImageType) { j++; continue; }

					// �ֺ��� Line�� ���� �Ǵ� ���
					if (abs(X1Center.y - X2Center.y) >= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// y ����
	//////////////////////////////////////////////////////////////////////////

	if (nYLineNumber > 1) // X Line�� ������ ���ؾ� ��
	{
		//����
		int PatternIndex;
		int	 YLine_G64 = 0;
		int YLine_SP2 = 0;

		// G64 Pattern���� ���� �� Line Defect�� Count
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� ���� ���� ���� ���� Pattern�� ���� Pattern��
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

		// Special - 2 Pattern���� ���� �� Line Defect�� Count
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� ���� ���� ���� ���� Pattern�� ���� Pattern��
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

		//G64 Pattern�� �������� Special Pattern���� ����� Line Defect�� ���Ѵ�, ���� ���� ��ġ�� Special Pattern�� �ҷ��� �����.��
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK)
			{
				CPoint	Y1St, Y1End, Y1Center;
				int		Y1DefectImageType;

				Y1St.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1End.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1Center.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio;
				Y1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����

				// ���ϰ� ���� Pattern �̿��� �Ѵ�..��
				if (Y1DefectImageType != 6) continue;

				// ���� �ҷ�
				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
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
					Y2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

					// ���� Pattern�� ������ �ȵ� �ٸ� Pattern�� ����
					if (Y2DefectImageType == Y1DefectImageType) { j++; continue; }

					// �ֺ��� Line�� ���� �Ǵ� ���
					if (abs(Y1Center.x - Y2Center.x) >= nOffSet)
					{
						// ����Ʈ ������ ������
						if (i > j)	 i--;

						// �ٸ� Pattern�� ��� ��� ��� Line�� ��� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
					}

					// �ֺ��� ������ ���� ���
					else  j++;
				}
			}
		}
	}

	// Special Pattern�� �ִ� Line Defect���� ����� �Ф� �� �����µ� �� ���ٴ�... ����
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

		X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����

		if (X2DefectImageType == 6)
		{
			resultPanelData.m_ListDefectInfo.RemoveAt(j);
		}
		else  j++;
	}

	return true;
}




//////////////////////////////////////////////////////////////////////////
// Align �� ROI ��� �̻� Ȯ��
//////////////////////////////////////////////////////////////////////////
long BumpInspection::CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult,
	bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode)
{
	const int DUST_RETRY_LIMIT = 1;
	// test
	CCPUTimer tact;
	tact.Start();
	long nErrorCode = E_ERROR_CODE_TRUE;

	// ��� Ÿ��
	int nEqpType = theApp.m_Config.GetEqpType();

	// ���� �˰��� �˻� �Ķ���� ��������
	// AD Parameter ������ ������ ��. ToDo.
	double* dAlgPara = theApp.GetFindDefectADParameter(nImageNum, nCameraNum);

	// �˰��� ���� ��ȣ
	int nAlgImg = theApp.GetImageClassify(nImageNum);

	// Alg �α׿�
	wchar_t strAlgLog[MAX_PATH] = { 0, };
	swprintf(strAlgLog, _T("ID:%s\tPat:%s"), (LPCWSTR)strPanelID, theApp.GetGrabStepName(nImageNum));

	// Dust ������ ���
	if (nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST)
	{
		if (!theApp.GetUseFindDefectAD(nImageNum, nCameraNum))
		{
			// Dust GV �˻� ���� ���� ��� ��˶� �߻�
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Disabled Check DUST - InspSkip !!!. PanelID: %s, Stage: %02d, CAM: %02d"),
				strPanelID, nStageNo, nCameraNum);

			//theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_DIABLE_CHECK_DUST, eALARMTYPE_HEAVY, bIsHeavyAlarm);

			// Seq. Task �� Dust ���� �� ��� �ۺ�
			bNeedRetry = false;
			bIsNormalDust = true;
			bChkDustEnd = true;
		}
		else
		{
			// AD - ��Ⱚ Ȯ��
			nErrorCode = Align_FindDefectAD_GV(MatOrgImage, dAlgPara, dMeanResult, ptCorner, nEqpType, nAlgImg, strAlgLog);

			if (nErrorCode != E_ERROR_CODE_TRUE)
			{
				bIsNormalDust = false;

				if (bUseDustRetry && nDustRetryCnt < DUST_RETRY_LIMIT)
				{
					// Seq. Task �� Dust ���� �� ��� �ۺ�
					bNeedRetry = true;
					// �˶� �߻���Ű�� �ʰ� �˻� ���� �� ���� ����
					bIsHeavyAlarm = true;
				}
				else
				{
					bNeedRetry = false;
					// Dust ���� �̻� �˶� �߻�
					//theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_DUST_ABNORMAL, eALARMTYPE_HEAVY, bIsHeavyAlarm);
				}


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
				// Seq. Task �� Dust ���� �� ��� �ۺ�
				bNeedRetry = false;
				bIsNormalDust = true;
				// Alg DLL End Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("AD GV Algorithm End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Avg : %.2f GV )"),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0]);
			}
			bChkDustEnd = true;
		}
	}
	// Dust ������ �ƴ� ���
	else
	{
		// AVI �� Dust ���� ���� üũ ����ñ��� ���
		while (!bChkDustEnd)
		{
			Sleep(10);
		}

		if (bIsNormalDust)
		{
			// ����� AD GV�� Ȯ�� �ڵ�
			if (theApp.GetUseFindDefectAD(nImageNum, nCameraNum))
			{
				//////////////////////////////////////////////////////////////////////////
				// KSW 17.11.23
				// 5.99" �������� �߻� - Ư�� ��ġ ���κҷ� ����
				// ���� �ҷ����� ���� AD���� ( ���κҷ��� ��Ƽ� AD���� ) -> �˻����� ���� : E�� ����
				// ��˻� ���� ���� ��, AD �˻� ����ǵ��� ����
				//////////////////////////////////////////////////////////////////////////

				// ��� ������ ���
				if (dAlgPara[E_PARA_CHECK_NON_DEL_FLAG] > 0)
				{
					cv::Mat matTempBuf = MatOrgImage.clone();

					// ��˻� ���� ROI ���� �޾ƿ���
					int nFilterROICnt = theApp.GetFilterROICnt(nImageNum, nCameraNum);

					// ��˻� ���� ROI ���� ��ŭ
					for (int nROICnt = 0; nROICnt < nFilterROICnt; nROICnt++)
					{
						// ��� ���� Ȯ��
						if (theApp.GetUseFilterROI(nImageNum, nCameraNum, nROICnt))
						{
							// ��˻� ���� ROI ��������
							// P/S ����� ���, ����
							CRect rectFilterArea = theApp.GetFilterROI(nImageNum, nCameraNum, nROICnt, nRatio);

							// ��˻� ���� - Left-Top ��ǥ�� �������� ��ǥ���� ������ ����
							rectFilterArea.OffsetRect(CPoint(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_TOP].y));

							// ���� ũ�Լ����ؼ� ��� ( �Ķ����ȭ �ʿ� )
							int nOut = (int)dAlgPara[E_PARA_CHECK_NON_DEL_OFFSET];
							rectFilterArea.InflateRect(nOut, nOut, nOut, nOut);

							// ����ó��
							if (rectFilterArea.left < 0)	rectFilterArea.left = 0;
							if (rectFilterArea.top < 0)	rectFilterArea.top = 0;
							if (rectFilterArea.right >= matTempBuf.cols)	rectFilterArea.right = matTempBuf.cols - 1;
							if (rectFilterArea.bottom >= matTempBuf.rows)	rectFilterArea.bottom = matTempBuf.rows - 1;

							// ROI
							cv::Mat matZeroBuf = matTempBuf(cv::Rect(rectFilterArea.left, rectFilterArea.top, rectFilterArea.Width(), rectFilterArea.Height()));

							// ROI 0���� �ʱ�ȭ
							matZeroBuf.setTo(0);

							// ����
							matZeroBuf.release();
						}
					}

					// ��� ���� Ȯ��
					//cv::imwrite("D:\\asd.bmp", matTempBuf);

					// AD - ��Ⱚ Ȯ��
					nErrorCode = Align_FindDefectAD_GV(matTempBuf, dAlgPara, dMeanResult, ptCorner, nEqpType, nAlgImg, strAlgLog);

					matTempBuf.release();
				}
				// ��� ���� ���� ���
				else
				{
					// AD - ��Ⱚ Ȯ��
					nErrorCode = Align_FindDefectAD_GV(MatOrgImage, dAlgPara, dMeanResult, ptCorner, nEqpType, nAlgImg, strAlgLog);
				}

				// ������ ���, ���� �ڵ� & �α� ���
				if (nErrorCode != E_ERROR_CODE_TRUE)
				{
					// Alg DLL Stop Log
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("AD GV Defect - InspStop !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s..\n\t\t\t\t( Avg : %.2f GV / Label : %d )"),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0], (int)dMeanResult[2]);

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

// Resolution ���
double BumpInspection::calcResolution(CWriteResultInfo WrtResultInfo)
{
	// Panel Size
	double dPanelSizeX = WrtResultInfo.GetPanelSizeX();
	double dPanelSizeY = WrtResultInfo.GetPanelSizeY();

	// ����� ���� ����
	double dTempX, dTempY, dImageSizeX, dImageSizeY;
	dTempX = m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x - m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x;
	dTempY = m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y - m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y;
	dImageSizeX = sqrt(dTempX*dTempX + dTempY*dTempY);

	// ����� ���� ����
	dTempX = m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x - m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x;
	dTempY = m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y - m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y;
	dImageSizeY = sqrt(dTempX*dTempX + dTempY*dTempY);

	// ����, ���� Resolution
	double dResolutionX = dPanelSizeX / dImageSizeX;
	double dResolutionY = dPanelSizeY / dImageSizeY;

	// ��� Resolution ( Non P/S Mode �� ����� Resolution )
	double dResolution = (dResolutionX + dResolutionY) / 2.0 * m_stThrdAlignInfo.nRatio;

	//CString strTemp;
	//strTemp.Format(_T("%f, %f\n%f, %f\n%f, %f, %f"), dPanelSizeX, dPanelSizeY, dImageSizeX, dImageSizeY, dResolutionX, dResolutionY, dResolution);
	//AfxMessageBox(strTemp);

	return dResolution;
}

bool BumpInspection::JudgementRepeatCount(CString strPanelID, ResultPanelData& resultPanelData)
{
	// ������ �ߺ� ���� ����
	// ���� ��� ������ ������� �ݺ� Ƚ�� üũ - �г� ���� ���� ��� ����
	EnterCriticalSection(&theApp.m_csJudgeRepeatCount);

	ListCurDefect* pStCurDefList = new ListCurDefect();
	std::list<RepeatDefectInfo>* pList = theApp.GetRepeatDefectInfo();					// ���� �ҷ� ����Ʈ(Pixel ��ǥ)
	static bool bIsAlarm = false;														// ���� �˶� �߻� ����

	CCPUTimer ttRepeatCount;
	ttRepeatCount.Start();

	memcpy(pStCurDefList->bUseChkRptDefect, theApp.m_Config.GetUseCCDAlarm(), sizeof(pStCurDefList->bUseChkRptDefect));

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (0) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	// Pixel / Work ��ǥ üũ �� �� ������� ���� ��� ���� List Clear�ϰ� ���� Return
	if (!pStCurDefList->bUseChkRptDefect[ePIXEL] && !pStCurDefList->bUseChkRptDefect[eWORK])
	{
		// ���� ���� �̷� ������ ����
		for (int nKind = 0; nKind < eMACHINE; nKind++)			// 2018. 09. 21 MDJ APP Repeat Defect �߰��� ���Ͽ� �������� 1�� ������ ����
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
			// Point Dark / Bright�� ���ϴ� ��츸 �� list ����
			// Point Dark�� ���
		case E_DEFECT_JUDGEMENT_POINT_DARK:
		case E_DEFECT_JUDGEMENT_POINT_GROUP_DARK:
			// Pixel, Work ��ǥ �߰� - �� �ʿ��� �׸� �߰��� ��
			pStCurDefList->Add_Tail(ePIXEL, E_DEFECT_JUDGEMENT_POINT_DARK, &resultPanelData.m_ListDefectInfo[i]);
			pStCurDefList->Add_Tail(eWORK, E_DEFECT_JUDGEMENT_POINT_DARK, &resultPanelData.m_ListDefectInfo[i]);
			nTotalCount++;
			break;

			// Point Bright�� ���
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
	int nMaxRepeatCount[eMACHINE] = { 0, };					// 2018. 09. 21 MDJ APP Repeat Defect �߰��� ���Ͽ� �������� 1�� ������ ����

#pragma omp parallel for schedule(dynamic)		// ����ó�� �߰�
	for (int nKind = 0; nKind < eMACHINE; nKind++)			// 2018. 09. 21 MDJ APP Repeat Defect �߰��� ���Ͽ� �������� 1�� ������ ����
	{
		std::list<RepeatDefectInfo>::iterator iterDst, iterSrc;
		bool bRepeat = false;
		// ���� �ҷ� ����Ʈ
		for (iterDst = pList[nKind].begin(); iterDst != pList[nKind].end(); )
		{
			bRepeat = false;

			// ���� �ҷ� ��ġ Area ����
			// CCD �ҷ� - ���� Pixel / ���� �ҷ� - ���� um
			CRect rcDst(iterDst->ptCenterPos.x - nRptOffset[nKind] - 1, iterDst->ptCenterPos.y - nRptOffset[nKind] - 1,			// LT
				iterDst->ptCenterPos.x + nRptOffset[nKind] + 1, iterDst->ptCenterPos.y + nRptOffset[nKind] + 1);					// RB

			// ���� �ҷ� ����Ʈ
			for (iterSrc = pStCurDefList->listCurDefInfo[nKind].begin(); iterSrc != pStCurDefList->listCurDefInfo[nKind].end(); )
			{
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
						// ī��Ʈ ���� �� �ִ� �ݺ� �߻� Ƚ�� ����
						if (iterDst->nRepeatCount >= nRptLArmCnt[nKind])
						{
							nMaxRepeatCount[nKind] = MAX(nMaxRepeatCount[nKind], iterDst->nRepeatCount);		// �ִ� �ݺ� �߻� Ƚ��

							// Log �� ���ڿ�
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
						// ���� ����Ʈ ��հ� ��� �� ī��Ʈ ���� �� ���� �ҷ� ����
						iterSrc = pStCurDefList->listCurDefInfo[nKind].erase(iterSrc);
						// �ߺ� �ҷ� ���� �� �� �� �ҷ� �Ű� �Ⱦ�
						// (���ο� �ҷ� ����Ʈ�� Offset �̳��� ���� ��ġ�� �ҷ��� ���ٰ� ����)
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

			// �ϳ��� ��ġ�°� ������ ���� ����Ʈ�� �ҷ� ����
			if (!bRepeat)
				iterDst = pList[nKind].erase(iterDst);
			else
				iterDst++;
		}
	}

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (2) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	// ���� ��ġ �ݺ� �ҷ� �� �ߺ� Ƚ���� ���� ���� �ҷ��� ������ ��/�߾˶� �������� ���� ��� -> ��˶� ��� ����
	for (int nKind = 0; nKind < eMACHINE; nKind++)			// 2018. 09. 21 MDJ APP Repeat Defect �߰��� ���Ͽ� �������� 1�� ������ ����
	{
		if (pStCurDefList->bUseChkRptDefect[nKind])
		{
			if (nMaxRepeatCount[nKind] >= nRptHArmCnt[nKind])
			{
				bIsAlarm = true;
				// nKind 0 : CCD �߾˶� (3000) / nKind 1 : �����ҷ� �߾˶� (3001)
				theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eAutoRun, eALARMID_CCD_DEFECT_ERROR + nKind, eALARMTYPE_HEAVY);
			}
			else
			{
				bIsAlarm = false;
			}

			// ��ġ�� �׸� ���� ���� ����Ʈ�� ���� �߻��� �ҷ� �߰�
			// merge�ϱ� ���� �����ϴ� ���� �߰�
			pList[nKind].sort();
			pStCurDefList->listCurDefInfo[nKind].sort();
			pList[nKind].merge(pStCurDefList->listCurDefInfo[nKind]);
		}
	}

	// ���ο� ����Ʈ ���Ϸ� ����
	bRet = m_fnSaveRepeatDefInfo(pList);

	SAFE_DELETE(pStCurDefList);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (3) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	LeaveCriticalSection(&theApp.m_csJudgeRepeatCount);

	return bRet;
}

bool BumpInspection::m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo)
{
	// List ���� ���
	if (pListRepeatInfo == NULL)			return false;

	// TXT ����
	std::list<RepeatDefectInfo>::iterator iterList;
	CStdioFile	fileWriter;
	CString		strRepeatInfoHeader, strRepeatInfo;
	BOOL bRet = FALSE;

	// ��� ������� ���� ���� �ڵ����� Clear
	for (int nKind = 0; nKind < eMACHINE; nKind++)			// 2018. 09. 21 MDJ APP Repeat Defect �߰��� ���Ͽ� �������� 1�� ������ ����
	{
		// ���� ����
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
			// ��� ���� - for GUI
			strRepeatInfoHeader = _T("Type,DefectX,DefectY,RepeatCount\r\n");
			fileWriter.WriteString(strRepeatInfoHeader);
			// �ߺ� ��ǥ ���� ��ŭ
			for (iterList = pListRepeatInfo[nKind].begin(); iterList != pListRepeatInfo[nKind].end(); ++iterList)
			{
				// ���� ��ġ ��ǥ ����
				strRepeatInfo.Format(_T("%d,%d,%d,%d\r\n"), (int)iterList->eDefType,
					iterList->ptCenterPos.x, iterList->ptCenterPos.y, iterList->nRepeatCount);

				fileWriter.SeekToEnd();
				fileWriter.WriteString(strRepeatInfo);
			}

			// ���� ���� ��츸 �ݱ�
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

	// ROI ���� Ȯ��
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

	// ���� Index
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

	// ���� �޾ƿ���
	int nDelete_Defect_Area;			// �ҷ� ũ��
	int nDelete_Defect_Offset;			// CHole Offset

	//////////////////////////////////////////////////////////////////////////

										// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nCHoleADCnt = 0;

	// CHole AD �ҷ� ��/�� üũ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
		if (nDefect_Type == E_DEFECT_JUDGEMENT_DISPLAY_CHOLE_ABNORMAL) nCHoleADCnt++;
	}

	// CHole AD ����
	if (nCHoleADCnt > 0)
	{
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefect1_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
			int nDefect1_ImgNo = resultPanelData.m_ListDefectInfo[i].Img_Number;

			if (nDefect1_Type != E_DEFECT_JUDGEMENT_DISPLAY_CHOLE_ABNORMAL) continue;

			// CHole ����
			CRect rectCHole_Rect;
			rectCHole_Rect.left = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
			rectCHole_Rect.top = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;
			rectCHole_Rect.right = resultPanelData.m_ListDefectInfo[i].Pixel_End_X;
			rectCHole_Rect.bottom = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y;

			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); j++)
			{
				//���� �ҷ� �϶� �Ѿ��
				if (i == j) continue;

				int nDefect2_Type = resultPanelData.m_ListDefectInfo[j].Defect_Type;
				int nDefect2_ImgNo = resultPanelData.m_ListDefectInfo[j].Img_Number;

				// CHole AD �� �� �Ѿ��
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

	// CHole 2�� ����
	else
	{
		int nDefect_Cnt = 0;		// Point �ҷ�
		int nDefect2_Cnt = 0;		// Line &  Type2 Y �ҷ�
		int nDefect3_Cnt = 0;		// ���� �ҷ�
									//int nDefect3_Cnt = 0;			// �ӽ� Type2 Y����

		int nMuraStart = E_DEFECT_JUDGEMENT_MURA_START;
		int nMuraEnd = E_DEFECT_JUDGEMENT_MURA_END;
		for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
		{
			int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			switch (nDefect_Type)
			{
				// ����
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

		if (nDefect_Cnt > 0) // ���� ���ſ�
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

				// ���� ���� Ȯ��
				// ���� ũ��
				int nDefect_Width = resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;
				int nDefect_Height = resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y;

				// ���� �ҷ��� �˻�
				if (nDefect_Width * nDefect_Height > nDelete_Defect_Area) continue;

				// ���� �߽� ��ǥ
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

					//����ó��
					if (rectDefect_Center.x < 0) continue;
					if (rectDefect_Center.y < 0) continue;
					if (rectDefect_Center.x >= matCHoleCoordBuf.cols - rectDefect_Center.width)		continue;
					if (rectDefect_Center.y >= matCHoleCoordBuf.rows - rectDefect_Center.height)	continue;

					cv::Scalar scrMean = cv::mean(matCHoleCoordBuf(rectDefect_Center));
					//���� ���� ������ JHK 20200805
					//if (scrMean[0] > 0)
					{
						resultPanelData.m_ListDefectInfo.RemoveAt(i--);
						break;
					}
				}
			}
		}

		if (nDefect2_Cnt > 0) // Type2 & Line ���� ���ſ�
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
					//��� ����ó�� �̰� �߻�
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
				// Type2 ��ġ �� ũ��
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

						// CHole �ֺ���
						if (nDefect_Start_X > nCHole_End_X)		continue;
						if (nDefect_End_X < nCHole_Start_X)		continue;
						if (nDefect_Start_Y > nCHole_End_Y)		continue;
						if (nDefect_End_Y < nCHole_Start_Y)		continue;

						// CHole Area ��
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


		if (nDefect3_Cnt > 0 && nDelete_Def_Offset > 0) // LJH 20201019 ī�޶� Ȧ�κ� ���� ����
		{

			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				int nDefect_Type = resultPanelData.m_ListDefectInfo[i].Defect_Type;
				int nDefect_ImgNo = resultPanelData.m_ListDefectInfo[i].Img_Number;

				if (nDefect_Type < E_DEFECT_JUDGEMENT_MURA_START || nDefect_Type > E_DEFECT_JUDGEMENT_MURA_END) continue;

				// �ش� ���� �ҷ��� ũ�� 
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
					//CȦ ������
					cv::Rect rMuraCHole;
					rMuraCHole.x = nCHole_Start_X;
					rMuraCHole.y = nCHole_Start_Y;
					rMuraCHole.width = nCHole_End_X - nCHole_Start_X;
					rMuraCHole.height = nCHole_End_Y - nCHole_Start_Y;

					if (rMuraCHole.x < 0 || rMuraCHole.y < 0 || rMuraCHole.width < 0 || rMuraCHole.height < 0) continue;
					// ������ �ҷ�
					cv::Rect rAnd;
					rAnd = rMuraDef & rMuraCHole;
					double dInRate = (double)(rAnd.width*rAnd.height) / (double)(rMuraDef.width*rMuraDef.height);
					// ī�޶� Ȧ�κа� ����� �ҷ� �κ��� ��ġ�� �κ��� ���� �ҷ��� 60%�� �Ѵ´ٸ� ī�޶� Ȧ�� �����ִ� ���� �� ����
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


// �Է°����� ���� Wafer�� ������ ã��
int BumpInspection::FindWarferArea(cv::Mat& matSrc, int nFristPositionThreshold, float fInspectionCheckArea)
{
	// ���� Ȯ���� ���Ͽ� �Է� Paramter�� threshold ����
	cv::Mat matThresholdROI;
	cv::threshold(matSrc, matThresholdROI, nFristPositionThreshold, 1, THRESH_BINARY);

	// ���Ҷ� ����
	cv::Rect rectSumROI(0, 0, matSrc.cols, matSrc.rows);

	// bianaryȭ �� ������ ������ ������ Ȯ�� 
	size_t sizeCheck = 0;
	sizeCheck = (size_t)cv::sum(matThresholdROI(rectSumROI))[0];

	// ���� ROI�� ����� ���� ���� Ȯ��
	size_t sizeArea = matSrc.cols * matSrc.rows;
	float sizeAreaRatio = (float)sizeCheck / (float)sizeArea * 100;

	// ��ü ������ % ���� Ȯ��
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
	// �ʱ� ��ġ ã�� �� ���� ���� ���� ã���� ���ƴٴϸ鼭 ã�� 
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
				// �ʱ� ��ġ ã��... 
				if (bFirst)
				{
					if (stParam->bImageDirection)
					{
						// ó�� ã�� �������� Align Mark�� ã�Ѵ��� Ȯ��
						if (classBump.SearchAlignMark2(stParam, stParam->matOrgImage(ROI), stParam->vecmatAlignMark, i, ROI.height, true) != 0)
							// ã�� ���ߴٸ�
							bFirst = true;
						else
						{
							// ã�Ҵٸ� 
							bFirst = false;
							break;
						}
					}
					else
					{
						// ó�� ã�� �������� Align Mark�� ã�Ѵ��� Ȯ��
						if (classBump.SearchAlignMark2(stParam, stParam->matOrgImage(ROI), stParam->vecmatAlignMarkInvers, i, ROI.height, true) != 0)
							// ã�� ���ߴٸ�
							bFirst = true;
						else
						{
							// ã�Ҵٸ� 
							bFirst = false;
							break;
						}
					}

					

				}
			}
		}
	}
	// ������ ��ġ 
	while (bFlag)
	{
		classBump.SearchAlignMark2(stParam, stParam->matOrgImage, stParam->vecmatAlignMark, i, 0, false);
		
		// Test
		if (stParam->bImageDirection)
		{
			// End �߰� �ʿ�
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
	// ó����ġ ã�� ���
	if (bType)
	{
		// Align Mark�� ������ �ϰ�� ���
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
		// TemplateMatching ��� ���� �̸� �Ҵ�
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
		// Noise ���� 
		cv::cuda::bilateralFilter(gpumatOrigin, gpumatBilateralOrigin, -1, 15, 15);
	

		// �޸� ����
		gpumatOrigin.release();
		

		//Sobel X, Y Filter ����
		cv::Ptr<cuda::Filter> SobelfilterX = cv::cuda::createSobelFilter(CV_8U, CV_32F, 1, 0);
		cv::Ptr<cuda::Filter> SobelfilterY = cv::cuda::createSobelFilter(CV_8U, CV_32F, 0, 1);

		// ����
		SobelfilterX->apply(gpumatBilateralOrigin, gpumatSobelOriginX);
		SobelfilterY->apply(gpumatBilateralOrigin, gpumatSobelOriginY);

		gpumatBilateralOrigin.release();

		// ���밪���� ��ȯ
		cv::cuda::abs(gpumatSobelOriginX, gpumatSobelOriginAbsX);
		cv::cuda::abs(gpumatSobelOriginY, gpumatSobelOriginAbsY);

		// �޸� ����
		gpumatSobelOriginX.release();
		gpumatSobelOriginY.release();

		// �ο��� ��ġ�� ������ 0.5�� ���� 
		cv::cuda::addWeighted(gpumatSobelOriginAbsX, 0.5, gpumatSobelOriginAbsY, 0.5, 0, gpumatSobelOriginAbsXY);

		// �޸� ����
		gpumatSobelOriginAbsX.release();
		gpumatSobelOriginAbsY.release();

		// ��ģ���� 8bit�� ��ȯ
		gpumatSobelOriginAbsXY.convertTo(gpumatSobelOriginAbsXY8Bit, CV_8U);

		// �޸� ����
		gpumatSobelOriginAbsXY.release();

		SobelfilterX.release();
		SobelfilterY.release();

		cv::cuda::bilateralFilter(gpumatAlignMark, gpumatBilateralMark, -1, 15, 15);
		gpumatAlignMark.release();

		SobelfilterX = cv::cuda::createSobelFilter(CV_8U, CV_32F, 1, 0);
		SobelfilterY = cv::cuda::createSobelFilter(CV_8U, CV_32F, 0, 1);

		// Sobel ����
		SobelfilterX->apply(gpumatBilateralMark, gpumatSobelMarkX);
		SobelfilterY->apply(gpumatBilateralMark, gpumatSobelMarkY);

		SobelfilterX.release();
		SobelfilterY.release();

		// �޸� ����
		gpumatBilateralMark.release();

		// ���밪���� ��ȯ
		cv::cuda::abs(gpumatSobelMarkX, gpumatSobelMarkAbsX);
		cv::cuda::abs(gpumatSobelMarkY, gpumatSobelMarkAbsY);

		// �޸� ����
		gpumatSobelMarkX.release();
		gpumatSobelMarkY.release();

		// �ο��� ��ġ�� ������ 0.5�� ���� 
		cv::cuda::addWeighted(gpumatSobelMarkAbsX, 0.5, gpumatSobelMarkAbsY, 0.5, 0, gpumatSobelMarkAbsXY);

		// �޸� ����
		gpumatSobelMarkAbsX.release();
		gpumatSobelMarkAbsY.release();
		// ��ģ���� 8bit�� ��ȯ
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

		// �޸� ����
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
		//���� ����ȭ // �̰� �ʹ� �����ɸ�..
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

		// ���� �� �´ºκ� �ĺ��� ����
		cv::Mat matTemplateThreshold;
		// Threshold�� �Ķ���ͷ� ������
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

		// �츮�� ã�� �����Ͱ� ���� �ٽ� ���� �ö󰡼� �ٽ� �˻�
		if (nTotalLabel == 0)
		{
			return 1;
		}
		SetMapStartPosition(stParam);

		TemplateResultPositionCheck(stParam, nTotalLabel, matSrc, matTemplateResult, matLabel, matStats, vecmatAlignMark[0].cols, vecmatAlignMark[0].rows, vecFoundMark, vecRectMark, nStartYPos * nCutLength);
		//ExtractAlignMark(nAlignMarkType, vecFoundMark, vecRectMark, vecCenterPoint);
		// ��ġ ����
		// ó�� Align Mark�� �����ϴ� Map ��ġ�������� �����ؾ���
		// �� ó����ġ ��ǥ �˷������..

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
		// blob ����
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

		// ���� ������� ����ó��
		if (nSX < 0)	nSX = 0;
		if (nSY < 0)	nSY = 0;
		if (nSX >= matTemplateResult.cols)	continue;
		if (nSY >= matTemplateResult.rows)	continue;
		if (nEX >= matTemplateResult.cols)	nEX = matTemplateResult.cols - 1;
		if (nEY >= matTemplateResult.rows)	nEY = matTemplateResult.rows - 1;

		cv::Rect rectTemp(nSX, nSY, nEX - nSX + 1, nEY - nSY + 1);

		cv::Mat matTmp_src = matTemplateResult(rectTemp);		//	������ ROI
		cv::Mat matTmp_label = matLabel(rectTemp);				//	Label �� ROI

		double dMin = 1e+10;
		int nYPos = -1;
		int nXPos = -1;
		for (int y = 0; y < rectTemp.height; y++)
		{
			int		*ptrLabel = (int *)matTmp_label.ptr(y);
			float	*ptrGray = (float *)matTmp_src.ptr(y);

			for (int x = 0; x < rectTemp.width; x++, ptrLabel++, ptrGray++)
			{
				// �ش� ��ü
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

		// �뷫�� ��ġ ã��
		if (dMin != 1e+10)
		{
			// ó�� �Է� ���� ��ġ���� ã��
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

			// ���� ��ġ�� �����Ͽ� �־���
			vecRectMark.push_back(rectPos);

			
			// ó�� ��ġ Ȯ��
			// ������
			if (stParam->bImageDirection)
			{
				int nX = rectPos.x / (stParam->fDieWidth * 1000);
				int nCameraScanPositionX = stParam->nCameraScanPositionX;
				int nCameraScanPositionY = stParam->cpStartPoint.y;
				int nWaferMapX = nCameraScanPositionX + nX;

				if (stParam->structMapDie[nCameraScanPositionY][nWaferMapX].nDie_Class == GOOD_DIE)
				{
					// ó����ġ Ȯ�� ����
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
			else// ������
			{
				int nX = rectPos.x / (stParam->fDieWidth * 1000);
				int nCameraScanPositionX = stParam->nCameraScanPositionX;
				int nCameraScanPositionY = stParam->cpStartPoint.y;
				int nCamScanDie = stParam->nCamScanDie - 1;

				int nWaferMapX = nCameraScanPositionX + (nCamScanDie - nX);

				if (stParam->structMapDie[nCameraScanPositionY][nWaferMapX].nDie_Class == GOOD_DIE)
				{
					// ó����ġ Ȯ�� ����
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


	// AlignMark ��� ������ 1�� �̻��� ���� ���
	if (nAlignMarkSize > 1)
	{
		bool bOffsetFirst = true;
		// �̰� ���� �ؾ���.
#pragma omp parallel for schedule(dynamic)
		for (int x = nStartX; x < nSizeX; x++)
		{
			// ù��° ��ġ�� �������� ������ ��ġ�� ���� ã��
			// �ϳ��� ������ �� ã�� �ŷ� ó��
			// ���� ���� ã�ƾ��ϴ��� Ȯ���ϴ� ó��
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
			// �ӽ÷� ��ġ ���� ����
			vector<cv::Point> vecTemp;
			// ���� �ȵ� ���� �����ؾ���

			// ó�� ��ġ 
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

				// ���� ó�� �ʿ�
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

				// ���� ���� ���� ����ó�� �ʿ�
				
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

				// mask ��밡������ Ȯ�� �ʿ�
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
						// �ι�° ��ġ
						vecTemp.push_back(cv::Point(nNextStartX + stParam->vecmatAlignMark[m + 1].cols, nNextStartY));
					}
					else if (m == 1)
					{
						// ����° ��ġ
						vecTemp.push_back(cv::Point(nNextStartX + stParam->vecmatAlignMark[m + 1].cols, nNextStartY + stParam->vecmatAlignMark[m + 1].rows));
					}
					else if (m == 2)
					{
						// �׹�° ��ġ
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
					// ���� �� ã������ ����ó�� �ʿ�
				}
			}

			if (bFlagNext)
			{
				// ã�� ��ġ�� ��� ���� 
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
								// ROI������ ��ǥ�� ����
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
				// �� ã���� ��
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

// ���� ��ġ�� ���� ��ġ ����
int BumpInspection::NextAlingMarkPosition(stInspectThreadParameter* stParam)
{
	bool bFind = true;

	// X ���� Scan�ϴ� ������ ���� ��ĵ�ϴ� ��ġ
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

	// �ϴ� �Ʒ������� ã��
	vector<cv::Point> vecPoint;
	vector<cv::Rect> vecRect;

	//vecPoint.resize(nCamScanDie);
	//vecRect.resize(nCamScanDie);
#pragma omp parallel for schedule(dynamic)
	for (int x = nCheckStartX; x < nCheckSizeX; x++)
	{
		// �Ѱ��� ã�Ƽ� ���̳� �������� Ȯ���ؾߵǴ� ���... ����ó�� �ʿ�
		cv::Mat matAlignROI;

		// ó�� ��ġ 
		cv::Rect rectROI(stParam->structMapDie[stParam->cpStartPoint.y][x].vecPoint[0].x - stParam->nAlignSearchPitch,
			stParam->structMapDie[stParam->cpStartPoint.y][x].vecPoint[0].y - stParam->nAlignSearchPitch + stParam->nAlignMarkDistanceY,
			stParam->vecmatAlignMark[0].cols + stParam->nAlignSearchPitch * 2,
			stParam->vecmatAlignMark[0].rows + stParam->nAlignSearchPitch * 2);

		// ���� ó��
		// GijunAdd 220603
		// stParam->structMapDie[stParam->cpStartPoint.y][x].vecPoint[0].x �� 0 �ΰ�찡 �߻��ϸ�, Pitch�� ���� - ��ǥ�� �Ѿ��...
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
			// ���� �ڵ�?
			continue;
		}

		// Align Mark ��ġ ��� 
		stParam->matOrgImage(rectROI).copyTo(matAlignROI);

		// TemplateMatching���� ������ ����
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

		// ���� ���� ���� ��ġ ã��
		// ��ġ �� �� ã�� ��찡 ���� �� ���� ����ó�� �ʿ�
		cv::Point cpMin;
		double dMin = 0;
		cv::minMaxLoc(matTemplateResult, &dMin, NULL, &cpMin, NULL);

		// AligMark�� �߽����� ������ �ٽ� ���
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

	// ��ã�� ��ġ�� ���� �Ҷ�

	//if (vecPoint.size() != nCamScanDie)
	//{
	//	for (int i = 0; i < nCamScanDie; i++)
	//	{
	//		if (pDieArr[cpStartPoint.y][i].bFoundMark == false)
	//		{
	//			// �ι�° Align Mark�� ã�� ��ġ�� ����ؾ���
	//			// ã�� ��ġ ���� AlignMark ��ġ Ȯ�α��� ���ο��� �����ؾ���..
	//			// FindAligMarkSecond();
	//			// 
	//			// �̷��� ����� �ȵȴٸ�.. �ٸ� ��� ã�ƾ���..
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

	// Die �˻�� ���� ��ġ�� �߽����� �ʿ��� ���� �յڷ�
	int nScan = stParam->nInspectionScanDie / 2; // ���� 3/2 �̹Ƿ� 1
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
			// x ����ó��.
			// ó�� �������� 3x3 ���� 0���� ������,
			if (x - nScan < nCheckStartX) {
				nOffsetX = x + nScan;
			}
			else if (x + nScan > nCheckSizeX - 1) { // ������ ����
				nOffsetX = x - nScan;
			}
			else { // ó���� �ƴϰ� ������ ������ �ƴѰ��,
				// �״�� �α�...
				nOffsetX = x;
			}

			for (int y = 0; y < nMapHeight; y++)
			{
				int nOffsetY = 0;
				bool bInspectFlag = false;
				// y ����ó��
				// ó���κа�
				if (y - nScan < 0)
				{
					nOffsetY = y + nScan;
				}
				// ������ �κ� ����
				else if (y + nScan > nMapHeight - 1)
				{
					nOffsetY = -nScan;
				}
				// ���� ��ġ�� 0���� ū�� Ȯ��
				else
				{
					// ���� ��ġ Ȯ��
					// ���� Die�� �˻����� �ʴ� �κ��̰� ���۵Ǵ� ��ġ��� 
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
				// �Է��� ���� ��ŭ ������ ã�Ƽ� �˻�� �Ѱ���
				if (stParam->structMapDie[y][x].nDie_Class == GOOD_DIE &&
					stParam->structMapDie[y][x].bFoundMark == true)
				{
					// ���� ����� �� �� ���� Ȯ���ϴ� Count ����
					int nInspectCnt = 0;
					for (int nDieY = -nScan + nOffsetY; nDieY < nScan + nOffsetY; nDieY++) {
						for (int nDieX = nOffsetX - nScan; nDieX < nOffsetX + nScan + 1; nDieX++) {
							if (stParam->structMapDie[y + nDieY][nDieX].nDie_Class == GOOD_DIE &&
								stParam->structMapDie[y + nDieY][nDieX].bFoundMark == true) {
								// ���� ���� �Ѿ
								vecPoint.push_back(cv::Point(nDieX, y + nDieY));
								nInspectCnt++;
								continue;
							}
							else {
								bInspectFlag = false;
							}
						}
					}
					// ���� ���� ���� �̻� �˻��ȰŸ� PASS
					if (nInspectCnt > stParam->nInspectionScanDie) {
						bInspectFlag = true;
						nInspectCnt = 0;
					}						
					else nInspectCnt = 0;

					// scan ���з� �Է� �ٽ� �˻��ϰ�
					if (!bInspectFlag)
					{
						vecPoint.clear();
						std::vector<Point>().swap(vecPoint);
					}
					else {
						// ���� (���� ���� ���� �̻� �˻���.)
						// �˻� ����
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
			// Ȥ�� ������ ������..
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
			// ����ִ� ���߿� �ϳ��� ������ ����
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
						// ũ�⸦ ���߱� ���Ͽ� ���۸� ���� �� �����͸� ����
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

	// ������ ������ ũ�Ⱑ ���� ���ƾ� ��
	size_t nImageSize = vecmatSample[0].cols * vecmatSample[0].rows;

	matResult = cv::Mat::zeros(vecmatSample[0].size(), vecmatSample[0].type());
	cv::Mat matSum = cv::Mat::zeros(vecmatSample[0].size(), CV_32FC1);
	uchar* ucpResultData = (uchar*)matResult.data;
	float* fDataSum = (float*)matSum.data;
	// ���� �ʼ�?
#pragma omp parallel for schedule(dynamic)
	for (int j = 0; j < nImageSize; j++)
	{
		// ��
		for (int i = 0; i < nSampleSize; i++)
		{
			uchar* ucpData = vecmatSample[i].data;
			fDataSum[j] += ucpData[j];
		}
		// ���
		float fAvg = fDataSum[j] / (float)nSampleSize;

		// Median �� ���ϴµ�, Cnt�ؼ� ��� �� �Ʒ� �ټ� ���� �����ͷ� ���� ����� �����.
		int nCntUpper = 0;
		int nCntLower = 0;
		float fNewSumValue = 0.0f;
		// 3�� �� ��պ��� ū�� ������ ������ ������ Count.
		for (int i = 0; i < nSampleSize; i++)
		{
			uchar* ucpData = vecmatSample[i].data;
			if (ucpData[j] >= fAvg) nCntUpper++;
			else nCntLower++;
		}
		fDataSum[j] = 0.0;
		// ��պ��� ������ �� ���� ���,
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
		else if (nCntUpper == nCntLower) { // ��պ��� ����Cnt�� ���� Cnt�� �������!!
			// ��հ� �Է�
			uchar ucData = (uchar)roundf(fAvg);
			ucpResultData[j] = ucData;
			continue; //�ƹ��͵� ����. Avg=avg �� �Է�.
		}
		else { // ��պ��� ������ �� ���� ���.
			for (int i = 0; i < nSampleSize; i++)
			{
				uchar* ucpData = vecmatSample[i].data;
				if (ucpData[j] < fAvg) {
					fDataSum[j] += ucpData[j];
				}
			}
			fAvg = fDataSum[j] / nCntLower;
		}

		// ��հ� �Է�
		uchar ucData = (uchar)roundf(fAvg);
		ucpResultData[j] = ucData;
	}
	return 0;
}

int BumpInspection::Inspection(stInspectThreadParameter* stParam, cv::Mat Die1, cv::Mat Die2, cv::Mat& Result)
{
	// ���� ���� ũ�� Ȯ��
	if (Die1.cols != Die2.cols || Die1.rows != Die2.rows)
		return 1;

	Result = cv::Mat::zeros(Die1.size(), Die1.type());

	// ���� ���������� �ϳ��� ���� 
	int nWidth = Die1.cols;
	int nHeight = Die1.rows;

	// �ܰ�ó�� 
	int nSearchY = stParam->nDiffSearchY;
	int nSearchX = stParam->nDiffSearchX;
	int nStartY = nSearchY;
	int nStartX = nSearchX;
	int nEndY = nHeight - 1 + nSearchY;
	int nEndX = nWidth - 1 + nSearchX;

	// ������� Ȯ��
	cv::Mat matBorderSrc1;
	cv::Mat matBorderSrc2;
	cv::copyMakeBorder(Die1, matBorderSrc1, nSearchY, nSearchX, nSearchX, nSearchY, BORDER_REFLECT);
	cv::copyMakeBorder(Die2, matBorderSrc2, nSearchY, nSearchX, nSearchX, nSearchY, BORDER_REFLECT);

	cv::GaussianBlur(matBorderSrc1, matBorderSrc1, cv::Size(3, 3), 1.4);
	cv::GaussianBlur(matBorderSrc2, matBorderSrc2, cv::Size(3, 3), 1.4);

	// ������ ���� Pointer �ѱ�
	uchar* upData1 = matBorderSrc1.ptr(0);
	uchar* upData2 = matBorderSrc2.ptr(0);
	uchar* upResult = Result.data;

	int nSrcStep = matBorderSrc1.step;
	int nDstStep = Result.step;

	// SIMD ��ɾ�� ����? Ȯ�� �ʿ� 
	// ������ Ȯ�� 
	//#if defined(_OPENMP) && defined(NDEBUG)
#pragma omp parallel for schedule(dynamic)
	//#endif
	for (int y = nStartY; y < nEndY; y++)
	{
		for (int x = nStartX; x < nEndX; x++)
		{
			// �ּҰ� Ȯ�ο�
			int nMinDiffValue = 256;
			int nCurrentDiff = 0;
			// ���̰� ��ġ ���
			int nStartM = y - nSearchY;
			int nEndM = y + nSearchY;
			// ���Ⱚ ��ġ ���
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
						// Wafer ���ο� �ִٸ�,
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
						// Wafer ���ο� �ִٸ�,
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
						// Wafer ���ο� �ִٸ�,
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
						// Wafer ���ο� �ִٸ�,
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
				// Wafer ���ο� �ִٸ�,
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

	// ���� �о����.
	CString strMapPath = strPath;
	std::ifstream file(strMapPath);

	if (true == file.is_open()) {
		std::string strValue;
		// �켱 ���� �ִ��� Ȯ��.
		while (!file.eof()) {
			std::getline(file, strValue);
			if (!strValue.find("[CENTER]"))	break;
			else nYCnt++;
		}
		file.clear();
		file.seekg(0, std::ios_base::beg); //���� ó������ �̵�
										   // �� �� ���� �����Ͱ� �ִ��� Ȯ��.
		while (!file.eof()) {
			std::getline(file, strValue, ',');	// �޸� �������� ���پ� ������ ���� vector�� ����.
			if (!strValue.find("[CENTER]")) break;
			else vecWaferMap.push_back(stoi(strValue));
		}
		nXCnt = vecWaferMap.size();
		nXCnt /= nYCnt;	// X Arr Cap ���
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
			// Die Rect ��ǥ ����
			cv::Rect2f rcDie(x*(fDieWidth)
				, y*(fDieHeight)
				, fDieWidth
				, fDieHeight);
			// ��ǥ�� ���� Vector�� ������ Good Die�� �Բ� ��� Structure �� ����.
			DieArr[y][x].fX = rcDie.x;
			DieArr[y][x].fY = rcDie.y;
			DieArr[y][x].fCenterX = rcDie.x + (rcDie.width / 2.0);
			DieArr[y][x].fCenterY = rcDie.y + (rcDie.height / 2.0);
			if (vecWaferMap[x + (y*nWidthCnt)] >= 1) { // Center ǥ�� 2�� �صξ���;;
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

	// if (Math.pow(R, 2) > (Math.pow(X - TX, 2) + Math.pow(Y - TY, 2))) { ���� = true; }
	// ���
	// ���
	deltaY = CircleCenterY - IsRect.y;
	deltaX = CircleCenterX - IsRect.x;
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nInCircle += 1;
	}

	// ���
	// ���
	deltaY = CircleCenterY - IsRect.y;
	deltaX = CircleCenterX - (IsRect.x + IsRect.width);
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nInCircle += 1;
	}

	// ���
	// ���
	deltaY = CircleCenterY - (IsRect.y + IsRect.height);
	deltaX = CircleCenterX - IsRect.x;
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nInCircle += 1;
	}

	// ���
	// ���
	deltaY = CircleCenterY - (IsRect.y + IsRect.height);
	deltaX = CircleCenterX - (IsRect.x + IsRect.width);
	length = (deltaX * deltaX) + (deltaY * deltaY);
	if (length > pow(CircleRadius, 2)) {
		nInCircle += 1;
	}

	// ��  ��
	//   ��
	// ��  ��
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
