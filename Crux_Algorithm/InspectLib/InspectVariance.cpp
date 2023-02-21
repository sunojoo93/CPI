/************************************************************************/
// Variance 불량 검출 관련 소스
// 수정일 : 22.01.11 - CKI
/************************************************************************/

#include "StdAfx.h"
#include "InspectVariance.h"


enum enFlattening
{
	Flattening_None, Flattening_Horizental, Flattening_Vertical, Flattening_All_H, Flattening_All_V
};

#define _BIAS_	128

CInspectVariance::CInspectVariance()
{
	cMem = NULL;
	m_cInspectLibLog = NULL;
	m_strAlgLog = NULL;
	m_tInitTime = 0;
	m_tBeforeTime = 0;

}


CInspectVariance::~CInspectVariance()
{
}

long CInspectVariance::DoFindVarianceDefect(cv::Mat matSrcBuffer, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer, cv::Mat matMask_Active,
	cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, CDefectCCD *cCCD, cv::Mat* matCholeBuffer)
{
	// 파라미터 NULL인 경우
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	long	nWidth = (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcBuffer.rows;	// 영상 세로 크기

												// 영상 번호
	long	nImageClassify = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];

	// ID 찾기
	CString strPathTemp;

	// 오른쪽 끝에 "\\" 제거
	strPathTemp.Format(_T("%s"), strAlgPath.Left(strAlgPath.GetLength() - 1));

	// 오른쪽에서 "\\" 찾기
	int nLength = strPathTemp.ReverseFind(_T('\\'));

	// ID만 찾기
	wchar_t	strID[MAX_PATH] = { 0, };
	swprintf_s(strID, MAX_PATH, L"%s", (LPCWSTR)strPathTemp.Right(strPathTemp.GetLength() - nLength - 1));

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Start."));

	// 범위 크게 설정
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	// 검사 영역이 영상 크기를 벗어난 경우
	if (rectROI.left < 0 ||
		rectROI.top < 0 ||
		rectROI.right >= nWidth ||
		rectROI.bottom >= nHeight)	return E_ERROR_CODE_ROI_OVER;

	if (rectROI.left >= rectROI.right)	return E_ERROR_CODE_ROI_OVER;
	if (rectROI.top >= rectROI.bottom)	return E_ERROR_CODE_ROI_OVER;

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	cv::Mat matDstImage[E_DEFECT_COLOR_COUNT];

	// 버퍼 할당 & 초기화
	// KSW 17.09.26 - Dust 패턴에서 Dark	- Area 큰 부유성 이물
	// KSW 17.09.26 - Dust 패턴에서 BRIGHT	- 모든 부유성 이물
	matDstImage[E_DEFECT_COLOR_DARK] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());
	matDstImage[E_DEFECT_COLOR_BRIGHT] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());

	// 결과 버퍼가 NULL이 아닌 경우 에러
	if (matDstImage[E_DEFECT_COLOR_DARK].empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstImage[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;

	switch (nImageClassify)
	{
		//RGB Pattern 검사
	case AREA_CAM_IMG_CLASSIFY_NUM:
	{

		nErrorCode = LCP_AREA_Variance(matSrcBuffer, matDstImage, matMask_Active, rectROI, rcAlignCellROI, dPara, nCommonPara, strAlgPath, strBubblePath, strBubblePath_Menual, eInspMode, EngineerDefectJudgment);


	}
	break;

	// 영상 번호가 잘못 들어온 경우.
	default:
		return E_ERROR_CODE_TRUE;
	}


	matDarkBuffer = matDstImage[E_DEFECT_COLOR_DARK].clone();
	matBrightBuffer = matDstImage[E_DEFECT_COLOR_BRIGHT].clone();

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Result Copy."));

	// 할당 해제
	matDstImage[E_DEFECT_COLOR_DARK].release();
	matDstImage[E_DEFECT_COLOR_BRIGHT].release();

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("End.Dispersion"));

	return nErrorCode;
}

