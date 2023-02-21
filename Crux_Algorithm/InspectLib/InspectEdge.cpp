#include "StdAfx.h"
#include "Define.h"
#include "FeatureExtraction.h"
#include "InspectEdge.h"
#include "math.h"
#include "ExportLibrary.h"
#include "BasicLibrary.h"

using namespace std;
using namespace cv;

CInspectEdge::CInspectEdge(void)
{
	cMem=NULL;
}


CInspectEdge::~CInspectEdge(void)
{
}

// 중복 데이터 비교
bool CInspectEdge::FindInSamples(std::vector <cv::Point2i>& ptSamples, cv::Point2i ptIndexs)
{
	for (int i=0 ; i<ptSamples.size() ; i++)
	{
		if ( ptSamples[i].x == ptIndexs.x &&
			ptSamples[i].y == ptIndexs.y )
			return true;
	}

	return false;
}
bool CInspectEdge::FindInSamples(std::vector <cv::Point2f>& ptSamples, cv::Point2f ptIndexs)
{
	for (int i=0 ; i<ptSamples.size() ; i++)
	{
		if ( ptSamples[i].x == ptIndexs.x &&
			ptSamples[i].y == ptIndexs.y )
			return true;
	}

	return false;
}

// 데이터에서 중복되지 않게 N개의 무작위 샘플을 취득
long  CInspectEdge::GetRandomSamples(std::vector <cv::Point2i>& ptSrcIndexs, std::vector <cv::Point2i>& ptSamples, int nSampleCount)
{
	// 원본 크기
	int nSize = (int)ptSrcIndexs.size();

	// 설정한 N개 만큼
	while ( ptSamples.size() < nSampleCount )
	{
		// 원본에서 무작위 샘플
		int j = rand() % nSize;

		// 중복 되지 않도록
		if ( !FindInSamples(ptSamples, ptSrcIndexs[j]) )
		{
			// 좌표 벡터 추가
			ptSamples.push_back( ptSrcIndexs[j] );
		}
	}

	return true;
}

long CInspectEdge::GetRandomSamples(std::vector <cv::Point2f>& ptSrcIndexs, std::vector <cv::Point2f>& ptSamples, int nSampleCount)
{
	// 원본 크기
	int nSize = (int)ptSrcIndexs.size();

	// 설정한 N개 만큼
	while ( ptSamples.size() < nSampleCount )
	{
		// 원본에서 무작위 샘플
		int j = rand() % nSize;

		// 중복 되지 않도록
		if ( !FindInSamples(ptSamples, ptSrcIndexs[j]) )
		{
			// 좌표 벡터 추가
			ptSamples.push_back( ptSrcIndexs[j] );
		}
	}

	return true;
}


// 라인 추정 계산
long  CInspectEdge::calcLineFit(std::vector <cv::Point2i>& ptSamples, long double& dA, long double& dB)
{
	// 데이터 갯수 없는 경우
	if( ptSamples.size() <= 0 )	return E_ERROR_CODE_ALIGN_NO_DATA;

	// PCA 방식으로 직선 모델의 파라미터를 예측
	long double sx  = 0.0, sy  = 0.0;
	long double sxx = 0.0, syy = 0.0;
	long double sxy = 0.0, sw  = (long double)ptSamples.size();
	long double x, y;

	for(int i=0 ; i<ptSamples.size() ; i++)
	{
		x = (long double)ptSamples[i].x;
		y = (long double)ptSamples[i].y;

		sx  += x;
		sy  += y;
		sxx += x*x;
		sxy += x*y;
		syy += y*y;
	}

	// variance
	long double vxx = (sxx - sx * sx / sw) / sw;
	long double vxy = (sxy - sx * sy / sw) / sw;
	long double vyy = (syy - sy * sy / sw) / sw;

	// principal axis
	long double theta = atan2(2. * vxy, vxx - vyy) / 2.;

	// center of mass(xc, yc)
	sx /= sw;
	sy /= sw;

	//직선의 방정식: sin(theta)*(x - sx) = cos(theta)*(y - sy);
	//dA = sin(theta) / cos(theta);
	dA = tan(theta);
	dB = sy - dA * sx;

	return E_ERROR_CODE_TRUE;
}

long  CInspectEdge::calcLineFit(std::vector <cv::Point2f>& ptSamples, long double& dA, long double& dB)
{
	// 데이터 갯수 없는 경우
	if( ptSamples.size() <= 0 )	return E_ERROR_CODE_ALIGN_NO_DATA;

	// PCA 방식으로 직선 모델의 파라미터를 예측
	long double sx  = 0.0, sy  = 0.0;
	long double sxx = 0.0, syy = 0.0;
	long double sxy = 0.0, sw  = (long double)ptSamples.size();
	long double x, y;

	for(int i=0 ; i<ptSamples.size() ; i++)
	{
		x = (long double)ptSamples[i].x;
		y = (long double)ptSamples[i].y;

		sx  += x;
		sy  += y;
		sxx += x*x;
		sxy += x*y;
		syy += y*y;
	}

	// variance
	long double vxx = (sxx - sx * sx / sw) / sw;
	long double vxy = (sxy - sx * sy / sw) / sw;
	long double vyy = (syy - sy * sy / sw) / sw;

	// principal axis
	long double theta = atan2(2. * vxy, vxx - vyy) / 2.;

	// center of mass(xc, yc)
	sx /= sw;
	sy /= sw;

	//직선의 방정식: sin(theta)*(x - sx) = cos(theta)*(y - sy);
	//dA = sin(theta) / cos(theta);
	dA = tan(theta);
	dB = sy - dA * sx;

	return E_ERROR_CODE_TRUE;
}

// 유효한 데이터 갯수 파악
long  CInspectEdge::calcLineVerification(std::vector <cv::Point2i>& ptSrcIndexs, std::vector <cv::Point2i>& ptInliers, long double& dA, long double& dB, double distThreshold)
{
	for(int i=0 ; i<ptSrcIndexs.size() ; i++)
	{
		// 직선에 내린 수선의 길이를 계산
		// 한 점(x)로부터 직선(line)에 내린 수선의 길이(distance)를 계산
		// | ax + bx + c | / sqrt( a*a + b*b )
		double distance = abs( ptSrcIndexs[i].x * dA - ptSrcIndexs[i].y + dB ) / sqrt( dA * dA  + 1. );

		// 예측된 모델에서 유효한 데이터인 경우, 유효한 데이터 추가
		if ( distance < distThreshold )
			ptInliers.push_back( ptSrcIndexs[i] );
	}	

	return (long)ptInliers.size();
}
long  CInspectEdge::calcLineVerification(std::vector <cv::Point2f>& ptSrcIndexs, std::vector <cv::Point2f>& ptInliers, long double& dA, long double& dB, double distThreshold)
{
	for(int i=0 ; i<ptSrcIndexs.size() ; i++)
	{
		// 직선에 내린 수선의 길이를 계산
		// 한 점(x)로부터 직선(line)에 내린 수선의 길이(distance)를 계산
		// | ax + bx + c | / sqrt( a*a + b*b )
		double distance = abs( ptSrcIndexs[i].x * dA - ptSrcIndexs[i].y + dB ) / sqrt( dA * dA  + 1. );

		// 예측된 모델에서 유효한 데이터인 경우, 유효한 데이터 추가
		if ( distance < distThreshold )
			ptInliers.push_back( ptSrcIndexs[i] );
	}	

	return (long)ptInliers.size();
}

// RANSAC 알고리즘
long CInspectEdge::calcRANSAC(std::vector <cv::Point2i>& ptSrcIndexs, long double& dA, long double& dB, int nMinSamples, double distThreshold)
{
	long nMaxCost = 0;

	// 벡터 초기화
	vector <cv::Point2i> ptSamples, ptInliers;

	// 임시 라인 방정식 y = ax + b
	long double dAA = 0, dBB = 0;

	// 반복 횟수 정하기
	int nMaxIter = (int)(1 + log(1. - 0.99)/log(1. - pow(0.5, nMinSamples)));

	long nErrorCode = E_ERROR_CODE_TRUE ;
	for (int i = 0; i<nMaxIter; i++)
	{
		// 벡터 초기화
		vector<cv::Point2i>().swap(ptSamples);
		vector<cv::Point2i>().swap(ptInliers);

		// hypothesis
		// 원본 데이터에서 임의로 N개의 샘플 데이터를 취득
		GetRandomSamples(ptSrcIndexs, ptSamples, nMinSamples);

		// 현재 데이터를 정상적인 데이터로 보고 모델 파라미터 예측
		calcLineFit(ptSamples, dAA, dBB);

		// Verification
		// 원본 데이터가 예측된 모델에 잘 맞는지 검사
		long cost = calcLineVerification(ptSrcIndexs, ptInliers, dAA, dBB, distThreshold);

		// 만일 예측된 모델이 잘 맞는다면, 유효한 데이터로 새로운 모델을 구함
		if( nMaxCost < cost )
		{
			nMaxCost = cost;
			calcLineFit(ptInliers, dA, dB);
		}

		// 벡터 데이터 제거
		vector<cv::Point2i>().swap(ptInliers);
	}

	// 벡터 데이터 제거
	vector<cv::Point2i>().swap(ptSamples);

	// 계산 불가
	if( nMaxCost <= 0 )		return 2;

	return E_ERROR_CODE_TRUE;
}

long CInspectEdge::calcRANSAC(std::vector <cv::Point2f>& ptSrcIndexs, long double& dA, long double& dB, int nMinSamples, double distThreshold)
{
	if(ptSrcIndexs.size() < nMinSamples)
		return E_ERROR_CODE_FALSE;

	long nMaxCost = 0;

	// 벡터 초기화
	vector <cv::Point2f> ptSamples, ptInliers;

	// 임시 라인 방정식 y = ax + b
	long double dAA = 0, dBB = 0;

	// 반복 횟수 정하기
	int nMaxIter = (int)(1 + log(1. - 0.99)/log(1. - pow(0.5, nMinSamples)));

	long nErrorCode = E_ERROR_CODE_TRUE ;
	for (int i = 0; i<nMaxIter; i++)
	{
		// 벡터 초기화
		vector<cv::Point2f>().swap(ptSamples);
		vector<cv::Point2f>().swap(ptInliers);

		// hypothesis
		// 원본 데이터에서 임의로 N개의 샘플 데이터를 취득
		GetRandomSamples(ptSrcIndexs, ptSamples, nMinSamples);

		// 현재 데이터를 정상적인 데이터로 보고 모델 파라미터 예측
		calcLineFit(ptSamples, dAA, dBB);

		// Verification
		// 원본 데이터가 예측된 모델에 잘 맞는지 검사
		long cost = calcLineVerification(ptSrcIndexs, ptInliers, dAA, dBB, distThreshold);

		// 만일 예측된 모델이 잘 맞는다면, 유효한 데이터로 새로운 모델을 구함
		if( nMaxCost < cost )
		{
			nMaxCost = cost;
			calcLineFit(ptInliers, dA, dB);
		}

		// 벡터 데이터 제거
		vector<cv::Point2f>().swap(ptInliers);
	}

	// 벡터 데이터 제거
	vector<cv::Point2f>().swap(ptSamples);

	// 계산 불가
	if( nMaxCost <= 0 )		return E_ERROR_CODE_FALSE;

	return E_ERROR_CODE_TRUE;
}


// 방향에 따른 데이터 획득
long CInspectEdge::RobustFitLine(IplImage* img,	long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType)
{
	int x, y, nSamp	= 5;								//  5 Pixel 씩 샘플링

	vector<cv::Point2i>	ptSrcIndexs;
	vector<cv::Point2i>().swap(ptSrcIndexs);

	// 라인 데이터 벡터 추출
	switch(nType)
	{
	case LEFT:
		{
			for(y=0 ; y<img->height ; y+=nSamp)
			{
				for(x=0 ; x<img->width ; x++)
				{
					// 값이 있는 경우
					if((uchar)img->imageData[(y * img->widthStep) + x]>0)
					{
						// 좌표 벡터 추가
						ptSrcIndexs.push_back( cv::Point2i(x, y) );

						break;
					}
				}
			}
		}
		break;

	case TOP:
		{
			for(x=0 ; x<img->width ; x+=nSamp)
			{
				for(y=0; y<img->height ; y++)
				{
					// 값이 있는 경우
					if((uchar)img->imageData[(y * img->widthStep) + x]>0)
					{
						// 좌표 벡터 추가
						ptSrcIndexs.push_back( cv::Point2i(x, y) );

						break;
					}
				}
			}
		}
		break;

	case RIGHT:
		{
			for(y=0 ; y<img->height ; y+=nSamp)
			{
				for(x=img->width-1 ; x>=0 ; x--)
				{
					// 값이 있는 경우
					if((uchar)img->imageData[(y * img->widthStep) + x]>0)	
					{
						// 좌표 벡터 추가
						ptSrcIndexs.push_back( cv::Point2i(x, y) );

						break;
					}
				}
			}
		}		
		break;

	case BOTTOM:
		{
			for(x=0 ; x<img->width ; x+=nSamp)
			{
				for(y=img->height-1 ; y>=0 ; y--)
				{
					// 값이 있는 경우
					if((uchar)img->imageData[(y * img->widthStep) + x]>0)
					{
						// 좌표 벡터 추가
						ptSrcIndexs.push_back( cv::Point2i(x, y) );

						break;
					}
				}
			}
		}		
		break;

		// 파라미터 이상인 경우
	default:
		{
			return false;
		}
		break;
	}

	// 추출된 데이터로 라인 찾기
	long nErrorCode = calcRANSAC(ptSrcIndexs, dA, dB, nMinSamples, distThreshold);

	// 벡터 데이터 제거
	vector<cv::Point2i>().swap(ptSrcIndexs);

	return true;
}

// 이진화 된영상에서 물체의 안쪽에서 바깥으로 좌표탐색
long CInspectEdge::ObjectInAreaGetLine(cv::Mat& matTempBuf, cv::Rect rectImgSize, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType)
{
	// 영상 크기
	int nW = matTempBuf.cols;
	int nH = matTempBuf.rows;
	int nWidthStep = matTempBuf.step;
	uchar* ucImagedata = matTempBuf.data;


	// 범위 설정
	int nStartX		= 0;								//초기화
	int nEndX		= 0;								
	int nStartY		= 0;				
	int nEndY		= 0;								

	vector<cv::Point2i>	ptSrcIndexs;//좌표 취득 부
	vector<cv::Point2i>().swap(ptSrcIndexs);	
	int nX=0,nY=0,nSample=50;
	int nStPtOffset=100;			//Panel의 안쪽에서 외곽면을 Search할때 외곽면으로 부터 안쪽 시작지점의 Offset값
	uchar* p=0;						//영상 Data Pointer

	switch(nType)
	{
	case E_ALIGN_TYPE_LEFT:
		nStartX = rectImgSize.width/2;							//영상의 가로 중앙
		nEndX	= 0;											//영상의 가로 시작점
		nStartY = rectImgSize.height/3;							//영상의 세로 1/3 지점
		nEndY	= rectImgSize.height*2/3;						//Panel의 세로 3/4지점

		for(nY = nStartY; nY < nEndY; nY+=nSample)
		{
			p = ucImagedata+(nWidthStep*nY);
			for(nX = nStartX; nX > nEndX; nX--)
			{
				if((int)*(p+nX) == 0)
				{
					ptSrcIndexs.push_back(cv::Point2i(nX,nY));
					break;
				}
			}		
		}
		break;
	case E_ALIGN_TYPE_TOP:
		nStartX = rectImgSize.width/3;							//영상의 가로 1/3지점
		nEndX	= rectImgSize.width*2/3;						//영상의 가로 2/3지점
		nStartY = rectImgSize.height/2;							//영상의 세로 중앙
		nEndY   = 0;											//영상의 세로 시작점

		for(nX = nStartX; nX < nEndX; nX+=nSample)
		{
			p = ucImagedata+nX;
			for(nY = nStartY; nY > nEndY; nY--)
			{

				if((int)*(p+nY*nWidthStep) == 0)
				{
					ptSrcIndexs.push_back(cv::Point2i(nX, nY));
					break;
				}
			}

		}
		break;
	case E_ALIGN_TYPE_RIGHT:
		nStartX = rectImgSize.width/2;						//영상의 가로 중앙
		nEndX	= matTempBuf.cols;							//영상의 가로 끝점
		nStartY = rectImgSize.height/3;						//영상의 세로 1/3지점
		nEndY	= rectImgSize.height/3*2;					//영상의 세로 2/3지점

		for(nY = nStartY; nY < nEndY; nY+=nSample)
		{
			p = ucImagedata+(nWidthStep*nY);
			for(nX = nStartX; nX < nEndX; nX++)
			{
				if((int)*(p+nX) == 0)
				{
					ptSrcIndexs.push_back(cv::Point2i(nX, nY));
					break;
				}
			}
		}
		break;
	case E_ALIGN_TYPE_BOTTOM:
		nStartX = rectImgSize.width/3;							//영상의 가로 1/3지점
		nEndX	= rectImgSize.width/3*2;						//영상의 가로 2/3지점
		nStartY = rectImgSize.height/2;							//영상의 세로 중앙
		nEndY	= matTempBuf.rows;								//영상의 세로 끝점

		for(nX = nStartX; nX < nEndX; nX+=nSample)
		{
			p = ucImagedata+nX;
			for(nY = nStartY; nY < nEndY; nY++)
			{
				if((int)*(p+nY*nWidthStep) == 0)
				{
					ptSrcIndexs.push_back(cv::Point2i(nX, nY));
					break;
				}
			}
		}
		break;

	default:
		return E_ERROR_CODE_ALIGN_WARNING_PARA;
		break;
	}

	if(ptSrcIndexs.size() < 1)
		return E_ERROR_CODE_FALSE;

	// 추출된 데이터로 라인 찾기
	long nErrorCode = calcRANSAC(ptSrcIndexs, dA, dB, nMinSamples, distThreshold);

	// 벡터 데이터 제거
	vector<cv::Point2i>().swap(ptSrcIndexs);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	return E_ERROR_CODE_TRUE;
}




bool CInspectEdge::Inspection_CompareEdgeBrokenArea(IplImage* img, IplImage* BrokenImg, long double* dA, long double* dB, CvPoint* CornerPoint,  const  int nSerarchSize, const int* const pCCut_x, const int*  const pCCut_y, int nBrokenOffset, bool* bReverseXY)
{

	const int PixelOffset	= 4;	

	#pragma omp parallel 
	{
		//	Left
		int nLeftStart	= CornerPoint[E_CORNER_LEFT_TOP].y		+ nBrokenOffset + pCCut_y[E_CORNER_LEFT_TOP];
		int nLeftEnd	= CornerPoint[E_CORNER_LEFT_BOTTOM].y	- nBrokenOffset - pCCut_y[E_CORNER_LEFT_BOTTOM];

		#pragma omp for
		for(int h = nLeftStart; h < nLeftEnd; h++)
		{
			int sw;
			int ew;
			if(bReverseXY[LEFT])	sw = h*dA[LEFT]+dB[LEFT] + nBrokenOffset;
			else					sw = (h-dB[LEFT])/dA[LEFT] + nBrokenOffset;				
			
			ew = sw+nSerarchSize; if(ew>img->width) ew= img->width;
			for(int w = sw; w<sw+nSerarchSize; w++)
			{
				if((uchar)img->imageData[h*img->widthStep+w] < 100)	
					BrokenImg->imageData[h*img->widthStep+w] = 125;
			}
		}

		int nTopStart	= CornerPoint[E_CORNER_LEFT_TOP].x	+ nBrokenOffset + pCCut_x[E_CORNER_LEFT_TOP];
		int nTopEnd		= CornerPoint[E_CORNER_RIGHT_TOP].x	- nBrokenOffset - pCCut_x[E_CORNER_RIGHT_TOP];
	
		//	TOP
		#pragma omp for
		for(int w = nTopStart ; w < nTopEnd ; w++)
		{
			int sh;
			int eh;
			if(bReverseXY[TOP])		sh = (w-dB[TOP])/dA[TOP] + nBrokenOffset;
			else					sh = dA[TOP]*w+dB[TOP] + nBrokenOffset;
				
			eh = sh+nSerarchSize;	if(eh>=img->height) eh = img->height;
			for(int h = sh; h<eh; h++)	
			{
				if((uchar)img->imageData[h*img->widthStep+w] < 100)	
					BrokenImg->imageData[h*img->widthStep+w] = 125;
			}
		}
	
		int nRightStart		= CornerPoint[E_CORNER_RIGHT_TOP].y	+ nBrokenOffset + pCCut_y[E_CORNER_RIGHT_TOP];
		int nRightEnd		= CornerPoint[E_CORNER_RIGHT_BOTTOM].y	- nBrokenOffset - pCCut_y[E_CORNER_RIGHT_BOTTOM];

		//	Right
		#pragma omp for
		for(int h = nRightStart; h < nRightEnd; h++)
		{
			int sw;
			int ew;
			if(bReverseXY[RIGHT])	sw = h*dA[RIGHT]+dB[RIGHT] - nBrokenOffset;
			else					sw = (h-dB[RIGHT])/dA[RIGHT] - nBrokenOffset;

			ew = sw-nSerarchSize; if(ew<=0) ew=0;
			for(int w = sw; w>ew; w--)	
			{
				if((uchar)img->imageData[h*img->widthStep+w] < 100)	
					BrokenImg->imageData[h*img->widthStep+w] = 125;
			}
		}
	
		int nBottomStart		= CornerPoint[E_CORNER_LEFT_BOTTOM].x	+ nBrokenOffset + pCCut_x[E_CORNER_LEFT_BOTTOM];
		int nBottomEnd			= CornerPoint[E_CORNER_RIGHT_BOTTOM].x	- nBrokenOffset - pCCut_x[E_CORNER_RIGHT_BOTTOM];
		//	Bottom
		#pragma omp for
		for(int w = nBottomStart; w < nBottomEnd ; w++)	
		{
			int sh;
			int eh;
			if(bReverseXY[BOTTOM])	sh = (w-dB[BOTTOM])/dA[BOTTOM] - nBrokenOffset;
			else					sh = dA[BOTTOM]*w+dB[BOTTOM] - nBrokenOffset;

			eh = sh-nSerarchSize; if(eh<=0) eh=0;
			for(int h = sh; h>eh; h--)	
			{
				if((uchar)img->imageData[h*img->widthStep+w] < 100)	
					BrokenImg->imageData[h*img->widthStep+w] = 125;
			}
		}
	}

	return true;
}


