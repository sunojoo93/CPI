
#include "Define.h"


// Binarization ����



EXTERNC AFX_API_EXPORT BOOL  GetCrossPointByTwoStraightLines(float a1, float b1, float c1, float a2, float b2, float c2, float &fx, float &fy );


EXTERNC AFX_API_EXPORT BOOL GetDistance_PointToLine( float x0, float y0, float a1, float b1, float c1, float& fDis );

///////////  Edge ����

// Basic Sobel : 3 x 3 Mask
EXTERNC AFX_API_EXPORT BOOL	 Edge_BasicSobel(Mat& Src, Mat& Des, int nKernalSize = 3, double dScale=1, double dDelta=0, int nBorderType =BORDER_DEFAULT);

// Basic Thinning : �β��� ������ �߽� ������ ã�´�. ���� : ū ������ �ӵ��� ���� �� �ִ�.
EXTERNC AFX_API_EXPORT void	 Edge_Thinning(Mat& src, Mat& dst);

// Edge Ehance 
EXTERNC AFX_API_EXPORT BOOL	GuidedEdgeEnhance(const Mat Src, Mat& O_Des);

//Detail Enhancement
EXTERNC AFX_API_EXPORT BOOL DetailEnhancement(Mat& src, Mat& dst, float factor);

//Threshold Multi Otsu
EXTERNC AFX_API_EXPORT void Threshold_MultiOtsu(Mat src, Mat& dst);

//���� ū Blob ��ȯ
EXTERNC AFX_API_EXPORT void FindBiggestBlob(cv::Mat& src, cv::Mat& dst);

//���� ū Contour ��ȯ
EXTERNC AFX_API_EXPORT void FindBiggestContour(cv::Mat src, vector<vector<cv::Point>> &ptBiggest);

//nSelectInx ��° ��ŭ ū Blob ��ȯ
EXTERNC AFX_API_EXPORT void SelectBiggestBlob(cv::Mat& src, cv::Mat& dst, int nSelectInx);

//�ּ��ڽ¹����� ���� �޾ƿ���
EXTERNC AFX_API_EXPORT long MethodOfLeastSquares(vector<cv::Point> pt, double &nA);

EXTERNC AFX_API_EXPORT void MinimumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);

EXTERNC AFX_API_EXPORT void MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);

