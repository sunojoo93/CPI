#include "StdAfx.h"
#include "InspectActive.h"
#include "ExportLibrary.h"
#include "FeatureExtraction.h"

#define pi 3.141592

const TCHAR* strActiveBurnt[] = {ROI_ACTIVE_BURNT_1, ROI_ACTIVE_BURNT_2 ,ROI_ACTIVE_BURNT_3};

CInspectActive::CInspectActive(void)
{
	cMem=NULL;
	m_cInspectLibLog	= NULL;
	m_strAlgLog			= NULL;
	m_tInitTime			= 0;
	m_tBeforeTime		= 0;
}


CInspectActive::~CInspectActive(void)
{
}

long	CInspectActive::ImagePreprecessing(Mat matSrc, cv::Mat& MatDes)
{
	const int		nGaussianKeranlSize		=	11;
	const double	dGaussianKeranlSigma	=	0.0;

	Mat matGauss;

	cv::GaussianBlur(matSrc, matGauss, Size(nGaussianKeranlSize,nGaussianKeranlSize), dGaussianKeranlSigma);

	GuidedEdgeEnhance(matGauss, MatDes);

	return 0;
}

long	CInspectActive::FindBubbleDefect(Mat matSrc,  Mat& matDrawBuffer, int nMinBubbleSize, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	// �Ķ���� ���� ���δ� ���߿� ��� �� ���� 
	const	int		nGaussianKeranlSize		=	31;
	const	double	dGaussianKeranlSigma	=	0.0;

	const	int		nMinSize				= nMinBubbleSize;

	bool			bSave					=	false;

	Mat matGauss, matSobelBinary, matBubble;

	cv::GaussianBlur(matSrc, matGauss, Size(nGaussianKeranlSize,nGaussianKeranlSize), dGaussianKeranlSigma);

// ���� ����̺� D:\ ���� �ƴ� - �ʿ� �� InspectLibLog �� GETDRV() Ȱ��
// 	if(bSave)
// 		imwrite("D:\\Active Bubble Gauss.bmp", matGauss);


	Edge_BasicSobel(matGauss, matSobelBinary, 5);

// ���� ����̺� D:\ ���� �ƴ� - �ʿ� �� InspectLibLog �� GETDRV() Ȱ��
// 	if(bSave)
// 		imwrite("D:\\Active Bubble Sobel.bmp", matSobelBinary);

	cv::threshold(matSobelBinary, matSobelBinary, 120, 255, cv::THRESH_BINARY);

// ���� ����̺� D:\ ���� �ƴ� - �ʿ� �� InspectLibLog �� GETDRV() Ȱ��
// 	if(bSave)
// 		imwrite("D:\\Active Bubble Sobel.bmp", matSobelBinary);

	int nMode = RETR_EXTERNAL;
	int nMethod = CHAIN_APPROX_SIMPLE;

	vector<vector<Point>> contours;		// ��ô ��� ����
	// �ܰ��� ����
	findContours(matSobelBinary, contours, noArray(), nMode, nMethod);

	if(bSave)
	{
		Mat Draw;

		cv::cvtColor(matSobelBinary, Draw, CV_GRAY2RGB);

		// ���� ����� ��� ���ۿ� �׷� �ֱ� 
		for(int k=0; k < contours.size();k++)
		{
			Scalar color(255,0,0);
			drawContours(Draw, contours, k, color, 1);
		}

// ���� ����̺� D:\ ���� �ƴ� - �ʿ� �� InspectLibLog �� GETDRV() Ȱ��
// 		imwrite("D:\\Active Bubble Binary Edge.bmp", Draw);
	}

	matBubble = Mat::zeros(matSrc.size(), CV_8UC1);

	for(long n=0; n< (int)contours.size() ; n++)
	{
		// ������ �ܰ� 4���� ���ϱ�
		int nLeft, nRight, nTop, nBottom;
		nLeft	= matSobelBinary.cols;
		nTop	= matSobelBinary.rows;
		nBottom = 0;
		nRight	= 0;

		int nCount = (int) contours[n].size();

		for(long n2=0; n2 < nCount ; n2++)
		{

			if(nLeft > contours.at(n).at(n2).x)
				nLeft = contours.at(n).at(n2).x;

			if(nRight < contours.at(n).at(n2).x)
				nRight = contours.at(n).at(n2).x;

			if(nTop > contours.at(n).at(n2).y)
				nTop = contours.at(n).at(n2).y;

			if(nBottom < contours.at(n).at(n2).y)
				nBottom = contours.at(n).at(n2).y;
		}
		
		if( abs( nBottom - nTop ) < nMinSize || abs( nRight - nLeft  ) < nMinSize )
		{
			continue;
		}

		drawContours(matBubble, contours, n, Scalar(255,255,255), -1);

	}

	Mat element(3,3, CV_8U, Scalar(1));

	cv::morphologyEx(matBubble, matBubble,cv::MORPH_ERODE,  element, Point(-1, -1), 5);  //  ���� �ּ� ����



	if(bSave)
	{
// ���� ����̺� D:\ ���� �ƴ� - �ʿ� �� InspectLibLog �� GETDRV() Ȱ��
// 		imwrite("D:\\Active Bubble Flood Image Binary.bmp", matBubble);

		Mat Draw;

		cv::cvtColor(matSrc, Draw, CV_GRAY2RGB);

		Scalar color(255,0,0);
	
		vector<vector<Point>> contoursInner;
		findContours(matBubble, contoursInner, noArray(), nMode, nMethod);


		// ���� ����� ��� ���ۿ� �׷� �ֱ� 
		for(int k=0; k < contoursInner.size();k++)
		{
			Scalar color(255,0,0);
			drawContours(Draw, contoursInner, k, color, 1);
		}


// ���� ����̺� D:\ ���� �ƴ� - �ʿ� �� InspectLibLog �� GETDRV() Ȱ��
//		imwrite("D:\\Active Bubble Flood Image Gray.bmp", Draw);

	}
	//long CFeatureExtraction::DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	//	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType,  bool bPtRotate)

	// ��� ���� ��� ���� 
	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem(cMem);
	cFeatureExtraction.DoDefectBlobSingleJudgment(matGauss, matBubble, matDrawBuffer, pCommonPara, E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob, E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLE, false);
	cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);
	return 0;

}
long	CInspectActive::FindDent_Top(Mat matSrc, Mat& matDrawBuffer, double* dPara, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	return 0;
}

long	CInspectActive::FindDent_Bottom(Mat matSrc, Mat& matDrawBuffer, double* dPara, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	return 0;
}

long	CInspectActive::FindScratchDefect(Mat matSrc, Mat& matDrawBuffer, double* dPara, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	return 0;
}

long	CInspectActive::FindPeelingefect(Mat matSrc, Mat& matDrawBuffer, double* dPara, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	return 0;
}

//Before
//long CInspectActive::FindActiveDefect(Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* pCommonPara, wchar_t* strAlgPath, 
//	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
//{
//
//	int nPos =0;
//	// Bubble Parameter
//	const	int		nUseBubble				=		(int)dPara[nPos++];
//	const	int		nMinimunBubbleSize		=		(int)dPara[nPos++];	// ���忡 �־ �ǳ�, ���ʿ��� ����� �����ϱ� ���Ͽ� �Ķ���ͷ� ���
//
//	// Peeling Parameter
//	const	int		nUsePeeling				=		(int)dPara[nPos++];
//
//	// Scratch Parameter
//	const	int		nUseScratch				=		(int)dPara[nPos++];
//
//	// Stamped Parameter
//	const	int		bUseDent_Top			=		(int)dPara[nPos++];
//
//	// Stamped Parameter
//	const	int		bUseDent_Bottom			=		(int)dPara[nPos++];
//
//	if(matSrcBuffer.empty())
//	{
//		// ���� ���� �ڵ� �߰� 
//		return 1;
//	}
//
//	Mat matInspActive = matSrcBuffer(rtInspArea);
//	Mat matInspDraw  = matDrawBuffer(rtInspArea);
//	
//	Mat matEnhance;
//
//	if(true)
//		imwrite("D:\\Active Original.bmp", matInspActive);
//
//	// ���μ� ��ȭ�� ���� ��ó�� �˰���
//	ImagePreprecessing(matInspActive, matEnhance);
//
//	if(true)
//		imwrite("D:\\Active Enhance.bmp", matEnhance);
//
//	int nPrevDefectCount = pResultBlob->nDefectCount;
//
//	// ��ũ��ġ
//	if( nUseScratch )
//	{
//		FindScratchDefect(matEnhance, matInspDraw, dPara, pCommonPara,  strAlgPath, EngineerDefectJudgment, pResultBlob, strContourTxt);
//	}
//	
//	//Ż�� 
//	if( nUsePeeling )
//	{
//		FindPeelingefect(matEnhance, matInspDraw, dPara, pCommonPara,  strAlgPath, EngineerDefectJudgment, pResultBlob, strContourTxt);
//	}
//	
//	// ���� 
//	if( nUseBubble )
//	{
//		FindBubbleDefect(matEnhance, matInspDraw, nMinimunBubbleSize, pCommonPara,  strAlgPath, EngineerDefectJudgment, pResultBlob, strContourTxt);
//	}
//
//	// ����
//	if( bUseDent_Bottom)
//	{
//		FindDent_Bottom(matEnhance, matInspDraw, dPara, pCommonPara,  strAlgPath, EngineerDefectJudgment, pResultBlob, strContourTxt);
//	}
//
//
//
//
//	int nCurrDefectCount = pResultBlob->nDefectCount;
//
//	for(long n =nPrevDefectCount; n < nCurrDefectCount ;n++ )
//	{
//		pResultBlob->ptLT[n].x		+= rtInspArea.x;
//		pResultBlob->ptLT[n].y		+= rtInspArea.y;
//		pResultBlob->ptRT[n].x		+= rtInspArea.x;	
//		pResultBlob->ptRT[n].y		+= rtInspArea.y;
//		pResultBlob->ptRB[n].x		+= rtInspArea.x;
//		pResultBlob->ptRB[n].y		+= rtInspArea.y;
//		pResultBlob->ptLB[n].x		+= rtInspArea.x;
//		pResultBlob->ptLB[n].y		+= rtInspArea.y;
//	}
//
//
//
//
//	return 0;
//}


