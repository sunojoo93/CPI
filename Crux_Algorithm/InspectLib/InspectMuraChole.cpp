
/************************************************************************/
// Mura 불량 검출 관련 소스
// 수정일 : 18.05.31 - KSW
/************************************************************************/

#include "StdAfx.h"
#include "InspectMuraChole.h"
#include "ExportLibrary.h"    //TIANMA 사용

#define round(fp) (int)((fp) >= 0 ? (fp) + 0.5 : (fp) - 0.5)


// Pattern String
// E_IMAGE_CLASSIFY_SVI_COUNT 수정될 경우, 수정해야 함
CString gg_strPatChole[E_IMAGE_CLASSIFY_AVI_COUNT] = {
	_T("R"),			// E_IMAGE_CLASSIFY_SVI_R			,	// 00 R
	_T("G"),			// E_IMAGE_CLASSIFY_SVI_G			,	// 01 G
	_T("B"),			// E_IMAGE_CLASSIFY_SVI_B			,	// 02 B
	_T("BLACK"),		// E_IMAGE_CLASSIFY_SVI_BLACK		,	// 03 BLACK
	_T("WHITE"),		// E_IMAGE_CLASSIFY_SVI_WHITE		,	// 04 WHITE
	_T("GRAY32"),		// E_IMAGE_CLASSIFY_SVI_GRAY_32		,	// 06 GRAY_32
	_T("GRAY64"),		// E_IMAGE_CLASSIFY_SVI_GRAY_64		,	// 07 GRAY_64
	_T("GRAY87"),		// E_IMAGE_CLASSIFY_SVI_GRAY_87		,	// 08 GRAY_87
	_T("GRAY128")		// E_IMAGE_CLASSIFY_SVI_GRAY_128	,	// 09 GRAY_128
};

// Camera String
CString gg_strCamChole[2] = {
	_T("Coaxial"),
	_T("Side")
};


CInspectMuraChole::CInspectMuraChole(void)
{
	cMem = NULL;
	m_cInspectLibLog = NULL;
	m_strAlgLog = NULL;
	m_tInitTime = 0;
	m_tBeforeTime = 0;
}


CInspectMuraChole::~CInspectMuraChole(void)
{
}

// Main 검사 알고리즘
long CInspectMuraChole::DoFindMuraDefect(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, cv::Rect* rcCHoleROI, cv::Mat* matCholeBuffer, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Mat& matDrawBuffer)
{
	// 파라미터 NULL인 경우
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	long	nWidth = (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcBuffer.rows;	// 영상 세로 크기	

												// 범위 설정
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	// 검사 영역이 영상 크기를 벗어난 경우
	if (rectROI.left	<	0 ||
		rectROI.top		<	0 ||
		rectROI.right >= nWidth ||
		rectROI.bottom >= nHeight)	return E_ERROR_CODE_ROI_OVER;

	if (rectROI.left >= rectROI.right)	return E_ERROR_CODE_ROI_OVER;
	if (rectROI.top >= rectROI.bottom)	return E_ERROR_CODE_ROI_OVER;

	

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	// 버퍼 할당 & 초기화
	cv::Mat matDstImage[E_DEFECT_COLOR_COUNT];

	// 알고리즘 영상 번호
	int nAlgImgNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];

	
	// 버퍼 할당
	matDstImage[E_DEFECT_COLOR_DARK] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());
	matDstImage[E_DEFECT_COLOR_BRIGHT] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());

	// 결과 버퍼가 NULL이 아닌 경우 에러
	if (matDstImage[E_DEFECT_COLOR_DARK].empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstImage[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;

	// 영상 마다 알고리즘 다름
	switch (nAlgImgNum)
	{
	case E_IMAGE_CLASSIFY_AVI_R:
	case E_IMAGE_CLASSIFY_AVI_G:
	case E_IMAGE_CLASSIFY_AVI_B:
	case E_IMAGE_CLASSIFY_AVI_BLACK:
	case E_IMAGE_CLASSIFY_AVI_DUST:
	case E_IMAGE_CLASSIFY_AVI_GRAY_32:
	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
		if (nAlgImgNum == E_IMAGE_CLASSIFY_AVI_GRAY_64)
		{
			nErrorCode = LogicStart_CholeMura(matSrcBuffer, matSrcBufferRGB, matDstImage, matBKBuffer, matCholeBuffer,rectROI, rcCHoleROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment, pResultBlob);

			break;
		}
	case E_IMAGE_CLASSIFY_AVI_GRAY_87:
	case E_IMAGE_CLASSIFY_AVI_GRAY_128:
	case E_IMAGE_CLASSIFY_AVI_WHITE:
	default:
		return E_ERROR_CODE_TRUE;
	}

	// BK 이미지 제거
	cv::subtract(matDstImage[E_DEFECT_COLOR_BRIGHT], matBKBuffer, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	cv::subtract(matDstImage[E_DEFECT_COLOR_DARK], matBKBuffer, matDstImage[E_DEFECT_COLOR_DARK]);


	// 천마 알고리즘 베이스로 만들어서 이걸 왜 하는지는 모르겠음
// 	matBrightBuffer = matDstImage[E_DEFECT_COLOR_BRIGHT].clone();		//메모리 할당
// 	matDarkBuffer = matDstImage[E_DEFECT_COLOR_DARK].clone();			//메모리 할당

																		
	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("Copy Clone Result."));


	// 할당 해제
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	cv::Rect rectBlobROI;
	rectBlobROI.x = rectROI.left;
	rectBlobROI.y = rectROI.top;
	rectBlobROI.width = rectROI.Width();
	rectBlobROI.height = rectROI.Height();
	// 라벨
	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem(&cMatBufTemp);
	cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_MURA_CHOLE, m_tInitTime, m_tBeforeTime, m_strAlgLog);


	nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstImage[E_DEFECT_COLOR_BRIGHT](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
		nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("DM_"), EngineerDefectJudgment, pResultBlob);


	nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstImage[E_DEFECT_COLOR_DARK](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
		nCommonPara, E_DEFECT_COLOR_DARK, _T("DM_"), EngineerDefectJudgment, pResultBlob);


	// 결과 외곽선 그리기
	cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);


	matDstImage[E_DEFECT_COLOR_BRIGHT].release();
	matDstImage[E_DEFECT_COLOR_DARK].release();


	return nErrorCode;
}


// Dust 제거 후, 결과 벡터 넘겨주기
long CInspectMuraChole::GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
	double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
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
	if (matDustBuffer[E_DEFECT_COLOR_DARK].empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDustBuffer[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;

	// 사용 파라미터
	bool	bFlagW = (dPara[E_PARA_AVI_MURA_CHOLE_DUST_BRIGHT_FLAG] > 0) ? true : false;
	bool	bFlagD = (dPara[E_PARA_AVI_MURA_CHOLE_DUST_DARK_FLAG] > 0) ? true : false;
	int		nSize = (int)dPara[E_PARA_AVI_MURA_CHOLE_DUST_BIG_AREA];
	int		nRange = (int)dPara[E_PARA_AVI_MURA_CHOLE_DUST_ADJUST_RANGE];

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	//////////////////////////////////////////////////////////////////////////

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

	//////////////////////////////////////////////////////////////////////////

	// Dust 패턴 불량 없음
	if (nImageNum == E_IMAGE_CLASSIFY_AVI_DUST)
		return E_ERROR_CODE_TRUE;

	// 범위 설정 ( 이미 검사 시, 예외처리 검사 함 )
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);
	cv::Mat matDustTemp = cMatBufTemp.GetMat(matDustBuffer[E_DEFECT_COLOR_BRIGHT].size(), matDustBuffer[E_DEFECT_COLOR_BRIGHT].type(), false);

	// 임시 버퍼 설정
	cv::Mat matResBuf = cMatBufTemp.GetMat(matDstBuffer[E_DEFECT_COLOR_BRIGHT].size(), matDstBuffer[E_DEFECT_COLOR_BRIGHT].type(), false);

	int nModePS = 1;
	// Dust 영상이 있는 경우 먼지 제거 로직 실행
	if (!matDustBuffer[E_DEFECT_COLOR_BRIGHT].empty() &&
		!matDustBuffer[E_DEFECT_COLOR_DARK].empty())
	{
		// 영상 비율 같은지 확인		
		if (matSrcBuffer.rows == matDustBuffer[E_DEFECT_COLOR_DARK].rows	&&
			matSrcBuffer.cols == matDustBuffer[E_DEFECT_COLOR_DARK].cols)
			nModePS = 1;
		else
			nModePS = 2;

		// 밝은 먼지 영상만
		//matDustBuffer[E_DEFECT_COLOR_BRIGHT].copyTo(matDustTemp);

		// 밝은 & 어두운 먼지 영상
		cv::add(matDustBuffer[E_DEFECT_COLOR_BRIGHT], matDustBuffer[E_DEFECT_COLOR_DARK], matDustTemp);

		// 큰 먼지만 남도록...
		nErrorCode = DeleteArea1(matDustTemp, nSize, &cMatBufTemp);
		if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DeleteArea."));
	}
	// Dust 영상 버퍼 없는 경우
	// 제거 로직 없이 불량 추출
	else
	{
		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Empty Dust Image."));
	}

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Dark_ResThreshold.jpg"), strAlgPath, gg_strPatChole[nImageNum], gg_strCamChole[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstBuffer[E_DEFECT_COLOR_DARK]);

		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bright_ResThreshold.jpg"), strAlgPath, gg_strPatChole[nImageNum], gg_strCamChole[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstBuffer[E_DEFECT_COLOR_BRIGHT]);

		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bright_Dust.jpg"), strAlgPath, gg_strPatChole[nImageNum], gg_strCamChole[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDustTemp);
	}

	// 불량 판정 & 결과 보내기
	{
		cv::Rect rectBlobROI;
		rectBlobROI.x = rectROI.left;
		rectBlobROI.y = rectROI.top;
		rectBlobROI.width = rectROI.Width();
		rectBlobROI.height = rectROI.Height();
		// 라벨
		CFeatureExtraction cFeatureExtraction;
		cFeatureExtraction.SetMem(&cMatBufTemp);
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_MURA, m_tInitTime, m_tBeforeTime, m_strAlgLog);

		// E_DEFECT_COLOR_DARK 결과
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_BRIGHT](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("DM_"), EngineerDefectJudgment, pResultBlob);

		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			// 메모리 해제
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}
		
		// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
		// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
		if (!USE_ALG_CONTOURS)	// 결과 외곽선 저장
			cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

		// 결과 외곽선 그리기
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);
	}
	{
		cv::Rect rectBlobROI;
		rectBlobROI.x = rectROI.left;
		rectBlobROI.y = rectROI.top;
		rectBlobROI.width = rectROI.Width();
		rectBlobROI.height = rectROI.Height();
		// 라벨
		CFeatureExtraction cFeatureExtraction;
		cFeatureExtraction.SetMem(&cMatBufTemp);
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_MURA, m_tInitTime, m_tBeforeTime, m_strAlgLog);

		// E_DEFECT_COLOR_DARK 결과
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_DARK](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_DARK, _T("DM_"), EngineerDefectJudgment, pResultBlob);

		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			// 메모리 해제
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}

		// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
		// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
		if (!USE_ALG_CONTOURS)	// 결과 외곽선 저장
			cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

		// 결과 외곽선 그리기
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);
	}
	// 메모리 해제
	matSrcBuffer.release();
	matDstBuffer[E_DEFECT_COLOR_DARK].release();
	matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

	return nErrorCode;
}

