/************************************************************************/
// Concealment �ҷ� ���� ���� �ҽ�
// ������ : 22.01.11 - CKI
/************************************************************************/

#include "StdAfx.h"
#include "InspectConcealment.h"

enum enFlattening
{
	Flattening_None, Flattening_Horizental, Flattening_Vertical, Flattening_All_H, Flattening_All_V
};

#define _BIAS_	128

CInspectConcealment::CInspectConcealment()
{
	cMem = NULL;
	m_cInspectLibLog = NULL;
	m_strAlgLog = NULL;
	m_tInitTime = 0;
	m_tBeforeTime = 0;
}


CInspectConcealment::~CInspectConcealment()
{
}

long CInspectConcealment::DoFindConcealmentDefect(cv::Mat matSrcBuffer, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer, cv::Mat matMask_Active,
	cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, CDefectCCD *cCCD, cv::Mat* matCholeBuffer)
{
	// �Ķ���� NULL�� ���
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	long	nWidth = (long)matSrcBuffer.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcBuffer.rows;	// ���� ���� ũ��

												// ���� ��ȣ
	long	nImageClassify = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];

	// ID ã��
	CString strPathTemp;

	// ������ ���� "\\" ����
	strPathTemp.Format(_T("%s"), strAlgPath.Left(strAlgPath.GetLength() - 1));

	// �����ʿ��� "\\" ã��
	int nLength = strPathTemp.ReverseFind(_T('\\'));

	// ID�� ã��
	wchar_t	strID[MAX_PATH] = { 0, };
	swprintf_s(strID, MAX_PATH, L"%s", (LPCWSTR)strPathTemp.Right(strPathTemp.GetLength() - nLength - 1));

	writeInspectLog(E_ALG_TYPE_SVI_MURA, __FUNCTION__, _T("Start."));

	// ���� ũ�� ����
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	// �˻� ������ ���� ũ�⸦ ��� ���
	if (rectROI.left < 0 ||
		rectROI.top < 0 ||
		rectROI.right >= nWidth ||
		rectROI.bottom >= nHeight)	return E_ERROR_CODE_ROI_OVER;

	if (rectROI.left >= rectROI.right)	return E_ERROR_CODE_ROI_OVER;
	if (rectROI.top >= rectROI.bottom)	return E_ERROR_CODE_ROI_OVER;

	// ���� �ڵ�
	long nErrorCode = E_ERROR_CODE_TRUE;

	cv::Mat matDstImage[E_DEFECT_COLOR_COUNT];

	// ���� �Ҵ� & �ʱ�ȭ
	// KSW 17.09.26 - Dust ���Ͽ��� Dark	- Area ū ������ �̹�
	// KSW 17.09.26 - Dust ���Ͽ��� BRIGHT	- ��� ������ �̹�
	matDstImage[E_DEFECT_COLOR_DARK] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());
	matDstImage[E_DEFECT_COLOR_BRIGHT] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());

	// ��� ���۰� NULL�� �ƴ� ��� ����
	if (matDstImage[E_DEFECT_COLOR_DARK].empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstImage[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;

	switch (nImageClassify)
	{
		//RGB Pattern �˻�
	case AREA_CAM_IMG_CLASSIFY_NUM:
	{

		nErrorCode = LCP_AREA_Concealment(matSrcBuffer, matDstImage, matMask_Active, rectROI, rcAlignCellROI, dPara, nCommonPara, strAlgPath, strBubblePath, strBubblePath_Menual, eInspMode, EngineerDefectJudgment);


	}
	break;

	// ���� ��ȣ�� �߸� ���� ���.
	default:
		return E_ERROR_CODE_TRUE;
	}


	matDarkBuffer = matDstImage[E_DEFECT_COLOR_DARK].clone();
	matBrightBuffer = matDstImage[E_DEFECT_COLOR_BRIGHT].clone();

	writeInspectLog(E_ALG_TYPE_SVI_MURA, __FUNCTION__, _T("Result Copy."));

	// �Ҵ� ����
	matDstImage[E_DEFECT_COLOR_DARK].release();
	matDstImage[E_DEFECT_COLOR_BRIGHT].release();

	writeInspectLog(E_ALG_TYPE_SVI_MURA, __FUNCTION__, _T("End.Dispersion"));

	return nErrorCode;
}

long CInspectConcealment::LCP_AREA_Concealment(cv::Mat& matSrcImage, cv::Mat* matDstImage, cv::Mat matMask_Active, CRect rectROI, cv::Rect rcAlignCellROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// ���� �Ķ����
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;  //LCP �߰��̹���
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	//float 	pow_value = (float)dPara[E_PARA_LCP_VARIANCE_COMMON_POW_VALUE];
	//////////////////////////////////////////////////////////////////////////
	int nMask_Down_Sz = (int)dPara[E_PARA_LCP_CONCEALMENT_AREA_MASK_SIZE_DOWN];
	//////////////////////////////////////////////////////////////////////////

	CRect rectTemp(rectROI);

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_CONCEALMENT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}


	RangeAvgThreshold(matSrcImage, matDstImage, rectTemp, dPara, cMem);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_CONCEALMENT_%02d_Concealment_RangeThreshold_Dark.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}

	cv::Mat matActive;
	matMask_Active.copyTo(matActive);
	Mask_SizeDown(matActive, nMask_Down_Sz);

	cv::bitwise_and(matDstImage[E_DEFECT_COLOR_DARK], matActive, matDstImage[E_DEFECT_COLOR_DARK]);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_CONCEALMENT_%02d_Concealment_Active_Mask.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}

	// First Judge
	cv::Mat matLabels, stats_1, centroids;
	cv::Rect rectTemp_Dust;
	int labelCount = 0;

	labelCount = First_blob_Concealment(matSrcImage, matDstImage[E_DEFECT_COLOR_DARK], matLabels, stats_1, centroids, rectTemp_Dust, dPara);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_Concealment_%02d_Dark_First_blob.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectConcealment::RangeAvgThreshold(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	// ���� �ڵ�
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if (matSrcImage.type() == CV_8U)
		nErrorCode = RangeAvgThreshold_8bit(matSrcImage, matDstImage, rectROI, dPara, cMemSub);
	else
		nErrorCode = RangeAvgThreshold_16bit(matSrcImage, matDstImage, rectROI, dPara, cMemSub);

	return nErrorCode;
}

