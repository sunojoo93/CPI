#include "stdafx.h"
#include "JudgeDefect.h"
#include "VSAlgorithmTask.h"
#include "DllInterface.h"
#include "Markup.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

JudgeDefect::JudgeDefect()
{
}

JudgeDefect::~JudgeDefect(void)
{
}

bool JudgeDefect::m_fnDefectFiltering(cv::Mat& MatDrawBuffer, int nImageNum, int nCameraNum, stDefectInfo* pResultBlob, tAlignInfo stThrdAlignInfo, int nRatio)
{
	TRACE("\n[%s][%s] - Start\n", __FILE__, __FUNCTION__);

	// ���� ó��
	if( pResultBlob == NULL )	return false;

	// �ҷ� ����
	int nDefectCount = pResultBlob->nDefectCount;

	// ��˻� ���� ROI ���� �޾ƿ���
	int			nFilterROICnt = theApp.GetFilterROICnt(nImageNum, nCameraNum);
 	cv::Point	ptDefectCenter;
	CRect		rectFilterArea;

	// ��˻� ���� ROI ���� ��ŭ
	for (int nROICnt= 0; nROICnt < nFilterROICnt; nROICnt++)
	{
		if (theApp.GetUseFilterROI(nImageNum, nCameraNum, nROICnt))
		{
			// ��˻� ���� ROI ��������
			rectFilterArea = theApp.GetFilterROI(nImageNum, nCameraNum, nROICnt, nRatio);

			// ��˻� ���� - Left-Top ��ǥ�� �������� ��ǥ���� ������ ����
			rectFilterArea.OffsetRect( CPoint(stThrdAlignInfo.ptStandard.x, stThrdAlignInfo.ptStandard.y) );

			// Cell ������ ��ǥ
			cv::Point ptPoint[4];

			// ���� ��ǥ�� ��ȸ�� �ؾ���
			if(theApp.m_Config.GetEqpType()!=EQP_APP)
				RotateRect(rectFilterArea, ptPoint, stThrdAlignInfo);
			//APP�� ȸ�� ��Ű�� ���� 2017.09.23 LJM
			else
				RotateRect(rectFilterArea, ptPoint, stThrdAlignInfo, false);
			
			// ��˻� ROI �׸���
			DrawAdjustROI(MatDrawBuffer, ptPoint, theApp.GetFilterROIName(nImageNum, nCameraNum, nROICnt), nROICnt, eFilterROI);

			// �ҷ� ���� ��ŭ
			for (int nForDefect=0 ; nForDefect<nDefectCount ; nForDefect++)
			{
				// 			// �ҷ� �߽�
				// 			if( m_stThrdAlignInfo.ptCorner )
				// 			{
				// 				// ���� ���� ��ǥ�� ����
				// 				ptDefectCenter.x = (LONG)pResultBlob->nCenterx[nForDefect] + m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x;
				// 				ptDefectCenter.y = (LONG)pResultBlob->nCentery[nForDefect] + m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y;
				// 			}
				// 			else
				// 			{
				// �ҷ� ��ǥ - ���� ���� ����
				ptDefectCenter.x = (pResultBlob->ptLT[nForDefect].x + pResultBlob->ptRB[nForDefect].x) / 2;
				ptDefectCenter.y = (pResultBlob->ptLT[nForDefect].y + pResultBlob->ptRB[nForDefect].y) / 2;
				// 			}			

				// �߽��� ��˻� ���� �ȿ� �ִ°�?
				if (PtInPolygon( ptPoint, ptDefectCenter, 4 ) )
				{
					pResultBlob->bUseResult[nForDefect] = false;
				}
			}
		}
	}

	TRACE("\n[%s][%s] - End\n", __FILE__, __FUNCTION__);

	return true;
}

// �ٰ��� ���� �ȿ� ���� �ִ��� ��/��
bool JudgeDefect::PtInPolygon(cv::Point* ptPolygon, cv::Point ptPoint, int nPolygonCnt)
{
	bool flag = false;

	// step 1.
	for ( int i=0, j=nPolygonCnt-1 ; i<nPolygonCnt ; j=i++ )
	{
		// step 2.
		if ( ((ptPolygon[i].y > ptPoint.y) != (ptPolygon[j].y > ptPoint.y)) &&
			// step 3.
			(ptPoint.x < (ptPoint.y - ptPolygon[i].y) * (ptPolygon[j].x - ptPolygon[i].x) / (ptPolygon[j].y - ptPolygon[i].y) + ptPolygon[i].x) ) 
			flag = !flag;
	}

	return flag;
}

