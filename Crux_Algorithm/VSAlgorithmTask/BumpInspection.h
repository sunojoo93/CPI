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
#include "Define.h"
#define CPU 0
#define GPU 1

enum E_BUMP_PARAMETER {
	E_BUMP_FIRST_TEXT,
	E_BUMP_SCANDIE,
	E_BUMP_SCAN_AREA_BRIGHT,
	E_BUMP_CHECK_AREA_RATIO,

	E_BUMP_ALIGNMARK_TEXT,
	E_BUMP_ALIGNMARK_SEARCHPITCH,

	E_BUMP_INSPECTION_TEXT,
	E_BUMP_DIFFSEARCH_X,
	E_BUMP_DIFFSEARCH_Y,
	E_BUMP_INSPECT_SCAN_DIE
};

enum ThreadWorkingType {
	E_THREAD_STOP,
	E_THREAD_RUNNING,
	E_THREAD_PAUSE
};

enum ThreadMemoryMap {
	E_MEMORY_EMPTY,
	E_MEMORY_FULL,
	E_MEMORY_COMPLTE
};

enum ThreadSearchMap {
	E_SEARCH_WAIT,
	E_SEARCH_CAN,
	E_SEARCH_SCANING,
	E_SEARCH_EMPTY,
	E_SEARCH_EXIST,
	E_SEARCH_COMPLTE
};