// ���� ���� ����ȭ
long CInspectConcealment::RangeAvgThreshold_8bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	long	nBlurLoop_5x5 = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_BLUR_LOOP];
	long	nBlur_Size = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_BLUR_LOOP_SIZE];

	long	nSegX = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_SEG_X];
	long	nSegY = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_SEG_Y];
	long	nEdgeArea = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_EDGE_AREA];

	float	fActiveDarkRatio = (float)dPara[E_PARA_LCP_CONCEALMENT_COMMON_ACTIVE_DARK_RATIO];
	float	fActiveBrightRatio = (float)dPara[E_PARA_LCP_CONCEALMENT_COMMON_ACTIVE_BRIGHT_RATIO];
	float	fEdgeDarkRatio = (float)dPara[E_PARA_LCP_CONCEALMENT_COMMON_EDGE_DARK_RATIO];
	float	fEdgeBrightRatio = (float)dPara[E_PARA_LCP_CONCEALMENT_COMMON_EDGE_BRIGHT_RATIO];

	int		nMinThGV = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_MIN_THRESHOLD];
	int		nMedian = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_MEDIAN];

	// ���� ���� 10���� ���� ���.
	if (nSegX <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;
	if (nSegY <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;

	// �̹��� ũ��
	long	nWidth = (long)matSrcImage.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcImage.rows;	// ���� ���� ũ��

	long nStart_Y, nEnd_Y;

	// Active ������ �˻�
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// Temp �޸� �Ҵ�
	cv::Mat matBlurBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	cv::Mat matBlurBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);

	// Range Avg
	cv::Mat matAvgBuf = cMatBufTemp.GetMat(nRangeY, nRangeX, matSrcImage.type(), false);



	int nBlur = nBlur_Size;

	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nBlur;
	//rtInspROI.y = rectROI.top - nBlur;
	//rtInspROI.width = rectROI.Width() + nBlur * 2;
	//rtInspROI.height = rectROI.Height() + nBlur * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nBlur);

	if (nBlurLoop_5x5 > 0)
	{
		cv::blur(matSrcImage(rtInspROI), matBlurBuf(rtInspROI), cv::Size(nBlur, nBlur));

		if (nBlurLoop_5x5 > 1)
		{
			// Avg
			for (int i = 1; i < nBlurLoop_5x5; i++)
			{
				cv::blur(matBlurBuf(rtInspROI), matBlurBuf1(rtInspROI), cv::Size(nBlur, nBlur));

				matBlurBuf1.copyTo(matBlurBuf);
			}
		}
	}
	else
	{
		matSrcImage.copyTo(matBlurBuf);
	}
	matBlurBuf1.release();

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("BlurLoop."));

	// Range Rect
	cv::Rect *rectRange = new cv::Rect[nRangeX * nRangeY];
	memset(rectRange, 0, sizeof(cv::Rect) * nRangeX * nRangeY);

	// ���� ���
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

		// y ���� ���
		if (y < nRangeY - nEdgeArea)
		{
			nStart_Y = y * nSegY + rectROI.top;

			if (y == nRangeY - 1 - nEdgeArea)		nEnd_Y = rectROI.bottom - nEdgeArea * nSegY;
			else							nEnd_Y = nStart_Y + nSegY;
		}
		else
		{
			nStart_Y = rectROI.bottom - (nRangeY - y) * nSegY;
			nEnd_Y = nStart_Y + nSegY;
		}

		for (long x = 0; x < nRangeX; x++)
		{
			long nStart_X, nEnd_X;

			// x ���� ���
			if (x < nRangeX - nEdgeArea)
			{
				nStart_X = x * nSegX + rectROI.left;

				if (x == nRangeX - 1 - nEdgeArea)		nEnd_X = rectROI.right - nEdgeArea * nSegX;
				else							nEnd_X = nStart_X + nSegX;
			}
			else
			{
				nStart_X = rectROI.right - (nRangeX - x) * nSegX;
				nEnd_X = nStart_X + nSegX;
			}

			// ���� ����
			rectPtr[x].x = nStart_X;
			rectPtr[x].y = nStart_Y;
			rectPtr[x].width = nEnd_X - nStart_X + 1;
			rectPtr[x].height = nEnd_Y - nStart_Y + 1;
		}
	}

	// ��� ���
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Avg
		BYTE *ptr = (BYTE *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x = 0; x < nRangeX; x++)
		{
			// ���� ROI
			cv::Mat matTempBuf = matBlurBuf(rectPtr[x]);

			// mean & stdDev ���ϱ�
			//	cv::Scalar m, s;
			//	cv::meanStdDev(matTempBuf, m, s);

			// ������׷�
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			double dblAverage;
			double dblStdev;
			GetMeanStdDev_From_Histo(matHisto, 0, 255, dblAverage, dblStdev);

			// ��� ���� ����
			//	int nMinGV = (int)(m[0] - s[0]);
			//	int nMaxGV = (int)(m[0] + s[0]);
			int nMinGV = (int)(dblAverage - dblStdev);
			int nMaxGV = (int)(dblAverage + dblStdev);
			if (nMinGV < 0)	nMinGV = 0;
			if (nMaxGV > 255)	nMaxGV = 255;

			// �ʱ�ȭ
			__int64 nPixelSum = 0;
			__int64 nPixelCount = 0;

			// ������ ��� ������ŭ�� �����
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m = nMinGV; m <= nMaxGV; m++, pVal++)
			{
				nPixelSum += (m * *pVal);
				nPixelCount += *pVal;
			}

			// ������ �ϳ��� �־��...
			if (nPixelCount > 0)
				ptr[x] = (BYTE)(nPixelSum / (float)nPixelCount);
		}
	}

	// �ֺ� ��հ� -> �߰������� ����
	MedianFilter(matAvgBuf, matAvgBuf, nMedian, &cMatBufTemp);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("MedianFilter."));

	// ����ȭ
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Avg
		BYTE *ptr = (BYTE *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x = 0; x < nRangeX; x++)
		{
			// ���� ROI
			cv::Mat matTempBuf = matBlurBuf(rectPtr[x]);

			// ��� * Ratio
			long nDarkTemp, nBrightTemp;

			// Edge ���� ���� �ϱ�
			if (x < nEdgeArea ||
				y < nEdgeArea ||
				x >= nRangeX - nEdgeArea ||
				y >= nRangeY - nEdgeArea)
			{
				nDarkTemp = (long)(ptr[x] * fEdgeDarkRatio);
				nBrightTemp = (long)(ptr[x] * fEdgeBrightRatio);
			}
			else
			{
				nDarkTemp = (long)(ptr[x] * fActiveDarkRatio);
				nBrightTemp = (long)(ptr[x] * fActiveBrightRatio);
			}

			// Gray �ʹ� ��ο� ��� ����.
			// ( ��� GV 2 ~ 3 * fBrightRatio -> ����ȭ : ������ �� �ö�� )
			if (nBrightTemp < nMinThGV)	nBrightTemp = nMinThGV;

			// ���� ó��
			if (nDarkTemp < 0)		nDarkTemp = 0;
			if (nBrightTemp > 255)	nBrightTemp = 255;

			// �Ķ���� 0�ΰ�� ���� ó��
			if (fActiveDarkRatio == 0)		nDarkTemp = -1;
			if (fActiveBrightRatio == 0)	nBrightTemp = 256;

			// E_DEFECT_COLOR_DARK Threshold
			cv::Mat matTempBufT = matDstImage[E_DEFECT_COLOR_DARK](rectPtr[x]);
			cv::threshold(matTempBuf, matTempBufT, nDarkTemp, 255.0, THRESH_BINARY_INV);

			// E_DEFECT_COLOR_BRIGHT Threshold
			matTempBufT = matDstImage[E_DEFECT_COLOR_BRIGHT](rectPtr[x]);
			cv::threshold(matTempBuf, matTempBufT, nBrightTemp, 255.0, THRESH_BINARY);
		}
	}

	// ����
	matAvgBuf.release();
	delete[] rectRange;
	rectRange = NULL;

	// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
	// 	}

	return E_ERROR_CODE_TRUE;
}