bool CInspectEdge::Inspection_CompareEdgeBrokenArea(Mat src, Mat& dst)
{
	Mat mtConvexImage = Mat::zeros(src.size(), CV_8UC1);

	FillConvexhull(&IplImage(src), &IplImage(mtConvexImage));

	imwrite("C:\\IDB\\FillConvex.bmp", mtConvexImage);

	vector<vector<cv::Point>>	ptContours;
	vector<vector<cv::Point>>().swap(ptContours);
	cv::findContours(src, ptContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	int nMaxInx =-1;
	int nSize=0;
	for(int nContourInx =0; nContourInx < ptContours.size(); nContourInx++)
	{
		cv::Rect rtContour = cv::boundingRect(ptContours[nContourInx]);

		if(nSize < rtContour.width * rtContour.height)
		{
			nSize = rtContour.width * rtContour.height;
			nMaxInx = nContourInx;
		}
	}

	
	if(nMaxInx == -1)
		return false;

	Mat mtDrawContours = Mat::zeros(src.size(), CV_8UC1);
	drawContours(mtDrawContours, ptContours, nMaxInx, Scalar(255,255,255), 5);
	FillPoly(&IplImage(mtDrawContours));
	cv::subtract(mtConvexImage, mtDrawContours, dst);


	imwrite("C:\\IDB\\ConvexTest.bmp", dst);

	return true;

}

bool CInspectEdge::Inspection_CompareEdgeBrokenArea(Mat& src, Mat& dst, long double* dA, long double* dB, CvPoint* CornerPoint,  const  int nSerarchSize, const int* const pCCut_x, const int*  const pCCut_y, int nBrokenOffset)
{
	const int PixelOffset	= 3;	

#pragma omp parallel 
	{
		//	Left
		int nLeftStart	= CornerPoint[E_CORNER_LEFT_TOP].y		+ nBrokenOffset + pCCut_y[E_CORNER_LEFT_TOP];
		int nLeftEnd	= CornerPoint[E_CORNER_LEFT_BOTTOM].y	- nBrokenOffset - pCCut_y[E_CORNER_LEFT_BOTTOM];
		uchar* pSrcData;
		uchar* pDstData;
#pragma omp for
		for(int h = nLeftStart; h < nLeftEnd; h++)
		{
			int sw = (h-dB[LEFT])/dA[LEFT] + nBrokenOffset;
			int ew = sw+nSerarchSize; if(ew>src.cols) ew= src.rows;

			pSrcData = src.data + h*src.cols;
			pDstData = dst.data + h*dst.cols;
			for(int w = sw; w<sw+nSerarchSize; w++)
			{
				if((int)*(pSrcData+w) < 100)	
					*(pDstData+w) = 125;
			}
		}

		int nTopStart	= CornerPoint[E_CORNER_LEFT_TOP].x		+ nBrokenOffset + pCCut_x[E_CORNER_LEFT_TOP];
		int nTopEnd		= CornerPoint[E_CORNER_RIGHT_TOP].x	- nBrokenOffset - pCCut_x[E_CORNER_RIGHT_TOP];

		//	TOP
#pragma omp for
		for(int w = nTopStart ; w < nTopEnd ; w++)
		{
			int sh = dA[TOP]*w+dB[TOP] + nBrokenOffset;
			int eh = sh+nSerarchSize;	if(eh>=src.cols) eh = src.rows;
			pSrcData = src.data + w;
			pDstData = dst.data + w;
			for(int h = sh; h<eh; h++)	
			{
				if((int)*(pSrcData + h * src.cols) < 100)	
					*(pDstData + h * dst.cols) = 125;
			}
		}

		int nRightStart		= CornerPoint[E_CORNER_RIGHT_TOP].y	+ nBrokenOffset + pCCut_y[E_CORNER_RIGHT_TOP];
		int nRightEnd		= CornerPoint[E_CORNER_RIGHT_BOTTOM].y	- nBrokenOffset - pCCut_y[E_CORNER_RIGHT_BOTTOM];

		//	Right
#pragma omp for
		for(int h = nRightStart; h < nRightEnd; h++)
		{
			int sw = (h-dB[RIGHT])/dA[RIGHT] - nBrokenOffset;
			int ew = sw-nSerarchSize; if(ew<=0) ew=0;
			pSrcData = src.data + h*src.cols;
			pDstData = dst.data + h*dst.cols;
			for(int w = sw; w<sw+nSerarchSize; w++)
			{
				if((int)*(pSrcData+w) < 100)	
					*(pDstData+w) = 125;
			}
		}

		int nBottomStart		= CornerPoint[E_CORNER_LEFT_BOTTOM].x	+ nBrokenOffset + pCCut_x[E_CORNER_LEFT_BOTTOM];
		int nBottomEnd			= CornerPoint[E_CORNER_RIGHT_BOTTOM].x	- nBrokenOffset - pCCut_x[E_CORNER_RIGHT_BOTTOM];
		//	Bottom
#pragma omp for
		for(int w = nBottomStart; w < nBottomEnd ; w++)	
		{
			int sh = dA[BOTTOM]*w+dB[BOTTOM] - nBrokenOffset;
			int eh = sh-nSerarchSize; if(eh<=0) eh=0;
			pSrcData = src.data + w;
			pDstData = dst.data + w;
			for(int h = sh; h<eh; h++)	
			{
				if((int)*(pSrcData + h * src.cols) < 100)	
					*(pDstData + h * dst.cols) = 125;
			}
		}
	}

	return true;
}


bool CInspectEdge::Inspection_CompareEdgeBurrArea(IplImage* img, IplImage* BrokenImg, long double* dA, long double* dB, CvPoint* CornerPoint, const  int nSerarchSize, const int* const pCCut_x, const int* const pCCut_y, bool* bReverseXY)
{
	const int PixelOffset	= 4;

	#pragma omp parallel 
	{
		//	Left
		//	Left
		int nLeftStart	= CornerPoint[E_CORNER_LEFT_TOP].y		+ PixelOffset + pCCut_y[E_CORNER_LEFT_TOP];
		int nLeftEnd	= CornerPoint[E_CORNER_LEFT_BOTTOM].y	- PixelOffset - pCCut_y[E_CORNER_LEFT_BOTTOM];

		#pragma omp for
		for(int h = nLeftStart; h < nLeftEnd ; h++)	
		{
			int sw;
			int ew;

			if(bReverseXY[LEFT])	sw = dA[LEFT]*h + dB[LEFT] - PixelOffset;
			else					sw = (h-dB[LEFT])/dA[LEFT] - PixelOffset;
				
			ew = sw - nSerarchSize; if(ew<=0) ew = 0;
			for(int w = sw; w>ew; w--)	
			{
				if((uchar)img->imageData[h*img->widthStep+w] > 100)	
					BrokenImg->imageData[h*img->widthStep+w] = (uchar)255;
			}
		}
	
		//	TOP
		int nTopStart	= CornerPoint[E_CORNER_LEFT_TOP].x		+ PixelOffset + pCCut_x[E_CORNER_LEFT_TOP];
		int nTopEnd		= CornerPoint[E_CORNER_RIGHT_TOP].x	- PixelOffset - pCCut_x[E_CORNER_RIGHT_TOP];

		#pragma omp for
		for(int w = nTopStart ; w < nTopEnd ; w++)	
		{
			int sh; 
			int eh;
			if(bReverseXY[TOP])		sh = (w-dB[TOP])/dA[TOP] - PixelOffset;
			else					sh = dA[TOP]*w+dB[TOP] - PixelOffset;

			eh = sh-nSerarchSize;	if(eh<=0) eh = 0;
			for(int h = sh; h>eh; h--)
			{
				if((uchar)img->imageData[h*img->widthStep+w] > 100)	
					BrokenImg->imageData[h*img->widthStep+w] = (uchar)255;
			}
		}
	
		//	Right 
		int nRightStart		= CornerPoint[E_CORNER_RIGHT_TOP].y	+ PixelOffset + pCCut_y[E_CORNER_RIGHT_TOP];
		int nRightEnd		= CornerPoint[E_CORNER_RIGHT_BOTTOM].y	- PixelOffset - pCCut_y[E_CORNER_RIGHT_BOTTOM];

		#pragma omp for
		for(int h = nRightStart; h < nRightEnd ; h++)	
		{
			int sw;
			int ew;

			if(bReverseXY[RIGHT])	sw = dA[RIGHT]*h + dB[RIGHT] + PixelOffset;
			else					sw = (h-dB[RIGHT])/dA[RIGHT] + PixelOffset;
			
			ew = sw+nSerarchSize; if(ew>=img->width) ew = img->width;

			for(int w = sw; w<ew; w++)	
			{
				if((uchar)img->imageData[h*img->widthStep+w] > 100)	
					BrokenImg->imageData[h*img->widthStep+w] = (uchar)255;
			}
		}
	
	
		//	Bottom
		int nBottomStart		= CornerPoint[E_CORNER_LEFT_BOTTOM].x	+ PixelOffset + pCCut_x[E_CORNER_LEFT_BOTTOM];
		int nBottomEnd			= CornerPoint[E_CORNER_RIGHT_BOTTOM].x	- PixelOffset - pCCut_x[E_CORNER_RIGHT_BOTTOM];
		#pragma omp for
		for(int w = nBottomStart; w < nBottomEnd; w++)
		{
			int sh; 
			int eh; 

			if(bReverseXY[BOTTOM])	sh = (w- dB[BOTTOM])/dA[BOTTOM] + PixelOffset;
			else					sh = dA[BOTTOM]*w+dB[BOTTOM] + PixelOffset;
			
			eh = sh+nSerarchSize;	if(eh>=img->height) eh = img->height;

			for(int h = sh; h<eh; h++)	
			{
				if((uchar)img->imageData[h*img->widthStep+w] > 100)	
					BrokenImg->imageData[h*img->widthStep+w] = (uchar)255;
			}
		}
	}

	return true;
}



bool CInspectEdge::EdgeExtract(IplImage* img, long double dValueA[4], long double dValueB[4])
{
	int		nMinSamples		= 3;			
	double	distThreshold	= 5;			
	
	// 4방향 직선 찾기
	#pragma omp parallel 
	{
		#pragma omp for
		for(int nType =0; nType<4; nType++)
			RobustFitLine(img, dValueA[nType], dValueB[nType], nMinSamples, distThreshold, nType);
	}
	return true;
}

bool CInspectEdge::CornerExtract(IplImage* img, long double dValueA[4], long double dValueB[4], CvPoint ConerPoint[4])
{
	double dA1, dB1, dA2, dB2;

	dA1 = dValueA[LEFT];
	dB1 = dValueB[LEFT];
	dA2 = dValueA[TOP ];
	dB2 = dValueB[TOP];
	ConerPoint[E_CORNER_LEFT_TOP].x = (int)( (dB2 - dB1) / (dA1 - dA2) );
	ConerPoint[E_CORNER_LEFT_TOP].y = (int)( (dA2*dB1 - dA1*dB2) / (dA2 - dA1) );

	dA1 = dValueA[LEFT];
	dB1 = dValueB[LEFT];
	dA2 = dValueA[BOTTOM ];
	dB2 = dValueB[BOTTOM];
	ConerPoint[E_CORNER_LEFT_BOTTOM].x = (int)( (dB2 - dB1) / (dA1 - dA2) );
	ConerPoint[E_CORNER_LEFT_BOTTOM].y = (int)( (dA2*dB1 - dA1*dB2) / (dA2 - dA1) );

	dA1 = dValueA[RIGHT];
	dB1 = dValueB[RIGHT];
	dA2 = dValueA[TOP ];
	dB2 = dValueB[TOP];
	ConerPoint[E_CORNER_RIGHT_TOP].x = (int)( (dB2 - dB1) / (dA1 - dA2) );
	ConerPoint[E_CORNER_RIGHT_TOP].y = (int)( (dA2*dB1 - dA1*dB2) / (dA2 - dA1) );


	dA1 = dValueA[RIGHT];
	dB1 = dValueB[RIGHT];
	dA2 = dValueA[BOTTOM ];
	dB2 = dValueB[BOTTOM];
	ConerPoint[E_CORNER_RIGHT_BOTTOM].x = (int)( (dB2 - dB1) / (dA1 - dA2) );
	ConerPoint[E_CORNER_RIGHT_BOTTOM].y = (int)( (dA2*dB1 - dA1*dB2) / (dA2 - dA1) );


	return true;
}


bool CInspectEdge::DefectContours(IplImage* gray, CvSeq* &DefectContour)
{
	enum{ EdgeBroken, EdgeBurr };
	IplImage* edge = cvCreateImage(cvGetSize(gray), 8, 1);
	cvCanny(gray, edge, 30, 100, 3);
	CvMemStorage* storage = cvCreateMemStorage();
	DefectContour = NULL;
	int NC = cvFindContours(edge, storage, &DefectContour, sizeof(CvContour), CV_RETR_EXTERNAL);
	
	cvReleaseImage(&edge);

	return true;
}


bool CInspectEdge::DefectCheck(IplImage* img, CvSeq* Contour, CvPoint Corner[4], int dist, int PADSize, int mode)
{
	CvRect boundingBox = cvRect(0,0,0,0);
	CvPoint pt1, pt2;
	
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 3, CV_AA);
	char test[100];
	CString str;

	bool DefectCH = false;
	int EdgeDefectName = 0;
		
	for(CvSeq* c = Contour; c!= NULL;  c= c->h_next) {
		boundingBox = cvBoundingRect(c,0);
		pt1.x = boundingBox.x;
		pt1.y = boundingBox.y;
		pt2.x = boundingBox.x+boundingBox.width;
		pt2.y = boundingBox.y+boundingBox.height;
			
		//Defect Rect좌표에 따라 Broken/Burr의 Left, Top, Right, Bottom 구분
		if(mode != BURR)
		{
			if((((abs)(Corner[E_CORNER_LEFT_TOP].x	- pt1.x) < dist)||((abs)(Corner[E_CORNER_LEFT_BOTTOM].x  - pt1.x) < dist))
					&& ((Corner[E_CORNER_LEFT_TOP].y <= pt1.y) && (Corner[E_CORNER_LEFT_BOTTOM].y >= pt2.y)))
			{
				//Broken: Left
				EdgeDefectName = EDGE_BROKEN_LEFT;
				DefectCH = true;
				
				str.Format(_T("BROKEN_L"));
			}
			else if((((abs)(Corner[E_CORNER_LEFT_TOP].y - pt1.y) < dist)||((abs)(Corner[E_CORNER_RIGHT_TOP].y	 - pt1.y) < dist))	
				&&((Corner[E_CORNER_LEFT_TOP].x <= pt1.x) && (Corner[E_CORNER_RIGHT_TOP].x >= pt2.x)))
			{
				//Broken: Top	
				EdgeDefectName = EDGE_BROKEN_TOP;
				DefectCH = true;

				str.Format(_T("BROKEN_T"));
			}
			else if((((abs)(Corner[E_CORNER_RIGHT_TOP].x - pt2.x - PADSize) < dist)||((abs)(Corner[E_CORNER_RIGHT_BOTTOM].x  - pt2.x - PADSize) < dist))	
				&& ((Corner[E_CORNER_RIGHT_TOP].y <= pt1.y) && (Corner[E_CORNER_RIGHT_BOTTOM].y >= pt2.y)))
			{
				//Broken: Right	
				EdgeDefectName = EDGE_BROKEN_RIGTHT;
				DefectCH = true;

				str.Format(_T("BROKEN_R"));
			}
			else if((((abs)(Corner[E_CORNER_LEFT_BOTTOM].y  - pt2.y) < dist)||((abs)(Corner[E_CORNER_RIGHT_BOTTOM].y	 - pt2.y) < dist))	
				&&((Corner[E_CORNER_LEFT_BOTTOM].x <= pt1.x) && (Corner[E_CORNER_RIGHT_BOTTOM].x >= pt2.x)))
			{
				//Broken: Bottom	
				EdgeDefectName = EDGE_BROKEN_BOTTOM;
				DefectCH = true;

				str.Format(_T("BROKEN_B"));
			}
		} 
		else {
			if((((abs)(Corner[E_CORNER_LEFT_TOP].x - pt2.x) < dist) || ((abs)(Corner[E_CORNER_LEFT_BOTTOM].x - pt2.x) < dist)) 
					&& ((Corner[E_CORNER_LEFT_TOP].y <= pt1.y) && (Corner[E_CORNER_LEFT_BOTTOM].y >= pt2.y)))
			{
				//Burr: Left
				EdgeDefectName = EDGE_BURR_LEFT;
				DefectCH = true;

				str.Format(_T("BURR_L"));
			}
			else if((((abs)(Corner[E_CORNER_LEFT_TOP].y - pt2.y) < dist) || ((abs)(Corner[E_CORNER_RIGHT_TOP].y - pt2.y) < dist))
				&&((Corner[E_CORNER_LEFT_TOP].x <= pt1.x) && (Corner[E_CORNER_RIGHT_TOP].x >= pt2.x)))
			{
				//Burr: Top	
				EdgeDefectName = EDGE_BURR_TOP;
				DefectCH = true;

				str.Format(_T("BURR_T"));
			}
			else if((((abs)(Corner[E_CORNER_RIGHT_TOP].x - pt1.x - PADSize) < dist)||((abs)(Corner[E_CORNER_RIGHT_BOTTOM].x - pt1.x - PADSize) < dist))
				&& ((Corner[E_CORNER_RIGHT_TOP].y <= pt1.y) && (Corner[E_CORNER_RIGHT_BOTTOM].y >= pt2.y)))
			{
				//Burr: Right	
				EdgeDefectName = EDGE_BURR_RIGTHT;
				DefectCH = true;

				str.Format(_T("BURR_R"));
			}
			else if((((abs)(Corner[E_CORNER_LEFT_BOTTOM].y - pt1.y) < dist)||((abs)(Corner[E_CORNER_RIGHT_BOTTOM].y - pt1.y) < dist))	
			&&((Corner[E_CORNER_LEFT_BOTTOM].x <= pt1.x) && (Corner[E_CORNER_RIGHT_BOTTOM].x >= pt2.x)))
			{
				//Burr: Bottom	
				EdgeDefectName = EDGE_BURR_BOTTOM;
				DefectCH = true;

				str.Format(_T("BURR_B"));
			}
		}
			
		sprintf_s(test, (LPCSTR)CStringA(str));

		if(DefectCH != false)
		{
			cvRectangle(img, pt1, pt2, CV_RGB(255,0,255),1);
			CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 ); 
			cvDrawContours(img, c, color, color, 0, 1, 8);
			cvPutText(img, test, cvPoint(pt2.x+30,pt2.y+30), &font, cvScalar(255,0,255,0));

			DefectCH = false;
		}
	}
	
	return true;
}

bool CInspectEdge::FillPoly(IplImage* img)
{
	IplImage* gray = cvCreateImage(cvGetSize(img), 8, 1);
	cvCopy(img, gray);
	
	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* firstContour = NULL;
	cvFindContours(gray, storage, &firstContour, sizeof(CvContour), CV_RETR_EXTERNAL);
	
	for(CvSeq* c = firstContour; c!= NULL;  c= c->h_next)
	{
		double area = cvContourArea( c,CV_WHOLE_SEQ );
		cout<<area<<endl;

		CvPoint *point = new CvPoint[c->total];
		CvPoint *Point;

		for (int i = 0;i<c->total;i++)
		{
			Point = (CvPoint*)cvGetSeqElem(c,i);
			point[i].x =Point->x;
			point[i].y = Point->y;
		}

		int pts[1] = {c->total};
		cvFillPoly(img,&point,pts,1,CV_RGB(255, 255, 255));
	}

	cvReleaseImage(&gray);
	return true;
}

struct CONTOUR {
	CvContour* c;
	int Area;
};

bool Compare( CONTOUR a, CONTOUR b )
{
	return a.Area > b.Area;    // 오름차순 or return  a > b(내림차순)
}

