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

	// 예외 처리
	if( pResultBlob == NULL )	return false;

	// 불량 갯수
	int nDefectCount = pResultBlob->nDefectCount;

	// 비검사 영역 ROI 갯수 받아오기
	int			nFilterROICnt = theApp.GetFilterROICnt(nImageNum, nCameraNum);
 	cv::Point	ptDefectCenter;
	CRect		rectFilterArea;

	// 비검사 영역 ROI 갯수 만큼
	for (int nROICnt= 0; nROICnt < nFilterROICnt; nROICnt++)
	{
		if (theApp.GetUseFilterROI(nImageNum, nCameraNum, nROICnt))
		{
			// 비검사 영역 ROI 가져오기
			rectFilterArea = theApp.GetFilterROI(nImageNum, nCameraNum, nROICnt, nRatio);

			// 비검사 영역 - Left-Top 좌표를 원점으로 좌표값을 가지고 있음
			rectFilterArea.OffsetRect( CPoint(stThrdAlignInfo.ptStandard.x, stThrdAlignInfo.ptStandard.y) );

			// Cell 꼭지점 좌표
			cv::Point ptPoint[4];

			// 원본 좌표로 역회전 해야함
			if(theApp.m_Config.GetEqpType()!=EQP_APP)
				RotateRect(rectFilterArea, ptPoint, stThrdAlignInfo);
			//APP는 회전 시키지 않음 2017.09.23 LJM
			else
				RotateRect(rectFilterArea, ptPoint, stThrdAlignInfo, false);
			
			// 비검사 ROI 그리기
			DrawAdjustROI(MatDrawBuffer, ptPoint, theApp.GetFilterROIName(nImageNum, nCameraNum, nROICnt), nROICnt, eFilterROI);

			// 불량 갯수 만큼
			for (int nForDefect=0 ; nForDefect<nDefectCount ; nForDefect++)
			{
				// 			// 불량 중심
				// 			if( m_stThrdAlignInfo.ptCorner )
				// 			{
				// 				// 원본 기준 좌표로 변경
				// 				ptDefectCenter.x = (LONG)pResultBlob->nCenterx[nForDefect] + m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x;
				// 				ptDefectCenter.y = (LONG)pResultBlob->nCentery[nForDefect] + m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y;
				// 			}
				// 			else
				// 			{
				// 불량 좌표 - 원본 영상 기준
				ptDefectCenter.x = (pResultBlob->ptLT[nForDefect].x + pResultBlob->ptRB[nForDefect].x) / 2;
				ptDefectCenter.y = (pResultBlob->ptLT[nForDefect].y + pResultBlob->ptRB[nForDefect].y) / 2;
				// 			}			

				// 중심이 비검사 영역 안에 있는가?
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

// 다각형 영역 안에 점이 있는지 유/무
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
	// 결과값 취합
	stDefectInfo*		pRb;		

	// Image 별 검출 결과 (Alg) 파일 내보내기
	int nImageDefectCount = 0;	

	// 이미지별 결과 취합
	for(POSITION pos = pResultBlob_Total->GetHeadPosition_ResultBlob(); pos != NULL;)
	{
		pRb = pResultBlob_Total->GetNext_ResultBlob(pos);

		// KSW 17.09.07 - 필요할 경우 사용
		//if( theApp.GetCommonParameter()->bIFImageSaveFlag ) // 21.12.10 choi
		if (true)
		{
			CString strResultPath = _T("");		
			strResultPath.Format(_T("%s\\%s\\%d_%s_CAM%02d.csv"), INSP_PATH, strPanelID, pRb->nImageNumber, theApp.GetGrabStepName(nImageNum), nCameraNum);

			// 패턴별 결과 파일 저장
			BlobFeatureSave(pRb, strResultPath, &nImageDefectCount);
		}

		for(int nListCnt = 0; nListCnt < pRb->nDefectCount; nListCnt++)
		{
			// 보고 안하는 불량 필터링
			if (!pRb->bUseResult[nListCnt])	continue;

			// 17.03.10 어떤 패턴 영상에서 검출되었는지 추가 필요
			ResultDefectInfo* pResultDefectInfo = new ResultDefectInfo;

			// AVI/SVI/APP 알고리즘에서 나온 결과값(pRb)을 통합된 양식(pResultDefectInfo)으로 Casting
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
		// 역회전
		Align_DoRotatePoint(pPtDst[E_CORNER_LEFT_TOP],		pPtDst[E_CORNER_LEFT_TOP],		 stThrdAlignInfo.ptAlignCenter, -stThrdAlignInfo.dAlignTheta);
		Align_DoRotatePoint(pPtDst[E_CORNER_RIGHT_TOP],		pPtDst[E_CORNER_RIGHT_TOP],		 stThrdAlignInfo.ptAlignCenter, -stThrdAlignInfo.dAlignTheta);
		Align_DoRotatePoint(pPtDst[E_CORNER_RIGHT_BOTTOM],	pPtDst[E_CORNER_RIGHT_BOTTOM],	 stThrdAlignInfo.ptAlignCenter, -stThrdAlignInfo.dAlignTheta);
		Align_DoRotatePoint(pPtDst[E_CORNER_LEFT_BOTTOM],	pPtDst[E_CORNER_LEFT_BOTTOM],	 stThrdAlignInfo.ptAlignCenter, -stThrdAlignInfo.dAlignTheta);
	}
}

void JudgeDefect::NumberingDefect(const CString strModelID, const CString strPanelID, const CString strLotID, CWriteResultInfo WrtResultInfo, ResultPanelData& ResultPanelData, int nRatio)
{
	// 레서피 설정
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
		// 불량 순서 ( UI에서 1부터 보내야 함 )
		ResultPanelData.m_ListDefectInfo[nListCnt].Defect_No = nListCnt+1;

		Coord coordStart(0, 0), coordEnd(0, 0), coordCenter(0, 0), coordRepair(0, 0);
		GD_POINT lineStart(0, 0), lineEnd(0, 0), lineCenter(0, 0), lineRepair(0, 0);
		Coord CoordPixel(0, 0);
		int nCameraNum = 0, nDefectRatio = 0;

		nCameraNum		= ResultPanelData.m_ListDefectInfo[nListCnt].Camera_No;
		nDefectRatio	= ResultPanelData.m_ListDefectInfo[nListCnt].nRatio;

		// 불량 크기 0 일 경우 예외처리
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
			/// 17.07.07 전체 Panel 설계치에서 일할 계산하는 방식으로 변경
			coordStart		= WrtResultInfo.CalcWorkCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);
			lineStart		= WrtResultInfo.CalcGateDataCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);

			// Defect End X, Y
			CoordPixel.X	= (DOUBLE)ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_End_X;
			CoordPixel.Y	= (DOUBLE)ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_End_Y;
			coordEnd		= WrtResultInfo.CalcWorkCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);
			lineEnd			= WrtResultInfo.CalcGateDataCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);

			// Defect Center X, Y - 현재 미사용
			CoordPixel.X	= ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_Center_X;
			CoordPixel.Y	= ResultPanelData.m_ListDefectInfo[nListCnt].Pixel_Center_Y;
			coordCenter		= WrtResultInfo.CalcWorkCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);
			lineCenter		= WrtResultInfo.CalcGateDataCoord(m_stThrdAlignInfo.rcAlignCellROI, CoordPixel, nDefectRatio, nRatio);

			// Repair 에 넘겨주는 좌표
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

