
/************************************************************************
Line �˰��� ���� ���
************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "MatBuf.h"				// �޸� ����
#include "InspectLibLog.h"

//////////////////////////////////////////////////////////////////////////

// R, G, B
enum ENUM_PARA_LINE_RGB
{
	E_PARA_LINE_RGB_EMPTY_1					= 0	,	// �����
	E_PARA_LINE_RGB_EMPTY_2						,	// �����
	E_PARA_LINE_RGB_SHIFT_FLAG					,	// ��� ����
	E_PARA_LINE_RGB_SHIFT_X						,	// ��� ����
	E_PARA_LINE_RGB_SHIFT_Y						,	// ��� ����
	E_PARA_LINE_RGB_TARGET_GV					,	// ���ϴ� ��� ���߱�
	E_PARA_LINE_RGB_BLUR_SIZE					,	// ������ ����
	E_PARA_LINE_RGB_MEAN_FILTER_SIZE			,	// ���⼺ Blur
	E_PARA_LINE_RGB_BG_FILTER_SIZE				,	// ���⼺ �ݴ���Ͽ� ��� ���� ����� ( ������ ũ�� )
	E_PARA_LINE_RGB_IMAGE_FILTER_SIZE			,	// ���⼺ �ݴ�� ������ ��¦ ( ������ ���� )
	E_PARA_LINE_RGB_SEG_X						,	// ����ȭ ���� ����
	E_PARA_LINE_RGB_SEG_Y						,	// ����ȭ ���� ����
	E_PARA_LINE_RGB_OUTLINE						,	// �ܰ��� ���� ( ��ο� ���θ�... )
	E_PARA_LINE_RGB_DARK_RATIO_X				,	// ��� ��� ��ο��
	E_PARA_LINE_RGB_BRIGHT_RATIO_X				,	// ��� ��� ������
	E_PARA_LINE_RGB_DARK_RATIO_Y				,	// ��� ����
	E_PARA_LINE_RGB_BRIGHT_RATIO_Y				,	// ��� ����
	E_PARA_LINE_RGB_DELETE_AREA					,	// ���� ������ ����
	E_PARA_LINE_RGB_MORP_OPEN					,	// ���� �ҷ��� �����
	E_PARA_LINE_RGB_MORP_CLOSE					,	// ���� �ҷ� �̾� ���̱�
	E_PARA_LINE_RGB_MORP_TEMP					,	// ��â�ؼ� ������ �ֺ� ���� �ҷ� ����

	E_PARA_LINE_RGB_WEAK_TEXT					,	// ����� �Ķ����
	E_PARA_LINE_RGB_WEAK_FLAG					,	// ����� �˰��� ��� ��/��
	E_PARA_LINE_RGB_WEAK_RESIZE					,	// Gaussian Blur�� ���� ���� ���
	E_PARA_LINE_RGB_WEAK_BLUR					,	// ��� ����
	E_PARA_LINE_RGB_WEAK_TARGET_GV2				,	// ��� ����
	E_PARA_LINE_RGB_WEAK_OUTLINEBRIGHT			,	// BRIGHT �ܰ� �̰˻�
	E_PARA_LINE_RGB_WEAK_OUTLINEDARK			,	// DARK �ܰ� �̰˻�
	E_PARA_LINE_RGB_WEAK_TARGET_GV				,	// ���� ��� GV
	E_PARA_LINE_RGB_WEAK_BRIGHT_RATIO_X			,	// ��� ��� ������ X ����
	E_PARA_LINE_RGB_WEAK_DARK_RATIO_X			,	// ��� ��� ��ο�� X ����
	E_PARA_LINE_RGB_WEAK_BRIGHT_RATIO_Y			,	// ��� ��� ������ Y ����
	E_PARA_LINE_RGB_WEAK_DARK_RATIO_Y			,	// ��� ��� ��ο�� Y ����
	E_PARA_LINE_RGB_WEAK_GAUSSIAN				,	// Noise ����
	E_PARA_LINE_RGB_WEAK_PROJ_BLUR1				,	// ��� ������ ���� Projection Blur1
	E_PARA_LINE_RGB_WEAK_PROJ_BLUR2				,	// ��� ������ ���� Projection Blur2
	E_PARA_LINE_RGB_WEAK_PROJ_RANGE				,	// ��� ������ ���� Projection Max - Min ����
	E_PARA_LINE_RGB_WEAK_PROJ_MORPHOLOGY		,	// Morphology Size ũ��
	E_PARA_LINE_RGB_WEAK_PROJ_SIGAM				,	// Gaussian blur Sigma

	E_PARA_LINE_RGB_RANSAC_PROCESS_TEXT			,	// Text
	E_PARA_LINE_RGB_RANSAC_MAXFILTER_SIZE		,	// Max Filter Process Size
	E_PARA_LINE_RGB_RANSAC_AVG_TH_RATIO			,	// Max Filter Average Ratio
	E_PARA_LINE_RGB_RANSAC_TH_UNIT				,	// RANSAC Process Unit
	
	E_PARA_LINE_RGB_NORCH_PROCESS_TEXT			,	// Text
	E_PARA_LINE_RGB_NORCH_ONOFF					,	// On : 1 / Off : 0
	E_PARA_LINE_RGB_NORCH_MAXFILTER_SIZE		,   // Norch Process �� Max Filter Size
	E_PARA_LINE_RGB_NORCH_AVGFILTER_SIZE		,   // Norch Process �� Avg Filter Size
	E_PARA_LINE_RGB_NORCH_AVG_TH_RATIO			,	// Norch Average Ratio
	E_PARA_LINE_RGB_NORCH_MIN_GV				,	// Norch Min Data GV
	E_PARA_LINE_RGB_NORCH_INSP_RATIO_BRIGHT		,	// Insp. Ratio Bright
	E_PARA_LINE_RGB_NORCH_INSP_RATIO_DARK		,	// Insp. Ratio Dark

	E_PARA_LINE_RGB_TOTAL_COUNT						// Total
};

// Gray
enum ENUM_PARA_LINE_GRAY
{
	E_PARA_LINE_GRAY_EMPTY_1				= 0	,	// �����
	E_PARA_LINE_GRAY_EMPTY_2					,	// �����
	E_PARA_LINE_GRAY_SHIFT_FLAG					,	// ��� ����
	E_PARA_LINE_GRAY_SHIFT_X					,	// ��� ����
	E_PARA_LINE_GRAY_SHIFT_Y					,	// ��� ����
	E_PARA_LINE_GRAY_TARGET_GV					,	// ���ϴ� ��� ���߱�
	E_PARA_LINE_GRAY_BLUR_SIZE					,	// ������ ����
	E_PARA_LINE_GRAY_MEAN_FILTER_SIZE			,	// ���⼺ Blur
	E_PARA_LINE_GRAY_BG_FILTER_SIZE				,	// ���⼺ �ݴ���Ͽ� ��� ���� ����� ( ������ ũ�� )
	E_PARA_LINE_GRAY_IMAGE_FILTER_SIZE			,	// ���⼺ �ݴ�� ������ ��¦ ( ������ ���� )
	E_PARA_LINE_GRAY_SEG_X						,	// ����ȭ ���� ����
	E_PARA_LINE_GRAY_SEG_Y						,	// ����ȭ ���� ����
	E_PARA_LINE_GRAY_OUTLINE					,	// �ܰ��� ���� ( ��ο� ���θ�... )
	E_PARA_LINE_GRAY_DARK_RATIO_X				,	// ��� ��� ��ο��
	E_PARA_LINE_GRAY_BRIGHT_RATIO_X				,	// ��� ��� ������
	E_PARA_LINE_GRAY_DARK_RATIO_Y				,	// ��� ����
	E_PARA_LINE_GRAY_BRIGHT_RATIO_Y				,	// ��� ����
	E_PARA_LINE_GRAY_DELETE_AREA				,	// ���� ������ ����
	E_PARA_LINE_GRAY_MORP_OPEN					,	// ���� �ҷ��� �����
	E_PARA_LINE_GRAY_MORP_CLOSE					,	// ���� �ҷ� �̾� ���̱�
	E_PARA_LINE_GRAY_MORP_TEMP					,	// ��â�ؼ� ������ �ֺ� ���� �ҷ� ����

	E_PARA_LINE_GRAY_WEAK_TEXT					,	// ����� �Ķ����
	E_PARA_LINE_GRAY_WEAK_FLAG					,	// ����� �˰��� ��� ��/��
	E_PARA_LINE_GRAY_WEAK_RESIZE				,	// Gaussian Blur�� ���� ���� ���
	E_PARA_LINE_GRAY_WEAK_BLUR					,	// ��� ����
	E_PARA_LINE_GRAY_WEAK_TARGET_GV2			,	// ��� ����
	E_PARA_LINE_GRAY_WEAK_OUTLINEBRIGHT			,	// BRIGHT �ܰ� �̰˻�
	E_PARA_LINE_GRAY_WEAK_OUTLINEDARK			,	// DARK �ܰ� �̰˻�
	E_PARA_LINE_GRAY_WEAK_TARGET_GV				,	// ���� ��� GV
	E_PARA_LINE_GRAY_WEAK_BRIGHT_RATIO_X		,	// ��� ��� ������ X ����
	E_PARA_LINE_GRAY_WEAK_DARK_RATIO_X			,	// ��� ��� ��ο�� X ����
	E_PARA_LINE_GRAY_WEAK_BRIGHT_RATIO_Y		,	// ��� ��� ������ Y ����
	E_PARA_LINE_GRAY_WEAK_DARK_RATIO_Y			,	// ��� ��� ��ο�� Y ����
	E_PARA_LINE_GRAY_WEAK_GAUSSIAN				,	// Noise ����
	E_PARA_LINE_GRAY_WEAK_PROJ_BLUR1			,	// ��� ������ ���� Projection Blur1
	E_PARA_LINE_GRAY_WEAK_PROJ_BLUR2			,	// ��� ������ ���� Projection Blur2
	E_PARA_LINE_GRAY_WEAK_PROJ_RANGE			,	// ��� ������ ���� Projection Max - Min ����
	E_PARA_LINE_GRAY_WEAK_PROJ_MORPHOLOGY		,	// Morphology Size ũ��
	E_PARA_LINE_GRAY_WEAK_PROJ_SIGAM			,	// Gaussian blur Sigma

	E_PARA_LINE_GRAY_TOTAL_COUNT					// Total
};

// Black
enum ENUM_PARA_LINE_BLACK
{
	E_PARA_LINE_BLACK_WINDOW_SIZE		= 0	,	// ����þ� ���� ������
	E_PARA_LINE_BLACK_SIGMA					,	// ����þ� �ñ׸�
	E_PARA_LINE_BLACK_RESIZE				,	// 1/RESIZE ��� ���� ���
	E_PARA_LINE_BLACK_THRESHOLD_RATIO		,	// ǥ������ * RATIO
	E_PARA_LINE_BLACK_OUTLINE				,	// �ܰ� ����
	E_PARA_LINE_BLACK_ROTATION_FLAG			,	// ȸ�� On/Off
	E_PARA_LINE_BLACK_OFFSET				,	// �������� Sub �Ҷ� ����
	E_PARA_LINE_BLACK_THRESHOLD_XY			,	// X���� Y�������� �������� -> ����ȭ ��ġ
	E_PARA_LINE_BLACK_THRESHOLD_XX			,	// X���� X�������� �������� -> ����ȭ ��ġ
	E_PARA_LINE_BLACK_THRESHOLD_YX			,	// Y���� X�������� �������� -> ����ȭ ��ġ
	E_PARA_LINE_BLACK_THRESHOLD_YY			,	// Y���� Y�������� �������� -> ����ȭ ��ġ
	E_PARA_LINE_BLACK_THICKNESS				,	// �� ������ ��� ( ���� 1 ���� )
	E_PARA_LINE_BLACK_PIXEL_DISTANCE		,	// �� ������ �ִ� ������ �Ÿ� ( Pixel )

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

	// �޸� ����
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};

	// �α� ����
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

	// Main �˻� �˰���
	long		FindLineDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Mat& matBKBuffer,  vector<int> NorchIndex, CPoint OrgIndex, cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath,
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

protected:

	long CInspectLine::LogicStart_BlackWhite3(Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner);

	long CInspectLine::LogicStart_Crack(Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner);

	//RGB ���� ���� �˰���
	long CInspectLine::LogicStart_RGB5(Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner );

protected:

	void ApplyMeanGV(Mat &matSrcImage, double dblTargetGV, CRect rectTemp);

	long GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto);

	int GetBitFromImageDepth(int nDepth);

	long RangeAvgThreshold_Gray(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, long nLoop, long nSegX, long nSegY, float fDarkRatio, float fBrightRatio, float fDarkRatio_Edge, float fBrightRatio_Edge, CMatBuf* cMemSub = NULL);

	void MakeProfile(int nType,Mat &matSrcImage,Mat &matDstProjection, CMatBuf* cMemSub = NULL); // ���� ���������� ������ش�.

	void LineMeasurement(cv::Mat matSrcImage, cv::Mat& matDstImage, Mat* matProjection, double* dPara, CRect rectROI, int nDirection, int nOutLine, CMatBuf* cMemSub = NULL);

	void Sdtthreshold(cv::Mat matSrcImage, cv::Mat& matDstImage, float fThresholdRatio);

	void CInspectLine::filter8(BYTE* pbyInImg, BYTE* pbyOutImg, int nMin, int nMax, int nWidth, int nHeight); //choikwangil

	//////////////////////////////////////////////////////////////////////////
	// KSW 17.09.29 - ���⺰ ����
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
	// YDS 18.08.09 ����� �˰���
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
	//������׷����� ���
	long GetAverage_From_Histo(cv::Mat matHisto,int nMin,int nMax,double &dblAverage);

	//������׷����� ǥ������
	long GetMeanStdDev_From_Histo(cv::Mat matHisto,int nMin,int nMax,double &dblAverage,double &dblStdev);

	//////////////////////////////////////////////////////////////////////////
	// 
	void		Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset = 0);

protected:

	// preprocess
	long Morphology(Mat& matSrcImage, Mat& matDstImage, long nSizeX, long nSizeY, int nOperation, CMatBuf* cMemSub = NULL, int nIter=1);

	bool m_bProcess;
};