long CInspectConcealment::RangeAvgThreshold_16bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	long	nBlurLoop_5x5 = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_BLUR_LOOP];
	long	nBlur_Size = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_BLUR_LOOP_SIZE];

	long	nSegX = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_SEG_X];
	long	nSegY = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_SEG_Y];
	long	nEdgeArea = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_EDGE_AREA];

	float	fActiveDarkRatio = (float)dPara[E_PARA_LCP_CONCEALMENT_COMMON_ACTIVE_DARK_RATIO];
	float	fActiveBrightRatio = (float)dPara[E_PARA_LCP_CONCEALMENT_COMMON_ACTIVE_BRIGHT_RATIO];
	float	fEdgeDarkRatio = (float)dPara[E_PARA_LCP_CONCEALMENT_COMMON_EDGE_DARK_RATIO];
	float	fEdgeBrightRatio = (float)dPara[E_PARA_LCP_CONCEALMENT_COMMON_EDGE_BRIGHT_RATIO];

	int		nMinThGV = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_MIN_THRESHOLD];
	int		nMedian = (long)dPara[E_PARA_LCP_CONCEALMENT_COMMON_MEDIAN];

	// ���� ���� 10���� ���� ���.
	if (nSegX <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;
	if (nSegY <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;

	// �̹��� ũ��
	long	nWidth = (long)matSrcImage.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcImage.rows;	// ���� ���� ũ��

	long nStart_Y, nEnd_Y;

	// Active ������ �˻�
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// Temp �޸� �Ҵ�	
	cv::Mat matBlurBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());
	cv::Mat matBlurBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());

	// Range Avg
	cv::Mat matAvgBuf = cMatBufTemp.GetMat(nRangeY, nRangeX, matSrcImage.type(), false);

	int nBlur = nBlur_Size;

	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nBlur;
	//rtInspROI.y = rectROI.top - nBlur;
	//rtInspROI.width = rectROI.Width() + nBlur * 2;
	//rtInspROI.height = rectROI.Height() + nBlur * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nBlur);

	if (nBlurLoop_5x5 > 0)
	{
		cv::blur(matSrcImage(rtInspROI), matBlurBuf(rtInspROI), cv::Size(nBlur, nBlur));

		if (nBlurLoop_5x5 > 1)
		{
			// Avg
			for (int i = 1; i < nBlurLoop_5x5; i++)
			{
				cv::blur(matBlurBuf(rtInspROI), matBlurBuf1(rtInspROI), cv::Size(nBlur, nBlur));

				matBlurBuf1.copyTo(matBlurBuf);
			}
		}
	}
	else
	{
		matSrcImage.copyTo(matBlurBuf);
	}
	matBlurBuf1.release();

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("BlurLoop."));

	// Range Rect
	cv::Rect *rectRange = new cv::Rect[nRangeX * nRangeY];
	memset(rectRange, 0, sizeof(cv::Rect) * nRangeX * nRangeY);

	// ���� ���
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

		// y ���� ���
		if (y < nRangeY - nEdgeArea)
		{
			nStart_Y = y * nSegY + rectROI.top;

			if (y == nRangeY - 1 - nEdgeArea)		nEnd_Y = rectROI.bottom - nEdgeArea * nSegY;
			else							nEnd_Y = nStart_Y + nSegY;
		}
		else
		{
			nStart_Y = rectROI.bottom - (nRangeY - y) * nSegY;
			nEnd_Y = nStart_Y + nSegY;
		}

		for (long x = 0; x < nRangeX; x++)
		{
			long nStart_X, nEnd_X;

			// x ���� ���
			if (x < nRangeX - nEdgeArea)
			{
				nStart_X = x * nSegX + rectROI.left;

				if (x == nRangeX - 1 - nEdgeArea)		nEnd_X = rectROI.right - nEdgeArea * nSegX;
				else							nEnd_X = nStart_X + nSegX;
			}
			else
			{
				nStart_X = rectROI.right - (nRangeX - x) * nSegX;
				nEnd_X = nStart_X + nSegX;
			}

			// ���� ����
			rectPtr[x].x = nStart_X;
			rectPtr[x].y = nStart_Y;
			rectPtr[x].width = nEnd_X - nStart_X + 1;
			rectPtr[x].height = nEnd_Y - nStart_Y + 1;
		}
	}

	// ��� ���
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Avg
		ushort *ptr = (ushort *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x = 0; x < nRangeX; x++)
		{
			// ���� ROI
			cv::Mat matTempBuf = matBlurBuf(rectPtr[x]);

			// mean & stdDev ���ϱ�
			cv::Scalar m, s;
			cv::meanStdDev(matTempBuf, m, s);

			// ��� ���� ����
			int nMinGV = (int)(m[0] - s[0]);
			int nMaxGV = (int)(m[0] + s[0]);
			if (nMinGV < 0)	nMinGV = 0;
			if (nMaxGV > 4095)	nMaxGV = 4095;

			// ������׷�
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			// �ʱ�ȭ
			__int64 nPixelSum = 0;
			__int64 nPixelCount = 0;

			// ������ ��� ������ŭ�� �����
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m = nMinGV; m <= nMaxGV; m++, pVal++)
			{
				nPixelSum += (m * *pVal);
				nPixelCount += *pVal;
			}

			// ������ �ϳ��� �־��...
			if (nPixelCount > 0)
				ptr[x] = (ushort)(nPixelSum / (float)nPixelCount);
		}
	}

	// �ֺ� ��հ� -> �߰������� ����
	MedianFilter(matAvgBuf, matAvgBuf, nMedian, &cMatBufTemp);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("MedianFilter."));

	// ����ȭ
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Avg
		ushort *ptr = (ushort *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x = 0; x < nRangeX; x++)
		{
			// ���� ROI
			cv::Mat matTempBuf = matBlurBuf(rectPtr[x]);

			// ��� * Ratio
			long nDarkTemp, nBrightTemp;

			// Edge ���� ���� �ϱ�
			if (x < nEdgeArea ||
				y < nEdgeArea ||
				x >= nRangeX - nEdgeArea ||
				y >= nRangeY - nEdgeArea)
			{
				nDarkTemp = (long)(ptr[x] * fEdgeDarkRatio);
				nBrightTemp = (long)(ptr[x] * fEdgeBrightRatio);
			}
			else
			{
				nDarkTemp = (long)(ptr[x] * fActiveDarkRatio);
				nBrightTemp = (long)(ptr[x] * fActiveBrightRatio);
			}

			// Gray �ʹ� ��ο� ��� ����.
			// ( ��� GV 2 ~ 3 * fBrightRatio -> ����ȭ : ������ �� �ö�� )
			if (nBrightTemp < nMinThGV)	nBrightTemp = nMinThGV;

			// ���� ó��
			if (nDarkTemp < 0)			nDarkTemp = 0;
			if (nBrightTemp > 4095)	nBrightTemp = 4095;

			// �Ķ���� 0�ΰ�� ���� ó��
			if (fActiveDarkRatio == 0)		nDarkTemp = -1;
			if (fActiveBrightRatio == 0)	nBrightTemp = 4096;

			// E_DEFECT_COLOR_DARK Threshold
			cv::Mat matTempBufT = matDstImage[E_DEFECT_COLOR_DARK](rectPtr[x]);
			Binary_16bit(matTempBuf, matTempBufT, nDarkTemp, true, &cMatBufTemp);

			// E_DEFECT_COLOR_BRIGHT Threshold
			matTempBufT = matDstImage[E_DEFECT_COLOR_BRIGHT](rectPtr[x]);
			Binary_16bit(matTempBuf, matTempBufT, nBrightTemp, false, &cMatBufTemp);
		}
	}

	// ����
	matAvgBuf.release();
	delete[] rectRange;
	rectRange = NULL;

	// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
	// 	}

	return E_ERROR_CODE_TRUE;
}


