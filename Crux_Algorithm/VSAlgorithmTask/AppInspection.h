
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmInterface.h"
#include "InspMainSequence.h"


#define MAX_MEASURE_RESULT_DATA 12	//�ܰ� ���� ��� �� Data�� ���� (Panel �� 4���� ������, ����, ���)

//----------------------------------------------------------------------------------------------------
//	 Class ��	:	AppInspection
//	 Class���	:	
//   �ֿ���	:
//	 �� �� ��	:	
//	 �� �� ��	:	
//	 �������	:	
//	 ��    ��	:	TEST(�ܰ� Test�� ���� Avi code ���)
//----------------------------------------------------------------------------------------------------
#pragma once
class AppInspection:
	public InspMainSequence
{
private:
	DECLARE_DYNCREATE(AppInspection)

public:
	AppInspection(void);
	~AppInspection(void);

	private:

	// ��� �̹��� ����
	bool	DrawDefectImage(CString strPanelID, 
							cv::Mat (*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], 
							ResultPanelData& resultPanelData);

	// Algorithm �˻� �Լ�
	long	StartLogicAlgorithm(const CString strDrive, const CString strLcp_Bubble_Path, const tLogicPara& LogicPara, cv::Mat& MatColorBuffer,
								cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat& MatDrawBuffer,
								const int nImageNum, const int nROINumber, const int nAlgorithmNumber, 
								tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, 
								bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp);

	// Align ������ ���� Image ����, ���� �� Align ���� ����
	// AVI : Image ȸ���� ������ �����ϰ� ��ǥ ����
	bool	AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner);

	// ���� �̹��� ����
	// ���� ����
	bool	AdjustOriginImage(cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, tAlignInfo* pStAlignInfo);

	// �ܰ� �κ� ó��
	long	makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio);
	long	GetBMCorner(cv::Mat Src, double* dAlgPara, Point* ptPanelCorner, cv::Rect& rtBMCorner);

	// ���� �Լ�
	bool	Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
						const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, const int nCamNum,
						ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStateNo);

	// Repair ���񿡼� ����ϴ� ��ǥ�� �� �ڵ� ����
	bool	JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo);

	bool	JudgementPointToLine(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, const int nImageWidth, const int nImageHeight);

	bool	JudgeGroup(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT]);	

	bool	DeleteOverlapDefect(ResultPanelData& resultPanelData);

	// Casting - stDefectInfo���� �ʿ��� �κ� �����Ͽ� ResultPanelData�� ����
	bool	GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio);

	// Align �ϱ� �� AD �˻�
	long	CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio);
	// Pad Edge Cutting �̻� Ȯ��
	long	Check_Abnormal_PADEdge(cv::Mat MatOrgImage, int nCameraNum, int nBlurSize, int nThreshold, double dCompare_Theta, Rect rtObject);
	long	MethodOfLeastSquares(vector<cv::Point> pt, double &nA);
	// ROI GV �˻�
	long	CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult, 
		bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode);

	bool	WriteMeasureResultFile(CString strFilePath, CString strFileName, CString strColumn, stMeasureInfo* pMeasureInfo);

	bool	WriteResultFile(CString strPanelID, CString strFilePath, CString strFileName, CString strColumn, TCHAR* strResult);

	// Mura �ܰ��� ���� ����
	bool	JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt);

	// KSW 18.06.04 - �ܰ� ���� ( AVI & SVI ���� �� )
	bool	JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt);

	//Blob ���� �Լ�
	void FindBiggestBlob(cv::Mat& src, cv::Mat& dst);
	void SelectBiggestBlob(cv::Mat& src, cv::Mat& dst, int nSelectInx);
	bool JudgementRepeatCount(CString strPanelID, ResultPanelData& resultPanelData, int nStageNo);

	//bool m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo);
	bool m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo);
};