long CInspectVariance::LCP_AREA_Variance(cv::Mat& matSrcImage, cv::Mat* matDstImage, cv::Mat matMask_Active, CRect rectROI, cv::Rect rcAlignCellROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;  //LCP 중간이미지
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	//float 	pow_value = (float)dPara[E_PARA_LCP_VARIANCE_COMMON_POW_VALUE];
	//////////////////////////////////////////////////////////////////////////
	int nMaxican_Size = (int)dPara[E_PARA_LCP_VARIANCE_AREA_MAXICAN_SIZE];
	int nMaxican_GauSize = (int)dPara[E_PARA_LCP_VARIANCE_AREA_MAXICAN_GAU_SIZE];
	float fMaxican_GauSig = (float)dPara[E_PARA_LCP_VARIANCE_AREA_MAXICAN_GAU_SIG];

	int nGau_Size = (int)dPara[E_PARA_LCP_VARIANCE_AREA_DELETE_GAU_SIZE];
	float nGau_Sig = (float)dPara[E_PARA_LCP_VARIANCE_AREA_DELETE_GAU_SIG];
	int nThreshold = (int)dPara[E_PARA_LCP_VARIANCE_AREA_DELETE_THRESHOLD];

	int nJudge_Min_Sz = (int)dPara[E_PARA_LCP_VARIANCE_AREA_JUDGE_MIN_SIZE];
	int nJudge_Max_Sz = (int)dPara[E_PARA_LCP_VARIANCE_AREA_JUDGE_MAX_SIZE];
	int nJudge_Radius = (int)dPara[E_PARA_LCP_VARIANCE_AREA_JUDGE_RADIUS];
	//////////////////////////////////////////////////////////////////////////

	CRect rectTemp(rectROI);

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_VARIANCE_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}


	cv::Mat matSrc_TempBuf = cv::Mat::zeros(matSrcImage.size(), matSrcImage.type());
	matSrcImage.copyTo(matSrc_TempBuf);

	cv::equalizeHist(matSrc_TempBuf, matSrc_TempBuf);


	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_VARIANCE_%02d_Contrast.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrc_TempBuf);
	}

	C_Mexican_filter(matSrc_TempBuf, nMaxican_Size, nMaxican_GauSize, fMaxican_GauSig);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_VARIANCE_%02d_MexicanHat_Filter.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrc_TempBuf);
	}

	RangeAvgThreshold(matSrc_TempBuf, matDstImage, rectTemp, dPara, cMem);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_VARIANCE_%02d_Variance_RangeThreshold_Dark.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}

	cv::GaussianBlur(matDstImage[E_DEFECT_COLOR_DARK], matDstImage[E_DEFECT_COLOR_DARK], cv::Size(nGau_Size, nGau_Size), nGau_Sig);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_VARIANCE_%02d_Variance_Gau.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}

	cv::threshold(matDstImage[E_DEFECT_COLOR_DARK], matDstImage[E_DEFECT_COLOR_DARK], nThreshold, 255, CV_THRESH_BINARY);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_VARIANCE_%02d_Variance_Final_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}

	cv::bitwise_and(matDstImage[E_DEFECT_COLOR_DARK], matMask_Active, matDstImage[E_DEFECT_COLOR_DARK]);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_VARIANCE_%02d_Variance_Active_Mask.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}


	cv::Point ptCenter(rcAlignCellROI.x + rcAlignCellROI.width / 2, rcAlignCellROI.y + rcAlignCellROI.height / 2);

	cv::Mat matDraw;
	matDstImage[E_DEFECT_COLOR_DARK].copyTo(matDraw);
	cv::cvtColor(matDraw, matDraw, COLOR_GRAY2RGB);
	

	DeleteArea_Variance(matDstImage[E_DEFECT_COLOR_DARK], matDraw, rcAlignCellROI ,nJudge_Min_Sz, nJudge_Max_Sz, nJudge_Radius ,ptCenter, &cMatBufTemp);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_VARIANCE_%02d_Variance_Judge.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDraw);
	}

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_AREA_VARIANCE_%02d_Variance_Delete_Area.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}

	

	return E_ERROR_CODE_TRUE;
}

long CInspectVariance::RangeAvgThreshold(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if (matSrcImage.type() == CV_8U)
		nErrorCode = RangeAvgThreshold_8bit(matSrcImage, matDstImage, rectROI, dPara, cMemSub);
	else
		nErrorCode = RangeAvgThreshold_16bit(matSrcImage, matDstImage, rectROI, dPara, cMemSub);

	return nErrorCode;
}

