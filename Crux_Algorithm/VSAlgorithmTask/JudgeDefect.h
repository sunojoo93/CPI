/*****************************************************************************
  File Name		: JudgeDefect.h
  Version		: ver 1.0
  Create Date	: 2017.03.21
  Author		: CWH
  Description	: 검사 결과 취합 및 판정 Class
  Abbreviations	: 
 *****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InspThrd.h"
#include "AlgorithmInterface.h"

//----------------------------------------------------------------------------------------------------
//	 Class 명	:	JudgeDefect
//	 Class기능	:	검사 결과 취합 및 판정 Class
//   주요기능		:
//	 작 성 일		:	2017.03.21
//	 작 성 자		:	CWH
//	 수정기록		:	V.1.0 초기 작성
//	 참    고	:	
//----------------------------------------------------------------------------------------------------
class JudgeDefect :
	public CInspThrd, protected InspectAlgorithmInterface
{
public:
	JudgeDefect();
	~JudgeDefect(void);

protected:
	// 비검사 영역 불량 제거
	bool			m_fnDefectFiltering(cv::Mat& MatDrawBuffer, int nImageNum, int nCameraNum, stDefectInfo* pResultBlob, tAlignInfo stThrdAlignInfo, int nRatio);

	// 다각형 안에 점이 있는지 유/무
	bool			PtInPolygon(cv::Point* ptPolygon, cv::Point ptPoint, int nPolygonCnt = 4);
		
	// 불량 정보 취합
	void			ConsolidateResult(const CString strPanelID, const CString strDrive,
										CWriteResultInfo WrtResultInfo, ResultBlob_Total* pResultBlob_Total, ResultPanelData& resultPanelData, 
										const int nImageNum, const int nCameraNum, int nRatio, ENUM_INSPECT_MODE eInspMode);

	void			RotateRect(CRect rcSrc, cv::Point* pPtDst, tAlignInfo stThrdAlignInfo, bool bRotate=true);

	void			NumberingDefect(const CString strModelID, const CString strPanelID, const CString strLotID,CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, int nRatio);

	// AVI AD 불량 추가 함수
	bool			JudgeADDefect(int nImageNum, int nCameraNum, int nStageNo, int nImageWidth, int nImageHeight, ResultBlob_Total* pResultBlob_Total, int nDefectAD, ENUM_INSPECT_MODE eInspMode, bool bAD=true);
	
	bool			m_fnCompareValue(int nDefectCount, int nRefCount, int nSign);

	bool			JudgementPanelGrade(ResultPanelData& resultPanelData);

	bool			ApplyReportRule(ResultPanelData& resultPanelData);

	int			UserDefinedFilter(ResultPanelData& resultPanelData, int nTotalDefectCount); //사용자 정의 필터. N개 이상의 디펙이 설정한 갯수 이상일때 무조건 보고.

	///////////////////////////////////// 가상함수
	// 판정 함수
	virtual bool	Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
							const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, const int nCamNum,
							ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStageNo) = 0;

	// Repair 설비에서 사용하는 좌표값 및 코드 판정
	virtual bool	JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo) = 0;	
	virtual bool	JudgementPointToLine(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, const int nImageWidth, const int nImageHeight)		{	return	false	;};
	virtual	bool	JudgeGroup(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT])							{	return	false	;};
	// 필터링 함수
	virtual bool	DeleteOverlapDefect(ResultPanelData& resultPanelData)														{	return	false	;};	
	// Casting stDefectInfo에서 필요한 부분 추출하여 ResultPanelData에 적재
	virtual bool	GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, 
									int nBlobCnt, int nImageNum, int nCameraNum, int nRatio)									{	return	false	;};	

	// Stage 별 AD 불량 카운트 기능 추가
	bool JudgeDefect::m_fnCountingStageAD(int nImageNum, int nStageNo, int nDefectType);
};