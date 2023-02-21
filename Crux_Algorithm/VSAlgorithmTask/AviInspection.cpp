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

		// Mura - �̴ϸʿ� Rect �ȱ׸�
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_MURA_START	&&
			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_MURA_END)
		{
			// �̴ϸʿ� Rect �ȱ׸�
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_MULT_BP 
				&& resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT
				&& resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_EMD_DARK
				&& resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT
				&& resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_NUGI)

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

			//choi ����Ʈ

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

			//choi ����Ʈ
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

// �ܰ� �κ� ó��
long AviInspection::makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio)
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

bool AviInspection::Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
	const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, const int nCamNum,
	ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStageNo)
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

		// 2�� ���� LCP
		//
		//////////////////////////////////////////////////////////////////////////
		//�˻� Ȯ�� Path ����
		wchar_t wstrInspConfirmPath[MAX_PATH] = { 0, };

		CString				strPanelID_;
		strPanelID_.Format(_T("%s"), strPanelID);

		CString		stBasic_Path;
		stBasic_Path.Format(_T("%s\\�˻�Ȯ��"), theApp.m_Config.GetNetworkDrivePath() + strDrive);
		//CString		stBasic_Path = _T("D:\\NetworkDrive\\D_Drive\\�˻�Ȯ��");
		//if (nCommonPara[1] > 0) CreateDirectory(stBasic_Path, NULL);
		if (true) CreateDirectory(stBasic_Path, NULL); //LCP ����̹���
		swprintf(wstrInspConfirmPath, _T("%s\\%s"), (LPCWSTR)stBasic_Path, (LPCWSTR)strPanelID_);
		//if (nCommonPara[1] > 0) CreateDirectory(wstrInspConfirmPath, NULL);
		if (true ) CreateDirectory(wstrInspConfirmPath, NULL); //LCP ����̹���

		int nCameraNum_ = nCamNum;

		double dMoi_per = 0.0; //���� Log Ȯ�ο�
		double dVari_per = 0.0; //���� Log Ȯ�ο�
		//////////////////////////////////////////////////////////////////////////

		// Report ����
		JudgementDelReport(resultPanelData);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Del Report. Panel ID : %s"), strPanelID);
		
	
		//JudgementSpot(resultPanelData);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Spot. Panel ID : %s"), strPanelID);

		// PNZ:17.11.27 ���� �ߺ� �κ� ������ Line Merge
		// �Ķ���� �޾ƿ���
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
//		Delet Some Line Defect ����
//		������ �ֺ� ����� ����
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
// 		// Crack ����
// 		JudgementCrack(resultPanelData);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Crack Pattern Judge End. Panel ID : %s"), strPanelID);
// 
// 		// PCD CRACK ���� ( ��˻� ���� 5.99" )
// 		JudgementPCDCrack(resultPanelData, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - PCD Crack End. Panel ID : %s"), strPanelID);
// 
// 		// 2018-07-31 Camera Tap�� ���� ���� ����
// 		JudgementCameraTapOverKill(resultPanelData, MatOrgImage, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Camera Tap OverKill Delet End. Panel ID : %s"), strPanelID);
// 
// 		// DGS ����
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
// 		// ���� ���� �˰���
// 		DeleteOverlapDefect_DimpleDelet(resultPanelData, MatOrgImage, dAlignPara);		//choikwangil boe11 �ӽ� �ּ� 19 08 09
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Dimple Overkill Defect End. Panel ID : %s"), strPanelID);
// 
// 		// 2018-04-19 ver. 0.0.3 PNZ
// 		// �ѵ��� Spot ���� �˰���
// 		//�ߺ����� �ڷ� �̵�
// // 		DeleteOverlapDefect_SpecInSpotDelet(resultPanelData, MatOrgImage, dAlignPara);
// // 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Spot Overkill Defect End. Panel ID : %s"), strPanelID);
// 
// 		//������
// 		//DeleteOverlapDefect_BlackHole(resultPanelData, MatOrgImage, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Black Hole Spot Overkill Defect End. Panel ID : %s"), strPanelID);
// 
// 		// 2018-05-17 ver. 0.0.1 PNZ
// 		// ���� Ȯ�� �˰���
// 		DeleteOverlapDefect_DustDelet(resultPanelData, MatOrgImage, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Dust Defect End. Panel ID : %s"), strPanelID);
// 
// 		// 2018-04-19 ver. 0.0.1 PNZ
// 		// �ѵ��� Spot ���� �˰���
// 		DeleteOverlapDefect_BlackSmallDelet(resultPanelData, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Black Pattern Small Point Defect End. Panel ID : %s"), strPanelID);
// 
// 		// white spot ������ judge
// 		//Judge_Defect_BP_WSpot(resultPanelData, dAlignPara);
// 
// 		DeleteOverlapDefect_SpecInSpotDelet(resultPanelData, MatOrgImage, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Spot Overkill Defect End. Panel ID : %s"), strPanelID); //05_26 choi white mura �ߺ� ���� ������ �ű�
// 
// 	 	// �ߺ� ��ǥ ����
// 	 	DeleteOverlapDefect(resultPanelData, dAlignPara);
// 	 	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Overlap Defect End. Panel ID : %s"), strPanelID);
// 
// 		
// 		
// 		//choikwangil
// 		//Judge_DefectSize(resultPanelData, dAlignPara);
// 		// Black Pattern ���� Merge ����
// 		JudgementBlackPatternMerge(resultPanelData);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Black Pattern Merge End. Panel ID : %s"), strPanelID);
// 
// 		// KSW 17.09.11 - ȥ�� ���� �߻� -> ���� ���� ��� ET ����
// 		// ���ο��� End �α� ����
// 		JudgementET(resultPanelData, dAlignPara, strPanelID);
// 
// 		// KSW 17.09.11 - ���� �κ� ���� -> Pad Bright ����
// 		//JudgementEdgePadBright(resultPanelData, dAlignPara);
// 		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Edge Pad Bright End. Panel ID : %s"), strPanelID);
// 
// 		JudgeCHoleJudgment(resultPanelData, tCHoleAlignData, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - CHole Judgement End. Panel ID : %s"), strPanelID);

		// ������ Panel Size - Resolution ���
		double dResolution = calcResolution(WrtResultInfo);

		// KSW 17.09.25 - ���� & ���� ����
		//JudgeGroup(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
		//JudgeGroupTEST(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
		//JudgeGroupJudgment(resultPanelData, MatDrawBuffer, dAlignPara, dResolution);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Group End. Panel ID : %s"), strPanelID);

		// PNZ 20.04.09 Point G64 GDS Judge
		//JudgePointGDStoFgarde(resultPanelData, dAlignPara);

		// PNZ 19.04.15 Mura Normal �з�
		//JudgementMuraNormalClassification(resultPanelData, dAlignPara);
		//theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - MuraNormal Classification End. Panel ID : %s"), strPanelID);

		// PNZ 19.08.15 Mura Normal Type 3 Filtering + PNZ 19.09.20 CHole �ֺ� T3 Filtering �߰�
// 		JudgementMuraNormalT3Filter(resultPanelData, tCHoleAlignData, dAlignPara);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - MuraNormal T3 Filtering End. Panel ID : %s"), strPanelID);

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
// 		JudgeSaveMuraContours(resultPanelData, strContoursPath);
// 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Mura Contours End. Panel ID : %s"), strPanelID);
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

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judgement End. Panel ID : %s (%s)"), strPanelID, resultPanelData.m_ResultPanel.Judge);

	return true;
}

bool AviInspection::JudgementPointToLine(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, const int nImageWidth, const int nImageHeight)
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

// bool AviInspection::JudgeGroup(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
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

// bool AviInspection::JudgeGroupTEST(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
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

bool AviInspection::JudgeGroupJudgment(ResultPanelData& resultPanelData, cv::Mat(*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution)
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
	int		nDefect_PD_Count = 0;
	/////////////////////////////////////choikwangil �߰�
	// ��� group judge �� ���
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

	/////////////////////////////////////////////////////////////////////////////choikwangil �߰�
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
	// ���� ���� ���� ũ���� Ȯ��
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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	return true;
}
////////////////////////////////////////////////////////////////////////////////CHOIKWANGIL�߰�
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
// Mura Normal �ҷ� �з�
//////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementMuraNormalClassification(ResultPanelData& resultPanelData, double* dAlignPara)
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
		case E_DEFECT_JUDGEMENT_MURA_LINEMURA_X:	nMuraType2_Count++; break;
		case E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y:	nMuraType2_Count++; break;
		case E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG:		nMuraType3_Count++; break;
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
			if (nDefectType == E_DEFECT_JUDGEMENT_MURA_LINEMURA_X || nDefectType == E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y)
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
// PNZ 2019-08-15
// Type 3 ���� ���� ��� : �ѵ� Cell ���� Merge
//
//////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementMuraNormalT3Filter(ResultPanelData& resultPanelData, tCHoleAlignInfo tCHoleAlignData, double* dAlignPara)
{
	// ����ó��
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)	return true;

	double	dbTHDiffGV_BBS = (double)dAlignPara[E_PARA_MURANORMAL_OR_TYPE3_BBS_DIFFGV];
	double	dbTHDiffGV_SBS = (double)dAlignPara[E_PARA_MURANORMAL_OR_TYPE3_SBS_DIFFGV];
	double	dbTHDiffGV_CH = (double)dAlignPara[E_PARA_MURANORMAL_OR_TYPE3_CH_DIFFGV];
	int		nBubbleMergePara = (int)dAlignPara[E_PARA_MURANORMAL_OR_TYPE3_BUBBLEMERGE];

	int		nOffSet = 200;

	// ��� ��� ���ϸ� Reture
	if (dbTHDiffGV_BBS == 0 && dbTHDiffGV_SBS == 0 && dbTHDiffGV_CH == 0 && nBubbleMergePara == 0) return true;

	//////////////////////////////////////////////////////////////////////////
	// Camera Hole �ֺ� T3 ���� ����
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount();)
	{
		// �� �ٽ� �޾� ����
		dbTHDiffGV_CH = (double)dAlignPara[E_PARA_MURANORMAL_OR_TYPE3_CH_DIFFGV];

		// CHole Para ������� Ȯ��
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

			// 19.11.14 CHole �ֺ� �ҷ� ���� �߰�, ���� Size �ҷ� Diff ���� ����
			double dbDefectArea = resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel;

			if (dbDefectArea <= 3000) dbTHDiffGV_CH = 5;

			if (dbDefectArea >= 50000) { i++; continue; }

			if (dbDiffGV_CHole <= dbTHDiffGV_CH) bOverKill_GVCheck = true;

			// CHole �ֺ� ���� Ȯ��
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

				// CHole �ֺ��� ����
				if (nXDCheck == 1 && nYDCheck == 1) { bOverKill_Locatoin = true;  break; }
			}

			// �ҷ� ����
			if (bOverKill_GVCheck == true && bOverKill_Locatoin == true)
				resultPanelData.m_ListDefectInfo.RemoveAt(i);

			else { i++; continue; }
		}

		else { i++; continue; }
	}


	//////////////////////////////////////////////////////////////////////////
	// Bubble Merge Logic

	if (nBubbleMergePara != 0) // ��� �ؾ� ����
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

		if (nBubbleDefectCount != 0) // Bubble �ҷ��� �־�� ��
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

				// CHole �ֺ� ���� Ȯ��
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

	// �ѵ� ����
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

	// Type 3 BBS �ҷ� Filtering ( �ѵ� Cell�� �� ��� )
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