//----------------------------------------------------------------------------------------------------
//	 Class 명	:	AviInspection
//	 Class기능	:	
//   주요기능	:
//	 작 성 일	:	2017/02
//	 작 성 자	:	CWH
//	 수정기록		:	V.1.0 초기 작성
//	 참    고	:	
//----------------------------------------------------------------------------------------------------
class BumpInspection :
	public InspMainSequence
{
private:
	DECLARE_DYNCREATE(BumpInspection)
public:
	BumpInspection();
	virtual ~BumpInspection(void);

	CWinThread* m_pThreadScanMemory;
	CWinThread* m_pThreadScanArea;
	CWinThread* m_pThreadScanAlignMark;
	CWinThread* m_pThreadScanDie;
	CWinThread* m_pThreadScanInspection;

	ThreadWorkingType m_eThreadWorkMemory;
	ThreadWorkingType m_eThreadWorkArea;
	ThreadWorkingType m_eThreadWorkAlignMark;
	ThreadWorkingType m_eThreadWorkDie;
	ThreadWorkingType m_eThreadWorkInspection;

	
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

	//CMatBufferResultManager	cMemResultBuff;
protected:


private:
	// 결과 이미지 저장
	bool	DrawDefectImage(CString strPanelID, 
							cv::Mat (*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], 
							ResultPanelData& resultPanelData);

	// AVI 검사 함수
	long	StartLogicAlgorithm(const CString strDrive, const tLogicPara& LogicPara, 
								cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat& MatDrawBuffer,
								const int nImageNum, const int nROINumber, const int nAlgorithmNumber, 
								tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, 
								bool bpInspectEnd[][MAX_CAMERA_COUNT], int nRatio, bool *bPattern, bool *nCount, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp);

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
						const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio,
						ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatOrgImage_Real[][MAX_CAMERA_COUNT], bool bUseInspect, int nStageNo);

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

	// 20200629 LJH 검출된 라인 다발 불량명 변경
	bool	 JudgeMultLine(ResultPanelData& resultPanelData);
	//20200709 LJH 포인트 명점 분류 
	bool	 PatternPointBrightClassify(ResultPanelData& resultPanelData);

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
	bool 	GetModelNorchInfo( ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], vector<int>& NorchIndex, CPoint& OrgIndex, int nNorchPosition);

	// KSW - 중복 제거
	bool	DeleteOverlapDefect(ResultPanelData& resultPanelData, double* dAlignPara);

	// PNZ - Dimple 제거 알고리즘
	bool	DeleteOverlapDefect_DimpleDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);

	// PNZ - TOP3 Max GV Calculator
	bool	NewMaxGVMethold(Mat matSrcImage, double OldMaxGV, double& NewMaxGV, int nTopCountGV);

	// PNZ - Dimple 제거 알고리즘
	bool	DeleteOverlapDefect_SpecInSpotDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);
	
	// PNZ - Dust 제거 알고리즘
	bool	DeleteOverlapDefect_DustDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);

	// PNZ - Black Pattern 강명점 주변 약명점 제거 알고리즘
	bool	DeleteOverlapDefect_BlackSmallDelet(ResultPanelData& resultPanelData,  double* dAlignPara=NULL);

	// Black Pattern 명점 RGB Pattern 위치 GV 값 비교
	bool	DeleteOverlapDefect_Point_Black_RGB(ResultPanelData& resultPanelData, cv::Mat MatOrgImage_Real[][MAX_CAMERA_COUNT], double* dAlignPara = NULL);

	// PNZ - Mura Normal 불량 분류
	bool	JudgementMuraNormalClassification(ResultPanelData& resultPanelData, double* dAlignPara = NULL);


	// Contact 이상시 생기는 G64 알람 발생
	bool	JudgementContactAlram(ResultPanelData& resultPanelData, double* dAlignPara = NULL);

	//////////////////////////////////////////////////////////////////////////
	// KSW - Point - Point 중복 제거
	bool	DeleteOverlapDefect_Point_Point(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// LJH 20201127 고객사 요구 Point - Point 동일 패턴 중복 제거 (Point - Point 안쓸경우에만 쓴다고 함)
	bool	DeleteOverlapDefect_Point_Point_RGB(ResultPanelData& resultPanelData, double* dAlignPara = NULL);

	// KSW - Point - Line 중복 제거
	bool	DeleteOverlapDefect_Point_Line(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Point - Mura 중복 제거
	bool	DeleteOverlapDefect_Point_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Line - Mura 중복 제거
	bool	DeleteOverlapDefect_Line_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Mura - Mura 중복 제거
	bool	DeleteOverlapDefect_Mura_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

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

	bool	JudgeGroupJudgmentEach(ResultPanelData& resultPanelData, cv::Mat(*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution);
	// 20201012 LJH RGB 패턴 Sp3 각 패턴별로 그룹 판정
	bool	JudgeRGBSp3Judgment(ResultPanelData& resultPanelData, double* dAlignPara);
	// 20201125 LJH _BRIGHT_RGB 스크래치 과검 제거 
	bool	JudgeBrightRGBJudgment(ResultPanelData& resultPanelData, double* dAlignPara);
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


	void StartThreadScanMemory(stInspectThreadParameter * stParameter);

	void PauseThreadScanMemory();

	void StopThreadScanMemory();

	static UINT threadScanMemory(LPVOID _method);

	void StartThreadScanArea(stInspectThreadParameter * stParameter);

	void PauseThreadScanArea();

	void StopThreadScanArea();

	static UINT threadScanArea(LPVOID _method);

	int FindWarferArea(cv::Mat & matSrc, int nFristPositionThreshold, float fInspectionCheckArea);
	
	void StartThreadScanAlignMark(stInspectThreadParameter * stParameter);
	void PauseThreadScanAlignMark();
	void StopThreadScanAlignMark();
	static UINT threadScanAlignMark(LPVOID _method);
	int SearchAlignMark2(stInspectThreadParameter * stParam, cv::Mat & matSrc, vector<cv::Mat>& vecmatAlignMark, int nStartYPos, int nCutLength, bool bType);

	int TemplateResultPositionCheck(stInspectThreadParameter * stParam, int nTotalLabel, cv::Mat & matSrc, cv::Mat & matTemplateResult, cv::Mat & matLabel, cv::Mat & matStats, int nAlignMarkWidth, int nAlignMarkHeight, vector<cv::Mat>& vecmatFoundMark, vector<cv::Rect>& vecRectMark, int nStartYPos);

	int SetMapStartPosition(stInspectThreadParameter * stParam);

	int CheckMarkstate(stInspectThreadParameter * stParam);

	int MapAlignMarkMatching3(stInspectThreadParameter * stParam);

	int NextAlingMarkPosition(stInspectThreadParameter * stParam);

	void StartThreadScanDie(stInspectThreadParameter * stParameter);

	void PauseThreadScanDie();

	void StopThreadScanDie();

	static UINT threadScanDie(LPVOID _method);

	void StartThreadInspectionBump(stInspectThreadParameter * stParameter);

	void PauseThreadInspectionBump();

	void StopThreadInspectionBump();

	static UINT threadInspectionBump(LPVOID _method);

	int DieTrimmedMean(vector<cv::Mat> vecmatSample, cv::Mat & matResult);

	int Inspection(stInspectThreadParameter * stParam, cv::Mat Die1, cv::Mat Die2, cv::Mat & Result);

	structDieArr ** CreateWaferDieMap(int nType, float fDieWidth_mm, float fDieHeight_mm, float fWaferDiameter_inch1, float fWaferDiameter_mm, float fEdgeLoss_mm, int nLossShift, float fOffsetHeight_mm, float fOffsetWidth_mm, int nOffset);
	structDieArr ** CreateWaferDieMap(CString strPath, float fDieWidth_mm, float fDieHeight_mm);

	int IsRectInCircle(float CircleCenterX, float CircleCenterY, float CircleRadius, cv::Rect IsRect, int nOffSet = 0);

	void setMapData(stInspectThreadParameter * stParam);
	
};