void JudgeDefect::ConsolidateResult(const CString strPanelID, const CString strDrive, CWriteResultInfo WrtResultInfo, ResultBlob_Total* pResultBlob_Total, ResultPanelData& resultPanelData, const int nImageNum, const int nCameraNum, int nRatio, ENUM_INSPECT_MODE eInspMode)
{
	// ����� ����
	stDefectInfo*		pRb;		

	// Image �� ���� ��� (Alg) ���� ��������
	int nImageDefectCount = 0;	

	// �̹����� ��� ����
	for(POSITION pos = pResultBlob_Total->GetHeadPosition_ResultBlob(); pos != NULL;)
	{
		pRb = pResultBlob_Total->GetNext_ResultBlob(pos);

		// KSW 17.09.07 - �ʿ��� ��� ���
		//if( theApp.GetCommonParameter()->bIFImageSaveFlag ) // 21.12.10 choi
		if (true)
		{
			CString strResultPath = _T("");		
			strResultPath.Format(_T("%s\\%s\\%d_%s_CAM%02d.csv"), INSP_PATH, strPanelID, pRb->nImageNumber, theApp.GetGrabStepName(nImageNum), nCameraNum);

			// ���Ϻ� ��� ���� ����
			BlobFeatureSave(pRb, strResultPath, &nImageDefectCount);
		}

		for(int nListCnt = 0; nListCnt < pRb->nDefectCount; nListCnt++)
		{
			// ���� ���ϴ� �ҷ� ���͸�
			if (!pRb->bUseResult[nListCnt])	continue;

			// 17.03.10 � ���� ���󿡼� ����Ǿ����� �߰� �ʿ�
			ResultDefectInfo* pResultDefectInfo = new ResultDefectInfo;

			// AVI/SVI/APP �˰��򿡼� ���� �����(pRb)�� ���յ� ���(pResultDefectInfo)���� Casting
			GetDefectInfo(WrtResultInfo, pResultDefectInfo, pRb, nListCnt, nImageNum, nCameraNum, nRatio);

			resultPanelData.Add_DefectInfo(*pResultDefectInfo);

			SAFE_DELETE(pResultDefectInfo);
		}
	}
}

void JudgeDefect::RotateRect(CRect rcSrc, cv::Point* pPtDst, tAlignInfo stThrdAlignInfo, bool bRotate)
{	
	pPtDst[E_CORNER_LEFT_TOP].x = rcSrc.left;
	pPtDst[E_CORNER_LEFT_TOP].y = rcSrc.top;
	pPtDst[E_CORNER_RIGHT_TOP].x = rcSrc.right;
	pPtDst[E_CORNER_RIGHT_TOP].y = rcSrc.top;
	pPtDst[E_CORNER_RIGHT_BOTTOM].x = rcSrc.right;
	pPtDst[E_CORNER_RIGHT_BOTTOM].y = rcSrc.bottom;
	pPtDst[E_CORNER_LEFT_BOTTOM].x = rcSrc.left;
	pPtDst[E_CORNER_LEFT_BOTTOM].y = rcSrc.bottom;

	if(bRotate)
	{
		// ��ȸ��
		Align_DoRotatePoint(pPtDst[E_CORNER_LEFT_TOP],		pPtDst[E_CORNER_LEFT_TOP],		 stThrdAlignInfo.ptAlignCenter, -stThrdAlignInfo.dAlignTheta);
		Align_DoRotatePoint(pPtDst[E_CORNER_RIGHT_TOP],		pPtDst[E_CORNER_RIGHT_TOP],		 stThrdAlignInfo.ptAlignCenter, -stThrdAlignInfo.dAlignTheta);
		Align_DoRotatePoint(pPtDst[E_CORNER_RIGHT_BOTTOM],	pPtDst[E_CORNER_RIGHT_BOTTOM],	 stThrdAlignInfo.ptAlignCenter, -stThrdAlignInfo.dAlignTheta);
		Align_DoRotatePoint(pPtDst[E_CORNER_LEFT_BOTTOM],	pPtDst[E_CORNER_LEFT_BOTTOM],	 stThrdAlignInfo.ptAlignCenter, -stThrdAlignInfo.dAlignTheta);
	}
}

