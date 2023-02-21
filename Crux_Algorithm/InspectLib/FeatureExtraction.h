
/************************************************************************/
// Blob 관련 헤더
// 수정일 : 17.03.08 - KSW
/************************************************************************/

#pragma once

// OpenCV Lib
#include "Define.h"

// 설정 시간 넘어가면, 클래스 종료
#include "TimeOut.h"

// 메모리 관리
#include "MatBuf.h"
#include "InspectLibLog.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////////

// Blob 그려주기
#define BLOB_DRAW_ROTATED_BOX				0x000010
#define BLOB_DRAW_BOUNDING_BOX				0x000020
#define BLOB_DRAW_BLOBS						0x000080
#define BLOB_DRAW_BLOBS_CONTOUR				0x000100

//////////////////////////////////////////////////////////////////////////

// 불량
enum ENUM_BOLB_FEATURE
{
	E_FEATURE_AREA			= 0		,	// 객체 면적
	E_FEATURE_BOX_AREA				,	// Bounding Box 면적
	E_FEATURE_BOX_RATIO				,	// 객체 면적 / Bounding Box 면적 비율
	E_FEATURE_SUM_GV				,	// 밝기 누적
	E_FEATURE_MIN_GV				,	// 최소 밝기
	E_FEATURE_MAX_GV				,	// 최대 밝기
	E_FEATURE_MEAN_GV				,	// 평균 밝기
	E_FEATURE_DIFF_GV				,	// ( 배경 - 객체 ) 밝기 차이
	E_FEATURE_BK_GV					,	// 배경(Background) 밝기
	E_FEATURE_STD_DEV				,	// 표준 편차
	E_FEATURE_SEMU					,	// SEMU
	E_FEATURE_COMPACTNESS			,	// 객체가 원 모양에 얼마나 가까운가?
	E_FEATURE_MIN_GV_RATIO			,	// 객체 최소 밝기 / 배경 밝기
	E_FEATURE_MAX_GV_RATIO			,	// 객체 최대 밝기 / 배경 밝기
	E_FEATURE_DIFF_GV_RATIO			,	// 객체 평균 밝기 / 배경 밝기
	E_FEATURE_PERIMETER				,	// 외곽선 길이
	E_FEATURE_ROUNDNESS				,	// 
	E_FEATURE_ELONGATION			,	// Box 가로 / 세로
	E_FEATURE_BOX_X					,	// Bounding Box 가로 길이
	E_FEATURE_BOX_Y					,	// Bounding Box 세로 길이

	E_FEATURE_MIN_BOX_AREA			,	// Feret’s area
	E_FEATURE_MINOR_AXIS			,	// 장축
	E_FEATURE_MAJOR_AXIS			,	// 단축
	E_FEATURE_AXIS_RATIO			,	// 장축 / 단축
	E_FEATURE_MIN_BOX_RATIO			,	// 객체 면적 / Min Bounding Box 면적 비율 ( Area porosity )

	E_FEATURE_GV_UP_COUNT_0			,	// 밝기 갯수
	E_FEATURE_GV_UP_COUNT_1			,	// 밝기 갯수
	E_FEATURE_GV_UP_COUNT_2			,	// 밝기 갯수
	E_FEATURE_GV_DOWN_COUNT_0		,	// 밝기 갯수
	E_FEATURE_GV_DOWN_COUNT_1		,	// 밝기 갯수
	E_FEATURE_GV_DOWN_COUNT_2		,	// 밝기 갯수

	E_FEATURE_MEANAREA_RATIO		,   // MeanAreaRatio choikwangil


	E_FEATURE_LAB_DEGREE,   // MeanAreaRatio choikwangil
	E_FEATURE_LAB_LENGHT,   // MeanAreaRatio choikwangil

 	E_FEATURE_LAB_L,   // MeanAreaRatio choikwangil
 	E_FEATURE_LAB_A,   // MeanAreaRatio choikwangil
 	E_FEATURE_LAB_B,   // MeanAreaRatio choikwangil
	E_FEATURE_MAX_MIN,
	E_FEATURE_RsubC, // 라운드 - 컴펙트
	E_FEATURE_Mean_Min, // 불량평균 - 최소값

