
/************************************************************************/
// Blob ���� ���
// ������ : 17.03.08 - KSW
/************************************************************************/

#pragma once

// OpenCV Lib
#include "Define.h"

// ���� �ð� �Ѿ��, Ŭ���� ����
#include "TimeOut.h"

// �޸� ����
#include "MatBuf.h"
#include "InspectLibLog.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////////

// Blob �׷��ֱ�
#define BLOB_DRAW_ROTATED_BOX				0x000010
#define BLOB_DRAW_BOUNDING_BOX				0x000020
#define BLOB_DRAW_BLOBS						0x000080
#define BLOB_DRAW_BLOBS_CONTOUR				0x000100

//////////////////////////////////////////////////////////////////////////

// �ҷ�
enum ENUM_BOLB_FEATURE
{
	E_FEATURE_AREA			= 0		,	// ��ü ����
	E_FEATURE_BOX_AREA				,	// Bounding Box ����
	E_FEATURE_BOX_RATIO				,	// ��ü ���� / Bounding Box ���� ����
	E_FEATURE_SUM_GV				,	// ��� ����
	E_FEATURE_MIN_GV				,	// �ּ� ���
	E_FEATURE_MAX_GV				,	// �ִ� ���
	E_FEATURE_MEAN_GV				,	// ��� ���
	E_FEATURE_DIFF_GV				,	// ( ��� - ��ü ) ��� ����
	E_FEATURE_BK_GV					,	// ���(Background) ���
	E_FEATURE_STD_DEV				,	// ǥ�� ����
	E_FEATURE_SEMU					,	// SEMU
	E_FEATURE_COMPACTNESS			,	// ��ü�� �� ��翡 �󸶳� ����?
	E_FEATURE_MIN_GV_RATIO			,	// ��ü �ּ� ��� / ��� ���
	E_FEATURE_MAX_GV_RATIO			,	// ��ü �ִ� ��� / ��� ���
	E_FEATURE_DIFF_GV_RATIO			,	// ��ü ��� ��� / ��� ���
	E_FEATURE_PERIMETER				,	// �ܰ��� ����
	E_FEATURE_ROUNDNESS				,	// 
	E_FEATURE_ELONGATION			,	// Box ���� / ����
	E_FEATURE_BOX_X					,	// Bounding Box ���� ����
	E_FEATURE_BOX_Y					,	// Bounding Box ���� ����

	E_FEATURE_MIN_BOX_AREA			,	// Feret��s area
	E_FEATURE_MINOR_AXIS			,	// ����
	E_FEATURE_MAJOR_AXIS			,	// ����
	E_FEATURE_AXIS_RATIO			,	// ���� / ����
	E_FEATURE_MIN_BOX_RATIO			,	// ��ü ���� / Min Bounding Box ���� ���� ( Area porosity )

	E_FEATURE_GV_UP_COUNT_0			,	// ��� ����
	E_FEATURE_GV_UP_COUNT_1			,	// ��� ����
	E_FEATURE_GV_UP_COUNT_2			,	// ��� ����
	E_FEATURE_GV_DOWN_COUNT_0		,	// ��� ����
	E_FEATURE_GV_DOWN_COUNT_1		,	// ��� ����
	E_FEATURE_GV_DOWN_COUNT_2		,	// ��� ����

	E_FEATURE_MEANAREA_RATIO		,   // MeanAreaRatio choikwangil


	E_FEATURE_LAB_DEGREE,   // MeanAreaRatio choikwangil
	E_FEATURE_LAB_LENGHT,   // MeanAreaRatio choikwangil

 	E_FEATURE_LAB_L,   // MeanAreaRatio choikwangil
 	E_FEATURE_LAB_A,   // MeanAreaRatio choikwangil
 	E_FEATURE_LAB_B,   // MeanAreaRatio choikwangil
	E_FEATURE_MAX_MIN,
	E_FEATURE_RsubC, // ���� - ����Ʈ
	E_FEATURE_Mean_Min, // �ҷ���� - �ּҰ�

	E_FEATURE_GVAREA_RATIO_TEST		,   // 04.17 choikwangil

	E_FEATURE_IS_EDGE_C				,	// �ܰ��� ��ġ�� �ִ��� �ڳ�
	E_FEATURE_IS_EDGE_V				,	// �ܰ��� ��ġ�� �ִ��� ����
	E_FEATURE_IS_EDGE_H				,	// �ܰ��� ��ġ�� �ִ��� ����
	E_FEATURE_IS_EDGE_CENTER		,	// �ܰ��� ��ġ�� �ƴѰ�

	E_FEATURE_COUNT
};

