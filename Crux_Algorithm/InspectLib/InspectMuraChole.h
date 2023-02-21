#pragma once

/************************************************************************/
// Mura 불량 검출 관련 헤더
// 수정일 : 18.05.31 - KSW
/************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "InspectLibLog.h"
#include "MatBuf.h"				// 메모리 관리

/////////// TIANMA Define by JHK ////////////////////////
#define nMAX(a, b)	(((a) > (b)) ? (a) : (b)) 
#define nMIN(a, b)	(((a) < (b)) ? (a) : (b))


enum ENUM_PARA_AVI_MURA_CHOLE
{
	E_PARA_AVI_MURA_CHOLE_DUST_TEXT = 0,										// 텍스트 표시
	E_PARA_AVI_MURA_CHOLE_DUST_BRIGHT_FLAG,										// 먼지 제거 사용
	E_PARA_AVI_MURA_CHOLE_DUST_DARK_FLAG,										// 먼지 제거 사용
	E_PARA_AVI_MURA_CHOLE_DUST_BIG_AREA,										// 큰 면적 불량만 남기기
	E_PARA_AVI_MURA_CHOLE_DUST_ADJUST_RANGE,									// 불량 주변으로 먼지가 있는 경우 불량 제거

	E_PARA_AVI_MURA_CHOLE_TEXT,													// TEXT
	E_PARA_AVI_MURA_CHOLE_ROI_X,									
	E_PARA_AVI_MURA_CHOLE_ROI_Y,

	E_PARA_AVI_MURA_CHOLE_RESIZEZOOM,
	
	E_PARA_AVI_MURA_CHOLE_GAUSS_SIZE,						
	E_PARA_AVI_MURA_CHOLE_GAUSS_SIGMA,

	E_PARA_AVI_MURA_CHOLE_CONTRAST_MAX,				
	E_PARA_AVI_MURA_CHOLE_CONTRAST_MIN,
	E_PARA_AVI_MURA_CHOLE_BRIGHTNESS_MAX,
	E_PARA_AVI_MURA_CHOLE_BRIGHTNESS_MIN,

	E_PARA_AVI_MURA_CHOLE_GAUSS_SIZE2,
	E_PARA_AVI_MURA_CHOLE_GAUSS_SIGMA2,

	E_PARA_AVI_MURA_CHOLE_THRESHOLD_RATIO_WHITE,									
	E_PARA_AVI_MURA_CHOLE_THRESHOLD_RATIO_BLACK,

	E_PARA_AVI_MURA_CHOLE_MORPHOLOGY,

	E_PARA_AVI_MURA_MURA_TOTAL_COUNT
};


class CInspectMuraChole
{
public:
	CInspectMuraChole(void);
	virtual ~CInspectMuraChole(void);

	// 메모리 관리
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
	//////////////////////////////////////////////////////////////////////////

	// Main 검사 알고리즘
	long		DoFindMuraDefect(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
		cv::Point* ptCorner, cv::Rect* rcCHoleROI, cv::Mat* matCholeBuffer,double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Mat& matDrawBuffer);

	// Dust 제거 후, 결과 벡터 넘겨주기
	long		GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
		double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);

protected:

	// 영상 저장 ( 8bit & 12bit )
	long		ImageSave(CString strPath, cv::Mat matSrcBuf);

	// Sub - Threshold ( 16bit )
	long		SubThreshold16(cv::Mat& matSrc1Buf, cv::Mat& matSrc2Buf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV);

	// Threshold ( 16bit )
	long		Threshold16(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV);
	long		Threshold16_INV(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV);

	// Pow ( 8bit & 12bit )
	long		Pow(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double dPow, int nMaxGV, CMatBuf* cMemSub = NULL);

	// 작은 면적 제거 & 근처 불량 잇기
	long		DeleteArea1(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub = NULL);

	// 작은 면적 제거 & 근처 불량 잇기
	long		DeleteArea2(cv::Mat& matSrcImage, int nCount, int nLength, CMatBuf* cMemSub = NULL);

	// 두개 영상 평균
	long		TwoImg_Average(cv::Mat matSrc1Buf, cv::Mat matSrc2Buf, cv::Mat &matDstBuf);

	// Dust 면적 큰 주변인 경우 제거
	long		DeleteCompareDust(cv::Mat& matSrcBuffer, int nOffset, stDefectInfo* pResultBlob, int nStartIndex, int nModePS);

	// 어두운 라인 불량 제거 ( 누기만 남기기 위함 )
	long		DeleteDarkLine(cv::Mat& matSrcBuffer, float fMajorAxisRatio, CMatBuf* cMemSub);

	// 가로 방향 Max GV 제한 : 명점 등등 밝은 불량 주변으로 검출되는 경우 발생 방지
	long		LimitMaxGV16X(cv::Mat& matSrcBuffer, float fOffset);

	// 회전 사각형 곂치는지(충돌) 확인
	bool		OrientedBoundingBox(cv::RotatedRect& rect1, cv::RotatedRect& rect2);



	//TIANMA ADD (CHOLE MURA AREA)
protected:
	//START Logic Alg
	long LogicStart_CholeMura(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, cv::Mat *matCholeBuffer, CRect rectROI, Rect* rcCHoleROI, double* dPara,
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);

	long	Pow(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double dPow);

	//VarianceFilter 추가
	void VarianceFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);
	void MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);

	//Line Mura
	void FunFilter(cv::Mat&, cv::Mat&, int, int);

	void FunWhiteMura(cv::Mat&, cv::Mat&, int, int, int);
	void FunBlackMura(cv::Mat&, cv::Mat&, int, int, int);

	void FunImageResize(cv::Mat&, long*, int, int, int, int, int);
	void FunLineMuraBlack(cv::Mat&, cv::Mat&, cv::Mat&, int, int, int, int, int, int);
	void FunLineMuraWhite(cv::Mat&, cv::Mat&, cv::Mat&, int, int, int, int, int, int);





	///////////////////////////////////////////////////////////////////////////

	void InspectionROI(cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat*, Rect&, CRect, Rect*, int, int, cv::Mat&, cv::Mat&);

	void Contrast(cv::Mat&, double, double, int, int);

	void ProfilingThreshold(cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, double, int);
};


