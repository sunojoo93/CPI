
/************************************************************************/
// Mura �ҷ� ���� ���� ���
// ������ : 18.05.31 - KSW
/************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "InspectLibLog.h"
#include "MatBuf.h"				// �޸� ����




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

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_FLAG,	// Flag
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MORP_OBJ,	// �ҷ� ũ�� ����
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MORP_BKG,	// ���� �κ� ũ�� ����  ( ex:���� )
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_THRESHOLD,	// ���� ���� Ȯ��

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_BRIGHT_RATIO,	// ���� ��� ����
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC1_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_1,	// ��ο� ��� ����1
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA1_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA1_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_1,	// ��ο� ��� Diff ����1
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC2_FLAG, //SPEC2 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_2,	// ��ο� ��� ����2
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA2_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA2_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_2,	// ��ο� ��� Diff ����2
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC3_FLAG, //SPEC3 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_3,	// ��ο� ��� ����3
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA3_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA3_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_3,	// ��ο� ��� Diff ����3
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC4_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_4,	// ��ο� ��� ����4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA4_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA4_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_4,	// ��ο� ��� Diff ����4

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC5_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_5,	// ��ο� ��� ����4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA5_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA5_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_5,	// ��ο� ��� Diff ����4

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC6_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_6,	// ��ο� ��� ����4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA6_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA6_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_6,	// ��ο� ��� Diff ����4

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC7_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_7,	// ��ο� ��� ����4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA7_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA7_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_7,	// ��ο� ��� Diff ����4

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC8_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_8,	// ��ο� ��� ����4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA8_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA8_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_8,	// ��ο� ��� Diff ����4

	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_LEFT,	// Edge ���� ����
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_TOP,	// Edge ���� ����
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_RIGHT,	// Edge ���� ����
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_BOTTOM,	// Edge ���� ����
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_BRIGHT_RATIO,	// ���� ��� ����
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC1_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_1,	// ��ο� ��� ����1
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA1_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA1_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_1,	// ��ο� ��� Diff ����1
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC2_FLAG, //SPEC2 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_2,	// ��ο� ��� ����2
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA2_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA2_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_2,	// ��ο� ��� Diff ����2
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC3_FLAG, //SPEC3 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_3,	// ��ο� ��� ����3
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA3_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA3_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_3,	// ��ο� ��� Diff ����3
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC4_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_4,	// ��ο� ��� ����4
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA4_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA4_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_4,	// ��ο� ��� Diff ����4
};


class CInspectMura3
{
public:
	CInspectMura3(void);
	virtual ~CInspectMura3(void);

	// �޸� ����
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data) { cMem = data; };
	CMatBuf*	GetMem() { return	cMem; };

	// �α� ����
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

	// Main �˻� �˰���
	long		DoFindMuraDefect(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);
	
	long		DoFindMuraDefect2(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Mat& matDrawBuffer, wchar_t* strContourTxt);

	long		GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
		double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);

	void		Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset = 0);

	void Filter8(BYTE* InImg, BYTE* OutImg,int nMin,int nMax, int width, int height);

// 	// Dust ���� ��, ��� ���� �Ѱ��ֱ�
// 	long		GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
// 		double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);

protected:
	long		LogicStart_SPOT(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath);


protected:

	// ���� ���� ( 8bit & 12bit )
	long		ImageSave(CString strPath, cv::Mat matSrcBuf);

	// ȸ�� �簢�� ��ġ����(�浹) Ȯ��
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