void JudgeDefect::NumberingDefect(const CString strModelID, const CString strPanelID, const CString strLotID, CWriteResultInfo WrtResultInfo, ResultPanelData& ResultPanelData, int nRatio)
{
	// ������ ����
	ResultPanelData.m_ResultPanel.Recipe_ID	= strModelID;
	ResultPanelData.m_ResultPanel.Panel_ID	= strPanelID;
	ResultPanelData.m_ResultPanel.LOT_ID = strLotID;
	ResultPanelData.m_ResultPanel.SetAlignCellROI(&m_stThrdAlignInfo.rcAlignCellROI, 1.0 / nRatio);

	double dHResolution = 0.0, dVResolution = 0.0;
	WrtResultInfo.GetCalcResolution(m_stThrdAlignInfo.rcAlignCellROI, dHResolution, dVResolution);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, FALSE, FALSE, 
				_T("Saved Resolution = %.3f -> Calc Resolution = H : %.3f, V : %.3f (Panel Size = Width : %d, Height : %d / Ratio : %d)"),
				WrtResultInfo.GetCamResolution(0), dHResolution, dVResolution, m_stThrdAlignInfo.rcAlignCellROI.width, m_stThrdAlignInfo.rcAlignCellROI.height, nRatio);
	
	for(int nListCnt = 0; nListCnt < ResultPanelData.m_ListDefectInfo.GetCount(); nListCnt++)
	{
		// �ҷ� ���� ( UI���� 1���� ������ �� )
		ResultPanelData.m_ListDefectInfo[nListCnt].Defect_No = nListCnt+1;

		Coord coordStart(0, 0), coordEnd(0, 0), coordCenter(0, 0), coordRepair(0, 0);
		GD_POINT lineStart(0, 0), lineEnd(0, 0), lineCenter(0, 0), lineRepair(0, 0);
		Coord CoordPixel(0, 0);
		int nCameraNum = 0, nDefectRatio = 0;

		nCameraNum		= ResultPanelData.m_ListDefectInfo[nListCnt].Camera_No;
		nDefectRatio	= ResultPanelData.m_ListDefectInfo[nListCnt].nRatio;

		// �ҷ� ũ�� 0 �� ��� ����ó��
		if (ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_Start_X - ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_End_X == 0 &&
			ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_Start_Y - ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_End_Y == 0)
		{
			coordStart = Coord(0, 0);
			coordCenter = Coord(0, 0);
			coordEnd = Coord(0, 0);
			coordRepair = Coord(0, 0);

			lineStart = GD_POINT(0, 0);
			lineCenter = GD_POINT(0, 0);
			lineEnd = GD_POINT(0, 0);			
			lineRepair = GD_POINT(0, 0);
		}
		else
		{
			// Defect Start X, Y
			CoordPixel.X	= (DOUBLE)ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_Start_X;
			CoordPixel.Y	= (DOUBLE)ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_Start_Y;
			/// 17.07.07 ��ü Panel ����ġ���� ���� ����ϴ� ������� ����
			coordStart		= WrtResultInfo.CalcWorkCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);
			lineStart		= WrtResultInfo.CalcGateDataCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);

			// Defect End X, Y
			CoordPixel.X	= (DOUBLE)ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_End_X;
			CoordPixel.Y	= (DOUBLE)ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_End_Y;
			coordEnd		= WrtResultInfo.CalcWorkCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);
			lineEnd			= WrtResultInfo.CalcGateDataCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);

			// Defect Center X, Y - ���� �̻��
			CoordPixel.X	= ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_Center_X;
			CoordPixel.Y	= ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_Center_Y;
			coordCenter		= WrtResultInfo.CalcWorkCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);
			lineCenter		= WrtResultInfo.CalcGateDataCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);

			// Repair �� �Ѱ��ִ� ��ǥ
			CoordPixel.X	= ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_Repair_X;
			CoordPixel.Y	= ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_Repair_Y;
			coordRepair		= WrtResultInfo.CalcWorkCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);
			lineRepair		= WrtResultInfo.CalcGateDataCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);			
		}

		ResultPanelData.m_ListDefectInfo[nListCnt].Gate_Start_No	= min(lineStart.Gate, lineEnd.Gate);
		ResultPanelData.m_ListDefectInfo[nListCnt].Data_Start_No	= min(lineStart.Data, lineEnd.Data);
		ResultPanelData.m_ListDefectInfo[nListCnt].Gate_End_No		= max(lineStart.Gate, lineEnd.Gate);
		ResultPanelData.m_ListDefectInfo[nListCnt].Data_End_No		= max(lineStart.Data, lineEnd.Data);
		ResultPanelData.m_ListDefectInfo[nListCnt].Coord_Start_X	= min(coordStart.X, coordEnd.X);
		ResultPanelData.m_ListDefectInfo[nListCnt].Coord_Start_Y	= min(coordStart.Y, coordEnd.Y);
		ResultPanelData.m_ListDefectInfo[nListCnt].Coord_End_X		= max(coordStart.X, coordEnd.X);
		ResultPanelData.m_ListDefectInfo[nListCnt].Coord_End_Y		= max(coordStart.Y, coordEnd.Y);		
		ResultPanelData.m_ListDefectInfo[nListCnt].Repair_Gate		= lineRepair.Gate;
		ResultPanelData.m_ListDefectInfo[nListCnt].Repair_Data		= lineRepair.Data;
		ResultPanelData.m_ListDefectInfo[nListCnt].Repair_Coord_X	= coordRepair.X;
		ResultPanelData.m_ListDefectInfo[nListCnt].Repair_Coord_Y	= coordRepair.Y;		
	}
}