long AviInspection::StartLogicAlgorithm(const CString strDrive, const CString strLcp_Bubble_Path ,const tLogicPara& LogicPara, cv::Mat& MatColorBuffer, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
	cv::Mat& MatDrawBuffer, const int nImageNum, const int nROINumber, const int nAlgorithmNumber, tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	// �Ķ���� ����
	cv::Mat				MatOriginImage_ = LogicPara.MatOrgImage;
	int					nAlgorithmNumber_ = nAlgorithmNumber;					// ���� �˰���	
	int					nCameraNum_ = LogicPara.nCameraNum;				// �˻� ī�޶�
	int					nThrdIndex_ = LogicPara.nThreadLog;
	cv::Mat*			MatOriImageRGB_[3];
	cv::Mat				MatBKG_ = LogicPara.MatBKG;					// Mura���� ��� ( Cell �̿� ���� ) / Dust, Black ������ ����

	ENUM_KIND_OF_LOG	eLogCamera = (ENUM_KIND_OF_LOG)nCameraNum_;

	// ���� �г� ID
	CString				strPanelID_;
	strPanelID_.Format(_T("%s"), LogicPara.tszPanelID);

	long lErrorCode = E_ERROR_CODE_TRUE;

	// �˰��� ��� ���� ���� ����
	SYSTEMTIME time;
	::GetLocalTime(&time);

	// ���� �˰��� �˻� �Ķ���� ��������
	double* dAlgPara = theApp.GetAlgorithmParameter(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);
	double*	dAlignPara = theApp.GetAlignParameter(nCameraNum_);

	
	int nColorCode = (int)dAlignPara[E_PARA_LCP_COLORCODE_NUM];
	

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

	CString strResultTime;
	strResultTime.Format(_T("%s%sARESULT"), theApp.m_Config.GetNetworkDrivePath(), strDrive);
	if (nCommonPara[1] > 0) CreateDirectory(strResultTime, NULL); //LCP �߰��̹���

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
	if (true) CreateDirectory(wstrAlgPath, NULL); //LCP �߰��̹���

	// Alg �α׿�
	wchar_t strAlgLog[MAX_PATH] = { 0, };
	swprintf(strAlgLog, _T("ID:%s\tPat:%s"), (LPCWSTR)strPanelID_, theApp.GetGrabStepName(nImageNum));

	//LCP Area Camera Bubble �˻�� Auto Run �ÿ�
	wchar_t wstrInspBubblePath[MAX_PATH] = { 0, };
	swprintf(wstrInspBubblePath,_T("%s\\%s\\"), ORIGIN_PATH, (LPCWSTR)strPanelID_);

	//LCP Area Camera Bubble �˻�� Menual �ÿ�
	CString strManualImagePath = _T("");

	if (eInspMode == eManualInspect) {
		strManualImagePath.Format(_T("%s\\"), strLcp_Bubble_Path);
	}

	wchar_t wstrInspBubblePath_Menual[MAX_PATH] = { 0, };
   	swprintf(wstrInspBubblePath_Menual, _T("%s"),(LPCWSTR)strManualImagePath);

	//���Ͼ����� �߰� ���� ���� ��� ���� ����
	wchar_t wstrFinalResultPath[MAX_PATH] = { 0, };

	CString		strFinal_Name = _T("Final Result");

	swprintf(wstrFinalResultPath, _T("%s%s"), wstrAlgPath, (LPCWSTR)strFinal_Name);
	if (true) CreateDirectory(wstrFinalResultPath, NULL); //LCP �߰��̹���

	//////////////////////////////////////////////////////////////////////////
	wchar_t wstrInspConfirmPath[MAX_PATH] = { 0, };
	
	//CString		stBasic_Path = _T("D:\\NetworkDrive\\D_Drive\\�˻�Ȯ��");

	CString		stBasic_Path;
	stBasic_Path.Format(_T("%s\\�˻�Ȯ��"), theApp.m_Config.GetNetworkDrivePath() + strDrive);
	//if (nCommonPara[1] > 0) CreateDirectory(stBasic_Path, NULL);
	if (true) CreateDirectory(stBasic_Path, NULL); //LCP ����̹���
	swprintf(wstrInspConfirmPath, _T("%s\\%s"), (LPCWSTR)stBasic_Path, (LPCWSTR)strPanelID_);
	//if (nCommonPara[1] > 0) CreateDirectory(wstrInspConfirmPath, NULL);
	if (true) CreateDirectory(wstrInspConfirmPath, NULL); //LCP ����̹���
	//////////////////////////////////////////////////////////////////////////

	// Result Buff ���� : LINE�� �ƴ� ��쿡 �� ���
	// ���� �Ҵ� & �ʱ�ȭ

	cv::Mat matReusltImage[E_DEFECT_COLOR_COUNT];
	cv::Mat matDustReusltImage[E_DEFECT_COLOR_COUNT];
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
	// White , Np , Gray Color �˻�
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

			// NP �̸� ������ Active ������� ��ȯ
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

			// C#���� C++�� cv::Mat ������ �޶� ����, ũ�� ���� �Ķ���� �Ѱ����.
			// MFC ����ϹǷ� cv::Mat �״�� �Ѱܵ� ������
			lErrorCode = Point_FindDefect(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT],
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI ,dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode ,EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);

		

			// ���� �� ���Ϻ� Blob �Ǵ� Contour �ϱ�
			//Mask ���ֱ�
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

			// ��� ī�޶� ��� ������ �˻� ���� ���
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

			// �˻� ��� ���� ���� �κ�
			if (nImageNum == theApp.GetImageNum(nInsp_Substance_Main_IMAGE) && nCameraNum_ == 0)
			{
				// ���� ������


				// ���� ���� ����� ����, Back, Side ���� �ҷ� ����
				MatResultImg[theApp.GetImageNum(nInsp_Substance_Compare_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK] -= (MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
					MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
					MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

				CString strFont2;
				strFont2.Format(_T("%s\\�������Ȯ��.jpg"), wstrFinalResultPath ,(LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Compare_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK]);

				// ���̵� ���� ����� ����, Back, Side ���� �ҷ� ����
				MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK] -= (MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
					MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
					MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);


				strFont2.Format(_T("%s\\���̵����Ȯ��.jpg"), wstrFinalResultPath ,(LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK]);


				MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] += MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
				MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT];

			
				strFont2.Format(_T("%s\\%s_DustȮ�ο�.jpg"), wstrInspConfirmPath, (LPCWSTR)strPanelID_);

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

						// 0GV �� �ʱ�ȭ ��, �Ѿ��
						cv::Mat matTempROI = MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK](rcRect);
						cv::Mat matLabelROI = matLabels(rcRect);

						for (int y = 0; y < rcRect.height; y++)
						{
							int		*ptrLabel = (int *)matLabelROI.ptr(y);
							uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

							for (int x = 0; x < rcRect.width; x++, ptrLabel++, ptrGray++)
							{
								// �ش� ��ü
								if (*ptrLabel == idx)	*ptrGray = 0;
							}
						}

						continue;
					}


				}
				
				strFont2.Format(_T("%s\\��ü�ҷ�_����Ʈ ����.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK]);


				matReusltImage[E_DEFECT_COLOR_DARK] =  MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK].clone(); //  Gray ����.... temp_gray;
				//matReusltImage[E_DEFECT_COLOR_BRIGHT] = MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT].clone();

				

				lErrorCode = Point_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_], MatDrawBuffer, stThrdAlignInfo.ptCorner,dAlignPara ,dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);

				// ���� ����
				CRect rectROI = CRect(
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x),
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y),
					max(stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x),
					max(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y));

				// �˻� ������ ���� ũ�⸦ ��� ���
				if (rectROI.left < 0 ||
					rectROI.top < 0 ||
					rectROI.right >= MatOriginImage_.cols ||
					rectROI.bottom >= MatOriginImage_.rows)	return E_ERROR_CODE_ROI_OVER;

				if (rectROI.left >= rectROI.right)	return E_ERROR_CODE_ROI_OVER;
				if (rectROI.top >= rectROI.bottom)	return E_ERROR_CODE_ROI_OVER;


// 				cv::Mat mOk_flip = MatColorBuffer.clone(); // UI ��� �׸����
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

			//�ٸ� �̹��� �̹� Dust �˻� ��� ��ٸ� 
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
			strFont2.Format(_T("%s\\�����_��ü�ҷ�_����Ʈ ����.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
			cv::imwrite((cv::String)(CStringA)strFont2, matReusltImage[E_DEFECT_COLOR_DARK]);


			lErrorCode = Concealment_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatDrawBuffer, stThrdAlignInfo.ptCorner,dAlignPara , dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);
			
			
		}
		break;

		default:
			break;
		}
	}
	// Black Color �˻�
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

			// C#���� C++�� cv::Mat ������ �޶� ����, ũ�� ���� �Ķ���� �Ѱ����.
			// MFC ����ϹǷ� cv::Mat �״�� �Ѱܵ� ������
			lErrorCode = Point_FindDefect_Black(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT],
				stThrdAlignInfo.ptCorner, stThrdAlignInfo.rcAlignCellROI, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, wstrInspBubblePath, wstrInspBubblePath_Menual, eInspMode, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]]);



			// ���� �� ���Ϻ� Blob �Ǵ� Contour �ϱ�
			//Mask ���ֱ�
			cv::bitwise_and(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_Active, matReusltImage[E_DEFECT_COLOR_DARK]);
			cv::bitwise_and(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_Active, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

			cv::subtract(matReusltImage[E_DEFECT_COLOR_DARK], m_stThrdAlignInfo.matMask_Hole, matReusltImage[E_DEFECT_COLOR_DARK]);
			cv::subtract(matReusltImage[E_DEFECT_COLOR_BRIGHT], m_stThrdAlignInfo.matMask_Hole, matReusltImage[E_DEFECT_COLOR_BRIGHT]);

			MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK] = matReusltImage[E_DEFECT_COLOR_DARK].clone();
			MatResultImg[nImageNum][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] = matReusltImage[E_DEFECT_COLOR_BRIGHT].clone();

			bpInspectAlgEnd[nImageNum][nCameraNum_][nAlgorithmNumber_] = true;

			// ��� ī�޶� ��� ������ �˻� ���� ���
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

			// �˻� ��� ���� ���� �κ�
			if (nImageNum == theApp.GetImageNum(nInsp_Substance_Main_IMAGE) && nCameraNum_ == 0)
			{
				// ���� ������


				CString strFont2;
				strFont2.Format(_T("%s\\���� ���� ���� �� Ȯ��.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

				// ���� ���� ����� ����, Back, Side ���� �ҷ� ����
				MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] -= (MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] +
					MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

			
				strFont2.Format(_T("%s\\���� ���� ���� �� Ȯ��.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

				MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT] += MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT];


				strFont2.Format(_T("%s\\%s_DustȮ�ο�.jpg"), wstrInspConfirmPath, (LPCWSTR)strPanelID_);

				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);

				matReusltImage[E_DEFECT_COLOR_BRIGHT] = MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT].clone(); 

				lErrorCode = Point_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_], MatDrawBuffer, stThrdAlignInfo.ptCorner,dAlignPara, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);

				// ���� ����
				CRect rectROI = CRect(
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x),
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y),
					max(stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x),
					max(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y));

				// �˻� ������ ���� ũ�⸦ ��� ���
				if (rectROI.left < 0 ||
					rectROI.top < 0 ||
					rectROI.right >= MatOriginImage_.cols ||
					rectROI.bottom >= MatOriginImage_.rows)	return E_ERROR_CODE_ROI_OVER;

				if (rectROI.left >= rectROI.right)	return E_ERROR_CODE_ROI_OVER;
				if (rectROI.top >= rectROI.bottom)	return E_ERROR_CODE_ROI_OVER;


				// 				cv::Mat mOk_flip = MatColorBuffer.clone(); // UI ��� �׸����
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

			//�ٸ� �̹��� �̹� Dust �˻� ��� ��ٸ� 
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
			strFont2.Format(_T("%s\\�����_��ü�ҷ�_����Ʈ ����.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
			cv::imwrite((cv::String)(CStringA)strFont2, matReusltImage[E_DEFECT_COLOR_DARK]);


			lErrorCode = Concealment_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlignPara ,dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);


		}
		break;

		default:
			break;
		}
	}
	// ���� �˻�
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

			// C#���� C++�� cv::Mat ������ �޶� ����, ũ�� ���� �Ķ���� �Ѱ����.
			// MFC ����ϹǷ� cv::Mat �״�� �Ѱܵ� ������
			cv::Mat matSrc_Stage = m_stThrdAlignInfo.matSrc_Stage.clone();

			lErrorCode = Point_FindDefect_Transparency(MatOriginImage_, MatBKG_, matReusltImage[E_DEFECT_COLOR_DARK], matReusltImage[E_DEFECT_COLOR_BRIGHT],
				stThrdAlignInfo.ptCorner, dAlignPara, stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, strAlgLog, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER]], matSrc_Stage);

			// ���� �� ���Ϻ� Blob �Ǵ� Contour �ϱ�
			//Mask ���ֱ�
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


			// ��� ī�޶� ��� ������ �˻� ���� ���
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


			// �˻� ��� ���� ���� �κ�
			if (nImageNum == theApp.GetImageNum(nInsp_Substance_Main_IMAGE) && nCameraNum_ == 0) //COAXIAL�� ��� ����
			{

				__int64  numOfLables = 0;
				cv::Mat matLabels, stats, centroids;
				numOfLables = connectedComponentsWithStats(MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK], matLabels, stats, centroids, 8, CV_32S, CCL_GRANA) - 1;

				//�ΰ� �̹��� ��� �� ��ġ�°͸� �ҷ����� ����
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

						// 0GV �� �ʱ�ȭ ��, �Ѿ��
						cv::Mat matTempROI = MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK](rcRect);
						cv::Mat matLabelROI = matLabels(rcRect);

						for (int y = 0; y < rcRect.height; y++)
						{
							int		*ptrLabel = (int *)matLabelROI.ptr(y);
							uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

							for (int x = 0; x < rcRect.width; x++, ptrLabel++, ptrGray++)
							{
								// �ش� ��ü
								if (*ptrLabel == idx)	*ptrGray = 0;
							}
						}

						continue;
					}


				}
				CString strFont2;

				strFont2.Format(_T("%s\\����_Back_bitwise_result.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK]);

				
				
				strFont2.Format(_T("%s\\%s_DustȮ�ο�.jpg"), wstrInspConfirmPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT]);


				//����Ʈ ����
				MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK] -= MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT];

				strFont2.Format(_T("%s\\��ü�ҷ�_����Ʈ ����.jpg"), wstrFinalResultPath, (LPCWSTR)strPanelID_);
				cv::imwrite((cv::String)(CStringA)strFont2, MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK]);


				matReusltImage[E_DEFECT_COLOR_DARK] = MatResultImg[theApp.GetImageNum(nInsp_Substance_Main_IMAGE)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_DARK].clone();
				//matReusltImage[E_DEFECT_COLOR_BRIGHT] = MatResultImg[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL)][nCameraNum_][nAlgorithmNumber_][E_DEFECT_COLOR_BRIGHT].clone();

				lErrorCode = Point_GetDefectList(MatOriginImage_, MatColorBuffer, matReusltImage, MatResultImg[theApp.GetImageNum(nInsp_Substance_Dust_IMAGE)][nCameraNum_][nAlgorithmNumber_], MatDrawBuffer, stThrdAlignInfo.ptCorner,dAlignPara, dAlgPara, nCommonPara, wstrAlgPath, EngineerDefectJudgment, pResultBlob, strAlgLog, false);

				// ���� ����
				CRect rectROI = CRect(
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x),
					min(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y),
					max(stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x),
					max(stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y, stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y));

				// �˻� ������ ���� ũ�⸦ ��� ���
				if (rectROI.left < 0 ||
					rectROI.top < 0 ||
					rectROI.right >= MatOriginImage_.cols ||
					rectROI.bottom >= MatOriginImage_.rows)	return E_ERROR_CODE_ROI_OVER;

				if (rectROI.left >= rectROI.right)	return E_ERROR_CODE_ROI_OVER;
				if (rectROI.top >= rectROI.bottom)	return E_ERROR_CODE_ROI_OVER;

				////////////////////////////////////////////////////
				stDefectInfo_Substance * pResultBlob_Sub = new stDefectInfo_Substance(MAX_DEFECT_COUNT, nImageNum);

			

				cv::Mat mOk_flip = MatColorBuffer.clone(); // UI ��� �׸����

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

