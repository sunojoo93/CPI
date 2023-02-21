
/************************************************************************/
// Align ���� ���� �ҽ�
// ������ : 18.02.07 - KSW
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


// Top Line ���� ã��
long CInspectAlign::DoFindTheta(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point& ptCenter, wchar_t* strID)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	cv::Mat matGrayBuf, matTempBuf;

	// ���۰� ���� ���.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// ���� �Ҵ� & �ʱ�ȭ
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	// �÷��� ��� ( SVI )
	if( matSrcBuf.channels() != 1 )
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// ����� ��� ( AVI, APP )
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
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples		= 3;	// ����
	double	distThreshold	= 10;	// ����

	int		nThreshold		= (int)dPara[E_PARA_ALIGN_THRESHOLD];
	int		nMorp			= (int)dPara[E_PARA_ALIGN_MORP];
	double	dAngleError		= dPara[E_PARA_ALIGN_ANGLE_ERR];
	double	dAngleWarning	= dPara[E_PARA_ALIGN_ANGLE_WAR];

	// �ܰ� : Cell ���� ( Pixel ���� )
	// ���� : ���� ���� ( Pixel ���� )
	int		nMinArea		= (int)(dPara[E_PARA_CELL_SIZE_X] * dPara[E_PARA_CELL_SIZE_Y]);

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	long	nWidth	= (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuf.rows;	// ���� ���� ũ��

	if( nMorp > 0 )
	{
		cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, Size(nMorp, nMorp), cv::Point(nMorp/2, nMorp/2));

		// Morphology Close ( Cell ���� ����� ä��� )
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

	// �˻� ���� Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matTempBuf, nMinArea, rectCell);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("FindCellArea."));

	// Cell �߽���
	ptCenter.x = rectCell.x + rectCell.width/2;
	ptCenter.y = rectCell.y + rectCell.height/2;

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		matGrayBuf.release();
		matTempBuf.release();
		return nErrorCode;
	}

	long double	dValueA = 0, dValueB = 0;

	// Top ���� ã��
	nErrorCode = RobustFitLine(matTempBuf, rectCell, dValueA, dValueB, nMinSamples, distThreshold, E_ALIGN_TYPE_TOP);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("FitLine."));

	// �޸� ����
	matGrayBuf.release();
	matTempBuf.release();

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	// Top Line(�庯)���� ���� ���ϱ�
	dTheta = atan( dValueA ) * 180. / PI;

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// AVI �˻� ���� ã��
long CInspectAlign::DoFindActive(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point* ptResCorner, int nRatio, cv::Point& ptCenter, wchar_t* strID)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// ���۰� ���� ���.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat matGrayBuf, matTempBuf;

	// ���� �Ҵ� & �ʱ�ȭ
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	// �÷��� ��� ( SVI )
	if( matSrcBuf.channels() != 1 )
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// ����� ��� ( AVI, APP )
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
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples		= 3;	// ����
	double	distThreshold	= 10;	// ����

	int		nThreshold		= dPara[E_PARA_ALIGN_THRESHOLD];
	int		nMorp			= dPara[E_PARA_ALIGN_MORP];
	double	dAngleError		= dPara[E_PARA_ALIGN_ANGLE_ERR];
	double	dAngleWarning	= dPara[E_PARA_ALIGN_ANGLE_WAR];

	// �ܰ� : Cell ���� ( Pixel ���� )
	// ���� : ���� ���� ( Pixel ���� )
	int		nMinArea		= (int)(dPara[E_PARA_CELL_SIZE_X] * dPara[E_PARA_CELL_SIZE_Y] * nRatio * nRatio);	// 3800 * 1900;	// APP

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	long	nWidth	= (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuf.rows;	// ���� ���� ũ��

	//////////////////////////////////////////////////////////////////////////
	// AVI�� ��츸 ���, Cell �迭 ���� ����� ä��� �뵵
	//////////////////////////////////////////////////////////////////////////

	if( nMorp > 0 )
	{
		cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, Size(nMorp, nMorp), cv::Point(nMorp/2, nMorp/2));

		// Morphology Close ( Cell ���� ����� ä��� )
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
	// Cell ���� ã��
	//////////////////////////////////////////////////////////////////////////

	// �˻� ���� Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matGrayBuf, nMinArea, rectCell);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("FindCellArea."));

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		matGrayBuf.release();
		matTempBuf.release();
		return nErrorCode;
	}

	// Cell �߽���
	ptCenter.x = rectCell.x + rectCell.width/2;
	ptCenter.y = rectCell.y + rectCell.height/2;

	//////////////////////////////////////////////////////////////////////////
	// ���� ���� ã��
	//////////////////////////////////////////////////////////////////////////

	// Contours ���ϱ�
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

	// ������ ��� Ȯ��
	//cv::Mat matTempBufff = matSrcBuf.clone();
	//cv::line(matTempBufff, vertices[0], vertices[1], CvScalar(255, 255, 255));
	//cv::line(matTempBufff, vertices[1], vertices[2], CvScalar(255, 255, 255));
	//cv::line(matTempBufff, vertices[2], vertices[3], CvScalar(255, 255, 255));
	//cv::line(matTempBufff, vertices[3], vertices[0], CvScalar(255, 255, 255));

	vector<vector<cv::Point>>().swap(contours);

	// �޸� ����
	matGrayBuf.release();
	matTempBuf.release();

	//////////////////////////////////////////////////////////////////////////
	// �ڳ� ������ 4�� ã��
	//////////////////////////////////////////////////////////////////////////

	nErrorCode = FindCornerPoint(vertices, ptResCorner, nWidth, nHeight);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Find 4-Corner."));

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;


	// Top Line(�庯)���� ���� ���ϱ�
	dTheta = BoundingBox.angle;

	// ����ó��
	if( 45.0 < dTheta && dTheta < 135.0 )	dTheta -= 90.0;
	if( -45.0 > dTheta && dTheta > -135.0 )	dTheta += 90.0;

	// ������ ���� Ʋ���� ��� ����. ( �߾˶� )
	if( abs(dTheta) > dAngleError )
		return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;

	// ������ ���� Ʋ���� ��� ����. ( ��˶� )
	else if( abs(dTheta) > dAngleWarning )
		return E_ERROR_CODE_ALIGN_ANGLE_RANGE_WARNING;
	
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// APP �˻� ���� ã��
long CInspectAlign::DoFindActive_APP(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point* ptResCorner, int nRatio, double dCamResolution, double dPannelSizeX, double dPannelSizeY, cv::Point& ptCenter, wchar_t* strID)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	cv::Mat matGrayBuf, matTempBuf;

	// ���۰� ���� ���.
	if (matSrcBuf.empty())
	{
		return E_ERROR_CODE_EMPTY_BUFFER;
	}

	// ���� �Ҵ� & �ʱ�ȭ
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1);

	matSrcBuf.copyTo(matGrayBuf);
	matGrayBuf.copyTo(matTempBuf);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("cvtColor."));

	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////
	int		nThreshold		= (int)dPara[E_PARA_APP_ALIGN_THRESHOLD];
	int		nDilate			= (int)dPara[E_PARA_APP_ALIGN_DILATE];
	int		nPanelEdgeTheta = (int)dPara[E_PARA_APP_ALIGN_PANEL_EDGE_THETA];
	int		nMinSamples		= 3;			// ����
	double	distThreshold	= 5;			// ����
	double	dAngleError		= (double)dPara[E_PARA_APP_ALIGN_ANGLE_ERR];
	double	dAngleWarning	= (double)dPara[E_PARA_APP_ALIGN_ANGLE_WAR];
	double	fAvgOffset		= (double)dPara[E_PARA_APP_ALIGN_AVG_OFFSET];
	bool	bUseOverRange	= (bool)dPara[E_PARA_APP_ALIGN_USE_OVERRANGE];
	int		dLengthError	= (int)dPara[E_PARA_APP_ALIGN_RANGE_OVER];

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;
	long nWidth = (long)matSrcBuf.cols;	// ���� ���� ũ��
	long nHeight = (long)matSrcBuf.rows;	// ���� ���� ũ��

	cv::Rect rtCell = cv::Rect(0, 0, matGrayBuf.cols, matGrayBuf.rows);
	long double ldValueB[4] = { 0.0, };
	long double	ldValueA[4] = { 0.0, };

	try
	{
		//����ȭ �� �� ���� ū Blob�� ���� (Stage ��� Noise ����)
		//Morphlogy�� Cell ���� Noise ����, Cell ���� �ſ��, And�� ����� Mask Size ũ�� Ű���

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
		// Line ã�� & RANSAC
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

		// ������ ���, ���� �ڵ� ���
		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			return nErrorCode;
		}

		//////////////////////////////////////////////////////////////////////////
		// �ڳ� ������ 4�� ã��
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

		// Top Line(�庯)���� ���� ���ϱ�
		dTheta = atan(ldValueA[E_ALIGN_TYPE_TOP]) * 180. / PI;
		if (abs(dTheta) > dAngleError)							// ������ ���� Ʋ���� ��� ����.
		{
			return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
		}
		else if (abs(dTheta) > dAngleWarning)					// ������ ���� Ʋ���� ��� ����. ( ��˶� )
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

		// PAD Cutting �̻��ϰ� �� ��� ã��
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


// AVI �˻� ���� ã��
long CInspectAlign::DoFindActive_SVI(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point* ptResCorner, int nCameraNum, int nRatio, cv::Point& ptCenter, wchar_t* strID)
{
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// ���۰� ���� ���.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;


	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples		= 3;	// ����
	double	distThreshold	= 10;	// ����

	int		nThreshold		= dPara[E_PARA_SVI_ALIGN_THRESHOLD];

	// ���� : ���� ���� ( Pixel ���� )
	int		nMinArea		= (int)(dPara[E_PARA_SVI_CELL_COAX_SIZE_X] * dPara[E_PARA_SVI_CELL_COAX_SIZE_Y] * nRatio * nRatio);

	// Side ī�޶�
	if( nCameraNum == 1 )
		nMinArea		= (int)(dPara[E_PARA_SVI_CELL_SIDE_SIZE_X] * dPara[E_PARA_SVI_CELL_SIDE_SIZE_Y] * nRatio * nRatio);

	// �������� �˻�
	//int		nRoundIn		= (int)dPara[E_PARA_SVI_ROUND_IN];

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	long	nWidth	= (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuf.rows;	// ���� ���� ũ��

	//////////////////////////////////////////////////////////////////////////
	// Gray & ����ȭ
	//////////////////////////////////////////////////////////////////////////

	// Color -> Gray
	cv::Mat matGrayBuf;
	cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);

	// Threshold
	cv::Mat matTempBuf;
	cv::threshold(matGrayBuf, matTempBuf, nThreshold, 255.0, THRESH_BINARY);


	//////////////////////////////////////////////////////////////////////////
	// Cell ���� ã��
	//////////////////////////////////////////////////////////////////////////

	// �˻� ���� Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matTempBuf, nMinArea, rectCell);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		matGrayBuf.release();
		matTempBuf.release();
		return nErrorCode;
	}

	// Cell �߽���
	ptCenter.x = rectCell.x + rectCell.width/2;
	ptCenter.y = rectCell.y + rectCell.height/2;


	//////////////////////////////////////////////////////////////////////////
	// Convex Hull - ����, ��ġ �κ� �ſ�� ����
	//////////////////////////////////////////////////////////////////////////

	// Contours ���ϱ�
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
	// ���� ���� ������ ã�� ( Fit )
	//////////////////////////////////////////////////////////////////////////

	long double	dValueAA[E_ALIGN_TYPE_END], dValueBB[E_ALIGN_TYPE_END];

	// 4���� ���� ����
	for(int nType=E_ALIGN_TYPE_LEFT ; nType<=E_ALIGN_TYPE_BOTTOM ; nType++)
	{
		nErrorCode = RobustFitLine(matTempBuf, rectCell, dValueAA[nType], dValueBB[nType], nMinSamples, distThreshold, nType, 10);

		// ������ ���, ���� �ڵ� ���
		if( nErrorCode != E_ERROR_CODE_TRUE )
			return nErrorCode;
	}

	// �ڳ� ������ 4�� ã��
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

	// ���� ���ϱ�
	dTheta = atan( dValueAA[E_ALIGN_TYPE_TOP] ) * 180.0 / PI;

	// ����ó��
	if( 45.0 < dTheta && dTheta < 135.0 )	dTheta -= 90.0;
	if( -45.0 > dTheta && dTheta > -135.0 )	dTheta += 90.0;


	//////////////////////////////////////////////////////////////////////////
	// ���� �������� ��¦ ���...
	//////////////////////////////////////////////////////////////////////////

	//ptResCorner[E_CORNER_LEFT_TOP].x		+= nRoundIn;
	//ptResCorner[E_CORNER_LEFT_TOP].y		+= nRoundIn;
	//ptResCorner[E_CORNER_RIGHT_TOP].x		-= nRoundIn;
	//ptResCorner[E_CORNER_RIGHT_TOP].y		+= nRoundIn;
	//ptResCorner[E_CORNER_LEFT_BOTTOM].x	+= nRoundIn;
	//ptResCorner[E_CORNER_LEFT_BOTTOM].y	-= nRoundIn;
	//ptResCorner[E_CORNER_RIGHT_BOTTOM].x	-= nRoundIn;
	//ptResCorner[E_CORNER_RIGHT_BOTTOM].y	-= nRoundIn;


	// �޸� ����
	matGrayBuf.release();
	matTempBuf.release();


	//////////////////////////////////////////////////////////////////////////
	// ��� Ȯ��
	//////////////////////////////////////////////////////////////////////////

	if( FALSE )
	//if( TRUE )
	{
		cv::Mat matRectBuf;

		// �÷��� ���
		if( matSrcBuf.channels() != 1 )	matRectBuf = matSrcBuf.clone();

		// ����� ���
		else	cv::cvtColor(matSrcBuf, matRectBuf, COLOR_GRAY2RGB);

		for (int k=0 ; k<E_CORNER_END ; k++)
			cv::line(matRectBuf, ptResCorner[k], ptResCorner[ (k+1)%E_CORNER_END ], cv::Scalar(0, 255, 0), 1);		// Fit

// ���� ����̺� D:\ ���� �ƴ� - �ʿ� �� InspectLibLog �� GETDRV() Ȱ��
// 		CString strTemp;
// 		strTemp.Format(_T("D:\\Active.bmp"));
// 		cv::imwrite( (cv::String)(CStringA)strTemp, matRectBuf);
	}

	return E_ERROR_CODE_TRUE;
}


// �ܰ� Round & Camera Hole � ���� & ���� ����
long CInspectAlign::SetFindContour(cv::Mat matSrcBuf, INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRountROICnt, double* dPara, int nAlgImg, int nRatio, CString strPath)
{
	//  nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// ���� ����
	if( nAlgImg == E_IMAGE_CLASSIFY_AVI_BLACK )		return E_ERROR_CODE_TRUE;
	else if(nAlgImg == E_IMAGE_CLASSIFY_AVI_PCD)	return E_ERROR_CODE_TRUE;

	// KSW 17.11.14 - Dust ���� ����
	// Round�� ��� ��� �ö�� ��� ����
	// White ���� �ܰ��� �����Ͽ� ���
	if( nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST )		return E_ERROR_CODE_TRUE;

	cv::Mat matTempBuf;

	// ���۰� ���� ���.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples		= 3;	// ����
	double	distThreshold	= 10;	// ����

	int		nMinArea		= (int)(dPara[E_PARA_CELL_SIZE_X] * dPara[E_PARA_CELL_SIZE_Y] * nRatio * nRatio);
	int		nThreshold		= dPara[E_PARA_ALIGN_THRESHOLD];
	int		nMorp			= dPara[E_PARA_ALIGN_MORP];
	double	dAngleError		= dPara[E_PARA_ALIGN_ANGLE_ERR];
	double	dAngleWarning	= dPara[E_PARA_ALIGN_ANGLE_WAR];

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// AVI�� ��츸 ���, Cell �迭 ���� ����� ä��� �뵵
	//////////////////////////////////////////////////////////////////////////

	cv::Mat matSrc8bit = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);		//�޸� �Ҵ�

	if( matSrcBuf.type() == CV_8UC1 )
		matSrcBuf.copyTo(matSrc8bit);
	else
		matSrcBuf.convertTo(matSrc8bit, CV_8UC1, 1./16.);

	if( nMorp > 0 )
	{
		cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, Size(nMorp, nMorp), cv::Point(nMorp/2, nMorp/2));

		// Morphology Close ( Cell ���� ����� ä��� )
		cv::morphologyEx(matSrc8bit, matTempBuf, MORPH_CLOSE, StructElem);

		StructElem.release();
	}
	else
		matTempBuf = matSrc8bit.clone();		//�޸� �Ҵ�

	// Threshold
	cv::threshold(matTempBuf, matTempBuf, nThreshold, 255.0, THRESH_BINARY);


	//////////////////////////////////////////////////////////////////////////
	// Cell ���� ã��
	//////////////////////////////////////////////////////////////////////////

	// �˻� ���� Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matTempBuf, nMinArea, rectCell);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		matTempBuf.release();
		return nErrorCode;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���� ȸ��
	//////////////////////////////////////////////////////////////////////////

	long double	dValueA, dValueB;

	// Top ���� ã��
	nErrorCode = RobustFitLine(matTempBuf, rectCell, dValueA, dValueB, nMinSamples, distThreshold, E_ALIGN_TYPE_TOP);

	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = atan( dValueA ) * 180. / PI;
	DoRotateImage(matTempBuf, matTempBuf, dTheta);

	//cv::Mat matSaveBuf = matTempBuf.clone();

	//////////////////////////////////////////////////////////////////////////
	// ���� ���� ã��
	//////////////////////////////////////////////////////////////////////////	

	// ��ü �ܰ���
	vector< vector< cv::Point2i > > contours;
	cv::findContours(matTempBuf, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	cv::RotatedRect BoundingBox = cv::minAreaRect(contours[0]);

	cv::Point2f vertices[E_CORNER_END];
	BoundingBox.points(vertices);

	//////////////////////////////////////////////////////////////////////////
	// �ڳ� ������ 4�� ã��
	//////////////////////////////////////////////////////////////////////////

	cv::Point	ptCorner[E_CORNER_END];
	
	long	nWidth	= (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuf.rows;	// ���� ���� ũ��

	nErrorCode = FindCornerPoint(vertices, ptCorner, nWidth, nHeight);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;


	//////////////////////////////////////////////////////////////////////////
	// Contour
	//////////////////////////////////////////////////////////////////////////

	// Pixel ����
	int nOffsetL		= 0;

	// ���� ũ�� ����
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x,		ptCorner[E_CORNER_LEFT_BOTTOM].x	) - nOffsetL,
		min(ptCorner[E_CORNER_LEFT_TOP].y,		ptCorner[E_CORNER_RIGHT_TOP].y		) - nOffsetL,
		max(ptCorner[E_CORNER_RIGHT_TOP].x,		ptCorner[E_CORNER_RIGHT_BOTTOM].x	) + nOffsetL,
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	) + nOffsetL );
	

	// � ���� ���� ���� ���� �ʿ� ( ������ ���� )
	CRect	rectTemp[MAX_MEM_SIZE_E_INSPECT_AREA];
	bool	nInside[MAX_MEM_SIZE_E_INSPECT_AREA][E_CORNER_END];

	 for (int i=0 ; i<nRountROICnt ; i++)
	{
		// UI ������ ���� ��������
		rectTemp[i] = RoundROI[i].rectROI;

		// PS ��� ����
		rectTemp[i].left	*= nRatio;
		rectTemp[i].top		*= nRatio;
		rectTemp[i].right	*= nRatio;
		rectTemp[i].bottom	*= nRatio;

		// Left-Top ��ǥ�� �������� ��ǥ���� ������ ����
		// ���� ���� ������ �°� ����
		rectTemp[i].OffsetRect( CPoint(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_TOP].y) );

		// ����ó��
		if( rectTemp[i].left	< 0 )		rectTemp[i].left	= 0;
		if( rectTemp[i].top		< 0 )		rectTemp[i].top		= 0;
		if( rectTemp[i].right	< 0 )		rectTemp[i].right 	= 0;
		if( rectTemp[i].bottom	< 0 )		rectTemp[i].bottom	= 0;

		if( rectTemp[i].left	>= matTempBuf.cols )	rectTemp[i].left	= matTempBuf.cols - 1;
		if( rectTemp[i].top		>= matTempBuf.rows )	rectTemp[i].top		= matTempBuf.rows - 1;
		if( rectTemp[i].right	>= matTempBuf.cols )	rectTemp[i].right	= matTempBuf.cols - 1;
		if( rectTemp[i].bottom	>= matTempBuf.rows )	rectTemp[i].bottom	= matTempBuf.rows - 1;

		// �������� Cell ���� ���� �ȿ� �����ϴ��� Ȯ��
		nInside[i][E_CORNER_LEFT_TOP		] = (matTempBuf.at<uchar>(rectTemp[i].top		, rectTemp[i].left	) != 0 ) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_TOP		] = (matTempBuf.at<uchar>(rectTemp[i].top		, rectTemp[i].right	) != 0 ) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_BOTTOM	] = (matTempBuf.at<uchar>(rectTemp[i].bottom	, rectTemp[i].right	) != 0 ) ? 1 : 0;
		nInside[i][E_CORNER_LEFT_BOTTOM		] = (matTempBuf.at<uchar>(rectTemp[i].bottom	, rectTemp[i].left	) != 0 ) ? 1 : 0;
	}

	// ��� Ȯ�� ��
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
	
	// � �ܰ���
	vector< cv::Point2i > ptContours[MAX_MEM_SIZE_E_INSPECT_AREA];
	for (int i=0 ; i<MAX_MEM_SIZE_E_INSPECT_AREA ; i++)
		vector< cv::Point2i >().swap(ptContours[i]);

	// ������ ���� ��ŭ
	for (int i=0; i<contours.size() ; i++)
	{
		for (int j=0 ; j<contours[i].size() ; j++)
		{
			// UI ������ ���� ������ŭ
			for (int k=0 ; k<nRountROICnt ; k++)
			{
				// ��� ���ϴ� �� ����
				if( !RoundROI[k].bUseROI )	continue;

				// ������ ���� �ȿ�, �ش� ��ǥ�� �ִ� ���
				if( rectTemp[k].PtInRect( CPoint(contours[i][j].x, contours[i][j].y) ) )
				{
					// ��ǥ �߰�
					ptContours[k].push_back( cv::Point2i(contours[i][j]) );
					break;
				}
			}
		}
		// �ʱ�ȭ
		vector< cv::Point2i >().swap(contours[i]);
	}

	//////////////////////////////////////////////////////////////////////////
	// ���� ����
	// �ٰ��� & ���� ����ä�ﶧ, ������� �ʿ� ��...
	//////////////////////////////////////////////////////////////////////////

	cv::Point2i ptTempS;
	for (int i=0 ; i<MAX_MEM_SIZE_E_INSPECT_AREA ; i++)
	{		
		for (int j=0 ; j<ptContours[i].size() ; j++ )
		{
			for (int m=j+1 ; m<ptContours[i].size() ; m++)
			{
				// y��ǥ�� ������ ���� ������ ����
				if( ptContours[i][j].y > ptContours[i][m].y )
				{
					ptTempS				= ptContours[i][j];
					ptContours[i][j]	= ptContours[i][m];
					ptContours[i][m]	= ptTempS;
				}
				// y��ǥ�� ���� ���
				else if( ptContours[i][j].y == ptContours[i][m].y )
				{
					// ���������� �հŸ��� ��� ����
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

	// ���� ���� ���
	CString str;
	str.Format(_T("%s\\CornerEdge"), strPath);
	CreateDirectory(str, NULL);

	for (int i=0; i<MAX_MEM_SIZE_E_INSPECT_AREA ; i++)
	{
		// ���� ���� ��� ����
		if ( ptContours[i].size() <= 0 )	continue;

		//////////////////////////////////////////////////////////////////////////
		// ���� ������, ����� Cell ���������� ���� ����� ������ ã��
		//////////////////////////////////////////////////////////////////////////

		int nCx = (rectTemp[i].left + rectTemp[i].right ) / 2;
		int nCy = (rectTemp[i].top  + rectTemp[i].bottom) / 2;

		// LT���� �Ÿ�
		int nLength = abs(ptCorner[E_CORNER_LEFT_TOP].x - nCx) + abs(ptCorner[E_CORNER_LEFT_TOP].y - nCy);
		int	nIndex	= E_CORNER_LEFT_TOP;
		for (int j=1 ; j<E_CORNER_END ; j++)
		{
			int nTempLenght = abs(ptCorner[j].x - nCx) + abs(ptCorner[j].y - nCy);

			// ���� �Ÿ��� ������ ã��
			if( nLength > nTempLenght)
			{
				nLength	= nTempLenght;
				nIndex	= j;
			}
		}
		
		//////////////////////////////////////////////////////////////////////////		

		// ���� ���� ���
		CStringA strTemp;
		strTemp.Format(("%s\\CornerEdge\\%s_%02d.EdgePT"), szPath, GetPatternStringA(nAlgImg), i);

		// ���� ���� ( �����ڵ� ȯ�� "t" -> "wt" )
		FILE *out = NULL;
		fopen_s(&out, strTemp, "wt");

		if( out != NULL )
		{
			// ����� �ε��� ǥ��
			fprintf_s(out, "CornerIndex%d\n", nIndex);

			// �������� Cell ���� �ȿ� �����ϴ��� Ȯ��
			fprintf_s(out, "CornerInside%d,%d,%d,%d\n", nInside[i][0], nInside[i][1], nInside[i][2], nInside[i][3]);

			for (int j=0 ; j<ptContours[i].size() ; j++)
			{
				// �ڳʸ� ���� & ���� 0��
				fprintf_s(out, "%d,%d\n", ptContours[i][j].x - ptCorner[nIndex].x, ptContours[i][j].y - ptCorner[nIndex].y);
			}

			// ���� �ݱ�
			fclose(out);
			out = NULL;

			// KSW 17.11.14 - White�� ��� Dust�������� ����
			if( nAlgImg == E_IMAGE_CLASSIFY_AVI_WHITE )
			{
				CStringA strCopy;
				strCopy.Format(("%s\\CornerEdge\\%s_%02d.EdgePT"), szPath, GetPatternStringA(E_IMAGE_CLASSIFY_AVI_DUST), i);

				CopyFile((CString)strTemp, (CString)strCopy, FALSE);
			}
		}

		// �ʱ�ȭ
		vector< cv::Point2i >().swap(ptContours[i]);
	}

	// �޸� ����
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
		//��� Check
		DWORD result;
		if (((result=GetFileAttributes(strSavePath)) == -1 ) || !(result&FILE_ATTRIBUTE_DIRECTORY) ) {
			CreateDirectory(strSavePath, NULL);
		}
	}

	long nErrorCode = E_ERROR_CODE_TRUE;

	float	fResize				= (float)dPara[E_PARA_APP_POLYGON_RESIZE];							//���� Resize�� ũ��
	int		nGausBlurSize		= (int)dPara[E_PARA_APP_POLYGON_GAUS_SIZE];							//������ ���ſ� Gaussian Blur Mask Size
	float	fGausSigma			= (float)dPara[E_PARA_APP_POLYGON_GAUS_SIGMA];						//������ ���ſ� Gaussian Blur Sigma ��
	int		nThreshold			= (int)dPara[E_PARA_APP_POLYGON_THRESHOLD];							//Sobel Edge ó�� �� �ʿ��� ���� �����ϱ� ���� �Ӱ谪
	int		nOpenSize			= (int)dPara[E_PARA_APP_POLYGON_OPEN_SIZE];							//Panel�� �ܰ���, BM Line��, Active �ܰ������� ����踦 ��Ȯ�ϰ� �ϱ� ���� �������� ũ��
	int		nSelectBlob			= (int)dPara[E_PARA_APP_POLYGON_SELECT_BLOB];						//���������� Blob �� �� ��°�� ū ���� ������ ������
	int		nDilateSize			= (int)dPara[E_PARA_APP_POLYGON_DILATE_SIZE];						//Active Mask�� ���� �� ���� �Ųٱ� ���� ��â���� ũ��
	int		nErodeSize			= (int)dPara[E_PARA_APP_POLYGON_ERODE_SIZE];						//Active Mask�� ������ �Ųٸ鼭 Ű�� ũ�⸦ ����ũ��� ������ ���� ħ�Ŀ��� ũ��

	//����ó�� Mask Size ����ó��
	GetCheckMaskSize(nGausBlurSize);
	GetCheckMaskSize(nOpenSize);
	GetCheckMaskSize(nErodeSize);

	Point ptPanelCorner[E_CORNER_END];
	for(int nCorner = E_CORNER_LEFT_TOP; nCorner < E_CORNER_END; nCorner++)
 	{
 		ptPanelCorner[nCorner] = ptAlignCorner[nCorner];
 	}

	// ���۰� ���� ���.
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

	//���� ū Blob ��ȯ
	Mat mtBiggist;
	mtBiggist = Mat::zeros(OrgImg.size(), OrgImg.type());
	FindBiggestBlob(OrgImg, mtBiggist);

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, mtBiggist);
	}

	//����
	cv::bitwise_not(mtBiggist, mtBiggist);

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, mtBiggist);
	}

	//Active ���� �и�
	cv::morphologyEx(mtBiggist, mtBiggist, cv::MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(nOpenSize, nOpenSize)));

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, mtBiggist);
	}

	//������ ���� ��°�� ū Blob ��ȯ
	Mat mtSelectBig;
	mtSelectBig = Mat::zeros(mtBiggist.size(), mtBiggist.type());
	SelectBiggestBlob(mtBiggist, mtSelectBig,  nSelectBlob);

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, mtSelectBig);
	}

	//����ȭ �� Active �������� ���۳� �� �Ųٱ�
	cv::morphologyEx(mtSelectBig, mtSelectBig, cv::MORPH_DILATE, getStructuringElement(MORPH_RECT, Size(nDilateSize, nDilateSize)));
	cv::morphologyEx(mtSelectBig, mtSelectBig, cv::MORPH_ERODE, getStructuringElement(MORPH_RECT, Size(nErodeSize, nErodeSize)));

	if(bImageSave)
	{
		strSaveName.Format(_T("%s\\%d.bmp"), strSavePath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strSaveName, mtSelectBig);
	}

	//���� ����ũ��� ����
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

	// ���� ���� ���
	CString str;
	str.Format(_T("%s\\CornerEdge"), strPath);
	CreateDirectory(str, NULL);

	//Polygon ROI �޾ƿ���
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

		// ���� ���� ���
		CStringA strTemp;
		strTemp.Format(("%s\\CornerEdge\\%s_%02d.EdgePT"), szPath, strImageName, nROIInx);

		// ���� ���� ( �����ڵ� ȯ�� "t" -> "wt" )
		FILE *out = NULL;
		fopen_s(&out, strTemp, "wt");

		if( out != NULL )
		{
			vector<vector<cv::Point>> ptActiveArea;
			
			findContours(mtActiveArea(rtPolygonROI[nROIInx]), ptActiveArea, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cv::Point(0,0));

			//Contour�� ���ٸ�
			if(ptActiveArea.size() < 1)
			{
				// ���� �ݱ�
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
						// y��ǥ ��������
						if( ptActiveArea[0][j].y > ptActiveArea[0][m].y )
						{
							ptTempS				= ptActiveArea[0][j];
							ptActiveArea[0][j]	= ptActiveArea[0][m];
							ptActiveArea[0][m]	= ptTempS;
						}
					}
					else
					{
						// y��ǥ ��������
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
			
			// ���� �ݱ�
			fclose(out);
			out = NULL;
		}
	}
	
	delete[] rtPolygonROI;


	return E_ERROR_CODE_TRUE;
}