// AD �ҷ� ����Ʈ �߰� ( �߰��� ��� true / �߰� �ȵȰ�� false )
bool JudgeDefect::JudgeADDefect(int nImageNum, int nCameraNum, int nStageNo, int nImageWidth, int nImageHeight, ResultBlob_Total* pResultBlob_Total, int nDefectAD, ENUM_INSPECT_MODE eInspMode, bool bAD)
{
	// AD �ҷ��� ��� ��� �߰� ��, ������ ( �ٸ� �˰��� �˻� ���� )

	// �˻� ���(�ҷ�) ���� ����ü
	stDefectInfo* pAD = new stDefectInfo(2, nImageNum);

	// UI�� �Ѱ��� ��� �ֱ�
	pAD->nArea			[0]		= 0;	//nImageWidth * nImageHeight;	// 17.06.23 ���̳ʽ� ��ǥ ����
	pAD->nMaxGV			[0]		= 255;
	pAD->nMinGV			[0]		= 0;
	pAD->dMeanGV		[0]		= 0;

	pAD->ptLT			[0].x	= 0;
	pAD->ptLT			[0].y	= 0;
	pAD->ptRT			[0].x	= 0;	//nImageWidth-1;	// 17.06.23 ���̳ʽ� ��ǥ ����
	pAD->ptRT			[0].y	= 0;
	pAD->ptRB			[0].x	= 0;	//nImageWidth-1;	// 17.06.23 ���̳ʽ� ��ǥ ����
	pAD->ptRB			[0].y	= 0;	//nImageHeight-1;	// 17.06.23 ���̳ʽ� ��ǥ ����
	pAD->ptLB			[0].x	= 0;
	pAD->ptLB			[0].y	= 0;	//nImageHeight-1;	// 17.06.23 ���̳ʽ� ��ǥ ����

	pAD->dBackGroundGV	[0]		= 0;

	pAD->dCompactness	[0]		= 0;
	pAD->dSigma			[0]		= 0;
	pAD->dBreadth		[0]		= 0;	//nImageHeight;	// 17.06.23 ���̳ʽ� ��ǥ ����
	pAD->dF_Min			[0]		= 0;	//nImageHeight;	// 17.06.23 ���̳ʽ� ��ǥ ����
	pAD->dF_Max			[0]		= 0;	//nImageWidth;	// 17.06.23 ���̳ʽ� ��ǥ ����
	pAD->dF_Elongation	[0]		= 0;
	pAD->dCompactness	[0]		= 0;
	
	// ���
	pAD->nDefectColor	[0]		= E_DEFECT_COLOR_DARK;

	pAD->nDefectJudge	[0]		= nDefectAD;
	pAD->nPatternClassify[0]	= nImageNum;

	// ī��Ʈ ����
	pAD->nDefectCount = 1;

	// �ҷ� ���� ����
	pResultBlob_Total->AddTail_ResultBlobAndAddOffset(pAD, NULL);

	// AutoRun �� ��� AD �ҷ� Counting
	if (eInspMode == eAutoRun && bAD)
		m_fnCountingStageAD(nImageNum, nStageNo, nDefectAD);

	// AD �ҷ��� ��� true
	return true;
}

// Stage �� AD �ҷ� ī��Ʈ ��� �߰�
bool JudgeDefect::m_fnCountingStageAD(int nImageNum, int nStageNo, int nDefectType)
{
	// ���ϸ� / ���� / Ű ����
	CString strLogPath = _T(""), strSection = _T(""), strKey = _T("");
	strLogPath.Format(_T("%s\\CountingStageAD_PC%02d.INI"), DEFECT_INFO_PATH, theApp.m_Config.GetPCNum());
	strSection.Format(_T("Stage_%d_%d"), nStageNo, theApp.m_Config.GetPCNum());
	if (nDefectType == E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL || nDefectType == E_DEFECT_JUDGEMENT_DISPLAY_OFF)
		strKey.Format(_T("AD"));
	else
		strKey.Format(_T("%s_GV"), theApp.GetGrabStepName(nImageNum));

	TRY 
	{
		// ���� Count �б� / ���� �� ����
		EnterCriticalSection(&theApp.m_csCntFileSafe);
		int nCurCount = GetPrivateProfileInt(strSection, strKey, 0, strLogPath);
		nCurCount++;
		CString strCount = _T("");
		strCount.Format(_T("%d"), nCurCount);
		WritePrivateProfileString(strSection, strKey, strCount, strLogPath);

		LeaveCriticalSection(&theApp.m_csCntFileSafe);
	}
	CATCH (CException, e)
	{
		e->Delete();
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Exception m_fnCountingStageAD()"));
		return false;
	}
	END_CATCH
		
	return true;
}