long CInspectConcealment::GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto)
{
	// ���۰� ���� ���.
	if (matSrcImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	int nHistSize = 0;
	if (matSrcImage.type() == CV_8U)
		nHistSize = 256;
	else
		nHistSize = 4096;

	float fHistRange[] = { (float)0, (float)nHistSize - 1 };
	const float *ranges[] = { fHistRange };

	cv::calcHist(&matSrcImage, 1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;
}

long CInspectConcealment::GetAverage_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage)
{
	double dblValSum = 0;
	double dblCountSum = 0;
	float *pVal = (float*)matHisto.data;

	// ����� ���Ѵ�.
	for (int i = nMin; i < nMax; i++)
	{
		dblValSum += (*pVal * i);
		dblCountSum += *pVal;
		pVal++;
	}

	dblAverage = dblValSum / dblCountSum;

	return E_ERROR_CODE_TRUE;
}

long CInspectConcealment::GetMeanStdDev_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage, double &dblStdev)
{
	GetAverage_From_Histo(matHisto, nMin, nMax, dblAverage);

	double dblValSum = 0;
	double dblCountSum = 0;
	double dblTmp;
	float *pVal = (float*)matHisto.data;

	// �л��� ���Ѵ�.
	for (int i = nMin; i < nMax; i++)
	{
		dblTmp = (i - dblAverage);
		dblTmp = dblTmp*dblTmp;
		dblValSum += (*pVal * dblTmp);
		dblCountSum += *pVal;
		pVal++;
	}

	dblStdev = dblValSum / dblCountSum; ;
	dblStdev = sqrt(dblStdev);

	return E_ERROR_CODE_TRUE;
}