// 분할 영역 이진화
long CInspectVariance::RangeAvgThreshold_8bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	long	nBlurLoop_5x5 = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_BLUR_LOOP];
	long	nBlur_Size = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_BLUR_LOOP_SIZE];

	long	nSegX = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_SEG_X];
	long	nSegY = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_SEG_Y];
	long	nEdgeArea = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_EDGE_AREA];

	float	fActiveDarkRatio = (float)dPara[E_PARA_LCP_VARIANCE_COMMON_ACTIVE_DARK_RATIO];
	float	fActiveBrightRatio = (float)dPara[E_PARA_LCP_VARIANCE_COMMON_ACTIVE_BRIGHT_RATIO];
	float	fEdgeDarkRatio = (float)dPara[E_PARA_LCP_VARIANCE_COMMON_EDGE_DARK_RATIO];
	float	fEdgeBrightRatio = (float)dPara[E_PARA_LCP_VARIANCE_COMMON_EDGE_BRIGHT_RATIO];

	int		nMinThGV = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_MIN_THRESHOLD];
	int		nMedian = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_MEDIAN];

	// 설정 값이 10보다 작은 경우.
	if (nSegX <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;
	if (nSegY <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;

	// 이미지 크기
	long	nWidth = (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcImage.rows;	// 영상 세로 크기

	long nStart_Y, nEnd_Y;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// Temp 메모리 할당
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

	// 범위 계산
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

		// y 범위 계산
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

			// x 범위 계산
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

			// 범위 설정
			rectPtr[x].x = nStart_X;
			rectPtr[x].y = nStart_Y;
			rectPtr[x].width = nEnd_X - nStart_X + 1;
			rectPtr[x].height = nEnd_Y - nStart_Y + 1;
		}
	}

	// 평균 계산
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
			// 영상 ROI
			cv::Mat matTempBuf = matBlurBuf(rectPtr[x]);

			// mean & stdDev 구하기
			//	cv::Scalar m, s;
			//	cv::meanStdDev(matTempBuf, m, s);

			// 히스토그램
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			double dblAverage;
			double dblStdev;
			GetMeanStdDev_From_Histo(matHisto, 0, 255, dblAverage, dblStdev);

			// 평균 범위 설정
			//	int nMinGV = (int)(m[0] - s[0]);
			//	int nMaxGV = (int)(m[0] + s[0]);
			int nMinGV = (int)(dblAverage - dblStdev);
			int nMaxGV = (int)(dblAverage + dblStdev);
			if (nMinGV < 0)	nMinGV = 0;
			if (nMaxGV > 255)	nMaxGV = 255;

			// 초기화
			__int64 nPixelSum = 0;
			__int64 nPixelCount = 0;

			// 설정된 평균 범위만큼만 재평균
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m = nMinGV; m <= nMaxGV; m++, pVal++)
			{
				nPixelSum += (m * *pVal);
				nPixelCount += *pVal;
			}

			// 개수가 하나라도 있어야...
			if (nPixelCount > 0)
				ptr[x] = (BYTE)(nPixelSum / (float)nPixelCount);
		}
	}

	// 주변 평균값 -> 중간값으로 설정
	MedianFilter(matAvgBuf, matAvgBuf, nMedian, &cMatBufTemp);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("MedianFilter."));

	// 이진화
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
			// 영상 ROI
			cv::Mat matTempBuf = matBlurBuf(rectPtr[x]);

			// 평균 * Ratio
			long nDarkTemp, nBrightTemp;

			// Edge 설정 따로 하기
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

			// Gray 너무 어두운 경우 있음.
			// ( 평균 GV 2 ~ 3 * fBrightRatio -> 이진화 : 노이즈 다 올라옴 )
			if (nBrightTemp < nMinThGV)	nBrightTemp = nMinThGV;

			// 예외 처리
			if (nDarkTemp < 0)		nDarkTemp = 0;
			if (nBrightTemp > 255)	nBrightTemp = 255;

			// 파라미터 0인경우 예외 처리
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

	// 해제
	matAvgBuf.release();
	delete[] rectRange;
	rectRange = NULL;

	// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
	// 	}

	return E_ERROR_CODE_TRUE;
}

