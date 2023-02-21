#pragma once

#include "Define.h"
#include "MatBuf.h"				// 메모리 관리
#include "InspectLibLog.h"

using namespace std;
using namespace cv;

#define  ROI_ACTIVE_BURNT_1 _T("ACTIVE_BURNT_1.bmp")
#define  ROI_ACTIVE_BURNT_2 _T("ACTIVE_BURNT_2.bmp")
#define  ROI_ACTIVE_BURNT_3 _T("ACTIVE_BURNT_3.bmp")


#define INSPECTION_ACTIVE_GAUSSIAN _T("%s\\%d_Active_Gaussian.jpg")
#define INSPECTION_ACTIVE_GAUSSIAN_RESIZE _T("%s\\%d_Active_Gaussian_Resize.bmp")

#define INSPECTION_ACTIVE_EDGE_IMAGE _T("%s\\%d_Acitve_Edge_Image.jpg")
#define INSPECTION_ACTIVE_EDGE_CLOSING_IMAGE _T("%s\\%d_Acitve_Edge_Close_Image.jpg")
#define INSPECTION_ACTIVE_EDGE_CONVEXHULL _T("%s\\%d_Acitve_Edge_Convexhull.jpg")

#define INSPECTION_ACTIVE_ROUNDMASK _T("%s\\%d_RoundMask.jpg")
#define INSPECTION_ACTIVE_ROUNDMASK_EDGE_IMAGE _T("%s\\%d_Acitve_Mask_Edge_Image.jpg")

#define INSPECTION_ACTIVE_FILMPEELING _T("%s\\%d_Acitve_FilmPeeling_Image.jpg")

#define INSPECTION_ACTIVE_BUBBLE _T("%s\\%d_Acitve__Bubble_Image.jpg")
#define INSPECTION_ACTIVE_BUBBLE_SUBSTANCE _T("%s\\%d_Acitve__BubbleSubstance_Image.jpg")
#define INSPECTION_ACTIVE_SUBSTANCE _T("%s\\%d_Acitve__Substance_Image.jpg")

#define INSPECTION_ACTIVE_CHECKHILL _T("%s\\%d_Check_Hill.jpg")

#define INSPECTION_ACTIVE_FEATURELABEL _T("%s\\%d_Feature_label.jpg")

#define INSPECTION_ACTIVE_DRAW _T("%s\\%d_Acitve_Draw_Image.jpg")


#define	IMG_DIVIDE_NUM_AABURNT	4

class CInspectActive
{

	//After
private:

	enum {ACTIVE_TOP, ACTIVE_LEFT, ACTIVE_BOTTOM, ACTIVE_NUM};


	enum INSPECTION_PARAM
	{
		INSPECTION_PARAM_SCALE = 0,
		INSPECTION_PARAM_BLUR_SIZE,
		INSPECTION_PARAM_BLUR_SIGMA,
		INSPECTION_PARAM_ROUND_DILATE,
		INSPECTION_PARAM_ROUND_ERODE,
		INSPECTION_PARAM_ROUND_CELL,
		INSPECTION_PARAM_EDGE_THRESHOLD,
		INSPECTION_PARAM_EDGE_DILATE,
		INSPECTION_PARAM_EDGE_ERODE,
		INSPECTION_PARAM_FAULTY_SIZE,
		INSPECTION_PARAM_HULL_BOUND_OUTSIDE,
		INSPECTION_PARAM_HILL_SIZE,
		INSPECTION_PARAM_HILL_CENTER_DISTANCE,
		INSPECTION_PARAM_HILL_STANDARD_DEVIATION,
		INSPECTION_PARAM_AXIS_RATIO,
		INSPECTION_PARAM_MIN_SUBATANCE,
		INSPECTION_PARAM_MAX_SUBATANCE,
		INSPECTION_PARAM_BUBBLE_DUST_MINAREA,
		INSPECTION_PARAM_BUBBLE_DUST_THRESHOLD,
		INSPECTION_PARAM_BAD_AREA_SIZE,
		INSPECTION_PARAM_BAD_GRAY_SIZE,
		INSPECTION_PARAM_NONE_INSPECTION_L,
		INSPECTION_PARAM_NONE_INSPECTION_T,
		INSPECTION_PARAM_NONE_INSPECTION_R,
		INSPECTION_PARAM_NONE_INSPECTION_B
	};
	
	enum ACTIVE_IMAGE_NUM
	{
		ACTIVE_IMAGE_ALL = 0,
		ACTIVE_IMAGE_RIGHT,
		ACTIVE_IMAGE_LEFT,
		ACTIVE_IMAGE_BOTTOM,
		ACTIVE_IMAGE_TOP,
		MAX_ACTIVE_IMAGE_NUM
	};

	enum DIRECTION{
		LT,
		RT,
		LB,
		RB
	};

	enum DEFECT_TYPE{
		E_BLACK,
		E_WHITE,
		E_NONE_TYPE
	};
	
	enum ACTIVE_DEFECT {					  
		E_Bubble			,
		E_Substance			,
		E_BubbleSubstance	,
		E_Scratch			,
		E_MURA				,
		E_HILL				,
		E_FilmPeeling		,
		E_NONE_NAME			,
		E_ACTIVE_DEFECT_NUM
	};