long CInspectConcealment::MedianFilter(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nKSize, CMatBuf* cMemSub)
{
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	// ���� �ڵ�
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if (matSrcBuf.type() == CV_8U)
	{
		cv::medianBlur(matSrcBuf, matDstBuf, nKSize);
	}
	else
	{
		CMatBuf cMatBufTemp;
		cMatBufTemp.SetMem(cMemSub);

		cv::Mat matTempBuf = cMatBufTemp.GetMat(matSrcBuf.size(), matSrcBuf.type(), false);

		int nHSize = nKSize / 2;
		ushort *nArr = new ushort[nKSize * nKSize];
		memset(nArr, 0, sizeof(ushort) * nKSize * nKSize);

		for (int yy = 0; yy < matSrcBuf.rows; yy++)
		{
			ushort *ptrDst = (ushort *)matTempBuf.ptr(yy);

			for (int xx = 0; xx < matSrcBuf.cols; xx++, ptrDst++)
			{
				// ���� Ȯ��
				int nSX = xx - nHSize;
				int nEX = xx + nHSize;
				int nSY = yy - nHSize;
				int nEY = yy + nHSize;

				// ���� ó��
				nSX = (nSX < 0) ? 0 : nSX;
				nSY = (nSY < 0) ? 0 : nSY;
				nEX = (nEX >= matSrcBuf.cols) ? matSrcBuf.cols - 1 : nEX;
				nEY = (nEY >= matSrcBuf.rows) ? matSrcBuf.rows - 1 : nEY;

				int nCount = 0;

				// ������ �ֱ�
				for (int nMy = nSY; nMy <= nEY; nMy++)
				{
					ushort *ptrSrc = (ushort *)matSrcBuf.ptr(nMy) + nSX;

					for (int nMx = nSX; nMx <= nEX; nMx++, ptrSrc++)
					{
						nArr[nCount++] = *ptrSrc;
					}
				}

				// ����
				for (int i = 0; i < nCount - 1; i++)
				{
					for (int j = i + 1; j < nCount; j++)
					{
						if (nArr[i] > nArr[j])
						{
							ushort nTemp = nArr[i];
							nArr[i] = nArr[j];
							nArr[j] = nTemp;
						}
					}
				}

				// ���
				*ptrDst = nArr[nCount / 2];
			}
		}

		delete[] nArr;
		nArr = NULL;

		// ��� ����
		matTempBuf.copyTo(matDstBuf);

		// �޸� ����
		matTempBuf.release();

		// 		if (m_cInspectLibLog->Use_AVI_Memory_Log) {
		// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
		// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
		// 		}

	}

	return nErrorCode;
}

void CInspectConcealment::Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset)
{
	rectInspROI.x = rectROI.left - nOffset;
	rectInspROI.y = rectROI.top - nOffset;
	rectInspROI.width = rectROI.Width() + nOffset * 2;
	rectInspROI.height = rectROI.Height() + nOffset * 2;

	if (rectInspROI.x < 0) rectInspROI.x = 0;
	if (rectInspROI.y < 0) rectInspROI.y = 0;
	if (rectInspROI.width > nWidth - rectInspROI.x) rectInspROI.width = nWidth - rectInspROI.x;
	if (rectInspROI.height > nHeight - rectInspROI.y) rectInspROI.height = nHeight - rectInspROI.y;
}