//After
long CInspectActive::FindActiveDefect2(Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Rect rtInspArea, double* dParam, int* pCommonPara, wchar_t* strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, vector<vector<Point2i>> ptRound, wchar_t* strContourTxt)
{
	// LOG -> Active Start
	writeInspectLog(E_ALG_TYPE_APP, __FUNCTION__, _T("Active Start"));

	const int nImageNum = pCommonPara[10];		//���� Pattern ��ȣ
	
#pragma region 0. Active Parameter Set
	
	const	float	fScale					=	(float)dParam[INSPECTION_PARAM_SCALE];						// 0.5
			int		nBlurSize				=	(int)dParam[INSPECTION_PARAM_BLUR_SIZE];					// GaussianBlur SIZE
	const	int		nBlurSigma				=	(int)dParam[INSPECTION_PARAM_BLUR_SIGMA];					// GaussianBlur SigmaX

			int		nRoundDilate			=	(int)dParam[INSPECTION_PARAM_ROUND_DILATE];					// Round ���� ũ�⸦ Ű�� Dilate Mask ũ��
			int		nRoundErode				=	(int)dParam[INSPECTION_PARAM_ROUND_ERODE];					// Round ���� ũ�⸦ ���� Erode Mask ũ��
	const	bool	bRoundCell				=	dParam[INSPECTION_PARAM_ROUND_CELL] > 0 ? true : false;		// Round Cell ����

	const	int		nEdgeThreshold			=	(int)dParam[INSPECTION_PARAM_EDGE_THRESHOLD];				// 30
			int		nEdgeDilate				=	(int)dParam[INSPECTION_PARAM_EDGE_DILATE];					// Edge ���� Dilate Mask ũ��
			int		nEdgeErode				=	(int)dParam[INSPECTION_PARAM_EDGE_ERODE];					// Edge ���� Erode Mask ũ��

			
	const	int		nFaultySize				=	(int)dParam[INSPECTION_PARAM_FAULTY_SIZE];					// �ҷ� ������


	const	int		nOutside				=	(int)dParam[INSPECTION_PARAM_HULL_BOUND_OUTSIDE];			//10

	const	int		nHillSize				=	(int)dParam[INSPECTION_PARAM_HILL_SIZE];					// ���� ���� Max Size
	const	float	fCenterDistance			=	(float)dParam[INSPECTION_PARAM_HILL_CENTER_DISTANCE];		// ������ �߽ɿ��� �󸶳� ������ �ִ����� ����
	const	float	fStandardDeviation		=	(float)dParam[INSPECTION_PARAM_HILL_STANDARD_DEVIATION];	// 5

			double	dAxisRatio				=	(double)dParam[INSPECTION_PARAM_AXIS_RATIO];				// �伺���� ���� ó�� �� ���̸� �󸶳� �� ���к��� ����ó���� �������� ���� ���� ��

	const	double	dMinSubstanceSize		=	(double)dParam[INSPECTION_PARAM_MIN_SUBATANCE];				// 0
	const	double	dMaxSubstanceSize		=	(double)dParam[INSPECTION_PARAM_MAX_SUBATANCE];				// 5

			int		nBubbleDustMinArea		=	(int)dParam[INSPECTION_PARAM_BUBBLE_DUST_MINAREA];			// �����̹� ���� �̹� �ּ� ������ ����
			float	fBubbleDustThreshold	=	(float)dParam[INSPECTION_PARAM_BUBBLE_DUST_THRESHOLD];		// �����̹� ���� �̹� ã�� ����ȭ ��

	const	int		nBadAreaSize			=	(int)dParam[INSPECTION_PARAM_BAD_AREA_SIZE];				// 
	const	int		nBadGraySize			=	(int)dParam[INSPECTION_PARAM_BAD_GRAY_SIZE];				// 

	int	nNoneInspArea[E_PANEL_DIRECTION_MAX];
	nNoneInspArea[0] = (int)dParam[INSPECTION_PARAM_NONE_INSPECTION_L];
	nNoneInspArea[1] = (int)dParam[INSPECTION_PARAM_NONE_INSPECTION_T];
	nNoneInspArea[2] = (int)dParam[INSPECTION_PARAM_NONE_INSPECTION_R];
	nNoneInspArea[3] = (int)dParam[INSPECTION_PARAM_NONE_INSPECTION_B];

	if (nBlurSize % 2 == 0)
	{
		nBlurSize++;
	}

	if (nEdgeDilate % 2 == 0)
	{
		nEdgeDilate++;
	}

	if (nEdgeErode % 2 == 0)
	{
		nEdgeErode++;
	}

	if (nRoundDilate % 2 == 0)
	{
		nRoundDilate++;
	}

	if (nRoundErode % 2 == 0)
	{
		nRoundErode++;
	}
	
	// �˰��� �߰������ �̹����� �������� ���θ� ����
	bool bImageSave	= (pCommonPara[1] > 0) ? true : false;

	cv::Mat matInspActive = matSrcBuffer(rtInspArea);
	cv::Mat matInspDraw  = matDrawBuffer(rtInspArea);

	cv::Mat matGray;
	matInspActive.copyTo(matGray);

	// Gray �̹��� ���� Ȯ��
	if (matGray.type() != CV_8UC1) 
	{
		// Gray �̹����� ��ȯ 
		cv::cvtColor(matGray, matGray, CV_BGR2GRAY);
	}

#pragma endregion


#pragma region 1. Bruring - RGB Pattern ����

	//////////////////////////////////////////////////////////////////////////
	// Resize - Tact Time ��ҿ�
	cv::resize(matGray, matGray, cv::Size(), fScale, fScale);
	
	// Resize �̹��� ����
	InspectionStepImage(matGray, bImageSave, INSPECTION_ACTIVE_GAUSSIAN_RESIZE, strAlgPath, nImageNum);


	//////////////////////////////////////////////////////////////////////////
	// GaussianBlur ����
	cv::GaussianBlur(matGray, matGray, cv::Size(nBlurSize, nBlurSize), nBlurSigma);

	// GaussianBlur ���� �̹��� ����
	InspectionStepImage(matGray, bImageSave, INSPECTION_ACTIVE_GAUSSIAN, strAlgPath, nImageNum);


	//////////////////////////////////////////////////////////////////////////
	// Active Mask ����
	Mat mtRoundMask = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());
	if (bRoundCell == true)
	{
		if (ptRound.size() > 0)
		{
			for (int nIndex = 0; nIndex < (int)ptRound.size(); nIndex++)
			{
				cv::drawContours(mtRoundMask, ptRound, nIndex, cv::Scalar(255), -1);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Active Mask Size ������ Morphology
	if (nRoundDilate >= 3)
	{
		cv::morphologyEx(mtRoundMask, mtRoundMask, MORPH_DILATE, getStructuringElement(MORPH_RECT, Size(nRoundDilate, nRoundDilate)));
	}

	if (nRoundErode >= 3)
	{
		cv::morphologyEx(mtRoundMask, mtRoundMask, MORPH_ERODE, getStructuringElement(MORPH_RECT, Size(nRoundErode, nRoundErode)));
	}

	mtRoundMask(rtInspArea).copyTo(mtRoundMask);


	//////////////////////////////////////////////////////////////////////////
	// Resize - Tact Time ��ҿ�
	cv::resize(mtRoundMask, mtRoundMask, cv::Size(), fScale, fScale);

	// RoundMask �̹��� ����
	InspectionStepImage(mtRoundMask, bImageSave, INSPECTION_ACTIVE_ROUNDMASK, strAlgPath, nImageNum);

#pragma endregion


#pragma region 2.1 Edge �˻�

	cv::Mat matEdge = cMem->GetMat(matGray.size(), matGray.type());
	cv::Mat matResult; 
	cv::cvtColor(matGray, matResult, CV_GRAY2BGR);

	//////////////////////////////////////////////////////////////////////////
	// Sobel Edge ����
	Edge_BasicSobel(matGray, matEdge);

	// Sobel Edge ������ mEdgeVal ������ ����ȭ
	cv::threshold(matEdge, matEdge, nEdgeThreshold, 255, CV_THRESH_BINARY);

	// Edge �̹��� ����
	InspectionStepImage(matEdge, bImageSave, INSPECTION_ACTIVE_EDGE_IMAGE, strAlgPath, nImageNum);
	
	// bitwise_and ���� ���� (�����ڴ� �Ѵ� 0�� �ƴ� ��츸 ���� ��� ��Ŵ)
	cv::bitwise_and(matEdge, mtRoundMask, matEdge);

	// bitwise_and ���� ���� �̹��� ����
	InspectionStepImage(matEdge, bImageSave, INSPECTION_ACTIVE_ROUNDMASK_EDGE_IMAGE, strAlgPath, nImageNum);
		

	if (nNoneInspArea[E_PANEL_LEFT] > 0 || nNoneInspArea[E_PANEL_TOP] > 0 || nNoneInspArea[E_PANEL_RIGHT] > 0 || nNoneInspArea[E_PANEL_BOTTOM] > 0)
	{
		cv::Rect rtNoneInsp;
		rtNoneInsp = GetNoneInspROI(nNoneInspArea, cv::Rect(0, 0, matEdge.cols, matEdge.rows), fScale);

		matEdge(rtNoneInsp).setTo(0);
	}

#pragma endregion


#pragma region 2.2 Edge ���� �� �߻��� Noise ����
	
	//////////////////////////////////////////////////////////////////////////
	// Closing ����
	if (nEdgeDilate > 2)
	{
		// MORPH_DILATE ��â ���� ����
		cv::morphologyEx(matEdge, matEdge, MORPH_DILATE, cv::getStructuringElement(MORPH_RECT, cv::Size(nEdgeDilate, nEdgeDilate)));

		int nErodeOffset = 2;
		if ((nEdgeDilate - nErodeOffset) > 2)
		{
			// MORPH_ERODE ħ�� ���� ����
			cv::morphologyEx(matEdge, matEdge, MORPH_ERODE, cv::getStructuringElement(MORPH_RECT, cv::Size(nEdgeErode - nErodeOffset, nEdgeErode - nErodeOffset)));
		}
	}
	
	// Closing ���� �� �̹��� ����
	InspectionStepImage(matEdge, bImageSave, INSPECTION_ACTIVE_EDGE_CLOSING_IMAGE, strAlgPath, nImageNum);


	// LOG -> Active PreProcess End
	writeInspectLog(E_ALG_TYPE_APP, __FUNCTION__, _T("Active PreProcess End"));




	vector<vector<cv::Point> > contours_edge;
	vector<cv::Vec4i> hierarcy;

	// ���� ������ ���� �� �̹��� �ܰ��� ���� ����
	//cv::resize(matGray,matGray,cv::Size(),1/fScale,1/fScale);
	//cv::resize(matEdge,matEdge,cv::Size(),1/fScale,1/fScale);
	//cv::morphologyEx(matEdge,matEdge,MORPH_ERODE,cv::getStructuringElement(MORPH_RECT,cv::Size(nEdgeErode,nEdgeErode)));

	findContours(matEdge, contours_edge, hierarcy, 0, CV_CHAIN_APPROX_NONE);

	vector<vector<cv::Point> >hull(contours_edge.size());
	cv::Mat drawhull = cMem->GetMat(matGray.size(), matGray.type());

	//�ܰ��� �ö�� �ҷ� �ĺ������� Edge�� ����� �� ���� �ܰ��� ���� Contour������ �Ȼ���� ������
	//�ܰ��� �ö�� �ҷ� �ĺ����鸸 Convexhull�� ������.
#pragma omp parallel for
	for (int i = 0; i < (int)contours_edge.size(); i++)
	{
		cv::Rect rtContous_Bound_Box = boundingRect(contours_edge[i]);
		if (rtContous_Bound_Box.x < (0 + nOutside) || rtContous_Bound_Box.y < (0 + nOutside) || (rtContous_Bound_Box.x + rtContous_Bound_Box.width) > (matGray.cols - nOutside) || (rtContous_Bound_Box.y + rtContous_Bound_Box.height) > (matGray.rows - nOutside))
		{
			// �����Ǿ��ִ� �������� ���� �ֿܰ��� �ִ� ������ ���̴� ������ ū ������ �� ����
			convexHull(cv::Mat(contours_edge[i]), hull[i], false);
		}
		else
		{
			hull[i] = contours_edge[i];
		}
	}

	if (bImageSave == true)
	{
		for (int i = 0; i < (int)contours_edge.size(); i++)
		{
			drawContours(drawhull, hull, i, 255, CV_FILLED, 8, vector<cv::Vec4i>(), 0, cv::Point());
		}

		// convexHull ���� �� �̹��� ����
		InspectionStepImage(drawhull, bImageSave, INSPECTION_ACTIVE_EDGE_CONVEXHULL, strAlgPath, nImageNum);
	}

	// LOG -> Active Contour and convexhull End
	writeInspectLog(E_ALG_TYPE_APP, __FUNCTION__, _T("Active Contour and convexhull End"));

#pragma endregion


#pragma region 3. Feature extract

	VectorDefectInfo vDefect;

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int i = 0; i < (int)hull.size(); i++ )
	{
		DEFECTINFO mDefect;

		cv::Point2f ptCenter;
		cv::Scalar mean, std, BKmean, BKstd;
		float fRadius = 0;

		// contour�� �����ϴ� ���� �� ���� �߽���ǥ��, �������� ����
		cv::minEnclosingCircle(hull[i], ptCenter, fRadius);
		mDefect.rect = boundingRect(hull[i]);

		Mat mask_edge = cMem->GetMat(cv::Size(mDefect.rect.width, mDefect.rect.height), matGray.type());
		drawContours(mask_edge, hull, i, 255, CV_FILLED, 8, vector<cv::Vec4i>(), 0, cv::Point(-mDefect.rect.tl().x, -mDefect.rect.tl().y));

		Mat mask_BK = cMem->GetMat(cv::Size(mDefect.rect.width + 10, mDefect.rect.height + 10), matGray.type());
		drawContours(mask_BK, hull, i, 255, CV_FILLED, 8, vector<cv::Vec4i>(), 0, cv::Point(-mDefect.rect.tl().x + 5, -mDefect.rect.tl().y + 5));

		// ���� TRUE�̸� ����� FALSE, FALSE�̸� ����� TRUE.
		cv::bitwise_not(mask_BK, mask_BK);

		//getClosedValue(Gray, center, radius, closedValue);	//������+15 ��ŭ ��� ���� �� �޾ƿ���

		// src�� ��հ� ǥ�������� ä�κ��� mean�� stddev�� ����Ѵ�.
		cv::meanStdDev(matGray(mDefect.rect), mean, std, mask_edge);

		//cv::Rect rtBK = Rect(mDefect.rect.x - 5, mDefect.rect.y - 5, mDefect.rect.width + 5, mDefect.rect.height + 5);
		cv::meanStdDev(matGray(mDefect.rect), BKmean, BKstd, mask_BK);

		double	dMin, dMax;

		// minMaxIdx �ְ�, �ִ밪��
		cv::minMaxIdx(matGray(mDefect.rect), &dMin, &dMax, NULL, NULL, mask_edge);
		mDefect.center					= ptCenter;																			// �ҷ� �߽�
		mDefect.area					= cv::contourArea(hull[i]);															// �ҷ� ����
		mDefect.mean					= mean[0];																			// �ҷ� ��� GV��
		mDefect.std						= std[0];																			// �ҷ� GV�� ǥ�� ����
		mDefect.min						= dMin;																				// �ҷ� �ּ� GV
		mDefect.max						= dMax;																				// �ҷ� �ִ� GV
		mDefect.rotated_rect			= minAreaRect(hull[i]);																// �ҷ��� ������ ����� �ҷ��� ���� �� �ִ� �ּ� Size ����								
		mDefect.perimeter				= arcLength(hull[i], true);															// �ҷ��� �ѷ�����
		mDefect.rougness				= (4 * pi * mDefect.area) / pow(mDefect.perimeter, 2);								// �ҷ��� ������ (1�̸� ��)
		mDefect.compactness				= pow(mDefect.perimeter, 2) / ( 4 * pi * mDefect.area);								// �ҷ��� �󸶳� �����ϳ�
		mDefect.length					= fRadius;																			// �ҷ��� ����
		mDefect.closedValueAvg			= BKmean[0];																		// ��� ��� GV��
		mDefect.closedValueSTD			= BKstd[0];																			// ��� GV�� ǥ�� ����
		mDefect.subDimension			= (abs)((pow(fRadius / 2, 2) * pi) / 4) - mDefect.area;								// Pass �Ⱦ�
		mDefect.MaxSubClosedIntensity	= (mean[0]) - mDefect.closedValueAvg;												// �ҷ� ��� GV�� - ��� ��� GV ��
		mDefect.MinSubClosedIntensity	= (std[0]) - mDefect.closedValueSTD;												// �ҷ� ��� GV�� - ��� ��� GV ��
		mDefect.centerDistance			= getDistance(cv::Point(matGray.cols / 2, matGray.rows / 2), ptCenter);				// ������ �߽ɿ��� �ҷ� �߽� ������ �Ÿ�

		if (mDefect.rotated_rect.size.width > mDefect.rotated_rect.size.height)
		{
			mDefect.Axis_Ratio			=  mDefect.rotated_rect.size.width / mDefect.rotated_rect.size.height;				//�庯 ���� / �ܺ� ����
		}
		else
		{
			mDefect.Axis_Ratio			= mDefect.rotated_rect.size.height / mDefect.rotated_rect.size.width;				// �ܺ� ���� / �庯 ����
		}

		mDefect.contour					= hull[i];																			// �ҷ� ��ǥ �迭
		mDefect.name					= E_NONE_NAME;																		// �ҷ� Name
		mDefect.type					= E_NONE_TYPE;																		// �ҷ� Type
		mDefect.unUse					= false;

		vDefect.push_back(mDefect);		
	}

	int nFeatureSize = (int)vDefect.size();
	if (nFeatureSize > 100)
	{
		return E_ERROR_CODE_FALSE;
	}

	cv::Mat mBlobResult[E_ACTIVE_DEFECT_NUM];
	vector<vector<cv::Point>> ptDefectContours[E_ACTIVE_DEFECT_NUM];
	for (int i = E_Bubble; i < E_ACTIVE_DEFECT_NUM; i++)
	{
		mBlobResult[i]	= cMem->GetMat(matGray.size(), matGray.type());
	}

	cv::Mat mAcitveParameterImg;
	cv::Mat mtFeature;
	if (bImageSave == true)
	{
		matGray.copyTo(mAcitveParameterImg);
		matGray.copyTo(m_mtDrawHill[nImageNum]);
		cv::cvtColor(m_mtDrawHill[nImageNum], m_mtDrawHill[nImageNum], CV_GRAY2BGR);
		matGray.copyTo(mtFeature);
		cv::cvtColor(mtFeature, mtFeature, CV_GRAY2BGR);

		vectordefectinfo_it it = vDefect.begin();
		for (int i = 0 ; it != vDefect.end(); ++it, i++)
		{
			DEFECTINFO defect = *it;

			CString strInx;
			strInx.Format(_T("%d"), i);
			cv::putText(mtFeature, (cv::String)(CStringA)strInx, cv::Point(defect.center.x, defect.center.y), FONT_HERSHEY_SIMPLEX, 1., cv::Scalar(255,0,0), 1);

			CString strFileName;
			strFileName.Format(_T("%d_Active_Feature.csv"), nImageNum);
			CString strColum;
			strColum.Format(_T("INDEX,AREA,LENGTH,CENTER,MIN,MAX,MEAN,STD,MAX_SUB,MIN_SUB,BG_MEAN,BG_STD,PERIMETER,ROUNDNESS,COMPACTNESS,CENTERDISTANCE,AXIS_RATIO"));
			CString strData;
			strData.Format(_T("%d, %.2lf, %.2lf, %.0f_%.0f, %.0f, %.0f, %.2lf,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf"), 
				i,
				defect.area,
				defect.length,
				defect.center.x, vDefect[i].center.y,
				defect.min,
				defect.max,
				defect.mean,
				defect.std,
				defect.MaxSubClosedIntensity,
				defect.MinSubClosedIntensity,
				defect.closedValueAvg,
				defect.closedValueSTD,
				defect.perimeter,
				defect.rougness,
				defect.compactness,
				defect.centerDistance,
				defect.Axis_Ratio);
			WriteResultFile(strAlgPath, strFileName, strColum, (TCHAR*)(LPCTSTR)strData);
		}
	}

	TCHAR tcFeatureNum[250];
	memset(tcFeatureNum, NULL, 250);
	_stprintf_s(tcFeatureNum, _T("Active Feature Save End [FeatureNum: %d]"), (int)vDefect.size());
	
	// LOG -> Active Feature Save End
	writeInspectLog(E_ALG_TYPE_APP, __FUNCTION__, tcFeatureNum);

#pragma endregion


#pragma region 4. Classification

	vectordefectinfo_it it = vDefect.begin();
	
#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int i = 0; i < (int)vDefect.size(); i++)
	{
		DEFECTINFO defect = vDefect.at(i);

		if (defect.area < 40)
		{
			continue;
		}

		// ���� ���� ���� Ȯ��
		if (CheckHill(matGray, defect, nHillSize, fStandardDeviation, fCenterDistance, nImageNum, bImageSave) == true)
		{
			continue;
		}

		// ���� ������ �������п� ������ ����
		if (defect.Axis_Ratio > dAxisRatio)
		{
			ptDefectContours[E_Scratch].push_back(hull[i]);
		}

		bool bState = false;
		if ((defect.area > nFaultySize) && (defect.length < 10) && (defect.Axis_Ratio < 1.3)  && (defect.MaxSubClosedIntensity > 0) && (defect.mean > 140))
		{
			bState = true;
		}

		// �������� ������..
		// ����ũ�� �̻��̸� ������ ū���...(���������� �����ִ� ū ����)
		if ((defect.MaxSubClosedIntensity > dMaxSubstanceSize) || (defect.area > nBadAreaSize) && (nBadGraySize < (defect.max - defect.min)) || (bState == true))
		{
			// Ż�� ���� Ȯ��
			if((defect.rect.x < nOutside) || (defect.rect.x + defect.rect.width > matGray.cols - nOutside) || (defect.rect.y < nOutside) || (defect.rect.y + defect.rect.height > matGray.rows - nOutside))
			{
				ptDefectContours[E_FilmPeeling].push_back(hull[i]);
				defect.type	= E_WHITE;
				defect.name	= E_FilmPeeling;
			}
			// �̹� ���� & ���� ���� Ȯ��
			else
			{
				cv::Mat mask_edge = cMem->GetMat(cv::Size(defect.rect.width, defect.rect.height), matGray.type());
				drawContours(mask_edge, hull, i, 255, CV_FILLED, 8, vector<cv::Vec4i>(), 0, cv::Point(-defect.rect.tl().x, -defect.rect.tl().y));

				//���� �̹�Ȯ��
				bool bBubbleSubstance = false;

				cv::Mat BubbleIArea = cMem->GetMat(cv::Size(defect.rect.width, defect.rect.height), matGray.type());
				//BubbleIArea.setTo(255);
				matGray(defect.rect).copyTo(BubbleIArea, mask_edge);

				cv::Mat mThreshold;
				double blackValue = defect.mean * fBubbleDustThreshold;
				cv::threshold(BubbleIArea, mThreshold, blackValue, 255, CV_THRESH_BINARY_INV);

				vector<vector<cv::Point> > contours_black;
				vector<cv::Vec4i> hierarcy_black;
				cv::findContours(mThreshold, contours_black, hierarcy_black, 0, CV_CHAIN_APPROX_NONE);

				cv::Point2f center_black;
				float fRadius_black = 0; 
				for (int j = 0; j < (int)contours_black.size(); j++) 
				{
					int nArea = cv::contourArea(contours_black[j]);
					if (nArea < nBubbleDustMinArea)
					{
						continue;
					}

					// contour�� �����ϴ� ���� �� ���� �߽���ǥ��, �������� ����
					minEnclosingCircle(contours_black[j], center_black, fRadius_black);

					double dDistance = getDistance(cv::Point(BubbleIArea.cols / 2, BubbleIArea.rows / 2), center_black);
					if (dDistance < defect.length / 4)
					{
						bBubbleSubstance = true;
					}
				}

				// �̹� ���� ���� Ȯ��
				if (bBubbleSubstance == true)
				{
					ptDefectContours[E_BubbleSubstance].push_back(hull[i]);
					defect.type	= E_WHITE;
					defect.name	= E_BubbleSubstance;
				}
				// ���� ���� Ȯ��
				else
				{
					ptDefectContours[E_Bubble].push_back(hull[i]);
					defect.type	= E_WHITE;
					defect.name	= E_Bubble;
				}
			}
		}

		// �̹� ���� Ȯ��
		if (defect.MinSubClosedIntensity > dMinSubstanceSize && defect.type == E_NONE_TYPE)
		{
			ptDefectContours[E_Substance].push_back(hull[i]);
			ptDefectContours[E_Scratch].push_back(hull[i]);
			defect.type	= E_BLACK;
			defect.name	= E_Substance;
		}
	}

	// LOG -> Active Feature Save End
	writeInspectLog(E_ALG_TYPE_APP, __FUNCTION__, _T("Active Classify End"));

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int nDefectInx = 0; nDefectInx < E_ACTIVE_DEFECT_NUM; nDefectInx++)
	{
		 //��������(����x)
		if (nDefectInx == E_HILL)
		{
			continue;
		}

		 //���� ��������
		if (nDefectInx == E_MURA)
		{
			continue;
		}

		fillPoly(mBlobResult[nDefectInx], ptDefectContours[nDefectInx], cv::Scalar(255));
	}



	// FilmPeeling �̹��� ����
	InspectionStepImage(mBlobResult[E_FilmPeeling], bImageSave, INSPECTION_ACTIVE_FILMPEELING, strAlgPath, nImageNum);

	// BubbleSubstance �̹��� ����
	InspectionStepImage(mBlobResult[E_BubbleSubstance], bImageSave, INSPECTION_ACTIVE_BUBBLE_SUBSTANCE, strAlgPath, nImageNum);

	// Bubble �̹��� ����
	InspectionStepImage(mBlobResult[E_Bubble], bImageSave, INSPECTION_ACTIVE_BUBBLE, strAlgPath, nImageNum);

	// Substance �̹��� ����
	InspectionStepImage(mBlobResult[E_Substance], bImageSave, INSPECTION_ACTIVE_SUBSTANCE, strAlgPath, nImageNum);

	// Check_Hill �̹��� ����
	InspectionStepImage(m_mtDrawHill[nImageNum], bImageSave, INSPECTION_ACTIVE_CHECKHILL, strAlgPath, nImageNum);

	// Feature_label �̹��� ����
	InspectionStepImage(mtFeature, bImageSave, INSPECTION_ACTIVE_FEATURELABEL, strAlgPath, nImageNum);
	
#pragma endregion


#pragma region 5. CRUX Labeling

	// 5. CRUX Labeling
	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem(cMem);

	int nPrevDefectCount = pResultBlob->nDefectCount;
	int nTemp = 0;

	for (int i = E_Bubble; i < E_ACTIVE_DEFECT_NUM; i++)
	{
		if (ptDefectContours[i].size() != 0)
		{
			//��������(����x)
			if (i == E_HILL)
			{
				continue; 
			}

			//���� ��������
			if (i == E_MURA)
			{
				continue;
			}

			Mat ResultMat = Mat::zeros(matInspActive.size(), CV_8UC3);

			cv:: resize(mBlobResult[i], ResultMat, ResultMat.size());
			cFeatureExtraction.DoDefectBlobSingleJudgment(matInspActive, ResultMat, matInspDraw, pCommonPara, E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob, i + E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLE, false);
			cFeatureExtraction.DrawBlob(matInspDraw, cv::Scalar(0,0,255), BLOB_DRAW_BLOBS_CONTOUR, true);

			for (long n = nTemp; n < pResultBlob->nDefectCount; n++)
			{
				pResultBlob->ptLT[n].x		+= rtInspArea.x;
				pResultBlob->ptLT[n].y		+= rtInspArea.y;
				pResultBlob->ptRT[n].x		+= rtInspArea.x;	
				pResultBlob->ptRT[n].y		+= rtInspArea.y;
				pResultBlob->ptRB[n].x		+= rtInspArea.x;
				pResultBlob->ptRB[n].y		+= rtInspArea.y;
				pResultBlob->ptLB[n].x		+= rtInspArea.x;	
				pResultBlob->ptLB[n].y		+= rtInspArea.y;
				pResultBlob->nCenterx[n]	+= rtInspArea.x;
				pResultBlob->nCentery[n]	+= rtInspArea.y;

				for (int nCt = 0; nCt < MAX_CONTOURS; nCt++)
				{
					if (pResultBlob->nContoursX[n][nCt] == 0 && pResultBlob->nContoursY[n][nCt] == 0)
					{
						break;
					}

					pResultBlob->nContoursX[n][nCt] += rtInspArea.x;
					pResultBlob->nContoursY[n][nCt] += rtInspArea.y;
				}				

				nTemp++;
			}

			// �ܰ� ���� ����ϴ� ���, Judgement()���� ���� ���� ( �ߺ� ���Ž�, �ش� �ҷ� �ܰ��� �ȱ׸� )
			// ��� ���ϴ� ���, Alg�ܿ��� ���� ���� ( �ߺ� ���ŵǵ�, �ش� �ҷ� �ܰ��� �׸� )
			// ��� �ܰ��� ����
			if (!USE_ALG_CONTOURS)
			{
				cFeatureExtraction.SaveTxt(pCommonPara, strContourTxt, true);
			}
		}
	}

	// Draw �̹��� ����
	InspectionStepImage(matInspDraw, bImageSave, INSPECTION_ACTIVE_DRAW, strAlgPath, nImageNum);

	// LOG -> Active Judge End
	writeInspectLog(E_ALG_TYPE_APP, __FUNCTION__, _T("Active Judge End"));

#pragma endregion
	
	return E_ERROR_CODE_TRUE;
}