// void AviInspection::DefectColorCheck(cv::Mat MatColorBuffer, cv::Mat MatOriginImage_, stDefectInfo* pResultBlob)
// {
// 	int& nDefectCount = pResultBlob->nDefectCount;
// 	cv::Mat matThresh2 = cv::Mat::zeros(MatOriginImage_.size(), CV_8UC1);
// 
// 	// �̹��� ���� RGB ��հ� ����ϰ�
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
// 		// �̹����� 1/5 ũ��� ROI �����
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
// 		// �� �� ����� ���ϴµ� ���صǴ� ��� ���� �� �����Ƿ� ����
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
// 	cv::imwrite("D:\\�̹��� ����.jpg", matDefectBuff_Color2);
// 
// 	// �ҷ� �ٽ� ����ȭ(���� �۰�)
// 	// �� ����
// 	for (int i = 0; i < nDefectCount; i++)
// 	{
// 		// ����� Blob�� �������� �ش� �ҷ��� ��ġ�� ã�� �ҷ� ũ�⺸�� ���� �� ū ROI ����
// 		int nOffSet = 0;			// �ҷ����� ũ�� ROI�� �����Ҷ� �� Pixel �� ũ�� ������ �������ִ� ��
// 
// 		Rect rectROI;
// 
// 		// ���� x��ǥ
// 		rectROI.x = pResultBlob->ptLT[i].x - nOffSet;
// 		rectROI.y = pResultBlob->ptLT[i].y - nOffSet;
// 		rectROI.width = rectROI.width = pResultBlob->ptRT[i].x - pResultBlob->ptLT[i].x + nOffSet * 2;
// 		rectROI.height = rectROI.height = pResultBlob->ptLB[i].y - pResultBlob->ptLT[i].y + nOffSet * 2;
// 
// 
// 		// �ش� ��ġ �̹��� �߶󳻱�
// 		cv::Mat matDefectBuff_Color = matDefectBuff_Color2(rectROI).clone();			// �ش� �κ� �޸𸮸� ���Ҵ� �ؾ� �ϴ°�??
// 		cv::Mat matGray = MatOriginImage_(rectROI).clone();								// Gray ����
// 		cv::Mat matDefectBuff_Thresh = matThresh2(rectROI);
// 
// 		double dMeanGV = pResultBlob->dMeanGV[i];
// 
// 
// 		cv::threshold(matGray, matDefectBuff_Thresh, dMeanGV - 5, 255, THRESH_BINARY_INV);
// 
// 
// 
// 		// �̹��� Lab ��ȯ -> Vector ������ �̿��ϸ� matLab ��� ���ص� �ɼ� ���� �޸𸮸� �����ؼ� �����غ�����
// 		cv::Mat matLab[3];
// 
// 		matDefectBuff_Color.convertTo(matDefectBuff_Color, CV_32FC3);			// float ������ ��ȯ
// 		matDefectBuff_Color *= 1.0 / 255.0;										// �Ҽ������� ��ȯ
// 
// 		cv::cvtColor(matDefectBuff_Color, matDefectBuff_Color, CV_BGR2Lab);		// Lab ��ȯ
// 		cv::split(matDefectBuff_Color, matLab);									// Lab �и�
// 
// 
// 
// 																				// �ҷ��� ��� Lab�� ���ϱ�
// 		double dMeanL = cv::mean(matLab[0], matDefectBuff_Thresh)[0];
// 		double dMeanA = cv::mean(matLab[1], matDefectBuff_Thresh)[0];
// 		double dMeanB = cv::mean(matLab[2], matDefectBuff_Thresh)[0];
// 
// 
// 		// ������
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
// 	cv::imwrite("D:\\���� ����ȭ.jpg", matThresh2);
// 
// 	matDefectBuff_Color2.release();
// 	matRGB->release();
// 	matThresh2.release();
// }



bool AviInspection::JudgementET(ResultPanelData& resultPanelData, double* dAlignPara, CString strPanelID)
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

			if (theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_32	&&
				theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_64	&&
				theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_GRAY_87)
				continue;

			// ���� �ٹ�
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
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
			//resultPanelData.m_ListDefectInfo[nIndexDark].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_POINT_DARK;

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


// bool AviInspection::JudgementEdgePadBright(ResultPanelData& resultPanelData, double* dAlignPara)
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
bool AviInspection::JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
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
bool AviInspection::JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
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


bool AviInspection::DeleteOverlapDefect(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.09.21 - 6.39" �ͺκ� ���� �ȵȰ��, E�� ����
	// EDGE_NUGI �� ���, EDGE_NUGI�� ������ ��� �ҷ� ����
	// SCJ 20.02.18 - B11 ���� ��û EDGE ���� // ACTIVE ���� NUGI �ҷ� �з� ��û
	//////////////////////////////////////////////////////////////////////////
	//
	//bool bNugiJudgeE = false;
	//
	//for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	//{
	//	// EDGE_NUGI ã��
	//	if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI)
	//	{
	//		// EDGE_NUGI ����Ʈ ó������ ����
	//		resultPanelData.m_ListDefectInfo[0] = resultPanelData.m_ListDefectInfo[i];
	//
	//		// E�� ����
	//		bNugiJudgeE = true;
	//
	//		break;
	//	}
	//}
	//
	//// EDGE_NUGI �ִ� ���...
	//if (bNugiJudgeE)
	//{
	//	// ��� �ҷ� ����
	//	for (int i = 1; i < resultPanelData.m_ListDefectInfo.GetCount(); )
	//		resultPanelData.m_ListDefectInfo.RemoveAt(i);
	//
	//	// ������, ������
	//	return true;
	//}

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
	// RGBBlack Bright Point - White Spot Mura �ߺ� ����
	//DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point(resultPanelData, dAlignPara); //�ӽ� �ּ� choi 05.07 �ʿ�����

	//Black mura �ߺ� ���� 
	DeleteOverlapDefect_Black_Mura_and_Judge(resultPanelData, dAlignPara);

	// Point - Point �ߺ� ����
	DeleteOverlapDefect_Point_Point(resultPanelData, dAlignPara);

	// Mura - Mura �ߺ� ����
	DeleteOverlapDefect_Mura_Mura(resultPanelData, dAlignPara);

	// Point - Line �ߺ� ����
	DeleteOverlapDefect_Point_Line(resultPanelData, dAlignPara);

	// Point - Mura �ߺ� ����
	DeleteOverlapDefect_Point_Mura(resultPanelData, dAlignPara);

	// Line - Mura �ߺ� ����
	DeleteOverlapDefect_Line_Mura(resultPanelData, dAlignPara);

	//RGBBlack Bright Point - White Spot Mura �ߺ� ����
	//DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point(resultPanelData, dAlignPara);

	return true;
}

//choikwangil Test �̻�� Enum �߰� �ʿ�
// bool AviInspection::Judge_DefectSize(ResultPanelData& resultPanelData, double* dAlignPara)
// {
// 	double Judge_Ds_Flag = (double)(dAlignPara[E_PARA_DSJ_FLAG]);
// 	// �ҷ� ����Ʈ �Ǵ� Flag ���� ��� ������
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
// 		// EDGE_NUGI ã��
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


bool AviInspection::DeleteOverlapDefect_SpecInSpotDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
{
	// ����ó�� Parameter �� ������
	if (dAlignPara == NULL)
		return false;

	// ����� -S�� �������� PARAMETER
	double	dblSpotThRatio_Active = (double)(dAlignPara[E_PARA_SISPOT_RATIO_ACTIVE]);	//�� 1�� ����
	double	dblSpotThRatio_Edge = (double)(dAlignPara[E_PARA_SISPOT_RATIO_EDGE]);		//�� 1.04�� ����
	int		nGVCount = (int)(dAlignPara[E_PARA_SISPOT_NUMBER_GVCOUNT]); //GV Count: 7000
	int		nPoint_Area = (int)(dAlignPara[E_PARA_SISPOT_AREA]);  // Point Area �ҷ� ���� ����


	// ������ -F�� �������� PARAMETER
	double	dStrong_blSpotThRatio_Active = (double)(dAlignPara[E_PARA_STRONG_SISPOT_RATIO_ACTIVE]);	//�� 1�� ����
	double	dStrong_blSpotThRatio_Edge = (double)(dAlignPara[E_PARA_STRONG_SISPOT_RATIO_EDGE]);		//�� 1.04�� ����
	int		nStrong_GVCount = (int)(dAlignPara[E_PARA_STRONG_SISPOT_NUMBER_GVCOUNT]); //GV Count: 7000
	int		nStrong_Point_Area = (int)(dAlignPara[E_PARA_STRONG_SISPOT_AREA]);  // Point Area �ҷ� ���� ����




	// ����ó�� Dimpe Ratio�� ������
	if (dblSpotThRatio_Active <= 0 || dblSpotThRatio_Edge <= 0 || nGVCount <= 0)
		return false;

	// ����ó�� Defect�� ������
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

		// ����� Point �ҷ��� ��츸 ã��
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT &&  nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_64 && resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel >= nPoint_Area)
		{
			nGrayDefect++;
		}
		//������
		if ((resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT || resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT) &&
			nImgNum1 == E_IMAGE_CLASSIFY_AVI_GRAY_64 && resultPanelData.m_ListDefectInfo[i].Defect_Size_Pixel >= nStrong_Point_Area)
		{
			nStrong_GrayDefect++;
		}

	}


	//////////////////////////////////////////////////////////////////////////
	// ���� �ҷ� Ȯ�� �۾� ver. 0.0ver
	// ����: PNZ/2018/04/19
	// ����: �ҷ��� ���� �������� Ȯ��, �ҷ� ��ǥ������ 
	//////////////////////////////////////////////////////////////////////////
	if (nGrayDefect > 0 && nPoint_Area >= 0)
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
// 				// �ӽ� Max/Mean Ratio�� �ִ� ������ �з� ��
// 				if ( MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
// 				{
// 					i++;
// 					continue;
// 				}

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

				int nRegionUnit = 50;

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




	//������
	if (nStrong_GrayDefect > 0 && nStrong_Point_Area >= 0)
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
				// 				// �ӽ� Max/Mean Ratio�� �ִ� ������ �з� ��
				// 				if ( MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
				// 				{
				// 					i++;
				// 					continue;
				// 				}

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
					dblSpotThRatio = dStrong_blSpotThRatio_Edge;
				}

				else
					dblSpotThRatio = dStrong_blSpotThRatio_Active;

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

				int nRegionUnit = 50;

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








	// white pattern (Black hole Mura ����)



	return true;
}