//////////////////////////////////////////////////////////////////////////

// Blob ����ü
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
	float				fLab_Degree;			// 	
	float				fLab_Lenght;			// 	




	//////////////////////////////////////////////////////////////////////////compare�̻�� ������ ����� ����
	float				fLab_avg_L;			// 	
	float				fLab_avg_a;			// 
	float				fLab_avg_b;			// 
	float				fLab_diff_L;			// 
	float				fLab_diff_a;			// 
	float				fLab_diff_b;			// 
	String				strColor;
	float				fAreaPer;			// choikwangil 04.20
	long				nJudge_GV;			// choikwangil 04.20
	long				nIn_Count;			// choikwangil 04.20

	float				fMax_Min;			// 
	float				fRsubC;			// fRoundness - fCompactness
	float				fMean_Min;			// Mean-Min

	__int64				nHist[IMAGE_MAX_GV];	// ��ü ������׷�

	CvSize2D32f			BoxSize;				// Box width and length

	vector <cv::Point>	ptIndexs;				// Blob �ȼ� ��ǥ
	vector <cv::Point>	ptContours;				// Blob �ܰ� ��ǥ

	// ����ü �ʱ�ȭ
	tBLOB_FEATURE():
	nArea(0)
	{
		bFiltering			= false;

		rectBox				= cv::Rect(0, 0, 0, 0);
		nArea				= 0;
		nBoxArea			= 0;
		fBoxRatio			= 0.0f;
		nSumGV				= 0;
		nMinGV				= 0;
		nMaxGV				= 0;
		fMeanGV				= 0.0f;
		fDiffGV				= 0.0f;
		fBKGV				= 0.0f;
		fStdDev				= 0.0f;
		fSEMU				= 0.0f;
		fCompactness		= 0.0f;
		nMinGVRatio			= 0.0f;
		nMaxGVRatio			= 0.0f;
		fDiffGVRatio		= 0.0f;
		fPerimeter			= 0.0f;
		fRoundness			= 0.0f;
		fElongation			= 0.0f;
		fMinBoxArea			= 0.0f;
		fMinorAxis			= 0.0f;
		fMajorAxis			= 0.0f;
		fAxisRatio			= 0.0f;
		fAngle				= 0.0f;
		fMinBoxRatio		= 0.0f;
		fMeanAreaRatio      = 0.0f; //choikwangil
		fLab_Degree			= 0.0f;			// 	
		fLab_Lenght			= 0.0f;			// 	
		fLab_avg_L			=0.0f;			// 	
		fLab_avg_a			=0.0f;			// 
		fLab_avg_b			=0.0f;			// 
		fLab_diff_L			=0.0f;			// 
		fLab_diff_a			=0.0f;			// 
		fLab_diff_b			=0.0f;			// 
		strColor			="";

		fMax_Min			= 0.0f;
		fRsubC				= 0.0f;
		fMean_Min			= 0.0f;

		fAreaPer			= 0.0f;	//choikwangil 04.20
		nJudge_GV			= 0;	//choikwangil 04.20
		nIn_Count			= 0;	//choikwangil 04.20


		memset(nHist, 0, sizeof(__int64) * IMAGE_MAX_GV);

		ptCenter			= CvPoint2D32f(0, 0);
		BoxSize				= CvSize2D32f(0, 0);

		// ���� �ʱ�ȭ
		vector <cv::Point>().swap(ptIndexs);
		vector <cv::Point>().swap(ptContours);
	}
};

class CFeatureExtraction
{
public:
	CFeatureExtraction(void);
	virtual ~CFeatureExtraction(void);