bool CInspectActive::SubPoly (cv::Mat src, cv::Mat dst, int nLineDirection)
{	
	if(nLineDirection == E_VERTICAL)
	{
#pragma omp parallel for
		for(int iw=0; iw<src.cols; iw++){
			cv::Mat R;

			FindPolynomial(src, R, iw, 1);

			for(int h=0; h<src.rows; h++){
				int FVal = 	(R.at<float>(0, 0)*pow((float)h,3)) +
					(R.at<float>(1, 0)*pow((float)h,2)) +
					(R.at<float>(2, 0)*pow((float)h,1)) +
					R.at<float>(3, 0);	

				dst.data[h*dst.step+iw] = FVal;
			}
		}
	}
	else
	{
#pragma omp parallel for
		for(int ih=0; ih<src.rows; ih++){
			cv::Mat R;

			FindPolynomial_H(src, R, ih, 1);

			for(int w=0; w<src.cols; w++){
				int FVal = 	(R.at<float>(0, 0)*pow((float)w,3)) +
					(R.at<float>(1, 0)*pow((float)w,2)) +
					(R.at<float>(2, 0)*pow((float)w,1)) +
					R.at<float>(3, 0);	

				dst.data[ih*dst.step+w] = FVal;
			}
		}
	}
	
	writeInspectLog(E_ALG_TYPE_APP, __FUNCTION__, _T("Active End"));

	return true;
}