bool CInspectEdge::BrokenDefectResult(IplImage* gray, IplImage* defectImg, CvPoint Corner[4])
{
	IplImage* imageTemp = cvCreateImage(cvGetSize(gray), 8, 1);
	cvCopy(gray, imageTemp);

	IplImage* CornerLine = cvCreateImage(cvGetSize(gray), 8, 1);
	cvZero(CornerLine);

	IplImage* DeleteLine = cvCreateImage(cvGetSize(gray), 8 , 1);
	cvZero(DeleteLine);

	//1) 후보군 Edge Line
	for(int nCornerInx = E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
	{
		cvLine(CornerLine, Corner[nCornerInx], Corner[(nCornerInx + 1 < E_CORNER_END) ? nCornerInx + 1 : 0], CV_RGB(255,255,255), 2, 8);
	}
	
	cvAdd(imageTemp, CornerLine, imageTemp);

	cvSaveImage("C:\\IDB\\Add.bmp", imageTemp);

	//2) Broken line 찾기: 최외곽으로부터 2번째 Line
	CvSeq* contour = NULL;
	CvMemStorage* storage = cvCreateMemStorage();
	int n = cvFindContours(imageTemp, storage, &contour, sizeof(CvContour), CV_RETR_LIST);
	cout<<n<<endl;
	
	vector<CONTOUR> vec_c;
	for(CvSeq* c = contour; c!= NULL;  c= c->h_next)  {  
		CONTOUR Cont;
		Cont.c = (CvContour*)c;
		Cont.Area = cvContourArea( c, CV_WHOLE_SEQ );
		vec_c.push_back(Cont);
	}  
	sort(vec_c.begin(), vec_c.end(), Compare); //면적 비교 내림차순
	
	//3) Broken Defect
	IplImage* BrokenLine = cvCreateImage(cvGetSize(gray), 8, 1);
	cvZero(BrokenLine);
	cvDrawContours(BrokenLine, (CvSeq*)vec_c[1].c, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, 1, 8);
	
	cvSaveImage("C:\\IDB\\contours.bmp", BrokenLine);

	IplImage* BrokenResult = cvCreateImage(cvGetSize(BrokenLine), 8, 1);
	cvZero(BrokenResult);

	//외곽선 제거
	cvSub(BrokenLine, CornerLine, BrokenResult);
	cvSaveImage("C:\\IDB\\Sub.bmp", BrokenResult);

	//FillConvexhull(BrokenResult, defectImg);

	cvReleaseImage(&CornerLine);
	cvReleaseImage(&DeleteLine);
	cvReleaseImage(&BrokenLine);
	cvReleaseImage(&BrokenResult);
	cvReleaseImage(&imageTemp);

	return true;
}

void CInspectEdge::FillConvexhull(IplImage* src, IplImage* dst)
{
	ASSERT(src != NULL);
	ASSERT(src->nChannels == 1);		

	IplImage* convexImage = cvCloneImage(src);
	CvSeq* hull=NULL;	

	int cnt = 0;
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contours = NULL;
	cvFindContours(convexImage, storage, &contours, sizeof (CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);	
	cvZero(convexImage);
	for(CvSeq* c = contours; c != NULL; c = c->h_next)
	{
		//CvMemStorage* mem_storage = cvCreateMemStorage( 0);
		hull = cvConvexHull2(c, 0, CV_CLOCKWISE, 1);
		cvDrawContours(convexImage, hull, cvScalarAll(255), cvScalarAll(255), -1, CV_FILLED, 8);

	}
	
	cvCopy(convexImage, dst);

	cvReleaseImage(&convexImage);
	cvReleaseMemStorage(&storage);
}

bool CInspectEdge::BurrDefectResult(IplImage* gray, IplImage* defectImg, CvPoint Corner[4])
{
	IplImage* imageTemp = cvCreateImage(cvGetSize(gray), 8, 1);
	cvCopy(gray, imageTemp);

	IplImage* CornerLine = cvCreateImage(cvGetSize(gray), 8, 1);
	cvZero(CornerLine);

	//1) Edge Line
	cvLine(CornerLine,	Corner[E_CORNER_LEFT_TOP]		,Corner[E_CORNER_RIGHT_TOP],			CV_RGB(255, 255, 255), 3, 8);
	cvLine(CornerLine,	Corner[E_CORNER_LEFT_TOP]		,Corner[E_CORNER_LEFT_BOTTOM],			CV_RGB(255, 255, 255), 3, 8);
	cvLine(CornerLine,	Corner[E_CORNER_RIGHT_TOP]		,Corner[E_CORNER_RIGHT_BOTTOM],		CV_RGB(255, 255, 255), 3, 8);
	cvLine(CornerLine,	Corner[E_CORNER_LEFT_BOTTOM]	,Corner[E_CORNER_RIGHT_BOTTOM],		CV_RGB(255, 255, 255), 3, 8);
	cvAdd(imageTemp, CornerLine, imageTemp);
	
	//2) Burr line 찾기: 최외곽으로부터 1번째 Line
	CvSeq* contour = NULL;
	CvMemStorage* storage = cvCreateMemStorage();
	int n = cvFindContours(imageTemp, storage, &contour, sizeof(CvContour), CV_RETR_LIST);
	cout<<n<<endl;
	
	vector<CONTOUR> vec_c;
	for(CvSeq* c = contour; c!= NULL;  c= c->h_next)  {  
		CONTOUR Cont;
		Cont.c = (CvContour*)c;
		Cont.Area = cvContourArea( c, CV_WHOLE_SEQ );
		vec_c.push_back(Cont);
	}  
	sort(vec_c.begin(), vec_c.end(), Compare);


	//3) Burr Defect
	IplImage* BurrLine = cvCreateImage(cvGetSize(gray), 8, 1);
	cvZero(BurrLine);
	cvDrawContours(BurrLine, (CvSeq*)vec_c[0].c, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, 1, 8);
	FillPoly(BurrLine); 
	
	IplImage* CellArea = cvCreateImage(cvGetSize(gray), 8, 1);
	cvZero(CellArea);
	cvLine(CellArea,	Corner[E_CORNER_LEFT_TOP]		,Corner[E_CORNER_RIGHT_TOP],			CV_RGB(255, 255, 255), 4, 8);
	cvLine(CellArea,	Corner[E_CORNER_LEFT_TOP]		,Corner[E_CORNER_LEFT_BOTTOM],		CV_RGB(255, 255, 255), 4, 8);
	cvLine(CellArea,	Corner[E_CORNER_RIGHT_TOP]		,Corner[E_CORNER_RIGHT_BOTTOM],		CV_RGB(255, 255, 255), 4, 8);
	cvLine(CellArea,	Corner[E_CORNER_LEFT_BOTTOM]	,Corner[E_CORNER_RIGHT_BOTTOM],		CV_RGB(255, 255, 255), 4, 8);
	FillPoly(CellArea); 

	cvSub(BurrLine, CellArea, defectImg);

	cvReleaseImage(&imageTemp);
	cvReleaseImage(&CornerLine);
	cvReleaseImage(&BurrLine);
	cvReleaseImage(&CellArea);
	
	return true;
}

long CInspectEdge::CCUT_AllocRefImg(Mat* src, STRU_INFO_PAD** stPadInfo, int* nUseCorner, int nCameraNum, Rect rtPanel, Point* ptDefaultFiducial)
{
	int nRefNum = E_CORNER_END * 2;

	for(int nRefInx = 0; nRefInx < nRefNum; nRefInx++)
	{
		Rect rtRef = stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetRectCoord(nRefInx);
		if(rtRef.x < rtPanel.x + rtPanel.width/2)
		{
			if(rtRef.y < rtPanel.y + rtPanel.height/2)
			{
				if(src[E_CORNER_LEFT_TOP].empty())
				{
					src[E_CORNER_LEFT_TOP] = cvarrToMat(stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetImage(nRefInx));
					ptDefaultFiducial[E_CORNER_LEFT_TOP] = stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetPolygonCoord(nRefInx)[0];
				}
				else
				{
					src[E_CORNER_LEFT_TOP + E_CORNER_END] = cvarrToMat(stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetImage(nRefInx));
					ptDefaultFiducial[E_CORNER_LEFT_TOP + E_CORNER_END] = stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetPolygonCoord(nRefInx)[0];
				}
			}
			else
			{
				if(src[E_CORNER_LEFT_BOTTOM].empty())
				{
					src[E_CORNER_LEFT_BOTTOM] = cvarrToMat(stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetImage(nRefInx));
					ptDefaultFiducial[E_CORNER_LEFT_BOTTOM] = stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetPolygonCoord(nRefInx)[0];
				}
				else
				{
					src[E_CORNER_LEFT_BOTTOM + E_CORNER_END] = cvarrToMat(stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetImage(nRefInx));
					ptDefaultFiducial[E_CORNER_LEFT_BOTTOM + E_CORNER_END] = stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetPolygonCoord(nRefInx)[0];
				}
			}
		}
		else
		{
			if(rtRef.y < rtPanel.y + rtPanel.height/2)
			{
				if(src[E_CORNER_RIGHT_TOP].empty())
				{
					src[E_CORNER_RIGHT_TOP] = cvarrToMat(stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetImage(nRefInx));
					ptDefaultFiducial[E_CORNER_RIGHT_TOP] = stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetPolygonCoord(nRefInx)[0];
				}
				else
				{
					src[E_CORNER_RIGHT_TOP + E_CORNER_END] = cvarrToMat(stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetImage(nRefInx));
					ptDefaultFiducial[E_CORNER_RIGHT_TOP + E_CORNER_END] = stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetPolygonCoord(nRefInx)[0];
				}
			}
			else
			{
				if(src[E_CORNER_RIGHT_BOTTOM].empty())
				{
					src[E_CORNER_RIGHT_BOTTOM] = cvarrToMat(stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetImage(nRefInx));
					ptDefaultFiducial[E_CORNER_RIGHT_BOTTOM] = stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetPolygonCoord(nRefInx)[0];
				}
				else
				{
					src[E_CORNER_RIGHT_BOTTOM + E_CORNER_END] = cvarrToMat(stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetImage(nRefInx));
					ptDefaultFiducial[E_CORNER_RIGHT_BOTTOM + E_CORNER_END] = stPadInfo[E_PAD_FIDUCIAL][nCameraNum].GetPolygonCoord(nRefInx)[0];
				}
			}
		}

	}
	
	return E_ERROR_CODE_TRUE;
}

bool CInspectEdge::CCUTLineExtract(Mat src, CCUTInfo& cCUTInfo)
{

	return true;
}

long CInspectEdge::FindCCutMark(Mat src, Mat& dst, Mat mtCCutMark, int nMarkThreshold, double dMarkSize, cv::Rect2f& artMarkMainPos, cv::Rect& artMarkSearchArea, int nCornerInx, int nCornerOffset, bool bUseMatchingCCutMark, Point ptDefaultFiducial)
{
	
	BOOL bSearchMark = TRUE;
	
	bSearchMark = FindCCutMark_Matching(src, dst, mtCCutMark, nMarkThreshold, dMarkSize, artMarkMainPos, artMarkSearchArea, ptDefaultFiducial);
	
	return bSearchMark;
}

long CInspectEdge::FindCCutMrak_Thresh(Mat src, Mat& dst, int nMarkThreshold, double dMarkSize, cv::Rect2f& artMarkMainPos, cv::Rect& artMarkSearchArea, int nCornerInx, int nCornerOffset)
{
	BOOL bSearchMark = TRUE;
	Rect rtCheckArea = Rect(0,0, dst.cols, dst.rows);

	Rect rtCornerROI;
	int nTotalLabel;
	cv::Mat matLabel, matStats, matCentroid;
	long	nBsetIdx = 1;

	src.copyTo(dst);

	switch(nCornerInx)
	{
	case E_CORNER_LEFT_TOP:
		rtCornerROI = Rect(0, 0, nCornerOffset, nCornerOffset);
		break;
	case E_CORNER_LEFT_BOTTOM:
		rtCornerROI = Rect(0, dst.rows - nCornerOffset, nCornerOffset, nCornerOffset);
		break;
	case E_CORNER_RIGHT_TOP:
		rtCornerROI = Rect(dst.cols - nCornerOffset, 0, nCornerOffset, nCornerOffset);
		break;
	case E_CORNER_RIGHT_BOTTOM:
		rtCornerROI = Rect(dst.cols - nCornerOffset, dst.rows - nCornerOffset, nCornerOffset, nCornerOffset);
		break;
	}

	GetCheckROIOver(rtCornerROI, rtCheckArea, rtCornerROI);

	Mat mtFiducialMarkCorner;
	dst(rtCornerROI).copyTo(mtFiducialMarkCorner);

	cv::threshold(mtFiducialMarkCorner, mtFiducialMarkCorner, NULL, 255, cv::THRESH_OTSU);

	nTotalLabel = cv::connectedComponentsWithStats(mtFiducialMarkCorner, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) ;

	int nSInx=0;
	double dArea=0;

	if(nTotalLabel > 2)
	{
		for(int nInx =1; nInx < nTotalLabel; nInx++)
		{
			Rect rt;
			rt.width = matStats.at<int>(nInx, CC_STAT_WIDTH);
			rt.height = matStats.at<int>(nInx, CC_STAT_HEIGHT);

			if(dArea < rt.width * rt.height)
			{
				dArea = rt.width * rt.height;
				nBsetIdx = nInx;
			}
		}
	}

	int	nCCutMarkROI_Offet=3;
	Rect rtCCutMarkROI;
	Mat mtCCutMark;
	int nWHOffset=3;
	int	nWidth = dMarkSize+nCCutMarkROI_Offet+nWHOffset;
	int nHeight = dMarkSize+nCCutMarkROI_Offet+nWHOffset;
	switch(nCornerInx)
	{
	case E_CORNER_LEFT_TOP:
		rtCCutMarkROI = Rect(matStats.at<int>(nBsetIdx, CC_STAT_LEFT) + rtCornerROI.x - nCCutMarkROI_Offet, matStats.at<int>(nBsetIdx, CC_STAT_TOP) + rtCornerROI.y - nCCutMarkROI_Offet, 
			nWidth, nHeight);
		break;
	case E_CORNER_LEFT_BOTTOM:
		rtCCutMarkROI = Rect(matStats.at<int>(nBsetIdx, CC_STAT_LEFT) + rtCornerROI.x-nCCutMarkROI_Offet, matStats.at<int>(nBsetIdx, CC_STAT_HEIGHT) + rtCornerROI.y - dMarkSize - nCCutMarkROI_Offet , 
			nWidth, nHeight);
		break;
	case E_CORNER_RIGHT_TOP:
		rtCCutMarkROI = Rect(matStats.at<int>(nBsetIdx, CC_STAT_WIDTH) + rtCornerROI.x - dMarkSize -nCCutMarkROI_Offet, matStats.at<int>(nBsetIdx, CC_STAT_TOP) + rtCornerROI.y - nCCutMarkROI_Offet , 
			nWidth, nHeight);
		break;
	case E_CORNER_RIGHT_BOTTOM:
		rtCCutMarkROI = Rect(matStats.at<int>(nBsetIdx, CC_STAT_WIDTH) + rtCornerROI.x - dMarkSize -nCCutMarkROI_Offet, matStats.at<int>(nBsetIdx, CC_STAT_HEIGHT) + rtCornerROI.y - dMarkSize - nCCutMarkROI_Offet , 
			nWidth, nHeight);
		break;
	}

	GetCheckROIOver(rtCCutMarkROI, rtCheckArea, rtCCutMarkROI);
	dst(rtCCutMarkROI).copyTo(mtCCutMark);

	cv::threshold(mtCCutMark, mtCCutMark, NULL, 255, CV_THRESH_OTSU);
	nTotalLabel = cv::connectedComponentsWithStats(mtCCutMark, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) ;

	float	fElongation = 100;
	float	fLableRatio = 0;				//Lalbe Rect안의 Object 비율

	dArea=0;

	for(long nIdx = 1 ;  nIdx < nTotalLabel ; nIdx ++)
	{
		Rect rt2;
		rt2.x		=	matStats.at<int>(nIdx, CC_STAT_LEFT);
		rt2.y		=	matStats.at<int>(nIdx, CC_STAT_TOP);
		rt2.width	=	matStats.at<int>(nIdx, CC_STAT_WIDTH);
		rt2.height	=	matStats.at<int>(nIdx, CC_STAT_HEIGHT);

		if(dArea < rt2.width*rt2.height)
		{
			dArea=rt2.width*rt2.height;
			nBsetIdx = nIdx;
		}
	}


	artMarkMainPos.x			=	matStats.at<int>(nBsetIdx, CC_STAT_LEFT) + rtCCutMarkROI.x + artMarkSearchArea.x;
	artMarkMainPos.y			=	matStats.at<int>(nBsetIdx, CC_STAT_TOP) + rtCCutMarkROI.y	 + artMarkSearchArea.y;
	artMarkMainPos.width		=	matStats.at<int>(nBsetIdx, CC_STAT_WIDTH);
	artMarkMainPos.height	=		matStats.at<int>(nBsetIdx, CC_STAT_HEIGHT);

	return bSearchMark;

}
long CInspectEdge::FindCCutMark_Matching(Mat src, Mat& dst, Mat mtCCutMark,int nMarkThreshold, double dMarkSize, cv::Rect2f& artMarkMainPos, cv::Rect& artMarkSearchArea, Point ptDefaultFiducial)
{
	src.copyTo(dst);

	Mat mtMatchingResult;

	double dMatchRate;
	cv::Point ptMatch;		//Matching 해서 나온 정수 좌표
	cv::Point2f ptMatch_Sub;	//SubPixel 보정들어간 실수 좌표

	Edge_BasicSobel(dst,dst);
	Edge_BasicSobel(mtCCutMark, mtCCutMark);

	cv::matchTemplate(dst, mtCCutMark, mtMatchingResult, CV_TM_CCOEFF_NORMED);
	cv::minMaxLoc(mtMatchingResult, NULL, &dMatchRate, NULL, &ptMatch);

	if(dMatchRate < 0.85)
	{
		artMarkMainPos.x			=	ptDefaultFiducial.x - mtCCutMark.cols/2 + artMarkSearchArea.x;
		artMarkMainPos.y			=	ptDefaultFiducial.y - mtCCutMark.rows/2 + artMarkSearchArea.y;
		artMarkMainPos.width		=	mtCCutMark.cols;
		artMarkMainPos.height		=	mtCCutMark.rows;
	}
	else
	{
		float* fMatchScore_arr = (float*)mtMatchingResult.ptr(ptMatch.y);
		double fMatchScore;
		//X 방향 Matching Rate Line Pitting
		vector<Point2f> ptMatchScore_X, ptMatchScore_Y;

		for(int nX = ptMatch.x-2; nX <= ptMatch.x+2; nX++)
		{

			if(nX < 0 || nX > mtMatchingResult.cols-1) continue;
			else	fMatchScore = *(fMatchScore_arr+nX)*100;

			ptMatchScore_X.push_back(Point2f(nX, fMatchScore));
		}

		for(int nY = ptMatch.y-2; nY <= ptMatch.y+2; nY++)
		{
			if(nY < 0 || nY > mtMatchingResult.rows-1)	continue;
			else{
				fMatchScore_arr = (float*)mtMatchingResult.ptr(nY);
				fMatchScore = *(fMatchScore_arr+ptMatch.x)*100;
			}
			ptMatchScore_Y.push_back(Point2f(nY, fMatchScore));
		}

		//곡선 Pitting 상수
		long double dA,dB,dC;
		long double dX_Maxpt, dY_Maxpt;

		if(calcCurveFit(ptMatchScore_X, dA, dB, dC)==-1)	return FALSE;
		dX_Maxpt = -1*(dB/(dA*2));

		if(calcCurveFit(ptMatchScore_Y, dA, dB, dC)==-1)	return FALSE;
		dY_Maxpt = -1*(dB/(dA*2));

		ptMatch_Sub = Point2f(dX_Maxpt, dY_Maxpt); 

		artMarkMainPos.x			=	ptMatch_Sub.x + artMarkSearchArea.x;
		artMarkMainPos.y			=	ptMatch_Sub.y + artMarkSearchArea.y;
		artMarkMainPos.width		=	mtCCutMark.cols;
		artMarkMainPos.height		=	mtCCutMark.rows;
	}
	

	return TRUE;
}
	

bool CInspectEdge::Inspection_CompareCCUTBrokenArea(const IplImage* const img, IplImage* BurrImg, const CCUTInfo cCUTInfo, const int nSerachSize )
{
	int PixelOffset = 2;

// 	LT
// 		if(cCUTInfo.GetUse(E_CORNER_LEFT_TOP))
// 		{
// 			int nLeftTopS = cCUTInfo.cCut_start[E_CORNER_LEFT_TOP].y	+	PixelOffset;
// 			int nLeftTopE = cCUTInfo.cCut_end[E_CORNER_LEFT_TOP].y;
// 	
// 			for(int h =  nLeftTopS; h < nLeftTopE; h++) 
// 			{
// 				int sw = ((h - cCUTInfo.cCutLine[E_CORNER_LEFT_TOP].B)/cCUTInfo.cCutLine[E_CORNER_LEFT_TOP].A) + PixelOffset;
// 				int ew = sw + nSerachSize;
// 	
// 				for(int w = sw; w < ew; w++)
// 				{
// 					if((uchar)img->imageData[h*img->widthStep+w] < 100)	
// 						BurrImg->imageData[h*img->widthStep+w] = (uchar)255;
// 				}
// 			}
// 		}
// 	
// 		//	LB
// 		if(cCUTInfo.GetUse(E_CORNER_LEFT_BOTTOM))
// 		{
// 			int nLeftBottomS = cCUTInfo.cCut_start[E_CORNER_LEFT_BOTTOM].y;
// 			int nLeftBottomE = cCUTInfo.cCut_end[E_CORNER_LEFT_BOTTOM].y - PixelOffset;
// 	
// 			for(int h =  nLeftBottomS; h < nLeftBottomE ; h++)
// 			{
// 				int sw = ((h - cCUTInfo.cCutLine[E_CORNER_LEFT_BOTTOM].B)/cCUTInfo.cCutLine[E_CORNER_LEFT_BOTTOM].A) + PixelOffset;
// 				int ew = sw + nSerachSize;
// 	
// 				for(int w = sw; w < ew; w++)
// 				{
// 					if((uchar)img->imageData[h*img->widthStep+w] < 100)	
// 						BurrImg->imageData[h*img->widthStep+w] = (uchar)255;
// 				}
// 			}
// 		}
// 	
// 		//	RT
// 		if(cCUTInfo.GetUse(E_CORNER_RIGHT_TOP))
// 		{
// 			int nRightTopS =	cCUTInfo.cCut_start[E_CORNER_RIGHT_TOP].y+PixelOffset;
// 			int nRightTopE =	cCUTInfo.cCut_end[E_CORNER_RIGHT_TOP].y;
// 	
// 			for(int h =  nRightTopS; h < nRightTopE; h++) 
// 			{
// 				int sw = ((h - cCUTInfo.cCutLine[E_CORNER_RIGHT_TOP].B)/cCUTInfo.cCutLine[E_CORNER_RIGHT_TOP].A) - PixelOffset;
// 				int ew = sw - nSerachSize;
// 	
// 				for(int w = sw; w>ew; w--)	
// 				{
// 					if((uchar)img->imageData[h*img->widthStep+w] < 100)	
// 						BurrImg->imageData[h*img->widthStep+w] = (uchar)255;
// 				}
// 			}
// 	
// 		}
// 	
// 		//	RB
// 		if(cCUTInfo.GetUse(E_CORNER_RIGHT_BOTTOM))
// 		{
// 			int nRightBottomS =	cCUTInfo.cCut_start[E_CORNER_RIGHT_BOTTOM].y ;
// 			int nRightBottomE =	cCUTInfo.cCut_end[E_CORNER_RIGHT_BOTTOM].y - PixelOffset;
// 	
// 			for(int h =  nRightBottomS; h < nRightBottomE; h++) 
// 			{
// 				int sw = ((h - cCUTInfo.cCutLine[E_CORNER_RIGHT_BOTTOM].B)/cCUTInfo.cCutLine[E_CORNER_RIGHT_BOTTOM].A) - PixelOffset;
// 				int ew = sw - nSerachSize;
// 	
// 				for(int w = sw; w>ew; w--)	
// 				{
// 					if((uchar)img->imageData[h*img->widthStep+w] < 100)	
// 						BurrImg->imageData[h*img->widthStep+w] = (uchar)255;
// 				}
// 			}
// 		}

	return true;
}


bool CInspectEdge::CCUTDefectResult(IplImage* gray, IplImage* defectImg, CCUTInfo cCUTInfo)
{
	IplImage* imageTemp = cvCreateImage(cvGetSize(gray), 8, 1);
	cvCopy(gray, imageTemp);

	IplImage* CCUTLine = cvCreateImage(cvGetSize(gray), 8, 1);
	cvZero(CCUTLine);

// 	1) 후보군 C-CUT Line
// 	
// 		if(cCUTInfo.GetUse(E_CORNER_LEFT_TOP))
// 			cvLine(CCUTLine,	cCUTInfo.cCut_start[E_CORNER_LEFT_TOP]	,cCUTInfo.cCut_end[E_CORNER_LEFT_TOP],	CV_RGB(255, 255, 255), 2, 8);
// 	
// 		if(cCUTInfo.GetUse(E_CORNER_LEFT_BOTTOM))
// 			cvLine(CCUTLine,	cCUTInfo.cCut_start[E_CORNER_LEFT_BOTTOM]	,cCUTInfo.cCut_end[E_CORNER_LEFT_BOTTOM],	CV_RGB(255, 255, 255), 2, 8);
// 	
// 		if(cCUTInfo.GetUse(E_CORNER_RIGHT_TOP))
// 			cvLine(CCUTLine,	cCUTInfo.cCut_start[E_CORNER_RIGHT_TOP]	,cCUTInfo.cCut_end[E_CORNER_RIGHT_TOP],	CV_RGB(255, 255, 255), 2, 8);
// 	
// 		if(cCUTInfo.GetUse(E_CORNER_RIGHT_BOTTOM))
// 			cvLine(CCUTLine,	cCUTInfo.cCut_start[E_CORNER_RIGHT_BOTTOM]	,cCUTInfo.cCut_end[E_CORNER_RIGHT_BOTTOM],	CV_RGB(255, 255, 255), 2, 8);
// 	
// 	
// 		cvAdd(imageTemp, CCUTLine, imageTemp);
// 	
// 		//2) Defect Sorting: 최외곽으로부터 1, 2번째 C-CUT RT, RB Line
// 		CvSeq* contour = NULL;
// 		CvMemStorage* storage = cvCreateMemStorage();
// 		int n = cvFindContours(imageTemp, storage, &contour, sizeof(CvContour), CV_RETR_LIST);
// 		
// 		vector<CONTOUR> vec_c;
// 		for(CvSeq* c = contour; c!= NULL;  c= c->h_next)  {  
// 			CONTOUR Cont;
// 			Cont.c = (CvContour*)c;
// 			Cont.Area = cvContourArea( c, CV_WHOLE_SEQ );
// 			vec_c.push_back(Cont);
// 		}  
// 		sort(vec_c.begin(), vec_c.end(), Compare);
// 		
// 		//3) C-CUT RT, RB Defect
// 		IplImage* DefectLine = cvCreateImage(cvGetSize(gray), 8, 1);
// 		cvZero(DefectLine);
// 		for(int i=0; i<2; i++){
// 			cvDrawContours(DefectLine, (CvSeq*)vec_c[i].c, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 0, 1, 8);
// 		}
// 		FillPoly(DefectLine);
// 		
// 		IplImage* DeleteLine = cvCreateImage(cvGetSize(gray), 8, 1);
// 		cvZero(DeleteLine);
// 	
// 	
// 		if(cCUTInfo.GetUse(E_CORNER_LEFT_TOP))
// 			cvLine(DeleteLine,	cCUTInfo.cCut_start[E_CORNER_LEFT_TOP]	,cCUTInfo.cCut_end[E_CORNER_LEFT_TOP],	CV_RGB(255, 255, 255), 2, 8);
// 	
// 		if(cCUTInfo.GetUse(E_CORNER_LEFT_BOTTOM))
// 			cvLine(DeleteLine,	cCUTInfo.cCut_start[E_CORNER_LEFT_BOTTOM]	,cCUTInfo.cCut_end[E_CORNER_LEFT_BOTTOM],	CV_RGB(255, 255, 255), 2, 8);
// 	
// 		if(cCUTInfo.GetUse(E_CORNER_RIGHT_TOP))
// 			cvLine(DeleteLine,	cCUTInfo.cCut_start[E_CORNER_RIGHT_TOP]	,cCUTInfo.cCut_end[E_CORNER_RIGHT_TOP],	CV_RGB(255, 255, 255), 2, 8);
// 	
// 		if(cCUTInfo.GetUse(E_CORNER_RIGHT_BOTTOM))
// 			cvLine(DeleteLine,	cCUTInfo.cCut_start[E_CORNER_RIGHT_BOTTOM]	,cCUTInfo.cCut_end[E_CORNER_RIGHT_BOTTOM],	CV_RGB(255, 255, 255), 2, 8);
// 	
// 	
// 		
// 		cvSub(DefectLine, DeleteLine, defectImg);
	
// 	cvReleaseImage(&imageTemp);
// 	cvReleaseImage(&CCUTLine);
// 	cvReleaseImage(&DefectLine);

	return true;
}

long CInspectEdge::FindCCutDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner,  double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
STRU_DEFECT_ITEM* EngineerDefectJudgment, CString strCurModelPath, stDefectInfo* pResultBlob, stMeasureInfo* pMeasureInfo, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt)
{
	long nErrorcode;

		nErrorcode = FindCCutDefect_MarkLine(matSrcBuffer, matDrawBuffer, ptCorner, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment, strCurModelPath, pResultBlob, pMeasureInfo, stPadInfo, strContourTxt);

	return nErrorcode;
}

long CInspectEdge::FindCCutDefect_MarkLine(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner,  double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, CString strCurModelPath, stDefectInfo* pResultBlob, stMeasureInfo* pMeasureInfo, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt)
{
		// Algorithm parameter
	const int Bluring						= 3;
	const int DefectDistance				= 1;				// 좌표에 따른 Defect 구분
	
	double dCameraResolution = pMeasureInfo->dCameraResolution;

	const int nCalcMaxNumber				= E_CORNER_END * 2;
	int nMeasureValueCnt					= 0;				//측정 Data 배열 포인터용	

	bool	bImageSave			=	(nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;	// 1 : 알고리즘 중간결과 Image Save 
	int		nCameraNum_			=	nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];

	CString strImageSave;

	bool	arbMarkUse[nCalcMaxNumber];
	Rect	artMarkSearchArea[nCalcMaxNumber];				//C-Cut FIducial Mark를 찾기 위한 영역

	bool	arbSearchOK[nCalcMaxNumber];
	Rect2f  artMarkSubPos[nCalcMaxNumber];		// 부분 영상 검색 위치
	Rect2f  artMarkMainPos[nCalcMaxNumber];	// 전체 영상 기준 위치 


	// 불량 검출에 필요한 최종 정보
	Point  arpDefectMarkPos[nCalcMaxNumber];		// artDefectSearch 영역 안에서의 마크 위치 
	Rect	artDefectSearch[E_CORNER_END];		// 불량 검출 영역 

	memset(arbSearchOK			, 0, sizeof(bool)	 * nCalcMaxNumber ) ;
	memset(arbMarkUse			, 0, sizeof(bool)	 * nCalcMaxNumber ) ;
	memset(artMarkSearchArea	, 0, sizeof(Rect)	 * nCalcMaxNumber ) ;
	memset(artMarkSubPos		, 0, sizeof(Rect2f)  * nCalcMaxNumber ) ;
	memset(artMarkMainPos		, 0, sizeof(Rect2f)  * nCalcMaxNumber ) ;

	//memset(arptLineData			, 0, sizeof(Point2d) * E_CORNER_END ) ;
	memset(artDefectSearch		, 0, sizeof(Rect)	 * E_CORNER_END ) ;
	memset(arpDefectMarkPos		, 0, sizeof(Point)	 * nCalcMaxNumber ) ;
	
	int nPos = 0;
	
	//현재 미사용/////////////////////////////////////////////////////////////
	int nCornerOffset											=		0;
	bool bUseMatchingCCutMark									=		1;
	/////////////////////////////////////////////////////////////////////////


	bool			bBMLine										=		dPara[nPos++] > 0 ? true : false;	
	const	float	fCellThreshold_Offset						=		(float)dPara[nPos++];	//Panel Blur 영상에서 평균값의 Offset값
	const	int		nMarkThreshold								=		(int)dPara[nPos++];		//Mark 임계값
	const   int		nMarkSearchSize								=		(int)dPara[nPos++];		//Mark 검색 ROI 크기
	const	float	fMatchingRate								=		(float)dPara[nPos++];		//Mark 탐색 시 사용하는 Pattern Matching율 허용 범위
	
	const	int		nSearch_ROI_Size							=		(int)dPara[nPos++];		//Panel 모서리 부터 패널 방향으로 들어갈 ROI SIze

	//C-Cut 사용 Corner 여부
	int		nUseCorner[E_CORNER_END];
	nUseCorner[E_CORNER_LEFT_TOP]								=		(int)dPara[nPos++];		//Left Top		
	nUseCorner[E_CORNER_LEFT_BOTTOM]							=		(int)dPara[nPos++];		//Left Bottom
	nUseCorner[E_CORNER_RIGHT_TOP]								=		(int)dPara[nPos++];		//Right Top
	nUseCorner[E_CORNER_RIGHT_BOTTOM]							=		(int)dPara[nPos++];		//Right Bottom

	double dMarkSize											=		(double)dPara[nPos++];	//C-Cut Mark Size
	double dRefCutDistance										=		(double)dPara[nPos++];	//C-Cut Cutting 기준 길이 현재 100um
	dMarkSize = dMarkSize / dCameraResolution;
	dRefCutDistance = dRefCutDistance/dCameraResolution;

	const int nCutLineOffset									=		(int)dPara[nPos++];		//C-Cut Line, Panel Line의 교점에서 얼마만큼 버리고 측정할 것인지
							
	int nBMThreshold[E_CORNER_END];
	for(int nCornerInx = E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
	{
		nBMThreshold[nCornerInx]								= (int)dPara[nPos++];		
	}
	int fBMposition[E_CORNER_END];
	for(int nCornerInx = E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
	{
		fBMposition[nCornerInx]								= (int)dPara[nPos++];		
	}
	
	const int nBMDetectionSampleNum							= (int)dPara[nPos++];

	int Offset_Height                                       = (int)dPara[nPos++];	//Panel Edge 선을 그대로 사용하면 C-Cut Mark에 딱 붙는 경우 좋지 않음.
	int Offset_Width                                        = (int)dPara[nPos++];  //CutLine만 취득하기 위해 가로 세로 없애는 Offset
	int nIgnore_GV                                          = (int)dPara[nPos++];
	int Morph_Open_Size                                     = (int)dPara[nPos++];
	int Morph_Close_Size                                    = (int)dPara[nPos++];

	if(Morph_Open_Size%2==0)
		Morph_Open_Size++;
	if(Morph_Close_Size%2==0)
		Morph_Close_Size++;

	if(matSrcBuffer.empty())
		return E_ERROR_CODE_EMPTY_BUFFER;

	
	Rect rtCheckArea;						//FOV Over Check 용

	rtCheckArea.x			=	0 ;
	rtCheckArea.y			=	0 ;
	rtCheckArea.width		=	matSrcBuffer.cols ;
	rtCheckArea.height		=	matSrcBuffer.rows ;

	Mat mtTemplate[E_CORNER_END];			//Mark Search ROI 영역으로 Template 매칭을 진행할 Full 영상
	Rect rtSearchROI[E_CORNER_END];			//Mark Search ROI 영역
	Mat	mtCutMark;							//C-CutMark 영상

	

	rtSearchROI[E_CORNER_LEFT_TOP]		= cv::Rect(ptCorner[E_CORNER_LEFT_TOP].x-Offset_Width, ptCorner[E_CORNER_LEFT_TOP].y-Offset_Height,
													nSearch_ROI_Size, nSearch_ROI_Size);
	rtSearchROI[E_CORNER_LEFT_BOTTOM]	= cv::Rect(ptCorner[E_CORNER_LEFT_BOTTOM].x-Offset_Width, ptCorner[E_CORNER_LEFT_BOTTOM].y+Offset_Height - nSearch_ROI_Size,
													nSearch_ROI_Size, nSearch_ROI_Size);
	rtSearchROI[E_CORNER_RIGHT_TOP]	= cv::Rect(ptCorner[E_CORNER_RIGHT_TOP].x + Offset_Width - nSearch_ROI_Size, ptCorner[E_CORNER_RIGHT_TOP].y-Offset_Height,
		nSearch_ROI_Size, nSearch_ROI_Size);
	rtSearchROI[E_CORNER_RIGHT_BOTTOM]	= cv::Rect(ptCorner[E_CORNER_RIGHT_BOTTOM].x + Offset_Width - nSearch_ROI_Size, ptCorner[E_CORNER_RIGHT_BOTTOM].y+Offset_Height - nSearch_ROI_Size,
													nSearch_ROI_Size, nSearch_ROI_Size);
	CString strIDB;	//이미지 디버깅용 경로

	Mat mtMatchingBuffer[E_CORNER_END*2];	//Mark는 2개이기 때문에 버퍼 2배 할당 <Template Matching 사용할 Pattern Buffer>
	Mat mtMatchingResult[E_CORNER_END*2];	//Pattern Matching 결과
	double dMatchRate[E_CORNER_END*2];		//매칭률
	cv::Point ptMatch[E_CORNER_END*2];		//매칭 좌표
	CString strMatchingBuffer;				//Matching 버퍼의 경로
	
	CString strCCutMark;
	strCCutMark.Format(_T("%s\\CCutMark.bmp"), strCurModelPath);
	mtCutMark = imread((cv::String)(CStringA)strCCutMark, -1);

	if(mtCutMark.empty())
		return E_ERROR_CODE_EMPTY_PARA;


	int nUseCornerNum = nUseCorner[E_CORNER_LEFT_TOP] * 2 + nUseCorner[E_CORNER_LEFT_BOTTOM] * 2 + 
						nUseCorner[E_CORNER_RIGHT_TOP] * 2 + nUseCorner[E_CORNER_RIGHT_BOTTOM] * 2;
	
	int nRefNum = stPadInfo[E_PAD_FIDUCIAL][nCameraNum_].GetRoiCount();
	if(nUseCornerNum > nRefNum)
		return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Point ptDefaultFiducial[E_CORNER_END * 2]; //Fiducial Mark 수동으로 Teaching 해둔 위치
	//Reference 영상 할당
	CCUT_AllocRefImg(mtMatchingBuffer, stPadInfo, nUseCorner, nCameraNum_, Rect(ptCorner[E_CORNER_LEFT_TOP], ptCorner[E_CORNER_RIGHT_BOTTOM]), ptDefaultFiducial);

	//1. Pattern Matching 진행
	for(int nCorner =E_CORNER_LEFT_TOP; nCorner < E_CORNER_END; nCorner++)
	{
		if(nUseCorner[nCorner])
		{
			GetCheckROIOver(rtSearchROI[nCorner], rtCheckArea, rtSearchROI[nCorner]);
			mtTemplate[nCorner] = matSrcBuffer(rtSearchROI[nCorner]);
			if (bImageSave)
			{
				strIDB.Format(_T("C:\\IDB\\ROI_%d.bmp"), nCorner);
				cv::imwrite((cv::String)(CStringA)strIDB, mtTemplate[nCorner]);
			}

			//Reference Pattern 영상이 없을 경우 Fail 처리
			if(mtMatchingBuffer[nCorner].empty())
				return E_ERROR_CODE_EMPTY_PARA;
			

			//Mark_1 Pattern Matching
			cv::matchTemplate(mtTemplate[nCorner], mtMatchingBuffer[nCorner], mtMatchingResult[nCorner], CV_TM_CCOEFF_NORMED);
			cv::minMaxLoc(mtMatchingResult[nCorner], NULL, &dMatchRate[nCorner], NULL, &ptMatch[nCorner]);

			//Mark_2 Pattern Matching
			cv::matchTemplate(mtTemplate[nCorner], mtMatchingBuffer[nCorner+E_CORNER_END], mtMatchingResult[nCorner+E_CORNER_END], CV_TM_CCOEFF_NORMED);
			cv::minMaxLoc(mtMatchingResult[nCorner+E_CORNER_END], NULL, &dMatchRate[nCorner+E_CORNER_END], NULL, &ptMatch[nCorner+E_CORNER_END]);

			

			if (bImageSave)	//Matching 위치 확인 용
			{
				Mat mtResult;

				cv::cvtColor(mtTemplate[nCorner], mtResult,CV_GRAY2RGB);

				cv::rectangle(mtResult, Rect(ptMatch[nCorner].x, ptMatch[nCorner].y, mtMatchingBuffer[nCorner].cols, mtMatchingBuffer[nCorner].rows), Scalar(0,255,0));
				cv::rectangle(mtResult, Rect(ptMatch[nCorner+E_CORNER_END].x, ptMatch[nCorner+E_CORNER_END].y, mtMatchingBuffer[nCorner+E_CORNER_END].cols, mtMatchingBuffer[nCorner+E_CORNER_END].rows), Scalar(0,255,0));
				
				CString strTest;
				strTest.Format(_T("C:\\IDB\\Template%d.bmp"), nCorner);

				imwrite((cv::String)(CStringA)strTest, mtResult);
			}

			arbMarkUse[nCorner] = true;
			arbMarkUse[nCorner+E_CORNER_END] = true;
		}
	}


	
	
	//2. C-Cut Fiducial Mark를 Search하기 위한 범위 선정
	for(long n=0; n < nCalcMaxNumber ; n++)
	{
		/*패턴 매칭스코어가 fMatchingRate 이상- Patter Matching을 통해 얻은 좌표로 진행
		                    fMatchingRate 이하- 검사 Fail*/

		Rect rtTtemp;	//C-Cut Mark 영역
		if(arbMarkUse[n])
		{
			if(dMatchRate[n] > fMatchingRate)
			{
				rtTtemp.x				= rtSearchROI[n % E_CORNER_END].x + ptMatch[n].x;
				rtTtemp.y				= rtSearchROI[n % E_CORNER_END].y + ptMatch[n].y;
				rtTtemp.width			= mtMatchingBuffer[n].cols;
				rtTtemp.height			= mtMatchingBuffer[n].rows;

				GetCheckROIOver(rtTtemp, rtCheckArea, rtTtemp);
				artMarkSearchArea[n]	= rtTtemp;
			}
			else
			{
				arbMarkUse[n] = false;
				//return E_ERROR_CODE_CUTTING_MARK_TYPE_PARA;
			}
		}
	}

	//3. 원본으로부터 이미지를 뜯어와서 C-Cut 기준 마크를  찾는다. 
	for(long n=0; n < nCalcMaxNumber ; n++)
	{
		if( arbMarkUse[n] )
		{
			Mat matInspSrc, matInspDes;
			matInspSrc	= matSrcBuffer(artMarkSearchArea[n]);
			matInspDes	= Mat::zeros(matInspSrc.size(), CV_8UC1);
	
			Mat mtCutMarkSrc;
			mtCutMark.copyTo(mtCutMarkSrc);
			arbSearchOK[n]	=	FindCCutMark(matInspSrc, matInspDes, mtCutMarkSrc, nMarkThreshold, dMarkSize, artMarkMainPos[n], artMarkSearchArea[n], n%E_CORNER_END, nCornerOffset, bUseMatchingCCutMark, ptDefaultFiducial[n]) > 0 ? true : false;

			if(bImageSave)
			{
				CString strTemp;
				strTemp.Format(_T("C:\\IDB\\CCut_Corner_Src_%d.bmp"), n); 
				cv::imwrite((cv::String)(CStringA)strTemp, matInspSrc);

				strTemp.Format(_T("C:\\IDB\\CCut_Corner_Binary_%d.bmp"), n); 
				cv::imwrite((cv::String)(CStringA)strTemp, matInspDes);

			}
		}
	}
	
	//C-Cut Mark 위치 표시
	for(long n=0; n < nCalcMaxNumber ; n++)
	{
		if( arbSearchOK[n] && !bBMLine)
		{
			cv::circle(matDrawBuffer, Point(artMarkMainPos[n].x+artMarkMainPos[n].width/2, artMarkMainPos[n].y+artMarkMainPos[n].height/2), 1, Scalar(0,0,255),1);
		}
	}

	if (bImageSave)
	{
		Mat Draw;
		cv::cvtColor(matSrcBuffer, Draw, CV_GRAY2RGB);

		for(long n=0; n < nCalcMaxNumber ; n++)
		{
			if( arbSearchOK[n] )
			{
				Rect rtDraw;

				rtDraw.x		= (int)( artMarkMainPos[n].x) ;
				rtDraw.y		= (int)( artMarkMainPos[n].y) ;
				rtDraw.width	= (int)( artMarkMainPos[n].width) ;
				rtDraw.height	= (int)( artMarkMainPos[n].height) ;

				cv::rectangle(Draw, rtDraw, Scalar(255,0,0));
			}
		}

		CString strTemp;
		strTemp.Format(_T("C:\\IDB\\CCut_Mark Result_FullImage.bmp")); 
		cv::imwrite((cv::String)(CStringA)strTemp, Draw);
	}

	// 불량 검색 영역 계산 커팅지점 2개, 코너점으로 실제 불량을 검색할 영역 지정 
	// 두 커팅 지점에 대한 직선의 방정식 계산 
	const int nClacOffsetX = 20;
	const int nClacOffsetY = 20;
	for(long n=0; n < E_CORNER_END ; n++)
	{
		// 둘 다 검색 되었어야 함!!
		artDefectSearch[n] = rtSearchROI[n];
		if( arbSearchOK[n]  && arbSearchOK[E_CORNER_END + n] )
		{
			arpDefectMarkPos[n].x =  artMarkMainPos[n].x + artMarkMainPos[n].width/2 - rtSearchROI[n].x;	
			arpDefectMarkPos[n].y =  artMarkMainPos[n].y + artMarkMainPos[n].height/2 - rtSearchROI[n].y;	

			arpDefectMarkPos[E_CORNER_END + n].x = artMarkMainPos[E_CORNER_END + n].x + artMarkMainPos[E_CORNER_END + n].width/2 - rtSearchROI[n].x;
			arpDefectMarkPos[E_CORNER_END + n].y = artMarkMainPos[E_CORNER_END + n].y + artMarkMainPos[E_CORNER_END + n].height/2 - rtSearchROI[n].y; 

			if(bImageSave)
			{
				Mat Src,Draw;
				Src = matSrcBuffer(artDefectSearch[n]);
				cv::cvtColor(Src, Draw, CV_GRAY2RGB);

				Point pt1, pt2;
				pt1		=	arpDefectMarkPos[n];
				pt2		=	arpDefectMarkPos[E_CORNER_END + n];

				line(Draw, pt1, pt2, Scalar(255,0,0));

				CString strTemp;
				strTemp.Format(_T("C:\\IDB\\CCut_Defect_Search_Area%d.bmp"), n); 
				cv::imwrite((cv::String)(CStringA)strTemp, Draw);
			}
		}
	}

	long double dBM_A[E_CORNER_END], dBM_B[E_CORNER_END];
	Point ptInterCeptX_Main[E_CORNER_END], ptInterCeptY_Main[E_CORNER_END];
	if(bBMLine)
	{
		
		int nBMSearchY[E_CORNER_END];
		Point ptCCutBM_Start[E_CORNER_END];	//BM Line을 찾기 위한 시작 Y 위치
		Point ptCCutBM_End[E_CORNER_END];	//BM Line을 찾기 위한 끝 Y 위치
		int nBMSearchRange = ((float)nBMDetectionSampleNum/2);	//Y방향 양방향범위 지정
		int nBMPointY[E_CORNER_END];

		for(int nCornerInx = E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
		{
			nBMPointY[nCornerInx] = 0;
			if(GetBMLinePointY(matSrcBuffer(rtSearchROI[nCornerInx]), nBMThreshold[nCornerInx], nCornerInx, nBMPointY[nCornerInx], nIgnore_GV ) == E_ERROR_CODE_FALSE)
				return E_ERROR_CODE_FALSE;
		}

		nBMSearchY[E_CORNER_LEFT_TOP]	 = rtSearchROI[E_CORNER_LEFT_TOP].y + nBMPointY[E_CORNER_LEFT_TOP] + fBMposition[E_CORNER_LEFT_TOP];
		nBMSearchY[E_CORNER_LEFT_BOTTOM] = rtSearchROI[E_CORNER_LEFT_BOTTOM].y  + nBMPointY[E_CORNER_LEFT_BOTTOM] - fBMposition[E_CORNER_LEFT_BOTTOM];
		nBMSearchY[E_CORNER_RIGHT_TOP]	 = rtSearchROI[E_CORNER_RIGHT_TOP].y + nBMPointY[E_CORNER_RIGHT_TOP] + fBMposition[E_CORNER_RIGHT_TOP];
		nBMSearchY[E_CORNER_RIGHT_BOTTOM]= rtSearchROI[E_CORNER_RIGHT_BOTTOM].y  + nBMPointY[E_CORNER_RIGHT_BOTTOM] - fBMposition[E_CORNER_RIGHT_BOTTOM];
	
		
		ptCCutBM_Start[E_CORNER_LEFT_TOP] = Point(ptCorner[E_CORNER_LEFT_TOP].x, nBMSearchY[E_CORNER_LEFT_TOP] - nBMSearchRange);
		ptCCutBM_End[E_CORNER_LEFT_TOP] = Point(ptCorner[E_CORNER_LEFT_TOP].x + nSearch_ROI_Size, nBMSearchY[E_CORNER_LEFT_TOP] + nBMSearchRange);

		ptCCutBM_Start[E_CORNER_RIGHT_TOP] = Point(ptCorner[E_CORNER_RIGHT_TOP].x, nBMSearchY[E_CORNER_RIGHT_TOP] - nBMSearchRange);
		ptCCutBM_End[E_CORNER_RIGHT_TOP] = Point(ptCorner[E_CORNER_RIGHT_TOP].x - nSearch_ROI_Size, nBMSearchY[E_CORNER_RIGHT_TOP] + nBMSearchRange);

		ptCCutBM_Start[E_CORNER_RIGHT_BOTTOM] = Point(ptCorner[E_CORNER_RIGHT_BOTTOM].x, nBMSearchY[E_CORNER_RIGHT_BOTTOM] - nBMSearchRange);
		ptCCutBM_End[E_CORNER_RIGHT_BOTTOM] =  Point(ptCorner[E_CORNER_RIGHT_BOTTOM].x - nSearch_ROI_Size, nBMSearchY[E_CORNER_RIGHT_BOTTOM] + nBMSearchRange);

		ptCCutBM_Start[E_CORNER_LEFT_BOTTOM] = Point(ptCorner[E_CORNER_LEFT_BOTTOM].x, nBMSearchY[E_CORNER_LEFT_BOTTOM] - nBMSearchRange);
		ptCCutBM_End[E_CORNER_LEFT_BOTTOM] = Point(ptCorner[E_CORNER_LEFT_BOTTOM].x + nSearch_ROI_Size, nBMSearchY[E_CORNER_LEFT_BOTTOM] + nBMSearchRange);

		Mat OrgImg;
		matSrcBuffer.copyTo(OrgImg);
		int nErrorCode = E_ERROR_CODE_TRUE;
		for(int nCornerInx=E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
		{
			nErrorCode = GetCCutBMLine(OrgImg, ptCCutBM_Start[nCornerInx], ptCCutBM_End[nCornerInx], nBMThreshold[nCornerInx], dBM_A[nCornerInx], dBM_B[nCornerInx], nCornerInx, nIgnore_GV);
			if(nErrorCode == E_ERROR_CODE_FALSE)
				return nErrorCode;
		}

		ptInterCeptX_Main[E_CORNER_LEFT_TOP]=	Point(0, dBM_B[E_CORNER_LEFT_TOP]);
		ptInterCeptY_Main[E_CORNER_LEFT_TOP]=	Point(-dBM_B[E_CORNER_LEFT_TOP]/dBM_A[E_CORNER_LEFT_TOP], 0);
		ptInterCeptX_Main[E_CORNER_LEFT_BOTTOM]=	Point(0, dBM_B[E_CORNER_LEFT_BOTTOM]);
		ptInterCeptY_Main[E_CORNER_LEFT_BOTTOM]=	Point((matSrcBuffer.rows-1-dBM_B[E_CORNER_LEFT_BOTTOM])/dBM_A[E_CORNER_LEFT_BOTTOM], matSrcBuffer.rows-1);
		ptInterCeptX_Main[E_CORNER_RIGHT_TOP]=		Point(matSrcBuffer.cols-1, (matSrcBuffer.cols-1)*dBM_A[E_CORNER_RIGHT_TOP]+dBM_B[E_CORNER_RIGHT_TOP]);
		ptInterCeptY_Main[E_CORNER_RIGHT_TOP]=		Point(-dBM_B[E_CORNER_RIGHT_TOP]/dBM_A[E_CORNER_RIGHT_TOP], 0);
		ptInterCeptX_Main[E_CORNER_RIGHT_BOTTOM]=	Point(matSrcBuffer.cols-1, (matSrcBuffer.cols-1)*dBM_A[E_CORNER_RIGHT_BOTTOM]+dBM_B[E_CORNER_RIGHT_BOTTOM]);
		ptInterCeptY_Main[E_CORNER_RIGHT_BOTTOM]=	Point((matSrcBuffer.rows-1-dBM_B[E_CORNER_RIGHT_BOTTOM])/dBM_A[E_CORNER_RIGHT_BOTTOM], matSrcBuffer.rows-1);

		if(bImageSave)
		{
			Mat TestBM;
			matSrcBuffer.copyTo(TestBM);
			cv::cvtColor(TestBM, TestBM, CV_GRAY2RGB);

			for(int nCornerInx= E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
			{
				line(TestBM, ptInterCeptX_Main[nCornerInx], ptInterCeptY_Main[nCornerInx], Scalar(255,255,0));
			}

			strImageSave.Format(_T("%s\\CCutBMLine.jpg"), strAlgPath);
			imwrite((cv::String)(CStringA)strImageSave, TestBM);
		}
		
	}

	// Defect 검색
	// 불량 입력 된 갯수
	const int nDefectCountPrev = pResultBlob->nDefectCount;
	int& nDefectCount = pResultBlob->nDefectCount;
	int		nPatternImageNum	= nCommonPara[10];						// 10 : UI 상 패턴 순서 영상 번호

	Mat matDefectBuffer = Mat::zeros(matSrcBuffer.size(), CV_8UC1);
	matSrcBuffer.copyTo(matDefectBuffer);
	for(long n=0; n < E_CORNER_END ; n++)
	{
		Mat matDefectTemp =matDefectBuffer(artDefectSearch[n]);
		Mat matDefectTemp2 =Mat::zeros(matDefectTemp.size(), CV_8UC1);

		if(nUseCorner[n] > 0)
		{
			if((arbSearchOK[n]  && arbSearchOK[E_CORNER_END + n]) || bBMLine )
			{

				Mat Src, Binary, BiggestBlob, Edge1, Edge2;
				matSrcBuffer(artDefectSearch[n]).copyTo(Src);

				cv::blur(Src, Src, Size(5,5));

				Scalar m,s;
				cv::meanStdDev(Src, m, s);

				int fCellThreshold = ((m[0] - s[0])) * fCellThreshold_Offset;

				cv::threshold(Src, Binary, fCellThreshold, 255, cv::THRESH_BINARY);

				if (bImageSave)
				{
					CString strTemp;

					strTemp.Format(_T("C:\\IDB\\CCut_Defect_Search_Src%d.bmp"), n);
					cv::imwrite((cv::String)(CStringA)strTemp, Src);

					strTemp.Format(_T("C:\\IDB\\CCut_Defect_Search_Binary%d.bmp"), n); 
					cv::imwrite((cv::String)(CStringA)strTemp, Binary);
				}

				Mat RectKernel = cv::getStructuringElement(MORPH_RECT, Size(Morph_Open_Size,Morph_Open_Size), Point(-1,-1));
				morphologyEx(Binary, Binary, MORPH_OPEN, RectKernel);

				BiggestBlob = cv::Mat::zeros(Binary.size(), CV_8UC1);

				FindBiggestBlob(Binary, BiggestBlob);

				Mat RectKernel2 = cv::getStructuringElement(MORPH_RECT, Size(Morph_Close_Size,Morph_Close_Size), Point(-1,-1));
				morphologyEx(BiggestBlob, BiggestBlob, MORPH_CLOSE, RectKernel2);

				Edge_BasicSobel(BiggestBlob, Edge1);

				Edge_Thinning(Edge1, Edge2);

				Mat BiggestBlob2 = cv::Mat::zeros(Edge2.size(), CV_8UC1);
				FindBiggestBlob(Edge2, BiggestBlob2);
				BiggestBlob2.copyTo(Edge2);

				if (bImageSave)
				{
					CString strTemp;

					strTemp.Format(_T("C:\\IDB\\CCut_Defect_Search_Edge%d.bmp"), n); 
					cv::imwrite((cv::String)(CStringA)strTemp, Edge1);

					strTemp.Format(_T("C:\\IDB\\CCut_Defect_Search_thinning%d.bmp"), n); 
					cv::imwrite((cv::String)(CStringA)strTemp, Edge2);
				}

				double nX1	;	
				double nX2	;	

				double nY1	;	
				double nY2	;	


				if( arpDefectMarkPos[n].x < arpDefectMarkPos[E_CORNER_END + n].x ) //좌측에 위치한 Mark의 위치 구분
				{
					nX1		=	arpDefectMarkPos[n].x;					//Mark 1 X 위치
					nY1		=	arpDefectMarkPos[n].y;					//Mark 1 Y 위치
					nX2		=	arpDefectMarkPos[E_CORNER_END + n].x;	//Mark 2 X
					nY2		=	arpDefectMarkPos[E_CORNER_END + n].y;	//Mark 2 Y
				}
				else
				{
					nX1		=	arpDefectMarkPos[E_CORNER_END + n].x;
					nY1		=	arpDefectMarkPos[E_CORNER_END + n].y;
					nX2		=	arpDefectMarkPos[n].x;
					nY2		=	arpDefectMarkPos[n].y;
				}

				if( nX2 - nX1 == 0.0 && bBMLine==0)
					continue;

				// 두 개의 마크를 지나는 직선과  마크 직선과 직교하는 두 직선을 구한다
				// 목적 : 에지 중 사용 가능 데이터 영역 구분 
				int		nDataSX =0, nDataSY = 0, nDataEX =0, nDataEY = 0;
				double dMarkLineA, dMarkLineB, dCrossLine1_A, dCrossLine1_B, dCrossLine2_A, dCrossLine2_B;
				double dSlope =  (double)( nY2 - nY1 ) / ( nX2 - nX1 ) ;
				dMarkLineA	=	dSlope;
				dMarkLineB	=	nY1 + -1 *dSlope * nX1 ;

				dCrossLine1_A = -1 * ( 1 / dSlope );
				dCrossLine1_B = -1 * dCrossLine1_A * nX1 + nY1;

				dCrossLine2_A = -1 * ( 1 / dSlope );
				dCrossLine2_B = -1 * dCrossLine2_A * nX2 + nY2;


				int nCornerIndex = n % E_CORNER_END;
				
				switch(nCornerIndex)
				{
				case E_CORNER_LEFT_TOP:
					nDataSY = Offset_Height + nCutLineOffset;
					nDataEY = Edge2.rows - 1;
					nDataSX = Offset_Width + nCutLineOffset;
					nDataEX = Edge2.cols - 1;
					break;
				case E_CORNER_RIGHT_TOP:
					nDataSY = Offset_Height + nCutLineOffset;
					nDataEY = Edge2.rows - 1;
					nDataSX = 0;
					nDataEX = Edge2.cols - Offset_Width - nCutLineOffset;
					break;
				case E_CORNER_RIGHT_BOTTOM:
					nDataSY = 0;
					nDataEY = Edge2.rows - Offset_Height - nCutLineOffset;
					nDataSX = 0;
					nDataEX = Edge2.cols - Offset_Width - nCutLineOffset;
					break;
				case E_CORNER_LEFT_BOTTOM:
					nDataSY = 0;
					nDataEY = Edge2.rows - Offset_Height - nCutLineOffset;
					nDataSX = Offset_Width + nCutLineOffset;
					nDataEX = Edge2.cols-1;
					break;
				}

				if( nDataSY >  nDataEY )	//Mark 위치 위아래 구분
				{	
					int nTemp	= nDataEY;
					nDataEY = nDataSY;
					nDataSY = nTemp;
				}
				if( nDataSX > nDataEX)		//Mark 위치 좌우 구분
				{
					int nTempX = nDataEX;
					nDataEX = nDataSX;
					nDataSX = nTempX;
				}

				vector<Point>  ptEdgeData;		//ROI영상에서의 좌표
				vector<Point>  ptEdgeData_Main;	//전체 영상에서의 좌표
				Rect rtDataArea(nDataSX, nDataSY, nDataEX - nDataSX, nDataEY -nDataSY);
				Mat matGetData = Edge2(rtDataArea);

				if (bImageSave)
				{
					CString strTemp;
					strTemp.Format(_T("C:\\IDB\\CCut_Defect_Search_ValidData%d.bmp"), n); 
					cv::imwrite((cv::String)(CStringA)strTemp, matGetData);
				}

				for(int y =0;  y < matGetData.rows ; y++ )
				{
					for(int x =0 ; x < matGetData.cols ; x++)
					{
						if(matGetData.at<uchar>(y, x))
						{
							ptEdgeData_Main.emplace_back(Point(x + rtDataArea.x + artDefectSearch[nCornerIndex].x, y + rtDataArea.y + artDefectSearch[nCornerIndex].y));
							break;
						}
					}
				}

				//Cut StartPoint 항상 위, EndPoint 항상 아래
				Point ptCCutST, ptCCutED;
				GetCCutLineToPanelEdgeCrossPoint(nCornerIndex, ptCorner, ptEdgeData_Main, ptCCutST, ptCCutED);
				
				Point ptCCutST_ROI, ptCCutED_ROI;
				ptCCutST_ROI = Point(ptCCutST.x - artDefectSearch[nCornerIndex].x, ptCCutST.y - artDefectSearch[nCornerIndex].y);
				ptCCutED_ROI = Point(ptCCutED.x - artDefectSearch[nCornerIndex].x, ptCCutED.y - artDefectSearch[nCornerIndex].y);

				Point ptSTBM, ptEDBM;
				long double dBM_ROI_A, dBM_ROI_B;//ROI상의 직선 상수 A,B
				dBM_ROI_A = (float)(((ptInterCeptX_Main[nCornerIndex].y - artDefectSearch[nCornerIndex].y) - (ptInterCeptY_Main[nCornerIndex].y - artDefectSearch[nCornerIndex].y)))
					/((ptInterCeptX_Main[nCornerIndex].x - artDefectSearch[nCornerIndex].x) - (ptInterCeptY_Main[nCornerIndex].x - artDefectSearch[nCornerIndex].x));
				dBM_ROI_B = (ptInterCeptX_Main[nCornerIndex].y - artDefectSearch[nCornerIndex].y)- dBM_ROI_A * (ptInterCeptX_Main[nCornerIndex].x - artDefectSearch[nCornerIndex].x);

				if(bBMLine)
					dCrossLine1_A = -1/dBM_ROI_A;

				ptSTBM = Point(ptCCutST_ROI.x, ptCCutST_ROI.x * dBM_ROI_A + (dBM_ROI_B));
				ptEDBM = Point(ptCCutED_ROI.x, ptCCutED_ROI.x * dBM_ROI_A + (dBM_ROI_B));

				for(int y =0; y < Edge2.rows; y++)
				{
					for(int x =0; x < Edge2.cols; x++)
					{
						if(ptCCutST_ROI.y+nCutLineOffset < y  && ptCCutED_ROI.y-nCutLineOffset > y)
						{
							if(Edge2.at<uchar>(y, x))
							{
								ptEdgeData.emplace_back(Point(x, y));
							}
						}
					}						
				}

				if(bImageSave)
				{
					Mat mtCCutLine;
					matDefectTemp.copyTo(mtCCutLine);
					cv::cvtColor(mtCCutLine,mtCCutLine,CV_GRAY2BGR);

					line(mtCCutLine, ptCCutST_ROI, ptCCutED_ROI, Scalar(0,255,0));
					if(bBMLine)	line(mtCCutLine, ptSTBM, ptEDBM, Scalar(255,255,0));
					cv::circle(mtCCutLine, arpDefectMarkPos[nCornerIndex], 1, Scalar(0,0,255));
					cv::circle(mtCCutLine, arpDefectMarkPos[nCornerIndex+E_CORNER_END], 1, Scalar(0,0,255));
					CString strCCutLine;
					strCCutLine.Format(_T("%s\\CCutLine_%d.jpg"), strAlgPath, nCornerIndex);

					imwrite((cv::String)(CStringA)strCCutLine, mtCCutLine);
				}


				int nEdgeData = (int)ptEdgeData.size();
				int nSumCount = 0;
				double dAvg =0, dSum = 0;
				double dMax =0;
				double dMin =100+matSrcBuffer.cols;

				for(long nData =0; nData < nEdgeData ;nData++)
				{
					float fX, fY;
					dCrossLine1_B =  -1 * dCrossLine1_A * ptEdgeData[nData].x + ptEdgeData[nData].y;

					//CutLine 점 대 Mark 직선의 거리 계산
					BOOL bCalc;
					if(bBMLine)		bCalc = GetCrossPointByTwoStraightLines(dBM_ROI_A, -1, dBM_ROI_B,  dCrossLine1_A, -1, dCrossLine1_B, fX, fY);
					else			bCalc = GetCrossPointByTwoStraightLines(dMarkLineA, -1, dMarkLineB,  dCrossLine1_A, -1, dCrossLine1_B, fX, fY);
					if(bCalc)
					{

						float fDataX = (fX - ptEdgeData[nData].x ) * (fX - ptEdgeData[nData].x );
						float fDataY = (fY - ptEdgeData[nData].y ) * (fY - ptEdgeData[nData].y );

						float fDis = sqrt(fDataX + fDataY);	

						dSum += fDis;
						nSumCount++;
						if(fDis > dMax)
							dMax = fDis;
						if(fDis < dMin)
							dMin = fDis;
						Point pt1((int)(fX), (int)(fY));

						// 디펙 출력용으로 그려 준다.
						cv::line(matDefectTemp2, ptEdgeData[nData], pt1,Scalar(255,255,255), 2);

					}

				}

				if(nSumCount == 0 )
					continue;

				dAvg = dSum / nSumCount; //평균 길이

				double dMarkOffsetDist;

				if(bBMLine)			dMarkOffsetDist = 0;
				else				dMarkOffsetDist = sqrt(pow(dMarkSize/2, 2) * 2);	//Mark 중심부터 Mark 모서리까지의 거리는 측정 길이에서 제외함.

				double difDistance = (dAvg - dMarkOffsetDist) - dRefCutDistance;	//Cutting Line 기준 길이와 현재길이의 차이

				double difDistanceMax = (dMax - dMarkOffsetDist) - dRefCutDistance; //CuttingLine 기준 길이와 현재 측정된 Max 길이의 차이
				double difDistanceMin = (dMin - dMarkOffsetDist) - dRefCutDistance; //CuttingLine 기준 길이와 현재 측정된 Min 길이의 차이

				CFeatureExtraction cFeature;
				BOOL bDefect = FALSE;
				BOOL bCCutComeOUT= FALSE;		//측정 결과가 기준치보다 작을 경우
				BOOL bCCutComeIN = FALSE;		//측정 결과가 기준치보다 클 경우
				if(EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_CUT_C_COME_OUT].bDefectItemUse)
				{
					if(EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_CUT_C_COME_OUT].Judgment[0].bUse)
					{
						bDefect += cFeature.Compare(difDistanceMax, EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_CUT_C_COME_OUT].Judgment[0].nSign, EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_CUT_C_COME_OUT].Judgment[0].dValue/dCameraResolution);
						if(bDefect == TRUE)	bCCutComeOUT = TRUE;
					}
				}
				if(EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_CUT_C_COME_IN].bDefectItemUse && bDefect == FALSE)
				{
					if(EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_CUT_C_COME_IN].Judgment[0].bUse)
					{
						bDefect += cFeature.Compare(difDistanceMin, EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_CUT_C_COME_IN].Judgment[0].nSign, EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_CUT_C_COME_IN].Judgment[0].dValue/dCameraResolution);
						if(bDefect == TRUE)	bCCutComeIN = TRUE;
					}
				}


				//측정 결과 기록용이기에 OK/NG 여부에 상관없이 계속하여 기록함.
				pMeasureInfo->dMeasureValue[nMeasureValueCnt++]		= (dMin - dMarkOffsetDist) * dCameraResolution;
				pMeasureInfo->dMeasureValue[nMeasureValueCnt++]		= (dMax - dMarkOffsetDist) * dCameraResolution;
				pMeasureInfo->dMeasureValue[nMeasureValueCnt++]		= (dAvg - dMarkOffsetDist) * dCameraResolution;

				int nMode = RETR_EXTERNAL;
				int nMethod = CHAIN_APPROX_SIMPLE;

				vector<vector<cv::Point>> contours;		// 검출 결과 좌표(Cutting Line, Mark1,2가 이루는 Line)

				// 외곽선 검출
				findContours(matDefectTemp2, contours, noArray(), nMode, nMethod, Point(artDefectSearch[n].x, artDefectSearch[n].y));


				if(bDefect && contours.size() > 0)	//불량이라면
				{
					//matDefectTemp2.copyTo(matDefectTemp);
					int nL, nT, nR, nB;

					int X = contours[0][0].x;
					int Y = contours[0][0].y;

					// 초기 설정
					nL = nR = X;
					nT = nB = Y;

					// 외곽선 갯수 만큼
					for (int k=1 ; k <  contours[0].size() ; k++)
					{
						// 회전 시, 예측 좌표 계산
						X = contours[0][k].x; 
						Y =	contours[0][k].y;

						// 갱신
						if( nL > X )	nL = X;
						if( nR < X )	nR = X;
						if( nT > Y )	nT = Y;
						if( nB < Y )	nB = Y;
					}


					pResultBlob->ptLT		[nDefectCount].x = nL;
					pResultBlob->ptLT		[nDefectCount].y = nT;

					pResultBlob->ptRT		[nDefectCount].x = nR;
					pResultBlob->ptRT		[nDefectCount].y = nT;

					pResultBlob->ptRB		[nDefectCount].x = nR;	
					pResultBlob->ptRB		[nDefectCount].y = nB;	


					pResultBlob->ptLB		[nDefectCount].x =  nL;
					pResultBlob->ptLB		[nDefectCount].y =	nB;

					pResultBlob->nDefectColor	[nDefectCount] = E_DEFECT_COLOR_BRIGHT;
					pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

					drawContours(matDrawBuffer, contours, 0,  cv::Scalar(135,206,250), 1);

					pResultBlob->dBreadth[nDefectCount]		= dAvg;
					pResultBlob->dF_Max[nDefectCount]		= dMin;
					pResultBlob->dF_Min[nDefectCount]		= dMax;
					pResultBlob->nArea[nDefectCount]		= abs(difDistanceMax) > abs(difDistanceMin) ? difDistanceMax : difDistanceMin;
					pMeasureInfo->bJudge					= false;
					pResultBlob->nDefectJudge[nDefectCount] = bCCutComeIN > bCCutComeOUT ? E_DEFECT_JUDGEMENT_APP_CUT_C_COME_IN : E_DEFECT_JUDGEMENT_APP_CUT_C_COME_OUT;	// 해당 불량
					if(pResultBlob->nDefectJudge[nDefectCount] == E_DEFECT_JUDGEMENT_APP_CUT_C_COME_IN)		
						cv::putText(matDrawBuffer, "COME IN", Point((nL+nR)/2, (nT+nB)/2), FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0,0,255));
					else
						cv::putText(matDrawBuffer, "COME OUT", Point((nL+nR)/2, (nT+nB)/2), FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0,0,255));


					nDefectCount++;

				}
				else
				{
					pMeasureInfo->bJudge					= true;
				}

			}
			else//Mark가 없을 경우
			{
				pMeasureInfo->dMeasureValue[nMeasureValueCnt++]=0;
				pMeasureInfo->dMeasureValue[nMeasureValueCnt++]=0;
				pMeasureInfo->dMeasureValue[nMeasureValueCnt++]=0;
				pMeasureInfo->bJudge = false;

				pResultBlob->ptLT		[nDefectCount].x = rtSearchROI[n].x;
				pResultBlob->ptLT		[nDefectCount].y = rtSearchROI[n].y;

				pResultBlob->ptRT		[nDefectCount].x = rtSearchROI[n].x + rtSearchROI[n].width;
				pResultBlob->ptRT		[nDefectCount].y = rtSearchROI[n].y;

				pResultBlob->ptRB		[nDefectCount].x = rtSearchROI[n].x + rtSearchROI[n].width;	
				pResultBlob->ptRB		[nDefectCount].y = rtSearchROI[n].y + rtSearchROI[n].height;	


				pResultBlob->ptLB		[nDefectCount].x =  rtSearchROI[n].x;
				pResultBlob->ptLB		[nDefectCount].y =	rtSearchROI[n].y + rtSearchROI[n].height;

				pResultBlob->nDefectColor	[nDefectCount] = E_DEFECT_COLOR_BRIGHT;
				pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;
				pResultBlob->nDefectJudge[nDefectCount] = E_DEFECT_JUDGEMENT_APP_CUT_C_COME_IN;

				Point ptCenter(rtSearchROI[n].x + rtSearchROI[n].width/2, rtSearchROI[n].y + rtSearchROI[n].height/2);

				cv::putText(matDrawBuffer, "COME IN(Don't be Mark)", ptCenter, FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0,0,255));

				nDefectCount++;
			}

		}
		else	//C-Cut 검사를 안하는 부분
		{
			nMeasureValueCnt+=3; //4모서리 중 한영역을 Skip 하는 것이므로 Min, Max, Avg 3부분을 넘김
		}
		
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectEdge::GetCCutBMLine(Mat mtImg, Point ptST, Point ptED, int nThreshold, long double& dA, long double& dB, int nCornerInx, int nIgnore_GV)
{
	blur(mtImg,mtImg, Size(3,3));
	bool bAdd_X;//반복문에서 X방향을 증가문으로 사용할 것인지
	int nPointSampleNum = 2;
	if(ptST.x > ptED.x)
		bAdd_X = false;
	else
		bAdd_X = true;
	
	uchar* pImgData;
	int nGrayValue;

	vector<Point> ptProfile; // X: 좌표,  Y: GrayValue
	vector<Point2f> ptBMLine;


	int nDeltaOffset = 3;
	int nDeltaGrayValue;
	int	nDeltaX;
	float fTheta;

	Mat mtDebugDraw;
	cv::cvtColor(mtImg, mtDebugDraw, CV_GRAY2RGB);

	switch(bAdd_X)
	{
	case true:
		for(int nY = ptST.y; nY < ptED.y; nY++)
		{
			pImgData = mtImg.data+nY*mtImg.step;
			for(int nX = ptST.x; nX < ptED.x; nX++)
			{
				nGrayValue = (int)*(pImgData + nX);
				if(nGrayValue<=nIgnore_GV)
					continue;
				nDeltaX = nX+nDeltaOffset;
				if(nDeltaX >= ptED.x )
					nDeltaX = ptED.x-1;
				nDeltaGrayValue = (int)*(pImgData + nDeltaX);
				fTheta = atan((float)(nDeltaGrayValue - nGrayValue)/(nDeltaX-nX)) * 180 / CV_PI;
				
				if( 90 > fTheta && fTheta > nThreshold)
				//if(nGrayValue >= nThreshold)
				{
// 					vector<Point>().swap(ptProfile);
// 					for(int nX2 = nX-nPointSampleNum; nX2 <= nX+nPointSampleNum; nX2++)
// 					{
// 						if(nX2 < 0 || nX2 > mtImg.cols)
// 							continue;
// 						nGrayValue = (int)*(pImgData + nX2);
// 						ptProfile.push_back(Point(nX2, nGrayValue));
// 					}
// 					calcLineFit(ptProfile, dProfileA, dProfileB);
// 					
// 					float fSubX = (nThreshold - dProfileB)/dProfileA;
					ptBMLine.push_back(Point2f(nDeltaX, nY));
					cv::circle(mtDebugDraw, Point2i(nDeltaX, nY), 1, Scalar(0,255,0));
					break;
				}
			}
		}
		break;
	case false:
		for(int nY = ptST.y; nY < ptED.y; nY++)
		{
			pImgData = mtImg.data+nY*mtImg.step;
			for(int nX = ptST.x; nX > ptED.x; nX--)
			{
				nGrayValue = (int)*(pImgData + nX);
				if(nGrayValue<=nIgnore_GV)
					continue;
				nDeltaX = nX - nDeltaOffset;
				if(nDeltaX <= ptED.x)
					nDeltaX = ptED.x + 1;
				nDeltaGrayValue = (int)*(pImgData + nDeltaX);
				fTheta = atan((float)(nDeltaGrayValue - nGrayValue)/(nX - nDeltaX)) * 180 / CV_PI;
				if( 90 > fTheta && fTheta > nThreshold)
				//if(nGrayValue >= nThreshold)
				{
// 					vector<Point>().swap(ptProfile);
// 					for(int nX2 = nX-nPointSampleNum; nX2 <= nX+nPointSampleNum; nX2++)
// 					{
// 						if(nX2 < 0 || nX2 > mtImg.cols)
// 							continue;
// 						nGrayValue = (int)*(pImgData + nX2);
// 						ptProfile.push_back(Point(nX2, nGrayValue));
// 					}
// 					calcLineFit(ptProfile, dProfileA, dProfileB);
// 
// 					float fSubX = (nThreshold - dProfileB)/dProfileA;
					ptBMLine.push_back(Point2f(nDeltaX, nY));
					cv::circle(mtDebugDraw, Point2i(nDeltaX, nY), 1, Scalar(0,255,0));
					break;
				}
			}
		}
		break;
	}

	CString strDebugImg;
	strDebugImg.Format(_T("C:\\IDB\\BMLine_%d.bmp"), nCornerInx);
	imwrite((cv::String)(CStringA)strDebugImg, mtDebugDraw);

	
	return calcRANSAC(ptBMLine, dA, dB, 3, 5);
}