// �ܰ� Round & Camera Hole � ���� & ���� ����
long CInspectAlign::SetFindContour_(cv::Mat matSrcBuf, INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], INSP_AREA CHoleROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRoundROICnt, int nCHoleROICnt, double* dPara, int nAlgImg, int nRatio, CString strPath)
{
	// ���� ����
	if (nAlgImg == E_IMAGE_CLASSIFY_AVI_BLACK)		return E_ERROR_CODE_TRUE;
	else if (nAlgImg == E_IMAGE_CLASSIFY_AVI_PCD)	return E_ERROR_CODE_TRUE;

	// KSW 17.11.14 - Dust ���� ����
	// Round�� ��� ��� �ö�� ��� ����
	// White ���� �ܰ��� �����Ͽ� ���
	if (nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST)		return E_ERROR_CODE_TRUE;

	// ���۰� ���� ���.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples = 3;	// ����
	double	distThreshold = 10;	// ����

	int		nMorp = dPara[E_PARA_ALIGN_MORP];
	int		nThreshold = dPara[E_PARA_ALIGN_THRESHOLD];
	int		nMinArea = (int)(dPara[E_PARA_CELL_SIZE_X] * dPara[E_PARA_CELL_SIZE_Y] * nRatio * nRatio);

	bool	bRoundSet = (dPara[E_PARA_ROUND_SETTING] > 0) ? true : false;
	bool	bCHoleSet = (dPara[E_PARA_CHOLE_SETTING] > 0) ? true : false;
	//////////////////////////////////////////////////////////////////////////

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	// ���� �Ҵ� & �ʱ�ȭ


	//////////////////////////////////////////////////////////////////////////
	// ShiftCopy
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matDstBuf = cMem->GetMat(matSrcBuf.size(), matSrcBuf.type());
	// �Ķ���� �߰� �ʿ�

	// ���� �������� ��¦ ���... ( ���� )
	int nInPixel = 3;

	// PS ����� ���
	if (nRatio != 1)
	{
		// Shift Copy Parameter �޾� ����
		int		nRedPattern = (int)dPara[E_PARA_SHIFT_COPY_R];
		int		nGreenPattern = (int)dPara[E_PARA_SHIFT_COPY_G];
		int		nBluePattern = (int)dPara[E_PARA_SHIFT_COPY_B];

		int nCpyX = 0, nCpyY = 0, nLoopX = 0, nLoopY = 0;

		// ���Ϻ�...
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

	// ������ ���, ���� �ڵ� ���
	if (nErrorCode != E_ERROR_CODE_TRUE)
	{
		return nErrorCode;
	}

	//////////////////////////////////////////////////////////////////////////
	// AVI�� ��츸 ���, Cell �迭 ���� ����� ä��� �뵵
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

		// Morphology Close ( Cell ���� ����� ä��� )
		cv::morphologyEx(matSrc8bit, matTempBuf, MORPH_CLOSE, StructElem);

		StructElem.release();
	}
	else
		matSrc8bit.copyTo(matTempBuf);

	cv::threshold(matTempBuf, matTempBuf, nThreshold, 255.0, THRESH_BINARY);


	//////////////////////////////////////////////////////////////////////////
	// Cell ���� ã��
	//////////////////////////////////////////////////////////////////////////

	// �˻� ���� Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matTempBuf, nMinArea, rectCell);

	// ������ ���, ���� �ڵ� ���
	if (nErrorCode != E_ERROR_CODE_TRUE)
	{
		matTempBuf.release();
		return nErrorCode;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���� ȸ��
	//////////////////////////////////////////////////////////////////////////

	long double	dValueA, dValueB;

	// Top ���� ã��
	nErrorCode = RobustFitLine(matTempBuf, rectCell, dValueA, dValueB, nMinSamples, distThreshold, E_ALIGN_TYPE_TOP);

	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = atan(dValueA) * 180. / PI;
	DoRotateImage(matTempBuf, matTempBuf, dTheta);


	//////////////////////////////////////////////////////////////////////////
	// ���� ���� ã�� & Hole ã��
	//////////////////////////////////////////////////////////////////////////	

	// �ܺ� �ܰ��� & ���� �ܰ���
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
	// �ڳ� ������ 4�� ã��
	//////////////////////////////////////////////////////////////////////////

	cv::Point	ptCorner[E_CORNER_END];

	long	nWidth = (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcBuf.rows;	// ���� ���� ũ��

	nErrorCode = FindCornerPoint(vertices, ptCorner, nWidth, nHeight);

	// ������ ���, ���� �ڵ� ���
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


// Round ��ǥ ����
void CInspectAlign::SetFindRound(cv::Mat& matTempBuf, vector< vector< cv::Point2i > > contours, cv::Point ptCorner[E_CORNER_END], INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRoundROICnt, int nContourIdx, int nAlgImg, int nRatio, CString strPath)
{
	// Image Save 0 On / -1 Off
	int nSaveImg = -1;
	// � ���� ���� ���� ���� �ʿ� ( ������ ���� )
	CRect	rectTemp[MAX_MEM_SIZE_E_INSPECT_AREA];
	bool	nInside[MAX_MEM_SIZE_E_INSPECT_AREA][E_CORNER_END];

	for (int i = 0; i < nRoundROICnt; i++)
	{
		// UI ������ ���� ��������
		rectTemp[i] = RoundROI[i].rectROI;

		// PS ��� ����
		rectTemp[i].left *= nRatio;
		rectTemp[i].top *= nRatio;
		rectTemp[i].right *= nRatio;
		rectTemp[i].bottom *= nRatio;

		// Left-Top ��ǥ�� �������� ��ǥ���� ������ ����
		// ���� ���� ������ �°� ����
		rectTemp[i].OffsetRect(CPoint(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_TOP].y));

		// ����ó��
		if (rectTemp[i].left < 0)		rectTemp[i].left = 0;
		if (rectTemp[i].top < 0)		rectTemp[i].top = 0;
		if (rectTemp[i].right < 0)		rectTemp[i].right = 0;
		if (rectTemp[i].bottom < 0)		rectTemp[i].bottom = 0;

		if (rectTemp[i].left >= matTempBuf.cols)	rectTemp[i].left = matTempBuf.cols - 1;
		if (rectTemp[i].top >= matTempBuf.rows)	rectTemp[i].top = matTempBuf.rows - 1;
		if (rectTemp[i].right >= matTempBuf.cols)	rectTemp[i].right = matTempBuf.cols - 1;
		if (rectTemp[i].bottom >= matTempBuf.rows)	rectTemp[i].bottom = matTempBuf.rows - 1;

		// �������� Cell ���� ���� �ȿ� �����ϴ��� Ȯ��
		nInside[i][E_CORNER_LEFT_TOP] = (matTempBuf.at<uchar>(rectTemp[i].top, rectTemp[i].left) != 0) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_TOP] = (matTempBuf.at<uchar>(rectTemp[i].top, rectTemp[i].right) != 0) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_BOTTOM] = (matTempBuf.at<uchar>(rectTemp[i].bottom, rectTemp[i].right) != 0) ? 1 : 0;
		nInside[i][E_CORNER_LEFT_BOTTOM] = (matTempBuf.at<uchar>(rectTemp[i].bottom, rectTemp[i].left) != 0) ? 1 : 0;
	}
	// ��� Ȯ�� ��
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

	// � �ܰ���
	vector< cv::Point2i > ptContours[MAX_MEM_SIZE_E_INSPECT_AREA];
	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
		vector< cv::Point2i >().swap(ptContours[i]);

	// ������ ���� ��ŭ
	for (int i = 0; i < contours.size(); i++)
	{
		if (i != nContourIdx) continue;
		for (int j = 0; j < contours[i].size(); j++)
		{
			// UI ������ ���� ������ŭ
			for (int k = 0; k < nRoundROICnt; k++)
			{
				// ��� ���ϴ� �� ����
				if (!RoundROI[k].bUseROI)	continue;

				// ������ ���� �ȿ�, �ش� ��ǥ�� �ִ� ���
				if (rectTemp[k].PtInRect(CPoint(contours[i][j].x, contours[i][j].y)))
				{
					// ��ǥ �߰�
					ptContours[k].push_back(cv::Point2i(contours[i][j]));
					break;
				}
			}
		}
		// �ʱ�ȭ
		vector< cv::Point2i >().swap(contours[i]);
	}

	//////////////////////////////////////////////////////////////////////////
	// ���� ����
	// �ٰ��� & ���� ����ä�ﶧ, ������� �ʿ� ��...
	//////////////////////////////////////////////////////////////////////////

	cv::Point2i ptTempS;
	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		for (int j = 0; j < ptContours[i].size(); j++)
		{
			for (int m = j + 1; m < ptContours[i].size(); m++)
			{
				// y��ǥ�� ������ ���� ������ ����
				if (ptContours[i][j].y > ptContours[i][m].y)
				{
					ptTempS = ptContours[i][j];
					ptContours[i][j] = ptContours[i][m];
					ptContours[i][m] = ptTempS;
				}
				// y��ǥ�� ���� ���
				else if (ptContours[i][j].y == ptContours[i][m].y)
				{
					// ���������� �հŸ��� ��� ����
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

	// ���� ���� ���
	CString str;
	str.Format(_T("%s\\CornerEdge"), strPath);
	CreateDirectory(str, NULL);

	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		// ���� ���� ��� ����
		if (ptContours[i].size() <= 0)	continue;

		//////////////////////////////////////////////////////////////////////////
		// ���� ������, ����� Cell ���������� ���� ����� ������ ã��
		//////////////////////////////////////////////////////////////////////////

		int nCx = (rectTemp[i].left + rectTemp[i].right) / 2;
		int nCy = (rectTemp[i].top + rectTemp[i].bottom) / 2;

		// LT���� �Ÿ�
		int nLength = abs(ptCorner[E_CORNER_LEFT_TOP].x - nCx) + abs(ptCorner[E_CORNER_LEFT_TOP].y - nCy);
		int	nIndex = E_CORNER_LEFT_TOP;
		for (int j = 1; j < E_CORNER_END; j++)
		{
			int nTempLenght = abs(ptCorner[j].x - nCx) + abs(ptCorner[j].y - nCy);

			// ���� �Ÿ��� ������ ã��
			if (nLength > nTempLenght)
			{
				nLength = nTempLenght;
				nIndex = j;
			}
		}

		//////////////////////////////////////////////////////////////////////////		

		// ���� ���� ���
		CStringA strTemp;
		strTemp.Format(("%s\\CornerEdge\\%s_%02d.EdgePT"), szPath, GetPatternStringA(nAlgImg), i);

		// ���� ���� ( �����ڵ� ȯ�� "t" -> "wt" )
		FILE *out = NULL;
		fopen_s(&out, strTemp, "wt");

		if (out != NULL)
		{
			// ����� �ε��� ǥ��
			fprintf_s(out, "CornerIndex%d\n", nIndex);

			// �������� Cell ���� �ȿ� �����ϴ��� Ȯ��
			fprintf_s(out, "CornerInside%d,%d,%d,%d\n", nInside[i][0], nInside[i][1], nInside[i][2], nInside[i][3]);

			for (int j = 0; j < ptContours[i].size(); j++)
			{
				// �ڳʸ� ���� & ���� 0��
				fprintf_s(out, "%d,%d\n", ptContours[i][j].x - ptCorner[nIndex].x, ptContours[i][j].y - ptCorner[nIndex].y);
			}

			// ���� �ݱ�
			fclose(out);
			out = NULL;

			// KSW 17.11.14 - White�� ��� Dust�������� ����
			if (nAlgImg == E_IMAGE_CLASSIFY_AVI_WHITE)
			{
				CStringA strCopy;
				strCopy.Format(("%s\\CornerEdge\\%s_%02d.EdgePT"), szPath, GetPatternStringA(E_IMAGE_CLASSIFY_AVI_DUST), i);

				CopyFile((CString)strTemp, (CString)strCopy, FALSE);
			}
		}

		// �ʱ�ȭ
		vector< cv::Point2i >().swap(ptContours[i]);
	}
}


// Camera Hole ��ǥ ����
void CInspectAlign::SetFindCHole(cv::Mat& matTempBuf, vector< vector< cv::Point2i > > contours, cv::Point ptCorner[E_CORNER_END], INSP_AREA CHoleROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nCHoleROICnt, int nContourIdx, int nAlgImg, int nRatio, CString strPath)
{
	// Image Save 0 On / -1 Off
	int		nSaveImg = -1;

	// � ���� ���� ���� ���� �ʿ� ( ������ ���� )
	CRect	rectTemp[MAX_MEM_SIZE_E_INSPECT_AREA];
	bool	nInside[MAX_MEM_SIZE_E_INSPECT_AREA][E_CORNER_END];

	for (int i = 0; i < nCHoleROICnt; i++)
	{
		// UI ������ ���� ��������
		rectTemp[i] = CHoleROI[i].rectROI;

		// PS ��� ����
		rectTemp[i].left *= nRatio;
		rectTemp[i].top *= nRatio;
		rectTemp[i].right *= nRatio;
		rectTemp[i].bottom *= nRatio;

		// Left-Top ��ǥ�� �������� ��ǥ���� ������ ����
		// ���� ���� ������ �°� ����
		rectTemp[i].OffsetRect(CPoint(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_TOP].y));

		// ����ó��
		if (rectTemp[i].left < 0)		rectTemp[i].left = 0;
		if (rectTemp[i].top < 0)		rectTemp[i].top = 0;
		if (rectTemp[i].right < 0)		rectTemp[i].right = 0;
		if (rectTemp[i].bottom < 0)		rectTemp[i].bottom = 0;

		if (rectTemp[i].left >= matTempBuf.cols)	rectTemp[i].left = matTempBuf.cols - 1;
		if (rectTemp[i].top >= matTempBuf.rows)	rectTemp[i].top = matTempBuf.rows - 1;
		if (rectTemp[i].right >= matTempBuf.cols)	rectTemp[i].right = matTempBuf.cols - 1;
		if (rectTemp[i].bottom >= matTempBuf.rows)	rectTemp[i].bottom = matTempBuf.rows - 1;

		// �������� Cell ���� ���� �ȿ� �����ϴ��� Ȯ��
		nInside[i][E_CORNER_LEFT_TOP] = (matTempBuf.at<uchar>(rectTemp[i].top, rectTemp[i].left) != 0) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_TOP] = (matTempBuf.at<uchar>(rectTemp[i].top, rectTemp[i].right) != 0) ? 1 : 0;
		nInside[i][E_CORNER_RIGHT_BOTTOM] = (matTempBuf.at<uchar>(rectTemp[i].bottom, rectTemp[i].right) != 0) ? 1 : 0;
		nInside[i][E_CORNER_LEFT_BOTTOM] = (matTempBuf.at<uchar>(rectTemp[i].bottom, rectTemp[i].left) != 0) ? 1 : 0;
	}
	// ��� Ȯ�� ��
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

	// � �ܰ���
	vector< cv::Point2i > ptContours[MAX_MEM_SIZE_E_INSPECT_AREA];
	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
		vector< cv::Point2i >().swap(ptContours[i]);

	// ������ ���� ��ŭ
	for (int i = 0; i < contours.size(); i++)
	{
		if (i == nContourIdx) continue;
		for (int j = 0; j < contours[i].size(); j++)
		{
			// UI ������ ���� ������ŭ
			for (int k = 0; k < nCHoleROICnt; k++)
			{
				// ��� ���ϴ� �� ����
				if (!CHoleROI[k].bUseROI)	continue;

				// ������ ���� �ȿ�, �ش� ��ǥ�� �ִ� ���
				if (rectTemp[k].PtInRect(CPoint(contours[i][j].x, contours[i][j].y)))
				{
					// ��ǥ �߰�
					ptContours[k].push_back(cv::Point2i(contours[i][j]));
					break;
				}
			}
		}
		// �ʱ�ȭ
		vector< cv::Point2i >().swap(contours[i]);
	}


	//////////////////////////////////////////////////////////////////////////
	// ���� ����
	// �ٰ��� & ���� ����ä�ﶧ, ������� �ʿ� ��...
	//////////////////////////////////////////////////////////////////////////

	cv::Point2i ptTempS;
	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		for (int j = 0; j < ptContours[i].size(); j++)
		{
			for (int m = j + 1; m < ptContours[i].size(); m++)
			{
				// y��ǥ�� ������ ���� ������ ����
				if (ptContours[i][j].y > ptContours[i][m].y)
				{
					ptTempS = ptContours[i][j];
					ptContours[i][j] = ptContours[i][m];
					ptContours[i][m] = ptTempS;
				}
				// y��ǥ�� ���� ���
				else if (ptContours[i][j].y == ptContours[i][m].y)
				{
					// ���������� �հŸ��� ��� ����
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

	// ���� ���� ���
	CString str;
	str.Format(_T("%s\\CameraHole"), strPath);
	CreateDirectory(str, NULL);

	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		// ���� ���� ��� ����
		if (ptContours[i].size() <= 0)	continue;

		//////////////////////////////////////////////////////////////////////////
		// ���� ������, ����� Cell ���������� ���� ����� ������ ã��
		//////////////////////////////////////////////////////////////////////////

		int nCx = (rectTemp[i].left + rectTemp[i].right) / 2;
		int nCy = (rectTemp[i].top + rectTemp[i].bottom) / 2;

		// LT���� �Ÿ�
		int nLength = abs(ptCorner[E_CORNER_LEFT_TOP].x - nCx) + abs(ptCorner[E_CORNER_LEFT_TOP].y - nCy);
		int	nIndex = E_CORNER_LEFT_TOP;
		for (int j = 1; j < E_CORNER_END; j++)
		{
			int nTempLenght = abs(ptCorner[j].x - nCx) + abs(ptCorner[j].y - nCy);

			// ���� �Ÿ��� ������ ã��
			if (nLength > nTempLenght)
			{
				nLength = nTempLenght;
				nIndex = j;
			}
		}

		//////////////////////////////////////////////////////////////////////////		

		// ���� ���� ���
		CStringA strTemp;
		strTemp.Format(("%s\\CameraHole\\%s_%02d.EdgePT"), szPath, GetPatternStringA(nAlgImg), i);

		// ���� ���� ( �����ڵ� ȯ�� "t" -> "wt" )
		FILE *out = NULL;
		fopen_s(&out, strTemp, "wt");

		if (out != NULL)
		{
			// ����� �ε��� ǥ��
			fprintf_s(out, "CameraHoleIndex%d\n", nIndex);

			// �������� Cell ���� �ȿ� �����ϴ��� Ȯ��
			fprintf_s(out, "CameraHoleInside%d,%d,%d,%d\n", nInside[i][0], nInside[i][1], nInside[i][2], nInside[i][3]);

			for (int j = 0; j < ptContours[i].size(); j++)
			{
				// �ڳʸ� ���� & ���� 0��
				fprintf_s(out, "%d,%d\n", ptContours[i][j].x - ptCorner[nIndex].x, ptContours[i][j].y - ptCorner[nIndex].y);
			}

			// ���� �ݱ�
			fclose(out);
			out = NULL;

			// KSW 17.11.14 - White�� ��� Dust�������� ����
			if (nAlgImg == E_IMAGE_CLASSIFY_AVI_WHITE)
			{
				CStringA strCopy;
				strCopy.Format(("%s\\CameraHole\\%s_%02d.EdgePT"), szPath, GetPatternStringA(E_IMAGE_CLASSIFY_AVI_DUST), i);

				CopyFile((CString)strTemp, (CString)strCopy, FALSE);
			}
		}

		// �ʱ�ȭ
		vector< cv::Point2i >().swap(ptContours[i]);
	}
}

long CInspectAlign::GetBMCorner(cv::Mat Src, double* dAlgPara, Point* ptPanelCorner, cv::Rect& rtBMCorner)
{
	int		nGausBlurSize		= (int)dAlgPara[E_PARA_APP_POLYGON_GAUS_SIZE];
	float	fGausSigma			= (float)dAlgPara[E_PARA_APP_POLYGON_GAUS_SIGMA];
	int		nCornerROISize		= (int)dAlgPara[E_PARA_APP_POLYGON_CORNER_ROI_SIZE];					//Corner ���� ����� ROI ũ��
	float	fBM_LT_Theta		= (float)dAlgPara[E_PARA_APP_POLYGON_BM_THETA_LT];						//BM Line Search�� ���� ����
	float	fBM_RB_Theta		= (float)dAlgPara[E_PARA_APP_POLYGON_BM_THETA_RB];						
	int		nBMIgnore			= (int)dAlgPara[E_PARA_APP_POLYGON_BM_IGNORE_GV];					//BM Line Search�� ���� ���� ������ �ּ� GV �� ����
	float	fBMThreshold_Ratio	= (float)dAlgPara[E_PARA_APP_POLYGON_BM_PRE_THRESH_RATIO];			//��� ������ Threshold �Ҷ� ��� ���� ������� ������ ����� �������� ���� ���� ũ��


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

	//���� ū ����� ���� Mask�� ���
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

	//Countour ��ǥ ������ 50�� ���� �̸� �˻� ���� ����.
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

// �뷫���� Cell ��ġ�� ã�´�.
long CInspectAlign::FindCellEdge_For_Morphology(cv::Mat matSrc, int nThreshold,cv::Rect &rcFindCellROI)
{

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//�ӵ� ����� ���� �ܰ��� �������� �ϱ����� ���� - ���������� ���������� �ϱ����� ������ ã�´�.
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//double dblTmpSum;

	//cv::Rect rcTempFindEdgeROI;

	cv::Size szTempBufSize = matSrc.size();
	int nTempBufWidth = szTempBufSize.width;
	int nTempBufHeight = szTempBufSize.height;

	//////////////////////////////////
	//Y ������� Edge�� ���� ã�´�.//
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
			//Y�� ���κ�
			
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
			//Y�� �Ʒ� �κ�
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

	if (nFindStart == -1 || nFindEnd == -1) //��ã���� ���
		return E_ERROR_CODE_ALIGN_LENGTH_RANGE_ERROR;
	
	rcFindCellROI.y = nFindStart;
	rcFindCellROI.height = nFindEnd - rcFindCellROI.y;

	/////////////////////////////
	//X ������� Edge��	ã�´�.//
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
			//���� ã��
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
			//������ ã��
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

	if (nFindStart == -1 || nFindEnd == -1) //��ã���� ���
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

//���������� �� ���� �����
long CInspectAlign::MakeRoI_For_Morphology(cv::Rect rcFindCellROI,
										   int nExtROI_Outer, int nExtROI_Inner_L, int nExtROI_Inner_R, int nExtROI_Inner_T, int nExtROI_Inner_B,
										   cv::Size rcLimit, cv::Rect  *prcMorpROI)
{
	//Cell �ٱ����δ� Outer
	//Cell �������δ� Inner

	int nAddInspArea = 100; // �ߺ� �˻� ����

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
	prcMorpROI[2].x = (prcMorpROI[0].x+ prcMorpROI[0].width) - nAddInspArea;	// Left ������ ���� - 150 pxl ��ŭ �ߺ� ��������
	prcMorpROI[2].y = prcMorpROI[0].y;			// Left ������ ����
	prcMorpROI[2].width = prcMorpROI[1].x - prcMorpROI[2].x + nAddInspArea;	// Right ������ ���� - 150 pxl ��ŭ �ߺ� ��������
	prcMorpROI[2].height = nExtROI_Inner_T + nExtROI_Outer;

	//bottom
	prcMorpROI[3].x = (prcMorpROI[0].x + prcMorpROI[0].width) - nAddInspArea;		// Left ������ ���� - 150 pxl ��ŭ �ߺ� ��������
	prcMorpROI[3].y = rcFindCellROI.y+ rcFindCellROI.height - nExtROI_Inner_B;
	prcMorpROI[3].width = prcMorpROI[1].x - prcMorpROI[3].x + nAddInspArea;	// Right ������ ���� - 150 pxl ��ŭ �ߺ� ��������
	prcMorpROI[3].height = nExtROI_Inner_B + nExtROI_Outer;

	//���� ���� ó��
	RecalRect(prcMorpROI[0], rcLimit);
	RecalRect(prcMorpROI[1], rcLimit);
	RecalRect(prcMorpROI[2], rcLimit);
	RecalRect(prcMorpROI[3], rcLimit);
	
	return E_ERROR_CODE_TRUE;
}

long CInspectAlign::Partial_Morphology(cv::Mat matSrc, cv::Mat matDst, int nMorpType, cv::Mat StructElem, cv::Rect  *prcMorpROI)
{
	//���������� �Էµ� �������� 4�� ����� �Ѵ�. - ���� ����, �¿� ���� ���� ó��		

	cv::Mat matTmpROI_Src_L = matSrc(prcMorpROI[0]);
	cv::Mat matTmpROI_Src_R = matSrc(prcMorpROI[1]);
	cv::Mat matTmpROI_Src_T = matSrc(prcMorpROI[2]);
	cv::Mat matTmpROI_Src_B = matSrc(prcMorpROI[3]);

	cv::Mat matTmpROI_Dst_L = matDst(prcMorpROI[0]);
	cv::Mat matTmpROI_Dst_R = matDst(prcMorpROI[1]);
	cv::Mat matTmpROI_Dst_T = matDst(prcMorpROI[2]);
	cv::Mat matTmpROI_Dst_B = matDst(prcMorpROI[3]);

	//���� ������ �������� - ���� ó���� ���� �Ѵ�.
#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int roop = 0; roop < 2; roop++)
	{
		if (roop == 0)
		{
			//���� ��������
			cv::morphologyEx(matTmpROI_Src_L, matTmpROI_Dst_L, nMorpType, StructElem);
		}

		if (roop == 1)
		{
			//������ ��������
			cv::morphologyEx(matTmpROI_Src_R, matTmpROI_Dst_R, nMorpType, StructElem);
		}
	}	

	//���� �Ʒ��� �������� - ���� ó���� ���� �Ѵ�.
#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int roop = 0; roop < 2; roop++)
	{
		if (roop == 0)
		{
			//���� ��������
			cv::morphologyEx(matTmpROI_Src_T, matTmpROI_Dst_T, nMorpType, StructElem);
		}

		if (roop == 1)
		{
			//������ ��������
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
	//���������� �Էµ� �������� 4�� ����� �Ѵ�. - ���� ����, �¿� ���� ���� ó��		

	cv::Mat matTmpROI_Src_L = matSrc(prcMorpROI[0]);
	cv::Mat matTmpROI_Src_R = matSrc(prcMorpROI[1]);
	cv::Mat matTmpROI_Src_T = matSrc(prcMorpROI[2]);
	cv::Mat matTmpROI_Src_B = matSrc(prcMorpROI[3]);

	cv::Mat matTmpROI_Dst_L = matDst(prcMorpROI[0]);
	cv::Mat matTmpROI_Dst_R = matDst(prcMorpROI[1]);
	cv::Mat matTmpROI_Dst_T = matDst(prcMorpROI[2]);
	cv::Mat matTmpROI_Dst_B = matDst(prcMorpROI[3]);

	//���� ������ �������� - ���� ó���� ���� �Ѵ�.
#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int roop = 0; roop < 2; roop++)
	{
		if (roop == 0)
		{
			//���� Laplacian
			cv::Laplacian(matTmpROI_Src_L, matTmpROI_Dst_L,CV_8U);
		}

		if (roop == 1)
		{
			//������ Laplacian
			cv::Laplacian(matTmpROI_Src_R, matTmpROI_Dst_R, CV_8U);
		}
	}

	//���� �Ʒ��� �������� - ���� ó���� ���� �Ѵ�.
#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int roop = 0; roop < 2; roop++)
	{
		if (roop == 0)
		{
			//���� Laplacian
			cv::Laplacian(matTmpROI_Src_T, matTmpROI_Dst_T, CV_8U);
		}

		if (roop == 1)
		{
			//������ Laplacian
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

// �ܰ� �κ� ó��
long CInspectAlign::DoFillOutArea(cv::Mat& matSrcBuf, cv::Mat& matBKGBuf, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], ROUND_SET tCHoleSet[MAX_MEM_SIZE_E_INSPECT_AREA], cv::Mat* matCHoleROIBuf, cv::Rect* rcCHoleROI, bool* bCHoleAD, double* dPara, int nAlgImg, int nRatio, wchar_t* strID)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));
	// 2021.02.24 HGM G3 ���� Test
	// G3 ���� ���� (2021.02.24 ���� G128�� G3�������� �����)
	if (nAlgImg == E_IMAGE_CLASSIFY_AVI_GRAY_128)		return E_ERROR_CODE_TRUE;

	// Black ���� ����
	if( nAlgImg == E_IMAGE_CLASSIFY_AVI_BLACK )		return E_ERROR_CODE_TRUE;

	// PCD ���� ����
	if (nAlgImg == E_IMAGE_CLASSIFY_AVI_PCD)		return E_ERROR_CODE_TRUE;
	// KSW 17.11.14 - Dust ���� -> DoFillOutAreaDust() ���
	// Round�� ��� ��� �ö�� ��� ����
	if( nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST )		return E_ERROR_CODE_TRUE;

	// ���۰� ���� ���.
	if( matSrcBuf.empty() )							return E_ERROR_CODE_EMPTY_BUFFER;

	// -1 : ���� ���� ����
	// 0 : ���� ����
	int nSaveImageCount = -1;
	 
	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	int		nMinSamples		= 3;	// ����
	double	distThreshold	= 10;	// ����

	int		nMinArea		= (int)(dPara[E_PARA_CELL_SIZE_X] * dPara[E_PARA_CELL_SIZE_Y] * nRatio * nRatio);
	int		nThreshold		= dPara[E_PARA_ALIGN_THRESHOLD];
	int		nMorp			= dPara[E_PARA_ALIGN_MORP];
	double	dAngleError		= dPara[E_PARA_ALIGN_ANGLE_ERR];
	double	dAngleWarning	= dPara[E_PARA_ALIGN_ANGLE_WAR];

	// Round ���� ã�� ���� ���� ( ��,��,��,�� )
	int		nFindRoundOffset = (int)dPara[E_PARA_ROUND_FIND_OFFSET];
	int		nFindCHoleOffset = (int)dPara[E_PARA_CHOLE_FIND_OFFSET];

	// �ܰ� ��� ä�ﶧ, �ּ� ��� GV ����
	// Round & CHole ���� �ܰ� ��� ä�ﶧ, �ּ� ��� GV ����
	int		nRoundMinGV = (int)dPara[E_PARA_ROUND_OTHER_MIN_GV];
	int		nCHoleMinGV = (int)dPara[E_PARA_CHOLE_ROI_MIN_GV];

	// Round & CHole Cell ��� ��/��
	bool	bRoundUse = (dPara[E_PARA_ROUND_USE] > 0) ? true : false;
	bool	bCHoleUse = (dPara[E_PARA_CHOLE_USE] > 0) ? true : false;

	// Round & CHole Cell �������� �� Pixel ����.... ( � �κи�... )
	int		nRoundIn = (int)(dPara[E_PARA_ROUND_IN]);
	int		nCHoleIn = (int)(dPara[E_PARA_CHOLE_IN]);

	nCHoleIn = nCHoleIn * 2 + 1;

	int nShiftX = 0;
	int nShiftY = 0;

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// ShiftCopy
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matDstBuf = cMem->GetMat(matSrcBuf.size(), matSrcBuf.type());
	// �Ķ���� �߰� �ʿ�

	// ���� �������� ��¦ ���... ( ���� )
	int nInPixel = 3;

	// PS ����� ���
	if( nRatio != 1 )
	{
		// Shift Copy Parameter �޾� ����
		int		nRedPattern		=	(int)dPara[E_PARA_SHIFT_COPY_R];
		int		nGreenPattern	=	(int)dPara[E_PARA_SHIFT_COPY_G];
		int		nBluePattern	=	(int)dPara[E_PARA_SHIFT_COPY_B];

		int nCpyX=0, nCpyY=0, nLoopX=0, nLoopY=0;

		// ���Ϻ�...
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

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		return nErrorCode;
	}

	//////////////////////////////////////////////////////////////////////////
	// AVI�� ��츸 ���, Cell �迭 ���� ����� ä��� �뵵
	//////////////////////////////////////////////////////////////////////////

	cv::Mat matSrc_8bit = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	if( matSrcBuf.type() == CV_8UC1 )
		matSrcBuf.copyTo(matSrc_8bit);
	else
		matSrcBuf.convertTo(matSrc_8bit, CV_8UC1, 1./16.);

	cv::Mat matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1);


	//////////////////////////////////////////////////////////////////////////////
	//�ӵ� ����� ���� �ܰ��� �������� �ϱ����� ���� - ���������� ���������� �ϱ����� ������ ã�´�.
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

	//���� ���������� ��� - ���߿� watch �� ���� �ؾ� �ɵ�
	if (bUsePartialMorp)
	{
		cv::Mat matFindCellEdge_For_Morphology = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
		bFindPartialMorpROI = true;
		
		cv::threshold(matSrc_8bit, matFindCellEdge_For_Morphology, nThreshold, 255.0, THRESH_BINARY); // ã�� ���� ����ȭ�ؼ� ã�´�.

		nErrorCode = FindCellEdge_For_Morphology(matFindCellEdge_For_Morphology, nFindCellROI_Threshold, rcFindCellROI);

		// ������ ���, ���� �ڵ� ��´�� ���� ���������� �������� �ʰ� ��ü ���������� �����Ѵ�.
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

		//���������� �� ���� ����� - ���� ã�� ���� ��
		if (bFindPartialMorpROI)
		{
			//�������� ������ left, right, top, bottom �������� �����Ͽ� ������ش�.
			MakeRoI_For_Morphology(rcFindCellROI, nExtROI_Outer, nExtROI_Inner_L, nExtROI_Inner_R, nExtROI_Inner_T, nExtROI_Inner_B, matSrcBuf.size(), rcMorpROI);		

			//�ܰ� ������ ���������� �Ұ��̱� ������ �߰������� �׳� ȸ������ ä����
			// => ���߿� Thresholding ��  Edge�� ����� �������� �߰� ������ �ʿ���� ������
			cv::Rect rcTempDrawRect;

			rcTempDrawRect.x = rcMorpROI[0].x + rcMorpROI[0].width;
			rcTempDrawRect.y = rcMorpROI[2].y + rcMorpROI[2].height;
			rcTempDrawRect.width = rcMorpROI[1].x - rcTempDrawRect.x;
			rcTempDrawRect.height = rcMorpROI[3].y - rcTempDrawRect.y;

			cv::Mat matTmpDrawRect = matTempBuf(rcTempDrawRect); // TempBuf�� ĥ����� ����ε� �������� ĥ�� ����.

			// Cell �߾� Camera Hole�� ã������ ���� ����

			//matSrcBuf(rcTempDrawRect).copyTo(matTmpDrawRect);

			matTmpDrawRect.setTo(128); // �߰� ������ 128 GV �� ĥ���ش�.

			matTmpDrawRect.release();
		}

		// �������� ROI ��� Ȯ�� ��
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

		//�κ� �������� ���� ���
		if (bUsePartialMorp && bFindPartialMorpROI)
		{
			Partial_Morphology(matSrc_8bit, matTempBuf, MORPH_CLOSE, StructElem, rcMorpROI); // �κ� ���������� ����Ѵ�.
		}
		else
		{	
			// Morphology Close ( Cell ���� ����� ä��� )
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
	// Camera Hole ã��
	// Morphology2 ����� �� Camera Hole�� ���� Contour �̻� �߻�
	// Morphology2 �ϱ� �� Camera Hole ���� ä��� ����
	// Morphology1 -> Threshold -> �ڳ���ǥ ȹ�� -> ���� & ��ġ ���� -> CHole Find -> CHole ä��� ( ���� )
	// Parallel Morphology ROI�� Camera Hole �־����
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
		// ���� ���� ã��
		//////////////////////////////////////////////////////////////////////////	

		// ��ü �ܰ���
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
		// �ڳ� ������ 4�� ã��
		//////////////////////////////////////////////////////////////////////////

		cv::Point ptResCorner[E_CORNER_END], ptCornerAlign[E_CORNER_END];

		long	nWidth = (long)matSrcBuf.cols;	// ���� ���� ũ��
		long	nHeight = (long)matSrcBuf.rows;	// ���� ���� ũ��	

		nErrorCode = FindCornerPoint(vertices, ptResCorner, nWidth, nHeight);

		// ������ ���, ���� �ڵ� ���
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
		// ���� ���� ���� ��, ������ ��ǥ ���
		// ��ϵ� � & ROI ������ ������ ��ǥ ����
		//////////////////////////////////////////////////////////////////////////

		// ȸ�� ��ǥ ��� ��, ���
		double	dTheta = -BoundingBox.angle;

		// ����ó��
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
			// ȸ�� ��, ���� ��ǥ ���
			ptCornerAlign[i].x = (int)(dCos * (ptResCorner[i].x - nCx) - dSin * (ptResCorner[i].y - nCy) + nCx);
			ptCornerAlign[i].y = (int)(dSin * (ptResCorner[i].x - nCx) + dCos * (ptResCorner[i].y - nCy) + nCy);
		}

		//////////////////////////////////////////////////////////////////////////
		// ��ϵ� � & ROI ���� -> ���� Grab�� �°� ��ǥ ����
		//////////////////////////////////////////////////////////////////////////

		// ����
		dTheta = BoundingBox.angle;

		// ����ó��
		if (45.0 < dTheta && dTheta < 135.0)	dTheta -= 90.0;
		if (-45.0 > dTheta && dTheta > -135.0)	dTheta += 90.0;

		dTheta *= PI;
		dTheta /= 180.0;
		dSin = sin(dTheta);
		dCos = cos(dTheta);
		dSin_ = sin(-dTheta);
		dCos_ = cos(-dTheta);

		//////////////////////////////////////////////////////////////////////////
		// ��ϵ� � & ROI ���� ��ġã��
		//////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
		{
			// � ���� ������, ���� ��� �ȵȰ����� �Ǵ� / ��������...
			if (tCHoleSet[i].nContourCount <= 0)	continue;

			// ��,��,��,�� ��������, ���� ���� ū �� ã�� ( �����¿� - pixel )
			const int nArrSize = (nFindCHoleOffset * 2 + 1) * (nFindCHoleOffset * 2 + 1);
			__int64 *nSum = (__int64 *)malloc(sizeof(__int64) * nArrSize);
			memset(nSum, 0, sizeof(__int64) * nArrSize);
			cv::Point2i ptTemp;

			// Align ���������� ���� ����� ������
			int nCornerAlign = tCHoleSet[i].nCornerMinLength;

			const int nCount = (int)tCHoleSet[i].nContourCount;
			cv::Point *ptPoly = new cv::Point[nCount];
			memset(ptPoly, 0, sizeof(cv::Point) * nCount);

			int nMinX = matCHoleBuf.cols, nMaxX = 0;
			int nMinY = matCHoleBuf.rows, nMaxY = 0;

			//////////////////////////////////////////////////////////////////////////
			// ����� ��ǥ ����
			//////////////////////////////////////////////////////////////////////////

			// � ���� �����ϴ� ���
			for (int j = 0; j < tCHoleSet[i].nContourCount; j++)
			{
				// ����� ��ǥ����, � ���� �������� �ִ� �Ÿ� ����
				// ��� & ������������ ���� ����
				int XX = tCHoleSet[i].ptContours[j].x + ptCornerAlign[nCornerAlign].x;
				int YY = tCHoleSet[i].ptContours[j].y + ptCornerAlign[nCornerAlign].y;

				// ȸ�� ��, ���� ��ǥ ���
				ptTemp.x = (int)(dCos * (XX - nCx) - dSin * (YY - nCy) + nCx);
				ptTemp.y = (int)(dSin * (XX - nCx) + dCos * (YY - nCy) + nCy);

				// ������ ��ǥ �ֱ�
				ptPoly[j].x = ptTemp.x;
				ptPoly[j].y = ptTemp.y;

				//matCHoleBuf3.at<uchar>(ptPoly[j].y, ptPoly[j].x) = (uchar)255;

				if (nMinX > ptPoly[j].x) nMinX = ptPoly[j].x;
				if (nMaxX < ptPoly[j].x) nMaxX = ptPoly[j].x;
				if (nMinY > ptPoly[j].y) nMinY = ptPoly[j].y;
				if (nMaxY < ptPoly[j].y) nMaxY = ptPoly[j].y;
			}

			//////////////////////////////////////////////////////////////////////////
			// CHole ROI �ּ� GV Check
			//////////////////////////////////////////////////////////////////////////

			int nMeanROIOffset = 40;
			rcCHoleROI[i].x = nMinX - nMeanROIOffset;
			rcCHoleROI[i].y = nMinY - nMeanROIOffset;
			rcCHoleROI[i].width = (nMaxX - nMinX + 1) + nMeanROIOffset * 2;
			rcCHoleROI[i].height = (nMaxY - nMinY + 1) + nMeanROIOffset * 2;

			// Pattern �� �������� GV ���� -> ����ȭ ���󿡼� �� ( CHole Area ũ�� Ȯ��?? )
			cv::Scalar scrSrcMeanGV;
			scrSrcMeanGV = cv::mean(matCHoleBuf(rcCHoleROI[i]));

			// �ּ� GV���� ���� �� CHole AD �ҷ�
			if (scrSrcMeanGV[0] < nCHoleMinGV) bCHoleAD[i] = true;

			// Log
			wchar_t wcLogTemp[MAX_PATH] = { 0 };
			swprintf_s(wcLogTemp, _T("CHole Mean (%.5f)"), scrSrcMeanGV[0]);
			writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, wcLogTemp);

			//////////////////////////////////////////////////////////////////////////
			// CHole ã��
			//////////////////////////////////////////////////////////////////////////

			// � ���� �����ϴ� ���
			for (int j = 0; j < tCHoleSet[i].nContourCount; j++)
			{
				// ��,��,��,�� ��������, ���� ���� ū �� ã��
				int m = 0;
				for (int y = -nFindCHoleOffset; y <= nFindCHoleOffset; y++)
				{
					for (int x = -nFindCHoleOffset; x <= nFindCHoleOffset; x++, m++)
					{
						int indexX = ptPoly[j].x + x;
						int indexY = ptPoly[j].y + y;

						// ����ó��
						if (indexX < 0)						continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if (indexY < 0)						continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if (indexX >= matCHoleBuf.cols)		continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if (indexY >= matCHoleBuf.rows)		continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;

						nSum[m] += matCHoleBuf2.at<uchar>(indexY, indexX);
					}
				}
			}
			// ��,��,��,�� ��������, ���� ���� ū �� ã��
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

			// ��ġ ����
			for (int j = 0; j < tCHoleSet[i].nContourCount; j++)
			{
				ptPoly[j].x += xx;
				ptPoly[j].y += yy;

				// ����ó��
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
			// ���� CHole ä���
			//////////////////////////////////////////////////////////////////////////

			int nCHoleROIOffSet = 40 + nCHoleIn;

			cv::Rect rectCHoleROI;
			rectCHoleROI.x = nMinX - nCHoleROIOffSet;
			rectCHoleROI.y = nMinY - nCHoleROIOffSet;
			rectCHoleROI.width = (nMaxX - nMinX + 1) + nCHoleROIOffSet * 2;
			rectCHoleROI.height = (nMaxY - nMinY + 1) + nCHoleROIOffSet * 2;

			// ��������
			cv::Mat matsrcCHoleROI = matSrcBuf(rectCHoleROI);

			// ����ȭ ���� (��� Mask)
			cv::Mat matCHoleROI = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type(), false);

			// ���� ����ȭ ����
			cv::Mat matSrcCHoleBuf = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type(), false);

			// CHole ä�� ��� ����
			cv::Mat matCHoleFill = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type());
			cv::Mat matCHoleFillX = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type());
			cv::Mat matCHoleFillY = cMem->GetMat(matsrcCHoleROI.size(), matsrcCHoleROI.type());
			
			// CHole ��ǥ ����
			cv::Mat matCHoleFillBuf = matCHoleBuf3(rectCHoleROI);
			// �ܰ��� ���� & CHoleIn
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

			//Pattern �� AD �� �ƴҶ�
			if (!bCHoleAD[i])
			{
				nCHoleROIOffSet = 40;
				rcCHoleROI[i].x = nMinX - nCHoleROIOffSet;
				rcCHoleROI[i].y = nMinY - nCHoleROIOffSet;
				rcCHoleROI[i].width = (nMaxX - nMinX + 1) + nCHoleROIOffSet * 2;
				rcCHoleROI[i].height = (nMaxY - nMinY + 1) + nCHoleROIOffSet * 2;

				matCHoleROIBuf[i] = matCHoleBuf3(rcCHoleROI[i]).clone();
			}

			// CHole �ֺ� ����ũ ����
			cv::blur(matCHoleFillBuf, matCHoleROI, cv::Size(61, 61));
			cv::subtract(matCHoleROI, matCHoleFillBuf, matCHoleROI);
			cv::threshold(matCHoleROI, matCHoleROI, 0.0, 255.0, THRESH_BINARY);

			cv::threshold(matsrcCHoleROI, matSrcCHoleBuf, nThreshold, 255.0, THRESH_BINARY);

			cv::Mat	StructElem = cv::getStructuringElement(MORPH_RECT, cv::Size(5, 5));
			cv::morphologyEx(matSrcCHoleBuf, matSrcCHoleBuf, MORPH_CLOSE, StructElem);
			StructElem.release();

			cv::bitwise_and(matCHoleBuf(rectCHoleROI), matCHoleROI, matCHoleROI);

			cv::bitwise_and(matSrcCHoleBuf, matCHoleROI, matCHoleROI);

			// ���� CHole ä��� ����
			for (int nY = 2; nY < matsrcCHoleROI.rows - 2; nY += 3)
			{
				// ���� ����
				cv::Mat matsrcFillROI = matsrcCHoleROI(cv::Rect(0, nY - 2, matsrcCHoleROI.cols, 5));
				// Mask
				cv::Mat matCHoleFillROI1 = matCHoleROI(cv::Rect(0, nY - 2, matsrcCHoleROI.cols, 5));

				cv::Scalar CHoleMeanGV = cv::mean(matsrcFillROI, matCHoleFillROI1);

				cv::Mat matCHoleFillROI2 = matCHoleFillX(cv::Rect(0, nY - 1, matsrcCHoleROI.cols, 3));

				matCHoleFillROI2.setTo(CHoleMeanGV[0]);
			}

			// ���� CHole ä��� ����
			for (int nX = 2; nX < matsrcCHoleROI.cols - 2; nX += 3)
			{
				// ���� ����
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

			// CHole ������ �����
			cv::bitwise_and(matCHoleFill, matCHoleFillBuf, matCHoleFill);
			cv::blur(matCHoleFill, matCHoleFill, cv::Size(5, 5));
			cv::bitwise_and(matCHoleFill, matCHoleFillBuf, matCHoleFill);

			// ���� ���� ä���
			cv::max(matsrcCHoleROI, matCHoleFill, matsrcCHoleROI);

			// Morphology2�� ���� CHole ä���
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
	// KSW 18.10.31 - �ҷ��� �ܰ����� ���� ���, ���� �κ����� ���� ������ ��ġ ���� �߻�
	// ( �ּ� �簢������ ������ ã��... ������ ��ġ�� 50pixel �̻� ���� ���� ��쵵 ���� )
	// ���� ��������� Ƣ��� ������ �����ϴ� ����
	// ������ ã�� �Ķ���͸� �÷��� ����ص� ����.... ( �� 1�� ���� �ӵ� �þ )
	//  => ��� �簢�� Cell������ ��� �Ұ�
	//////////////////////////////////////////////////////////////////////////

	cv::Mat matTemp2Buf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf.copyTo(matTemp2Buf);

	if( false )
	{
		//cv::Mat matBuf = matTempBuf.clone();

		cv::Mat	StructElem = cv::getStructuringElement(MORPH_RECT, cv::Size(101, 1));
	
		//�κ� �������� ���� ���
		if (bUsePartialMorp && bFindPartialMorpROI)
		{
			Partial_Morphology(matTemp2Buf, matTempBuf, MORPH_OPEN, StructElem, rcMorpROI); // �κ� ���������� ����Ѵ�.
		}
		else
		{
			cv::morphologyEx(matTemp2Buf, matTempBuf, MORPH_OPEN, StructElem);
		}
		
		StructElem.release();

		//////////////////////////////////////////////////////////////////////////

		StructElem = cv::getStructuringElement(MORPH_RECT, cv::Size(1, 101));

		//�κ� �������� ���� ���
		if (bUsePartialMorp && bFindPartialMorpROI)
		{
			Partial_Morphology(matTempBuf, matTemp2Buf, MORPH_OPEN, StructElem , rcMorpROI); // �κ� ���������� ����Ѵ�.
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
	// Cell ���� ã��
	//////////////////////////////////////////////////////////////////////////

	// �˻� ���� Rect
	cv::Rect rectCell;
	nErrorCode = FindCellArea(matTempBuf, nMinArea, rectCell);

	if (nSaveImageCount >= 0)
	{
		CString strTemp;
		strTemp.Format(_T("d:\\%02d_%02d_Area.jpg"), nAlgImg, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matTempBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("FindCellArea."));

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		matTempBuf.release();
		return nErrorCode;
	}

	// Temp �޸� �Ҵ�
	cv::Mat matThreshBuf = cMem->GetMat(matTempBuf.size(), matTempBuf.type());

	// Edge ������ ���� ū�� ã��
	//matTempBuf.copyTo(matThreshBuf);
	//���� ã�� ���� �� - ���ö�þȵ� �κ������� �Ѵ�.
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
	// ���� ���� ã��
	//////////////////////////////////////////////////////////////////////////	

	// ��ü �ܰ���
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
	// �ڳ� ������ 4�� ã��
	//////////////////////////////////////////////////////////////////////////
	cv::Point ptResCorner[E_CORNER_END], ptCornerAlign[E_CORNER_END];	

	long	nWidth	= (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuf.rows;	// ���� ���� ũ��	

	nErrorCode = FindCornerPoint(vertices, ptResCorner, nWidth, nHeight);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;
	
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Find 4-Corner."));

	// Align ��� Ȯ�� ��
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
	// ���� �������� ��¦ ���...
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
	// Rect ���� Ȯ�ο�

	// ���� ũ�� ���� ( �� ������ 200 Pixel �� ��� �־�� �� )
	// ��κ� ��� ä���
	int offset = 100;

	CRect rectROI = new CRect(
		min(ptResCorner[E_CORNER_LEFT_TOP].x,		ptResCorner[E_CORNER_LEFT_BOTTOM].x		) - offset,
		min(ptResCorner[E_CORNER_LEFT_TOP].y,		ptResCorner[E_CORNER_RIGHT_TOP].y		) - offset,
		max(ptResCorner[E_CORNER_RIGHT_TOP].x,		ptResCorner[E_CORNER_RIGHT_BOTTOM].x	) + offset,
		max(ptResCorner[E_CORNER_LEFT_BOTTOM].y,	ptResCorner[E_CORNER_RIGHT_BOTTOM].y	) + offset );

	// �˻� ������ ���� ũ�⸦ ��� ���
	if( rectROI.left	<	0		)	rectROI.left	= 0;
	if( rectROI.top		<	0		)	rectROI.top		= 0;
	if( rectROI.right	>= 	nWidth	)	rectROI.right	= nWidth - 1;
	if( rectROI.bottom	>= 	nHeight	)	rectROI.bottom	= nHeight - 1;

	// �˻� ������ ���� ũ�⸦ ��� ���
	//if( rectROI.left	<	0		||
	//	rectROI.top		<	0		||
	//	rectROI.right	>= 	nWidth	||
	//	rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	//////////////////////////////////////////////////////////////////////////
	// � ó��
	
	// ����ũ �ʱ�ȭ
	matTempBuf.setTo(0);

	int npt[] = { E_CORNER_END };

	// � ��� ���Ҷ� ( Rect �� ��� )
	if( !bRoundUse )
	{
		const cv::Point* ppt[1] = { ptResCorner };

		// ���� ���� ����ũ
		cv::fillPoly(matTempBuf, ppt, npt, 1, cv::Scalar(255, 255, 255));
	}

	// � �κ� ó��
	else
	{
		// Temp �޸� �Ҵ�
		cv::Mat matRndBuf = cMem->GetMat(matTempBuf.size(), matTempBuf.type());

		//////////////////////////////////////////////////////////////////////////
		// ���� ���� ���� ��, ������ ��ǥ ���
		// ��ϵ� � & ROI ������ ������ ��ǥ ����
		//////////////////////////////////////////////////////////////////////////

		// ȸ�� ��ǥ ��� ��, ���
		double	dTheta	= -BoundingBox.angle;

		// ����ó��
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
			// ȸ�� ��, ���� ��ǥ ���
			ptCornerAlign[i].x = (int)( dCos * (ptResCorner[i].x - nCx) - dSin * (ptResCorner[i].y - nCy) + nCx );
			ptCornerAlign[i].y = (int)( dSin * (ptResCorner[i].x - nCx) + dCos * (ptResCorner[i].y - nCy) + nCy );
		}

		//////////////////////////////////////////////////////////////////////////
		// ��ϵ� � & ROI ���� -> ���� Grab�� �°� ��ǥ ���� & ��ġ ã��
		//////////////////////////////////////////////////////////////////////////

		// ����
		dTheta	= BoundingBox.angle;

		// ����ó��
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
			// � ���� ������, ���� ��� �ȵȰ����� �Ǵ� / ��������...
			if( tRoundSet[i].nContourCount <= 0 )	continue;

			// ��,��,��,�� ��������, ���� ���� ū �� ã�� ( �����¿� - pixel )
			const int nArrSize	= (nFindRoundOffset *2+1) * (nFindRoundOffset *2+1);
			__int64 *nSum		= (__int64 *)malloc( sizeof(__int64) * nArrSize );
			memset(nSum, 0, sizeof(__int64) * nArrSize);
			cv::Point2i ptTemp;

			// ����� �ۿ� �ִ� ������ ����
			int nOutsideCnt = 0;
			for (int j=0 ; j<E_CORNER_END ; j++)
			{
				if( !tRoundSet[i].nCornerInside[j] )
					nOutsideCnt++;
			}

			// �Ѱ��� ���ٸ� ���� ������
// 			if( nOutsideCnt == 0 )
// 				AfxMessageBox(_T("Set Corner Err !!!"));

			const int nCount	= (int)tRoundSet[i].nContourCount;
			cv::Point *ptPoly	= new cv::Point[ nCount ];
			memset(ptPoly, 0, sizeof(cv::Point) * nCount );

			// Align ���������� ���� ����� ������
			int nCornerAlign = tRoundSet[i].nCornerMinLength;

			// � ���� �����ϴ� ���
			for (int j=0 ; j<tRoundSet[i].nContourCount ; j++)
			{
				// ����� ��ǥ����, � ���� �������� �ִ� �Ÿ�... ����
				// ��� & ������������ ���� ����
				int XX = tRoundSet[i].ptContours[j].x + ptCornerAlign[nCornerAlign].x;
				int YY = tRoundSet[i].ptContours[j].y + ptCornerAlign[nCornerAlign].y;

				// ȸ�� ��, ���� ��ǥ ���
				ptTemp.x = (int)( dCos * (XX - nCx) - dSin * (YY - nCy) + nCx );
				ptTemp.y = (int)( dSin * (XX - nCx) + dCos * (YY - nCy) + nCy );

				// ������ ��ǥ �ֱ�
				ptPoly[j].x = ptTemp.x;
				ptPoly[j].y = ptTemp.y;				

				// ��,��,��,�� ��������, ���� ���� ū �� ã��
				int m = 0;
				for (int y=-nFindRoundOffset; y<= nFindRoundOffset; y++)
				{
					for (int x=-nFindRoundOffset; x<= nFindRoundOffset; x++, m++)
					{
						int indexX = ptTemp.x + x;
						int indexY = ptTemp.y + y;

						// ����ó��
						if( indexX < 0 )						continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if( indexY < 0 )						continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if( indexX >= matThreshBuf.cols )		continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;
						if( indexY >= matThreshBuf.rows )		continue;	//return E_ERROR_CODE_ALIGN_IMAGE_OVER;

						// KSW 17.07.19 - ���� �������� ���� ��� / �������� �ϸ� ���� ������ ��� ��κ��� ã�� ��� �߻�
						//int k	= abs(matThreshBuf.at<uchar>(indexY+1, indexX) - matThreshBuf.at<uchar>(indexY-1, indexX))
						//		+ abs(matThreshBuf.at<uchar>(indexY, indexX+1) - matThreshBuf.at<uchar>(indexY, indexX-1));

						//nSum[m++] += k;

						nSum[m] += matThreshBuf.at<uchar>(indexY, indexX);
					}
				}
			}

			// ��,��,��,�� ��������, ���� ���� ū �� ã��
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

			// � �κ�, ����� �������� ��¦ �ֱ� ( ��Ȳ�� ���� �ٸ� )
			cv::Point ptRnd = calcRoundIn(tRoundSet, i, nRoundIn);
			xx += ptRnd.x;
			yy += ptRnd.y;

			// �ش� ���� ������ ã�� ���� ���
			// ��� & ������������ ���� ����
			// ã�� ������ ���̰ų� ( nFindOffset ), ���� ��ġ���� �׸���
// 			if( nMax == 0 )
// 				AfxMessageBox(_T("Find Range Over Err !!!"));

			// ã�� ��� 
			{
				// ��ġ ����
				int j = 0;
				for ( ; j<tRoundSet[i].nContourCount ; j++)
				{
					ptPoly[j].x += xx;
					ptPoly[j].y += yy;

					// ����ó��
					if( ptPoly[j].x < 0 )					return E_ERROR_CODE_ALIGN_IMAGE_OVER;
					if( ptPoly[j].y < 0 )					return E_ERROR_CODE_ALIGN_IMAGE_OVER;
					if( ptPoly[j].x >= matRndBuf.cols )		return E_ERROR_CODE_ALIGN_IMAGE_OVER;
					if( ptPoly[j].y >= matRndBuf.rows )		return E_ERROR_CODE_ALIGN_IMAGE_OVER;

					matRndBuf.at<uchar>(ptPoly[j].y, ptPoly[j].x) = (uchar)255;
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// � �̿� ���� ����
			//////////////////////////////////////////////////////////////////////////

			// ������
			ptTempLine[0] = ptPoly[0];
			ptTempLine[1] = ptPoly[nCount-1];

			delete ptPoly;
			ptPoly = NULL;

			// ������ ���
			if( i == 0 )
			{
				ptSE[0] = ptTempLine[0];
				ptSE[1] = ptTempLine[1];
			}
			// ������ ����
			else
			{
				// � �̿� ���� ����
				calcLineConnect(matRndBuf, ptSE, ptTempLine, nSE, i);
			}

// 			delete ptPoly;
// 			ptPoly = NULL;
		}
		// ������ ������ ����
		cv::line(matRndBuf, ptSE[0], ptSE[1], cv::Scalar(255, 255, 255));

		// KSW 17.11.12 - ������ �κ� �ձ�
		cv::dilate(matRndBuf, matTempBuf, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)) );

		// �ܰ��� �������� ä��� ( 4����? )
		cv::floodFill(matTempBuf, BoundingBox.center, cv::Scalar(255, 255, 255));

		matRndBuf.release();
	}	
	// KSW 17.07.17 � �κ� ó��
	//////////////////////////////////////////////////////////////////////////

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Corner Find."));

	// ��� ���� �ѱ�� ( Mura���� ��� )
	cv::bitwise_not(matTempBuf, matBKGBuf);
	
	cv::Mat matEdgeArea = cMem->GetMat(matTempBuf.size(), CV_8UC1, false);

	// ���� ���� �ܰ� ���� ����� ( ���� ���� ��� ��, ��� )
	nErrorCode = FindEdgeArea(matTempBuf, matEdgeArea, offset, cMem);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\0.EdgeArea.bmp"), matEdgeArea);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Area Find."));

	// ��� ���� �޸� �Ҵ�
	cv::Mat matMeanBuf = cMem->GetMat(matSrcBuf.size(), matSrcBuf.type(), false);
	matSrcBuf.copyTo(matMeanBuf);

	// Offset ( ���� ���� ��� ũ�� )
	// nSeg x nSeg ���� �ȿ�, ���� ������ �����ؾ� ��
	// 6.18"�� ���, ũ�� �÷��� �� ( 300 -> 500 )
	// �¿���� offset(���� 70) ��ŭ �þ ����
	// x, y �����Ͽ� ���
	int nSegX = 3;
	int nSegY = offset + offset;

	// ���� ���� ��� ä���
	nErrorCode = FillAreaMeanX(matMeanBuf, matEdgeArea, rectROI, nSegX, nSegY, nRoundMinGV);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\1.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\1.EdgeArea.bmp"), matEdgeArea);

	// Offset ( ���� ���� ��� ũ�� )
	// nSeg x nSeg ���� �ȿ�, ���� ������ �����ؾ� ��
	// 6.18"�� ���, ũ�� �÷��� �� ( 300 -> 500 )
	// �¿���� offset(���� 70) ��ŭ �þ ����
	// x, y �����Ͽ� ���
	nSegX = 800;
	nSegY = 3;

	// ���� ���� ��� ä���
	nErrorCode = FillAreaMeanY(matMeanBuf, matEdgeArea, rectROI, nSegX, nSegY, nRoundMinGV);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\2.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\2.EdgeArea.bmp"), matEdgeArea);

	// ����� �κи� �����
	nErrorCode = FillMerge(matSrcBuf, matMeanBuf, matTempBuf, nAlgImg, cMem );
	
	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\3.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\3.EdgeArea.bmp"), matEdgeArea);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Fill Average."));

	// �޸� ����
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


// �ܰ� �κ� ó��
long CInspectAlign::DoFillOutArea_SVI(cv::Mat& matSrcBuf, cv::Mat& matBKGBuf, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA],
	double* dPara, int nAlgImg, int nCameraNum, int nRatio, wchar_t* strID, cv::Point* ptCorner)
{
	// Black ���� ����
	if( nAlgImg == E_IMAGE_CLASSIFY_SVI_BLACK )		return E_ERROR_CODE_TRUE;

	// ���۰� ���� ���.
	if( matSrcBuf.empty() )							return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	// ����ȭ
	//int		nThreshold		= (int)dPara[E_PARA_SVI_ALIGN_THRESHOLD];

	// Round ���� ���� ����
	int		nCornerSize		= (int)dPara[E_PARA_SVI_ROUND_SIZE];

	// Round Cell ��� ��/��
	int		nRoundUse		= (int)dPara[E_PARA_SVI_ROUND_USE];

	// �������� �˻�
	int		nRoundIn		= (int)dPara[E_PARA_SVI_ROUND_IN];

	// Round ���� �ڳ� ���� ����
	int		nDiagonal		= (int)dPara[E_PARA_SVI_ROUND_DIAGONAL];

	long	nWidth			= (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight			= (long)matSrcBuf.rows;	// ���� ���� ũ��

	//////////////////////////////////////////////////////////////////////////
	// Align ���� �������� ��¦ ���...
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
	// ���� ����
	//////////////////////////////////////////////////////////////////////////

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	CRect rectROI = CRect(
		min(ptCornerIn[E_CORNER_LEFT_TOP].x,	ptCornerIn[E_CORNER_LEFT_BOTTOM].x	),
		min(ptCornerIn[E_CORNER_LEFT_TOP].y,	ptCornerIn[E_CORNER_RIGHT_TOP].y	),
		max(ptCornerIn[E_CORNER_RIGHT_TOP].x,	ptCornerIn[E_CORNER_RIGHT_BOTTOM].x	),
		max(ptCornerIn[E_CORNER_LEFT_BOTTOM].y,	ptCornerIn[E_CORNER_RIGHT_BOTTOM].y	));

	// �˻� ������ ���� ũ�⸦ ��� ���
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	cv::Mat matTempBuf = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);

	// Cell ���� ����
	for (int i=0 ; i<E_CORNER_END ; i++)
		cv::line(matTempBuf, ptCornerIn[i], ptCornerIn[(i+1)%4], cv::Scalar(255, 255, 255), 1);


	//////////////////////////////////////////////////////////////////////////
	// ������ 4���� Round Cell
	//////////////////////////////////////////////////////////////////////////

	if( nRoundUse > 0 )
	{
		// �ܰ� �κ� ����ǥ�÷� �����
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
	// ��ġ(Notch) Round Cell
	//////////////////////////////////////////////////////////////////////////

	if( nRoundUse > 1 )
	{
		// ���� ���� 60 pixel
		int nSizeX = 60;

		// ���� ���� 70%
		int nSizeY = (int)(rectROI.Height() * 0.7);

		// ����
		int nEmptyY = (rectROI.Height() - nSizeY) / 2;

		// Notch �κ� �߶󳻱�
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
	// ��� ���� ����
	//////////////////////////////////////////////////////////////////////////

	// �ܰ��� �������� ä���
	cv::floodFill(matTempBuf, cv::Point(rectROI.CenterPoint().x, rectROI.CenterPoint().y), cv::Scalar(255, 255, 255));

	// ��� ����
	cv::bitwise_not(matTempBuf, matBKGBuf);


	//////////////////////////////////////////////////////////////////////////
	// �ֺ� ä���
	//////////////////////////////////////////////////////////////////////////	

	// ���� ũ�� ���� ( �� ������ 200 Pixel �� ��� �־�� �� )
	int offset = 70;

	rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x,		ptCorner[E_CORNER_LEFT_BOTTOM].x	) - offset,
		min(ptCorner[E_CORNER_LEFT_TOP].y,		ptCorner[E_CORNER_RIGHT_TOP].y		) - offset,
		max(ptCorner[E_CORNER_RIGHT_TOP].x,		ptCorner[E_CORNER_RIGHT_BOTTOM].x	) + offset,
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	) + offset );

	// �˻� ������ ���� ũ�⸦ ��� ���
	if( rectROI.left	<	0		)	rectROI.left	= 0;
	if( rectROI.top		<	0		)	rectROI.top		= 0;
	if( rectROI.right	>= 	nWidth	)	rectROI.right	= nWidth - 1;
	if( rectROI.bottom	>= 	nHeight	)	rectROI.bottom	= nHeight - 1;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// ���� ���� �ܰ� ���� ����� ( ���� ���� ��� ��, ��� )
	cv::Mat matEdgeArea;
	nErrorCode = FindEdgeArea_SVI(matTempBuf, matEdgeArea, 30);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	// ��� ����
	cv::Mat matMeanBuf = matSrcBuf.clone();

	// ���� ���� ��� ä���
	nErrorCode = FillAreaMeanX_SVI(matMeanBuf, matEdgeArea, rectROI, 3, 100, 0);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	// ���� ���� ��� ä���
	nErrorCode = FillAreaMeanY_SVI(matMeanBuf, matEdgeArea, rectROI, 200, 3, 0);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	// Gray -> Color
	cv::cvtColor(matBKGBuf, matTempBuf, COLOR_GRAY2RGB);

	// ����� �κи� �����
	nErrorCode = FillMerge_SVI(matSrcBuf, matMeanBuf, matTempBuf);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	// �Ҵ� ����
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
	
	//Tact Time ���̱� ����
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

		//Tact Time ���̱� ����
		cv::resize(mtRef[nROIInx], mtRef[nROIInx], Size(), 0.25, 0.25);

 		cv::matchTemplate(mtOrg, mtRef[nROIInx], mtMatchRst, CV_TM_CCORR_NORMED);
 		cv::minMaxLoc(mtMatchRst, NULL, &dMatchRate, NULL, &ptMatchResize);
 		
		ptMatch = Point(ptMatchResize.x * 4, ptMatchResize.y * 4);
 		
		if(dMatchRate < 0.6)
			return E_ERROR_CODE_FALSE;

		
		vector<Point>().swap(pt);

		for(int n = 0; n < tRoundSet[nROIInx].nContourCount; n++)
		{
			//ROI �������� ��ǥ���� ������ü�� ���� ��ǥ�� ����
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

	// ���۰� ���� ���.
	if( matSrcBuf.empty() )							return E_ERROR_CODE_EMPTY_BUFFER;

	// Dust ������ �ƴ� ���
	if( nAlgImg != E_IMAGE_CLASSIFY_AVI_DUST )		return E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	// Round Cell & CHole ��� ��/��
	bool	bRoundUse		= (dPara[E_PARA_ROUND_USE] > 0) ? true : false;
	bool	bCHoleUse		= (dPara[E_PARA_CHOLE_USE] > 0) ? true : false;

	// CHole Threshold value
	double dThresValue = dPara[E_PARA_ALIGN_THRESHOLD];

	// Round Cell & CHole �������� �� Pixel ����.... ( � �κи�... )
	int		nRoundIn		= (int)(dPara[E_PARA_ROUND_IN]);
	int		nCHoleIn		= (int)dPara[E_PARA_CHOLE_IN];

	// Find CHole Offset
	int		nFindCHoleOffset = (int)dPara[E_PARA_CHOLE_FIND_OFFSET];

	// �ܰ� ��� ä�ﶧ, �ּ� ��� GV ����
	int		nRoundMinGV		= (int)dPara[E_PARA_ROUND_DUST_MIN_GV];

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;


	//////////////////////////////////////////////////////////////////////////
	// �ڳ� ������ 4�� ã��
	//////////////////////////////////////////////////////////////////////////
	cv::Point ptCornerAlign[E_CORNER_END];

	long	nWidth	= (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuf.rows;	// ���� ���� ũ��


	//////////////////////////////////////////////////////////////////////////
	// ���� �������� ��¦ ���...
	//////////////////////////////////////////////////////////////////////////

	// ���� �������� ��¦ ���... ( ���� )
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
	// Rect ���� Ȯ�ο�

	// ���� ũ�� ���� ( �� ������ 200 Pixel �� ��� �־�� �� )
	int offset = 70;

	CRect rectROI = new CRect(
		min(ptResCorner[E_CORNER_LEFT_TOP].x,		ptResCorner[E_CORNER_LEFT_BOTTOM].x		) - offset,
		min(ptResCorner[E_CORNER_LEFT_TOP].y,		ptResCorner[E_CORNER_RIGHT_TOP].y		) - offset,
		max(ptResCorner[E_CORNER_RIGHT_TOP].x,		ptResCorner[E_CORNER_RIGHT_BOTTOM].x	) + offset,
		max(ptResCorner[E_CORNER_LEFT_BOTTOM].y,	ptResCorner[E_CORNER_RIGHT_BOTTOM].y	) + offset );

	// �˻� ������ ���� ũ�⸦ ��� ���
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	//////////////////////////////////////////////////////////////////////////
	// � ó��

	// ���� ���� 8��Ʈ ��ȯ
	cv::Mat matSrc_8bit = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false); // ���X

	if( matSrcBuf.type() == CV_8UC1 )
		matSrcBuf.copyTo(matSrc_8bit);
	else
		matSrcBuf.convertTo(matSrc_8bit, CV_8UC1, 1./16.);

	cv::Mat matCHoleBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1);
	//CHole ��� ��/��
	if (bCHoleUse)
	{
		// Image Save 0 On / -1 Off
		//int nImgSave = -1;
		int nImgSave = (int)dPara[E_PARA_CHOLE_TEXT];

		writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("CHole FillArea Start."));
		//////////////////////////////////////////////////////////////////////////
		// ���� ���� ���� ��, ������ ��ǥ ���
		// ��ϵ� � & ROI ������ ������ ��ǥ ����
		//////////////////////////////////////////////////////////////////////////

		// ȸ�� ��ǥ ��� ��, ���
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
			// ȸ�� ��, ���� ��ǥ ���
			ptCornerAlign[i].x = (int)(dCos * (ptResCorner[i].x - nCx) - dSin * (ptResCorner[i].y - nCy) + nCx);
			ptCornerAlign[i].y = (int)(dSin * (ptResCorner[i].x - nCx) + dCos * (ptResCorner[i].y - nCy) + nCy);
		}

		//////////////////////////////////////////////////////////////////////////
		// ��ϵ� � & ROI ���� -> ���� Grab�� �°� ��ǥ ���� & ��ġ ã��
		//////////////////////////////////////////////////////////////////////////

		// ����
		dTheta = dAngle;
		dTheta *= PI;
		dTheta /= 180.0;
		dSin = sin(dTheta);
		dCos = cos(dTheta);
		dSin_ = sin(-dTheta);
		dCos_ = cos(-dTheta);

		for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
		{
			// � ���� ������, ���� ��� �ȵȰ����� �Ǵ� / ��������...
			if (tCHoleSet[i].nContourCount <= 0)	continue;

			cv::Point2i ptTemp;

			// ����� �ۿ� �ִ� ������ ����
			int nOutsideCnt = 0;
			for (int j = 0; j < E_CORNER_END; j++)
			{
				if (!tCHoleSet[i].nCornerInside[j])
					nOutsideCnt++;
			}

			const int nCount = (int)tCHoleSet[i].nContourCount;
			cv::Point *ptPoly = new cv::Point[nCount];
			memset(ptPoly, 0, sizeof(cv::Point) * nCount);

			// Align ���������� ���� ����� ������
			int nCornerAlign = tCHoleSet[i].nCornerMinLength;

			int nMaxX = 0, nMinX = matSrc_8bit.cols;
			int nMaxY = 0, nMinY = matSrc_8bit.rows;

			// � ���� �����ϴ� ���
			for (int j = 0; j < tCHoleSet[i].nContourCount; j++)
			{
				// ����� ��ǥ����, � ���� �������� �ִ� �Ÿ�... ����
				int XX = tCHoleSet[i].ptContours[j].x + ptCornerAlign[nCornerAlign].x;
				int YY = tCHoleSet[i].ptContours[j].y + ptCornerAlign[nCornerAlign].y;

				// ȸ�� ��, ���� ��ǥ ���
				ptTemp.x = (int)(dCos * (XX - nCx) - dSin * (YY - nCy) + nCx);
				ptTemp.y = (int)(dSin * (XX - nCx) + dCos * (YY - nCy) + nCy);

				// ������ ��ǥ �ֱ�
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

	// ����ũ
	cv::Mat matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1);

	int npt[] = { E_CORNER_END };

	// � ��� ���Ҷ� ( Rect �� ��� )
	if( !bRoundUse )
	{
		const cv::Point* ppt[1] = { ptResCorner };

		// ���� ���� ����ũ
		cv::fillPoly(matTempBuf, ppt, npt, 1, cv::Scalar(255, 255, 255));
	}

	// � �κ� ó��
	else
	{
		// Temp �޸� �Ҵ�
		cv::Mat matRndBuf = cMem->GetMat(matTempBuf.size(), matTempBuf.type());

		//////////////////////////////////////////////////////////////////////////
		// ���� ���� ���� ��, ������ ��ǥ ���
		// ��ϵ� � & ROI ������ ������ ��ǥ ����
		//////////////////////////////////////////////////////////////////////////

		// ȸ�� ��ǥ ��� ��, ���
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
			// ȸ�� ��, ���� ��ǥ ���
			ptCornerAlign[i].x = (int)( dCos * (ptResCorner[i].x - nCx) - dSin * (ptResCorner[i].y - nCy) + nCx );
			ptCornerAlign[i].y = (int)( dSin * (ptResCorner[i].x - nCx) + dCos * (ptResCorner[i].y - nCy) + nCy );
		}

		//////////////////////////////////////////////////////////////////////////
		// ��ϵ� � & ROI ���� -> ���� Grab�� �°� ��ǥ ���� & ��ġ ã��
		//////////////////////////////////////////////////////////////////////////

		// ����
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
			// � ���� ������, ���� ��� �ȵȰ����� �Ǵ� / ��������...
			if( tRoundSet[i].nContourCount <= 0 )	continue;

			cv::Point2i ptTemp;

			// ����� �ۿ� �ִ� ������ ����
			int nOutsideCnt = 0;
			for (int j=0 ; j<E_CORNER_END ; j++)
			{
				if( !tRoundSet[i].nCornerInside[j] )
					nOutsideCnt++;
			}

			// �Ѱ��� ���ٸ� ���� ������
// 			if( nOutsideCnt == 0 )
// 				AfxMessageBox(_T("Set Corner Err !!!"));

			const int nCount	= (int)tRoundSet[i].nContourCount;
			cv::Point *ptPoly	= new cv::Point[ nCount ];
			memset(ptPoly, 0, sizeof(cv::Point) * nCount );

			// Align ���������� ���� ����� ������
			int nCornerAlign = tRoundSet[i].nCornerMinLength;

			// � ���� �����ϴ� ���
			for (int j=0 ; j<tRoundSet[i].nContourCount ; j++)
			{
				// ����� ��ǥ����, � ���� �������� �ִ� �Ÿ�... ����
				int XX = tRoundSet[i].ptContours[j].x + ptCornerAlign[nCornerAlign].x;
				int YY = tRoundSet[i].ptContours[j].y + ptCornerAlign[nCornerAlign].y;

				// ȸ�� ��, ���� ��ǥ ���
				ptTemp.x = (int)( dCos * (XX - nCx) - dSin * (YY - nCy) + nCx );
				ptTemp.y = (int)( dSin * (XX - nCx) + dCos * (YY - nCy) + nCy );

				// ������ ��ǥ �ֱ�
				ptPoly[j].x = ptTemp.x;
				ptPoly[j].y = ptTemp.y;				
			}

			// � �κ�, ����� �������� ��¦ �ֱ� ( ��Ȳ�� ���� �ٸ� )
			cv::Point ptRnd = calcRoundIn(tRoundSet, i, nRoundIn);

			// ��ġ ����
			int j = 0;
			for ( ; j<tRoundSet[i].nContourCount ; j++)
			{
				ptPoly[j].x += ptRnd.x;
				ptPoly[j].y += ptRnd.y;

				// ����ó��
				if( ptPoly[j].x < 0 )					return E_ERROR_CODE_ALIGN_IMAGE_OVER;
				if( ptPoly[j].y < 0 )					return E_ERROR_CODE_ALIGN_IMAGE_OVER;
				if( ptPoly[j].x >= matRndBuf.cols )		return E_ERROR_CODE_ALIGN_IMAGE_OVER;
				if( ptPoly[j].y >= matRndBuf.rows )		return E_ERROR_CODE_ALIGN_IMAGE_OVER;

				matRndBuf.at<uchar>(ptPoly[j].y, ptPoly[j].x) = (uchar)255;
			}

			//////////////////////////////////////////////////////////////////////////
			// � �̿� ���� ����
			//////////////////////////////////////////////////////////////////////////

			// ������
			ptTempLine[0] = ptPoly[0];
			ptTempLine[1] = ptPoly[nCount-1];

			delete ptPoly;
			ptPoly = NULL;

			// ������ ���
			if( i == 0 )
			{
				ptSE[0] = ptTempLine[0];
				ptSE[1] = ptTempLine[1];
			}
			// ������ ����
			else
			{
				// � �̿� ���� ����
				calcLineConnect(matRndBuf, ptSE, ptTempLine, nSE, i);
			}			
		}
		// ������ ������ ����
		cv::line(matRndBuf, ptSE[0], ptSE[1], cv::Scalar(255, 255, 255));

		// KSW 17.11.12 - ������ �κ� �ձ�
		cv::dilate(matRndBuf, matTempBuf, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)) );

		// �ܰ��� �������� ä��� ( 4����? )
		cv::floodFill(matTempBuf, cv::Point(nWidth/2, nHeight/2), cv::Scalar(255, 255, 255));

		matRndBuf.release();
	}	
	// KSW 17.07.17 � �κ� ó��
	//////////////////////////////////////////////////////////////////////////

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Corner Find."));

	cv::Mat matEdgeArea = cMem->GetMat(matTempBuf.size(), CV_8UC1, false);

	// ���� ���� �ܰ� ���� ����� ( ���� ���� ��� ��, ��� )
	nErrorCode = FindEdgeArea(matTempBuf, matEdgeArea, offset, cMem);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\0.EdgeArea.bmp"), matEdgeArea);

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Area Find."));

	// ��� ���� �޸� �Ҵ�
	cv::Mat matMeanBuf = cMem->GetMat(matSrcBuf.size(), matSrcBuf.type(), false);
	matSrcBuf.copyTo(matMeanBuf);

	// Offset ( ���� ���� ��� ũ�� )
	// nSeg x nSeg ���� �ȿ�, ���� ������ �����ؾ� ��
	// 6.18"�� ���, ũ�� �÷��� �� ( 300 -> 500 )
	// �¿���� offset(���� 70) ��ŭ �þ ����
	// x, y �����Ͽ� ���
	int nSegX = 3;
	int nSegY = offset + offset/2;

	// ���� ���� ��� ä���
	nErrorCode = FillAreaMeanX(matMeanBuf, matEdgeArea, rectROI, nSegX, nSegY, nRoundMinGV);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\1.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\1.EdgeArea.bmp"), matEdgeArea);


	// Offset ( ���� ���� ��� ũ�� )
	// nSeg x nSeg ���� �ȿ�, ���� ������ �����ؾ� ��
	// 6.18"�� ���, ũ�� �÷��� �� ( 300 -> 500 )
	// �¿���� offset(���� 70) ��ŭ �þ ����
	// x, y �����Ͽ� ���
	nSegX = 800;
	nSegY = 3;

	// ���� ���� ��� ä���
	nErrorCode = FillAreaMeanY(matMeanBuf, matEdgeArea, rectROI, nSegX, nSegY, nRoundMinGV);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	//cv::imwrite(("D:\\2.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\2.EdgeArea.bmp"), matEdgeArea);
	//cv::imwrite(("D:\\2.Temp.bmp"), matTempBuf);

	// ����� �κи� �����
	nErrorCode = FillMerge(matSrcBuf, matMeanBuf, matTempBuf, nAlgImg ,cMem);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Edge Fill Average."));

	//cv::imwrite(("D:\\3.Mean.bmp"), matMeanBuf);
	//cv::imwrite(("D:\\3.EdgeArea.bmp"), matEdgeArea);

	// �޸� ����
	matMeanBuf.release();
	matTempBuf.release();

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("End."));
	
// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}


	return E_ERROR_CODE_TRUE;
}


// ���� ȸ��
long CInspectAlign::DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle)
{
	// ���۰� ���� ���. ���� �ڵ�
	if( matSrcBuffer.empty() )
		return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat matTempBuffer = cv::Mat::zeros(matSrcBuffer.size(), CV_8UC1);

	// ȸ�� ��Ʈ���� ����
	cv::Mat matRotation = cv::getRotationMatrix2D( Point(matSrcBuffer.cols/2, matSrcBuffer.rows/2), dAngle, 1.0 );

	// ���� ȸ��
	cv::warpAffine( matSrcBuffer, matTempBuffer, matRotation, matTempBuffer.size() );

	// �Ҵ� ���� ��, ��� �ֱ�
	if( !matDstBuffer.empty() )
		matDstBuffer.release();

	matDstBuffer = matTempBuffer.clone();

	// �޸� �Ҵ� ����
	matTempBuffer.release();
	matRotation.release();

	return E_ERROR_CODE_TRUE;
}


// ��ǥ ȸ��
long CInspectAlign::DoRotatePoint(cv::Point ptSrcPoint, cv::Point& ptDstPoint, cv::Point ptCenter, double dAngle)
{
	// OpenCV ���� ȸ�� �ݴ���� ( ��ȣ "-" ���� )
	double dTheta = -dAngle * PI / 180.;

	double dSin = sin( dTheta );
	double dCos = cos( dTheta );

	// ȸ�� ��, ���� ��ǥ ���
	int X = (int)( dCos * (ptSrcPoint.x - ptCenter.x) - dSin * (ptSrcPoint.y - ptCenter.y) + ptCenter.x );
	int Y = (int)( dSin * (ptSrcPoint.x - ptCenter.x) + dCos * (ptSrcPoint.y - ptCenter.y) + ptCenter.y );

	// ��� �ֱ�
	ptDstPoint.x = X;
	ptDstPoint.y = Y;

	return E_ERROR_CODE_TRUE;
}