bool CInspectActive::FindPolynomial(cv::Mat img, cv::Mat &R, int width, const int sample)
{
	const int		w			= img.cols;
	const int		h			= img.rows;
	const int		step		= img.step;
	const int		sampleSize	= h/sample;

	cv::Mat	X(sampleSize,4,CV_32FC1);
	cv::Mat	X_INV(4,sampleSize,CV_32FC1);
	cv::Mat	Y(sampleSize,1,CV_32FC1);

	int v = 0;
	/*#pragma omp parallel for*/
	for(int i=0; i<sampleSize; i++) {

		X.at<float>(i,0) = (float)pow((float)v,3);
		X.at<float>(i,1) = (float)pow((float)v,2);
		X.at<float>(i,2) = (float)v;
		X.at<float>(i,3) = 1.0;

		Y.at<float>(i, 0) = (float)img.data[v*step+width];
		v=i*sample;
	}

	X_INV = X.inv(cv::DECOMP_SVD);
	R = X_INV * Y;
	
	return true;
}

bool CInspectActive::FindPolynomial_H(cv::Mat img, cv::Mat &R, int height, const int nSample)
{
	const int		w			= img.cols;
	const int		h			= img.rows;
	const int		step		= img.step;
	const int		sampleSize	= w;

	cv::Mat	X(sampleSize,4,CV_32FC1);
	cv::Mat	X_INV(4,sampleSize,CV_32FC1);
	cv::Mat	Y(sampleSize,1,CV_32FC1);
	

	int v = 0;
	/*#pragma omp parallel for*/
	for(int i=0; i<sampleSize; i++) {

		X.at<float>(i,0) = (float)pow((float)v,3);
		X.at<float>(i,1) = (float)pow((float)v,2);
		X.at<float>(i,2) = (float)v;
		X.at<float>(i,3) = 1.0;

		Y.at<float>(i, 0) = (float)img.data[height*step+v];
		v=i*nSample;
	}

	X_INV = X.inv(cv::DECOMP_SVD);
	R = X_INV * Y;

	return true;
}

double CInspectActive::getAngle(cv::Point p1, cv::Point p2)
{
		double angle =  (atan2((double)(p2.y-p1.y), (double)(p2.x-p1.x)))*(180.0f/pi);

	return angle;
}


bool CInspectActive::DefectClassification(vector<DEFECTINFO> &vDefect, cv::Mat temp, int mXCenter, int mYCenter)
{
	for(int i = 0; i < vDefect.size(); i++ )
	{
		if((vDefect.at(i).name==E_Bubble)&&(vDefect.at(i).area>1000)) continue;

		for(int j = 0; j < vDefect.size(); j++ ){
			
			// Active ������� �з�
			if(i==j) continue;
			
			double dist = getDistance(vDefect.at(i).center,  vDefect.at(j).center);

			int SerachRate = 2;			//parameter�� ��������.. ���� ����..
			int MinSerachArea = 100;
			int MaxSerachArea = 500;
			double SerachArea1 = vDefect.at(i).length*SerachRate;
			if(SerachArea1<MinSerachArea) SerachArea1 = MinSerachArea;
			if(SerachArea1>MaxSerachArea) SerachArea1 = vDefect.at(i).length * 1.1;
			double SerachArea2 = vDefect.at(j).length*SerachRate;
			if(SerachArea2<MinSerachArea) SerachArea2 = MinSerachArea;
			if(SerachArea2>MaxSerachArea) SerachArea2 = vDefect.at(j).length * 1.1;
			/*int SerachMaxArea = 1000;
			double SerachArea1 = (1/vDefect.at(i).length);
			double SerachArea2 = (1/vDefect.at(j).length);*/

			int diffLength = 100;
			if(diffLength > (abs)(vDefect.at(i).length - vDefect.at(j).length))
			{
				if((dist<SerachArea1)||(dist<SerachArea2))
				{
					cv::Point cellCenter( temp.rows/2, temp.cols/2); 
					double di;

					if((vDefect.at(i).type==E_WHITE)&&(vDefect.at(j).type==E_BLACK))
					{
						di = (abs)(getAngle(vDefect.at(i).center, vDefect.at(j).center) - getAngle(cellCenter, vDefect.at(i).center));
						vDefect.at(i).conAng = getAngle(vDefect.at(i).center, vDefect.at(j).center);

						if(vDefect.at(i).center.y<=temp.rows/2)
						{

							if(vDefect.at(i).conAng>0)
							{
								if ((abs(vDefect.at(i).center.y - vDefect.at(j).center.y) >= mXCenter ||
									abs(vDefect.at(i).center.x - vDefect.at(j).center.x) >= mXCenter )&&
									vDefect.at(i).area > 100 && vDefect.at(j).area >100) 
								{
									vDefect.at(i).conNum = j;
									vDefect.at(i).unUse = true;
									vDefect.at(j).unUse = true;
									vDefect.at(i).name	= E_HILL;
									vDefect.at(j).name	= E_HILL;

									vDefect.at(i).conCount++;
									vDefect.at(j).conCount++;
								}
							}
						}

					}else if((vDefect.at(i).type==E_BLACK)&&(vDefect.at(j).type== E_WHITE))
					{
						di = (abs)(getAngle(vDefect.at(j).center, vDefect.at(i).center) - getAngle(cellCenter, vDefect.at(j).center));
						vDefect.at(i).conAng = getAngle(vDefect.at(j).center, vDefect.at(i).center);

						if(vDefect.at(i).center.y>temp.rows/2)
						{
							if(vDefect.at(i).conAng<0)
							{
								if ((abs(vDefect.at(i).center.y - vDefect.at(j).center.y) >= mYCenter ||
									abs(vDefect.at(i).center.x - vDefect.at(j).center.x) >= mYCenter )&&
									vDefect.at(i).area > 100 && vDefect.at(j).area >100) 
								{
									vDefect.at(i).conNum = j;
									vDefect.at(i).unUse = true;
									vDefect.at(j).unUse = true;
									vDefect.at(i).name	= E_HILL;
									vDefect.at(j).name	= E_HILL;

									vDefect.at(i).conCount++;
									vDefect.at(j).conCount++;
								}
							}
						}
					}
				}


			/*		if((vDefect.at(i).type==E_WHITE)&&(vDefect.at(j).type== E_WHITE))
					{
						vDefect.at(i).conNum = j;
						vDefect.at(i).unUse = false;
						vDefect.at(j).unUse = false;
						vDefect.at(i).name	= E_Bubble;
						vDefect.at(j).name	= E_Bubble;
					}*/



				/*	if((abs)(vDefect.at(i).rotated_rect.angle - vDefect.at(j).rotated_rect.angle)<10)
					{
						vDefect.at(i).conNum = j;
						vDefect.at(i).unUse = true;
						vDefect.at(j).unUse = true;
						vDefect.at(i).name	= E_HILL;
						vDefect.at(j).name	= E_HILL;
					}*/
				}

				

				//�̹�Ȯ��
				if((vDefect.at(i).type==E_WHITE)&&(vDefect.at(j).type==E_BLACK))
				{
					int d = getDistance(vDefect.at(i).center, vDefect.at(j).center);
					if(vDefect.at(i).length/2 > d)
					{
						vDefect.at(i).unUse = false;
						vDefect.at(i).name	= E_BubbleSubstance;
					}
				}

				//MURA: BLACK TYPE MURA�� CELL���� �������� ��Ÿ���ٰ� ��....
				if((vDefect.at(i).type==E_BLACK)&&(vDefect.at(i).area > 5000)) {
					int d = getDistance(vDefect.at(i).center, vDefect.at(j).center);
					if(vDefect.at(i).length/2 > d)
					{
						if( (vDefect.at(i).center.x < 100) ||
							(vDefect.at(i).center.y < 100) ||
							(temp.cols - 100 < vDefect.at(i).center.x) ||
							(temp.rows - 100 < vDefect.at(i).center.y)  )
							{
								vDefect.at(i).name	=  E_MURA;
							}
						else{
							vDefect.at(i).unUse = false;
						}
						vDefect.at(j).unUse = false;
					}
				}

				// Ż��: BUBBLE�� CELL���� �������� ��Ÿ���ٰ� ��....
				if(vDefect.at(i).name == E_Bubble)
				{
					int dist = 50;
					if( (vDefect.at(i).rect.x < dist) ||
						(vDefect.at(i).rect.y < dist) ||
						(temp.cols - dist < (vDefect.at(i).rect.x+vDefect.at(i).rect.width)) ||
						(temp.rows - dist < vDefect.at(i).rect.y + vDefect.at(i).rect.height	))
						{
								vDefect.at(i).name	=  E_FilmPeeling;
						}
				}

		}
	}


	// ������Ȯ�� TEST
	for(int i = 0; i < vDefect.size(); i++ )	{
	//	/*if(vDefect.at(i).conCount>3){*/
	//		//����Ȯ��
	//	//if(vDefect.at(i).conCount>2)
		if((vDefect.at(i).name==E_Bubble)&&(vDefect.at(i).area<1000))
			{
				vDefect.at(i).wSum = /*(10*(vDefect.at(i).area/100))+*/
									 (50/vDefect.at(i).compactness)+ 
									 (50*vDefect.at(i).rougness);

				if(vDefect.at(i).wSum>80){
					vDefect.at(i).name = E_Bubble;
					vDefect.at(i).unUse = false;
				}
				else{
					//���� ���
					vDefect.at(i).unUse = true;
				}
			}
	//	//}
	}


	//�̹�����
	for(int i = 0; i < vDefect.size(); i++ )	{
		int mCountW = 0;
		int mCountB = 0;
		vector<int> vCountW;
		vector<int> vCountB;

		if(vDefect.at(i).type==E_WHITE)
		{
			for(int j = 0; j < vDefect.size(); j++ )
			{
				if(i==j) continue;
				int d = getDistance(vDefect.at(i).center, vDefect.at(j).center);

				if(vDefect.at(j).type==E_WHITE) {
					if(vDefect.at(i).length > d) {
						mCountW++;
						vCountW.push_back(j);
					}
				}else{
					if(vDefect.at(i).length/2 > d)
					{
						mCountB++;
						vCountB.push_back(j);
					}
						
				}
			}
		}

		if((mCountW>0)&&(mCountB>0))
		{
			vDefect.at(i).unUse = false;
			vDefect.at(i).name	= E_BubbleSubstance;
			
			for(int v=0; v<vCountW.size(); v++){
				int id = vCountW.at(v);
				vDefect.at(id).unUse = false;
				vDefect.at(id).name	= E_BubbleSubstance;
			}

			for(int v=0; v<vCountB.size(); v++){
				int id = vCountB.at(v);
				vDefect.at(id).unUse = false;
				vDefect.at(id).name	= E_BubbleSubstance;
			}
		}
						
	}
	return true;
}

