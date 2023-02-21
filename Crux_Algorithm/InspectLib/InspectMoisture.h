/************************************************************************/
// MOISTURE 불량 검출 관련 헤더
// 수정일 : 22.01.11 - CKI
/************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "DefectCCD.h"
#include "MatBuf.h"				// 메모리 관리
#include "InspectLibLog.h"

enum ENUM_PARA_LCP_MOISTURE_COMMON
{
	E_PARA_LCP_MOISTURE_COMMON_BLUR_LOOP = 0,	// Blur 5x5 반복
	E_PARA_LCP_MOISTURE_COMMON_BLUR_LOOP_SIZE,	// Blur 5x5 반복
	E_PARA_LCP_MOISTURE_COMMON_SEG_X,	// 분할 이진화 영역 X
	E_PARA_LCP_MOISTURE_COMMON_SEG_Y,	// 분할 이진화 영역 Y
	E_PARA_LCP_MOISTURE_COMMON_MIN_THRESHOLD,	// 최소 이진화 값 ( 어두운 밝기일때 사용 )
	E_PARA_LCP_MOISTURE_COMMON_MEDIAN,	// 큰 불량인 경우, 영역 평균값이 주변대비 다름
	E_PARA_LCP_MOISTURE_COMMON_POW_USE,
	E_PARA_LCP_MOISTURE_COMMON_POW_VALUE,	// Pow
	E_PARA_LCP_MOISTURE_COMMON_ACTIVE_TEXT,	// 텍스트 표시
	E_PARA_LCP_MOISTURE_COMMON_ACTIVE_DARK_RATIO,	// Dark 불량 주변 밝기 대비
	E_PARA_LCP_MOISTURE_COMMON_ACTIVE_BRIGHT_RATIO,	// Bright 불량 주변 밝기 대비
	E_PARA_LCP_MOISTURE_COMMON_EDGE_TEXT,	// 텍스트 표시
	E_PARA_LCP_MOISTURE_COMMON_EDGE_AREA,	// 외곽으로부터 범위 설정
	E_PARA_LCP_MOISTURE_COMMON_EDGE_DARK_RATIO,	// Dark 불량 주변 밝기 대비
	E_PARA_LCP_MOISTURE_COMMON_EDGE_BRIGHT_RATIO,	// Bright 불량 주변 밝기 대비

	E_PARA_LCP_MOISTURE_COMMON_TOTAL_COUNT
};

enum ENUM_PARA_LCP_MOISTURE_AREA
{
	E_PARA_LCP_MOISTURE_AREA_MAXICAN_TEXT = E_PARA_LCP_MOISTURE_COMMON_TOTAL_COUNT,	
	E_PARA_LCP_MOISTURE_AREA_MAXICAN_SIZE,
	E_PARA_LCP_MOISTURE_AREA_MAXICAN_GAU_SIZE,
	E_PARA_LCP_MOISTURE_AREA_MAXICAN_GAU_SIG,

	E_PARA_LCP_MOISTURE_AREA_MASK_TEXT,
	E_PARA_LCP_MOISTURE_AREA_MASK_SIZE_DOWN,
	E_PARA_LCP_MOISTURE_AREA_MASK_SIZE_UP,

	E_PARA_LCP_MOISTURE_AREA_GAUBLUR_TEXT,
	E_PARA_LCP_MOISTURE_AREA_GAUBLUR_SIZE,

	E_PARA_LCP_MOISTURE_AREA_MEANBLUR_TEXT,
	E_PARA_LCP_MOISTURE_AREA_MEANBLUR_SIZE,

	E_PARA_LCP_MOISTURE_AREA_THRESHOLD_TEXT,
	E_PARA_LCP_MOISTURE_AREA_THRESHOLD_VALUE,

	E_PARA_LCP_MOISTURE_AREA_TOTAL_COUNT
};


// Morphology - Operation
enum ENUM_MOISTURE_MORP
{
	E_MOISTURE_MORP_ERODE = 0,
	E_MOISTURE_MORP_DILATE,
	E_MOISTURE_MORP_OPEN,
	E_MOISTURE_MORP_CLOSE
};

//////////////////////////////////////////////////////////////////////////

class CInspectMoisture
{
public:
	CInspectMoisture();
	virtual ~CInspectMoisture();

	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data) { cMem = data; };
	CMatBuf*	GetMem() { return	cMem; };


	// 로그 관리
	InspectLibLog*		m_cInspectLibLog;
	clock_t				m_tInitTime;
	clock_t				m_tBeforeTime;
	wchar_t*			m_strAlgLog;

	void		SetLog(InspectLibLog* cLog, clock_t tTimeI, clock_t tTimeB, wchar_t* strLog)
	{
		m_tInitTime = tTimeI;
		m_tBeforeTime = tTimeB;
		m_cInspectLibLog = cLog;
		m_strAlgLog = strLog;
	};

	void		writeInspectLog(int nAlgType, char* strFunc, wchar_t* strTxt)
	{
		if (m_cInspectLibLog == NULL)
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, m_strAlgLog);
	};

	void		writeInspectLog_Memory(int nAlgType, char* strFunc, wchar_t* strTxt, __int64 nMemory_Use_Value = 0)
	{
		if (m_cInspectLibLog == NULL)
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, nMemory_Use_Value, m_strAlgLog);
	};

	CString		GETDRV()
	{
		return m_cInspectLibLog->GETDRV();
	}

	//////////////////////////////////////////////////////////////////////////

	// Main 검사 알고리즘
	long		DoFindMoistureDefect(cv::Mat matSrcBuffer, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer, cv::Mat matMask_Active, cv::Mat matMask_Hole,
		cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, CDefectCCD *cCCD, cv::Mat* matCholeBuffer);

	long		LCP_AREA_Moisture(cv::Mat& matSrcImage, cv::Mat* matDstImage, cv::Mat matMask_Active, cv::Mat matMask_Hole, CRect rectROI, cv::Rect rcAlignCellROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment);


	long		GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer, cv::Mat matDstBuffer[2], cv::Mat& matDrawBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);
	//
	long RangeAvgThreshold_Bubble(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub);

	long RangeAvgThreshold_Bubble_8bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub);

	long RangeAvgThreshold_Bubble_16bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub);

	long GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto);

	long GetAverage_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage);

	long GetMeanStdDev_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage, double &dblStdev);

	long MedianFilter(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nKSize, CMatBuf* cMemSub);

	void Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset = 0);

	long ImageSave(CString strPath, cv::Mat matSrcBuf);

	long Image_Pow(int ImgType, double dpow, Mat& InPutImage, Mat& OutPutImage);

	long Binary_16bit(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double thresh, bool bInv, CMatBuf* cMemSub);

	long C_Mexican_filter(cv::Mat& ip, int sz, int nMx_blur_sz, int nMx_blur_sigma);

	float* computeKernel2D(int r);

	boolean  convolveFloat(cv::Mat& ip, float* kernel, int kw, int kh, int sz);

	double getScale(float* kernel, int sz);

	void Mask_SizeDown(cv::Mat matSrc, int nUpSize);

	void Mask_SizeUp(cv::Mat matSrc, int nUpSize);

	void VarianceFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);
};