// 8bit & 12bit 영상 저장
long CInspectMuraChole::ImageSave(CString strPath, cv::Mat matSrcBuf)
{
	if (matSrcBuf.type() == CV_8U)
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

// Sub - Threshold ( 16bit )
long CInspectMuraChole::SubThreshold16(cv::Mat& matSrc1Buf, cv::Mat& matSrc2Buf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV)
{
	// 버퍼가 없는 경우.
	if (matSrc1Buf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matSrc2Buf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	if (matSrc1Buf.type() != CV_16UC1)	return E_ERROR_CODE_MURA_WRONG_PARA;
	if (matSrc2Buf.type() != CV_16UC1)	return E_ERROR_CODE_MURA_WRONG_PARA;
	if (matDstBuf.type() != CV_8UC1)	return E_ERROR_CODE_MURA_WRONG_PARA;

	uchar LUT[4096] = { 0, };

	for (int i = 0; i < 4096; i++)
	{
		LUT[i] = (i > nThreshold) ? nMaxGV : 0;
	}

	MatIterator_<ushort> itSrc1, itSrc2, endSrc1;
	itSrc1 = matSrc1Buf.begin<ushort>();
	itSrc2 = matSrc2Buf.begin<ushort>();
	endSrc1 = matSrc1Buf.end<ushort>();
	MatIterator_<uchar> itDst = matDstBuf.begin<uchar>();

	int nSub = 0;
	for (; itSrc1 != endSrc1; itSrc1++, itSrc2++, itDst++)
	{
		nSub = *itSrc1 - *itSrc2;

		// 음수 값인 경우...
		if (nSub < 0)	nSub = 0;

		*itDst = LUT[nSub];
	}

	return E_ERROR_CODE_TRUE;
}


// Threshold ( 16bit )
long CInspectMuraChole::Threshold16(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV)
{
	// 버퍼가 없는 경우.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	if (matSrcBuf.type() != CV_16UC1)	return E_ERROR_CODE_MURA_WRONG_PARA;
	if (matDstBuf.type() != CV_8UC1)	return E_ERROR_CODE_MURA_WRONG_PARA;

	uchar LUT[4096] = { 0, };

	for (int i = 0; i < 4096; i++)
	{
		LUT[i] = (i > nThreshold) ? nMaxGV : 0;
	}

	MatIterator_<ushort> itSrc1, endSrc1;
	itSrc1 = matSrcBuf.begin<ushort>();
	endSrc1 = matSrcBuf.end<ushort>();
	MatIterator_<uchar> itDst = matDstBuf.begin<uchar>();

	for (; itSrc1 != endSrc1; itSrc1++, itDst++)
	{
		*itDst = LUT[*itSrc1];
	}

	return E_ERROR_CODE_TRUE;
}


// Threshold ( 16bit )
long CInspectMuraChole::Threshold16_INV(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV)
{
	// 버퍼가 없는 경우.
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	if (matSrcBuf.type() != CV_16UC1)	return E_ERROR_CODE_MURA_WRONG_PARA;
	if (matDstBuf.type() != CV_8UC1)	return E_ERROR_CODE_MURA_WRONG_PARA;

	uchar LUT[4096] = { 0, };

	for (int i = 0; i < 4096; i++)
	{
		LUT[i] = (i < nThreshold) ? nMaxGV : 0;
	}

	MatIterator_<ushort> itSrc1, endSrc1;
	itSrc1 = matSrcBuf.begin<ushort>();
	endSrc1 = matSrcBuf.end<ushort>();
	MatIterator_<uchar> itDst = matDstBuf.begin<uchar>();

	for (; itSrc1 != endSrc1; itSrc1++, itDst++)
	{
		*itDst = LUT[*itSrc1];
	}

	return E_ERROR_CODE_TRUE;
}


// Pow ( 8bit & 12bit )
long CInspectMuraChole::Pow(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double dPow, int nMaxGV, CMatBuf* cMemSub)
{
	if (dPow < 1.0)			return E_ERROR_CODE_MURA_WRONG_PARA;
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼가 없는 경우.
	if (matDstBuf.empty())
		matDstBuf = cv::Mat::zeros(matSrcBuf.size(), matSrcBuf.type());

	// 원본 8U인 경우
	if (matSrcBuf.type() == CV_8U)
	{
		// 결과 8U인 경우
		if (matDstBuf.type() == CV_8U)
		{
			uchar LUT[256] = { 0, };

			for (int i = 0; i < 256; i++)
			{
				double dVal = pow(i, dPow);
				if (dVal < 0)		dVal = 0;
				if (dVal > 255)	dVal = 255;

				LUT[i] = (uchar)dVal;
			}

			MatIterator_<uchar> itSrc, endSrc, itDst;
			itSrc = matSrcBuf.begin<uchar>();
			endSrc = matSrcBuf.end<uchar>();
			itDst = matDstBuf.begin<uchar>();

			for (; itSrc != endSrc; itSrc++, itDst++)
				*itDst = LUT[(*itSrc)];
		}
		// 결과 16U인 경우
		else
		{
			ushort LUT[4096] = { 0, };

			if (nMaxGV > 4095)	nMaxGV = 4095;

			for (int i = 0; i < 4096; i++)
			{
				double dVal = pow(i, dPow);
				if (dVal < 0)		dVal = 0;
				if (dVal > nMaxGV)	dVal = nMaxGV;

				LUT[i] = (ushort)dVal;
			}

			MatIterator_<uchar> itSrc, endSrc;
			itSrc = matSrcBuf.begin<uchar>();
			endSrc = matSrcBuf.end<uchar>();
			MatIterator_<ushort> itDst = matDstBuf.begin<ushort>();

			for (; itSrc != endSrc; itSrc++, itDst++)
				*itDst = LUT[(*itSrc)];
		}
	}
	// 원본 16U인 경우
	else
	{
		ushort LUT[4096] = { 0, };

		for (int i = 0; i < 4096; i++)
		{
			double dVal = pow(i, dPow);
			if (dVal < 0)		dVal = 0;
			if (dVal > 4095)	dVal = 4095;

			LUT[i] = (ushort)dVal;
		}

		MatIterator_<ushort> itSrc, endSrc, itDst;
		itSrc = matSrcBuf.begin<ushort>();
		endSrc = matSrcBuf.end<ushort>();
		itDst = matDstBuf.begin<ushort>();

		for (; itSrc != endSrc; itSrc++, itDst++)
			*itDst = LUT[(*itSrc)];
	}

	return E_ERROR_CODE_TRUE;
}


// 작은 면적 제거
long CInspectMuraChole::DeleteArea1(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우.
	if (matSrcImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

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
		if (nArea <= nCount)
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
	}

	// KSW 18.04.11 - 방식 바꿈
	/************************************************************************
	// 버퍼가 없는 경우.
	if( matSrcImage.empty() ) return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat DstBuffer;

	CMatBuf cMatBufTemp;

	// Temp 메모리 할당
	cMatBufTemp.SetMem(cMemSub);
	DstBuffer = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());

	// 주변 8방향
	bool	bConnect[8];
	int		nConnectX[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
	int		nConnectY[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
	int		nConnectCnt;

	for(int y=1 ; y<matSrcImage.rows-1 ; y++)
	{
	for(int x=1 ; x<matSrcImage.cols-1 ; x++)
	{
	if( matSrcImage.at<uchar>(y, x) == 0)		continue;

	memset(bConnect, 0, sizeof(bool) * 8);
	nConnectCnt = 1;

	// 주변 갯수 파악
	for (int z=0 ; z<8 ; z++)
	{
	if( matSrcImage.at<uchar>(y + nConnectY[z], x + nConnectX[z]) != 0)
	{
	bConnect[z] = true;
	nConnectCnt++;
	}
	}

	// 주변 갯수 설정 이하 제외
	if( nConnectCnt < nCount )	continue;

	// 주변 그리기
	for (int z=0 ; z<8 ; z++)
	{
	if( !bConnect[z] )	continue;

	DstBuffer.at<uchar>(y + nConnectY[z], x + nConnectX[z]) = (BYTE)255;
	}

	// 중심 그리기
	DstBuffer.at<uchar>(y, x) = (BYTE)255;
	}
	}

	matSrcImage = DstBuffer.clone();

	DstBuffer.release();
	************************************************************************/

	return E_ERROR_CODE_TRUE;
}

// 작은 면적 제거
long CInspectMuraChole::DeleteArea2(cv::Mat& matSrcImage, int nCount, int nLength, CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우.
	if (matSrcImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

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

	// 길이 제한
	int nX = (int)(matSrcImage.cols * 0.7);
	int nY = (int)(matSrcImage.rows * 0.7);

	// 개수만큼 메모리 할당
	cv::RotatedRect *rectRo = NULL;
	rectRo = new cv::RotatedRect[nTotalLabel + 1];
	memset(rectRo, 0, sizeof(cv::RotatedRect) * (nTotalLabel + 1));

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

		if (nArea <= nCount ||	// 면적 제한
			rectTemp.width >= nX ||	// 가로 길이 제한 ( 라인 불량... )
			rectTemp.height >= nY)	// 세로 길이 제한 ( 라인 불량... )
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

		// 면적 만족하는 경우
		{
			// ROI 자르기
			cv::Mat matLabelROI = matLabel(rectTemp);

			// 초기화
			vector<cv::Point>	ptIndexs;
			vector<cv::Point>().swap(ptIndexs);

			for (int y = 0; y<rectTemp.height; y++)
			{
				int *ptrLabel = (int *)matLabelROI.ptr(y);

				for (int x = 0; x<rectTemp.width; x++, ptrLabel++)
				{
					// 해당 객체
					if (*ptrLabel == idx)
					{
						// 원본 좌표 넣기
						ptIndexs.push_back(cv::Point(x + rectTemp.x, y + rectTemp.y));
					}
				}
			}

			// 최소 박스 찾기
			rectRo[idx] = cv::minAreaRect(ptIndexs);

			// 확장
			rectRo[idx].size.width += (nLength + nLength);
			rectRo[idx].size.height += (nLength + nLength);

			// 확인용
			//if( 0 )
			//{
			//	// 꼭지점 4개 찾기
			//	cv::Point2f vertices[E_CORNER_END], ptC;
			//	rectRo[idx].points(vertices);

			//	// 확인용
			//	cv::line(matSrcImage, vertices[0], vertices[1], cv::Scalar(100, 100, 100));
			//	cv::line(matSrcImage, vertices[1], vertices[2], cv::Scalar(100, 100, 100));
			//	cv::line(matSrcImage, vertices[2], vertices[3], cv::Scalar(100, 100, 100));
			//	cv::line(matSrcImage, vertices[3], vertices[0], cv::Scalar(100, 100, 100));
			//}
		}
	}

	// Blob 개수
	for (int k = 1; k <= nTotalLabel; k++)
	{
		// 값이 없는 경우
		if (rectRo[k].size.width <= 0)	continue;

		for (int m = 1; m <= nTotalLabel; m++)
		{
			// 같은 불량 제외
			if (k == m)	continue;

			// 값이 없는 경우
			if (rectRo[m].size.width <= 0)	continue;

			// 범위가 곂친 경우 선 그어서 합치기
			if (OrientedBoundingBox(rectRo[k], rectRo[m]))
			{
				// Center Point 구하기
				cv::Point ptLine[2];
				ptLine[0].x = (int)matCentroid.at<double>(k, 0);
				ptLine[0].y = (int)matCentroid.at<double>(k, 1);
				ptLine[1].x = (int)matCentroid.at<double>(m, 0);
				ptLine[1].y = (int)matCentroid.at<double>(m, 1);

				// 중심점 라인 그리기
				cv::line(matSrcImage, ptLine[0], ptLine[1], cv::Scalar(255, 255, 255));
			}
		}
	}

	// 메모리 해제
	delete rectRo;
	rectRo = NULL;

	return E_ERROR_CODE_TRUE;
}

// 두개 영상 평균
long CInspectMuraChole::TwoImg_Average(cv::Mat matSrc1Buf, cv::Mat matSrc2Buf, cv::Mat &matDstBuf)
{
	if (matSrc1Buf.empty())			return E_ERROR_CODE_EMPTY_BUFFER;
	if (matSrc2Buf.empty())			return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuf.empty())				return E_ERROR_CODE_EMPTY_BUFFER;

	if (matSrc1Buf.channels() != 1)	return E_ERROR_CODE_IMAGE_GRAY;
	if (matSrc2Buf.channels() != 1)	return E_ERROR_CODE_IMAGE_GRAY;

	if (matSrc1Buf.rows != matSrc2Buf.rows ||		// 가로 크기 같아야 함
		matSrc1Buf.cols != matSrc2Buf.cols)		// 세로 크기 같아야 함
		return E_ERROR_CODE_IMAGE_SIZE;

	// 원본 8U인 경우
	if (matSrc1Buf.type() == CV_8U)
	{
		cv::MatIterator_<uchar> itSrc1, itSrc2, itDst, endDst;
		itSrc1 = matSrc1Buf.begin<uchar>();
		itSrc2 = matSrc2Buf.begin<uchar>();
		itDst = matDstBuf.begin<uchar>();
		endDst = matDstBuf.end<uchar>();

		for (; itDst != endDst; itSrc1++, itSrc2++, itDst++)
			*itDst = (uchar)((*itSrc1 + *itSrc2) / 2);
	}
	// 원본 16U인 경우
	else
	{
		cv::MatIterator_<ushort> itSrc1, itSrc2, itDst, endDst;
		itSrc1 = matSrc1Buf.begin<ushort>();
		itSrc2 = matSrc2Buf.begin<ushort>();
		itDst = matDstBuf.begin<ushort>();
		endDst = matDstBuf.end<ushort>();

		for (; itDst != endDst; itSrc1++, itSrc2++, itDst++)
			*itDst = (ushort)((*itSrc1 + *itSrc2) / 2);
	}

	/************************************************************************
	for (int y=0 ; y<matSrc1Buf.rows ; y++)
	{
	BYTE *ptr1 = (BYTE *)matSrc1Buf.ptr(y);
	BYTE *ptr2 = (BYTE *)matSrc2Buf.ptr(y);
	BYTE *ptr3 = (BYTE *)matDstBuf.ptr(y);

	for (int x=0 ; x<matSrc1Buf.cols ; x++, ptr1++, ptr2++, ptr3++)
	{
	*ptr3 = (BYTE)abs( (*ptr1 + *ptr2) / 2.0 );
	}
	}
	************************************************************************/

	return E_ERROR_CODE_TRUE;
}

// Dust 면적 큰 주변인 경우 제거
long CInspectMuraChole::DeleteCompareDust(cv::Mat& matSrcBuffer, int nOffset, stDefectInfo* pResultBlob, int nStartIndex, int nModePS)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 파라미터 NULL인 경우
	if (matSrcBuffer.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (pResultBlob == NULL)		return E_ERROR_CODE_EMPTY_PARA;

	// 검출된 불량 개수
	int nDefectCount = pResultBlob->nDefectCount;

	// 불량 개수 만큼...
	for (int i = nStartIndex; i<nDefectCount; i++)
	{
		// Mura 불량이 아닌경우 제외
		if (pResultBlob->nDefectJudge[i] <= E_DEFECT_JUDGEMENT_MURA_START	&&
			pResultBlob->nDefectJudge[i] > E_DEFECT_JUDGEMENT_MURA_END)
		{
			//i++;
			continue;
		}

		// KSW 18.06.13 - 누기, 무정형 무라, 등등... 제외...
		if (pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_MURA_LINE_X ||
			pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_MURA_NUGI ||
			pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH)
		{
			//i++;
			continue;
		}

		// KSW 18.06.13 - 면적 큰거는 제외...?
		if (pResultBlob->nArea[i] > 1000)
		{
			//i++;
			continue;
		}

		// 주변 영역 설정
		cv::Rect rect(
			pResultBlob->ptLT[i].x,
			pResultBlob->ptLT[i].y,
			pResultBlob->ptRB[i].x - pResultBlob->ptLT[i].x,
			pResultBlob->ptRB[i].y - pResultBlob->ptLT[i].y);

		// 영상 크기에 맞게 조절하여 비교 ( nModePS )
		// Dust는 무조건 P/S 모드
		// 영상 크기가 틀리다면, 검사한 패턴 영상이 작은 경우임
		rect.x *= nModePS;
		rect.y *= nModePS;
		rect.width *= nModePS;
		rect.height *= nModePS;

		// nOffset 확장
		rect.x -= nOffset;
		rect.y -= nOffset;
		rect.width += (nOffset + nOffset);
		rect.height += (nOffset + nOffset);

		// 예외 처리
		if (rect.x < 0)	rect.x = 0;
		if (rect.y < 0)	rect.y = 0;
		if (rect.x + rect.width >= matSrcBuffer.cols)	rect.width = matSrcBuffer.cols - rect.x - 1;
		if (rect.y + rect.height >= matSrcBuffer.rows)	rect.height = matSrcBuffer.rows - rect.y - 1;

		// 해당 ROI 따기
		cv::Mat matTempBuf = matSrcBuffer(rect);

		//////////////////////////////////////////////////////////////////////////
		// 불량 주변에서 Dust 존재하는지 확인
		//////////////////////////////////////////////////////////////////////////

		// 불량 주변 -> Dust 영상 Max GV 찾기
		double valMax;
		cv::minMaxLoc(matTempBuf, NULL, &valMax);

		// Dust 확인 시, 큰 불량이 없는 경우 ( 큰 불량 존재 시, 255 / 4096 값이 존재 )
		if (valMax == 0)
		{
			//i++;
			continue;
		}

		// 해당 불량 보고 안하기
		pResultBlob->bUseResult[i] = false;

		/************************************************************************
		// 해당 불량 제거
		// 마지막 index 불량 --> 현재 index에 넣기
		{
		pResultBlob->nDefectJudge		[i] = pResultBlob->nDefectJudge		[nDefectCount - 1];
		pResultBlob->nDefectColor		[i] = pResultBlob->nDefectColor		[nDefectCount - 1];
		pResultBlob->nPatternClassify	[i] = pResultBlob->nPatternClassify	[nDefectCount - 1];
		pResultBlob->nArea				[i] = pResultBlob->nArea			[nDefectCount - 1];
		pResultBlob->ptLT				[i] = pResultBlob->ptLT				[nDefectCount - 1];
		pResultBlob->ptRT				[i] = pResultBlob->ptRT				[nDefectCount - 1];
		pResultBlob->ptRB				[i] = pResultBlob->ptRB				[nDefectCount - 1];
		pResultBlob->ptLB				[i] = pResultBlob->ptLB				[nDefectCount - 1];
		pResultBlob->dMeanGV			[i] = pResultBlob->dMeanGV			[nDefectCount - 1];
		pResultBlob->dSigma				[i] = pResultBlob->dSigma			[nDefectCount - 1];
		pResultBlob->nMinGV				[i] = pResultBlob->nMinGV			[nDefectCount - 1];
		pResultBlob->nMaxGV				[i] = pResultBlob->nMaxGV			[nDefectCount - 1];
		pResultBlob->dBackGroundGV		[i] = pResultBlob->dBackGroundGV	[nDefectCount - 1];
		pResultBlob->nCenterx			[i] = pResultBlob->nCenterx			[nDefectCount - 1];
		pResultBlob->nCentery			[i] = pResultBlob->nCentery			[nDefectCount - 1];
		pResultBlob->dBreadth			[i] = pResultBlob->dBreadth			[nDefectCount - 1];
		pResultBlob->dCompactness		[i] = pResultBlob->dCompactness		[nDefectCount - 1];
		pResultBlob->dF_Elongation		[i] = pResultBlob->dF_Elongation	[nDefectCount - 1];
		pResultBlob->dF_Min				[i] = pResultBlob->dF_Min			[nDefectCount - 1];
		pResultBlob->dF_Max				[i] = pResultBlob->dF_Max			[nDefectCount - 1];
		pResultBlob->Lab_avg_L			[i] = pResultBlob->Lab_avg_L		[nDefectCount - 1];
		pResultBlob->Lab_avg_a			[i] = pResultBlob->Lab_avg_a		[nDefectCount - 1];
		pResultBlob->Lab_avg_b			[i] = pResultBlob->Lab_avg_b		[nDefectCount - 1];
		pResultBlob->Lab_diff_L			[i] = pResultBlob->Lab_diff_L		[nDefectCount - 1];
		pResultBlob->Lab_diff_a			[i] = pResultBlob->Lab_diff_a		[nDefectCount - 1];
		pResultBlob->Lab_diff_b			[i] = pResultBlob->Lab_diff_b		[nDefectCount - 1];

		#if USE_ALG_HIST
		memcpy(pResultBlob->nHist[i], pResultBlob->nHist[nDefectCount - 1], sizeof(__int64) * IMAGE_MAX_GV);
		#endif

		pResultBlob->bUseResult			[i] = pResultBlob->bUseResult		[nDefectCount - 1];

		// 총 불량 수 하나 제거
		nDefectCount--;
		}
		************************************************************************/
	}

	// 최종 불량 개수 재설정
	//pResultBlob->nDefectCount = nDefectCount;

	return nErrorCode;
}


// 어두운 라인 불량 제거 ( 누기만 남기기 위함 )
long CInspectMuraChole::DeleteDarkLine(cv::Mat& matSrcBuffer, float fMajorAxisRatio, CMatBuf* cMemSub)
{
	// 예외 처리
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 메모리 할당
	cv::Mat matLabel, matStats, matCentroid;
	matLabel = cMatBufTemp.GetMat(matSrcBuffer.size(), CV_32SC1, false);

	// Blob 개수
	__int64 nTotalLabel = cv::connectedComponentsWithStats(matSrcBuffer, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

	cv::Rect rectBox;
	int nHW = matSrcBuffer.cols / 2;
	int nHH = matSrcBuffer.rows / 2;

	// 불량 개수만큼
	for (int idx = 1; idx <= nTotalLabel; idx++)
	{
		// 라벨 사각 영역
		rectBox.x = matStats.at<int>(idx, CC_STAT_LEFT);
		rectBox.y = matStats.at<int>(idx, CC_STAT_TOP);
		rectBox.width = matStats.at<int>(idx, CC_STAT_WIDTH);
		rectBox.height = matStats.at<int>(idx, CC_STAT_HEIGHT);

		// 불량 가로길이가, 점등된 영역의 반 가로 길이보다 큰경우 제거
		if (rectBox.width > nHW)
		{
			__int64 nSum = 0;
			int nEndX = rectBox.x + rectBox.width;

			// 라인 세로 두께 평균 구하기
			for (int x = rectBox.x; x < nEndX; x++)
				nSum += (unsigned int)(cv::sum(matSrcBuffer.col(x))[0]);

			// 두께보다 조금 크게 설정
			nSum *= fMajorAxisRatio;
			nSum /= rectBox.width;

			bool bLeft = false;

			for (int x = rectBox.x; x < nEndX; x++)
			{
				// 라인 세로 두께 평균보다 작은 경우 제거
				// DGS 누기같은 경우, 누기 영역을 남기기 위함
				if (cv::sum(matSrcBuffer.col(x))[0] < nSum)
					matSrcBuffer(cv::Rect(x, rectBox.y, 1, rectBox.height)).setTo(0);
				// 남기기
				else
				{
					if (bLeft)
					{
						if (rectBox.x < x)
						{
							bLeft = true;
							rectBox.x = x;
						}
					}
					else
					{
						rectBox.width = x - rectBox.x;
					}
				}
			}
		}

		// 불량 세로길이가, 점등된 영역의 반 세로 길이보다 큰경우 제거
		if (rectBox.height > nHH)
		{
			__int64 nSum = 0;
			int nEndY = rectBox.y + rectBox.height;

			// 라인 가로 두께 평균 구하기
			for (int y = rectBox.y; y < nEndY; y++)
				nSum += (unsigned int)(cv::sum(matSrcBuffer.row(y))[0]);

			// 두께보다 조금 크게 설정
			nSum *= fMajorAxisRatio;
			nSum /= rectBox.height;

			for (int y = rectBox.y; y < nEndY; y++)
			{
				// 라인 가로 두께 평균보다 작은 경우 제거
				// DGS 누기같은 경우, 누기 영역을 남기기 위함
				if (cv::sum(matSrcBuffer.row(y))[0] < nSum)
					matSrcBuffer(cv::Rect(rectBox.x, y, rectBox.width, 1)).setTo(0);
			}
		}
	}

	return nErrorCode;
}

// 가로 방향 Max GV 제한 : 명점 등등 밝은 불량 주변으로 검출되는 경우 발생 방지
long CInspectMuraChole::LimitMaxGV16X(cv::Mat& matSrcBuffer, float fOffset)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 예외 처리
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	// 가로 방향만...
	for (int y = 0; y < matSrcBuffer.rows; y++)
	{
		// 가로 방향의 평균보다 조금 밝게
		ushort nAvgGV = (ushort)(cv::sum(matSrcBuffer.row(y))[0] / matSrcBuffer.cols * fOffset);

		ushort *ptr = (ushort *)matSrcBuffer.ptr(y);

		// 밝기 제한하여, 평균 밝기로 변경
		for (int x = 0; x < matSrcBuffer.cols; x++, ptr++)
		{
			if (*ptr > nAvgGV)	*ptr = nAvgGV;
		}
	}

	return nErrorCode;
}

bool CInspectMuraChole::OrientedBoundingBox(cv::RotatedRect& rect1, cv::RotatedRect& rect2)
{
	// Distance Vector
	cv::Point2d dist, unit, vec[4];
	dist.x = rect1.center.x - rect2.center.x;
	dist.y = rect1.center.y - rect2.center.y;

	// rect1 Height Vector
	vec[0].x = rect1.size.height * cos((rect1.angle - 90.0) / 180.0 * PI) / 2.0;
	vec[0].y = rect1.size.height * sin((rect1.angle - 90.0) / 180.0 * PI) / 2.0;

	// rect2 Height Vector
	vec[1].x = rect2.size.height * cos((rect2.angle - 90.0) / 180.0 * PI) / 2.0;
	vec[1].y = rect2.size.height * sin((rect2.angle - 90.0) / 180.0 * PI) / 2.0;

	// rect1 Width Vector
	vec[2].x = rect1.size.width * cos(rect1.angle / 180.0 * PI) / 2.0;
	vec[2].y = rect1.size.width * sin(rect1.angle / 180.0 * PI) / 2.0;

	// rect2 Width Vector
	vec[3].x = rect2.size.width * cos(rect2.angle / 180.0 * PI) / 2.0;
	vec[3].y = rect2.size.width * sin(rect2.angle / 180.0 * PI) / 2.0;

	// 4개 벡터 중...
	bool bRes = true;
	for (int i = 0; i<4; i++)
	{
		double dSum = 0;

		double dSize = sqrt(vec[i].x * vec[i].x + vec[i].y * vec[i].y);
		unit.x = (double)(vec[i].x / dSize);
		unit.y = (double)(vec[i].y / dSize);

		// 4개 내적 중...
		for (int j = 0; j<4; j++)
			dSum += abs(unit.x * vec[j].x + unit.y * vec[j].y);

		// 하나라도 분리할수 있는 경우가 존재하는 경우
		if (abs(unit.x * dist.x + unit.y * dist.y) > dSum)
		{
			bRes = false;
			break;
		}
	}

	return bRes;
}


//////////////////////////TIANMA CHOLE MURA AREA /////////////////////////////
void CInspectMuraChole::VarianceFilter(cv::Mat src, cv::Mat& dst, int nMaskSize)
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

			if (dVariance<0.0f)   ucDstdata[nY * nCols + nX] = (BYTE)0;
			else if (dVariance>255.0f) ucDstdata[nY * nCols + nX] = (BYTE)255;
			else						ucDstdata[nY * nCols + nX] = (BYTE)dVariance;

		}//전체 영상 가로 크기 종료
	}//전체 영상 세로 크기 종료
}
void CInspectMuraChole::FunFilter(cv::Mat &Intput, cv::Mat &Output, int width, int height)
{
	int i, j, k, m;
	long nmax, nmin, nsum;
	double dvalue;
	int widthnew, heightnew;
	int coef[16] = { 2,6,10,15,20,13,14,18,8,2,-2,-3,-5,-5,-3,-2 };
	m = 16;

	widthnew = width + 2 * m;
	heightnew = height + 2 * m;

	//Image Ptr로 변환, 추후 전체 변경 예정
	uchar *data_ouput = Output.data;

	//Remove area
	// Mat Buffer = Mat::zeros(widthnew, heightnew, CV_8UC1);
	//uchar *data_Buffer = Buffer.data;



	long* lResizeBuff = new long[widthnew*heightnew];
	long* ldata_buff = new long[widthnew*heightnew];

	//Padding을 위한 Mask 크기만큼 좌,우,위,아래 16Pixel만큼 이미지 크기 증가
	FunImageResize(Intput, lResizeBuff, widthnew, heightnew, width, height, m);

	for (j = 1; j<widthnew; j++) lResizeBuff[j] += lResizeBuff[j - 1];

	for (i = 1; i<heightnew; i++) lResizeBuff[i*widthnew] += lResizeBuff[(i - 1)*widthnew];

	for (i = 1; i<heightnew; i++)
		for (j = 1; j<widthnew; j++)
			lResizeBuff[i*widthnew + j] += lResizeBuff[(i - 1)*widthnew + j] + lResizeBuff[i*widthnew + (j - 1)] - lResizeBuff[(i - 1)*widthnew + (j - 1)];

	for (i = m; i<heightnew - m; i++)
		for (j = m; j<widthnew - m; j++)
		{
			nsum = 0;

			for (k = 0; k<m; k++)
				nsum += (long)(coef[k] * (lResizeBuff[(i + k)*widthnew + (j + k)] - lResizeBuff[(i - (k + 1))*widthnew + (j + k)] - lResizeBuff[(i + k)*widthnew + (j - (k + 1))] + lResizeBuff[(i - (k + 1))*widthnew + (j - (k + 1))]));

			ldata_buff[i*widthnew + j] = nsum;
			//remove area
			// data_Buffer[i*widthnew + j] = nsum;
		}
	//ImageSave(_T("d:\\Filter_middle.bmp"), Buffer);
	nmax = 250000;
	nmin = -250000;

	for (i = 0; i<height; i++)
	{

		for (j = 0; j<width; j++)
		{
			dvalue = ((double)((ldata_buff[(i + m)*widthnew + (j + m)] - nmin)*255.0f) / (double)(nmax - nmin));

			if (dvalue<0.0f)        data_ouput[i*width + j] = (BYTE)0;
			else if (dvalue>255.0f) data_ouput[i*width + j] = (BYTE)255;
			else	                data_ouput[i*width + j] = (BYTE)dvalue;

		}
	}

	if (lResizeBuff)
		delete[] lResizeBuff;

	if (ldata_buff)
		delete[] ldata_buff;

}