void SaveResultImage(cv::Mat temp, CString str)
{
// ���� ����̺� D:\ ���� �ƴ� - �ʿ� �� InspectLibLog �� GETDRV() Ȱ��
// 	#define Path D:\\TEST\\
// 	cv::String SavePath;
// 	CString strPath;
// 	strPath.Format(_T("D:\\TEST\\")+str+_T(".jpg"));
// 	imwrite((cv::String)(CStringA)(strPath), temp);
}



bool CInspectActive::getClosedValue(cv::Mat src, cv::Point center, double radius, double value[3])
{
	double avg = 0.0;
	double variance = 0.0;
	int sum = 0;

	vector<int> val;
	double std =0.0;

	for(int degree = 0; degree < 360; degree++)
	{
		double radian = degree * pi / 180;
		int px = center.x + (int) ( radius +15)* cos(radian);
		int py = center.y + (int) ( radius +15)* sin(radian);
		if((px<=0)||(px>=src.cols)) continue;
		if((py<=0)||(py>=src.rows)) continue;

		val.push_back((int)(uchar)src.data[py*src.step+px]);	
	}

	for(int i=0; i<val.size(); i++) {
		sum += val.at(i);
	}

	if(val.size() > 0){
		avg =  (double)(sum/ val.size());
		vector<double> diviation;
		for(int i=0; i<val.size(); i++) {
			diviation.push_back(val.at(i) - avg);
		}
	
		for(int i=0; i<val.size(); i++) {
			variance += pow((double)diviation.at(i),2);
		}
		variance/=val.size();
		std = sqrt(variance);
	}
	
	value[0] = avg;
	value[1] = variance;
	value[2] = std;
	
	return true;
}

long CInspectActive::FindActiveDefect(Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* pCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{

	//Active parameter
	//const float scale		= 0.5;		//Image scale
	//const int	BlurSize	= 21;		//GaussianBlur size
	//const int	mCount_X	= 2;		//X Surface ����
	//const int	mCount_Y	= 2;		//Y Surface ����
	//const int	mWhiteVal	= 4;		//mWhite ����
	//const int	mBlackVal	= 4;		//mBlack ����

	int nPos = 0;

	const	float	scale			=	(float)dPara[nPos++];
	const	int		BlurSize		=	(int)dPara[nPos++];
	const	int		mCount_X		=	(int)dPara[nPos++];
	const	int		mCount_Y		=	(int)dPara[nPos++];
	const	int		mWhiteVal		=	(int)dPara[nPos++];
	const	int		mBlackVal		=	(int)dPara[nPos++];
	const	int		mXCenter		=	(int)dPara[nPos++];
	const	int		mYCenter		=	(int)dPara[nPos++];

	cv::String str;

	bool	bImageSave			= (pCommonPara[1] > 0) ? true : false;	// 1 : �˰��� �߰���� Image Save

	/*if(rtInspArea.width%2 >0)rtInspArea.width-=1;
	if(rtInspArea.height%2 >0)rtInspArea.height-=1;*/

// 	CString strBuffer;
// 		strBuffer.Format(_T("D:\\Crux\\DATA\\MODEL\\APP_TEST\\00_LAB_B.bmp"));
// 		Mat matInspActive = imread((cv::String)(CStringA)strBuffer, -1);
// 		Mat matInspDraw;
// 		matInspActive.copyTo(matInspDraw);
// 	
// 		if (bImageSave)
// 		{
// 			CString strTemp;
// 			strTemp.Format(_T("%s\\00_GRAY01.jpg"), strAlgPath);
// 			cv::imwrite((cv::String)(CStringA)strTemp, matInspActive);
// 	
// 			strTemp.Format(_T("%s\\00_GRAY00.jpg"), strAlgPath);
// 			cv::imwrite((cv::String)(CStringA)strTemp, matInspDraw);
// 		}

	Mat matInspActive = matSrcBuffer(rtInspArea);
	Mat matInspDraw  = matDrawBuffer(rtInspArea);

	Mat reImg;
	cv::resize(matInspActive, reImg,cv::Size(), scale, scale);

	Mat Gray;
	if(reImg.type() != CV_8UC1) {
		cv::cvtColor(reImg, Gray, CV_BGR2GRAY);
	}else{
		Gray = reImg;
	}

	//1. Bruring
	cv::GaussianBlur(Gray,Gray, cv::Size(5,5),0,0);

	//2. Sub Surface fitting
	Mat Surface;
	Surface = Mat::zeros( Gray.size(), CV_8UC1 );  
	for(int iy=0; iy<mCount_Y; iy++)
		for(int ix=0; ix<mCount_X; ix++)
		{
			int mW = Gray.cols /mCount_X;
			int mH = Gray.rows /mCount_Y;
			const int sX = mW*ix;
			const int sY = mH*iy;

			if(sX + mW > Gray.cols) mW = Gray.cols - sX;
			if(sY + mH > Gray.rows) mH = Gray.rows - sY;

			cv::Rect mRect(sX, sY, mW, mH);
			
			Mat mROI =  Mat::zeros( cv::Size(mW, mH), CV_8UC1); 
			Mat mROI2 =  Mat::zeros( cv::Size(mW, mH),CV_8UC1 );
			Gray(mRect).copyTo(mROI);
			SubPoly(mROI, mROI2, E_VERTICAL);
			Surface(mRect) += mROI2;
		}
		
	/*str = "Surface.bmp";
	imwrite(str, Surface)*/;

	// 3. White type: ����, �̹�����, SC
	Mat mWhite = Mat::zeros( Gray.size(), Gray.type() ); 
	mWhite = Gray - Surface;

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Acitve_White_Image_1.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, mWhite);
	}

	cv::threshold(mWhite, mWhite, mWhiteVal, 255, CV_THRESH_BINARY);
	
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Acitve_White_Image_2.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, mWhite);
	}

	/*Mat element(11, 11, CV_8UC1, Scalar(1));  
	morphologyEx(mWhite, mWhite, MORPH_CLOSE, element,cv::Point(5,5),3);  */
	
	// 4. Categorization
	DEFECTINFO mDefect;
	vector<DEFECTINFO> vDefect;
    vector<vector<cv::Point> > contours;
	cv::findContours(mWhite, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cv::Point(0,0));
	DefectCategorization(vDefect, contours, E_WHITE, E_Bubble);

	// 5. Black type: �̹�, MURA, SC
	Mat mBlack = Surface - Gray;

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Acitve_Black_Image_1.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, mBlack);
	}

	cv::threshold(mBlack, mBlack, mBlackVal, 255, CV_THRESH_BINARY);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Acitve_Black_Image_2.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, mBlack);
	}

	// 6. Categorization
	vector<vector<cv::Point> > contours2;
	cv::findContours(mBlack, contours2, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cv::Point(0,0));
	DefectCategorization(vDefect, contours2, E_BLACK, E_Substance);
	
	// 7. DefectClassification
	DefectClassification(vDefect, Gray, mXCenter, mYCenter);
	
	// 8. SAVE Result
	/*Mat temp;
	cv::cvtColor(Gray, temp, CV_BGR2GRAY);*/
	/*cv::resize(Gray, Gray,cv::Size(), scale, scale);*/
	Mat mResult;
	cv::cvtColor(Gray, mResult, CV_GRAY2BGR);
	Gray.copyTo(mResult);
	for(int i = 0; i < vDefect.size(); i++ ) {
		Scalar Color;
		if(vDefect.at(i).type != E_WHITE) {
			Color = Scalar (255,0,0);
		}else {
			Color = Scalar (0,0,255);
		}

		if(vDefect.at(i).unUse == true)	{
			Color = Scalar (255,0,255);
		}
		fillPoly (mResult, (Mat)vDefect.at(i).contour, Color);

		//circle( temp, vDefect.at(i).center, 3, Scalar(0,255,0), -1, 3, 0 );
		circle( mResult, vDefect.at(i).center, vDefect.at(i).length*2, Scalar(0,255,255), 1, 2, 0 );

		string s;
			switch(vDefect.at(i).name){
				case E_Bubble			: s = "E_Bubble"			;	break;	
				case E_Substance		: s = "E_Substance"			;	break;
				case E_BubbleSubstance	: s = "E_BubbleSubstance"	;	break;
				case E_Scratch			: s = "E_Scratch"			;	break;
				case E_MURA				: s = "E_MURA"				;	break;
				case E_HILL				: s = "E_HILL"				;	break;
				default: break;

			}
		
		Scalar color = Scalar(0,255,0,0);
		putText(mResult, s, vDefect.at(i).center , CV_FONT_HERSHEY_COMPLEX, 0.5, color);

	}

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Acitve_Image.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, mResult);
	}

	// 8. Report Active defect
	Mat mBlobResult[E_ACTIVE_DEFECT_NUM];
	int mBlobCount[E_ACTIVE_DEFECT_NUM];
	for(int i=E_Bubble; i<E_ACTIVE_DEFECT_NUM; i++)	{
		mBlobResult[i]	= Mat::zeros(Gray.size(), CV_8UC1);
		mBlobCount[i]	= 0;
	}

	//int countBubble			=	0;
	//int countSubstance		=	0;
	//int countBubbleSubstance	=	0;
	//int countScratch			=	0;
	//int countMURA				=	0;
	//int countHILL				=	0;
	//int countFilmPeeling		=	0;
						
	for(int i = 0; i < vDefect.size(); i++ ) {
		if(vDefect.at(i).unUse == false)	{
			int defectName = vDefect.at(i).name;
			fillConvexPoly (mBlobResult[defectName], &vDefect.at(i).contour[0], vDefect.at(i).contour.size(),255,8,0);
			mBlobCount[defectName]++;

				/*switch(vDefect.at(i).name){
					case E_Bubble			:	fillConvexPoly (mBlobResult[E_Bubble],			&vDefect.at(i).contour[0], vDefect.at(i).contour.size(),255,8,0);	countBubble++;			break;	
					case E_Substance		:	fillConvexPoly (mBlobResult[E_Substance],		&vDefect.at(i).contour[0], vDefect.at(i).contour.size(),255,8,0);	countSubstance++;		break;
					case E_BubbleSubstance	:	fillConvexPoly (mBlobResult[E_BubbleSubstance], &vDefect.at(i).contour[0], vDefect.at(i).contour.size(),255,8,0);	countBubbleSubstance++;	break;
					case E_Scratch			:	fillConvexPoly (mBlobResult[E_Scratch],			&vDefect.at(i).contour[0], vDefect.at(i).contour.size(),255,8,0);	countScratch++;			break;
					case E_MURA				:	fillConvexPoly (mBlobResult[E_MURA],			&vDefect.at(i).contour[0], vDefect.at(i).contour.size(),255,8,0);	countMURA++;			break;
					case E_HILL				:	fillConvexPoly (mBlobResult[E_HILL],			&vDefect.at(i).contour[0], vDefect.at(i).contour.size(),255,8,0);	countHILL++;			break;
					case E_FilmPeeling		:	fillConvexPoly (mBlobResult[E_FilmPeeling],		&vDefect.at(i).contour[0], vDefect.at(i).contour.size(),255,8,0);	countFilmPeeling++;		break;
					default: E_Bubble;	break;
				}*/
		}
	}

	// CRUX Labeling
	Mat ResultMat = Mat::zeros(matInspActive.size(), CV_8UC3);
	
	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem(cMem);

	int nPrevDefectCount = pResultBlob->nDefectCount;

	for(int i=E_Bubble; i<E_ACTIVE_DEFECT_NUM; i++)	{
		if( i == E_HILL)	continue; //��������(����x)
		if( i == E_Scratch) continue; //���� ��������

		if(mBlobCount[i] > 0)	{
			cv:: resize(mBlobResult[i], ResultMat,ResultMat.size());
			cFeatureExtraction.DoDefectBlobSingleJudgment(matInspActive, ResultMat, matInspDraw, pCommonPara,
			E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob, i + E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLE, false);
			cFeatureExtraction.DrawBlob(matInspDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);
		}
	}

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Acitve_Draw_Image.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, matInspDraw);
	}

	//if(countBubble > 0){
	//	cv:: resize(mBubble, ResultMat,ResultMat.size());
	//	cFeatureExtraction.DoDefectBlobSingleJudgment(matInspActive, ResultMat, matInspDraw, pCommonPara,
	//		E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob, E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLE, false);
	//	cFeatureExtraction.DrawBlob(matInspDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);
	//}

	//if(countSubstance > 0){
	//	cv:: resize(E_Substance, ResultMat,ResultMat.size());
	//	cFeatureExtraction.DoDefectBlobSingleJudgment(matInspActive, ResultMat, matInspDraw, pCommonPara,
	//	E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob, E_DEFECT_JUDGEMENT_APP_ACTIVE_SUBSTANCE, false);
	//	cFeatureExtraction.DrawBlob(matInspDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

	//}

	//if(countBubbleSubstance > 0){
	//	cv:: resize(mBubbleSubstance, ResultMat,ResultMat.size());
	//	cFeatureExtraction.DoDefectBlobSingleJudgment(matInspActive, ResultMat, matInspDraw, pCommonPara,
	//	E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob, E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLESUBSTANCE, false);
	//	cFeatureExtraction.DrawBlob(matInspDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);
	//}

	//if(countScratch > 0){
	//	// ���� ��������
	//}

	//if(countMURA > 0){
	//	cv:: resize(mMURA, ResultMat,ResultMat.size());
	//	cFeatureExtraction.DoDefectBlobSingleJudgment(matInspActive, ResultMat, matInspDraw, pCommonPara,
	//	E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob, E_DEFECT_JUDGEMENT_APP_ACTIVE_MURA, false);
	//	cFeatureExtraction.DrawBlob(matInspDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);
	//}

	//if(countHILL > 0){
	//	//��������(����x)
	//}

	//if(countFilmPeeling > 0){
	//	cv:: resize(mFilmPeeling, ResultMat,ResultMat.size());
	//	cFeatureExtraction.DoDefectBlobSingleJudgment(matInspActive, ResultMat, matInspDraw, pCommonPara,
	//	E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob, E_DEFECT_JUDGEMENT_APP_ACTIVE_FILM_PEELING, false);
	//	cFeatureExtraction.DrawBlob(matInspDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);
	//}


	int nCurrDefectCount = pResultBlob->nDefectCount;
	for(long n =nPrevDefectCount; n < nCurrDefectCount ;n++ )
	{
		pResultBlob->ptLT[n].x		+= rtInspArea.x;
		pResultBlob->ptLT[n].y		+= rtInspArea.y;
		pResultBlob->ptRT[n].x		+= rtInspArea.x;	
		pResultBlob->ptRT[n].y		+= rtInspArea.y;
		pResultBlob->ptRB[n].x		+= rtInspArea.x;
		pResultBlob->ptRB[n].y		+= rtInspArea.y;
		pResultBlob->ptLB[n].x		+= rtInspArea.x;	
		pResultBlob->ptLB[n].y		+= rtInspArea.y;
	}

	return 0;
}