bool AviInspection::DeleteOverlapDefect_BlackHole(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
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
	// ���� �ҷ� Ȯ�� �۾� ver. 0.0ver
	// ����: PNZ/2018/04/19
	// ����: �ҷ��� ���� �������� Ȯ��, �ҷ� ��ǥ������ 
	//////////////////////////////////////////////////////////////////////////
	if (nGrayDefect > 0)
	{
		// G64 Pattern ���� ���� Load
		cv::Mat mWhiteImage = MatOrgImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_WHITE)][0];

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
				// 				// �ӽ� Max/Mean Ratio�� �ִ� ������ �з� ��
				// 				if ( MaxMeanRatio > 1.83 && nDefectMaxGV > 120)
				// 				{
				// 					i++;
				// 					continue;
				// 				}

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
					//resultPanelData.m_ListDefectInfo.RemoveAt(i);

					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT;

				else i++;
			}

			else i++;
		}

		mWhiteImage.release();
	}




	// white pattern (Black hole Mura ����)



	return true;
}

// Dust Pattern�� �̿��Ͽ� ���� �ҷ��� �ҷ������� Ȯ��
bool AviInspection::DeleteOverlapDefect_DustDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
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


bool AviInspection::DeleteOverlapDefect_BlackSmallDelet(ResultPanelData& resultPanelData, double* dAlignPara)
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



bool AviInspection::DeleteOverlapDefect_Point_Point(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;


	int nOffSet = (int)(dAlignPara[E_PARA_Duplicate_offset]);

	//int nOffSet = 6;

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

			// 			// Black ������ ��츸...
			// 			nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
			// 			if( theApp.GetImageClassify(nImgNum) != E_IMAGE_CLASSIFY_AVI_BLACK )
			// 			{
			// 				j++;
			// 				continue;
			// 			}

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

				// �ҷ� �߽� ��ǥ
				// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
				CPoint ptSrc2;
				ptSrc2.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
				ptSrc2.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

				// �ҷ� �߽����� ���ٸ�...
				if (abs(ptSrc1.x - ptSrc2.x) < nOffSet &&	abs(ptSrc1.y - ptSrc2.y) < nOffSet)
				{
					int nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;


					// KSW 17.09.11 - �ߺ� �� ��, �ٸ� ���Ϻ��� Black ���� �켱 ���� ( ���� ��û )
					if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_BLACK)
					{
						ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
						ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
						resultPanelData.m_ListDefectInfo[i] = tTemp2;
						resultPanelData.m_ListDefectInfo[j] = tTemp1;
					}

					// 					// KSW 17.12.14 - �ߺ� �� ��, �ٸ� ���Ϻ��� Gray ���� �켱 ����
					// 					if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_GRAY_64)
					// 					{
					// 						ResultDefectInfo tTemp1 = resultPanelData.m_ListDefectInfo[i];
					// 						ResultDefectInfo tTemp2 = resultPanelData.m_ListDefectInfo[j];
					// 						resultPanelData.m_ListDefectInfo[i] = tTemp2;
					// 						resultPanelData.m_ListDefectInfo[j] = tTemp1;
					// 					}



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
					// choi 20.06.23
					else if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3)
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


bool AviInspection::DeleteOverlapDefect_Point_Line(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// KSW 17.07.14 - P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOffSet = 8; //choi 05.21 4->8�� ����

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


bool AviInspection::DeleteOverlapDefect_Point_Mura(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
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
					nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT)		// 2018-05-08 PNZ Mura �ҷ��̴�
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
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT ||		// �����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK ||		// �����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||		// �׷����
							nDefectTypeS == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT ||		// �׷����
							nDefectTypeS == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK ||		// ���� ���
							nDefectTypeS == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT ||		// ���� ���
							nDefectTypeS == E_DEFECT_JUDGEMENT_MURA_MULT_BP)
						{
							//�ش� �ҷ��� black pattern ���� �����̸� �������� ����. Mura�� ���� ���� pwj
							int nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
							if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_BLACK)
							{
								j++;
								continue;
							}
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

	//19-08-20 choikwangil �ּ� ->  choi 05.01 �ּ� ����
	//////////////////////////////////////////////////////////////////////////