	E_FEATURE_GVAREA_RATIO_TEST		,   // 04.17 choikwangil

	E_FEATURE_IS_EDGE_C				,	// 외곽의 위치에 있는지 코너
	E_FEATURE_IS_EDGE_V				,	// 외곽의 위치에 있는지 세로
	E_FEATURE_IS_EDGE_H				,	// 외곽의 위치에 있는지 가로
	E_FEATURE_IS_EDGE_CENTER		,	// 외곽의 위치가 아닌거

	E_FEATURE_COUNT
};

//////////////////////////////////////////////////////////////////////////

// Blob 구조체
struct tBLOB_FEATURE
{
	bool				bFiltering;				// 필터링 유/무

	cv::Rect			rectBox;				// Bounding Box
	long				nArea;					// 객체 면적
	long				nBoxArea;				// Bounding Box 면적
	float				fBoxRatio;				// Bounding Box 면적 비율 / 객체 면적 ( Rectangularity(= Extent)
	cv::Point			ptCenter;				// 중심점
	long				nSumGV;					// 밝기 누적
	long				nMinGV;					// 최소 밝기
	long				nMaxGV;					// 최대 밝기
	float				fMeanGV;				// 평균 밝기
	float				fDiffGV;				// ( 배경 - 객체 ) 밝기 차이
	float				fBKGV;					// 배경(Background) 밝기
	float				fStdDev;				// 표준 편차
	float				fSEMU;					// SEMU
	float				fCompactness;			// 객체가 원 모양에 얼마나 가까운가?
	float				nMinGVRatio;			// 객체 최소 밝기 / 배경 밝기
	float				nMaxGVRatio;			// 객체 최대 밝기 / 배경 밝기
	float				fDiffGVRatio;			// 객체 평균 밝기 / 배경 밝기
	float				fPerimeter;				// 외곽선 길이
	float				fRoundness;				// 
	float				fElongation;			// Box 가로 / 세로	
	float				fMinBoxArea;			// Feret’s area
	float				fMinorAxis;				// 장축 ( Feret’s Diameter )
	float				fMajorAxis;				// 단축 ( Feret’s diameter에 수직인 가장 긴 축의 길이 / Breath )
	float				fAxisRatio;				// 장축 / 단축( ELONGATION )
	float				fAngle;					// Angle between the horizontal axis ( Axis of least second moment )
	float				fMinBoxRatio;			// Min Bounding Box 면적 비율 / 객체 면적 ( Area porosity )
	float				fMeanAreaRatio;			// choikwangil
	float				fLab_Degree;			// 	
	float				fLab_Lenght;			// 	




	//////////////////////////////////////////////////////////////////////////compare미사용 데이터 저장용 변수
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

	__int64				nHist[IMAGE_MAX_GV];	// 객체 히스토그램

	CvSize2D32f			BoxSize;				// Box width and length

	vector <cv::Point>	ptIndexs;				// Blob 픽셀 좌표
	vector <cv::Point>	ptContours;				// Blob 외곽 좌표

	// 구조체 초기화
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

		// 벡터 초기화
		vector <cv::Point>().swap(ptIndexs);
		vector <cv::Point>().swap(ptContours);
	}
};

class CFeatureExtraction
{
public:
	CFeatureExtraction(void);
	virtual ~CFeatureExtraction(void);