long CInspectConcealment::ImageSave(CString strPath, cv::Mat matSrcBuf)
{
	if (matSrcBuf.type() == CV_8U)
	{
		cv::imwrite((cv::String)(CStringA)strPath, matSrcBuf);
	}
	else if (matSrcBuf.type() == CV_8UC3)
	{
		cv::imwrite((cv::String)(CStringA)strPath, matSrcBuf);
	}
	else
	{
		cv::Mat matTemp = cv::Mat::zeros(matSrcBuf.size(), CV_8U);
		matSrcBuf.convertTo(matTemp, CV_8U, 1. / 16.);

		cv::imwrite((cv::String)(CStringA)strPath, matTemp);

		matTemp.release();
	}

	return E_ERROR_CODE_TRUE;
}

//Blob
long CInspectConcealment::GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer, cv::Mat matDstBuffer[2], cv::Mat& matDrawBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{
	// ���� �ڵ�
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// �Ķ���� NULL�� ���.
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (pResultBlob == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if (matSrcBuffer.empty())							return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuffer[E_DEFECT_COLOR_DARK].empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuffer[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;



	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];
	bool	bDefectNum = (nCommonPara[E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG] > 0) ? true : false;
	bool	bDrawDust = (nCommonPara[E_PARA_COMMON_DRAW_DUST_FLAG] > 0) ? true : false;

	// ���� ũ�� ����
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	writeInspectLog(E_ALG_TYPE_SVI_MURA, __FUNCTION__, _T("Start."));


	// �ҷ� ���� & ��� ������
	{
		cv::Rect rectBlobROI;
		Insp_RectSet(rectBlobROI, rectROI, matSrcBuffer.cols, matSrcBuffer.rows);

		// ��
		CFeatureExtraction cFeatureExtraction;
		cFeatureExtraction.SetMem(cMem);
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_SVI_MURA, m_tInitTime, m_tBeforeTime, m_strAlgLog);

		// E_DEFECT_COLOR_DARK ���
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment_LCP(matSrcBuffer(rectBlobROI), MatColorBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_DARK](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_DARK, dPara ,_T("DP_"), EngineerDefectJudgment, pResultBlob);
		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			// �޸� ����
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}

		writeInspectLog(E_ALG_TYPE_SVI_MURA, __FUNCTION__, _T("BlobJudgment (DARK)."));



		// E_DEFECT_COLOR_BRIGHT ���
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment_LCP(matSrcBuffer(rectBlobROI), MatColorBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_BRIGHT](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_BRIGHT, dPara ,_T("BP_"), EngineerDefectJudgment, pResultBlob);
		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			// �޸� ����
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}

		writeInspectLog(E_ALG_TYPE_SVI_MURA, __FUNCTION__, _T("BlobJudgment (BRIGHT)."));
	}

	// �޸� ����
	matSrcBuffer.release();
	matDstBuffer[E_DEFECT_COLOR_DARK].release();
	matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

	writeInspectLog(E_ALG_TYPE_SVI_MURA, __FUNCTION__, _T("End."));

	return E_ERROR_CODE_TRUE;
}