long CInspectVariance::RangeAvgThreshold_16bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	long	nBlurLoop_5x5 = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_BLUR_LOOP];
	long	nBlur_Size = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_BLUR_LOOP_SIZE];

	long	nSegX = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_SEG_X];
	long	nSegY = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_SEG_Y];
	long	nEdgeArea = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_EDGE_AREA];

	float	fActiveDarkRatio = (float)dPara[E_PARA_LCP_VARIANCE_COMMON_ACTIVE_DARK_RATIO];
	float	fActiveBrightRatio = (float)dPara[E_PARA_LCP_VARIANCE_COMMON_ACTIVE_BRIGHT_RATIO];
	float	fEdgeDarkRatio = (float)dPara[E_PARA_LCP_VARIANCE_COMMON_EDGE_DARK_RATIO];
	float	fEdgeBrightRatio = (float)dPara[E_PARA_LCP_VARIANCE_COMMON_EDGE_BRIGHT_RATIO];

	int		nMinThGV = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_MIN_THRESHOLD];
	int		nMedian = (long)dPara[E_PARA_LCP_VARIANCE_COMMON_MEDIAN];

	// 설정 값이 10보다 작은 경우.
	if (nSegX <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;
	if (nSegY <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;

	// 이미지 크기
	long	nWidth = (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcImage.rows;	// 영상 세로 크기

	long nStart_Y, nEnd_Y;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// Temp 메모리 할당	
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

	// 범위 계산
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

		// y 범위 계산
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

			// x 범위 계산
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

			// 범위 설정
			rectPtr[x].x = nStart_X;
			rectPtr[x].y = nStart_Y;
			rectPtr[x].width = nEnd_X - nStart_X + 1;
			rectPtr[x].height = nEnd_Y - nStart_Y + 1;
		}
	}

	// 평균 계산
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
			// 영상 ROI
			cv::Mat matTempBuf = matBlurBuf(rectPtr[x]);

			// mean & stdDev 구하기
			cv::Scalar m, s;
			cv::meanStdDev(matTempBuf, m, s);

			// 평균 범위 설정
			int nMinGV = (int)(m[0] - s[0]);
			int nMaxGV = (int)(m[0] + s[0]);
			if (nMinGV < 0)	nMinGV = 0;
			if (nMaxGV > 4095)	nMaxGV = 4095;

			// 히스토그램
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			// 초기화
			__int64 nPixelSum = 0;
			__int64 nPixelCount = 0;

			// 설정된 평균 범위만큼만 재평균
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m = nMinGV; m <= nMaxGV; m++, pVal++)
			{
				nPixelSum += (m * *pVal);
				nPixelCount += *pVal;
			}

			// 개수가 하나라도 있어야...
			if (nPixelCount > 0)
				ptr[x] = (ushort)(nPixelSum / (float)nPixelCount);
		}
	}

	// 주변 평균값 -> 중간값으로 설정
	MedianFilter(matAvgBuf, matAvgBuf, nMedian, &cMatBufTemp);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("MedianFilter."));

	// 이진화
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
			// 영상 ROI
			cv::Mat matTempBuf = matBlurBuf(rectPtr[x]);

			// 평균 * Ratio
			long nDarkTemp, nBrightTemp;

			// Edge 설정 따로 하기
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

			// Gray 너무 어두운 경우 있음.
			// ( 평균 GV 2 ~ 3 * fBrightRatio -> 이진화 : 노이즈 다 올라옴 )
			if (nBrightTemp < nMinThGV)	nBrightTemp = nMinThGV;

			// 예외 처리
			if (nDarkTemp < 0)			nDarkTemp = 0;
			if (nBrightTemp > 4095)	nBrightTemp = 4095;

			// 파라미터 0인경우 예외 처리
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

	// 해제
	matAvgBuf.release();
	delete[] rectRange;
	rectRange = NULL;

	// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
	// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
	// 	}

	return E_ERROR_CODE_TRUE;
}


long CInspectVariance::GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto)
{
	// 버퍼가 없는 경우.
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

long CInspectVariance::GetAverage_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage)
{
	double dblValSum = 0;
	double dblCountSum = 0;
	float *pVal = (float*)matHisto.data;

	// 평균을 구한다.
	for (int i = nMin; i < nMax; i++)
	{
		dblValSum += (*pVal * i);
		dblCountSum += *pVal;
		pVal++;
	}

	dblAverage = dblValSum / dblCountSum;

	return E_ERROR_CODE_TRUE;
}

