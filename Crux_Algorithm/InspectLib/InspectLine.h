
/************************************************************************
Line 알고리즘 관련 헤더
************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "MatBuf.h"				// 메모리 관리
#include "InspectLibLog.h"

//////////////////////////////////////////////////////////////////////////

// R, G, B
enum ENUM_PARA_LINE_RGB
{
	E_PARA_LINE_RGB_EMPTY_1					= 0	,	// 빈공간
	E_PARA_LINE_RGB_EMPTY_2						,	// 빈공간
	E_PARA_LINE_RGB_SHIFT_FLAG					,	// 사용 안함
	E_PARA_LINE_RGB_SHIFT_X						,	// 사용 안함
	E_PARA_LINE_RGB_SHIFT_Y						,	// 사용 안함
	E_PARA_LINE_RGB_TARGET_GV					,	// 원하는 밝기 맞추기
	E_PARA_LINE_RGB_BLUR_SIZE					,	// 노이즈 제거
	E_PARA_LINE_RGB_MEAN_FILTER_SIZE			,	// 방향성 Blur
	E_PARA_LINE_RGB_BG_FILTER_SIZE				,	// 방향성 반대로하여 배경 영상 만들기 ( 사이즈 크게 )
	E_PARA_LINE_RGB_IMAGE_FILTER_SIZE			,	// 방향성 반대로 사이즈 살짝 ( 노이즈 제거 )
	E_PARA_LINE_RGB_SEG_X						,	// 이진화 구역 설정
	E_PARA_LINE_RGB_SEG_Y						,	// 이진화 구역 설정
	E_PARA_LINE_RGB_OUTLINE						,	// 외곽선 제거 ( 어두운 라인만... )
	E_PARA_LINE_RGB_DARK_RATIO_X				,	// 평균 대비 어두운것
	E_PARA_LINE_RGB_BRIGHT_RATIO_X				,	// 평균 대비 밝은것
	E_PARA_LINE_RGB_DARK_RATIO_Y				,	// 사용 안함
	E_PARA_LINE_RGB_BRIGHT_RATIO_Y				,	// 사용 안함
	E_PARA_LINE_RGB_DELETE_AREA					,	// 면적 작은것 제거
	E_PARA_LINE_RGB_MORP_OPEN					,	// 라인 불량만 남기기
	E_PARA_LINE_RGB_MORP_CLOSE					,	// 라인 불량 이어 붙이기
	E_PARA_LINE_RGB_MORP_TEMP					,	// 팽창해서 강라인 주변 과검 불량 제거

	E_PARA_LINE_RGB_WEAK_TEXT					,	// 약라인 파라미터
	E_PARA_LINE_RGB_WEAK_FLAG					,	// 약라인 알고리즘 사용 유/무
	E_PARA_LINE_RGB_WEAK_RESIZE					,	// Gaussian Blur를 위한 영상 축소
	E_PARA_LINE_RGB_WEAK_BLUR					,	// 사용 안함
	E_PARA_LINE_RGB_WEAK_TARGET_GV2				,	// 사용 안함
	E_PARA_LINE_RGB_WEAK_OUTLINEBRIGHT			,	// BRIGHT 외곽 미검사
	E_PARA_LINE_RGB_WEAK_OUTLINEDARK			,	// DARK 외곽 미검사
	E_PARA_LINE_RGB_WEAK_TARGET_GV				,	// 영상 평균 GV
	E_PARA_LINE_RGB_WEAK_BRIGHT_RATIO_X			,	// 평균 대비 밝은것 X 방향
	E_PARA_LINE_RGB_WEAK_DARK_RATIO_X			,	// 평균 대비 어두운것 X 방향
	E_PARA_LINE_RGB_WEAK_BRIGHT_RATIO_Y			,	// 평균 대비 밝은것 Y 방향
	E_PARA_LINE_RGB_WEAK_DARK_RATIO_Y			,	// 평균 대비 어두운것 Y 방향
	E_PARA_LINE_RGB_WEAK_GAUSSIAN				,	// Noise 제거
	E_PARA_LINE_RGB_WEAK_PROJ_BLUR1				,	// 배경 설정을 위한 Projection Blur1
	E_PARA_LINE_RGB_WEAK_PROJ_BLUR2				,	// 배경 설정을 위한 Projection Blur2
	E_PARA_LINE_RGB_WEAK_PROJ_RANGE				,	// 배경 설정을 위한 Projection Max - Min 범위
	E_PARA_LINE_RGB_WEAK_PROJ_MORPHOLOGY		,	// Morphology Size 크기
	E_PARA_LINE_RGB_WEAK_PROJ_SIGAM				,	// Gaussian blur Sigma

	E_PARA_LINE_RGB_RANSAC_PROCESS_TEXT			,	// Text
	E_PARA_LINE_RGB_RANSAC_MAXFILTER_SIZE		,	// Max Filter Process Size
	E_PARA_LINE_RGB_RANSAC_AVG_TH_RATIO			,	// Max Filter Average Ratio
	E_PARA_LINE_RGB_RANSAC_TH_UNIT				,	// RANSAC Process Unit
	
	E_PARA_LINE_RGB_NORCH_PROCESS_TEXT			,	// Text
	E_PARA_LINE_RGB_NORCH_ONOFF					,	// On : 1 / Off : 0
	E_PARA_LINE_RGB_NORCH_MAXFILTER_SIZE		,   // Norch Process 시 Max Filter Size
	E_PARA_LINE_RGB_NORCH_AVGFILTER_SIZE		,   // Norch Process 시 Avg Filter Size
	E_PARA_LINE_RGB_NORCH_AVG_TH_RATIO			,	// Norch Average Ratio
	E_PARA_LINE_RGB_NORCH_MIN_GV				,	// Norch Min Data GV
	E_PARA_LINE_RGB_NORCH_INSP_RATIO_BRIGHT		,	// Insp. Ratio Bright
	E_PARA_LINE_RGB_NORCH_INSP_RATIO_DARK		,	// Insp. Ratio Dark

	E_PARA_LINE_RGB_TOTAL_COUNT						// Total
};

// Gray
enum ENUM_PARA_LINE_GRAY
{
	E_PARA_LINE_GRAY_EMPTY_1				= 0	,	// 빈공간
	E_PARA_LINE_GRAY_EMPTY_2					,	// 빈공간
	E_PARA_LINE_GRAY_SHIFT_FLAG					,	// 사용 안함
	E_PARA_LINE_GRAY_SHIFT_X					,	// 사용 안함
	E_PARA_LINE_GRAY_SHIFT_Y					,	// 사용 안함
	E_PARA_LINE_GRAY_TARGET_GV					,	// 원하는 밝기 맞추기
	E_PARA_LINE_GRAY_BLUR_SIZE					,	// 노이즈 제거
	E_PARA_LINE_GRAY_MEAN_FILTER_SIZE			,	// 방향성 Blur
	E_PARA_LINE_GRAY_BG_FILTER_SIZE				,	// 방향성 반대로하여 배경 영상 만들기 ( 사이즈 크게 )
	E_PARA_LINE_GRAY_IMAGE_FILTER_SIZE			,	// 방향성 반대로 사이즈 살짝 ( 노이즈 제거 )
	E_PARA_LINE_GRAY_SEG_X						,	// 이진화 구역 설정
	E_PARA_LINE_GRAY_SEG_Y						,	// 이진화 구역 설정
	E_PARA_LINE_GRAY_OUTLINE					,	// 외곽선 제거 ( 어두운 라인만... )
	E_PARA_LINE_GRAY_DARK_RATIO_X				,	// 평균 대비 어두운것
	E_PARA_LINE_GRAY_BRIGHT_RATIO_X				,	// 평균 대비 밝은것
	E_PARA_LINE_GRAY_DARK_RATIO_Y				,	// 사용 안함
	E_PARA_LINE_GRAY_BRIGHT_RATIO_Y				,	// 사용 안함
	E_PARA_LINE_GRAY_DELETE_AREA				,	// 면적 작은것 제거
	E_PARA_LINE_GRAY_MORP_OPEN					,	// 라인 불량만 남기기
	E_PARA_LINE_GRAY_MORP_CLOSE					,	// 라인 불량 이어 붙이기
	E_PARA_LINE_GRAY_MORP_TEMP					,	// 팽창해서 강라인 주변 과검 불량 제거

	E_PARA_LINE_GRAY_WEAK_TEXT					,	// 약라인 파라미터
	E_PARA_LINE_GRAY_WEAK_FLAG					,	// 약라인 알고리즘 사용 유/무
	E_PARA_LINE_GRAY_WEAK_RESIZE				,	// Gaussian Blur를 위한 영상 축소
	E_PARA_LINE_GRAY_WEAK_BLUR					,	// 사용 안함
	E_PARA_LINE_GRAY_WEAK_TARGET_GV2			,	// 사용 안함
	E_PARA_LINE_GRAY_WEAK_OUTLINEBRIGHT			,	// BRIGHT 외곽 미검사
	E_PARA_LINE_GRAY_WEAK_OUTLINEDARK			,	// DARK 외곽 미검사
	E_PARA_LINE_GRAY_WEAK_TARGET_GV				,	// 영상 평균 GV
	E_PARA_LINE_GRAY_WEAK_BRIGHT_RATIO_X		,	// 평균 대비 밝은것 X 방향
	E_PARA_LINE_GRAY_WEAK_DARK_RATIO_X			,	// 평균 대비 어두운것 X 방향
	E_PARA_LINE_GRAY_WEAK_BRIGHT_RATIO_Y		,	// 평균 대비 밝은것 Y 방향
	E_PARA_LINE_GRAY_WEAK_DARK_RATIO_Y			,	// 평균 대비 어두운것 Y 방향
	E_PARA_LINE_GRAY_WEAK_GAUSSIAN				,	// Noise 제거
	E_PARA_LINE_GRAY_WEAK_PROJ_BLUR1			,	// 배경 설정을 위한 Projection Blur1
	E_PARA_LINE_GRAY_WEAK_PROJ_BLUR2			,	// 배경 설정을 위한 Projection Blur2
	E_PARA_LINE_GRAY_WEAK_PROJ_RANGE			,	// 배경 설정을 위한 Projection Max - Min 범위
	E_PARA_LINE_GRAY_WEAK_PROJ_MORPHOLOGY		,	// Morphology Size 크기
	E_PARA_LINE_GRAY_WEAK_PROJ_SIGAM			,	// Gaussian blur Sigma

	E_PARA_LINE_GRAY_TOTAL_COUNT					// Total
};

// Black
enum ENUM_PARA_LINE_BLACK
{
	E_PARA_LINE_BLACK_WINDOW_SIZE		= 0	,	// 가우시안 필터 사이즈
	E_PARA_LINE_BLACK_SIGMA					,	// 가우시안 시그마
	E_PARA_LINE_BLACK_RESIZE				,	// 1/RESIZE 배로 영상 축소
	E_PARA_LINE_BLACK_THRESHOLD_RATIO		,	// 표준편차 * RATIO
	E_PARA_LINE_BLACK_OUTLINE				,	// 외곽 제거
	E_PARA_LINE_BLACK_ROTATION_FLAG			,	// 회전 On/Off
	E_PARA_LINE_BLACK_OFFSET				,	// 프로젝션 Sub 할때 범위
	E_PARA_LINE_BLACK_THRESHOLD_XY			,	// X선을 Y방향으로 프로젝션 -> 이진화 수치
	E_PARA_LINE_BLACK_THRESHOLD_XX			,	// X선을 X방향으로 프로젝션 -> 이진화 수치
	E_PARA_LINE_BLACK_THRESHOLD_YX			,	// Y선을 X방향으로 프로젝션 -> 이진화 수치
	E_PARA_LINE_BLACK_THRESHOLD_YY			,	// Y선을 Y방향으로 프로젝션 -> 이진화 수치
	E_PARA_LINE_BLACK_THICKNESS				,	// 선 이을때 사용 ( 현재 1 고정 )
	E_PARA_LINE_BLACK_PIXEL_DISTANCE		,	// 선 이을때 최대 떨어진 거리 ( Pixel )

	E_PARA_LINE_BLACK_TOTAL_COUNT				// Total
};

//////////////////////////////////////////////////////////////////////////

enum ENUM_MORP2
{
	EP_MORP_INIT		= 0	,
	EP_MORP_ERODE			,
	EP_MORP_DILATE			,
	EP_MORP_OPEN			,
	EP_MORP_CLOSE			,
	EP_MORP_GRADIENT		,
	EP_MORP_TOPHAT			,
	EP_MORP_BLACKHAT		,
};

enum ENUM_PROFILE
{
	E_PROFILE_ROW	= 0	,
	E_PROFILE_COL		
};

enum ENUM_WEAK_FILTER_DIRECTION
{
	E_WEAK_FILTER_X		= 0	,
	E_WEAK_FILTER_Y
};

enum ENUM_WEAK_DEFECT_TYPE
{
	E_WEAK_DEFECT_BRIGHT	= 0	,
	E_WEAK_DEFECT_DARK
};

enum ENUM_BINARY_IMAGE
{
	E_BINARY_BRIGHT_X	= 0	,
	E_BINARY_BRIGHT_Y		,
	E_BINARY_DARK_X			,
	E_BINARY_DARK_Y			,

	E_BINARY_TOTAL_COUNT
};

enum ENUM_BIT_TYPE
{
	E_BIT_8_IMAGE		= 0	,
	E_BIT_16_IMAGE		
};

#define _MIN_(a,b) ((a) < (b) ? (a) : (b)) 
#define _MAX_(a,b) ((a) > (b) ? (a) : (b)) 
#define DISTANCE1(x1, x2)			fabs(x2-x1)
#define DISTANCE2(x1, y1, x2, y2)	sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))

//////////////////////////////////////////////////////////////////////////

class CInspectLine
{
public:
	CInspectLine(void);
	virtual ~CInspectLine(void);

	// 메모리 관리
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};

	// 로그 관리
	InspectLibLog*		m_cInspectLibLog;
	clock_t				m_tInitTime;
	clock_t				m_tBeforeTime;
	wchar_t*			m_strAlgLog;

	void		SetLog(InspectLibLog* cLog, clock_t tTimeI, clock_t tTimeB, wchar_t* strLog)
	{
		m_tInitTime			= tTimeI;
		m_tBeforeTime		= tTimeB;
		m_cInspectLibLog	= cLog;
		m_strAlgLog			= strLog;
	};

	void		writeInspectLog(int nAlgType, char* strFunc, wchar_t* strTxt)
	{
		if( m_cInspectLibLog == NULL )
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, m_strAlgLog);
	};

	//////////////////////////////////////////////////////////////////////////

	void		writeInspectLog_Memory(int nAlgType, char* strFunc, wchar_t* strTxt, __int64 nMemory_Use_Value = 0)
	{
		if (m_cInspectLibLog == NULL)
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, nMemory_Use_Value, m_strAlgLog);
	};

///////////////////////////////////////////////////////////////////

	// Main 검사 알고리즘
	long		FindLineDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Mat& matBKBuffer,  vector<int> NorchIndex, CPoint OrgIndex, cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath,
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

protected:

	long CInspectLine::LogicStart_BlackWhite3(Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner);

	long CInspectLine::LogicStart_Crack(Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner);

	//RGB 패턴 검출 알고리즘
	long CInspectLine::LogicStart_RGB5(Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner );

protected:

	void ApplyMeanGV(Mat &matSrcImage, double dblTargetGV, CRect rectTemp);

	long GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto);

	int GetBitFromImageDepth(int nDepth);

	long RangeAvgThreshold_Gray(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, long nLoop, long nSegX, long nSegY, float fDarkRatio, float fBrightRatio, float fDarkRatio_Edge, float fBrightRatio_Edge, CMatBuf* cMemSub = NULL);

	void MakeProfile(int nType,Mat &matSrcImage,Mat &matDstProjection, CMatBuf* cMemSub = NULL); // 라인 프로파일을 만들어준다.

	void LineMeasurement(cv::Mat matSrcImage, cv::Mat& matDstImage, Mat* matProjection, double* dPara, CRect rectROI, int nDirection, int nOutLine, CMatBuf* cMemSub = NULL);

	void Sdtthreshold(cv::Mat matSrcImage, cv::Mat& matDstImage, float fThresholdRatio);

	void CInspectLine::filter8(BYTE* pbyInImg, BYTE* pbyOutImg, int nMin, int nMax, int nWidth, int nHeight); //choikwangil

	//////////////////////////////////////////////////////////////////////////
	// KSW 17.09.29 - 방향별 라인
	long	calcLine_BrightX(cv::Mat& matSrcImage, cv::Mat& matDstImage, cv::Mat& matTempBuf1, cv::Mat& matTempBuf2, CRect rectResize, double* dPara, int* nCommonPara, CString strAlgPath);
	long	calcLine_BrightY(cv::Mat& matSrcImage, cv::Mat& matDstImage, cv::Mat& matTempBuf1, cv::Mat& matTempBuf2, CRect rectResize, double* dPara, int* nCommonPara, CString strAlgPath);
	long	calcLine_DarkX	(cv::Mat& matSrcImage, cv::Mat& matDstImage, cv::Mat& matTempBuf1, cv::Mat& matTempBuf2, CRect rectResize, double* dPara, int* nCommonPara, CString strAlgPath);
	long	calcLine_DarkY	(cv::Mat& matSrcImage, cv::Mat& matDstImage, cv::Mat& matTempBuf1, cv::Mat& matTempBuf2, CRect rectResize, double* dPara, int* nCommonPara, CString strAlgPath);

	// KSW 17.10.02
	long	calcRGBMain(cv::Mat& matSrcImage, cv::Mat& matThImage, cv::Mat* matDstImage, cv::Mat* matBinaryMorp, double* dPara, int* nCommonPara,
		CRect rectResize, cv::Point* ptCorner, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, CMatBuf* cMemSub=NULL);

	//////////////////////////////////////////////////////////////////////////

	long	LogicStart_Weak(cv::Mat& matSrcImage, cv::Mat* matDstImage, vector<int> NorchIndex, CPoint OrgIndex, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner);

	///////////////////////////////////////////////////////////////////////////
	// YDS 18.08.09 약라인 알고리즘
	long	Image_Pow(int ImgType, double dpow, Mat&InPutImage, Mat&OutPutImage);

	long	resizeGaussian(Mat&InPutImage, Mat&OutPutImage, int reSizeValue, int MaskSize, double dSigma, CMatBuf* cMemSub = NULL);

	long	calcWeakLine_Y(Mat& InPutImage, Mat& OutPutImage1, Mat& OutPutImage2, Mat* matProjectionY, vector<int> NorchIndex, CPoint OrgIndex, CRect rectROI, double dbThresholdBY, double dbThresholdDY, int nBlur, int nBlur2, int nRange, int nMorp, int nOutLineBright, int nOutLineDark, int* nCommonPara, CString strAlgPath, double* dPara);
	
	long	calcWeakLine_X(Mat& InPutImage, Mat& OutPutImage1, Mat& OutPutImage2, Mat* matProjectionX, CRect rectROI, double dbThresholdBY, double dbThresholdDY, int nBlur, int nBlur2, int nRange, int nMorp, int nOutLineBright, int nOutLineDark, int* nCommonPara, CString strAlgPath, double* dPara);
	
	long	MakeProjection(Mat& InPutImage, Mat&MatproYBuf, int width, int height, int Type, int UseNorch, int NorchUnit, vector<int> NorchIndex, CPoint OrgIndex);
	
	long	calcProjection(Mat& MatproSrc, Mat& MatproYDst, Mat* matProjection, int size, int nBlur, int nBlur2, int nRange, int nMorp, int Type, int* nCommonPara, CString strAlgPath);
	
	long	calcWeakLine_BrigtY(Mat& MatproSrc, Mat& MatproDst, Mat* matProjectionY, Mat& OutPutImage, CRect rectROI, int nNorchUnit, int size,  double dbThresholdBY, int nOutLineBright, int* nCommonPara, CString strAlgPath, double* dPara);
	
	long	calcWeakLine_DarkY(Mat& MatproSrc, Mat& MatproDst, Mat* matProjectionY, Mat& OutPutImage, CRect rectROI, int nNorchUnit, int size,  double dbThresholdDY, int nOutLineDark, int* nCommonPara, CString strAlgPath, double* dPara);
	
	long	calcWeakLine_BrigtX(Mat& MatproSrc, Mat& MatproDst, Mat* matProjectionX, Mat& OutPutImage, CRect rectROI, int size,  double dbThresholdBX, int nOutLineBright, int* nCommonPara, CString strAlgPath, double* dPara);
	
	long	calcWeakLine_DarkX(Mat& MatproSrc, Mat& MatproDst, Mat* matProjectionX, Mat& OutPutImage, CRect rectROI, int size,  double dbThresholdDX, int nOutLineDark, int* nCommonPara, CString strAlgPath, double* dPara);
	
	long	RangeThreshold_Weak(Mat& MatproSub, Mat& OutPutImage1, CRect rectROI, int size, double dbThreshold, int nOutLine, int Type, int* nCommonPara, CString strAlgPath);
	
	///////////////////////////////////////////////////////////////////////////
	// RANSAC Process

	long	RangRANSACProcess(Mat& MatproSub, Mat& MatSubRANSAC, int size, int* nCommonPara, CString strAlgPath, double* dPara);

	long	ProfileMaxFilter(Mat& MatproSub, Mat& MatSubRANSAC,  int size, int* nCommonPara, CString strAlgPath, double* dPara, int nOutLine);

	long	NorchValueProcess(Mat& MatproSub, Mat& MatNorchMaxADD, int size, int* nCommonPara, CString strAlgPath, double* dPara, int nOutLine, int nNorchUnit);

	long	RRM_Thresholding(Mat& MatproSub, Mat& MatSubRANSAC, Mat& OutPutImage, CRect rectROI, int nNorchUnit,	int size, double dbThreshold, int nOutLine, int Type, int* nCommonPara, CString strAlgPath, double* dPara, double dInspRatio);

	bool	FindInSamples(vector <cv::Point2i>& ptSamples, cv::Point2i ptIndexs);

	long	GetRandomSamples(vector <cv::Point2i>& ptSrcIndexs, vector <cv::Point2i>& ptSamples, int nSampleCount);

	long	calcLineFit(vector <cv::Point2i>& ptSamples, long double& dA, long double& dB);

	long	calcLineVerification(vector <cv::Point2i>& ptSrcIndexs, vector <cv::Point2i>& ptInliers, long double& dA, long double& dB, double distThreshold);

	long	calcRANSAC(vector <cv::Point2i>& ptSrcIndexs, long double& dA, long double& dB);

	long	ImageSave(CString strPath, cv::Mat matSrcBuf);

	//////////////////////////////////////////////////////////////////////////
	//히스토그램으로 평균
	long GetAverage_From_Histo(cv::Mat matHisto,int nMin,int nMax,double &dblAverage);

	//히스토그램으로 표준편차
	long GetMeanStdDev_From_Histo(cv::Mat matHisto,int nMin,int nMax,double &dblAverage,double &dblStdev);

	//////////////////////////////////////////////////////////////////////////
	// 
	void		Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset = 0);

protected:

	// preprocess
	long Morphology(Mat& matSrcImage, Mat& matDstImage, long nSizeX, long nSizeY, int nOperation, CMatBuf* cMemSub = NULL, int nIter=1);

	bool m_bProcess;
};