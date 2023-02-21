
/************************************************************************/
// Mura 불량 검출 관련 헤더
// 수정일 : 18.05.31 - KSW
/************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "InspectLibLog.h"
#include "MatBuf.h"				// 메모리 관리




enum ENUM_PARA_AVI_MURA3_GRAY
{
	
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_RESIZE,
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_BLUR_SIZE,
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_BLUR_SIGMA,

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_BRIGHT_FLAG,			//choi_2104
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_BRIGHT_RATIO,
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_BRIGHT_EDGE_RATIO,

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_DARK_FLAG,			//choi_2104
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_DARK_RATIO,          //choi_2104
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_DARK_EDGE_RATIO,	    //choi_2104
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_DARK_MINIMUM_SIZE,	    //choi_2104

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SEG_X,
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SEG_Y,

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_TEXT,				//choi_2104
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_FILTER_SIZE,		//choi_2104
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_BLUR_SIZE,			//choi_2104
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_BLUR_SIGMA,		//choi_2104

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_DARK_TEXT,				//choi_2104
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_DARK_FILTER_SIZE,		//choi_2104
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_DARK_BLUR_SIZE,			//choi_2104
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_DARK_BLUR_SIGMA,		//choi_2104

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_TEXT,	// 텍스트 표시
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_FLAG,	// Flag
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MORP_OBJ,	// 불량 크게 설정
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MORP_BKG,	// 밝은 부분 크게 설정  ( ex:명점 )
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_THRESHOLD,	// 밝은 명점 확인

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_TEXT,	// 텍스트 표시
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_BRIGHT_RATIO,	// 밝은 경우 조건
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC1_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_1,	// 어두운 경우 조건1
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA1_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA1_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_1,	// 어두운 경우 Diff 조건1
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC2_FLAG, //SPEC2 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_2,	// 어두운 경우 조건2
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA2_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA2_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_2,	// 어두운 경우 Diff 조건2
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC3_FLAG, //SPEC3 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_3,	// 어두운 경우 조건3
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA3_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA3_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_3,	// 어두운 경우 Diff 조건3
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC4_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_4,	// 어두운 경우 조건4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA4_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA4_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_4,	// 어두운 경우 Diff 조건4

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC5_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_5,	// 어두운 경우 조건4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA5_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA5_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_5,	// 어두운 경우 Diff 조건4

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC6_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_6,	// 어두운 경우 조건4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA6_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA6_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_6,	// 어두운 경우 Diff 조건4

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC7_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_7,	// 어두운 경우 조건4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA7_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA7_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_7,	// 어두운 경우 Diff 조건4

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC8_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_8,	// 어두운 경우 조건4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA8_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA8_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_8,	// 어두운 경우 Diff 조건4

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_TEXT,	// 텍스트 표시
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_LEFT,	// Edge 영역 설정
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_TOP,	// Edge 영역 설정
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_RIGHT,	// Edge 영역 설정
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_BOTTOM,	// Edge 영역 설정
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_BRIGHT_RATIO,	// 밝은 경우 조건
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC1_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_1,	// 어두운 경우 조건1
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA1_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA1_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_1,	// 어두운 경우 Diff 조건1
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC2_FLAG, //SPEC2 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_2,	// 어두운 경우 조건2
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA2_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA2_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_2,	// 어두운 경우 Diff 조건2
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC3_FLAG, //SPEC3 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_3,	// 어두운 경우 조건3
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA3_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA3_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_3,	// 어두운 경우 Diff 조건3
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC4_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_4,	// 어두운 경우 조건4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA4_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA4_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_4,	// 어두운 경우 Diff 조건4
};


class CInspectMura3
{
public:
	CInspectMura3(void);
	virtual ~CInspectMura3(void);

	// 메모리 관리
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data) { cMem = data; };
	CMatBuf*	GetMem() { return	cMem; };

	// 로그 관리
	InspectLibLog*		m_cInspectLibLog;
	clock_t				m_tInitTime;
	clock_t				m_tBeforeTime;
	wchar_t*			m_strAlgLog;

//private:
	//static int sz;				 //choi 05.08

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
	//////////////////////////////////////////////////////////////////////////

	// Main 검사 알고리즘
	long		DoFindMuraDefect(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);
	
	long		DoFindMuraDefect2(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Mat& matDrawBuffer, wchar_t* strContourTxt);

	long		GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
		double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);

	void		Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset = 0);

	void Filter8(BYTE* InImg, BYTE* OutImg,int nMin,int nMax, int width, int height);

// 	// Dust 제거 후, 결과 벡터 넘겨주기
// 	long		GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
// 		double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);

protected:
	long		LogicStart_SPOT(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath);


protected:

	// 영상 저장 ( 8bit & 12bit )
	long		ImageSave(CString strPath, cv::Mat matSrcBuf);

	// 회전 사각형 곂치는지(충돌) 확인
	bool		OrientedBoundingBox(cv::RotatedRect& rect1, cv::RotatedRect& rect2);

	////////////////////////////////////////////////////////////////////////// choi 05.01
	long  CInspectMura3::C_Mexican_filter(cv::Mat& ip , int sz ,int nMx_blur_sz, int nMx_blur_sigma);

	//float*  CInspectMura3::gauss1D(int r);

	float*  CInspectMura3::diff2Gauss1D(int r);

	float*  CInspectMura3::computeKernel2D(int r);

	boolean  CInspectMura3::convolveFloat(cv::Mat& ip, float* kernel, int kw, int kh, int sz);

	double CInspectMura3::getScale(float* kernel, int sz);

	long CInspectMura3::RangeAvgThreshold_Gray(cv::Mat& matSrcImage, cv::Mat& matDstImage, CRect rectROI,
		long nLoop, long nSegX, long nSegY, float fDarkRatio, float fBrightRatio, float fDarkRatio_Edge, float fBrightRatio_Edge,  int Defect_Color_mode, CMatBuf* cMemSub);

//	void CInspectMura3::Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset);

	long CInspectMura3::GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto);

	int CInspectMura3::GetBitFromImageDepth(int nDepth);

	long CInspectMura3::GetMeanStdDev_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage, double &dblStdev);

	long CInspectMura3::GetAverage_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage);

	void CInspectMura3::MinimumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);

	void CInspectMura3::MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);

	bool CInspectMura3::cMeanFilte(cv::Mat matActiveImage, cv::Mat& matDstImage);

//	long CInspectMura3::ImageSave(CString strPath, cv::Mat matSrcBuf);

	long CInspectMura3::JudgeWhiteSpot(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, stDefectInfo* pResultBlob, CMatBuf* cMemSub);
	//////////////////////////////////////////////////////////////////////////

};