long CInspectEdge::FindCuttingDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	// Algorithm parameter
	bool	bImageSave			= (nCommonPara[1] > 0) ? true : false;	// 01 : 알고리즘 중간결과 Image Save 

	const int Bluring						= 3;
	const int DefectDistance				= 3;	// 좌표에 따른 Defect 구분


	int nPos = 0;

	const	int		nCellThreshold			=		(int)dPara[nPos++];
	const	int		nSearchSize				=		(int)dPara[nPos++];
	
	//CCut 영역을 사용하는지 안하는지 0: 사용, 1: 미사용
	int		nUseCCut[E_CORNER_END];

	for(int nCornerInx = E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
	{
		nUseCCut[nCornerInx]				=		(int)dPara[nPos++];
	}

	const	int		nUseOffsetX_Right		=		(int)dPara[nPos++];
	const	int		nUseOffsetY_Right		=		(int)dPara[nPos++];
	const	int		nUseOffsetX_Left		=		(int)dPara[nPos++];
	const	int		nUseOffsetY_Left		=		(int)dPara[nPos++];

	const	float	fGamma					=		(float)dPara[nPos++];
	const	int		nBrokenOffset			=		(int)dPara[nPos++];

	//사용하지 않는 CCut영역의 Offset Size
	int		arnCutOffsetX[E_CORNER_END];	
	int		arnCutOffsetY[E_CORNER_END];


	for(int nCornerInx = E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
	{
		arnCutOffsetX[nCornerInx]		=		0;
		arnCutOffsetY[nCornerInx]		=		0;

		switch(nCornerInx)
		{
		case E_CORNER_LEFT_TOP:
			arnCutOffsetX[nCornerInx] = nUseOffsetX_Left;
			arnCutOffsetY[nCornerInx] = nUseOffsetY_Left;
			break;
		case E_CORNER_RIGHT_TOP:
			arnCutOffsetX[nCornerInx] = nUseOffsetX_Right;
			arnCutOffsetY[nCornerInx] = nUseOffsetY_Right;
			break;
		case E_CORNER_RIGHT_BOTTOM:
			arnCutOffsetX[nCornerInx] = nUseOffsetX_Right;
			arnCutOffsetY[nCornerInx] = nUseOffsetY_Right;
			break;
		case E_CORNER_LEFT_BOTTOM:
			arnCutOffsetX[nCornerInx] = nUseOffsetX_Left;
			arnCutOffsetY[nCornerInx] = nUseOffsetY_Left;
			break;
		}
	}

	if(matSrcBuffer.empty())
		return E_ERROR_CODE_EMPTY_BUFFER;

	Rect rtCalcArea, rtCheckArea;

	rtCheckArea.x			=	0 ;
	rtCheckArea.y			=	0 ;
	rtCheckArea.width		=	matSrcBuffer.cols ;
	rtCheckArea.height		=	matSrcBuffer.rows ;


	rtCalcArea.x			=	ptCorner[E_CORNER_LEFT_TOP].x	- nSearchSize	;
	rtCalcArea.y			=	ptCorner[E_CORNER_LEFT_TOP].y	- nSearchSize	;

	rtCalcArea.width		=	ptCorner[E_CORNER_RIGHT_TOP].x		- ptCorner[E_CORNER_LEFT_TOP].x  + nSearchSize * 2 ;
	rtCalcArea.height		=	ptCorner[E_CORNER_LEFT_BOTTOM].y	- ptCorner[E_CORNER_LEFT_TOP].y  + nSearchSize * 2 ;

	GetCheckROIOver(rtCalcArea, rtCheckArea, rtCalcArea);

	//Panel 외곽 Line ROI 적용
	Mat matSrcEdgebuffer	=	matSrcBuffer(rtCalcArea);
	Mat matDstEdgebuffer	= Mat::zeros(matSrcEdgebuffer.size(), CV_8UC1);
	Mat matSrcDrawbuffer	=	matDrawBuffer(rtCalcArea);

	//0. Encoding
// 	float gamma = fGamma;
// 	for(int h=0; h<matSrcEdgebuffer.rows; h++)
// 		for(int w=0; w<matSrcEdgebuffer.cols; w++) {
// 			matSrcEdgebuffer.data[h*matSrcEdgebuffer.step+w] = 255 * 
// 				(pow(((float)(uchar)matSrcEdgebuffer.data[h*matSrcEdgebuffer.step+w])/255, gamma));
// 		}

	// 1. Bluring
	cv::blur(matSrcEdgebuffer, matDstEdgebuffer, cv::Size(Bluring, Bluring));
	//cv::medianBlur(matSrcEdgebuffer, matDstEdgebuffer, 3);

	// 2. Threshold
	cv::threshold(matDstEdgebuffer, matDstEdgebuffer, nCellThreshold, 255, CV_THRESH_BINARY);
	
	

// 	cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, Size(3, 3), cv::Point(3/2, 3/2));
// 	cv::morphologyEx(matDstEdgebuffer, matDstEdgebuffer, MORPH_OPEN, StructElem);

	// 3. Corner Extract
	CvPoint Corner[4];

	for(long n=0; n <  E_CORNER_END ; n++)
	{
		Corner[n].x = ptCorner[n].x - rtCalcArea.x;
		Corner[n].y = ptCorner[n].y - rtCalcArea.y;
	}


	long double dValueA[LINE_MAX];
	long double dValueB[LINE_MAX];

	bool bReverse[LINE_MAX] = {false,};	// X,Y 축을 바꾸었는지 안 바꾸었는지

	int nY = Corner[E_CORNER_LEFT_BOTTOM].y - Corner[E_CORNER_LEFT_TOP].y ;	//Panel 왼쪽 Y 변위
	int nX = Corner[E_CORNER_LEFT_BOTTOM].x - Corner[E_CORNER_LEFT_TOP].x ;	//Panel 왼쪽 X 변위
	double dSlope;//기울기
	if(nX == 0  )
	{
		dSlope = nX / nY;
		bReverse[LEFT] = true;
		dValueA[LEFT]		= dSlope;
		dValueB[LEFT]		= -1 * dValueA[LEFT]  * Corner[E_CORNER_LEFT_BOTTOM].y + Corner[E_CORNER_LEFT_BOTTOM].x;
	}
	else
	{
		dSlope = nY / nX;
		dValueA[LEFT]		= dSlope;
		dValueB[LEFT]		= -1 * dValueA[LEFT]  * Corner[E_CORNER_LEFT_BOTTOM].x + Corner[E_CORNER_LEFT_BOTTOM].y;
	}
	


	nY = Corner[E_CORNER_RIGHT_TOP].y - Corner[E_CORNER_LEFT_TOP].y ;	//Panel 위쪽 Y 변위
	nX = Corner[E_CORNER_RIGHT_TOP].x - Corner[E_CORNER_LEFT_TOP].x ;	//Panel 위쪽 X 변위

	if(nX == 0  )
	{
		dSlope = nX / nY;
		bReverse[TOP] = true;
		dValueA[TOP]		= dSlope;
		dValueB[TOP]		= -1 * dValueA[TOP]  * Corner[E_CORNER_RIGHT_TOP].y + Corner[E_CORNER_RIGHT_TOP].x;
	}
	else
	{
		dSlope = nY / nX;
		dValueA[TOP]		= dSlope;
		dValueB[TOP]		= -1 * dValueA[TOP]  * Corner[E_CORNER_RIGHT_TOP].x + Corner[E_CORNER_RIGHT_TOP].y;
	}



	nY = Corner[E_CORNER_RIGHT_BOTTOM].y - Corner[E_CORNER_RIGHT_TOP].y ;
	nX = Corner[E_CORNER_RIGHT_BOTTOM].x - Corner[E_CORNER_RIGHT_TOP].x ;
	if(nX == 0  )
	{
		dSlope = nX / nY;
		bReverse[RIGHT] = true;
		dValueA[RIGHT]		= dSlope;
		dValueB[RIGHT]		= -1 * dValueA[RIGHT]  * Corner[E_CORNER_RIGHT_BOTTOM].y + Corner[E_CORNER_RIGHT_BOTTOM].x;
	}
	else
	{
		dSlope = nY / nX;
		dValueA[RIGHT]		= dSlope;
		dValueB[RIGHT]		= -1 * dValueA[RIGHT]  * Corner[E_CORNER_RIGHT_BOTTOM].x + Corner[E_CORNER_RIGHT_BOTTOM].y;
	}
	


	nY = Corner[E_CORNER_RIGHT_BOTTOM].y - Corner[E_CORNER_LEFT_BOTTOM].y ;
	nX = Corner[E_CORNER_RIGHT_BOTTOM].x - Corner[E_CORNER_LEFT_BOTTOM].x ;
	if(nX == 0  )
	{
		dSlope = nX / nY;
		bReverse[BOTTOM] = true;
		dValueA[BOTTOM]		= dSlope;
		dValueB[BOTTOM]		= -1 * dValueA[BOTTOM]  * Corner[E_CORNER_RIGHT_BOTTOM].y + Corner[E_CORNER_RIGHT_BOTTOM].x;
	}
	else
	{
		dSlope = nY / nX;
		dValueA[BOTTOM]		=dSlope;
		dValueB[BOTTOM]		= -1 * dValueA[BOTTOM]  * Corner[E_CORNER_RIGHT_BOTTOM].x + Corner[E_CORNER_RIGHT_BOTTOM].y;
	}
	
	
	// SrcImage
	IplImage* cvImgSrc	= &IplImage(matDstEdgebuffer);
	IplImage* ImgTemp	= cvCreateImage( cvSize(cvImgSrc->width,cvImgSrc->height), IPL_DEPTH_8U, 1 );
	cvCopy(cvImgSrc, ImgTemp);

	// 4. Compare Line: FindDefect(BrokenDefectImg, BurrDefectImg)
	// 4.1 Broken
	IplImage*  BrokenDefectImg = cvCreateImage( cvSize(ImgTemp->width,ImgTemp->height), IPL_DEPTH_8U, 1 );
	//Mat	matBrokenDefect = Mat::zeros(matDstEdgebuffer.size(), CV_8UC1);
	cvZero(BrokenDefectImg);
	//Mat mtTest = Mat::zeros(matDstEdgebuffer.size(), CV_8UC1);
//	Inspection_CompareEdgeBrokenArea(matDstEdgebuffer, mtTest);
	Inspection_CompareEdgeBrokenArea(ImgTemp, BrokenDefectImg, dValueA, dValueB, Corner, nSearchSize, arnCutOffsetX, arnCutOffsetY, nBrokenOffset, bReverse);
	if(bImageSave)
		cvSaveImage("C:\\IDB\\BrokenFill.bmp", BrokenDefectImg);
	BrokenDefectResult(BrokenDefectImg, BrokenDefectImg, Corner);

	if(bImageSave)
		cvSaveImage("C:\\IDB\\Broken_Result.bmp", BrokenDefectImg);

	// 4.2 Burr
	IplImage*  BurrDefectImg = cvCreateImage( cvSize(ImgTemp->width,ImgTemp->height), IPL_DEPTH_8U, 1 );	
	cvZero(BurrDefectImg);
 	Inspection_CompareEdgeBurrArea(ImgTemp, BurrDefectImg, dValueA, dValueB, Corner, nSearchSize, arnCutOffsetX, arnCutOffsetY, bReverse);
 	BurrDefectResult(BurrDefectImg, BurrDefectImg, Corner);


	//(7),(8) Crux 통합 후 수정: Blob시 Defect name, Corner, 좌표에 따른 예외처리 등
	// 5. Labeling
	// CRUX Labeling
	CFeatureExtraction cFeatureExtraction;

	int nPrevDefectCount = pResultBlob->nDefectCount;
	
	

 	cFeatureExtraction.SetMem(cMem);
	cFeatureExtraction.DoDefectBlobJudgment(matSrcEdgebuffer, cvarrToMat(BrokenDefectImg), matSrcDrawbuffer, 
		nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("BROKEN"), EngineerDefectJudgment, pResultBlob, false);
	cFeatureExtraction.DrawBlob(matSrcDrawbuffer, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

	//Edge Burr
 	cFeatureExtraction.DoDefectBlobJudgment(matSrcEdgebuffer, cvarrToMat(BurrDefectImg), matSrcDrawbuffer, 
 		nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("BURR"), EngineerDefectJudgment, pResultBlob, false);
 	cFeatureExtraction.DrawBlob(matSrcDrawbuffer, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

		int nCurrDefectCount = pResultBlob->nDefectCount;

	for(long n =nPrevDefectCount; n < nCurrDefectCount ;n++ )
	{
		pResultBlob->ptLT[n].x		+= rtCalcArea.x;
		pResultBlob->ptLT[n].y		+= rtCalcArea.y;
		pResultBlob->ptRT[n].x		+= rtCalcArea.x;	
		pResultBlob->ptRT[n].y		+= rtCalcArea.y;
		pResultBlob->ptRB[n].x		+= rtCalcArea.x;
		pResultBlob->ptRB[n].y		+= rtCalcArea.y;
		pResultBlob->ptLB[n].x		+= rtCalcArea.x;	
		pResultBlob->ptLB[n].y		+= rtCalcArea.y;
	}

	cvReleaseImage(&ImgTemp);
	cvReleaseImage(&BrokenDefectImg);
	cvReleaseImage(&BurrDefectImg);

	return E_ERROR_CODE_TRUE;
}


long CInspectEdge::PanelSizeMeasure(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner,  double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, stMeasureInfo* pMeasureInfo, Mat& mtMeasureImage, wchar_t* strContourTxt)
{

#if USE_LOG_APP_EDGE
	wchar_t	strLogPath[MAX_PATH] = {0, };
	swprintf_s(strLogPath, MAX_PATH, L"%s\\EDGE.csv", LOG_PATH_COMMON);

	cLogAlg.initialize(strLogPath, L"Panel_Size");

	cLogAlg.writeLog(__FUNCTION__, _T("Start."));
#endif

	// 파라미터 NULL인 경우.
	if( dPara == NULL )					return E_ERROR_CODE_EMPTY_PARA;
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	//Panel 모서리 4부분 좌표
	long nErrocode = E_ERROR_CODE_TRUE;

	int nBlurMaskSize			= dPara[0];		//Blur 마스크 크기
	int nThreshold				= dPara[1];		//영상 이진화 값
	int nMinSample				= dPara[2];		//RANSAC 최소 Sample 갯수
	int nDistThreshold			= dPara[3];		//RANSAC 유효Data 거리 임계치
	int nHorizontalLineStandard	= dPara[4];		//Panel Horizontal Line Size 기준
	int nVerticalLineStandard	= dPara[5];		//Panel Vertical Line Size 기준


 	cv::Point ptPanelCorner[E_CORNER_END];			//Panel 모서리
 	double dPanelLength[LINE_MAX];					//Panel 길이
// 	//Line의 좌표 정보
 	cv::Point ptLineLT[LINE_MAX], ptLineRB[LINE_MAX];

	for(int i=E_CORNER_LEFT_TOP ; i<=E_CORNER_LEFT_BOTTOM ; i++)
	{
		ptPanelCorner[i] = ptCorner[i];
	}

	for(int i=E_CORNER_LEFT_TOP ; i<=E_CORNER_LEFT_BOTTOM ; i++)
	{
		//Panel 길이 측정
		PointToPointLength(ptPanelCorner[i], ptPanelCorner[(i+1) < 4 ? i+1 : 0], dPanelLength[(i+1) < 4 ? i+1 : 0]);
	}

	//측정 결과 Data 구조체에 담기
	pMeasureInfo->dMeasureValue[LEFT] = dPanelLength[LEFT];	
	pMeasureInfo->dMeasureValue[TOP] = dPanelLength[TOP];
	pMeasureInfo->dMeasureValue[RIGHT] = dPanelLength[RIGHT];
	pMeasureInfo->dMeasureValue[BOTTOM] = dPanelLength[BOTTOM];
		
	ptLineLT[LEFT] = ptPanelCorner[E_CORNER_LEFT_TOP];
	ptLineRB[LEFT] = ptPanelCorner[E_CORNER_LEFT_BOTTOM];
	
	ptLineLT[TOP] = ptPanelCorner[E_CORNER_LEFT_TOP];
	ptLineRB[TOP] = ptPanelCorner[E_CORNER_RIGHT_TOP];

	ptLineLT[RIGHT] = ptPanelCorner[E_CORNER_RIGHT_TOP];
	ptLineRB[RIGHT] = ptPanelCorner[E_CORNER_RIGHT_BOTTOM];

	ptLineLT[BOTTOM] = ptPanelCorner[E_CORNER_LEFT_BOTTOM];
	ptLineRB[BOTTOM] = ptPanelCorner[E_CORNER_RIGHT_BOTTOM];
	
	double dLineDif[LINE_MAX];//기준 길이와 현재 측정한 길이와의 차이
	dLineDif[LEFT]	= nVerticalLineStandard - dPanelLength[LEFT];
	dLineDif[TOP]	= nHorizontalLineStandard - dPanelLength[TOP];
	dLineDif[RIGHT]	= nVerticalLineStandard - dPanelLength[RIGHT];
	dLineDif[BOTTOM]= nHorizontalLineStandard - dPanelLength[BOTTOM];

	
	const int nJudgementMax = 4;						//측정에서 사용 하는 Judgement 기준 갯수 현재 4개 2017.09.18 by LJM
	double dJudgeValue[nJudgementMax];					//Judgement 기준 Value
	int	   nSign[nJudgementMax];						//Judgement Value 기준에서의 부등호
	bool bUse[nJudgementMax];							//Judgement 사용, 미사용
	BOOL bJudge[LINE_MAX]={FALSE};	//각 Line의 판정 결과
	
	
	//측정기준 좌표가 정상적인지 확인하기 위한 영상 Draw
	CString strCornerPT;
	for(int nCornerInx = E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
	{
		strCornerPT.Format(_T("X: %d, Y: %d"), ptPanelCorner[nCornerInx].x, ptPanelCorner[nCornerInx].y);
		cv::putText(mtMeasureImage, (cv::String)(CStringA)strCornerPT, ptPanelCorner[nCornerInx], cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(0,255,0));
		cv::line(mtMeasureImage, ptPanelCorner[nCornerInx], ptPanelCorner[(nCornerInx+1 < E_CORNER_END) ? nCornerInx + 1 : 0], Scalar(0,0,255), 1);
	}


	//Measure DefectItem을 사용한다면
	if(EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_MEASURE].bDefectItemUse)
	{
		//for문
		//0: Horizontal Line MAX Spec
		//1:				 MIN Spec
		//2: Vertical Line MAX Spec
		//3:          LIne MIN Spec
		for(int nJudgeInx =0; nJudgeInx < nJudgementMax; nJudgeInx++)
		{
			dJudgeValue[nJudgeInx]	= EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_MEASURE].Judgment[nJudgeInx].dValue;
			nSign[nJudgeInx]		= EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_MEASURE].Judgment[nJudgeInx].nSign;
			bUse[nJudgeInx]			= EngineerDefectJudgment[E_DEFECT_JUDGEMENT_APP_MEASURE].Judgment[nJudgeInx].bUse > 0 ? true : false;	
		}

		CFeatureExtraction cFeature; 


		//불량 판정
		if(bUse[0])
		{
			bJudge[TOP]		+= cFeature.Compare(dLineDif[TOP], nSign[0], dJudgeValue[0]);
			bJudge[BOTTOM]	+= cFeature.Compare(dLineDif[BOTTOM], nSign[0], dJudgeValue[0]);
		}
		if(bUse[1])
		{
			bJudge[TOP]		+= cFeature.Compare(dLineDif[TOP], nSign[1], dJudgeValue[1]);
			bJudge[BOTTOM]	+= cFeature.Compare(dLineDif[BOTTOM], nSign[1], dJudgeValue[1]);
		}
		if(bUse[2])
		{
			bJudge[LEFT]	+= cFeature.Compare(dLineDif[LEFT], nSign[2], dJudgeValue[2]);
			bJudge[RIGHT]	+= cFeature.Compare(dLineDif[RIGHT], nSign[2], dJudgeValue[2]);
		}
		if(bUse[3])
		{
			bJudge[LEFT]	+= cFeature.Compare(dLineDif[LEFT], nSign[3], dJudgeValue[3]);
			bJudge[RIGHT]	+= cFeature.Compare(dLineDif[RIGHT], nSign[3], dJudgeValue[3]);
		}

		int& nDefectCount = pResultBlob->nDefectCount;

		for(int nLineIdx = LEFT; nLineIdx < LINE_MAX; nLineIdx++)
		{
			if(bJudge[nLineIdx])
			{
				
				pResultBlob->ptLT			[nDefectCount].x	= ptLineLT[nLineIdx].x;
				pResultBlob->ptLT			[nDefectCount].y	= ptLineLT[nLineIdx].y;
				pResultBlob->ptLB			[nDefectCount].x	= ptLineLT[nLineIdx].x;	
				pResultBlob->ptLB			[nDefectCount].y	= ptLineRB[nLineIdx].y;
				pResultBlob->ptRT			[nDefectCount].x	= ptLineRB[nLineIdx].x;
				pResultBlob->ptRT			[nDefectCount].y	= ptLineLT[nLineIdx].y;
				pResultBlob->ptRB			[nDefectCount].x	= ptLineRB[nLineIdx].x;
				pResultBlob->ptRB			[nDefectCount].y	= ptLineRB[nLineIdx].y;
				pResultBlob->nArea			[nDefectCount]		= dPanelLength[nLineIdx];
				pResultBlob->nDefectJudge	[nDefectCount] = E_DEFECT_JUDGEMENT_APP_MEASURE;			// 해당 불량
				pResultBlob->nPatternClassify[nDefectCount]= nCommonPara[10];							//UI 상 패턴 순서 영상 번호

				nDefectCount++;

				cv::line(matDrawBuffer, ptLineLT[nLineIdx], ptLineRB[nLineIdx], Scalar(0,0,255), 1);

				pMeasureInfo->bJudge = false;
			}
		}

	}
	


	return nErrocode;
}

long CInspectEdge::PanelSizeMeasure_CrossMark(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner,  double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, stMeasureInfo* pMeasureInfo, Mat& mtMeasureImage, CString strCurModelPath, vector<CRect> pFilterROI, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt)
{
	Mat mtDebug;
	matDrawBuffer.copyTo(mtDebug);
	bool	bImageSave				= (nCommonPara[1] > 0) ? true : false;	// 1 : 알고리즘 중간결과 Image Save 
	int nErrorcode = E_ERROR_CODE_TRUE;

	double dCameraResolution = pMeasureInfo->dCameraResolution;

	int nPos=0;

	int nROISize					=	(int)dPara[nPos++];
	int nThreshold[LINE_MAX];		
	nThreshold[LEFT]				=	(int)dPara[nPos++];
	nThreshold[TOP]					=	(int)dPara[nPos++];
	nThreshold[RIGHT]				=	(int)dPara[nPos++];
	nThreshold[BOTTOM]				=	(int)dPara[nPos++];
	float fPanelEdgeAvg_Offset		=	(float)dPara[nPos++];
	float fStandardDist[LINE_MAX];
	fStandardDist[LEFT]				=	(float)dPara[nPos++];
	fStandardDist[TOP]				=	(float)dPara[nPos++];
	fStandardDist[RIGHT]			=	(float)dPara[nPos++];
	fStandardDist[BOTTOM]			=	(float)dPara[nPos++];
	float	fMarkDist				=   (float)dPara[nPos++];
	float	fThresh_Resolution		=	(float)dPara[nPos++];		//Resolution 오차 허용범위
	int		nSamplingNum			=	(int)dPara[nPos++];			//측정 Sampling 갯수
	BOOL	bUseRansac				=	(BOOL)dPara[nPos++];
	BOOL	bUseSubPixel			=	(BOOL)dPara[nPos++];

	int		nBMSearchRange			=	(int)dPara[nPos++];			//Panel Edge Line에서 BM을 찾아 들어갈 범위 크기
	float fBMLimitTheta[LINE_MAX];									//BM Line Pro pile 기준 임계 Theta
	fBMLimitTheta[LEFT]				=	(int)dPara[nPos++];
	fBMLimitTheta[TOP]				=	(int)dPara[nPos++];
	fBMLimitTheta[BOTTOM]			=	(int)dPara[nPos++];
		


	int nCCutOffset[E_CORNER_END];										//C-Cut영역을 제외하기 위한 모서리로 부터의 Offset 길이

	for(int nCornerInx = 0; nCornerInx < E_CORNER_END; nCornerInx++)
		nCCutOffset[nCornerInx] = (int)dPara[nPos++];

	bool bInspUseLine[LINE_MAX] = {false,};											//검사를 진행하는 Line
	for(int nLineInx = LEFT; nLineInx < LINE_MAX; nLineInx++)
		bInspUseLine[nLineInx] = (int)dPara[nPos++] > 0 ? true : false;


	//1. Cross Mark Search를 위한 ROI영상 생성
	Mat mtROI[E_CORNER_END];

	Rect rtROI[E_CORNER_END];
	rtROI[E_CORNER_RIGHT_TOP]		= Rect(ptCorner[E_CORNER_RIGHT_TOP].x-nROISize, ptCorner[E_CORNER_RIGHT_TOP].y, nROISize, nROISize);
	rtROI[E_CORNER_RIGHT_BOTTOM]	= Rect(ptCorner[E_CORNER_RIGHT_BOTTOM].x-nROISize, ptCorner[E_CORNER_RIGHT_BOTTOM].y-nROISize, nROISize, nROISize);

	matSrcBuffer(rtROI[E_CORNER_RIGHT_TOP]).copyTo(mtROI[E_CORNER_RIGHT_TOP]);
	matSrcBuffer(rtROI[E_CORNER_RIGHT_BOTTOM]).copyTo(mtROI[E_CORNER_RIGHT_BOTTOM]);

	//2. Reference Image Read
	Mat mtRefCross[E_CORNER_END];

	mtRefCross[E_CORNER_RIGHT_TOP]		= cvarrToMat(stPadInfo[E_PAD_ALIGN_MARK][0].GetImage(0));
	mtRefCross[E_CORNER_RIGHT_BOTTOM]	= cvarrToMat(stPadInfo[E_PAD_ALIGN_MARK][0].GetImage(1));

	if(mtRefCross[E_CORNER_RIGHT_TOP].empty() || mtRefCross[E_CORNER_RIGHT_BOTTOM].empty())
		return E_ERROR_CODE_EMPTY_CROSS_MARK_BUFFER;

	//3. Pattern Matching
	Mat mtTemplateRst[E_CORNER_END];
	cv::Point ptMatch[E_CORNER_END];
	Point2f ptMarkROI[E_CORNER_END];		//ROI영상에서의 Mark 좌표
	Point2f ptMarkOrg[E_CORNER_END];		//원본영상에서의 Mark 좌표
	
	CString strROI;

	for(int nCornerInx = E_CORNER_RIGHT_TOP; nCornerInx <= E_CORNER_RIGHT_BOTTOM; nCornerInx++)
	{
		if(!MARKPOSITION(mtROI[nCornerInx], mtRefCross[nCornerInx], ptMarkROI[nCornerInx]))
			return E_ERROR_CODE_CROSS_MARK_SEARCH_FAIL;//Mark가 없을 경우 
		
		ptMarkOrg[nCornerInx] = Point2f(rtROI[nCornerInx].x + ptMarkROI[nCornerInx].x, rtROI[nCornerInx].y + ptMarkROI[nCornerInx].y);
		
		if(bImageSave)	circle(mtMeasureImage, Point(ptMarkOrg[nCornerInx].x, ptMarkOrg[nCornerInx].y), 1, Scalar(0,0,255));
		
	}

	double dMarkDistance;
	double dMarkDistance_X;
	double dMarkDistance_Y;
	
	dMarkDistance_X = ptMarkOrg[E_CORNER_RIGHT_TOP].x - ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x;
	dMarkDistance_Y = ptMarkOrg[E_CORNER_RIGHT_TOP].y - ptMarkOrg[E_CORNER_RIGHT_BOTTOM].y;
	dMarkDistance = sqrt(pow(dMarkDistance_X,2) + pow(dMarkDistance_Y,2));

	//4. 4개의 직선 방정식 계산
	double dA[LINE_MAX];
	double dB[LINE_MAX];
	
	bool bReverse = false;
	

	if(dMarkDistance_X == 0){
		dA[RIGHT] = dMarkDistance_X/dMarkDistance_Y;
		dB[RIGHT] = ptMarkOrg[E_CORNER_RIGHT_TOP].x - dA[RIGHT]*ptMarkOrg[E_CORNER_RIGHT_TOP].y;
		bReverse = true;

		if(bImageSave)
		{
			line(mtMeasureImage, Point(dA[RIGHT]*ptMarkOrg[E_CORNER_RIGHT_TOP].y + dB[RIGHT], ptMarkOrg[E_CORNER_RIGHT_TOP].y),
				Point(dA[RIGHT]*ptMarkOrg[E_CORNER_RIGHT_BOTTOM].y + dB[RIGHT], ptMarkOrg[E_CORNER_RIGHT_BOTTOM].y ), Scalar(255,255,0));
		}	
		line(matDrawBuffer, Point(dA[RIGHT]*ptMarkOrg[E_CORNER_RIGHT_TOP].y + dB[RIGHT], ptMarkOrg[E_CORNER_RIGHT_TOP].y),
			Point(dA[RIGHT]*ptMarkOrg[E_CORNER_RIGHT_BOTTOM].y + dB[RIGHT], ptMarkOrg[E_CORNER_RIGHT_BOTTOM].y ), Scalar(255,255,0));
	}
	else{						
		dA[RIGHT] = dMarkDistance_Y/dMarkDistance_X;
		dB[RIGHT] = ptMarkOrg[E_CORNER_RIGHT_TOP].y - dA[RIGHT]*ptMarkOrg[E_CORNER_RIGHT_TOP].x;
		bReverse = false;

		if(bImageSave){
			line(mtMeasureImage, Point(ptMarkOrg[E_CORNER_RIGHT_TOP].x , dA[RIGHT]*ptMarkOrg[E_CORNER_RIGHT_TOP].x + dB[RIGHT]),
				Point(ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x, dA[RIGHT]*ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x + dB[RIGHT]), Scalar(255,255,0));
		}
		line(matDrawBuffer, Point(ptMarkOrg[E_CORNER_RIGHT_TOP].x , dA[RIGHT]*ptMarkOrg[E_CORNER_RIGHT_TOP].x + dB[RIGHT]),
			Point(ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x, dA[RIGHT]*ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x + dB[RIGHT]), Scalar(255,255,0));
	}
	
	//BMLine Fitting
	bool bPadofActive[LINE_MAX] = {true,};

#pragma omp parallel for
	for(int nLineInx = LEFT; nLineInx < LINE_MAX; nLineInx++)
	{
		if(nLineInx != RIGHT)
			bPadofActive[nLineInx] = PadofActiveLineFitting(matSrcBuffer, dA[nLineInx], dB[nLineInx], ptCorner, nLineInx, fBMLimitTheta[nLineInx], nCCutOffset, nBMSearchRange);
	}
	if(!bPadofActive[LEFT] || !bPadofActive[TOP] || !bPadofActive[BOTTOM])
		return E_ERROR_CODE_MEASURE_BM_FITTING_FAIL;

	line(mtMeasureImage, Point(ptCorner[E_CORNER_LEFT_TOP].x+200 , dA[TOP]*(ptMarkOrg[E_CORNER_LEFT_TOP].x + 200) + dB[TOP]),
		Point(ptCorner[E_CORNER_RIGHT_TOP].x-200 , dA[TOP]*(ptMarkOrg[E_CORNER_RIGHT_TOP].x - 200) + dB[TOP]), Scalar(255,255,0));
	line(mtMeasureImage, Point(ptCorner[E_CORNER_LEFT_BOTTOM].x+200 , dA[BOTTOM]*(ptMarkOrg[E_CORNER_LEFT_BOTTOM].x + 200) + dB[BOTTOM]),
		Point(ptCorner[E_CORNER_RIGHT_BOTTOM].x-200 , dA[BOTTOM]*(ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x - 200) + dB[BOTTOM]), Scalar(255,255,0));

	dB[TOP] = ptMarkOrg[E_CORNER_RIGHT_TOP].y - dA[TOP]*ptMarkOrg[E_CORNER_RIGHT_TOP].x;
	dB[BOTTOM] = ptMarkOrg[E_CORNER_RIGHT_BOTTOM].y - dA[BOTTOM]*ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x;

	if(bImageSave)
	{
		line(mtMeasureImage, Point(ptMarkOrg[E_CORNER_RIGHT_TOP].x , dA[TOP]*ptMarkOrg[E_CORNER_RIGHT_TOP].x + dB[TOP]),
			Point(0, dB[TOP]), Scalar(255,255,0));
		line(mtMeasureImage, Point(ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x, dA[BOTTOM]*ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x + dB[BOTTOM]),
			Point(0, dB[BOTTOM]), Scalar(255, 255, 0));
	}

	line(matDrawBuffer, Point(ptMarkOrg[E_CORNER_RIGHT_TOP].x , dA[TOP]*ptMarkOrg[E_CORNER_RIGHT_TOP].x + dB[TOP]),
		Point(0, dB[TOP]), Scalar(255,255,0));
	line(matDrawBuffer, Point(ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x, dA[BOTTOM]*ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x + dB[BOTTOM]),
		Point(0, dB[BOTTOM]), Scalar(255, 255, 0));

	Point ptLeft_ST, ptLeft_ED;
	ptLeft_ST = Point(-dB[LEFT]/ dA[LEFT],0);
	ptLeft_ED = Point((matSrcBuffer.rows-1-dB[LEFT])/dA[LEFT], matSrcBuffer.rows-1);

	if(bImageSave)
	{
		line(mtMeasureImage, ptLeft_ST, ptLeft_ED, Scalar(255, 255, 0));
	}
	line(matDrawBuffer, ptLeft_ST, ptLeft_ED, Scalar(255, 255, 0));
	
	//5. Panel Edge 좌표 취득
	unsigned int nGV_Level=0;
	vector<cv::Point2f>	ptPanelEdge[LINE_MAX];

	Mat mtSrc;
	matSrcBuffer.copyTo(mtSrc);
	blur(mtSrc,mtSrc,Size(3,3));
	Mat mtThresh = Mat::zeros(matSrcBuffer.size(), CV_8UC1);

	Mat mtDraw;
	cv::cvtColor(mtSrc, mtDraw, CV_GRAY2BGR);
	
	if(bUseSubPixel==FALSE)
	{
		cv::threshold(mtSrc, mtThresh, nThreshold[LEFT], 255, THRESH_BINARY);

		#pragma omp parallel for
		for(int nLineInx = LEFT; nLineInx < LINE_MAX; nLineInx++)
		{
			GetEdgeLine(mtThresh, ptPanelEdge[nLineInx], ptCorner, nLineInx, ptMarkOrg, bImageSave, pFilterROI);
		}
	}
	else
	{
		#pragma omp parallel for
		for(int nLineInx = LEFT; nLineInx < LINE_MAX; nLineInx++)
		{
			subPixeling_RectangleEdge(mtSrc,mtDraw,nThreshold[nLineInx],ptCorner, nLineInx, ptPanelEdge[nLineInx], bImageSave, pFilterROI, ptMarkOrg, fPanelEdgeAvg_Offset, nCCutOffset);
		}
		if(	ptPanelEdge[LEFT].size() < 1	||
			ptPanelEdge[TOP].size() < 1		||
			ptPanelEdge[RIGHT].size() < 1	||
			ptPanelEdge[BOTTOM].size() < 1	)
			return E_ERROR_CODE_MEASURE_PANEL_FITTING_FAIL;
	}

	if(bImageSave)
	{
		CString strAresult;
		strAresult.Format(_T("%s\\SearchLine.bmp"), strAlgPath);
		imwrite((cv::String)(CStringA)strAresult, mtDraw);
	}

	if(bUseRansac)
	{
		//Sampling 갯수 만큼씩 Ransac하여 좌표 변환
		for(int nLineInx=LEFT; nLineInx < LINE_MAX; nLineInx++)
		{
			if(SamplingRansac(ptPanelEdge[nLineInx], nSamplingNum, nLineInx) != E_ERROR_CODE_TRUE)
				return E_ERROR_CODE_FALSE;
		}
	}

	//7. 측정 진행

// 	float fCurMarkDistance_PX = sqrt(pow(ptMarkOrg[E_CORNER_RIGHT_TOP].x - ptMarkOrg[E_CORNER_RIGHT_BOTTOM].x, 2) + pow(ptMarkOrg[E_CORNER_RIGHT_TOP].y - ptMarkOrg[E_CORNER_RIGHT_BOTTOM].y, 2));
// 	float fCurResolution = fMarkDist/fCurMarkDistance_PX;
// 	pMeasureInfo->dMeasureValue[12] = fCurMarkDistance_PX;
// 	if(abs(fCurResolution - dCameraResolution) > fThresh_Resolution)	//Resolution차이가 심하다는 것은 Curr이 심하게 형성 되었다고 판단하여 검사 Skip
// 		return E_ERROR_CODE_FALSE;

	double fCurResolution = dCameraResolution;

	for(int nLineInx = LEFT; nLineInx < LINE_MAX; nLineInx++)
	{
		fStandardDist[nLineInx] /= fCurResolution;
	}

	Point ptRefLine;
	double	dAvgLength[LINE_MAX];				//평균 길이
	double  dSumLength[LINE_MAX]={0,};			//길이 총합
	double	dMinLength[LINE_MAX]={(double)mtSrc.cols,};	//현재길이 최소값
	double	dMaxLength[LINE_MAX]={0};											//현재길이 최대값
	double	dDifDist;							//현재 길이 - 기준길이
	float fDistMin[LINE_MAX] = {(float)mtSrc.cols,};	//현재 길이 - 기준길이 최소값
	float fDistMax[LINE_MAX] = {0,};				//현재 길이 - 기준길이 최대값
	int	  nDistMinInx[LINE_MAX]={0,};
	int	  nDistMaxInx[LINE_MAX]={0,};
	int	  nDistMinLengthInx[LINE_MAX]={0,};
	int	  nDistMaxLengthInx[LINE_MAX]={0,};
	float	fLength;
	CString strLength;
	vector<float> pLength[E_CORNER_END];
	BOOL bJudge_Max[LINE_MAX]={0,};			//각 라인별 판정 결과
	BOOL bJudge_Min[LINE_MAX]={0,};
	double dAvg_PT100_Dist;
	double dSum_PT100_Dist;
	int nCnt=0;
	int nLengthCnt=0;
	
	for(int nLineInx=LEFT; nLineInx < LINE_MAX; nLineInx++)
	{
		dSum_PT100_Dist=0;
		nCnt=0;
		for(int nInx=0; nInx < ptPanelEdge[nLineInx].size(); nInx++)
		{
			if(bReverse && nLineInx == RIGHT)	
				GetDistance_PointToLine(ptPanelEdge[nLineInx][nInx].x, ptPanelEdge[nLineInx][nInx].y, 1, -dA[nLineInx], -dB[nLineInx], fLength);
			else			
				GetDistance_PointToLine(ptPanelEdge[nLineInx][nInx].x, ptPanelEdge[nLineInx][nInx].y, -dA[nLineInx], 1, -dB[nLineInx], fLength);

			JudgePointDirection(ptPanelEdge[nLineInx][nInx], dA[nLineInx], dB[nLineInx], nLineInx, bReverse, fLength);

			dSumLength[nLineInx] += fLength;
			dSum_PT100_Dist+=(double)fLength;
			nCnt++;
			
			if(nCnt==nSamplingNum)
			{
				dAvg_PT100_Dist = dSum_PT100_Dist/nCnt;
				strLength.Format(_T("%.2f"), dAvg_PT100_Dist*fCurResolution);
				dDifDist = dAvg_PT100_Dist - fStandardDist[nLineInx];
				pLength[nLineInx].push_back((float)dAvg_PT100_Dist);
				if(dDifDist < fDistMin[nLineInx]){
					fDistMin[nLineInx] = dDifDist;
					nDistMinInx[nLineInx] = nInx;
					nDistMinLengthInx[nLineInx] = nLengthCnt;
				}

				if(dDifDist > fDistMax[nLineInx]){
					fDistMax[nLineInx] = dDifDist;
					nDistMaxInx[nLineInx] = nInx;
					nDistMaxLengthInx[nLineInx] = nLengthCnt;
				}
				if(dMaxLength[nLineInx] < fLength)
				{
					dMaxLength[nLineInx] = dAvg_PT100_Dist;
				}
				if(dMinLength[nLineInx] > fLength)
				{
					dMinLength[nLineInx] = dAvg_PT100_Dist;
				}

				if(bImageSave)
				{
					circle(mtMeasureImage, ptPanelEdge[nLineInx][nInx], 1, Scalar(0,255,255));
					cv::putText(mtMeasureImage, (cv::String)(CStringA)strLength, ptPanelEdge[nLineInx][nInx], cv::FONT_HERSHEY_SIMPLEX, 0.3f, Scalar(255,0,0));			
				}

				nCnt=0;
				nLengthCnt++;
				dSum_PT100_Dist=0;
			}
		}
		nLengthCnt=0;
		dAvgLength[nLineInx] = dSumLength[nLineInx]/ptPanelEdge[nLineInx].size();

		pMeasureInfo->dMeasureValue[nLineInx*3]	  = pLength[nLineInx][0] * fCurResolution;
		pMeasureInfo->dMeasureValue[nLineInx*3+1] = pLength[nLineInx][pLength[nLineInx].size()-1] * fCurResolution;
		pMeasureInfo->dMeasureValue[nLineInx*3+2] = dAvgLength[nLineInx] * fCurResolution;
	}

	if(bImageSave){
		CString strAresult;
		strAresult.Format(_T("%s\\Measure.bmp"), strAlgPath);
		imwrite((cv::String)(CStringA)strAresult, mtMeasureImage);
	}

	//각도 측정 현재 필요 없을 것으로 판단 되므로 사용 보류
// 	long double dA_PanelEdge[LINE_MAX];
// 	long double dB_PanelEdge[LINE_MAX];
// 	float dPanelEdgeTH[LINE_MAX];
// 	float dPanelEdgeTH_Degree[LINE_MAX];
// 	for(int nLineInx=LEFT; nLineInx < LINE_MAX; nLineInx++)
// 	{
// 		nErrorcode = calcRANSAC(ptPanelEdge[nLineInx], dA_PanelEdge[nLineInx], dB_PanelEdge[nLineInx], 3, 5);
// 		if(nErrorcode != E_ERROR_CODE_FALSE){
// 			dPanelEdgeTH[nLineInx] = atan(dA_PanelEdge[nLineInx]);
// 			dPanelEdgeTH_Degree[nLineInx] = dPanelEdgeTH[nLineInx]*180/CV_PI;
// 		}
// 	}
	
	for(int nLineInx=0; nLineInx < LINE_MAX; nLineInx++)
	{
		
		if(nLineInx%2==0)
		{
			bJudge_Max[nLineInx] = MeasureJudgement(EngineerDefectJudgment, E_DEFECT_JUDGEMENT_APP_MEASURE, fDistMax[nLineInx] * fCurResolution, 0);
			bJudge_Min[nLineInx] = MeasureJudgement(EngineerDefectJudgment, E_DEFECT_JUDGEMENT_APP_MEASURE, fDistMin[nLineInx] * fCurResolution, 1);
		}
		else
		{
			bJudge_Max[nLineInx] = MeasureJudgement(EngineerDefectJudgment, E_DEFECT_JUDGEMENT_APP_MEASURE, fDistMax[nLineInx] * fCurResolution, 2);
			bJudge_Min[nLineInx] = MeasureJudgement(EngineerDefectJudgment, E_DEFECT_JUDGEMENT_APP_MEASURE, fDistMin[nLineInx] * fCurResolution, 3);
		}

		if(bInspUseLine[nLineInx] == false)
		{
			bJudge_Max[nLineInx] = FALSE;
			bJudge_Min[nLineInx] = FALSE;
		}
	}
	

	int& nDefectCount = pResultBlob->nDefectCount;
	int nPtInx=0;
	int nLengthInx=0;
	float fDistValue;
	for(int nLineIdx = LEFT; nLineIdx < LINE_MAX; nLineIdx++)
	{
		if(bJudge_Max[nLineIdx] || bJudge_Min[nLineIdx])
		{
			bool bMax = bJudge_Max[nLineIdx] >= bJudge_Min[nLineIdx] ? 1 : 0;

			if(bMax){
				nPtInx = nDistMaxInx[nLineIdx];
				nLengthInx = nDistMaxLengthInx[nLineIdx];
				fDistValue = fDistMax[nLineIdx];
			}
			else{
				nPtInx = nDistMinInx[nLineIdx];
				nLengthInx = nDistMinLengthInx[nLineIdx];
				fDistValue = fDistMin[nLineIdx];
			}
		

			pResultBlob->ptLT			[nDefectCount].x	= ptPanelEdge[nLineIdx][nPtInx].x;
			pResultBlob->ptLT			[nDefectCount].y	= ptPanelEdge[nLineIdx][nPtInx].y;
			pResultBlob->ptLB			[nDefectCount].x	= ptPanelEdge[nLineIdx][nPtInx].x;	
			pResultBlob->ptLB			[nDefectCount].y	= ptPanelEdge[nLineIdx][nPtInx].y;
			pResultBlob->ptRT			[nDefectCount].x	= ptPanelEdge[nLineIdx][nPtInx].x;	
			pResultBlob->ptRT			[nDefectCount].y	= ptPanelEdge[nLineIdx][nPtInx].y;
			pResultBlob->ptRB			[nDefectCount].x	= ptPanelEdge[nLineIdx][nPtInx].x;
			pResultBlob->ptRB			[nDefectCount].y	= ptPanelEdge[nLineIdx][nPtInx].y;
			pResultBlob->nArea			[nDefectCount]		= pLength[nLineIdx][nLengthInx];
			pResultBlob->nDefectJudge	[nDefectCount] = E_DEFECT_JUDGEMENT_APP_MEASURE;			// 해당 불량
			pResultBlob->nPatternClassify[nDefectCount]= nCommonPara[10];							//UI 상 패턴 순서 영상 번호

			nDefectCount++;

			CString strDistValue;
			strDistValue.Format(_T("%.2f [Pixel]"), pLength[nLineIdx][nLengthInx]);
			circle(matDrawBuffer, ptPanelEdge[nLineIdx][nPtInx], 1, Scalar(255,255,0));
			putText(matDrawBuffer, (cv::String)(CStringA)strDistValue, ptPanelEdge[nLineIdx][nPtInx], cv::FONT_HERSHEY_SIMPLEX, 0.4f, Scalar(255,255,0));
			
			strDistValue.Format(_T("%.2f [um]"), pLength[nLineIdx][nLengthInx]*fCurResolution);
			putText(matDrawBuffer, (cv::String)(CStringA)strDistValue, Point(ptPanelEdge[nLineIdx][nPtInx].x, ptPanelEdge[nLineIdx][nPtInx].y + 15), cv::FONT_HERSHEY_SIMPLEX, 0.4f, Scalar(255,255,0));
			
			strDistValue.Format(_T("Dif: %.2f [um]"), (pLength[nLineIdx][nLengthInx] - fStandardDist[nLineIdx]) * fCurResolution);
			putText(matDrawBuffer, (cv::String)(CStringA)strDistValue, Point(ptPanelEdge[nLineIdx][nPtInx].x, ptPanelEdge[nLineIdx][nPtInx].y + 30), cv::FONT_HERSHEY_SIMPLEX, 0.4f, Scalar(255,255,0));

			pMeasureInfo->bJudge = false;
		}
	}
	

	return nErrorcode;
}

bool CInspectEdge::PointToPointLength(cv::Point pt1, cv::Point pt2, double& dLength)
{
	bool bRet = true;

	long double dX_2pow = pow((double)(pt1.x - pt2.x), 2);
	long double dY_2pow = pow((double)(pt1.y - pt2.y), 2);

	dLength = sqrt( dX_2pow + dY_2pow );

	return bRet;
}

void CInspectEdge::FindBiggestBlob(cv::Mat& src, cv::Mat& dst)
{
	int largest_area=0;
	int largest_contour_index=0;

	vector< vector<Point> > contours; // Vector for storing contour
	vector<Vec4i> hierarchy;

	findContours( src, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
	cv::Rect bounding_rect;
	for( int i = 0; i< contours.size(); i++ ) {// iterate through each contour. 
		double a=contourArea( contours[i],false);  //  Find the area of contour
		if(a>largest_area){
			largest_area=a;
			largest_contour_index=i;              //Store the index of largest contour
			bounding_rect=boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
		}
	}

	drawContours( dst, contours, largest_contour_index, Scalar(255), CV_FILLED, 8, hierarchy ); // Draw the largest contour using previously stored index.
}

void CInspectEdge::FindBiggestBoundingRt(cv::Mat& src, cv::Mat& dst)
{
	int largest_area=0;
	int largest_contour_index=0;

	vector< vector<Point> > contours; // Vector for storing contour
	vector<Vec4i> hierarchy;

	findContours( src, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
	cv::Rect bounding_rect;
	for( int i = 0; i< contours.size(); i++ ) {// iterate through each contour. 
		bounding_rect=boundingRect(contours[i]);
		double a= bounding_rect.width*bounding_rect.height;
		if(a>largest_area){
			largest_area=a;
			largest_contour_index=i;              //Store the index of largest contour
			 // Find the bounding rectangle for biggest contour
		}
	}

	drawContours( dst, contours, largest_contour_index, Scalar(255), CV_FILLED, 8, hierarchy ); // Draw the largest contour using previously stored index.
}

BOOL CInspectEdge::CheckCCutROI(Rect rtROI)
{
	BOOL bRet = true;
	if(rtROI.x < 0)						bRet *= false;
	if(rtROI.y < 0)						bRet *= false;
	if((rtROI.width + rtROI.x) <= 0)	bRet *= false;
	if((rtROI.height + rtROI.y) <= 0)	bRet *= false;
	
	return bRet;
}

bool CInspectEdge::MARKPOSITION(cv::Mat Src, cv::Mat Mark, cv::Point2f& mPosition)
{
	//cv::Mat Match(Src.rows, Src.cols, CV_8UC1);
	Mat Match;
	cv::matchTemplate(Src, Mark, Match, CV_TM_CCOEFF_NORMED);
	//cv::normalize(Match, Match, 1, 0, CV_MINMAX);

	double minValue,maxValue;
	cv::Point2i minloc, ptMatch;
	cv::minMaxLoc(Match, &minValue, &maxValue, NULL, &ptMatch);
	cv::Rect rect(ptMatch.x, ptMatch.y, Mark.cols, Mark.rows); 

	if(maxValue < 0.8)
		return false;



	float* fMatchScore_arr = (float*)Match.ptr(ptMatch.y);
	double fMatchScore;
	//X 방향 Matching Rate Line Pitting
	vector<Point2f> ptMatchScore_X, ptMatchScore_Y;

	int nSamplingNum = 5;
	for(int nX = ptMatch.x-nSamplingNum; nX <= ptMatch.x+nSamplingNum; nX++)
	{

		if(nX < 0 || nX > Match.cols-1)	continue;
		else	fMatchScore = *(fMatchScore_arr+nX)*100;

		ptMatchScore_X.push_back(Point2f(nX, fMatchScore));
	}

	for(int nY = ptMatch.y-nSamplingNum; nY <= ptMatch.y+nSamplingNum; nY++)
	{
		if(nY < 0 || nY > Match.rows-1) continue;
		else{
			fMatchScore_arr = (float*)Match.ptr(nY);
			fMatchScore = *(fMatchScore_arr+ptMatch.x)*100;

			ptMatchScore_Y.push_back(Point2f(nY, fMatchScore));
		}
	}

	//곡선 Pitting 상수
	long double dA,dB,dC;
	long double dX_Maxpt, dY_Maxpt;

	if(calcCurveFit(ptMatchScore_X, dA, dB, dC)==-1)	return FALSE;
	dX_Maxpt = -1*(dB/(dA*2));

	if(calcCurveFit(ptMatchScore_Y, dA, dB, dC)==-1)	return FALSE;
	dY_Maxpt = -1*(dB/(dA*2));


	mPosition = Point2f(dX_Maxpt+Mark.cols/2, dY_Maxpt+Mark.rows/2);

	return true;
}

BOOL CInspectEdge::MeasureJudgement(STRU_DEFECT_ITEM* EngineerDefectJudgment, int nDefectItemInx, double dValue, int nStandardInx)
{
	double dJudgeValue;
	int		nSign;
	BOOL	bUse;		

	BOOL bJudge;

	//Measure DefectItem을 사용한다면
	if(EngineerDefectJudgment[nDefectItemInx].bDefectItemUse)
	{
			dJudgeValue		= EngineerDefectJudgment[nDefectItemInx].Judgment[nStandardInx].dValue;
			nSign			= EngineerDefectJudgment[nDefectItemInx].Judgment[nStandardInx].nSign;
			bUse			= EngineerDefectJudgment[nDefectItemInx].Judgment[nStandardInx].bUse;	
		
		CFeatureExtraction cFeature; 


		//불량 판정
		if(bUse)
		{
			bJudge		= cFeature.Compare(dValue, nSign, dJudgeValue);
		}
	
	}
	
	return bJudge;
}

long CInspectEdge::SamplingRansac(vector<cv::Point2f>& pt, int nSampling, int nLineInx)
{
	if(pt.size() < nSampling)
		return false;
	
	int		nMinSamples = 3;
	double	distThreshold = 5;
	long double *dA, *dB;

	int nSize = pt.size();

	double dPartNum = (double)nSize / nSampling;

	vector<cv::Point2f> ptBackup = pt;
	vector<cv::Point2i> ptBackup_Int;

	for(int nInx=0; nInx < pt.size(); nInx++)
	{
		ptBackup_Int.push_back(cv::Point2f(pt[nInx].x, pt[nInx].y));
	}

	vector<cv::Point2f> *ptPart;
	ptPart = new vector<cv::Point2f>[(int)dPartNum];
	dA = new long double[(int)dPartNum];
	dB = new long double[(int)dPartNum];

	for(int n=0; n < (int)dPartNum; n++)
	{
		for(int j=n*nSampling; j < (n+1)*nSampling; j++)
		{
			ptPart[n].push_back(pt[j]);
		}

		long nMaxCost = 0;

		// 벡터 초기화
		vector <cv::Point2i> ptSamples, ptInliers;

		// 임시 라인 방정식 y = ax + b
		long double dAA = 0, dBB = 0;

		// 반복 횟수 정하기
		int nMaxIter = (int)(1 + log(1. - 0.99)/log(1. - pow(0.5, nMinSamples)));

		for (int i = 0; i<nMaxIter; i++)
		{
			// 벡터 초기화
			vector<cv::Point2i>().swap(ptSamples);
			vector<cv::Point2i>().swap(ptInliers);

			// hypothesis
			// 원본 데이터에서 임의로 N개의 샘플 데이터를 취득
			GetRandomSamples(ptBackup_Int, ptSamples, nMinSamples);

			// 현재 데이터를 정상적인 데이터로 보고 모델 파라미터 예측
			long nErrorCode = calcLineFit(ptSamples, dAA, dBB);

			// 에러인 경우, 에러 코드 출력
			if( nErrorCode != E_ERROR_CODE_TRUE )
			{
				vector<cv::Point2i>().swap(ptSamples);
				vector<cv::Point2i>().swap(ptInliers);

				return nErrorCode;
			}

			// Verification
			// 원본 데이터가 예측된 모델에 잘 맞는지 검사
			long cost = calcLineVerification(ptBackup_Int, ptInliers, dAA, dBB, distThreshold);

			// 만일 예측된 모델이 잘 맞는다면, 유효한 데이터로 새로운 모델을 구함
			if( nMaxCost < cost )
			{
				nMaxCost = cost;
				calcLineFit(ptInliers, dA[n], dB[n]);
			}

			// 벡터 데이터 제거
			vector<cv::Point2i>().swap(ptInliers);
		}

		// 벡터 데이터 제거
		vector<cv::Point2i>().swap(ptSamples);

		// 계산 불가
		if( nMaxCost <= 0 )		return E_ERROR_CODE_ALIGN_CAN_NOT_CALC;
	}

	vector<cv::Point2f>().swap(pt);

	long double dPartA;
	long double dPartB;

	int nPartInx=0;
	int nX,nY;
	
	for(int n=0; n < nSize; n++)
	{
		dPartA = dA[nPartInx];
		dPartB = dB[nPartInx];

		if(nLineInx%2==0)
		{
			nY = ptBackup[n].y;
			nX = (nY - dPartB) / dPartA;
		}
		else
		{
			nX = ptBackup[n].x;
			nY = dPartA*nX + dPartB;
		}

		pt.push_back(cv::Point2f(nX,nY));

		if((nPartInx+1) * nSampling < n + 1)
		{
			if(nPartInx < (int)dPartNum-1)
				nPartInx++;
		}
	}

	delete[] ptPart;
	delete[] dA;
	delete[] dB;

	return E_ERROR_CODE_TRUE;
}

bool CInspectEdge::subPixeling_RectangleEdge(Mat mtSrc, Mat& mtDraw, int nThreshold_Theta, Point* ptCorner, int nLineInx, vector<cv::Point2f>& pt, bool bImageSave,  vector<CRect> pFilterROI, Point2f* ptMark, int fAvgOffset, int* nCCutOffset)
{
	int nRangeOffset = 200;
	int nSearchRangeOffset = 10;
	uchar* ucImgData;

	int nGV_Level, nGV_Level2;

	vector<cv::Point> ptProfile;
	long double dA, dB;

	double dX,dY;

	float fSum;
	float fAvg;
	int	  nCnt;

	float fTheta;
	int nDeltaOffset = 3;

	switch(nLineInx)
	{
	case LEFT:
		vector<cv::Point>().swap(ptProfile);
		//LEFT
		for(int nY=ptCorner[E_CORNER_LEFT_TOP].y + nCCutOffset[E_CORNER_LEFT_TOP]; nY < ptCorner[E_CORNER_LEFT_BOTTOM].y - nCCutOffset[E_CORNER_LEFT_BOTTOM]; nY++)
		{
			ucImgData = mtSrc.data+nY*mtSrc.step;
			for(int nX=ptCorner[E_CORNER_LEFT_TOP].x - nSearchRangeOffset; nX < ptCorner[E_CORNER_LEFT_TOP].x + nSearchRangeOffset; nX++)
			{
				nGV_Level = *(ucImgData+nX);
				int nDeltaX = nX+nDeltaOffset;
				if(nDeltaX > mtSrc.cols-1)
					nDeltaX = mtSrc.cols-1;

				nGV_Level2 = *(ucImgData+nDeltaX);

				fTheta = atan((float)(nGV_Level2 - nGV_Level)/nDeltaOffset)*180/PI;

				if(90 > fTheta && fTheta > nThreshold_Theta)
				{
					nCnt = 0;
					fSum = 0;
					for(int nX2 = nDeltaX-3; nX2 <= nDeltaX+2; nX2++)
					{
						nGV_Level = *(ucImgData+nX2);
						fSum += nGV_Level;
						nCnt++;
						ptProfile.push_back(cv::Point(nX2,nGV_Level));
					}
					calcLineFit(ptProfile, dA, dB);
					if(dA==0)
						break;
					fAvg = fSum/nCnt*fAvgOffset;
					dX = (fAvg - dB) / dA;

					pt.push_back(cv::Point2f(dX, nY));
					vector<cv::Point>().swap(ptProfile);
					break;
				}
			}
		}
		break;
	case TOP:
		vector<cv::Point>().swap(ptProfile);
		//TOP
		for(int nX=ptCorner[E_CORNER_LEFT_TOP].x + nCCutOffset[E_CORNER_LEFT_TOP]; nX < ptCorner[E_CORNER_RIGHT_TOP].x - nCCutOffset[E_CORNER_RIGHT_TOP]; nX++)
		{
			ucImgData = mtSrc.data+nX;
			for(int nY=ptCorner[E_CORNER_RIGHT_TOP].y - nSearchRangeOffset; nY < ptCorner[E_CORNER_RIGHT_TOP].y + nSearchRangeOffset; nY++)
			{
				nGV_Level = *(ucImgData+nY*mtSrc.step);
				int nDeltaY = nY + nDeltaOffset;
				if(nDeltaY > mtSrc.rows-1)
					nDeltaY = mtSrc.rows-1;
				nGV_Level2 = *(ucImgData+nDeltaY*mtSrc.step);

				fTheta = atan((float)(nGV_Level2 - nGV_Level)/nDeltaOffset)*180/PI;

				if(90 > fTheta && fTheta > nThreshold_Theta)
				{
					fSum = 0;
					nCnt = 0;
					for(int nY2 = nDeltaY-2; nY2 <= nDeltaY+3; nY2++)
					{
						nGV_Level = *(ucImgData+nY2*mtSrc.step);
						fSum += nGV_Level;
						nCnt++;
						ptProfile.push_back(cv::Point(nY2, nGV_Level));
					}
					if(calcLineFit(ptProfile, dA, dB)!=0)
						break;

					if(dA==0)
						break;
					fAvg = fSum/nCnt*fAvgOffset;
					dY = (fAvg - dB) / dA;
					pt.push_back(cv::Point2f(nX, dY));
					vector<cv::Point>().swap(ptProfile);
					break;
				}
			}
		}
		break;
	case RIGHT:
		vector<cv::Point>().swap(ptProfile);
		//Right
		for(int nY=ptCorner[E_CORNER_RIGHT_TOP].y + nCCutOffset[E_CORNER_RIGHT_TOP]; nY < ptCorner[E_CORNER_RIGHT_BOTTOM].y - nCCutOffset[E_CORNER_RIGHT_BOTTOM]; nY++)
		{
			ucImgData = mtSrc.data+nY*mtSrc.step;
			for(int nX=ptCorner[E_CORNER_RIGHT_TOP].x + nSearchRangeOffset; nX > ptCorner[E_CORNER_RIGHT_TOP].x - nSearchRangeOffset; nX--)
			{
				nGV_Level = *(ucImgData+nX);
				int nDeltaX = nX-nDeltaOffset;
				if(nDeltaX < 0)
					nDeltaX = 0;
				nGV_Level2 = *(ucImgData+nDeltaX);
				fTheta = atan((float)(nGV_Level2-nGV_Level)/nDeltaOffset)*180/PI;

				if(90 > fTheta && fTheta > nThreshold_Theta)
				{
					fSum=0;
					nCnt=0;
					for(int nX2 = nDeltaX-2; nX2 <= nDeltaX+3; nX2++)
					{
						nGV_Level = *(ucImgData+nX2);
						fSum += nGV_Level;
						nCnt++;
						ptProfile.push_back(cv::Point(nX2,nGV_Level));
					}
					if(calcLineFit(ptProfile, dA, dB)!=0)
						break;

					if(dA==0)
						break;
					fAvg = fSum/nCnt*fAvgOffset;
					dX = (fAvg - dB) / dA;
					pt.push_back(cv::Point2f(dX, nY));
					vector<cv::Point>().swap(ptProfile);
					break;

				}
			}
		}
		break;
	case BOTTOM:
		vector<cv::Point>().swap(ptProfile);
		//BOTTOM
		for(int nX=ptCorner[E_CORNER_LEFT_BOTTOM].x + nCCutOffset[E_CORNER_LEFT_BOTTOM]; nX < ptMark[E_CORNER_RIGHT_BOTTOM].x - nCCutOffset[E_CORNER_RIGHT_BOTTOM]; nX++)
		{
			ucImgData = mtSrc.data+nX;
			for(int nY=ptCorner[E_CORNER_RIGHT_BOTTOM].y + nSearchRangeOffset; nY > ptCorner[E_CORNER_RIGHT_BOTTOM].y - nSearchRangeOffset; nY--)
			{
				nGV_Level = *(ucImgData+nY*mtSrc.step);
				int nDeltaY = nY - nDeltaOffset;
				if(nDeltaY < 0)
					nDeltaY =0;
				nGV_Level2 = *(ucImgData+nDeltaY*mtSrc.step);
				fTheta = atan((float)(nGV_Level2-nGV_Level)/nDeltaOffset)*180/PI;
				if(90 > fTheta && fTheta > nThreshold_Theta)
				{
					fSum=0;
					nCnt=0;
					for(int nY2 = nDeltaY-3; nY2 <= nDeltaY+2; nY2++)
					{
						nGV_Level = *(ucImgData+nY2*mtSrc.step);
						fSum += nGV_Level;
						nCnt++;
						ptProfile.push_back(cv::Point(nY2, nGV_Level));
					}
					if(calcLineFit(ptProfile, dA, dB)!=0)
						break;

					if(dA==0)
						break;
					fAvg = fSum/nCnt*fAvgOffset;
					dY = (fAvg - dB) / dA;
					pt.push_back(cv::Point2f(nX, dY));
					
					vector<cv::Point>().swap(ptProfile);
					break;
				}
			}
		}
		break;
	}

	vector<cv::Point2f> ptBackup = pt;

	vector<cv::Point2f>().swap(pt);

	BOOL bPT = TRUE;

	for(int ptInx=0; ptInx < ptBackup.size(); ptInx++)
	{
		for(int nROIInx = 0; nROIInx < pFilterROI.size(); nROIInx++)
		{
			if(ptBackup[ptInx].x > pFilterROI[nROIInx].TopLeft().x + ptCorner[E_CORNER_LEFT_TOP].x &&
				ptBackup[ptInx].x < pFilterROI[nROIInx].BottomRight().x + ptCorner[E_CORNER_LEFT_TOP].x && 
				ptBackup[ptInx].y > pFilterROI[nROIInx].TopLeft().y + ptCorner[E_CORNER_LEFT_TOP].y&&
				ptBackup[ptInx].y < pFilterROI[nROIInx].BottomRight().y + ptCorner[E_CORNER_LEFT_TOP].y)
				bPT = FALSE;
		}
		if(bPT == TRUE)
		{
			pt.push_back(cv::Point2f(ptBackup[ptInx]));
			if(bImageSave)	cv::circle(mtDraw,Point(ptBackup[ptInx].x, ptBackup[ptInx].y), 1, Scalar(255,255,0));
		}
		
		bPT = TRUE;
	}

	return true;
}

bool CInspectEdge::GetEdgeLine(Mat mtSrc, vector<cv::Point2f>& pt, Point* ptCorner, int nLineInx, Point2f* ptMark, bool bImageSave,  vector<CRect> pFilterROI)
{
	int nCornerOffset = 200;
	int nMarkOffset = 150;
	uchar* pImagedata;
	int nGV_Level;

	switch(nLineInx)
	{
	case LEFT:
		//LEFT
		for(int nY = ptCorner[E_CORNER_LEFT_TOP].y + nCornerOffset; nY < ptCorner[E_CORNER_LEFT_BOTTOM].y-nCornerOffset; nY++)
		{
			pImagedata = mtSrc.data+nY*mtSrc.step;
			for(int nX = ptCorner[E_CORNER_LEFT_TOP].x + 100; nX > 0; nX--)
			{
				nGV_Level = *(pImagedata+nX);
				if(nGV_Level==0)
				{
					pt.push_back(cv::Point2f(nX,nY));
					break;
				}
			}
		}
		break;
	case TOP:
		//TOP LINE
		for(int nX = ptMark[E_CORNER_RIGHT_TOP].x; nX > ptCorner[E_CORNER_LEFT_TOP].x+nCornerOffset; nX--)
		{
			pImagedata = mtSrc.data+nX;
			for(int nY=ptMark[E_CORNER_RIGHT_TOP].y; nY > 0; nY--)
			{
				nGV_Level = *(pImagedata+nY*mtSrc.step);
				if(nGV_Level==0)
				{
					pt.push_back(cv::Point2f(nX,nY));
					break;
				}
			}
		}
		break;
	case RIGHT:
		//RIGHT
		for(int nY=ptMark[E_CORNER_RIGHT_TOP].y + nMarkOffset; nY < ptMark[E_CORNER_RIGHT_BOTTOM].y-nMarkOffset; nY++)
		{
			pImagedata = mtSrc.data+nY*mtSrc.step;
			for(int nX=ptMark[E_CORNER_RIGHT_TOP].x; nX < mtSrc.cols; nX++)
			{
				nGV_Level = *(pImagedata+nX);
				if(nGV_Level==0)
				{
					pt.push_back(cv::Point2f(nX,nY));
					break;
				}
			}
		}
		break;
	case BOTTOM:
		//BOTTOM
		for(int nX=ptMark[E_CORNER_RIGHT_BOTTOM].x; nX > ptCorner[E_CORNER_LEFT_BOTTOM].x + nCornerOffset; nX--)
		{
			pImagedata = mtSrc.data+nX;
			for(int nY=ptMark[E_CORNER_RIGHT_BOTTOM].y ; nY < mtSrc.rows; nY++)
			{
				nGV_Level = *(pImagedata+nY*mtSrc.step);
				if(nGV_Level==0)
				{
					pt.push_back(Point2f(nX,nY));
					break;
				}
			}
		}
		break;
	}
	

	return true;
}

bool CInspectEdge::JudgePointDirection(Point2f pt, double dA, double dB, int nLineInx, bool bXY_Reverse, float& fLength)
{
	double dY, dX;
	switch(nLineInx)
	{
	case LEFT:
		//if(bXY_Reverse)		dX = dA*pt.y+dB;
		/*else*/				dX = (pt.y - dB) / dA;

		if(dX < pt.x)
			fLength*=-1;
		break;
	case TOP:
		//if(bXY_Reverse)		dY = (pt.x - dB) / dA;
		/*else	*/			dY = dA*pt.x+dB;

		if(dY < pt.y)
			fLength*=-1;
		break;
	case RIGHT:
		if(bXY_Reverse)		dX = dA*pt.y+dB;
		else				dX = (pt.y - dB) / dA;
		if(dX > pt.x)
			fLength*=-1;
		break;
	case BOTTOM:
		/*if(bXY_Reverse)*/		dY = (pt.x - dB) / dA;
		/*else*/				dY = dA*pt.x+dB;

		if(dY > pt.y)
			fLength*=-1;
		break;
	}
	return true;
}

bool CInspectEdge::PadofActiveLineFitting(Mat src, double& dA, double& dB, Point* ptCorner, int nLineInx, float fLimitTheata, int* nCCutOffset, int nBMSearchRange)
{
	int nRangeOffset = nBMSearchRange;
	uchar* pImagedata;
	int nGV_Level;

	vector<Point> ptProfile;

	double dX,dY;
	vector<Point2f> ptPadLine;

	cv::Scalar mean, std;

	Rect rtMean;
	Mat mtMean;

	switch(nLineInx)
	{
	case LEFT:
		for(int nY= ptCorner[E_CORNER_LEFT_TOP].y + nCCutOffset[E_CORNER_LEFT_TOP]; nY < ptCorner[E_CORNER_LEFT_BOTTOM].y - nCCutOffset[E_CORNER_RIGHT_BOTTOM]; nY+=100)
		{
			pImagedata = src.data+nY*src.step;
			for(int nX = ptCorner[E_CORNER_LEFT_TOP].x; nX < ptCorner[E_CORNER_LEFT_TOP].x + nRangeOffset; nX++)
			{
				nGV_Level = *(pImagedata + nX);

				int nX_Add = nX + 3;
				if(nX_Add > ptCorner[E_CORNER_LEFT_TOP].x + nRangeOffset-1)
					nX_Add = ptCorner[E_CORNER_LEFT_TOP].x + nRangeOffset-1;
				int nGV_Level_Add = *(pImagedata + nX_Add);

				float fTheta = atan((float)(nGV_Level_Add - nGV_Level)/(nX_Add-nX)) * 180 / CV_PI;

				if(fTheta < 90 && fTheta > fLimitTheata)
				{

					dX = nX_Add;
					dY = (double)nY;

					ptPadLine.push_back(Point2f(dX,dY));
					break;
				}
			}
		}
		break;
	case TOP:
		for(int nX = ptCorner[E_CORNER_LEFT_TOP].x + nCCutOffset[E_CORNER_LEFT_TOP]; nX < ptCorner[E_CORNER_RIGHT_TOP].x - nCCutOffset[E_CORNER_RIGHT_TOP]; nX+=100)
		{
			pImagedata = src.data+nX;
			for(int nY= ptCorner[E_CORNER_LEFT_TOP].y; nY < ptCorner[E_CORNER_LEFT_TOP].y + nRangeOffset; nY++)
			{
				nGV_Level = *(pImagedata + nY*src.step);

				int nY_Add = nY + 3;
				if(nY_Add > ptCorner[E_CORNER_RIGHT_TOP].y + nRangeOffset - 1)
					nY_Add = ptCorner[E_CORNER_RIGHT_TOP].y + nRangeOffset - 1;

				int nGV_Level_Add = *(pImagedata + nY_Add*src.step);

				float fTheta = atan((float)(nGV_Level_Add - nGV_Level)/(nY_Add-nY)) * 180 / CV_PI;


				if(fTheta < 90 && fTheta > fLimitTheata)
				{

					dX = (double)nX;
					dY = nY_Add;

					ptPadLine.push_back(Point2f(dX,dY));
					break;
				}
			}
		}
		break;
	case BOTTOM:
		for(int nX = ptCorner[E_CORNER_LEFT_BOTTOM].x + nCCutOffset[E_CORNER_LEFT_BOTTOM]; nX < ptCorner[E_CORNER_RIGHT_BOTTOM].x - nCCutOffset[E_CORNER_RIGHT_BOTTOM]; nX+=100)
		{
			pImagedata = src.data+nX;
			for(int nY= ptCorner[E_CORNER_LEFT_BOTTOM].y; nY > ptCorner[E_CORNER_LEFT_BOTTOM].y-nRangeOffset; nY--)

			{
				nGV_Level = *(pImagedata + nY*src.step);

				int nY_Add = nY - 3;
				if(nY_Add < ptCorner[E_CORNER_RIGHT_TOP].y - nRangeOffset)
					nY_Add = ptCorner[E_CORNER_RIGHT_TOP].y - nRangeOffset;

				int nGV_Level_Add = *(pImagedata + nY_Add*src.step);

				float fTheta = atan((float)(nGV_Level_Add - nGV_Level)/(nY-nY_Add)) * 180 / CV_PI;


				if(fTheta < 90 && fTheta > fLimitTheata)
				{
					dX = (double)nX;
					dY = nY_Add;

					ptPadLine.push_back(Point2f(dX,dY));
					break;
				}
			}
		}
		break;

	default:
		return false;
		break;
	}
	

	long double dPadLineA, dPadLineB;
	if(ptPadLine.size()<3)
		return false;
	calcRANSAC(ptPadLine, dPadLineA, dPadLineB, 3, 5);
	//calcLineFit(ptPadLine, dPadLineA, dPadLineB);

	dA = (double)dPadLineA;
	dB = (double)dPadLineB;

	return true;
}


//2017.11.13 Curve Fitting by LJM
//2차 방정식 Curve Fitting
//Y = AX^2+BX+C
//R[3][1]= {A},{B},{C};
long CInspectEdge::calcCurveFit(std::vector <cv::Point2f>& ptSamples, long double& dA, long double& dB, long double& dC)
{
	if(ptSamples.size() < 3)
		return -1;


	Mat X(ptSamples.size(),3,CV_32FC1);//2차 방정식 X
	Mat Y(ptSamples.size(),1, CV_32FC1);//2차 방정식 Y
	Mat R(3,1,CV_32FC1);

	

	for(int nRow = 0; nRow < ptSamples.size(); nRow++)
	{
		X.at<float>(nRow, 0) = pow(ptSamples[nRow].x,2);
		X.at<float>(nRow, 1) = ptSamples[nRow].x;
		X.at<float>(nRow, 2) = 1;

		Y.at<float>(nRow, 0) = ptSamples[nRow].y;
	}

	Mat InvX(3,ptSamples.size(),CV_32FC1);
	InvX = X.inv(cv::DECOMP_SVD);

	R = InvX*Y;

	dA = (long double)R.at<float>(0,0);
	dB = (long double)R.at<float>(1,0);
	dC = (long double)R.at<float>(2,0);

	return 0;
}

long CInspectEdge::GetCCutLineToPanelEdgeCrossPoint(int nCornerInx, Point* ptCorner, vector<Point> ptCCutLine, Point& ptCrossST, Point& ptCrossED)
{
	double dHorizontal_A;
	double dHorizontal_B;
	double dVertical_A;
	double dVertical_B;

	bool bReverse = false;	//Vertical Line 중 X, Y를 반전시켜서 사용한 경우 true

	switch(nCornerInx)
	{
	case E_CORNER_LEFT_TOP:
		dHorizontal_A = (ptCorner[E_CORNER_RIGHT_TOP].y - ptCorner[E_CORNER_LEFT_TOP].y)/(ptCorner[E_CORNER_RIGHT_TOP].x - ptCorner[E_CORNER_LEFT_TOP].x);
		dHorizontal_B = ptCorner[E_CORNER_LEFT_TOP].y - ptCorner[E_CORNER_LEFT_TOP].x * dHorizontal_A;

		if(ptCorner[E_CORNER_LEFT_BOTTOM].x - ptCorner[E_CORNER_LEFT_TOP].x == 0)
		{
			bReverse = true;
			dVertical_A = (ptCorner[E_CORNER_LEFT_BOTTOM].x - ptCorner[E_CORNER_LEFT_TOP].x)/(ptCorner[E_CORNER_LEFT_BOTTOM].y - ptCorner[E_CORNER_LEFT_TOP].y);
			dVertical_B = ptCorner[E_CORNER_LEFT_TOP].x - ptCorner[E_CORNER_LEFT_TOP].y * dVertical_A;
		}
		else
		{
			dVertical_A = (ptCorner[E_CORNER_LEFT_BOTTOM].y - ptCorner[E_CORNER_LEFT_TOP].y)/(ptCorner[E_CORNER_LEFT_BOTTOM].x - ptCorner[E_CORNER_LEFT_TOP].x);
			dVertical_B = ptCorner[E_CORNER_LEFT_TOP].y - ptCorner[E_CORNER_LEFT_TOP].x * dVertical_A;
		}
		break;
	case E_CORNER_RIGHT_TOP:
		dHorizontal_A = (ptCorner[E_CORNER_RIGHT_TOP].y - ptCorner[E_CORNER_LEFT_TOP].y)/(ptCorner[E_CORNER_RIGHT_TOP].x - ptCorner[E_CORNER_LEFT_TOP].x);
		dHorizontal_B = ptCorner[E_CORNER_LEFT_TOP].y - ptCorner[E_CORNER_LEFT_TOP].x * dHorizontal_A;

		if(ptCorner[E_CORNER_RIGHT_BOTTOM].x - ptCorner[E_CORNER_RIGHT_TOP].x == 0)
		{
			bReverse = true;
			dVertical_A = (ptCorner[E_CORNER_RIGHT_BOTTOM].x - ptCorner[E_CORNER_RIGHT_TOP].x)/(ptCorner[E_CORNER_RIGHT_BOTTOM].y - ptCorner[E_CORNER_RIGHT_TOP].y);
			dVertical_B = ptCorner[E_CORNER_RIGHT_TOP].x - ptCorner[E_CORNER_RIGHT_TOP].y * dVertical_A;
		}
		else
		{
			dVertical_A = (ptCorner[E_CORNER_RIGHT_BOTTOM].y - ptCorner[E_CORNER_RIGHT_TOP].y)/(ptCorner[E_CORNER_RIGHT_BOTTOM].x - ptCorner[E_CORNER_RIGHT_TOP].x);
			dVertical_B = ptCorner[E_CORNER_RIGHT_TOP].y - ptCorner[E_CORNER_RIGHT_TOP].x * dVertical_A;
		}
		break;
	case E_CORNER_RIGHT_BOTTOM:
		dHorizontal_A = (ptCorner[E_CORNER_RIGHT_BOTTOM].y - ptCorner[E_CORNER_LEFT_BOTTOM].y)/(ptCorner[E_CORNER_RIGHT_BOTTOM].x - ptCorner[E_CORNER_LEFT_BOTTOM].x);
		dHorizontal_B = ptCorner[E_CORNER_LEFT_BOTTOM].y - ptCorner[E_CORNER_LEFT_BOTTOM].x * dHorizontal_A;

		if(ptCorner[E_CORNER_RIGHT_BOTTOM].x - ptCorner[E_CORNER_RIGHT_TOP].x == 0)
		{
			bReverse = true;
			dVertical_A = (ptCorner[E_CORNER_RIGHT_BOTTOM].x - ptCorner[E_CORNER_RIGHT_TOP].x)/(ptCorner[E_CORNER_RIGHT_BOTTOM].y - ptCorner[E_CORNER_RIGHT_TOP].y);
			dVertical_B = ptCorner[E_CORNER_RIGHT_TOP].x - ptCorner[E_CORNER_RIGHT_TOP].y * dVertical_A;
		}
		else
		{
			dVertical_A = (ptCorner[E_CORNER_RIGHT_BOTTOM].y - ptCorner[E_CORNER_RIGHT_TOP].y)/(ptCorner[E_CORNER_RIGHT_BOTTOM].x - ptCorner[E_CORNER_RIGHT_TOP].x);
			dVertical_B = ptCorner[E_CORNER_RIGHT_TOP].y - ptCorner[E_CORNER_RIGHT_TOP].x * dVertical_A;
		}
		break;
	case E_CORNER_LEFT_BOTTOM:
		dHorizontal_A = (ptCorner[E_CORNER_RIGHT_BOTTOM].y - ptCorner[E_CORNER_LEFT_BOTTOM].y)/(ptCorner[E_CORNER_RIGHT_BOTTOM].x - ptCorner[E_CORNER_LEFT_BOTTOM].x);
		dHorizontal_B = ptCorner[E_CORNER_LEFT_BOTTOM].y - ptCorner[E_CORNER_LEFT_BOTTOM].x * dHorizontal_A;

		if(ptCorner[E_CORNER_LEFT_BOTTOM].x - ptCorner[E_CORNER_LEFT_TOP].x == 0)
		{
			bReverse = true;
			dVertical_A = (ptCorner[E_CORNER_LEFT_BOTTOM].x - ptCorner[E_CORNER_LEFT_TOP].x)/(ptCorner[E_CORNER_LEFT_BOTTOM].y - ptCorner[E_CORNER_LEFT_TOP].y);
			dVertical_B = ptCorner[E_CORNER_LEFT_TOP].x - ptCorner[E_CORNER_LEFT_TOP].y * dVertical_A;
		}
		else
		{
			dVertical_A = (ptCorner[E_CORNER_LEFT_BOTTOM].y - ptCorner[E_CORNER_LEFT_TOP].y)/(ptCorner[E_CORNER_LEFT_BOTTOM].x - ptCorner[E_CORNER_LEFT_TOP].x);
			dVertical_B = ptCorner[E_CORNER_LEFT_TOP].y - ptCorner[E_CORNER_LEFT_TOP].x * dVertical_A;
		}
		break;
	}

	long double dCCutLineA, dCCutLineB;
	calcLineFit(ptCCutLine, dCCutLineA, dCCutLineB);
	
	float fVertical_X, fVertical_Y, fHorizontal_X, fHorizontal_Y;
	
	if(bReverse)
	{
		//aX+bY+c=0
		GetCrossPointByTwoStraightLines(1, -dVertical_A, -dVertical_B, dCCutLineA, -1, dCCutLineB, fVertical_X, fVertical_Y);	
	}
	else
	{
		GetCrossPointByTwoStraightLines(dVertical_A, -1, dVertical_B, dCCutLineA, -1, dCCutLineB, fVertical_X, fVertical_Y);
	}
	GetCrossPointByTwoStraightLines(dHorizontal_A, -1, dHorizontal_B, dCCutLineA, -1, dCCutLineB, fHorizontal_X, fHorizontal_Y);

	switch(nCornerInx)
	{
	case E_CORNER_LEFT_TOP:
		ptCrossST = Point((int)fHorizontal_X, (int)fHorizontal_Y);
		ptCrossED = Point((int)fVertical_X, (int)fVertical_Y);
		break;
	case E_CORNER_RIGHT_TOP:
		ptCrossST = Point((int)fHorizontal_X, (int)fHorizontal_Y);
		ptCrossED = Point((int)fVertical_X, (int)fVertical_Y);
		break;
	case E_CORNER_RIGHT_BOTTOM:
		ptCrossST = Point((int)fVertical_X, (int)fVertical_Y);
		ptCrossED = Point((int)fHorizontal_X, (int)fHorizontal_Y);
		break;
	case E_CORNER_LEFT_BOTTOM:
		ptCrossST = Point((int)fVertical_X, (int)fVertical_Y);
		ptCrossED = Point((int)fHorizontal_X, (int)fHorizontal_Y);
		break;
	}

	return E_ERROR_CODE_TRUE;
}

bool CInspectEdge::SortGraylevel(Mat src, int nIndex)
{
	uchar* pImgdata;

	int nSum;
	int	nCnt;

	float* fAvg;
	int nNum;

	switch(nIndex)
	{
	case 0:		//colum 평균, 정렬
		fAvg = new float[src.rows];
		nNum = src.rows;
		for(int nY=0; nY < src.rows; nY++)
		{
			nSum =0;
			nCnt =0;
			pImgdata = src.data+nY*src.step;
			for(int nX=0; nX < src.cols; nX++)
			{
				nSum += (int)*(pImgdata+nX);
				nCnt++;
			}
			fAvg[nY] = nSum/nCnt;
		}
		break;
	case 1:		//row 평균, 정렬
		fAvg = new float[src.cols];
		nNum = src.cols;
		for(int nX=0; nX < src.cols; nX++)
		{
			nSum =0;
			nCnt =0;
			pImgdata = src.data+nX;
			for(int nY=0; nY < src.rows; nY++)
			{
				nSum += (int)*(pImgdata+nY*src.step);
				nCnt++;
			}
			fAvg[nX] = nSum/nCnt;
		}
		break;
	default:
		return false;
		break;
	}


	float fValue;
	
	for(int n =0; n < nNum; n++)
	{
		for(int j =n+1; j < nNum; j++)
		{
			if(j > nNum-1)
				continue;

			if(fAvg[n] > fAvg[j])
			{
				fValue = fAvg[n];
				fAvg[n] = fAvg[j];
				fAvg[j] = fValue;
			}
		}
	}

	delete[] fAvg;
	
	return true;
}
long CInspectEdge::GetBMLinePointY(cv::Mat mtImg, int nThreshold, int nCornerInx, int &nPointY, int nIgnore_GV)
{
	Mat mtDebugDraw;
	cv::cvtColor(mtImg, mtDebugDraw, CV_GRAY2RGB);

	cv::blur(mtImg,mtImg,Size(3,3));
	uchar* pImgData;
	int nGrayValue;
	int nDeltaOffset = 3;
	int nDeltaGrayValue;
	int	nDeltaY;
	float fTheta;

	vector<Point2f> ptBMLine;

	switch(nCornerInx)
	{
	case E_CORNER_LEFT_TOP:
		{			
			for(int nX = mtImg.cols; nX > mtImg.cols-5; nX--)
			{
				pImgData = mtImg.data + nX;
				for(int nY = 0; nY < mtImg.rows; nY++)
				{
					nGrayValue = (int)*(pImgData + nY * mtImg.step);
					if(nGrayValue<=nIgnore_GV)
						continue;
					nDeltaY = nY + nDeltaOffset;
					if(nDeltaY >= mtImg.rows)
						nDeltaY = mtImg.rows-1;
					nDeltaGrayValue = (int)*(pImgData + nDeltaY * mtImg.step);
					fTheta = atan((float)(nDeltaGrayValue-nGrayValue)/(nDeltaY-nY))*180/CV_PI;

					if( 90>fTheta && fTheta > nThreshold)
					{
						ptBMLine.push_back(Point2f(nX, nDeltaY));
						cv::circle(mtDebugDraw, Point2i(nX, nDeltaY),1, Scalar(255,0,0));
						break;
					}
				}
			}
		}
		break;

	case E_CORNER_RIGHT_TOP:
		{
			for(int nX = 0; nX < 5; nX++)
			{
				pImgData = mtImg.data + nX;
				for(int nY = 0; nY < mtImg.rows; nY++)
				{
					nGrayValue = (int)*(pImgData + nY * mtImg.step);
					if(nGrayValue<=nIgnore_GV)
						continue;
					nDeltaY = nY + nDeltaOffset;
					if(nDeltaY >= mtImg.rows)
						nDeltaY = mtImg.rows-1;
					nDeltaGrayValue = (int)*(pImgData + nDeltaY * mtImg.step);
					fTheta = atan((float)(nDeltaGrayValue-nGrayValue)/(nDeltaY-nY))*180/CV_PI;

					if( 90 > fTheta && fTheta > nThreshold)
					{
						ptBMLine.push_back(Point2f(nX, nDeltaY));
						cv::circle(mtDebugDraw, Point2i(nX, nDeltaY),1, Scalar(255,0,0));
						break;
					}
				}
			}
		}
		break;

	case E_CORNER_RIGHT_BOTTOM:
		{
			for(int nX = 0; nX < 5; nX++)
			{
				pImgData = mtImg.data + nX;
				for(int nY = mtImg.rows; nY > 0; nY--)
				{
					nGrayValue = (int)*(pImgData + nY * mtImg.step);
					if(nGrayValue<=nIgnore_GV)
						continue;
					nDeltaY = nY - nDeltaOffset;
					if(nDeltaY < 0)
						nDeltaY = 0;
					nDeltaGrayValue = (int)*(pImgData + nDeltaY * mtImg.step);
					fTheta = atan((float)(nGrayValue-nDeltaGrayValue)/(nDeltaY-nY))*180/CV_PI;

					if( 90 > fTheta && fTheta > nThreshold)
					{
						ptBMLine.push_back(Point2f(nX, nDeltaY));
						cv::circle(mtDebugDraw, Point2i(nX, nDeltaY),1, Scalar(255,0,0));
						break;
					}
				}
			}
		}
		break;

	case E_CORNER_LEFT_BOTTOM:
		{
			for(int nX = mtImg.cols; nX > mtImg.cols-5; nX--)
			{
				pImgData = mtImg.data + nX;
				for(int nY = mtImg.rows; nY > 0; nY--)
				{
					nGrayValue = (int)*(pImgData + nY * mtImg.step);
					if(nGrayValue<=nIgnore_GV)
						continue;
					nDeltaY = nY - nDeltaOffset;
					if(nDeltaY < 0)
						nDeltaY = 0;
					nDeltaGrayValue = (int)*(pImgData + nDeltaY * mtImg.step);
					fTheta = atan((float)(nGrayValue-nDeltaGrayValue)/(nDeltaY-nY))*180/CV_PI;

					if( 90 > fTheta && fTheta > nThreshold)
					{
						ptBMLine.push_back(Point2f(nX, nDeltaY));
						cv::circle(mtDebugDraw, Point2i(nX, nDeltaY),1, Scalar(255,0,0));
						break;
					}
				}
			}
		}
		break;
	default:
		break;
	}

	CString strDebugImg;
	strDebugImg.Format(_T("C:\\IDB\\BMLinePointY_%d.bmp"), nCornerInx);
	imwrite((cv::String)(CStringA)strDebugImg, mtDebugDraw);


	if(ptBMLine.size() < 3)
		return E_ERROR_CODE_FALSE;

	for(int n=0; n < ptBMLine.size(); n++)
		nPointY += ptBMLine[n].y;

	nPointY = nPointY/ptBMLine.size();

	return E_ERROR_CODE_TRUE;
}