	// �޸� ����
	bool	Release();

public:
	// �޸� ����
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};

	// �α� ����
	InspectLibLog*		m_cInspectLibLog;
	clock_t				m_tInitTime;
	clock_t				m_tBeforeTime;
	wchar_t*			m_strAlgLog;
	int					m_nAlgType;

	void		SetLog(InspectLibLog* cLog, int nAlgType, clock_t tTimeI, clock_t tTimeB, wchar_t* strLog)
	{
		m_tInitTime			= tTimeI;
		m_tBeforeTime		= tTimeB;
		m_cInspectLibLog	= cLog;
		m_strAlgLog			= strLog;
		m_nAlgType			= nAlgType;
	};

	void		writeInspectLog(char* strFunc, wchar_t* strTxt)
	{
		if( m_cInspectLibLog == NULL )
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(m_nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, m_strAlgLog);
	};

	void		writeInspectLog_Memory(int nAlgType, char* strFunc, wchar_t* strTxt, __int64 nMemory_Use_Value = 0 , int nSub_AlgType = NULL)
	{
		if (m_cInspectLibLog == NULL)
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, nMemory_Use_Value, nSub_AlgType ,m_strAlgLog);
	};

	//////////////////////////////////////////////////////////////////////////

	// Blob �˰��� ����
	bool	DoBlobCalculate(cv::Mat ThresholdBuffer, cv::Mat GrayBuffer = cv::Mat(), int nMaxDefectCount=99999);

	// Blob �˰��� ���� ROI
	bool	DoBlobCalculate(cv::Mat ThresholdBuffer, CRect rectROI, cv::Mat GrayBuffer = cv::Mat(), int nMaxDefectCount = 99999);
	//////////////////////////////////////////////////////////////////////////

	bool	DoBlobCalculate_LCP(cv::Mat ThresholdBuffer, CRect rectROI, double* dPara, cv::Mat GrayBuffer = cv::Mat(), cv::Mat ColorBuffer = cv::Mat(),  int nMaxDefectCount = 99999);
	//////////////////////////////////////////////////////////////////////////

	// Blob & ���� ��� ( Single �� )
	long	DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, int* nCommonPara,
		long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType,  bool bPtRotate=true);

	// Blob & ���� ���
	long	DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, int* nCommonPara,
								long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob,   bool bPtRotate=true);

	// Blob & ���� ��� ( Single �� ) ROI
	long	DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI, int* nCommonPara,
		long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType, bool bPtRotate = true);

	// Blob & ���� ��� ROI
	long	DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI, int* nCommonPara,
								long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob,   bool bPtRotate=true);

	long	DoDefectBlobJudgment_LCP(cv::Mat& matSrcImage, cv::Mat& matColorImage , cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI, int* nCommonPara,
		long nDefectColor, double* dPara, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate = true);

	// ���� ���� ������...
	long	DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, STRU_DEFECT_ITEM* EngineerDefectJudgment, int nDefectType = 0, int nMaxDefectCount=99999 );

	// ���� ���� ������...
	long	DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, STRU_DEFECT_ITEM* EngineerDefectJudgment, int nMaxDefectCount=99999);

	// ��� �׷��ֱ�
	bool	DrawBlob(cv::Mat& DrawBuffer, CvScalar DrawColor, long nOption, bool bSelect = false, float fFontSize = 0.4f);

	// �ܰ��� ��ǥ �ؽ�Ʈ ����
	bool	SaveTxt(int* nCommonPara, wchar_t* strContourTxt, bool bUse=false);

	// ��� �ѱ��
	bool	GetResultblob(vector<tBLOB_FEATURE>& OutBlob);

	// ���� �ܰ����� ���͸�
	long	DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, int* nCommonPara, 
				long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate, CRect prerectROI, int offset);

	long	DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
							int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType,  bool bPtRotate, CRect prerectROI, int offset);
							
	long	DoDefectBlobMultiJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
							int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, vector<int> nDefectType,  bool bPtRotate, CRect prerectROI, int offset);

	// �� ��
	bool	Compare(double dFeatureValue, int nSign, double dValue);

	// ��ǥ ����
	void	CoordApply(CRect rectROI, int nTotalLabel);

protected:
	bool					m_bComplete;	// Blob �Ϸ� �ߴ��� Ȯ�ο�.
	vector<tBLOB_FEATURE>	m_BlobResult;	// Blob ��� ����Ʈ

	// �ҷ� ���� ���͸�
	bool			DoColorFilter(int nDefectName, int nDefectColor);

	// ���͸�
	bool			DoFiltering(tBLOB_FEATURE& tBlobResult, int nBlobFilter, int nSign, double dValue);

	// Feature Calculate ( 8bit & 12bit )
	bool			DoFeatureBasic_8bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel, CMatBuf* cMemSub=NULL);
	bool			DoFeatureBasic_8bit_LCP(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, cv::Mat& ColorBuffer, double* dPara, int nTotalLabel, CMatBuf* cMemSub = NULL);


	bool			DoFeatureBasic_16bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel, CMatBuf* cMemSub=NULL);

	// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
	bool			calcContours(int* nContoursX, int* nContoursY, int nDefectIndex, float fAngle, int nCx, int nCy, int nPs);

	// ���� �ܰ����� ���͸�
	bool			is_edge(tBLOB_FEATURE& tBlobResult, int nBlobFilter, CRect prerectROI, int offset);
	
private:
	// �гκ� ��ǥ txt ������ �ϳ��� ����. ���� ���� ����
	CRITICAL_SECTION m_csCoordFile;

	// ���� �ð� �Ѿ��, Ŭ���� ����
	CTimeOut cTimeOut;

};