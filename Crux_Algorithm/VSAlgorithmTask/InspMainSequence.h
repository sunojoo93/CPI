/*****************************************************************************
  File Name		: InspMainSequence.h
  Version		: ver 1.0
  Create Date	: 2017.03.21
  Author		: CWH
  Description	: Area ���� �˻� ������
  Abbreviations	: 
 *****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PI acos(-1.)

#include "VSAlgorithmTask.h"
#include "JudgeDefect.h"

//----------------------------------------------------------------------------------------------------
//	 Class ��	:	InspMainSequence
//	 Class���	:	
//   �ֿ���	:
//	 �� �� ��	:	2017/02
//	 �� �� ��	:	CWH
//	 �������	:	V.1.0 �ʱ� �ۼ�
//	 ��    ��	:	
//----------------------------------------------------------------------------------------------------
class InspMainSequence :
	protected JudgeDefect
{
public:
	InspMainSequence();
	virtual ~InspMainSequence(void);


protected:

	// Generated message map functions
	afx_msg	LRESULT OnStartInspection(WPARAM wParam, LPARAM lParam);
	afx_msg	LRESULT OnStartSaveImage(WPARAM wParam, LPARAM lParam);

	BOOL	m_fnMainSequence(const CString strModelID, const CString strPanelID,const CString strLotID, const CString strLcp_Bubble_Path ,TCHAR* strSaveDrive,
							tAlignInfo stCamAlignInfo[MAX_CAMERA_COUNT], CWriteResultInfo& WrtResultInfo, ResultPanelData& ResultPanelData,
		cv::Mat MatOriginImg[][MAX_CAMERA_COUNT], cv::Mat MatColorBuffer[][MAX_CAMERA_COUNT] , cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], tCHoleAlignInfo& tCHoleAlignData,
							bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bStAlignEnd[MAX_CAMERA_COUNT], bool bStBackAlignEnd[MAX_CAMERA_COUNT], bool bStVacuumAlignEnd[MAX_CAMERA_COUNT], bool bStLabelMaskMakeEnd[MAX_CAMERA_COUNT], bool bStStageDustEnd[MAX_CAMERA_COUNT], long nError_Thread[MAX_CAMERA_COUNT], int nError_Image_Num[MAX_CAMERA_COUNT], bool bStSuccess[MAX_CAMERA_COUNT],
		bool bAlignEnd[MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER],bool& bChkDustEnd, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool bUseInspect, bool& bIsHeavyAlarm,
							ENUM_INSPECT_MODE eInspMode,STRU_IMAGE_INFO* pStImageInfo = NULL );							/*Common Information*/

	BOOL	CheckImageIsNormal(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, int nRatio, int nImageNum, int nCameraNum, int nStageNo,
						tAlignInfo stCamAlignInfo[MAX_CAMERA_COUNT], ResultBlob_Total* pResultBlobTotal, double dCamResolution,double dPannelSizeX,double dPannelSizeY, 
						bool bAlignEnd[MAX_CAMERA_COUNT], bool& bChkDustEnd, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, 
						ENUM_INSPECT_MODE eInspMode);


	BOOL	CheckImageIsNormal_LCP(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, cv::Mat& MatColorImage, cv::Mat& MatDrawImage, int nRatio, int nImageNum, int nCameraNum, int nStageNo,
		tAlignInfo stCamAlignInfo[MAX_CAMERA_COUNT], ResultBlob_Total* pResultBlobTotal, double dCamResolution, double dPannelSizeX, double dPannelSizeY,
		bool bStAlignEnd[MAX_CAMERA_COUNT], bool bStBackAlignEnd[MAX_CAMERA_COUNT], bool bStVacuumAlignEnd[MAX_CAMERA_COUNT],bool bStLabelMaskMakeEnd[MAX_CAMERA_COUNT], bool bStStageDustEnd[MAX_CAMERA_COUNT], long nError_Thread[MAX_CAMERA_COUNT], int nError_Image_Num[MAX_CAMERA_COUNT], bool bStSuccess[MAX_CAMERA_COUNT], 
		bool bAlignEnd[MAX_CAMERA_COUNT], bool& bChkDustEnd, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm,
		ENUM_INSPECT_MODE eInspMode);

	int		InspectImage(const CString strModelID, const CString strPanelID, const CString strDrive, const CString strLcp_Bubble_Path,
		cv::Mat MatOriginImg[][MAX_CAMERA_COUNT], cv::Mat& MatColorBuffer, cv::Mat& MatDrawBuffer, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], tCHoleAlignInfo& tCHoleAlignData,
						ResultBlob_Total* pResultBlob_Total, const int nImageNum, const int nCameraNum,	bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp);

	int		WriteResultData(const CString strPanelID, const CString strDrive,
						cv::Mat MatResult[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT],
						CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, ENUM_INSPECT_MODE eInspMode);

	void	SaveCropImage(CString strPanelID, CString strDrive, cv::Mat (*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], ResultPanelData& resultPanelData, ENUM_INSPECT_MODE eInspMode);

	// ��� �̹��� ����
	virtual bool	DrawDefectImage(CString strPanelID, 
										cv::Mat MatResult[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat (*MatDrawBuffer)[MAX_CAMERA_COUNT], 
										ResultPanelData& resultPanelData) = 0;


	//////////////////////////////////////////////////////////////////////////

	void Area_Cam_Img_Transform(cv::Mat& matSrc, tAlignInfo* pStAlignInfo , CString strSave_Path, double* dAlignPara);

	//////////////////////////////////////////////////////////////////////////
	long Lotte_Align(cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, cv::Mat& MatResultImage, int ImageClassify , bool bAlignEnd[MAX_CAMERA_COUNT] , int nCameraNum ,cv::Point &pAlign_Center ,Rect& boundingRect, double& fDegree , double* AlignPara);

	void MakeLabelMask(cv::Mat& matSrc, cv::Mat& matDstActive);

	long MakeMask(cv::Mat& matSrc, cv::Mat& matDstActive, cv::Mat& matDstLabel, cv::Mat& matDstHole, int nThreshActive, int nThreshLabel, int nThreshHole, cv::Point pAlign_Center, int offset);

	void MakeActiveMask(cv::Mat& matSrc, cv::Mat& matDstActive, int nThreshActive);

	void MinimumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);

	void MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);

	void Contrast(cv::Mat& matSrcImage, double dContrast_Max, double dContrast_Min, int nBrightness_Max, int nBrightness_Min);

	long RangeAvgThreshold_DUST(cv::Mat& matSrcImage, cv::Mat& matDstImage);

	long GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto);

	long GetMeanStdDev_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage, double &dblStdev);

	long GetAverage_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage);

	long MedianFilter(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nKSize);

	long Test();

	long Test2();

	long Test3();

	struct tBLOB_FEATURE
	{
		bool				bFiltering;				// ���͸� ��/��

		cv::Rect			rectBox;				// Bounding Box
		long				nArea;					// ��ü ����
		long				nBoxArea;				// Bounding Box ����
		float				fBoxRatio;				// Bounding Box ���� ���� / ��ü ���� ( Rectangularity(= Extent)
		cv::Point			ptCenter;				// �߽���
		long				nSumGV;					// ��� ����
		long				nMinGV;					// �ּ� ���
		long				nMaxGV;					// �ִ� ���
		float				fMeanGV;				// ��� ���
		float				fDiffGV;				// ( ��� - ��ü ) ��� ����
		float				fBKGV;					// ���(Background) ���
		float				fStdDev;				// ǥ�� ����
		float				fSEMU;					// SEMU
		float				fCompactness;			// ��ü�� �� ��翡 �󸶳� ����?
		float				nMinGVRatio;			// ��ü �ּ� ��� / ��� ���
		float				nMaxGVRatio;			// ��ü �ִ� ��� / ��� ���
		float				fDiffGVRatio;			// ��ü ��� ��� / ��� ���
		float				fPerimeter;				// �ܰ��� ����
		float				fRoundness;				// 
		float				fElongation;			// Box ���� / ����	
		float				fMinBoxArea;			// Feret��s area
		float				fMinorAxis;				// ���� ( Feret��s Diameter )
		float				fMajorAxis;				// ���� ( Feret��s diameter�� ������ ���� �� ���� ���� / Breath )
		float				fAxisRatio;				// ���� / ����( ELONGATION )
		float				fAngle;					// Angle between the horizontal axis ( Axis of least second moment )
		float				fMinBoxRatio;			// Min Bounding Box ���� ���� / ��ü ���� ( Area porosity )
		float				fMeanAreaRatio;			// choikwangil

		float				fAreaPer;			// choikwangil 04.20
		long				nJudge_GV;			// choikwangil 04.20
		long				nIn_Count;			// choikwangil 04.20

		__int64				nHist[IMAGE_MAX_GV];	// ��ü ������׷�

		CvSize2D32f			BoxSize;				// Box width and length

		vector <cv::Point>	ptIndexs;				// Blob �ȼ� ��ǥ
		vector <cv::Point>	ptContours;				// Blob �ܰ� ��ǥ

													// ����ü �ʱ�ȭ
		tBLOB_FEATURE() :
			nArea(0)
		{
			bFiltering = false;

			rectBox = cv::Rect(0, 0, 0, 0);
			nArea = 0;
			nBoxArea = 0;
			fBoxRatio = 0.0f;
			nSumGV = 0;
			nMinGV = 0;
			nMaxGV = 0;
			fMeanGV = 0.0f;
			fDiffGV = 0.0f;
			fBKGV = 0.0f;
			fStdDev = 0.0f;
			fSEMU = 0.0f;
			fCompactness = 0.0f;
			nMinGVRatio = 0.0f;
			nMaxGVRatio = 0.0f;
			fDiffGVRatio = 0.0f;
			fPerimeter = 0.0f;
			fRoundness = 0.0f;
			fElongation = 0.0f;
			fMinBoxArea = 0.0f;
			fMinorAxis = 0.0f;
			fMajorAxis = 0.0f;
			fAxisRatio = 0.0f;
			fAngle = 0.0f;
			fMinBoxRatio = 0.0f;
			fMeanAreaRatio = 0.0f; //choikwangil

			fAreaPer = 0.0f;	//choikwangil 04.20
			nJudge_GV = 0;	//choikwangil 04.20
			nIn_Count = 0;	//choikwangil 04.20


			memset(nHist, 0, sizeof(__int64) * IMAGE_MAX_GV);

			ptCenter = CvPoint2D32f(0, 0);
			BoxSize = CvSize2D32f(0, 0);

			// ���� �ʱ�ȭ
			vector <cv::Point>().swap(ptIndexs);
			vector <cv::Point>().swap(ptContours);
		}
	};


	vector<tBLOB_FEATURE>	m_BlobResult_Coaxial;	// Blob ��� ����Ʈ
	vector<tBLOB_FEATURE>	m_BlobResult_Side;	// Blob ��� ����Ʈ
	vector<tBLOB_FEATURE>	m_BlobResult_Dust;	// Blob ��� ����Ʈ

	vector<tBLOB_FEATURE>	m_BlobResult_CS;	// Blob ��� ����Ʈ
	vector<tBLOB_FEATURE>	m_BlobResult_Back;	// Blob ��� ����Ʈ

	vector<tBLOB_FEATURE>	m_BlobResult_Result;	// Blob ��� ����Ʈ
	//vector<tBLOB_FEATURE>	m_BlobResult_Final_Result;	// Blob ��� ����Ʈ

	bool DoFeatureBasic_8bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel, vector<tBLOB_FEATURE>& m_BlobResult);

	void Align_Test(cv::Mat MatOrgImage, cv::Mat& MatRotateImage ,float* fR_Degree, RotatedRect& rRotate_Rect);
	//////////////////////////////////////////////////////////////////////////
};