long CInspectConcealment::Image_Pow(int ImgType, double dpow, Mat& InPutImage, Mat& OutPutImage)
{
	double MaxValue = 4096;
	if (ImgType == CV_8UC1)
	{
		ushort LUT[256] = { 0 };
		double val = 0;
		for (int i = 0; i < 256; i++)
		{
			val = pow(i, dpow);

			if (val >= MaxValue) val = MaxValue - 1;
			if (val < 0) val = 0;
			LUT[i] = (ushort)val;
		}
		cv::MatIterator_<uchar> itSrc, endSrc;
		itSrc = InPutImage.begin<uchar>();
		endSrc = InPutImage.end<uchar>();
		cv::MatIterator_<ushort> itDst = OutPutImage.begin<ushort>();

		for (; itSrc != endSrc; itSrc++, itDst++)
			*itDst = LUT[(*itSrc)];
	}
	else
	{
		InPutImage.copyTo(OutPutImage);
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectConcealment::Binary_16bit(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double thresh, bool bInv, CMatBuf* cMemSub)
{
	// ���۰� ���� ���
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	// ���۰� ���� ���.
	if (matDstBuf.empty())
		matDstBuf = cv::Mat::zeros(matSrcBuf.size(), matSrcBuf.type());

	// ���۰� ������, 8U�� ���
	if (matDstBuf.type() == CV_8U)
	{
		uchar LUT[4096] = { 0, };
		memset(LUT, 0, sizeof(uchar) * 4096);

		// ����
		if (bInv)
		{
			for (int i = 0; i < thresh; i++)
				LUT[i] = (uchar)255;
		}
		else
		{
			for (int i = thresh + 1; i < 4096; i++)
				LUT[i] = (uchar)255;
		}

		MatIterator_<ushort> itSrc, endSrc;
		itSrc = matSrcBuf.begin<ushort>();
		endSrc = matSrcBuf.end<ushort>();
		MatIterator_<uchar> itDst = matDstBuf.begin<uchar>();

		for (; itSrc != endSrc; itSrc++, itDst++)
			*itDst = LUT[(int)(*itSrc)];
	}

	// ���۰� ������, 16U�� ���
	else
	{
		ushort LUT[4096] = { 0, };
		memset(LUT, 0, sizeof(ushort) * 4096);

		// ����
		if (bInv)
		{
			for (int i = 0; i < thresh; i++)
				LUT[i] = (ushort)4095;
		}
		else
		{
			for (int i = thresh + 1; i < 4096; i++)
				LUT[i] = (ushort)4095;
		}

		MatIterator_<ushort> itSrc, endSrc, itDst;
		itSrc = matSrcBuf.begin<ushort>();
		endSrc = matSrcBuf.end<ushort>();
		itDst = matDstBuf.begin<ushort>();

		for (; itSrc != endSrc; itSrc++, itDst++)
			*itDst = LUT[(int)(*itSrc)];
	}

	return E_ERROR_CODE_TRUE;
}

long  CInspectConcealment::C_Mexican_filter(cv::Mat& ip, int sz, int nMx_blur_sz, int nMx_blur_sigma) {
	int r = (sz - 1) / 2;
	// 	float* kernx = gauss1D(r);
	// 	float* kern_diff = diff2Gauss1D(r);
	// 	//kernel = new float[3][];
	// 	kernel[0] = kernx;
	// 	kernel[1] = kern_diff;
	float* kernel2 = computeKernel2D(r);
	//	kernel[2] = kernel2;

	// 	if (sep)
	// 		convolveSep(ip, kernx, kern_diff);
	// 	else {
	//Convolver con = new Convolver();
	boolean err = true;
	err = convolveFloat(ip, kernel2, sz, sz, sz);
	//}
	double sigma2 = (sz - 1) / 6.0;
	sigma2 *= sigma2;
	ip *= sigma2;

	cv::GaussianBlur(ip, ip, cv::Size(nMx_blur_sz, nMx_blur_sz), nMx_blur_sigma, nMx_blur_sigma);

	free(kernel2);

	if (err) {
		return E_ERROR_CODE_TRUE;
	}
	else {
		return E_ERROR_CODE_MURA_WRONG_PARA;
	}
}

float*  CInspectConcealment::computeKernel2D(int r) {
	int sz = 2 * r + 1;
	double sigma2 = 2 * ((double)r / 3.0 + 1 / 6.0)*((double)r / 3.0 + 1 / 6.0);
	float* kernel;
	kernel = (float *)malloc(sz*sz * sizeof(float));
	double PIs = 4 / sqrt(PI*sigma2) / sigma2 / sigma2;
	float sum = 0;
	for (int u = -r; u <= r; u++) {
		for (int w = -r; w <= r; w++) {
			double x2 = u*u + w*w;
			int idx = u + r + sz*(w + r);
			kernel[idx] = (float)((x2 - sigma2)*exp(-x2 / sigma2)*PIs);
			///System.out.print(kernel[c] +" ");		
			sum += kernel[idx];

		}
	}
	sum = abs(sum);
	if (sum < 1e-5) sum = 1;
	if (sum != 1) {
		for (int i = 0; i < sz*sz; i++) {
			kernel[i] /= sum;
			//System.out.print(kernel[i] +" ");
		}
	}
	return kernel;
}

boolean  CInspectConcealment::convolveFloat(cv::Mat& ip, float* kernel, int kw, int kh, int sz) {

	//if (canceled) return false;
	int width = ip.cols;
	int height = ip.rows;
	Rect r(0, 0, width, height);
	int x1 = r.x;
	int y1 = r.y;
	int x2 = x1 + r.width;
	int y2 = y1 + r.height;
	int uc = kw / 2;
	int vc = kh / 2;
	//float pixels = width*height;
	bool e_size = true;

	cv::Mat pixels;
	cv::Mat pixels2;
	pixels = ip;
	ip.copyTo(pixels2);

	double scale = getScale(kernel, sz);
	// 	Thread thread = Thread.currentThread();
	// 	boolean isMainThread = thread == mainThread || thread.getName().indexOf("Preview") != -1;
	// 	if (isMainThread) pass++;
	double sum;
	int offset, i;
	boolean edgePixel;
	int xedge = width - uc;
	int yedge = height - vc;
	//long lastTime = System.currentTimeMillis();

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int y = y1; y < y2; y++) {
		// 		long time = System.currentTimeMillis();
		// 		if (time - lastTime > 100) {
		// 			lastTime = time;
		// 			if (thread.isInterrupted()) return false;
		// 			if (isMainThread) {
		// 				if (IJ.escapePressed()) {
		// 					canceled = true;
		// 					ip.reset();
		// 					ImageProcessor originalIp = imp.getProcessor();
		// 					if (originalIp.getNChannels() > 1)
		// 						originalIp.reset();
		// 					return false;
		// 				}
		// 				showProgress((y - y1) / (double)(y2 - y1));
		// 			}
		// 		}
		for (int x = x1; x < x2; x++) {
			//if (canceled) return false;
			sum = 0.0;
			i = 0;
			edgePixel = y < vc || y >= yedge || x < uc || x >= xedge;
			for (int v = -vc; v <= vc; v++) {
				offset = x + (y + v)*width;
				for (int u = -uc; u <= uc; u++) {
					if (edgePixel) {
						if (i >= sz*sz) {
							e_size = false;
							break;
						}

						int x_r = x + u;
						int y_r = y + v;

						if (x_r < 0) { x_r = 0; }
						if (x_r > width - 1) { x_r = width - 1; }
						if (y_r < 0) { y_r = 0; }
						if (y_r > height - 1) { y_r = height - 1; }

						sum += pixels2.at<uchar>(y_r, x_r)*kernel[i++];
					}
					else {

						sum += pixels2.data[offset + u] * kernel[i++];
					}
				}
			}
			int input = sum*scale;
			if (input < 0) { input = 0; }
			if (input > 255) { input = 255; }
			pixels.data[x + y*width] = input;
		}

		if (!e_size) {
			break;
		}

	}

	if (e_size) {
		return true;
	}
	else {
		return false;
	}

}

double CInspectConcealment::getScale(float* kernel, int sz) {
	double scale = 1.0;
	double sum = 0.0;
	for (int i = 0; i < sz*sz; i++)
		sum += kernel[i];
	if (sum != 0.0)
		scale = 1.0 / sum;
	return scale;
}

void CInspectConcealment::Mask_SizeDown(cv::Mat matSrc, int nUpSize)
{
	cv::Mat Temp16;

	cv::distanceTransform(matSrc, Temp16, CV_DIST_L2, 3);

	cv::threshold(Temp16, Temp16, nUpSize, 255, CV_THRESH_BINARY);
	Temp16.convertTo(matSrc, CV_8UC1);
}

void CInspectConcealment::Mask_SizeUp(cv::Mat matSrc, int nDownSize)
{
	cv::Mat matTemp;
	cv::Mat Temp16;

	cv::bitwise_not(matSrc, matTemp);
	cv::distanceTransform(matTemp, Temp16, CV_DIST_L2, 3);

	cv::threshold(Temp16, Temp16, nDownSize, 255, CV_THRESH_BINARY);
	Temp16.convertTo(matTemp, CV_8UC1);
	cv::bitwise_not(matTemp, matSrc);
}

int CInspectConcealment::First_blob_Concealment(cv::Mat Srcimg, cv::Mat DstImg, cv::Mat matLabels, cv::Mat stats, cv::Mat centroids, cv::Rect rectTemp_Dust, double* dPara)
{
	float	fAxis_Ratio = (float)dPara[E_PARA_LCP_CONCEALMENT_AREA_FIRST_JUDGE_AXIO_RATIO];
	float	fArea_Ratio = (float)dPara[E_PARA_LCP_CONCEALMENT_AREA_FIRST_JUDGE_AREA_RATIO];
	long	nArea_Min = (long)dPara[E_PARA_LCP_CONCEALMENT_AREA_FIRST_JUDGE_AREA_MIN];
	long	nArea_Max = (long)dPara[E_PARA_LCP_CONCEALMENT_AREA_FIRST_JUDGE_AREA_MAX];
	long	nJudge1_Flag = (long)dPara[E_PARA_LCP_CONCEALMENT_AREA_FIRST_JUDGE_SPEC1_USE];
	long	nJudge1_Area_Min = (long)dPara[E_PARA_LCP_CONCEALMENT_AREA_FIRST_JUDGE_SPEC1_AREA_MIN];
	float	fJudge1_Axis_Ratio = (float)dPara[E_PARA_LCP_CONCEALMENT_AREA_FIRST_JUDGE_SPEC1_AXIO_RATIO];


	__int64 numOfLables;
	numOfLables = connectedComponentsWithStats(DstImg, matLabels, stats, centroids, 8, CV_32S) - 1;

	double ratio = 0;
	double area_ratio = 0;
	int rect_count = 0;


	// Blob ����
	for (int idx = 1; idx <= numOfLables; idx++)
	{
		// ��ü ����
		long nArea = stats.at<int>(idx, CC_STAT_AREA);

		// Blob ���� Rect

		rectTemp_Dust.x = stats.at<int>(idx, CC_STAT_LEFT);
		rectTemp_Dust.y = stats.at<int>(idx, CC_STAT_TOP);
		rectTemp_Dust.width = stats.at<int>(idx, CC_STAT_WIDTH);
		rectTemp_Dust.height = stats.at<int>(idx, CC_STAT_HEIGHT);


		if (rectTemp_Dust.width >= rectTemp_Dust.height)
			ratio = ((double)rectTemp_Dust.width / (double)rectTemp_Dust.height);
		else
			ratio = ((double)rectTemp_Dust.height / (double)rectTemp_Dust.width);


		area_ratio = ((double)nArea / ((double)rectTemp_Dust.width *  (double)rectTemp_Dust.height)) * 100.0;


		rect_count++;

		// ���� ����
		// 		if (
		// 			ratio >= 3 ||
		// 			area_ratio < 50 ||
		// 			nArea <= 4 || nArea >= 200 ||
		// 			(nArea >= 50 && ratio >= 2.2))
		// 		{
		if (
			ratio >= fAxis_Ratio ||
			area_ratio < fArea_Ratio ||
			nArea <= nArea_Min || nArea >= nArea_Max ||
			(nJudge1_Flag > 0 && nArea >= nJudge1_Area_Min && ratio >= fJudge1_Axis_Ratio))
		{
			rect_count--;
			// 0GV �� �ʱ�ȭ ��, �Ѿ��
			cv::Mat matTempROI = DstImg(rectTemp_Dust);
			cv::Mat matLabelROI = matLabels(rectTemp_Dust);

			for (int y = 0; y < rectTemp_Dust.height; y++)
			{
				int		*ptrLabel = (int *)matLabelROI.ptr(y);
				uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

				for (int x = 0; x < rectTemp_Dust.width; x++, ptrLabel++, ptrGray++)
				{
					// �ش� ��ü
					if (*ptrLabel == idx)	*ptrGray = 0;
				}
			}

			continue;
		}


	}

	return numOfLables;
}