// AD 불량 리스트 추가 ( 추가된 경우 true / 추가 안된경우 false )
bool JudgeDefect::JudgeADDefect(int nImageNum, int nCameraNum, int nStageNo, int nImageWidth, int nImageHeight, ResultBlob_Total* pResultBlob_Total, int nDefectAD, ENUM_INSPECT_MODE eInspMode, bool bAD)
{
	// AD 불량인 경우 목록 추가 후, 나가기 ( 다른 알고리즘 검사 안함 )

	// 검사 결과(불량) 정보 구조체
	stDefectInfo* pAD = new stDefectInfo(2, nImageNum);

	// UI에 넘겨줄 결과 넣기
	pAD->nArea			[0]		= 0;	//nImageWidth * nImageHeight;	// 17.06.23 마이너스 좌표 수정
	pAD->nMaxGV			[0]		= 255;
	pAD->nMinGV			[0]		= 0;
	pAD->dMeanGV		[0]		= 0;

	pAD->ptLT			[0].x	= 0;
	pAD->ptLT			[0].y	= 0;
	pAD->ptRT			[0].x	= 0;	//nImageWidth-1;	// 17.06.23 마이너스 좌표 수정
	pAD->ptRT			[0].y	= 0;
	pAD->ptRB			[0].x	= 0;	//nImageWidth-1;	// 17.06.23 마이너스 좌표 수정
	pAD->ptRB			[0].y	= 0;	//nImageHeight-1;	// 17.06.23 마이너스 좌표 수정
	pAD->ptLB			[0].x	= 0;
	pAD->ptLB			[0].y	= 0;	//nImageHeight-1;	// 17.06.23 마이너스 좌표 수정

	pAD->dBackGroundGV	[0]		= 0;

	pAD->dCompactness	[0]		= 0;
	pAD->dSigma			[0]		= 0;
	pAD->dBreadth		[0]		= 0;	//nImageHeight;	// 17.06.23 마이너스 좌표 수정
	pAD->dF_Min			[0]		= 0;	//nImageHeight;	// 17.06.23 마이너스 좌표 수정
	pAD->dF_Max			[0]		= 0;	//nImageWidth;	// 17.06.23 마이너스 좌표 수정
	pAD->dF_Elongation	[0]		= 0;
	pAD->dCompactness	[0]		= 0;
	
	// 밝기
	pAD->nDefectColor	[0]		= E_DEFECT_COLOR_DARK;

	pAD->nDefectJudge	[0]		= nDefectAD;
	pAD->nPatternClassify[0]	= nImageNum;

	// 카운트 증가
	pAD->nDefectCount = 1;

	// 불량 정보 병합
	pResultBlob_Total->AddTail_ResultBlobAndAddOffset(pAD, NULL);

	// AutoRun 일 경우 AD 불량 Counting
	if (eInspMode == eAutoRun && bAD)
		m_fnCountingStageAD(nImageNum, nStageNo, nDefectAD);

	// AD 불량인 경우 true
	return true;
}

