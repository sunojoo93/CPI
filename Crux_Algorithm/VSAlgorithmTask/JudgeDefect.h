/*****************************************************************************
  File Name		: JudgeDefect.h
  Version		: ver 1.0
  Create Date	: 2017.03.21
  Author		: CWH
  Description	: �˻� ��� ���� �� ���� Class
  Abbreviations	: 
 *****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InspThrd.h"
#include "AlgorithmInterface.h"

//----------------------------------------------------------------------------------------------------
//	 Class ��	:	JudgeDefect
//	 Class���	:	�˻� ��� ���� �� ���� Class
//   �ֿ���		:
//	 �� �� ��		:	2017.03.21
//	 �� �� ��		:	CWH
//	 �������		:	V.1.0 �ʱ� �ۼ�
//	 ��    ��	:	
//----------------------------------------------------------------------------------------------------
class JudgeDefect :
	public CInspThrd, protected InspectAlgorithmInterface
{
public:
	JudgeDefect();
	~JudgeDefect(void);

protected:
	// ��˻� ���� �ҷ� ����
	bool			m_fnDefectFiltering(cv::Mat& MatDrawBuffer, int nImageNum, int nCameraNum, stDefectInfo* pResultBlob, tAlignInfo stThrdAlignInfo, int nRatio);

	// �ٰ��� �ȿ� ���� �ִ��� ��/��
	bool			PtInPolygon(cv::Point* ptPolygon, cv::Point ptPoint, int nPolygonCnt = 4);
		
	// �ҷ� ���� ����
	void			ConsolidateResult(const CString strPanelID, const CString strDrive,
										CWriteResultInfo WrtResultInfo, ResultBlob_Total* pResultBlob_Total, ResultPanelData& resultPanelData, 
										const int nImageNum, const int nCameraNum, int nRatio, ENUM_INSPECT_MODE eInspMode);

	void			RotateRect(CRect rcSrc, cv::Point* pPtDst, tAlignInfo stThrdAlignInfo, bool bRotate=true);

	void			NumberingDefect(const CString strModelID, const CString strPanelID, const CString strLotID,CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, int nRatio);

	// AVI AD �ҷ� �߰� �Լ�
	bool			JudgeADDefect(int nImageNum, int nCameraNum, int nStageNo, int nImageWidth, int nImageHeight, ResultBlob_Total* pResultBlob_Total, int nDefectAD, ENUM_INSPECT_MODE eInspMode, bool bAD=true);
	
	bool			m_fnCompareValue(int nDefectCount, int nRefCount, int nSign);

	bool			JudgementPanelGrade(ResultPanelData& resultPanelData);

	bool			ApplyReportRule(ResultPanelData& resultPanelData);

	int			UserDefinedFilter(ResultPanelData& resultPanelData, int nTotalDefectCount); //����� ���� ����. N�� �̻��� ������ ������ ���� �̻��϶� ������ ����.

	///////////////////////////////////// �����Լ�
	// ���� �Լ�
	virtual bool	Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
							const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, const int nCamNum,
							ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStageNo) = 0;

	// Repair ���񿡼� ����ϴ� ��ǥ�� �� �ڵ� ����
	virtual bool	JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo) = 0;	
	virtual bool	JudgementPointToLine(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, const int nImageWidth, const int nImageHeight)		{	return	false	;};
	virtual	bool	JudgeGroup(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT])							{	return	false	;};
	// ���͸� �Լ�
	virtual bool	DeleteOverlapDefect(ResultPanelData& resultPanelData)														{	return	false	;};	
	// Casting stDefectInfo���� �ʿ��� �κ� �����Ͽ� ResultPanelData�� ����
	virtual bool	GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, 
									int nBlobCnt, int nImageNum, int nCameraNum, int nRatio)									{	return	false	;};	

	// Stage �� AD �ҷ� ī��Ʈ ��� �߰�
	bool JudgeDefect::m_fnCountingStageAD(int nImageNum, int nStageNo, int nDefectType);
};