bool CInspectActive::DefectCategorization(vector<DEFECTINFO> &vDefect, vector<vector<cv::Point>> contours, int type, int name)
{
	RotatedRect mr;

	for( int i = 0; i < contours.size(); i++ ) {

		Point2f center; 
		float radius = 0;  
		minEnclosingCircle(contours[i], center, radius);  

		DEFECTINFO mDefect;

		mDefect.center = center;
		mDefect.area = contourArea(contours[i]);///*cv::Mat(contours[i])*/);
		mDefect.rect = boundingRect(contours[i]);
		mDefect.rotated_rect = minAreaRect(contours[i]);
		mDefect.unUse = false;
		mDefect.type = type;
		mDefect.name = name;
		mDefect.length = radius;
		mDefect.perimeter = arcLength(contours[i], true);

		Scalar mean, std;
		/*meanStdDev(contours[i], mean, std, contours[i]);*/
		mDefect.std = 0;//std[0];

		mDefect.rougness = (4* pi* mDefect.area)/ pow(mDefect.perimeter,2);
		mDefect.compactness = pow(mDefect.perimeter,2)/(4* pi* mDefect.area);

		mDefect.conNum = 0;

		if(mDefect.name != -1){
			mDefect.contour = contours[i];
			vDefect.push_back(mDefect);
			continue;
		}
	}

	return true;
}


//// 2017.10.24 LDH
long CInspectActive::FindActiveBurnt(Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* pCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, stActiveBurntROI* stROI, CString strCurRecipePath, wchar_t* strContourTxt)
{
 	int nPos = 0;
 
 	int		BlurSize		=	(int)dPara[nPos++];			
	int		nGausBlurSize	=	(int)dPara[nPos++];
	int		nGausSigma		=	(int)dPara[nPos++];
 	if((BlurSize%2)==0){ BlurSize++; }
	if((nGausBlurSize%2) == 0){nGausBlurSize++;}
 	const	double	dGamma			=	(double)dPara[nPos++];
	
	int nThreshold[ACTIVE_NUM];

	for(int nType = 0; nType < ACTIVE_NUM; nType++)
	{
		nThreshold[nType] = (int)dPara[nPos++];
	}

	const	int		nDeleteOffset	=	(int)dPara[nPos++];

 	cv::String str;
 
 	bool	bImageSave			= (pCommonPara[1] > 0) ? true : false;	// 1 : �˰��� �߰���� Image Save
 
	Mat matMatchBuffer[ACTIVE_NUM];
	Mat	matMatchRst;
	CString strMatchBuffer;
	Point2i ptMatch;

 	for (int nType = 0; nType < ACTIVE_NUM; nType++)
 	{
		Mat Gray;
		Rect rtSrc = Rect(0,0,matSrcBuffer.cols,matSrcBuffer.rows);
		GetCheckROIOver(stROI->rtROI[nType], rtSrc, stROI->rtROI[nType]);

 		matSrcBuffer(stROI->rtROI[nType]).copyTo(Gray);
		
 		Mat matThreshold, matGaussian, matBackGround, matSubstance;

		Mat SurfaceFitting = Mat::zeros(Gray.size(), CV_8UC1);
		GaussianBlur(Gray, Gray , Size(nGausBlurSize, nGausBlurSize), nGausSigma);

		Mat mtDivide_Org[IMG_DIVIDE_NUM_AABURNT];	//������ ����
		Mat mtDivide_Bg[IMG_DIVIDE_NUM_AABURNT];		//��濵�� ����
		int nDivide;
		int nDivideST = 0;
		Rect rtDivide;
		bool bLongVertical = 0;	//������ ������ �� ����� Flag

		if(Gray.cols > Gray.rows)
			nDivide = (float)Gray.cols/IMG_DIVIDE_NUM_AABURNT;
		else
		{
			nDivide = (float)Gray.rows/IMG_DIVIDE_NUM_AABURNT;
			bLongVertical=1;
		}

		for(int nDivideInx = 0; nDivideInx < IMG_DIVIDE_NUM_AABURNT; nDivideInx++)
		{
			if(bLongVertical)	
			{
				if(nDivideInx == IMG_DIVIDE_NUM_AABURNT - 1)
					nDivide = Gray.rows - nDivideST;
				rtDivide = Rect(0, nDivideST, Gray.cols, nDivide);
			}
			else
			{
				if(nDivideInx == IMG_DIVIDE_NUM_AABURNT - 1)
					nDivide = Gray.cols - nDivideST;
				rtDivide = Rect(nDivideST, 0, nDivide, Gray.rows);
			}

			mtDivide_Org[nDivideInx] = Gray(rtDivide);
			mtDivide_Bg[nDivideInx] = SurfaceFitting(rtDivide);

			if(bLongVertical)
				SubPoly(mtDivide_Org[nDivideInx], mtDivide_Bg[nDivideInx], E_VERTICAL);
			else
				SubPoly(mtDivide_Org[nDivideInx], mtDivide_Bg[nDivideInx], E_HORIZONTAL);

			nDivideST += nDivide;
		}

		matSubstance = SurfaceFitting - Gray;

		cv::threshold(matSubstance, matThreshold, nThreshold[nType], 255, CV_THRESH_BINARY);

		Rect rtDeleteArea_1;
		Rect rtDeleteArea_2;

		switch (nType)
		{
		case ACTIVE_TOP:
			rtDeleteArea_1 = Rect(0, 0, matThreshold.cols, nDeleteOffset);
			rtDeleteArea_2 = Rect(0, matThreshold.rows - nDeleteOffset, matThreshold.cols, nDeleteOffset);
			break;
		case ACTIVE_LEFT:
			rtDeleteArea_1 = Rect(0, 0, nDeleteOffset, matThreshold.rows);
			rtDeleteArea_2 = Rect(matThreshold.cols - nDeleteOffset, 0, nDeleteOffset, matThreshold.rows);
			break;
		case ACTIVE_BOTTOM:
			rtDeleteArea_1 = Rect(0, 0, matThreshold.cols, nDeleteOffset);
			rtDeleteArea_2 = Rect(0, matThreshold.rows - nDeleteOffset, matThreshold.cols, nDeleteOffset);
			break;
		}
		matThreshold(rtDeleteArea_1).setTo(0);
		matThreshold(rtDeleteArea_2).setTo(0);
			
		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%d_Src.bmp"), strAlgPath,	nType);			cv::imwrite((cv::String)(CStringA)strTemp, matSrcBuffer(stROI->rtROI[nType]));
 			strTemp.Format(_T("%s\\%d_Src_Gaus.bmp"), strAlgPath,	nType);			cv::imwrite((cv::String)(CStringA)strTemp, Gray);
 			strTemp.Format(_T("%s\\%d_BK_Gaus.bmp"), strAlgPath,	nType);			cv::imwrite((cv::String)(CStringA)strTemp, SurfaceFitting);
			strTemp.Format(_T("%s\\%d_Sub.bmp"), strAlgPath,		nType);			cv::imwrite((cv::String)(CStringA)strTemp, matSubstance);
			strTemp.Format(_T("%s\\%d_Thresh.bmp"), strAlgPath,	nType);			cv::imwrite((cv::String)(CStringA)strTemp, matThreshold);
		}


		CFeatureExtraction cFeatureExtraction;
		cFeatureExtraction.SetMem(cMem);
		int nPrevDefectCount = pResultBlob->nDefectCount;

		Rect rtMatchOrigin = Rect(stROI->rtROI[nType].x, stROI->rtROI[nType].y, stROI->rtROI[nType].width, stROI->rtROI[nType].height);

		cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rtMatchOrigin), matThreshold, matDrawBuffer(rtMatchOrigin), pCommonPara,
			E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob, false);
		cFeatureExtraction.DrawBlob(matDrawBuffer(rtMatchOrigin), cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);
 
 		int nCurrDefectCount = pResultBlob->nDefectCount;
   		for(long n =nPrevDefectCount; n < nCurrDefectCount ;n++ )
   		{
   
   			pResultBlob->ptLT[n].x		+= stROI->rtROI[nType].x;
   			pResultBlob->ptLT[n].y		+= stROI->rtROI[nType].y;
   			pResultBlob->ptRT[n].x		+= stROI->rtROI[nType].x;
   			pResultBlob->ptRT[n].y		+= stROI->rtROI[nType].y;
   			pResultBlob->ptRB[n].x		+= stROI->rtROI[nType].x;
   			pResultBlob->ptRB[n].y		+= stROI->rtROI[nType].y;
   			pResultBlob->ptLB[n].x		+= stROI->rtROI[nType].x;
   			pResultBlob->ptLB[n].y		+= stROI->rtROI[nType].y;
   		}
 	}

	

	return E_ERROR_CODE_TRUE;
}