bool JudgeDefect::JudgementPanelGrade(ResultPanelData& resultPanelData)
{
	int nDefectTypeNum = 0;
	memset(resultPanelData.m_nDefectTrend, 0, sizeof(int) * E_PANEL_DEFECT_TREND_COUNT);

	// �ҷ� Counting - Panel �� ��� �۾����� Ȱ��
	for (int nIndex = 0; nIndex < resultPanelData.m_ListDefectInfo.GetCount(); nIndex++)
	{
		nDefectTypeNum = resultPanelData.m_ListDefectInfo[nIndex].Defect_Type;
		resultPanelData.m_nDefectTrend[nDefectTypeNum]++;

		// Repair ������ ���� BP + DP ���� Count
		if (nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_BRIGHT	||
			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_DARK		)
			resultPanelData.m_nDefectTrend[E_DEFECT_BP_PLUS_DP]++;


		// BP + DP + WD + BD + GD + GB ���� Count
// 		if (nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_BRIGHT		||
// 			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_DARK			||
// 			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT	||
// 			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK	||
// 			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK	||
// 			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
// 			resultPanelData.m_nDefectTrend[E_DEFECT_BP_DP_WB_BD_GD_GB_PLUS]++;



		if (nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_BRIGHT ||
			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_DARK ||
			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT ||
			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK )
			resultPanelData.m_nDefectTrend[E_DEFECT_BP_DP_WB_BD_GD_GB_PLUS]++;

		// Re-test ����
		if (nDefectTypeNum >= E_DEFECT_JUDGEMENT_RETEST_POINT_DARK	&&
			nDefectTypeNum <= E_DEFECT_JUDGEMENT_RETEST_MURA_BRIGHT	)
			resultPanelData.m_nDefectTrend[E_DEFECT_RETEST]++;

		// BP + WB + GB
		if (nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_BRIGHT		||
			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT	||
			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			resultPanelData.m_nDefectTrend[E_DEFECT_BP_WB_GB]++;
	}	

	// �г� ���� �켱������ ���� ��
	std::vector<stPanelJudgePriority> vPanelJudge = theApp.GetPanelJudgeInfo();
	bool bNextOrder = false;
	int nPriority = 0;
	for ( ; nPriority < vPanelJudge.size(); nPriority++)
	{
		bNextOrder = false;
		for (nDefectTypeNum = 0; nDefectTypeNum < E_PANEL_DEFECT_TREND_COUNT; nDefectTypeNum++)
		{
			if (!m_fnCompareValue(resultPanelData.m_nDefectTrend[nDefectTypeNum], vPanelJudge[nPriority].stJudgeInfo[nDefectTypeNum].nRefVal, vPanelJudge[nPriority].stJudgeInfo[nDefectTypeNum].nSign))
			{
				bNextOrder = true;
				break;
			}
		}
		if (!bNextOrder)
			break;
	}
	// ��ġ�ϴ� ������ ������ NG
	if (nPriority == vPanelJudge.size())
	{
		// �� ���� ���� ���
		if (resultPanelData.m_ListDefectInfo.GetCount() == 0)
			resultPanelData.m_ResultPanel.Judge = _T("S");		// �ҷ� ������ V
		else
			resultPanelData.m_ResultPanel.Judge = _T("F");		// S - Scrap (NG)
	}
	// ������ �ش� Grade ����
	else
	{
		resultPanelData.m_ResultPanel.Judge.Format(_T("%s"), vPanelJudge[nPriority].strGrade);
	}

	return true;
}

// �ҷ� �������� ���͸� ���� ���� �� ��ǥ �ҷ� ����
bool JudgeDefect::ApplyReportRule(ResultPanelData& resultPanelData)
{
	int nDefectTypeNum = 0;
		
	int nTotalDefectCount = (int)resultPanelData.m_ListDefectInfo.GetCount();
	int* pDefectTrend = new int[E_PANEL_DEFECT_TREND_COUNT];
	memcpy(pDefectTrend, resultPanelData.m_nDefectTrend, sizeof(int) * E_PANEL_DEFECT_TREND_COUNT);		// ���� Trend ������ �״�� �ΰ� ���� ���� ���� Trend �迭 ����

	int nJudgeIndex = theApp.GetPanelJudgeIndex(resultPanelData.m_ResultPanel.Judge);

	stPanelJudgeInfo stFilterInfo[E_PANEL_DEFECT_TREND_COUNT];
	memcpy(stFilterInfo,theApp.GetReportFilter(nJudgeIndex),sizeof(stPanelJudgeInfo)*E_PANEL_DEFECT_TREND_COUNT);
	//int nOverlapDefectNum[E_DEFECT_JUDGEMENT_COUNT] = {0,}; // ��ǥ �ҷ� �������� �ߺ��� ������ ���� ��� �ش� Defect Num ����
	vector<int>::iterator iter;
	if (/*pReportFilter->bUse*/ TRUE)
	{
		//for (nDefectTypeNum = 0; nDefectTypeNum < E_DEFECT_JUDGEMENT_COUNT; nDefectTypeNum++)	//YWS Defect Count 190510
		for (nDefectTypeNum = 0; nDefectTypeNum < E_PANEL_DEFECT_TREND_COUNT; nDefectTypeNum++)
		{
			if (pDefectTrend[nDefectTypeNum] == 0)	continue;		// �ҷ��� �ϳ��� ������ ���͸� �� �ʿ� �����Ƿ� Skip

			// ���͸� ���ǿ� �´� Type �� �ҷ��� ���� ���� �������� ǥ�� (bUseReport = false)
			if (m_fnCompareValue(pDefectTrend[nDefectTypeNum], stFilterInfo[nDefectTypeNum].nRefVal, stFilterInfo[nDefectTypeNum].nSign))
			{// ���� ���� �ҷ� ����
				 
				pDefectTrend[nDefectTypeNum] = 0;
				for (int nIndex = 0; nIndex < resultPanelData.m_ListDefectInfo.GetCount(); nIndex++)
				{
					if (resultPanelData.m_ListDefectInfo[nIndex].Defect_Type == nDefectTypeNum)
					{
						resultPanelData.m_ListDefectInfo[nIndex].bUseReport = false;
						nTotalDefectCount--;
					}
				}
			}
		}
	}


	nTotalDefectCount = UserDefinedFilter(resultPanelData,nTotalDefectCount); // ����� ���� ����. N�� �̻��� ������ ������ ���� �̻��϶� ������ ����.

	// ��ǥ�ҷ� ����	
	if (nTotalDefectCount != 0)		// �ҷ��� �ϳ��� ������
	{
		// ��ǥ�ҷ� ����	%
		int nHighestDefectRank = 0;
		int nCurDefType = 0;
		int nMostDefectNum = -1;

		for (int nIndex = 0; nIndex < resultPanelData.m_ListDefectInfo.GetCount(); nIndex++)
		{			
			if (resultPanelData.m_ListDefectInfo[nIndex].bUseReport == true)
			{
				nCurDefType = resultPanelData.m_ListDefectInfo[nIndex].Defect_Type;

				if (nMostDefectNum == -1)	// ���� �ҷ�
				{
					nMostDefectNum = nCurDefType;
					nHighestDefectRank = theApp.GetDefectRank(nCurDefType);
					continue;
				}
				// ���� Defect Type �� �� ���� ������ ������ ���� Defect Type�� ����
				if(nHighestDefectRank > theApp.GetDefectRank(nCurDefType))
				{
					nHighestDefectRank = theApp.GetDefectRank(nCurDefType);
					nMostDefectNum = nCurDefType;
				}
				// ������ �����̸� �ҷ� ������ �� ���� Type�� ����
				else if( nHighestDefectRank == theApp.GetDefectRank(nCurDefType))
				{
					if (pDefectTrend[nMostDefectNum] < pDefectTrend[nCurDefType])
					{
						nMostDefectNum = nCurDefType;
					}
				}
			}
		}
		resultPanelData.m_ResultPanel.nFinalDefectNum = nMostDefectNum;

		/// �ҷ����� ��ǥ�ҷ� ���� ��� /////////////////////////////////////
		// �����Ͼ� ��û���� ����/����/ȥ�� �ҷ��� ���� �ϳ��� �����ϵ��� ����
		// �ʿ� �� �ҷ����� ��ǥ�ҷ� ��� �����ϴ� UI �۾� �ʿ�
		// ���� �ϳ��� �ҷ�( == �ҷ���)�� ���ؼ��� ��ǥ�ҷ� �����ϴ� ������ �ʿ�.
		// �������� �ҷ��� �ϳ��� �ҷ������� �ν��ϴ� Ȯ�强�� ���� ��ǥ �ҷ� ��� ���θ� TRUE/FALSE �� �������� �ʰ� Index �ο� (���� Index �� ���� �ҷ���)
		// �ִ� E_DEFECT_JUDGEMENT_COUNT ��ŭ �ҷ��� ���� ����. �ҷ������� ��ǥ�ҷ� �����ϸ� �ߺ� üũ ���� �ʱ� ���� �÷���
		bool bFindDefect[E_DEFECT_JUDGEMENT_COUNT] = {false, };

		for (nDefectTypeNum = 0; nDefectTypeNum < E_DEFECT_JUDGEMENT_COUNT; nDefectTypeNum++)
		{
			// �ҷ����� ��ǥ�ҷ� ��� ����ϴ� ��� (0�̸� �ҷ����� ��ǥ�ҷ� ��� ��� ����)
			// �������� �ҷ��� �ϳ��� �ҷ������� �ν��ؾ� �� ��� ���� �۾� �ʿ�. - �۾� �Ϸ�
			int nGroupIndex = theApp.GetDefectGroup(nDefectTypeNum);
			if (nGroupIndex != 0)		// Defect Type �� ��ǥ �ҷ� �����ϴ� �ҷ������� �����Ǿ� ���� ���
			{				
				// �ҷ��� �ϳ� �̻��� ��
				if (pDefectTrend[nDefectTypeNum] >= 1)
				{
					for (int nIndex = 0; nIndex < resultPanelData.m_ListDefectInfo.GetCount(); nIndex++)
					{
						// ���� �ϳ��� �ҷ��� �����ϰ� ���� ����
						if (resultPanelData.m_ListDefectInfo[nIndex].Defect_Type == nDefectTypeNum)
						{
							if (!bFindDefect[nGroupIndex])
							{
								bFindDefect[nGroupIndex] = true;	// ���� �ҷ������� ��ǥ �ҷ� ������
								continue;
							}
							resultPanelData.m_ListDefectInfo[nIndex].bUseReport = false;
						}
					}
				}
			}
		}
		////////////////////////////////////////////////////////////////////////////////

		try
		{
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Apply Report Rule Read DefectItem List"));
			//////////////////////////////////// ���� ������(Ÿ��) ���� �ε�
			CString strMsg = _T("");
			CFileFind find;
			BOOL bFindFile = FALSE;
			CString strDefItemListXMLPath;
			strDefItemListXMLPath.Format(_T("%s:\\CRUX\\DATA\\TXT\\DEFITEM_LIST.xml"), theApp.m_Config.GETDRV());

			bFindFile = find.FindFile(strDefItemListXMLPath);
			find.Close();

			if (!bFindFile)
			{
				// 			strMsg.Format(_T("Not found defect item list xml file. (%s)"), strDefItemListXMLPath);		
				// 			AfxMessageBox(strMsg);		
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Not found defect item list xml file. (%s)"),strDefItemListXMLPath);
				return false;
			}

			// XML ���� �ε�
			CMarkup xmlDefectItem;
			if(!xmlDefectItem.Load(strDefItemListXMLPath))
			{
				// 			strMsg.Format(_T("Model xml load fail. (%s)"), strDefItemListXMLPath);
				// 			AfxMessageBox(strMsg);
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Model xml load fail. (%s)"),strDefItemListXMLPath);
				return false;
			}

			xmlDefectItem.FindElem();		// DEF_ITEM
			xmlDefectItem.IntoElem();		// inside DEF_ITEM

			CString strDefSysName = _T(""), strDefCode = _T("");
			CMarkup* xmlDefItemList = new CMarkup(xmlDefectItem);
			stDefClassification* stDefClass = new stDefClassification[MAX_MEM_SIZE_E_DEFECT_NAME_COUNT];
			//theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Create stDefClassification"));
			for (int nDefItemIndex=0; nDefItemIndex<MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; nDefItemIndex++)
			{		
				if (xmlDefItemList->FindElem(_T("DefType_%d"), nDefItemIndex))
				{
					strDefSysName = xmlDefItemList->GetAttrib(_T("SysName"));
					//memcpy(stDefClass[nDefItemIndex].strDefectName, strDefSysName.GetBuffer(0), sizeof(stDefClass[nDefItemIndex].strDefectName) - sizeof(TCHAR));
					//_tcscat(stDefClass[nDefItemIndex].strDefectName, _T("\0"));
					COPY_CSTR2TCH(stDefClass[nDefItemIndex].strDefectName,strDefSysName,sizeof(stDefClass[nDefItemIndex].strDefectName));
					strDefCode = xmlDefItemList->GetAttrib(_T("DefCode"));
					//memcpy(stDefClass[nDefItemIndex].strDefectCode, strDefCode.GetBuffer(0), sizeof(stDefClass[nDefItemIndex].strDefectCode) - sizeof(TCHAR));
					//_tcscat(stDefClass[nDefItemIndex].strDefectCode, _T("\0"));
					COPY_CSTR2TCH(stDefClass[nDefItemIndex].strDefectCode,strDefCode,sizeof(stDefClass[nDefItemIndex].strDefectCode));
					xmlDefItemList->ResetMainPos();
				}		
			}
			//theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("SetDefectClassify set data"));
			theApp.SetDefectClassify(stDefClass);
			
			SAFE_DELETE_ARR(stDefClass);
			SAFE_DELETE(xmlDefItemList);


			if( pDefectTrend[E_DEFECT_JUDGEMENT_POINT_BRIGHT] + pDefectTrend[E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT] +
				pDefectTrend[E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK] + pDefectTrend[E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT] >= 2)
			{
				stDefClassification* stDefClass = new stDefClassification[MAX_MEM_SIZE_E_DEFECT_NAME_COUNT];
				memcpy(stDefClass, theApp.GetDefectClassify(), sizeof(stDefClassification) * MAX_MEM_SIZE_E_DEFECT_NAME_COUNT);
				//stDefClass[5].strDefectCode
				_tcscpy(stDefClass[E_DEFECT_JUDGEMENT_POINT_BRIGHT].strDefectCode,_T("PB1000"));
				_tcscpy(stDefClass[E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT].strDefectCode,_T("PB1000"));
				_tcscpy(stDefClass[E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK].strDefectCode,_T("PB1000"));
				_tcscpy(stDefClass[E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT].strDefectCode,_T("PB2000"));
				theApp.SetDefectClassify(stDefClass);
				//theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Change SetDefectClassify"));
			}
			resultPanelData.m_ResultPanel.judge_code_1.Format(_T("%s"), theApp.GetDefectSysName(nMostDefectNum));
			resultPanelData.m_ResultPanel.judge_code_2.Format(_T("%s"), theApp.GetDefectCode(nMostDefectNum));
		}
		catch (CMemoryException* e)
		{
			TCHAR strErr[256];
			e->GetErrorMessage(strErr, 256);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Apply report rule memory Exception : %s"), strErr);
			e->Delete();
			return false;
		}
		catch (CFileException* e)
		{
			TCHAR strErr[256];
			e->GetErrorMessage(strErr, 256);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Apply report rule file Exception : %s"), strErr);
			e->Delete();
			return false;
		}
		catch (CException* e)
		{
			TCHAR strErr[256];
			e->GetErrorMessage(strErr, 256);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Apply report rule Exception : %s"), strErr);
			e->Delete();
			return false;
		}
	}
	else
	{
		resultPanelData.m_ResultPanel.nFinalDefectNum = -1;
		resultPanelData.m_ResultPanel.judge_code_1 = _T("NOTHING");
		resultPanelData.m_ResultPanel.judge_code_2 = _T("NOTHING");
	}
	SAFE_DELETE_ARR(pDefectTrend);

	return true;
}

int JudgeDefect::UserDefinedFilter(ResultPanelData& resultPanelData, int nTotalDefectCount)
{
	int* pDefectTrendCount = new int[E_DEFECT_JUDGEMENT_COUNT];
	//memcpy(pDefectTrend, resultPanelData.m_nDefectTrend, sizeof(int) * E_DEFECT_JUDGEMENT_COUNT);		// ���� Trend ������ �״�� �ΰ� ���� ���� ���� Trend �迭 ����
	for (int nIndex = 0; nIndex < E_DEFECT_JUDGEMENT_COUNT; nIndex++)
		pDefectTrendCount[nIndex] = resultPanelData.m_nDefectTrend[nIndex];
	CString strGrade;

	resultPanelData.m_ResultPanel.Judge;

	//resultPanelData.m_ResultPanel.Judge; �� strGrade�� ���ؼ� ���� ���� GRADE�� ����
	//pDefectTrend�� ���� ������ ������ �޾ƿ� ����
	//�� ����������� ������ ������ �� �̻��̸� ���� �÷��� TRUE

	int nDefectCount = 0;
	std::vector<stUserDefinedFilter> vUserFilter = theApp.GetUserDefinedFilter();
	for (int i = 0 ; i < vUserFilter.size(); i++)
	{
		strGrade.Format(_T("%s"),vUserFilter[i].strGrade);
		if(strGrade == resultPanelData.m_ResultPanel.Judge)
		{
			for(int j = 0 ; j < vUserFilter[i].nFilterItemsCount ; j++)
				nDefectCount += pDefectTrendCount [ vUserFilter[i].nFilterItems[j] ]; // ����� ���� ���Ϳ� ���� ���ִ� Defect�� ��

			if  (m_fnCompareValue(nDefectCount, vUserFilter[i].stFilterInfo.nRefVal, vUserFilter[i].stFilterInfo.nSign) ) // Defect�� ���� ���Ϳ� ���ǵ� ���ǿ� ������
			{
				for(int j = 0 ; j < vUserFilter[i].nFilterItemsCount ; j++)  // 
				{
					for (int nIndex = 0; nIndex < resultPanelData.m_ListDefectInfo.GetCount(); nIndex++)
					{
						if (resultPanelData.m_ListDefectInfo[nIndex].Defect_Type == vUserFilter[i].nFilterItems[j])
						{
							resultPanelData.m_ListDefectInfo[nIndex].bUseReport = true; // ���� ���� �÷��� �ٽ� �츲.
							nTotalDefectCount++;
						}
					}
				}
			}
		}
	}
	SAFE_DELETE_ARR(pDefectTrendCount);
	return nTotalDefectCount;
}

bool JudgeDefect::m_fnCompareValue(int nDefectCount, int nRefCount, int nSign)
{
	bool bRet = false;
	switch(nSign)
	{
	case 0:
		if (nDefectCount == nRefCount)	bRet = true;
		break;
	case 1:
		if (nDefectCount != nRefCount)	bRet = true;
		break;
	case 2:
		if (nDefectCount > nRefCount)	bRet = true;
		break;
	case 3:
		if (nDefectCount < nRefCount)	bRet = true;
		break;
	case 4:
		if (nDefectCount >= nRefCount)	bRet = true;
		break;
	case 5:
		if (nDefectCount <= nRefCount)	bRet = true;
		break;
	}
	return bRet;
}