void CInspectMuraChole::FunWhiteMura(cv::Mat &Intput, cv::Mat &Output, int width, int height, int nThres)
{
	int i, j;

	//Image Ptr로 변환, 추후 전체 변경 예정
	Mat matTemp = Mat::zeros(Intput.size(), CV_8UC1);
	matTemp = Intput.clone();

	uchar *data_FilImg = matTemp.data;
	uchar *data_Output = Output.data;


	for (i = 0; i<height; i++)
		for (j = 0; j<width; j++)
		{
			data_Output[i*width + j] = data_FilImg[i*width + j] > nThres ? (BYTE)255 : (BYTE)0;
		}

	//Temp Image 메모리 해제
	if (!matTemp.empty())				matTemp.release();
}

void CInspectMuraChole::FunBlackMura(cv::Mat &Intput, cv::Mat &Output, int width, int height, int nThres)
{
	int i, j;

	//Image Ptr로 변환, 추후 전체 변경 예정
	Mat matTemp = Mat::zeros(Intput.size(), CV_8UC1);
	matTemp = Intput.clone();

	uchar *data_FilImg = matTemp.data;
	uchar *data_Output = Output.data;


	for (i = 0; i<height; i++)
		for (j = 0; j<width; j++)
		{
			data_Output[i*width + j] = data_FilImg[i*width + j] < nThres ? (BYTE)255 : (BYTE)0;
		}

	//Temp Image 메모리 해제
	if (!matTemp.empty())				matTemp.release();
}




