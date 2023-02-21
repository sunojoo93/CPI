
#include "Define.h"


// Binarization 관련



EXTERNC AFX_API_EXPORT BOOL  GetCrossPointByTwoStraightLines(float a1, float b1, float c1, float a2, float b2, float c2, float &fx, float &fy );


EXTERNC AFX_API_EXPORT BOOL GetDistance_PointToLine( float x0, float y0, float a1, float b1, float c1, float& fDis );

///////////  Edge 관련

// Basic Sobel : 3 x 3 Mask
EXTERNC AFX_API_EXPORT BOOL	 Edge_BasicSobel(Mat& Src, Mat& Des, int nKernalSize = 3, double dScale=1, double dDelta=0, int nBorderType =BORDER_DEFAULT);

// Basic Thinning : 두꺼운 에지의 중심 라인을 찾는다. 주의 : 큰 영상은 속도가 느릴 수 있다.
EXTERNC AFX_API_EXPORT void	 Edge_Thinning(Mat& src, Mat& dst);

// Edge Ehance 
EXTERNC AFX_API_EXPORT BOOL	GuidedEdgeEnhance(const Mat Src, Mat& O_Des);

//Detail Enhancement
EXTERNC AFX_API_EXPORT BOOL DetailEnhancement(Mat& src, Mat& dst, float factor);

//Threshold Multi Otsu
EXTERNC AFX_API_EXPORT void Threshold_MultiOtsu(Mat src, Mat& dst);

//가장 큰 Blob 반환
EXTERNC AFX_API_EXPORT void FindBiggestBlob(cv::Mat& src, cv::Mat& dst);

//가장 큰 Contour 반환
EXTERNC AFX_API_EXPORT void FindBiggestContour(cv::Mat src, vector<vector<cv::Point>> &ptBiggest);

//nSelectInx 번째 만큼 큰 Blob 반환
EXTERNC AFX_API_EXPORT void SelectBiggestBlob(cv::Mat& src, cv::Mat& dst, int nSelectInx);

//최소자승법으로 기울기 받아오기
EXTERNC AFX_API_EXPORT long MethodOfLeastSquares(vector<cv::Point> pt, double &nA);

EXTERNC AFX_API_EXPORT void MinimumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);

EXTERNC AFX_API_EXPORT void MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);

