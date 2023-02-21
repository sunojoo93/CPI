
#pragma once

#include "InspectLibLog.h"

class CInspectEdge
{
	enum{	BROKEN, BURR };
	enum{	LEFT, TOP, RIGHT, BOTTOM, LINE_MAX};						//Line
	enum{	EDGE_BROKEN_LEFT,	EDGE_BROKEN_TOP,	EDGE_BROKEN_RIGTHT,	EDGE_BROKEN_BOTTOM,
			EDGE_BURR_LEFT,		EDGE_BURR_TOP,		EDGE_BURR_RIGTHT,	EDGE_BURR_BOTTOM	};

	enum C_CUT_MARK				//C-CUT Line 옆의 사각 Mark
	{
		C_CUT_MARK_TOP=0,
		C_CUT_MARK_BOTTOM,
		C_CUT_MARK_MAX
	};

	struct LINE{
		double A;
		double B;
	};
 
	struct CCUTInfo {
		int nCCutNum;														//C-Cut의 갯수
		cv::Point	ptPanelCorner[E_CORNER_END];							//Panel 모서리 좌표
		cv::Point	ptMarkOffset_Distance[E_CORNER_END][C_CUT_MARK_MAX];	//Panel의 모서리로 부터의 C-Cut Mark 거리
		cv::Rect	rtROISize[E_CORNER_END];								//C-Cut처리 할때의 ROI Size								
		cv::Rect	rtMarkROISize[E_CORNER_END];							//Mark Search 할때 사용될 ROI의 Size(ROI= Size X Size)
// 		CvPoint cCutLength	[E_CORNER_END];
		cv::Point ptMark[E_CORNER_END][C_CUT_MARK_MAX];
 		cv::Point cCut_start[E_CORNER_END];									//C-Cut Mark에서 수직인선을 그어 찾은 C-Cut Line 시작점							
 		cv::Point cCut_end	[E_CORNER_END];									//										     끝점
// 		LINE	cCutLine	[E_CORNER_END];

		 bool  GetUse(int nPos) const
		{
// 			if(cCutLength[nPos].x > 0 && cCutLength[nPos].y > 0)
// 				return true;
// 			else
// 				return false;
			return true;
		}
	};


private:

	//RANSAC
	bool FindInSamples(std::vector <cv::Point2i>& ptSamples, cv::Point2i ptIndexs);
	bool FindInSamples(std::vector <cv::Point2f>& ptSamples, cv::Point2f ptIndexs);
	long GetRandomSamples(std::vector <cv::Point2i>& ptSrcIndexs, std::vector <cv::Point2i>& ptSamples, int nSampleCount);
	long GetRandomSamples(std::vector <cv::Point2f>& ptSrcIndexs, std::vector <cv::Point2f>& ptSamples, int nSampleCount);
	long calcLineFit(std::vector <cv::Point2i>& ptSamples, long double& dA, long double& dB);
	long calcLineFit(std::vector <cv::Point2f>& ptSamples, long double& dA, long double& dB);
	long calcCurveFit(std::vector <cv::Point2f>& ptSamples, long double& dA, long double& dB, long double& dC);
	long calcLineVerification(std::vector <cv::Point2i>& ptSrcIndexs, std::vector <cv::Point2i>& ptInliers, long double& dA, long double& dB, double distThreshold);
	long calcLineVerification(std::vector <cv::Point2f>& ptSrcIndexs, std::vector <cv::Point2f>& ptInliers, long double& dA, long double& dB, double distThreshold);
	long calcRANSAC(std::vector <cv::Point2i>& ptSrcIndexs, long double& dA, long double& dB, int nMinSamples, double distThreshold);
	long calcRANSAC(std::vector <cv::Point2f>& ptSrcIndexs, long double& dA, long double& dB, int nMinSamples, double distThreshold);
	long RobustFitLine(IplImage* img, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType);
	long ObjectInAreaGetLine(cv::Mat& matTempBuf, cv::Rect rectImgSize, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType);		// 이진화 된영상에서 물체의 안쪽에서 바깥으로 좌표탐색