void CInspectMuraChole::FunLineMuraBlack(cv::Mat &Intput, cv::Mat &OutputHorizontal, cv::Mat &OutputVertical, int width, int height, int nThresV, int nThresH, int nThresVEdge, int nThresHEdge)
{
	int i, j;
	long nSum;
	int N;
	int nmin, nmax;
	//	int ny;

	N = 90;

	CRect m_nROI = CRect(3, 3, width - 3, height - 3);

	//Image Ptr로 변환, 추후 전체 변경 예정
	Mat matTempHorizontal = Mat::zeros(Intput.size(), CV_8UC1);
	Mat matTempVertical = Mat::zeros(Intput.size(), CV_8UC1);

	Mat matTempHorizontalResize = Mat::zeros(Intput.rows - 5, Intput.cols - 6, CV_8UC1);
	Mat matTempVerticalResize = Mat::zeros(Intput.rows - 6, Intput.cols - 5, CV_8UC1);

	Mat matOutputHorizontalTemp = Mat::zeros(Intput.size(), CV_8UC1);
	Mat matOutputVerticalTemp = Mat::zeros(Intput.size(), CV_8UC1);

	uchar *data_FilImg = Intput.data;
	uchar *data_OutputHorizontal = OutputHorizontal.data;
	uchar *data_OutputVertical = OutputVertical.data;
	uchar *data_matTempHorizontal = matTempHorizontal.data;
	uchar *data_matTempVertical = matTempVertical.data;
	uchar *data_matTempHorizontalResize = matTempHorizontalResize.data;
	uchar *data_matTempVerticalResize = matTempVerticalResize.data;
	uchar *data_OutputHorizontalTemp = matOutputHorizontalTemp.data;
	uchar *data_OutputVerticalTemp = matOutputVerticalTemp.data;


	long* lSumImg = new long[width*height];

	for (i = 0; i<height; i++)
		for (j = 0; j<width; j++)
		{
			lSumImg[i*width + j] = data_FilImg[i*width + j];
		}

	for (j = 1; j<width; j++) lSumImg[j] += lSumImg[j - 1];

	for (i = 1; i<height; i++) lSumImg[i*width] += lSumImg[(i - 1)*width];

	for (i = 1; i<height; i++)
		for (j = 1; j<width; j++)
			lSumImg[i*width + j] += lSumImg[(i - 1)*width + j] + lSumImg[i*width + (j - 1)] - lSumImg[(i - 1)*width + (j - 1)];

	//	nmin=22000; //N=60;
	//	nmax=24000;
	//nmin = 32000; //N=90;
	//nmax = 36000;
	//	 nmin = 28000; //N=90;
	//	 nmax = 32000;
	nmin = 22000; //N=90;
	nmax = 48000;
	//	nmin=10000000;
	//	nmax=0;

	//	ny=(m_nROI.top+m_nROI.bottom)/2;

	//  [1] vertical binary image

	for (j = m_nROI.left; j <= m_nROI.right; j++)
		//for(i=m_nROI.top;i<=ny+N;i++)
		for (i = m_nROI.top + 1; i <= m_nROI.bottom - N - 1; i++)
		{
			nSum = lSumImg[(i + N - 1)*width + (j + 1)] - lSumImg[(i - 1)*width + (j + 1)] - lSumImg[(i + N - 1)*width + (j - 2)] + lSumImg[(i - 1)*width + (j - 2)];

			//			if(nSum<nmin) nmin=nSum;
			//			if(nSum>nmax) nmax=nSum;

			//TmpImg[i*width+j]=(BYTE)((double)nSum/(double)(3*N));
			data_matTempVertical[i*width + j] = (BYTE)((double)(nSum - nmin) / (double)(nmax - nmin)*255.0f);
		}

	for (j = m_nROI.left; j <= m_nROI.right; j++)
		//for(i=m_nROI.bottom;i>=ny-N;i--)
		for (i = m_nROI.bottom; i >= m_nROI.top + N + 1; i--)
		{
			nSum = lSumImg[i*width + (j + 1)] - lSumImg[(i - N)*width + (j + 1)] - lSumImg[i*width + (j - 2)] + lSumImg[(i - N)*width + (j - 2)];

			//TmpImg[i*width+j]=nMAX(TmpImg[i*width+j],(BYTE)((double)nSum/(double)(3*N)));
			data_matTempVertical[i*width + j] = nMAX(data_matTempVertical[i*width + j], (BYTE)((double)(nSum - nmin) / (double)(nmax - nmin)*255.0f));
			//TmpImg[i*width+j]=(BYTE)((double)nSum/(double)(3*N));
		}
	ImageSave(_T("d:\\VW.bmp"), matTempVertical);

	// 외각영역 크기 보정(CRect m_nROI = CRect(3, 3, width - 3, height - 3); 해당 구문 "3" 숫자 변경 시 해당 보정 수치 변경됨 터치 불가 
	for (i = 0; i <= height - 7; i++)
		for (j = 0; j <= width - 6; j++)
			data_matTempVerticalResize[i*(width - 5) + j] = data_matTempVertical[(i + 4)*width + (j + 3)];

	ImageSave(_T("d:\\VWE.bmp"), matTempVerticalResize);
	cv::resize(matTempVerticalResize, matOutputVerticalTemp, matOutputVerticalTemp.size(), INTER_LINEAR);


	for (i = 0; i <= height - 1; i++)
		for (j = 0; j <= width - 1; j++)
		{
			if (j < 10 || j > width - 10)
			{
				data_OutputVertical[i*width + j] = data_OutputVerticalTemp[i*width + j] > nThresVEdge ? (BYTE)0 : (BYTE)255; //m_nThresV 입력 받은 값

			}
			else
			{
				data_OutputVertical[i*width + j] = data_OutputVerticalTemp[i*width + j] > nThresV ? (BYTE)0 : (BYTE)255; //m_nThresV 입력 받은 값
			}
		}


	ImageSave(_T("d:\\VWEND.bmp"), OutputVertical);

	//	[2] hori_Binary_Imagez
	for (i = m_nROI.top; i <= m_nROI.bottom; i++)
		for (j = m_nROI.left + 1; j <= m_nROI.right - N - 1; j++)
		{
			nSum = lSumImg[(i + 1)*width + (j + N - 1)] - lSumImg[(i + 1)*width + (j - 1)] - lSumImg[(i - 2)*width + (j + N - 1)] + lSumImg[(i - 2)*width + (j - 1)];

			//TmpImg[i*width+j]=(BYTE)((double)nSum/(double)(3*N));
			data_matTempHorizontal[i*width + j] = (BYTE)((double)(nSum - nmin) / (double)(nmax - nmin)*255.0f);
		}

	for (i = m_nROI.top; i <= m_nROI.bottom; i++)
		for (j = m_nROI.right; j >= m_nROI.left + N + 1; j--)
		{
			nSum = lSumImg[(i + 1)*width + j] - lSumImg[(i + 1)*width + (j - N)] - lSumImg[(i - 2)*width + j] + lSumImg[(i - 2)*width + (j - N)];

			//TmpImg[i*width+j]=(BYTE)((double)nSum/(double)(3*N));
			//TmpImg[i*width+j]=nMAX(TmpImg[i*width+j],(BYTE)((double)nSum/(double)(3*N)));
			data_matTempHorizontal[i*width + j] = nMAX(data_matTempHorizontal[i*width + j], (BYTE)((double)(nSum - nmin) / (double)(nmax - nmin)*255.0f));
		}
	ImageSave(_T("d:\\XW.bmp"), matTempHorizontal);


	// 외각영역 크기 보정(CRect m_nROI = CRect(3, 3, width - 3, height - 3); 해당 구문 "3" 숫자 변경 시 해당 보정 수치 변경됨 터치 불가
	for (i = 0; i <= height - 6; i++)
		for (j = 0; j <= width - 7; j++)
			data_matTempHorizontalResize[i*(width - 6) + j] = data_matTempHorizontal[(i + 3)*width + (j + 4)];

	ImageSave(_T("d:\\XWE.bmp"), matTempHorizontalResize);
	cv::resize(matTempHorizontalResize, matOutputHorizontalTemp, matOutputHorizontalTemp.size(), INTER_LINEAR);

	ImageSave(_T("d:\\XWENDResize.bmp"), matOutputHorizontalTemp);


	for (i = 0; i <= height - 1; i++)
		for (j = 0; j <= width - 1; j++)
		{
			if (i < 10 || i > height - 10)
			{
				data_OutputHorizontal[i*width + j] = data_OutputHorizontalTemp[i*width + j] > nThresHEdge ? (BYTE)0 : (BYTE)255; //  m_nThresH : 입력 변수
			}
			else
			{
				data_OutputHorizontal[i*width + j] = data_OutputHorizontalTemp[i*width + j] > nThresH ? (BYTE)0 : (BYTE)255; //  m_nThresH : 입력 변수
			}

		}


	ImageSave(_T("d:\\XWEND.bmp"), OutputHorizontal);


	//Temp Image 메모리 해제
	if (!matTempHorizontal.empty())					matTempHorizontal.release();
	if (!matTempVertical.empty())						matTempVertical.release();
	if (!matTempHorizontalResize.empty())				matTempHorizontalResize.release();
	if (!matTempVerticalResize.empty())				matTempVerticalResize.release();
	if (!matOutputHorizontalTemp.empty())				matOutputHorizontalTemp.release();
	if (!matOutputVerticalTemp.empty())				matOutputVerticalTemp.release();

	if (lSumImg)
		delete[] lSumImg;
}
void CInspectMuraChole::FunLineMuraWhite(cv::Mat &Intput, cv::Mat &OutputHorizontal, cv::Mat &OutputVertical, int width, int height, int nThresV, int nThresH, int nThresVEdge, int nThresHEdge)
{
	int i, j;
	long nSum;
	constexpr int N = 90;
	int nmin, nmax;
	//	int ny;

	CRect m_nROI = CRect(3, 3, width - 3, height - 3);

	//Image Ptr로 변환, 추후 전체 변경 예정
	Mat matTempHorizontal = Mat::zeros(Intput.size(), CV_8UC1);
	Mat matTempVertical = Mat::zeros(Intput.size(), CV_8UC1);

	Mat matTempHorizontalResize = Mat::zeros(Intput.rows - 5, Intput.cols - 6, CV_8UC1);
	Mat matTempVerticalResize = Mat::zeros(Intput.rows - 6, Intput.cols - 5, CV_8UC1);

	Mat matOutputHorizontalTemp = Mat::zeros(Intput.size(), CV_8UC1);
	Mat matOutputVerticalTemp = Mat::zeros(Intput.size(), CV_8UC1);

	uchar *data_FilImg = Intput.data;
	uchar *data_OutputHorizontal = OutputHorizontal.data;
	uchar *data_OutputVertical = OutputVertical.data;
	uchar *data_matTempHorizontal = matTempHorizontal.data;
	uchar *data_matTempVertical = matTempVertical.data;
	uchar *data_matTempHorizontalResize = matTempHorizontalResize.data;
	uchar *data_matTempVerticalResize = matTempVerticalResize.data;
	uchar *data_OutputHorizontalTemp = matOutputHorizontalTemp.data;
	uchar *data_OutputVerticalTemp = matOutputVerticalTemp.data;


	long* lSumImg = new long[width*height];

	for (i = 0; i<height; i++)
		for (j = 0; j<width; j++)
		{
			lSumImg[i*width + j] = data_FilImg[i*width + j];
		}

	for (j = 1; j<width; j++) lSumImg[j] += lSumImg[j - 1];

	for (i = 1; i<height; i++) lSumImg[i*width] += lSumImg[(i - 1)*width];

	for (i = 1; i<height; i++)
		for (j = 1; j<width; j++)
			lSumImg[i*width + j] += lSumImg[(i - 1)*width + j] + lSumImg[i*width + (j - 1)] - lSumImg[(i - 1)*width + (j - 1)];

	//	nmin=22000; //N=60;
	//	nmax=24000;
	//nmin = 32000; //N=90;
	//nmax = 36000;
	//	 nmin = 28000; //N=90;
	//	 nmax = 32000;
	nmin = 28000; //N=90;
	nmax = 32000;
	//	nmin=10000000;
	//	nmax=0;

	//	ny=(m_nROI.top+m_nROI.bottom)/2;

	//  [1] vertical binary image

	for (j = m_nROI.left; j <= m_nROI.right; j++)
		//for(i=m_nROI.top;i<=ny+N;i++)
		for (i = m_nROI.top + 1; i <= m_nROI.bottom - N - 1; i++)
		{
			nSum = lSumImg[(i + N - 1)*width + (j + 1)] - lSumImg[(i - 1)*width + (j + 1)] - lSumImg[(i + N - 1)*width + (j - 2)] + lSumImg[(i - 1)*width + (j - 2)];

			//			if(nSum<nmin) nmin=nSum;
			//			if(nSum>nmax) nmax=nSum;

			//TmpImg[i*width+j]=(BYTE)((double)nSum/(double)(3*N));
			data_matTempVertical[i*width + j] = (BYTE)((double)(nSum - nmin) / (double)(nmax - nmin)*255.0f);
		}

	for (j = m_nROI.left; j <= m_nROI.right; j++)
		//for(i=m_nROI.bottom;i>=ny-N;i--)
		for (i = m_nROI.bottom; i >= m_nROI.top + N + 1; i--)
		{
			nSum = lSumImg[i*width + (j + 1)] - lSumImg[(i - N)*width + (j + 1)] - lSumImg[i*width + (j - 2)] + lSumImg[(i - N)*width + (j - 2)];

			//TmpImg[i*width+j]=nMAX(TmpImg[i*width+j],(BYTE)((double)nSum/(double)(3*N)));
			data_matTempVertical[i*width + j] = nMAX(data_matTempVertical[i*width + j], (BYTE)((double)(nSum - nmin) / (double)(nmax - nmin)*255.0f));
			//TmpImg[i*width+j]=(BYTE)((double)nSum/(double)(3*N));
		}
	ImageSave(_T("d:\\VW.bmp"), matTempVertical);

	// 외각영역 크기 보정(CRect m_nROI = CRect(3, 3, width - 3, height - 3); 해당 구문 "3" 숫자 변경 시 해당 보정 수치 변경됨 터치 불가 
	for (i = 0; i <= height - 7; i++)
		for (j = 0; j <= width - 6; j++)
			data_matTempVerticalResize[i*(width - 5) + j] = data_matTempVertical[(i + 4)*width + (j + 3)];

	ImageSave(_T("d:\\VWE.bmp"), matTempVerticalResize);
	cv::resize(matTempVerticalResize, matOutputVerticalTemp, matOutputVerticalTemp.size(), INTER_LINEAR);


	for (i = 0; i <= height - 1; i++)
		for (j = 0; j <= width - 1; j++)
		{
			if (j < 10 || j > width - 10)
			{
				data_OutputVertical[i*width + j] = data_OutputVerticalTemp[i*width + j] > nThresVEdge ? (BYTE)0 : (BYTE)255; //m_nThresV 입력 받은 값

			}
			else
			{
				data_OutputVertical[i*width + j] = data_OutputVerticalTemp[i*width + j] > nThresV ? (BYTE)0 : (BYTE)255; //m_nThresV 입력 받은 값
			}
		}

	ImageSave(_T("d:\\VWEND.bmp"), OutputVertical);

	//	[2] hori_Binary_Imagez
	for (i = m_nROI.top; i <= m_nROI.bottom; i++)
		for (j = m_nROI.left + 1; j <= m_nROI.right - N - 1; j++)
		{
			nSum = lSumImg[(i + 1)*width + (j + N - 1)] - lSumImg[(i + 1)*width + (j - 1)] - lSumImg[(i - 2)*width + (j + N - 1)] + lSumImg[(i - 2)*width + (j - 1)];

			//TmpImg[i*width+j]=(BYTE)((double)nSum/(double)(3*N));
			data_matTempHorizontal[i*width + j] = (BYTE)((double)(nSum - nmin) / (double)(nmax - nmin)*255.0f);
		}

	for (i = m_nROI.top; i <= m_nROI.bottom; i++)
		for (j = m_nROI.right; j >= m_nROI.left + N + 1; j--)
		{
			nSum = lSumImg[(i + 1)*width + j] - lSumImg[(i + 1)*width + (j - N)] - lSumImg[(i - 2)*width + j] + lSumImg[(i - 2)*width + (j - N)];

			//TmpImg[i*width+j]=(BYTE)((double)nSum/(double)(3*N));
			//TmpImg[i*width+j]=nMAX(TmpImg[i*width+j],(BYTE)((double)nSum/(double)(3*N)));
			data_matTempHorizontal[i*width + j] = nMAX(data_matTempHorizontal[i*width + j], (BYTE)((double)(nSum - nmin) / (double)(nmax - nmin)*255.0f));
		}
	ImageSave(_T("d:\\XW.bmp"), matTempHorizontal);


	// 외각영역 크기 보정(CRect m_nROI = CRect(3, 3, width - 3, height - 3); 해당 구문 "3" 숫자 변경 시 해당 보정 수치 변경됨 터치 불가
	for (i = 0; i <= height - 6; i++)
		for (j = 0; j <= width - 7; j++)
			data_matTempHorizontalResize[i*(width - 6) + j] = data_matTempHorizontal[(i + 3)*width + (j + 4)];

	ImageSave(_T("d:\\XWE.bmp"), matTempHorizontalResize);
	cv::resize(matTempHorizontalResize, matOutputHorizontalTemp, matOutputHorizontalTemp.size(), INTER_LINEAR);

	ImageSave(_T("d:\\XWENDResize.bmp"), matOutputHorizontalTemp);


	for (i = 0; i <= height - 1; i++)
		for (j = 0; j <= width - 1; j++)
		{
			if (i < 10 || i > height - 10)
			{
				data_OutputHorizontal[i*width + j] = data_OutputHorizontalTemp[i*width + j] > nThresHEdge ? (BYTE)0 : (BYTE)255; //  m_nThresH : 입력 변수
			}
			else
			{
				data_OutputHorizontal[i*width + j] = data_OutputHorizontalTemp[i*width + j] > nThresH ? (BYTE)0 : (BYTE)255; //  m_nThresH : 입력 변수
			}

		}

	ImageSave(_T("d:\\XWEND.bmp"), OutputHorizontal);


	//Temp Image 메모리 해제
	if (!matTempHorizontal.empty())					matTempHorizontal.release();
	if (!matTempVertical.empty())						matTempVertical.release();
	if (!matTempHorizontalResize.empty())				matTempHorizontalResize.release();
	if (!matTempVerticalResize.empty())				matTempVerticalResize.release();
	if (!matOutputHorizontalTemp.empty())				matOutputHorizontalTemp.release();
	if (!matOutputVerticalTemp.empty())				matOutputVerticalTemp.release();

	if (lSumImg)
		delete[] lSumImg;
}
void CInspectMuraChole::FunImageResize(cv::Mat& Intput, long* lResizeBuff, int widthnew, int heightnew, int width, int height, int m)
{
	int i, j;

	uchar *data_input = Intput.data;

	// [1] copy
	for (i = m; i<heightnew - m; i++)
		for (j = m; j<widthnew - m; j++)
			lResizeBuff[i*widthnew + j] = data_input[(i - m)*width + (j - m)];

	// [2] top,bottom
	for (j = m; j<widthnew - m; j++)
		for (i = 0; i<m; i++)
		{
			lResizeBuff[i*widthnew + j] = data_input[(m - 1 - i)*width + (j - m)];
			lResizeBuff[(m + height + i)*widthnew + j] = data_input[(height - 1 - i)*width + (j - m)];
		}

	// [3] left, right
	for (i = 0; i<heightnew; i++)
		for (j = 0; j<m; j++)
		{
			lResizeBuff[i*widthnew + j] = lResizeBuff[i*widthnew + (2 * m - 1 - j)];
			lResizeBuff[i*widthnew + (m + width + j)] = lResizeBuff[i*widthnew + (m + width - 1 - j)];
		}
}