	// 메모리 해제
	bool	Release();

public:
	// 메모리 관리
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};

	// 로그 관리
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

	// Blob 알고리즘 실행
	bool	DoBlobCalculate(cv::Mat ThresholdBuffer, cv::Mat GrayBuffer = cv::Mat(), int nMaxDefectCount=99999);

	// Blob 알고리즘 실행 ROI
	bool	DoBlobCalculate(cv::Mat ThresholdBuffer, CRect rectROI, cv::Mat GrayBuffer = cv::Mat(), int nMaxDefectCount = 99999);
	//////////////////////////////////////////////////////////////////////////

	bool	DoBlobCalculate_LCP(cv::Mat ThresholdBuffer, CRect rectROI, double* dPara, cv::Mat GrayBuffer = cv::Mat(), cv::Mat ColorBuffer = cv::Mat(),  int nMaxDefectCount = 99999);
	//////////////////////////////////////////////////////////////////////////

	// Blob & 판정 결과 ( Single 용 )
	long	DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, int* nCommonPara,
		long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType,  bool bPtRotate=true);

	// Blob & 판정 결과
	long	DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, int* nCommonPara,
								long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob,   bool bPtRotate=true);

	// Blob & 판정 결과 ( Single 용 ) ROI
	long	DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI, int* nCommonPara,
		long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType, bool bPtRotate = true);

	// Blob & 판정 결과 ROI
	long	DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI, int* nCommonPara,
								long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob,   bool bPtRotate=true);

	long	DoDefectBlobJudgment_LCP(cv::Mat& matSrcImage, cv::Mat& matColorImage , cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI, int* nCommonPara,
		long nDefectColor, double* dPara, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate = true);

	// 보고 없이 정보만...
	long	DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, STRU_DEFECT_ITEM* EngineerDefectJudgment, int nDefectType = 0, int nMaxDefectCount=99999 );

	// 보고 없이 정보만...
	long	DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, STRU_DEFECT_ITEM* EngineerDefectJudgment, int nMaxDefectCount=99999);

	// 결과 그려주기
	bool	DrawBlob(cv::Mat& DrawBuffer, CvScalar DrawColor, long nOption, bool bSelect = false, float fFontSize = 0.4f);

	// 외곽선 좌표 텍스트 저장
	bool	SaveTxt(int* nCommonPara, wchar_t* strContourTxt, bool bUse=false);

	// 결과 넘기기
	bool	GetResultblob(vector<tBLOB_FEATURE>& OutBlob);

	// 수정 외곽인지 필터링
	long	DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, int* nCommonPara, 
				long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate, CRect prerectROI, int offset);

	long	DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
							int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType,  bool bPtRotate, CRect prerectROI, int offset);
							
	long	DoDefectBlobMultiJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
							int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, vector<int> nDefectType,  bool bPtRotate, CRect prerectROI, int offset);

	// 값 비교
	bool	Compare(double dFeatureValue, int nSign, double dValue);

	// 좌표 보정
	void	CoordApply(CRect rectROI, int nTotalLabel);

protected:
	bool					m_bComplete;	// Blob 완료 했는지 확인용.
	vector<tBLOB_FEATURE>	m_BlobResult;	// Blob 결과 리스트

	// 불량 색상 필터링
	bool			DoColorFilter(int nDefectName, int nDefectColor);

	// 필터링
	bool			DoFiltering(tBLOB_FEATURE& tBlobResult, int nBlobFilter, int nSign, double dValue);

	// Feature Calculate ( 8bit & 12bit )
	bool			DoFeatureBasic_8bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel, CMatBuf* cMemSub=NULL);
	bool			DoFeatureBasic_8bit_LCP(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, cv::Mat& ColorBuffer, double* dPara, int nTotalLabel, CMatBuf* cMemSub = NULL);


	bool			DoFeatureBasic_16bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel, CMatBuf* cMemSub=NULL);

	// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
	bool			calcContours(int* nContoursX, int* nContoursY, int nDefectIndex, float fAngle, int nCx, int nCy, int nPs);

	// 수정 외곽인지 필터링
	bool			is_edge(tBLOB_FEATURE& tBlobResult, int nBlobFilter, CRect prerectROI, int offset);
	
private:
	// 패널별 좌표 txt 파일은 하나만 기입. 동시 접근 제거
	CRITICAL_SECTION m_csCoordFile;

	// 설정 시간 넘어가면, 클래스 종료
	CTimeOut cTimeOut;

};