	//Edge Inspection
	bool Inspection_CompareEdgeBrokenArea(IplImage* img, IplImage* BrokenImg, long double* dA, long double* dB, CvPoint* CornerPoint,  const  int nSerarchSize, const int* const pCCut_x, const int*  const pCCut_y, int nBrokenOffset, bool* bReverseXY);
	bool Inspection_CompareEdgeBrokenArea(Mat& src, Mat& dst, long double* dA, long double* dB, CvPoint* CornerPoint,  const  int nSerarchSize, const int* const pCCut_x, const int*  const pCCut_y, int nBrokenOffset);
	bool Inspection_CompareEdgeBrokenArea(Mat src, Mat& dst);
	bool Inspection_CompareEdgeBurrArea(IplImage* img, IplImage* BrokenImg, long double* dA, long double* dB, CvPoint* CornerPoint, const  int nSerarchSize, const int* const pCCut_x, const int* const pCCut_y, bool* bReverseXY);
	bool DefectContours(IplImage* gray, CvSeq* &DefectContour);
	bool DefectCheck(IplImage* img, CvSeq* Contour, CvPoint Corner[4], int dist, int PADSize, int mode);
	bool EdgeExtract(IplImage* img, long double ValueA[4], long double dValueB[4]);
	bool CornerExtract(IplImage* img, long double dValueA[4], long double dValueB[4], CvPoint conerPoint[4]);
	bool BurrDefectResult(IplImage* gray, IplImage* defectImg, CvPoint Corner[4]);
	bool BrokenDefectResult(IplImage* gray, IplImage* defectImg, CvPoint Corner[4]);
	bool FillPoly(IplImage* img);
	void FillConvexhull(IplImage* src, IplImage* dst);
	
	
	//C-CUT Inspection
	long CCUT_AllocRefImg(Mat* src, STRU_INFO_PAD** stPadInfo, int* nUseCorner, int nCameraNum, Rect rtPanel, Point* ptDefaultFiducial);
	bool CCUTLineExtract(Mat src, CCUTInfo& cCUTInfo);
	long FindCCutMark(Mat src, Mat& dst, Mat mtCCutMark,int nMarkThreshold, double dMarkSize, cv::Rect2f& artMarkMainPos, cv::Rect& artMarkSearchArea, int nCornerInx, int nCornerOffset, bool bUseMatchingCCutMark, Point ptDefaultFiducial);
	long FindCCutMrak_Thresh(Mat src, Mat& dst, int nMarkThreshold, double dMarkSize, cv::Rect2f& artMarkMainPos, cv::Rect& artMarkSearchArea, int nCornerInx, int nCornerOffset);
	long FindCCutMark_Matching(Mat src, Mat& dst, Mat mtCCutMark,int nMarkThreshold, double dMarkSize, cv::Rect2f& artMarkMainPos, cv::Rect& artMarkSearchArea, Point ptDefaultFiducial);
	bool Inspection_CompareCCUTBrokenArea(const IplImage* const img, IplImage* BurrImg, const CCUTInfo cCUTInfo, const int nSerachSize );
	bool CCUTDefectResult(IplImage* gray, IplImage* defectImg, CCUTInfo cCUTInfo);
	long GetCCutBMLine(Mat mtImg, Point ptST, Point ptED, int nTheshold, long double& dA, long double& dB, int nCornerInx, int nIgnore_GV);
	long GetCCutLineToPanelEdgeCrossPoint(int nCornerInx, Point* ptCorner, vector<Point> ptCCutLine, Point& ptCrossST, Point& ptCrossED);
	BOOL CheckCCutROI(Rect rtROI);		//C-Cut Mark를 찾은 ROI에 대한 문제점 Check
	long GetBMLinePointY(cv::Mat mtImg, int nThreshold, int nCornerInx, int &nPointY, int nIgnore_GV);

	//Measure Inspection
	bool PointToPointLength(cv::Point pt1, cv::Point pt2, double& dLength);				//Point 2개가 이루는 직선의 길이.
	bool MARKPOSITION(cv::Mat Src, cv::Mat Mark, cv::Point2f& mPosition);
	BOOL MeasureJudgement(STRU_DEFECT_ITEM* EngineerDefectJudgment, int nDefectItemInx, double dValue, int nStandardInx);
	long SamplingRansac(vector<cv::Point2f>& pt, int nSampling, int nLineInx);
	bool subPixeling_RectangleEdge(Mat mtSrc, Mat& mtDraw, int nThreshold_Theta, Point* ptCorner, int nLineInx, vector<cv::Point2f>& pt, bool bImageSave,  vector<CRect> pFilterROI, Point2f* ptMark, int fAvgOffset, int* nCCutOffset);
	bool GetEdgeLine(Mat mtSrc, vector<cv::Point2f>& pt, Point* ptCorner, int nLineInx, Point2f* ptMark, bool bImageSave, vector<CRect> pFilterROI);
	bool JudgePointDirection(Point2f pt, double dA, double dB, int nLineInx, bool bXY_Reverse, float& fLength);
	bool PadofActiveLineFitting(Mat src, double& dA, double& dB, Point* ptCorner, int nLineInx, float fLimitTheata, int* nCCutOffset, int nBMSearchRange);
	bool SortGraylevel(Mat src, int nIndex);
	bool LimitThetaSearch_Line(Mat src, float fTheta);

	//ETC
	void FindBiggestBlob(cv::Mat& src, cv::Mat& dst);	//가장 큰 Blob만 남김
	void FindBiggestBoundingRt(cv::Mat& src, cv::Mat& dst);


public:
	CInspectEdge(void);
	~CInspectEdge(void);

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

	// 메모리 관리
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};

	//Edge Inspection
	long FindCuttingDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

	long FindCCutDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner,  double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, CString strCurModelPath, stDefectInfo* pResultBlob, stMeasureInfo* pMeasureInfo, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt);

	long FindCCutDefect_MarkLine(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner,  double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, CString strCurModelPath, stDefectInfo* pResultBlob, stMeasureInfo* pMeasureInfo, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt);

	//Align시 취득한 좌표에서 측정을하면 TacTime에 유리하지만 Align 보정된 영상에서 재측정 들어가는 것이 측정정합성에 더 좋음 _2017.09.14 by LJM
	//Panel 외곽 Size 측정, Panel 외곽 Line이 측정 기준위치(류위엔 담당자)_2017.09.14 by LJM
	long PanelSizeMeasure(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner,  double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, stMeasureInfo* pMeasureInfo, Mat& mtMeasureImage, wchar_t* strContourTxt = NULL);

	long PanelSizeMeasure_CrossMark(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner,  double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, stMeasureInfo* pMeasureInfo, Mat& mtMeasureImage, CString strCurModelPath, vector<CRect> pFilterROI, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt = NULL);
};