long CInspectVariance::GetMeanStdDev_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage, double &dblStdev)
{
	GetAverage_From_Histo(matHisto, nMin, nMax, dblAverage);

	double dblValSum = 0;
	double dblCountSum = 0;
	double dblTmp;
	float *pVal = (float*)matHisto.data;

	// 분산을 구한다.
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

long CInspectVariance::MedianFilter(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nKSize, CMatBuf* cMemSub)
{
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	// 에러 코드
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
				// 범위 확인
				int nSX = xx - nHSize;
				int nEX = xx + nHSize;
				int nSY = yy - nHSize;
				int nEY = yy + nHSize;

				// 예외 처리
				nSX = (nSX < 0) ? 0 : nSX;
				nSY = (nSY < 0) ? 0 : nSY;
				nEX = (nEX >= matSrcBuf.cols) ? matSrcBuf.cols - 1 : nEX;
				nEY = (nEY >= matSrcBuf.rows) ? matSrcBuf.rows - 1 : nEY;

				int nCount = 0;

				// 데이터 넣기
				for (int nMy = nSY; nMy <= nEY; nMy++)
				{
					ushort *ptrSrc = (ushort *)matSrcBuf.ptr(nMy) + nSX;

					for (int nMx = nSX; nMx <= nEX; nMx++, ptrSrc++)
					{
						nArr[nCount++] = *ptrSrc;
					}
				}

				// 정렬
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

				// 결과
				*ptrDst = nArr[nCount / 2];
			}
		}

		delete[] nArr;
		nArr = NULL;

		// 결과 복사
		matTempBuf.copyTo(matDstBuf);

		// 메모리 해제
		matTempBuf.release();

		// 		if (m_cInspectLibLog->Use_AVI_Memory_Log) {
		// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
		// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
		// 		}

	}

	return nErrorCode;
}

void CInspectVariance::Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset)
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

long CInspectVariance::ImageSave(CString strPath, cv::Mat matSrcBuf)
{
	if (matSrcBuf.type() == CV_8U)
	{
		cv::imwrite((cv::String)(CStringA)strPath, matSrcBuf);
	}
	else if(matSrcBuf.type() == CV_8UC3)
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
long CInspectVariance::GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer, cv::Mat matDstBuffer[2], cv::Mat& matDrawBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 파라미터 NULL인 경우.
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (pResultBlob == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if (matSrcBuffer.empty())							return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuffer[E_DEFECT_COLOR_DARK].empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuffer[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;



	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
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

	// 범위 크게 설정
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("Start."));


	// 불량 판정 & 결과 보내기
	{
		cv::Rect rectBlobROI;
		Insp_RectSet(rectBlobROI, rectROI, matSrcBuffer.cols, matSrcBuffer.rows);

		// 라벨
		CFeatureExtraction cFeatureExtraction;
		cFeatureExtraction.SetMem(cMem);
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_MURA_4, m_tInitTime, m_tBeforeTime, m_strAlgLog);

		// E_DEFECT_COLOR_DARK 결과
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment_LCP(matSrcBuffer(rectBlobROI), MatColorBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_DARK](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_DARK, dPara,_T("DP_"), EngineerDefectJudgment, pResultBlob);
		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			// 메모리 해제
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}

		writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("BlobJudgment (DARK)."));



		// E_DEFECT_COLOR_BRIGHT 결과
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment_LCP(matSrcBuffer(rectBlobROI), MatColorBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_BRIGHT](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_BRIGHT, dPara, _T("BP_"), EngineerDefectJudgment, pResultBlob);
		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			// 메모리 해제
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}

		writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("BlobJudgment (BRIGHT)."));
	}

	// 메모리 해제
	matSrcBuffer.release();
	matDstBuffer[E_DEFECT_COLOR_DARK].release();
	matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

	writeInspectLog(E_ALG_TYPE_AVI_MURA_4, __FUNCTION__, _T("End."));

	return E_ERROR_CODE_TRUE;
}

long CInspectVariance::Image_Pow(int ImgType, double dpow, Mat& InPutImage, Mat& OutPutImage)
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