// AVI AD �˻� / dResult : ���� Cell ��ġ��
long CInspectAlign::DoFindDefectAD(cv::Mat matSrcBuf, double* dPara, double* dResult, int nRatio)
{
	// ���۰� ���� ���.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	
	// �Ķ���� NULL�� ���.
	if( dPara == NULL )			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	cv::Mat matGrayBuf;

	// ���� �Ҵ� & �ʱ�ȭ
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), matSrcBuf.type(), false);

	// �÷��� ��� ( SVI )
	if( matSrcBuf.channels() != 1 )
	{
		//cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
		//	������D ����.	17.07.07
		//	�����찡 § �ҽ��� ����� ��.
		//	������D �� ������ ���.
		//	AD �� Align ���� ���������� tac time �� ����� ũ�� ������ �� �� ���� �����Ƿ� �׳� �� ���� ���.
		//	�� �� AD �˻� �����ؼ� �� ����ȭ �� ��� SVI �� AVI �Լ� ���� ����.

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
	// ����� ��� ( AVI, APP )
	else
		matSrcBuf.copyTo(matGrayBuf);
		//matGrayBuf = matSrcBuf.clone();

	//////////////////////////////////////////////////////////////////////////
	// KSW 17.08.07 - ������ �ƿ� �ȵȰ�� ������
	//////////////////////////////////////////////////////////////////////////

	// stdDev ���ϱ�
	cv::Scalar m, s;
	cv::meanStdDev(matGrayBuf, m, s);

	// 1 : ��� ��� (Mean GV)
	dResult[1] = double(m[0]);

	// 2 : ǥ������ (Std)
	dResult[2] = double(s[0]);

	// ������ �������� �����Ƿ�.... ( �غ��� 0.xxx ���� ��... )
	// ��ü ���� ��չ�� 1���� ������ ����ȵȰ����� �Ǵ�
	if( dResult[1] < 1.0 )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_OFF;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	double	dRate	= dPara[E_PARA_AD_RATE];
	double	dMinGV	= dPara[E_PARA_AD_MIN_GV];
	double	dMaxGV	= dPara[E_PARA_AD_MAX_GV];
	double	dMaxStd	= dPara[E_PARA_AD_MAX_STD];
	int		nLineYCount = (int)dPara[E_PARA_AD_Y_LINE];

	// Block Ȯ��
	int		nBlockGV_X		= (int)(dPara[E_PARA_AD_BLOCK_X_GV]);
	float	fBlockArea_X	= (float)(dPara[E_PARA_AD_BLOCK_X_AREA]);
	int		nBlockGV_Y		= (int)(dPara[E_PARA_AD_BLOCK_Y_GV]);
	float	fBlockArea_Y	= (float)(dPara[E_PARA_AD_BLOCK_Y_AREA]);

	double	dZoom	= dPara[E_PARA_AD_ZOOM];
	int		nModelX	= (int)dPara[E_PARA_CELL_SIZE_X] * nRatio;
	int		nModelY	= (int)dPara[E_PARA_CELL_SIZE_Y] * nRatio;

	// �Ķ���� �̻��� ���
	if( dZoom < 1 )	return E_ERROR_CODE_ALIGN_WARNING_PARA;

	// ��� ���ϴ� ��� ������
	if( dRate <= 0 )
	{
		dResult[0] = 100.0;
		dResult[1] = 0;
		dResult[2] = 0;

		return E_ERROR_CODE_TRUE;
	}

	// ��ҵ� �̹��� ũ��
	int nWidth	= matGrayBuf.cols / dZoom;
	int nHeight	= matGrayBuf.rows / dZoom;

	// ���� ũ�� ���
	cv::Mat matResizeBuf, matResBuf, matModelBuf;

	// ���� �Ҵ� & �ʱ�ȭ
	matResizeBuf	= cMem->GetMat(nHeight, nWidth, matSrcBuf.type(), false);
	matResBuf		= cMem->GetMat(nHeight, nWidth, CV_32FC1, false);
	matModelBuf		= cMem->GetMat((int)(nModelY/dZoom), (int)(nModelX/dZoom), CV_8UC1, false);

	cv::resize(matGrayBuf, matResizeBuf, cv::Size(nWidth, nHeight));

	// ���� ��� �� ���� ( Gray Value : 50 )
	matModelBuf.setTo(50);

	if( matResizeBuf.type() == CV_8UC1 )
	{
		// ��Ī
		cv::matchTemplate(matResizeBuf, matModelBuf, matResBuf, CV_TM_CCORR_NORMED);
	}
	else
	{
		// 12bit --> 8bit ��ȯ
		cv::Mat matTemp_8bit = cMem->GetMat(nHeight, nWidth, CV_8UC1, false);
		matResizeBuf.convertTo(matTemp_8bit, CV_8UC1, 1./16.);

		// ��Ī
		cv::matchTemplate(matTemp_8bit, matModelBuf, matResBuf, CV_TM_CCORR_NORMED);

		matTemp_8bit.release();
	}	

	// ��Ī��
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

	// 0 : ��ġ�� (Rate)
	// �ۼ�Ʈ�� ����
	dResult[0] *= 100.0;

	// �޸� �Ҵ� ����
	matResizeBuf.release();
	matResBuf.release();
	matModelBuf.release();
	matGrayBuf.release();

	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ��ã�� ���
	// �˻��ϰ��� �ϴ� ��Ⱑ �ƴ� ���
	//////////////////////////////////////////////////////////////////////////

	// �ִ밪 ��Ī���� ������ ������ ������� ���÷��� �̻�
	if( dResult[0] < dRate )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// ��Ī �� �κ� ��� �˻�
	cv::Rect rect(xx*dZoom, yy*dZoom, nModelX, nModelY);

	// �˻� ������ ���� ũ�⸦ ��� ���
	if( rect.x	<	0								||
		rect.y	<	0								||
		rect.x + rect.width		>= 	matSrcBuf.cols	||
		rect.y + rect.height	>= 	matSrcBuf.rows	)		return E_ERROR_CODE_ALIGN_IMAGE_OVER;
		
	if( rect.width <= 1 )									return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rect.height <= 1 )									return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// ��Ī�� �κ� ���� �ڸ���
	matGrayBuf = matSrcBuf(rect);

	// stdDev ���ϱ�
	cv::meanStdDev(matGrayBuf, m, s);
	
	// 1 : ��� ��� (Mean GV)
	dResult[1] = double(m[0]);

	// 2 : ǥ������ (Std)
	dResult[2] = double(s[0]);

	// ������ ���Ѱ��, ���÷��� �̻�
	if (dResult[2] > dMaxStd)
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// ��Ⱑ ���� ���, ���÷��� �̻�
	if (dResult[1] > dMaxGV)
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// ��Ⱑ ��ο� ���, ���÷��� �̻�
	if (dResult[1] < dMinGV)
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.11.05 - ���� �ҷ����� ���� ���� ������ �ʹ� ���� ��� ���� �̻�
	//////////////////////////////////////////////////////////////////////////

	// ��Ī�� �κ� ���� �ڸ���
	//cv::Mat matROIBuf = matGrayBuf(rect);

	// ������ ����
	int nGauSize = 31;
	double dGauSig = 2;
	cv::Mat matGauBuf = cMem->GetMat(matGrayBuf.size(), matGrayBuf.type(), false);
	cv::GaussianBlur(matGrayBuf, matGauBuf, cv::Size(nGauSize, nGauSize), dGauSig, dGauSig);
	//cv::imwrite("D:\\01.Gau.bmp", matGauBuf);

	// Edge
	cv::Mat matEdgeBuf = cMem->GetMat(matGauBuf.size(), matGauBuf.type(), false);
	cv::Sobel(matGauBuf, matEdgeBuf, CV_8UC1, 1, 0);
	//cv::imwrite("D:\\02.Edge.bmp", matEdgeBuf);

	// ���ذ�
	float fAvg = (float)(cv::mean(matEdgeBuf)[0] * 2.75f);

	// ���ι��� ���� ���
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

	// ���� ������ �ʹ� ������ ���� �̻����� �Ǵ�
	if (nLineYCount > 0			&&
		nCount > nLineYCount	)
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}


	//////////////////////////////////////////////////////////////////////////
	// ������ 2���� �Ǿ� ��Ⱑ �ٸ� ���
	//////////////////////////////////////////////////////////////////////////

	matModelBuf = cMem->GetMat(matGrayBuf.size(), matGrayBuf.type(), false);
	cv::blur(matGrayBuf, matModelBuf, cv::Size(5, 5));

	matResizeBuf = cMem->GetMat(cv::Size(matModelBuf.cols/10, matModelBuf.rows/10), matModelBuf.type(), false);
	cv::resize(matModelBuf, matResizeBuf, matResizeBuf.size());

	// �迭 �ʱ�ȭ
	int *nCntX = new int[matResizeBuf.cols];
	int *nCntY = new int[matResizeBuf.rows];
	memset(nCntX, 0, sizeof(int) * matResizeBuf.cols);
	memset(nCntY, 0, sizeof(int) * matResizeBuf.rows);


	// ���ι��� ����
	int nAvgY = 0;
	int nAvgY_Up = 0;
	int nAvgY_Down = 0;
	int nCntY_Up = 0;
	int nCntY_Down = 0;

	// ���ι��� ����
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

	// �޸� �Ҵ� ����
	delete[] nCntX;		nCntX		= NULL;
	delete[] nCntY;		nCntY		= NULL;
	matModelBuf.release();
	matGrayBuf.release();
	matResizeBuf.release();

	// GV ���̰� ū ���
	if( nAvgX_Up - nAvgX_Down	> nBlockGV_X	)
	{
		int nTemp = ( nCntX_Up > nCntX_Down ) ? nCntX_Down : nCntX_Up;

		// ���� ������ 20% �̻��� ���
		if( nTemp/(float)(nCntX_Up + nCntX_Down) > fBlockArea_X )
		{
			dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
			return E_ERROR_CODE_ALIGN_DISPLAY;
		}
	}

	// GV ���̰� ū ���
	if( nAvgY_Up - nAvgY_Down	> nBlockGV_Y	)
	{
		int nTemp = ( nCntY_Up > nCntY_Down ) ? nCntY_Down : nCntY_Up;

		// ���� ������ 20% �̻��� ���
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

// SVI AD �˻� / dResult : ���� Cell ��ġ��
long CInspectAlign::DoFindDefectAD_SVI(cv::Mat matSrcBuf, double* dPara, double* dResult, int nCameraNum, int nRatio)
{
	// ���۰� ���� ���.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// �Ķ���� NULL�� ���.
	if( dPara == NULL )			return E_ERROR_CODE_ALIGN_WARNING_PARA;
	
	//////////////////////////////////////////////////////////////////////////
	// ������ �ƿ� �ȵȰ�� ������
	//////////////////////////////////////////////////////////////////////////

	// Color -> Gray
	cv::Mat matGrayBuf;
	cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);

	// stdDev ���ϱ�
	cv::Scalar m, s;
	cv::meanStdDev(matGrayBuf, m, s);

	// 1 : ��� ��� (Mean GV)
	dResult[1] = double(m[0]);

	// 2 : ǥ������ (Std)
	dResult[2] = double(s[0]);

	// ������ �������� �����Ƿ�.... ( �غ��� 0.xxx ���� ��... )
	// ��ü ���� ��չ�� 1���� ������ ����ȵȰ����� �Ǵ�
	if( dResult[1] < 1.0 )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_OFF;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	double	dRate	= dPara[E_PARA_SVI_AD_RATE];
	double	dMinGV	= dPara[E_PARA_SVI_AD_MIN_GV];
	double	dMaxGV	= dPara[E_PARA_SVI_AD_MAX_GV];
	double	dMaxStd	= dPara[E_PARA_SVI_AD_MAX_STD];

	double	dZoom	= dPara[E_PARA_SVI_AD_ZOOM];
	int		nModelX	= (int)dPara[E_PARA_SVI_CELL_COAX_SIZE_X] * nRatio;
	int		nModelY	= (int)dPara[E_PARA_SVI_CELL_COAX_SIZE_Y] * nRatio;

	// Side ī�޶�
	if( nCameraNum == 1 )
	{
		nModelX	= (int)dPara[E_PARA_SVI_CELL_SIDE_SIZE_X] * nRatio;
		nModelY	= (int)dPara[E_PARA_SVI_CELL_SIDE_SIZE_Y] * nRatio;
	}

	// �Ķ���� �̻��� ���
	if( dZoom < 1 )	return E_ERROR_CODE_ALIGN_WARNING_PARA;

	// ��� ���ϴ� ��� ������
	if( dRate <= 0 )
	{
		dResult[0] = 100.0;
		dResult[1] = 0;
		dResult[2] = 0;
		return E_ERROR_CODE_TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	// Cell ��ġ ã��
	//////////////////////////////////////////////////////////////////////////

	// ��ҵ� �̹��� ũ��
	int nWidth	= matGrayBuf.cols / dZoom;
	int nHeight	= matGrayBuf.rows / dZoom;

	// ���� ũ�� ���
	cv::Mat matResizeBuf;
	cv::resize(matGrayBuf, matResizeBuf, cv::Size(nWidth, nHeight));

	// ���� ��� �� ���� ( Gray Value : 50 )
	cv::Mat matModelBuf = cv::Mat::zeros((int)(nModelY/dZoom), (int)(nModelX/dZoom), CV_8UC1);
	matModelBuf.setTo(50);

	// ��Ī
	cv::Mat matResBuf = cv::Mat::zeros(nHeight, nWidth, CV_32FC1);
	cv::matchTemplate(matResizeBuf, matModelBuf, matResBuf, CV_TM_CCORR_NORMED);

	// ��Ī��
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

	// 0 : ��ġ�� (Rate)
	// �ۼ�Ʈ�� ����
	dResult[0] *= 100.0;

	// �޸� �Ҵ� ����
	matResizeBuf.release();
	matResBuf.release();
	matModelBuf.release();
	matGrayBuf.release();

	// �ִ밪 ��Ī���� ������ ������ ������� ���÷��� �̻�
	if( dResult[0] < dRate )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// ��Ī �� �κ� ��� �˻�
	cv::Rect rect(xx*dZoom, yy*dZoom, nModelX, nModelY);

	// �˻� ������ ���� ũ�⸦ ��� ���
	if( rect.x	<	0								||
		rect.y	<	0								||
		rect.x + rect.width		>= 	matSrcBuf.cols	||
		rect.y + rect.height	>= 	matSrcBuf.rows	)		return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	if( rect.width <= 1 )									return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rect.height <= 1 )									return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// ��Ī�� �κ� ���� �ڸ���
	matGrayBuf = matSrcBuf(rect);

	// stdDev ���ϱ�
	cv::meanStdDev(matGrayBuf, m, s);

	// 1 : ��� ��� (Mean GV)
	dResult[1] = double(m[0]);

	// 2 : ǥ������ (Std)
	dResult[2] = double(s[0]);

	matGrayBuf.release();


	//////////////////////////////////////////////////////////////////////////
	// AD üũ
	//////////////////////////////////////////////////////////////////////////

	// ������ ���Ѱ��, ���÷��� �̻�
	if( dResult[2] > dMaxStd )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// ��Ⱑ ���� ���, ���÷��� �̻�
	if( dResult[1] > dMaxGV )
	{
		dResult[3] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// ��Ⱑ ��ο� ���, ���÷��� �̻�
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

	//������ 95%�̻� Panel�� ���� �ϰ� ������ Display �̻� ó��
	if(size / dImageArea > 0.95)
		return E_ERROR_CODE_FALSE;

	Rect rtObject = boundingRect(contours[0]);

	vector<cv::Rect> pADCheck;
	pADCheck.push_back(Rect(rtObject.x, rtObject.y, rtObject.width/4, rtObject.height));   //���� ����
	pADCheck.push_back(Rect(rtObject.x + rtObject.width*3/4, rtObject.y, rtObject.width/4, rtObject.height)); //������ ����
	pADCheck.push_back(Rect(rtObject.x, rtObject.y, rtObject.width, rtObject.height/4));   //���� ����
	pADCheck.push_back(Rect(rtObject.x, rtObject.y + rtObject.height*3/4, rtObject.width, rtObject.height/4)); //�Ʒ��� ����

	//�Ϻ� ���� �̻� Check
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
	//Pad Edge�κ� Cutting�̻� Ȯ�� 

	//Object �ܰ� �������� ���� Edge Search�� ����
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

	//���� �����ؼ� Top, Bottom, Middle 3���� ����
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

	//�ּ� �ڽ¹����� ������ �������� ���� ���ϱ�
	for(int i=0; i<E_ABNORMAL_PAD_EDGE;i++)
	{
		if(MethodOfLeastSquares(pt[i],dSlope[i])==E_ERROR_CODE_FALSE)
			return E_ERROR_CODE_FALSE;
	}

	//���� ���ϱ�
	double dTheth[E_ABNORMAL_PAD_EDGE];

	for(int i=0; i<E_ABNORMAL_PAD_EDGE;i++)
	{
		dTheth[i] = atan(dSlope[i])*180./PI; 
		dTheth[i]+=90;
	}

	//���� ����
	double dMin = dTheth[E_ABNORMAL_PAD_EDGE_TOTAL] - dCompare_Theta;
	double dMax = dTheth[E_ABNORMAL_PAD_EDGE_TOTAL] + dCompare_Theta;

	//���� ���̱�
	for(int i=0; i<E_ABNORMAL_PAD_EDGE;i++)
	{
		vector<cv::Point>().swap(pt[i]);
	}

	//���� ��
	if((dTheth[E_ABNORMAL_PAD_EDGE_TOP]    < dMin || dTheth[E_ABNORMAL_PAD_EDGE_TOP]    > dMax)
		||(dTheth[E_ABNORMAL_PAD_EDGE_BOTTOM] < dMin || dTheth[E_ABNORMAL_PAD_EDGE_BOTTOM] > dMax)
		||(dTheth[E_ABNORMAL_PAD_EDGE_MIDDLE] < dMin || dTheth[E_ABNORMAL_PAD_EDGE_BOTTOM] > dMax))
		return E_ERROR_CODE_FALSE;

	return E_ERROR_CODE_TRUE;

}

// AVI AD GV �˻� ( 8bit & 12bit )
long CInspectAlign::DoFindDefectAD_GV(cv::Mat& matSrcBuf, double* dPara, double* dResult, cv::Point* ptCorner, CDefectCCD *cCCD)
{
	// ���۰� ���� ���.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// �Ķ���� NULL�� ���.
	if( dPara == NULL )			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	double	dADGVMin	= dPara[E_PARA_CHECK_MIN_GV];
	double	dADGVMax	= dPara[E_PARA_CHECK_MAX_GV];
	double	dADGVAVG	= dPara[E_PARA_CHECK_AVG_GV];
	double	dADGVLAB	= dPara[E_PARA_CHECK_PATTERN_LABEL];

	bool	bAreaFlag	= (dPara[E_PARA_CHECK_AREA_FLAG] > 0) ? true : false;
	int		nAreaGV		= (int)dPara[E_PARA_CHECK_AREA_GV];
	double	dAreaRatio	= dPara[E_PARA_CHECK_AREA_RATIO];

	long	nWidth	= (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuf.rows;	// ���� ���� ũ��

	//////////////////////////////////////////////////////////////////////////

	// ��� ���ϴ� ��� ������
	//if( dUse <= 0 )		return E_ERROR_CODE_TRUE;	// ��� ����

	// �ӽ÷� ���� �۰� ����
	int nOffset = 5;
	CRect rectROI = new CRect(
		max(ptCorner[E_CORNER_LEFT_TOP].x		+ nOffset,	ptCorner[E_CORNER_LEFT_BOTTOM].x	+ nOffset	),
		max(ptCorner[E_CORNER_LEFT_TOP].y		+ nOffset,	ptCorner[E_CORNER_RIGHT_TOP].y		+ nOffset	),
		min(ptCorner[E_CORNER_RIGHT_TOP].x		- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].x	- nOffset	),
		min(ptCorner[E_CORNER_LEFT_BOTTOM].y	- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	- nOffset	) );

	// �˻� ������ ���� ũ�⸦ ��� ���
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_DISPLAY;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_DISPLAY;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_DISPLAY;

	int nROIWidth	= rectROI.Width();
	int nROIHeight	= rectROI.Height();

	// Image Buff �����
	cv::Mat matSrcCopyBuf	= cMem->GetMat(matSrcBuf.size(), matSrcBuf.type(), false);
	cv::Mat matSubBuf		= cMem->GetMat(nROIHeight, nROIWidth, matSrcBuf.type(), false);
	
	// ���� ���� �޾� ����
	matSrcBuf.copyTo(matSrcCopyBuf);

	cv::Mat matMeanBuf = matSrcCopyBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );

	// ���� ���� ��� ���ϱ�
	cv::Scalar m = cv::mean( matMeanBuf );	

	// ��� ���
	dResult[0] = m[0];

	// ��Ⱑ ���� ���, ���÷��� �̻�
	if( dResult[0] > dADGVMax )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// ��Ⱑ ��ο� ���, ���÷��� �̻�
	if( dResult[0] < dADGVMin )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	//////////////////////////////////////////////////////////////////////////

	// Black ���Ͽ��� ����� ��� Point �˰��� �ʹ� ���� �ɸ�
	// Black ���� ����� ���� ������ AD ����
	// 18.03.14 PNZ Black Pattern ���� ���� AD Check ����
	// ����: ���� GV �̻� Pixel ���� Count
	// ����: 1GV���� �� Pattern�� �쿰�쿰 ����
	// ����: ���� ���� 2��ȭ �Ͽ� ��â�ϰ� �����ϴ� ���� ���ϱ�, �� ��ü���� 15% �������� ���� ��
	if( bAreaFlag )
	{
		__int64 nCount = 0;

		// ���� ���� ���� Mean, StdDev�� ���Ѵ�, Line�� ���� ���� �� ���� ���� ��踦 ����
		cv::Scalar m, s;
		cv::meanStdDev( matMeanBuf, m, s);

		double dMeanGV	= m[0];
		double dStdGV	= s[0];

		__int64 nTotalLabel = 0;

		// ���� 8U�� ���
		if(( matMeanBuf.type() == CV_8U )&& (dMeanGV > 0.8 || dStdGV > 1.0))
		{
			MatIterator_<uchar> itSrc, endSrc;
			itSrc	= matMeanBuf.begin<uchar>();
			endSrc	= matMeanBuf.end<uchar>();

			// ������ GV���� ���� Pixel ī��Ʈ
			for ( ; itSrc != endSrc; itSrc++ )
				( *itSrc > nAreaGV) ? nCount++ : NULL ;
		}

		// ���ǿ� �����ϸ� 
		else if ( matMeanBuf.type() == CV_8U && dMeanGV <= 0.8 && dStdGV <= 1.0)
		{
			int nPS = 2;

			if ( cCCD != NULL )
			{
				// CCD �ҷ� ��ġ ����
				// KSW 17.07.08 Black pattern�� ����ϹǷ�, Black Pattern������ ���
				// KSW 17.07.11 ���� ���� �ٷ� ����
				// PS��� �� 1pixel ���� �����Ƿ�, �ֺ� 1pixel �� ����
				cCCD->OffsetDefectCCD(matSrcCopyBuf, 1, nPS);

				// CCD �ҷ� ��ġ ����
				// KSW 17.07.08 Black pattern�� ����ϹǷ�, Black Pattern������ ���
				// KSW 17.07.11 ���� ���� �ٷ� ����
				// PS��� �� 1pixel ���� �����Ƿ�, �ֺ� 1pixel �� ����
				cCCD->DeleteDefectCCD(matSrcCopyBuf, 1, nPS);
			}

			//2��ȭ ��
			cv::threshold(matMeanBuf, matSubBuf, 1.0, 255.0, THRESH_BINARY);
		
			// Labeling �� Buff
			cv::Mat matLabel		= cMem->GetMat(nROIHeight, nROIWidth, CV_32SC1, false);
			//cv::Mat matStats		= cMem->GetMat(nROIHeight, nROIWidth, matSrcBuf.type());
			//cv::Mat matCentroid		= cMem->GetMat(nROIHeight, nROIWidth, matSrcBuf.type());

			//nTotalLabel = cv::connectedComponentsWithStats(matMeanBuf, matLabel, matStats, matCentroid, 8, CV_32S) - 1;
			nTotalLabel = cv::connectedComponents(matSubBuf, matLabel, 8, CV_32S, CCL_GRANA) - 1;
		}
		// ���� 16U�� ���
		else
		{
			MatIterator_<ushort> itSrc, endSrc;
			itSrc	= matMeanBuf.begin<ushort>();
			endSrc	= matMeanBuf.end<ushort>();

			// ������ GV���� ���� Pixel ī��Ʈ
			for ( ; itSrc != endSrc; itSrc++ )
				( *itSrc > nAreaGV) ? nCount++ : NULL ;
		}
		
		// �����ϴ� ����
 		double dRatio = nCount / (double)(matMeanBuf.rows * matMeanBuf.cols);

		// ������ ������ ���� ���
		if( dRatio > dAreaRatio )
		{
			dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
			return E_ERROR_CODE_ALIGN_DISPLAY;
		}

		// ������ ������ ���� ���
		if( nTotalLabel > dADGVLAB )
		{
			dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
			dResult[2] = (double)nTotalLabel;
			return E_ERROR_CODE_ALIGN_DISPLAY;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	// ���ϴ� ���� ���� ����
	if( dADGVAVG > 0 )
		cv::multiply(matSrcBuf, dADGVAVG / dResult[0], matSrcBuf);

	matMeanBuf.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}


//LJH Dust 4���� ���
long CInspectAlign::DoFindDefectAD_GV_DUST(cv::Mat& matSrcBuf, double* dPara, double* dResult, cv::Point* ptCorner)
{
	// ���۰� ���� ���.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// �Ķ���� NULL�� ���.
	if( dPara == NULL )			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	double	dADGVMin	= dPara[E_PARA_CHECK_MIN_GV];
	double	dADGVMax	= dPara[E_PARA_CHECK_MAX_GV];
	double	dADGVAVG	= dPara[E_PARA_CHECK_AVG_GV];

	bool	bAreaFlag	= (dPara[E_PARA_CHECK_AREA_FLAG] > 0) ? true : false;
	int		nAreaGV		= (int)dPara[E_PARA_CHECK_AREA_GV];
	double	dAreaRatio	= dPara[E_PARA_CHECK_AREA_RATIO];

	long	nWidth	= (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuf.rows;	// ���� ���� ũ��

	//////////////////////////////////////////////////////////////////////////

	// ��� ���ϴ� ��� ������
	//if( dUse <= 0 )		return E_ERROR_CODE_TRUE;	// ��� ����

	// �ӽ÷� ���� �۰� ����
	int nOffset = 5;
	CRect rectROI = new CRect(
		max(ptCorner[E_CORNER_LEFT_TOP].x		+ nOffset,	ptCorner[E_CORNER_LEFT_BOTTOM].x	+ nOffset	),
		max(ptCorner[E_CORNER_LEFT_TOP].y		+ nOffset,	ptCorner[E_CORNER_RIGHT_TOP].y		+ nOffset	),
		min(ptCorner[E_CORNER_RIGHT_TOP].x		- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].x	- nOffset	),
		min(ptCorner[E_CORNER_LEFT_BOTTOM].y	- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	- nOffset	) );

	// �˻� ������ ���� ũ�⸦ ��� ���
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_DISPLAY;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_DISPLAY;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_DISPLAY;


	//LJH ���� ���� 4��� ��� ���ϱ�
	int halfWidth	= rectROI.Width() / 2;
	int halfHeight	= rectROI.Height() / 2;

	cv::Mat matMeanBuf1 = matSrcBuf( cv::Rect(rectROI.left				, rectROI.top				, halfWidth, halfHeight) ); //�»�� ����
	cv::Mat matMeanBuf2 = matSrcBuf( cv::Rect(rectROI.left + halfWidth	, rectROI.top				, halfWidth, halfHeight) ); //���� ����
	cv::Mat matMeanBuf3 = matSrcBuf( cv::Rect(rectROI.left				, rectROI.top +halfHeight	, halfWidth, halfHeight) ); //���ϴ� ����
	cv::Mat matMeanBuf4 = matSrcBuf( cv::Rect(rectROI.left + halfWidth	, rectROI.top +halfHeight	, halfWidth, halfHeight) ); //���ϴ� ����

	cv::Scalar m1 = cv::mean( matMeanBuf1 );	//�»�� ���� ���
	cv::Scalar m2 = cv::mean( matMeanBuf2 );	//���� ���� ���
	cv::Scalar m3 = cv::mean( matMeanBuf3 );	//���ϴ� ���� ���
	cv::Scalar m4 = cv::mean( matMeanBuf4 );	//���ϴ� ���� ���

	// ���� ���� ��� ȹ�� ��, ���� ����
	matMeanBuf1.release();
	matMeanBuf2.release();
	matMeanBuf3.release();
	matMeanBuf4.release();

	// ��Ⱑ ���� ���, ���÷��� �̻�
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

	// ��Ⱑ ��ο� ���, ���÷��� �̻�
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

	// ��� ���ϱ�
	// LJH �� 4��� ���� ��� �̻� ���� �� ��ü ���� ��� ���� ����
	cv::Mat matMeanBuf = matSrcBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Scalar m = cv::mean( matMeanBuf );	

	// ��� ���
	dResult[0] = m[0];

	// ��Ⱑ ���� ���, ���÷��� �̻�
	if( dResult[0] > dADGVMax )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// ��Ⱑ ��ο� ���, ���÷��� �̻�
	if( dResult[0] < dADGVMin )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	//////////////////////////////////////////////////////////////////////////

	// Black ���Ͽ��� ����� ��� Point �˰��� �ʹ� ���� �ɸ�
	// Black ���� ����� ���� ������ AD ����
	if( bAreaFlag )
	{
		__int64 nCount = 0;

		// ���� 8U�� ���
		if( matMeanBuf.type() == CV_8U )
		{
			MatIterator_<uchar> itSrc, endSrc;
			itSrc	= matMeanBuf.begin<uchar>();
			endSrc	= matMeanBuf.end<uchar>();

			// ������ GV���� ���� Pixel ī��Ʈ
			for ( ; itSrc != endSrc; itSrc++ )
				( *itSrc > nAreaGV) ? nCount++ : NULL ;
		}
		// ���� 16U�� ���
		else
		{
			MatIterator_<ushort> itSrc, endSrc;
			itSrc	= matMeanBuf.begin<ushort>();
			endSrc	= matMeanBuf.end<ushort>();

			// ������ GV���� ���� Pixel ī��Ʈ
			for ( ; itSrc != endSrc; itSrc++ )
				( *itSrc > nAreaGV) ? nCount++ : NULL ;
		}
		
		// �����ϴ� ����
		double dRatio = nCount / (double)(matMeanBuf.rows * matMeanBuf.cols);

		// ������ ������ ���� ���
		if( dRatio > dAreaRatio )
		{
			dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
			return E_ERROR_CODE_ALIGN_DISPLAY;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	// ���ϴ� ���� ���� ����
	if( dADGVAVG > 0 )
		cv::multiply(matSrcBuf, dADGVAVG / dResult[0], matSrcBuf);

	matMeanBuf.release();

	return E_ERROR_CODE_TRUE;
}

// SVI AD GV �˻�
long CInspectAlign::DoFindDefectAD_GV_SVI(cv::Mat& matSrcBuf, double* dPara, double* dResult, cv::Point* ptCorner)
{
	// ���۰� ���� ���.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// �Ķ���� NULL�� ���.
	if( dPara == NULL )			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	//////////////////////////////////////////////////////////////////////////
	// �Ķ����
	//////////////////////////////////////////////////////////////////////////

	double	dADGVMin	= dPara[E_PARA_SVI_CHECK_MIN_GV];
	double	dADGVMax	= dPara[E_PARA_SVI_CHECK_MAX_GV];

	long	nWidth	= (long)matSrcBuf.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuf.rows;	// ���� ���� ũ��

	//////////////////////////////////////////////////////////////////////////

	// �ӽ÷� ���� �۰� ����
	int nOffset = 5;
	CRect rectROI = new CRect(
		max(ptCorner[E_CORNER_LEFT_TOP].x		+ nOffset,	ptCorner[E_CORNER_LEFT_BOTTOM].x	+ nOffset	),
		max(ptCorner[E_CORNER_LEFT_TOP].y		+ nOffset,	ptCorner[E_CORNER_RIGHT_TOP].y		+ nOffset	),
		min(ptCorner[E_CORNER_RIGHT_TOP].x		- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].x	- nOffset	),
		min(ptCorner[E_CORNER_LEFT_BOTTOM].y	- nOffset,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	- nOffset	) );

	// �˻� ������ ���� ũ�⸦ ��� ���
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ALIGN_DISPLAY;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ALIGN_DISPLAY;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ALIGN_DISPLAY;

	// ��� ���ϱ�
	cv::Mat matMeanBuf = matSrcBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Scalar m = cv::mean( matMeanBuf );
	matMeanBuf.release();

	// ��� ���
	dResult[0] = (m[0] + m[1] + m[2] ) / 3;

	// ��Ⱑ ���� ���, ���÷��� �̻�
	if( dResult[0] > dADGVMax )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	// ��Ⱑ ��ο� ���, ���÷��� �̻�
	if( dResult[0] < dADGVMin )
	{
		dResult[1] = E_DEFECT_JUDGEMENT_DISPLAY_DARK;
		return E_ERROR_CODE_ALIGN_DISPLAY;
	}

	return E_ERROR_CODE_TRUE;
}


// Cell ���� ã��
long CInspectAlign::FindCellArea(cv::Mat matThreshBuf, int nMinArea, cv::Rect& rectCell)
{
	// ���۰� ���� ���.
	if( matThreshBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;	

	// ���� �Ҵ� & �ʱ�ȭ
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	// �޸� �Ҵ�
	cv::Mat matLabel, matStats, matCentroid;
	matLabel = cMatBufTemp.GetMat(matThreshBuf.size(), CV_32SC1);

	// Blob ����
	__int64 nTotalLabel = cv::connectedComponentsWithStats(matThreshBuf, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

	// ���� ���� ũ�� �̻� Blob ����
	int nLabelCount = 0;

	// Blob ����
	for( int idx=1 ; idx<=nTotalLabel ; idx++ )
	{
		// ��ü ����
		long nArea = matStats.at<int>(idx, CC_STAT_AREA);

		// Blob ���� Rect
		cv::Rect rectTemp;
		rectTemp.x			= matStats.at<int>(idx, CC_STAT_LEFT);
		rectTemp.y			= matStats.at<int>(idx, CC_STAT_TOP);
		rectTemp.width		= matStats.at<int>(idx, CC_STAT_WIDTH);
		rectTemp.height		= matStats.at<int>(idx, CC_STAT_HEIGHT);

		// ���� ����
		if( nArea <= nMinArea )
		{
			// 0GV �� �ʱ�ȭ ��, �Ѿ��
			cv::Mat matTempROI	= matThreshBuf( rectTemp );
			cv::Mat matLabelROI	= matLabel( rectTemp );

			for(int y=0 ; y<rectTemp.height ; y++)
			{
				int		*ptrLabel	= (int *)matLabelROI.ptr(y);
				uchar	*ptrGray	= (uchar *)matTempROI.ptr(y);

				for(int x=0 ; x<rectTemp.width ; x++, ptrLabel++, ptrGray++)
				{
					// �ش� ��ü
					if( *ptrLabel == idx )	*ptrGray = 0;
				}
			}

			continue;
		}

		// Cell ���� Rect
		rectCell = rectTemp;

		// ���� ���� ũ�� �̻� Blob ����
		nLabelCount++;
	}

	// �޸� �Ҵ� ����
	matLabel.release();
	matStats.release();
	matCentroid.release();

	// ������ 1���� �ƴ� ���
	if( nLabelCount != 1 )	return E_ERROR_CODE_ALIGN_NO_FIND_CELL;

	// KSW 18.04.11 - ��� �ٲ�
	/************************************************************************
	// ���۰� ���� ���.
	if( matThreshBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	CMatBuf cMatBufTemp;

	// ���� �Ҵ� & �ʱ�ȭ
	cMatBufTemp.SetMem(cMem);
	cv::Mat matLabelBuffer = cMatBufTemp.GetMat(matThreshBuf.size(), CV_32SC1);

	matThreshBuf.convertTo(matLabelBuffer, CV_32SC1);
	matThreshBuf.setTo(0);

	// starts at 2 because 0,1 are used already
	int nLabelCount = 2;

	// �˻� ���� Rect
	cv::Rect rectTemp;
	int nTempCount = 0;

	for(int y=0 ; y<matLabelBuffer.rows ; y++)
	{
		int *row = (int*)matLabelBuffer.ptr(y);

		for(int x=0 ; x<matLabelBuffer.cols ; x++)
		{
			if(row[x] != 255)	continue;

			nTempCount++;

			// ��ü ����
			long nArea = cv::floodFill(matLabelBuffer, cv::Point(x, y), nTempCount, &rectTemp);

			// ���� ����
			if( nArea <  nMinArea )	continue;

			// Cell ���� Rect
			rectCell = rectTemp;

			int nEndXX = rectTemp.x+rectTemp.width;
			int nEndYY = rectTemp.y+rectTemp.height;

			// ����ó��
			if( nEndYY >= matLabelBuffer.rows )	nEndYY = matLabelBuffer.rows - 1;
			if( nEndXX >= matLabelBuffer.cols )	nEndXX = matLabelBuffer.cols - 1;

			// �ش� �� ���� �ȼ� ��ǥ
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

	// ������ 1���� �ƴ� ���. ( nLabelCount 2���� ���� )
	if( nLabelCount != 3 )	return E_ERROR_CODE_ALIGN_NO_FIND_CELL;
	************************************************************************/

	return E_ERROR_CODE_TRUE;
}


// �ߺ� ������ ��
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

// �����Ϳ��� �ߺ����� �ʰ� N���� ������ ������ ���
long CInspectAlign::GetRandomSamples(vector <cv::Point2i>& ptSrcIndexs, vector <cv::Point2i>& ptSamples, int nSampleCount)
{
	// ���� ũ��
	int nSize = (int)ptSrcIndexs.size();

	// ������ N�� ��ŭ
	while ( ptSamples.size() < nSampleCount )
	{
		// �������� ������ ����
		int j = rand() % nSize;

		// �ߺ� ���� �ʵ���
		if ( !FindInSamples(ptSamples, ptSrcIndexs[j]) )
		{
			// ��ǥ ���� �߰�
			ptSamples.push_back( ptSrcIndexs[j] );
		}
	}

	return E_ERROR_CODE_TRUE;
}

// ���� ���� ���
long CInspectAlign::calcLineFit(vector <cv::Point2i>& ptSamples, long double& dA, long double& dB)
{
	// ������ ���� ���� ���
	if( ptSamples.size() <= 0 )	return E_ERROR_CODE_ALIGN_NO_DATA;

	// PCA ������� ���� ���� �Ķ���͸� ����
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

	//������ ������: sin(theta)*(x - sx) = cos(theta)*(y - sy);
	//dA = sin(theta) / cos(theta);
	dA = tan(theta);
	dB = sy - dA * sx;

	return E_ERROR_CODE_TRUE;
}

// ��ȿ�� ������ ���� �ľ�
long CInspectAlign::calcLineVerification(vector <cv::Point2i>& ptSrcIndexs, vector <cv::Point2i>& ptInliers, long double& dA, long double& dB, double distThreshold)
{
	for(int i=0 ; i<ptSrcIndexs.size() ; i++)
	{
		// ������ ���� ������ ���̸� ���
		// �� ��(x)�κ��� ����(line)�� ���� ������ ����(distance)�� ���
		// | ax + bx + c | / sqrt( a*a + b*b )
		double distance = abs( ptSrcIndexs[i].x * dA - ptSrcIndexs[i].y + dB ) / sqrt( dA * dA  + 1. );

		// ������ �𵨿��� ��ȿ�� �������� ���, ��ȿ�� ������ �߰�
		if ( distance < distThreshold )
			ptInliers.push_back( ptSrcIndexs[i] );
	}	

	return (long)ptInliers.size();
}

// RANSAC �˰���
long CInspectAlign::calcRANSAC(vector <cv::Point2i>& ptSrcIndexs, long double& dA, long double& dB, int nMinSamples, double distThreshold)
{
	// ������ ���� ��� ��� ����
	// 2���� �׳� ���������� ���ϸ� ��
	// ���� �������� ����� ��
	if( nMinSamples < 2 )					return E_ERROR_CODE_ALIGN_WARNING_PARA;
	if( ptSrcIndexs.size() <= nMinSamples )	return E_ERROR_CODE_ALIGN_WARNING_PARA;

	long nMaxCost = 0;

	// ���� �ʱ�ȭ
	vector <cv::Point2i> ptSamples, ptInliers;

	// �ӽ� ���� ������ y = ax + b
	long double dAA = 0, dBB = 0;

	// �ݺ� Ƚ�� ���ϱ�
	int nMaxIter = (int)(1 + log(1. - 0.99)/log(1. - pow(0.5, nMinSamples)));

	for (int i = 0; i<nMaxIter; i++)
	{
		// ���� �ʱ�ȭ
		vector<cv::Point2i>().swap(ptSamples);
		vector<cv::Point2i>().swap(ptInliers);

		// hypothesis
		// ���� �����Ϳ��� ���Ƿ� N���� ���� �����͸� ���
		GetRandomSamples(ptSrcIndexs, ptSamples, nMinSamples);

		// ���� �����͸� �������� �����ͷ� ���� �� �Ķ���� ����
		long nErrorCode = calcLineFit(ptSamples, dAA, dBB);

		// ������ ���, ���� �ڵ� ���
		if( nErrorCode != E_ERROR_CODE_TRUE )
		{
			vector<cv::Point2i>().swap(ptSamples);
			vector<cv::Point2i>().swap(ptInliers);

			return nErrorCode;
		}

		// Verification
		// ���� �����Ͱ� ������ �𵨿� �� �´��� �˻�
		long cost = calcLineVerification(ptSrcIndexs, ptInliers, dAA, dBB, distThreshold);

		// ���� ������ ���� �� �´´ٸ�, ��ȿ�� �����ͷ� ���ο� ���� ����
		if( nMaxCost < cost )
		{
			nMaxCost = cost;
			calcLineFit(ptInliers, dA, dB);
		}

		// ���� ������ ����
		vector<cv::Point2i>().swap(ptInliers);
	}

	// ���� ������ ����
	vector<cv::Point2i>().swap(ptSamples);

	// ��� �Ұ�
	if( nMaxCost <= 0 )		return E_ERROR_CODE_ALIGN_CAN_NOT_CALC;

	// ���� 50�� �̻� ����ϱ�
	//if( nMaxCost < 50 )	return E_ERROR_CODE_ALIGN_CAN_NOT_CALC;

	return E_ERROR_CODE_TRUE;
}

// ���⿡ ���� ������ ȹ��
long CInspectAlign::RobustFitLine(cv::Mat& matTempBuf, cv::Rect rectCell, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType, int nSamp)
{
	// ���� ũ��
	int nW = matTempBuf.cols;
	int nH = matTempBuf.rows;

	// ���� ó��
	if( rectCell.x <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.x >=	nW	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.y <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.y >=	nH	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// ���� ó��
	if( rectCell.x + rectCell.width <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.x + rectCell.width >=	nW	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.y + rectCell.height <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
	if( rectCell.y + rectCell.height >=	nH	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// ���� ����
	int nSrartX		= rectCell.x + rectCell.width / 4;	//  ���� 1/4 ����
	int nEndX		= nSrartX + rectCell.width / 2;		//  ���� 3/4 ����

	int nStartY		= rectCell.y + rectCell.height / 4;	//  ���� 1/4 ����
	int nEndY		= nStartY + rectCell.height / 2;	//  ���� 3/4 ����

	int x, y;

	vector<cv::Point2i>	ptSrcIndexs;
	vector<cv::Point2i>().swap(ptSrcIndexs);

	// ���� ������ ���� ����
	switch(nType)
	{
	case E_ALIGN_TYPE_LEFT:
		{
			for(y=nStartY ; y<=nEndY ; y+=nSamp)
			{
				for(x=rectCell.x ; x<=nEndX ; x++)
				{
					// ���� �ִ� ���
					if( matTempBuf.at<uchar>(y, x) )
					{
						// ��ǥ ���� �߰�
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
					// ���� �ִ� ���
					if( matTempBuf.at<uchar>(y, x) )
					{
						// ��ǥ ���� �߰�
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
					// ���� �ִ� ���
					if( matTempBuf.at<uchar>(y, x) )	
					{
						// ��ǥ ���� �߰�
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
					// ���� �ִ� ���
					if( matTempBuf.at<uchar>(y, x) )	
					{
						// ��ǥ ���� �߰�
						ptSrcIndexs.push_back( cv::Point2i(x, y) );

						break;
					}
				}
			}
		}		
		break;

		// �Ķ���� �̻��� ���
	default:
		{
			return E_ERROR_CODE_ALIGN_WARNING_PARA;
		}
		break;
	}

	// ����� �����ͷ� ���� ã��
	long nErrorCode = calcRANSAC(ptSrcIndexs, dA, dB, nMinSamples, distThreshold);

	// ���� ������ ����
	vector<cv::Point2i>().swap(ptSrcIndexs);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	return E_ERROR_CODE_TRUE;
}

// ����ȭ �ȿ��󿡼� ��ü�� ���ʿ��� �ٱ����� ��ǥŽ��
long CInspectAlign::ObjectInAreaGetLine(cv::Mat& matTempBuf, cv::Rect rectImgSize, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType)
{
	// ���� ũ��
	int nW = matTempBuf.cols;
	int nH = matTempBuf.rows;
	uchar* ucImagedata = matTempBuf.data;


	// ���� ����
	int nStartX		= 0;								//�ʱ�ȭ
	int nEndX		= 0;								
	int nStartY		= 0;				
	int nEndY		= 0;								

	vector<cv::Point2i>	ptSrcIndexs;//��ǥ ��� ��
	vector<cv::Point2i>().swap(ptSrcIndexs);	
		int nX=0,nY=0,nSample=50;
	int nStPtOffset=100;			//Panel�� ���ʿ��� �ܰ����� Search�Ҷ� �ܰ������� ���� ���� ���������� Offset��
	uchar* p=0;						//���� Data Pointer

	switch(nType)
	{
	case E_ALIGN_TYPE_LEFT:
		nStartX = rectImgSize.width/2;							//������ ���� �߾�
		nEndX	= 0;											//������ ���� ������
		nStartY = rectImgSize.height/3;							//������ ���� 1/3 ����
		nEndY	= rectImgSize.height*2/3;						//Panel�� ���� 3/4����

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
		nStartX = rectImgSize.width/3;							//������ ���� 1/3����
		nEndX	= rectImgSize.width*2/3;						//������ ���� 2/3����
		nStartY = rectImgSize.height/2;							//������ ���� �߾�
		nEndY   = 0;											//������ ���� ������

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
		nStartX = rectImgSize.width/2;						//������ ���� �߾�
		nEndX	= matTempBuf.cols;							//������ ���� ����
		nStartY = rectImgSize.height/3;						//������ ���� 1/3����
		nEndY	= rectImgSize.height/3*2;					//������ ���� 2/3����
				
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
		nStartX = rectImgSize.width/3;							//������ ���� 1/3����
		nEndX	= rectImgSize.width/3*2;						//������ ���� 2/3����
		nStartY = rectImgSize.height/2;							//������ ���� �߾�
		nEndY	= matTempBuf.rows;								//������ ���� ����

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


	// ����� �����ͷ� ���� ã��
	long nErrorCode = calcRANSAC(ptSrcIndexs, dA, dB, nMinSamples, distThreshold);

	// ���� ������ ����
	vector<cv::Point2i>().swap(ptSrcIndexs);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	return E_ERROR_CODE_TRUE;
}

// ���� ū Blob ��ȯ APP
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
	// ���� ũ��
	int nW = matTempBuf.cols;
	int nH = matTempBuf.rows;

	// ���� ó��
// 	if( rectCell.x <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.x >=	nW	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.y <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.y >=	nH	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// ���� ó��
// 	if( rectCell.x + rectCell.width <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.x + rectCell.width >=	nW	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.y + rectCell.height <	0	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;
// 	if( rectCell.y + rectCell.height >=	nH	)	return E_ERROR_CODE_ALIGN_IMAGE_OVER;

	// ���� ����
	int nStartX		= rectCell.x + rectCell.width / 4;	//  ���� 1/4 ����
	int nEndX		= nStartX + rectCell.width / 4 * 2;		//  ���� 3/4 ����

	int nStartY		= rectCell.y + rectCell.height / 4;	//  ���� 1/4 ����
	int nEndY		= nStartY + rectCell.height / 4 * 2;	//  ���� 3/4 ����

	int nSamp	= 50;								//  50 Pixel �� ���ø�

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

	// ����� �����ͷ� ���� ã��
	long nErrorCode = calcRANSAC(pt, dA, dB, nMinSamples, distThreshold);

	// ���� ������ ����
	vector<cv::Point2i>().swap(pt);

	// ������ ���, ���� �ڵ� ���
	if( nErrorCode != E_ERROR_CODE_TRUE )
		return nErrorCode;

	return E_ERROR_CODE_TRUE;
}

// 4���� ������ ��ġ ã��
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

// ���� ���� �ܰ��κ� �����
long CInspectAlign::FindEdgeArea(cv::Mat matSrcBuf, cv::Mat& matDstBuf, int nLength, CMatBuf* cMemSub)
{
	// �Ķ���� NULL�� ���
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	int nSize = nLength * 2 + 1;
	cv::Mat matTempBuf = cMatBufTemp.GetMat(matSrcBuf.size(), matSrcBuf.type(), false);

	// �ܰ� ��հ� ä��� ����ũ
	cv::blur(matSrcBuf, matTempBuf, cv::Size(nSize, nSize));
	cv::subtract(matSrcBuf, matTempBuf, matDstBuf);
	cv::threshold(matDstBuf, matDstBuf, 0, 255, THRESH_BINARY);

 	matTempBuf.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}
	// KSW 18.03.28 - �ð� �����ɷ��� ����
	/************************************************************************
	cv::Mat matDistBuf1 = cMatBufTemp.GetMat(matSrcBuf.size(), CV_32FC1);
	cv::Mat matDistBuf2 = cMatBufTemp.GetMat(matSrcBuf.size(), CV_32FC1);
	cv::Mat matDistBuf3 = cMatBufTemp.GetMat(matSrcBuf.size(), CV_8UC1);

	cv::distanceTransform(matSrcBuf, matDistBuf1, CV_DIST_L2, 3);

	cv::threshold(matDistBuf1, matDistBuf2, nLength, 255.0, THRESH_BINARY);

	// bit ��ȯ
	matDistBuf2.convertTo(matDistBuf3, matDistBuf3.type());

	// �ֿܰ� 70Pixel ���� �����
	cv::bitwise_xor(matDistBuf3, matSrcBuf, matDstBuf);

	matDistBuf1.release();
	matDistBuf2.release();
	matDistBuf3.release();
	************************************************************************/

	return E_ERROR_CODE_TRUE;
}


// ���� ���� �ܰ��κ� �����
long CInspectAlign::FindEdgeArea_SVI(cv::Mat matSrcBuf, cv::Mat& matDstBuf, int nLength)
{
	// �Ķ���� NULL�� ���
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	int nSize = nLength * 2 + 1;
	cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, cv::Size(nSize, nSize), Point(nLength, nLength) );

	cv::Mat matTempBuf = cv::Mat::zeros(matSrcBuf.size(), matSrcBuf.type());	

	cv::morphologyEx(matSrcBuf, matTempBuf, MORPH_ERODE, StructElem);

	cv::subtract(matSrcBuf, matTempBuf, matDstBuf);

	StructElem.release();
	matTempBuf.release();


	// KSW 18.03.28 - �ð� �����ɷ��� ����
	/************************************************************************
	cv::Mat matDistBuf1, matDistBuf2, matDistBuf3;

	cv::distanceTransform(matSrcBuf, matDistBuf1, CV_DIST_L2, 3);

	cv::threshold(matDistBuf1, matDistBuf2, nLength, 255.0, THRESH_BINARY);

	// bit ��ȯ
	matDistBuf2.convertTo(matDistBuf3, CV_8UC1);

	// �ֿܰ� �����
	cv::bitwise_xor(matDistBuf3, matSrcBuf, matDstBuf);

	matDistBuf1.release();
	matDistBuf2.release();
	matDistBuf3.release();
	************************************************************************/

	return E_ERROR_CODE_TRUE;
}


// ���� ���� ��� ä���
long CInspectAlign::FillAreaMeanX(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV)
{
	// �Ķ���� NULL�� ���
	if( matMeanBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matEdgeBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// Active ������ �˻�
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	for (int y=0 ; y<2 ; y++)
	{
		long nStart_Y, nEnd_Y;

		// y ���� ���
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

			// x ���� ���
			nStart_X = x * nSegX + rectROI.left;
			if( x==nRangeX-1)		nEnd_X = rectROI.right;
			else					nEnd_X = nStart_X + nSegX;

			// ���� ����
			cv::Rect rectTemp;
			rectTemp.x		= nStart_X;
			rectTemp.y		= nStart_Y;
			rectTemp.width	= nSegX;
			rectTemp.height	= nSegY;

			// ���� ����� ��� ����ó��
			if( rectTemp.x < 0 )	rectTemp.x = 0;
			if( rectTemp.y < 0 )	rectTemp.y = 0;
			if( rectTemp.x + rectTemp.width  >= matMeanBuf.cols )	rectTemp.width  = matMeanBuf.cols - rectTemp.x - 1;
			if( rectTemp.y + rectTemp.height >= matMeanBuf.rows )	rectTemp.height = matMeanBuf.rows - rectTemp.y - 1;

			// ���� �̻��� ���, ����ó��
			if( rectTemp.width  <= 0 )	continue;
			if( rectTemp.height <= 0 )	continue;

			// ���� ROI
			cv::Mat matTempROIBuf = matMeanBuf( rectTemp );

			// ����ũ ROI
			cv::Mat matMaskROIBuf = matEdgeBuf( rectTemp );

			// ��� ���ϱ�
			cv::Scalar m = cv::mean( matTempROIBuf, matMaskROIBuf );

			// nMinGV ���� ū ��츸 ��� ä���
			// ���� �����Ͽ� ��� ���� ��, ���� ������ �Ȱɸ��� ��� ���� ( �𼭸� �κ� )
			// ���� ���� �־�����, 0������ ����� ��� ����
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

// ���� ���� ��� ä���
long CInspectAlign::FillAreaMeanX_SVI(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV)
{
	// �Ķ���� NULL�� ���
	if( matMeanBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matEdgeBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// Active ������ �˻�
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	for (int y=0 ; y<2 ; y++)
	{
		long nStart_Y, nEnd_Y;

		// y ���� ���
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

			// x ���� ���
			nStart_X = x * nSegX + rectROI.left;
			if( x==nRangeX-1)		nEnd_X = rectROI.right;
			else					nEnd_X = nStart_X + nSegX;

			// ���� ����
			cv::Rect rectTemp;
			rectTemp.x		= nStart_X;
			rectTemp.y		= nStart_Y;
			rectTemp.width	= nSegX;
			rectTemp.height	= nSegY;

			// ���� ����� ��� ����ó��
			if( rectTemp.x < 0 )	rectTemp.x = 0;
			if( rectTemp.y < 0 )	rectTemp.y = 0;
			if( rectTemp.x + rectTemp.width  >= matMeanBuf.cols )	rectTemp.width  = matMeanBuf.cols - rectTemp.x - 1;
			if( rectTemp.y + rectTemp.height >= matMeanBuf.rows )	rectTemp.height = matMeanBuf.rows - rectTemp.y - 1;

			// ���� �̻��� ���, ����ó��
			if( rectTemp.width  <= 0 )	continue;
			if( rectTemp.height <= 0 )	continue;

			// ���� ROI
			cv::Mat matTempROIBuf = matMeanBuf( rectTemp );

			// ����ũ ROI
			cv::Mat matMaskROIBuf = matEdgeBuf( rectTemp );

			// ��� ���ϱ�
			cv::Scalar m = cv::mean( matTempROIBuf, matMaskROIBuf );

			// nMinGV ���� ū ��츸 ��� ä���
			// ���� �����Ͽ� ��� ���� ��, ���� ������ �Ȱɸ��� ��� ���� ( �𼭸� �κ� )
			// ���� ���� �־�����, 0������ ����� ��� ����
			if( (m[0] + m[1] + m[2])/3 > nMinGV )
			{
				// ��� ä���
				matTempROIBuf.setTo( m );

				// ����ũ ä���
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

// ���� ���� ��� ä���
long CInspectAlign::FillAreaMeanY(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV)
{
	// �Ķ���� NULL�� ���
	if( matMeanBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matEdgeBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// Active ������ �˻�
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	for (int x=0 ; x<2 ; x++)
	{
		long nStart_X, nEnd_X;

		// x ���� ���
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

			// y ���� ���
			nStart_Y = y * nSegY + rectROI.top;
			if( y==nRangeY-1)		nEnd_Y = rectROI.bottom;
			else					nEnd_Y = nStart_Y + nSegY;

			// ���� ����
			cv::Rect rectTemp;
			rectTemp.x		= nStart_X;
			rectTemp.y		= nStart_Y;
			rectTemp.width	= nSegX;
			rectTemp.height	= nSegY;

			// ���� ����� ��� ����ó��
			if( rectTemp.x < 0 )	rectTemp.x = 0;
			if( rectTemp.y < 0 )	rectTemp.y = 0;
			if( rectTemp.x + rectTemp.width  >= matMeanBuf.cols )	rectTemp.width  = matMeanBuf.cols - rectTemp.x - 1;
			if( rectTemp.y + rectTemp.height >= matMeanBuf.rows )	rectTemp.height = matMeanBuf.rows - rectTemp.y - 1;

			// ���� �̻��� ���, ����ó��
			if( rectTemp.width  <= 0 )	continue;
			if( rectTemp.height <= 0 )	continue;

			// ���� ROI
			cv::Mat matTempROIBuf = matMeanBuf( rectTemp );

			// ����ũ ROI
			cv::Mat matMaskROIBuf = matEdgeBuf( rectTemp );

			// ��� ���ϱ�
			cv::Scalar m = cv::mean( matTempROIBuf, matMaskROIBuf );

			// nMinGV ���� ū ��츸 ��� ä���
			// ���� �����Ͽ� ��� ���� ��, ���� ������ �Ȱɸ��� ��� ���� ( �𼭸� �κ� )
			// ���� ���� �־�����, 0������ ����� ��� ����
			if( m[0] > nMinGV )
			{
				// x���⿡�� �켱 ó���Ͽ� ��� ����
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


// ���� ���� ��� ä���
long CInspectAlign::FillAreaMeanY_SVI(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV)
{
	// �Ķ���� NULL�� ���
	if( matMeanBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matEdgeBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// Active ������ �˻�
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	for (int x=0 ; x<2 ; x++)
	{
		long nStart_X, nEnd_X;

		// x ���� ���
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

			// y ���� ���
			nStart_Y = y * nSegY + rectROI.top;
			if( y==nRangeY-1)		nEnd_Y = rectROI.bottom;
			else					nEnd_Y = nStart_Y + nSegY;

			// ���� ����
			cv::Rect rectTemp;
			rectTemp.x		= nStart_X;
			rectTemp.y		= nStart_Y;
			rectTemp.width	= nSegX;
			rectTemp.height	= nSegY;

			// ���� ����� ��� ����ó��
			if( rectTemp.x < 0 )	rectTemp.x = 0;
			if( rectTemp.y < 0 )	rectTemp.y = 0;
			if( rectTemp.x + rectTemp.width  >= matMeanBuf.cols )	rectTemp.width  = matMeanBuf.cols - rectTemp.x - 1;
			if( rectTemp.y + rectTemp.height >= matMeanBuf.rows )	rectTemp.height = matMeanBuf.rows - rectTemp.y - 1;

			// ���� �̻��� ���, ����ó��
			if( rectTemp.width  <= 0 )	continue;
			if( rectTemp.height <= 0 )	continue;

			// ���� ROI
			cv::Mat matTempROIBuf = matMeanBuf( rectTemp );

			// ����ũ ROI
			cv::Mat matMaskROIBuf = matEdgeBuf( rectTemp );

			// ��� ���ϱ�
			cv::Scalar m = cv::mean( matTempROIBuf, matMaskROIBuf );

			// nMinGV ���� ū ��츸 ��� ä���
			// ���� �����Ͽ� ��� ���� ��, ���� ������ �Ȱɸ��� ��� ���� ( �𼭸� �κ� )
			// ���� ���� �־�����, 0������ ����� ��� ����
			if( (m[0] + m[1] + m[2])/3 > nMinGV )
			{
				// ��� ä���
				matTempROIBuf.setTo( m );

				// ����ũ ä���
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
	// �Ķ���� NULL�� ���
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

		// ����
		cv::bitwise_not(matMaskBuf, matMaskBuf);

		// ��� ��� �κи� �����
		cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

		// ����
		cv::add(matSrcBuf, matMeanBuf, matSrcBuf);
		*/

		////////////////////////////////////////////////////////////////////////// choikwangil 04.06 Test
		if (nAlgImg == E_IMAGE_CLASSIFY_AVI_WHITE || nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST) {

			cv::bitwise_and(matSrcBuf, matMaskBuf, matSrcBuf);

			// ����
			cv::bitwise_not(matMaskBuf, matMaskBuf);

			// ��� ��� �κи� �����
			cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

			// ����
			cv::add(matSrcBuf, matMeanBuf, matSrcBuf);
		}

		else {

			cv::Mat Test_tmp = cMatBufTemp.GetMat(matSrcBuf.size(), matSrcBuf.type(),false);

			// ����
			cv::bitwise_not(matMaskBuf, matMaskBuf);

			// ��� ��� �κи� �����
			cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

			cv::min(matMeanBuf, matSrcBuf, Test_tmp);

			if (nAlgImg == E_IMAGE_CLASSIFY_AVI_G) { //choi 06.04
				Threshold = 5;
			}
			else {
				Threshold = 20; //pwj 12.04.14 10->20���� ����
			}

				cv::threshold(Test_tmp, Test_tmp, Threshold, 255.0, THRESH_BINARY);
				//cv::imwrite("E:\\notch\\1_threshold.bmp", Test_tmp);

				cv::subtract(matMeanBuf, Test_tmp, matMeanBuf);
				//cv::imwrite("E:\\notch\\2_subtract.bmp", matMeanBuf);
				// ����	
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

		// ����
		cv::bitwise_not(matTempBuf16, matTempBuf16);

		// ��� ��� �κи� �����
		cv::bitwise_and(matMeanBuf, matTempBuf16, matMeanBuf);

		// ����
		cv::add(matSrcBuf, matMeanBuf, matSrcBuf);*/

		////////////////////////////////////////////////////////////////////////// choikwangil 04.06 Test
		if (nAlgImg == E_IMAGE_CLASSIFY_AVI_WHITE || nAlgImg == E_IMAGE_CLASSIFY_AVI_DUST) {

			cv::bitwise_and(matSrcBuf, matMaskBuf, matSrcBuf);

			// ����
			cv::bitwise_not(matMaskBuf, matMaskBuf);

			// ��� ��� �κи� �����
			cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

			// ����
			cv::add(matSrcBuf, matMeanBuf, matSrcBuf);
		}

		else {

			cv::Mat Test_tmp = cMatBufTemp.GetMat(matSrcBuf.size(), matSrcBuf.type(), false);

			// ����
			cv::bitwise_not(matMaskBuf, matMaskBuf);

			// ��� ��� �κи� �����
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
			// ����	
			cv::max(matSrcBuf, matMeanBuf, matSrcBuf);
		}
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}


// ����� �κи� �����
long CInspectAlign::FillMerge_SVI(cv::Mat& matSrcBuf, cv::Mat matMeanBuf, cv::Mat matMaskBuf)
{
	// �Ķ���� NULL�� ���
	if( matSrcBuf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matMeanBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matMaskBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat matTemp;
	cv::subtract(matSrcBuf, matMaskBuf, matTemp);

	// ��� ��� �κи� �����
	cv::bitwise_and(matMeanBuf, matMaskBuf, matMeanBuf);

	// ����
	cv::add(matTemp, matMeanBuf, matSrcBuf);

	matTemp.release();

	return E_ERROR_CODE_TRUE;
}


// � �κ�, ����� �������� ��¦ �ֱ� ( ��Ȳ�� ���� �ٸ� )
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


// � �̿� ���� ����
long CInspectAlign::calcLineConnect(cv::Mat& matSrcBuf, cv::Point ptSE[2], cv::Point ptPoly[2], int& nSE, int nSetArea)
{
	// ���۰� ���� ���.
	if( matSrcBuf.empty()	)		return E_ERROR_CODE_EMPTY_BUFFER;

	// �Ÿ� Ȯ��
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

		// ��� ���Ѱ� ���������� ���
		nSE = nMinIndex / 2;
	}
	else
	{
		// ptSE[0]���� ��� ��
		if( nSE == 0 )
		{				
			nMinIndex = ( nLength[0] > nLength[1] ) ? 1 : 0;
		}
		else
		{
			nMinIndex = ( nLength[2] > nLength[3] ) ? 3 : 2;
		}
	}


	// �Ÿ� ª�� ���� ����
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

// �ڳ� �κ� ���� ä��� ( Color )
long CInspectAlign::FillCorner(cv::Mat& matSrcROIBuf, cv::Mat& matMaskROIBuf, int nType)
{
	// ���۰� ���� ���.
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
	// ���۰� ���� ���.
	if( matSrcImage.empty()	)		return E_ERROR_CODE_EMPTY_BUFFER;

	// ����ó��
	if(nShiftX < 0)		nShiftX = 0;
	if(nShiftY < 0)		nShiftY = 0;

	nShiftLoopX++;
	nShiftLoopY++;

	//////////////////////////////////////////////////////////////////////////

	// ���� & ���
	cv::Mat matSrcBuf, matDstBuf;

	CMatBuf cMatBufTemp;

	// ���� �Ҵ� & �ʱ�ȭ
	cMatBufTemp.SetMem(cMem);
	matSrcBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matDstBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matSrcImage.copyTo(matSrcBuf);
	matSrcImage.copyTo(matDstBuf);

	// ���� ũ��
	int nImageSizeX = matSrcBuf.cols;
	int nImageSizeY = matSrcBuf.rows;

	// �ӽ� ����
	cv::Mat matTempBuf1, matTempBuf2;

	// x ����
	int nOffsetX = 0;
	for (int x=1 ; x<nShiftLoopX ; x++)
	{
		nOffsetX = x * nShiftX;

		matTempBuf1 = matDstBuf( cv::Rect(0, 0, nImageSizeX-nOffsetX, nImageSizeY)			);
		matTempBuf2 = matSrcBuf( cv::Rect(nOffsetX, 0, nImageSizeX-nOffsetX, nImageSizeY)	);

		// ����Ʈ �ҷ��� ��� ���� �ȵǾ�� �� �κ��� �迭�� ������ ����
		// ����� �ҷ��� ������Ƿ� ��� �Ұ�
		cv::add(matTempBuf1, matTempBuf2, matTempBuf1);
		//cv::max(matTempBuf1, matTempBuf2, matTempBuf1);
	}

	// y ����
	int nOffsetY = 0;
	matDstBuf.copyTo(matSrcBuf);
	for (int y=1 ; y<nShiftLoopY ; y++)
	{
		nOffsetY = y * nShiftY;

		matTempBuf1 = matDstBuf( cv::Rect(0, 0, nImageSizeX, nImageSizeY-nOffsetY)			);
		matTempBuf2 = matSrcBuf( cv::Rect(0, nOffsetY, nImageSizeX, nImageSizeY-nOffsetY)	);

		// ����Ʈ �ҷ��� ��� ���� �ȵǾ�� �� �κ��� �迭�� ������ ����
		// ����� �ҷ��� ������Ƿ� ��� �Ұ�
		cv::add(matTempBuf1, matTempBuf2, matTempBuf1);
		//cv::max(matTempBuf1, matTempBuf2, matTempBuf1);
	}

	// ���� ������ŭ ������
	//matDstBuf /= (nShiftLoopX * nShiftLoopY);

	// ��, �� �������θ� �����Ͽ����Ƿ�, �߾����� ������ �̵�
	nOffsetX /= 2;
	nOffsetY /= 2;
	//matDstImage = cv::Mat::zeros(matDstBuf.size(), matDstBuf.type());
	matDstBuf( cv::Rect(0, 0, matDstBuf.cols-nOffsetX, matDstBuf.rows-nOffsetY) ).copyTo( matDstImage(cv::Rect(nOffsetX, nOffsetY, matDstBuf.cols-nOffsetX, matDstBuf.rows-nOffsetY)) );

	// ���� ����
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
	// ���۰� ���� ���.
	if( matSrcImage.empty()	)		return E_ERROR_CODE_EMPTY_BUFFER;

	// ����ó��
	if(nShiftX < 0)		nShiftX = 0;
	if(nShiftY < 0)		nShiftY = 0;

	nShiftLoopX++;
	nShiftLoopY++;

	//////////////////////////////////////////////////////////////////////////

	// 16��Ʈ ��ȯ
	cv::Mat matSrcBuf16;
	matSrcImage.convertTo(matSrcBuf16, CV_16U);

	// ��� ����
	cv::Mat matDstBuf16 = matSrcBuf16.clone();

	// ���� ũ��
	int nImageSizeX = matSrcBuf16.cols;
	int nImageSizeY = matSrcBuf16.rows;

	// �ӽ� ����
	cv::Mat matTempBuf1, matTempBuf2;

	// x ����
	int nOffsetX = 0;
	for (int x=1 ; x<nShiftLoopX ; x++)
	{
		nOffsetX = x * nShiftX;

		matTempBuf1 = matDstBuf16( cv::Rect(0, 0, nImageSizeX-nOffsetX, nImageSizeY)		);
		matTempBuf2 = matSrcBuf16( cv::Rect(nOffsetX, 0, nImageSizeX-nOffsetX, nImageSizeY)	);

		cv::add(matTempBuf1, matTempBuf2, matTempBuf1);
	}

	// y ����
	int nOffsetY = 0;
	matSrcBuf16 = matDstBuf16.clone();
	for (int y=1 ; y<nShiftLoopY ; y++)
	{
		nOffsetY = y * nShiftY;

		matTempBuf1 = matDstBuf16( cv::Rect(0, 0, nImageSizeX, nImageSizeY-nOffsetY)		);
		matTempBuf2 = matSrcBuf16( cv::Rect(0, nOffsetY, nImageSizeX, nImageSizeY-nOffsetY)	);

		cv::add(matTempBuf1, matTempBuf2, matTempBuf1);
	}

	// ���� ������ŭ ������
	matDstBuf16 /= (nShiftLoopX * nShiftLoopY);

	// 8��Ʈ ��ȯ
	cv::Mat matDstBuf;
	matDstBuf16.convertTo(matTempBuf1, CV_8U);

	// ��, �� �������θ� �����Ͽ����Ƿ�, �߾����� ������ �̵�
	nOffsetX /= 2;
	nOffsetY /= 2;
	matDstImage = cv::Mat::zeros(matTempBuf1.size(), matTempBuf1.type());
	matTempBuf1( cv::Rect(0, 0, matTempBuf1.cols-nOffsetX, matTempBuf1.rows-nOffsetY) ).copyTo( matDstImage(cv::Rect(nOffsetX, nOffsetY, matTempBuf1.cols-nOffsetX, matTempBuf1.rows-nOffsetY)) );

	// ���� ����
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
// 	nCpyX	= (int)	(ShiftValue											)	/ 1000	;	// X ���� Unit
// 	nCpyY	= (int) (ShiftValue - nCpyX*1000							)	/ 100	;	// Y ���� Unit
// 	nLoopX	= (int) (ShiftValue	- nCpyX*1000 - nCpyY*100				)	/ 10	;	// X ���� Loop
// 	nLoopY	= (int) (ShiftValue - nCpyX*1000 - nCpyY*100 - nLoopX*10	)	/ 1		;	// Y ���� Loop

	nCpyX	= (int)	(ShiftValue/ 1000	%10);	// X ���� Unit
	nCpyY	= (int) (ShiftValue/ 100	%10);	// Y ���� Unit
	nLoopX	= (int) (ShiftValue/ 10		%10);	// X ���� Loop
	nLoopY	= (int) (ShiftValue/ 1		%10);	// Y ���� Loop

	return E_ERROR_CODE_TRUE;
}



// ���� �̸� ��������
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

// ���� �̸� ��������
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

	//�Ķ����
	BOOL bCurlUse        = (BOOL)dpara[E_PARA_APP_CURL_USE];         //�� ���� ��� ����
	if(bCurlUse < 1)
	{
		bCurl = FALSE;
		return E_ERROR_CODE_TRUE;
	}
	int nXRange                = (int)dpara[E_PARA_APP_CURL_RANGE_X];                 // x�� �˻� ����          100
	int nYRangeTop             = (int)dpara[E_PARA_APP_CURL_RANGE_Y_TOP];             // y�� �˻� ���� TOP      200
	int nYRangeBottom          = (int)dpara[E_PARA_APP_CURL_RANGE_Y_BOTTOM];          // y�� �˻� ���� Bottom   200(5.99Q�� Y�� ���� Bottom�� �۰�)
	int nDetectionCondition	   = (int)dpara[E_PARA_APP_CURL_CONDITION_COUNT];         // ���� ����               2
	double dSearchRatio        = dpara[E_PARA_APP_CURL_SEARCH_RATIO];            // ���� ����              1.5
	double dStandardGVRatio    = dpara[E_PARA_APP_CURL_STANDARD_GV_RATIO];       // ��� GV ����           0.85
	int nCornerSearchCount     = (int)dpara[E_PARA_APP_CURL_CORNER_SEARCH_COUNT];     // �ڳ� ���� ����           5 
	double dCornerSearchRatio  = dpara[E_PARA_APP_CURL_CORNER_SEARCH_RATIO];     // �ڳ� ���� ����          2.75
	int nStartOffsetLeft       = (int)dpara[E_PARA_APP_CURL_START_OFFSET_LEFT];       // �˻� ���� ������		  300
	int nStartOffsetRight	   = (int)dpara[E_PARA_APP_CURL_END_OFFSET_RIGHT];      // �˻� �� ������
	int nBlurSize              = (int)dpara[E_PARA_APP_CURL_BLUR_SIZE];               // �� ������		       11
	float nGausSigma		   = (float)dpara[E_PARA_APP_CURL_GAUSSIGMA];				 // ����þ� �� Sigma	    5
	int	  nProfileOffset_W	   = (int)dpara[E_PARA_APP_CURL_PROFILE_SEARCH_OFFSET];	//Profile���� �ּ� �� ���ķ� Parameter ���� �ʺ�� ����Ͽ� ���� ���� ���, �ʺ� ���� ���� ����

	if(nBlurSize%2==0)
		nBlurSize++;
	Size szGaussianBlurSize(nBlurSize,nBlurSize);

	Mat mtSrc;
	matSrcBuf.copyTo(mtSrc);

	cv::GaussianBlur(mtSrc, mtSrc, szGaussianBlurSize, nGausSigma);             // ����þ� ��

#ifdef _DEBUG
	// ���� ����̺� D:\ ���� �ƴ� - �ʿ� �� InspectLibLog �� GETDRV() Ȱ��
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
				cv::meanStdDev(mtSrc(cv::Rect(nX, nY, nXRange, 1)), m, s);  //nXRange ������ ��� GV
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
			// TOM ���� CUR ������ ���
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
							nDetectionCount[nDirection]++;  // �Ÿ� ����
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

			// BUTTOM ���� CUR �������
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
							nDetectionCount[nDirection]++;  // �Ÿ� ����
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

		// �ڳ� �κ� ���� �˻�
		for(int n = 0; n < nCornerSearchCount; n++)
		{
			nCornerSearchDistance[nDirection].push_back(nDistance[nDirection][n]);
		}
		for(int n = 0; n < nCornerSearchCount; n++)
		{
			nCornerSearchDistance[nDirection].push_back(nDistance[nDirection][nDistance[nDirection].size()-(n+1)]);
		}

		nCornerMaxDistance[nDirection] = *max_element(nCornerSearchDistance[nDirection].begin(), nCornerSearchDistance[nDirection].end());// �ڳ� �κ� �ִ�
		nDistance[nDirection].erase(nDistance[nDirection].begin(), nDistance[nDirection].begin()+nCornerSearchCount);
		nDistance[nDirection].erase(nDistance[nDirection].end()-nCornerSearchCount, nDistance[nDirection].end());

		nMaxDistance[nDirection] = *max_element(nDistance[nDirection].begin(), nDistance[nDirection].end()); 
		dMeanDistance[nDirection]=0;
		for(int n=0; n < nDistance[nDirection].size(); n++)
		{
			dMeanDistance[nDirection]+=nDistance[nDirection][n]; //�Ÿ� ���
		}
		dMeanDistance[nDirection] = dMeanDistance[nDirection]/nDistance[nDirection].size();
		nCount[nDirection]=0;
		for(int n = 0; n < nDistance[nDirection].size(); n++)
		{
			if(nDistance[nDirection][n] >= dMeanDistance[nDirection] * dSearchRatio) //(��� �Ÿ� * �������)���� ���� �Ÿ� ī��Ʈ
				nCount[nDirection]++;			
		}
		for(int n = 0; n < nCornerSearchDistance[nDirection].size();n++)
		{
			if(nCornerSearchDistance[nDirection][n] >= dMeanDistance[nDirection]*dCornerSearchRatio) //(��� �Ÿ� * �������)���� ���� �Ÿ� ī��Ʈ(�ڳ� �κ�)
				nCount[nDirection]++;
		}
		if(nDirection==0&&nCount[nDirection]>=nDetectionCondition)
			nCurlJudge++;//�� ����

		else if(nDirection==1&&nCount[nDirection]>=nDetectionCondition)
			nCurlJudge++;//�� ����	
	}

	if(bSaveImage)
	{
		CString strSavePath;
		strSavePath.Format(_T("%s\\Curl_0.bmp"),strPath);
		imwrite((cv::String)(CStringA)strSavePath,mtCurl);
	}
	
	//���� ���̱�
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

	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
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

	// ���۰� ���� ���.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;


	cv::Mat matGrayBuf, matTresBuf, matDrawBuf_Temp ,matTempBuf;

	// ���� �Ҵ� & �ʱ�ȭ
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matDrawBuf_Temp = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	matDrawBuf.copyTo(matDrawBuf_Temp);

	// �÷��� ��� ( SVI )
	if (matSrcBuf.channels() != 1)
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// ����� ��� ( AVI, APP )
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


	Mat M(2, 3, CV_32FC1); // warpAffine ���꿡 ���Ǵ� 2*3 ���
	vector<Vec3f> circles; // ���� �� ã�� (�ȵǸ� Ÿ�� ã�� or PerspectiveTransform)
	vector<vector<Point>> contours; // ū Ÿ�� ã��
	vector<Vec4i> hierarchy;

	RotatedRect e; // ū Ÿ�� ����
	Vec3f find_circle; // ���� �� ����
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
		if (dArea > nCircle_MinArea) // ǥ�� 3�� ���� ���� �� 112��
		{
			e = fitEllipse(contours[i]);
			// cout << "e.angle : " << e.angle << " e.center : " << e.center << " e.size : " << e.size << endl;
			fMax_dist = e.size.height / 2; // Ÿ���� ����/2
			fCal_dist = e.size.width / 2; // Ÿ���� ����/2 (�ѷ� ����)

			ellipse(matDrawBuf_Temp, e, Scalar(255, 0, 0), 2); // �׸��� üũ (Ÿ�� �׵θ�)
			circle(matDrawBuf_Temp, e.center, 2, Scalar(0, 0, 255), 3); // �׸��� üũ (Ÿ�� �߽�)
			line(matDrawBuf_Temp, e.center, Point2f(e.center.x, e.center.y + e.size.height / 2), Scalar(127, 0, 127), 2, 8, 0); // �׸��� üũ (Ÿ�� ����)

			Size2f size(e.size.width / 1.3, e.size.height / 1.3); // ���� ��� ����� -> ���� �� ã�� ����
			e.size = size;
			fMin_dist = e.size.width / 2; // ���� Ÿ���� ����/2

			ellipse(matDrawBuf_Temp, e, Scalar(255, 255, 0), 2); // �׸��� üũ (���� Ÿ�� �׵θ�)
			line(matDrawBuf_Temp, e.center, Point2f(e.center.x + e.size.width / 2, e.center.y), Scalar(127, 0, 127), 2, 8, 0); // �׸��� üũ (���� Ÿ�� ����)

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
	Mask_SizeDown(matMask_Active, nActive_MaskIn); //�ܰ� ���� �ҷ� ����

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("matMask_Active"));
		imwrite((cv::String)(CStringA)strSaveName, matMask_Active);
	}
	//////////
	// ���� �� ã��
	medianBlur(matGrayBuf, matTempBuf, nMedian); // blur�� ��������
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

			line(matDrawBuf_Temp, e.center, Point2f(pCircle_center.x, pCircle_center.y), Scalar(0, 255, 255), 2, 8, 0); // �׸��� üũ (���� �� �߽���)

			if (fCircle_dist > fMin_dist && fCircle_dist < fMax_dist) // ���� �����ȿ� ������ �Ϸ�
			{
				circle(matDrawBuf_Temp, pCircle_center, find_circle[2], Scalar(0, 255, 0), 1); // �׸��� üũ (�� �׵θ�)
				circle(matDrawBuf_Temp, pCircle_center, 1, Scalar(0, 0, 255), 2); // �׸��� üũ (�� �߽�)

				circle(matMask_Hole, pCircle_center, find_circle[2], Scalar(255), -1); // �׸��� üũ (�� �׵θ�) Mask
				Mask_SizeUp(matMask_Hole, nHole_MaskOut);

				fCircle_x = find_circle[0]; // ��ǥ ���� �� x��ǥ
				fCircle_y = find_circle[1]; // ��ǥ ���� �� y��ǥ

				break; // break�� �ּ����� �ϸ� ��� ���� ���� ������ �׷���. ���� ������ ���� �� �׵θ��� �׷���.
			}
		}

	

		if (nImageSave_Flag)
		{
			CString strSaveName;
			strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("matMask_Hole"));
			imwrite((cv::String)(CStringA)strSaveName, matMask_Hole);
		}
		// ���� �����ȿ� ���� ���� 2���̻��̸�? -> ��� ��ƼĿ�� ��˻� �������� ����� or ���� �� �߽����� ��� ���� or ���� �߾� �ٻ簪���� �̱� ��.
		// ���� �� �߽����� ���� ���͸� �ϴ°� ���� ���� ����̶� ���������� �ش� �ȼ��� ���� ����� ���޾Ƽ�.. ����

		if (fCircle_x <= 0 || fCircle_y <= 0) // ���� �����ȿ� ���� ���� 1���� ������
		{
			return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
		}
	}
	else if (circles.size() == 0) // ���� ���� ��ã����
	{
		return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
	}

	// ���� ���
	// ���ﰢ�Լ��� ū Ÿ���� �߽����� ���� ���� �߽��� ���� ��Ÿ�� ���Ͽ� 3�ù������� ȸ��.
	// ��ũź��Ʈ�� radian���� ��ȯ�ǹǷ� degree�� ��ȯ
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
		M2.at<float>(1, 1) = 1.f; // ���� ��� -> ��, ������ �״��
		M2.at<float>(0, 2) = ptCenter.x - fCircle_x; // ù ������ ���� ���� �������� x�� �̵���
		M2.at<float>(1, 2) = ptCenter.y - fCircle_y; // ù ������ ���� ���� �������� y�� �̵���
		cv::warpAffine(matSrcBuf, matSrcBuf, M2, matSrcBuf.size()); // �̵�
																  //  		boundingRect.x += (fMain_Stan[0] - fCircle_x);
																  //  		boundingRect.y += (fMain_Stan[1] - fCircle_y);
	}


	// Ÿ���� ����
// 	double dRes_P = 2 * CV_PI * sqrt((pow(fMax_dist * 2, 2) + pow(fCal_dist * 2, 2)) / 2) * um; // 2�� * sqrt((����^2 + ����^2) / 2)
// 	double dRes_area = CV_PI * fMax_dist * fCal_dist * um; // �� * ����/2 * ����/2

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

	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
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

	// ���� �Ҵ� & �ʱ�ȭ
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matDrawBuf_Temp = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	matDrawBuf.copyTo(matDrawBuf_Temp);

	// �÷��� ��� ( SVI )
	if (matSrcBuf.channels() != 1)
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// ����� ��� ( AVI, APP )
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
	// 		circle(matTemp, vPoint_Circle[i], 1, Scalar(155), 2); // �׸��� üũ (�� �׵θ�)
	// 	}

	if (vPoint_Circle.size() == 0) // Edge ��ã���� ����
	{
		return E_ERROR_CODE_LCP_ALIGN_ACTIVE_MASK_FAIL;
	}

	RotatedRect e2;
	e2 = cv::fitEllipse(vPoint_Circle);

	cv::ellipse(matTemp, e2, Scalar(255), -1); // �׸��� üũ (Ÿ�� �׵θ�)

	//imwrite("D:\\Align_Test.bmp", matTemp);
	///////////////////////////////////

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("Stage sub"));
		imwrite((cv::String)(CStringA)strSaveName, matGrayBuf);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Stage sub."));

	Mat M(2, 3, CV_32FC1); // warpAffine ���꿡 ���Ǵ� 2*3 ���
	vector<Vec3f> circles; // ���� �� ã�� (�ȵǸ� Ÿ�� ã�� or PerspectiveTransform)
	vector<vector<Point>> contours; // ū Ÿ�� ã��
	vector<Vec4i> hierarchy;

	RotatedRect e; // ū Ÿ�� ����
	Vec3f find_circle; // ���� �� ����
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
		if (dArea > nCircle_MinArea) // ǥ�� 3�� ���� ���� �� 112��
		{
			e = fitEllipse(contours[i]);
			// cout << "e.angle : " << e.angle << " e.center : " << e.center << " e.size : " << e.size << endl;
			fMax_dist = e.size.height / 2; // Ÿ���� ����/2
			fCal_dist = e.size.width / 2; // Ÿ���� ����/2 (�ѷ� ����)

			ellipse(matDrawBuf_Temp, e, Scalar(255, 0, 0), 2); // �׸��� üũ (Ÿ�� �׵θ�)
			circle(matDrawBuf_Temp, e.center, 2, Scalar(0, 0, 255), 3); // �׸��� üũ (Ÿ�� �߽�)
			line(matDrawBuf_Temp, e.center, Point2f(e.center.x, e.center.y + e.size.height / 2), Scalar(127, 0, 127), 2, 8, 0); // �׸��� üũ (Ÿ�� ����)

			Size2f size(e.size.width / 1.3, e.size.height / 1.3); // ���� ��� ����� -> ���� �� ã�� ����
			e.size = size;
			fMin_dist = e.size.width / 2; // ���� Ÿ���� ����/2

			ellipse(matDrawBuf_Temp, e, Scalar(255, 255, 0), 2); // �׸��� üũ (���� Ÿ�� �׵θ�)
			line(matDrawBuf_Temp, e.center, Point2f(e.center.x + e.size.width / 2, e.center.y), Scalar(127, 0, 127), 2, 8, 0); // �׸��� üũ (���� Ÿ�� ����)

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
	// ���� �� ã��
	medianBlur(matGrayBuf, matTempBuf, nMedian); // blur�� ��������
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

			line(matDrawBuf_Temp, e.center, Point2f(pCircle_center.x, pCircle_center.y), Scalar(0, 255, 255), 2, 8, 0); // �׸��� üũ (���� �� �߽���)

			if (fCircle_dist > fMin_dist && fCircle_dist < fMax_dist) // ���� �����ȿ� ������ �Ϸ�
			{
				circle(matDrawBuf_Temp, pCircle_center, find_circle[2], Scalar(0, 255, 0), 1); // �׸��� üũ (�� �׵θ�)
				circle(matDrawBuf_Temp, pCircle_center, 1, Scalar(0, 0, 255), 2); // �׸��� üũ (�� �߽�)

				circle(matMask_Hole, pCircle_center, find_circle[2], Scalar(255), -1); // �׸��� üũ (�� �׵θ�) Mask
				//Mask_SizeUp(matMask_Hole, 20);
				Mask_SizeUp(matMask_Hole, nHole_MaskOut);

				fCircle_x = find_circle[0]; // ��ǥ ���� �� x��ǥ
				fCircle_y = find_circle[1]; // ��ǥ ���� �� y��ǥ

				break; // break�� �ּ����� �ϸ� ��� ���� ���� ������ �׷���. ���� ������ ���� �� �׵θ��� �׷���.
			}
		}

		if (nImageSave_Flag)
		{
			CString strSaveName;
			strSaveName.Format(_T("%s\\%d_%s.bmp"), strAlgPath, nImageClassify, _T("matMask_Hole"));
			imwrite((cv::String)(CStringA)strSaveName, matMask_Hole);
		}

		// ���� �����ȿ� ���� ���� 2���̻��̸�? -> ��� ��ƼĿ�� ��˻� �������� ����� or ���� �� �߽����� ��� ���� or ���� �߾� �ٻ簪���� �̱� ��.
		// ���� �� �߽����� ���� ���͸� �ϴ°� ���� ���� ����̶� ���������� �ش� �ȼ��� ���� ����� ���޾Ƽ�.. ����

		if (fCircle_x <= 0 || fCircle_y <= 0) // ���� �����ȿ� ���� ���� 1���� ������
		{
			return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
		}
	}
	else if (circles.size() == 0) // ���� ���� ��ã����
	{
		return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
	}

	// ���� ���
	// ���ﰢ�Լ��� ū Ÿ���� �߽����� ���� ���� �߽��� ���� ��Ÿ�� ���Ͽ� 3�ù������� ȸ��.
	// ��ũź��Ʈ�� radian���� ��ȯ�ǹǷ� degree�� ��ȯ
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
// 		M2.at<float>(1, 1) = 1.f; // ���� ��� -> ��, ������ �״��
// 		M2.at<float>(0, 2) = ptCenter.x - fCircle_x; // ù ������ ���� ���� �������� x�� �̵���
// 		M2.at<float>(1, 2) = ptCenter.y - fCircle_y; // ù ������ ���� ���� �������� y�� �̵���
// 		cv::warpAffine(matSrcBuf, matSrcBuf, M2, matSrcBuf.size()); // �̵�
// 																	//  		boundingRect.x += (fMain_Stan[0] - fCircle_x);
// 																	//  		boundingRect.y += (fMain_Stan[1] - fCircle_y);
// 	}


	// Ÿ���� ����
// 	double dRes_P = 2 * CV_PI * sqrt((pow(fMax_dist * 2, 2) + pow(fCal_dist * 2, 2)) / 2) * um; // 2�� * sqrt((����^2 + ����^2) / 2)
// 	double dRes_area = CV_PI * fMax_dist * fCal_dist * um; // �� * ����/2 * ����/2

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
// 	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
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
// 	// ���۰� ���� ���.
// 	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
// 
// 
// 	cv::Mat matGrayBuf, matTresBuf, matVariBuf;
// 
// 	// ���� �Ҵ� & �ʱ�ȭ
// 	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 	matVariBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 
// 	// �÷��� ��� ( SVI )
// 	if (matSrcBuf.channels() != 1)
// 		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
// 	// ����� ��� ( AVI, APP )
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
// // 	// Active ������ ã�� ���� ����ȭ
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
// // 	// Active Mask �����
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
// 	// Active ������ �����
// 	//cv::bitwise_and(matSrcBuf, matDstActive, matGrayBuf);
// 
// 
// 	//////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// // 	// ���⼭ ���Ͱ� Label ã��
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
// // 	////////////////////////////////// ������
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
// // 	// Lable Mask �����
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
// 	// Hole Mask �����
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
// 	// �޸� ����
// 	// �̸��̸� �صθ� ������ �ϴ� Test... 
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
// 	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
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
// 	// ���۰� ���� ���.
// 	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
// 
// 
// 	cv::Mat matGrayBuf, matTresBuf, matVariBuf;
// 
// 	// ���� �Ҵ� & �ʱ�ȭ
// 	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 	matVariBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
// 
// 	// �÷��� ��� ( SVI )
// 	if (matSrcBuf.channels() != 1)
// 		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
// 	// ����� ��� ( AVI, APP )
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
// 	// 	// Active ������ ã�� ���� ����ȭ
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
// 	// 	// Active Mask �����
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
// 	// Active ������ �����
// 	//cv::bitwise_and(matSrcBuf, matDstActive, matGrayBuf);
// 
// 
// 	//////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// 	// 	// ���⼭ ���Ͱ� Label ã��
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
// 	// 	////////////////////////////////// ������
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
// 	// 	// Lable Mask �����
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
// 	// Hole Mask �����
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
// 	// �޸� ����
// 	// �̸��̸� �صθ� ������ �ϴ� Test... 
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

	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	int nImageSave_Flag = (int)dPara[E_PARA_LCP_MASK_LABEL_TEXT];

	int nThreshLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_TH];
	int nGauSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_GAU_SZ];
	double dGauSigLabel = (double)dPara[E_PARA_LCP_MASK_LABEL_GAU_SIG];
	int nMaxSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MAX];
	int nMinSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MIN];
	int nRound_In = (int)dPara[E_PARA_LCP_MASK_LABEL_ROUND_IN];
	int nRound_Out = (int)dPara[E_PARA_LCP_MASK_LABEL_ROUND_OUT];
	// ���۰� ���� ���.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;


	cv::Mat matGrayBuf, matTresBuf, matVariBuf;

	// ���� �Ҵ� & �ʱ�ȭ
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matVariBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);

	// �÷��� ��� ( SVI )
	if (matSrcBuf.channels() != 1)
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// ����� ��� ( AVI, APP )
	else
	{
		if (matSrcBuf.type() == CV_8UC1)
			matSrcBuf.copyTo(matGrayBuf);
		else
			matSrcBuf.convertTo(matGrayBuf, CV_8UC1, 1. / 16.);

	}

	// ���⼭ ���Ͱ� Label ã��
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

	// Lable Mask �����
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

										// ���۰� ���� ���.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;


	cv::Mat matGrayBuf, matTresBuf, matDrawBuf_Temp, matTempBuf;

	// ���� �Ҵ� & �ʱ�ȭ
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, true);


	// �÷��� ��� ( SVI )
	if (matSrcBuf.channels() != 1)
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// ����� ��� ( AVI, APP )
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

	vector<vector<Point>> contours; // ū Ÿ�� ã��
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
		if (dArea > nBack_MinArea) // ǥ�� 3�� ���� ���� �� 112��
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
		if (dArea > nBack_MinArea) // ǥ�� 3�� ���� ���� �� 112��
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

	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	int nImageSave_Flag = (int)dPara[E_PARA_LCP_MASK_VACUUM_TEXT];
	double nThreshold = (double)dPara[E_PARA_LCP_MASK_VACUUM_IMAGE_TRESHOLD];
	int nMin_Area = (int)dPara[E_PARA_LCP_MASK_VACUUM_IMAGE_MIN_AREA];
	int nVac_RoundOut = (int)dPara[E_PARA_LCP_MASK_VACUUM_IMAGE_ROUNDOUT];

	//const double um = dCamResolution; // 30um -> �ȳ��ͼ� �ϴ� 300um����
	const double um = dCamResolution;
	// ���۰� ���� ���.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;


	cv::Mat matGrayBuf, matTresBuf, matDrawBuf_Temp, matTempBuf;

	// ���� �Ҵ� & �ʱ�ȭ
	matGrayBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, true);
	matTresBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, true);
	matTempBuf = cMem->GetMat(matSrcBuf.size(), CV_8UC1, true);


	// �÷��� ��� ( SVI )
	if (matSrcBuf.channels() != 1)
		cv::cvtColor(matSrcBuf, matGrayBuf, COLOR_RGB2GRAY);
	// ����� ��� ( AVI, APP )
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

		if (dArea_RC > nMin_Area) // ���� ū��
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
	// �ι�°�� ū ������ ��Ƴ��� - ���ڵ� ���� (���� ����: ���ڵ�� �׻� ��γ� �Ϻ� ���� �ϳ��� �����ǿ����� ��Ÿ������)

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

		if (dArea2 > dLargest_Area2) // ���� ū��
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

		if (dArea_RC > dLargest_Area_RC) // ���� ū��
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

	// ���� ū ����(Active ������ ã��)
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

	//Mask Size�� ��� �����Ƿ� �� ����
	nMaskSizeX = nMaskSize;
	nMaskSizeY = nMaskSize;

	//����ũ 
	cv::Mat matFilterMask = cv::Mat::zeros(nMaskSizeX, nMaskSizeY, CV_8UC1);
	nMaskSize = nMaskSize / 2 * -1;

	//���� �ȼ� ������ ����
	//�Է� Image
	uchar* ucSrcdata;
	ucSrcdata = prc.data;

	//��� Image
	uchar* ucDstdata;
	ucDstdata = dst.data;

	//Mask Image
	uchar* ucFilterMask;
	ucFilterMask = matFilterMask.data;

	//��ü ���� ���� ũ��
	for (int nY = 0; nY < nRows; nY++)
	{
		//�� �ܰ� ������ ���� ���� ����(���� ó�� ������ �⺻������ ���� �߾ӿ� ��ġ�ϹǷ� ���� ������ ���� ó�� �ʿ� ����
		if (nY + nMaskSize < 0 || nY + abs(nMaskSize) > nRows - 1)
			continue;

		//��ü ���� ���� ũ��
		for (int nX = 0; nX < nCols; nX++)
		{

			//�� �ܰ� ������ ���� ���� ����(���� ó�� ������ �⺻������ ���� �߾ӿ� ��ġ�ϹǷ� ���� ������ ���� ó�� �ʿ� ����
			if (nX + nMaskSize < 0 || nX + nMaskSize > nCols - 1)
				continue;

			//Mask y Size
			for (int nFy = 0; nFy < nMaskSizeY; nFy++)
			{
				//Mask x Size
				for (int nFx = 0; nFx < nMaskSizeX; nFx++)
				{
					//�簢�� Ÿ��
					ucFilterMask[nFy * nMaskSizeX + nFx] = ucSrcdata[((nY - (nMaskSizeY / 2) + nFy) * nCols) + ((nX - (nMaskSizeX / 2)) + nFx)];
					//������ (���� �ѹ� ���� �ѹ� ���ϱ�)
				}//Mask y Size
			}//Mask x Size

			 // ���� ����ũ ��ü �̹����� SDV ���ϱ� 
			Scalar m, s;
			double dVariance = 0;
			cv::meanStdDev(matFilterMask, m, s);
			dVariance = (double)s[0] * (double)s[0];

			if (dVariance < 0.0f)   ucDstdata[nY * nCols + nX] = (BYTE)0;
			else if (dVariance > 255.0f) ucDstdata[nY * nCols + nX] = (BYTE)255;
			else						ucDstdata[nY * nCols + nX] = (BYTE)dVariance;

		}//��ü ���� ���� ũ�� ����
	}//��ü ���� ���� ũ�� ����
}

long CInspectAlign::DoFindActive_LCP_AutoCal_Mask(cv::Mat matBackBuf, cv::Mat matEmptyBuf, cv::Mat matSide, CString Path_Result,CString ColorCode, CString T_Judge_Code ,double* dPara)
{
	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Start."));

	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	if (dPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	//////////////////////////////////////////////////////////////////////////
	//Param�� Gui���� �޾ƿ;���
	//Active ��
// 	int nImageSave_Flag		= (int)dPara[E_PARA_LCP_ALIGN_LIMPI_TEXT];
// 	int nEdgeTH				= (int)dPara[E_PARA_LCP_ALIGN_LIMPI_EDGE_TH]; //120
// 	int nCircle_MinArea		= (int)dPara[E_PARA_LCP_ALIGN_LIMPI_BIGCIRCLE_MINAREA]; //10000000
// 
// 	int nActive_MaskIn		= (int)dPara[E_PARA_LCP_ALIGN_LIMPI_ACTIVEMASK_IN]; //150
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	//Label ��
// 	int nThreshLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_TH]; //NP: 5 , ����: 70
// 	int nGauSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_GAU_SZ]; //3
// 	double dGauSigLabel = (double)dPara[E_PARA_LCP_MASK_LABEL_GAU_SIG]; //3.0
// 	int nMaxSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MAX]; //5
// 	int nMinSzLabel = (int)dPara[E_PARA_LCP_MASK_LABEL_MIN]; //5
// 	int nRound_In = (int)dPara[E_PARA_LCP_MASK_LABEL_ROUND_IN]; //20
// 	int nRound_Out = (int)dPara[E_PARA_LCP_MASK_LABEL_ROUND_OUT]; //20
// 	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Param�� Gui���� �޾ƿ;���
	//Active ��
	int nImageSave_Flag = 1;
	int nEdgeTH = 200; //120
	int nCircle_MinArea = 10000000; //10000000

	int nActive_MaskIn = 150; //150

																		   //////////////////////////////////////////////////////////////////////////
																		   //Label ��
	int nThreshLabel = 70; //NP: 5 , ����: 70
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

	
	// ���� �Ҵ� & �ʱ�ȭ
	//////////////////////////////////////////////////////////////////////////
	//Active Mask ��
	matGrayBuf = cMem->GetMat(matBackBuf.size(), CV_8UC1, false);
	matTresBuf = cMem->GetMat(matBackBuf.size(), CV_8UC1, false);
	matTempBuf = cMem->GetMat(matBackBuf.size(), CV_8UC1, false);
	matMask_Active = cMem->GetMat(matBackBuf.size(), CV_8UC1, false);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Label Mask��

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

	if (vPoint_Circle.size() == 0) // Edge ��ã���� ����
	{
		return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
	}

	RotatedRect e2;
	e2 = cv::fitEllipse(vPoint_Circle);

	cv::ellipse(matTemp, e2, Scalar(255), -1); // �׸��� üũ (Ÿ�� �׵θ�)

	if (nImageSave_Flag)
	{
		CString strSaveName;
		strSaveName.Format(_T("%s\\%s.bmp"), Path_Result,  _T("Draw Circle"));
		imwrite((cv::String)(CStringA)strSaveName, matTemp);
	}

	writeInspectLog(E_ALG_TYPE_COMMON_ALIGN, __FUNCTION__, _T("Draw Circle."));

	Mat M(2, 3, CV_32FC1); // warpAffine ���꿡 ���Ǵ� 2*3 ���
	vector<Vec3f> circles; // ���� �� ã�� (�ȵǸ� Ÿ�� ã�� or PerspectiveTransform)
	vector<vector<Point>> contours; // ū Ÿ�� ã��
	vector<Vec4i> hierarchy;

	RotatedRect e; // ū Ÿ�� ����
	double dArea;

	matTemp.copyTo(matTresBuf);

	findContours(matTresBuf, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


	for (int i = 0; i < contours.size(); i++) {
		dArea = contourArea(contours[i]);
		if (dArea > nCircle_MinArea) // ǥ�� 3�� ���� ���� �� 112��
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

		// Lable Mask �����
		MakeLabelMask(Maximum, matDstLabel);

		Mask_SizeUp(matDstLabel, nRound_Out);
	
		CString strResult_Label;
		strResult_Active.Format(_T("%s\\%s.bmp"), Path_Result, _T("Label_Mask"));
		imwrite((cv::String)(CStringA)strResult_Active, matDstLabel);
	}
	else {
		cv::Mat matDstLabel = cv::Mat::zeros(matBackBuf.size(), CV_8U); // ���� np �ƴҶ� 0���� ä��

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