long CInspectMuraChole::LogicStart_CholeMura(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, cv::Mat *matCholeBuffer, CRect rectROI, Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{
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
	//////////////////////////////////////////////////////////////////////////
	// 검사 파라미터
	int nCholeROI_X = (int)dPara[E_PARA_AVI_MURA_CHOLE_ROI_X];//1000;
	int nCholeROI_Y = (int)dPara[E_PARA_AVI_MURA_CHOLE_ROI_Y];//1000;

	int nResizeZoom = (int)dPara[E_PARA_AVI_MURA_CHOLE_RESIZEZOOM];//2;

	int nGauSize = (int)dPara[E_PARA_AVI_MURA_CHOLE_GAUSS_SIZE]; //15;
	double dGauSig = (double)dPara[E_PARA_AVI_MURA_CHOLE_GAUSS_SIGMA]; // 2.0;

	double dContrast_Max = (double)dPara[E_PARA_AVI_MURA_CHOLE_CONTRAST_MAX]; //1.02;
	double dContrast_Min = (double)dPara[E_PARA_AVI_MURA_CHOLE_CONTRAST_MIN]; //0.98;
	int nBrightness_Max = (int)dPara[E_PARA_AVI_MURA_CHOLE_BRIGHTNESS_MAX]; //0;
	int nBrightness_Min = (int)dPara[E_PARA_AVI_MURA_CHOLE_BRIGHTNESS_MIN]; //0;

	int nGauSize2 = (int)dPara[E_PARA_AVI_MURA_CHOLE_GAUSS_SIZE2]; //31;
	double dGauSig2 = (double)dPara[E_PARA_AVI_MURA_CHOLE_GAUSS_SIGMA2]; // 5.0;

	double dThRatio_Bright = (double)dPara[E_PARA_AVI_MURA_CHOLE_THRESHOLD_RATIO_WHITE]; // 1.02;
	double dThRatio_Dark = (double)dPara[E_PARA_AVI_MURA_CHOLE_THRESHOLD_RATIO_BLACK]; // 0.95;

	int nMorSize = (int)dPara[E_PARA_AVI_MURA_CHOLE_MORPHOLOGY];
	//
	//////////////////////////////////////////////////////////////////////////

	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole Logic_Spot Start."));

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	/////////////////////////// PreProcessing Start ////////////////////////////////

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	cv::Mat matSrcROI = cMatBufTemp.GetMat(nCholeROI_X, nCholeROI_Y, matSrcImage.type());
	cv::Mat matCholeMask = cMatBufTemp.GetMat(nCholeROI_X, nCholeROI_Y, matSrcImage.type());
	cv::Mat matBGMask = cMatBufTemp.GetMat(nCholeROI_X, nCholeROI_Y, matSrcImage.type());
	// 검사 영역 설정 & Chole 마스크 만들기
	// 해당 불량은 특정 위치에 특정 모양으로 발생하기 때문에 위치 지정 검사가 가능함
	// Chole Rect 기준으로 영역 지정
	
	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole Logic_Spot Buf Set End."));

	Rect rcInspROI;

	InspectionROI(matSrcImage, matSrcROI, matCholeMask, matCholeBuffer,rcInspROI, rectROI, rcCHoleROI, nCholeROI_X, nCholeROI_Y, matBKBuffer, matBGMask);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole InspRoi Set End."));

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_SrcROI.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcROI);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Chole Mask.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matCholeMask);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_BK_Mask.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBGMask);
	}


	// Image Resize(검사 속도 증가를 위한 Resize)
	// 현재는 그냥 절반으로 이미지 크기를 줄임 차후 필요하면 파라메타를 받아와 이미지크기를 수정하도록 설정

	cv::resize(matSrcROI, matSrcROI, cv::Size(nCholeROI_X / nResizeZoom, nCholeROI_Y / nResizeZoom), 0, 0, CV_INTER_NN);
	
	cv::resize(matCholeMask, matCholeMask, cv::Size(nCholeROI_X / nResizeZoom, nCholeROI_Y / nResizeZoom), 0, 0, CV_INTER_NN);

	cv::resize(matBGMask, matBGMask, cv::Size(nCholeROI_X / nResizeZoom, nCholeROI_Y / nResizeZoom), 0, 0, CV_INTER_NN);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole Resize1 End."));

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_SrcROI_Resize.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcROI);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Chole Mask_Resize.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matCholeMask);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_BK_Mask_Resize.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBGMask);
	}



	// 잡음제거
	cv::GaussianBlur(matSrcROI, matSrcROI, cv::Size(nGauSize, nGauSize), dGauSig, dGauSig);


	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole GaussianBlur1 End."));
	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_GaussianBlur.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcROI);
	}


	// 밝기 대비 증가
	Contrast(matSrcROI, dContrast_Max, dContrast_Min, nBrightness_Max, nBrightness_Min);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole Contrast End."));

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Contrast Image.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcROI);
	}
	
	////////////////////////// Main Processing Start ///////////////////////////
		
	// Image ProFile & Threshold

	cv::Mat matDstBright = cMatBufTemp.GetMat(matSrcROI.size(), CV_8UC1);
	cv::Mat matDstDark = cMatBufTemp.GetMat(matSrcROI.size(), CV_8UC1);


	cv::GaussianBlur(matSrcROI, matSrcROI, cv::Size(nGauSize2, nGauSize2), dGauSig2);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole GaussianBlur2 End."));

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_GaussianBlur2.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcROI);
	}

	ProfilingThreshold(matSrcROI, matDstBright, matCholeMask, matBGMask, dThRatio_Bright,THRESH_BINARY);
	ProfilingThreshold(matSrcROI, matDstDark, matCholeMask, matBGMask, dThRatio_Dark, THRESH_BINARY_INV);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole ProfileTH End."));
	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Defect_Bright_zoom.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstBright);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Defect_Dark_zoom.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstDark);
	}

	cv::morphologyEx(matDstBright, matDstBright, MORPH_CLOSE, Mat(nMorSize, nMorSize, CV_8UC1, Scalar(1)));
	cv::morphologyEx(matDstDark, matDstDark, MORPH_CLOSE, Mat(nMorSize, nMorSize, CV_8UC1, Scalar(1)));

	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole MorphologyEx End."));

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Morphology_Bright_zoom.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstBright);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Morphology_Dark_zoom.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstDark);
	}

	// Image ReSize

	cv::resize(matDstBright, matDstBright, cv::Size(nCholeROI_X , nCholeROI_Y ), 0, 0, CV_INTER_NN);
	cv::resize(matDstDark, matDstDark, cv::Size(nCholeROI_X, nCholeROI_Y), 0, 0, CV_INTER_NN);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole Resize2 End."));

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Defect_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstBright);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Defect_Dark.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstDark);
	}

	matDstBright.copyTo(matDstImage[E_DEFECT_COLOR_BRIGHT](rcInspROI));
	matDstDark.copyTo(matDstImage[E_DEFECT_COLOR_DARK](rcInspROI));

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Bright_Final.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_Chole_%02d_Dark_Final.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}


	matSrcROI.release();
	matCholeMask.release();
	matDstBright.release();
	matDstDark.release();

	writeInspectLog(E_ALG_TYPE_AVI_MURA_CHOLE, __FUNCTION__, _T("MuraChole Logic_Spot End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// Pow ( 8bit & 12bit )
long CInspectMuraChole::Pow(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double dPow)
{
//	if (dPow < 1.0)			return E_ERROR_CODE_MURA_WRONG_PARA;
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼가 없는 경우.
	if (matDstBuf.empty())
		matDstBuf = cv::Mat::zeros(matSrcBuf.size(), matSrcBuf.type());

	// 원본 8U인 경우
	if (matSrcBuf.type() == CV_8U)
	{
		// 결과 8U인 경우
		if (matDstBuf.type() == CV_8U)
		{
			uchar LUT[256] = { 0, };

			for (int i = 0; i < 256; i++)
			{
				double dVal = pow(i, dPow);
				if (dVal < 0)		dVal = 0;
				if (dVal > 255)	dVal = 255;

				LUT[i] = (uchar)dVal;
			}

			MatIterator_<uchar> itSrc, endSrc, itDst;
			itSrc = matSrcBuf.begin<uchar>();
			endSrc = matSrcBuf.end<uchar>();
			itDst = matDstBuf.begin<uchar>();

			for (; itSrc != endSrc; itSrc++, itDst++)
				*itDst = LUT[(*itSrc)];
		}
		// 결과 16U인 경우
		else
		{
			ushort LUT[4096] = { 0, };

			for (int i = 0; i < 4096; i++)
			{
				double dVal = pow(i, dPow);
				if (dVal < 0)		dVal = 0;
				if (dVal > 4095)	dVal = 4095;

				LUT[i] = (ushort)dVal;
			}

			MatIterator_<uchar> itSrc, endSrc;
			itSrc = matSrcBuf.begin<uchar>();
			endSrc = matSrcBuf.end<uchar>();
			MatIterator_<ushort> itDst = matDstBuf.begin<ushort>();

			for (; itSrc != endSrc; itSrc++, itDst++)
				*itDst = LUT[(*itSrc)];
		}
	}
	// 원본 16U인 경우
	else
	{
		ushort LUT[4096] = { 0, };

		for (int i = 0; i < 4096; i++)
		{
			double dVal = pow(i, dPow);
			if (dVal < 0)		dVal = 0;
			if (dVal > 4095)	dVal = 4095;

			LUT[i] = (ushort)dVal;
		}

		MatIterator_<ushort> itSrc, endSrc, itDst;
		itSrc = matSrcBuf.begin<ushort>();
		endSrc = matSrcBuf.end<ushort>();
		itDst = matDstBuf.begin<ushort>();

		for (; itSrc != endSrc; itSrc++, itDst++)
			*itDst = LUT[(*itSrc)];
	}

	return E_ERROR_CODE_TRUE;
}

void CInspectMuraChole::MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize)
{
	Mat prc;
	src.copyTo(prc);

	if (nMaskSize % 2 == 0)
		nMaskSize++;

	int nCols = src.cols;
	int nRows = src.rows;
	int nStep = src.step;

	int nMaskX_ST, nMaskY_ST, nMask_ST;

	nMask_ST = nMaskSize / 2 * -1;
	int nMax;

	uchar* ucSrcdata;
	uchar* ucDstdata;

	for (int nY = 0; nY < nRows; nY++)
	{
		if (nY + nMask_ST < 0 || nY + abs(nMask_ST) > nRows - 1)
			continue;
		ucDstdata = prc.ptr(nY);
		for (int nX = 0; nX < nCols; nX++)
		{
			if (nX + nMask_ST < 0 || nX + nMask_ST > nCols - 1)
				continue;

			nMax = 0;

			nMaskY_ST = nMask_ST;
			for (int nMaskY = 0; nMaskY < nMaskSize; nMaskY++)
			{
				int nY_ = nY + nMaskY_ST;
				ucSrcdata = src.ptr(nY_);

				nMaskX_ST = nMask_ST;
				for (int nMaskX = 0; nMaskX < nMaskSize; nMaskX++)
				{
					int nX_ = nX + nMaskX_ST;
					if ((int)ucSrcdata[nX_] > nMax)
						nMax = (int)ucSrcdata[nX_];
					nMaskX_ST++;
				}
				nMaskY_ST++;
			}
			ucDstdata[nX] = (uchar)nMax;
		}
	}

	prc.copyTo(dst);
}












// 검사영역 지정 및 Chole 마스크 만드는 함수
void CInspectMuraChole::InspectionROI(cv::Mat& matSrcImage, cv::Mat& matSrcROI, cv::Mat& matCholeMask,cv::Mat *matCholeBuffer, Rect& rcInspROI, CRect rectROI, Rect* rcCHoleROI, int nCholeROI_X, int nCholeROI_Y, cv::Mat& matBKBuffer, cv::Mat& matBGMask)
{
	// Chole이 무조건 1개뿐이라고 생각하고 만든 알고리즘 차후 문제가되면 수정 필요

	// Chole 중심 찾기
	cv::Point ptCHoleCenter;
	int nStartx = 0, nStarty = 0;
	int nCHoleraius = 0;

	cv::Mat matChole_Temp;
	Rect rectChol;

	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		int nArea = 0;
		if (nArea < rcCHoleROI[i].area())
		{
			ptCHoleCenter.x = rcCHoleROI[i].x + (rcCHoleROI[i].width / 2);
			ptCHoleCenter.y = rcCHoleROI[i].y + (rcCHoleROI[i].height / 2);

			rectChol.x = rcCHoleROI[i].x;
			rectChol.y = rcCHoleROI[i].y;
			rectChol.width = rcCHoleROI[i].width;
			rectChol.height = rcCHoleROI[i].height;
			
// 			if (rcCHoleROI[i].width < rcCHoleROI[i].height)
// 				nCHoleraius = rcCHoleROI[i].width / 3;
// 			else
// 				nCHoleraius = rcCHoleROI[i].height / 3;

			nArea = rcCHoleROI[i].area();
		}

		// 2021.02.26 Chole 크기 Chole In과 동일하게
		if (!matCholeBuffer[i].empty())
		{
			matCholeBuffer[i].copyTo(matChole_Temp);
		}
	}

	// 검사 영역 시작 좌표 계산
	nStartx = ptCHoleCenter.x - (nCholeROI_X / 2);
	if (nStartx < rectROI.left)
		nStartx = rectROI.left;

	nStarty = ptCHoleCenter.y - (nCholeROI_Y / 2);
	if (nStarty < rectROI.top)
		nStarty = rectROI.top;

	rcInspROI.x = nStartx;
	rcInspROI.y = nStarty;
	rcInspROI.width = nCholeROI_X;
	rcInspROI.height = nCholeROI_Y;

	matSrcImage(rcInspROI).copyTo(matSrcROI);

	cv::Mat matGholeBuff = cv::Mat::zeros(matSrcImage.size(), matSrcImage.type());

	






	// Chole Mask 만들기

	cv::Mat matSrcBuf = cv::Mat::zeros(matSrcImage.size(), matSrcImage.type());

	//cv::circle(matSrcBuf, ptCHoleCenter, nCHoleraius, cv::Scalar(255), -1);

	matChole_Temp.copyTo(matSrcBuf(rectChol));

	matSrcBuf(rcInspROI).copyTo(matCholeMask);

	// Back Ground Mask 만들기

	matBKBuffer(rcInspROI).copyTo(matBGMask);

	matChole_Temp.release();
	matSrcBuf.release();
}