// Stage 별 AD 불량 카운트 기능 추가
bool JudgeDefect::m_fnCountingStageAD(int nImageNum, int nStageNo, int nDefectType)
{
	// 파일명 / 섹션 / 키 지정
	CString strLogPath = _T(""), strSection = _T(""), strKey = _T("");
	strLogPath.Format(_T("%s\\CountingStageAD_PC%02d.INI"), DEFECT_INFO_PATH, theApp.m_Config.GetPCNum());
	strSection.Format(_T("Stage_%d_%d"), nStageNo, theApp.m_Config.GetPCNum());
	if (nDefectType == E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL || nDefectType == E_DEFECT_JUDGEMENT_DISPLAY_OFF)
		strKey.Format(_T("AD"));
	else
		strKey.Format(_T("%s_GV"), theApp.GetGrabStepName(nImageNum));

	TRY 
	{
		// 현재 Count 읽기 / 증가 후 쓰기
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

	// 불량 Counting - Panel 별 통계 작업에도 활용
	for (int nIndex = 0; nIndex < resultPanelData.m_ListDefectInfo.GetCount(); nIndex++)
	{
		nDefectTypeNum = resultPanelData.m_ListDefectInfo[nIndex].Defect_Type;
		resultPanelData.m_nDefectTrend[nDefectTypeNum]++;

		// Repair 판정을 위해 BP + DP 개수 Count
		if (nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_BRIGHT	||
			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_DARK		)
			resultPanelData.m_nDefectTrend[E_DEFECT_BP_PLUS_DP]++;


		// BP + DP + WD + BD + GD + GB 개수 Count
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

		// Re-test 개수
		if (nDefectTypeNum >= E_DEFECT_JUDGEMENT_RETEST_POINT_DARK	&&
			nDefectTypeNum <= E_DEFECT_JUDGEMENT_RETEST_MURA_BRIGHT	)
			resultPanelData.m_nDefectTrend[E_DEFECT_RETEST]++;

		// BP + WB + GB
		if (nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_BRIGHT		||
			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT	||
			nDefectTypeNum == E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT)
			resultPanelData.m_nDefectTrend[E_DEFECT_BP_WB_GB]++;
	}	

	// 패널 판정 우선순위에 따라 비교
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
	// 일치하는 유형이 없으면 NG
	if (nPriority == vPanelJudge.size())
	{
		// 비교 구문 없는 경우
		if (resultPanelData.m_ListDefectInfo.GetCount() == 0)
			resultPanelData.m_ResultPanel.Judge = _T("S");		// 불량 없으면 V
		else
			resultPanelData.m_ResultPanel.Judge = _T("F");		// S - Scrap (NG)
	}
	// 있으면 해당 Grade 기입
	else
	{
		resultPanelData.m_ResultPanel.Judge.Format(_T("%s"), vPanelJudge[nPriority].strGrade);
	}

	return true;
}

// 불량 상위보고 필터링 조건 적용 후 대표 불량 선정
bool JudgeDefect::ApplyReportRule(ResultPanelData& resultPanelData)
{
	int nDefectTypeNum = 0;
		
	int nTotalDefectCount = (int)resultPanelData.m_ListDefectInfo.GetCount();
	int* pDefectTrend = new int[E_PANEL_DEFECT_TREND_COUNT];
	memcpy(pDefectTrend, resultPanelData.m_nDefectTrend, sizeof(int) * E_PANEL_DEFECT_TREND_COUNT);		// 기존 Trend 파일은 그대로 두고 상위 보고 기준 Trend 배열 생성

	int nJudgeIndex = theApp.GetPanelJudgeIndex(resultPanelData.m_ResultPanel.Judge);

	stPanelJudgeInfo stFilterInfo[E_PANEL_DEFECT_TREND_COUNT];
	memcpy(stFilterInfo,theApp.GetReportFilter(nJudgeIndex),sizeof(stPanelJudgeInfo)*E_PANEL_DEFECT_TREND_COUNT);
	//int nOverlapDefectNum[E_DEFECT_JUDGEMENT_COUNT] = {0,}; // 대표 불량 선정에서 중복된 순위가 있을 경우 해당 Defect Num 저장
	vector<int>::iterator iter;
	if (/*pReportFilter->bUse*/ TRUE)
	{
		//for (nDefectTypeNum = 0; nDefectTypeNum < E_DEFECT_JUDGEMENT_COUNT; nDefectTypeNum++)	//YWS Defect Count 190510
		for (nDefectTypeNum = 0; nDefectTypeNum < E_PANEL_DEFECT_TREND_COUNT; nDefectTypeNum++)
		{
			if (pDefectTrend[nDefectTypeNum] == 0)	continue;		// 불량이 하나도 없으면 필터링 할 필요 없으므로 Skip

			// 필터링 조건에 맞는 Type 의 불량은 상위 보고 안함으로 표시 (bUseReport = false)
			if (m_fnCompareValue(pDefectTrend[nDefectTypeNum], stFilterInfo[nDefectTypeNum].nRefVal, stFilterInfo[nDefectTypeNum].nSign))
			{// 상위 보고 불량 없음
				 
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


	nTotalDefectCount = UserDefinedFilter(resultPanelData,nTotalDefectCount); // 사용자 정의 필터. N개 이상의 디펙이 설정한 갯수 이상일때 무조건 보고.

	// 대표불량 선정	
	if (nTotalDefectCount != 0)		// 불량이 하나라도 있으면
	{
		// 대표불량 선정	%
		int nHighestDefectRank = 0;
		int nCurDefType = 0;
		int nMostDefectNum = -1;

		for (int nIndex = 0; nIndex < resultPanelData.m_ListDefectInfo.GetCount(); nIndex++)
		{			
			if (resultPanelData.m_ListDefectInfo[nIndex].bUseReport == true)
			{
				nCurDefType = resultPanelData.m_ListDefectInfo[nIndex].Defect_Type;

				if (nMostDefectNum == -1)	// 최초 불량
				{
					nMostDefectNum = nCurDefType;
					nHighestDefectRank = theApp.GetDefectRank(nCurDefType);
					continue;
				}
				// 현재 Defect Type 이 더 높은 순위를 가지면 현재 Defect Type을 선택
				if(nHighestDefectRank > theApp.GetDefectRank(nCurDefType))
				{
					nHighestDefectRank = theApp.GetDefectRank(nCurDefType);
					nMostDefectNum = nCurDefType;
				}
				// 동일한 순위이면 불량 개수가 더 많은 Type을 선택
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

		/// 불량군별 대표불량 선정 기능 /////////////////////////////////////
		// 엔지니어 요청으로 백점/누기/혼색 불량은 각각 하나만 보고하도록 설정
		// 필요 시 불량군별 대표불량 기능 설정하는 UI 작업 필요
		// 현재 하나의 불량( == 불량군)에 대해서만 대표불량 보고하는 구문만 필요.
		// 여러개의 불량을 하나의 불량군으로 인식하는 확장성을 위해 대표 불량 사용 여부를 TRUE/FALSE 로 구분하지 않고 Index 부여 (동일 Index 는 동일 불량군)
		// 최대 E_DEFECT_JUDGEMENT_COUNT 만큼 불량군 생성 가능. 불량군에서 대표불량 선정하면 중복 체크 하지 않기 위한 플래그
		bool bFindDefect[E_DEFECT_JUDGEMENT_COUNT] = {false, };

		for (nDefectTypeNum = 0; nDefectTypeNum < E_DEFECT_JUDGEMENT_COUNT; nDefectTypeNum++)
		{
			// 불량군별 대표불량 기능 사용하는 경우 (0이면 불량군별 대표불량 기능 사용 안함)
			// 여러개의 불량을 하나의 불량군으로 인식해야 할 경우 별도 작업 필요. - 작업 완료
			int nGroupIndex = theApp.GetDefectGroup(nDefectTypeNum);
			if (nGroupIndex != 0)		// Defect Type 이 대표 불량 보고하는 불량군으로 선정되어 있을 경우
			{				
				// 불량이 하나 이상일 때
				if (pDefectTrend[nDefectTypeNum] >= 1)
				{
					for (int nIndex = 0; nIndex < resultPanelData.m_ListDefectInfo.GetCount(); nIndex++)
					{
						// 최초 하나의 불량을 제외하고 보고 안함
						if (resultPanelData.m_ListDefectInfo[nIndex].Defect_Type == nDefectTypeNum)
						{
							if (!bFindDefect[nGroupIndex])
							{
								bFindDefect[nGroupIndex] = true;	// 현재 불량군에서 대표 불량 선정함
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
			//////////////////////////////////// 디펙 아이템(타입) 파일 로드
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

			// XML 파일 로드
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
	//memcpy(pDefectTrend, resultPanelData.m_nDefectTrend, sizeof(int) * E_DEFECT_JUDGEMENT_COUNT);		// 기존 Trend 파일은 그대로 두고 상위 보고 기준 Trend 배열 생성
	for (int nIndex = 0; nIndex < E_DEFECT_JUDGEMENT_COUNT; nIndex++)
		pDefectTrendCount[nIndex] = resultPanelData.m_nDefectTrend[nIndex];
	CString strGrade;

	resultPanelData.m_ResultPanel.Judge;

	//resultPanelData.m_ResultPanel.Judge; 랑 strGrade를 비교해서 현재 나온 GRADE에 대해
	//pDefectTrend로 디펙 아이템 개수를 받아온 다음
	//각 디펙아이템의 개수가 설정된 값 이상이면 보고 플래그 TRUE

	int nDefectCount = 0;
	std::vector<stUserDefinedFilter> vUserFilter = theApp.GetUserDefinedFilter();
	for (int i = 0 ; i < vUserFilter.size(); i++)
	{
		strGrade.Format(_T("%s"),vUserFilter[i].strGrade);
		if(strGrade == resultPanelData.m_ResultPanel.Judge)
		{
			for(int j = 0 ; j < vUserFilter[i].nFilterItemsCount ; j++)
				nDefectCount += pDefectTrendCount [ vUserFilter[i].nFilterItems[j] ]; // 사용자 정의 필터에 설정 되있는 Defect의 합

			if  (m_fnCompareValue(nDefectCount, vUserFilter[i].stFilterInfo.nRefVal, vUserFilter[i].stFilterInfo.nSign) ) // Defect의 합이 필터에 정의된 조건에 맞으면
			{
				for(int j = 0 ; j < vUserFilter[i].nFilterItemsCount ; j++)  // 
				{
					for (int nIndex = 0; nIndex < resultPanelData.m_ListDefectInfo.GetCount(); nIndex++)
					{
						if (resultPanelData.m_ListDefectInfo[nIndex].Defect_Type == vUserFilter[i].nFilterItems[j])
						{
							resultPanelData.m_ListDefectInfo[nIndex].bUseReport = true; // 디펙 보고 플래그 다시 살림.
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