// 	// Mura Spot & ������ ��� -> ���� ���� & Mura ����
// 	//////////////////////////////////////////////////////////////////////////
// 
	int nOffSet = 5;

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Mura ���� Spot �ҷ� �ƴ� ��� ����
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT &&
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT)   // mura ������ ���� boe11 choikwangil -> 05.01 choi ����
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
				//			if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)
			{
				j++;
				continue;
			}
			// 

			// �ҷ� �߽� ��ǥ
			CPoint ptSrc;
			ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// ���� �ȿ� �ҷ� ����?
			if (rectTemp.PtInRect(ptSrc))
			{

				// �� ���� ������ S�� ���� ���� �߰� �� �ߺ� �������� ����
				int nImgNum = resultPanelData.m_ListDefectInfo[j].Img_Number;
				if (theApp.GetImageClassify(nImgNum) == E_IMAGE_CLASSIFY_AVI_BLACK && resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT) // s�� ������ ��� ������ ������ ���� ���� pwj 20.08.25 (���� ������ ���� ����)
				{
					j++;
					continue;
				}

				double meanRatio = resultPanelData.m_ListDefectInfo[j].Defect_MeanGV / (double)resultPanelData.m_ListDefectInfo[j].Defect_Size_Pixel;

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


//choikwangil Test
bool AviInspection::DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// B11
	// CKI 19.08.20
	// ���� ����� ���� ���� ������ ���ؼ� R,G,B,Black ���� ���� �������� G64 ���� ���� ����
	//////////////////////////////////////////////////////////////////////////

	if (dAlignPara != NULL)
	{
		int		nDefectTypeM, nDefectTypeS;
		int		nAdjustRange = (int)dAlignPara[E_PARA_STRONG_DEFECT_NOISE_ADJUST_RANGE];
		CRect	rcDeleteArea;
		CPoint	ptCenterSub;

		int	nImgNum = 0;
		int	nImgNum1 = 0;

		//�ȼ� �ҷ��϶� ���� ����
		if (nAdjustRange > 0)
		{
			// �ҷ� ���� ��ŭ
			for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
			{
				// Pattern ����
				nImgNum = 0;
				nImgNum1 = 0;
				nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
				nImgNum1 = theApp.GetImageClassify(nImgNum);



				// Point �ҷ��� R , G , B �� �ִ� ��츸 ã��
				if (nImgNum1 == E_IMAGE_CLASSIFY_AVI_R || nImgNum1 == E_IMAGE_CLASSIFY_AVI_G || nImgNum1 == E_IMAGE_CLASSIFY_AVI_B) // 04.09 choikwangil Black ���� ����
				{


					//Defect_Type
					nDefectTypeM = resultPanelData.m_ListDefectInfo[i].Defect_Type;

					if (		// ����
						nDefectTypeM == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||		// ����
						nDefectTypeM == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT ||		// �����
						nDefectTypeM == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT ||		// �׷����
						nDefectTypeM == E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT ||		// ���� ���
						nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_MULT_BP)
					{

						// P/S ��� ��ǥ ����
						// ũ��� �ڽ��� ���� - ���Ⱑ ������ ������ ������ ���µ� �ټ� ����
// 						rcDeleteArea.SetRect(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio,
// 							resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio,
// 							resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio,
// 							resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio);
// 
// 						// ��ǥ�� ����ȭ
// 						rcDeleteArea.NormalizeRect();
// 
// 						// ���� Ȯ��
// 						rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);

						// P/S ��� ��ǥ ����
						ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_X / resultPanelData.m_ListDefectInfo[i].nRatio); //choikwangil 04.10 ����
						ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[i].nRatio);

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

							if (nDefectTypeS == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT || nDefectTypeS == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT)	// ��������				//04.16 choi		
							{
								// P/S ��� ��ǥ ����
// 								ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
// 								ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

								rcDeleteArea.SetRect(resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio,	//choikwangil 04.10 ����
									resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio,
									resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio,
									resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

								// ��ǥ�� ����ȭ
								rcDeleteArea.NormalizeRect();

								// ���� Ȯ��
								rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);

								// ���� �ȿ� ������ ����
								if (rcDeleteArea.PtInRect(ptCenterSub))
								{
									// White Spot �ҷ� ����
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
	}


	return true;
}
//choikwangil Test �ӽ� �ּ�
// int AviInspection::Judge_Defect_BP_WSpot(ResultPanelData& resultPanelData, int index ,int Defect_Judge,double* dAlignPara)
// {
// 	// P/S ��忡 ���� ��ǥ ��
// 	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
// 
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// B11
// 	// CKI 19.08.20
// 	// G64 ���Ͽ��� ������ �������� MaxGV/MeanGV �̿��� ������
// 	//////////////////////////////////////////////////////////////////////////
// 	if ((int)dAlignPara[E_PARA_WHITESPOT_JUDGE_MAXRATIO_ONOFF] == 0) {
// 		return Defect_Judge;
// 	}
// 
// 	if (dAlignPara != NULL)
// 	{
// 		int		nDefectTypeM;
// 		int		nAdjustRange = (int)dAlignPara[E_PARA_STRONG_DEFECT_NOISE_ADJUST_RANGE]; //���� �����ؾ��� MaxGV_Judge ���� paramȭ
// 		CRect	rcDeleteArea;
// 		CPoint	ptCenterSub;
// 
// 		int	nImgNum = 0;
// 		int	nImgNum1 = 0;
// 
// 		double MaxGV_Ratio = 0.0;
// 		double MaxGV_Judge = (int)dAlignPara[E_PARA_WHITESPOT_JUDGE_MAXRATIO_VALUE]; //���� �����ؾ��� MaxGV_Judge ���� paramȭ
// 
// 		if (MaxGV_Judge > 0)
// 		{
// 			// �ҷ� ���� ��ŭ
// 
// 				// Pattern ����
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
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nOffSet = 4;

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
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT || //04.16 choi

					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EMD_BRIGHT ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EMD_DARK ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_UP_CM ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_CORNER_CM ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_FINGER_CM ||
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_NUGI			||	// KSW 18.05.21 - ���� ��û : ���� & ���� �ҷ� ���� ����
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BN_CORNER ||
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_STAMPED		||	// KSW 18.05.21 - ���� ��û : ���� & ���� �ҷ� ���� ����
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI		||	// KSW 18.05.21 - ���� ��û : ���� & ���� �ҷ� ���� ����
					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_		||	// KSW 18.05.21 - ���� ��û : ���� & ���� �ҷ� ���� ����
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

bool AviInspection::DeleteOverlapDefect_Black_Mura_and_Judge(ResultPanelData& resultPanelData, double* dAlignPara) //choikwangil 04.07 �Լ� ���� ��
{
	// P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//pwj
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �ҷ� ���� ��ŭ

	int	nImgNum = 0;

	int nImgNum1 = 0;


	int		nAdjustRange = (int)dAlignPara[E_PARA_STRONG_DEFECT_NOISE_ADJUST_RANGE];

	CRect	rcDeleteArea;
	CPoint	ptCenterSub;

	int nOffSet = 20; // choi 05.07

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Mura �ҷ� �ƴѰ� ���� ( ��ƼBP ���� )
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT);
		else continue;

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

			if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT || resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT) { //04.16 choi

				CRect rectS = CRect(
					(resultPanelData.m_ListDefectInfo[j].Pixel_Start_X / resultPanelData.m_ListDefectInfo[j].nRatio),
					(resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[j].nRatio),
					(resultPanelData.m_ListDefectInfo[j].Pixel_End_X / resultPanelData.m_ListDefectInfo[j].nRatio),
					(resultPanelData.m_ListDefectInfo[j].Pixel_End_Y / resultPanelData.m_ListDefectInfo[j].nRatio));

				// �ߺ� ���� Ȯ��
				CRect rectRes;
				IntersectRect(rectRes, rectM, rectS);

				// ���� �ȿ� ������ �̸� �ӽ� ����
				if ((rectRes.Width() + rectRes.Height()) > 0)
				{

					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_CM;



					// ���� ����Ʈ���� ���� ����Ʈ ���� ��...
					if (i > j)	i--;

					// �ش� �ҷ� ����
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
				}
				// �ߺ� �ȵŸ�... ���� �ҷ�����...
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
// 					// P/S ��� ��ǥ ����
// 					// ũ��� �ڽ��� ���� - ���Ⱑ ������ ������ ������ ���µ� �ټ� ����
// 					rcDeleteArea.SetRect(resultPanelData.m_ListDefectInfo[i].Pixel_Start_X / resultPanelData.m_ListDefectInfo[i].nRatio,
// 						resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y / resultPanelData.m_ListDefectInfo[i].nRatio,
// 						resultPanelData.m_ListDefectInfo[i].Pixel_End_X / resultPanelData.m_ListDefectInfo[i].nRatio,
// 						resultPanelData.m_ListDefectInfo[i].Pixel_End_Y / resultPanelData.m_ListDefectInfo[i].nRatio);
// 
// 					// ��ǥ�� ����ȭ
// 					rcDeleteArea.NormalizeRect();
// 
// 					// ���� Ȯ��
// 					rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);
// 
// 					ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
// 					ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);
// 
// 					// ���� �ȿ� ������ ����
// 					if (rcDeleteArea.PtInRect(ptCenterSub))
// 					{
// 
// 						resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_CM;
// 
// 
// 
// 						// ���� ����Ʈ���� ���� ����Ʈ ���� ��...
// 						if (i > j)	i--;
// 
// 						// �ش� �ҷ� ����
// 						resultPanelData.m_ListDefectInfo.RemoveAt(j);
// 					}
// 					// �ߺ� �ȵŸ�... ���� �ҷ�����...
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

	// black mura �� ���� ���� ��ġ�� �ʴ� �͵��� ��� ����
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT)
		{
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
			i--;
		}
	}


	// �ӽ÷� �����ߴ� �̸��� �ٽ� ������� ����
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
	// P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// �ߺ��Ǵ� Mura �ҷ� ����
	//////////////////////////////////////////////////////////////////////////

	int nOffSet = 0;




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
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH) //choi 06.04
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

			// �ߺ� ���� Ȯ��
			CRect rectRes;
			IntersectRect(rectRes, rectM, rectS);

			// ���� �ȿ� ������ ����
			if ((rectRes.Width() + rectRes.Height()) > 0)
			{
				// ���� & ���� & �Ϻ����� �켱������ �����
				if (//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT || //choi 06.04
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT ||
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||	// ����
					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT ||	// 04.16 choi
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

	//pwj
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �ҷ� ���� ��ŭ
	/*for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// Mura �ҷ� �ƴѰ� ���� ( ��ƼBP ���� )
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

			// �ߺ� ���� Ȯ��
			CRect rectRes;
			IntersectRect(rectRes, rectM, rectS);

			// ���� �ȿ� ������ ����
			if ((rectRes.Width() + rectRes.Height()) > 0)
			{
				// ���� & ���� & �Ϻ����� �켱������ �����
				// 				if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT ||
				// 					//resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT ||	// ����
				// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_NUGI ||	// ����
				// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI ||	// ���� ����
				// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_ ||	// ���� ����
				// 					resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_STAMPED)	// �Ϻ� ����
				// 				{

				if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT && resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)
				{
					resultPanelData.m_ListDefectInfo[j].Defect_Type = E_DEFECT_JUDGEMENT_MURA_CM; // ��¥ �ҷ��� �ӽ÷� �ٸ� �̸����� ����



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
				//}

				if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT && resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)
					resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_CM;



				// ���� ����Ʈ���� ���� ����Ʈ ���� ��...
				if (i > j)	i--;

				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// �ߺ� �ȵŸ�... ���� �ҷ�����...
			else j++;
		}
	}

	// black mura �� ���� ���� ��ġ�� �ʴ� �͵��� ��� ����
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT)
		{
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
			i--;
		}
	}


	// �ӽ÷� �����ߴ� �̸��� �ٽ� ������� ����
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_MURA_CM)
		{
			resultPanelData.m_ListDefectInfo[i].Defect_Type = E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT;
		}
	}*/

	return true;
}


// Casting - stDefectInfo���� �ʿ��� �κ� �����Ͽ� ResultPanelData�� ����
bool AviInspection::GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio)
{
	// AVI�� ȸ������ ���� ���� ���� ��ǥ�� ����ϹǷ� ���� ���� �� ��ũ��ǥ ��� ���� ���� ��ǥ�� ȸ���ؼ� �Ѱ���� ��
	_tcscpy_s(pResultDefectInfo->Defect_Code, _T("CODE"));
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_TOP] = cv::Point(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_TOP] = cv::Point(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM] = cv::Point(pResultBlob->ptRB[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_BOTTOM] = cv::Point(pResultBlob->ptLB[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].y);

	// choi ��� ����
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

	//choi ��� ����
	// AD �ƴ� �ҷ���...
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
// 	// AD �ҷ��� ��� ȸ������ �ʰ� ��ü ��ǥ �״�� ��ȯ
// 	else
// 	{

	pResultDefectInfo->Pixel_Start_X = pResultBlob->ptLT[nBlobCnt].x;
	pResultDefectInfo->Pixel_Start_Y = pResultBlob->ptLT[nBlobCnt].y;
	pResultDefectInfo->Pixel_End_X = pResultBlob->ptRB[nBlobCnt].x;
	pResultDefectInfo->Pixel_End_Y = pResultBlob->ptRB[nBlobCnt].y;
	//}

	pResultDefectInfo->Pixel_Center_X = (int)(pResultDefectInfo->Pixel_Start_X + pResultDefectInfo->Pixel_End_X) / 2;
	pResultDefectInfo->Pixel_Center_Y = (int)(pResultDefectInfo->Pixel_Start_Y + pResultDefectInfo->Pixel_End_Y) / 2;

	// ������ ���� �̹��� ���� Crop �� �ҷ� ���� ����
	pResultDefectInfo->Pixel_Crop_Start_X = min(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].x);
	pResultDefectInfo->Pixel_Crop_Start_Y = min(pResultBlob->ptLT[nBlobCnt].y, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Pixel_Crop_End_X = max(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].x);
	pResultDefectInfo->Pixel_Crop_End_Y = max(pResultBlob->ptLB[nBlobCnt].y, pResultBlob->ptRB[nBlobCnt].y);

	//choi ��� ����
// 	pResultDefectInfo->Pixel_Crop_Start_X = pResultDefectInfo->Pixel_Start_X;
// 	pResultDefectInfo->Pixel_Crop_Start_Y = pResultDefectInfo->Pixel_Start_Y;
// 	pResultDefectInfo->Pixel_Crop_End_X = pResultDefectInfo->Pixel_End_X;
// 	pResultDefectInfo->Pixel_Crop_End_Y = pResultDefectInfo->Pixel_End_Y;

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
	pResultDefectInfo->strColor = pResultBlob->strColor[nBlobCnt]; //220216 choi
	// KSW 17.12.04 - (���� + ����) / 2 -> Spec �߰� ( ���� ��û )
	pResultDefectInfo->dDimension = (pResultBlob->dF_Max[nBlobCnt] + pResultBlob->dF_Min[nBlobCnt]) / 2.0;

	////////////////////////////////////////////////////////////////////////// 0607 choi ���� ��ġ �������� ���� Data �߰�
	pResultDefectInfo->d_L = pResultBlob->Lab_avg_L[nBlobCnt];
	pResultDefectInfo->d_Degree = pResultBlob->Lab_Degree[nBlobCnt];
	pResultDefectInfo->d_Lenght = pResultBlob->Lab_Lenght[nBlobCnt];
	//////////////////////////////////////////////////////////////////////////
	
	// 2021.12.08~ MDJ LCP ������ ���� �ֱ� ���� ���� �߰�
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
	// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
	memcpy(pResultDefectInfo->nContoursX, pResultBlob->nContoursX[nBlobCnt], sizeof(int) * MAX_CONTOURS);
	memcpy(pResultDefectInfo->nContoursY, pResultBlob->nContoursY[nBlobCnt], sizeof(int) * MAX_CONTOURS);
#endif

	return true;
}

bool AviInspection::AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner)
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
long AviInspection::CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio)
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
bool AviInspection::JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo)
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

bool AviInspection::JudgementSpot(ResultPanelData& resultPanelData)
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
			resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT		&& //04.16 choi
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

bool AviInspection::JudgementDelReport(ResultPanelData& resultPanelData)
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

bool AviInspection::JudgementDeletLineDefect(ResultPanelData& resultPanelData, double* dAlignPara)
{
	int nLineNumber = 0;	// ��¥ ���� �ҷ�
	int nLineDefect = 0;	// ���� ����
	int nOffSet = 30;	// �߽��� �Ÿ� ����
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

bool AviInspection::JudgementSamePatternDefectMerge(ResultPanelData& resultPanelData)
{
	int nLineNumber = 0;	// ��¥ ���� �ҷ�
	int nSSLineNumber = 0;	// ���� Pattern, ���� ���⿡ �ҷ�
	int nOffSet = 10;	// �߽��� �Ÿ� ����

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
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK)
			{
				CPoint	X1Center, X1Length;
				int		X1DefectImageType, X1DefectLength;

				X1Length.x = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Start_X;	// ���� ������
				X1Length.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_End_X;	// ���� ����
				X1Center.y = (long)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y; // Center��ġ
				X1DefectImageType = (long)resultPanelData.m_ListDefectInfo[i].Pattern_Type;	// ����
				X1DefectLength = abs(X1Length.x - X1Length.y);								// ���� ����

				for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
				{
					// ���� �ҷ� �� ����
					if (i == j) { j++; continue; }

					// X ���� �ҷ� �ƴѰ� ���� ( ����ε� ���� �� �ֱ⿡ �߰� ���� )
					if (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_BRIGHT	 &&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_OPEN	 &&
						resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_LINE_X_DARK)
					{
						j++; continue;
					}

					CPoint	X2Center, X2Length;
					int		X2DefectImageType, X2DefectLength;

					X2Length.x = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Start_X;	// ���� ������
					X2Length.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_End_X;	// ���� ����
					X2Center.y = (long)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y; // Center��ġ
					X2DefectImageType = (long)resultPanelData.m_ListDefectInfo[j].Pattern_Type;	// ����
					X2DefectLength = abs(X2Length.x - X2Length.y);								// ���� ����

					// ���� Pattern�� ����
					if (X1DefectImageType != X2DefectImageType) { j++; continue; }

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
						resultPanelData.m_ListDefectInfo[i].Pixel_Start_X = min(X1Length.x, X2Length.x);
						resultPanelData.m_ListDefectInfo[i].Pixel_End_X = max(X1Length.y, X2Length.y);

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



//////////////////////////////////////////////////////////////////////////////////////////////////
//��	 ��: Black Pattern ���� �ֺ� Bright Line Merge												//  
//�۾���: 2017-10-21																				//
//       17.10.30 ���� ���: Black Pattern ������ Dark Line ����									//
//�۾���: PNZ																						//  
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementBlackPatternMerge(ResultPanelData& resultPanelData)
{
	int nXLineNumber = 0;	// ��¥ ���� �ҷ�
	int nYLineNumber = 0;	// ��¥ ���� �ҷ�
	int nOffSet = 20;	// �߽��� �Ÿ� ����

	// �ҷ� ������ ���� �κ�
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
			// ��¥ X Line �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_X_DARK:
		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
		case E_DEFECT_JUDGEMENT_LINE_DGS_X:
		case E_DEFECT_JUDGEMENT_LINE_PCD_CRACK:
			nXLineNumber++;	// ��¥ ����
			break;

			// ��¥ Y Line �ҷ��� ��츸 Count
		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:
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
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT)
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
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_BRIGHT)
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
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT)
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
			if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT)
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

