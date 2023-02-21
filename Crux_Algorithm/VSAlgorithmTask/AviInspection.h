/*****************************************************************************
  File Name		: AviInspection.h
  Version		: ver 1.0
  Create Date	: 2017.03.21
  Author		: CWH
  Description	: Area 대응 검사 쓰레드
  Abbreviations	: 
 *****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmInterface.h"
#include "InspMainSequence.h"
#include "MatBufferResultManager.h"	// 메모리 관리
#include "MatResultBuf.h"			// 메모리 관리

//----------------------------------------------------------------------------------------------------
//	 Class 명	:	AviInspection
//	 Class기능	:	
//   주요기능	:
//	 작 성 일	:	2017/02
//	 작 성 자	:	CWH
//	 수정기록		:	V.1.0 초기 작성
//	 참    고	:	
//----------------------------------------------------------------------------------------------------
class AviInspection :
	public InspMainSequence
{
private:
	DECLARE_DYNCREATE(AviInspection)
public:
	AviInspection();
	virtual ~AviInspection(void);

	// 메모리 관리
// 	CMatResultBuf*	cMemResult[1];
// 
// 	void SetMem_Multi_Result(int nCnt, CMatResultBuf** data)
// 	{
// 		for (int i = 0; i < nCnt; i++)
// 		{
// 			cMemResult[i] = data[i];
// 		}
// 	};

// 메모리 관리
	CMatResultBuf*	cMemResult;
	void			SetMem_Result(CMatResultBuf* data)	{ cMemResult = data;	};
	CMatResultBuf*	GetMem_Result()						{ return	cMemResult; };


protected:


private:
	// 결과 이미지 저장
	bool	DrawDefectImage(CString strPanelID, 
							cv::Mat (*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], 
							ResultPanelData& resultPanelData);

	// AVI 검사 함수
	long	StartLogicAlgorithm(const CString strDrive, const CString strLcp_Bubble_Path, const tLogicPara& LogicPara, cv::Mat& MatColorBuffer,
								cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat& MatDrawBuffer,
								const int nImageNum, const int nROINumber, const int nAlgorithmNumber, 
								tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, 
								bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp);

	// Align 정보에 따른 Image 보정, 보정 후 Align 정보 갱신
	// AVI : Image 회전한 것으로 가정하고 좌표 보정
	bool	AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner);

	// 원본 이미지 보정
	// AVI : 보정 안함
	bool	AdjustOriginImage(cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, tAlignInfo* pStAlignInfo)		{	return true		;};

	// 외곽 부분 처리
	long	makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio);

	// AVI 판정 함수
	bool	Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
						const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, int nCamNum,
						ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStageNo);


	//////////////////////////////////////////////////////////////////////////
	//LCP 판정 함수
	bool JudgementMoistureDefect(ResultPanelData& resultPanelData, double* dAlignPara, double& dMoi_per, cv::Mat matMask_Active, cv::Rect rcAlignCellROI);

	bool JudgementVarianceDefect(ResultPanelData& resultPanelData, double* dAlignPara, double& dVari_per, cv::Mat matMask_Active, cv::Rect rcAlignCellROI);

	bool JudgementDeleteChips(ResultPanelData& resultPanelData, double* dAlignPara, cv::Mat matSrc_Draw, cv::Rect rcAlignCellROI , CString strPanelID, CString strDrive);

	bool DeleteOverlapDefect_LCP_Candidate_Substance(ResultPanelData& resultPanelData, double* dAlignPara);

	bool DeleteOverlapDefect_LCP_Moisture_Substance(ResultPanelData& resultPanelData, double* dAlignPara);

	bool Judgement_DefectColor_Trans(ResultPanelData& resultPanelData, double* dAlignPara);

	bool Report_FinalDefect(cv::Mat matColorImage[][MAX_CAMERA_COUNT],CString strReportPath, CString strPanelID_,double dResolution, int nCamnum, ResultPanelData& resultPanelData, double* dAlignPara);

	bool Judgement_Defect_Use_Result_Trans(ResultPanelData& resultPanelData, double* dAlignPara);
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////













	// Repair 설비에서 사용하는 좌표값 및 코드 판정
	bool	JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo &WrtResultInfo);

	// Point 여러개 있는 경우 -> Line 판정
	bool	JudgementPointToLine(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, const int nImageWidth, const int nImageHeight);		

	// Report 삭제
	bool	JudgementDelReport(ResultPanelData& resultPanelData);

	// Spot 분류 재확인
	bool	JudgementSpot(ResultPanelData& resultPanelData);

	// 강라인 주변 약라인 제거
	bool	JudgementDeletLineDefect(ResultPanelData& resultPanelData, double* dAlignPara);

	// Line Classification
	bool	JudgementClassifyLineDefect(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT]);

	// 6.39QHD Notch Y Line Delete
	bool	JudgementNotchDefect(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara);

	// Black Pattern 기준 Merge 판정
	bool	JudgementBlackPatternMerge(ResultPanelData& resultPanelData);

	// Crack 판정
	bool	JudgementCrack(ResultPanelData& resultPanelData);

	// DGS 판정
	bool	JudgementNewDGS(ResultPanelData& resultPanelData);

	// Vth Pattern DGS
	bool	JudgementDGS_Vth(ResultPanelData& resultPanelData);

	// Same Pattern Defect Merge
	bool	JudgementSamePatternDefectMerge(ResultPanelData& resultPanelData);

	// Weak Plan B Test
	bool	JudgementSpecialGrayLineDefect(ResultPanelData& resultPanelData);
	
	// PCD CRACK 판정 ( 비검사 영역 5.99" )
	bool	JudgementPCDCrack(ResultPanelData& resultPanelData, double* dAlignPara);

	// PNZ - Camera Tap에 인한 약 LINE 과검 제거
	bool	JudgementCameraTapOverKill(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara);

	// PNZ - GV Comparison
	bool	DefectLDRCompair(cv::Mat matSrcImage, cv::Rect rectTemp, double& Left_MeanValue, double& Defect_MeanValue, double& Right_MeanValue, double& Defect_MaxGV);

	// PNZ - Camera Tap 정보 받아 오기
	bool	CameraTapInfo(int CameraType, vector<int> &Direction, vector<int> &Position, vector<double> &GV, vector<double> &LR_Diff_GV);

	// PNZ - Modle Norch 등록 정보 받아 오기
	bool 	GetModelNorchInfo( ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], vector<int>& NorchIndex, CPoint& OrgIndex);

	// KSW - 중복 제거
	bool	DeleteOverlapDefect(ResultPanelData& resultPanelData, double* dAlignPara);

	// CKI - Defect size 에 따른 Judge
	bool    Judge_DefectSize(ResultPanelData& resultPanelData, double* dAlignPara);
	
	// PNZ - Dimple 제거 알고리즘
	bool	DeleteOverlapDefect_DimpleDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);

	// PNZ - TOP3 Max GV Calculator
	bool	NewMaxGVMethold(Mat matSrcImage, double OldMaxGV, double& NewMaxGV, int nTopCountGV);

	// PNZ - Dimple 제거 알고리즘
	bool	DeleteOverlapDefect_SpecInSpotDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);

	bool	DeleteOverlapDefect_BlackHole(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara = NULL);
	
	// PNZ - Dust 제거 알고리즘
	bool	DeleteOverlapDefect_DustDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);

	// PNZ - Black Pattern 강명점 주변 약명점 제거 알고리즘
	bool	DeleteOverlapDefect_BlackSmallDelet(ResultPanelData& resultPanelData,  double* dAlignPara=NULL);

	// KSW - Point - Point 중복 제거
	bool	DeleteOverlapDefect_Point_Point(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Point - Line 중복 제거
	bool	DeleteOverlapDefect_Point_Line(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Point - Mura 중복 제거
	bool	DeleteOverlapDefect_Point_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Line - Mura 중복 제거
	bool	DeleteOverlapDefect_Line_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Mura - Mura 중복 제거
	bool	DeleteOverlapDefect_Mura_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// CKI - RGB Bright Point - White Spot Mura 중복 제거  //B11 명점 19.08.20
	bool	DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point(ResultPanelData& resultPanelData, double* dAlignPara = NULL);

	// CKI - Black hole 중복 판정 //B11 명점 20.04.07
	bool    DeleteOverlapDefect_Black_Mura_and_Judge(ResultPanelData& resultPanelData, double* dAlignPara = NULL);

	// CKI - G64 패턴에서 명점과 백점무라를 표준편차를 이용해 재판정
//	int    Judge_Defect_BP_WSpot(ResultPanelData& resultPanelData, int index,int Defect_Judge ,double* dAlignPara = NULL);

	// KSW 17.09.11 - 혼색 많이 발생 -> 개수 많은 경우 ET 판정
	bool	JudgementET(ResultPanelData& resultPanelData, double* dAlignPara, CString strPanelID);

	// KSW 17.09.11 - 엣지 부분 명점 -> Pad Bright 판정
	bool	JudgementEdgePadBright(ResultPanelData& resultPanelData, double* dAlignPara);

	// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
	bool	JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt);

	// Mura 외곽선 정보 저장
	bool	JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt);

	// KSW 17.09.25 - 인접 & 군집 판정
	bool	JudgeGroup(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution);

	// KSW 17.11.29 - 인접 & 군집 판정
	bool	JudgeGroupTEST(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution);

	// PNZ 19.01.11 - Group 판정 최신 버전
	bool	JudgeGroupJudgment(ResultPanelData& resultPanelData, cv::Mat(*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution);

	// 19.04.04 - CHole AD 판정 및 2차 판정
	bool	JudgeCHoleJudgment(ResultPanelData& resultPanelData, tCHoleAlignInfo tCHoleAlignData, double* dAlignPara);

	// Casting - stDefectInfo에서 필요한 부분 추출하여 ResultPanelData에 적재
	bool	GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio);

	// Align 하기 전 AD 검사
	long	CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio);

	// ROI GV 검사
	long	CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult, 
						bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode);

	// KSW 17.11.24 - Resolution 계산
	double	calcResolution(CWriteResultInfo WrtResultInfo);

	// 17.12.18 동일 위치 불량 판정 시퀀스 추가	
	bool	JudgementRepeatCount(CString strPanelID, ResultPanelData& resultPanelData);

	bool	m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo);

	//////////////////////////////////////////////////////////////////////////choi goldencell 05.08
	bool JudgementMuraNormalClassification(ResultPanelData& resultPanelData, double* dAlignPara);
	bool JudgementMuraNormalT3Filter(ResultPanelData& resultPanelData, tCHoleAlignInfo tCHoleAlignData, double* dAlignPara);


	//void DefectColorCheck(cv::Mat MatColorBuffer, cv::Mat MatOriginImage_, stDefectInfo* pResultBlob);

	long JudgeSubStanceSpot(cv::Mat& matSrcBuffer, CRect rectROI, double* dPara, int* nCommonPara, 
		stDefectInfo* pResultBlob, stDefectInfo_Substance* pResultBlob_Sub);

	long JudgeSubStanceSpot_Transparency(cv::Mat& matSrcBuffer, CRect rectROI, double* dPara, int* nCommonPara,
		stDefectInfo* pResultBlob, stDefectInfo_Substance* pResultBlob_Sub);

	void BlobFeatureSave_Sub(stDefectInfo_Substance *resultBlob, CString strPath, int* nImageDefectCount);
	
	void BlobFeatureSave_LCP(stDefectInfo *resultBlob, CString strPath, CString strPanel_ID, double dResolution, int* nImageDefectCount);
	void BlobFeatureSave_LCP(ResultPanelData& resultPanelData, CString strPath, CString strPanel_ID, double dResolution, int* nImageDefectCount);

	void Mask_SizeUp(cv::Mat matSrc, int nDownSize);

// 	template <typename Object>
// 	void		KalMan_Filter(cv::Mat* stack, int nStack_size, double percentvar, double gain);
// 
// 	template <typename Object>
// 	Object		fromDouble(double[] array, int bitDepth);
// 
// 	template <typename Object>
// 	double*		toDouble(Object array, int bitDepth);

	void		KalMan_Filter(cv::Mat* stack, int nStack_size, double percentvar, double gain);

	byte*		fromDouble(double* darray, int nlenght);

	double*		toDouble(byte* darray, int nlenght);

	long		Image_Pow(int ImgType, double dpow, Mat& InPutImage, Mat& OutPutImage);
	
	void		Lipschitz_Fillter(cv::Mat& ip); //choi
};