long CInspectVariance::Binary_16bit(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double thresh, bool bInv, CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼가 없는 경우.
	if (matDstBuf.empty())
		matDstBuf = cv::Mat::zeros(matSrcBuf.size(), matSrcBuf.type());

	// 버퍼가 있으나, 8U인 경우
	if (matDstBuf.type() == CV_8U)
	{
		uchar LUT[4096] = { 0, };
		memset(LUT, 0, sizeof(uchar) * 4096);

		// 반전
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

	// 버퍼가 있으나, 16U인 경우
	else
	{
		ushort LUT[4096] = { 0, };
		memset(LUT, 0, sizeof(ushort) * 4096);

		// 반전
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

long  CInspectVariance::C_Mexican_filter(cv::Mat& ip, int sz, int nMx_blur_sz, int nMx_blur_sigma) {
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

float*  CInspectVariance::computeKernel2D(int r) {
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

boolean  CInspectVariance::convolveFloat(cv::Mat& ip, float* kernel, int kw, int kh, int sz) {

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

double CInspectVariance::getScale(float* kernel, int sz) {
	double scale = 1.0;
	double sum = 0.0;
	for (int i = 0; i < sz*sz; i++)
		sum += kernel[i];
	if (sum != 0.0)
		scale = 1.0 / sum;
	return scale;
}

long CInspectVariance::DeleteArea_Variance(cv::Mat& matSrcImage, cv::Mat& matDraw, cv::Rect rcAlignCellROI, int nMin_Count, int nMax_Count, int nRadius, cv::Point ptCenter ,CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우.
	if (matSrcImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	//중간 이미지 확인용

	cv::circle(matDraw, ptCenter, nRadius, Scalar(255,255,0),3);

	//////////////////////////////////////////////////////////////////////////

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);


	// 메모리 할당
	cv::Mat matLabel, matStats, matCentroid;
	matLabel = cMatBufTemp.GetMat(matSrcImage.size(), CV_32SC1, false);

	// Blob 개수
	__int64 nTotalLabel = cv::connectedComponentsWithStats(matSrcImage, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

	// 개수 없는 경우 나가기
	if (nTotalLabel <= 0)	return E_ERROR_CODE_TRUE;

	// Blob 개수
	for (int idx = 1; idx <= nTotalLabel; idx++)
	{
		// 객체 면적
		long nArea = matStats.at<int>(idx, CC_STAT_AREA);

		// Blob 영역 Rect
		cv::Rect rectTemp;
		rectTemp.x = matStats.at<int>(idx, CC_STAT_LEFT);
		rectTemp.y = matStats.at<int>(idx, CC_STAT_TOP);
		rectTemp.width = matStats.at<int>(idx, CC_STAT_WIDTH);
		rectTemp.height = matStats.at<int>(idx, CC_STAT_HEIGHT);

		// 면적 제한
		if (nArea <= nMin_Count || nArea >= nMax_Count)
		{
			// 0GV 로 초기화 후, 넘어가기
			cv::Mat matTempROI = matSrcImage(rectTemp);
			cv::Mat matLabelROI = matLabel(rectTemp);

			for (int y = 0; y<rectTemp.height; y++)
			{
				int		*ptrLabel = (int *)matLabelROI.ptr(y);
				uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

				for (int x = 0; x<rectTemp.width; x++, ptrLabel++, ptrGray++)
				{
					// 해당 객체
					if (*ptrLabel == idx)	*ptrGray = 0;
				}
			}

			continue;
		}
		else {
			vector<vector<cv::Point>>	contours;
			vector<vector<cv::Point>>().swap(contours);
			cv::findContours(matSrcImage(rectTemp), contours, CV_RETR_LIST, CHAIN_APPROX_NONE);

			int nBig_Size = 0;
			int nBig_Index = 0;

			for (int i = 0; i < contours.size(); i++) {
				if (contours[i].size() > nBig_Size) {
					nBig_Size = contours[i].size();
					nBig_Index = i;
				}
			}

			cv::RotatedRect BoundingBox = cv::minAreaRect(contours[nBig_Index]);

			//////////////////////////////////////////////////////////////////////////
			cv::Point2f vertices[E_CORNER_END];
			BoundingBox.points(vertices);

			cv::Point2f ptOri(rectTemp.x, rectTemp.y);
				// 확인용
			
			//////////////////////////////////////////////////////////////////////////
			double dDistance_1 = sqrt(pow(vertices[0].x - vertices[1].x, 2) + pow(vertices[0].y - vertices[1].y, 2));
			double dDistance_2 = sqrt(pow(vertices[1].x - vertices[2].x, 2) + pow(vertices[1].y - vertices[2].y, 2));

			double dAngle = 0.0;
			int nVertical_mode = 0;

			if (dDistance_1 > dDistance_2) {
				if ((vertices[1].x - vertices[0].x) != 0.f) {
					dAngle = (vertices[1].y - vertices[0].y) / (vertices[1].x - vertices[0].x);
				}
				else {
					dAngle = (vertices[1].y - vertices[0].y) / (vertices[1].x - vertices[0].x);
					nVertical_mode = 1;
				}
			}
			else {
				if ((vertices[2].x - vertices[1].x) != 0.f) {
					dAngle = (vertices[2].y - vertices[1].y) / (vertices[2].x - vertices[1].x);
				}
				else {
					dAngle = (vertices[2].y - vertices[1].y) / (vertices[2].x - vertices[1].x);
					nVertical_mode = 1;
				}
			}

			//double dTheta = atan(-dAngle) * 180. / PI;

			cv::Point2d pt_1(rectTemp.x + BoundingBox.center.x, rectTemp.y + BoundingBox.center.y);

			
			double a = 0.0;
			double b = 0.0;
			double c = 0.0;
			double d = 0.0;

			if (nVertical_mode <= 0) {
				a = dAngle;
				b = -1;
				c = -dAngle*pt_1.x + pt_1.y;
				d = abs(a* (double)ptCenter.x + b*(double)ptCenter.y + c) / sqrt((a*a) + (b*b));
			}
			else {
				a = 0.0;
				b = 0.0;
				c = ptCenter.x;
				d = abs(pt_1.x - c);
			}

			//////////////////////////////////////////////////////////////////////////

			vector<double> vt_Y;

			cv::Point pt1;
			cv::Point pt2;

			if (nVertical_mode <= 0) {
				for (int i = 0; i < matSrcImage.cols; i++) {

					vt_Y.push_back(-a*i - c);
				}

					pt1.x = 0;
					pt1.y = -vt_Y[0];

					pt2.x = matSrcImage.cols - 1;
					pt2.y = -vt_Y[matSrcImage.cols - 1];

				
			}
			else {
				pt1.x = pt_1.x;
				pt1.y = 0;

				pt2.x = pt_1.x;
				pt2.y = matSrcImage.rows - 1;
			
			}

		
			
			//////////////////////////////////////////////////////////////////////////

			if (d > nRadius) {

				cv::Mat matTempROI = matSrcImage(rectTemp);
				cv::Mat matLabelROI = matLabel(rectTemp);

				for (int y = 0; y < rectTemp.height; y++)
				{
					int		*ptrLabel = (int *)matLabelROI.ptr(y);
					uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

					for (int x = 0; x < rectTemp.width; x++, ptrLabel++, ptrGray++)
					{
						// 해당 객체
						if (*ptrLabel == idx)	*ptrGray = 0;
					}
				}

				cv::circle(matDraw, pt_1, 2, Scalar(255, 0, 255), 2);

				cv::line(matDraw, ptOri + vertices[0], ptOri + vertices[1], cv::Scalar(100, 100, 100),5);
				cv::line(matDraw, ptOri + vertices[1], ptOri + vertices[2], cv::Scalar(100, 100, 100),5);
				cv::line(matDraw, ptOri + vertices[2], ptOri + vertices[3], cv::Scalar(100, 100, 100),5);
				cv::line(matDraw, ptOri + vertices[3], ptOri + vertices[0], cv::Scalar(100, 100, 100),5);

				cv::line(matDraw, pt1, pt2, Scalar(0, 0, 255), 2);

				continue;
			}
			else {

				cv::circle(matDraw, pt_1, 2, Scalar(255, 0, 255), 2);

				cv::line(matDraw, ptOri + vertices[0], ptOri + vertices[1], cv::Scalar(0, 255, 0),5);
				cv::line(matDraw, ptOri + vertices[1], ptOri + vertices[2], cv::Scalar(0, 255, 0),5);
				cv::line(matDraw, ptOri + vertices[2], ptOri + vertices[3], cv::Scalar(0, 255, 0),5);
				cv::line(matDraw, ptOri + vertices[3], ptOri + vertices[0], cv::Scalar(0, 255, 0),5);

				cv::line(matDraw, pt1, pt2, Scalar(0, 255, 0), 2);

				continue;
			}
		}
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}
	

	return E_ERROR_CODE_TRUE;
}