//////////////////////////////////////////////////////////////////////////
// ����: Camera Tap�� ���� �� Line �ҷ� ���� ����
// �۾���: 2018-07-31
// �۾���: PNZ
//////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementCameraTapOverKill(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara)
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
				if (abs(nDefectLocation - CTO_Position_X) <= CTO_Offset)
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

bool AviInspection::DefectLDRCompair(cv::Mat matSrcImage, cv::Rect rectTemp, double& Left_MeanValue, double& Defect_MeanValue, double& Right_MeanValue, double& Defect_MaxGV)
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

bool 	AviInspection::CameraTapInfo(int CameraType, vector<int> &Direction, vector<int> &Position, vector<double> &BrightDiffGV, vector<double> &DarkDiffGV)
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

bool AviInspection::JudgementPCDCrack(ResultPanelData& resultPanelData, double* dAlignPara)
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

bool AviInspection::JudgementNewDGS(ResultPanelData& resultPanelData)
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
//��	 ��: Crack ���� ver 0.2																		//  
//�۾���: 2017-11-10																				// 
//�۾���: PNZ																						//
//Crack Pattern: G32 ���� (Image Number 9)														//
//////////////////////////////////////////////////////////////////////////////////////////////////

bool AviInspection::JudgementCrack(ResultPanelData& resultPanelData)
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

		if (nImgNum1 != E_IMAGE_CLASSIFY_AVI_GRAY_32) continue; //choikwangil 64���� ����

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

bool AviInspection::JudgementClassifyLineDefect(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT])
{
	int nLineNumber = 0;	// ��¥ ���� �ҷ�
	int nLineDefect = 0;	// ���� ����
	int nOffSet = 20;	// Line �ҷ� ���� ����

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

bool AviInspection::JudgementSpecialGrayLineDefect(ResultPanelData& resultPanelData)
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
long AviInspection::CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult,
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

			theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_DIABLE_CHECK_DUST, eALARMTYPE_HEAVY, bIsHeavyAlarm);

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
double AviInspection::calcResolution(CWriteResultInfo WrtResultInfo)
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

bool AviInspection::JudgementRepeatCount(CString strPanelID, ResultPanelData& resultPanelData)
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

bool AviInspection::m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo)
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

	// ROI ���� Ȯ��
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

	// ���� Index
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

									//int nDefect3_Cnt = 0;			// �ӽ� Type2 Y����

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

		if (nDefect_Cnt > 0) // ���� ���ſ�
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

					if (scrMean[0] > 0)
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
				// Type2 ��ġ �� ũ��
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
	}

	return true;
}