// 밝기 대비 증가
void CInspectMuraChole::Contrast(cv::Mat& matSrcImage,double dContrast_Max, double dContrast_Min, int nBrightness_Max, int nBrightness_Min)
{

	double dAvg = cv::mean(matSrcImage, matSrcImage>0)[0];

	int nMax = (int)(dAvg * (dContrast_Max));
	int nMin = (int)(dAvg * (dContrast_Min));
	if (nMax > 255)	nMax = 240;
	if (nMin < 0)	nMin = 0;

	float LUT[256] = { 0, };
	for (int i = 0; i < 256; i++)
	{
		if (i < nMin)		LUT[i] = nMin;
		else if (i > nMax)	LUT[i] = nMax;
		else				LUT[i] = i;
	}

	//Vec3f형으로 선언하면 외터짐?...질문하자..
	cv::MatIterator_<uchar> itSrc, endSrc;
	itSrc = matSrcImage.begin<uchar>();
	endSrc = matSrcImage.end<uchar>();

	for (; itSrc != endSrc; itSrc++)
	{
		(*itSrc) = LUT[((uchar)*itSrc)];
	}

	int nSize = 5;
	cv::blur(matSrcImage, matSrcImage, cv::Size(nSize, nSize));

	///////////////////////////////////////////////////////
	// Min, Max

	nMax += nBrightness_Max;
	nMin -= nBrightness_Min;

	double dVal = 255.0 / (nMax - nMin);
	cv::subtract(matSrcImage, nMin, matSrcImage);
	cv::multiply(matSrcImage, dVal, matSrcImage);


}


