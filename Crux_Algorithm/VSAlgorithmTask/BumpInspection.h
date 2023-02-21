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
#include "MatBufferResultManager.h"	// �޸� ����
#include "MatResultBuf.h"			// �޸� ����
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
//	 Class ��	:	AviInspection
//	 Class���	:	
//   �ֿ���	:
//	 �� �� ��	:	2017/02
//	 �� �� ��	:	CWH
//	 �������		:	V.1.0 �ʱ� �ۼ�
//	 ��    ��	:	
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

	
	// �޸� ����
// 	CMatResultBuf*	cMemResult[1];
// 
// 	void SetMem_Multi_Result(int nCnt, CMatResultBuf** data)
// 	{
// 		for (int i = 0; i < nCnt; i++)
// 		{
// 			cMemResult[i] = data[i];
// 		}
// 	};

// �޸� ����
	CMatResultBuf*	cMemResult;
	void			SetMem_Result(CMatResultBuf* data)	{ cMemResult = data;	};
	CMatResultBuf*	GetMem_Result()						{ return	cMemResult; };

	//CMatBufferResultManager	cMemResultBuff;
protected:


private:
	// ��� �̹��� ����
	bool	DrawDefectImage(CString strPanelID, 
							cv::Mat (*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], 
							ResultPanelData& resultPanelData);

	// AVI �˻� �Լ�
	long	StartLogicAlgorithm(const CString strDrive, const tLogicPara& LogicPara, 
								cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat& MatDrawBuffer,
								const int nImageNum, const int nROINumber, const int nAlgorithmNumber, 
								tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, 
								bool bpInspectEnd[][MAX_CAMERA_COUNT], int nRatio, bool *bPattern, bool *nCount, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp);

	// Align ������ ���� Image ����, ���� �� Align ���� ����
	// AVI : Image ȸ���� ������ �����ϰ� ��ǥ ����
	bool	AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner);

	// ���� �̹��� ����
	// AVI : ���� ����
	bool	AdjustOriginImage(cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, tAlignInfo* pStAlignInfo)		{	return true		;};

	// �ܰ� �κ� ó��
	long	makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio);

	// AVI ���� �Լ�
	bool	Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
						const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio,
						ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatOrgImage_Real[][MAX_CAMERA_COUNT], bool bUseInspect, int nStageNo);

	// Repair ���񿡼� ����ϴ� ��ǥ�� �� �ڵ� ����
	bool	JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo &WrtResultInfo);

	// Point ������ �ִ� ��� -> Line ����
	bool	JudgementPointToLine(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, const int nImageWidth, const int nImageHeight);		

	// Report ����
	bool	JudgementDelReport(ResultPanelData& resultPanelData);

	// Spot �з� ��Ȯ��
	bool	JudgementSpot(ResultPanelData& resultPanelData);

	// ������ �ֺ� ����� ����
	bool	JudgementDeletLineDefect(ResultPanelData& resultPanelData, double* dAlignPara);

	// Line Classification
	bool	JudgementClassifyLineDefect(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT]);

	// 6.39QHD Notch Y Line Delete
	bool	JudgementNotchDefect(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara);

	// Black Pattern ���� Merge ����
	bool	JudgementBlackPatternMerge(ResultPanelData& resultPanelData);

	// Crack ����
	bool	JudgementCrack(ResultPanelData& resultPanelData);

	// DGS ����
	bool	JudgementNewDGS(ResultPanelData& resultPanelData);

	// Vth Pattern DGS
	bool	JudgementDGS_Vth(ResultPanelData& resultPanelData);

	// Same Pattern Defect Merge
	bool	JudgementSamePatternDefectMerge(ResultPanelData& resultPanelData);

	// 20200629 LJH ����� ���� �ٹ� �ҷ��� ����
	bool	 JudgeMultLine(ResultPanelData& resultPanelData);
	//20200709 LJH ����Ʈ ���� �з� 
	bool	 PatternPointBrightClassify(ResultPanelData& resultPanelData);

	// Weak Plan B Test
	bool	JudgementSpecialGrayLineDefect(ResultPanelData& resultPanelData);
	
	// PCD CRACK ���� ( ��˻� ���� 5.99" )
	bool	JudgementPCDCrack(ResultPanelData& resultPanelData, double* dAlignPara);

	// PNZ - Camera Tap�� ���� �� LINE ���� ����
	bool	JudgementCameraTapOverKill(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara);

	// PNZ - GV Comparison
	bool	DefectLDRCompair(cv::Mat matSrcImage, cv::Rect rectTemp, double& Left_MeanValue, double& Defect_MeanValue, double& Right_MeanValue, double& Defect_MaxGV);

	// PNZ - Camera Tap ���� �޾� ����
	bool	CameraTapInfo(int CameraType, vector<int> &Direction, vector<int> &Position, vector<double> &GV, vector<double> &LR_Diff_GV);

	// PNZ - Modle Norch ��� ���� �޾� ����
	bool 	GetModelNorchInfo( ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], vector<int>& NorchIndex, CPoint& OrgIndex, int nNorchPosition);

	// KSW - �ߺ� ����
	bool	DeleteOverlapDefect(ResultPanelData& resultPanelData, double* dAlignPara);

	// PNZ - Dimple ���� �˰���
	bool	DeleteOverlapDefect_DimpleDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);

	// PNZ - TOP3 Max GV Calculator
	bool	NewMaxGVMethold(Mat matSrcImage, double OldMaxGV, double& NewMaxGV, int nTopCountGV);

	// PNZ - Dimple ���� �˰���
	bool	DeleteOverlapDefect_SpecInSpotDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);
	
	// PNZ - Dust ���� �˰���
	bool	DeleteOverlapDefect_DustDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);

	// PNZ - Black Pattern ������ �ֺ� ����� ���� �˰���
	bool	DeleteOverlapDefect_BlackSmallDelet(ResultPanelData& resultPanelData,  double* dAlignPara=NULL);

	// Black Pattern ���� RGB Pattern ��ġ GV �� ��
	bool	DeleteOverlapDefect_Point_Black_RGB(ResultPanelData& resultPanelData, cv::Mat MatOrgImage_Real[][MAX_CAMERA_COUNT], double* dAlignPara = NULL);

	// PNZ - Mura Normal �ҷ� �з�
	bool	JudgementMuraNormalClassification(ResultPanelData& resultPanelData, double* dAlignPara = NULL);


	// Contact �̻�� ����� G64 �˶� �߻�
	bool	JudgementContactAlram(ResultPanelData& resultPanelData, double* dAlignPara = NULL);

	//////////////////////////////////////////////////////////////////////////
	// KSW - Point - Point �ߺ� ����
	bool	DeleteOverlapDefect_Point_Point(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// LJH 20201127 ���� �䱸 Point - Point ���� ���� �ߺ� ���� (Point - Point �Ⱦ���쿡�� ���ٰ� ��)
	bool	DeleteOverlapDefect_Point_Point_RGB(ResultPanelData& resultPanelData, double* dAlignPara = NULL);

	// KSW - Point - Line �ߺ� ����
	bool	DeleteOverlapDefect_Point_Line(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Point - Mura �ߺ� ����
	bool	DeleteOverlapDefect_Point_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Line - Mura �ߺ� ����
	bool	DeleteOverlapDefect_Line_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Mura - Mura �ߺ� ����
	bool	DeleteOverlapDefect_Mura_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW 17.09.11 - ȥ�� ���� �߻� -> ���� ���� ��� ET ����
	bool	JudgementET(ResultPanelData& resultPanelData, double* dAlignPara, CString strPanelID);

	// KSW 17.09.11 - ���� �κ� ���� -> Pad Bright ����
	bool	JudgementEdgePadBright(ResultPanelData& resultPanelData, double* dAlignPara);

	// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
	bool	JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt);

	// Mura �ܰ��� ���� ����
	bool	JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt);

	// KSW 17.09.25 - ���� & ���� ����
	bool	JudgeGroup(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution);

	// KSW 17.11.29 - ���� & ���� ����
	bool	JudgeGroupTEST(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution);

	// PNZ 19.01.11 - Group ���� �ֽ� ����
	bool	JudgeGroupJudgment(ResultPanelData& resultPanelData, cv::Mat(*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution);

	bool	JudgeGroupJudgmentEach(ResultPanelData& resultPanelData, cv::Mat(*MatDraw)[MAX_CAMERA_COUNT], double* dAlignPara, double dResolution);
	// 20201012 LJH RGB ���� Sp3 �� ���Ϻ��� �׷� ����
	bool	JudgeRGBSp3Judgment(ResultPanelData& resultPanelData, double* dAlignPara);
	// 20201125 LJH _BRIGHT_RGB ��ũ��ġ ���� ���� 
	bool	JudgeBrightRGBJudgment(ResultPanelData& resultPanelData, double* dAlignPara);
	// 19.04.04 - CHole AD ���� �� 2�� ����
	bool	JudgeCHoleJudgment(ResultPanelData& resultPanelData, tCHoleAlignInfo tCHoleAlignData, double* dAlignPara);



	// Casting - stDefectInfo���� �ʿ��� �κ� �����Ͽ� ResultPanelData�� ����
	bool	GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio);

	// Align �ϱ� �� AD �˻�
	long	CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio);

	// ROI GV �˻�
	long	CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult, 
						bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode);

	// KSW 17.11.24 - Resolution ���
	double	calcResolution(CWriteResultInfo WrtResultInfo);

	// 17.12.18 ���� ��ġ �ҷ� ���� ������ �߰�	
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
