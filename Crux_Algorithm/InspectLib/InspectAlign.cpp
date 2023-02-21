
/************************************************************************/
// Align 통합 관련 소스
// 수정일 : 18.02.07 - KSW
/************************************************************************/

#include "StdAfx.h"
#include "InspectAlign.h"
#include "ExportLibrary.h"
#include <stdlib.h>

CInspectAlign::CInspectAlign(void)
{
	cMem				= NULL;
	m_cInspectLibLog	= NULL;
	m_strAlgLog			= NULL;
	m_tInitTime			= 0;
	m_tBeforeTime		= 0;
}


CInspectAlign::~CInspectAlign(void)
{
}


// Top Line 각도 찾기
long CInspectAlign::DoFindTheta(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point& ptCenter, wchar_t* strID)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	cv::Mat matGrayBuf, matTempBuf;

	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼 할당 & 초기화
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	// 컬러인 경우 ( SVI )
	if( matSrcBuf.channels() != 1 )
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// 흑백인 경우 ( AVI, APP )
	else
	{
		if( matSrcBuf.type() == CV_8UC1 )
			matSrcBuf.copyTo(matGrayBuf);
		else
			matSrcBuf.convertTo(matGrayBuf, CV_8UC1, 1./16.);

		//matSrcBuf.copyTo(matGrayBuf);
		//matGrayBuf = matSrcBuf.clone();
	}

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples		= 3;	// 고정
	double	distThreshold	= 10;	// 고정

	int		nThreshold		= (int)dPara[E_PARA_ALIGN_THRESHOLD];
	int		nMorp			= (int)dPara[E_PARA_ALIGN_MORP];
	double	dAngleError		= dPara[E_PARA_ALIGN_ANGLE_ERR];
	double	dAngleWarning	= dPara[E_PARA_ALIGN_ANGLE_WAR];

	// 외관 : Cell 면적 ( Pixel 갯수 )
	// 점등 : 점등 면적 ( Pixel 갯수 )
	int		nMinArea		= (int)(dPara[E_PARA_CELL_SIZE_X] * dPara[E_PARA_CELL_SIZE_Y]);

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	long	nWidth	= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuf.rows;	// 영상 세로 크기

	if( nMorp > 0 )
	{
		cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, Size(nMorp, nMorp), cv::Point(nMorp/2, nMorp/2));

		// Morphology Close ( Cell 사이 빈공간 채우기 )
		cv::morphologyEx(matGrayBuf, matTempBuf, MORPH_CLOSE, StructElem);

		StructElem.release();
	}
	else
		matGrayBuf.copyTo(matTempBuf);
		//matTempBuf = matGrayBuf.clone();

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Morphology."));

	// Threshold
	cv::threshold(matTempBuf, matTempBuf, nThreshold, 255.0, THRESH_BINARY);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("threshold."));

	// 검사 영역 Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matTempBuf, nMinArea, rectCell);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("FindCellArea."));

	// Cell 중심점
	ptCenter.x = rectCell.x + rectCell.width/2;
	ptCenter.y = rectCell.y + rectCell.height/2;

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		matGrayBuf.release();
		matTempBuf.release();
		return nErrorCode;
	}

	long double	dValueA = 0, dValueB = 0;

	// Top 직선 찾기
	nErrorCode = RobustFitLine(matTempBuf, rectCell, dValueA, dValueB, nMinSamples, distThreshold, E_ALIGN_TYPE_TOP);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("FitLine."));

	// 메모리 해제
	matGrayBuf.release();
	matTempBuf.release();

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	// Top Line(장변)으로 각도 구하기
	dTheta = atan( dValueA ) * 180. / PI;

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// AVI 검사 영역 찾기
long CInspectAlign::DoFindActive(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point* ptResCorner, int nRatio, cv::Point& ptCenter, wchar_t* strID)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat matGrayBuf, matTempBuf;

	// 버퍼 할당 & 초기화
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	// 컬러인 경우 ( SVI )
	if( matSrcBuf.channels() != 1 )
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// 흑백인 경우 ( AVI, APP )
	else
	{
		if( matSrcBuf.type() == CV_8UC1 )
			matSrcBuf.copyTo(matGrayBuf);
		else
			matSrcBuf.convertTo(matGrayBuf, CV_8UC1, 1./16.);

		//matSrc_8bit.copyTo(matGrayBuf);
		//matGrayBuf = matSrc_8bit.clone();
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("cvtColor."));

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples		= 3;	// 고정
	double	distThreshold	= 10;	// 고정

	int		nThreshold		= dPara[E_PARA_ALIGN_THRESHOLD];
	int		nMorp			= dPara[E_PARA_ALIGN_MORP];
	double	dAngleError		= dPara[E_PARA_ALIGN_ANGLE_ERR];
	double	dAngleWarning	= dPara[E_PARA_ALIGN_ANGLE_WAR];

	// 외관 : Cell 면적 ( Pixel 갯수 )
	// 점등 : 점등 면적 ( Pixel 갯수 )
	int		nMinArea		= (int)(dPara[E_PARA_CELL_SIZE_X] * dPara[E_PARA_CELL_SIZE_Y] * nRatio * nRatio);	// 3800 * 1900;	// APP

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	long	nWidth	= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuf.rows;	// 영상 세로 크기

	//////////////////////////////////////////////////////////////////////////
	// AVI의 경우만 사용, Cell 배열 사이 빈공간 채우는 용도
	//////////////////////////////////////////////////////////////////////////

	if( nMorp > 0 )
	{
		cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, Size(nMorp, nMorp), cv::Point(nMorp/2, nMorp/2));

		// Morphology Close ( Cell 사이 빈공간 채우기 )
		cv::morphologyEx(matGrayBuf, matTempBuf, MORPH_CLOSE, StructElem);

		StructElem.release();
	}
	else
		matGrayBuf.copyTo(matTempBuf);
		//matTempBuf = matGrayBuf.clone();

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Morphology."));

	//cv::imwrite("D:\\1.Src.bmp", matGrayBuf);
	//cv::imwrite("D:\\2.Morp.bmp", matTempBuf);

	// Threshold
	cv::threshold(matTempBuf, matGrayBuf, nThreshold, 255.0, THRESH_BINARY);

	//cv::imwrite("D:\\3.Th.bmp", matGrayBuf);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("threshold."));

	//////////////////////////////////////////////////////////////////////////
	// Cell 영역 찾기
	//////////////////////////////////////////////////////////////////////////

	// 검사 영역 Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matGrayBuf, nMinArea, rectCell);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("FindCellArea."));

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		matGrayBuf.release();
		matTempBuf.release();
		return nErrorCode;
	}

	// Cell 중심점
	ptCenter.x = rectCell.x + rectCell.width/2;
	ptCenter.y = rectCell.y + rectCell.height/2;

	//////////////////////////////////////////////////////////////////////////
	// 점등 영역 찾기
	//////////////////////////////////////////////////////////////////////////

	// Contours 구하기
	vector<vector<cv::Point>>	contours;
	vector<vector<cv::Point>>().swap(contours);

	int nContourSize = 0;
	int nContourIdx = 0;

	cv::findContours(matGrayBuf, contours, CV_RETR_LIST, CHAIN_APPROX_NONE);

	for (int i = 0; i < contours.size(); i++)
	{
		if (nContourSize < contours[i].size())
		{
			nContourSize = contours[i].size();
			nContourIdx = i;
		}
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Contours."));

	cv::RotatedRect BoundingBox = cv::minAreaRect(contours[nContourIdx]);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("RotatedRect."));

	cv::Point2f vertices[E_CORNER_END];
	BoundingBox.points(vertices);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("BoundingBox."));

	// 꼭지점 결과 확인
	//cv::Mat matTempBufff = matSrcBuf.clone();
	//cv::line(matTempBufff, vertices[0], vertices[1], CvScalar(255, 255, 255));
	//cv::line(matTempBufff, vertices[1], vertices[2], CvScalar(255, 255, 255));
	//cv::line(matTempBufff, vertices[2], vertices[3], CvScalar(255, 255, 255));
	//cv::line(matTempBufff, vertices[3], vertices[0], CvScalar(255, 255, 255));

	vector<vector<cv::Point>>().swap(contours);

	// 메모리 해제
	matGrayBuf.release();
	matTempBuf.release();

	//////////////////////////////////////////////////////////////////////////
	// 코너 꼭지점 4개 찾기
	//////////////////////////////////////////////////////////////////////////

	nErrorCode = FindCornerPoint(vertices, ptResCorner, nWidth, nHeight);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Find 4-Corner."));

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;


	// Top Line(장변)으로 각도 구하기
	dTheta = BoundingBox.angle;

	// 예외처리
	if( 45.0 < dTheta && dTheta < 135.0 )	dTheta -= 90.0;
	if( -45.0 > dTheta && dTheta > -135.0 )	dTheta += 90.0;

	// 각도가 많이 틀어진 경우 에러. ( 중알람 )
	if( abs(dTheta) > dAngleError )
		return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;

	// 각도가 많이 틀어진 경우 에러. ( 경알람 )
	else if( abs(dTheta) > dAngleWarning )
		return E_ERROR_CODE_ALIGN_ANGLE_RANGE_WARNING;
	
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// APP 검사 영역 찾기
long CInspectAlign::DoFindActive_APP(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point* ptResCorner, int nRatio, double dCamResolution, double dPannelSizeX, double dPannelSizeY, cv::Point& ptCenter, wchar_t* strID)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	cv::Mat matGrayBuf, matTempBuf;

	// 버퍼가 없는 경우.
	if (matSrcBuf.empty())
	{
		return E_ERROR_CODE_EMPTY_BUFFER;
	}

	// 버퍼 할당 & 초기화
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1);

	matSrcBuf.copyTo(matGrayBuf);
	matGrayBuf.copyTo(matTempBuf);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("cvtColor."));

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////
	int		nThreshold		= (int)dPara[E_PARA_APP_ALIGN_THRESHOLD];
	int		nDilate			= (int)dPara[E_PARA_APP_ALIGN_DILATE];
	int		nPanelEdgeTheta = (int)dPara[E_PARA_APP_ALIGN_PANEL_EDGE_THETA];
	int		nMinSamples		= 3;			// 공통
	double	distThreshold	= 5;			// 공통
	double	dAngleError		= (double)dPara[E_PARA_APP_ALIGN_ANGLE_ERR];
	double	dAngleWarning	= (double)dPara[E_PARA_APP_ALIGN_ANGLE_WAR];
	double	fAvgOffset		= (double)dPara[E_PARA_APP_ALIGN_AVG_OFFSET];
	bool	bUseOverRange	= (bool)dPara[E_PARA_APP_ALIGN_USE_OVERRANGE];
	int		dLengthError	= (int)dPara[E_PARA_APP_ALIGN_RANGE_OVER];

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;
	long nWidth = (long)matSrcBuf.cols;	// 영상 가로 크기
	long nHeight = (long)matSrcBuf.rows;	// 영상 세로 크기

	cv::Rect rtCell = cv::Rect(0, 0, matGrayBuf.cols, matGrayBuf.rows);
	long double ldValueB[4] = { 0.0, };
	long double	ldValueA[4] = { 0.0, };

	try
	{
		//이진화 한 후 가장 큰 Blob만 남김 (Stage 배경 Noise 제거)
		//Morphlogy로 Cell 근접 Noise 제거, Cell 빵꾸 매우기, And에 사용할 Mask Size 크기 키우기

		bool bState = false;

		Mat matThreshold;
		cv::threshold(matTempBuf, matThreshold, nThreshold, 255, THRESH_BINARY);

		Mat matBiggest;
		if (FindBiggestBlob_APP(matThreshold, matBiggest) == E_ERROR_CODE_FALSE)
		{
			return E_ERROR_CODE_FALSE;
		}
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("FindBiggestBlob_APP End."));

		GetCheckMaskSize(nDilate);
		if (nDilate - 4 >= 3)
		{
			cv::morphologyEx(matBiggest, matBiggest, MORPH_ERODE, cv::getStructuringElement(MORPH_RECT, cv::Size(nDilate - 4, nDilate - 4)));
		}
		cv::morphologyEx(matBiggest, matBiggest, MORPH_DILATE, cv::getStructuringElement(MORPH_RECT, cv::Size(nDilate, nDilate)));

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Morphlogy End."));

		cv::bitwise_and(matTempBuf, matBiggest, matTempBuf);
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("bitwise_and End."));

		cv::blur(matTempBuf, matTempBuf, cv::Size(3, 3));
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("blur End."));

		//////////////////////////////////////////////////////////////////////////
		// Line 찾기 & RANSAC
		//////////////////////////////////////////////////////////////////////////	
#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(4)
#endif
		for (int nType = E_ALIGN_TYPE_LEFT; nType <= E_ALIGN_TYPE_BOTTOM; nType++)
		{
			nErrorCode |= ObjectOutAreaGetLine(matTempBuf, rtCell, ldValueA[nType], ldValueB[nType], nMinSamples, distThreshold, nType, nPanelEdgeTheta, fAvgOffset);
		}
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("FitLine."));

		// 에러인 경우, 에러 코드 출력
		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			return nErrorCode;
		}

		//////////////////////////////////////////////////////////////////////////
		// 코너 꼭지점 4개 찾기
		//////////////////////////////////////////////////////////////////////////
		double dA1, dB1, dA2, dB2 = 0.0;

		for (int i = E_CORNER_LEFT_TOP; i <= E_CORNER_LEFT_BOTTOM; i++)
		{
			dA1 = ldValueA[i];
			dB1 = ldValueB[i];
			dA2 = ldValueA[(i + 1) < 4 ? i + 1 : 0];
			dB2 = ldValueB[(i + 1) < 4 ? i + 1 : 0];

			ptResCorner[i].x = (int)(((dB2 - dB1) / (dA1 - dA2)) + 0.5f);
			ptResCorner[i].y = (int)(((dA2*dB1 - dA1*dB2) / (dA2 - dA1)) + 0.5f);

			if (ptResCorner[i].x < 0)			return E_ERROR_CODE_ALIGN_IMAGE_OVER;
			if (ptResCorner[i].y < 0)			return E_ERROR_CODE_ALIGN_IMAGE_OVER;
			if (ptResCorner[i].x >= nWidth)		return E_ERROR_CODE_ALIGN_IMAGE_OVER;
			if (ptResCorner[i].y >= nHeight)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
		}
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Find 4-Corner."));

		// Top Line(장변)으로 각도 구하기
		dTheta = atan(ldValueA[E_ALIGN_TYPE_TOP]) * 180. / PI;
		if (abs(dTheta) > dAngleError)							// 각도가 많이 틀어진 경우 에러.
		{
			return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
		}
		else if (abs(dTheta) > dAngleWarning)					// 각도가 많이 틀어진 경우 에러. ( 경알람 )
		{
			return E_ERROR_CODE_ALIGN_ANGLE_RANGE_WARNING;
		}

		if (DoRotateImage(matTempBuf, matTempBuf, dTheta) == E_ERROR_CODE_EMPTY_BUFFER)
		{
			return E_ERROR_CODE_EMPTY_BUFFER;
		}
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("RotateImage End."));

		cv::Point ptRotateCorner[E_CORNER_END];
		for (int nCornerInx = E_CORNER_LEFT_TOP; nCornerInx <= E_CORNER_LEFT_BOTTOM; nCornerInx++)
		{
			DoRotatePoint(ptResCorner[nCornerInx], ptRotateCorner[nCornerInx], cv::Point(matTempBuf.cols / 2, matTempBuf.rows / 2), dTheta);
		}
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("RotatePoint End."));

		int nNormalLength = (int)(dPannelSizeX * 1000 / dCamResolution);
		int nHorizenLength = ptRotateCorner[E_CORNER_RIGHT_TOP].x - ptRotateCorner[E_CORNER_LEFT_TOP].x;

		if (bUseOverRange == true)
		{
			int OverRange = abs(nNormalLength - nHorizenLength);
			if (OverRange > dLengthError)
			{
				return E_ERROR_CODE_ALIGN_LENGTH_RANGE_ERROR;
			}
		}

		//int nADThresh = (int)dPara[E_PARA_APP_AD_THRESHOLD];
		//int dCompare_Theta = (double)dPara[E_PARA_APP_PAD_EDGE_THETA];
		//cv::Rect rtObject = cv::Rect(ptRotateCorner[E_CORNER_LEFT_TOP], ptRotateCorner[E_CORNER_RIGHT_BOTTOM]);

		//cv::threshold(matTempBuf, matTempBuf, nADThresh, 255, THRESH_BINARY);

		// PAD Cutting 이상하게 된 경우 찾기
		//if (Check_Abnormal_PADEdge(matTempBuf, nThreshold, dCompare_Theta, rtObject) == E_ERROR_CODE_FALSE)
		//{
		//	return E_ERROR_CODE_ALIGN_DISPLAY;
		//}
	}
	catch (const std::exception&)
	{
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("DoFindActive_APP."));

		return E_ERROR_CODE_FALSE;
	}

	return E_ERROR_CODE_TRUE;
}


// AVI 검사 영역 찾기
long CInspectAlign::DoFindActive_SVI(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point* ptResCorner, int nCameraNum, int nRatio, cv::Point& ptCenter, wchar_t* strID)
{
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;


	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples		= 3;	// 고정
	double	distThreshold	= 10;	// 고정

	int		nThreshold		= dPara[E_PARA_SVI_ALIGN_THRESHOLD];

	// 점등 : 점등 면적 ( Pixel 갯수 )
	int		nMinArea		= (int)(dPara[E_PARA_SVI_CELL_COAX_SIZE_X] * dPara[E_PARA_SVI_CELL_COAX_SIZE_Y] * nRatio * nRatio);

	// Side 카메라
	if( nCameraNum == 1 )
		nMinArea		= (int)(dPara[E_PARA_SVI_CELL_SIDE_SIZE_X] * dPara[E_PARA_SVI_CELL_SIDE_SIZE_Y] * nRatio * nRatio);

	// 안쪽으로 검사
	//int		nRoundIn		= (int)dPara[E_PARA_SVI_ROUND_IN];

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	long	nWidth	= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuf.rows;	// 영상 세로 크기

	//////////////////////////////////////////////////////////////////////////
	// Gray & 이진화
	//////////////////////////////////////////////////////////////////////////

	// Color -> Gray
	cv::Mat matGrayBuf;
	cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);

	// Threshold
	cv::Mat matTempBuf;
	cv::threshold(matGrayBuf, matTempBuf, nThreshold, 255.0, THRESH_BINARY);


	//////////////////////////////////////////////////////////////////////////
	// Cell 영역 찾기
	//////////////////////////////////////////////////////////////////////////

	// 검사 영역 Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matTempBuf, nMinArea, rectCell);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		matGrayBuf.release();
		matTempBuf.release();
		return nErrorCode;
	}

	// Cell 중심점
	ptCenter.x = rectCell.x + rectCell.width/2;
	ptCenter.y = rectCell.y + rectCell.height/2;


	//////////////////////////////////////////////////////////////////////////
	// Convex Hull - 누기, 노치 부분 매우기 위함
	//////////////////////////////////////////////////////////////////////////

	// Contours 구하기
	vector<vector<cv::Point>>	contours;
	vector<vector<cv::Point>>().swap(contours);

	cv::findContours( matTempBuf, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );

	matTempBuf.setTo(0);

	vector<cv::Point> ptContours, ptConvexHull;
	for (int m=0 ; m<contours.size() ; m++)
	{
		for(int k=0 ; k<contours.at(m).size() ; k++)
			ptContours.push_back( contours[m][k] );
	}

	// convexHull
	cv::convexHull(ptContours, ptConvexHull);
	cv::fillConvexPoly(matTempBuf, ptConvexHull, cv::Scalar(255, 255, 255));

	vector<vector<cv::Point>>().swap(contours);
	vector<cv::Point>().swap(ptContours);
	vector<cv::Point>().swap(ptConvexHull);


	//////////////////////////////////////////////////////////////////////////
	// 점등 영역 꼭지점 찾기 ( Fit )
	//////////////////////////////////////////////////////////////////////////

	long double	dValueAA[E_ALIGN_TYPE_END], dValueBB[E_ALIGN_TYPE_END];

	// 4방향 라인 피팅
	for(int nType=E_ALIGN_TYPE_LEFT ; nType<=E_ALIGN_TYPE_BOTTOM ; nType++)
	{
		nErrorCode = RobustFitLine(matTempBuf, rectCell, dValueAA[nType], dValueBB[nType], nMinSamples, distThreshold, nType, 10);

		// 에러인 경우, 에러 코드 출력
		if( nErrorCode != E_ERROR_CODE_TRUE )
			return nErrorCode;
	}

	// 코너 꼭지점 4개 찾기
	{
		for(int i=E_CORNER_LEFT_TOP ; i<=E_CORNER_LEFT_BOTTOM ; i++)
		{
			double dA1 = dValueAA[i];
			double dB1 = dValueBB[i];
			double dA2 = dValueAA[ (i+1)%E_CORNER_END ];
			double dB2 = dValueBB[ (i+1)%E_CORNER_END ];

			ptResCorner[i].x = (int)( (dB2 - dB1) / (dA1 - dA2) );
			ptResCorner[i].y = (int)( (dA2*dB1 - dA1*dB2) / (dA2 - dA1) );
		}
	}

	// 각도 구하기
	dTheta = atan( dValueAA[E_ALIGN_TYPE_TOP] ) * 180.0 / PI;

	// 예외처리
	if( 45.0 < dTheta && dTheta < 135.0 )	dTheta -= 90.0;
	if( -45.0 > dTheta && dTheta > -135.0 )	dTheta += 90.0;


	//////////////////////////////////////////////////////////////////////////
	// 영역 안쪽으로 살짝 축소...
	//////////////////////////////////////////////////////////////////////////

	//ptResCorner[E_CORNER_LEFT_TOP].x		+= nRoundIn;
	//ptResCorner[E_CORNER_LEFT_TOP].y		+= nRoundIn;
	//ptResCorner[E_CORNER_RIGHT_TOP].x		-= nRoundIn;
	//ptResCorner[E_CORNER_RIGHT_TOP].y		+= nRoundIn;
	//ptResCorner[E_CORNER_LEFT_BOTTOM].x	+= nRoundIn;
	//ptResCorner[E_CORNER_LEFT_BOTTOM].y	-= nRoundIn;
	//ptResCorner[E_CORNER_RIGHT_BOTTOM].x	-= nRoundIn;
	//ptResCorner[E_CORNER_RIGHT_BOTTOM].y	-= nRoundIn;


	// 메모리 해제
	matGrayBuf.release();
	matTempBuf.release();


	//////////////////////////////////////////////////////////////////////////
	// 결과 확인
	//////////////////////////////////////////////////////////////////////////

	if( FALSE )
	//if( TRUE )
	{
		cv::Mat matRectBuf;

		// 컬러인 경우
		if( matSrcBuf.channels() != 1 )	matRectBuf = matSrcBuf.clone();

		// 흑백인 경우
		else	cv::cvtColor(matSrcBuf, matRectBuf, COLOR_GRAY2RGB);

		for (int k=0 ; k<E_CORNER_END ; k++)
			cv::line(matRectBuf, ptResCorner[k], ptResCorner[ (k+1)%E_CORNER_END ], cv::Scalar(0, 255, 0), 1);		// Fit

// 실행 드라이브 D:\ 고정 아님 - 필요 시 InspectLibLog 의 GETDRV() 활용
// 		CString strTemp;
// 		strTemp.Format(_T("D:\\Active.bmp"));
// 		cv::imwrite( (cv::String)(CStringA)strTemp, matRectBuf);
	}

	return E_ERROR_CODE_TRUE;
}


// 외곽 Round & Camera Hole 곡선 설정 & 파일 저장
long CInspectAlign::SetFindContour(cv::Mat matSrcBuf, INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRountROICnt, double* dPara, int nAlgImg, int nRatio, CString strPath)
{
	//  nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 패턴 제외
	if( nAlgImg == E_IMAGE_CLASSIFY_AVI_BLACK )		return E_ERROR_CODE_TRUE;
	else if(nAlgImg == E_IMAGE_CLASSIFY_AVI_PCD)	return E_ERROR_CODE_TRUE;

	// KSW 17.11.14 - Dust 패턴 제외
	// Round인 경우 배경 올라올 경우 존재
	// White 패턴 외곽선 복사하여 사용
	if( nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST )		return E_ERROR_CODE_TRUE;

	cv::Mat matTempBuf;

	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples		= 3;	// 고정
	double	distThreshold	= 10;	// 고정

	int		nMinArea		= (int)(dPara[E_PARA_CELL_SIZE_X] * dPara[E_PARA_CELL_SIZE_Y] * nRatio * nRatio);
	int		nThreshold		= dPara[E_PARA_ALIGN_THRESHOLD];
	int		nMorp			= dPara[E_PARA_ALIGN_MORP];
	double	dAngleError		= dPara[E_PARA_ALIGN_ANGLE_ERR];
	double	dAngleWarning	= dPara[E_PARA_ALIGN_ANGLE_WAR];

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// AVI의 경우만 사용, Cell 배열 사이 빈공간 채우는 용도
	//////////////////////////////////////////////////////////////////////////

	cv::Mat matSrc8bit = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);		//메모리 할당

	if( matSrcBuf.type() == CV_8UC1 )
		matSrcBuf.copyTo(matSrc8bit);
	else
		matSrcBuf.convertTo(matSrc8bit, CV_8UC1, 1./16.);

	if( nMorp > 0 )
	{
		cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, Size(nMorp, nMorp), cv::Point(nMorp/2, nMorp/2));

		// Morphology Close ( Cell 사이 빈공간 채우기 )
		cv::morphologyEx(matSrc8bit, matTempBuf, MORPH_CLOSE, StructElem);

		StructElem.release();
	}
	else
		matTempBuf = matSrc8bit.clone();		//메모리 할당

	// Threshold
	cv::threshold(matTempBuf, matTempBuf, nThreshold, 255.0, THRESH_BINARY);


	//////////////////////////////////////////////////////////////////////////
	// Cell 영역 찾기
	//////////////////////////////////////////////////////////////////////////

	// 검사 영역 Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matTempBuf, nMinArea, rectCell);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		matTempBuf.release();
		return nErrorCode;
	}

	//////////////////////////////////////////////////////////////////////////
	// 영상 회전
	//////////////////////////////////////////////////////////////////////////

	long double	dValueA, dValueB;

	// Top 직선 찾기
	nErrorCode = RobustFitLine(matTempBuf, rectCell, dValueA, dValueB, nMinSamples, distThreshold, E_ALIGN_TYPE_TOP);

	// 회전 좌표 계산 시, 사용
	double dTheta = atan( dValueA ) * 180. / PI;
	DoRotateImage(matTempBuf, matTempBuf, dTheta);

	//cv::Mat matSaveBuf = matTempBuf.clone();

	//////////////////////////////////////////////////////////////////////////
	// 점등 영역 찾기
	//////////////////////////////////////////////////////////////////////////	

	// 전체 외각선
	vector< vector< cv::Point2i > > contours;
	cv::findContours(matTempBuf, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	cv::RotatedRect BoundingBox = cv::minAreaRect(contours[0]);

	cv::Point2f vertices[E_CORNER_END];
	BoundingBox.points(vertices);

	//////////////////////////////////////////////////////////////////////////
	// 코너 꼭지점 4개 찾기
	//////////////////////////////////////////////////////////////////////////

	cv::Point	ptCorner[E_CORNER_END];
	
	long	nWidth	= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuf.rows;	// 영상 세로 크기

	nErrorCode = FindCornerPoint(vertices, ptCorner, nWidth, nHeight);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;


	//////////////////////////////////////////////////////////////////////////
	// Contour
	//////////////////////////////////////////////////////////////////////////

	// Pixel 범위
	int nOffsetL		= 0;

	// 범위 크게 설정
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x,		ptCorner[E_CORNER_LEFT_BOTTOM].x	) - nOffsetL,
		min(ptCorner[E_CORNER_LEFT_TOP].y,		ptCorner[E_CORNER_RIGHT_TOP].y		) - nOffsetL,
		max(ptCorner[E_CORNER_RIGHT_TOP].x,		ptCorner[E_CORNER_RIGHT_BOTTOM].x	) + nOffsetL,
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	) + nOffsetL );
	

	// 곡선 영역 설정 영역 보정 필요 ( 꼭지점 보정 )
	CRect	rectTemp[MAX_MEM_SIZE_E_INSPECT_AREA];
	bool	nInside[MAX_MEM_SIZE_E_INSPECT_AREA][E_CORNER_END];

	 for (int i=0 ; i<nRountROICnt ; i++)
	{
		// UI 설정한 영역 가져오기
		rectTemp[i] = RoundROI[i].rectROI;

		// PS 모드 보정
		rectTemp[i].left	*= nRatio;
		rectTemp[i].top		*= nRatio;
		rectTemp[i].right	*= nRatio;
		rectTemp[i].bottom	*= nRatio;

		// Left-Top 좌표를 원점으로 좌표값을 가지고 있음
		// 현재 점등 영역에 맞게 보정
		rectTemp[i].OffsetRect( CPoint(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_TOP].y) );

		// 예외처리
		if( rectTemp[i].left	< 0 )		rectTemp[i].left	= 0;
		if( rectTemp[i].top		< 0 )		rectTemp[i].top		= 0;
		if( rectTemp[i].right	< 0 )		rectTemp[i].right 	= 0;
		if( rectTemp[i].bottom	< 0 )		rectTemp[i].bottom	= 0;

		if( rectTemp[i].left	>= matTempBuf.cols )	rectTemp[i].left	= matTempBuf.cols - 1;
		if( rectTemp[i].top		>= matTempBuf.rows )	rectTemp[i].top		= matTempBuf.rows - 1;
		if( rectTemp[i].right	>= matTempBuf.cols )	rectTemp[i].right	= matTempBuf.cols - 1;
		if( rectTemp[i].bottom	>= matTempBuf.rows )	rectTemp[i].bottom	= matTempBuf.rows - 1;

		// 꼭지점이 Cell 점등 영역 안에 존재하는지 확인
		nInside[i][E_CORNER_LEFT_TOP		] = (matTempBuf.at<uchar>(rectTemp[i].top		, rectTemp[i].left	) != 0 ) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_TOP		] = (matTempBuf.at<uchar>(rectTemp[i].top		, rectTemp[i].right	) != 0 ) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_BOTTOM	] = (matTempBuf.at<uchar>(rectTemp[i].bottom	, rectTemp[i].right	) != 0 ) ? 1 : 0;
		nInside[i][E_CORNER_LEFT_BOTTOM		] = (matTempBuf.at<uchar>(rectTemp[i].bottom	, rectTemp[i].left	) != 0 ) ? 1 : 0;
	}

	// 결과 확인 용
	//{
	//	for (int i=0 ; i<MAX_MEM_SIZE_E_INSPECT_AREA ; i++)
	//	{
	//		if( RoundROI[i].bUseROI )
	//		{
	//			CRect rect = rectTemp[i];
	//
	//			cv::rectangle(matSaveBuf, cv::Rect(rect.left, rect.top, rect.Width(), rect.Height()), cv::Scalar(128, 128, 128) );
	//		}
	//	}
	//
	//	cv::imwrite("D:\\temp.bmp", matSaveBuf);
	//	matSaveBuf.release();
	//}
	
	// 곡선 외각선
	vector< cv::Point2i > ptContours[MAX_MEM_SIZE_E_INSPECT_AREA];
	for (int i=0 ; i<MAX_MEM_SIZE_E_INSPECT_AREA ; i++)
		vector< cv::Point2i >().swap(ptContours[i]);

	// 꼭지점 개수 만큼
	for (int i=0; i<contours.size() ; i++)
	{
		for (int j=0 ; j<contours[i].size() ; j++)
		{
			// UI 설정한 영역 개수만큼
			for (int k=0 ; k<nRountROICnt ; k++)
			{
				// 사용 안하는 것 제외
				if( !RoundROI[k].bUseROI )	continue;

				// 보정된 영역 안에, 해당 좌표가 있는 경우
				if( rectTemp[k].PtInRect( CPoint(contours[i][j].x, contours[i][j].y) ) )
				{
					// 좌표 추가
					ptContours[k].push_back( cv::Point2i(contours[i][j]) );
					break;
				}
			}
		}
		// 초기화
		vector< cv::Point2i >().swap(contours[i]);
	}

	//////////////////////////////////////////////////////////////////////////
	// 순서 정렬
	// 다각형 & 내부 색상채울때, 순서대로 필요 함...
	//////////////////////////////////////////////////////////////////////////

	cv::Point2i ptTempS;
	for (int i=0 ; i<MAX_MEM_SIZE_E_INSPECT_AREA ; i++)
	{		
		for (int j=0 ; j<ptContours[i].size() ; j++ )
		{
			for (int m=j+1 ; m<ptContours[i].size() ; m++)
			{
				// y좌표가 작은것 위로 오도록 정렬
				if( ptContours[i][j].y > ptContours[i][m].y )
				{
					ptTempS				= ptContours[i][j];
					ptContours[i][j]	= ptContours[i][m];
					ptContours[i][m]	= ptTempS;
				}
				// y좌표가 같은 경우
				else if( ptContours[i][j].y == ptContours[i][m].y )
				{
					// 꼭지점에서 먼거리인 경우 변경
					if( abs(ptCorner[i].x - ptContours[i][j].x) < abs(ptCorner[i].x - ptContours[i][m].x) )
					{
						ptTempS				= ptContours[i][j];
						ptContours[i][j]	= ptContours[i][m];
						ptContours[i][m]	= ptTempS;
					}
				}
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// Save
	//////////////////////////////////////////////////////////////////////////

	char szPath[256] = {0, };
	WideCharToMultiByte(CP_ACP, 0, strPath, -1, szPath, sizeof(szPath), NULL, NULL);

	// 파일 저장 경로
	CString str;
	str.Format(_T("%s\\CornerEdge"), strPath);
	CreateDirectory(str, NULL);

	for (int i=0; i<MAX_MEM_SIZE_E_INSPECT_AREA ; i++)
	{
		// 개수 없는 경우 제외
		if ( ptContours[i].size() <= 0 )	continue;

		//////////////////////////////////////////////////////////////////////////
		// 설정 영역이, 점등된 Cell 꼭지점에서 제일 가까운 꼭지점 찾기
		//////////////////////////////////////////////////////////////////////////

		int nCx = (rectTemp[i].left + rectTemp[i].right ) / 2;
		int nCy = (rectTemp[i].top  + rectTemp[i].bottom) / 2;

		// LT와의 거리
		int nLength = abs(ptCorner[E_CORNER_LEFT_TOP].x - nCx) + abs(ptCorner[E_CORNER_LEFT_TOP].y - nCy);
		int	nIndex	= E_CORNER_LEFT_TOP;
		for (int j=1 ; j<E_CORNER_END ; j++)
		{
			int nTempLenght = abs(ptCorner[j].x - nCx) + abs(ptCorner[j].y - nCy);

			// 작은 거리인 꼭지점 찾기
			if( nLength > nTempLenght)
			{
				nLength	= nTempLenght;
				nIndex	= j;
			}
		}
		
		//////////////////////////////////////////////////////////////////////////		

		// 파일 저장 경로
		CStringA strTemp;
		strTemp.Format(("%s\\CornerEdge\\%s_%02d.EdgePT"), szPath, GetPatternStringA(nAlgImg), i);

		// 파일 열기 ( 유니코드 환경 "t" -> "wt" )
		FILE *out = NULL;
		fopen_s(&out, strTemp, "wt");

		if( out != NULL )
		{
			// 가까운 인덱스 표기
			fprintf_s(out, "CornerIndex%d\n", nIndex);

			// 꼭지점이 Cell 영역 안에 존재하는지 확인
			fprintf_s(out, "CornerInside%d,%d,%d,%d\n", nInside[i][0], nInside[i][1], nInside[i][2], nInside[i][3]);

			for (int j=0 ; j<ptContours[i].size() ; j++)
			{
				// 코너를 원점 & 각도 0도
				fprintf_s(out, "%d,%d\n", ptContours[i][j].x - ptCorner[nIndex].x, ptContours[i][j].y - ptCorner[nIndex].y);
			}

			// 파일 닫기
			fclose(out);
			out = NULL;

			// KSW 17.11.14 - White인 경우 Dust패턴으로 복사
			if( nAlgImg == E_IMAGE_CLASSIFY_AVI_WHITE )
			{
				CStringA strCopy;
				strCopy.Format(("%s\\CornerEdge\\%s_%02d.EdgePT"), szPath, GetPatternStringA(E_IMAGE_CLASSIFY_AVI_DUST), i);

				CopyFile((CString)strTemp, (CString)strCopy, FALSE);
			}
		}

		// 초기화
		vector< cv::Point2i >().swap(ptContours[i]);
	}

	// 메모리 해제
	matTempBuf.release();

	return E_ERROR_CODE_TRUE;
}
long CInspectAlign::SetFindContour_APP(cv::Mat matSrcBuf, INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRountROICnt, double* dPara, int nAlgImg, int nRatio, CString strPath, Point* ptAlignCorner, CStringA strImageName, double dAlignTH, bool bImageSave)
{
	int nImageSaveInx = 0;

	CString strSavePath;
	CString strSaveName;
	strSavePath.Format(_T("D:\\ActiveMaskPrc"));
	if(bImageSave)
	{
		//경로 Check
		DWORD result;
		if (((result=GetFileAttributes(strSavePath)) == -1 ) || !(result&FILE_ATTRIBUTE_DIRECTORY) ) {
			CreateDirectory(strSavePath, NULL);
		}
	}

	long nErrorCode = E_ERROR_CODE_TRUE;

	float	fResize				= (float)dPara[E_PARA_APP_POLYGON_RESIZE];							//영상 Resize할 크기
	int		nGausBlurSize		= (int)dPara[E_PARA_APP_POLYGON_GAUS_SIZE];							//노이즈 제거용 Gaussian Blur Mask Size
	float	fGausSigma			= (float)dPara[E_PARA_APP_POLYGON_GAUS_SIGMA];						//노이즈 제거용 Gaussian Blur Sigma 값
	int		nThreshold			= (int)dPara[E_PARA_APP_POLYGON_THRESHOLD];							//Sobel Edge 처리 후 필요한 선만 추출하기 위한 임계값
	int		nOpenSize			= (int)dPara[E_PARA_APP_POLYGON_OPEN_SIZE];							//Panel의 외곽선, BM Line선, Active 외곽선들의 선경계를 명확하게 하기 위한 열림연산 크기
	int		nSelectBlob			= (int)dPara[E_PARA_APP_POLYGON_SELECT_BLOB];						//내림차순한 Blob 중 몇 번째로 큰 것을 선택할 것인지
	int		nDilateSize			= (int)dPara[E_PARA_APP_POLYGON_DILATE_SIZE];						//Active Mask에 구멍 난 것을 매꾸기 위한 팽창연산 크기
	int		nErodeSize			= (int)dPara[E_PARA_APP_POLYGON_ERODE_SIZE];						//Active Mask의 구멍을 매꾸면서 키운 크기를 원래크기로 돌리기 위한 침식연산 크기

	//영상처리 Mask Size 예외처리
	GetCheckMaskSize(nGausBlurSize);
	GetCheckMaskSize(nOpenSize);
	GetCheckMaskSize(nErodeSize);

	Point ptPanelCorner[E_CORNER_END];
	for(int nCorner = E_CORNER_LEFT_TOP; nCorner < E_CORNER_END; nCorner++)
 	{
 		ptPanelCorner[nCorner] = ptAlignCorner[nCorner];
 	}

	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	nErrorCode = E_ERROR_CODE_ALIGN_ROUND_SETTING;

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, matSrcBuf);
	}

	Mat OrgImg;
	matSrcBuf.copyTo(OrgImg);

	DoRotateImage(OrgImg, OrgImg, dAlignTH);

	Mat OrgRotateImage;
	OrgImg.copyTo(OrgRotateImage);

	cv::resize(OrgImg, OrgImg, cv::Size(), fResize, fResize);

	cv::GaussianBlur(OrgImg, OrgImg, Size(nGausBlurSize, nGausBlurSize), fGausSigma);

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, OrgImg);
	}

	Mat grad_X, grad_Y;
	Mat abs_grad_X, abs_grad_Y;
	int nDepth = CV_16S;

	Sobel( OrgImg, grad_X, nDepth, 1, 0, 3);
	convertScaleAbs( grad_X, abs_grad_X );

	Sobel( OrgImg, grad_Y, nDepth, 0, 1, 3);
	convertScaleAbs( grad_Y, abs_grad_Y );

	cv::addWeighted( abs_grad_X, 0.5, abs_grad_Y, 0.5, 0, OrgImg );

	cv::threshold(OrgImg, OrgImg, nThreshold, 255, CV_THRESH_BINARY);

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, OrgImg);
	}

	//가장 큰 Blob 반환
	Mat mtBiggist;
	mtBiggist = Mat::zeros(OrgImg.size(), OrgImg.type());
	FindBiggestBlob(OrgImg, mtBiggist);

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, mtBiggist);
	}

	//반전
	cv::bitwise_not(mtBiggist, mtBiggist);

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, mtBiggist);
	}

	//Active 영역 분리
	cv::morphologyEx(mtBiggist, mtBiggist, cv::MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(nOpenSize, nOpenSize)));

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, mtBiggist);
	}

	//선택한 순서 번째로 큰 Blob 반환
	Mat mtSelectBig;
	mtSelectBig = Mat::zeros(mtBiggist.size(), mtBiggist.type());
	SelectBiggestBlob(mtBiggist, mtSelectBig,  nSelectBlob);

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, mtSelectBig);
	}

	//이진화 된 Active 영역에서 구멍난 곳 매꾸기
	cv::morphologyEx(mtSelectBig, mtSelectBig, cv::MORPH_DILATE, getStructuringElement(MORPH_RECT, Size(nDilateSize, nDilateSize)));
	cv::morphologyEx(mtSelectBig, mtSelectBig, cv::MORPH_ERODE, getStructuringElement(MORPH_RECT, Size(nErodeSize, nErodeSize)));

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, mtSelectBig);
	}

	//원본 영상크기로 복구
	cv::resize(mtSelectBig, mtSelectBig, cv::Size(), 1/fResize, 1/fResize);

	vector<vector<cv::Point>> ptSelectBig;
	findContours(mtSelectBig, ptSelectBig, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cv::Point(0,0));

	Mat mtActiveArea;
	mtActiveArea = Mat::zeros(mtSelectBig.size(), mtSelectBig.type());
	drawContours(mtActiveArea, ptSelectBig, 0, Scalar(255));

	//////////////////////////////////////////////////////////////////////////
	// Save
	//////////////////////////////////////////////////////////////////////////

	char szPath[256] = {0, };
	WideCharToMultiByte(CP_ACP, 0, strPath, -1, szPath, sizeof(szPath), NULL, NULL);

	// 파일 저장 경로
	CString str;
	str.Format(_T("%s\\CornerEdge"), strPath);
	CreateDirectory(str, NULL);

	//Polygon ROI 받아오기
	Rect* rtPolygonROI;
	rtPolygonROI = new Rect[nRountROICnt];

	for(int nROIInx = 0; nROIInx < nRountROICnt; nROIInx++)
	{
		rtPolygonROI[nROIInx] = Rect(Point(RoundROI[nROIInx].rectROI.left + ptPanelCorner[E_CORNER_LEFT_TOP].x, 
									RoundROI[nROIInx].rectROI.top + ptPanelCorner[E_CORNER_LEFT_TOP].y),
								Point(RoundROI[nROIInx].rectROI.right + ptPanelCorner[E_CORNER_LEFT_TOP].x, 
									RoundROI[nROIInx].rectROI.bottom + ptPanelCorner[E_CORNER_LEFT_TOP].y));

		GetCheckROIOver(rtPolygonROI[nROIInx], Rect(0,0,matSrcBuf.cols,matSrcBuf.rows), rtPolygonROI[nROIInx]);
		
		CStringA strROI;
		strROI.Format("%s\\CornerEdge\\%d.bmp", szPath, nROIInx);
		imwrite((cv::String)strROI, OrgRotateImage(rtPolygonROI[nROIInx]));

		// 파일 저장 경로
		CStringA strTemp;
		strTemp.Format(("%s\\CornerEdge\\%s_%02d.EdgePT"), szPath, strImageName, nROIInx);

		// 파일 열기 ( 유니코드 환경 "t" -> "wt" )
		FILE *out = NULL;
		fopen_s(&out, strTemp, "wt");

		if( out != NULL )
		{
			vector<vector<cv::Point>> ptActiveArea;
			
			findContours(mtActiveArea(rtPolygonROI[nROIInx]), ptActiveArea, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cv::Point(0,0));

			//Contour가 없다면
			if(ptActiveArea.size() < 1)
			{
				// 파일 닫기
				fclose(out);
				out = NULL;
				continue;
			}

			int nPanelCenterX = ptPanelCorner[E_CORNER_LEFT_TOP].x +(ptPanelCorner[E_CORNER_RIGHT_TOP].x - ptPanelCorner[E_CORNER_LEFT_TOP].x)/2;

			Point ptTempS;

			for (int j=0 ; j<ptActiveArea[0].size() ; j++ )
			{
				for (int m=j+1 ; m<ptActiveArea[0].size() ; m++)
				{
					if(nPanelCenterX > rtPolygonROI[nROIInx].tl().x )
					{
						// y좌표 오름차순
						if( ptActiveArea[0][j].y > ptActiveArea[0][m].y )
						{
							ptTempS				= ptActiveArea[0][j];
							ptActiveArea[0][j]	= ptActiveArea[0][m];
							ptActiveArea[0][m]	= ptTempS;
						}
					}
					else
					{
						// y좌표 내림차순
						if( ptActiveArea[0][j].y < ptActiveArea[0][m].y )
						{
							ptTempS				= ptActiveArea[0][j];
							ptActiveArea[0][j]	= ptActiveArea[0][m];
							ptActiveArea[0][m]	= ptTempS;
						}
					}
				
				}
			}

			fprintf_s(out, "CornerIndex %d\n", (int)ptActiveArea[0].size());

			for (int j=0 ; j<ptActiveArea[0].size() ; j++)
			{
				fprintf_s(out, "%d,%d\n", ptActiveArea[0][j].x, ptActiveArea[0][j].y);
			}
			
			// 파일 닫기
			fclose(out);
			out = NULL;
		}
	}
	
	delete[] rtPolygonROI;


	return E_ERROR_CODE_TRUE;
}

// 외곽 Round & Camera Hole 곡선 설정 & 파일 저장
long CInspectAlign::SetFindContour_(cv::Mat matSrcBuf, INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], INSP_AREA CHoleROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRoundROICnt, int nCHoleROICnt, double* dPara, int nAlgImg, int nRatio, CString strPath)
{
	// 패턴 제외
	if (nAlgImg == E_IMAGE_CLASSIFY_AVI_BLACK)		return E_ERROR_CODE_TRUE;
	else if (nAlgImg == E_IMAGE_CLASSIFY_AVI_PCD)	return E_ERROR_CODE_TRUE;

	// KSW 17.11.14 - Dust 패턴 제외
	// Round인 경우 배경 올라올 경우 존재
	// White 패턴 외곽선 복사하여 사용
	if (nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST)		return E_ERROR_CODE_TRUE;

	// 버퍼가 없는 경우.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples = 3;	// 고정
	double	distThreshold = 10;	// 고정

	int		nMorp = dPara[E_PARA_ALIGN_MORP];
	int		nThreshold = dPara[E_PARA_ALIGN_THRESHOLD];
	int		nMinArea = (int)(dPara[E_PARA_CELL_SIZE_X] * dPara[E_PARA_CELL_SIZE_Y] * nRatio * nRatio);

	bool	bRoundSet = (dPara[E_PARA_ROUND_SETTING] > 0) ? true : false;
	bool	bCHoleSet = (dPara[E_PARA_CHOLE_SETTING] > 0) ? true : false;
	//////////////////////////////////////////////////////////////////////////

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	// 버퍼 할당 & 초기화


	//////////////////////////////////////////////////////////////////////////
	// ShiftCopy
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matDstBuf = cMem->GetMat(matSrcBuf.size(), matSrcBuf.type());
	// 파라미터 추가 필요

	// 영역 안쪽으로 살짝 축소... ( 영역 )
	int nInPixel = 3;

	// PS 모드인 경우
	if (nRatio != 1)
	{
		// Shift Copy Parameter 받아 오기
		int		nRedPattern = (int)dPara[E_PARA_SHIFT_COPY_R];
		int		nGreenPattern = (int)dPara[E_PARA_SHIFT_COPY_G];
		int		nBluePattern = (int)dPara[E_PARA_SHIFT_COPY_B];

		int nCpyX = 0, nCpyY = 0, nLoopX = 0, nLoopY = 0;

		// 패턴별...
		switch (nAlgImg)
		{
		case E_IMAGE_CLASSIFY_AVI_R:
		{
			if (nRedPattern == 0) break;
			ShiftCopyParaCheck(nRedPattern, nCpyX, nCpyY, nLoopX, nLoopY);
			nErrorCode = ShiftCopy(matSrcBuf, matDstBuf, nCpyX, nCpyY, nLoopX, nLoopY);
			matDstBuf.copyTo(matSrcBuf);
			matDstBuf.release();
		}
		break;

		case E_IMAGE_CLASSIFY_AVI_G:
		{
			if (nGreenPattern == 0) break;
			ShiftCopyParaCheck(nGreenPattern, nCpyX, nCpyY, nLoopX, nLoopY);
			nErrorCode = ShiftCopy(matSrcBuf, matDstBuf, nCpyX, nCpyY, nLoopX, nLoopY);
			matDstBuf.copyTo(matSrcBuf);
			matDstBuf.release();
		}
		break;

		case E_IMAGE_CLASSIFY_AVI_B:
		{
			if (nBluePattern == 0) break;
			ShiftCopyParaCheck(nBluePattern, nCpyX, nCpyY, nLoopX, nLoopY);
			nErrorCode = ShiftCopy(matSrcBuf, matDstBuf, nCpyX, nCpyY, nLoopX, nLoopY);
			matDstBuf.copyTo(matSrcBuf);
			matDstBuf.release();
		}
		break;

		default:
			break;
		}

	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("ShiftCopy."));

	// 에러인 경우, 에러 코드 출력
	if (nErrorCode != E_ERROR_CODE_TRUE)
	{
		return nErrorCode;
	}

	//////////////////////////////////////////////////////////////////////////
	// AVI의 경우만 사용, Cell 배열 사이 빈공간 채우는 용도
	//////////////////////////////////////////////////////////////////////////

	cv::Mat matSrc8bit = cMem->GetMat(matSrcBuf.size(), CV_8UC1);

	if (matSrcBuf.type() == CV_8UC1)
		matSrcBuf.copyTo(matSrc8bit);
	else
		matSrcBuf.convertTo(matSrc8bit, CV_8UC1, 1. / 16.);

	cv::Mat matTempBuf = cMem->GetMat(matSrc8bit.size(), CV_8UC1);

	if (nMorp > 0)
	{
		cv::Mat	StructElem = cv::getStructuringElement(MORPH_RECT, Size(nMorp, nMorp), cv::Point(nMorp / 2, nMorp / 2));

		// Morphology Close ( Cell 사이 빈공간 채우기 )
		cv::morphologyEx(matSrc8bit, matTempBuf, MORPH_CLOSE, StructElem);

		StructElem.release();
	}
	else
		matSrc8bit.copyTo(matTempBuf);

	cv::threshold(matTempBuf, matTempBuf, nThreshold, 255.0, THRESH_BINARY);


	//////////////////////////////////////////////////////////////////////////
	// Cell 영역 찾기
	//////////////////////////////////////////////////////////////////////////

	// 검사 영역 Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matTempBuf, nMinArea, rectCell);

	// 에러인 경우, 에러 코드 출력
	if (nErrorCode != E_ERROR_CODE_TRUE)
	{
		matTempBuf.release();
		return nErrorCode;
	}

	//////////////////////////////////////////////////////////////////////////
	// 영상 회전
	//////////////////////////////////////////////////////////////////////////

	long double	dValueA, dValueB;

	// Top 직선 찾기
	nErrorCode = RobustFitLine(matTempBuf, rectCell, dValueA, dValueB, nMinSamples, distThreshold, E_ALIGN_TYPE_TOP);

	// 회전 좌표 계산 시, 사용
	double dTheta = atan(dValueA) * 180. / PI;
	DoRotateImage(matTempBuf, matTempBuf, dTheta);


	//////////////////////////////////////////////////////////////////////////
	// 점등 영역 찾기 & Hole 찾기
	//////////////////////////////////////////////////////////////////////////	

	// 외부 외각선 & 내부 외각선
	vector< vector< cv::Point2i > > contours;
	int nContourSize = 0;
	int nContourIdx = 0;
	cv::findContours(matTempBuf, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	for (int i = 0; i < contours.size(); i++)
	{
		if (nContourSize < contours[i].size())
		{
			nContourSize = contours[i].size();
			nContourIdx = i;
		}
	}

	cv::RotatedRect BoundingBox = cv::minAreaRect(contours[nContourIdx]);

	cv::Point2f vertices[E_CORNER_END];
	BoundingBox.points(vertices);


	//////////////////////////////////////////////////////////////////////////
	// 코너 꼭지점 4개 찾기
	//////////////////////////////////////////////////////////////////////////

	cv::Point	ptCorner[E_CORNER_END];

	long	nWidth = (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcBuf.rows;	// 영상 세로 크기

	nErrorCode = FindCornerPoint(vertices, ptCorner, nWidth, nHeight);

	// 에러인 경우, 에러 코드 출력
	if (nErrorCode != E_ERROR_CODE_TRUE)
		return nErrorCode;


	//////////////////////////////////////////////////////////////////////////
	// Contour
	//////////////////////////////////////////////////////////////////////////

	if (bRoundSet)
		SetFindRound(matTempBuf, contours, ptCorner, RoundROI, nRoundROICnt, nContourIdx, nAlgImg, nRatio, strPath);

	if (bCHoleSet)
		SetFindCHole(matTempBuf, contours, ptCorner, CHoleROI, nCHoleROICnt, nContourIdx, nAlgImg, nRatio, strPath);

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return nErrorCode;
}


// Round 좌표 저장
void CInspectAlign::SetFindRound(cv::Mat& matTempBuf, vector< vector< cv::Point2i > > contours, cv::Point ptCorner[E_CORNER_END], INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRoundROICnt, int nContourIdx, int nAlgImg, int nRatio, CString strPath)
{
	// Image Save 0 On / -1 Off
	int nSaveImg = -1;
	// 곡선 영역 설정 영역 보정 필요 ( 꼭지점 보정 )
	CRect	rectTemp[MAX_MEM_SIZE_E_INSPECT_AREA];
	bool	nInside[MAX_MEM_SIZE_E_INSPECT_AREA][E_CORNER_END];

	for (int i = 0; i < nRoundROICnt; i++)
	{
		// UI 설정한 영역 가져오기
		rectTemp[i] = RoundROI[i].rectROI;

		// PS 모드 보정
		rectTemp[i].left *= nRatio;
		rectTemp[i].top *= nRatio;
		rectTemp[i].right *= nRatio;
		rectTemp[i].bottom *= nRatio;

		// Left-Top 좌표를 원점으로 좌표값을 가지고 있음
		// 현재 점등 영역에 맞게 보정
		rectTemp[i].OffsetRect(CPoint(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_TOP].y));

		// 예외처리
		if (rectTemp[i].left < 0)		rectTemp[i].left = 0;
		if (rectTemp[i].top < 0)		rectTemp[i].top = 0;
		if (rectTemp[i].right < 0)		rectTemp[i].right = 0;
		if (rectTemp[i].bottom < 0)		rectTemp[i].bottom = 0;

		if (rectTemp[i].left >= matTempBuf.cols)	rectTemp[i].left = matTempBuf.cols - 1;
		if (rectTemp[i].top >= matTempBuf.rows)	rectTemp[i].top = matTempBuf.rows - 1;
		if (rectTemp[i].right >= matTempBuf.cols)	rectTemp[i].right = matTempBuf.cols - 1;
		if (rectTemp[i].bottom >= matTempBuf.rows)	rectTemp[i].bottom = matTempBuf.rows - 1;

		// 꼭지점이 Cell 점등 영역 안에 존재하는지 확인
		nInside[i][E_CORNER_LEFT_TOP] = (matTempBuf.at<uchar>(rectTemp[i].top, rectTemp[i].left) != 0) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_TOP] = (matTempBuf.at<uchar>(rectTemp[i].top, rectTemp[i].right) != 0) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_BOTTOM] = (matTempBuf.at<uchar>(rectTemp[i].bottom, rectTemp[i].right) != 0) ? 1 : 0;
		nInside[i][E_CORNER_LEFT_BOTTOM] = (matTempBuf.at<uchar>(rectTemp[i].bottom, rectTemp[i].left) != 0) ? 1 : 0;
	}
	// 결과 확인 용
	if (nSaveImg >= 0)
	{
		cv::Mat matSaveBuf = matTempBuf.clone();
		for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
		{
			if (RoundROI[i].bUseROI)
			{
				CRect rect = rectTemp[i];

				cv::rectangle(matSaveBuf, cv::Rect(rect.left, rect.top, rect.Width(), rect.Height()), cv::Scalar(128, 128, 128));
			}
		}

		cv::imwrite("D:\\Round.bmp", matSaveBuf);
		matSaveBuf.release();
	}

	// 곡선 외각선
	vector< cv::Point2i > ptContours[MAX_MEM_SIZE_E_INSPECT_AREA];
	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
		vector< cv::Point2i >().swap(ptContours[i]);

	// 꼭지점 개수 만큼
	for (int i = 0; i < contours.size(); i++)
	{
		if (i != nContourIdx) continue;
		for (int j = 0; j < contours[i].size(); j++)
		{
			// UI 설정한 영역 개수만큼
			for (int k = 0; k < nRoundROICnt; k++)
			{
				// 사용 안하는 것 제외
				if (!RoundROI[k].bUseROI)	continue;

				// 보정된 영역 안에, 해당 좌표가 있는 경우
				if (rectTemp[k].PtInRect(CPoint(contours[i][j].x, contours[i][j].y)))
				{
					// 좌표 추가
					ptContours[k].push_back(cv::Point2i(contours[i][j]));
					break;
				}
			}
		}
		// 초기화
		vector< cv::Point2i >().swap(contours[i]);
	}

	//////////////////////////////////////////////////////////////////////////
	// 순서 정렬
	// 다각형 & 내부 색상채울때, 순서대로 필요 함...
	//////////////////////////////////////////////////////////////////////////

	cv::Point2i ptTempS;
	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		for (int j = 0; j < ptContours[i].size(); j++)
		{
			for (int m = j + 1; m < ptContours[i].size(); m++)
			{
				// y좌표가 작은것 위로 오도록 정렬
				if (ptContours[i][j].y > ptContours[i][m].y)
				{
					ptTempS = ptContours[i][j];
					ptContours[i][j] = ptContours[i][m];
					ptContours[i][m] = ptTempS;
				}
				// y좌표가 같은 경우
				else if (ptContours[i][j].y == ptContours[i][m].y)
				{
					// 꼭지점에서 먼거리인 경우 변경
					if (abs(ptCorner[i].x - ptContours[i][j].x) < abs(ptCorner[i].x - ptContours[i][m].x))
					{
						ptTempS = ptContours[i][j];
						ptContours[i][j] = ptContours[i][m];
						ptContours[i][m] = ptTempS;
					}
				}
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// Save
	//////////////////////////////////////////////////////////////////////////

	char szPath[256] = { 0, };
	WideCharToMultiByte(CP_ACP, 0, strPath, -1, szPath, sizeof(szPath), NULL, NULL);

	// 파일 저장 경로
	CString str;
	str.Format(_T("%s\\CornerEdge"), strPath);
	CreateDirectory(str, NULL);

	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		// 개수 없는 경우 제외
		if (ptContours[i].size() <= 0)	continue;

		//////////////////////////////////////////////////////////////////////////
		// 설정 영역이, 점등된 Cell 꼭지점에서 제일 가까운 꼭지점 찾기
		//////////////////////////////////////////////////////////////////////////

		int nCx = (rectTemp[i].left + rectTemp[i].right) / 2;
		int nCy = (rectTemp[i].top + rectTemp[i].bottom) / 2;

		// LT와의 거리
		int nLength = abs(ptCorner[E_CORNER_LEFT_TOP].x - nCx) + abs(ptCorner[E_CORNER_LEFT_TOP].y - nCy);
		int	nIndex = E_CORNER_LEFT_TOP;
		for (int j = 1; j < E_CORNER_END; j++)
		{
			int nTempLenght = abs(ptCorner[j].x - nCx) + abs(ptCorner[j].y - nCy);

			// 작은 거리인 꼭지점 찾기
			if (nLength > nTempLenght)
			{
				nLength = nTempLenght;
				nIndex = j;
			}
		}

		//////////////////////////////////////////////////////////////////////////		

		// 파일 저장 경로
		CStringA strTemp;
		strTemp.Format(("%s\\CornerEdge\\%s_%02d.EdgePT"), szPath, GetPatternStringA(nAlgImg), i);

		// 파일 열기 ( 유니코드 환경 "t" -> "wt" )
		FILE *out = NULL;
		fopen_s(&out, strTemp, "wt");

		if (out != NULL)
		{
			// 가까운 인덱스 표기
			fprintf_s(out, "CornerIndex%d\n", nIndex);

			// 꼭지점이 Cell 영역 안에 존재하는지 확인
			fprintf_s(out, "CornerInside%d,%d,%d,%d\n", nInside[i][0], nInside[i][1], nInside[i][2], nInside[i][3]);

			for (int j = 0; j < ptContours[i].size(); j++)
			{
				// 코너를 원점 & 각도 0도
				fprintf_s(out, "%d,%d\n", ptContours[i][j].x - ptCorner[nIndex].x, ptContours[i][j].y - ptCorner[nIndex].y);
			}

			// 파일 닫기
			fclose(out);
			out = NULL;

			// KSW 17.11.14 - White인 경우 Dust패턴으로 복사
			if (nAlgImg == E_IMAGE_CLASSIFY_AVI_WHITE)
			{
				CStringA strCopy;
				strCopy.Format(("%s\\CornerEdge\\%s_%02d.EdgePT"), szPath, GetPatternStringA(E_IMAGE_CLASSIFY_AVI_DUST), i);

				CopyFile((CString)strTemp, (CString)strCopy, FALSE);
			}
		}

		// 초기화
		vector< cv::Point2i >().swap(ptContours[i]);
	}
}


// Camera Hole 좌표 저장
void CInspectAlign::SetFindCHole(cv::Mat& matTempBuf, vector< vector< cv::Point2i > > contours, cv::Point ptCorner[E_CORNER_END], INSP_AREA CHoleROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nCHoleROICnt, int nContourIdx, int nAlgImg, int nRatio, CString strPath)
{
	// Image Save 0 On / -1 Off
	int		nSaveImg = -1;

	// 곡선 영역 설정 영역 보정 필요 ( 꼭지점 보정 )
	CRect	rectTemp[MAX_MEM_SIZE_E_INSPECT_AREA];
	bool	nInside[MAX_MEM_SIZE_E_INSPECT_AREA][E_CORNER_END];

	for (int i = 0; i < nCHoleROICnt; i++)
	{
		// UI 설정한 영역 가져오기
		rectTemp[i] = CHoleROI[i].rectROI;

		// PS 모드 보정
		rectTemp[i].left *= nRatio;
		rectTemp[i].top *= nRatio;
		rectTemp[i].right *= nRatio;
		rectTemp[i].bottom *= nRatio;

		// Left-Top 좌표를 원점으로 좌표값을 가지고 있음
		// 현재 점등 영역에 맞게 보정
		rectTemp[i].OffsetRect(CPoint(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_TOP].y));

		// 예외처리
		if (rectTemp[i].left < 0)		rectTemp[i].left = 0;
		if (rectTemp[i].top < 0)		rectTemp[i].top = 0;
		if (rectTemp[i].right < 0)		rectTemp[i].right = 0;
		if (rectTemp[i].bottom < 0)		rectTemp[i].bottom = 0;

		if (rectTemp[i].left >= matTempBuf.cols)	rectTemp[i].left = matTempBuf.cols - 1;
		if (rectTemp[i].top >= matTempBuf.rows)	rectTemp[i].top = matTempBuf.rows - 1;
		if (rectTemp[i].right >= matTempBuf.cols)	rectTemp[i].right = matTempBuf.cols - 1;
		if (rectTemp[i].bottom >= matTempBuf.rows)	rectTemp[i].bottom = matTempBuf.rows - 1;

		// 꼭지점이 Cell 점등 영역 안에 존재하는지 확인
		nInside[i][E_CORNER_LEFT_TOP] = (matTempBuf.at<uchar>(rectTemp[i].top, rectTemp[i].left) != 0) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_TOP] = (matTempBuf.at<uchar>(rectTemp[i].top, rectTemp[i].right) != 0) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_BOTTOM] = (matTempBuf.at<uchar>(rectTemp[i].bottom, rectTemp[i].right) != 0) ? 1 : 0;
		nInside[i][E_CORNER_LEFT_BOTTOM] = (matTempBuf.at<uchar>(rectTemp[i].bottom, rectTemp[i].left) != 0) ? 1 : 0;
	}
	// 결과 확인 용
	if (nSaveImg >= 0)
	{
		cv::Mat matSaveBuf = matTempBuf.clone();

		for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
		{
			if (CHoleROI[i].bUseROI)
			{
				CRect rect = rectTemp[i];

				cv::rectangle(matSaveBuf, cv::Rect(rect.left, rect.top, rect.Width(), rect.Height()), cv::Scalar(128, 128, 128));
			}
		}
		CString strTemp;
		strTemp.Format(_T("D:\\CHole\\%02d_CHole_Coord.bmp"), nAlgImg);
		cv::imwrite((cv::String)(CStringA)strTemp, matSaveBuf);
		matSaveBuf.release();
	}

	// 곡선 외각선
	vector< cv::Point2i > ptContours[MAX_MEM_SIZE_E_INSPECT_AREA];
	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
		vector< cv::Point2i >().swap(ptContours[i]);

	// 꼭지점 개수 만큼
	for (int i = 0; i < contours.size(); i++)
	{
		if (i == nContourIdx) continue;
		for (int j = 0; j < contours[i].size(); j++)
		{
			// UI 설정한 영역 개수만큼
			for (int k = 0; k < nCHoleROICnt; k++)
			{
				// 사용 안하는 것 제외
				if (!CHoleROI[k].bUseROI)	continue;

				// 보정된 영역 안에, 해당 좌표가 있는 경우
				if (rectTemp[k].PtInRect(CPoint(contours[i][j].x, contours[i][j].y)))
				{
					// 좌표 추가
					ptContours[k].push_back(cv::Point2i(contours[i][j]));
					break;
				}
			}
		}
		// 초기화
		vector< cv::Point2i >().swap(contours[i]);
	}


	//////////////////////////////////////////////////////////////////////////
	// 순서 정렬
	// 다각형 & 내부 색상채울때, 순서대로 필요 함...
	//////////////////////////////////////////////////////////////////////////

	cv::Point2i ptTempS;
	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		for (int j = 0; j < ptContours[i].size(); j++)
		{
			for (int m = j + 1; m < ptContours[i].size(); m++)
			{
				// y좌표가 작은것 위로 오도록 정렬
				if (ptContours[i][j].y > ptContours[i][m].y)
				{
					ptTempS = ptContours[i][j];
					ptContours[i][j] = ptContours[i][m];
					ptContours[i][m] = ptTempS;
				}
				// y좌표가 같은 경우
				else if (ptContours[i][j].y == ptContours[i][m].y)
				{
					// 꼭지점에서 먼거리인 경우 변경
					if (abs(ptCorner[i].x - ptContours[i][j].x) < abs(ptCorner[i].x - ptContours[i][m].x))
					{
						ptTempS = ptContours[i][j];
						ptContours[i][j] = ptContours[i][m];
						ptContours[i][m] = ptTempS;
					}
				}
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// Save
	//////////////////////////////////////////////////////////////////////////

	char szPath[256] = { 0, };
	WideCharToMultiByte(CP_ACP, 0, strPath, -1, szPath, sizeof(szPath), NULL, NULL);

	// 파일 저장 경로
	CString str;
	str.Format(_T("%s\\CameraHole"), strPath);
	CreateDirectory(str, NULL);

	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		// 개수 없는 경우 제외
		if (ptContours[i].size() <= 0)	continue;

		//////////////////////////////////////////////////////////////////////////
		// 설정 영역이, 점등된 Cell 꼭지점에서 제일 가까운 꼭지점 찾기
		//////////////////////////////////////////////////////////////////////////

		int nCx = (rectTemp[i].left + rectTemp[i].right) / 2;
		int nCy = (rectTemp[i].top + rectTemp[i].bottom) / 2;

		// LT와의 거리
		int nLength = abs(ptCorner[E_CORNER_LEFT_TOP].x - nCx) + abs(ptCorner[E_CORNER_LEFT_TOP].y - nCy);
		int	nIndex = E_CORNER_LEFT_TOP;
		for (int j = 1; j < E_CORNER_END; j++)
		{
			int nTempLenght = abs(ptCorner[j].x - nCx) + abs(ptCorner[j].y - nCy);

			// 작은 거리인 꼭지점 찾기
			if (nLength > nTempLenght)
			{
				nLength = nTempLenght;
				nIndex = j;
			}
		}

		//////////////////////////////////////////////////////////////////////////		

		// 파일 저장 경로
		CStringA strTemp;
		strTemp.Format(("%s\\CameraHole\\%s_%02d.EdgePT"), szPath, GetPatternStringA(nAlgImg), i);

		// 파일 열기 ( 유니코드 환경 "t" -> "wt" )
		FILE *out = NULL;
		fopen_s(&out, strTemp, "wt");

		if (out != NULL)
		{
			// 가까운 인덱스 표기
			fprintf_s(out, "CameraHoleIndex%d\n", nIndex);

			// 꼭지점이 Cell 영역 안에 존재하는지 확인
			fprintf_s(out, "CameraHoleInside%d,%d,%d,%d\n", nInside[i][0], nInside[i][1], nInside[i][2], nInside[i][3]);

			for (int j = 0; j < ptContours[i].size(); j++)
			{
				// 코너를 원점 & 각도 0도
				fprintf_s(out, "%d,%d\n", ptContours[i][j].x - ptCorner[nIndex].x, ptContours[i][j].y - ptCorner[nIndex].y);
			}

			// 파일 닫기
			fclose(out);
			out = NULL;

			// KSW 17.11.14 - White인 경우 Dust패턴으로 복사
			if (nAlgImg == E_IMAGE_CLASSIFY_AVI_WHITE)
			{
				CStringA strCopy;
				strCopy.Format(("%s\\CameraHole\\%s_%02d.EdgePT"), szPath, GetPatternStringA(E_IMAGE_CLASSIFY_AVI_DUST), i);

				CopyFile((CString)strTemp, (CString)strCopy, FALSE);
			}
		}

		// 초기화
		vector< cv::Point2i >().swap(ptContours[i]);
	}
}

long CInspectAlign::GetBMCorner(cv::Mat Src, double* dAlgPara, Point* ptPanelCorner, cv::Rect& rtBMCorner)
{
	int		nGausBlurSize		= (int)dAlgPara[E_PARA_APP_POLYGON_GAUS_SIZE];
	float	fGausSigma			= (float)dAlgPara[E_PARA_APP_POLYGON_GAUS_SIGMA];
	int		nCornerROISize		= (int)dAlgPara[E_PARA_APP_POLYGON_CORNER_ROI_SIZE];					//Corner 영역 사용할 ROI 크기
	float	fBM_LT_Theta		= (float)dAlgPara[E_PARA_APP_POLYGON_BM_THETA_LT];						//BM Line Search용 기준 각도
	float	fBM_RB_Theta		= (float)dAlgPara[E_PARA_APP_POLYGON_BM_THETA_RB];						
	int		nBMIgnore			= (int)dAlgPara[E_PARA_APP_POLYGON_BM_IGNORE_GV];					//BM Line Search한 기준 각도 지점의 최소 GV 값 조건
	float	fBMThreshold_Ratio	= (float)dAlgPara[E_PARA_APP_POLYGON_BM_PRE_THRESH_RATIO];			//평균 값으로 Threshold 할때 평균 값을 어느정도 비율로 사용할 것인지에 대한 비율 크기


	Mat mtLeftTop;
	Src(Rect(ptPanelCorner[E_CORNER_LEFT_TOP].x, 
		ptPanelCorner[E_CORNER_LEFT_TOP].y, 
		nCornerROISize, nCornerROISize)).copyTo(mtLeftTop);

	Mat mtRightBottom;
	Src(Rect(ptPanelCorner[E_CORNER_RIGHT_BOTTOM].x - nCornerROISize, 
		ptPanelCorner[E_CORNER_RIGHT_BOTTOM].y - nCornerROISize, 
		nCornerROISize, nCornerROISize)).copyTo(mtRightBottom);

	GaussianBlur(mtLeftTop, mtLeftTop, Size(nGausBlurSize, nGausBlurSize), fGausSigma);
	GaussianBlur(mtRightBottom, mtRightBottom, Size(nGausBlurSize, nGausBlurSize), fGausSigma);

	Scalar mean_LT, mean_RB, std_LT, std_RB;

	cv::meanStdDev(mtLeftTop, mean_LT, std_LT);
	cv::meanStdDev(mtRightBottom, mean_RB, std_RB);

	Mat mtThreshold_LT, mtThreshold_RB;
	cv::threshold(mtLeftTop, mtThreshold_LT, mean_LT[0] * 0.8, 255, CV_THRESH_BINARY);
	cv::threshold(mtRightBottom, mtThreshold_RB, mean_RB[0] * 0.8, 255, CV_THRESH_BINARY);

	//가장 큰 블랍만 남겨 Mask로 사용
	Mat mtBigLT, mtBigRB;
	mtBigLT = Mat::zeros(mtLeftTop.size(), mtLeftTop.type());
	mtBigRB = Mat::zeros(mtLeftTop.size(), mtLeftTop.type());

	FindBiggestBlob(mtThreshold_LT, mtBigLT);
	FindBiggestBlob(mtThreshold_RB, mtBigRB);

	Mat mtMaskLT, mtMaskRB;
	mtLeftTop.copyTo(mtMaskLT, mtBigLT);
	mtRightBottom.copyTo(mtMaskRB, mtBigRB);

	vector<vector<Point>> ptLT(1), ptRB(1);

	int nDeltaX = 3;
	int nDeltaY = 3;


	for(int nY = 0; nY < mtMaskLT.rows; nY++)
	{
		uchar *ucDataLT = mtMaskLT.data + nY * mtMaskLT.step;
		uchar *ucDataRB = mtMaskRB.data + nY * mtMaskRB.step;

		for(int nX = 0; nX < mtMaskLT.cols; nX++)
		{
			if(nX + nDeltaX > mtMaskLT.cols - 1)
				continue;

			int nGV1_LT, nGV2_LT;
			nGV1_LT = (int)*(ucDataLT + nX);
			nGV2_LT = (int)*(ucDataLT + nX + nDeltaX);

			double fThetaLT = atan((float)(nGV2_LT - nGV1_LT)/nDeltaX) * 180 / CV_PI;

			if(90 > fThetaLT && fThetaLT > fBM_LT_Theta){
				if(nBMIgnore > nGV2_LT)
					continue;
				ptLT[0].push_back(Point(nX + nDeltaX, nY));
				break;
			}

		}

		for(int nX2 = mtMaskRB.cols - 1; nX2 >= 0; nX2--)
		{
			if(nX2 - nDeltaX < 0)
				continue;

			int nGV1_RB, nGV2_RB;
			nGV1_RB = (int)*(ucDataRB + nX2);
			nGV2_RB = (int)*(ucDataRB + nX2 - nDeltaX);

			double fThetaRB = atan((float)(nGV2_RB - nGV1_RB)/nDeltaX) * 180 / CV_PI;

			if(90 > fThetaRB && fThetaRB > fBM_RB_Theta){
				if(nBMIgnore > nGV2_RB)
					continue;
				ptRB[0].push_back(Point(nX2 - nDeltaX, nY));
				break;
			}
		}
	}


	for(int nX = 0; nX < mtMaskLT.cols; nX++)
	{
		uchar *ucDataLT = mtMaskLT.data + nX;
		uchar *ucDataRB = mtMaskRB.data + nX;


		for(int nY = 0; nY < mtMaskLT.rows; nY++)
		{
			if(nY + nDeltaY > mtMaskLT.rows - 1)
				continue;

			int nGV1_LT, nGV2_LT;
			nGV1_LT = (int)*(ucDataLT + nY  * mtMaskLT.step);
			nGV2_LT = (int)*(ucDataLT + (nY + nDeltaY) * mtMaskLT.step);

			double fThetaLT = (float)atan((float)(nGV2_LT - nGV1_LT)/nDeltaY) * 180 / CV_PI;


			if(90 > fThetaLT && fThetaLT > fBM_LT_Theta){
				if(nBMIgnore > nGV2_LT)
					continue;
				ptLT[0].push_back(Point(nX, nY + nDeltaY));
				break;
			}
		}

		for(int nY2 = mtMaskRB.rows; nY2 >= 0; nY2--)
		{
			if(nY2 - nDeltaY < 0)
				continue;

			int nGV1_RB, nGV2_RB;

			nGV1_RB = (int)*(ucDataRB + nY2  * mtMaskRB.step);
			nGV2_RB = (int)*(ucDataRB + (nY2 - nDeltaY) * mtMaskRB.step);

			double fThetaRB = (float)atan((float)(nGV2_RB - nGV1_RB)/nDeltaY) * 180 / CV_PI;
			if(90 > fThetaRB && fThetaRB > fBM_RB_Theta)
			{
				if(nBMIgnore > nGV2_RB)
					continue;
				ptRB[0].push_back(Point(nX, nY2 - nDeltaY));
				break;
			}
		}
	}

	//Countour 좌표 갯수가 50개 이하 이면 검사 진행 안함.
	if(ptLT[0].size() < 50 || ptRB[0].size() < 50)
		return E_ERROR_CODE_FALSE;

	cv::Rect rtLT, rtRB;
	rtLT = boundingRect(ptLT[0]);
	rtRB = boundingRect(ptRB[0]);

	rtBMCorner = Rect(Point(rtLT.tl().x + ptPanelCorner[E_CORNER_LEFT_TOP].x, 
		rtLT.tl().y + ptPanelCorner[E_CORNER_LEFT_TOP].y),
		Point(rtRB.br().x + ptPanelCorner[E_CORNER_RIGHT_BOTTOM].x - nCornerROISize,
		rtRB.br().y + ptPanelCorner[E_CORNER_RIGHT_BOTTOM].y - nCornerROISize));

	return E_ERROR_CODE_TRUE;
}

// 대략적인 Cell 위치를 찾는다.
long CInspectAlign::FindCellEdge_For_Morphology(cv::Mat matSrc, int nThreshold,cv::Rect &rcFindCellROI)
{

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//속도 향상을 위한 외곽만 모폴로지 하기위한 연산 - 모폴로지를 국부적으로 하기위한 지점을 찾는다.
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//double dblTmpSum;

	//cv::Rect rcTempFindEdgeROI;

	cv::Size szTempBufSize = matSrc.size();
	int nTempBufWidth = szTempBufSize.width;
	int nTempBufHeight = szTempBufSize.height;

	//////////////////////////////////
	//Y 축방향의 Edge를 먼저 찾는다.//
	//////////////////////////////////

	int nHalfWidth = nTempBufWidth / 2;

	cv::Mat  matTempFindEdgeYROI = matSrc(cv::Rect(nHalfWidth - (nHalfWidth / 2), 0, nHalfWidth, nTempBufHeight));
	int nROIWidth = matTempFindEdgeYROI.size().width;
	
	int nFindStart = -1;
	int nFindEnd = -1;	

#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int roop = 0; roop < 2; roop++)
	{
		if (roop == 0)
		{
			double dblTmpSum;
			//Y축 윗부분
			
			for (int i = 0; i < nTempBufHeight; i++)
			{
				dblTmpSum = cv::sum(matTempFindEdgeYROI.row(i))[0] / nROIWidth;
				if (dblTmpSum > nThreshold)
				{
					nFindStart = i;					
					break;
				}
			}
		}

		if (roop == 1)
		{
			double dblTmpSum;
			//Y축 아래 부분
			for (int i = nTempBufHeight - 1; i > 0; i--)
			{
				dblTmpSum = cv::sum(matTempFindEdgeYROI.row(i))[0] / nROIWidth;
				if (dblTmpSum > nThreshold)
				{
					nFindEnd = i;
					break;
				}
			}
		}
	}

	if (nFindStart == -1 || nFindEnd == -1) //못찾았을 경우
		return E_ERROR_CODE_ALIGN_LENGTH_RANGE_ERROR;
	
	rcFindCellROI.y = nFindStart;
	rcFindCellROI.height = nFindEnd - rcFindCellROI.y;

	/////////////////////////////
	//X 축방향의 Edge를	찾는다.//
	/////////////////////////////

	cv::Mat  matTempFindEdgeXROI = matSrc(cv::Rect(0, rcFindCellROI.y, nTempBufWidth, rcFindCellROI.height));
	int nROIHeight = matTempFindEdgeXROI.size().height;

	nFindStart = -1;
	nFindEnd = -1;

#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int roop = 0; roop < 2; roop++)
	{
		if (roop == 0)
		{
			double dblTmpSum;
			//왼쪽 찾기
			for (int i = 0; i < nTempBufWidth; i++)
			{
				dblTmpSum = cv::sum(matTempFindEdgeXROI.col(i))[0] / nROIHeight;
				if (dblTmpSum > nThreshold)
				{
					nFindStart = i;
					break;
				}
			}
		}

		if (roop == 1)
		{
			double dblTmpSum;
			//오른쪽 찾기
			for (int i = nTempBufWidth - 1; i > 0; i--)
			{
				dblTmpSum = cv::sum(matTempFindEdgeXROI.col(i))[0] / nROIHeight;
				if (dblTmpSum > nThreshold)
				{
					nFindEnd = i;
					break;
				}
			}
		}
	}

	if (nFindStart == -1 || nFindEnd == -1) //못찾았을 경우
		return E_ERROR_CODE_ALIGN_LENGTH_RANGE_ERROR;

	rcFindCellROI.x = nFindStart;
	rcFindCellROI.width = nFindEnd - rcFindCellROI.x;

	matTempFindEdgeYROI.release();
	matTempFindEdgeXROI.release();	

	return E_ERROR_CODE_TRUE;
}

void CInspectAlign::RecalRect(cv::Rect &rcRect, cv::Size szLimit)
{
	if (rcRect.x < 0)
		rcRect.x = 0;

	if (rcRect.y < 0)
		rcRect.y = 0;

	if (rcRect.width > szLimit.width - rcRect.x)
		rcRect.width = szLimit.width - rcRect.x;

	if (rcRect.height > szLimit.height - rcRect.y)
		rcRect.height = szLimit.height - rcRect.y;
}

//모폴로지를 할 영역 만들기
long CInspectAlign::MakeRoI_For_Morphology(cv::Rect rcFindCellROI,
										   int nExtROI_Outer, int nExtROI_Inner_L, int nExtROI_Inner_R, int nExtROI_Inner_T, int nExtROI_Inner_B,
										   cv::Size rcLimit, cv::Rect  *prcMorpROI)
{
	//Cell 바깥으로는 Outer
	//Cell 안쪽으로는 Inner

	int nAddInspArea = 100; // 중복 검사 영역

	//Left
	prcMorpROI[0].x = rcFindCellROI.x - nExtROI_Outer;
	prcMorpROI[0].y = rcFindCellROI.y - nExtROI_Outer;
	prcMorpROI[0].width = nExtROI_Inner_L + nExtROI_Outer;
	prcMorpROI[0].height = rcFindCellROI.height + (nExtROI_Outer*2);

	//right
	prcMorpROI[1].x = rcFindCellROI.x + rcFindCellROI.width - nExtROI_Inner_R;
	prcMorpROI[1].y = rcFindCellROI.y - nExtROI_Outer;
	prcMorpROI[1].width = nExtROI_Inner_R + nExtROI_Outer;
	prcMorpROI[1].height = rcFindCellROI.height + (nExtROI_Outer * 2);

	//top
	prcMorpROI[2].x = (prcMorpROI[0].x+ prcMorpROI[0].width) - nAddInspArea;	// Left 데이터 참조 - 150 pxl 만큼 중복 모폴로지
	prcMorpROI[2].y = prcMorpROI[0].y;			// Left 데이터 참조
	prcMorpROI[2].width = prcMorpROI[1].x - prcMorpROI[2].x + nAddInspArea;	// Right 데이터 참조 - 150 pxl 만큼 중복 모폴로지
	prcMorpROI[2].height = nExtROI_Inner_T + nExtROI_Outer;

	//bottom
	prcMorpROI[3].x = (prcMorpROI[0].x + prcMorpROI[0].width) - nAddInspArea;		// Left 데이터 참조 - 150 pxl 만큼 중복 모폴로지
	prcMorpROI[3].y = rcFindCellROI.y+ rcFindCellROI.height - nExtROI_Inner_B;
	prcMorpROI[3].width = prcMorpROI[1].x - prcMorpROI[3].x + nAddInspArea;	// Right 데이터 참조 - 150 pxl 만큼 중복 모폴로지
	prcMorpROI[3].height = nExtROI_Inner_B + nExtROI_Outer;

	//범위 예외 처리
	RecalRect(prcMorpROI[0], rcLimit);
	RecalRect(prcMorpROI[1], rcLimit);
	RecalRect(prcMorpROI[2], rcLimit);
	RecalRect(prcMorpROI[3], rcLimit);
	
	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::Partial_Morphology(cv::Mat matSrc, cv::Mat matDst, int nMorpType, cv::Mat StructElem, cv::Rect  *prcMorpROI)
{
	//모폴로지를 입력된 구역별로 4번 해줘야 한다. - 상하 동시, 좌우 동시 병렬 처리		

	cv::Mat matTmpROI_Src_L = matSrc(prcMorpROI[0]);
	cv::Mat matTmpROI_Src_R = matSrc(prcMorpROI[1]);
	cv::Mat matTmpROI_Src_T = matSrc(prcMorpROI[2]);
	cv::Mat matTmpROI_Src_B = matSrc(prcMorpROI[3]);

	cv::Mat matTmpROI_Dst_L = matDst(prcMorpROI[0]);
	cv::Mat matTmpROI_Dst_R = matDst(prcMorpROI[1]);
	cv::Mat matTmpROI_Dst_T = matDst(prcMorpROI[2]);
	cv::Mat matTmpROI_Dst_B = matDst(prcMorpROI[3]);

	//왼쪽 오른쪽 모폴로지 - 병렬 처리로 같이 한다.
#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int roop = 0; roop < 2; roop++)
	{
		if (roop == 0)
		{
			//왼쪽 모폴로지
			cv::morphologyEx(matTmpROI_Src_L, matTmpROI_Dst_L, nMorpType, StructElem);
		}

		if (roop == 1)
		{
			//오른쪽 모폴로지
			cv::morphologyEx(matTmpROI_Src_R, matTmpROI_Dst_R, nMorpType, StructElem);
		}
	}	

	//위쪽 아래쪽 모폴로지 - 병렬 처리로 같이 한다.
#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int roop = 0; roop < 2; roop++)
	{
		if (roop == 0)
		{
			//왼쪽 모폴로지
			cv::morphologyEx(matTmpROI_Src_T, matTmpROI_Dst_T, nMorpType, StructElem);
		}

		if (roop == 1)
		{
			//오른쪽 모폴로지
			cv::morphologyEx(matTmpROI_Src_B, matTmpROI_Dst_B, nMorpType, StructElem);
		}
	}

	matTmpROI_Src_L.release();
	matTmpROI_Src_R.release();
	matTmpROI_Src_T.release();
	matTmpROI_Src_B.release();

	matTmpROI_Dst_L.release();
	matTmpROI_Dst_R.release();
	matTmpROI_Dst_T.release();
	matTmpROI_Dst_B.release();

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::Partial_Laplacian(cv::Mat matSrc, cv::Mat matDst, cv::Rect  *prcMorpROI)
{
	//모폴로지를 입력된 구역별로 4번 해줘야 한다. - 상하 동시, 좌우 동시 병렬 처리		

	cv::Mat matTmpROI_Src_L = matSrc(prcMorpROI[0]);
	cv::Mat matTmpROI_Src_R = matSrc(prcMorpROI[1]);
	cv::Mat matTmpROI_Src_T = matSrc(prcMorpROI[2]);
	cv::Mat matTmpROI_Src_B = matSrc(prcMorpROI[3]);

	cv::Mat matTmpROI_Dst_L = matDst(prcMorpROI[0]);
	cv::Mat matTmpROI_Dst_R = matDst(prcMorpROI[1]);
	cv::Mat matTmpROI_Dst_T = matDst(prcMorpROI[2]);
	cv::Mat matTmpROI_Dst_B = matDst(prcMorpROI[3]);

	//왼쪽 오른쪽 모폴로지 - 병렬 처리로 같이 한다.
#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int roop = 0; roop < 2; roop++)
	{
		if (roop == 0)
		{
			//왼쪽 Laplacian
			cv::Laplacian(matTmpROI_Src_L, matTmpROI_Dst_L,CV_8U);
		}

		if (roop == 1)
		{
			//오른쪽 Laplacian
			cv::Laplacian(matTmpROI_Src_R, matTmpROI_Dst_R, CV_8U);
		}
	}

	//위쪽 아래쪽 모폴로지 - 병렬 처리로 같이 한다.
#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int roop = 0; roop < 2; roop++)
	{
		if (roop == 0)
		{
			//왼쪽 Laplacian
			cv::Laplacian(matTmpROI_Src_T, matTmpROI_Dst_T, CV_8U);
		}

		if (roop == 1)
		{
			//오른쪽 Laplacian
			cv::Laplacian(matTmpROI_Src_B, matTmpROI_Dst_B, CV_8U);
		}
	}
	
	matTmpROI_Src_L.release();
	matTmpROI_Src_R.release();
	matTmpROI_Src_T.release();
	matTmpROI_Src_B.release();

	matTmpROI_Dst_L.release();
	matTmpROI_Dst_R.release();
	matTmpROI_Dst_T.release();
	matTmpROI_Dst_B.release();

	return E_ERROR_CODE_TRUE;
}

// 외곽 부분 처리
long CInspectAlign::DoFillOutArea(cv::Mat& matSrcBuf, cv::Mat& matBKGBuf, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], ROUND_SET tCHoleSet[MAX_MEM_SIZE_E_INSPECT_AREA], cv::Mat* matCHoleROIBuf, cv::Rect* rcCHoleROI, bool* bCHoleAD, double* dPara, int nAlgImg, int nRatio, wchar_t* strID)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));
	// 2021.02.24 HGM G3 관련 Test
	// G3 패턴 제외 (2021.02.24 기준 G128을 G3패턴으로 사용중)
	if (nAlgImg == E_IMAGE_CLASSIFY_AVI_GRAY_128)		return E_ERROR_CODE_TRUE;

	// Black 패턴 제외
	if( nAlgImg == E_IMAGE_CLASSIFY_AVI_BLACK )		return E_ERROR_CODE_TRUE;

	// PCD 패턴 제외
	if (nAlgImg == E_IMAGE_CLASSIFY_AVI_PCD)		return E_ERROR_CODE_TRUE;
	// KSW 17.11.14 - Dust 패턴 -> DoFillOutAreaDust() 사용
	// Round인 경우 배경 올라올 경우 존재
	if( nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST )		return E_ERROR_CODE_TRUE;

	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )							return E_ERROR_CODE_EMPTY_BUFFER;

	// -1 : 파일 저장 안함
	// 0 : 파일 저장
	int nSaveImageCount = -1;
	 
	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples		= 3;	// 고정
	double	distThreshold	= 10;	// 고정

	int		nMinArea		= (int)(dPara[E_PARA_CELL_SIZE_X] * dPara[E_PARA_CELL_SIZE_Y] * nRatio * nRatio);
	int		nThreshold		= dPara[E_PARA_ALIGN_THRESHOLD];
	int		nMorp			= dPara[E_PARA_ALIGN_MORP];
	double	dAngleError		= dPara[E_PARA_ALIGN_ANGLE_ERR];
	double	dAngleWarning	= dPara[E_PARA_ALIGN_ANGLE_WAR];

	// Round 영역 찾는 범위 설정 ( 상,하,좌,우 )
	int		nFindRoundOffset = (int)dPara[E_PARA_ROUND_FIND_OFFSET];
	int		nFindCHoleOffset = (int)dPara[E_PARA_CHOLE_FIND_OFFSET];

	// 외곽 평균 채울때, 최소 평균 GV 설정
	// Round & CHole 영역 외곽 평균 채울때, 최소 평균 GV 설정
	int		nRoundMinGV = (int)dPara[E_PARA_ROUND_OTHER_MIN_GV];
	int		nCHoleMinGV = (int)dPara[E_PARA_CHOLE_ROI_MIN_GV];

	// Round & CHole Cell 사용 유/무
	bool	bRoundUse = (dPara[E_PARA_ROUND_USE] > 0) ? true : false;
	bool	bCHoleUse = (dPara[E_PARA_CHOLE_USE] > 0) ? true : false;

	// Round & CHole Cell 안쪽으로 몇 Pixel 들어갈지.... ( 곡선 부분만... )
	int		nRoundIn = (int)(dPara[E_PARA_ROUND_IN]);
	int		nCHoleIn = (int)(dPara[E_PARA_CHOLE_IN]);

	nCHoleIn = nCHoleIn * 2 + 1;

	int nShiftX = 0;
	int nShiftY = 0;

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// ShiftCopy
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matDstBuf = cMem->GetMat(matSrcBuf.size(), matSrcBuf.type());
	// 파라미터 추가 필요

	// 영역 안쪽으로 살짝 축소... ( 영역 )
	int nInPixel = 3;

	// PS 모드인 경우
	if( nRatio != 1 )
	{
		// Shift Copy Parameter 받아 오기
		int		nRedPattern		=	(int)dPara[E_PARA_SHIFT_COPY_R];
		int		nGreenPattern	=	(int)dPara[E_PARA_SHIFT_COPY_G];
		int		nBluePattern	=	(int)dPara[E_PARA_SHIFT_COPY_B];

		int nCpyX=0, nCpyY=0, nLoopX=0, nLoopY=0;

		// 패턴별...
		switch( nAlgImg )
		{
		case E_IMAGE_CLASSIFY_AVI_R:
			{
				if ( nRedPattern == 0 ) break;
				ShiftCopyParaCheck(nRedPattern, nCpyX, nCpyY, nLoopX, nLoopY);
				nErrorCode = ShiftCopy(matSrcBuf, matDstBuf, nCpyX, nCpyY, nLoopX, nLoopY);
				matDstBuf.copyTo(matSrcBuf);
				matDstBuf.release();
			}
			break;

		case E_IMAGE_CLASSIFY_AVI_G:
			{
				if ( nGreenPattern == 0 ) break;
				ShiftCopyParaCheck(nGreenPattern, nCpyX, nCpyY, nLoopX, nLoopY);
				nErrorCode = ShiftCopy(matSrcBuf, matDstBuf, nCpyX, nCpyY, nLoopX, nLoopY);
				matDstBuf.copyTo(matSrcBuf);
				matDstBuf.release();
			}
			break;

		case E_IMAGE_CLASSIFY_AVI_B:
			{
				if ( nBluePattern == 0 ) break;
				ShiftCopyParaCheck(nBluePattern, nCpyX, nCpyY, nLoopX, nLoopY);
				nErrorCode = ShiftCopy(matSrcBuf, matDstBuf, nCpyX, nCpyY, nLoopX, nLoopY);
				matDstBuf.copyTo(matSrcBuf);
				matDstBuf.release();
			}
			break;

		default:
			break;
		}

		nShiftX = nCpyX;
		nShiftY = nCpyY;

		if (nShiftX % 2 == 1) nShiftX += 1;
		if (nShiftY % 2 == 1) nShiftY += 1;

	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("ShiftCopy."));

	if(nSaveImageCount >= 0)
	{
		CString strTemp;
		strTemp.Format(_T("d:\\%02d_%02d_ShiftCopy.jpg"), nAlgImg, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matSrcBuf);
	}

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		return nErrorCode;
	}

	//////////////////////////////////////////////////////////////////////////
	// AVI의 경우만 사용, Cell 배열 사이 빈공간 채우는 용도
	//////////////////////////////////////////////////////////////////////////

	cv::Mat matSrc_8bit = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	if( matSrcBuf.type() == CV_8UC1 )
		matSrcBuf.copyTo(matSrc_8bit);
	else
		matSrcBuf.convertTo(matSrc_8bit, CV_8UC1, 1./16.);

	cv::Mat matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1);


	//////////////////////////////////////////////////////////////////////////////
	//속도 향상을 위한 외곽만 모폴로지 하기위한 연산 - 모폴로지를 국부적으로 하기위한 지점을 찾는다.
	//////////////////////////////////////////////////////////////////////////////

	bool bUsePartialMorp 		  =((int)dPara[E_PARA_ALIGN_PARTIAL_USE] > 0)? true : false;		//= true;
	int nFindCellROI_Threshold 	  = (int)dPara[E_PARA_ALIGN_PARTIAL_THRESHOLD];						//= 10;
	int nExtROI_Outer 			  = (int)dPara[E_PARA_ALIGN_PARTIAL_OUTER];							//= 200;
	int nExtROI_Inner_L 		  = (int)dPara[E_PARA_ALIGN_PARTIAL_LEFT_OFFSET];					//= 800;
	int nExtROI_Inner_R 		  = (int)dPara[E_PARA_ALIGN_PARTIAL_RIGHT_OFFSET];					//= 800;
	int nExtROI_Inner_T 		  = (int)dPara[E_PARA_ALIGN_PARTIAL_TOP_OFFSET];					//= 300;
	int nExtROI_Inner_B 		  = (int)dPara[E_PARA_ALIGN_PARTIAL_BOTTOM_OFFSET];					//= 300;

	cv::Rect rcFindCellROI;
	cv::Rect rcMorpROI[4]; // 0 : Left , 1 : Right , 2 : Top , 3 : Bottom
	
	bool bFindPartialMorpROI = false;	

	//국부 모폴로지로 사용 - 나중에 watch 는 끄고 해야 될듯
	if (bUsePartialMorp)
	{
		cv::Mat matFindCellEdge_For_Morphology = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
		bFindPartialMorpROI = true;
		
		cv::threshold(matSrc_8bit, matFindCellEdge_For_Morphology, nThreshold, 255.0, THRESH_BINARY); // 찾기 쉽게 이진화해서 찾는다.

		nErrorCode = FindCellEdge_For_Morphology(matFindCellEdge_For_Morphology, nFindCellROI_Threshold, rcFindCellROI);

		// 에러인 경우, 에러 코드 출력대신 국부 모폴로지를 실행하지 않고 전체 모폴로지를 실행한다.
		if (nErrorCode != E_ERROR_CODE_TRUE ||
			dPara[E_PARA_CELL_SIZE_X] * nRatio > rcFindCellROI.width ||
			dPara[E_PARA_CELL_SIZE_Y] * nRatio > rcFindCellROI.height)
		{
			bFindPartialMorpROI = false;
		}

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Find Morphology ROI"));

		if (nSaveImageCount >= 0)
		{
			CString strTemp;
			cv::Mat matSaveTmpROI = matFindCellEdge_For_Morphology(rcFindCellROI);
			strTemp.Format(_T("d:\\%02d_%02d_FindROI.jpg"), nAlgImg, nSaveImageCount++);
			cv::imwrite((cv::String)(CStringA)strTemp, matSaveTmpROI);
			matSaveTmpROI.release();
		}

		//모폴로지를 할 영역 만들기 - 영역 찾기 성공 시
		if (bFindPartialMorpROI)
		{
			//모폴로지 영역을 left, right, top, bottom 영역으로 분할하여 만들어준다.
			MakeRoI_For_Morphology(rcFindCellROI, nExtROI_Outer, nExtROI_Inner_L, nExtROI_Inner_R, nExtROI_Inner_T, nExtROI_Inner_B, matSrcBuf.size(), rcMorpROI);		

			//외곽 영역만 모폴로지를 할것이기 때문에 중간영역은 그냥 회색으로 채워줌
			// => 나중에 Thresholding 후  Edge만 사용할 것임으로 중간 영역은 필요없는 지역임
			cv::Rect rcTempDrawRect;

			rcTempDrawRect.x = rcMorpROI[0].x + rcMorpROI[0].width;
			rcTempDrawRect.y = rcMorpROI[2].y + rcMorpROI[2].height;
			rcTempDrawRect.width = rcMorpROI[1].x - rcTempDrawRect.x;
			rcTempDrawRect.height = rcMorpROI[3].y - rcTempDrawRect.y;

			cv::Mat matTmpDrawRect = matTempBuf(rcTempDrawRect); // TempBuf에 칠해줘야 제대로된 목적지에 칠해 진다.

			// Cell 중앙 Camera Hole을 찾기위한 원본 복사

			//matSrcBuf(rcTempDrawRect).copyTo(matTmpDrawRect);

			matTmpDrawRect.setTo(128); // 중간 영역은 128 GV 로 칠해준다.

			matTmpDrawRect.release();
		}

		// 모폴로지 ROI 결과 확인 용
		if (nSaveImageCount >= 0)
		{
			cv::Mat matColor;
			cv::cvtColor(matSrc_8bit, matColor, COLOR_GRAY2RGB);

			cv::rectangle(matColor, rcMorpROI[0], cv::Scalar(255, 0, 0)); // Left
			cv::rectangle(matColor, rcMorpROI[1], cv::Scalar(255, 0, 0)); // Right

			cv::rectangle(matColor, rcMorpROI[2], cv::Scalar(0, 255, 0)); // Top
			cv::rectangle(matColor, rcMorpROI[3], cv::Scalar(0, 255, 0)); // Bottom

			CString strTemp;
			strTemp.Format(_T("d:\\%02d_%02d_MorpROI_Rect.jpg"), nAlgImg, nSaveImageCount++);
			cv::imwrite((cv::String)(CStringA)strTemp, matColor);

			matColor.release();
		}
		
		matFindCellEdge_For_Morphology.release();
	}

	///////////////////////////////////////////////////////////////////////////////

	if( nMorp > 0 )
	{
		cv::Mat	StructElem = cv::getStructuringElement(MORPH_RECT, Size(nMorp, nMorp), cv::Point(nMorp / 2, nMorp / 2));

		//부분 모폴로지 관련 사용
		if (bUsePartialMorp && bFindPartialMorpROI)
		{
			Partial_Morphology(matSrc_8bit, matTempBuf, MORPH_CLOSE, StructElem, rcMorpROI); // 부분 모폴로지를 사용한다.
		}
		else
		{	
			// Morphology Close ( Cell 사이 빈공간 채우기 )
			cv::morphologyEx(matSrc_8bit, matTempBuf, MORPH_CLOSE, StructElem);			
		}
		StructElem.release();
	}
	else
		matSrc_8bit.copyTo(matTempBuf);

	if (nSaveImageCount >= 0)
	{
		CString strTemp;
		strTemp.Format(_T("d:\\%02d_%02d_Morp1.jpg"), nAlgImg, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matTempBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Morphology 1."));



	//////////////////////////////////////////////////////////////////////////
	// Camera Hole 찾기
	// Morphology2 사용할 시 Camera Hole로 인한 Contour 이상 발생
	// Morphology2 하기 전 Camera Hole 내부 채우는 목적
	// Morphology1 -> Threshold -> 코너좌표 획득 -> 각도 & 위치 보정 -> CHole Find -> CHole 채우기 ( 원본 )
	// Parallel Morphology ROI에 Camera Hole 있어야함
	//////////////////////////////////////////////////////////////////////////

	if (bCHoleUse)
	{
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("CHole Fill Area Start."));

		cv::Mat matCHoleBuf;
		cv::Mat matCHoleBuf2;
		cv::Mat matCHoleBuf3;

		// Image Save 0 On / -1 Off
		//int nImgSave = -1;
		int nImgSave = (int)dPara[E_PARA_CHOLE_TEXT];

		matCHoleBuf = cMem->GetMat(matTempBuf.size(), CV_8UC1, false);
		matCHoleBuf2 = cMem->GetMat(matTempBuf.size(), CV_8UC1, false);
		matCHoleBuf3 = cMem->GetMat(matTempBuf.size(), CV_8UC1);
		cv::threshold(matTempBuf, matCHoleBuf, nThreshold, 255, THRESH_BINARY);

		if (nImgSave >= 1)
		{
			CString strTemp;
			strTemp.Format(_T("D:\\CHole\\%02d_%02d_CHole Thres.bmp"), nAlgImg, nImgSave++);
			cv::imwrite((cv::String)(CStringA)strTemp, matCHoleBuf);
		}

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Threshold."));

		cv::Laplacian(matCHoleBuf, matCHoleBuf2, CV_8UC1);

		if (nImgSave >= 1)
		{
			CString strTemp;
			strTemp.Format(_T("D:\\CHole\\%02d_%02d_CHole Raplacian.bmp"), nAlgImg, nImgSave++);
			cv::imwrite((cv::String)(CStringA)strTemp, matCHoleBuf2);
		}

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Laplacian."));

		//////////////////////////////////////////////////////////////////////////
		// 점등 영역 찾기
		//////////////////////////////////////////////////////////////////////////	

		// 전체 외각선
		vector< vector< cv::Point2i > > contours;
		cv::findContours(matCHoleBuf2, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

		int nMaxIndex = 0;
		double dMaxSize = 0.0;
		for (int i = 0; i < (int)contours.size(); i++)
		{
			double dValue = cv::contourArea(contours[i]);
			if (dValue > dMaxSize)
			{
				dMaxSize = dValue;
				nMaxIndex = i;
			}
		}

		cv::RotatedRect BoundingBox = cv::minAreaRect(contours[nMaxIndex]);

		cv::Point2f vertices[E_CORNER_END];
		BoundingBox.points(vertices);

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Active Area Find."));

		//////////////////////////////////////////////////////////////////////////
		// 코너 꼭지점 4개 찾기
		//////////////////////////////////////////////////////////////////////////

		cv::Point ptResCorner[E_CORNER_END], ptCornerAlign[E_CORNER_END];

		long	nWidth = (long)matSrcBuf.cols;	// 영상 가로 크기
		long	nHeight = (long)matSrcBuf.rows;	// 영상 세로 크기	

		nErrorCode = FindCornerPoint(vertices, ptResCorner, nWidth, nHeight);

		// 에러인 경우, 에러 코드 출력
		if (nErrorCode != E_ERROR_CODE_TRUE)
			return nErrorCode;

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Find 4-Corner."));

		if (nImgSave >= 1)
		{
			cv::Mat matColor;
			cv::cvtColor(matSrcBuf, matColor, COLOR_GRAY2RGB);

			cv::line(matColor, ptResCorner[0], ptResCorner[1], cv::Scalar(255, 0, 0));
			cv::line(matColor, ptResCorner[1], ptResCorner[2], cv::Scalar(255, 0, 0));
			cv::line(matColor, ptResCorner[2], ptResCorner[3], cv::Scalar(255, 0, 0));
			cv::line(matColor, ptResCorner[3], ptResCorner[0], cv::Scalar(255, 0, 0));

			CString strTemp;
			strTemp.Format(_T("D:\\CHole\\%02d_%02d_ROI_Rect.bmp"), nAlgImg, nImgSave++);
			cv::imwrite((cv::String)(CStringA)strTemp, matColor);

			matColor.release();
		}

		//////////////////////////////////////////////////////////////////////////
		// 기울기 보정 했을 때, 꼭지점 좌표 얻기
		// 등록된 곡선 & ROI 영역은 꼭지점 좌표 기준
		//////////////////////////////////////////////////////////////////////////

		// 회전 좌표 계산 시, 사용
		double	dTheta = -BoundingBox.angle;

		// 예외처리
		if (45.0 < dTheta && dTheta < 135.0)	dTheta -= 90.0;
		if (-45.0 > dTheta && dTheta > -135.0)	dTheta += 90.0;

		dTheta *= PI;
		dTheta /= 180.0;
		double	dSin = sin(dTheta);
		double	dCos = cos(dTheta);
		double	dSin_ = sin(-dTheta);
		double	dCos_ = cos(-dTheta);
		int		nCx = matSrcBuf.cols / 2;
		int		nCy = matSrcBuf.rows / 2;

		for (int i = 0; i < E_CORNER_END; i++)
		{
			// 회전 시, 예측 좌표 계산
			ptCornerAlign[i].x = (int)(dCos * (ptResCorner[i].x - nCx) - dSin * (ptResCorner[i].y - nCy) + nCx);
			ptCornerAlign[i].y = (int)(dSin * (ptResCorner[i].x - nCx) + dCos * (ptResCorner[i].y - nCy) + nCy);
		}

		//////////////////////////////////////////////////////////////////////////
		// 등록된 곡선 & ROI 영역 -> 현재 Grab에 맞게 좌표 보정
		//////////////////////////////////////////////////////////////////////////

		// 재계산
		dTheta = BoundingBox.angle;

		// 예외처리
		if (45.0 < dTheta && dTheta < 135.0)	dTheta -= 90.0;
		if (-45.0 > dTheta && dTheta > -135.0)	dTheta += 90.0;

		dTheta *= PI;
		dTheta /= 180.0;
		dSin = sin(dTheta);
		dCos = cos(dTheta);
		dSin_ = sin(-dTheta);
		dCos_ = cos(-dTheta);

		//////////////////////////////////////////////////////////////////////////
		// 등록된 곡선 & ROI 영역 위치찾기
		//////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
		{
			// 곡선 개수 없으면, 영역 등록 안된것으로 판단 / 다음으로...
			if (tCHoleSet[i].nContourCount <= 0)	continue;

			// 좌,우,상,하 움직여서, 제일 편차 큰 곳 찾기 ( 상하좌우 - pixel )
			const int nArrSize = (nFindCHoleOffset * 2 + 1) * (nFindCHoleOffset * 2 + 1);
			__int64 *nSum = (__int64 *)malloc(sizeof(__int64) * nArrSize);
			memset(nSum, 0, sizeof(__int64) * nArrSize);
			cv::Point2i ptTemp;

			// Align 꼭지점에서 제일 가까운 꼭지점
			int nCornerAlign = tCHoleSet[i].nCornerMinLength;

			const int nCount = (int)tCHoleSet[i].nContourCount;
			cv::Point *ptPoly = new cv::Point[nCount];
			memset(ptPoly, 0, sizeof(cv::Point) * nCount);

			int nMinX = matCHoleBuf.cols, nMaxX = 0;
			int nMinY = matCHoleBuf.rows, nMaxY = 0;

			//////////////////////////////////////////////////////////////////////////
			// 얼라인 좌표 보정
			//////////////////////////////////////////////////////////////////////////

			// 곡선 개수 존재하는 경우
			for (int j = 0; j < tCHoleSet[i].nContourCount; j++)
			{
				// 얼라인 좌표부터, 곡선 설정 영역까지 최단 거리 보정
				// 장비 & 스테이지마다 편차 존재
				int XX = tCHoleSet[i].ptContours[j].x + ptCornerAlign[nCornerAlign].x;
				int YY = tCHoleSet[i].ptContours[j].y + ptCornerAlign[nCornerAlign].y;

				// 회전 시, 예측 좌표 계산
				ptTemp.x = (int)(dCos * (XX - nCx) - dSin * (YY - nCy) + nCx);
				ptTemp.y = (int)(dSin * (XX - nCx) + dCos * (YY - nCy) + nCy);

				// 보정된 좌표 넣기
				ptPoly[j].x = ptTemp.x;
				ptPoly[j].y = ptTemp.y;

				//matCHoleBuf3.at<uchar>(ptPoly[j].y, ptPoly[j].x) = (uchar)255;

				if (nMinX > ptPoly[j].x) nMinX = ptPoly[j].x;
				if (nMaxX < ptPoly[j].x) nMaxX = ptPoly[j].x;
				if (nMinY > ptPoly[j].y) nMinY = ptPoly[j].y;
				if (nMaxY < ptPoly[j].y) nMaxY = ptPoly[j].y;
			}

			//////////////////////////////////////////////////////////////////////////
			// CHole ROI 최소 GV Check
			//////////////////////////////////////////////////////////////////////////

			int nMeanROIOffset = 40;
			rcCHoleROI[i].x = nMinX - nMeanROIOffset;
			rcCHoleROI[i].y = nMinY - nMeanROIOffset;
			rcCHoleROI[i].width = (nMaxX - nMinX + 1) + nMeanROIOffset * 2;
			rcCHoleROI[i].height = (nMaxY - nMinY + 1) + nMeanROIOffset * 2;

			// Pattern 별 원본영상 GV 차이 -> 이진화 영상에서 비교 ( CHole Area 크기 확인?? )
			cv::Scalar scrSrcMeanGV;
			scrSrcMeanGV = cv::mean(matCHoleBuf(rcCHoleROI[i]));

			// 최소 GV보다 낮을 시 CHole AD 불량
			if (scrSrcMeanGV[0] < nCHoleMinGV) bCHoleAD[i] = true;

			// Log
			wchar_t wcLogTemp[MAX_PATH] = { 0 };
			swprintf_s(wcLogTemp, _T("CHole Mean (%.5f)"), scrSrcMeanGV[0]);
			writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, wcLogTemp);

			//////////////////////////////////////////////////////////////////////////
			// CHole 찾기
			//////////////////////////////////////////////////////////////////////////

			// 곡선 개수 존재하는 경우
			for (int j = 0; j < tCHoleSet[i].nContourCount; j++)
			{
				// 좌,우,상,하 움직여서, 제일 편차 큰 곳 찾기
				int m = 0;
				for (int y = -nFindCHoleOffset; y <= nFindCHoleOffset; y++)
				{
					for (int x = -nFindCHoleOffset; x <= nFindCHoleOffset; x++, m++)
					{
						int indexX = ptPoly[j].x + x;
						int indexY = ptPoly[j].y + y;

						// 예외처리
						if (indexX < 0)						continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if (indexY < 0)						continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if (indexX >= matCHoleBuf.cols)		continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if (indexY >= matCHoleBuf.rows)		continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;

						nSum[m] += matCHoleBuf2.at<uchar>(indexY, indexX);
					}
				}
			}
			// 좌,우,상,하 움직여서, 제일 편차 큰 곳 찾기
			long nMax = 0;
			int m = 0;
			int xx = 0;
			int yy = 0;
			for (int y = -nFindCHoleOffset; y <= nFindCHoleOffset; y++)
			{
				for (int x = -nFindCHoleOffset; x <= nFindCHoleOffset; x++, m++)
				{
					if (nMax < nSum[m])
					{
						nMax = nSum[m];
						xx = x;
						yy = y;
					}
				}
			}

			free(nSum);
			nSum = NULL;

			nMinX = matCHoleBuf.cols;
			nMinY = matCHoleBuf.rows;
			nMaxX = 0;
			nMaxY = 0;

			// 위치 수정
			for (int j = 0; j < tCHoleSet[i].nContourCount; j++)
			{
				ptPoly[j].x += xx;
				ptPoly[j].y += yy;

				// 예외처리
				if (ptPoly[j].x < 0)					return E_ERROR_CODE_ALIGN_IMAGE_OVER;
				if (ptPoly[j].y < 0)					return E_ERROR_CODE_ALIGN_IMAGE_OVER;
				if (ptPoly[j].x >= matCHoleBuf2.cols)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
				if (ptPoly[j].y >= matCHoleBuf2.rows)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

				if (nMinX > ptPoly[j].x) nMinX = ptPoly[j].x;
				if (nMaxX < ptPoly[j].x) nMaxX = ptPoly[j].x;
				if (nMinY > ptPoly[j].y) nMinY = ptPoly[j].y;
				if (nMaxY < ptPoly[j].y) nMaxY = ptPoly[j].y;

				matCHoleBuf3.at<uchar>(ptPoly[j].y, ptPoly[j].x) = (uchar)255;
			}

			delete ptPoly;
			ptPoly = NULL;

			writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Find CHole."));

			//////////////////////////////////////////////////////////////////////////
			// 영상 CHole 채우기
			//////////////////////////////////////////////////////////////////////////

			int nCHoleROIOffSet = 40 + nCHoleIn;

			cv::Rect rectCHoleROI;
			rectCHoleROI.x = nMinX - nCHoleROIOffSet;
			rectCHoleROI.y = nMinY - nCHoleROIOffSet;
			rectCHoleROI.width = (nMaxX - nMinX + 1) + nCHoleROIOffSet * 2;
			rectCHoleROI.height = (nMaxY - nMinY + 1) + nCHoleROIOffSet * 2;

			// 원본영상
			cv::Mat matsrcCHoleROI = matSrcBuf(rectCHoleROI);

			// 이진화 영상 (평균 Mask)
			cv::Mat matCHoleROI = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type(), false);

			// 원본 이진화 영상
			cv::Mat matSrcCHoleBuf = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type(), false);

			// CHole 채울 평균 영상
			cv::Mat matCHoleFill = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type());
			cv::Mat matCHoleFillX = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type());
			cv::Mat matCHoleFillY = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type());
			
			// CHole 좌표 영상
			cv::Mat matCHoleFillBuf = matCHoleBuf3(rectCHoleROI);
			// 외각선 연결 & CHoleIn
			if (nCHoleIn >= 3)
			{
				cv::Mat	StructElem = cv::getStructuringElement(MORPH_CROSS, cv::Size(nCHoleIn + nShiftX, nCHoleIn + nShiftY));
				cv::morphologyEx(matCHoleFillBuf, matCHoleFillBuf, MORPH_DILATE, StructElem);
				StructElem.release();
			}
			else
			{
				cv::Mat	StructElem = cv::getStructuringElement(MORPH_CROSS, cv::Size(3, 3));
				cv::morphologyEx(matCHoleFillBuf, matCHoleFillBuf, MORPH_CLOSE, StructElem);
				StructElem.release();
			}

			cv::floodFill(matCHoleFillBuf, cv::Point(matCHoleFillBuf.cols / 2, matCHoleFillBuf.rows / 2), cv::Scalar(255));

			writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("CHoleIn."));

			//Pattern 별 AD 가 아닐때
			if (!bCHoleAD[i])
			{
				nCHoleROIOffSet = 40;
				rcCHoleROI[i].x = nMinX - nCHoleROIOffSet;
				rcCHoleROI[i].y = nMinY - nCHoleROIOffSet;
				rcCHoleROI[i].width = (nMaxX - nMinX + 1) + nCHoleROIOffSet * 2;
				rcCHoleROI[i].height = (nMaxY - nMinY + 1) + nCHoleROIOffSet * 2;

				matCHoleROIBuf[i] = matCHoleBuf3(rcCHoleROI[i]).clone();
			}

			// CHole 주변 마스크 생성
			cv::blur(matCHoleFillBuf, matCHoleROI, cv::Size(61, 61));
			cv::subtract(matCHoleROI, matCHoleFillBuf, matCHoleROI);
			cv::threshold(matCHoleROI, matCHoleROI, 0.0, 255.0, THRESH_BINARY);

			cv::threshold(matsrcCHoleROI, matSrcCHoleBuf, nThreshold, 255.0, THRESH_BINARY);

			cv::Mat	StructElem = cv::getStructuringElement(MORPH_RECT, cv::Size(5, 5));
			cv::morphologyEx(matSrcCHoleBuf, matSrcCHoleBuf, MORPH_CLOSE, StructElem);
			StructElem.release();

			cv::bitwise_and(matCHoleBuf(rectCHoleROI), matCHoleROI, matCHoleROI);

			cv::bitwise_and(matSrcCHoleBuf, matCHoleROI, matCHoleROI);

			// 원본 CHole 채우기 가로
			for (int nY = 2; nY < matsrcCHoleROI.rows - 2; nY += 3)
			{
				// 원본 영상
				cv::Mat matsrcFillROI = matsrcCHoleROI(cv::Rect(0, nY - 2, matsrcCHoleROI.cols, 5));
				// Mask
				cv::Mat matCHoleFillROI1 = matCHoleROI(cv::Rect(0, nY - 2, matsrcCHoleROI.cols, 5));

				cv::Scalar CHoleMeanGV = cv::mean(matsrcFillROI, matCHoleFillROI1);

				cv::Mat matCHoleFillROI2 = matCHoleFillX(cv::Rect(0, nY - 1, matsrcCHoleROI.cols, 3));

				matCHoleFillROI2.setTo(CHoleMeanGV[0]);
			}

			// 원본 CHole 채우기 세로
			for (int nX = 2; nX < matsrcCHoleROI.cols - 2; nX += 3)
			{
				// 원본 영상
				cv::Mat matsrcFillROI = matsrcCHoleROI(cv::Rect(nX - 2, 0, 5, matsrcCHoleROI.rows));
				// Mask
				cv::Mat matCHoleFillROI1 = matCHoleROI(cv::Rect(nX - 2, 0, 5, matsrcCHoleROI.rows));

				cv::Scalar CHoleMeanGV = cv::mean(matsrcFillROI, matCHoleFillROI1);

				cv::Mat matCHoleFillROI2 = matCHoleFillY(cv::Rect(nX - 1, 0, 3, matsrcCHoleROI.rows));

				matCHoleFillROI2.setTo(CHoleMeanGV[0]);
			}

			// Minimum
			cv::min(matCHoleFillX, matCHoleFillY, matCHoleFill);

			// Aver
			//matCHoleFill = (matCHoleFillX + matCHoleFillY) / 2;

			// TEST
			//cv::Mat matSrcThresbuf = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type(), false);
			//cv::threshold(matsrcCHoleROI, matSrcThresbuf, 30, 255, CV_THRESH_BINARY_INV);
			//
			//cv::bitwise_and(matSrcThresbuf, matCHoleFillBuf, matSrcThresbuf);

			//TEST
			//cv::bitwise_and(matCHoleFill, matSrcThresbuf, matCHoleFill);

			// CHole 영역만 남기기
			cv::bitwise_and(matCHoleFill, matCHoleFillBuf, matCHoleFill);
			cv::blur(matCHoleFill, matCHoleFill, cv::Size(5, 5));
			cv::bitwise_and(matCHoleFill, matCHoleFillBuf, matCHoleFill);

			// 원본 영상 채우기
			cv::max(matsrcCHoleROI, matCHoleFill, matsrcCHoleROI);

			// Morphology2를 위한 CHole 채우기
			cv::add(matTempBuf(rectCHoleROI), matCHoleFillBuf, matTempBuf(rectCHoleROI));

			if (nImgSave >= 1)
			{
				CString strTemp;
				strTemp.Format(_T("D:\\CHole\\%02d_%02d_CHole_Mean.bmp"), nAlgImg, nImgSave++);
				cv::imwrite((cv::String)(CStringA)strTemp, matCHoleFill);
			}

			if (nImgSave >= 1)
			{
				CString strTemp;
				strTemp.Format(_T("D:\\CHole\\%s_%02d_%02d_CHole_src_ROI.bmp"), strID, nAlgImg, nImgSave++);
				cv::imwrite((cv::String)(CStringA)strTemp, matsrcCHoleROI);
			}



			if (nImgSave >= 1)
			{
				CString strTemp;
				strTemp.Format(_T("D:\\CHole\\%02d_%02d_CHole_ROI.bmp"), nAlgImg, nImgSave++);
				cv::imwrite((cv::String)(CStringA)strTemp, matCHoleROI);
			}
		}

		if (nImgSave >= 1)
		{
			CString strTemp;
			strTemp.Format(_T("D:\\CHole\\%02d_%02d_CHole_Line.bmp"), nAlgImg, nImgSave++);
			cv::imwrite((cv::String)(CStringA)strTemp, matCHoleBuf2);
		}

		if (nImgSave >= 1)
		{
			CString strTemp;
			strTemp.Format(_T("D:\\CHole\\%02d_%02d_CHole_Line_Fill.bmp"), nAlgImg, nImgSave++);
			cv::imwrite((cv::String)(CStringA)strTemp, matCHoleBuf3);
		}

		if (nImgSave >= 1)
		{
			CString strTemp;
			strTemp.Format(_T("D:\\CHole\\%02d_%02d_CHole_FillArea.bmp"), nAlgImg, nImgSave++);
			cv::imwrite((cv::String)(CStringA)strTemp, matTempBuf);
		}

		if (nImgSave >= 1)
		{
			CString strTemp;
			strTemp.Format(_T("D:\\CHole\\%s_%02d_%02d_CHole_FillArea_src.bmp"), strID, nAlgImg, nImgSave++);
			cv::imwrite((cv::String)(CStringA)strTemp, matSrcBuf);
		}

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("CHole Fill Area End."));
	}



	//////////////////////////////////////////////////////////////////////////
	// KSW 18.10.31 - 불량이 외곽에서 밝은 경우, 밝은 부분으로 인해 꼭지점 위치 오류 발생
	// ( 최소 사각형으로 꼭지점 찾음... 꼭지점 위치가 50pixel 이상 차이 나는 경우도 존재 )
	// 원래 점등영역에서 튀어나온 영역을 제거하는 목적
	// 꼭지점 찾는 파라미터를 늘려서 사용해도 무방.... ( 약 1초 정도 속도 늘어남 )
	//  => 대신 사각형 Cell에서는 사용 불가
	//////////////////////////////////////////////////////////////////////////

	cv::Mat matTemp2Buf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf.copyTo(matTemp2Buf);

	if( false )
	{
		//cv::Mat matBuf = matTempBuf.clone();

		cv::Mat	StructElem = cv::getStructuringElement(MORPH_RECT, cv::Size(101, 1));
	
		//부분 모폴로지 관련 사용
		if (bUsePartialMorp && bFindPartialMorpROI)
		{
			Partial_Morphology(matTemp2Buf, matTempBuf, MORPH_OPEN, StructElem, rcMorpROI); // 부분 모폴로지를 사용한다.
		}
		else
		{
			cv::morphologyEx(matTemp2Buf, matTempBuf, MORPH_OPEN, StructElem);
		}
		
		StructElem.release();

		//////////////////////////////////////////////////////////////////////////

		StructElem = cv::getStructuringElement(MORPH_RECT, cv::Size(1, 101));

		//부분 모폴로지 관련 사용
		if (bUsePartialMorp && bFindPartialMorpROI)
		{
			Partial_Morphology(matTempBuf, matTemp2Buf, MORPH_OPEN, StructElem , rcMorpROI); // 부분 모폴로지를 사용한다.
		}
		else
		{
			cv::morphologyEx(matTempBuf, matTemp2Buf, MORPH_OPEN, StructElem);
		}

		StructElem.release();

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Morphology 2."));
	}

	if (nSaveImageCount >= 0)
	{
		CString strTemp;
		strTemp.Format(_T("d:\\%02d_%02d_Morp2.jpg"), nAlgImg, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matTemp2Buf);
	}

	// Threshold
	cv::threshold(matTemp2Buf, matTempBuf, nThreshold, 255.0, THRESH_BINARY);
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("threshold."));

	if (nSaveImageCount >= 0)
	{
		CString strTemp;
		strTemp.Format(_T("d:\\%02d_%02d_threshold.jpg"), nAlgImg, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matTempBuf);
	}

	//////////////////////////////////////////////////////////////////////////
	// Cell 영역 찾기
	//////////////////////////////////////////////////////////////////////////

	// 검사 영역 Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matTempBuf, nMinArea, rectCell);

	if (nSaveImageCount >= 0)
	{
		CString strTemp;
		strTemp.Format(_T("d:\\%02d_%02d_Area.jpg"), nAlgImg, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matTempBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("FindCellArea."));

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		matTempBuf.release();
		return nErrorCode;
	}

	// Temp 메모리 할당
	cv::Mat matThreshBuf = cMem->GetMat(matTempBuf.size(), matTempBuf.type());

	// Edge 돌려서 편차 큰곳 찾기
	//matTempBuf.copyTo(matThreshBuf);
	//영역 찾기 성공 시 - 라플라시안도 부분적으로 한다.
	if (bUsePartialMorp && bFindPartialMorpROI)
	{
		Partial_Laplacian(matTempBuf, matThreshBuf, rcMorpROI);
	}
	else
	{
		cv::Laplacian(matTempBuf, matThreshBuf, CV_8U);
	}

	if (nSaveImageCount >= 0)
	{
		CString strTemp;
		strTemp.Format(_T("d:\\%02d_%02d_Laplacian.jpg"), nAlgImg, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matThreshBuf);
	}

	//////////////////////////////////////////////////////////////////////////
	// 점등 영역 찾기
	//////////////////////////////////////////////////////////////////////////	

	// 전체 외각선
	vector< vector< cv::Point2i > > contours;
	cv::findContours(matTempBuf, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	int nMaxIndex = 0;
	double dMaxSize = 0.0;
	for (int i = 0; i < (int)contours.size(); i++)
	{
		double dValue = cv::contourArea(contours[i]);
		if (dValue > dMaxSize)
		{
			dMaxSize = dValue;
			nMaxIndex = i;
		}
	}

	cv::RotatedRect BoundingBox = cv::minAreaRect(contours[nMaxIndex]);

	cv::Point2f vertices[E_CORNER_END];
	BoundingBox.points(vertices);

	//////////////////////////////////////////////////////////////////////////
	// 코너 꼭지점 4개 찾기
	//////////////////////////////////////////////////////////////////////////
	cv::Point ptResCorner[E_CORNER_END], ptCornerAlign[E_CORNER_END];	

	long	nWidth	= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuf.rows;	// 영상 세로 크기	

	nErrorCode = FindCornerPoint(vertices, ptResCorner, nWidth, nHeight);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;
	
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Find 4-Corner."));

	// Align 결과 확인 용
	if (nSaveImageCount >= 0)
	{
		cv::Mat matColor;
		cv::cvtColor(matSrcBuf, matColor, COLOR_GRAY2RGB);

		cv::line(matColor, ptResCorner[0], ptResCorner[1], cv::Scalar(255, 0, 0));
		cv::line(matColor, ptResCorner[1], ptResCorner[2], cv::Scalar(255, 0, 0));
		cv::line(matColor, ptResCorner[2], ptResCorner[3], cv::Scalar(255, 0, 0));
		cv::line(matColor, ptResCorner[3], ptResCorner[0], cv::Scalar(255, 0, 0));

		CString strTemp;
		strTemp.Format(_T("d:\\%02d_%02d_Line.jpg"), nAlgImg, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matColor);

		matColor.release();
	}

	//////////////////////////////////////////////////////////////////////////
	// 영역 안쪽으로 살짝 축소...
	//////////////////////////////////////////////////////////////////////////

	ptResCorner[E_CORNER_LEFT_TOP].x		+= nInPixel;
	ptResCorner[E_CORNER_LEFT_TOP].y		+= nInPixel;
	ptResCorner[E_CORNER_RIGHT_TOP].x		-= nInPixel;
	ptResCorner[E_CORNER_RIGHT_TOP].y		+= nInPixel;
	ptResCorner[E_CORNER_LEFT_BOTTOM].x		+= nInPixel;
	ptResCorner[E_CORNER_LEFT_BOTTOM].y		-= nInPixel;
	ptResCorner[E_CORNER_RIGHT_BOTTOM].x	-= nInPixel;
	ptResCorner[E_CORNER_RIGHT_BOTTOM].y	-= nInPixel;	

	//////////////////////////////////////////////////////////////////////////
	// Rect 범위 확인용

	// 범위 크게 설정 ( 양 옆으로 200 Pixel 빈 배경 있어야 함 )
	// 빈부분 평균 채우기
	int offset = 100;

	CRect rectROI = new CRect(
		min(ptResCorner[E_CORNER_LEFT_TOP].x,		ptResCorner[E_CORNER_LEFT_BOTTOM].x		) - offset,
		min(ptResCorner[E_CORNER_LEFT_TOP].y,		ptResCorner[E_CORNER_RIGHT_TOP].y		) - offset,
		max(ptResCorner[E_CORNER_RIGHT_TOP].x,		ptResCorner[E_CORNER_RIGHT_BOTTOM].x	) + offset,
		max(ptResCorner[E_CORNER_LEFT_BOTTOM].y,	ptResCorner[E_CORNER_RIGHT_BOTTOM].y	) + offset );

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rectROI.left	<	0		)	rectROI.left	= 0;
	if( rectROI.top		<	0		)	rectROI.top		= 0;
	if( rectROI.right	>= 	nWidth	)	rectROI.right	= nWidth - 1;
	if( rectROI.bottom	>= 	nHeight	)	rectROI.bottom	= nHeight - 1;

	// 검사 영역이 영상 크기를 벗어난 경우
	//if( rectROI.left	<	0		||
	//	rectROI.top		<	0		||
	//	rectROI.right	>= 	nWidth	||
	//	rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	//////////////////////////////////////////////////////////////////////////
	// 곡선 처리
	
	// 마스크 초기화
	matTempBuf.setTo(0);

	int npt[] = { E_CORNER_END };

	// 곡선 사용 안할때 ( Rect 인 경우 )
	if( !bRoundUse )
	{
		const cv::Point* ppt[1] = { ptResCorner };

		// 점등 영역 마스크
		cv::fillPoly(matTempBuf, ppt, npt, 1, cv::Scalar(255, 255, 255));
	}

	// 곡선 부분 처리
	else
	{
		// Temp 메모리 할당
		cv::Mat matRndBuf = cMem->GetMat(matTempBuf.size(), matTempBuf.type());

		//////////////////////////////////////////////////////////////////////////
		// 기울기 보정 했을 때, 꼭지점 좌표 얻기
		// 등록된 곡선 & ROI 영역은 꼭지점 좌표 기준
		//////////////////////////////////////////////////////////////////////////

		// 회전 좌표 계산 시, 사용
		double	dTheta	= -BoundingBox.angle;

		// 예외처리
		if( 45.0 < dTheta && dTheta < 135.0 )	dTheta -= 90.0;
		if( -45.0 > dTheta && dTheta > -135.0 )	dTheta += 90.0;

		dTheta	*= PI;
		dTheta	/= 180.0;
		double	dSin	= sin( dTheta );
		double	dCos	= cos( dTheta );
		double	dSin_	= sin( -dTheta );
		double	dCos_	= cos( -dTheta );
		int		nCx		= matSrcBuf.cols / 2;
		int		nCy		= matSrcBuf.rows / 2;

		for (int i=0 ; i<E_CORNER_END ; i++)
		{
			// 회전 시, 예측 좌표 계산
			ptCornerAlign[i].x = (int)( dCos * (ptResCorner[i].x - nCx) - dSin * (ptResCorner[i].y - nCy) + nCx );
			ptCornerAlign[i].y = (int)( dSin * (ptResCorner[i].x - nCx) + dCos * (ptResCorner[i].y - nCy) + nCy );
		}

		//////////////////////////////////////////////////////////////////////////
		// 등록된 곡선 & ROI 영역 -> 현재 Grab에 맞게 좌표 보정 & 위치 찾기
		//////////////////////////////////////////////////////////////////////////

		// 재계산
		dTheta	= BoundingBox.angle;

		// 예외처리
		if( 45.0 < dTheta && dTheta < 135.0 )	dTheta -= 90.0;
		if( -45.0 > dTheta && dTheta > -135.0 )	dTheta += 90.0;

		dTheta	*= PI;
		dTheta	/= 180.0;
		dSin	= sin( dTheta );
		dCos	= cos( dTheta );
		dSin_	= sin( -dTheta );
		dCos_	= cos( -dTheta );

		cv::Point	ptSE[2], ptTempLine[2];
		int			nSE;

		for (int i=0; i<MAX_MEM_SIZE_E_INSPECT_AREA ; i++)
		{
			// 곡선 개수 없으면, 영역 등록 안된것으로 판단 / 다음으로...
			if( tRoundSet[i].nContourCount <= 0 )	continue;

			// 좌,우,상,하 움직여서, 제일 편차 큰 곳 찾기 ( 상하좌우 - pixel )
			const int nArrSize	= (nFindRoundOffset *2+1) * (nFindRoundOffset *2+1);
			__int64 *nSum		= (__int64 *)malloc( sizeof(__int64) * nArrSize );
			memset(nSum, 0, sizeof(__int64) * nArrSize);
			cv::Point2i ptTemp;

			// 점등영역 밖에 있는 꼭지점 개수
			int nOutsideCnt = 0;
			for (int j=0 ; j<E_CORNER_END ; j++)
			{
				if( !tRoundSet[i].nCornerInside[j] )
					nOutsideCnt++;
			}

			// 한개도 없다면 설정 에러임
// 			if( nOutsideCnt == 0 )
// 				AfxMessageBox(_T("Set Corner Err !!!"));

			const int nCount	= (int)tRoundSet[i].nContourCount;
			cv::Point *ptPoly	= new cv::Point[ nCount ];
			memset(ptPoly, 0, sizeof(cv::Point) * nCount );

			// Align 꼭지점에서 제일 가까운 꼭지점
			int nCornerAlign = tRoundSet[i].nCornerMinLength;

			// 곡선 개수 존재하는 경우
			for (int j=0 ; j<tRoundSet[i].nContourCount ; j++)
			{
				// 얼라인 좌표부터, 곡선 설정 영역까지 최단 거리... 보정
				// 장비 & 스테이지마다 편차 존재
				int XX = tRoundSet[i].ptContours[j].x + ptCornerAlign[nCornerAlign].x;
				int YY = tRoundSet[i].ptContours[j].y + ptCornerAlign[nCornerAlign].y;

				// 회전 시, 예측 좌표 계산
				ptTemp.x = (int)( dCos * (XX - nCx) - dSin * (YY - nCy) + nCx );
				ptTemp.y = (int)( dSin * (XX - nCx) + dCos * (YY - nCy) + nCy );

				// 보정된 좌표 넣기
				ptPoly[j].x = ptTemp.x;
				ptPoly[j].y = ptTemp.y;				

				// 좌,우,상,하 움직여서, 제일 편차 큰 곳 찾기
				int m = 0;
				for (int y=-nFindRoundOffset; y<= nFindRoundOffset; y++)
				{
					for (int x=-nFindRoundOffset; x<= nFindRoundOffset; x++, m++)
					{
						int indexX = ptTemp.x + x;
						int indexY = ptTemp.y + y;

						// 예외처리
						if( indexX < 0 )						continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if( indexY < 0 )						continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if( indexX >= matThreshBuf.cols )		continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if( indexY >= matThreshBuf.rows )		continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;

						// KSW 17.07.19 - 이진 영상으로 편차 계산 / 원본으로 하면 단일 패턴의 경우 빈부분을 찾는 경우 발생
						//int k	= abs(matThreshBuf.at<uchar>(indexY+1, indexX) - matThreshBuf.at<uchar>(indexY-1, indexX))
						//		+ abs(matThreshBuf.at<uchar>(indexY, indexX+1) - matThreshBuf.at<uchar>(indexY, indexX-1));

						//nSum[m++] += k;

						nSum[m] += matThreshBuf.at<uchar>(indexY, indexX);
					}
				}
			}

			// 좌,우,상,하 움직여서, 제일 편차 큰 곳 찾기
			long nMax	= 0;
			int m		= 0;
			int xx		= 0;
			int yy		= 0;
			for (int y=-nFindRoundOffset; y<= nFindRoundOffset; y++)
			{
				for (int x=-nFindRoundOffset; x<= nFindRoundOffset; x++, m++)
				{
					if( nMax < nSum[m] )
					{
						nMax = nSum[m];
						xx = x;
						yy = y;
					}
				}
			}
			free(nSum);
			nSum = NULL;

			// 곡선 부분, 점등영역 안쪽으로 살짝 넣기 ( 상황에 따라 다름 )
			cv::Point ptRnd = calcRoundIn(tRoundSet, i, nRoundIn);
			xx += ptRnd.x;
			yy += ptRnd.y;

			// 해당 범위 내에서 찾지 못한 경우
			// 장비 & 스테이지마다 편차 존재
			// 찾는 범위를 높이거나 ( nFindOffset ), 원래 위치에다 그리기
// 			if( nMax == 0 )
// 				AfxMessageBox(_T("Find Range Over Err !!!"));

			// 찾은 경우 
			{
				// 위치 수정
				int j = 0;
				for ( ; j<tRoundSet[i].nContourCount ; j++)
				{
					ptPoly[j].x += xx;
					ptPoly[j].y += yy;

					// 예외처리
					if( ptPoly[j].x < 0 )					return E_ERROR_CODE_ALIGN_IMAGE_OVER;
					if( ptPoly[j].y < 0 )					return E_ERROR_CODE_ALIGN_IMAGE_OVER;
					if( ptPoly[j].x >= matRndBuf.cols )		return E_ERROR_CODE_ALIGN_IMAGE_OVER;
					if( ptPoly[j].y >= matRndBuf.rows )		return E_ERROR_CODE_ALIGN_IMAGE_OVER;

					matRndBuf.at<uchar>(ptPoly[j].y, ptPoly[j].x) = (uchar)255;
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// 곡선 이외 직선 연결
			//////////////////////////////////////////////////////////////////////////

			// 끝지점
			ptTempLine[0] = ptPoly[0];
			ptTempLine[1] = ptPoly[nCount-1];

			delete ptPoly;
			ptPoly = NULL;

			// 끝지점 등록
			if( i == 0 )
			{
				ptSE[0] = ptTempLine[0];
				ptSE[1] = ptTempLine[1];
			}
			// 끝지점 연결
			else
			{
				// 곡선 이외 직선 연결
				calcLineConnect(matRndBuf, ptSE, ptTempLine, nSE, i);
			}

// 			delete ptPoly;
// 			ptPoly = NULL;
		}
		// 마지막 끝지점 연결
		cv::line(matRndBuf, ptSE[0], ptSE[1], cv::Scalar(255, 255, 255));

		// KSW 17.11.12 - 끊어진 부분 잇기
		cv::dilate(matRndBuf, matTempBuf, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)) );

		// 외곽선 안쪽으로 채우기 ( 4방향? )
		cv::floodFill(matTempBuf, BoundingBox.center, cv::Scalar(255, 255, 255));

		matRndBuf.release();
	}	
	// KSW 17.07.17 곡선 부분 처리
	//////////////////////////////////////////////////////////////////////////

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Corner Find."));

	// 배경 영역 넘기기 ( Mura에서 사용 )
	cv::bitwise_not(matTempBuf, matBKGBuf);
	
	cv::Mat matEdgeArea = cMem->GetMat(matTempBuf.size(), CV_8UC1, false);

	// 점등 영역 외곽 조금 남기기 ( 점등 영역 평균 시, 사용 )
	nErrorCode = FindEdgeArea(matTempBuf, matEdgeArea, offset, cMem);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\0.EdgeArea.bmp"), matEdgeArea);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Area Find."));

	// 평균 버퍼 메모리 할당
	cv::Mat matMeanBuf = cMem->GetMat(matSrcBuf.size(), matSrcBuf.type(), false);
	matSrcBuf.copyTo(matMeanBuf);

	// Offset ( 점등 영역 평균 크기 )
	// nSeg x nSeg 영역 안에, 점등 영역이 존재해야 함
	// 6.18"의 경우, 크기 늘려야 함 ( 300 -> 500 )
	// 좌우상하 offset(현재 70) 만큼 늘어난 상태
	// x, y 분할하여 사용
	int nSegX = 3;
	int nSegY = offset + offset;

	// 가로 방향 평균 채우기
	nErrorCode = FillAreaMeanX(matMeanBuf, matEdgeArea, rectROI, nSegX, nSegY, nRoundMinGV);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\1.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\1.EdgeArea.bmp"), matEdgeArea);

	// Offset ( 점등 영역 평균 크기 )
	// nSeg x nSeg 영역 안에, 점등 영역이 존재해야 함
	// 6.18"의 경우, 크기 늘려야 함 ( 300 -> 500 )
	// 좌우상하 offset(현재 70) 만큼 늘어난 상태
	// x, y 분할하여 사용
	nSegX = 800;
	nSegY = 3;

	// 세로 방향 평균 채우기
	nErrorCode = FillAreaMeanY(matMeanBuf, matEdgeArea, rectROI, nSegX, nSegY, nRoundMinGV);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\2.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\2.EdgeArea.bmp"), matEdgeArea);

	// 점등된 부분만 남기기
	nErrorCode = FillMerge(matSrcBuf, matMeanBuf, matTempBuf, nAlgImg, cMem );
	
	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\3.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\3.EdgeArea.bmp"), matEdgeArea);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Fill Average."));

	// 메모리 해제
	matMeanBuf.release();
	matTempBuf.release();
	matThreshBuf.release();
	
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}


// 외곽 부분 처리
long CInspectAlign::DoFillOutArea_SVI(cv::Mat& matSrcBuf, cv::Mat& matBKGBuf, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA],
	double* dPara, int nAlgImg, int nCameraNum, int nRatio, wchar_t* strID, cv::Point* ptCorner)
{
	// Black 패턴 제외
	if( nAlgImg == E_IMAGE_CLASSIFY_SVI_BLACK )		return E_ERROR_CODE_TRUE;

	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )							return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	// 이진화
	//int		nThreshold		= (int)dPara[E_PARA_SVI_ALIGN_THRESHOLD];

	// Round 영역 범위 설정
	int		nCornerSize		= (int)dPara[E_PARA_SVI_ROUND_SIZE];

	// Round Cell 사용 유/무
	int		nRoundUse		= (int)dPara[E_PARA_SVI_ROUND_USE];

	// 안쪽으로 검사
	int		nRoundIn		= (int)dPara[E_PARA_SVI_ROUND_IN];

	// Round 영역 코너 설정 높이
	int		nDiagonal		= (int)dPara[E_PARA_SVI_ROUND_DIAGONAL];

	long	nWidth			= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight			= (long)matSrcBuf.rows;	// 영상 세로 크기

	//////////////////////////////////////////////////////////////////////////
	// Align 영역 안쪽으로 살짝 축소...
	//////////////////////////////////////////////////////////////////////////
	cv::Point ptCornerIn[E_CORNER_END];

	ptCornerIn[E_CORNER_LEFT_TOP].x		= ptCorner[E_CORNER_LEFT_TOP].x		+ nRoundIn;
	ptCornerIn[E_CORNER_LEFT_TOP].y		= ptCorner[E_CORNER_LEFT_TOP].y		+ nRoundIn;
	ptCornerIn[E_CORNER_RIGHT_TOP].x	= ptCorner[E_CORNER_RIGHT_TOP].x	- nRoundIn;
	ptCornerIn[E_CORNER_RIGHT_TOP].y	= ptCorner[E_CORNER_RIGHT_TOP].y	+ nRoundIn;
	ptCornerIn[E_CORNER_LEFT_BOTTOM].x	= ptCorner[E_CORNER_LEFT_BOTTOM].x	+ nRoundIn;
	ptCornerIn[E_CORNER_LEFT_BOTTOM].y	= ptCorner[E_CORNER_LEFT_BOTTOM].y	- nRoundIn;
	ptCornerIn[E_CORNER_RIGHT_BOTTOM].x	= ptCorner[E_CORNER_RIGHT_BOTTOM].x	- nRoundIn;
	ptCornerIn[E_CORNER_RIGHT_BOTTOM].y	= ptCorner[E_CORNER_RIGHT_BOTTOM].y	- nRoundIn;


	//////////////////////////////////////////////////////////////////////////
	// 점등 영역
	//////////////////////////////////////////////////////////////////////////

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	CRect rectROI = CRect(
		min(ptCornerIn[E_CORNER_LEFT_TOP].x,	ptCornerIn[E_CORNER_LEFT_BOTTOM].x	),
		min(ptCornerIn[E_CORNER_LEFT_TOP].y,	ptCornerIn[E_CORNER_RIGHT_TOP].y	),
		max(ptCornerIn[E_CORNER_RIGHT_TOP].x,	ptCornerIn[E_CORNER_RIGHT_BOTTOM].x	),
		max(ptCornerIn[E_CORNER_LEFT_BOTTOM].y,	ptCornerIn[E_CORNER_RIGHT_BOTTOM].y	));

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	cv::Mat matTempBuf = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);

	// Cell 점등 영역
	for (int i=0 ; i<E_CORNER_END ; i++)
		cv::line(matTempBuf, ptCornerIn[i], ptCornerIn[(i+1)%4], cv::Scalar(255, 255, 255), 1);


	//////////////////////////////////////////////////////////////////////////
	// 꼭지점 4방향 Round Cell
	//////////////////////////////////////////////////////////////////////////

	if( nRoundUse > 0 )
	{
		// 외곽 부분 십자표시로 지우기
		for (int i=0 ; i<E_CORNER_END ; i++)
		{
			cv::line(matTempBuf,  cv::Point(ptCornerIn[i].x-nCornerSize, ptCornerIn[i].y), cv::Point(ptCornerIn[i].x+nCornerSize, ptCornerIn[i].y), cv::Scalar(0, 0, 0), 1);
			cv::line(matTempBuf,  cv::Point(ptCornerIn[i].x, ptCornerIn[i].y-nCornerSize), cv::Point(ptCornerIn[i].x, ptCornerIn[i].y+nCornerSize), cv::Scalar(0, 0, 0), 1);
		}

		cv::Point pt1, pt2, pt3;

		// E_CORNER_LEFT_TOP
		pt1 = cv::Point( ptCornerIn[E_CORNER_LEFT_TOP].x+nCornerSize, ptCornerIn[E_CORNER_LEFT_TOP].y );
		pt2 = cv::Point( ptCornerIn[E_CORNER_LEFT_TOP].x, ptCornerIn[E_CORNER_LEFT_TOP].y+nCornerSize );
		pt3.x = ( pt1.x + pt2.x ) / 2 - nDiagonal;
		pt3.y = ( pt1.y + pt2.y ) / 2 - nDiagonal;
		cv::line(matTempBuf, pt1, pt3, cv::Scalar(255, 255, 255), 1);
		cv::line(matTempBuf, pt2, pt3, cv::Scalar(255, 255, 255), 1);

		// E_CORNER_RIGHT_TOP
		pt1 = cv::Point( ptCornerIn[E_CORNER_RIGHT_TOP].x-nCornerSize, ptCornerIn[E_CORNER_RIGHT_TOP].y );
		pt2 = cv::Point( ptCornerIn[E_CORNER_RIGHT_TOP].x, ptCornerIn[E_CORNER_RIGHT_TOP].y+nCornerSize );
		pt3.x = ( pt1.x + pt2.x ) / 2 + nDiagonal;
		pt3.y = ( pt1.y + pt2.y ) / 2 - nDiagonal;
		cv::line(matTempBuf, pt1, pt3, cv::Scalar(255, 255, 255), 1);
		cv::line(matTempBuf, pt2, pt3, cv::Scalar(255, 255, 255), 1);

		// E_CORNER_RIGHT_BOTTOM
		pt1 = cv::Point( ptCornerIn[E_CORNER_RIGHT_BOTTOM].x-nCornerSize, ptCornerIn[E_CORNER_RIGHT_BOTTOM].y );
		pt2 = cv::Point( ptCornerIn[E_CORNER_RIGHT_BOTTOM].x, ptCornerIn[E_CORNER_RIGHT_BOTTOM].y-nCornerSize );
		pt3.x = ( pt1.x + pt2.x ) / 2 + nDiagonal;
		pt3.y = ( pt1.y + pt2.y ) / 2 + nDiagonal;
		cv::line(matTempBuf, pt1, pt3, cv::Scalar(255, 255, 255), 1);
		cv::line(matTempBuf, pt2, pt3, cv::Scalar(255, 255, 255), 1);

		// E_CORNER_LEFT_BOTTOM
		pt1 = cv::Point( ptCornerIn[E_CORNER_LEFT_BOTTOM].x+nCornerSize, ptCornerIn[E_CORNER_LEFT_BOTTOM].y );
		pt2 = cv::Point( ptCornerIn[E_CORNER_LEFT_BOTTOM].x, ptCornerIn[E_CORNER_LEFT_BOTTOM].y-nCornerSize );
		pt3.x = ( pt1.x + pt2.x ) / 2 - nDiagonal;
		pt3.y = ( pt1.y + pt2.y ) / 2 + nDiagonal;
		cv::line(matTempBuf, pt1, pt3, cv::Scalar(255, 255, 255), 1);
		cv::line(matTempBuf, pt2, pt3, cv::Scalar(255, 255, 255), 1);
	}

	//////////////////////////////////////////////////////////////////////////
	// 노치(Notch) Round Cell
	//////////////////////////////////////////////////////////////////////////

	if( nRoundUse > 1 )
	{
		// 가로 방향 60 pixel
		int nSizeX = 60;

		// 세로 방향 70%
		int nSizeY = (int)(rectROI.Height() * 0.7);

		// 여백
		int nEmptyY = (rectROI.Height() - nSizeY) / 2;

		// Notch 부분 잘라내기
		cv::Mat matNotchBuf = matSrcBuf( cv::Rect(rectROI.left, rectROI.top+nEmptyY, nSizeX, nSizeY) );
		
		// Gray -> Color
		cv::Mat matNotchGrayBuf, matNotchEdgeBuf;
		cv::cvtColor(matNotchBuf, matNotchGrayBuf, COLOR_RGB2GRAY);

		// Blur
		//cv::blur(matNotchGrayBuf, matNotchEdgeBuf, cv::Size(3, 3));

		// Threshold
		cv::threshold(matNotchGrayBuf, matNotchEdgeBuf, 10, 255.0, THRESH_BINARY);

		// Edge
		cv::Laplacian(matNotchEdgeBuf, matNotchEdgeBuf, CV_8UC1);
		

		//////////////////////////////////////////////////////////////////////////

		int nMax, nTemp, nIndexX=0, nIndexY1=0, nIndexY2=0;

		nMax = 0;
		for (int y=1 ; y<matNotchEdgeBuf.rows/2-1 ; y++)
		{
			nTemp  = (int)( cv::sum( matNotchEdgeBuf.row(y-1) )[0] );
			nTemp += (int)( cv::sum( matNotchEdgeBuf.row(y  ) )[0] );
			nTemp += (int)( cv::sum( matNotchEdgeBuf.row(y+1) )[0] );

			if( nMax < nTemp )
			{
				nMax = nTemp;
				nIndexY1 = y;
			}
		}

		nMax = 0;
		for (int y=matNotchEdgeBuf.rows/2+1 ; y<matNotchEdgeBuf.rows-1 ; y++)
		{
			nTemp  = (int)( cv::sum( matNotchEdgeBuf.row(y-1) )[0] );
			nTemp += (int)( cv::sum( matNotchEdgeBuf.row(y  ) )[0] );
			nTemp += (int)( cv::sum( matNotchEdgeBuf.row(y+1) )[0] );

			if( nMax < nTemp )
			{
				nMax = nTemp;
				nIndexY2 = y;
			}
		}

		nMax = 0;
		for (int x=0 ; x<matNotchEdgeBuf.cols ; x++)
		{
			nTemp = (int)( cv::sum( matNotchEdgeBuf.col(x) )[0] );

			if( nMax < nTemp )
			{
				nMax = nTemp;
				nIndexX = x;
			}
		}

		//////////////////////////////////////////////////////////////////////////

		cv::Mat matNotchROIBuf = matTempBuf( cv::Rect(rectROI.left, rectROI.top+nEmptyY, nSizeX, nSizeY) );

		cv::line(matNotchROIBuf, cv::Point(0, nIndexY1-9),			cv::Point(0, nIndexY2+9),			cv::Scalar(0, 0, 0),		1);

		cv::line(matNotchROIBuf, cv::Point(0, nIndexY1-10),			cv::Point(nIndexX+5, nIndexY1-10),	cv::Scalar(255, 255, 255),	1);
		cv::line(matNotchROIBuf, cv::Point(nIndexX+5, nIndexY2+10),	cv::Point(nIndexX+5, nIndexY1-10),	cv::Scalar(255, 255, 255),	1);
		cv::line(matNotchROIBuf, cv::Point(0, nIndexY2+10),			cv::Point(nIndexX+5, nIndexY2+10),	cv::Scalar(255, 255, 255),	1);	
	}


	//////////////////////////////////////////////////////////////////////////
	// 배경 영역 추출
	//////////////////////////////////////////////////////////////////////////

	// 외곽선 안쪽으로 채우기
	cv::floodFill(matTempBuf, cv::Point(rectROI.CenterPoint().x, rectROI.CenterPoint().y), cv::Scalar(255, 255, 255));

	// 배경 영역
	cv::bitwise_not(matTempBuf, matBKGBuf);


	//////////////////////////////////////////////////////////////////////////
	// 주변 채우기
	//////////////////////////////////////////////////////////////////////////	

	// 범위 크게 설정 ( 양 옆으로 200 Pixel 빈 배경 있어야 함 )
	int offset = 70;

	rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x,		ptCorner[E_CORNER_LEFT_BOTTOM].x	) - offset,
		min(ptCorner[E_CORNER_LEFT_TOP].y,		ptCorner[E_CORNER_RIGHT_TOP].y		) - offset,
		max(ptCorner[E_CORNER_RIGHT_TOP].x,		ptCorner[E_CORNER_RIGHT_BOTTOM].x	) + offset,
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	) + offset );

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rectROI.left	<	0		)	rectROI.left	= 0;
	if( rectROI.top		<	0		)	rectROI.top		= 0;
	if( rectROI.right	>= 	nWidth	)	rectROI.right	= nWidth - 1;
	if( rectROI.bottom	>= 	nHeight	)	rectROI.bottom	= nHeight - 1;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// 점등 영역 외곽 조금 남기기 ( 점등 영역 평균 시, 사용 )
	cv::Mat matEdgeArea;
	nErrorCode = FindEdgeArea_SVI(matTempBuf, matEdgeArea, 30);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	// 평균 버퍼
	cv::Mat matMeanBuf = matSrcBuf.clone();

	// 가로 방향 평균 채우기
	nErrorCode = FillAreaMeanX_SVI(matMeanBuf, matEdgeArea, rectROI, 3, 100, 0);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	// 세로 방향 평균 채우기
	nErrorCode = FillAreaMeanY_SVI(matMeanBuf, matEdgeArea, rectROI, 200, 3, 0);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	// Gray -> Color
	cv::cvtColor(matBKGBuf, matTempBuf, COLOR_GRAY2RGB);

	// 점등된 부분만 남기기
	nErrorCode = FillMerge_SVI(matSrcBuf, matMeanBuf, matTempBuf);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	// 할당 해제
	if( !matTempBuf.empty() )	matTempBuf.release();
	if( !matEdgeArea.empty() )	matEdgeArea.release();
	if( !matMeanBuf.empty() )	matMeanBuf.release();

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::DoFillOutArea_APP(cv::Mat& matSrcBuf, cv::Mat& matBKGBuf, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], double* dPara, int nAlgImg, int nCameraNum, int nRatio, wchar_t* strID, cv::Point* ptCorner, vector<vector<Point2i>> &ptActive, double dAlignTH, CString strPath, bool bImageSave)
{
	Mat mtOrg;
	matSrcBuf.copyTo(mtOrg);

	Mat mtContour = Mat::zeros(mtOrg.size(), mtOrg.type());

	DoRotateImage(mtOrg, mtOrg, dAlignTH);
	
	//Tact Time 줄이기 위함
	cv::resize(mtOrg, mtOrg, Size(), 0.25, 0.25);

	Mat mtRef[MAX_MEM_SIZE_E_INSPECT_AREA];
	Mat mtMatchRst;
	CString strRefImagePath;
	strRefImagePath.Format(_T("%s\\CornerEdge"), strPath);

	vector<vector<Point2i>> ptContours;
	vector<Point2i>			pt;

	double dMatchRate;
	Point  ptMatch;
	Point  ptMatchResize;
	for(int nROIInx = 0; nROIInx < MAX_MEM_SIZE_E_INSPECT_AREA; nROIInx++)
	{
		strRefImagePath.Format(_T("%s\\CornerEdge\\%d.bmp"), strPath, nROIInx);
		mtRef[nROIInx] = imread((cv::String)(CStringA)strRefImagePath, -1);
		if(mtRef[nROIInx].empty())
			continue;

		//Tact Time 줄이기 위함
		cv::resize(mtRef[nROIInx], mtRef[nROIInx], Size(), 0.25, 0.25);

 		cv::matchTemplate(mtOrg, mtRef[nROIInx], mtMatchRst, CV_TM_CCORR_NORMED);
 		cv::minMaxLoc(mtMatchRst, NULL, &dMatchRate, NULL, &ptMatchResize);
 		
		ptMatch = Point(ptMatchResize.x * 4, ptMatchResize.y * 4);
 		
		if(dMatchRate < 0.6)
			return E_ERROR_CODE_FALSE;

		
		vector<Point>().swap(pt);

		for(int n = 0; n < tRoundSet[nROIInx].nContourCount; n++)
		{
			//ROI 영역안의 좌표에서 영상전체에 대한 좌표로 변경
			Point ptTlans = Point(tRoundSet[nROIInx].ptContours[n].x + ptMatch.x,
								  tRoundSet[nROIInx].ptContours[n].y + ptMatch.y);
			if(ptTlans.x < 0 || ptTlans.x >= matSrcBuf.cols || ptTlans.y < 0 || ptTlans.y >= matSrcBuf.rows)
				return E_ERROR_CODE_ALIGN_IMAGE_OVER;

			pt.push_back(ptTlans);
			cv::circle(mtContour, ptTlans, 1, Scalar(255));
		}

		ptContours.push_back(pt);
	}

	for(int nContourInx = 0; nContourInx < ptContours.size(); nContourInx++)
	{
		int nNextContourInx = nContourInx + 1;	
		if(nNextContourInx >= ptContours.size())
			nNextContourInx = 0;
		cv::line(mtContour, ptContours[nContourInx][ptContours[nContourInx].size()-1], ptContours[nNextContourInx][0], Scalar(255));
	}

	cv::findContours(mtContour, ptActive, RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	return E_ERROR_CODE_TRUE;
}


long CInspectAlign::DoFillOutAreaDust(cv::Mat& matSrcBuf, cv::Point ptResCorner[E_CORNER_END], double dAngle, cv::Rect* rcCHoleROI, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], ROUND_SET tCHoleSet[MAX_MEM_SIZE_E_INSPECT_AREA], double* dPara, int nAlgImg, int nRatio, wchar_t* strID)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )							return E_ERROR_CODE_EMPTY_BUFFER;

	// Dust 패턴이 아닌 경우
	if( nAlgImg != E_IMAGE_CLASSIFY_AVI_DUST )		return E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	// Round Cell & CHole 사용 유/무
	bool	bRoundUse		= (dPara[E_PARA_ROUND_USE] > 0) ? true : false;
	bool	bCHoleUse		= (dPara[E_PARA_CHOLE_USE] > 0) ? true : false;

	// CHole Threshold value
	double dThresValue = dPara[E_PARA_ALIGN_THRESHOLD];

	// Round Cell & CHole 안쪽으로 몇 Pixel 들어갈지.... ( 곡선 부분만... )
	int		nRoundIn		= (int)(dPara[E_PARA_ROUND_IN]);
	int		nCHoleIn		= (int)dPara[E_PARA_CHOLE_IN];

	// Find CHole Offset
	int		nFindCHoleOffset = (int)dPara[E_PARA_CHOLE_FIND_OFFSET];

	// 외곽 평균 채울때, 최소 평균 GV 설정
	int		nRoundMinGV		= (int)dPara[E_PARA_ROUND_DUST_MIN_GV];

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;


	//////////////////////////////////////////////////////////////////////////
	// 코너 꼭지점 4개 찾기
	//////////////////////////////////////////////////////////////////////////
	cv::Point ptCornerAlign[E_CORNER_END];

	long	nWidth	= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuf.rows;	// 영상 세로 크기


	//////////////////////////////////////////////////////////////////////////
	// 영역 안쪽으로 살짝 축소...
	//////////////////////////////////////////////////////////////////////////

	// 영역 안쪽으로 살짝 축소... ( 영역 )
	int nInPixel = 3;

	ptResCorner[E_CORNER_LEFT_TOP].x		+= nInPixel;
	ptResCorner[E_CORNER_LEFT_TOP].y		+= nInPixel;
	ptResCorner[E_CORNER_RIGHT_TOP].x		-= nInPixel;
	ptResCorner[E_CORNER_RIGHT_TOP].y		+= nInPixel;
	ptResCorner[E_CORNER_LEFT_BOTTOM].x		+= nInPixel;
	ptResCorner[E_CORNER_LEFT_BOTTOM].y		-= nInPixel;
	ptResCorner[E_CORNER_RIGHT_BOTTOM].x	-= nInPixel;
	ptResCorner[E_CORNER_RIGHT_BOTTOM].y	-= nInPixel;

	//////////////////////////////////////////////////////////////////////////
	// Rect 범위 확인용

	// 범위 크게 설정 ( 양 옆으로 200 Pixel 빈 배경 있어야 함 )
	int offset = 70;

	CRect rectROI = new CRect(
		min(ptResCorner[E_CORNER_LEFT_TOP].x,		ptResCorner[E_CORNER_LEFT_BOTTOM].x		) - offset,
		min(ptResCorner[E_CORNER_LEFT_TOP].y,		ptResCorner[E_CORNER_RIGHT_TOP].y		) - offset,
		max(ptResCorner[E_CORNER_RIGHT_TOP].x,		ptResCorner[E_CORNER_RIGHT_BOTTOM].x	) + offset,
		max(ptResCorner[E_CORNER_LEFT_BOTTOM].y,	ptResCorner[E_CORNER_RIGHT_BOTTOM].y	) + offset );

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	//////////////////////////////////////////////////////////////////////////
	// 곡선 처리

	// 원본 영상 8비트 변환
	cv::Mat matSrc_8bit = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false); // 사용X

	if( matSrcBuf.type() == CV_8UC1 )
		matSrcBuf.copyTo(matSrc_8bit);
	else
		matSrcBuf.convertTo(matSrc_8bit, CV_8UC1, 1./16.);

	cv::Mat matCHoleBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1);
	//CHole 사용 유/무
	if (bCHoleUse)
	{
		// Image Save 0 On / -1 Off
		//int nImgSave = -1;
		int nImgSave = (int)dPara[E_PARA_CHOLE_TEXT];

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("CHole FillArea Start."));
		//////////////////////////////////////////////////////////////////////////
		// 기울기 보정 했을 때, 꼭지점 좌표 얻기
		// 등록된 곡선 & ROI 영역은 꼭지점 좌표 기준
		//////////////////////////////////////////////////////////////////////////

		// 회전 좌표 계산 시, 사용
		double	dTheta = -dAngle;
		dTheta *= PI;
		dTheta /= 180.0;
		double	dSin = sin(dTheta);
		double	dCos = cos(dTheta);
		double	dSin_ = sin(-dTheta);
		double	dCos_ = cos(-dTheta);
		int		nCx = matSrcBuf.cols / 2;
		int		nCy = matSrcBuf.rows / 2;

		for (int i = 0; i < E_CORNER_END; i++)
		{
			// 회전 시, 예측 좌표 계산
			ptCornerAlign[i].x = (int)(dCos * (ptResCorner[i].x - nCx) - dSin * (ptResCorner[i].y - nCy) + nCx);
			ptCornerAlign[i].y = (int)(dSin * (ptResCorner[i].x - nCx) + dCos * (ptResCorner[i].y - nCy) + nCy);
		}

		//////////////////////////////////////////////////////////////////////////
		// 등록된 곡선 & ROI 영역 -> 현재 Grab에 맞게 좌표 보정 & 위치 찾기
		//////////////////////////////////////////////////////////////////////////

		// 재계산
		dTheta = dAngle;
		dTheta *= PI;
		dTheta /= 180.0;
		dSin = sin(dTheta);
		dCos = cos(dTheta);
		dSin_ = sin(-dTheta);
		dCos_ = cos(-dTheta);

		for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
		{
			// 곡선 개수 없으면, 영역 등록 안된것으로 판단 / 다음으로...
			if (tCHoleSet[i].nContourCount <= 0)	continue;

			cv::Point2i ptTemp;

			// 점등영역 밖에 있는 꼭지점 개수
			int nOutsideCnt = 0;
			for (int j = 0; j < E_CORNER_END; j++)
			{
				if (!tCHoleSet[i].nCornerInside[j])
					nOutsideCnt++;
			}

			const int nCount = (int)tCHoleSet[i].nContourCount;
			cv::Point *ptPoly = new cv::Point[nCount];
			memset(ptPoly, 0, sizeof(cv::Point) * nCount);

			// Align 꼭지점에서 제일 가까운 꼭지점
			int nCornerAlign = tCHoleSet[i].nCornerMinLength;

			int nMaxX = 0, nMinX = matSrc_8bit.cols;
			int nMaxY = 0, nMinY = matSrc_8bit.rows;

			// 곡선 개수 존재하는 경우
			for (int j = 0; j < tCHoleSet[i].nContourCount; j++)
			{
				// 얼라인 좌표부터, 곡선 설정 영역까지 최단 거리... 보정
				int XX = tCHoleSet[i].ptContours[j].x + ptCornerAlign[nCornerAlign].x;
				int YY = tCHoleSet[i].ptContours[j].y + ptCornerAlign[nCornerAlign].y;

				// 회전 시, 예측 좌표 계산
				ptTemp.x = (int)(dCos * (XX - nCx) - dSin * (YY - nCy) + nCx);
				ptTemp.y = (int)(dSin * (XX - nCx) + dCos * (YY - nCy) + nCy);

				// 보정된 좌표 넣기
				ptPoly[j].x = ptTemp.x;
				ptPoly[j].y = ptTemp.y;

				matCHoleBuf.at<uchar>(ptPoly[j].y, ptPoly[j].x) = (uchar)255;

				if (nMaxX < ptPoly[j].x) nMaxX = ptPoly[j].x;
				if (nMinX > ptPoly[j].x) nMinX = ptPoly[j].x;
				if (nMaxY < ptPoly[j].y) nMaxY = ptPoly[j].y;
				if (nMinY > ptPoly[j].y) nMinY = ptPoly[j].y;
			}

			int nDustOffset = 20;

			rcCHoleROI[i].x = nMinX - nDustOffset;
			rcCHoleROI[i].y = nMinY - nDustOffset;
			rcCHoleROI[i].width = (nMaxX - nMinX + 1) + nDustOffset * 2;
			rcCHoleROI[i].height = (nMaxY - nMinY + 1) + nDustOffset * 2;
		}
		if (nImgSave >= 1)
		{
			CString strTemp;
			strTemp.Format(_T("D:\\CHole\\%s_%02d_%02d_Dust_CHole_src.bmp"), strID, nAlgImg, nImgSave++);
			cv::imwrite((cv::String)(CStringA)strTemp, matSrcBuf);
		}
	}

	// 마스크
	cv::Mat matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1);

	int npt[] = { E_CORNER_END };

	// 곡선 사용 안할때 ( Rect 인 경우 )
	if( !bRoundUse )
	{
		const cv::Point* ppt[1] = { ptResCorner };

		// 점등 영역 마스크
		cv::fillPoly(matTempBuf, ppt, npt, 1, cv::Scalar(255, 255, 255));
	}

	// 곡선 부분 처리
	else
	{
		// Temp 메모리 할당
		cv::Mat matRndBuf = cMem->GetMat(matTempBuf.size(), matTempBuf.type());

		//////////////////////////////////////////////////////////////////////////
		// 기울기 보정 했을 때, 꼭지점 좌표 얻기
		// 등록된 곡선 & ROI 영역은 꼭지점 좌표 기준
		//////////////////////////////////////////////////////////////////////////

		// 회전 좌표 계산 시, 사용
		double	dTheta	= -dAngle;
		dTheta	*= PI;
		dTheta	/= 180.0;
		double	dSin	= sin( dTheta );
		double	dCos	= cos( dTheta );
		double	dSin_	= sin( -dTheta );
		double	dCos_	= cos( -dTheta );
		int		nCx		= matSrcBuf.cols / 2;
		int		nCy		= matSrcBuf.rows / 2;

		for (int i=0 ; i<E_CORNER_END ; i++)
		{
			// 회전 시, 예측 좌표 계산
			ptCornerAlign[i].x = (int)( dCos * (ptResCorner[i].x - nCx) - dSin * (ptResCorner[i].y - nCy) + nCx );
			ptCornerAlign[i].y = (int)( dSin * (ptResCorner[i].x - nCx) + dCos * (ptResCorner[i].y - nCy) + nCy );
		}

		//////////////////////////////////////////////////////////////////////////
		// 등록된 곡선 & ROI 영역 -> 현재 Grab에 맞게 좌표 보정 & 위치 찾기
		//////////////////////////////////////////////////////////////////////////

		// 재계산
		dTheta	= dAngle;
		dTheta	*= PI;
		dTheta	/= 180.0;
		dSin	= sin( dTheta );
		dCos	= cos( dTheta );
		dSin_	= sin( -dTheta );
		dCos_	= cos( -dTheta );

		cv::Point	ptSE[2], ptTempLine[2];
		int			nSE;

		for (int i=0; i<MAX_MEM_SIZE_E_INSPECT_AREA ; i++)
		{
			// 곡선 개수 없으면, 영역 등록 안된것으로 판단 / 다음으로...
			if( tRoundSet[i].nContourCount <= 0 )	continue;

			cv::Point2i ptTemp;

			// 점등영역 밖에 있는 꼭지점 개수
			int nOutsideCnt = 0;
			for (int j=0 ; j<E_CORNER_END ; j++)
			{
				if( !tRoundSet[i].nCornerInside[j] )
					nOutsideCnt++;
			}

			// 한개도 없다면 설정 에러임
// 			if( nOutsideCnt == 0 )
// 				AfxMessageBox(_T("Set Corner Err !!!"));

			const int nCount	= (int)tRoundSet[i].nContourCount;
			cv::Point *ptPoly	= new cv::Point[ nCount ];
			memset(ptPoly, 0, sizeof(cv::Point) * nCount );

			// Align 꼭지점에서 제일 가까운 꼭지점
			int nCornerAlign = tRoundSet[i].nCornerMinLength;

			// 곡선 개수 존재하는 경우
			for (int j=0 ; j<tRoundSet[i].nContourCount ; j++)
			{
				// 얼라인 좌표부터, 곡선 설정 영역까지 최단 거리... 보정
				int XX = tRoundSet[i].ptContours[j].x + ptCornerAlign[nCornerAlign].x;
				int YY = tRoundSet[i].ptContours[j].y + ptCornerAlign[nCornerAlign].y;

				// 회전 시, 예측 좌표 계산
				ptTemp.x = (int)( dCos * (XX - nCx) - dSin * (YY - nCy) + nCx );
				ptTemp.y = (int)( dSin * (XX - nCx) + dCos * (YY - nCy) + nCy );

				// 보정된 좌표 넣기
				ptPoly[j].x = ptTemp.x;
				ptPoly[j].y = ptTemp.y;				
			}

			// 곡선 부분, 점등영역 안쪽으로 살짝 넣기 ( 상황에 따라 다름 )
			cv::Point ptRnd = calcRoundIn(tRoundSet, i, nRoundIn);

			// 위치 수정
			int j = 0;
			for ( ; j<tRoundSet[i].nContourCount ; j++)
			{
				ptPoly[j].x += ptRnd.x;
				ptPoly[j].y += ptRnd.y;

				// 예외처리
				if( ptPoly[j].x < 0 )					return E_ERROR_CODE_ALIGN_IMAGE_OVER;
				if( ptPoly[j].y < 0 )					return E_ERROR_CODE_ALIGN_IMAGE_OVER;
				if( ptPoly[j].x >= matRndBuf.cols )		return E_ERROR_CODE_ALIGN_IMAGE_OVER;
				if( ptPoly[j].y >= matRndBuf.rows )		return E_ERROR_CODE_ALIGN_IMAGE_OVER;

				matRndBuf.at<uchar>(ptPoly[j].y, ptPoly[j].x) = (uchar)255;
			}

			//////////////////////////////////////////////////////////////////////////
			// 곡선 이외 직선 연결
			//////////////////////////////////////////////////////////////////////////

			// 끝지점
			ptTempLine[0] = ptPoly[0];
			ptTempLine[1] = ptPoly[nCount-1];

			delete ptPoly;
			ptPoly = NULL;

			// 끝지점 등록
			if( i == 0 )
			{
				ptSE[0] = ptTempLine[0];
				ptSE[1] = ptTempLine[1];
			}
			// 끝지점 연결
			else
			{
				// 곡선 이외 직선 연결
				calcLineConnect(matRndBuf, ptSE, ptTempLine, nSE, i);
			}			
		}
		// 마지막 끝지점 연결
		cv::line(matRndBuf, ptSE[0], ptSE[1], cv::Scalar(255, 255, 255));

		// KSW 17.11.12 - 끊어진 부분 잇기
		cv::dilate(matRndBuf, matTempBuf, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)) );

		// 외곽선 안쪽으로 채우기 ( 4방향? )
		cv::floodFill(matTempBuf, cv::Point(nWidth/2, nHeight/2), cv::Scalar(255, 255, 255));

		matRndBuf.release();
	}	
	// KSW 17.07.17 곡선 부분 처리
	//////////////////////////////////////////////////////////////////////////

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Corner Find."));

	cv::Mat matEdgeArea = cMem->GetMat(matTempBuf.size(), CV_8UC1, false);

	// 점등 영역 외곽 조금 남기기 ( 점등 영역 평균 시, 사용 )
	nErrorCode = FindEdgeArea(matTempBuf, matEdgeArea, offset, cMem);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\0.EdgeArea.bmp"), matEdgeArea);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Area Find."));

	// 평균 버퍼 메모리 할당
	cv::Mat matMeanBuf = cMem->GetMat(matSrcBuf.size(), matSrcBuf.type(), false);
	matSrcBuf.copyTo(matMeanBuf);

	// Offset ( 점등 영역 평균 크기 )
	// nSeg x nSeg 영역 안에, 점등 영역이 존재해야 함
	// 6.18"의 경우, 크기 늘려야 함 ( 300 -> 500 )
	// 좌우상하 offset(현재 70) 만큼 늘어난 상태
	// x, y 분할하여 사용
	int nSegX = 3;
	int nSegY = offset + offset/2;

	// 가로 방향 평균 채우기
	nErrorCode = FillAreaMeanX(matMeanBuf, matEdgeArea, rectROI, nSegX, nSegY, nRoundMinGV);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\1.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\1.EdgeArea.bmp"), matEdgeArea);


	// Offset ( 점등 영역 평균 크기 )
	// nSeg x nSeg 영역 안에, 점등 영역이 존재해야 함
	// 6.18"의 경우, 크기 늘려야 함 ( 300 -> 500 )
	// 좌우상하 offset(현재 70) 만큼 늘어난 상태
	// x, y 분할하여 사용
	nSegX = 800;
	nSegY = 3;

	// 세로 방향 평균 채우기
	nErrorCode = FillAreaMeanY(matMeanBuf, matEdgeArea, rectROI, nSegX, nSegY, nRoundMinGV);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\2.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\2.EdgeArea.bmp"), matEdgeArea);
	//cv::imwrite(("D:\\2.Temp.bmp"), matTempBuf);

	// 점등된 부분만 남기기
	nErrorCode = FillMerge(matSrcBuf, matMeanBuf, matTempBuf, nAlgImg ,cMem);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Fill Average."));

	//cv::imwrite(("D:\\3.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\3.EdgeArea.bmp"), matEdgeArea);

	// 메모리 해제
	matMeanBuf.release();
	matTempBuf.release();

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));
	
// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}


	return E_ERROR_CODE_TRUE;
}


// 영상 회전
long CInspectAlign::DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle)
{
	// 버퍼가 없는 경우. 에러 코드
	if( matSrcBuffer.empty() )
		return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat matTempBuffer = cv::Mat::zeros(matSrcBuffer.size(), CV_8UC1);

	// 회전 매트릭스 생성
	cv::Mat matRotation = cv::getRotationMatrix2D( Point(matSrcBuffer.cols/2, matSrcBuffer.rows/2), dAngle, 1.0 );

	// 영상 회전
	cv::warpAffine( matSrcBuffer, matTempBuffer, matRotation, matTempBuffer.size() );

	// 할당 해제 후, 결과 넣기
	if( !matDstBuffer.empty() )
		matDstBuffer.release();

	matDstBuffer = matTempBuffer.clone();

	// 메모리 할당 해제
	matTempBuffer.release();
	matRotation.release();

	return E_ERROR_CODE_TRUE;
}


// 좌표 회전
long CInspectAlign::DoRotatePoint(cv::Point ptSrcPoint, cv::Point& ptDstPoint, cv::Point ptCenter, double dAngle)
{
	// OpenCV 영상 회전 반대방향 ( 부호 "-" 설정 )
	double dTheta = -dAngle * PI / 180.;

	double dSin = sin( dTheta );
	double dCos = cos( dTheta );

	// 회전 시, 예측 좌표 계산
	int X = (int)( dCos * (ptSrcPoint.x - ptCenter.x) - dSin * (ptSrcPoint.y - ptCenter.y) + ptCenter.x );
	int Y = (int)( dSin * (ptSrcPoint.x - ptCenter.x) + dCos * (ptSrcPoint.y - ptCenter.y) + ptCenter.y );

	// 결과 넣기
	ptDstPoint.x = X;
	ptDstPoint.y = Y;

	return E_ERROR_CODE_TRUE;
}

// AVI AD 검사 / dResult : 현재 Cell 일치율
long CInspectAlign::DoFindDefectAD(cv::Mat matSrcBuf, double* dPara, double* dResult, int nRatio)
{
	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	
	// 파라미터 NULL인 경우.
	if( dPara == NULL )			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	cv::Mat matGrayBuf;

	// 버퍼 할당 & 초기화
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), matSrcBuf.type(), false);

	// 컬러인 경우 ( SVI )
	if( matSrcBuf.channels() != 1 )
	{
		//cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
		//	김형주D 수정.	17.07.07
		//	강선우가 짠 소스에 묻어가는 중.
		//	조우현D 와 상의한 결과.
		//	AD 및 Align 관련 시퀀스상의 tac time 이 현재는 크게 문제가 될 것 같지 않으므로 그냥 이 구조 사용.
		//	추 후 AD 검사 관련해서 더 세분화 될 경우 SVI 와 AVI 함수 독립 예정.

		cv::Mat matLab;
		cv::cvtColor(matSrcBuf, matLab, CV_BGR2Lab);
		vector<Mat> vLab(3);
		cv::split(matLab, vLab);
		vLab.at(0).convertTo( matGrayBuf, CV_8UC1, 2.55);

		if( !matLab.empty() )			matLab.release();
		if( !vLab.at(0).empty() )		vLab.at(0).release();
		if( !vLab.at(1).empty() )		vLab.at(1).release();
		if( !vLab.at(2).empty() )		vLab.at(2).release();
		vector<Mat>().swap( vLab );
	}
	// 흑백인 경우 ( AVI, APP )
	else
		matSrcBuf.copyTo(matGrayBuf);
		//matGrayBuf = matSrcBuf.clone();

	//////////////////////////////////////////////////////////////////////////
	// KSW 17.08.07 - 점등이 아예 안된경우 나가기
	//////////////////////////////////////////////////////////////////////////

	// stdDev 구하기
	cv::Scalar m, s;
	cv::meanStdDev(matGrayBuf, m, s);

	// 1 : 평균 밝기 (Mean GV)
	dResult[1] = double(m[0]);

	// 2 : 표준편차 (Std)
	dResult[2] = double(s[0]);

	// 노이즈 있을수도 있으므로.... ( 해봤자 0.xxx 나올 듯... )
	// 전체 영상 평균밝기 1보다 작으면 점등안된것으로 판단
	if( dResult[1] < 1.0 )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_OFF;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	double	dRate	= dPara[E_PARA_AD_RATE];
	double	dMinGV	= dPara[E_PARA_AD_MIN_GV];
	double	dMaxGV	= dPara[E_PARA_AD_MAX_GV];
	double	dMaxStd	= dPara[E_PARA_AD_MAX_STD];
	int		nLineYCount = (int)dPara[E_PARA_AD_Y_LINE];

	// Block 확인
	int		nBlockGV_X		= (int)(dPara[E_PARA_AD_BLOCK_X_GV]);
	float	fBlockArea_X	= (float)(dPara[E_PARA_AD_BLOCK_X_AREA]);
	int		nBlockGV_Y		= (int)(dPara[E_PARA_AD_BLOCK_Y_GV]);
	float	fBlockArea_Y	= (float)(dPara[E_PARA_AD_BLOCK_Y_AREA]);

	double	dZoom	= dPara[E_PARA_AD_ZOOM];
	int		nModelX	= (int)dPara[E_PARA_CELL_SIZE_X] * nRatio;
	int		nModelY	= (int)dPara[E_PARA_CELL_SIZE_Y] * nRatio;

	// 파라미터 이상한 경우
	if( dZoom < 1 )	return E_ERROR_CODE_ALIGN_WARNING_PARA;

	// 사용 안하는 경우 나가기
	if( dRate <= 0 )
	{
		dResult[0] = 100.0;
		dResult[1] = 0;
		dResult[2] = 0;

		return E_ERROR_CODE_TRUE;
	}

	// 축소된 이미지 크기
	int nWidth	= matGrayBuf.cols / dZoom;
	int nHeight	= matGrayBuf.rows / dZoom;

	// 원본 크기 축소
	cv::Mat matResizeBuf, matResBuf, matModelBuf;

	// 버퍼 할당 & 초기화
	matResizeBuf	= cMem->GetMat(nHeight, nWidth, matSrcBuf.type(), false);
	matResBuf		= cMem->GetMat(nHeight, nWidth, CV_32FC1, false);
	matModelBuf		= cMem->GetMat((int)(nModelY/dZoom), (int)(nModelX/dZoom), CV_8UC1, false);

	cv::resize(matGrayBuf, matResizeBuf, cv::Size(nWidth, nHeight));

	// 일정 밝기 모델 생성 ( Gray Value : 50 )
	matModelBuf.setTo(50);

	if( matResizeBuf.type() == CV_8UC1 )
	{
		// 매칭
		cv::matchTemplate(matResizeBuf, matModelBuf, matResBuf, CV_TM_CCORR_NORMED);
	}
	else
	{
		// 12bit --> 8bit 변환
		cv::Mat matTemp_8bit = cMem->GetMat(nHeight, nWidth, CV_8UC1, false);
		matResizeBuf.convertTo(matTemp_8bit, CV_8UC1, 1./16.);

		// 매칭
		cv::matchTemplate(matTemp_8bit, matModelBuf, matResBuf, CV_TM_CCORR_NORMED);

		matTemp_8bit.release();
	}	

	// 매칭률
	//cv::minMaxLoc(matResBuf, NULL, &dResult);

	int xx=0, yy=0;
	dResult[0] = 0.0;
	for (int y=0 ; y<matResBuf.rows ; y++)
	{
		float *ptr = (float *)matResBuf.ptr(y);
		for (int x=0 ; x<matResBuf.cols ; x++, ptr++)
		{
			if( dResult[0] < *ptr )
			{
				dResult[0] = *ptr;
				xx = x;
				yy = y;
			}
		}
	}

	// 0 : 일치율 (Rate)
	// 퍼센트로 변경
	dResult[0] *= 100.0;

	// 메모리 할당 해제
	matResizeBuf.release();
	matResBuf.release();
	matModelBuf.release();
	matGrayBuf.release();

	//////////////////////////////////////////////////////////////////////////
	// 점등 영역을 못찾는 경우
	// 검사하고자 하는 밝기가 아닌 경우
	//////////////////////////////////////////////////////////////////////////

	// 최대값 매칭율이 설정한 값보다 낮은경우 디스플레이 이상
	if( dResult[0] < dRate )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 매칭 된 부분 통계 검사
	cv::Rect rect(xx*dZoom, yy*dZoom, nModelX, nModelY);

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rect.x	<	0								||
		rect.y	<	0								||
		rect.x + rect.width		>= 	matSrcBuf.cols	||
		rect.y + rect.height	>= 	matSrcBuf.rows	)		return E_ERROR_CODE_ALIGN_IMAGE_OVER;
		
	if( rect.width <= 1 )									return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rect.height <= 1 )									return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// 매칭된 부분 원본 자르기
	matGrayBuf = matSrcBuf(rect);

	// stdDev 구하기
	cv::meanStdDev(matGrayBuf, m, s);
	
	// 1 : 평균 밝기 (Mean GV)
	dResult[1] = double(m[0]);

	// 2 : 표준편차 (Std)
	dResult[2] = double(s[0]);

	// 편차가 심한경우, 디스플레이 이상
	if (dResult[2] > dMaxStd)
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 밝기가 밝은 경우, 디스플레이 이상
	if (dResult[1] > dMaxGV)
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 밝기가 어두운 경우, 디스플레이 이상
	if (dResult[1] < dMinGV)
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.11.05 - 컨텍 불량으로 인해 세로 라인이 너무 많은 경우 컨텍 이상
	//////////////////////////////////////////////////////////////////////////

	// 매칭된 부분 원본 자르기
	//cv::Mat matROIBuf = matGrayBuf(rect);

	// 노이즈 제거
	int nGauSize = 31;
	double dGauSig = 2;
	cv::Mat matGauBuf = cMem->GetMat(matGrayBuf.size(), matGrayBuf.type(), false);
	cv::GaussianBlur(matGrayBuf, matGauBuf, cv::Size(nGauSize, nGauSize), dGauSig, dGauSig);
	//cv::imwrite("D:\\01.Gau.bmp", matGauBuf);

	// Edge
	cv::Mat matEdgeBuf = cMem->GetMat(matGauBuf.size(), matGauBuf.type(), false);
	cv::Sobel(matGauBuf, matEdgeBuf, CV_8UC1, 1, 0);
	//cv::imwrite("D:\\02.Edge.bmp", matEdgeBuf);

	// 기준값
	float fAvg = (float)(cv::mean(matEdgeBuf)[0] * 2.75f);

	// 세로방향 누적 평균
	matGauBuf.setTo(0);
	int nCount = 0;
	for (int x = 0; x < matEdgeBuf.cols; x++)
	{
		float fTemp = (float)cv::sum(matEdgeBuf.col(x))[0] / (float)matEdgeBuf.rows;

		if (fTemp > fAvg)
		{
			cv::line(matGauBuf, cv::Point(x, 0), cv::Point(x, matGauBuf.rows), cv::Scalar(255), 1);
			nCount++;
		}
	}
	//cv::imwrite("D:\\03.Line.bmp", matGauBuf);

	matEdgeBuf.release();
	matGauBuf.release();

	// 세로 라인이 너무 많으면 컨텍 이상으로 판단
	if (nLineYCount > 0			&&
		nCount > nLineYCount	)
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}


	//////////////////////////////////////////////////////////////////////////
	// 영역이 2분할 되어 밝기가 다른 경우
	//////////////////////////////////////////////////////////////////////////

	matModelBuf = cMem->GetMat(matGrayBuf.size(), matGrayBuf.type(), false);
	cv::blur(matGrayBuf, matModelBuf, cv::Size(5, 5));

	matResizeBuf = cMem->GetMat(cv::Size(matModelBuf.cols/10, matModelBuf.rows/10), matModelBuf.type(), false);
	cv::resize(matModelBuf, matResizeBuf, matResizeBuf.size());

	// 배열 초기화
	int *nCntX = new int[matResizeBuf.cols];
	int *nCntY = new int[matResizeBuf.rows];
	memset(nCntX, 0, sizeof(int) * matResizeBuf.cols);
	memset(nCntY, 0, sizeof(int) * matResizeBuf.rows);


	// 가로방향 누적
	int nAvgY = 0;
	int nAvgY_Up = 0;
	int nAvgY_Down = 0;
	int nCntY_Up = 0;
	int nCntY_Down = 0;

	// 세로방향 누적
	int nAvgX = 0;
	int nAvgX_Up = 0;
	int nAvgX_Down = 0;
	int nCntX_Up = 0;
	int nCntX_Down = 0;

#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int i = 0; i < 2; i++)
	{
		switch (i)
		{
		case 0:
		{
			nAvgY = 0;
			for (int y = 0; y < matResizeBuf.rows; y++)
			{
				nCntY[y] = (int)(cv::sum(matResizeBuf.row(y))[0] / matResizeBuf.cols);
				nAvgY += nCntY[y];
			}
			nAvgY /= matResizeBuf.rows;
		}
			break;
		case 1:
		{
			nAvgX = 0;
			for (int x = 0; x < matResizeBuf.cols; x++)
			{
				nCntX[x] = (int)(cv::sum(matResizeBuf.col(x))[0] / matResizeBuf.rows);
				nAvgX += nCntX[x];
			}
			nAvgX /= matResizeBuf.cols;
		}
			break;
		}
	}

#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int i = 0; i < 2; i++)
	{
		switch (i)
		{
		case 0:
		{
			nAvgY_Up = 0;
			nAvgY_Down = 0;
			nCntY_Up = 0;
			nCntY_Down = 0;

			for (int y = 0; y < matResizeBuf.rows; y++)
			{
				if (nCntY[y] >= nAvgY)
				{
					nAvgY_Up += nCntY[y];
					nCntY_Up++;
				}
				else
				{
					nAvgY_Down += nCntY[y];
					nCntY_Down++;
				}
			}
			if (nCntY_Up > 0)	nAvgY_Up /= nCntY_Up;
			if (nCntY_Down > 0)	nAvgY_Down /= nCntY_Down;
		}
			break;
		case 1:
		{
			nAvgX_Up = 0;
			nAvgX_Down = 0;
			nCntX_Up = 0;
			nCntX_Down = 0;
			for (int x = 0; x < matResizeBuf.cols; x++)
			{
				if (nCntX[x] >= nAvgX)
				{
					nAvgX_Up += nCntX[x];
					nCntX_Up++;
				}
				else
				{
					nAvgX_Down += nCntX[x];
					nCntX_Down++;
				}
			}
			if (nCntX_Up > 0)	nAvgX_Up /= nCntX_Up;
			if (nCntX_Down > 0)	nAvgX_Down /= nCntX_Down;
		}
			break;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////

	// 메모리 할당 해제
	delete[] nCntX;		nCntX		= NULL;
	delete[] nCntY;		nCntY		= NULL;
	matModelBuf.release();
	matGrayBuf.release();
	matResizeBuf.release();

	// GV 차이가 큰 경우
	if( nAvgX_Up - nAvgX_Down	> nBlockGV_X	)
	{
		int nTemp = ( nCntX_Up > nCntX_Down ) ? nCntX_Down : nCntX_Up;

		// 면적 비율이 20% 이상인 경우
		if( nTemp/(float)(nCntX_Up + nCntX_Down) > fBlockArea_X )
		{
			dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
			return E_ERROR_CODE_ALIGN_DISPLAY;
		}
	}

	// GV 차이가 큰 경우
	if( nAvgY_Up - nAvgY_Down	> nBlockGV_Y	)
	{
		int nTemp = ( nCntY_Up > nCntY_Down ) ? nCntY_Down : nCntY_Up;

		// 면적 비율이 20% 이상인 경우
		if( nTemp/(float)(nCntY_Up + nCntY_Down) > fBlockArea_Y )
		{
			dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
			return E_ERROR_CODE_ALIGN_DISPLAY;
		}		
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// SVI AD 검사 / dResult : 현재 Cell 일치율
long CInspectAlign::DoFindDefectAD_SVI(cv::Mat matSrcBuf, double* dPara, double* dResult, int nCameraNum, int nRatio)
{
	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 파라미터 NULL인 경우.
	if( dPara == NULL )			return E_ERROR_CODE_ALIGN_WARNING_PARA;
	
	//////////////////////////////////////////////////////////////////////////
	// 점등이 아예 안된경우 나가기
	//////////////////////////////////////////////////////////////////////////

	// Color -> Gray
	cv::Mat matGrayBuf;
	cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);

	// stdDev 구하기
	cv::Scalar m, s;
	cv::meanStdDev(matGrayBuf, m, s);

	// 1 : 평균 밝기 (Mean GV)
	dResult[1] = double(m[0]);

	// 2 : 표준편차 (Std)
	dResult[2] = double(s[0]);

	// 노이즈 있을수도 있으므로.... ( 해봤자 0.xxx 나올 듯... )
	// 전체 영상 평균밝기 1보다 작으면 점등안된것으로 판단
	if( dResult[1] < 1.0 )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_OFF;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	double	dRate	= dPara[E_PARA_SVI_AD_RATE];
	double	dMinGV	= dPara[E_PARA_SVI_AD_MIN_GV];
	double	dMaxGV	= dPara[E_PARA_SVI_AD_MAX_GV];
	double	dMaxStd	= dPara[E_PARA_SVI_AD_MAX_STD];

	double	dZoom	= dPara[E_PARA_SVI_AD_ZOOM];
	int		nModelX	= (int)dPara[E_PARA_SVI_CELL_COAX_SIZE_X] * nRatio;
	int		nModelY	= (int)dPara[E_PARA_SVI_CELL_COAX_SIZE_Y] * nRatio;

	// Side 카메라
	if( nCameraNum == 1 )
	{
		nModelX	= (int)dPara[E_PARA_SVI_CELL_SIDE_SIZE_X] * nRatio;
		nModelY	= (int)dPara[E_PARA_SVI_CELL_SIDE_SIZE_Y] * nRatio;
	}

	// 파라미터 이상한 경우
	if( dZoom < 1 )	return E_ERROR_CODE_ALIGN_WARNING_PARA;

	// 사용 안하는 경우 나가기
	if( dRate <= 0 )
	{
		dResult[0] = 100.0;
		dResult[1] = 0;
		dResult[2] = 0;
		return E_ERROR_CODE_TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	// Cell 위치 찾기
	//////////////////////////////////////////////////////////////////////////

	// 축소된 이미지 크기
	int nWidth	= matGrayBuf.cols / dZoom;
	int nHeight	= matGrayBuf.rows / dZoom;

	// 원본 크기 축소
	cv::Mat matResizeBuf;
	cv::resize(matGrayBuf, matResizeBuf, cv::Size(nWidth, nHeight));

	// 일정 밝기 모델 생성 ( Gray Value : 50 )
	cv::Mat matModelBuf = cv::Mat::zeros((int)(nModelY/dZoom), (int)(nModelX/dZoom), CV_8UC1);
	matModelBuf.setTo(50);

	// 매칭
	cv::Mat matResBuf = cv::Mat::zeros(nHeight, nWidth, CV_32FC1);
	cv::matchTemplate(matResizeBuf, matModelBuf, matResBuf, CV_TM_CCORR_NORMED);

	// 매칭률
	//cv::minMaxLoc(matResBuf, NULL, &dResult);

	int xx=0, yy=0;
	dResult[0] = 0.0;
	for (int y=0 ; y<matResBuf.rows ; y++)
	{
		float *ptr = (float *)matResBuf.ptr(y);
		for (int x=0 ; x<matResBuf.cols ; x++, ptr++)
		{
			if( dResult[0] < *ptr )
			{
				dResult[0] = *ptr;
				xx = x;
				yy = y;
			}
		}
	}

	// 0 : 일치율 (Rate)
	// 퍼센트로 변경
	dResult[0] *= 100.0;

	// 메모리 할당 해제
	matResizeBuf.release();
	matResBuf.release();
	matModelBuf.release();
	matGrayBuf.release();

	// 최대값 매칭율이 설정한 값보다 낮은경우 디스플레이 이상
	if( dResult[0] < dRate )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 매칭 된 부분 통계 검사
	cv::Rect rect(xx*dZoom, yy*dZoom, nModelX, nModelY);

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rect.x	<	0								||
		rect.y	<	0								||
		rect.x + rect.width		>= 	matSrcBuf.cols	||
		rect.y + rect.height	>= 	matSrcBuf.rows	)		return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	if( rect.width <= 1 )									return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rect.height <= 1 )									return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// 매칭된 부분 원본 자르기
	matGrayBuf = matSrcBuf(rect);

	// stdDev 구하기
	cv::meanStdDev(matGrayBuf, m, s);

	// 1 : 평균 밝기 (Mean GV)
	dResult[1] = double(m[0]);

	// 2 : 표준편차 (Std)
	dResult[2] = double(s[0]);

	matGrayBuf.release();


	//////////////////////////////////////////////////////////////////////////
	// AD 체크
	//////////////////////////////////////////////////////////////////////////

	// 편차가 심한경우, 디스플레이 이상
	if( dResult[2] > dMaxStd )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 밝기가 밝은 경우, 디스플레이 이상
	if( dResult[1] > dMaxGV )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 밝기가 어두운 경우, 디스플레이 이상
	if( dResult[1] < dMinGV )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}	

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::DoFindDefectAD_APP(cv::Mat MatOrgImage, double* dAlgPara, double* dResult, int nRatio)
{
	long nErrorCode;

	if(MatOrgImage.empty())
		nErrorCode = E_ERROR_CODE_EMPTY_BUFFER;

	cv::Scalar m, s;
	cv::meanStdDev(MatOrgImage, m, s);

	if(m[0] == 0)
		return E_ERROR_CODE_EMPTY_BUFFER;

	int nBlurSize			= (int)dAlgPara[E_PARA_APP_AD_BLUR_SIZE];		//21
	int nThreshold			= (int)dAlgPara[E_PARA_APP_AD_THRESHOLD];		//20
	double dLimitSize		= dAlgPara[E_PARA_APP_AD_LIMIT_AREA];			//10000
	double dCompare_Theta	= (double)dAlgPara[E_PARA_APP_PAD_EDGE_THETA];
	int	nADGV				= (int)dAlgPara[E_PARA_APP_AD_GV];

	if(nBlurSize%2==0)
		nBlurSize++;


	cv::Mat temp;
	cv::blur(MatOrgImage, temp, cv::Size(nBlurSize,nBlurSize));
	Mat mtThresh;
	cv::threshold(temp, mtThresh, nThreshold, 255, CV_THRESH_BINARY);
	bool flag = false;

	vector<vector<cv::Point> > contours;

	FindBiggestContour(mtThresh, contours);
	
	if(contours.size() < 1)
	{
		return E_ERROR_CODE_FALSE;
	}

	double size = contourArea(contours[0]);
	if(size > dLimitSize)	flag = true;

	double dImageArea = MatOrgImage.cols * MatOrgImage.rows;

	//영상의 95%이상 Panel이 차지 하고 있으면 Display 이상 처리
	if(size / dImageArea > 0.95)
		return E_ERROR_CODE_FALSE;

	Rect rtObject = boundingRect(contours[0]);

	vector<cv::Rect> pADCheck;
	pADCheck.push_back(Rect(rtObject.x, rtObject.y, rtObject.width/4, rtObject.height));   //왼쪽 조명
	pADCheck.push_back(Rect(rtObject.x + rtObject.width*3/4, rtObject.y, rtObject.width/4, rtObject.height)); //오른쪽 조명
	pADCheck.push_back(Rect(rtObject.x, rtObject.y, rtObject.width, rtObject.height/4));   //윗쪽 조명
	pADCheck.push_back(Rect(rtObject.x, rtObject.y + rtObject.height*3/4, rtObject.width, rtObject.height/4)); //아래쪽 조명

	//일부 조명 이상 Check
	for(int i=0; i<pADCheck.size(); i++)
	{	
		Scalar m, s;
		GetCheckROIOver(pADCheck[i], Rect(0,0,MatOrgImage.cols,MatOrgImage.rows), pADCheck[i]);
		Mat mtLightcheck;
		MatOrgImage(pADCheck[i]).copyTo(mtLightcheck);
		cv::meanStdDev(mtLightcheck, m, s);
		if(m[0]<nADGV)
			flag = false;
	}

	if(flag == true)	nErrorCode = E_ERROR_CODE_TRUE;
	else				nErrorCode = E_ERROR_CODE_FALSE;

	return nErrorCode;
}

long CInspectAlign::Check_Abnormal_PADEdge(cv::Mat MatOrgImage, int nThreshold, double dCompare_Theta, Rect rtObject)
{
	//Pad Edge부분 Cutting이상 확인 

	//Object 외곽 영역으로 나가 Edge Search할 영역
	Rect rtSearchArea = Rect(rtObject.x, rtObject.y + 100, rtObject.width + 100, rtObject.height - 100);

	GetCheckROIOver(rtSearchArea, Rect(0,0,MatOrgImage.cols,MatOrgImage.rows),rtSearchArea);

	vector<cv::Point> pt[E_ABNORMAL_PAD_EDGE];

	uchar* ucImgData;
	int nGV;
	for(int nY = rtSearchArea.y; nY < rtSearchArea.height; nY+=10 )
	{
		ucImgData = MatOrgImage.data + (nY * MatOrgImage.step);
		for(int nX = rtSearchArea.width; nX >= 0; nX--)
		{
			nGV = (int)*(nX+ucImgData);
			if(nGV==255)
			{
				pt[E_ABNORMAL_PAD_EDGE_TOTAL].push_back(cv::Point(nX, nY));
				break;
			}
		}
	}

	//벡터 복사해서 Top, Bottom, Middle 3개로 나눔
	pt[E_ABNORMAL_PAD_EDGE_TOP]    = pt[E_ABNORMAL_PAD_EDGE_TOTAL];
	pt[E_ABNORMAL_PAD_EDGE_BOTTOM] = pt[E_ABNORMAL_PAD_EDGE_TOTAL];
	pt[E_ABNORMAL_PAD_EDGE_MIDDLE] = pt[E_ABNORMAL_PAD_EDGE_TOTAL];

	pt[E_ABNORMAL_PAD_EDGE_TOP].erase(pt[E_ABNORMAL_PAD_EDGE_TOP].begin()+ pt[E_ABNORMAL_PAD_EDGE_TOP].size()/2 
		,pt[E_ABNORMAL_PAD_EDGE_TOP].end());
	pt[E_ABNORMAL_PAD_EDGE_BOTTOM].erase(pt[E_ABNORMAL_PAD_EDGE_BOTTOM].begin() 
		,pt[E_ABNORMAL_PAD_EDGE_BOTTOM].begin() + pt[E_ABNORMAL_PAD_EDGE_BOTTOM].size()/2);
	pt[E_ABNORMAL_PAD_EDGE_MIDDLE].erase(pt[E_ABNORMAL_PAD_EDGE_MIDDLE].begin() 
		,pt[E_ABNORMAL_PAD_EDGE_MIDDLE].begin() + pt[E_ABNORMAL_PAD_EDGE_TOTAL].size()/3);
	pt[E_ABNORMAL_PAD_EDGE_MIDDLE].erase(pt[E_ABNORMAL_PAD_EDGE_MIDDLE].end()-pt[E_ABNORMAL_PAD_EDGE_TOTAL].size()/3 
		,pt[E_ABNORMAL_PAD_EDGE_MIDDLE].end());

	double dSlope[E_ABNORMAL_PAD_EDGE];

	//최소 자승법으로 직선의 방정식의 기울기 구하기
	for(int i=0; i<E_ABNORMAL_PAD_EDGE;i++)
	{
		if(MethodOfLeastSquares(pt[i],dSlope[i])==E_ERROR_CODE_FALSE)
			return E_ERROR_CODE_FALSE;
	}

	//각도 구하기
	double dTheth[E_ABNORMAL_PAD_EDGE];

	for(int i=0; i<E_ABNORMAL_PAD_EDGE;i++)
	{
		dTheth[i] = atan(dSlope[i])*180./PI; 
		dTheth[i]+=90;
	}

	//각도 범위
	double dMin = dTheth[E_ABNORMAL_PAD_EDGE_TOTAL] - dCompare_Theta;
	double dMax = dTheth[E_ABNORMAL_PAD_EDGE_TOTAL] + dCompare_Theta;

	//벡터 죽이기
	for(int i=0; i<E_ABNORMAL_PAD_EDGE;i++)
	{
		vector<cv::Point>().swap(pt[i]);
	}

	//각도 비교
	if((dTheth[E_ABNORMAL_PAD_EDGE_TOP]    < dMin || dTheth[E_ABNORMAL_PAD_EDGE_TOP]    > dMax)
		||(dTheth[E_ABNORMAL_PAD_EDGE_BOTTOM] < dMin || dTheth[E_ABNORMAL_PAD_EDGE_BOTTOM] > dMax)
		||(dTheth[E_ABNORMAL_PAD_EDGE_MIDDLE] < dMin || dTheth[E_ABNORMAL_PAD_EDGE_BOTTOM] > dMax))
		return E_ERROR_CODE_FALSE;

	return E_ERROR_CODE_TRUE;

}

// AVI AD GV 검사 ( 8bit & 12bit )
long CInspectAlign::DoFindDefectAD_GV(cv::Mat& matSrcBuf, double* dPara, double* dResult, cv::Point* ptCorner, CDefectCCD *cCCD)
{
	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 파라미터 NULL인 경우.
	if( dPara == NULL )			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	double	dADGVMin	= dPara[E_PARA_CHECK_MIN_GV];
	double	dADGVMax	= dPara[E_PARA_CHECK_MAX_GV];
	double	dADGVAVG	= dPara[E_PARA_CHECK_AVG_GV];
	double	dADGVLAB	= dPara[E_PARA_CHECK_PATTERN_LABEL];

	bool	bAreaFlag	= (dPara[E_PARA_CHECK_AREA_FLAG] > 0) ? true : false;
	int		nAreaGV		= (int)dPara[E_PARA_CHECK_AREA_GV];
	double	dAreaRatio	= dPara[E_PARA_CHECK_AREA_RATIO];

	long	nWidth	= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuf.rows;	// 영상 세로 크기

	//////////////////////////////////////////////////////////////////////////

	// 사용 안하는 경우 나가기
	//if( dUse <= 0 )		return E_ERROR_CODE_TRUE;	// 사용 안함

	// 임시로 범위 작게 설정
	int nOffset = 5;
	CRect rectROI = new CRect(
		max(ptCorner[E_CORNER_LEFT_TOP].x		+ nOffset,	ptCorner[E_CORNER_LEFT_BOTTOM].x	+ nOffset	),
		max(ptCorner[E_CORNER_LEFT_TOP].y		+ nOffset,	ptCorner[E_CORNER_RIGHT_TOP].y		+ nOffset	),
		min(ptCorner[E_CORNER_RIGHT_TOP].x		- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].x	- nOffset	),
		min(ptCorner[E_CORNER_LEFT_BOTTOM].y	- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	- nOffset	) );

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_DISPLAY;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_DISPLAY;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_DISPLAY;

	int nROIWidth	= rectROI.Width();
	int nROIHeight	= rectROI.Height();

	// Image Buff 만들기
	cv::Mat matSrcCopyBuf	= cMem->GetMat(matSrcBuf.size(), matSrcBuf.type(), false);
	cv::Mat matSubBuf		= cMem->GetMat(nROIHeight, nROIWidth, matSrcBuf.type(), false);
	
	// 영상 정보 받아 오기
	matSrcBuf.copyTo(matSrcCopyBuf);

	cv::Mat matMeanBuf = matSrcCopyBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );

	// 정등 영역 평균 구하기
	cv::Scalar m = cv::mean( matMeanBuf );	

	// 평균 결과
	dResult[0] = m[0];

	// 밝기가 밝은 경우, 디스플레이 이상
	if( dResult[0] > dADGVMax )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 밝기가 어두운 경우, 디스플레이 이상
	if( dResult[0] < dADGVMin )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	//////////////////////////////////////////////////////////////////////////

	// Black 패턴에서 점등된 경우 Point 알고리즘 너무 오래 걸림
	// Black 패턴 점등된 영역 비율로 AD 제외
	// 18.03.14 PNZ Black Pattern 정등 영역 AD Check 수정
	// 기존: 일정 GV 이상 Pixel 개수 Count
	// 현상: 1GV정도 전 Pattern에 띠염띠염 있음
	// 수정: 정등 영상 2진화 하여 팽창하고 차지하는 영역 구하기, 현 전체영역 15% 기준으로 설정 됨
	if( bAreaFlag )
	{
		__int64 nCount = 0;

		// 점등 영상에 대해 Mean, StdDev를 구한다, Line이 없고 점등 되 영상에 대해 통계를 내면
		cv::Scalar m, s;
		cv::meanStdDev( matMeanBuf, m, s);

		double dMeanGV	= m[0];
		double dStdGV	= s[0];

		__int64 nTotalLabel = 0;

		// 원본 8U인 경우
		if(( matMeanBuf.type() == CV_8U )&& (dMeanGV > 0.8 || dStdGV > 1.0))
		{
			MatIterator_<uchar> itSrc, endSrc;
			itSrc	= matMeanBuf.begin<uchar>();
			endSrc	= matMeanBuf.end<uchar>();

			// 설정한 GV보다 높은 Pixel 카운트
			for ( ; itSrc != endSrc; itSrc++ )
				( *itSrc > nAreaGV) ? nCount++ : NULL ;
		}

		// 조건에 만족하면 
		else if ( matMeanBuf.type() == CV_8U && dMeanGV <= 0.8 && dStdGV <= 1.0)
		{
			int nPS = 2;

			if ( cCCD != NULL )
			{
				// CCD 불량 위치 보정
				// KSW 17.07.08 Black pattern만 등록하므로, Black Pattern에서만 사용
				// KSW 17.07.11 원본 영상 바로 수정
				// PS모드 시 1pixel 오차 있으므로, 주변 1pixel 더 보정
				cCCD->OffsetDefectCCD(matSrcCopyBuf, 1, nPS);

				// CCD 불량 위치 제거
				// KSW 17.07.08 Black pattern만 등록하므로, Black Pattern에서만 사용
				// KSW 17.07.11 원본 영상 바로 수정
				// PS모드 시 1pixel 오차 있으므로, 주변 1pixel 더 제거
				cCCD->DeleteDefectCCD(matSrcCopyBuf, 1, nPS);
			}

			//2진화 함
			cv::threshold(matMeanBuf, matSubBuf, 1.0, 255.0, THRESH_BINARY);
		
			// Labeling 용 Buff
			cv::Mat matLabel		= cMem->GetMat(nROIHeight, nROIWidth, CV_32SC1, false);
			//cv::Mat matStats		= cMem->GetMat(nROIHeight, nROIWidth, matSrcBuf.type());
			//cv::Mat matCentroid		= cMem->GetMat(nROIHeight, nROIWidth, matSrcBuf.type());

			//nTotalLabel = cv::connectedComponentsWithStats(matMeanBuf, matLabel, matStats, matCentroid, 8, CV_32S) - 1;
			nTotalLabel = cv::connectedComponents(matSubBuf, matLabel, 8, CV_32S, CCL_GRANA) - 1;
		}
		// 원본 16U인 경우
		else
		{
			MatIterator_<ushort> itSrc, endSrc;
			itSrc	= matMeanBuf.begin<ushort>();
			endSrc	= matMeanBuf.end<ushort>();

			// 설정한 GV보다 높은 Pixel 카운트
			for ( ; itSrc != endSrc; itSrc++ )
				( *itSrc > nAreaGV) ? nCount++ : NULL ;
		}
		
		// 차지하는 비율
 		double dRatio = nCount / (double)(matMeanBuf.rows * matMeanBuf.cols);

		// 설정한 값보다 높은 경우
		if( dRatio > dAreaRatio )
		{
			dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
			return E_ERROR_CODE_ALIGN_DISPLAY;
		}

		// 설정한 값보다 높은 경우
		if( nTotalLabel > dADGVLAB )
		{
			dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
			dResult[2] = (double)nTotalLabel;
			return E_ERROR_CODE_ALIGN_DISPLAY;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	// 원하는 밝기로 영상 변경
	if( dADGVAVG > 0 )
		cv::multiply(matSrcBuf, dADGVAVG / dResult[0], matSrcBuf);

	matMeanBuf.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}


//LJH Dust 4구역 평균
long CInspectAlign::DoFindDefectAD_GV_DUST(cv::Mat& matSrcBuf, double* dPara, double* dResult, cv::Point* ptCorner)
{
	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 파라미터 NULL인 경우.
	if( dPara == NULL )			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	double	dADGVMin	= dPara[E_PARA_CHECK_MIN_GV];
	double	dADGVMax	= dPara[E_PARA_CHECK_MAX_GV];
	double	dADGVAVG	= dPara[E_PARA_CHECK_AVG_GV];

	bool	bAreaFlag	= (dPara[E_PARA_CHECK_AREA_FLAG] > 0) ? true : false;
	int		nAreaGV		= (int)dPara[E_PARA_CHECK_AREA_GV];
	double	dAreaRatio	= dPara[E_PARA_CHECK_AREA_RATIO];

	long	nWidth	= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuf.rows;	// 영상 세로 크기

	//////////////////////////////////////////////////////////////////////////

	// 사용 안하는 경우 나가기
	//if( dUse <= 0 )		return E_ERROR_CODE_TRUE;	// 사용 안함

	// 임시로 범위 작게 설정
	int nOffset = 5;
	CRect rectROI = new CRect(
		max(ptCorner[E_CORNER_LEFT_TOP].x		+ nOffset,	ptCorner[E_CORNER_LEFT_BOTTOM].x	+ nOffset	),
		max(ptCorner[E_CORNER_LEFT_TOP].y		+ nOffset,	ptCorner[E_CORNER_RIGHT_TOP].y		+ nOffset	),
		min(ptCorner[E_CORNER_RIGHT_TOP].x		- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].x	- nOffset	),
		min(ptCorner[E_CORNER_LEFT_BOTTOM].y	- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	- nOffset	) );

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_DISPLAY;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_DISPLAY;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_DISPLAY;


	//LJH 점등 영역 4등분 평균 구하기
	int halfWidth	= rectROI.Width() / 2;
	int halfHeight	= rectROI.Height() / 2;

	cv::Mat matMeanBuf1 = matSrcBuf( cv::Rect(rectROI.left				, rectROI.top				, halfWidth, halfHeight) ); //좌상단 영역
	cv::Mat matMeanBuf2 = matSrcBuf( cv::Rect(rectROI.left + halfWidth	, rectROI.top				, halfWidth, halfHeight) ); //우상단 영역
	cv::Mat matMeanBuf3 = matSrcBuf( cv::Rect(rectROI.left				, rectROI.top +halfHeight	, halfWidth, halfHeight) ); //좌하단 영역
	cv::Mat matMeanBuf4 = matSrcBuf( cv::Rect(rectROI.left + halfWidth	, rectROI.top +halfHeight	, halfWidth, halfHeight) ); //우하단 영역

	cv::Scalar m1 = cv::mean( matMeanBuf1 );	//좌상단 영역 평균
	cv::Scalar m2 = cv::mean( matMeanBuf2 );	//우상단 영역 평균
	cv::Scalar m3 = cv::mean( matMeanBuf3 );	//좌하단 영역 평균
	cv::Scalar m4 = cv::mean( matMeanBuf4 );	//우하단 영역 평균

	// 분할 영역 평균 획득 후, 버퍼 해제
	matMeanBuf1.release();
	matMeanBuf2.release();
	matMeanBuf3.release();
	matMeanBuf4.release();

	// 밝기가 밝은 경우, 디스플레이 이상
	if( m1[0] > dADGVMax )
	{
		dResult[0] = m1[0];
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}
	else if ( m2[0] > dADGVMax )
	{
		dResult[0] = m2[0];
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}
	else if ( m3[0] > dADGVMax )
	{
		dResult[0] = m3[0];
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}
	else if ( m4[0] > dADGVMax )
	{
		dResult[0] = m4[0];
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 밝기가 어두운 경우, 디스플레이 이상
	if( m1[0] < dADGVMin )
	{
		dResult[0] = m1[0];
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}
	else if ( m2[0] < dADGVMin )
	{
		dResult[0] = m2[0];
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}
	else if ( m3[0] < dADGVMin )
	{
		dResult[0] = m3[0];
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}
	else if ( m4[0] < dADGVMin )
	{
		dResult[0] = m4[0];
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 평균 구하기
	// LJH 각 4등분 영역 밝기 이상 없을 시 전체 영역 평균 로직 실행
	cv::Mat matMeanBuf = matSrcBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Scalar m = cv::mean( matMeanBuf );	

	// 평균 결과
	dResult[0] = m[0];

	// 밝기가 밝은 경우, 디스플레이 이상
	if( dResult[0] > dADGVMax )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 밝기가 어두운 경우, 디스플레이 이상
	if( dResult[0] < dADGVMin )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	//////////////////////////////////////////////////////////////////////////

	// Black 패턴에서 점등된 경우 Point 알고리즘 너무 오래 걸림
	// Black 패턴 점등된 영역 비율로 AD 제외
	if( bAreaFlag )
	{
		__int64 nCount = 0;

		// 원본 8U인 경우
		if( matMeanBuf.type() == CV_8U )
		{
			MatIterator_<uchar> itSrc, endSrc;
			itSrc	= matMeanBuf.begin<uchar>();
			endSrc	= matMeanBuf.end<uchar>();

			// 설정한 GV보다 높은 Pixel 카운트
			for ( ; itSrc != endSrc; itSrc++ )
				( *itSrc > nAreaGV) ? nCount++ : NULL ;
		}
		// 원본 16U인 경우
		else
		{
			MatIterator_<ushort> itSrc, endSrc;
			itSrc	= matMeanBuf.begin<ushort>();
			endSrc	= matMeanBuf.end<ushort>();

			// 설정한 GV보다 높은 Pixel 카운트
			for ( ; itSrc != endSrc; itSrc++ )
				( *itSrc > nAreaGV) ? nCount++ : NULL ;
		}
		
		// 차지하는 비율
		double dRatio = nCount / (double)(matMeanBuf.rows * matMeanBuf.cols);

		// 설정한 값보다 높은 경우
		if( dRatio > dAreaRatio )
		{
			dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
			return E_ERROR_CODE_ALIGN_DISPLAY;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	// 원하는 밝기로 영상 변경
	if( dADGVAVG > 0 )
		cv::multiply(matSrcBuf, dADGVAVG / dResult[0], matSrcBuf);

	matMeanBuf.release();

	return E_ERROR_CODE_TRUE;
}

// SVI AD GV 검사
long CInspectAlign::DoFindDefectAD_GV_SVI(cv::Mat& matSrcBuf, double* dPara, double* dResult, cv::Point* ptCorner)
{
	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 파라미터 NULL인 경우.
	if( dPara == NULL )			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	//////////////////////////////////////////////////////////////////////////
	// 파라미터
	//////////////////////////////////////////////////////////////////////////

	double	dADGVMin	= dPara[E_PARA_SVI_CHECK_MIN_GV];
	double	dADGVMax	= dPara[E_PARA_SVI_CHECK_MAX_GV];

	long	nWidth	= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuf.rows;	// 영상 세로 크기

	//////////////////////////////////////////////////////////////////////////

	// 임시로 범위 작게 설정
	int nOffset = 5;
	CRect rectROI = new CRect(
		max(ptCorner[E_CORNER_LEFT_TOP].x		+ nOffset,	ptCorner[E_CORNER_LEFT_BOTTOM].x	+ nOffset	),
		max(ptCorner[E_CORNER_LEFT_TOP].y		+ nOffset,	ptCorner[E_CORNER_RIGHT_TOP].y		+ nOffset	),
		min(ptCorner[E_CORNER_RIGHT_TOP].x		- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].x	- nOffset	),
		min(ptCorner[E_CORNER_LEFT_BOTTOM].y	- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	- nOffset	) );

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_DISPLAY;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_DISPLAY;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_DISPLAY;

	// 평균 구하기
	cv::Mat matMeanBuf = matSrcBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Scalar m = cv::mean( matMeanBuf );
	matMeanBuf.release();

	// 평균 결과
	dResult[0] = (m[0] + m[1] + m[2] ) / 3;

	// 밝기가 밝은 경우, 디스플레이 이상
	if( dResult[0] > dADGVMax )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// 밝기가 어두운 경우, 디스플레이 이상
	if( dResult[0] < dADGVMin )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	return E_ERROR_CODE_TRUE;
}


// Cell 영역 찾기
long CInspectAlign::FindCellArea(cv::Mat matThreshBuf, int nMinArea, cv::Rect& rectCell)
{
	// 버퍼가 없는 경우.
	if( matThreshBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;	

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	// 메모리 할당
	cv::Mat matLabel, matStats, matCentroid;
	matLabel = cMatBufTemp.GetMat(matThreshBuf.size(), CV_32SC1);

	// Blob 개수
	__int64 nTotalLabel = cv::connectedComponentsWithStats(matThreshBuf, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

	// 일정 면적 크기 이상 Blob 개수
	int nLabelCount = 0;

	// Blob 개수
	for( int idx=1 ; idx<=nTotalLabel ; idx++ )
	{
		// 객체 면적
		long nArea = matStats.at<int>(idx, CC_STAT_AREA);

		// Blob 영역 Rect
		cv::Rect rectTemp;
		rectTemp.x			= matStats.at<int>(idx, CC_STAT_LEFT);
		rectTemp.y			= matStats.at<int>(idx, CC_STAT_TOP);
		rectTemp.width		= matStats.at<int>(idx, CC_STAT_WIDTH);
		rectTemp.height		= matStats.at<int>(idx, CC_STAT_HEIGHT);

		// 면적 제한
		if( nArea <= nMinArea )
		{
			// 0GV 로 초기화 후, 넘어가기
			cv::Mat matTempROI	= matThreshBuf( rectTemp );
			cv::Mat matLabelROI	= matLabel( rectTemp );

			for(int y=0 ; y<rectTemp.height ; y++)
			{
				int		*ptrLabel	= (int *)matLabelROI.ptr(y);
				uchar	*ptrGray	= (uchar *)matTempROI.ptr(y);

				for(int x=0 ; x<rectTemp.width ; x++, ptrLabel++, ptrGray++)
				{
					// 해당 객체
					if( *ptrLabel == idx )	*ptrGray = 0;
				}
			}

			continue;
		}

		// Cell 영역 Rect
		rectCell = rectTemp;

		// 일정 면적 크기 이상 Blob 개수
		nLabelCount++;
	}

	// 메모리 할당 해제
	matLabel.release();
	matStats.release();
	matCentroid.release();

	// 개수가 1개가 아닌 경우
	if( nLabelCount != 1 )	return E_ERROR_CODE_ALIGN_NO_FIND_CELL;

	// KSW 18.04.11 - 방식 바꿈
	/************************************************************************
	// 버퍼가 없는 경우.
	if( matThreshBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	CMatBuf cMatBufTemp;

	// 버퍼 할당 & 초기화
	cMatBufTemp.SetMem(cMem);
	cv::Mat matLabelBuffer = cMatBufTemp.GetMat(matThreshBuf.size(), CV_32SC1);

	matThreshBuf.convertTo(matLabelBuffer, CV_32SC1);
	matThreshBuf.setTo(0);

	// starts at 2 because 0,1 are used already
	int nLabelCount = 2;

	// 검사 영역 Rect
	cv::Rect rectTemp;
	int nTempCount = 0;

	for(int y=0 ; y<matLabelBuffer.rows ; y++)
	{
		int *row = (int*)matLabelBuffer.ptr(y);

		for(int x=0 ; x<matLabelBuffer.cols ; x++)
		{
			if(row[x] != 255)	continue;

			nTempCount++;

			// 객체 면적
			long nArea = cv::floodFill(matLabelBuffer, cv::Point(x, y), nTempCount, &rectTemp);

			// 면적 제한
			if( nArea <  nMinArea )	continue;

			// Cell 영역 Rect
			rectCell = rectTemp;

			int nEndXX = rectTemp.x+rectTemp.width;
			int nEndYY = rectTemp.y+rectTemp.height;

			// 예외처리
			if( nEndYY >= matLabelBuffer.rows )	nEndYY = matLabelBuffer.rows - 1;
			if( nEndXX >= matLabelBuffer.cols )	nEndXX = matLabelBuffer.cols - 1;

			// 해당 라벨 각각 픽셀 좌표
			for(int j=rectTemp.y ; j<=nEndYY ; j++)
			{
				int		*row2 = (int *)matLabelBuffer.ptr(j);
				BYTE	*row3 = (BYTE *)matThreshBuf.ptr(j);

				for(int i=rectTemp.x ; i<=nEndXX ; i++)
				{
					if(row2[i] != nTempCount)	continue;

					row3[i] = (BYTE)255;
				}
			}

			nLabelCount++;
		}
	}

	matLabelBuffer.release();

	// 갯수가 1개가 아닌 경우. ( nLabelCount 2부터 시작 )
	if( nLabelCount != 3 )	return E_ERROR_CODE_ALIGN_NO_FIND_CELL;
	************************************************************************/

	return E_ERROR_CODE_TRUE;
}


// 중복 데이터 비교
bool CInspectAlign::FindInSamples(vector <cv::Point2i>& ptSamples, cv::Point2i ptIndexs)
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
long CInspectAlign::GetRandomSamples(vector <cv::Point2i>& ptSrcIndexs, vector <cv::Point2i>& ptSamples, int nSampleCount)
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

	return E_ERROR_CODE_TRUE;
}

// 라인 추정 계산
long CInspectAlign::calcLineFit(vector <cv::Point2i>& ptSamples, long double& dA, long double& dB)
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
long CInspectAlign::calcLineVerification(vector <cv::Point2i>& ptSrcIndexs, vector <cv::Point2i>& ptInliers, long double& dA, long double& dB, double distThreshold)
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
long CInspectAlign::calcRANSAC(vector <cv::Point2i>& ptSrcIndexs, long double& dA, long double& dB, int nMinSamples, double distThreshold)
{
	// 갯수가 적을 경우 사용 안함
	// 2개면 그냥 방정식으로 구하면 됨
	// 원본 갯수보다 적어야 함
	if( nMinSamples < 2 )					return E_ERROR_CODE_ALIGN_WARNING_PARA;
	if( ptSrcIndexs.size() <= nMinSamples )	return E_ERROR_CODE_ALIGN_WARNING_PARA;

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
		GetRandomSamples(ptSrcIndexs, ptSamples, nMinSamples);

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
	if( nMaxCost <= 0 )		return E_ERROR_CODE_ALIGN_CAN_NOT_CALC;

	// 샘플 50개 이상 사용하기
	//if( nMaxCost < 50 )	return E_ERROR_CODE_ALIGN_CAN_NOT_CALC;

	return E_ERROR_CODE_TRUE;
}

// 방향에 따른 데이터 획득
long CInspectAlign::RobustFitLine(cv::Mat& matTempBuf, cv::Rect rectCell, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType, int nSamp)
{
	// 영상 크기
	int nW = matTempBuf.cols;
	int nH = matTempBuf.rows;

	// 예외 처리
	if( rectCell.x <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.x >=	nW	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.y <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.y >=	nH	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// 예외 처리
	if( rectCell.x + rectCell.width <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.x + rectCell.width >=	nW	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.y + rectCell.height <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.y + rectCell.height >=	nH	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// 범위 설정
	int nSrartX		= rectCell.x + rectCell.width / 4;	//  가로 1/4 지점
	int nEndX		= nSrartX + rectCell.width / 2;		//  가로 3/4 지점

	int nStartY		= rectCell.y + rectCell.height / 4;	//  세로 1/4 지점
	int nEndY		= nStartY + rectCell.height / 2;	//  세로 3/4 지점

	int x, y;

	vector<cv::Point2i>	ptSrcIndexs;
	vector<cv::Point2i>().swap(ptSrcIndexs);

	// 라인 데이터 벡터 추출
	switch(nType)
	{
	case E_ALIGN_TYPE_LEFT:
		{
			for(y=nStartY ; y<=nEndY ; y+=nSamp)
			{
				for(x=rectCell.x ; x<=nEndX ; x++)
				{
					// 값이 있는 경우
					if( matTempBuf.at<uchar>(y, x) )
					{
						// 좌표 벡터 추가
						ptSrcIndexs.push_back( cv::Point2i(x, y) );

						break;
					}
				}
			}
		}
		break;

	case E_ALIGN_TYPE_TOP:
		{
			for(x=nSrartX ; x<=nEndX ; x+=nSamp)
			{
				for(y=rectCell.y ; y<=nEndY ; y++)
				{
					// 값이 있는 경우
					if( matTempBuf.at<uchar>(y, x) )
					{
						// 좌표 벡터 추가
						ptSrcIndexs.push_back( cv::Point2i(x, y) );

						break;
					}
				}
			}
		}
		break;

	case E_ALIGN_TYPE_RIGHT:
		{
			for(y=nStartY ; y<=nEndY ; y+=nSamp)
			{
				for(x=rectCell.x+rectCell.width ; x>=nSrartX ; x--)
				{
					// 값이 있는 경우
					if( matTempBuf.at<uchar>(y, x) )	
					{
						// 좌표 벡터 추가
						ptSrcIndexs.push_back( cv::Point2i(x, y) );

						break;
					}
				}
			}
		}		
		break;

	case E_ALIGN_TYPE_BOTTOM:
		{
			for(x=nSrartX ; x<=nEndX ; x+=nSamp)
			{
				for(y=rectCell.y+rectCell.height ; y>=nStartY ; y--)
				{
					// 값이 있는 경우
					if( matTempBuf.at<uchar>(y, x) )	
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
			return E_ERROR_CODE_ALIGN_WARNING_PARA;
		}
		break;
	}

	// 추출된 데이터로 라인 찾기
	long nErrorCode = calcRANSAC(ptSrcIndexs, dA, dB, nMinSamples, distThreshold);

	// 벡터 데이터 제거
	vector<cv::Point2i>().swap(ptSrcIndexs);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	return E_ERROR_CODE_TRUE;
}

// 이진화 된영상에서 물체의 안쪽에서 바깥으로 좌표탐색
long CInspectAlign::ObjectInAreaGetLine(cv::Mat& matTempBuf, cv::Rect rectImgSize, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType)
{
	// 영상 크기
	int nW = matTempBuf.cols;
	int nH = matTempBuf.rows;
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
			p = ucImagedata+(nW*nY);
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
				
				if((int)*(p+nY*nW) == 0)
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
			p = ucImagedata+(nW*nY);
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
				if((int)*(p+nY*nW) == 0)
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


	// 추출된 데이터로 라인 찾기
	long nErrorCode = calcRANSAC(ptSrcIndexs, dA, dB, nMinSamples, distThreshold);

	// 벡터 데이터 제거
	vector<cv::Point2i>().swap(ptSrcIndexs);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	return E_ERROR_CODE_TRUE;
}

// 가장 큰 Blob 반환 APP
long CInspectAlign::FindBiggestBlob_APP(cv::Mat & src, cv::Mat & dst)
{
	try
	{
		Mat matSrc;
		src.copyTo(matSrc);

		dst = Mat::zeros(matSrc.size(), matSrc.type());

		double dLargest_Area = 0;
		int nLargest_Contour_Index = 0;

		vector<vector<cv::Point> > vContours; // Vector for storing contour
		vector<cv::Vec4i> vHierarchy;

		findContours(matSrc, vContours, vHierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("findContours END."));

		cv::Rect rtArea = cv::Rect(0, 0, 0, 0);
		for (int i = 0; i < (int)vContours.size(); i++)
		{
			// iterate through each contour. 
			double dArea = contourArea(vContours[i], false);		// Find the area of contour
			if (dArea > dLargest_Area)
			{
				dLargest_Area = (int)dArea;
				nLargest_Contour_Index = i;							// Store the index of largest contour

				rtArea = boundingRect(vContours[i]);				// Find the bounding rectangle for biggest contour
			}
		}

		drawContours(dst, vContours, nLargest_Contour_Index, Scalar(255), CV_FILLED, 8, vHierarchy); // Draw the largest contour using previously stored index.
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("drawContours END."));
	}
	catch (const std::exception&)
	{
		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("ERROR."));

		return E_ERROR_CODE_FALSE;
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::ObjectOutAreaGetLine(cv::Mat& matTempBuf, cv::Rect rectCell, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType, int nThreshold_Theta, float fAvgOffset)
{
	// 영상 크기
	int nW = matTempBuf.cols;
	int nH = matTempBuf.rows;

	// 예외 처리
// 	if( rectCell.x <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.x >=	nW	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.y <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.y >=	nH	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// 예외 처리
// 	if( rectCell.x + rectCell.width <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.x + rectCell.width >=	nW	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.y + rectCell.height <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.y + rectCell.height >=	nH	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// 범위 설정
	int nStartX		= rectCell.x + rectCell.width / 4;	//  가로 1/4 지점
	int nEndX		= nStartX + rectCell.width / 4 * 2;		//  가로 3/4 지점

	int nStartY		= rectCell.y + rectCell.height / 4;	//  세로 1/4 지점
	int nEndY		= nStartY + rectCell.height / 4 * 2;	//  세로 3/4 지점

	int nSamp	= 50;								//  50 Pixel 씩 샘플링

	uchar* ucImgData;

	int nGV_Level, nGV_Level2;

	vector<cv::Point> ptProfile;
	vector<cv::Point> pt;
	long double dA_, dB_;

	double dX,dY;

	float fSum;
	float fAvg;
	int	  nCnt;

	float fTheta;
	int nDeltaOffset = 3;

	switch(nType)
	{
	case E_ALIGN_TYPE_LEFT:
		vector<cv::Point>().swap(ptProfile);
		//LEFT
		for(int nY=nStartY; nY < nEndY; nY+=nSamp)
		{
			ucImgData = matTempBuf.data+nY*matTempBuf.step;
			for(int nX=0; nX < nEndX; nX++)
			{
				nGV_Level = *(ucImgData+nX);
				int nDeltaX = nX+nDeltaOffset;
				if(nDeltaX > matTempBuf.cols-1)
					nDeltaX = matTempBuf.cols-1;

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
					calcLineFit(ptProfile, dA_, dB_);
					if(dA_==0)
						break;
					fAvg = fSum/nCnt*fAvgOffset;
					dX = (fAvg - dB_) / dA_;

					pt.push_back(cv::Point2f(dX, nY));
					vector<cv::Point>().swap(ptProfile);
					break;
				}
			}
		}
		break;
	case E_ALIGN_TYPE_TOP:
		vector<cv::Point>().swap(ptProfile);
		//TOP
		for(int nX=nStartX; nX < nEndX; nX+=nSamp)
		{
			ucImgData = matTempBuf.data+nX;
			for(int nY=0; nY < nEndY; nY++)
			{
				nGV_Level = *(ucImgData+nY*matTempBuf.step);
				int nDeltaY = nY + nDeltaOffset;
				if(nDeltaY > matTempBuf.rows-1)
					nDeltaY = matTempBuf.rows-1;
				nGV_Level2 = *(ucImgData+nDeltaY*matTempBuf.step);

				fTheta = atan((float)(nGV_Level2 - nGV_Level)/nDeltaOffset)*180/PI;

				if(90 > fTheta && fTheta > nThreshold_Theta)
				{
					fSum = 0;
					nCnt = 0;
					for(int nY2 = nDeltaY-2; nY2 <= nDeltaY+3; nY2++)
					{
						nGV_Level = *(ucImgData+nY2*matTempBuf.step);
						fSum += nGV_Level;
						nCnt++;
						ptProfile.push_back(cv::Point(nY2, nGV_Level));
					}
					if(calcLineFit(ptProfile, dA_, dB_)!=0)
						break;

					if(dA_==0)
						break;
					fAvg = fSum/nCnt*fAvgOffset;
					dY = (fAvg - dB_) / dA_;
					pt.push_back(cv::Point2f(nX, dY));
					vector<cv::Point>().swap(ptProfile);
					break;
				}
			}
		}
		break;
	case E_ALIGN_TYPE_RIGHT:
		vector<cv::Point>().swap(ptProfile);
		//Right
		for(int nY=nStartY; nY < nEndY; nY+=nSamp)
		{
			ucImgData = matTempBuf.data+nY*matTempBuf.step;
			for(int nX= matTempBuf.cols-1; nX > nStartX; nX--)
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
					if(calcLineFit(ptProfile, dA_, dB_)!=0)
						break;

					if(dA_==0)
						break;
					fAvg = fSum/nCnt*fAvgOffset;
					dX = (fAvg - dB_) / dA_;
					pt.push_back(cv::Point2f(dX, nY));
					vector<cv::Point>().swap(ptProfile);
					break;

				}
			}
		}
		break;
	case E_ALIGN_TYPE_BOTTOM:
		vector<cv::Point>().swap(ptProfile);
		//BOTTOM
		for(int nX=nStartX; nX < nEndX; nX+=50)
		{
			ucImgData = matTempBuf.data+nX;
			for(int nY= matTempBuf.rows-1; nY > nStartY; nY--)
			{
				nGV_Level = *(ucImgData+nY*matTempBuf.step);
				int nDeltaY = nY - nDeltaOffset;
				if(nDeltaY < 0)
					nDeltaY =0;
				nGV_Level2 = *(ucImgData+nDeltaY*matTempBuf.step);
				fTheta = atan((float)(nGV_Level2-nGV_Level)/nDeltaOffset)*180/PI;
				if(90 > fTheta && fTheta > nThreshold_Theta)
				{
					fSum=0;
					nCnt=0;
					for(int nY2 = nDeltaY-3; nY2 <= nDeltaY+2; nY2++)
					{
						nGV_Level = *(ucImgData+nY2*matTempBuf.step);
						fSum += nGV_Level;
						nCnt++;
						ptProfile.push_back(cv::Point(nY2, nGV_Level));
					}
					if(calcLineFit(ptProfile, dA_, dB_)!=0)
						break;

					if(dA_==0)
						break;
					fAvg = fSum/nCnt*fAvgOffset;
					dY = (fAvg - dB_) / dA_;
					pt.push_back(cv::Point2f(nX, dY));

					vector<cv::Point>().swap(ptProfile);
					break;
				}
			}
		}
		break;
	}

	// 추출된 데이터로 라인 찾기
	long nErrorCode = calcRANSAC(pt, dA, dB, nMinSamples, distThreshold);

	// 벡터 데이터 제거
	vector<cv::Point2i>().swap(pt);

	// 에러인 경우, 에러 코드 출력
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	return E_ERROR_CODE_TRUE;
}

// 4개의 꼭지점 위치 찾기
long CInspectAlign::FindCornerPoint(cv::Point2f ptSrc[E_CORNER_END], cv::Point ptDst[E_CORNER_END], long nWidth, long nHeight)
{
	int nHx = nWidth / 2;
	int nHy = nHeight / 2;

	for (int i=0 ; i<E_CORNER_END ; i++)
	{
		if( ptSrc[i].x <= 0 )		return E_ERROR_CODE_ALIGN_IMAGE_OVER;
		if( ptSrc[i].y <= 0 )		return E_ERROR_CODE_ALIGN_IMAGE_OVER;
		if( ptSrc[i].x >= nWidth )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
		if( ptSrc[i].y >= nHeight )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

		if( ptSrc[i].x < nHx	&&
			ptSrc[i].y < nHy	)
		{
			ptDst[E_CORNER_LEFT_TOP].x = ptSrc[i].x;
			ptDst[E_CORNER_LEFT_TOP].y = ptSrc[i].y;
		}
		else if(	ptSrc[i].x > nHx	&&
					ptSrc[i].y > nHy	)
		{
			ptDst[E_CORNER_RIGHT_BOTTOM].x = ptSrc[i].x;
			ptDst[E_CORNER_RIGHT_BOTTOM].y = ptSrc[i].y;
		}
		else if(	ptSrc[i].x > nHx	&&
					ptSrc[i].y < nHy	)
		{
			ptDst[E_CORNER_RIGHT_TOP].x = ptSrc[i].x;
			ptDst[E_CORNER_RIGHT_TOP].y = ptSrc[i].y;
		}
		else
		{
			ptDst[E_CORNER_LEFT_BOTTOM].x = ptSrc[i].x;
			ptDst[E_CORNER_LEFT_BOTTOM].y = ptSrc[i].y;
		}
	}

	return E_ERROR_CODE_TRUE;
}

// 점등 영역 외곽부분 남기기
long CInspectAlign::FindEdgeArea(cv::Mat matSrcBuf, cv::Mat& matDstBuf, int nLength, CMatBuf* cMemSub)
{
	// 파라미터 NULL인 경우
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	int nSize = nLength * 2 + 1;
	cv::Mat matTempBuf = cMatBufTemp.GetMat(matSrcBuf.size(), matSrcBuf.type(), false);

	// 외곽 평균값 채우는 마스크
	cv::blur(matSrcBuf, matTempBuf, cv::Size(nSize, nSize));
	cv::subtract(matSrcBuf, matTempBuf, matDstBuf);
	cv::threshold(matDstBuf, matDstBuf, 0, 255, THRESH_BINARY);

 	matTempBuf.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}
	// KSW 18.03.28 - 시간 오래걸려서 변경
	/************************************************************************
	cv::Mat matDistBuf1 = cMatBufTemp.GetMat(matSrcBuf.size(), CV_32FC1);
	cv::Mat matDistBuf2 = cMatBufTemp.GetMat(matSrcBuf.size(), CV_32FC1);
	cv::Mat matDistBuf3 = cMatBufTemp.GetMat(matSrcBuf.size(), CV_8UC1);

	cv::distanceTransform(matSrcBuf, matDistBuf1, CV_DIST_L2, 3);

	cv::threshold(matDistBuf1, matDistBuf2, nLength, 255.0, THRESH_BINARY);

	// bit 변환
	matDistBuf2.convertTo(matDistBuf3, matDistBuf3.type());

	// 최외곽 70Pixel 정도 남기기
	cv::bitwise_xor(matDistBuf3, matSrcBuf, matDstBuf);

	matDistBuf1.release();
	matDistBuf2.release();
	matDistBuf3.release();
	************************************************************************/

	return E_ERROR_CODE_TRUE;
}


// 점등 영역 외곽부분 남기기
long CInspectAlign::FindEdgeArea_SVI(cv::Mat matSrcBuf, cv::Mat& matDstBuf, int nLength)
{
	// 파라미터 NULL인 경우
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	int nSize = nLength * 2 + 1;
	cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, cv::Size(nSize, nSize), Point(nLength, nLength) );

	cv::Mat matTempBuf = cv::Mat::zeros(matSrcBuf.size(), matSrcBuf.type());	

	cv::morphologyEx(matSrcBuf, matTempBuf, MORPH_ERODE, StructElem);

	cv::subtract(matSrcBuf, matTempBuf, matDstBuf);

	StructElem.release();
	matTempBuf.release();


	// KSW 18.03.28 - 시간 오래걸려서 변경
	/************************************************************************
	cv::Mat matDistBuf1, matDistBuf2, matDistBuf3;

	cv::distanceTransform(matSrcBuf, matDistBuf1, CV_DIST_L2, 3);

	cv::threshold(matDistBuf1, matDistBuf2, nLength, 255.0, THRESH_BINARY);

	// bit 변환
	matDistBuf2.convertTo(matDistBuf3, CV_8UC1);

	// 최외곽 남기기
	cv::bitwise_xor(matDistBuf3, matSrcBuf, matDstBuf);

	matDistBuf1.release();
	matDistBuf2.release();
	matDistBuf3.release();
	************************************************************************/

	return E_ERROR_CODE_TRUE;
}


// 가로 방향 평균 채우기
long CInspectAlign::FillAreaMeanX(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV)
{
	// 파라미터 NULL인 경우
	if( matMeanBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matEdgeBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	for (int y=0 ; y<2 ; y++)
	{
		long nStart_Y, nEnd_Y;

		// y 범위 계산
		if( y==0 )
		{			
			nStart_Y	= rectROI.top;
			nEnd_Y		= nStart_Y + nSegY;
		}
		else
		{
			nEnd_Y		= rectROI.bottom;
			nStart_Y	= nEnd_Y - nSegY;
		}

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
		for (int x=0 ; x<nRangeX ; x++)
		{
			long nStart_X, nEnd_X;

			// x 범위 계산
			nStart_X = x * nSegX + rectROI.left;
			if( x==nRangeX-1)		nEnd_X = rectROI.right;
			else					nEnd_X = nStart_X + nSegX;

			// 범위 설정
			cv::Rect rectTemp;
			rectTemp.x		= nStart_X;
			rectTemp.y		= nStart_Y;
			rectTemp.width	= nSegX;
			rectTemp.height	= nSegY;

			// 영상 벗어나는 경우 예외처리
			if( rectTemp.x < 0 )	rectTemp.x = 0;
			if( rectTemp.y < 0 )	rectTemp.y = 0;
			if( rectTemp.x + rectTemp.width  >= matMeanBuf.cols )	rectTemp.width  = matMeanBuf.cols - rectTemp.x - 1;
			if( rectTemp.y + rectTemp.height >= matMeanBuf.rows )	rectTemp.height = matMeanBuf.rows - rectTemp.y - 1;

			// 범위 이상한 경우, 예외처리
			if( rectTemp.width  <= 0 )	continue;
			if( rectTemp.height <= 0 )	continue;

			// 영상 ROI
			cv::Mat matTempROIBuf = matMeanBuf( rectTemp );

			// 마스크 ROI
			cv::Mat matMaskROIBuf = matEdgeBuf( rectTemp );

			// 평균 구하기
			cv::Scalar m = cv::mean( matTempROIBuf, matMaskROIBuf );

			// nMinGV 보다 큰 경우만 평균 채우기
			// 범위 설정하여 평균 측정 시, 점등 영역이 안걸리는 경우 존재 ( 모서리 부분 )
			// 원래 값이 있었으나, 0값으로 덮어쓰는 경우 존재
			if( m[0] > nMinGV )
			{
				matMaskROIBuf.setTo( 255 );
				matTempROIBuf.setTo( (unsigned int)m[0] );
			}
			else
			{
				matMaskROIBuf.setTo( 0 );
			}
		}
	}

	return E_ERROR_CODE_TRUE;
}

// 가로 방향 평균 채우기
long CInspectAlign::FillAreaMeanX_SVI(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV)
{
	// 파라미터 NULL인 경우
	if( matMeanBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matEdgeBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	for (int y=0 ; y<2 ; y++)
	{
		long nStart_Y, nEnd_Y;

		// y 범위 계산
		if( y==0 )
		{			
			nStart_Y	= rectROI.top;
			nEnd_Y		= nStart_Y + nSegY;
		}
		else
		{
			nEnd_Y		= rectROI.bottom;
			nStart_Y	= nEnd_Y - nSegY;
		}

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
		for (int x=0 ; x<nRangeX ; x++)
		{
			long nStart_X, nEnd_X;

			// x 범위 계산
			nStart_X = x * nSegX + rectROI.left;
			if( x==nRangeX-1)		nEnd_X = rectROI.right;
			else					nEnd_X = nStart_X + nSegX;

			// 범위 설정
			cv::Rect rectTemp;
			rectTemp.x		= nStart_X;
			rectTemp.y		= nStart_Y;
			rectTemp.width	= nSegX;
			rectTemp.height	= nSegY;

			// 영상 벗어나는 경우 예외처리
			if( rectTemp.x < 0 )	rectTemp.x = 0;
			if( rectTemp.y < 0 )	rectTemp.y = 0;
			if( rectTemp.x + rectTemp.width  >= matMeanBuf.cols )	rectTemp.width  = matMeanBuf.cols - rectTemp.x - 1;
			if( rectTemp.y + rectTemp.height >= matMeanBuf.rows )	rectTemp.height = matMeanBuf.rows - rectTemp.y - 1;

			// 범위 이상한 경우, 예외처리
			if( rectTemp.width  <= 0 )	continue;
			if( rectTemp.height <= 0 )	continue;

			// 영상 ROI
			cv::Mat matTempROIBuf = matMeanBuf( rectTemp );

			// 마스크 ROI
			cv::Mat matMaskROIBuf = matEdgeBuf( rectTemp );

			// 평균 구하기
			cv::Scalar m = cv::mean( matTempROIBuf, matMaskROIBuf );

			// nMinGV 보다 큰 경우만 평균 채우기
			// 범위 설정하여 평균 측정 시, 점등 영역이 안걸리는 경우 존재 ( 모서리 부분 )
			// 원래 값이 있었으나, 0값으로 덮어쓰는 경우 존재
			if( (m[0] + m[1] + m[2])/3 > nMinGV )
			{
				// 평균 채우기
				matTempROIBuf.setTo( m );

				// 마스크 채우기
				matMaskROIBuf.setTo( 255 );
			}
			else
			{
				matMaskROIBuf.setTo( 0 );
			}
		}
	}

	return E_ERROR_CODE_TRUE;
}

// 세로 방향 평균 채우기
long CInspectAlign::FillAreaMeanY(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV)
{
	// 파라미터 NULL인 경우
	if( matMeanBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matEdgeBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	for (int x=0 ; x<2 ; x++)
	{
		long nStart_X, nEnd_X;

		// x 범위 계산
		if( x==0 )
		{
			nStart_X	= rectROI.left;
			nEnd_X		= nStart_X + nSegX;
		}
		else
		{
			nEnd_X		= rectROI.right;
			nStart_X	= nEnd_X - nSegX;
		}

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
		for (int y=0 ; y<nRangeY ; y++)
		{
			long nStart_Y, nEnd_Y;

			// y 범위 계산
			nStart_Y = y * nSegY + rectROI.top;
			if( y==nRangeY-1)		nEnd_Y = rectROI.bottom;
			else					nEnd_Y = nStart_Y + nSegY;

			// 범위 설정
			cv::Rect rectTemp;
			rectTemp.x		= nStart_X;
			rectTemp.y		= nStart_Y;
			rectTemp.width	= nSegX;
			rectTemp.height	= nSegY;

			// 영상 벗어나는 경우 예외처리
			if( rectTemp.x < 0 )	rectTemp.x = 0;
			if( rectTemp.y < 0 )	rectTemp.y = 0;
			if( rectTemp.x + rectTemp.width  >= matMeanBuf.cols )	rectTemp.width  = matMeanBuf.cols - rectTemp.x - 1;
			if( rectTemp.y + rectTemp.height >= matMeanBuf.rows )	rectTemp.height = matMeanBuf.rows - rectTemp.y - 1;

			// 범위 이상한 경우, 예외처리
			if( rectTemp.width  <= 0 )	continue;
			if( rectTemp.height <= 0 )	continue;

			// 영상 ROI
			cv::Mat matTempROIBuf = matMeanBuf( rectTemp );

			// 마스크 ROI
			cv::Mat matMaskROIBuf = matEdgeBuf( rectTemp );

			// 평균 구하기
			cv::Scalar m = cv::mean( matTempROIBuf, matMaskROIBuf );

			// nMinGV 보다 큰 경우만 평균 채우기
			// 범위 설정하여 평균 측정 시, 점등 영역이 안걸리는 경우 존재 ( 모서리 부분 )
			// 원래 값이 있었으나, 0값으로 덮어쓰는 경우 존재
			if( m[0] > nMinGV )
			{
				// x방향에서 우선 처리하여 사용 안함
				//matMaskROIBuf.setTo( 255 );
				matTempROIBuf.setTo( (unsigned int)m[0] );
			}
			//else
			//{
			//	matMaskROIBuf.setTo( 0 );
			//}
		}
	}

	return E_ERROR_CODE_TRUE;
}


// 세로 방향 평균 채우기
long CInspectAlign::FillAreaMeanY_SVI(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV)
{
	// 파라미터 NULL인 경우
	if( matMeanBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matEdgeBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	for (int x=0 ; x<2 ; x++)
	{
		long nStart_X, nEnd_X;

		// x 범위 계산
		if( x==0 )
		{
			nStart_X	= rectROI.left;
			nEnd_X		= nStart_X + nSegX;
		}
		else
		{
			nEnd_X		= rectROI.right;
			nStart_X	= nEnd_X - nSegX;
		}

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
		for (int y=0 ; y<nRangeY ; y++)
		{
			long nStart_Y, nEnd_Y;

			// y 범위 계산
			nStart_Y = y * nSegY + rectROI.top;
			if( y==nRangeY-1)		nEnd_Y = rectROI.bottom;
			else					nEnd_Y = nStart_Y + nSegY;

			// 범위 설정
			cv::Rect rectTemp;
			rectTemp.x		= nStart_X;
			rectTemp.y		= nStart_Y;
			rectTemp.width	= nSegX;
			rectTemp.height	= nSegY;

			// 영상 벗어나는 경우 예외처리
			if( rectTemp.x < 0 )	rectTemp.x = 0;
			if( rectTemp.y < 0 )	rectTemp.y = 0;
			if( rectTemp.x + rectTemp.width  >= matMeanBuf.cols )	rectTemp.width  = matMeanBuf.cols - rectTemp.x - 1;
			if( rectTemp.y + rectTemp.height >= matMeanBuf.rows )	rectTemp.height = matMeanBuf.rows - rectTemp.y - 1;

			// 범위 이상한 경우, 예외처리
			if( rectTemp.width  <= 0 )	continue;
			if( rectTemp.height <= 0 )	continue;

			// 영상 ROI
			cv::Mat matTempROIBuf = matMeanBuf( rectTemp );

			// 마스크 ROI
			cv::Mat matMaskROIBuf = matEdgeBuf( rectTemp );

			// 평균 구하기
			cv::Scalar m = cv::mean( matTempROIBuf, matMaskROIBuf );

			// nMinGV 보다 큰 경우만 평균 채우기
			// 범위 설정하여 평균 측정 시, 점등 영역이 안걸리는 경우 존재 ( 모서리 부분 )
			// 원래 값이 있었으나, 0값으로 덮어쓰는 경우 존재
			if( (m[0] + m[1] + m[2])/3 > nMinGV )
			{
				// 평균 채우기
				matTempROIBuf.setTo( m );

				// 마스크 채우기
				//matMaskROIBuf.setTo( 255 );
			}
			//else
			//{
			//	matMaskROIBuf.setTo( 0 );
			//}
		}
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::FillMerge(cv::Mat& matSrcBuf, cv::Mat matMeanBuf, cv::Mat matMaskBuf, int nAlgImg , CMatBuf* cMemSub )
{
	// 파라미터 NULL인 경우
	if (matSrcBuf.empty())			return E_ERROR_CODE_EMPTY_BUFFER;
	if (matMeanBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matMaskBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	int Threshold = 0;
	// 8bit
	if (matSrcBuf.type() == CV_8U)
	{
		/*	cv::bitwise_and(matSrcBuf, matMaskBuf, matSrcBuf);

		// 반전
		cv::bitwise_not(matMaskBuf, matMaskBuf);

		// 배경 평균 부분만 남기기
		cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

		// 병합
		cv::add(matSrcBuf, matMeanBuf, matSrcBuf);
		*/

		////////////////////////////////////////////////////////////////////////// choikwangil 04.06 Test
		if (nAlgImg == E_IMAGE_CLASSIFY_AVI_WHITE || nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST) {

			cv::bitwise_and(matSrcBuf, matMaskBuf, matSrcBuf);

			// 반전
			cv::bitwise_not(matMaskBuf, matMaskBuf);

			// 배경 평균 부분만 남기기
			cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

			// 병합
			cv::add(matSrcBuf, matMeanBuf, matSrcBuf);
		}

		else {

			cv::Mat Test_tmp = cMatBufTemp.GetMat(matSrcBuf.size(), matSrcBuf.type(),false);

			// 반전
			cv::bitwise_not(matMaskBuf, matMaskBuf);

			// 배경 평균 부분만 남기기
			cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

			cv::min(matMeanBuf, matSrcBuf, Test_tmp);

			if (nAlgImg == E_IMAGE_CLASSIFY_AVI_G) { //choi 06.04
				Threshold = 5;
			}
			else {
				Threshold = 20; //pwj 12.04.14 10->20으로 변경
			}

				cv::threshold(Test_tmp, Test_tmp, Threshold, 255.0, THRESH_BINARY);
				//cv::imwrite("E:\\notch\\1_threshold.bmp", Test_tmp);

				cv::subtract(matMeanBuf, Test_tmp, matMeanBuf);
				//cv::imwrite("E:\\notch\\2_subtract.bmp", matMeanBuf);
				// 병합	
				cv::max(matSrcBuf, matMeanBuf, matSrcBuf);
				//cv::imwrite("E:\\notch\\3_max.bmp", matSrcBuf);

		}

	}
	// 12bit
	else
	{
		cv::Mat matTempBuf16 = cMatBufTemp.GetMat(matSrcBuf.size(), matSrcBuf.type(), false);
		matMaskBuf.convertTo(matTempBuf16, matSrcBuf.type());

		// threshold
		MatIterator_<ushort> it, end;
		for (it = matTempBuf16.begin<ushort>(), end = matTempBuf16.end<ushort>(); it != end; it++)
			*it = (*it) ? 4095 : 0;

		/*cv::bitwise_and(matSrcBuf, matTempBuf16, matSrcBuf);

		// 반전
		cv::bitwise_not(matTempBuf16, matTempBuf16);

		// 배경 평균 부분만 남기기
		cv::bitwise_and(matMeanBuf, matTempBuf16, matMeanBuf);

		// 병합
		cv::add(matSrcBuf, matMeanBuf, matSrcBuf);*/

		////////////////////////////////////////////////////////////////////////// choikwangil 04.06 Test
		if (nAlgImg == E_IMAGE_CLASSIFY_AVI_WHITE || nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST) {

			cv::bitwise_and(matSrcBuf, matMaskBuf, matSrcBuf);

			// 반전
			cv::bitwise_not(matMaskBuf, matMaskBuf);

			// 배경 평균 부분만 남기기
			cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

			// 병합
			cv::add(matSrcBuf, matMeanBuf, matSrcBuf);
		}

		else {

			cv::Mat Test_tmp = cMatBufTemp.GetMat(matSrcBuf.size(), matSrcBuf.type(), false);

			// 반전
			cv::bitwise_not(matMaskBuf, matMaskBuf);

			// 배경 평균 부분만 남기기
			cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

			cv::min(matMeanBuf, matSrcBuf, Test_tmp);

			if (nAlgImg == E_IMAGE_CLASSIFY_AVI_G) { //choi 06.04
				Threshold = 5;
			}
			else {
				Threshold = 20;
			}

			cv::threshold(Test_tmp, Test_tmp, Threshold, 255.0, THRESH_BINARY);

			cv::subtract(matMeanBuf, Test_tmp, matMeanBuf);
			// 병합	
			cv::max(matSrcBuf, matMeanBuf, matSrcBuf);
		}
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}


// 점등된 부분만 남기기
long CInspectAlign::FillMerge_SVI(cv::Mat& matSrcBuf, cv::Mat matMeanBuf, cv::Mat matMaskBuf)
{
	// 파라미터 NULL인 경우
	if( matSrcBuf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matMeanBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matMaskBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat matTemp;
	cv::subtract(matSrcBuf, matMaskBuf, matTemp);

	// 배경 평균 부분만 남기기
	cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

	// 병합
	cv::add(matTemp, matMeanBuf, matSrcBuf);

	matTemp.release();

	return E_ERROR_CODE_TRUE;
}


// 곡선 부분, 점등영역 안쪽으로 살짝 넣기 ( 상황에 따라 다름 )
cv::Point CInspectAlign::calcRoundIn(ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], int nIndex, int nRoundIn)
{
// Top
	if( tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_TOP]		&&
		tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_TOP]		&&
		!tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_BOTTOM] &&
		!tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_BOTTOM]	)
	{
		return cv::Point(0, nRoundIn);
	}
	// Left
	else if( tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_TOP]			&&			 
			 !tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_TOP]		&&
			 !tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_BOTTOM]	&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_BOTTOM]		)
	{
		return cv::Point(-nRoundIn, 0);
	}
	// Right
	else if( !tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_TOP]		&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_TOP]		&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_BOTTOM]		&&			 
			 !tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_BOTTOM]		)
	{
		return cv::Point(nRoundIn, 0);
	}
	// Bottom
	else if( !tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_TOP]		&&
			 !tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_TOP]		&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_BOTTOM]		&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_BOTTOM]		)
	{
		return cv::Point(0, -nRoundIn);
	}

	//////////////////////////////////////////////////////////////////////////

	else if( tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_TOP]			&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_TOP]		&&
			 !tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_BOTTOM]	&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_BOTTOM]		)
	{
		return cv::Point(-nRoundIn, -nRoundIn);
	}
	else if( tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_TOP]			&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_TOP]		&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_BOTTOM]		&&
			 !tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_BOTTOM]		)
	{
		return cv::Point(nRoundIn, -nRoundIn);
	}
	else if( !tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_TOP]		&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_TOP]		&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_BOTTOM]		&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_BOTTOM]		)
	{
		return cv::Point(nRoundIn, nRoundIn);
	}
	else if( tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_TOP]			&&
			 !tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_TOP]		&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_BOTTOM]		&&
			 tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_BOTTOM]		)
	{
		return cv::Point(-nRoundIn, nRoundIn);
	}

	//////////////////////////////////////////////////////////////////////////

	else if( tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_TOP]	)
	{
		return cv::Point(-nRoundIn, -nRoundIn);
	}
	else if( tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_TOP] )
	{
		return cv::Point(nRoundIn, -nRoundIn);
	}
	else if( tRoundSet[nIndex].nCornerInside[E_CORNER_RIGHT_BOTTOM] )
	{
		return cv::Point(nRoundIn, nRoundIn);
	}
	else if( tRoundSet[nIndex].nCornerInside[E_CORNER_LEFT_BOTTOM] )
	{
		return cv::Point(-nRoundIn, nRoundIn);
	}

	//////////////////////////////////////////////////////////////////////////
	else
	{
// 		AfxMessageBox(_T("Set Corner Err !!!"));
	}

	return cv::Point(0, 0);
}


// 곡선 이외 직선 연결
long CInspectAlign::calcLineConnect(cv::Mat& matSrcBuf, cv::Point ptSE[2], cv::Point ptPoly[2], int& nSE, int nSetArea)
{
	// 버퍼가 없는 경우.
	if( matSrcBuf.empty()	)		return E_ERROR_CODE_EMPTY_BUFFER;

	// 거리 확인
	int nLength[4] = { 0, };
	nLength[0] = abs(ptSE[0].x - ptPoly[0].x)	+  abs(ptSE[0].y - ptPoly[0].y);
	nLength[1] = abs(ptSE[0].x - ptPoly[1].x)	+  abs(ptSE[0].y - ptPoly[1].y);
	nLength[2] = abs(ptSE[1].x - ptPoly[0].x)	+  abs(ptSE[1].y - ptPoly[0].y);
	nLength[3] = abs(ptSE[1].x - ptPoly[1].x)	+  abs(ptSE[1].y - ptPoly[1].y);

	int nMinIndex, nMinValue;

	if( nSetArea == 1 )
	{
		nMinIndex = 0;
		nMinValue = nLength[0];

		for (int k=1 ; k<4 ; k++)
		{					
			if( nMinValue > nLength[k] )
			{
				nMinValue = nLength[k];
				nMinIndex = k;
			}
		}

		// 사용 안한것 마지막에만 사용
		nSE = nMinIndex / 2;
	}
	else
	{
		// ptSE[0]으로 계속 비교
		if( nSE == 0 )
		{				
			nMinIndex = ( nLength[0] > nLength[1] ) ? 1 : 0;
		}
		else
		{
			nMinIndex = ( nLength[2] > nLength[3] ) ? 3 : 2;
		}
	}


	// 거리 짧은 지점 연결
	switch(nMinIndex)
	{
		// nLength[0] = abs(ptSE[0].x - ptPoly[0].x)	+  abs(ptSE[0].y - ptPoly[0].y);
	case 0:
		cv::line(matSrcBuf, ptSE[0],  ptPoly[0], cv::Scalar(255, 255, 255));
		ptSE[0] = ptPoly[1];
		break;

		// nLength[1] = abs(ptSE[0].x - ptPoly[1].x)	+  abs(ptSE[0].y - ptPoly[1].y);
	case 1:
		cv::line(matSrcBuf, ptSE[0],  ptPoly[1], cv::Scalar(255, 255, 255));
		ptSE[0] = ptPoly[0];
		break;

		// nLength[2] = abs(ptSE[1].x - ptPoly[0].x)	+  abs(ptSE[1].y - ptPoly[0].y);
	case 2:
		cv::line(matSrcBuf, ptSE[1],  ptPoly[0], cv::Scalar(255, 255, 255));
		ptSE[1] = ptPoly[1];
		break;

		// nLength[3] = abs(ptSE[1].x - ptPoly[1].x)	+  abs(ptSE[1].y - ptPoly[1].y);
	case 3:
		cv::line(matSrcBuf, ptSE[1],  ptPoly[1], cv::Scalar(255, 255, 255));
		ptSE[1] = ptPoly[0];
		break;
	}

	return E_ERROR_CODE_TRUE;
}

// 코너 부분 영역 채우기 ( Color )
long CInspectAlign::FillCorner(cv::Mat& matSrcROIBuf, cv::Mat& matMaskROIBuf, int nType)
{
	// 버퍼가 없는 경우.
	if( matSrcROIBuf.empty()	)		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matMaskROIBuf.empty()	)		return E_ERROR_CODE_EMPTY_BUFFER;

	bool bLoop = true;

	switch( nType )
	{
	case E_CORNER_LEFT_TOP:
		while( bLoop )
		{
			bLoop = false;

			for(int j=0 ; j<matMaskROIBuf.rows-1 ; j++ )
			{
				for( int i=1 ; i<matMaskROIBuf.cols-1 ; i++ )
				{
					if( matMaskROIBuf.at<uchar>(j, i-1) == 255 )
						continue;

					if( matMaskROIBuf.at<uchar>(j+1, i-1) == 0 )
						continue;

					if( matMaskROIBuf.at<uchar>(j, i) == 0 )
						continue;

					matMaskROIBuf.at<uchar>(j, i-1) = 255;

					matSrcROIBuf.at<cv::Vec3b>(j, i-1)[0] = (matSrcROIBuf.at<cv::Vec3b>(j+1, i-1)[0] + matSrcROIBuf.at<cv::Vec3b>(j, i)[0]) / 2;
					matSrcROIBuf.at<cv::Vec3b>(j, i-1)[1] = (matSrcROIBuf.at<cv::Vec3b>(j+1, i-1)[1] + matSrcROIBuf.at<cv::Vec3b>(j, i)[1]) / 2;
					matSrcROIBuf.at<cv::Vec3b>(j, i-1)[2] = (matSrcROIBuf.at<cv::Vec3b>(j+1, i-1)[2] + matSrcROIBuf.at<cv::Vec3b>(j, i)[2]) / 2;

					bLoop = true;
				}
			}
		}
		break;

	case E_CORNER_RIGHT_TOP:
		while( bLoop )
		{
			bLoop = false;

			for(int j=0 ; j<matMaskROIBuf.rows-1 ; j++ )
			{
				for( int i=1 ; i<matMaskROIBuf.cols ; i++ )
				{
					if( matMaskROIBuf.at<uchar>(j, i) == 255 )
						continue;

					if( matMaskROIBuf.at<uchar>(j+1, i) == 0 )
						continue;

					if( matMaskROIBuf.at<uchar>(j, i-1) == 0 )
						continue;

					matMaskROIBuf.at<uchar>(j, i) = 255;

					matSrcROIBuf.at<cv::Vec3b>(j, i)[0] = (matSrcROIBuf.at<cv::Vec3b>(j+1, i)[0] + matSrcROIBuf.at<cv::Vec3b>(j, i-1)[0]) / 2;
					matSrcROIBuf.at<cv::Vec3b>(j, i)[1] = (matSrcROIBuf.at<cv::Vec3b>(j+1, i)[1] + matSrcROIBuf.at<cv::Vec3b>(j, i-1)[1]) / 2;
					matSrcROIBuf.at<cv::Vec3b>(j, i)[2] = (matSrcROIBuf.at<cv::Vec3b>(j+1, i)[2] + matSrcROIBuf.at<cv::Vec3b>(j, i-1)[2]) / 2;

					bLoop = true;
				}
			}
		}
		break;

	case E_CORNER_RIGHT_BOTTOM:
		while( bLoop )
		{
			bLoop = false;

			for(int j=1 ; j<matMaskROIBuf.rows ; j++ )
			{
				for( int i=1 ; i<matMaskROIBuf.cols ; i++ )
				{
					if( matMaskROIBuf.at<uchar>(j, i) == 255 )
						continue;

					if( matMaskROIBuf.at<uchar>(j, i-1) == 0 )
						continue;

					if( matMaskROIBuf.at<uchar>(j-1, i) == 0 )
						continue;

					matMaskROIBuf.at<uchar>(j, i) = 255;

					matSrcROIBuf.at<cv::Vec3b>(j, i)[0] = (matSrcROIBuf.at<cv::Vec3b>(j, i-1)[0] + matSrcROIBuf.at<cv::Vec3b>(j-1, i)[0]) / 2;
					matSrcROIBuf.at<cv::Vec3b>(j, i)[1] = (matSrcROIBuf.at<cv::Vec3b>(j, i-1)[1] + matSrcROIBuf.at<cv::Vec3b>(j-1, i)[1]) / 2;
					matSrcROIBuf.at<cv::Vec3b>(j, i)[2] = (matSrcROIBuf.at<cv::Vec3b>(j, i-1)[2] + matSrcROIBuf.at<cv::Vec3b>(j-1, i)[2]) / 2;

					bLoop = true;
				}
			}
		}
		break;

	case E_CORNER_LEFT_BOTTOM:
		while( bLoop )
		{
			bLoop = false;

			for(int j=1 ; j<matMaskROIBuf.rows ; j++ )
			{
				for( int i=1 ; i<matMaskROIBuf.cols ; i++ )
				{
					if( matMaskROIBuf.at<uchar>(j, i-1) == 255 )
						continue;

					if( matMaskROIBuf.at<uchar>(j, i) == 0 )
						continue;

					if( matMaskROIBuf.at<uchar>(j-1, i-1) == 0 )
						continue;

					matMaskROIBuf.at<uchar>(j, i-1) = 255;

					matSrcROIBuf.at<cv::Vec3b>(j, i-1)[0] = (matSrcROIBuf.at<cv::Vec3b>(j, i)[0] + matSrcROIBuf.at<cv::Vec3b>(j-1, i-1)[0]) / 2;
					matSrcROIBuf.at<cv::Vec3b>(j, i-1)[1] = (matSrcROIBuf.at<cv::Vec3b>(j, i)[1] + matSrcROIBuf.at<cv::Vec3b>(j-1, i-1)[1]) / 2;
					matSrcROIBuf.at<cv::Vec3b>(j, i-1)[2] = (matSrcROIBuf.at<cv::Vec3b>(j, i)[2] + matSrcROIBuf.at<cv::Vec3b>(j-1, i-1)[2]) / 2;

					bLoop = true;
				}
			}
		}
		break;
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::ShiftCopy(cv::Mat& matSrcImage, cv::Mat& matDstImage, int nShiftX, int nShiftY, int nShiftLoopX, int nShiftLoopY)
{
	// 버퍼가 없는 경우.
	if( matSrcImage.empty()	)		return E_ERROR_CODE_EMPTY_BUFFER;

	// 예외처리
	if(nShiftX < 0)		nShiftX = 0;
	if(nShiftY < 0)		nShiftY = 0;

	nShiftLoopX++;
	nShiftLoopY++;

	//////////////////////////////////////////////////////////////////////////

	// 원본 & 결과
	cv::Mat matSrcBuf, matDstBuf;

	CMatBuf cMatBufTemp;

	// 버퍼 할당 & 초기화
	cMatBufTemp.SetMem(cMem);
	matSrcBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matDstBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matSrcImage.copyTo(matSrcBuf);
	matSrcImage.copyTo(matDstBuf);

	// 영상 크기
	int nImageSizeX = matSrcBuf.cols;
	int nImageSizeY = matSrcBuf.rows;

	// 임시 버퍼
	cv::Mat matTempBuf1, matTempBuf2;

	// x 방향
	int nOffsetX = 0;
	for (int x=1 ; x<nShiftLoopX ; x++)
	{
		nOffsetX = x * nShiftX;

		matTempBuf1 = matDstBuf( cv::Rect(0, 0, nImageSizeX-nOffsetX, nImageSizeY)			);
		matTempBuf2 = matSrcBuf( cv::Rect(nOffsetX, 0, nImageSizeX-nOffsetX, nImageSizeY)	);

		// 포인트 불량의 경우 점등 안되어야 할 부분의 배열이 켜질수 있음
		// 덮어쓰면 불량이 사라지므로 사용 불가
		cv::add(matTempBuf1, matTempBuf2, matTempBuf1);
		//cv::max(matTempBuf1, matTempBuf2, matTempBuf1);
	}

	// y 방향
	int nOffsetY = 0;
	matDstBuf.copyTo(matSrcBuf);
	for (int y=1 ; y<nShiftLoopY ; y++)
	{
		nOffsetY = y * nShiftY;

		matTempBuf1 = matDstBuf( cv::Rect(0, 0, nImageSizeX, nImageSizeY-nOffsetY)			);
		matTempBuf2 = matSrcBuf( cv::Rect(0, nOffsetY, nImageSizeX, nImageSizeY-nOffsetY)	);

		// 포인트 불량의 경우 점등 안되어야 할 부분의 배열이 켜질수 있음
		// 덮어쓰면 불량이 사라지므로 사용 불가
		cv::add(matTempBuf1, matTempBuf2, matTempBuf1);
		//cv::max(matTempBuf1, matTempBuf2, matTempBuf1);
	}

	// 더한 갯수만큼 나누기
	//matDstBuf /= (nShiftLoopX * nShiftLoopY);

	// 좌, 상 방향으로만 복사하였으므로, 중앙으로 오도록 이동
	nOffsetX /= 2;
	nOffsetY /= 2;
	//matDstImage = cv::Mat::zeros(matDstBuf.size(), matDstBuf.type());
	matDstBuf( cv::Rect(0, 0, matDstBuf.cols-nOffsetX, matDstBuf.rows-nOffsetY) ).copyTo( matDstImage(cv::Rect(nOffsetX, nOffsetY, matDstBuf.cols-nOffsetX, matDstBuf.rows-nOffsetY)) );

	// 버퍼 해제
	matTempBuf1.release();
	matTempBuf2.release();
	matSrcBuf.release();
	matDstBuf.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::ShiftCopy16Bit(cv::Mat& matSrcImage, cv::Mat& matDstImage, int nShiftX, int nShiftY, int nShiftLoopX, int nShiftLoopY)
{
	// 버퍼가 없는 경우.
	if( matSrcImage.empty()	)		return E_ERROR_CODE_EMPTY_BUFFER;

	// 예외처리
	if(nShiftX < 0)		nShiftX = 0;
	if(nShiftY < 0)		nShiftY = 0;

	nShiftLoopX++;
	nShiftLoopY++;

	//////////////////////////////////////////////////////////////////////////

	// 16비트 변환
	cv::Mat matSrcBuf16;
	matSrcImage.convertTo(matSrcBuf16, CV_16U);

	// 결과 버퍼
	cv::Mat matDstBuf16 = matSrcBuf16.clone();

	// 영상 크기
	int nImageSizeX = matSrcBuf16.cols;
	int nImageSizeY = matSrcBuf16.rows;

	// 임시 버퍼
	cv::Mat matTempBuf1, matTempBuf2;

	// x 방향
	int nOffsetX = 0;
	for (int x=1 ; x<nShiftLoopX ; x++)
	{
		nOffsetX = x * nShiftX;

		matTempBuf1 = matDstBuf16( cv::Rect(0, 0, nImageSizeX-nOffsetX, nImageSizeY)		);
		matTempBuf2 = matSrcBuf16( cv::Rect(nOffsetX, 0, nImageSizeX-nOffsetX, nImageSizeY)	);

		cv::add(matTempBuf1, matTempBuf2, matTempBuf1);
	}

	// y 방향
	int nOffsetY = 0;
	matSrcBuf16 = matDstBuf16.clone();
	for (int y=1 ; y<nShiftLoopY ; y++)
	{
		nOffsetY = y * nShiftY;

		matTempBuf1 = matDstBuf16( cv::Rect(0, 0, nImageSizeX, nImageSizeY-nOffsetY)		);
		matTempBuf2 = matSrcBuf16( cv::Rect(0, nOffsetY, nImageSizeX, nImageSizeY-nOffsetY)	);

		cv::add(matTempBuf1, matTempBuf2, matTempBuf1);
	}

	// 더한 갯수만큼 나누기
	matDstBuf16 /= (nShiftLoopX * nShiftLoopY);

	// 8비트 변환
	cv::Mat matDstBuf;
	matDstBuf16.convertTo(matTempBuf1, CV_8U);

	// 좌, 상 방향으로만 복사하였으므로, 중앙으로 오도록 이동
	nOffsetX /= 2;
	nOffsetY /= 2;
	matDstImage = cv::Mat::zeros(matTempBuf1.size(), matTempBuf1.type());
	matTempBuf1( cv::Rect(0, 0, matTempBuf1.cols-nOffsetX, matTempBuf1.rows-nOffsetY) ).copyTo( matDstImage(cv::Rect(nOffsetX, nOffsetY, matTempBuf1.cols-nOffsetX, matTempBuf1.rows-nOffsetY)) );

	// 버퍼 해제
	matTempBuf1.release();
	matTempBuf2.release();
	matSrcBuf16.release();
	matDstBuf16.release();

	return E_ERROR_CODE_TRUE;
}

// PNZ ShiftCopy Parameter Check ( 18.10.18 )
long	CInspectAlign::ShiftCopyParaCheck(int ShiftValue, int& nCpyX, int& nCpyY, int& nLoopX, int& nLoopY)
{
	if ( ShiftValue == 0 ) return false;
// 
// 	nCpyX	= (int)	(ShiftValue											)	/ 1000	;	// X 방향 Unit
// 	nCpyY	= (int) (ShiftValue - nCpyX*1000							)	/ 100	;	// Y 방향 Unit
// 	nLoopX	= (int) (ShiftValue	- nCpyX*1000 - nCpyY*100				)	/ 10	;	// X 방향 Loop
// 	nLoopY	= (int) (ShiftValue - nCpyX*1000 - nCpyY*100 - nLoopX*10	)	/ 1		;	// Y 방향 Loop

	nCpyX	= (int)	(ShiftValue/ 1000	%10);	// X 방향 Unit
	nCpyY	= (int) (ShiftValue/ 100	%10);	// Y 방향 Unit
	nLoopX	= (int) (ShiftValue/ 10		%10);	// X 방향 Loop
	nLoopY	= (int) (ShiftValue/ 1		%10);	// Y 방향 Loop

	return E_ERROR_CODE_TRUE;
}



// 패턴 이름 가져오기
CString	CInspectAlign::GetPatternString(int nPattern)
{
	switch(nPattern)
	{
	case E_IMAGE_CLASSIFY_AVI_R:
		return _T("R");
		break;

	case E_IMAGE_CLASSIFY_AVI_G:	
		return _T("G");
		break;

	case E_IMAGE_CLASSIFY_AVI_B:
		return _T("B");
		break;

	case E_IMAGE_CLASSIFY_AVI_BLACK:
		return _T("BLACK");
		break;

	case E_IMAGE_CLASSIFY_AVI_WHITE:
		return _T("WHITE");
		break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_32:
		return _T("G32");
		break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
		return _T("G64");
		break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_87:
		return _T("G87");
		break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_128:
		return _T("G128");
		break;

	case E_IMAGE_CLASSIFY_AVI_VTH:
		return _T("VTH");
		break;

	case E_IMAGE_CLASSIFY_AVI_DUST:
		return _T("DUST");
		break;

	case E_IMAGE_CLASSIFY_AVI_PCD:
		return _T("PCD");
		break;

	default:
		return _T("NULL");
		break;
	}

	return _T("NULL");
}

// 패턴 이름 가져오기
CStringA CInspectAlign::GetPatternStringA(int nPattern)
{
	switch(nPattern)
	{
	case E_IMAGE_CLASSIFY_AVI_R:
		return ("R");
		break;

	case E_IMAGE_CLASSIFY_AVI_G:	
		return ("G");
		break;

	case E_IMAGE_CLASSIFY_AVI_B:
		return ("B");
		break;

	case E_IMAGE_CLASSIFY_AVI_BLACK:
		return ("BLACK");
		break;

	case E_IMAGE_CLASSIFY_AVI_WHITE:
		return ("WHITE");
		break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_32:
		return ("G32");
		break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
		return ("G64");
		break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_87:
		return ("G87");
		break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_128:
		return ("G128");
		break;

	case E_IMAGE_CLASSIFY_AVI_VTH:
		return ("VTH");
		break;

	case E_IMAGE_CLASSIFY_AVI_DUST:
		return ("DUST");
		break;

	case E_IMAGE_CLASSIFY_AVI_PCD:
		return ("PCD");
		break;

	default:
		return ("NULL");
		break;
	}

	return ("NULL");
}


long CInspectAlign::CurlJudge(cv::Mat matSrcBuf, double* dpara, cv::Point* ptCorner, BOOL& bCurl, stMeasureInfo* stCurlMeasure,BOOL bSaveImage,CString strPath)
{
	if( matSrcBuf.empty())		
		return E_ERROR_CODE_EMPTY_BUFFER;

	Mat matResult;
	cv::cvtColor(matSrcBuf, matResult,CV_GRAY2RGB);

	//파라미터
	BOOL bCurlUse        = (BOOL)dpara[E_PARA_APP_CURL_USE];         //컬 판정 사용 유무
	if(bCurlUse < 1)
	{
		bCurl = FALSE;
		return E_ERROR_CODE_TRUE;
	}
	int nXRange                = (int)dpara[E_PARA_APP_CURL_RANGE_X];                 // x축 검사 범위          100
	int nYRangeTop             = (int)dpara[E_PARA_APP_CURL_RANGE_Y_TOP];             // y축 검사 범위 TOP      200
	int nYRangeBottom          = (int)dpara[E_PARA_APP_CURL_RANGE_Y_BOTTOM];          // y축 검사 범위 Bottom   200(5.99Q는 Y축 비율 Bottom이 작게)
	int nDetectionCondition	   = (int)dpara[E_PARA_APP_CURL_CONDITION_COUNT];         // 검출 조건               2
	double dSearchRatio        = dpara[E_PARA_APP_CURL_SEARCH_RATIO];            // 검출 비율              1.5
	double dStandardGVRatio    = dpara[E_PARA_APP_CURL_STANDARD_GV_RATIO];       // 평균 GV 비율           0.85
	int nCornerSearchCount     = (int)dpara[E_PARA_APP_CURL_CORNER_SEARCH_COUNT];     // 코너 검출 조건           5 
	double dCornerSearchRatio  = dpara[E_PARA_APP_CURL_CORNER_SEARCH_RATIO];     // 코너 검출 비율          2.75
	int nStartOffsetLeft       = (int)dpara[E_PARA_APP_CURL_START_OFFSET_LEFT];       // 검사 시작 오프셋		  300
	int nStartOffsetRight	   = (int)dpara[E_PARA_APP_CURL_END_OFFSET_RIGHT];      // 검사 끝 오프셋
	int nBlurSize              = (int)dpara[E_PARA_APP_CURL_BLUR_SIZE];               // 블러 사이즈		       11
	float nGausSigma		   = (float)dpara[E_PARA_APP_CURL_GAUSSIGMA];				 // 가우시안 블러 Sigma	    5
	int	  nProfileOffset_W	   = (int)dpara[E_PARA_APP_CURL_PROFILE_SEARCH_OFFSET];	//Profile에서 최소 값 이후로 Parameter 값을 너비로 사용하여 값을 남겨 사용, 너비 이후 값은 삭제

	if(nBlurSize%2==0)
		nBlurSize++;
	Size szGaussianBlurSize(nBlurSize,nBlurSize);

	Mat mtSrc;
	matSrcBuf.copyTo(mtSrc);

	cv::GaussianBlur(mtSrc, mtSrc, szGaussianBlurSize, nGausSigma);             // 가우시안 블러

#ifdef _DEBUG
	// 실행 드라이브 D:\ 고정 아님 - 필요 시 InspectLibLog 의 GETDRV() 활용
	//imwrite("D:\\IDB\\CurlBlur.bmp", mtSrc);
#endif

	std::vector<int> nGV[LINE_END];                 
	std::vector<int> nDistance[LINE_END];          
	std::vector<int> nCornerSearchDistance[LINE_END];
	Point ptLocation[4] = {ptCorner[0], ptCorner[3], ptCorner[1], ptCorner[2]};
	int nMaxGV[LINE_END]={0,0};
	int nMinGV[LINE_END]={255,255};
	int nMinGVInx[LINE_END]={0,0};
	/*double nMeanGV[LINE_END];*/
	double nDetectionCount[LINE_END];
	int nImageGV[LINE_END];
	int nCurlJudge=0;
	double dMeanDistance[LINE_END];
	double nMaxDistance[LINE_END];
	int nCornerMaxDistance[LINE_END];
	int nCount[LINE_END];	

	Mat mtCurl;
	
	matSrcBuf.copyTo(mtCurl);
	cv::cvtColor(mtCurl, mtCurl, CV_GRAY2RGB);

	CString strTest;

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
	for(int nDirection =0; nDirection < LINE_END; nDirection++)
	{
		int nStageNumOffset = 10;
		double dStageAvg=0;

		for(int nX = ptLocation[nDirection].x + nStartOffsetLeft; nX < ptLocation[nDirection+2].x - nStartOffsetRight; nX+=nXRange)
		{
			for(int nY = ptLocation[nDirection].y - nYRangeTop; nY <ptLocation[nDirection].y + nYRangeBottom; nY++)
			{
				Scalar m, s;
				cv::meanStdDev(mtSrc(cv::Rect(nX, nY, nXRange, 1)), m, s);  //nXRange 길이의 평균 GV
				nImageGV[nDirection] = (int)m[0];
				nGV[nDirection].push_back(nImageGV[nDirection]);
				if(nMinGV[nDirection] > nGV[nDirection][nGV[nDirection].size()-1])
				{
					nMinGV[nDirection] = nGV[nDirection][nGV[nDirection].size()-1];
					nMinGVInx[nDirection] = nGV[nDirection].size()-1;
				}
			}
			if(nDirection==0) // TOP
			{
				for(int nStage = 0; nStage < nStageNumOffset; nStage++)
					dStageAvg += nGV[nDirection][nStage];
				dStageAvg /= nStageNumOffset;				
			}

			if(nDirection==1) // BOTTOM
			{
				for(int nStage = nGV[nDirection].size()-1; nStage > nGV[nDirection].size()-1-10; nStage--)
					dStageAvg += nGV[nDirection][nStage];
				dStageAvg /= nStageNumOffset;
			}
			
			cv::Point pt1, pt2;
			// TOM 방향 CUR 측정할 경우
			if(nDirection == 0)
			{
				for(int n=0; n < nGV[nDirection].size(); n++)
				{
					if(nGV[nDirection][n] <= dStageAvg * dStandardGVRatio)
					{
						pt1 = cv::Point(nX, ptLocation[nDirection].y - nYRangeTop + n);
						nDetectionCount[nDirection] = 0;
						for(int m = n ; m < nGV[nDirection].size(); m++)
						{
							nDetectionCount[nDirection]++;  // 거리 측정
							if(nGV[nDirection][m] > dStageAvg*dStandardGVRatio)
							{
								pt2 = cv::Point(nX, ptLocation[nDirection].y - nYRangeTop + m);
								break;
							}
						}
						break;
					}
				}
				CString test_dist;
				test_dist.Format(_T("%.1lf"),nDetectionCount[nDirection]);
				cv::line(mtCurl, pt1, pt2, Scalar(255,0,0));
				cv::putText(mtCurl,((cv::String)(CStringA)test_dist),cv::Point(pt1.x-5,pt1.y-30),FONT_HERSHEY_SIMPLEX, 1., Scalar(255,0,0), 1);
			}

			// BUTTOM 방향 CUR 측정경우
			else if(nDirection ==1)
			{
				for(int n=nGV[nDirection].size()-1; n >=0 ; n--)
				{
					if(nGV[nDirection][n] <= dStageAvg * dStandardGVRatio)
					{
						pt1 = cv::Point(nX, ptLocation[nDirection].y - nYRangeTop + n);
						nDetectionCount[nDirection] = 0;
						for(int m = n ; m >= 0 ; m--)
						{
							nDetectionCount[nDirection]++;  // 거리 측정
							if(nGV[nDirection][m] > dStageAvg*dStandardGVRatio)
							{
								pt2 = cv::Point(nX, ptLocation[nDirection].y - nYRangeTop + m);
								break;
							}
						}
						break;
					}
				}
				CString test_dist;
				test_dist.Format(_T("%.1lf"),nDetectionCount[nDirection]);
				cv::line(mtCurl, pt1, pt2, Scalar(255,0,0));
				cv::putText(mtCurl,((cv::String)(CStringA)test_dist),cv::Point(pt1.x-5,pt1.y-30),FONT_HERSHEY_SIMPLEX, 1., Scalar(255,0,0), 1);
			}
			nDistance[nDirection].push_back(nDetectionCount[nDirection]); 
			nGV[nDirection].erase(nGV[nDirection].begin(),nGV[nDirection].end());
		}

		// 코너 부분 따로 검사
		for(int n = 0; n < nCornerSearchCount; n++)
		{
			nCornerSearchDistance[nDirection].push_back(nDistance[nDirection][n]);
		}
		for(int n = 0; n < nCornerSearchCount; n++)
		{
			nCornerSearchDistance[nDirection].push_back(nDistance[nDirection][nDistance[nDirection].size()-(n+1)]);
		}

		nCornerMaxDistance[nDirection] = *max_element(nCornerSearchDistance[nDirection].begin(), nCornerSearchDistance[nDirection].end());// 코너 부분 최댓값
		nDistance[nDirection].erase(nDistance[nDirection].begin(), nDistance[nDirection].begin()+nCornerSearchCount);
		nDistance[nDirection].erase(nDistance[nDirection].end()-nCornerSearchCount, nDistance[nDirection].end());

		nMaxDistance[nDirection] = *max_element(nDistance[nDirection].begin(), nDistance[nDirection].end()); 
		dMeanDistance[nDirection]=0;
		for(int n=0; n < nDistance[nDirection].size(); n++)
		{
			dMeanDistance[nDirection]+=nDistance[nDirection][n]; //거리 평균
		}
		dMeanDistance[nDirection] = dMeanDistance[nDirection]/nDistance[nDirection].size();
		nCount[nDirection]=0;
		for(int n = 0; n < nDistance[nDirection].size(); n++)
		{
			if(nDistance[nDirection][n] >= dMeanDistance[nDirection] * dSearchRatio) //(평균 거리 * 검출비율)보다 높은 거리 카운트
				nCount[nDirection]++;			
		}
		for(int n = 0; n < nCornerSearchDistance[nDirection].size();n++)
		{
			if(nCornerSearchDistance[nDirection][n] >= dMeanDistance[nDirection]*dCornerSearchRatio) //(평균 거리 * 검출비율)보다 높은 거리 카운트(코너 부분)
				nCount[nDirection]++;
		}
		if(nDirection==0&&nCount[nDirection]>=nDetectionCondition)
			nCurlJudge++;//컬 판정

		else if(nDirection==1&&nCount[nDirection]>=nDetectionCondition)
			nCurlJudge++;//컬 판정	
	}

	if(bSaveImage)
	{
		CString strSavePath;
		strSavePath.Format(_T("%s\\Curl_0.bmp"),strPath);
		imwrite((cv::String)(CStringA)strSavePath,mtCurl);
	}
	
	//벡터 죽이기
	for(int i=0; i<2;i++)
	{
		vector<int>().swap(nGV[i]);
		vector<int>().swap(nDistance[i]);
		vector<int>().swap(nCornerSearchDistance[i]);
	}

	for(int n=0;n<LINE_END;n++)
	{
		stCurlMeasure->dMeasureValue[n] = nMaxDistance[n];
		stCurlMeasure->dMeasureValue[n+2] = nCornerMaxDistance[n];
	}

	if(nCurlJudge>=1)
	{
		bCurl = TRUE;
		stCurlMeasure->bJudge = TRUE;
	}
	else
	{
		bCurl = FALSE;
		stCurlMeasure->bJudge = FALSE;
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::DoFindActive_LCP(cv::Mat& matSrcBuf, cv::Mat matDrawBuf,cv::Mat matBackStage,cv::Mat& matMask_Active, cv::Mat& matMask_Hole, double* dPara, double& dTheta, int nAlignImageClassify , int nCameraNum ,int nImageClassify ,cv::Point* ptResCorner, cv::Rect2i& rcCellRoi, double dCamResolution, int nRatio, cv::Point& ptCenter, cv::Point& ptAlignCenter, wchar_t* strID, wchar_t* strAlgPath)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	int nImageSave_Flag = (int)dPara[E_PARA_LCP_ALIGN_TEXT];
	int nGauSz = (int)dPara[E_PARA_LCP_ALIGN_GAUSSIAN_SIZE];
	double dGauSig = (int)dPara[E_PARA_LCP_ALIGN_GAUSSIAN_SIGMA];
	int nThreshold = (int)dPara[E_PARA_LCP_ALIGN_TH];
	int nRoundIn_Sz = (int)dPara[E_PARA_LCP_ALIGN_ROUND_IN_SZ];
	int nRoundOut_Sz = (int)dPara[E_PARA_LCP_ALIGN_ROUND_OUT_SZ];
	int nMedian = (int)dPara[E_PARA_LCP_ALIGN_MEDIAN_BLUR];
	int nCircle_MinArea = (int)dPara[E_PARA_LCP_ALIGN_BIGCIRCLE_MINAREA];
	double dHough_dp = (int)dPara[E_PARA_LCP_ALIGN_HOUGH_DP];
	double dHough_minDist = (int)dPara[E_PARA_LCP_ALIGN_HOUGH_MINDIST];
	double dHough_param1 = (int)dPara[E_PARA_LCP_ALIGN_HOUGH_PARAM1];
	double dHough_param2 = (int)dPara[E_PARA_LCP_ALIGN_HOUGH_PARAM2];
	int nHough_minRadius = (int)dPara[E_PARA_LCP_ALIGN_HOUGH_MINRADIUD];
	int nHough_maxRadius = (int)dPara[E_PARA_LCP_ALIGN_HOUGH_MAXRADIUD];

	int nActive_MaskIn = (int)dPara[E_PARA_LCP_ALIGN_ACTIVEMASK_IN];
	int nHole_MaskOut = (int)dPara[E_PARA_LCP_ALIGN_HOLEMASK_OUT];

	// 버퍼가 없는 경우.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;


	cv::Mat matGrayBuf, matTresBuf, matDrawBuf_Temp ,matTempBuf;

	// 버퍼 할당 & 초기화
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matDrawBuf_Temp = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	matDrawBuf.copyTo(matDrawBuf_Temp);

	// 컬러인 경우 ( SVI )
	if (matSrcBuf.channels() != 1)
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// 흑백인 경우 ( AVI, APP )
	else
	{
		if (matSrcBuf.type() == CV_8UC1)
			matSrcBuf.copyTo(matGrayBuf);
		else
			matSrcBuf.convertTo(matGrayBuf, CV_8UC1, 1. / 16.);

		//matSrc_8bit.copyTo(matGrayBuf);
		//matGrayBuf = matSrc_8bit.clone();
	}

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath,nImageClassify,_T("Src"));
		imwrite((cv::String)(CStringA)strSaveName, matSrcBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("cvtColor."));


	Mat M(2, 3, CV_32FC1); // warpAffine 연산에 사용되는 2*3 행렬
	vector<Vec3f> circles; // 작은 원 찾기 (안되면 타원 찾기 or PerspectiveTransform)
	vector<vector<Point>> contours; // 큰 타원 찾기
	vector<Vec4i> hierarchy;

	RotatedRect e; // 큰 타원 정보
	Vec3f find_circle; // 작은 원 정보
	float fCircle_x = 0.f, fCircle_y = 0.f, fMin_dist = 0.f, fMax_dist = 0.f, fCircle_dist = 0.f, fCal_dist = 0.f;
	double dArea;

	cv::GaussianBlur(matGrayBuf, matTresBuf, Size(nGauSz, nGauSz), dGauSig);
	cv::threshold(matTresBuf, matTresBuf, nThreshold, 255, 0);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Threshold"));
		imwrite((cv::String)(CStringA)strSaveName, matTresBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Threshold."));

	Mask_SizeDown(matTresBuf, nRoundIn_Sz);
	Mask_SizeUp(matTresBuf, nRoundOut_Sz);
	Mask_SizeDown(matTresBuf, nRoundIn_Sz);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Threshold_Round"));
		imwrite((cv::String)(CStringA)strSaveName, matTresBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Threshold_Round."));

	findContours(matTresBuf, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//////////////////////////////////////////////////////////////////////////
	if (contours.size() < 0) return E_ERROR_CODE_LCP_ALIGN_ACTIVE_MASK_FAIL;
	//////////////////////////////////////////////////////////////////////////

	bool bJudge_Active = true;

	for (int i = 0; i < contours.size(); i++) {
		dArea = contourArea(contours[i]);
		if (dArea > nCircle_MinArea) // 표본 3개 기준 전부 약 112만
		{
			e = fitEllipse(contours[i]);
			// cout << "e.angle : " << e.angle << " e.center : " << e.center << " e.size : " << e.size << endl;
			fMax_dist = e.size.height / 2; // 타원의 장축/2
			fCal_dist = e.size.width / 2; // 타원의 단축/2 (둘레 계산용)

			ellipse(matDrawBuf_Temp, e, Scalar(255, 0, 0), 2); // 그리기 체크 (타원 테두리)
			circle(matDrawBuf_Temp, e.center, 2, Scalar(0, 0, 255), 3); // 그리기 체크 (타원 중심)
			line(matDrawBuf_Temp, e.center, Point2f(e.center.x, e.center.y + e.size.height / 2), Scalar(127, 0, 127), 2, 8, 0); // 그리기 체크 (타원 장축)

			Size2f size(e.size.width / 1.3, e.size.height / 1.3); // 도넛 모양 만들기 -> 작은 원 찾는 범위
			e.size = size;
			fMin_dist = e.size.width / 2; // 안쪽 타원의 단축/2

			ellipse(matDrawBuf_Temp, e, Scalar(255, 255, 0), 2); // 그리기 체크 (안쪽 타원 테두리)
			line(matDrawBuf_Temp, e.center, Point2f(e.center.x + e.size.width / 2, e.center.y), Scalar(127, 0, 127), 2, 8, 0); // 그리기 체크 (안쪽 타원 장축)

			if (nImageClassify == nAlignImageClassify) {

				rcCellRoi = cv::boundingRect(contours[i]);
				ptAlignCenter = e.center;

				ptResCorner[E_CORNER_LEFT_TOP].x = rcCellRoi.x;
				ptResCorner[E_CORNER_LEFT_TOP].y = rcCellRoi.y;

				ptResCorner[E_CORNER_LEFT_BOTTOM].x = rcCellRoi.x;
				ptResCorner[E_CORNER_LEFT_BOTTOM].y = rcCellRoi.y + rcCellRoi.height;

				ptResCorner[E_CORNER_RIGHT_TOP].x = rcCellRoi.x + rcCellRoi.width;
				ptResCorner[E_CORNER_RIGHT_TOP].y = rcCellRoi.y;

				ptResCorner[E_CORNER_RIGHT_BOTTOM].x = rcCellRoi.x + rcCellRoi.width;
				ptResCorner[E_CORNER_RIGHT_BOTTOM].y = rcCellRoi.y + rcCellRoi.height;
			}

			rectangle(matDrawBuf_Temp, cv::boundingRect(contours[i]), Scalar(0, 0, 255), 2);
			drawContours(matTresBuf, contours, i, Scalar(255), -1);

			bJudge_Active = false;

			break;
		}
	}

	if (bJudge_Active) return E_ERROR_CODE_LCP_ALIGN_ACTIVE_MASK_FAIL;

	matTresBuf.copyTo(matMask_Active);
	Mask_SizeDown(matMask_Active, nActive_MaskIn); //외곽 범위 불량 제거

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("matMask_Active"));
		imwrite((cv::String)(CStringA)strSaveName, matMask_Active);
	}
	//////////
	// 작은 원 찾기
	medianBlur(matGrayBuf, matTempBuf, nMedian); // blur도 괜찮은듯
								   // getPerspectiveTransform();
								   // warpPerspective();
	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("medianBlur"));
		imwrite((cv::String)(CStringA)strSaveName, matTempBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("medianBlur."));


	HoughCircles(matTempBuf, circles, HOUGH_GRADIENT, dHough_dp, dHough_minDist, dHough_param1, dHough_param2, nHough_minRadius, nHough_maxRadius); // 1, 20, 255.0, 35.0, 15, 20

	if (circles.size() > 0)
	{
		for (size_t i = 0; i < circles.size(); i++)
		{
			find_circle = circles[i];
			Point2f pCircle_center(find_circle[0], find_circle[1]);
			fCircle_dist = sqrt(pow(e.center.x - pCircle_center.x, 2) + pow(e.center.y - pCircle_center.y, 2));

			line(matDrawBuf_Temp, e.center, Point2f(pCircle_center.x, pCircle_center.y), Scalar(0, 255, 255), 2, 8, 0); // 그리기 체크 (작은 원 중심점)

			if (fCircle_dist > fMin_dist && fCircle_dist < fMax_dist) // 도넛 범위안에 들어오면 완료
			{
				circle(matDrawBuf_Temp, pCircle_center, find_circle[2], Scalar(0, 255, 0), 1); // 그리기 체크 (원 테두리)
				circle(matDrawBuf_Temp, pCircle_center, 1, Scalar(0, 0, 255), 2); // 그리기 체크 (원 중심)

				circle(matMask_Hole, pCircle_center, find_circle[2], Scalar(255), -1); // 그리기 체크 (원 테두리) Mask
				Mask_SizeUp(matMask_Hole, nHole_MaskOut);

				fCircle_x = find_circle[0]; // 대표 작은 원 x좌표
				fCircle_y = find_circle[1]; // 대표 작은 원 y좌표

				break; // break를 주석으로 하면 모든 작은 원의 정보가 그려짐. 도넛 범위에 오면 원 테두리도 그려짐.
			}
		}

	

		if (nImageSave_Flag)
		{
			CString strSaveName;
			strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("matMask_Hole"));
			imwrite((cv::String)(CStringA)strSaveName, matMask_Hole);
		}
		// 도넛 범위안에 작은 원이 2개이상이면? -> 흰색 스티커를 비검사 영역으로 만들기 or 작은 원 중심점의 밝기 제한 or 도넛 중앙 근사값으로 뽑기 등.
		// 작은 원 중심점의 밝기로 필터링 하는게 가장 좋은 방법이라 생각되지만 해당 픽셀의 값을 제대로 못받아서.. 보류

		if (fCircle_x <= 0 || fCircle_y <= 0) // 도넛 범위안에 작은 원이 1개라도 없으면
		{
			return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
		}
	}
	else if (circles.size() == 0) // 작은 원을 못찾으면
	{
		return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
	}

	// 각도 계산
	// 역삼각함수로 큰 타원의 중심점과 작은 원의 중심점 간의 쎄타를 구하여 3시방향으로 회전.
	// 아크탄젠트는 radian으로 반환되므로 degree로 변환
	if (nImageClassify == nAlignImageClassify) {

		dTheta = atan2f(fCircle_y - e.center.y, fCircle_x - e.center.x) * 180.f / CV_PI;
		ptCenter.x = fCircle_x;
		ptCenter.y = fCircle_y;

	}
	else
	{
		// 
		Mat M2(2, 3, CV_32FC1);
		M2.at<float>(0, 0) = 1.f;
		M2.at<float>(1, 0) = 0.f;
		M2.at<float>(0, 1) = 0.f;
		M2.at<float>(1, 1) = 1.f; // 단위 행렬 -> 즉, 각도는 그대로
		M2.at<float>(0, 2) = ptCenter.x - fCircle_x; // 첫 영상의 작은 원을 기준으로 x축 이동값
		M2.at<float>(1, 2) = ptCenter.y - fCircle_y; // 첫 영상의 작은 원을 기준으로 y축 이동값
		cv::warpAffine(matSrcBuf, matSrcBuf, M2, matSrcBuf.size()); // 이동
																  //  		boundingRect.x += (fMain_Stan[0] - fCircle_x);
																  //  		boundingRect.y += (fMain_Stan[1] - fCircle_y);
	}


	// 타원의 정보
// 	double dRes_P = 2 * CV_PI * sqrt((pow(fMax_dist * 2, 2) + pow(fCal_dist * 2, 2)) / 2) * um; // 2π * sqrt((장축^2 + 단축^2) / 2)
// 	double dRes_area = CV_PI * fMax_dist * fCal_dist * um; // π * 단축/2 * 장축/2

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Align_Result"));
		imwrite((cv::String)(CStringA)strSaveName, matSrcBuf);
	}

	
	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Align_Draw"));
		imwrite((cv::String)(CStringA)strSaveName, matDrawBuf_Temp);
	}

	

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::DoFindActive_LCP_Limpidity(cv::Mat& matSrcBuf, cv::Mat matDrawBuf, cv::Mat matStage, cv::Mat& matMask_Active, cv::Mat& matMask_Hole, double* dPara, double& dTheta, int nAlignImageClassify, int nCameraNum, int nImageClassify, cv::Point* ptResCorner, cv::Rect2i& rcCellRoi, double dCamResolution, int nRatio, cv::Point& ptCenter, cv::Point& ptAlignCenter, wchar_t* strID, wchar_t* strAlgPath)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	int nImageSave_Flag = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_TEXT];
	int nEdgeTH = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_EDGE_TH];
	int nMedian = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_MEDIAN_BLUR];
	int nCircle_MinArea = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_BIGCIRCLE_MINAREA];
	double dHough_dp = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_HOUGH_DP];
	double dHough_minDist = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_HOUGH_MINDIST];
	double dHough_param1 = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_HOUGH_PARAM1];
	double dHough_param2 = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_HOUGH_PARAM2];
	int nHough_minRadius = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_HOUGH_MINRADIUD];
	int nHough_maxRadius = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_HOUGH_MAXRADIUD];

	int nActive_MaskIn = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_ACTIVEMASK_IN];
	int nHole_MaskOut = (int)dPara[E_PARA_LCP_ALIGN_LIMPI_HOLEMASK_OUT];

	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;


	cv::Mat matGrayBuf, matTresBuf, matDrawBuf_Temp, matTempBuf;

	// 버퍼 할당 & 초기화
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matDrawBuf_Temp = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	matDrawBuf.copyTo(matDrawBuf_Temp);

	// 컬러인 경우 ( SVI )
	if (matSrcBuf.channels() != 1)
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// 흑백인 경우 ( AVI, APP )
	else
	{
		if (matSrcBuf.type() == CV_8UC1)
			matSrcBuf.copyTo(matGrayBuf);
		else
			matSrcBuf.convertTo(matGrayBuf, CV_8UC1, 1. / 16.);

		//matSrc_8bit.copyTo(matGrayBuf);
		//matGrayBuf = matSrc_8bit.clone();
	}

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Src"));
		imwrite((cv::String)(CStringA)strSaveName, matSrcBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("cvtColor."));

	cv::subtract(matStage, matGrayBuf, matGrayBuf);

	///////////////////////////////////
	vector<Point> vPoint_Circle;
	cv::Mat matTemp = cv::Mat::zeros(matGrayBuf.size(), matGrayBuf.type());

	for (int nMode = 0; nMode < 2; nMode++) {

		uchar nPre_Gv = 0;

		switch (nMode)
		{
		case 0: {
			for (int nx = 0; nx < matGrayBuf.cols; nx++) {
				for (int ny = 0; ny < matGrayBuf.rows; ny++) {
					if (ny == 0) {
						nPre_Gv = matGrayBuf.at<uchar>(ny, nx);
					}
					else {
						int nSub = abs(matGrayBuf.at<uchar>(ny, nx) - nPre_Gv);
						if (nSub > nEdgeTH) {
							vPoint_Circle.push_back(cv::Point(nx, ny));
							break;
						}
					}
				}
			}


			break;
		}

		case 1: {
			for (int nx = 0; nx < matGrayBuf.cols; nx++) {
				for (int ny = matGrayBuf.rows - 1; ny > 0; ny--) {
					if (ny == matGrayBuf.rows - 1) {
						nPre_Gv = matGrayBuf.at<uchar>(ny, nx);
					}
					else {
						int nSub = abs(matGrayBuf.at<uchar>(ny, nx) - nPre_Gv);
						if (nSub > nEdgeTH) {
							vPoint_Circle.push_back(cv::Point(nx, ny));
							break;
						}
					}
				}
			}

			break;
		}

		default:
			break;
		}
	}

	// 	for (int i = 0; i < vPoint_Circle.size(); i++) {
	// 		circle(matTemp, vPoint_Circle[i], 1, Scalar(155), 2); // 그리기 체크 (원 테두리)
	// 	}

	if (vPoint_Circle.size() == 0) // Edge 못찾으면 에러
	{
		return E_ERROR_CODE_LCP_ALIGN_ACTIVE_MASK_FAIL;
	}

	RotatedRect e2;
	e2 = cv::fitEllipse(vPoint_Circle);

	cv::ellipse(matTemp, e2, Scalar(255), -1); // 그리기 체크 (타원 테두리)

	//imwrite("D:\\Align_Test.bmp", matTemp);
	///////////////////////////////////

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Stage sub"));
		imwrite((cv::String)(CStringA)strSaveName, matGrayBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Stage sub."));

	Mat M(2, 3, CV_32FC1); // warpAffine 연산에 사용되는 2*3 행렬
	vector<Vec3f> circles; // 작은 원 찾기 (안되면 타원 찾기 or PerspectiveTransform)
	vector<vector<Point>> contours; // 큰 타원 찾기
	vector<Vec4i> hierarchy;

	RotatedRect e; // 큰 타원 정보
	Vec3f find_circle; // 작은 원 정보
	float fCircle_x = 0.f, fCircle_y = 0.f, fMin_dist = 0.f, fMax_dist = 0.f, fCircle_dist = 0.f, fCal_dist = 0.f;
	double dArea;

	matTemp.copyTo(matTresBuf);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Threshold"));
		imwrite((cv::String)(CStringA)strSaveName, matTresBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Threshold."));

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Threshold_Round"));
		imwrite((cv::String)(CStringA)strSaveName, matTresBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Threshold_Round."));

	findContours(matTresBuf, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//////////////////////////////////////////////////////////////////////////
	if (contours.size() < 0) return E_ERROR_CODE_LCP_ALIGN_ACTIVE_MASK_FAIL;
	//////////////////////////////////////////////////////////////////////////

	bool bJudge_Active = true;

	for (int i = 0; i < contours.size(); i++) {
		dArea = contourArea(contours[i]);
		if (dArea > nCircle_MinArea) // 표본 3개 기준 전부 약 112만
		{
			e = fitEllipse(contours[i]);
			// cout << "e.angle : " << e.angle << " e.center : " << e.center << " e.size : " << e.size << endl;
			fMax_dist = e.size.height / 2; // 타원의 장축/2
			fCal_dist = e.size.width / 2; // 타원의 단축/2 (둘레 계산용)

			ellipse(matDrawBuf_Temp, e, Scalar(255, 0, 0), 2); // 그리기 체크 (타원 테두리)
			circle(matDrawBuf_Temp, e.center, 2, Scalar(0, 0, 255), 3); // 그리기 체크 (타원 중심)
			line(matDrawBuf_Temp, e.center, Point2f(e.center.x, e.center.y + e.size.height / 2), Scalar(127, 0, 127), 2, 8, 0); // 그리기 체크 (타원 장축)

			Size2f size(e.size.width / 1.3, e.size.height / 1.3); // 도넛 모양 만들기 -> 작은 원 찾는 범위
			e.size = size;
			fMin_dist = e.size.width / 2; // 안쪽 타원의 단축/2

			ellipse(matDrawBuf_Temp, e, Scalar(255, 255, 0), 2); // 그리기 체크 (안쪽 타원 테두리)
			line(matDrawBuf_Temp, e.center, Point2f(e.center.x + e.size.width / 2, e.center.y), Scalar(127, 0, 127), 2, 8, 0); // 그리기 체크 (안쪽 타원 장축)

			if (nImageClassify == nAlignImageClassify) {

				rcCellRoi = cv::boundingRect(contours[i]);
				ptAlignCenter = e.center;

				ptResCorner[E_CORNER_LEFT_TOP].x = rcCellRoi.x;
				ptResCorner[E_CORNER_LEFT_TOP].y = rcCellRoi.y;

				ptResCorner[E_CORNER_LEFT_BOTTOM].x = rcCellRoi.x;
				ptResCorner[E_CORNER_LEFT_BOTTOM].y = rcCellRoi.y + rcCellRoi.height;

				ptResCorner[E_CORNER_RIGHT_TOP].x = rcCellRoi.x + rcCellRoi.width;
				ptResCorner[E_CORNER_RIGHT_TOP].y = rcCellRoi.y;

				ptResCorner[E_CORNER_RIGHT_BOTTOM].x = rcCellRoi.x + rcCellRoi.width;
				ptResCorner[E_CORNER_RIGHT_BOTTOM].y = rcCellRoi.y + rcCellRoi.height;
			}

			rectangle(matDrawBuf_Temp, cv::boundingRect(contours[i]), Scalar(0, 0, 255), 2);
			drawContours(matTresBuf, contours, i, Scalar(255), -1);

			bJudge_Active = false;

			break;
		}
	}

	if (bJudge_Active) return E_ERROR_CODE_LCP_ALIGN_ACTIVE_MASK_FAIL;

	matTresBuf.copyTo(matMask_Active);
	//Mask_SizeDown(matMask_Active, 150);
	Mask_SizeDown(matMask_Active, nActive_MaskIn);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("matMask_Active"));
		imwrite((cv::String)(CStringA)strSaveName, matMask_Active);
	}


	//////////
	// 작은 원 찾기
	medianBlur(matGrayBuf, matTempBuf, nMedian); // blur도 괜찮은듯
												 // getPerspectiveTransform();
												 // warpPerspective();
	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("medianBlur"));
		imwrite((cv::String)(CStringA)strSaveName, matTempBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("medianBlur."));


	HoughCircles(matTempBuf, circles, HOUGH_GRADIENT, dHough_dp, dHough_minDist, dHough_param1, dHough_param2, nHough_minRadius, nHough_maxRadius); // 1, 20, 255.0, 35.0, 15, 20

	if (circles.size() > 0)
	{
		for (size_t i = 0; i < circles.size(); i++)
		{
			find_circle = circles[i];
			Point2f pCircle_center(find_circle[0], find_circle[1]);
			fCircle_dist = sqrt(pow(e.center.x - pCircle_center.x, 2) + pow(e.center.y - pCircle_center.y, 2));

			line(matDrawBuf_Temp, e.center, Point2f(pCircle_center.x, pCircle_center.y), Scalar(0, 255, 255), 2, 8, 0); // 그리기 체크 (작은 원 중심점)

			if (fCircle_dist > fMin_dist && fCircle_dist < fMax_dist) // 도넛 범위안에 들어오면 완료
			{
				circle(matDrawBuf_Temp, pCircle_center, find_circle[2], Scalar(0, 255, 0), 1); // 그리기 체크 (원 테두리)
				circle(matDrawBuf_Temp, pCircle_center, 1, Scalar(0, 0, 255), 2); // 그리기 체크 (원 중심)

				circle(matMask_Hole, pCircle_center, find_circle[2], Scalar(255), -1); // 그리기 체크 (원 테두리) Mask
				//Mask_SizeUp(matMask_Hole, 20);
				Mask_SizeUp(matMask_Hole, nHole_MaskOut);

				fCircle_x = find_circle[0]; // 대표 작은 원 x좌표
				fCircle_y = find_circle[1]; // 대표 작은 원 y좌표

				break; // break를 주석으로 하면 모든 작은 원의 정보가 그려짐. 도넛 범위에 오면 원 테두리도 그려짐.
			}
		}

		if (nImageSave_Flag)
		{
			CString strSaveName;
			strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("matMask_Hole"));
			imwrite((cv::String)(CStringA)strSaveName, matMask_Hole);
		}

		// 도넛 범위안에 작은 원이 2개이상이면? -> 흰색 스티커를 비검사 영역으로 만들기 or 작은 원 중심점의 밝기 제한 or 도넛 중앙 근사값으로 뽑기 등.
		// 작은 원 중심점의 밝기로 필터링 하는게 가장 좋은 방법이라 생각되지만 해당 픽셀의 값을 제대로 못받아서.. 보류

		if (fCircle_x <= 0 || fCircle_y <= 0) // 도넛 범위안에 작은 원이 1개라도 없으면
		{
			return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
		}
	}
	else if (circles.size() == 0) // 작은 원을 못찾으면
	{
		return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
	}

	// 각도 계산
	// 역삼각함수로 큰 타원의 중심점과 작은 원의 중심점 간의 쎄타를 구하여 3시방향으로 회전.
	// 아크탄젠트는 radian으로 반환되므로 degree로 변환
	if (nImageClassify == nAlignImageClassify) {

		dTheta = atan2f(fCircle_y - e.center.y, fCircle_x - e.center.x) * 180.f / CV_PI;
		ptCenter.x = fCircle_x;
		ptCenter.y = fCircle_y;

	}
// 	else
// 	{
// 		// 
// 		Mat M2(2, 3, CV_32FC1);
// 		M2.at<float>(0, 0) = 1.f;
// 		M2.at<float>(1, 0) = 0.f;
// 		M2.at<float>(0, 1) = 0.f;
// 		M2.at<float>(1, 1) = 1.f; // 단위 행렬 -> 즉, 각도는 그대로
// 		M2.at<float>(0, 2) = ptCenter.x - fCircle_x; // 첫 영상의 작은 원을 기준으로 x축 이동값
// 		M2.at<float>(1, 2) = ptCenter.y - fCircle_y; // 첫 영상의 작은 원을 기준으로 y축 이동값
// 		cv::warpAffine(matSrcBuf, matSrcBuf, M2, matSrcBuf.size()); // 이동
// 																	//  		boundingRect.x += (fMain_Stan[0] - fCircle_x);
// 																	//  		boundingRect.y += (fMain_Stan[1] - fCircle_y);
// 	}


	// 타원의 정보
// 	double dRes_P = 2 * CV_PI * sqrt((pow(fMax_dist * 2, 2) + pow(fCal_dist * 2, 2)) / 2) * um; // 2π * sqrt((장축^2 + 단축^2) / 2)
// 	double dRes_area = CV_PI * fMax_dist * fCal_dist * um; // π * 단축/2 * 장축/2

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Align_Result"));
		imwrite((cv::String)(CStringA)strSaveName, matSrcBuf);
	}


	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Align_Draw"));
		imwrite((cv::String)(CStringA)strSaveName, matDrawBuf_Temp);
	}



	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// long CInspectAlign::DoFindActive_LCP_Mask_D(cv::Mat matSrcBuf, double* dPara, cv::Mat& matDstActive, cv::Mat& matDstHole, int nCameraNum, int nImageClassify, double dCamResolution, int nRatio, cv::Point& ptCenter, wchar_t* strID, wchar_t* strAlgPath)
// {
// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));
// 
// 	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
// 	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;
// 
// 	int nImageSave_Flag = (int)dPara[E_PARA_LCP_MASK_TEXT];
// // 	int nThreshActive = (int)dPara[E_PARA_LCP_MASK_ACTIVE_TH];
// // 	int nResizeActive = (int)dPara[E_PARA_LCP_MASK_ACTIVE_RESIZE];
// // 	int nVarianceActive = dPara[E_PARA_LCP_MASK_ACTIVE_VARIANCE];
// 
// // 	int nThreshLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_TH];
// // 	int nGauSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_GAU_SZ];
// // 	double dGauSigLabel = (double)dPara[E_PARA_LCP_MASK_LABEL_GAU_SIG];
// // 	int nMaxSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MAX];
// // 	int nMinSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MIN];
// 
// 
// // 	int nThreshHole = (int)dPara[E_PARA_LCP_MASK_CIRCLE_TH];
// // 	int nOffset = (int)dPara[E_PARA_LCP_MASK_CIRCLE_OFFSET];
// 
// 	// 버퍼가 없는 경우.
// 	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
// 
// 
// 	cv::Mat matGrayBuf, matTresBuf, matVariBuf;
// 
// 	// 버퍼 할당 & 초기화
// 	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 	matVariBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 
// 	// 컬러인 경우 ( SVI )
// 	if (matSrcBuf.channels() != 1)
// 		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
// 	// 흑백인 경우 ( AVI, APP )
// 	else
// 	{
// 		if (matSrcBuf.type() == CV_8UC1)
// 			matSrcBuf.copyTo(matGrayBuf);
// 		else
// 			matSrcBuf.convertTo(matGrayBuf, CV_8UC1, 1. / 16.);
// 
// 		//matSrc_8bit.copyTo(matGrayBuf);
// 		//matGrayBuf = matSrc_8bit.clone();
// 	}
// 
// 
// 
// // 	cv::Mat matThreshold;
// // 
// // 	// Active 영역을 찾기 위한 이진화
// // 	cv::resize(matGrayBuf, matGrayBuf, matGrayBuf.size() / nResizeActive);
// // 	cv::resize(matVariBuf, matVariBuf, matVariBuf.size() / nResizeActive);
// // 
// // 	VarianceFilter(matGrayBuf, matVariBuf, nVarianceActive);
// // 
// // 	cv::resize(matGrayBuf, matGrayBuf, matSrcBuf.size());
// // 	cv::resize(matVariBuf, matVariBuf, matSrcBuf.size());
// // 
// // 	if (nImageSave_Flag)
// // 	{
// // 		CString strSaveName;
// // 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("0_MakeActive_Variance"));
// // 		imwrite((cv::String)(CStringA)strSaveName, matVariBuf);
// // 	}
// // 
// // 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeActive_Variance."));
// // 
// // 	cv::threshold(matVariBuf, matTresBuf, nThreshActive, 255, THRESH_BINARY);
// // 
// // 	if (nImageSave_Flag)
// // 	{
// // 		CString strSaveName;
// // 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("0_MakeActive_Treshold"));
// // 		imwrite((cv::String)(CStringA)strSaveName, matTresBuf);
// // 	}
// // 
// // 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeActive_Threshold."));
// // 
// // 	// Active Mask 만들기
// // 	MakeActiveMask(matTresBuf, matDstActive, nThreshActive);
// // 
// 	if (nImageSave_Flag)
// 	{
// 		CString strSaveName;
// 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("0_MakeActive_Result"));
// 		imwrite((cv::String)(CStringA)strSaveName, matDstActive);
// 	}
// // 
//  	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeActive_Result."));
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// 	cv::Mat Test;
// 	// 	cv::Mat Test2;
// 	// 
// 	// 	cv::distanceTransform(matDstActive, Test, CV_DIST_L2, 3);
// 	// 	cv::threshold(Test, Test, 50, 255.0, CV_THRESH_BINARY);
// 	// 	Test.convertTo(Test2, matDstActive.type());
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	// Active 영역만 남기기
// 	//cv::bitwise_and(matSrcBuf, matDstActive, matGrayBuf);
// 
// 
// 	//////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// // 	// 여기서 부터가 Label 찾기
// // 	cv::Mat matGauss;
// // 	cv::GaussianBlur(matGrayBuf, matGauss, Size(nGauSzLabel, nGauSzLabel), dGauSigLabel);
// // 
// // 	if (nImageSave_Flag)
// // 	{
// // 		CString strSaveName;
// // 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_Gau"));
// // 		imwrite((cv::String)(CStringA)strSaveName, matGauss);
// // 	}
// // 
// // 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_Gau."));
// // 
// // 
// // 	cv::Mat Maximum = Mat::zeros(matGauss.size(), matGauss.type());
// // 	cv::Mat Minimum = Mat::zeros(matGauss.size(), matGauss.type());;
// // 
// // 	MinimumFilter(matGauss, Minimum, nMinSzLabel);
// // 
// // 	MaximumFilter(matGauss, Maximum, nMaxSzLabel);
// // 
// // 	cv::subtract(Maximum, Minimum, Maximum);
// // 
// // 	if (nImageSave_Flag)
// // 	{
// // 		CString strSaveName;
// // 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_MaxMin_Sub"));
// // 		imwrite((cv::String)(CStringA)strSaveName, Maximum);
// // 	}
// // 
// // 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_MaxMin_Sub."));
// // 
// // 	/*cv::imwrite("D:\\TEST\\Mask\\test_2_3.jpg", Maximum);*/
// // 
// // 	cv::threshold(Maximum, Maximum, nThreshLabel, 255, THRESH_BINARY);
// // 
// // 	////////////////////////////////// 정리중
// // 
// // 	if (nImageSave_Flag)
// // 	{
// // 		CString strSaveName;
// // 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_Threshold"));
// // 		imwrite((cv::String)(CStringA)strSaveName, Maximum);
// // 	}
// // 
// // 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_Treshold."));
// // 
// // 	//cv::bitwise_and(Maximum, Test2, Maximum);
// // 
// // 
// // 	// Lable Mask 만들기
// // 	MakeLabelMask(Maximum, matDstLabel);
// // 
// // 	if (nImageSave_Flag)
// // 	{
// // 		CString strSaveName;
// // 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_Result"));
// // 		imwrite((cv::String)(CStringA)strSaveName, matDstLabel);
// // 	}
// // 
// // 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_Result."));
// 
// 	/*cv::imwrite("D:\\TEST\\Mask\\Label_Mask.jpg", matDstLabel);*/
// 	//////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// 	// Hole Mask 만들기
// 
// 	cv::Mat mHoleMask = Mat::zeros(matGrayBuf.size(), matGrayBuf.type());
// 	 
// 	//cv::GaussianBlur(matGrayBuf, matGrayBuf, Size(3, 3), 3.0);
// 
// 	cv::threshold(matGrayBuf, matDstHole, nThreshHole, 255, THRESH_BINARY_INV);
// 
// 	cv::bitwise_and(matDstHole, matDstActive, matDstHole);
// 
// 	//cv::subtract(matDstHole, matDstLabel, matDstHole);
// 
// 	// 	MinimumFilter(matDstHole, matDstHole, 15);
// 	// 	MaximumFilter(matDstHole, matDstHole, 15);
// 
// 
// 
// 	cv::Point LT = cv::Point(ptCenter.x - nOffset, ptCenter.y - nOffset);
// 	cv::Point RB = cv::Point(ptCenter.x + nOffset, ptCenter.y + nOffset);
// 
// 	if (LT.x > 0 && LT.y > 0 && RB.x < matGrayBuf.cols && RB.y < matGrayBuf.rows) {
// 		cv::Rect2i rHole = Rect2i(LT, RB);
// 		matDstHole(rHole).copyTo(mHoleMask(rHole));
// 		mHoleMask.copyTo(matDstHole);
// 	}
// 
// 	Mask_SizeUp(matDstHole, 10);
// 
// 	if (nImageSave_Flag)
// 	{
// 		CString strSaveName;
// 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("2_MakeHole_Result"));
// 		imwrite((cv::String)(CStringA)strSaveName, matDstHole);
// 	}
// 
// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeHole_Result."));
// 
// 	// 메모리 정리
// 	// 미리미리 해두면 좋지만 일단 Test... 
// 
// //	matThreshold.release();
// // 	matGauss.release();
// // 	Maximum.release();
// // 	Maximum.release();
// 
// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));
// 
// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}
// 
// 	return E_ERROR_CODE_TRUE;
// }

// long CInspectAlign::DoFindActive_LCP_Mask_B(cv::Mat matSrcBuf, double* dPara, cv::Mat& matDstActive, cv::Mat& matDstHole, int nCameraNum, int nImageClassify, double dCamResolution, int nRatio, cv::Point& ptCenter, wchar_t* strID, wchar_t* strAlgPath)
// {
// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));
// 
// 	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
// 	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;
// 
// 	//int nImageSave_Flag = (int)dPara[E_PARA_LCP_MASK_TEXT];
// 	// 	int nThreshActive = (int)dPara[E_PARA_LCP_MASK_ACTIVE_TH];
// 	// 	int nResizeActive = (int)dPara[E_PARA_LCP_MASK_ACTIVE_RESIZE];
// 	// 	int nVarianceActive = dPara[E_PARA_LCP_MASK_ACTIVE_VARIANCE];
// 
// 	// 	int nThreshLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_TH];
// 	// 	int nGauSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_GAU_SZ];
// 	// 	double dGauSigLabel = (double)dPara[E_PARA_LCP_MASK_LABEL_GAU_SIG];
// 	// 	int nMaxSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MAX];
// 	// 	int nMinSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MIN];
// 
// 
// // 	int nThreshHole = (int)dPara[E_PARA_LCP_MASK_CIRCLE_TH];
// // 	int nOffset = (int)dPara[E_PARA_LCP_MASK_CIRCLE_OFFSET];
// 
// 	// 버퍼가 없는 경우.
// 	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
// 
// 
// 	cv::Mat matGrayBuf, matTresBuf, matVariBuf;
// 
// 	// 버퍼 할당 & 초기화
// 	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 	matVariBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 
// 	// 컬러인 경우 ( SVI )
// 	if (matSrcBuf.channels() != 1)
// 		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
// 	// 흑백인 경우 ( AVI, APP )
// 	else
// 	{
// 		if (matSrcBuf.type() == CV_8UC1)
// 			matSrcBuf.copyTo(matGrayBuf);
// 		else
// 			matSrcBuf.convertTo(matGrayBuf, CV_8UC1, 1. / 16.);
// 
// 		//matSrc_8bit.copyTo(matGrayBuf);
// 		//matGrayBuf = matSrc_8bit.clone();
// 	}
// 
// 
// 
// 	// 	cv::Mat matThreshold;
// 	// 
// 	// 	// Active 영역을 찾기 위한 이진화
// 	// 	cv::resize(matGrayBuf, matGrayBuf, matGrayBuf.size() / nResizeActive);
// 	// 	cv::resize(matVariBuf, matVariBuf, matVariBuf.size() / nResizeActive);
// 	// 
// 	// 	VarianceFilter(matGrayBuf, matVariBuf, nVarianceActive);
// 	// 
// 	// 	cv::resize(matGrayBuf, matGrayBuf, matSrcBuf.size());
// 	// 	cv::resize(matVariBuf, matVariBuf, matSrcBuf.size());
// 	// 
// 	// 	if (nImageSave_Flag)
// 	// 	{
// 	// 		CString strSaveName;
// 	// 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("0_MakeActive_Variance"));
// 	// 		imwrite((cv::String)(CStringA)strSaveName, matVariBuf);
// 	// 	}
// 	// 
// 	// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeActive_Variance."));
// 	// 
// 	// 	cv::threshold(matVariBuf, matTresBuf, nThreshActive, 255, THRESH_BINARY);
// 	// 
// 	// 	if (nImageSave_Flag)
// 	// 	{
// 	// 		CString strSaveName;
// 	// 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("0_MakeActive_Treshold"));
// 	// 		imwrite((cv::String)(CStringA)strSaveName, matTresBuf);
// 	// 	}
// 	// 
// 	// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeActive_Threshold."));
// 	// 
// 	// 	// Active Mask 만들기
// 	// 	MakeActiveMask(matTresBuf, matDstActive, nThreshActive);
// 	// 
// 	if (nImageSave_Flag)
// 	{
// 		CString strSaveName;
// 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("0_MakeActive_Result"));
// 		imwrite((cv::String)(CStringA)strSaveName, matDstActive);
// 	}
// 	// 
// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeActive_Result."));
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// 	cv::Mat Test;
// 	// 	cv::Mat Test2;
// 	// 
// 	// 	cv::distanceTransform(matDstActive, Test, CV_DIST_L2, 3);
// 	// 	cv::threshold(Test, Test, 50, 255.0, CV_THRESH_BINARY);
// 	// 	Test.convertTo(Test2, matDstActive.type());
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	// Active 영역만 남기기
// 	//cv::bitwise_and(matSrcBuf, matDstActive, matGrayBuf);
// 
// 
// 	//////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// 	// 	// 여기서 부터가 Label 찾기
// 	// 	cv::Mat matGauss;
// 	// 	cv::GaussianBlur(matGrayBuf, matGauss, Size(nGauSzLabel, nGauSzLabel), dGauSigLabel);
// 	// 
// 	// 	if (nImageSave_Flag)
// 	// 	{
// 	// 		CString strSaveName;
// 	// 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_Gau"));
// 	// 		imwrite((cv::String)(CStringA)strSaveName, matGauss);
// 	// 	}
// 	// 
// 	// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_Gau."));
// 	// 
// 	// 
// 	// 	cv::Mat Maximum = Mat::zeros(matGauss.size(), matGauss.type());
// 	// 	cv::Mat Minimum = Mat::zeros(matGauss.size(), matGauss.type());;
// 	// 
// 	// 	MinimumFilter(matGauss, Minimum, nMinSzLabel);
// 	// 
// 	// 	MaximumFilter(matGauss, Maximum, nMaxSzLabel);
// 	// 
// 	// 	cv::subtract(Maximum, Minimum, Maximum);
// 	// 
// 	// 	if (nImageSave_Flag)
// 	// 	{
// 	// 		CString strSaveName;
// 	// 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_MaxMin_Sub"));
// 	// 		imwrite((cv::String)(CStringA)strSaveName, Maximum);
// 	// 	}
// 	// 
// 	// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_MaxMin_Sub."));
// 	// 
// 	// 	/*cv::imwrite("D:\\TEST\\Mask\\test_2_3.jpg", Maximum);*/
// 	// 
// 	// 	cv::threshold(Maximum, Maximum, nThreshLabel, 255, THRESH_BINARY);
// 	// 
// 	// 	////////////////////////////////// 정리중
// 	// 
// 	// 	if (nImageSave_Flag)
// 	// 	{
// 	// 		CString strSaveName;
// 	// 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_Threshold"));
// 	// 		imwrite((cv::String)(CStringA)strSaveName, Maximum);
// 	// 	}
// 	// 
// 	// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_Treshold."));
// 	// 
// 	// 	//cv::bitwise_and(Maximum, Test2, Maximum);
// 	// 
// 	// 
// 	// 	// Lable Mask 만들기
// 	// 	MakeLabelMask(Maximum, matDstLabel);
// 	// 
// 	// 	if (nImageSave_Flag)
// 	// 	{
// 	// 		CString strSaveName;
// 	// 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_Result"));
// 	// 		imwrite((cv::String)(CStringA)strSaveName, matDstLabel);
// 	// 	}
// 	// 
// 	// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_Result."));
// 
// 	/*cv::imwrite("D:\\TEST\\Mask\\Label_Mask.jpg", matDstLabel);*/
// 	//////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// 	// Hole Mask 만들기
// 
// 	cv::Mat mHoleMask = Mat::zeros(matGrayBuf.size(), matGrayBuf.type());
// 
// 	//cv::GaussianBlur(matGrayBuf, matGrayBuf, Size(3, 3), 3.0);
// 
// 	cv::threshold(matGrayBuf, matDstHole, nThreshHole, 255, THRESH_BINARY);
// 
// 	cv::bitwise_and(matDstHole, matDstActive, matDstHole);
// 
// 	//cv::subtract(matDstHole, matDstLabel, matDstHole);
// 
// 	// 	MinimumFilter(matDstHole, matDstHole, 15);
// 	// 	MaximumFilter(matDstHole, matDstHole, 15);
// 
// 
// 
// 	cv::Point LT = cv::Point(ptCenter.x - nOffset, ptCenter.y - nOffset);
// 	cv::Point RB = cv::Point(ptCenter.x + nOffset, ptCenter.y + nOffset);
// 
// 	if (LT.x > 0 && LT.y > 0 && RB.x < matGrayBuf.cols && RB.y < matGrayBuf.rows) {
// 		cv::Rect2i rHole = Rect2i(LT, RB);
// 		matDstHole(rHole).copyTo(mHoleMask(rHole));
// 		mHoleMask.copyTo(matDstHole);
// 	}
// 
// 	Mask_SizeUp(matDstHole, 10);
// 
// 	if (nImageSave_Flag)
// 	{
// 		CString strSaveName;
// 		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("2_MakeHole_Result"));
// 		imwrite((cv::String)(CStringA)strSaveName, matDstHole);
// 	}
// 
// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeHole_Result."));
// 
// 	// 메모리 정리
// 	// 미리미리 해두면 좋지만 일단 Test... 
// 
// 	//	matThreshold.release();
// 	// 	matGauss.release();
// 	// 	Maximum.release();
// 	// 	Maximum.release();
// 
// 	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));
// 
// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}
// 
// 	return E_ERROR_CODE_TRUE;
// }


long CInspectAlign::DoFindActive_LCP_Mask(cv::Mat matSrcBuf, double* dPara, cv::Mat& matDstActive, cv::Mat& matDstLabel, cv::Mat& matDstHole, int nCameraNum, int nImageClassify, double dCamResolution, int nRatio, cv::Point& ptCenter, wchar_t* strID, wchar_t* strAlgPath)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	int nImageSave_Flag = (int)dPara[E_PARA_LCP_MASK_LABEL_TEXT];

	int nThreshLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_TH];
	int nGauSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_GAU_SZ];
	double dGauSigLabel = (double)dPara[E_PARA_LCP_MASK_LABEL_GAU_SIG];
	int nMaxSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MAX];
	int nMinSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MIN];
	int nRound_In = (int)dPara[E_PARA_LCP_MASK_LABEL_ROUND_IN];
	int nRound_Out = (int)dPara[E_PARA_LCP_MASK_LABEL_ROUND_OUT];
	// 버퍼가 없는 경우.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;


	cv::Mat matGrayBuf, matTresBuf, matVariBuf;

	// 버퍼 할당 & 초기화
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matVariBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	// 컬러인 경우 ( SVI )
	if (matSrcBuf.channels() != 1)
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// 흑백인 경우 ( AVI, APP )
	else
	{
		if (matSrcBuf.type() == CV_8UC1)
			matSrcBuf.copyTo(matGrayBuf);
		else
			matSrcBuf.convertTo(matGrayBuf, CV_8UC1, 1. / 16.);

	}

	// 여기서 부터가 Label 찾기
	cv::Mat matGauss;
	cv::GaussianBlur(matGrayBuf, matGauss, Size(nGauSzLabel, nGauSzLabel), dGauSigLabel);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_Gau"));
		imwrite((cv::String)(CStringA)strSaveName, matGauss);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_Gau."));

	cv::Mat Maximum = Mat::zeros(matGauss.size(), matGauss.type());
	cv::Mat Minimum = Mat::zeros(matGauss.size(), matGauss.type());;

	MinimumFilter(matGauss, Minimum, nMinSzLabel);

	MaximumFilter(matGauss, Maximum, nMaxSzLabel);

	cv::subtract(Maximum, Minimum, Maximum);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_MaxMin_Sub"));
		imwrite((cv::String)(CStringA)strSaveName, Maximum);
	}

	cv::threshold(Maximum, Maximum, nThreshLabel, 255, THRESH_BINARY);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_Threshold"));
		imwrite((cv::String)(CStringA)strSaveName, Maximum);
	}

	cv::Mat matActiveMask_Temp;
	matDstActive.copyTo(matActiveMask_Temp);
	Mask_SizeDown(matActiveMask_Temp, nRound_In);

	cv::bitwise_and(Maximum, matActiveMask_Temp, Maximum);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_Threshold_Sub"));
		imwrite((cv::String)(CStringA)strSaveName, Maximum);
	}

	// Lable Mask 만들기
	MakeLabelMask(Maximum, matDstLabel);

	Mask_SizeUp(matDstLabel, nRound_Out);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("1_MakeLabel_Result"));
		imwrite((cv::String)(CStringA)strSaveName, matDstLabel);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_Result."));


	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::DoFindActive_LCP_BACK(cv::Mat& matSrcBuf, cv::Mat& matBackStageBuf, double* dPara, int nAlignImageClassify, int nCameraNum, int nImageClassify, cv::Rect2i& rcActiveRoi, double dCamResolution, int nRatio, cv::Point& ptStandard, wchar_t* strID, wchar_t* strAlgPath)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	int nImageSave_Flag = (int)dPara[E_PARA_LCP_BACK_TEXT];
	double nThreshold = (double)dPara[E_PARA_LCP_BACK_THRESHOLD];
	int nBack_MinArea = (int)dPara[E_PARA_LCP_BACK_BIGCIRCLE_MINAREA];
	int nBack_RoundIn = (int)dPara[E_PARA_LCP_BACK_ROUNDIN];

										// 버퍼가 없는 경우.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;


	cv::Mat matGrayBuf, matTresBuf, matDrawBuf_Temp, matTempBuf;

	// 버퍼 할당 & 초기화
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, true);


	// 컬러인 경우 ( SVI )
	if (matSrcBuf.channels() != 1)
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// 흑백인 경우 ( AVI, APP )
	else
	{
		if (matSrcBuf.type() == CV_8UC1)
			matSrcBuf.copyTo(matGrayBuf);
		else
			matSrcBuf.convertTo(matGrayBuf, CV_8UC1, 1. / 16.);
	}

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Src"));
		imwrite((cv::String)(CStringA)strSaveName, matSrcBuf);
	}


	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("cvtColor."));

	//////////////////////////////////////////////////////////////////////////
	cv::Rect rtMask = cv::Rect(100, 1800, matGrayBuf.cols-(100+100), matGrayBuf.rows-(1800+100));
	cv::Mat matEdgeMask = cv::Mat::zeros(matGrayBuf.size(), CV_8UC1);
	matEdgeMask(rtMask).setTo(255.0);

	

	cv::bitwise_not(matEdgeMask, matEdgeMask);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("EdgeMask"));
		imwrite((cv::String)(CStringA)strSaveName, matEdgeMask);
	}

	matGrayBuf -= matEdgeMask;

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Offset Cut."));
	//////////////////////////////////////////////////////////////////////////
	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("EdgeMask And"));
		imwrite((cv::String)(CStringA)strSaveName, matGrayBuf);
	}


	cv::threshold(matGrayBuf, matTresBuf, nThreshold, 255.0, THRESH_BINARY);
	
	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Threshold"));
		imwrite((cv::String)(CStringA)strSaveName, matTresBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Threshold."));

	vector<vector<Point>> contours; // 큰 타원 찾기
	vector<Vec4i> hierarchy;
	cv::Rect rcBackStage;

	double dArea = 0.0;
	bool bErrorJudge = true;

	findContours(matTresBuf, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//////////////////////////////////////////////////////////////////////////
	if (contours.size() < 0) return E_ERROR_CODE_LCP_ALIGN_BACK_MASK_FAIL;
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < contours.size(); i++) {
		dArea = contourArea(contours[i]);
		if (dArea > nBack_MinArea) // 표본 3개 기준 전부 약 112만
		{
			//rcBackStage = boundingRect(contours[i]);

			rectangle(matTempBuf, cv::boundingRect(contours[i]), Scalar(255),-1);
			bErrorJudge = false;

			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if (bErrorJudge) return E_ERROR_CODE_LCP_ALIGN_BACK_MASK_FAIL;
	//////////////////////////////////////////////////////////////////////////

	Mask_SizeDown(matTempBuf, nBack_RoundIn);


	findContours(matTempBuf, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//////////////////////////////////////////////////////////////////////////
	if (contours.size() < 0) return E_ERROR_CODE_LCP_ALIGN_BACK_MASK_FAIL;
	//////////////////////////////////////////////////////////////////////////

	bErrorJudge = true;

	for (int i = 0; i < contours.size(); i++) {
		dArea = contourArea(contours[i]);
		if (dArea > nBack_MinArea) // 표본 3개 기준 전부 약 112만
		{
			rcBackStage = boundingRect(contours[i]);
			bErrorJudge = false;

			break;
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	if (bErrorJudge) return E_ERROR_CODE_LCP_ALIGN_BACK_MASK_FAIL;
	//////////////////////////////////////////////////////////////////////////

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Rect"));
		imwrite((cv::String)(CStringA)strSaveName, matTempBuf);
	}
	
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("BackStage Rect."));

	matTempBuf.copyTo(matBackStageBuf);

	rcActiveRoi = rcBackStage;
	ptStandard.x = rcBackStage.x;
	ptStandard.y = rcBackStage.y;

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::DoFindActive_LCP_Vaccum(cv::Mat& matSrcBuf, cv::Mat& matVaccumHoleBuf, double* dPara, int nAlignImageClassify, int nCameraNum, int nImageClassify, cv::Rect2i& rcActiveRoi, double dCamResolution, int nRatio, cv::Point& ptStandard, wchar_t* strID, wchar_t* strAlgPath)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	int nImageSave_Flag = (int)dPara[E_PARA_LCP_MASK_VACUUM_TEXT];
	double nThreshold = (double)dPara[E_PARA_LCP_MASK_VACUUM_IMAGE_TRESHOLD];
	int nMin_Area = (int)dPara[E_PARA_LCP_MASK_VACUUM_IMAGE_MIN_AREA];
	int nVac_RoundOut = (int)dPara[E_PARA_LCP_MASK_VACUUM_IMAGE_ROUNDOUT];

	//const double um = dCamResolution; // 30um -> 안나와서 일단 300um으로
	const double um = dCamResolution;
	// 버퍼가 없는 경우.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;


	cv::Mat matGrayBuf, matTresBuf, matDrawBuf_Temp, matTempBuf;

	// 버퍼 할당 & 초기화
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, true);
	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, true);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, true);


	// 컬러인 경우 ( SVI )
	if (matSrcBuf.channels() != 1)
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// 흑백인 경우 ( AVI, APP )
	else
	{
		if (matSrcBuf.type() == CV_8UC1)
			matSrcBuf.copyTo(matGrayBuf);
		else
			matSrcBuf.convertTo(matGrayBuf, CV_8UC1, 1. / 16.);
	}

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Src"));
		imwrite((cv::String)(CStringA)strSaveName, matSrcBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("cvtColor."));

	cv::threshold(matGrayBuf, matTresBuf, nThreshold, 255.0, THRESH_BINARY);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Threshold"));
		imwrite((cv::String)(CStringA)strSaveName, matTresBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Threshold."));

	vector<vector<cv::Point> > vContours_RC; // Vector for storing contour
	vector<cv::Vec4i> vHierarchy_RC;
	cv::RotatedRect rcRotate = cv::RotatedRect();
	vector<vector<cv::Point> > vContours_Big;

	double dLargest_Area_RC = 0;
	int nLargest_Contour_Index_RC = 0;

	findContours(matTresBuf, vContours_RC, vHierarchy_RC, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image

	for (int i = 0; i < (int)vContours_RC.size(); i++)
	{
		// iterate through each contour. 
		double dArea_RC = contourArea(vContours_RC[i], false);		// Find the area of contour

		if (dArea_RC > nMin_Area) // 가장 큰놈
		{
			vContours_Big.push_back(vContours_RC[i]);
		}
	}

	for (int i = 0; i < vContours_Big.size(); i++) {
		drawContours(matTempBuf, vContours_Big, i, Scalar(255), -1, 8);
	}

	cv::bitwise_not(matTempBuf, matTempBuf);

	Mask_SizeUp(matTempBuf, nVac_RoundOut);

	matTempBuf.copyTo(matVaccumHoleBuf);
	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Vaccum Result"));
		imwrite((cv::String)(CStringA)strSaveName, matTempBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

void CInspectAlign::Mask_SizeDown(cv::Mat matSrc, int nUpSize)
{
	cv::Mat Temp16;

	cv::distanceTransform(matSrc, Temp16, CV_DIST_L2, 3);

	cv::threshold(Temp16, Temp16, nUpSize, 255, CV_THRESH_BINARY);
	Temp16.convertTo(matSrc, CV_8UC1);
}

void CInspectAlign::Mask_SizeUp(cv::Mat matSrc, int nDownSize)
{
	cv::Mat matTemp;
	cv::Mat Temp16;

	cv::bitwise_not(matSrc, matTemp);
	cv::distanceTransform(matTemp, Temp16, CV_DIST_L2, 3);

	cv::threshold(Temp16, Temp16, nDownSize, 255, CV_THRESH_BINARY);
	Temp16.convertTo(matTemp, CV_8UC1);
	cv::bitwise_not(matTemp, matSrc);
}

void CInspectAlign::MakeLabelMask(cv::Mat& matSrc, cv::Mat& matDstActive)
{
	// 두번째로 큰 컨투어 잡아내기 - 바코드 영역 (선제 조건: 바코드는 항상 상부나 하부 둘중 하나의 포지션에서만 나타나야함)

	if (matDstActive.empty())
		matDstActive = Mat::zeros(matSrc.size(), matSrc.type());

	cv::Mat matTemp = Mat::zeros(matSrc.size(), matSrc.type());

	double dLargest_Area2 = 0;
	int nLargest_Contour_Index2 = 0;

	double dSecondLargest_Area2 = 0;
	int nSecondLargest_Contour_Index2 = 0;

	vector<vector<cv::Point> > vContours2; // Vector for storing contour
	vector<cv::Vec4i> vHierarchy2;

	findContours(matSrc, vContours2, vHierarchy2, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
																						  //	findContours(matSrc, vContours, vHierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	cv::Rect rtArea2 = cv::Rect(0, 0, 0, 0);
	for (int i = 0; i < (int)vContours2.size(); i++)
	{
		// iterate through each contour. 
		double dArea2 = contourArea(vContours2[i], false);		// Find the area of contour

		if (dArea2 > dLargest_Area2) // 가장 큰놈
		{
			dLargest_Area2 = (int)dArea2;
// 			nSecondLargest_Contour_Index2 = nLargest_Contour_Index2;
 			nLargest_Contour_Index2 = i;							// Store the index of largest contour
// 
 			rtArea2 = boundingRect(vContours2[nLargest_Contour_Index2]);				// Find the bounding rectangle for biggest contour
		}


	}

	drawContours(matDstActive, vContours2, nLargest_Contour_Index2, Scalar(255), 1, 8, vHierarchy2);
	matDstActive.copyTo(matTemp);

	//cv::floodFill(matDstActive, cv::Point(rtArea2.x + (rtArea2.width / 2), rtArea2.y + (rtArea2.height / 2)), cv::Scalar(255));
	cv::floodFill(matDstActive(rtArea2), cv::Point(matDstActive(rtArea2).cols/2, matDstActive(rtArea2).rows / 2), cv::Scalar(255));

	cv::subtract(matDstActive,matTemp,matDstActive);

	////////////////////////////////////////////////////////////////////////// 21.11.08 choi
	vector<vector<cv::Point> > vContours_RC; // Vector for storing contour
	vector<cv::Vec4i> vHierarchy_RC;
	cv::RotatedRect rcRotate = cv::RotatedRect();

	double dLargest_Area_RC = 0;
	int nLargest_Contour_Index_RC = 0;

	for (int i = 0; i < (int)vContours_RC.size(); i++)
	{
		// iterate through each contour. 
		double dArea_RC = contourArea(vContours_RC[i], false);		// Find the area of contour

		if (dArea_RC > dLargest_Area_RC) // 가장 큰놈
		{
			nLargest_Contour_Index_RC = i;
		}
	}

	findContours(matDstActive, vContours_RC, vHierarchy_RC, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
	rcRotate = minAreaRect(vContours_RC[nLargest_Contour_Index_RC]);
	
	cv::Point2f vertices2f[4];
	rcRotate.points(vertices2f);

	// Convert them so we can use them in a fillConvexPoly
	cv::Point vertices[4];
	for (int i = 0; i < 4; ++i) {
		vertices[i] = vertices2f[i];
	}

	cv::fillConvexPoly(matDstActive,vertices,4,Scalar(255));
	//////////////////////////////////////////////////////////////////////////

//	cv::bitwise_not(matDstActive, matDstActive);

//	MaximumFilter(matDstActive, matDstActive, 11);
}



void CInspectAlign::MakeActiveMask(cv::Mat& matSrc, cv::Mat& matDstActive, int nThreshActive)
{

	if (matDstActive.empty())
		matDstActive = cv::Mat::zeros(matSrc.size(), matSrc.type());

	// 가장 큰 영역(Active 영역을 찾기)
	double dLargest_Area = 0;
	int nLargest_Contour_Index = 0;

	vector<vector<cv::Point> > vContours; // Vector for storing contour
	vector<cv::Vec4i> vHierarchy;

	findContours(matSrc, vContours, vHierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image

	cv::Rect rtArea = cv::Rect(0, 0, 0, 0);
	for (int i = 0; i < (int)vContours.size(); i++)
	{
		// iterate through each contour. 
		double dArea = contourArea(vContours[i], false);		// Find the area of contour
		if (dArea > dLargest_Area)
		{
			dLargest_Area = (int)dArea;
			nLargest_Contour_Index = i;							// Store the index of largest contour

			rtArea = boundingRect(vContours[i]);				// Find the bounding rectangle for biggest contour
		}
	}

	drawContours(matDstActive, vContours, nLargest_Contour_Index, Scalar(255), CV_FILLED, 8, vHierarchy); // Draw the largest contour using previously stored index.
}

void CInspectAlign::VarianceFilter(cv::Mat src, cv::Mat& dst, int nMaskSize)
{
	Mat prc;
	src.copyTo(prc);

	if (nMaskSize % 2 == 0)
		nMaskSize++;

	int nCols = src.cols;
	int nRows = src.rows;
	int nStep = src.step;

	//	 int nMaskX_ST, nMaskY_ST, nMask_ST;
	int nMaskSizeX, nMaskSizeY;

	//Mask Size의 경우 레시피로 뺄 예정
	nMaskSizeX = nMaskSize;
	nMaskSizeY = nMaskSize;

	//마스크 
	cv::Mat matFilterMask = cv::Mat::zeros(nMaskSizeX, nMaskSizeY, CV_8UC1);
	nMaskSize = nMaskSize / 2 * -1;

	//영상 픽셀 포인터 접근
	//입력 Image
	uchar* ucSrcdata;
	ucSrcdata = prc.data;

	//결과 Image
	uchar* ucDstdata;
	ucDstdata = dst.data;

	//Mask Image
	uchar* ucFilterMask;
	ucFilterMask = matFilterMask.data;

	//전체 영상 세로 크기
	for (int nY = 0; nY < nRows; nY++)
	{
		//최 외각 영역에 대한 오류 제거(영상 처리 영역은 기본적으로 영상 중앙에 위치하므로 예외 영역에 대한 처리 필요 없음
		if (nY + nMaskSize < 0 || nY + abs(nMaskSize) > nRows - 1)
			continue;

		//전체 영상 가로 크기
		for (int nX = 0; nX < nCols; nX++)
		{

			//최 외각 영역에 대한 오류 제거(영상 처리 영역은 기본적으로 영상 중앙에 위치하므로 예외 영역에 대한 처리 필요 없음
			if (nX + nMaskSize < 0 || nX + nMaskSize > nCols - 1)
				continue;

			//Mask y Size
			for (int nFy = 0; nFy < nMaskSizeY; nFy++)
			{
				//Mask x Size
				for (int nFx = 0; nFx < nMaskSizeX; nFx++)
				{
					//사각형 타입
					ucFilterMask[nFy * nMaskSizeX + nFx] = ucSrcdata[((nY - (nMaskSizeY / 2) + nFy) * nCols) + ((nX - (nMaskSizeX / 2)) + nFx)];
					//막대형 (가로 한번 세로 한번 더하기)
				}//Mask y Size
			}//Mask x Size

			 // 필터 마스크 전체 이미지의 SDV 구하기 
			Scalar m, s;
			double dVariance = 0;
			cv::meanStdDev(matFilterMask, m, s);
			dVariance = (double)s[0] * (double)s[0];

			if (dVariance < 0.0f)   ucDstdata[nY * nCols + nX] = (BYTE)0;
			else if (dVariance > 255.0f) ucDstdata[nY * nCols + nX] = (BYTE)255;
			else						ucDstdata[nY * nCols + nX] = (BYTE)dVariance;

		}//전체 영상 가로 크기 종료
	}//전체 영상 세로 크기 종료
}

long CInspectAlign::DoFindActive_LCP_AutoCal_Mask(cv::Mat matBackBuf, cv::Mat matEmptyBuf, cv::Mat matSide, CString Path_Result,CString ColorCode, CString T_Judge_Code ,double* dPara)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	//////////////////////////////////////////////////////////////////////////
	//Param은 Gui에서 받아와야함
	//Active 용
// 	int nImageSave_Flag		= (int)dPara[E_PARA_LCP_ALIGN_LIMPI_TEXT];
// 	int nEdgeTH				= (int)dPara[E_PARA_LCP_ALIGN_LIMPI_EDGE_TH]; //120
// 	int nCircle_MinArea		= (int)dPara[E_PARA_LCP_ALIGN_LIMPI_BIGCIRCLE_MINAREA]; //10000000
// 
// 	int nActive_MaskIn		= (int)dPara[E_PARA_LCP_ALIGN_LIMPI_ACTIVEMASK_IN]; //150
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	//Label 용
// 	int nThreshLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_TH]; //NP: 5 , 투명: 70
// 	int nGauSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_GAU_SZ]; //3
// 	double dGauSigLabel = (double)dPara[E_PARA_LCP_MASK_LABEL_GAU_SIG]; //3.0
// 	int nMaxSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MAX]; //5
// 	int nMinSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MIN]; //5
// 	int nRound_In = (int)dPara[E_PARA_LCP_MASK_LABEL_ROUND_IN]; //20
// 	int nRound_Out = (int)dPara[E_PARA_LCP_MASK_LABEL_ROUND_OUT]; //20
// 	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Param은 Gui에서 받아와야함
	//Active 용
	int nImageSave_Flag = 1;
	int nEdgeTH = 200; //120
	int nCircle_MinArea = 10000000; //10000000

	int nActive_MaskIn = 150; //150

																		   //////////////////////////////////////////////////////////////////////////
																		   //Label 용
	int nThreshLabel = 70; //NP: 5 , 투명: 70
	int nGauSzLabel = 3; //3
	double dGauSigLabel = 3.0; //3.0
	int nMaxSzLabel = 5; //5
	int nMinSzLabel = 5; //5
	int nRound_In = 20; //20
	int nRound_Out = 20; //20
																  //////////////////////////////////////////////////////////////////////////

	if (matBackBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matEmptyBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matSide.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat matGrayBuf, matTresBuf, matTempBuf , matMask_Active ,matDstLabel;

	
	// 버퍼 할당 & 초기화
	//////////////////////////////////////////////////////////////////////////
	//Active Mask 용
	matGrayBuf = cMem->GetMat(matBackBuf.size(), CV_8UC1, false);
	matTresBuf = cMem->GetMat(matBackBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matBackBuf.size(), CV_8UC1, false);
	matMask_Active = cMem->GetMat(matBackBuf.size(), CV_8UC1, false);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Label Mask용

	//////////////////////////////////////////////////////////////////////////
	matDstLabel = cMem->GetMat(matBackBuf.size(), CV_8UC1, false);

	//////////////////////////////////////////////////////////////////////////
	//Active Mask Find Start
	matBackBuf.copyTo(matGrayBuf);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%s.bmp"), Path_Result,  _T("Src"));
		imwrite((cv::String)(CStringA)strSaveName, matBackBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("cvtColor."));

	cv::subtract(matEmptyBuf, matGrayBuf, matGrayBuf);

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%s.bmp"), Path_Result, _T("Stage sub"));
		imwrite((cv::String)(CStringA)strSaveName, matGrayBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Stage sub."));

	///////////////////////////////////
	vector<Point> vPoint_Circle;
	cv::Mat matTemp = cv::Mat::zeros(matGrayBuf.size(), matGrayBuf.type());

	for (int nMode = 0; nMode < 2; nMode++) {

		uchar nPre_Gv = 0;

		switch (nMode)
		{
		case 0: {
			for (int nx = 0; nx < matGrayBuf.cols; nx++) {
				for (int ny = 0; ny < matGrayBuf.rows; ny++) {
					if (ny == 0) {
						nPre_Gv = matGrayBuf.at<uchar>(ny, nx);
					}
					else {
						int nSub = abs(matGrayBuf.at<uchar>(ny, nx) - nPre_Gv);
						if (nSub > nEdgeTH) {
							vPoint_Circle.push_back(cv::Point(nx, ny));
							break;
						}
					}
				}
			}


			break;
		}

		case 1: {
			for (int nx = 0; nx < matGrayBuf.cols; nx++) {
				for (int ny = matGrayBuf.rows - 1; ny > 0; ny--) {
					if (ny == matGrayBuf.rows - 1) {
						nPre_Gv = matGrayBuf.at<uchar>(ny, nx);
					}
					else {
						int nSub = abs(matGrayBuf.at<uchar>(ny, nx) - nPre_Gv);
						if (nSub > nEdgeTH) {
							vPoint_Circle.push_back(cv::Point(nx, ny));
							break;
						}
					}
				}
			}

			break;
		}

		default:
			break;
		}
	}

	if (vPoint_Circle.size() == 0) // Edge 못찾으면 에러
	{
		return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
	}

	RotatedRect e2;
	e2 = cv::fitEllipse(vPoint_Circle);

	cv::ellipse(matTemp, e2, Scalar(255), -1); // 그리기 체크 (타원 테두리)

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%s.bmp"), Path_Result,  _T("Draw Circle"));
		imwrite((cv::String)(CStringA)strSaveName, matTemp);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Draw Circle."));

	Mat M(2, 3, CV_32FC1); // warpAffine 연산에 사용되는 2*3 행렬
	vector<Vec3f> circles; // 작은 원 찾기 (안되면 타원 찾기 or PerspectiveTransform)
	vector<vector<Point>> contours; // 큰 타원 찾기
	vector<Vec4i> hierarchy;

	RotatedRect e; // 큰 타원 정보
	double dArea;

	matTemp.copyTo(matTresBuf);

	findContours(matTresBuf, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


	for (int i = 0; i < contours.size(); i++) {
		dArea = contourArea(contours[i]);
		if (dArea > nCircle_MinArea) // 표본 3개 기준 전부 약 112만
		{
			e = fitEllipse(contours[i]);

			drawContours(matTresBuf, contours, i, Scalar(255), -1);

			break;
		}
	}

	matTresBuf.copyTo(matMask_Active);
	Mask_SizeDown(matMask_Active, nActive_MaskIn);

	CString strResult_Active;
	strResult_Active.Format(_T("%s\\%s.bmp"), Path_Result, _T("Active_Mask"));
	imwrite((cv::String)(CStringA)strResult_Active, matMask_Active);
	
	//Active Mask End
	////////////////////////////////////////////////////////////////////////// 


	//////////////////////////////////////////////////////////////////////////
	//Label Mask Find Start
	if (ColorCode.MakeUpper() == "NP" || T_Judge_Code.MakeUpper() == "T") {
		
		cv::Mat matGauss;

		matSide.copyTo(matGrayBuf);
		cv::GaussianBlur(matGrayBuf, matGauss, Size(nGauSzLabel, nGauSzLabel), dGauSigLabel);

		if (nImageSave_Flag)
		{
			CString strSaveName;
			strSaveName.Format(_T("%s\\%s.bmp"), Path_Result, _T("1_MakeLabel_Gau"));
			imwrite((cv::String)(CStringA)strSaveName, matGauss);
		}

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("MakeLabel_Gau."));

		cv::Mat Maximum = Mat::zeros(matGauss.size(), matGauss.type());
		cv::Mat Minimum = Mat::zeros(matGauss.size(), matGauss.type());;

		MinimumFilter(matGauss, Minimum, nMinSzLabel);

		MaximumFilter(matGauss, Maximum, nMaxSzLabel);

		cv::subtract(Maximum, Minimum, Maximum);

		if (nImageSave_Flag)
		{
			CString strSaveName;
			strSaveName.Format(_T("%s\\%s.bmp"), Path_Result, _T("1_MakeLabel_MaxMin_Sub"));
			imwrite((cv::String)(CStringA)strSaveName, Maximum);
		}

		cv::threshold(Maximum, Maximum, nThreshLabel, 255, THRESH_BINARY);

		if (nImageSave_Flag)
		{
			CString strSaveName;
			strSaveName.Format(_T("%s\\%s.bmp"), Path_Result, _T("1_MakeLabel_Threshold"));
			imwrite((cv::String)(CStringA)strSaveName, Maximum);
		}

		cv::Mat matActiveMask_Temp;
		matMask_Active.copyTo(matActiveMask_Temp);
		Mask_SizeDown(matActiveMask_Temp, nRound_In);

		cv::bitwise_and(Maximum, matActiveMask_Temp, Maximum);

		if (nImageSave_Flag)
		{
			CString strSaveName;
			strSaveName.Format(_T("%s\\%s.bmp"), Path_Result, _T("1_MakeLabel_Threshold_Sub"));
			imwrite((cv::String)(CStringA)strSaveName, Maximum);
		}

		// Lable Mask 만들기
		MakeLabelMask(Maximum, matDstLabel);

		Mask_SizeUp(matDstLabel, nRound_Out);
	
		CString strResult_Label;
		strResult_Active.Format(_T("%s\\%s.bmp"), Path_Result, _T("Label_Mask"));
		imwrite((cv::String)(CStringA)strResult_Active, matDstLabel);
	}
	else {
		cv::Mat matDstLabel = cv::Mat::zeros(matBackBuf.size(), CV_8U); // 투명 np 아닐땐 0으로 채움

		CString strResult_Label;
		strResult_Active.Format(_T("%s\\%s.bmp"), Path_Result, _T("Label_Mask"));
		imwrite((cv::String)(CStringA)strResult_Active, matDstLabel);
	}
	//Label Mask End
	////////////////////////////////////////////////////////////////////////// 

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));


	return E_ERROR_CODE_TRUE;
}


long CInspectAlign::DoFindActive_LCP_AutoCal(cv::Mat matSrcBuf, cv::Mat matActiveMask, cv::Mat matLabelMask, CString PattenName,CString ColorCode, CString T_Judge_Code, CString Path_Result, int nMinGV, int nMaxGV, double& MeanGV, bool& Judge_OK_NG, double* dPara)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;



	cv::Mat matSrcTemp;
	cv::Mat matAcitveMaskTemp;
	cv::Mat matLabelMaskTemp;

	matSrcBuf.copyTo(matSrcTemp);
	matActiveMask.copyTo(matAcitveMaskTemp);
	matLabelMask.copyTo(matLabelMaskTemp);

	if (ColorCode.MakeUpper() == "NP" || T_Judge_Code.MakeUpper() == "T") {
		matSrcTemp -= matLabelMaskTemp;
	}

	cv::Scalar m, s;
	cv::meanStdDev(matSrcTemp, m, s, matAcitveMaskTemp);

	MeanGV = m[0];


	if (MeanGV >= nMinGV && MeanGV <= nMaxGV) {
		Judge_OK_NG = true;

		CString strname;
		strname.Format(_T("%s\\%s_%f.bmp"),Path_Result,PattenName,MeanGV);
		cv::imwrite((cv::String)(CStringA)strname, matSrcTemp);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));


	return E_ERROR_CODE_TRUE;
}