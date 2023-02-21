/************************************************************************/
// Vacriance �ҷ� ���� ���� ���
// ������ : 22.01.11 - CKI
/************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "DefectCCD.h"
#include "MatBuf.h"				// �޸� ����
#include "InspectLibLog.h"

enum ENUM_PARA_LCP_VARIANCE_COMMON
{
	E_PARA_LCP_VARIANCE_COMMON_BLUR_LOOP = 0,	// Blur 5x5 �ݺ�
	E_PARA_LCP_VARIANCE_COMMON_BLUR_LOOP_SIZE,	// Blur 5x5 �ݺ�
	E_PARA_LCP_VARIANCE_COMMON_SEG_X,	// ���� ����ȭ ���� X
	E_PARA_LCP_VARIANCE_COMMON_SEG_Y,	// ���� ����ȭ ���� Y
	E_PARA_LCP_VARIANCE_COMMON_MIN_THRESHOLD,	// �ּ� ����ȭ �� ( ��ο� ����϶� ��� )
	E_PARA_LCP_VARIANCE_COMMON_MEDIAN,	// ū �ҷ��� ���, ���� ��հ��� �ֺ���� �ٸ�
	E_PARA_LCP_VARIANCE_COMMON_POW_USE,
	E_PARA_LCP_VARIANCE_COMMON_POW_VALUE,	// Pow
	E_PARA_LCP_VARIANCE_COMMON_ACTIVE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_VARIANCE_COMMON_ACTIVE_DARK_RATIO,	// Dark �ҷ� �ֺ� ��� ���
	E_PARA_LCP_VARIANCE_COMMON_ACTIVE_BRIGHT_RATIO,	// Bright �ҷ� �ֺ� ��� ���
	E_PARA_LCP_VARIANCE_COMMON_EDGE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_VARIANCE_COMMON_EDGE_AREA,	// �ܰ����κ��� ���� ����
	E_PARA_LCP_VARIANCE_COMMON_EDGE_DARK_RATIO,	// Dark �ҷ� �ֺ� ��� ���
	E_PARA_LCP_VARIANCE_COMMON_EDGE_BRIGHT_RATIO,	// Bright �ҷ� �ֺ� ��� ���

	E_PARA_LCP_VARIANCE_COMMON_TOTAL_COUNT
};

enum ENUM_PARA_LCP_VARIANCE_AREA
{
	E_PARA_LCP_VARIANCE_AREA_MAXICAN_TEXT = E_PARA_LCP_VARIANCE_COMMON_TOTAL_COUNT,
	E_PARA_LCP_VARIANCE_AREA_MAXICAN_SIZE,
	E_PARA_LCP_VARIANCE_AREA_MAXICAN_GAU_SIZE,
	E_PARA_LCP_VARIANCE_AREA_MAXICAN_GAU_SIG,

	E_PARA_LCP_VARIANCE_AREA_DELETE_TEXT,
	E_PARA_LCP_VARIANCE_AREA_DELETE_GAU_SIZE,
	E_PARA_LCP_VARIANCE_AREA_DELETE_GAU_SIG,
	E_PARA_LCP_VARIANCE_AREA_DELETE_THRESHOLD,

	E_PARA_LCP_VARIANCE_AREA_JUDGE_TEXT,
	E_PARA_LCP_VARIANCE_AREA_JUDGE_MIN_SIZE,
	E_PARA_LCP_VARIANCE_AREA_JUDGE_MAX_SIZE,
	E_PARA_LCP_VARIANCE_AREA_JUDGE_RADIUS,

	E_PARA_LCP_VARIANCE_AREA_TOTAL_COUNT
};


// Morphology - Operation
enum ENUM_VARIANCE_MORP
{
	E_VARIANCE_MORP_ERODE = 0,
	E_VARIANCE_MORP_DILATE,
	E_VARIANCE_MORP_OPEN,
	E_VARIANCE_MORP_CLOSE
};

class CInspectVariance
{
public:
	CInspectVariance();
	virtual ~CInspectVariance();

	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data) { cMem = data; };
	CMatBuf*	GetMem() { return	cMem; };


	// �α� ����
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

	// Main �˻� �˰���
	long		DoFindVarianceDefect(cv::Mat matSrcBuffer, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer, cv::Mat matMask_Active,
		cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, CDefectCCD *cCCD, cv::Mat* matCholeBuffer);

	long		LCP_AREA_Variance(cv::Mat& matSrcImage, cv::Mat* matDstImage, cv::Mat matMask_Active, CRect rectROI, cv::Rect rcAlignCellROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment);


	long		GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer, cv::Mat matDstBuffer[2], cv::Mat& matDrawBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);
	//
	long RangeAvgThreshold(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub);

	long RangeAvgThreshold_8bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub);

	long RangeAvgThreshold_16bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub);

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

	long DeleteArea_Variance(cv::Mat& matSrcImage,cv::Mat& matDraw, cv::Rect rcAlignCellROI, int nMin_Count, int nMax_Count, int nRadius, cv::Point ptCenter ,CMatBuf* cMemSub);

};