// ProFiling & Threshold
void CInspectMuraChole::ProfilingThreshold(cv::Mat& matSrcImage, cv::Mat& matDstBuf, cv::Mat& matCholeMask, cv::Mat& matBGMask, double dThresholdRatio, int nType)
{
	// Chole Mask를 이용한 Chole 좌표 찾기

	cv::Mat matLabels, stats, centroids;
	int numOfLables;


	numOfLables = connectedComponentsWithStats(matCholeMask, matLabels, stats, centroids, 8, CV_32S, CCL_GRANA);

	Rect rectHoleROI(0, 0, 0, 0);
	int nArea = 0;
	for (int j = 1; j < numOfLables; j++)
	{
		int iArea = stats.at<int>(j, CC_STAT_AREA);
		
		if (nArea < iArea)
		{
			rectHoleROI.x = stats.at<int>(j, CC_STAT_LEFT);
			rectHoleROI.y = stats.at<int>(j, CC_STAT_TOP);
			rectHoleROI.width = stats.at<int>(j, CC_STAT_WIDTH);
			rectHoleROI.height = stats.at<int>(j, CC_STAT_HEIGHT);
		}
	}

	// ProFile 영역 설정

	Rect rcProX(0, 0, rectHoleROI.x, matSrcImage.rows);
	Rect rcProY(0, rectHoleROI.y + rectHoleROI.height, matSrcImage.cols, matSrcImage.rows - (rectHoleROI.y + rectHoleROI.height));

	// BG + Chole Mask
	cv::Mat matMask;
	cv::add(matBGMask, matCholeMask, matMask);
	matMask = ~matMask;

	//matMask(Rect(0, rectHoleROI.y + rectHoleROI.height, rectHoleROI.x, matMask.rows -(rectHoleROI.y + rectHoleROI.height))).setTo(0);

	// Profile
	cv::Mat matProX = cv::Mat::zeros(Size(1,matSrcImage.rows), CV_8UC1);
	cv::Mat matProY = cv::Mat::zeros(Size(matSrcImage.cols,1), CV_8UC1);

// 	uchar *ptr = (uchar *)matProX.ptr(0);
// 	for (int i = 0; i < matSrcImage.rows; i++, ptr++)
// 	{
// 		*ptr = (uchar)(cv::mean(matSrcImage(rcProX).row(i), (matBGMask)(rcProX))[0]);
// 	
// 	}
// 	
// 	ptr = (uchar *)matProY.ptr(0);
// 	for (int i = 0; i < matSrcImage.cols; i++, ptr++)
// 	{
// 		*ptr = (uchar)(cv::mean(matSrcImage(rcProY).col(i))[0]);
// 	
// 	}



	
	//////////////////////////////////////
	// X방향
	cv::Mat matTest = matMask(rcProX);
	cv::Mat matTestImage = matSrcImage(rcProX);
	uchar *ptr = (uchar*)matProX.ptr(0);

	for (int y = 0; y < matTestImage.rows; y++, ptr++)
	{
		int nSum = 0;
		int nCount = 0;

		for (int x = 0; x < matTestImage.cols; x++)
		{
			if (matTest.data[y * matTest.step + x] > 0)
			{
				nCount++;
				nSum += matTestImage.data[y * matTestImage.step + x];
			}
		}

		if (nCount > 0)
			*ptr = nSum / nCount;
		else
			*ptr = 0;
	}

	// Y방향
	matTest = matMask(rcProY);
	matTestImage = matSrcImage(rcProY);
	ptr = (uchar*)matProY.ptr(0);

	for (int x = 0; x < matTestImage.cols; x++, ptr++)
	{
		int nSum = 0;
		int nCount = 0;

		for (int y = 0; y < matTestImage.rows; y++)
		{
			if (matTest.data[y * matTest.step + x] > 0)
			{
				nCount++;
				nSum += matTestImage.data[y * matTestImage.step + x];
			}
		}

		if (nCount > 0)
			*ptr = nSum / nCount;
		else
			*ptr = 0;
	}
	/////////////////////////////////////////








	// 이진화
	cv::threshold(matProX, matProX, (cv::mean(matProX, matProX>0)[0] * dThresholdRatio), 255, nType);
	cv::threshold(matProY, matProY, (cv::mean(matProY, matProY>0)[0] * dThresholdRatio), 255, nType);


	rcProX = Rect(0, 0, rectHoleROI.x + (rectHoleROI.width / 2), matSrcImage.rows);
	rcProY = Rect(0, rectHoleROI.y + (rectHoleROI.height / 2), matSrcImage.cols, matSrcImage.rows - (rectHoleROI.y + (rectHoleROI.height / 2)));

	cv::resize(matProX, matProX, Size(rectHoleROI.x + (rectHoleROI.width / 2), matSrcImage.rows));
	cv::resize(matProY, matProY, Size(matSrcImage.cols, matSrcImage.rows - (rectHoleROI.y + (rectHoleROI.height / 2))));

	matProX(Rect(0, rectHoleROI.y + rectHoleROI.height, matProX.cols, matProX.rows - (rectHoleROI.y + rectHoleROI.height))).setTo(0);
	matProY(Rect(0, 0, rectHoleROI.x, matProY.rows)).setTo(0);

	//cv::add(matDstBuf, matCholeMask, matDstBuf);
	cv::add(matProX, matDstBuf(rcProX), matDstBuf(rcProX));
	cv::add(matProY, matDstBuf(rcProY), matDstBuf(rcProY));

	
	cv::subtract(matDstBuf, ~matMask, matDstBuf);


	// 불량 마스크 만들어 특정 불량 모양만 검출
	cv::Mat matDefectMask = matCholeMask.clone();

	matDefectMask(Rect(0, rectHoleROI.y, rectHoleROI.x + rectHoleROI.width, rectHoleROI.height)).setTo(255);
	matDefectMask(Rect(rectHoleROI.x, rectHoleROI.y, rectHoleROI.width, matDefectMask.rows - rectHoleROI.y)).setTo(255);

	cv::bitwise_and(matDstBuf, matDefectMask, matDstBuf);


	matProX.release();
	matProY.release();
}