	enum LINE_DIRECTION
	{
		E_VERTICAL			,
		E_HORIZONTAL		,
		E_LINE_DIRECTION_NUM		
	};

	/*typedef struct{
		double STD;
		double Perimeter;
		double Rougness;
		double Compactness;
	}Item;*/
	typedef struct{
		std::vector<cv::Point> contour;
		cv::Point2f center;
		/*cv::Scalar mean;
		cv::Scalar std;*/
		cv::Rect rect;
		cv::RotatedRect rotated_rect;

		double area;
		double length;
		double mean;
		double std;
		double perimeter;
		double rougness;
		double compactness;
		double min;
		double max;
		double closedValueAvg;
		double closedValueSTD;
		double subDimension;
		double MaxSubClosedIntensity;
		double MinSubClosedIntensity;
		double centerDistance;
		double wSum;
		double Axis_Ratio;

		int name;
		int type; //balck: 0, white: 1
		int conNum;
		double conAng;
		int conCount;
		bool unUse;		
	}DEFECTINFO;

	typedef vector<DEFECTINFO>	VectorDefectInfo;
	typedef VectorDefectInfo::iterator	vectordefectinfo_it;

	Mat m_mtDrawHill[MAX_ACTIVE_IMAGE_NUM];	//흡착 돌기 Check용 영상

public:
	CInspectActive(void);
	~CInspectActive(void);

private:

	__inline double getDistance(cv::Point p1, cv::Point p2)
	{
		double distance = sqrt( pow((double)(p1.x - p2.x) ,2) + pow((double)(p1.y - p2.y) ,2));
		return distance;
	}

	//Before
	long ImagePreprecessing(Mat matSrc, Mat& MatDes);
	long FindBubbleDefect(Mat matSrc, Mat& matDrawBuffer, int nMinBubbleSize, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);		// 기포
	long FindDent_Top(Mat matSrc, Mat& matDrawBuffer, double* dPara, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);			// 상부 찍힘
	long FindDent_Bottom(Mat matSrc, Mat& matDrawBuffer, double* dPara, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);		// 하부 찍힘
	long FindScratchDefect(Mat matSrc, Mat& matDrawBuffer, double* dPara, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);		// 스크래치
	long FindPeelingefect(Mat matSrc, Mat& matDrawBuffer, double* dPara, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);	// 탈막

	//After
	bool FindPolynomial(cv::Mat img, cv::Mat &R, int width, const int sample);
	bool FindPolynomial_H(cv::Mat img, cv::Mat &R, int height, const int nSample);
	bool SubPoly (cv::Mat src, cv::Mat dst, int nLineDirection);
	double getAngle(cv::Point p1, cv::Point p2);
	

	bool GetRoundCellArea(cv::Mat Img, vector<vector<cv::Point>>* ptRoundArea, float* fThreshold_Per, int nCornerROISize, Scalar* scMean, Scalar* scSTDEV);

	bool DefectCategorization(vector<DEFECTINFO> &vDefect, vector<vector<cv::Point>> contours, int type, int name);
	bool DefectClassification(vector<DEFECTINFO> &vDefect, cv::Mat temp, int mXCenter, int mYCenter);

	long Bright_Adjust(cv::Mat* mtImg, int nImageNum);
	int GetBitFromImageDepth(int nDepth);
	long GetHistogram(Mat& matSrcImage, Mat& matHisto);
	bool GetAvgOfStdevRange(Mat matHisto, Scalar Avg, Scalar Stdev, double& dReturnAvg);

	/*double getClosedValue(cv::Mat src, cv::Point center, double radius);
	double getClosedSTD(cv::Mat src, cv::Point center, double radius);*/
	bool getClosedValue(cv::Mat src, cv::Point center, double radius, double value[3]);

	bool CheckHill(cv::Mat src, DEFECTINFO & defect, int nSize, float fSTDVal, float fCenterDistance, int nImageInx, bool bImageSave = false);
	bool CheckHill(cv::Mat src, vector<DEFECTINFO> &vDefect, int nDefectInx, int nSize, float fSTDVal, float fCenterDistance, int nImageInx, bool bImageSave = false);
	cv::Rect GetNoneInspROI(int* nNoneInspArea, cv::Rect rtOrg, float fScale);

	//Active Feature Save
	bool WriteResultFile(CString strFilePath, CString strFileName, CString strColumn, TCHAR* strResult);
	
	void InspectionStepImage(cv::Mat matImage, bool bImageSave, CString strStepName, wchar_t * strAlgPath, int nImageNum);

public :

	// 메모리 관리
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};

	//Active Inspection
	long FindActiveDefect(Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

	long FindActiveDefect2(Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Rect rtInspArea, double* dParam, int* nCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, vector<vector<Point2i>> ptRound, wchar_t* strContourTxt = NULL);

	long FindActiveBurnt(Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, stActiveBurntROI* stROI, CString strCurRecipePath, wchar_t* strContourTxt = NULL);

	long FindActiveDust(Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

	void GammaCorrection(	Mat matSrcBuffer,Mat matThrBuffer, float fGamma );

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
};

