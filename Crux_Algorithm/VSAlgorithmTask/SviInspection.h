/*****************************************************************************
  File Name		: AviInspection.h
  Version		: ver 1.0
  Create Date	: 2017.03.21
  Author		: CWH
  Description	: Area ���� �˻� ������
  Abbreviations	: 
 *****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmInterface.h"
#include "InspMainSequence.h"

//----------------------------------------------------------------------------------------------------
//	 Class ��	:	SviInspection
//	 Class���	:	
//   �ֿ���	:
//	 �� �� ��	:	2017/02
//	 �� �� ��	:	CWH
//	 �������	:	V.1.0 �ʱ� �ۼ�
//	 ��    ��	:	
//----------------------------------------------------------------------------------------------------
class SviInspection :
	public InspMainSequence
{
private:
	DECLARE_DYNCREATE(SviInspection)
public:
	SviInspection();
	virtual ~SviInspection(void);

protected:


private:

	// ��� �̹��� ����
	bool	DrawDefectImage(CString strPanelID, 
							cv::Mat (*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], 
							ResultPanelData& resultPanelData);

	// AVI �˻� �Լ�
	long	StartLogicAlgorithm(const CString strDrive, const CString strLcp_Bubble_Path, const tLogicPara& LogicPara, cv::Mat& MatColorBuffer,
								cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat& MatDrawBuffer,
								const int nImageNum, const int nROINumber, const int nAlgorithmNumber, 
								tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, 
								bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp);

	// Align ������ ���� Image ����, ���� �� Align ���� ����
	// SVI : Warping �� ��ǥ ���
	bool	AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner);

	// ���� �̹��� ����
	// SVI : Warping
	bool	AdjustOriginImage(cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, tAlignInfo* pStAlignInfo);

	// �ܰ� �κ� ó��
	long	makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio);

	// SVI ���� �Լ�
	bool	Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
		const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, const int nCamNum,
						ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStage);

	// Repair ���񿡼� ����ϴ� ��ǥ�� �� �ڵ� ����
	bool	JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo);

	// Casting - stDefectInfo���� �ʿ��� �κ� �����Ͽ� ResultPanelData�� ����	
	bool	GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio);

	// Align �ϱ� �� AD �˻�
	long	CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio);
	
	// ROI GV �˻�
	long	CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult, 
						bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode);

	// �ߺ� ��ǥ ����
	bool	DeleteOverlapDefect(ResultPanelData& resultPanelData);

	// �ܰ� ���� ( ���� �� )
	bool	JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt);

	// Mura �ܰ��� ���� ����
	bool	JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt);
};