//// 2017.10.24 LDH
long CInspectActive::FindActiveDust(Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* pCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	int nPos = 0;

	const	int		BlurSize		=	(int)dPara[nPos++];		/// 5
	const	int		nMeanSize		=	(int)dPara[nPos++];		/// 20
	const	double	dGamma			=	(double)dPara[nPos++];	/// 0.5
	const	int		nThreshold		=	(int)dPara[nPos++];		/// 10

	cv::String str;

	bool	bImageSave			= (pCommonPara[1] > 0) ? true : false;	// 1 : �˰��� �߰���� Image Save

	Mat matInspActive = matSrcBuffer(rtInspArea);
	Mat matInspDraw  = matDrawBuffer(rtInspArea);

	Mat Gray;
	if(matInspActive.type() != CV_8UC1) {
		cv::cvtColor(matInspActive, Gray, CV_BGR2GRAY);
	}else{
		Gray = matInspActive;
	}

	if (BlurSize < 3)
	{
		(int)BlurSize = 3;
	}

	if (nMeanSize < 3)
	{
		(int)nMeanSize = 3;
	}

	Mat matBlur, matThreshold, matGaussian, matSub;

	if (dGamma == 0)
	{
		cv::GaussianBlur(Gray,matGaussian, cv::Size(BlurSize,BlurSize),2);

		cv::threshold(matGaussian, matThreshold, nThreshold, 255, THRESH_BINARY );
		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\Dust_Gray_Image.jpg"), strAlgPath);
			cv::imwrite((cv::String)(CStringA)strTemp, Gray);

			strTemp.Format(_T("%s\\Dust_Threshold_Image.jpg"), strAlgPath);
			cv::imwrite((cv::String)(CStringA)strTemp, matThreshold);

			strTemp.Format(_T("%s\\Dust_matGaussian_Image.jpg"), strAlgPath);
			cv::imwrite((cv::String)(CStringA)strTemp, matGaussian);
		}
	}
	else
	{
		cv::GaussianBlur(Gray,matGaussian, cv::Size(BlurSize,BlurSize),0,0);

		cv::blur(Gray, matBlur, cv::Size(nMeanSize, nMeanSize));

		cv::subtract(matBlur, matGaussian, matSub);

		cv::threshold(matSub, matThreshold, nThreshold, 255, THRESH_BINARY );

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\Dust_Gray_Image.jpg"), strAlgPath);
			cv::imwrite((cv::String)(CStringA)strTemp, Gray);

			strTemp.Format(_T("%s\\Dust_Blur_Image.jpg"), strAlgPath);
			cv::imwrite((cv::String)(CStringA)strTemp, matBlur);

			strTemp.Format(_T("%s\\Dust_matGaussian_Image.jpg"), strAlgPath);
			cv::imwrite((cv::String)(CStringA)strTemp, matGaussian);

			strTemp.Format(_T("%s\\Dust_Sub_Image.jpg"), strAlgPath);
			cv::imwrite((cv::String)(CStringA)strTemp, matSub);
		}
	}

	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem(cMem);
	int nPrevDefectCount = pResultBlob->nDefectCount;

	cFeatureExtraction.DoDefectBlobJudgment(matInspActive, matThreshold, matInspDraw, pCommonPara,
		E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob, false);
	cFeatureExtraction.DrawBlob(matInspDraw, cv::Scalar(0,0,255), BLOB_DRAW_BLOBS_CONTOUR, true);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Dust_Draw_Image.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, matInspDraw);
	}

	int nCurrDefectCount = pResultBlob->nDefectCount;
	for(long n =nPrevDefectCount; n < nCurrDefectCount ;n++ )
	{
		pResultBlob->ptLT[n].x		+= rtInspArea.x;
		pResultBlob->ptLT[n].y		+= rtInspArea.y;
		pResultBlob->ptRT[n].x		+= rtInspArea.x;
		pResultBlob->ptRT[n].y		+= rtInspArea.y;
		pResultBlob->ptRB[n].x		+= rtInspArea.x;
		pResultBlob->ptRB[n].y		+= rtInspArea.y;
		pResultBlob->ptLB[n].x		+= rtInspArea.x;
		pResultBlob->ptLB[n].y		+= rtInspArea.y;
	}

	return 0;
}

void CInspectActive::GammaCorrection(Mat matSrcBuffer,Mat matThrBuffer, float fGamma)
{

	register int i, j;

	// Get Image Size
	int		nWidth		= matSrcBuffer.cols;
	int		nHeight		= matSrcBuffer.rows;

	int		nMatSrcStep		= matSrcBuffer.step;

	int		nMatDstStep		= matThrBuffer.step;

	float fInputLevel = 0.0f;
	float fGammaCorrection = 1.0f / fGamma;
	int nTransLevel[256] = {0, };

	for ( i = 0 ; i < 256 ; i++)
	{
		nTransLevel[i] = (int)(255.0f * pow(fInputLevel, fGammaCorrection));
		fInputLevel += 0.00390625;
	}

	for( i = 0 ; i < nHeight ; i++ )
	{
		for( j = 0 ; j < nWidth ; j++ ) 
		{
			matThrBuffer.data[i * nMatDstStep + j] = (unsigned char)( nTransLevel[ matSrcBuffer.data[i * nMatSrcStep + j] ] );
		}
	}

}

