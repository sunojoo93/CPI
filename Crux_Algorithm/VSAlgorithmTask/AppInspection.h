
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmInterface.h"
#include "InspMainSequence.h"


#define MAX_MEASURE_RESULT_DATA 12	//외관 측정 결과 값 Data의 갯수 (Panel 각 4변의 시작점, 끝점, 평균)

//----------------------------------------------------------------------------------------------------
//	 Class 명	:	AppInspection
//	 Class기능	:	
//   주요기능	:
//	 작 성 일	:	
//	 작 성 자	:	
//	 수정기록	:	
//	 참    고	:	TEST(외관 Test를 위해 Avi code 사용)
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

	// 결과 이미지 저장
	bool	DrawDefectImage(CString strPanelID, 
							cv::Mat (*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], 
							ResultPanelData& resultPanelData);

	// Algorithm 검사 함수
	long	StartLogicAlgorithm(const CString strDrive, const CString strLcp_Bubble_Path, const tLogicPara& LogicPara, cv::Mat& MatColorBuffer,
								cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat& MatDrawBuffer,
								const int nImageNum, const int nROINumber, const int nAlgorithmNumber, 
								tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, 
								bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp);

	// Align 정보에 따른 Image 보정, 보정 후 Align 정보 갱신
	// AVI : Image 회전한 것으로 가정하고 좌표 보정
	bool	AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner);

	// 원본 이미지 보정
	// 보정 안함
	bool	AdjustOriginImage(cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, tAlignInfo* pStAlignInfo);

	// 외곽 부분 처리
	long	makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio);
	long	GetBMCorner(cv::Mat Src, double* dAlgPara, Point* ptPanelCorner, cv::Rect& rtBMCorner);

	// 판정 함수
	bool	Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
						const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, const int nCamNum,
						ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStateNo);

	// Repair 설비에서 사용하는 좌표값 및 코드 판정
	bool	JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo);

	bool	JudgementPointToLine(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, const int nImageWidth, const int nImageHeight);

	bool	JudgeGroup(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT]);	

	bool	DeleteOverlapDefect(ResultPanelData& resultPanelData);

	// Casting - stDefectInfo에서 필요한 부분 추출하여 ResultPanelData에 적재
	bool	GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio);

	// Align 하기 전 AD 검사
	long	CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio);
	// Pad Edge Cutting 이상 확인
	long	Check_Abnormal_PADEdge(cv::Mat MatOrgImage, int nCameraNum, int nBlurSize, int nThreshold, double dCompare_Theta, Rect rtObject);
	long	MethodOfLeastSquares(vector<cv::Point> pt, double &nA);
	// ROI GV 검사
	long	CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult, 
		bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode);

	bool	WriteMeasureResultFile(CString strFilePath, CString strFileName, CString strColumn, stMeasureInfo* pMeasureInfo);

	bool	WriteResultFile(CString strPanelID, CString strFilePath, CString strFileName, CString strColumn, TCHAR* strResult);

	// Mura 외곽선 정보 저장
	bool	JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt);

	// KSW 18.06.04 - 외곽 정보 ( AVI & SVI 머지 툴 )
	bool	JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt);

	//Blob 관련 함수
	void FindBiggestBlob(cv::Mat& src, cv::Mat& dst);
	void SelectBiggestBlob(cv::Mat& src, cv::Mat& dst, int nSelectInx);
	bool JudgementRepeatCount(CString strPanelID, ResultPanelData& resultPanelData, int nStageNo);

	//bool m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo);
	bool m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo);
};