// long AviInspection::JudgeSubStanceSpot(cv::Mat& matSrcBuffer, CRect rectROI, double* dPara, int* nCommonPara,
// 	stDefectInfo* pResultBlob)
// {
// 	// �ҷ��� ���� ��� ������
// 	int nCount = pResultBlob->nDefectCount;
// 	if (nCount <= 0)		return E_ERROR_CODE_TRUE;
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// ���� �Ķ����
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
// 	long	nWidth = (long)matSrcBuffer.cols;	// ���� ���� ũ��
// 	long	nHeight = (long)matSrcBuffer.rows;	// ���� ���� ũ��
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
// 	// �ҷ� ���� ��ŭ
// 	for (int i = 0; i < nCount; i++)
// 	{
// 		if( pResultBlob->bUseResult[i] == false)
// 			continue;
// 		// Spot �ҷ��� �ƴ� ��� �Ѿ��
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
// 		// �ҷ� �ֺ� ����
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
// 		// �ҷ� ����
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
// 		// �ҷ� ���� ũ�� ����
// 		int nValue = nMorpObj * 2 + 1;
// 		if (nMorpObj >= 1)
// 			cv::morphologyEx(matDefectResBuf, matDefectMorp1Buf, MORPH_DILATE, cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(nValue, nValue)));
// 		else
// 			matDefectResBuf.copyTo(matDefectMorp1Buf);
// 
// 		// ��� �κ�
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
// 		// ���� ����
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
	// �ҷ��� ���� ��� ������
	int nCount = pResultBlob->nDefectCount;
	if (nCount <= 0)		return E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
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
	long	nWidth = (long)matSrcBuffer.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcBuffer.rows;	// ���� ���� ũ��

	int nMorpObj = 0;
	int nOffset = 0;
	int nOffset_Defect = 0;


	//////////////////////////////////////////////////////////////////////////
	// Active ���� ������ ��� ���� ��� GV Set
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
	// �ҷ� ���� ��ŭ
	for (int i = 0; i < nCount; i++)
	{

		// Spot �ҷ��� �ƴ� ��� �Ѿ��
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

		
		//�ҷ� �߽� ���� Crop Rect
		cv::Rect rectTempROI(pResultBlob->nCenterx[i] - nOffset, pResultBlob->nCentery[i] - nOffset, nOffset * 2, nOffset * 2);
		if (rectTempROI.x < 0)						rectTempROI.x = 0;
		if (rectTempROI.y < 0)						rectTempROI.y = 0;
		if (rectTempROI.x + nOffset >= nWidth)		rectTempROI.x = nWidth - nOffset - 1;
		if (rectTempROI.y + nOffset >= nHeight)	rectTempROI.y = nHeight - nOffset - 1;
		//////////////////////////////////////////////////////////////////////////

		//Crop�� �̹��� ��� ��� ���ϱ� ���� Range
		nRangeX = rectTempROI.width / nSegX;
		nRangeY = rectTempROI.height / nSegY;
		//////////////////////////////////////////////////////////////////////////

		//Crop Rect ���� �ҷ� ũ�� Rect
		cv::Rect rectDefectTempROI(pResultBlob->ptLT[i].x - rectTempROI.x, pResultBlob->ptLT[i].y - rectTempROI.y, nOffset_Defect_W, nOffset_Defect_H);
		if (rectDefectTempROI.x < 0)						rectDefectTempROI.x = 0;
		if (rectDefectTempROI.y < 0)						rectDefectTempROI.y = 0;
		if (rectDefectTempROI.x + nOffset_Defect_W >= rectTempROI.width)		rectDefectTempROI.x = rectTempROI.width - nOffset_Defect_W - 1;
		if (rectDefectTempROI.y + nOffset_Defect_H >= rectTempROI.height)	rectDefectTempROI.y = rectTempROI.height - nOffset_Defect_H - 1;

		
		//////////////////////////////////////////////////////////////////////////
		//Defect �߽� Mask ����
		cv::Mat matDefectSrcBuf = matSrcRoI(rectTempROI);
		cv::Mat matDefectCenter = cv::Mat::zeros(matDefectSrcBuf.size(), matDefectSrcBuf.type());
		cv::Mat matBrightLine_Mask = cv::Mat::zeros(matDefectSrcBuf.size(), matDefectSrcBuf.type());

		matDefectCenter(rectDefectTempROI).setTo(255);
		matDefectCenter.copyTo(matBrightLine_Mask);

		Mask_SizeUp(matBrightLine_Mask, nBright_Line_Offset);
		//matBrightLine_Mask(rectBrightLineROI).setTo(255);
		matBrightLine_Mask -= matDefectCenter;
		//////////////////////////////////////////////////////////////////////////

		//Crop �� �̹��� ��� ��� ���
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


		//���� �ҷ� TH�� �ٸ��� Set
		nBack_Dark_Thres = nThres * dBK_Dark_Ratio;
		nBack_Bright_Thres = nThres * 1.1;

		nThres *= dDefect_Dark_Ratio;

		
		//////////////////////////////////////////////////////////////////////////

		cv::Mat matDefectResBuf;
		cv::Mat matDefectResBuf_Temp;
		cv::Mat matDefectResBuf_Dark_Back;
		cv::Mat matDefectResBuf_Bright_Back;

		cv::threshold(matDefectSrcBuf, matDefectResBuf, nThres, 255.0, CV_THRESH_BINARY_INV); //�ҷ� ���� Threshold
		cv::threshold(matDefectSrcBuf, matDefectResBuf_Dark_Back, nBack_Dark_Thres, 255.0, CV_THRESH_BINARY_INV); //��� ���� Threshold Dark
		cv::threshold(matDefectSrcBuf, matDefectResBuf_Bright_Back, nBack_Bright_Thres, 255.0, CV_THRESH_BINARY); //��� ���� Threshold Bright

		matDefectResBuf.copyTo(matDefectResBuf_Temp);

		cv::bitwise_and(matDefectResBuf, matDefectCenter, matDefectResBuf); //�ҷ� Center Mask�� �ҷ��� ����

		double valMin, valMax;
		cv::minMaxLoc(matDefectResBuf, &valMin, &valMax, 0, 0);

		//Center �ҷ��� ���������� �ƹ����� ������ ����
		if (valMax <= 0) {
			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_LINE_X_VTH_DARK;
			//pResultBlob->bUseResult[i] = false;

			pResultBlob_Sub->bUseResult[i] = pResultBlob->bUseResult[i];
			pResultBlob_Sub->nDefectJudge[i] = pResultBlob->nDefectJudge[i];
			pResultBlob_Sub->nDefectColor[i] = pResultBlob->nDefectColor[i];


			continue;
		}
		//////////////////////////////////////////////////////////////////////////

		// �ҷ� ���� ũ�� ����
		int nValue = nMorpObj * 2 + 1;
		if (nMorpObj >= 1)
			cv::morphologyEx(matDefectResBuf, matDefectMorp1Buf, MORPH_DILATE, cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(nValue, nValue)));
		else
			matDefectResBuf.copyTo(matDefectMorp1Buf);

		// ��� �κ�
		cv::bitwise_not(matDefectMorp1Buf, matDefectBKBuf); //��濡�� �ҷ� �κ� ����

		cv::Mat matBG_Bright_Line;
		int nBG_Bright_Line_Size = 0;

		cv::bitwise_and(matDefectResBuf_Bright_Back, matBrightLine_Mask, matBG_Bright_Line); //�ҷ� ������ �ֺ� ����� �ö�� �ĺ��� ����
		for (int i = 0; i < matBG_Bright_Line.rows * matBG_Bright_Line.cols; i++) {
			if (matBG_Bright_Line.data[i] == 255) { nBG_Bright_Line_Size++; }
		}

		cv::Mat matBG_Defect;
		int nBG_Defect_Size = 0;

		cv::bitwise_and(matDefectResBuf_Dark_Back, matDefectBKBuf, matBG_Defect); //�ҷ� ������ �ֺ� ����� �ö�� �ĺ��� ����
		for (int i = 0; i < matBG_Defect.rows * matBG_Defect.cols; i++) {
			if (matBG_Defect.data[i] == 255) { nBG_Defect_Size++; }
		}

		cv::Mat mat_Defect;
		int nDefect_Size = 0;

		cv::bitwise_and(matDefectResBuf_Temp, matDefectMorp1Buf, mat_Defect); // �ҷ� ũ��
		for (int i = 0; i < mat_Defect.rows * mat_Defect.cols; i++) {
			if (mat_Defect.data[i] == 255) { nDefect_Size++; }
		}

		cv::Scalar meanBK, stdBK;
		cv::meanStdDev(matDefectSrcBuf, meanBK, stdBK, matDefectBKBuf); //�ҷ� ������ ����� ���GV, ǥ������

		cv::Scalar meanORJ, stdORJ;
		cv::meanStdDev(matDefectSrcBuf, meanORJ, stdORJ, matDefectMorp1Buf); //�ҷ��� ���GV, ǥ������


		pResultBlob->dMuraObj[i] = meanORJ[0]; 
		pResultBlob->dMuraBk[i] = meanBK[0];

		// ���� ����
		double	dArea = pResultBlob->nArea[i];
		double	dSub = meanORJ[0] - meanBK[0]; //�ҷ����GV - �ҷ� ������ ����� ���GV
		double	dDiff = meanORJ[0] / meanBK[0]; //�ҷ����GV / �ҷ� ������ ����� ���GV


		double ratio = 0;
		double area_ratio = 0;

		//////////////////////////////////////////////////////////////////////////
		//matDefectResBuf_Dark_Back �߽� �Ȼ� ���
		//mat_Defect ������ ����
		//matBG_Defect ���� �� ���
		//////////////////////////////////////////////////////////////////////////

		cv::Mat matLabel, matStats, matCentroid;

		__int64 nTotalLabel = 0;

		nTotalLabel = cv::connectedComponentsWithStats(matDefectResBuf_Dark_Back, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1; //Crop ����ȿ��� �󺧸� (�ҷ� �ֺ� �پ��ִ� ��ο� ��� ã������)
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
		
			//�ҷ��� �پ��ִ� ��� ����� ����, Area ���� ���
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

		//�ҷ��� �پ��ִ� ��濡�� �ҷ� ����;
		matCenter_BK -= mat_Defect;

		cv::Scalar meanBK_Center, stdBK_Center;
		cv::meanStdDev(matDefectSrcBuf, meanBK_Center, stdBK_Center, matCenter_BK); //�ҷ��� �پ��ִ� ��� ���GV, ǥ������ ���
		int nCenterBK_Cnt = 0;

		for (int i = 0; i < matCenter_BK.rows*matCenter_BK.cols; i++) { //�ҷ��� �پ��ִ� ��� ũ��
			if (matCenter_BK.data[i] > 0) nCenterBK_Cnt++;
		}


		double	dSub_Center = meanORJ[0] - meanBK_Center[0]; //�ҷ� ���GV - �ҷ��� �پ��ִ� ��� ���GV
		double	dDiff_Center = meanORJ[0] / meanBK_Center[0]; //�ҷ� ���GV / �ҷ��� �پ��ִ� ��� ���GV

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
		//��� Ȯ�� �����

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
	// �ҷ��� ���� ��� ������
	int nCount = pResultBlob->nDefectCount;
	if (nCount <= 0)		return E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
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
	long	nWidth = (long)matSrcBuffer.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcBuffer.rows;	// ���� ���� ũ��

	int nMorpObj = 0;
	int nOffset = 0;
	int nOffset_Defect = 0;


	//////////////////////////////////////////////////////////////////////////
	// Active ���� ������ ��� ���� ��� GV Set
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
	// �ҷ� ���� ��ŭ
	for (int i = 0; i < nCount; i++)
	{

		// Spot �ҷ��� �ƴ� ��� �Ѿ��
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


		//�ҷ� �߽� ���� Crop Rect
		cv::Rect rectTempROI(pResultBlob->nCenterx[i] - nOffset, pResultBlob->nCentery[i] - nOffset, nOffset * 2, nOffset * 2);
		if (rectTempROI.x < 0)						rectTempROI.x = 0;
		if (rectTempROI.y < 0)						rectTempROI.y = 0;
		if (rectTempROI.x + nOffset >= nWidth)		rectTempROI.x = nWidth - nOffset - 1;
		if (rectTempROI.y + nOffset >= nHeight)	rectTempROI.y = nHeight - nOffset - 1;
		//////////////////////////////////////////////////////////////////////////

		//Crop�� �̹��� ��� ��� ���ϱ� ���� Range
		nRangeX = rectTempROI.width / nSegX;
		nRangeY = rectTempROI.height / nSegY;
		//////////////////////////////////////////////////////////////////////////

		//Crop Rect ���� �ҷ� ũ�� Rect
		cv::Rect rectDefectTempROI(pResultBlob->ptLT[i].x - rectTempROI.x, pResultBlob->ptLT[i].y - rectTempROI.y, nOffset_Defect_W, nOffset_Defect_H);
		if (rectDefectTempROI.x < 0)						rectDefectTempROI.x = 0;
		if (rectDefectTempROI.y < 0)						rectDefectTempROI.y = 0;
		if (rectDefectTempROI.x + nOffset_Defect >= nWidth)		rectDefectTempROI.x = nWidth - nOffset_Defect_W - 1;
		if (rectDefectTempROI.y + nOffset_Defect >= nHeight)	rectDefectTempROI.y = nHeight - nOffset_Defect_H - 1;


		//////////////////////////////////////////////////////////////////////////
		//Defect �߽� Mask ����
		cv::Mat matDefectSrcBuf = matSrcRoI(rectTempROI);
		cv::Mat matDefectCenter = cv::Mat::zeros(matDefectSrcBuf.size(), matDefectSrcBuf.type());
		matDefectCenter(rectDefectTempROI).setTo(255);
		//////////////////////////////////////////////////////////////////////////

		//Crop �� �̹��� ��� ��� ���
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


		//���� �ҷ� TH�� �ٸ��� Set
		nBack_Thres = nThres * dBK_Dark_Ratio;
		nThres *= dDefect_Dark_Ratio;
		//////////////////////////////////////////////////////////////////////////

		cv::Mat matDefectResBuf;
		cv::Mat matDefectResBuf_Temp;
		cv::Mat matDefectResBuf_Back;

		cv::threshold(matDefectSrcBuf, matDefectResBuf, nThres, 255.0, CV_THRESH_BINARY_INV); //�ҷ� ���� Threshold
		cv::threshold(matDefectSrcBuf, matDefectResBuf_Back, nBack_Thres, 255.0, CV_THRESH_BINARY_INV); //��� ���� Threshold
		matDefectResBuf.copyTo(matDefectResBuf_Temp);

		cv::bitwise_and(matDefectResBuf, matDefectCenter, matDefectResBuf); //�ҷ� Center Mask�� �ҷ��� ����

		double valMin, valMax;
		cv::minMaxLoc(matDefectResBuf, &valMin, &valMax, 0, 0);

		//Center �ҷ��� ���������� �ƹ����� ������ ����
		if (valMax <= 0) {
			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT;
			//pResultBlob->bUseResult[i] = false;

			pResultBlob_Sub->bUseResult[i] = pResultBlob->bUseResult[i];
			pResultBlob_Sub->nDefectJudge[i] = pResultBlob->nDefectJudge[i];
			pResultBlob_Sub->nDefectColor[i] = pResultBlob->nDefectColor[i];


			continue;
		}
		//////////////////////////////////////////////////////////////////////////

		// �ҷ� ���� ũ�� ����
		int nValue = nMorpObj * 2 + 1;
		if (nMorpObj >= 1)
			cv::morphologyEx(matDefectResBuf, matDefectMorp1Buf, MORPH_DILATE, cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(nValue, nValue)));
		else
			matDefectResBuf.copyTo(matDefectMorp1Buf);

		// ��� �κ�
		cv::bitwise_not(matDefectMorp1Buf, matDefectBKBuf); //��濡�� �ҷ� �κ� ����

		cv::Mat matBG_Defect;
		int nBG_Defect_Size = 0;

		cv::bitwise_and(matDefectResBuf_Back, matDefectBKBuf, matBG_Defect); //�ҷ� ������ �ֺ� ����� �ö�� �ĺ��� ����
		for (int i = 0; i < matBG_Defect.rows * matBG_Defect.cols; i++) {
			if (matBG_Defect.data[i] == 255) { nBG_Defect_Size++; }
		}

		cv::Mat mat_Defect;
		int nDefect_Size = 0;

		cv::bitwise_and(matDefectResBuf_Temp, matDefectMorp1Buf, mat_Defect); // �ҷ� ũ��
		for (int i = 0; i < mat_Defect.rows * mat_Defect.cols; i++) {
			if (mat_Defect.data[i] == 255) { nDefect_Size++; }
		}

		cv::Scalar meanBK, stdBK;
		cv::meanStdDev(matDefectSrcBuf, meanBK, stdBK, matDefectBKBuf); //�ҷ� ������ ����� ���GV, ǥ������

		cv::Scalar meanORJ, stdORJ;
		cv::meanStdDev(matDefectSrcBuf, meanORJ, stdORJ, matDefectMorp1Buf); //�ҷ��� ���GV, ǥ������


		pResultBlob->dMuraObj[i] = meanORJ[0];
		pResultBlob->dMuraBk[i] = meanBK[0];

		// ���� ����
		double	dArea = pResultBlob->nArea[i];
		double	dSub = meanORJ[0] - meanBK[0]; //�ҷ����GV - �ҷ� ������ ����� ���GV
		double	dDiff = meanORJ[0] / meanBK[0]; //�ҷ����GV / �ҷ� ������ ����� ���GV


		double ratio = 0;
		double area_ratio = 0;

		//////////////////////////////////////////////////////////////////////////
		//matDefectResBuf_Back �߽� �Ȼ� ���
		//mat_Defect ������ ����
		//matBG_Defect ���� �� ���
		//////////////////////////////////////////////////////////////////////////

		cv::Mat matLabel, matStats, matCentroid;

		__int64 nTotalLabel = 0;

		nTotalLabel = cv::connectedComponentsWithStats(matDefectResBuf_Back, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1; //Crop ����ȿ��� �󺧸� (�ҷ� �ֺ� �پ��ִ� ��ο� ��� ã������)
		cv::Point2i  ptCenter(mat_Defect.cols / 2, mat_Defect.rows / 2);
		cv::Rect rcBK_Defect;
		cv::Mat matCenter_BK = cv::Mat::zeros(matDefectResBuf_Back.size(), matDefectResBuf_Back.type());



		for (int idx = 1; idx <= nTotalLabel; idx++) {

			int nArea = matStats.at<int>(idx, CC_STAT_AREA);

			rcBK_Defect.x = matStats.at<int>(idx, CC_STAT_LEFT);
			rcBK_Defect.y = matStats.at<int>(idx, CC_STAT_TOP);
			rcBK_Defect.width = matStats.at<int>(idx, CC_STAT_WIDTH);
			rcBK_Defect.height = matStats.at<int>(idx, CC_STAT_HEIGHT);

			//�ҷ��� �پ��ִ� ��� ����� ����, Area ���� ���
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

		//�ҷ��� �پ��ִ� ��濡�� �ҷ� ����;
		matCenter_BK -= mat_Defect;

		cv::Scalar meanBK_Center, stdBK_Center;
		cv::meanStdDev(matDefectSrcBuf, meanBK_Center, stdBK_Center, matCenter_BK); //�ҷ��� �پ��ִ� ��� ���GV, ǥ������ ���
		int nCenterBK_Cnt = 0;

		for (int i = 0; i < matCenter_BK.rows*matCenter_BK.cols; i++) { //�ҷ��� �پ��ִ� ��� ũ��
			if (matCenter_BK.data[i] > 0) nCenterBK_Cnt++;
		}


		double	dSub_Center = meanORJ[0] - meanBK_Center[0]; //�ҷ� ���GV - �ҷ��� �پ��ִ� ��� ���GV
		double	dDiff_Center = meanORJ[0] / meanBK_Center[0]; //�ҷ� ���GV / �ҷ��� �پ��ִ� ��� ���GV

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
		//��� Ȯ�� �����

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
	// 	Mat M(2, 3, CV_32FC1); // warpAffine ���꿡 ���Ǵ� 2*3 ���
	// 	cv::Point2f center;
	// 	center.x = (float)ptCenter.x;
	// 	center.y = (float)ptCenter.y;
	// 	M = getRotationMatrix2D(center, Theta, 1.0);
	// 	cv::warpAffine(matResult, matResult, M, matResult.size()); // ȸ��
	// 
	// 	imwrite("D:\\Lipschitz_Fillter.bmp", matResult);


}
void AviInspection::BlobFeatureSave_Sub(stDefectInfo_Substance *resultBlob, CString strPath, int* nImageDefectCount)
{
	if (resultBlob == NULL)	return;

	int	nDefectNum = 0;

	// ��� Log ����
	FILE *out = NULL;

	if (nImageDefectCount == NULL)
		nImageDefectCount = &nDefectNum;

	if (*nImageDefectCount == 0)
		_wfopen_s(&out, strPath, _T("wt"));
	else
		_wfopen_s(&out, strPath, _T("at"));

	// ���� ó��.
	if (out == NULL)		return;

	// �������� ROI ��� ���� �� ���ʿ��� Header ����
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
	// KSW 17.07.27 ���α׷� �ٿ� ����
	char szPath[MAX_PATH] = { 0, };
	for (int nFori = 0; nFori < resultBlob->nDefectCount; nFori++)
	{
		if (resultBlob->bUseResult[nFori] == false) continue;
		// KSW 17.07.27 ���α׷� �ٿ� ����
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, theApp.GetDefectTypeName(resultBlob->nDefectJudge[nFori]), -1, szPath, sizeof(szPath), NULL, NULL);

		// KSW 17.07.27 �ҷ� ���� �������� ���α׷� �ٿ� �߻�
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

	// ��� Log ����
	FILE *out = NULL;

	if (nImageDefectCount == NULL)
		nImageDefectCount = &nDefectNum;

	if (*nImageDefectCount == 0)
		_wfopen_s(&out, strPath, _T("wt"));
	else
		_wfopen_s(&out, strPath, _T("at"));

	// ���� ó��.
	if (out == NULL)		return;

	// �������� ROI ��� ���� �� ���ʿ��� Header ����
	if (*nImageDefectCount == 0)
	{
		fprintf_s(out, "%S,,", strPanel_ID);

		fprintf_s(out, "\n");


		fprintf_s(out, ",\
 						�̹� ����,,");

		fprintf_s(out, "\n");

		fprintf_s(out, "No						,\
 						��						,\
 						û						,\
						ȫ						,\
						�̹�������(um)			,\
						�̹�Area(pixel)			,\
						��������				,\
						�̹�������(����um)		,,");

		fprintf_s(out, "\n");
	}

	// KSW 17.07.27 ���α׷� �ٿ� ����
	char szPath[MAX_PATH] = { 0, };
	for (int nFori = 0; nFori < resultBlob->nDefectCount; nFori++)
	{
		// 		if (resultBlob->bUseResult[nFori] == false) continue;
		// 		// KSW 17.07.27 ���α׷� �ٿ� ����
		// 		memset(szPath, 0, sizeof(char) * MAX_PATH);
		// 		WideCharToMultiByte(CP_ACP, 0, theApp.GetDefectTypeName(resultBlob->nDefectJudge[nFori]), -1, szPath, sizeof(szPath), NULL, NULL);

		// KSW 17.07.27 �ҷ� ���� �������� ���α׷� �ٿ� �߻�
		//USES_CONVERSION;
		//char *cstrName = W2A( theApp.GetDefectTypeName(resultBlob->nDefectJudge[nFori]) );
		double nDefect_distance = sqrt(pow((double)(resultBlob->ptRB[nFori].x - resultBlob->ptLT[nFori].x), 2) +
			pow((double)(resultBlob->ptRB[nFori].y - resultBlob->ptLT[nFori].y), 2));

		//������ ���� �ҷ�ũ��� ����..
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

	// ��� Log ����
	FILE *out = NULL;

	if (nImageDefectCount == NULL)
		nImageDefectCount = &nDefectNum;

	if (*nImageDefectCount == 0)
		_wfopen_s(&out, strPath, _T("wt"));
	else
		_wfopen_s(&out, strPath, _T("at"));

	// ���� ó��.
	if (out == NULL)		return;

	// �������� ROI ��� ���� �� ���ʿ��� Header ����
	if (*nImageDefectCount == 0)
	{
		fprintf_s(out, "%S,,", strPanel_ID);

		fprintf_s(out, "\n");


		fprintf_s(out, ",\
 						�̹� ����,,");

		fprintf_s(out, "\n");

		fprintf_s(out, "Index  No				        ,\
						Defect No				        ,\
 						��						,\
 						û						,\
						ȫ						,\
						�̹�������(um)			,\
						�̹�Area(pixel)			,\
						��������				,\
						�̹�������(����um)		,,");

		fprintf_s(out, "\n");
	}

	// KSW 17.07.27 ���α׷� �ٿ� ����
	char szPath[MAX_PATH] = { 0, };
	for (int nFori = 0; nFori < resultPanelData.m_ListDefectInfo.GetCount(); nFori++)
	{

		if (
			//////////////////////////////////////////////////////////////////////////�ҷ���
			//��ο� �ҷ�
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE ||
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||

			//////////////////////////////////////////////////////////////////////////
			//���� �ҷ�
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||

			////////////////////////////////////////////////////////////////////////// �ĺ���
			resultPanelData.m_ListDefectInfo[nFori].Defect_Type == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK)
		{
			double nDefect_distance = sqrt(pow((double)(resultPanelData.m_ListDefectInfo[nFori].Pixel_End_X - resultPanelData.m_ListDefectInfo[nFori].Pixel_Start_X), 2) +
				pow((double)(resultPanelData.m_ListDefectInfo[nFori].Pixel_End_Y - resultPanelData.m_ListDefectInfo[nFori].Pixel_Start_Y), 2));


			//������ ���� �ҷ�ũ��� ����..
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
			////////////////////////////////////////////////////////////////////////// ��Ĩ Contour ��ǥ Get

			
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
			////////////////////////////////////////////////////////////////////////// ��Ĩ�� �ҷ� ���� ��ǥ �� ���� Get
			

			cv::line(matSrc_Draw, vertices[0], vertices[1], cv::Scalar(0, 255, 0), 1);//Ȯ�ο�
			cv::line(matSrc_Draw, vertices[1], vertices[2], cv::Scalar(0, 255, 0), 1);//Ȯ�ο�
			cv::line(matSrc_Draw, vertices[2], vertices[3], cv::Scalar(0, 255, 0), 1);//Ȯ�ο�
			cv::line(matSrc_Draw, vertices[3], vertices[0], cv::Scalar(0, 255, 0), 1);//Ȯ�ο�

			//cv::circle(matSrc_Draw, ptCenter, (int)r, Scalar(255, 0, 0), 1);//Ȯ�ο�
			//cv::rectangle(matSrc_Draw, BoungRect, Scalar(0, 0, 255), 1);//Ȯ�ο�
			
			cv::circle(matSrc_Draw, ptCenter, 40.0, Scalar(0, 0, 255), 5);//Ȯ�ο�

			cv::circle(matSrc_Draw, pt1, 1, Scalar(255, 255, 255), 1);//Ȯ�ο�
			cv::circle(matSrc_Draw, pt2, 1, Scalar(255, 255, 255), 1);//Ȯ�ο�

			////////////////////////////////////////////////////////////////////////// �̹����� ���ü� �˻�
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
					////////////////////////////////////////////////////////////////////////// �̹� Contour ��ǥ Get
					float dSub_r = 0.f;
					cv::Point2f ptSubCenter;

					cv::minEnclosingCircle(vPt_Sub, ptSubCenter, dSub_r);

					//cv::Point2d ptSubCenter(resultPanelData.m_ListDefectInfo[k].Pixel_Center_X, resultPanelData.m_ListDefectInfo[k].Pixel_Center_Y);

					cv::circle(matSrc_Draw, ptSubCenter, 3, Scalar(255, 153, 51), 1); //Ȯ�ο�

					double dDistance_Main = 0.0;

					double dDistance_Sub1 = (double)sqrt(pow(pt1.x - ptSubCenter.x, 2) + pow(pt1.y - ptSubCenter.y, 2));
					double dDistance_Sub2 = (double)sqrt(pow(pt2.x - ptSubCenter.x, 2) + pow(pt2.y - ptSubCenter.y, 2));

					cv::Point2d ptMain(0.0 , 0.0);

					if (dDistance_Sub1 < dDistance_Sub2) {
						dDistance_Main = dDistance_Sub1;
						ptMain = pt1;
					}// ��Ĩ�� �ҷ� ������ �̹� �ҷ����� �Ÿ� ���
					else {
						dDistance_Main = dDistance_Sub2;
						ptMain = pt2;
					}

					if (dDistance_Main < 40.0) {
						cv::circle(matSrc_Draw, ptSubCenter, 15, Scalar(0, 0, 255), 2); //Ȯ�ο�
						//cv::line(matMask_Active, ptSubCenter, ptMain, cv::Scalar(0, 0, 255), 1);//Ȯ�ο�
						//////////////////////////////////////////////////////////////////////////
						// �Ÿ��� �����ٸ� ��Ĩ�� �ҷ��� ������ ���� ������ �̹��ҷ��� �� ������ ���ϴ��� Judge
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

							if (d < 3.0) { // ��Ĩ�� �ҷ� ������ ������ �̹��� �ҷ��� �������� ��ġ�� ����

								cv::circle(matSrc_Draw, ptSubCenter, 20, Scalar(0, 255, 0), 2); //Ȯ�ο�
								cv::line(matSrc_Draw, ptSubCenter, ptMain, cv::Scalar(0, 255, 0), 2);//Ȯ�ο�


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

			
			resultPanelData.m_ListDefectInfo.RemoveAt(i); //Judge ���� ��Ĩ�� ���� ����
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
	// P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	//////////////////////////////////////////////////////////////////////////
	// CKI 22.02.16 - �ҷ� �ĺ��� �ֺ� �̹� ����
	//////////////////////////////////////////////////////////////////////////

	if (dAlignPara != NULL)
	{
		int		nDefectTypeM, nDefectTypeS;
		int		nAdjustRange = 10;
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
				if (nDefectTypeM == E_DEFECT_JUDGEMENT_RETEST_POINT_DARK)		//�ĺ��� �̹�

				{
					// P/S ��� ��ǥ ����

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
						if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
						{

							// P/S ��� ��ǥ ����
							ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
							ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

							// ���� �ȿ� ������ ����
							if (rcDeleteArea.PtInRect(ptCenterSub))
							{
								// Point �ҷ� ����
								resultPanelData.m_ListDefectInfo[j].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_POINT_DARK;
							}
							else
							{
								j++; //�����ҷ�����
								continue;
							}
						}
						else j++; // �ĺ��� �̹� �ҷ��� �ƴ� ��� �����ҷ�����
					}
				}
			}
		}
	}

	return true;
}

bool AviInspection::DeleteOverlapDefect_LCP_Moisture_Substance(ResultPanelData& resultPanelData, double* dAlignPara)
{
	// P/S ��忡 ���� ��ǥ ��
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if (resultPanelData.m_ListDefectInfo.GetCount() <= 0)
		return true;

	int nDeleteFlag = (int)dAlignPara[E_PARA_LCP_MOISTURE_SUBSTANCE_DELETE_FLAG];

	if (nDeleteFlag <= 0) return true;
	//////////////////////////////////////////////////////////////////////////
	// CKI 22.02.16 - �ҷ� �ĺ��� �ֺ� �̹� ����
	//////////////////////////////////////////////////////////////////////////

	if (dAlignPara != NULL)
	{
		int		nDefectTypeM, nDefectTypeS;
		int		nAdjustRange = 10;
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
				if (nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT)		//�ĺ��� �̹�

				{
					// P/S ��� ��ǥ ����

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
						if (resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE ||
							resultPanelData.m_ListDefectInfo[j].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
						{

							// P/S ��� ��ǥ ����
							ptCenterSub.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
							ptCenterSub.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

							// ���� �ȿ� ������ ����
							if (rcDeleteArea.PtInRect(ptCenterSub))
							{
								// Point �ҷ� ����
								resultPanelData.m_ListDefectInfo[j].Defect_Type = E_DEFECT_JUDGEMENT_RETEST_POINT_DARK;
							}
							else
							{
								j++; //�����ҷ�����
								continue;
							}
						}
						else j++; // �ĺ��� �̹� �ҷ��� �ƴ� ��� �����ҷ�����
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
	//Excel Data ����
	//////////////////////////////////////////////////////////////////////////
	int nImageDefectCount = 0;
	// 				BlobFeatureSave_Sub(pResultBlob_Sub, strResultPath, &nImageDefectCount);
	//////////////////////////////////////////////////////////////////////////

	//strResultPath.Format(_T("%s\\%s\\%d_%s_CAM%02d_LCP.csv"), INSP_PATH, strPanelID_, nImageNum, theApp.GetGrabStepName(nImageNum), nCameraNum_);
	CString strExcelPath;
	strExcelPath.Format(_T("%s\\%d_%s_CAM%02d_LCP.csv"), strReportPath, theApp.GetImageNum(nMainImage_Num), theApp.GetGrabStepName(theApp.GetImageNum(nMainImage_Num)), 0);
	//�ӽ� ��������� ����
	//strResultPath.Format(_T("D:\\%s.csv"), strPanelID_);

	nImageDefectCount = 0;

	BlobFeatureSave_LCP(resultPanelData, strExcelPath, strPanelID_, dResolution, &nImageDefectCount);


	//////////////////////////////////////////////////////////////////////////
	//��� Image ����
	//////////////////////////////////////////////////////////////////////////
	int rect_offset = 20;
	CString strFont;

	

	cv::Mat matReport;

	matColorImage[theApp.GetImageNum(nMainImage_Num)][0].copyTo(matReport);

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (
			//////////////////////////////////////////////////////////////////////////�ҷ���
			//��ο� �ҷ�
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_DIMPLE ||
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_LINE_Y_DARK ||

			//////////////////////////////////////////////////////////////////////////
			//���� �ҷ�
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||

			////////////////////////////////////////////////////////////////////////// �ĺ���
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

	strFont.Format(_T("%s\\%s_�˻�Ȯ�ο�.bmp"), strReportPath, (LPCWSTR)strPanelID_);
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