bool CInspectActive::GetRoundCellArea(cv::Mat Img, vector<vector<cv::Point>>* ptRoundArea, float* fThreshold_Per, int nCornerROISize, Scalar* scMean, Scalar* scSTDEV)
{
	//�𼭸� 4���� ROI
	Mat mtCorner[E_CORNER_END];
	Rect rtCornerROI[E_CORNER_END];

	rtCornerROI[E_CORNER_LEFT_TOP]		= Rect(0, 0, nCornerROISize, nCornerROISize);
	rtCornerROI[E_CORNER_RIGHT_TOP]		= Rect(Img.cols - nCornerROISize, 0, nCornerROISize, nCornerROISize);
	rtCornerROI[E_CORNER_RIGHT_BOTTOM]	= Rect(Img.cols - nCornerROISize, Img.rows - nCornerROISize, nCornerROISize, nCornerROISize);
	rtCornerROI[E_CORNER_LEFT_BOTTOM]	= Rect(0, Img.rows - nCornerROISize, nCornerROISize, nCornerROISize);	

	
	vector<vector<cv::Point>>contours[E_CORNER_END];
	double dMax;
	int nMaxInx[E_CORNER_END];
	Mat FillContour[E_CORNER_END];
	Mat mtThreshold;
	cv::Point ptContour_Offset;
	for(int nCornerInx=E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
	{
		Img(rtCornerROI[nCornerInx]).copyTo(mtCorner[nCornerInx]);

		FillContour[nCornerInx] = Mat::zeros(mtCorner[nCornerInx].size(),CV_8UC1);

		cv::meanStdDev(mtCorner[nCornerInx], scMean[nCornerInx], scSTDEV[nCornerInx]);

		Mat mtHisto;
		double dAvg=0;
		GetHistogram(mtCorner[nCornerInx], mtHisto);
		GetAvgOfStdevRange(mtHisto, scMean[nCornerInx], scSTDEV[nCornerInx], dAvg);

		mtThreshold = Mat::zeros(mtCorner[nCornerInx].size(), CV_8UC1);

		int nThreshold = dAvg * fThreshold_Per[nCornerInx];

		cv::threshold(mtCorner[nCornerInx], mtThreshold, nThreshold, 255, CV_THRESH_BINARY);

		findContours(mtThreshold, contours[nCornerInx], CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		dMax=0;
		for(int n=0; n < contours[nCornerInx].size(); n++)
		{
			double dSize = contourArea(contours[nCornerInx][n]);
			if(dMax < dSize)
			{
				dMax = dSize;
				nMaxInx[nCornerInx] = n;
			}
		}
		drawContours(FillContour[nCornerInx], contours[nCornerInx], nMaxInx[nCornerInx], Scalar(255,255,255),-1);

		cv::bitwise_not(FillContour[nCornerInx], FillContour[nCornerInx]);
		cv::morphologyEx(FillContour[nCornerInx], FillContour[nCornerInx], MORPH_OPEN, Mat(3,3,CV_8UC1,Scalar(1)));
		cv::morphologyEx(FillContour[nCornerInx], FillContour[nCornerInx], MORPH_CLOSE, Mat(11,11,CV_8UC1,Scalar(1)));
		cv::morphologyEx(FillContour[nCornerInx], FillContour[nCornerInx], MORPH_DILATE, Mat(31,31,CV_8UC1,Scalar(1)));

		Mat FillContour_INV;
		cv::bitwise_not(FillContour[nCornerInx], FillContour_INV);
		cv::meanStdDev(mtCorner[nCornerInx], scMean[nCornerInx], scSTDEV[nCornerInx], FillContour_INV);

		switch(nCornerInx)
		{
		case E_CORNER_LEFT_TOP:
			ptContour_Offset = Point(0,0);
			break;
		case E_CORNER_RIGHT_TOP:
			ptContour_Offset = Point(Img.cols-nCornerROISize, 0);
			break;
		case E_CORNER_RIGHT_BOTTOM:
			ptContour_Offset = Point(Img.cols-nCornerROISize, Img.rows-nCornerROISize);
			break;
		case E_CORNER_LEFT_BOTTOM:
			ptContour_Offset = Point(0, Img.rows-nCornerROISize);
			break;
		}

		findContours(FillContour[nCornerInx], ptRoundArea[nCornerInx], CV_RETR_LIST, CV_CHAIN_APPROX_NONE, ptContour_Offset);
	}
	
	return true;
	
}

long CInspectActive::Bright_Adjust(cv::Mat* mtImg, int nImageNum)
{
	int** nHist;
	nHist = new int*[nImageNum];

	for(int n=0; n < nImageNum; n++)
	{
		nHist[n] = new int[256];
		memset(nHist[n], 0, 256 * sizeof(int));
	}

	vector<double> dMax;
	vector<double> dMean;
	Scalar  s;
	Scalar* m;
	m = new Scalar[nImageNum];
	uchar* pImgdata;

	for(int n=0; n < nImageNum; n++)
	{
		//cv::minMaxIdx(mtImg[n], 0, &dM);
		cv::meanStdDev(mtImg[n], m[n], s);
		//dMax.push_back(dM);
		dMean.push_back(double(m[n][0]));
		for(int nY=0; nY < mtImg[n].rows; nY++)
		{
			pImgdata = mtImg[n].data+nY*mtImg[n].step;
			for(int nX=0; nX < mtImg[n].cols; nX++)
			{
				nHist[n][(int)*(pImgdata+nX)]++;
			}
		}
	}
	
	sort(dMean.begin(), dMean.end());

	int** nLut_Adjust;
	nLut_Adjust = new int*[nImageNum];
	for(int n=0; n < nImageNum; n++)
	{
		nLut_Adjust[n] = new int[256];
		memset(nLut_Adjust[n], 0, sizeof(int) * 256);
	}
	
	unsigned int nSum[256]={0,};

	for(int n=0; n < nImageNum; n++)
	{
		
		for(int i=0; i < 256; i++)
		{
			memset(nSum,0, sizeof(unsigned int)*256);
			for(int j=0; j <= i; j++)
			{
				nSum[i]				+= nHist[n][j];
			}
			nLut_Adjust[n][i]	= (float)nSum[i]/(mtImg[n].cols*mtImg[n].rows)*255;	
		}
	}
	
	Mat *mtDst;
	mtDst = new Mat[nImageNum];
	uchar *pDstData;

	for(int n=0; n < nImageNum; n++)
	{
		double dOffsetMean = dMean[dMean.size()-1] - m[n][0];
		mtDst[n] = Mat::zeros(mtImg[n].size(), CV_8UC1);
		for(int nY=0; nY < mtImg[n].rows; nY++)
		{
			pImgdata = mtImg[n].data + nY*mtImg[n].step;
			pDstData = mtDst[n].data + nY*mtDst[n].step;
			for(int nX=0; nX < mtImg[n].cols; nX++)
			{
				//*(pDstData+nX) = (uchar)nLut_Adjust[n][(int)*(pImgdata+nX)];
				if((int)*(pImgdata+nX)!=0)
					*(pDstData+nX) = (uchar)((int)*(pImgdata+nX)+(int)dOffsetMean);
			}
		}
	}

	for(int n=0; n < nImageNum; n++)
	{
		mtDst[n].copyTo(mtImg[n]);

		delete[] nHist[n];
		delete[] nLut_Adjust[n];
	}

	delete[] nHist;
	delete[] nLut_Adjust;
	delete[] mtDst;
	delete[] m;

	return 0;
}


bool CInspectActive::CheckHill(cv::Mat src, DEFECTINFO & defect, int nSize, float fSTDVal, float fCenterDistance, int nImageInx, bool bImageSave)
{
	//�߽ɰ��� �Ÿ��� �ִٸ�
	if (defect.centerDistance > fCenterDistance)
	{
		return false;
	}

	//���� �ĺ����� �庯���� ȹ��
	float fMajorLength = defect.rotated_rect.size.width > defect.rotated_rect.size.height ? defect.rotated_rect.size.width : defect.rotated_rect.size.height;

	vector<int> val;
	double std = 0.0;

	cv::Rect rtHill;
	int nST_X, nST_Y, nED_X, nED_Y;
	nST_X = defect.center.x - fMajorLength / 2;
	nST_Y = defect.center.y - fMajorLength / 2;
	nED_X = defect.center.x + fMajorLength / 2;
	nED_Y = defect.center.y + fMajorLength / 2;

	if (nST_X < 0)
	{
		nST_X = 0;
	}

	if (nST_Y < 0)
	{
		nST_Y = 0;
	}

	if (nED_X >= src.cols)
	{
		nED_X = src.cols - 1;
	}

	if (nED_Y >= src.rows)
	{
		nED_Y = src.rows - 1;
	}

	rtHill = cv::Rect(cv::Point(nST_X, nST_Y), cv::Point(nED_X, nED_Y));

	cv::Mat mtMask = cMem->GetMat(src.size(), CV_8UC1);
	cv::circle(mtMask(rtHill), cv::Point(mtMask(rtHill).cols / 2, mtMask(rtHill).rows / 2), (int)fMajorLength, cv::Scalar(255), -1); 

	cv::Scalar m, s;
	cv::meanStdDev(src(rtHill), m, s, mtMask(rtHill));
	if (bImageSave == true)
	{
		if (!m_mtDrawHill[nImageInx].empty())
		{
			cv::circle(m_mtDrawHill[nImageInx], defect.center, (int)fMajorLength, cv::Scalar(0,255,0)); 
		}

		CString strStdev;
		strStdev.Format(_T("%.2f, %.2f"), s[0], defect.length);
		cv::putText(m_mtDrawHill[nImageInx], (cv::String)(CStringA)strStdev, cv::Point(defect.center.x, defect.center.y), FONT_HERSHEY_SIMPLEX, 1., cv::Scalar(255,0,0), 1);
	}

	if (s[0] > fSTDVal && defect.length < nSize)
	{
		return true;
	}

	return false;
}

bool CInspectActive::CheckHill(cv::Mat src, vector<DEFECTINFO> &vDefect, int nDefectInx, int nSize, float fSTDVal, float fCenterDistance, int nImageInx, bool bImageSave)
{
	//�߽ɰ��� �Ÿ��� �ִٸ�
	if (vDefect[nDefectInx].centerDistance > fCenterDistance)
	{
		return false;
	}

	//���� �ĺ����� �庯���� ȹ��
 	float fMajorLength = vDefect[nDefectInx].rotated_rect.size.width > vDefect[nDefectInx].rotated_rect.size.height ? vDefect[nDefectInx].rotated_rect.size.width : vDefect[nDefectInx].rotated_rect.size.height;

	vector<int> val;
	double std = 0.0;

	cv::Rect rtHill;
	int nST_X, nST_Y, nED_X, nED_Y;
	nST_X = vDefect[nDefectInx].center.x - fMajorLength / 2;
	nST_Y = vDefect[nDefectInx].center.y - fMajorLength / 2;
	nED_X = vDefect[nDefectInx].center.x + fMajorLength / 2;
	nED_Y = vDefect[nDefectInx].center.y + fMajorLength / 2;

	if (nST_X < 0)
	{
		nST_X = 0;
	}

	if (nST_Y < 0)
	{
		nST_Y = 0;
	}

	if (nED_X >= src.cols)
	{
		nED_X = src.cols - 1;
	}

	if (nED_Y >= src.rows)
	{
		nED_Y = src.rows - 1;
	}

	rtHill = cv::Rect(cv::Point(nST_X, nST_Y), cv::Point(nED_X, nED_Y));

	cv::Mat mtMask = cMem->GetMat(src.size(), CV_8UC1);
	cv::circle(mtMask(rtHill), cv::Point(mtMask(rtHill).cols/2, mtMask(rtHill).rows/2), (int)fMajorLength, cv::Scalar(255), -1); 

	cv::Scalar m, s;
	cv::meanStdDev(src(rtHill), m, s, mtMask(rtHill));

	if (bImageSave == true)
	{
		if (!m_mtDrawHill[nImageInx].empty())
		{
			cv::circle(m_mtDrawHill[nImageInx], vDefect[nDefectInx].center, (int)fMajorLength, cv::Scalar(0,255,0)); 
		}

		CString strStdev;
		strStdev.Format(_T("%.2f"), s[0]);
		cv::putText(m_mtDrawHill[nImageInx], (cv::String)(CStringA)strStdev, cv::Point(vDefect[nDefectInx].center.x, vDefect[nDefectInx].center.y), FONT_HERSHEY_SIMPLEX, 1., cv::Scalar(255,0,0), 1);
	}

	if (s[0] > fSTDVal && vDefect[nDefectInx].length < nSize)
	{
		return true;
	}
	
	return false;
}

long CInspectActive::GetHistogram(Mat& matSrcImage, Mat& matHisto)
{
	int nBit = GetBitFromImageDepth(matSrcImage.depth());
	if(nBit == -1)	return E_ERROR_CODE_APP_HISTO;

	int nHistSize = (int)pow((double)256,(double)nBit);
	float fHistRange[] = {(float)0, (float)nHistSize-1};
	const float *ranges[] = { fHistRange };

	cv::calcHist(&matSrcImage,1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;

}


bool CInspectActive::GetAvgOfStdevRange(Mat matHisto, Scalar Avg, Scalar Stdev, double& dReturnAvg)
{
	int nMin = (int)(Avg[0] - Stdev[0]);
	int nMax = (int)(Avg[0] + Stdev[0]);

	if(nMin < 0)
		nMin = 0;
	if(nMax > 255)
		nMax = 255;

	double dSum = 0;
	int nSumCnt = 0;

	float* fHistData = (float*)matHisto.data + nMin;

	for(int n = nMin; n < nMax; n++, fHistData++)
	{
		// 		if((int)*fHistData == 0)
		// 			continue;
		dSum += *fHistData * n;
		nSumCnt += (int)*fHistData;
	}

	dReturnAvg = dSum / nSumCnt;

	return true;

}

int CInspectActive::GetBitFromImageDepth(int nDepth)
{
	int nRet = -1;

	switch(nDepth)
	{
	case CV_8U :
	case CV_8S :
		nRet = 1;
		break;

	case CV_16U :
	case CV_16S :
		nRet = 2;
		break;

	case CV_32S :
	case CV_32F :
		nRet = 4;
		break;

	case CV_64F :
		nRet = 8;
		break;

	default:
		nRet = -1;
		break;
	}

	return nRet;

}

//////////////////////////////////////////////////////////
// ��˿��� ROI�� �Ѱ��� by 2018.01.24
/////////////////////////////////////////////////////////
cv::Rect CInspectActive::GetNoneInspROI(int* nNoneInspArea, cv::Rect rtOrg, float fScale)
{
	cv::Rect rtROI;

	int nDirection = -1;
	if(nNoneInspArea[E_PANEL_LEFT] != 0)
		nDirection = E_PANEL_LEFT;
	if(nNoneInspArea[E_PANEL_TOP] != 0)
		nDirection = E_PANEL_TOP;
	if(nNoneInspArea[E_PANEL_RIGHT] != 0)
		nDirection = E_PANEL_RIGHT;
	if(nNoneInspArea[E_PANEL_BOTTOM] != 0)
		nDirection = E_PANEL_BOTTOM;

	switch(nDirection)
	{
	case E_PANEL_LEFT:
		rtROI = Rect((nNoneInspArea[E_PANEL_LEFT]*fScale), 0, rtOrg.width - (nNoneInspArea[E_PANEL_LEFT]*fScale), rtOrg.height);
		break;
	case E_PANEL_TOP:
		rtROI = Rect(0, nNoneInspArea[E_PANEL_TOP]*fScale, rtOrg.width, rtOrg.height - nNoneInspArea[E_PANEL_TOP]*fScale);
		break;
	case E_PANEL_RIGHT:
		rtROI = Rect(0, 0, rtOrg.width - nNoneInspArea[E_PANEL_RIGHT]*fScale, rtOrg.height);
		break;
	case E_PANEL_BOTTOM:
		rtROI = Rect(0, 0, rtOrg.width, rtOrg.height - nNoneInspArea[E_PANEL_BOTTOM]*fScale);
		break;
	}

	GetCheckROIOver(rtROI, rtOrg, rtROI);

	return rtROI;
}

bool CInspectActive::WriteResultFile(CString strFilePath, CString strFileName, CString strColumn, TCHAR* strResult)
{
	CString strResultData;
	SYSTEMTIME	time;
	GetLocalTime(&time);

	CString strTime;
	strTime.Format(_T("%02u:%02u:%02u"), time.wHour, time.wMinute, time.wSecond);

	CString strFullPath;
	strFullPath.Format(_T("%s\\%s"), strFilePath, strFileName);

	CStdioFile stdFile;

	char cSavePath[256];
	memset(cSavePath, NULL, 256);
	WideCharToMultiByte(CP_ACP, NULL, strFullPath, -1, cSavePath, 256, NULL, NULL);

	int nFileCheck = GetFileAttributes(strFullPath);

	if(!stdFile.Open(strFullPath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::shareDenyWrite, NULL))
	{
		return false;
	}

	stdFile.SeekToEnd(); //���� �������� ������ �̵�

	CString strValue;
	strValue.Format(_T("\n%s"), strResult);

	if(nFileCheck == -1)		
	{
		stdFile.WriteString(strColumn);
	}

	stdFile.WriteString(strValue);

	stdFile.Close();

	return true;
}


void CInspectActive::InspectionStepImage(cv::Mat matImage, bool bImageSave, CString strStepName, wchar_t * strAlgPath, int nImageNum)
{
	if (bImageSave == true)
	{
		CString strTemp = _T("");
		strTemp.Format(strStepName, strAlgPath, nImageNum);
		cv::imwrite((cv::String)(CStringA)strTemp, matImage);
	}
}