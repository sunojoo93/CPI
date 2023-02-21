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

//----------------------------------------------------------------------------------------------------
//	 Class ��	:	AviInspection
//	 Class���	:	
//   �ֿ���	:
//	 �� �� ��	:	2017/02
//	 �� �� ��	:	CWH
//	 �������		:	V.1.0 �ʱ� �ۼ�
//	 ��    ��	:	
//----------------------------------------------------------------------------------------------------
class AviInspection :
	public InspMainSequence
{
private:
	DECLARE_DYNCREATE(AviInspection)
public:
	AviInspection();
	virtual ~AviInspection(void);

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
	// AVI : Image ȸ���� ������ �����ϰ� ��ǥ ����
	bool	AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner);

	// ���� �̹��� ����
	// AVI : ���� ����
	bool	AdjustOriginImage(cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, tAlignInfo* pStAlignInfo)		{	return true		;};

	// �ܰ� �κ� ó��
	long	makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio);

	// AVI ���� �Լ�
	bool	Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
						const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, int nCamNum,
						ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStageNo);


	//////////////////////////////////////////////////////////////////////////
	//LCP ���� �Լ�
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
	bool 	GetModelNorchInfo( ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], vector<int>& NorchIndex, CPoint& OrgIndex);

	// KSW - �ߺ� ����
	bool	DeleteOverlapDefect(ResultPanelData& resultPanelData, double* dAlignPara);

	// CKI - Defect size �� ���� Judge
	bool    Judge_DefectSize(ResultPanelData& resultPanelData, double* dAlignPara);
	
	// PNZ - Dimple ���� �˰���
	bool	DeleteOverlapDefect_DimpleDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);

	// PNZ - TOP3 Max GV Calculator
	bool	NewMaxGVMethold(Mat matSrcImage, double OldMaxGV, double& NewMaxGV, int nTopCountGV);

	// PNZ - Dimple ���� �˰���
	bool	DeleteOverlapDefect_SpecInSpotDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);

	bool	DeleteOverlapDefect_BlackHole(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara = NULL);
	
	// PNZ - Dust ���� �˰���
	bool	DeleteOverlapDefect_DustDelet(ResultPanelData& resultPanelData, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], double* dAlignPara=NULL);

	// PNZ - Black Pattern ������ �ֺ� ����� ���� �˰���
	bool	DeleteOverlapDefect_BlackSmallDelet(ResultPanelData& resultPanelData,  double* dAlignPara=NULL);

	// KSW - Point - Point �ߺ� ����
	bool	DeleteOverlapDefect_Point_Point(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Point - Line �ߺ� ����
	bool	DeleteOverlapDefect_Point_Line(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Point - Mura �ߺ� ����
	bool	DeleteOverlapDefect_Point_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Line - Mura �ߺ� ����
	bool	DeleteOverlapDefect_Line_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// KSW - Mura - Mura �ߺ� ����
	bool	DeleteOverlapDefect_Mura_Mura(ResultPanelData& resultPanelData, double* dAlignPara=NULL);

	// CKI - RGB Bright Point - White Spot Mura �ߺ� ����  //B11 ���� 19.08.20
	bool	DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point(ResultPanelData& resultPanelData, double* dAlignPara = NULL);

	// CKI - Black hole �ߺ� ���� //B11 ���� 20.04.07
	bool    DeleteOverlapDefect_Black_Mura_and_Judge(ResultPanelData& resultPanelData, double* dAlignPara = NULL);

	// CKI - G64 ���Ͽ��� ������ �������� ǥ�������� �̿��� ������
//	int    Judge_Defect_BP_WSpot(ResultPanelData& resultPanelData, int index,int Defect_Judge ,double* dAlignPara = NULL);

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
