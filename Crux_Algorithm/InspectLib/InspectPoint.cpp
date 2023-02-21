
/************************************************************************/
// Point 불량 검출 관련 소스
// 수정일 : 17.07.10 - KSW
/************************************************************************/

#include "StdAfx.h"
#include "InspectPoint.h"

enum enFlattening
{
	Flattening_None, Flattening_Horizental, Flattening_Vertical, Flattening_All_H, Flattening_All_V
};

#define _BIAS_	128
CInspectPoint::CInspectPoint(void)
{
	cMem[0]				= NULL;
	cMem[1]				= NULL;
	m_cInspectLibLog	= NULL;
	m_strAlgLog			= NULL;
	m_tInitTime			= 0;
	m_tBeforeTime		= 0;
}


CInspectPoint::~CInspectPoint(void)
{

}


long CInspectPoint::DoFindPointDefect(cv::Mat matSrcBuffer, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, CDefectCCD *cCCD , cv::Mat* matCholeBuffer)
{
	// 파라미터 NULL인 경우
	if( dPara == NULL )					return E_ERROR_CODE_EMPTY_PARA;
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcBuffer.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;

	long	nWidth	= (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuffer.rows;	// 영상 세로 크기

	// 영상 번호
	long	nImageClassify = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];

	// ID 찾기
	CString strPathTemp;

	// 오른쪽 끝에 "\\" 제거
	strPathTemp.Format(_T("%s"), strAlgPath.Left(strAlgPath.GetLength()-1));

	// 오른쪽에서 "\\" 찾기
	int nLength = strPathTemp.ReverseFind( _T('\\') );

	// ID만 찾기
	wchar_t	strID[MAX_PATH] = {0, };
	swprintf_s(strID, MAX_PATH, L"%s", (LPCWSTR)strPathTemp.Right(strPathTemp.GetLength() - nLength - 1));

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// 범위 크게 설정
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x,		ptCorner[E_CORNER_LEFT_BOTTOM].x	),
		min(ptCorner[E_CORNER_LEFT_TOP].y,		ptCorner[E_CORNER_RIGHT_TOP].y		),
		max(ptCorner[E_CORNER_RIGHT_TOP].x,		ptCorner[E_CORNER_RIGHT_BOTTOM].x	),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	) );

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ROI_OVER;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ROI_OVER;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ROI_OVER;

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;	

	cv::Mat matDstImage[E_DEFECT_COLOR_COUNT];	

	// 버퍼 할당 & 초기화
	// KSW 17.09.26 - Dust 패턴에서 Dark	- Area 큰 부유성 이물
	// KSW 17.09.26 - Dust 패턴에서 BRIGHT	- 모든 부유성 이물
	matDstImage[E_DEFECT_COLOR_DARK]	= cMem[0]->GetMat(matSrcBuffer.size(), matSrcBuffer.type());
	matDstImage[E_DEFECT_COLOR_BRIGHT]	= cMem[0]->GetMat(matSrcBuffer.size(), matSrcBuffer.type());

	// 결과 버퍼가 NULL이 아닌 경우 에러
	if( matDstImage[E_DEFECT_COLOR_DARK].empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstImage[E_DEFECT_COLOR_BRIGHT].empty() )	return E_ERROR_CODE_EMPTY_BUFFER;

	switch(nImageClassify)
	{
	//RGB Pattern 검사
	case E_IMAGE_CLASSIFY_AVI_COAXIAL:
	{
	
		nErrorCode = LCP_COAXIAL(matSrcBuffer, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment);

	}
	break;

	case E_IMAGE_CLASSIFY_AVI_DUST:
	{
		nErrorCode = LCP_DUST(matSrcBuffer, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment);

	}
	break;

	case E_IMAGE_CLASSIFY_AVI_SIDE:
	{
		nErrorCode = LCP_SIDE(matSrcBuffer, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment);

		
	}
	break;

	case E_IMAGE_CLASSIFY_AVI_BACK:
	{
		nErrorCode = LCP_BACK(matSrcBuffer, matDstImage, rectROI, rcAlignCellROI ,dPara, nCommonPara, strAlgPath, strBubblePath,strBubblePath_Menual,eInspMode ,EngineerDefectJudgment);
	}
	break;

	
		// 영상 번호가 잘못 들어온 경우.
	default:
		return E_ERROR_CODE_TRUE;
	}
	

	matDarkBuffer = matDstImage[E_DEFECT_COLOR_DARK].clone();
	matBrightBuffer = matDstImage[E_DEFECT_COLOR_BRIGHT].clone();


	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Result Copy."));

	// 할당 해제
	matDstImage[E_DEFECT_COLOR_DARK].release();
	matDstImage[E_DEFECT_COLOR_BRIGHT].release();

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("End."));

	return nErrorCode;
}

long CInspectPoint::DoFindPointDefect_Transparency(cv::Mat matSrcBuffer, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, CDefectCCD *cCCD, cv::Mat* matCholeBuffer,cv::Mat matSrc_Stage)
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

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// 범위 크게 설정
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

	cv::Mat matDstImage[E_DEFECT_COLOR_COUNT];

	// 버퍼 할당 & 초기화
	// KSW 17.09.26 - Dust 패턴에서 Dark	- Area 큰 부유성 이물
	// KSW 17.09.26 - Dust 패턴에서 BRIGHT	- 모든 부유성 이물
	matDstImage[E_DEFECT_COLOR_DARK] = cMem[0]->GetMat(matSrcBuffer.size(), matSrcBuffer.type());
	matDstImage[E_DEFECT_COLOR_BRIGHT] = cMem[0]->GetMat(matSrcBuffer.size(), matSrcBuffer.type());

	// 결과 버퍼가 NULL이 아닌 경우 에러
	if (matDstImage[E_DEFECT_COLOR_DARK].empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstImage[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;

	// 영상 마다 알고리즘 다름.

	bool bGray = false;


	switch (nImageClassify)
	{
		//RGB Pattern 검사
	case E_IMAGE_CLASSIFY_AVI_COAXIAL:
	{

		nErrorCode = LCP_COAXIAL_Transparency(matSrcBuffer, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment);
	
	}
	break;

	case E_IMAGE_CLASSIFY_AVI_DUST:
	{
	//	nErrorCode = LCP_DUST_Transparency(matSrcBuffer, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment);
	}
	break;

	case E_IMAGE_CLASSIFY_AVI_SIDE:
	{
		nErrorCode = LCP_SIDE_Transparency(matSrcBuffer, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment);
	}
	break;

	case E_IMAGE_CLASSIFY_AVI_BACK:
	{
		nErrorCode = LCP_BACK_Transparency(matSrcBuffer, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment, matSrc_Stage);
	}
	break;

	// 영상 번호가 잘못 들어온 경우.
	default:
		return E_ERROR_CODE_TRUE;
	}
	matDarkBuffer = matDstImage[E_DEFECT_COLOR_DARK].clone();
	matBrightBuffer = matDstImage[E_DEFECT_COLOR_BRIGHT].clone();


	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Result Copy."));

	// 할당 해제
	matDstImage[E_DEFECT_COLOR_DARK].release();
	matDstImage[E_DEFECT_COLOR_BRIGHT].release();

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("End."));

	return nErrorCode;
}

long CInspectPoint::DoFindPointDefect_Black(cv::Mat matSrcBuffer, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
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

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

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
	matDstImage[E_DEFECT_COLOR_DARK] = cMem[0]->GetMat(matSrcBuffer.size(), matSrcBuffer.type());
	matDstImage[E_DEFECT_COLOR_BRIGHT] = cMem[0]->GetMat(matSrcBuffer.size(), matSrcBuffer.type());

	// 결과 버퍼가 NULL이 아닌 경우 에러
	if (matDstImage[E_DEFECT_COLOR_DARK].empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstImage[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;

	switch (nImageClassify)
	{
		//RGB Pattern 검사
	case E_IMAGE_CLASSIFY_AVI_COAXIAL:
	{

		nErrorCode = LCP_COAXIAL_Black(matSrcBuffer, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment);

	}
	break;

	case E_IMAGE_CLASSIFY_AVI_DUST:
	{
		nErrorCode = LCP_DUST_Black(matSrcBuffer, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment);

	}
	break;

	case E_IMAGE_CLASSIFY_AVI_SIDE:
	{
		//nErrorCode = LCP_SIDE(matSrcBuffer, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment);

	}
	break;

	case E_IMAGE_CLASSIFY_AVI_BACK:
	{
		nErrorCode = LCP_BACK_Black(matSrcBuffer, matDstImage, rectROI, rcAlignCellROI, dPara, nCommonPara, strAlgPath, strBubblePath, strBubblePath_Menual, eInspMode, EngineerDefectJudgment);
	}
	break;


	// 영상 번호가 잘못 들어온 경우.
	default:
		return E_ERROR_CODE_TRUE;
	}


	matDarkBuffer = matDstImage[E_DEFECT_COLOR_DARK].clone();
	matBrightBuffer = matDstImage[E_DEFECT_COLOR_BRIGHT].clone();


	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Result Copy."));

	// 할당 해제
	matDstImage[E_DEFECT_COLOR_DARK].release();
	matDstImage[E_DEFECT_COLOR_BRIGHT].release();

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("End."));

	return nErrorCode;
}

void CInspectPoint::Flattening(int nFlatteningType, BYTE *pImage, CSize szImage)
{
	if (!pImage || szImage.cx <= 0 || szImage.cy <= 0) return;

	switch (nFlatteningType)
	{
	case Flattening_Horizental:
		FlattenMeanHorizontal(pImage, szImage);
		break;
	case Flattening_Vertical:
		FlattenMeanVertical(pImage, szImage);
		break;
	case Flattening_All_H:
		FlattenMeanHorizontal(pImage, szImage);
		FlattenMeanVertical(pImage, szImage);
		break;
	case Flattening_All_V:
		FlattenMeanVertical(pImage, szImage);
		FlattenMeanHorizontal(pImage, szImage);
		break;
	}
}

void CInspectPoint::FlattenMeanHorizontal(BYTE *pImage, CSize szImage)
{
	if (!pImage || szImage.cx <= 0 || szImage.cy <= 0) return;

	register int nX = 0, nY = 0, nXY = 0;
	float fSum = 0, fMean = 0, fFlatten;
	float fWidth = 0.0f;

	fWidth = (float)szImage.cx;
	for (nY = 0; nY < szImage.cy; nY++)
	{
		fSum = 0;
		nXY = szImage.cx * nY;
		for (nX = 0; nX < szImage.cx; nX++)
			fSum += pImage[nXY + nX];

		fMean = fSum / fWidth;
		for (nX = 0; nX < szImage.cx; nX++)
		{
			fFlatten = pImage[nXY + nX] + (_BIAS_ - fMean);

			if (fFlatten < 0) pImage[nXY + nX] = 0;
			else if (fFlatten > 255) pImage[nXY + nX] = 255;
			else pImage[nXY + nX] = (BYTE)fFlatten;
		}
	}
}

void CInspectPoint::FlattenMeanVertical(BYTE *pImage, CSize szImage)
{
	if (!pImage || szImage.cx <= 0 || szImage.cy <= 0) return;

	register int nX = 0, nY = 0, nXY = 0;
	float fSubX = 0, fFlatten = 0;
	float fHeight = 0.0f;
	float *pfSum = new float[szImage.cx];

	memset(pfSum, 0, sizeof(float) * szImage.cx);

	for (nY = 0; nY < szImage.cy; nY++)
	{
		nXY = szImage.cx * nY;
		for (nX = 0; nX < szImage.cx; nX++)
			pfSum[nX] += pImage[nXY + nX];
	}

	fHeight = (float)szImage.cy;
	for (nX = 0; nX < szImage.cx; nX++)
		pfSum[nX] /= fHeight;

	for (nY = 0; nY < szImage.cy; nY++)
	{
		nXY = szImage.cx * nY;
		for (nX = 0; nX < szImage.cx; nX++)
		{
			fFlatten = pImage[nXY + nX] - pfSum[nX] + _BIAS_;

			if (fFlatten < 0) pImage[nXY + nX] = 0;
			else if (fFlatten > 255) pImage[nXY + nX] = 255;
			else pImage[nXY + nX] = (BYTE)fFlatten;
		}
	}

	delete[] pfSum;
}



// Dust 제거 후, 결과 벡터 넘겨주기
long CInspectPoint::GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer,cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer,
	 cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 파라미터 NULL인 경우.
	if( dPara == NULL )					return E_ERROR_CODE_EMPTY_PARA;
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcBuffer.empty() )							return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstBuffer[E_DEFECT_COLOR_DARK].empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstBuffer[E_DEFECT_COLOR_BRIGHT].empty() )	return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDustBuffer[E_DEFECT_COLOR_DARK].empty() )	return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDustBuffer[E_DEFECT_COLOR_BRIGHT].empty() )	return E_ERROR_CODE_EMPTY_BUFFER;


	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	bool	bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	int&	nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	bool	bDefectNum			= (nCommonPara[E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG	] > 0) ? true : false;
	bool	bDrawDust			= (nCommonPara[E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;

	// 범위 크게 설정
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// 불량 판정 & 결과 보내기
	{
		cv::Rect rectBlobROI;
		Insp_RectSet(rectBlobROI, rectROI, matSrcBuffer.cols, matSrcBuffer.rows);

		// 라벨
		CFeatureExtraction cFeatureExtraction;
		cFeatureExtraction.SetMem( cMem[0] );
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_POINT, m_tInitTime, m_tBeforeTime, m_strAlgLog);

		// E_DEFECT_COLOR_DARK 결과
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment_LCP(matSrcBuffer(rectBlobROI),MatColorBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_DARK](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_DARK, dPara,_T("DP_"), EngineerDefectJudgment, pResultBlob);
		if( nErrorCode != E_ERROR_CODE_TRUE )
		{
			// 메모리 해제
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("BlobJudgment (DARK)."));



		// E_DEFECT_COLOR_BRIGHT 결과
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment_LCP(matSrcBuffer(rectBlobROI), MatColorBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_BRIGHT](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_BRIGHT, dPara,_T("BP_"), EngineerDefectJudgment, pResultBlob);
		if( nErrorCode != E_ERROR_CODE_TRUE )
		{
			// 메모리 해제
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("BlobJudgment (BRIGHT)."));
	}

	// 메모리 해제
	matSrcBuffer.release();
	matDstBuffer[E_DEFECT_COLOR_DARK].release();
	matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("End."));

	return E_ERROR_CODE_TRUE;
}

// 기포 검출 후, 결과 벡터 넘겨주기
long CInspectPoint::GetDefectList_Bubble(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer,
	 cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{
	// 먼지 : 255GV로 설정
	// 기포 : 200GV로 설정

	long nErrorCode = E_ERROR_CODE_TRUE;

	// 범위 크게 설정
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x, ptCorner[E_CORNER_LEFT_BOTTOM].x),
		min(ptCorner[E_CORNER_LEFT_TOP].y, ptCorner[E_CORNER_RIGHT_TOP].y),
		max(ptCorner[E_CORNER_RIGHT_TOP].x, ptCorner[E_CORNER_RIGHT_BOTTOM].x),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y, ptCorner[E_CORNER_RIGHT_BOTTOM].y));

	// 먼지 검출 
	cv::Mat matTempBuf1 = cMem[0]->GetMat(matDstBuffer[E_DEFECT_COLOR_DARK].size(), matDstBuffer[E_DEFECT_COLOR_DARK].type(), false);
	cv::threshold(matDstBuffer[E_DEFECT_COLOR_DARK], matTempBuf1, 230, 255, THRESH_BINARY);
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Dust threshold."));

	// 먼지 제거
	cv::Mat matTempBuf2 = cMem[0]->GetMat(matDstBuffer[E_DEFECT_COLOR_DARK].size(), matDstBuffer[E_DEFECT_COLOR_DARK].type(), false);
	cv::subtract(matDstBuffer[E_DEFECT_COLOR_DARK], matTempBuf1, matTempBuf2);
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("subtract."));

	// 기포 검출
	cv::threshold(matTempBuf2, matTempBuf1, 100, 255, THRESH_BINARY);
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Bubble threshold."));

	cv::Rect rectBlobROI;
	Insp_RectSet(rectBlobROI, rectROI, matSrcBuffer.cols, matSrcBuffer.rows);

	// 라벨
	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem(cMem[0]);
	cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_POINT, m_tInitTime, m_tBeforeTime, m_strAlgLog);

	// BUBBLE 결과
	nErrorCode = cFeatureExtraction.DoDefectBlobSingleJudgment(matSrcBuffer(rectBlobROI), matTempBuf1(rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
		nCommonPara, E_DEFECT_COLOR_DARK, _T("Bubble_"), EngineerDefectJudgment, pResultBlob, E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLE, true);
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Bubble Blob."));

	return nErrorCode;
}

//LCP 동축 검출 알고리즘
long CInspectPoint::LCP_COAXIAL(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false; //LCP 중간이미지
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];


	int nPowFlag = (int)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_USE];
	double nPowValue = (double)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE];

	//밝은 불량 파라미터
	//	Blur 배경 Alg
	int nBL_Blur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_BL_BLUR_SIZE];
	int nBL_GauBlur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_BL_GAUBLUR_SIZE];
	double nBL_GauBlur_Sig = (double)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_BL_GAUBLUR_SIG];
	int nBL_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_BL_THRESHOLD];

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	/////////////////////////////////////// Gray_20 에서는 제곱해서 사용해야함
	
	if (nPowFlag > 0) {
		cv::Mat	 matPow_Result;

		//Image Pow
		cv::Mat matSrcPow_Temp = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);
		cv::Mat matPow_TempBuf = cv::Mat::zeros(matSrcImage.size(), CV_16U);

		Image_Pow(matSrcImage.type(), nPowValue, matSrcImage, matPow_TempBuf);

		matPow_TempBuf.convertTo(matPow_Result, CV_8UC1);

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Side_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matPow_Result);
		}

		matSrcImage = matPow_Result.clone();
	}
	//////////////////////////////////////////////////////////////
	
	//RangeTH Alg
	CRect rectTemp(rectROI);

	RangeAvgThreshold(matSrcImage, matDstImage, rectTemp, dPara, cMem[0]);

	cv::Mat matRT_Dark_Result;

	matRT_Dark_Result = matDstImage[E_DEFECT_COLOR_DARK].clone();

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_Dark_RangeTh.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Dark_Result);
	}

	// Blur Dust Delete Alg

	cv::Mat matBL_Blur;
	cv::Mat matBL_Gau_Blur;
	cv::Mat matBL_Result;

	cv::blur(matSrcImage, matBL_Blur, cv::Size(nBL_Blur_Size, nBL_Blur_Size)); // 30 ,30

	cv::GaussianBlur(matSrcImage, matBL_Gau_Blur, cvSize(nBL_GauBlur_Size, nBL_GauBlur_Size), nBL_GauBlur_Sig); // 3,3  1.5

	cv::subtract(matBL_Gau_Blur, matBL_Blur, matBL_Gau_Blur);

	cv::threshold(matBL_Gau_Blur, matBL_Result, nBL_Threshold, 255, CV_THRESH_BINARY); // 4

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_Bright_BL_Th.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBL_Result);
	}


	//First Judge
	cv::Mat matLabels, stats_1, centroids;
	cv::Rect rectTemp_Dust;
	int labelCount = 0;

	labelCount = First_blob_Coxial(matSrcImage, matRT_Dark_Result, matLabels, stats_1, centroids, rectTemp_Dust,dPara);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_First_blob.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Dark_Result);
	}
	
	matDstImage[E_DEFECT_COLOR_DARK] = matRT_Dark_Result.clone(); // 임시 결과 넘김
	matDstImage[E_DEFECT_COLOR_BRIGHT] = matBL_Result.clone(); // 임시 결과 넘김

	matRT_Dark_Result.release();
	matBL_Result.release();

	return E_ERROR_CODE_TRUE;
}

long CInspectPoint::LCP_COAXIAL_Black(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false; //LCP 중간이미지
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];


	int nPowFlag = (int)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_USE];
	double nPowValue = (double)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE];

	//밝은 불량 파라미터
	//	Blur 배경 Alg
	int nBL_Blur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_BL_BLUR_SIZE];
	int nBL_GauBlur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_BL_GAUBLUR_SIZE];
	double nBL_GauBlur_Sig = (double)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_BL_GAUBLUR_SIG];
	int nBL_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_BL_THRESHOLD];

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	/////////////////////////////////////// Gray_20 에서는 제곱해서 사용해야함

	if (nPowFlag > 0) {
		cv::Mat	 matPow_Result;

		//Image Pow
		cv::Mat matSrcPow_Temp = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);
		cv::Mat matPow_TempBuf = cv::Mat::zeros(matSrcImage.size(), CV_16U);

		Image_Pow(matSrcImage.type(), nPowValue, matSrcImage, matPow_TempBuf);

		matPow_TempBuf.convertTo(matPow_Result, CV_8UC1);

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Side_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matPow_Result);
		}

		matSrcImage = matPow_Result.clone();
	}
	//////////////////////////////////////////////////////////////

	//RangeTH Alg
	CRect rectTemp(rectROI);

	RangeAvgThreshold(matSrcImage, matDstImage, rectTemp, dPara, cMem[0]);

	cv::Mat matRT_Dark_Result;

	matRT_Dark_Result = matDstImage[E_DEFECT_COLOR_DARK].clone();

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_Dark_RangeTh.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Dark_Result);
	}

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_Bright_RangeTh.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}
	// Blur Dust Delete Alg

	cv::Mat matBL_Blur;
	cv::Mat matBL_Gau_Blur;
	cv::Mat matBL_Result;

	cv::blur(matSrcImage, matBL_Blur, cv::Size(nBL_Blur_Size, nBL_Blur_Size)); // 30 ,30

	cv::GaussianBlur(matSrcImage, matBL_Gau_Blur, cvSize(nBL_GauBlur_Size, nBL_GauBlur_Size), nBL_GauBlur_Sig); // 3,3  1.5

	cv::subtract(matBL_Gau_Blur, matBL_Blur, matBL_Gau_Blur);

	cv::threshold(matBL_Gau_Blur, matBL_Result, nBL_Threshold, 255, CV_THRESH_BINARY); // 4

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_Bright_BL_Th.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBL_Result);
	}


	//First Judge
	cv::Mat matLabels, stats_1, centroids;
	cv::Rect rectTemp_Dust;
	int labelCount = 0;

	labelCount = First_blob_Coxial(matSrcImage, matRT_Dark_Result, matLabels, stats_1, centroids, rectTemp_Dust, dPara);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_First_blob.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Dark_Result);
	}

	matDstImage[E_DEFECT_COLOR_DARK] = matRT_Dark_Result.clone(); // 임시 결과 넘김
	//matDstImage[E_DEFECT_COLOR_BRIGHT] = matBL_Result.clone(); // 임시 결과 넘김

	

	matRT_Dark_Result.release();
	matBL_Result.release();

	return E_ERROR_CODE_TRUE;
}


long CInspectPoint::LCP_COAXIAL_Transparency(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	//밝은 불량 파라미터
	//	Blur 배경 Alg
	int nBL_Blur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_BL_BLUR_SIZE];
	int nBL_GauBlur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_BL_GAUBLUR_SIZE];
	double nBL_GauBlur_Sig = (double)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_BL_GAUBLUR_SIG];
	int nBL_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_BL_THRESHOLD];

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	cv::Mat matRT_Dark_Result;
	
	CRect rectTemp(rectROI);

	//RangeTH Alg
	RangeAvgThreshold(matSrcImage, matDstImage, rectTemp, dPara, cMem[0]);

	matRT_Dark_Result = matDstImage[E_DEFECT_COLOR_DARK].clone();
	

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_Dark_RangeTh.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Dark_Result);
	}




	// 밝은 불량 및 스크레치
	cv::Mat matBL_Blur;
	cv::Mat matBL_Gau_Blur;
	cv::Mat matBL_Result;

	cv::blur(matSrcImage, matBL_Blur, cv::Size(nBL_Blur_Size, nBL_Blur_Size)); // 30 ,30

	cv::GaussianBlur(matSrcImage, matBL_Gau_Blur, cvSize(nBL_GauBlur_Size, nBL_GauBlur_Size), nBL_GauBlur_Sig); // 3,3  1.5

	cv::subtract(matBL_Gau_Blur, matBL_Blur, matBL_Gau_Blur);

	cv::threshold(matBL_Gau_Blur, matBL_Result, nBL_Threshold, 255, CV_THRESH_BINARY); // 10

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_Bright_BL_Th.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBL_Result);
	}

	cv::Mat matLabels, stats_1, centroids;
	cv::Rect rectTemp_Dust;
	int labelCount = 0;

	labelCount = First_blob_Transparency_Coxial(matSrcImage, matRT_Dark_Result, matLabels, stats_1, centroids, rectTemp_Dust,dPara);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_COAXIAL_POINT_%02d_First_blob.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Dark_Result);
	}

	matDstImage[E_DEFECT_COLOR_DARK] = matRT_Dark_Result.clone(); // 임시 결과 넘김
	matDstImage[E_DEFECT_COLOR_BRIGHT] = matBL_Result.clone(); // 임시 결과 넘김

	matRT_Dark_Result.release();
	matBL_Result.release();



	return E_ERROR_CODE_TRUE;
}

long CInspectPoint::LCP_SIDE(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false; //LCP 중간이미지
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	int nPowFlag = (int)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_USE];
	double nPowValue = (double)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE];

	//밝은 불량 파라미터
	//	Line Mura 제거 Alg
	int nLM_Maximum_Size_1 = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_LM_MAXIMUM_SIZE];
	int nLM_Blur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_LM_BLUR_SIZE];
	int nLM_GauBlur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_LM_GAUBLUR_SIZE];
	double dLM_GauBlur_Sig = (double)dPara[E_PARA_LCP_SUBSTANCE_SIDE_LM_GAUBLUR_SIG];
	int nLM_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_LM_THRESHOLD];
	int nLM_Maximum_Size_2 = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_LM_MAXIMUM_SIZE_2];

	//	Blur 배경 Alg
	int nBL_Blur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_BL_BLUR_SIZE];
	int nBL_GauBlur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_BL_GAUBLUR_SIZE];
	double dBL_GauBlur_Sig = (double)dPara[E_PARA_LCP_SUBSTANCE_SIDE_BL_GAUBLUR_SIG];
	int nBL_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_BL_BLUR_THRESHOLD];

	//  RangeThreshold 후처리
	int nRT_Resize = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_RT_RESIZE];
	int nRT_Maximum_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_RT_MAXIMUM_SIZE];
	int nRT_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_SIDE_RT_THRESHOLD];
	//////////////////////////////////////////////////////////////////////////

	CRect rectTemp(rectROI);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}
	/////////////////////////////////////// Gray_20 에서는 제곱해서 사용해야함
	
	if (nPowFlag > 0) {

		cv::Mat	 matPow_Result;

		//Image Pow
		cv::Mat matSrcPow_Temp = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);
		cv::Mat matPow_TempBuf = cv::Mat::zeros(matSrcImage.size(), CV_16U);

		Image_Pow(matSrcImage.type(), nPowValue, matSrcImage, matPow_TempBuf);

		matPow_TempBuf.convertTo(matPow_Result, CV_8UC1);

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Side_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matPow_Result);
		}

		matSrcImage = matPow_Result.clone();
	}

	/////////////////////////////////////// Line Mura Delete (팬케이크 외곽 주름 제거 Alg)
	
	cv::Mat matSrc_LineMura = matSrcImage.clone();

	cv::Mat matLM_Blur;
	cv::Mat matLM_Gau_Blur;
	cv::Mat matLM_Result;

	MaximumFilter(matSrc_LineMura, matSrc_LineMura, nLM_Maximum_Size_1); //7

	cv::blur(matSrcImage, matLM_Blur, cv::Size(nLM_Blur_Size, nLM_Blur_Size)); // 17,17

	cv::GaussianBlur(matSrc_LineMura, matLM_Gau_Blur, cvSize(nLM_GauBlur_Size, nLM_GauBlur_Size), dLM_GauBlur_Sig); //1,1  1.5

	cv::subtract(matLM_Gau_Blur, matLM_Blur, matLM_Result);

	cv::threshold(matLM_Result, matLM_Result, nLM_Threshold, 255, CV_THRESH_BINARY); //15

	MaximumFilter(matLM_Result, matLM_Result, nLM_Maximum_Size_2); //7

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Bright_Line_Mura.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matLM_Result);
	}

	///////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//RangeTh Alg
	RangeAvgThreshold(matSrcImage, matDstImage, rectTemp, dPara, cMem[0]);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_DARK_RangeTH_Result.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Bright_RangeTH_Result.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}
	
	cv::Mat matRT_Dark_Result;
	cv::Mat matRT_Bright_Result;

	matRT_Dark_Result = matDstImage[E_DEFECT_COLOR_DARK].clone();
	matRT_Bright_Result = matDstImage[E_DEFECT_COLOR_BRIGHT].clone();

	//////////////////////////////////////////////////////////////////////////
	// Blur Dust Delete Alg
	cv::Mat matBL_Blur;
	cv::Mat matBL_Gau_Blur;
	cv::Mat matBL_Result;
	

	cv::blur(matSrcImage, matBL_Blur, cv::Size(nBL_Blur_Size, nBL_Blur_Size)); //30,30

	cv::GaussianBlur(matSrcImage, matBL_Gau_Blur, cvSize(nBL_GauBlur_Size, nBL_GauBlur_Size), dBL_GauBlur_Sig); //3,3 1.5

	cv::subtract(matBL_Gau_Blur, matBL_Blur, matBL_Gau_Blur);

	cv::threshold(matBL_Gau_Blur, matBL_Result, nBL_Threshold, 255, CV_THRESH_BINARY); //5


	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_BRIGHT_BL_Result.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBL_Result);
	}

	//RangeTh 결과랑 BL Alg 결과 통합
	//matRT_Bright_Result += matBL_Result;

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_BRIGHT_RT+BL_Result.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Bright_Result);
	}

	//후처리
	cv::resize(matRT_Bright_Result, matRT_Bright_Result, matRT_Bright_Result.size() / nRT_Resize, INTER_LINEAR);

	MaximumFilter(matRT_Bright_Result, matRT_Bright_Result, nRT_Maximum_Size);

	cv::resize(matRT_Bright_Result, matRT_Bright_Result, matSrcImage.size(), INTER_LINEAR);

	cv::threshold(matRT_Bright_Result, matRT_Bright_Result, nRT_Threshold, 255, CV_THRESH_BINARY);
	 
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_BRIGHT_BL+RT_MF.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Bright_Result);
	}

	//Dark 결과와 Birhgt 결과 붙어있으면 Dark 제거 Alg
	LCP_Dark_Bright_Compare_Del(matRT_Dark_Result, matRT_Bright_Result);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Dark_Bright_Compare_Del_Result.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Dark_Result);
	}
	

	// First Judge
	cv::Mat matLabels, stats_1, centroids;
	cv::Rect rectTemp_Dust;
	int labelCount = 0;

	labelCount = First_blob_Side(matSrcImage, matRT_Dark_Result, matLabels, stats_1, centroids, rectTemp_Dust,dPara);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Dark_First_blob.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Dark_Result);
	}

	

	// Line Mura 결과도 통합
	//matRT_Bright_Result += matLM_Result;

	matDstImage[E_DEFECT_COLOR_BRIGHT] = matRT_Bright_Result.clone(); // 임시 결과 넘김
	matDstImage[E_DEFECT_COLOR_DARK] = matRT_Dark_Result.clone(); // 임시 결과 넘김



	matRT_Bright_Result.release();
	matRT_Dark_Result.release();

	return E_ERROR_CODE_TRUE;

}

long CInspectPoint::LCP_SIDE_Transparency(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	float	pow_value = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE];

	int		nRT_Resize = (int)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_SIDE_RT_RESIZE];
	int		nRT_Maximum_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_SIDE_RT_MAXIMUM_SIZE];
	int		nRT_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_SIDE_RT_THRESHOLD];

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	cv::Mat	 matPow_Result;

	CRect rectTemp(rectROI);

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem[0]);

	//Image Pow
	cv::Mat matSrcPow_Temp = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);
	cv::Mat matPow_TempBuf = cv::Mat::zeros(matSrcImage.size(), CV_16U);

	Image_Pow(matSrcImage.type(), pow_value, matSrcImage, matPow_TempBuf);

	matPow_TempBuf.convertTo(matPow_Result, CV_8UC1);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Side_Bright_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matPow_Result);
	}

	//RangeTH Alg
	RangeAvgThreshold_DUST(matPow_Result, matDstImage, rectTemp, dPara, &cMatBufTemp);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Side_Bright_RangeTH.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	//RangeTH 후처리 Alg
	cv::resize(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], matSrcImage.size() / nRT_Resize, INTER_LINEAR);

	MaximumFilter(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], nRT_Maximum_Size);

	cv::resize(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], matSrcImage.size(), INTER_LINEAR);

	cv::threshold(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], nRT_Threshold, 255, CV_THRESH_BINARY);


	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_SIDE_POINT_%02d_Side_Bright_RangeTH_MF.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectPoint::LCP_DUST(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터 
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false; //LCP 중간이미지
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];
	float	pow_value = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE];

	//밝은 불량 파라미터
	//	Blur 배경 Alg
	int nBL_Blur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_BL_BLUR_SIZE];
	int nBL_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_BL_THRESHOLD];
	int nBL_Maximum_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_RT_MAXIMUM_SIZE];

	//  RangeThreshold 후처리
	int nRT_Resize = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_RT_RESIZE];
	int nRT_Maximum_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_RT_MAXIMUM_SIZE];
	int nRT_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_RT_THRESHOLD];
	
	//////////////////////////////////////////////////////////////////////////
	CRect rectTemp(rectROI);
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem[0]);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	//////////////////////////////////////////////////////////////////////////
	//원본 이미지 Pow
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matSrcDust_Pow;
	matSrcDust_Pow = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matPowTemp = cv::Mat::zeros(matSrcImage.size(), CV_16UC1);

	Image_Pow(matSrcImage.type(), pow_value, matSrcImage, matPowTemp);

	matPowTemp.convertTo(matSrcDust_Pow,CV_8UC1);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcDust_Pow);
	}

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Blur Dust 검출 Alg
	cv::Mat matBL_Blur;
	cv::Mat matBL_Result;
	
	cv::blur(matSrcDust_Pow, matBL_Blur, cv::Size(nBL_Blur_Size, nBL_Blur_Size));

	cv::subtract(matSrcDust_Pow, matBL_Blur, matBL_Result);

	cv::threshold(matBL_Result, matBL_Result, nBL_Threshold, 255, CV_THRESH_BINARY);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_Result_Blur_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBL_Result);
	}
	
	MaximumFilter(matBL_Result, matBL_Result, nBL_Maximum_Size);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_Result_Blur_MF_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBL_Result);
	}
	//////////////////////////////////////////////////////////////////////////

	
	//////////////////////////////////////////////////////////////////////////
	//Range Th 이용 Dust 검출
	RangeAvgThreshold_DUST(matSrcDust_Pow, matDstImage, rectTemp, dPara, &cMatBufTemp);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_RangeThreshold_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Range Th 이후 후처리
	cv::resize(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], matSrcImage.size() / nRT_Resize, INTER_LINEAR);

	MaximumFilter(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], nRT_Maximum_Size);

	cv::resize(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], matSrcImage.size(), INTER_LINEAR);

	cv::threshold(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], nRT_Threshold, 255, CV_THRESH_BINARY);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_RangeThreshold_Bright_MF.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}


	
	//////////////////////////////////////////////////////////////////////////
	//결과 통합
	matDstImage[E_DEFECT_COLOR_BRIGHT] += matBL_Result;


	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_FinalResult_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	matSrcDust_Pow.release();
	matBL_Blur.release();
	matBL_Result.release();

	return E_ERROR_CODE_TRUE;
}

long CInspectPoint::LCP_DUST_Black(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터 
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false; //LCP 중간이미지
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];
	float	pow_value = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE];

	//밝은 불량 파라미터
	//	Blur 배경 Alg
	int nBL_Blur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_BL_BLUR_SIZE];
	int nBL_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_BL_THRESHOLD];
	int nBL_Maximum_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_RT_MAXIMUM_SIZE];

	//  RangeThreshold 후처리
	int nRT_Resize = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_RT_RESIZE];
	int nRT_Maximum_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_RT_MAXIMUM_SIZE];
	int nRT_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_DUST_RT_THRESHOLD];

	//////////////////////////////////////////////////////////////////////////
	CRect rectTemp(rectROI);
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem[0]);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	//////////////////////////////////////////////////////////////////////////
	//원본 이미지 Pow
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matSrcDust_Pow;
	matSrcDust_Pow = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matPowTemp = cv::Mat::zeros(matSrcImage.size(), CV_16UC1);

	Image_Pow(matSrcImage.type(), pow_value, matSrcImage, matPowTemp);

	matPowTemp.convertTo(matSrcDust_Pow, CV_8UC1);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcDust_Pow);
	}

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Blur Dust 검출 Alg
	cv::Mat matBL_Blur;
	cv::Mat matBL_Result;

	cv::blur(matSrcDust_Pow, matBL_Blur, cv::Size(nBL_Blur_Size, nBL_Blur_Size));

	cv::subtract(matSrcDust_Pow, matBL_Blur, matBL_Result);

	cv::threshold(matBL_Result, matBL_Result, nBL_Threshold, 255, CV_THRESH_BINARY);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_Result_Blur_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBL_Result);
	}

	MaximumFilter(matBL_Result, matBL_Result, nBL_Maximum_Size);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_Result_Blur_MF_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBL_Result);
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//Range Th 이용 Dust 검출
	RangeAvgThreshold_DUST(matSrcDust_Pow, matDstImage, rectTemp, dPara, &cMatBufTemp);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_RangeThreshold_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Range Th 이후 후처리
	cv::resize(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], matSrcImage.size() / nRT_Resize, INTER_LINEAR);

	MaximumFilter(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], nRT_Maximum_Size);

	cv::resize(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], matSrcImage.size(), INTER_LINEAR);

	cv::threshold(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], nRT_Threshold, 255, CV_THRESH_BINARY);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_RangeThreshold_Bright_MF.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}



	//////////////////////////////////////////////////////////////////////////
	//결과 통합
	matDstImage[E_DEFECT_COLOR_BRIGHT] += matBL_Result;


	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_FinalResult_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	matSrcDust_Pow.release();
	matBL_Blur.release();
	matBL_Result.release();

	return E_ERROR_CODE_TRUE;
}

long CInspectPoint::LCP_DUST_Transparency(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	float	pow_value = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE];

	int		nBL_GauBlur_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_DUST_BL_GAUBLUR_SIZE];
	double	nBL_GauBlur_Sig = (double)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_DUST_BL_GAUBLUR_SIG];
	int		nBL_Threshold = (int)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_DUST_BL_THRESHOLD];

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}


	cv::Mat matSrcPow_Temp = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);
	cv::Mat matPow_TempBuf = cv::Mat::zeros(matSrcImage.size(), CV_16U);

	cv::Mat matPow_Result;

	matSrcImage.copyTo(matSrcPow_Temp);

	//Image Pow
	Image_Pow(matSrcPow_Temp.type(), pow_value, matSrcPow_Temp, matPow_TempBuf);

	matPow_TempBuf.convertTo(matPow_Result, CV_8UC1);

	GaussianBlur(matPow_Result, matPow_Result, Size(nBL_GauBlur_Size, nBL_GauBlur_Size), nBL_GauBlur_Sig); //3,3 ,3.0

	//밝기편차 고려 Filter
	Lipschitz_Fillter(matPow_Result);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_Lipschitz.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matPow_Result);
	}

	cv::threshold(matPow_Result, matPow_Result, nBL_Threshold, 255.0, CV_THRESH_BINARY); //14


	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_DUST_POINT_%02d_TH_Result.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matPow_Result);
	}

	matPow_Result.release();

	return E_ERROR_CODE_TRUE;
}

long CInspectPoint::LCP_BACK(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, cv::Rect rcAlignCellROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false; //LCP 중간이미지
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	float	pow_value = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE];

	int nRT_Maximum_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_BACK_RT_MAXIMUM_SIZE];

	int nBubble_Flag = (int)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_FLAG];

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	//Image Pow
	cv::Mat matSrcPow_Temp;

	CRect rectTemp(rectROI);

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem[0]);

	cv::Mat matPowtemp = cv::Mat::zeros(matSrcImage.size(), CV_16UC1);

	matSrcPow_Temp = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);

	Image_Pow(matSrcImage.type(), pow_value, matSrcImage, matPowtemp);

	matPowtemp.convertTo(matSrcPow_Temp, CV_8UC1);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcPow_Temp);
	}
	//////////////////////////////////////////////////////////////////////////

	//RangeTH Alg
	RangeAvgThreshold(matSrcPow_Temp, matDstImage, rectTemp, dPara, cMem[0]);

	//////////////////////////////////////////////////////////////////////////
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Bright_RangeTH.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	//RangeTH 후처리
	MaximumFilter(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], nRT_Maximum_Size);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Bright_MF.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	////////////////////////////////////////////////////////////////////////
	//Area 카메라 기포 검사
	if (nBubble_Flag > 0) {
		cv::Mat matSrcBubble;

		if (eInspMode == eManualInspect) {
			matSrcBubble = cv::imread((cv::String)(CStringA)strBubblePath_Menual + (cv::String)(CStringA)_T("Bubble.bmp"), IMREAD_UNCHANGED);
			cv::imwrite((cv::String)(CStringA)strBubblePath + (cv::String)(CStringA)_T("Bubble.bmp"), matSrcBubble);
		}else{
			matSrcBubble = cv::imread((cv::String)(CStringA)strBubblePath + (cv::String)(CStringA)_T("Bubble.bmp"), IMREAD_UNCHANGED);
		}

		if (matSrcBubble.empty()) {
			return E_ERROR_CODE_EMPTY_BUFFER;
		}

		//cv::Mat matBubble_Temp = cv::Mat::zeros(matSrcImage.size(), matSrcImage.type());
// 		cv::Mat matMoi;
// 		matSrcBubble.copyTo(matMoi);
// 		cv::resize(matMoi, matMoi, Size(rcAlignCellROI.width, rcAlignCellROI.height), INTER_CUBIC);
// 		cv::imwrite((cv::String)(CStringA)strBubblePath + "Bubble_Resize.bmp", matMoi);

		cv::resize(matSrcBubble, matSrcBubble, Size(matSrcBubble.cols * 2, matSrcBubble.rows * 2), INTER_CUBIC);
	
		cv::Mat matBubble_Temp = cv::Mat::zeros(rcAlignCellROI.size(), matSrcBubble.type());

		//cv::Mat matBubble_WA_Result = cv::Mat::zeros(matSrcImage.size(), matSrcImage.type());
		//방법1
		// 			cv::resize(matMoi, matMoi, Size(stCamAlignInfo[nCameraNum].rcAlignCellROI.width, stCamAlignInfo[nCameraNum].rcAlignCellROI.height),INTER_CUBIC);
		// 			 
		// 			matMoi.copyTo(matMoi_Temp(stCamAlignInfo[nCameraNum].rcAlignCellROI));
		////////////////////////////////////////////////////////////////////////// 

		//방법2
// 		cv::resize(matSrcBubble, matSrcBubble, Size(matSrcBubble.cols * 2, matSrcBubble.rows * 2), INTER_CUBIC);
// 
// 		cv::Point2i ptCenter_ori(rcAlignCellROI.x + (rcAlignCellROI.width / 2), rcAlignCellROI.y + (rcAlignCellROI.height / 2));
// 		cv::Point2i ptCenter_obj(matSrcBubble.cols / 2, matSrcBubble.rows / 2);
// 
// 		Mat M2(2, 3, CV_32FC1);
// 		M2.at<float>(0, 0) = 1.f;
// 		M2.at<float>(1, 0) = 0.f;
// 		M2.at<float>(0, 1) = 0.f;
// 		M2.at<float>(1, 1) = 1.f; // 단위 행렬 -> 즉, 각도는 그대로
// 		M2.at<float>(0, 2) = ptCenter_ori.x - ptCenter_obj.x; // 첫 영상의 작은 원을 기준으로 x축 이동값
// 		M2.at<float>(1, 2) = ptCenter_ori.y - ptCenter_obj.y; // 첫 영상의 작은 원을 기준으로 y축 이동값
// 		cv::warpAffine(matSrcBubble, matBubble_Temp, M2, matBubble_Temp.size()); // 이동
// 
// 		cv::imwrite((cv::String)(CStringA)strBubblePath + "Bubble_WarpAffine.bmp", matBubble_Temp);

		/////////////////////////////////////////////////////////////

		//방법3
		//////////////////////////////////////////////////////////////////////////
		cv::Point2f ptSrc[4] = { cv::Point2f(0, 0),
			cv::Point2f(matSrcBubble.cols ,	0),
			cv::Point2f(matSrcBubble.cols ,	matSrcBubble.rows),
			cv::Point2f(0,	matSrcBubble.rows) };

		cv::Point2f ptDst[4] = { cv::Point2f((float)rectROI.left,	(float)rectROI.top),
			cv::Point2f((float)rectROI.right,	(float)rectROI.top),
			cv::Point2f((float)rectROI.right,(float)rectROI.bottom),
			cv::Point2f((float)rectROI.left,	(float)rectROI.bottom) };



		cv::Mat mat2Warp = cv::getPerspectiveTransform(ptSrc, ptDst);		//	Warping 계수
		cv::warpPerspective(matSrcBubble, matBubble_Temp, mat2Warp, matSrcImage.size(), CV_INTER_AREA);
	
		cv::imwrite((cv::String)(CStringA)strBubblePath + "Bubble_WarpAffine.bmp", matBubble_Temp);

// 		matBubble_Temp.copyTo(matBubble_WA_Result(rcAlignCellROI));
// 
// 		cv::imwrite((cv::String)(CStringA)strBubblePath + "Bubble_WarpAffine_Result.bmp", matBubble_WA_Result);
		//////////////////////////////////////////////////////////////////////////

		//Bubble 검사 시작

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_Back_POINT_%02d_Bubble_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matBubble_Temp);
		}


		CRect rectTemp(rectROI);

		CMatBuf cMatBufTemp;
		cMatBufTemp.SetMem(cMem[0]);

		cv::Mat matBubble_Result[2];

		matBubble_Result[E_DEFECT_COLOR_DARK] = cv::Mat::zeros(matDstImage[E_DEFECT_COLOR_DARK].size(), matDstImage[E_DEFECT_COLOR_DARK].type());
		matBubble_Result[E_DEFECT_COLOR_BRIGHT] = cv::Mat::zeros(matDstImage[E_DEFECT_COLOR_BRIGHT].size(), matDstImage[E_DEFECT_COLOR_BRIGHT].type());

		RangeAvgThreshold_Bubble(matBubble_Temp, matBubble_Result, rectTemp, dPara, cMem[0]);
		//MaximumFilter(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], nMaximum_Size_Dust);

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Bubble_RangeThreshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matBubble_Result[E_DEFECT_COLOR_BRIGHT]);
		}


		////////////////////////////////////////////////////////////
	}

	matSrcPow_Temp.release();

	return E_ERROR_CODE_TRUE;
}

long CInspectPoint::LCP_BACK_Black(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, cv::Rect rcAlignCellROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false; //LCP 중간이미지
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	float	pow_value = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE];

	int nRT_Maximum_Size = (int)dPara[E_PARA_LCP_SUBSTANCE_BACK_RT_MAXIMUM_SIZE];

	int nBubble_Flag = (int)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_FLAG];

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	//Image Pow
	cv::Mat matSrcPow_Temp;

	CRect rectTemp(rectROI);

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem[0]);

	cv::Mat matPowtemp = cv::Mat::zeros(matSrcImage.size(), CV_16UC1);

	matSrcPow_Temp = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);

	Image_Pow(matSrcImage.type(), pow_value, matSrcImage, matPowtemp);

	matPowtemp.convertTo(matSrcPow_Temp, CV_8UC1);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcPow_Temp);
	}
	//////////////////////////////////////////////////////////////////////////

	//RangeTH Alg
	RangeAvgThreshold(matSrcPow_Temp, matDstImage, rectTemp, dPara, cMem[0]);

	//////////////////////////////////////////////////////////////////////////
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Bright_RangeTH.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	//RangeTH 후처리
	MaximumFilter(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], nRT_Maximum_Size);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Bright_MF.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	////////////////////////////////////////////////////////////////////////
	//Area 카메라 기포 검사
	if (nBubble_Flag > 0) {
		cv::Mat matSrcBubble;

		if (eInspMode == eManualInspect) {
			matSrcBubble = cv::imread((cv::String)(CStringA)strBubblePath_Menual + (cv::String)(CStringA)_T("Bubble.bmp"), IMREAD_UNCHANGED);
			cv::imwrite((cv::String)(CStringA)strBubblePath + (cv::String)(CStringA)_T("Bubble.bmp"), matSrcBubble);
		}
		else {
			matSrcBubble = cv::imread((cv::String)(CStringA)strBubblePath + (cv::String)(CStringA)_T("Bubble.bmp"), IMREAD_UNCHANGED);
		}

		if (matSrcBubble.empty()) {
			return E_ERROR_CODE_EMPTY_BUFFER;
		}

		//cv::Mat matBubble_Temp = cv::Mat::zeros(matSrcImage.size(), matSrcImage.type());
		// 		cv::Mat matMoi;
		// 		matSrcBubble.copyTo(matMoi);
		// 		cv::resize(matMoi, matMoi, Size(rcAlignCellROI.width, rcAlignCellROI.height), INTER_CUBIC);
		// 		cv::imwrite((cv::String)(CStringA)strBubblePath + "Bubble_Resize.bmp", matMoi);

		cv::resize(matSrcBubble, matSrcBubble, Size(matSrcBubble.cols * 2, matSrcBubble.rows * 2), INTER_CUBIC);

		cv::Mat matBubble_Temp = cv::Mat::zeros(rcAlignCellROI.size(), matSrcBubble.type());

		//cv::Mat matBubble_WA_Result = cv::Mat::zeros(matSrcImage.size(), matSrcImage.type());
		//방법1
		// 			cv::resize(matMoi, matMoi, Size(stCamAlignInfo[nCameraNum].rcAlignCellROI.width, stCamAlignInfo[nCameraNum].rcAlignCellROI.height),INTER_CUBIC);
		// 			 
		// 			matMoi.copyTo(matMoi_Temp(stCamAlignInfo[nCameraNum].rcAlignCellROI));
		////////////////////////////////////////////////////////////////////////// 

		//방법2
		// 		cv::resize(matSrcBubble, matSrcBubble, Size(matSrcBubble.cols * 2, matSrcBubble.rows * 2), INTER_CUBIC);
		// 
		// 		cv::Point2i ptCenter_ori(rcAlignCellROI.x + (rcAlignCellROI.width / 2), rcAlignCellROI.y + (rcAlignCellROI.height / 2));
		// 		cv::Point2i ptCenter_obj(matSrcBubble.cols / 2, matSrcBubble.rows / 2);
		// 
		// 		Mat M2(2, 3, CV_32FC1);
		// 		M2.at<float>(0, 0) = 1.f;
		// 		M2.at<float>(1, 0) = 0.f;
		// 		M2.at<float>(0, 1) = 0.f;
		// 		M2.at<float>(1, 1) = 1.f; // 단위 행렬 -> 즉, 각도는 그대로
		// 		M2.at<float>(0, 2) = ptCenter_ori.x - ptCenter_obj.x; // 첫 영상의 작은 원을 기준으로 x축 이동값
		// 		M2.at<float>(1, 2) = ptCenter_ori.y - ptCenter_obj.y; // 첫 영상의 작은 원을 기준으로 y축 이동값
		// 		cv::warpAffine(matSrcBubble, matBubble_Temp, M2, matBubble_Temp.size()); // 이동
		// 
		// 		cv::imwrite((cv::String)(CStringA)strBubblePath + "Bubble_WarpAffine.bmp", matBubble_Temp);

		/////////////////////////////////////////////////////////////

		//방법3
		//////////////////////////////////////////////////////////////////////////
		cv::Point2f ptSrc[4] = { cv::Point2f(0, 0),
			cv::Point2f(matSrcBubble.cols ,	0),
			cv::Point2f(matSrcBubble.cols ,	matSrcBubble.rows),
			cv::Point2f(0,	matSrcBubble.rows) };

		cv::Point2f ptDst[4] = { cv::Point2f((float)rectROI.left,	(float)rectROI.top),
			cv::Point2f((float)rectROI.right,	(float)rectROI.top),
			cv::Point2f((float)rectROI.right,(float)rectROI.bottom),
			cv::Point2f((float)rectROI.left,	(float)rectROI.bottom) };



		cv::Mat mat2Warp = cv::getPerspectiveTransform(ptSrc, ptDst);		//	Warping 계수
		cv::warpPerspective(matSrcBubble, matBubble_Temp, mat2Warp, matSrcImage.size(), CV_INTER_AREA);

		cv::imwrite((cv::String)(CStringA)strBubblePath + "Bubble_WarpAffine.bmp", matBubble_Temp);

		// 		matBubble_Temp.copyTo(matBubble_WA_Result(rcAlignCellROI));
		// 
		// 		cv::imwrite((cv::String)(CStringA)strBubblePath + "Bubble_WarpAffine_Result.bmp", matBubble_WA_Result);
		//////////////////////////////////////////////////////////////////////////

		//Bubble 검사 시작

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_Back_POINT_%02d_Bubble_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matBubble_Temp);
		}


		CRect rectTemp(rectROI);

		CMatBuf cMatBufTemp;
		cMatBufTemp.SetMem(cMem[0]);

		cv::Mat matBubble_Result[2];

		matBubble_Result[E_DEFECT_COLOR_DARK] = cv::Mat::zeros(matDstImage[E_DEFECT_COLOR_DARK].size(), matDstImage[E_DEFECT_COLOR_DARK].type());
		matBubble_Result[E_DEFECT_COLOR_BRIGHT] = cv::Mat::zeros(matDstImage[E_DEFECT_COLOR_BRIGHT].size(), matDstImage[E_DEFECT_COLOR_BRIGHT].type());

		RangeAvgThreshold_Bubble(matBubble_Temp, matBubble_Result, rectTemp, dPara, cMem[0]);
		//MaximumFilter(matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT], nMaximum_Size_Dust);

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Bubble_RangeThreshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matBubble_Result[E_DEFECT_COLOR_BRIGHT]);
		}


		////////////////////////////////////////////////////////////
	}

	matSrcPow_Temp.release();

	return E_ERROR_CODE_TRUE;
}

long CInspectPoint::LCP_BACK_Transparency(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Mat matSrc_Stage)
{
	long	nErrorCode = E_ERROR_CODE_TRUE;
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	float	pow_value = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE];

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	cv::Mat  matRT_Dark_Result;

	cv::Mat matStatetemp = matSrc_Stage.clone();
	cv::subtract(matSrc_Stage, matSrcImage, matStatetemp);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_EmtySteage_Subtract.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matStatetemp);
	}

	for (int i = 0; i < matStatetemp.cols*matStatetemp.rows; i++)
	{
		matStatetemp.data[i] = 255 - matStatetemp.data[i];
	}

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_Invert.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matStatetemp);
	}

	CRect rectTemp(rectROI);

	RangeAvgThreshold(matStatetemp, matDstImage, rectTemp, dPara, cMem[0]);

	matRT_Dark_Result = matDstImage[E_DEFECT_COLOR_DARK].clone();

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_RangeTH.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Dark_Result);
	}

	// First Blob
	cv::Mat matLabels, stats_1, centroids;
	cv::Rect rectTemp_Dust;
	int labelCount = 0;

	labelCount = First_blob_Transparency_Back(matSrcImage, matRT_Dark_Result, matLabels, stats_1, centroids, rectTemp_Dust, dPara);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_BACK_POINT_%02d_First_blob.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matRT_Dark_Result);
	}

	matDstImage[E_DEFECT_COLOR_DARK] = matRT_Dark_Result.clone(); // 임시 결과 넘김


	matRT_Dark_Result.release();

	return E_ERROR_CODE_TRUE;
}



void CInspectPoint::LCP_Dark_Bright_Compare_Del(cv::Mat Gray_Side_Dark, cv::Mat Gray_Side_Bright)
{

	cv::Mat matLabels_dark, stats_dark, centroids_dark;
	cv::Rect rectTemp_Dark;
	cv::Rect rectTemp_Dark2;
	cv::Rect rectTemp_Dark3;

	__int64 numOfLables_dark;
	numOfLables_dark = connectedComponentsWithStats(Gray_Side_Dark, matLabels_dark, stats_dark, centroids_dark, 8, CV_32S) - 1;

	for (int idx = 1; idx <= numOfLables_dark; idx++)
	{
		// 객체 면적
		long nArea = stats_dark.at<int>(idx, CC_STAT_AREA);
		if (nArea > 10000)
			continue;

		// Blob 영역 Rect

		rectTemp_Dark.x = stats_dark.at<int>(idx, CC_STAT_LEFT);
		rectTemp_Dark.y = stats_dark.at<int>(idx, CC_STAT_TOP);
		rectTemp_Dark.width = stats_dark.at<int>(idx, CC_STAT_WIDTH);
		rectTemp_Dark.height = stats_dark.at<int>(idx, CC_STAT_HEIGHT);

		cv::Mat matTempROI = Gray_Side_Dark(rectTemp_Dark);
		cv::Mat matLabelROI = matLabels_dark(rectTemp_Dark);
		cv::Mat matTempROI2 = Gray_Side_Bright(rectTemp_Dark);

		cv::Mat matTempROI_2 = Gray_Side_Dark(rectTemp_Dark);

		// 0으로 초기화
		bool duplication = false;

		for (int y = 0; y < rectTemp_Dark.height; y++)
		{
			int		*ptrLabel = matLabelROI.ptr<int>(y);
			uchar	*ptrGray = matTempROI.ptr<uchar>(y);
			uchar	*ptrGray2 = matTempROI2.ptr<uchar>(y);
			for (int x = 0; x < rectTemp_Dark.width; x++, ptrGray++, ptrLabel++, ptrGray2++)
			{
				if (ptrLabel[x] == idx && ptrGray2[x] > 0)
					duplication = true;
			}
		}

		if (duplication == true)
		{
			for (int k = 0; k < rectTemp_Dark.height; k++)
			{
				uchar	*ptrGray2 = matTempROI_2.ptr<uchar>(k);
				for (int j = 0; j < rectTemp_Dark.width; j++, ptrGray2++)
				{
					*ptrGray2 = 0;
				}

			}
		}

		continue;
	}

}

void CInspectPoint::MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize)
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

int CInspectPoint::First_blob_Coxial(cv::Mat Srcimg, cv::Mat DstImg, cv::Mat matLabels, cv::Mat stats, cv::Mat centroids, cv::Rect rectTemp_Dust, double* dPara)
{
	float	fAxis_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_AXIO_RATIO];
	float	fArea_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_AREA_RATIO];
	long	nArea_Min = (long)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_AREA_MIN];
	long	nArea_Max = (long)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_AREA_MAX];
	long	nJudge1_Flag = (long)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_SPEC1_USE];
	long	nJudge1_Area_Min = (long)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_SPEC1_AREA_MIN];
	float	fJudge1_Axis_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_SPEC1_AXIO_RATIO];


	__int64 numOfLables;
	numOfLables = connectedComponentsWithStats(DstImg, matLabels, stats, centroids, 8, CV_32S) - 1;

	double ratio = 0;
	double area_ratio = 0;
 	int rect_count = 0;

	
	// Blob 개수
	for (int idx = 1; idx <= numOfLables; idx++)
	{
		// 객체 면적
		long nArea = stats.at<int>(idx, CC_STAT_AREA);

		// Blob 영역 Rect

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

		// 면적 제한
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
			// 0GV 로 초기화 후, 넘어가기
			cv::Mat matTempROI = DstImg(rectTemp_Dust);
			cv::Mat matLabelROI = matLabels(rectTemp_Dust);

			for (int y = 0; y < rectTemp_Dust.height; y++)
			{
				int		*ptrLabel = (int *)matLabelROI.ptr(y);
				uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

				for (int x = 0; x < rectTemp_Dust.width; x++, ptrLabel++, ptrGray++)
				{
					// 해당 객체
					if (*ptrLabel == idx)	*ptrGray = 0;
				}
			}

			continue;
		}


	}

	return numOfLables;
}

int CInspectPoint::First_blob_Side(cv::Mat Srcimg, cv::Mat DstImg, cv::Mat matLabels, cv::Mat stats, cv::Mat centroids, cv::Rect rectTemp_Dust, double* dPara)
{
	float	fAxis_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_AXIO_RATIO];
	float	fArea_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_AREA_RATIO];
	long	nArea_Min = (long)dPara[E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_AREA_MIN];
	long	nArea_Max = (long)dPara[E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_AREA_MAX];
	long	nJudge1_Flag = (long)dPara[E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_SPEC1_USE];
	long	nJudge1_Area_Min = (long)dPara[E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_SPEC1_AREA_MIN];
	float	fJudge1_Axis_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_SPEC1_AXIO_RATIO];


	__int64 numOfLables;
	numOfLables = connectedComponentsWithStats(DstImg, matLabels, stats, centroids, 8, CV_32S) - 1;

	double ratio = 0;
	double area_ratio = 0;
	int rect_count = 0;


	// Blob 개수
	for (int idx = 1; idx <= numOfLables; idx++)
	{
		// 객체 면적
		long nArea = stats.at<int>(idx, CC_STAT_AREA);

		// Blob 영역 Rect

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

		// 면적 제한
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
			// 0GV 로 초기화 후, 넘어가기
			cv::Mat matTempROI = DstImg(rectTemp_Dust);
			cv::Mat matLabelROI = matLabels(rectTemp_Dust);

			for (int y = 0; y < rectTemp_Dust.height; y++)
			{
				int		*ptrLabel = (int *)matLabelROI.ptr(y);
				uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

				for (int x = 0; x < rectTemp_Dust.width; x++, ptrLabel++, ptrGray++)
				{
					// 해당 객체
					if (*ptrLabel == idx)	*ptrGray = 0;
				}
			}

			continue;
		}


	}

	return numOfLables;
}

int CInspectPoint::First_blob_Transparency_Coxial(cv::Mat Srcimg, cv::Mat DstImg, cv::Mat matLabels, cv::Mat stats, cv::Mat centroids, cv::Rect rectTemp_Dust, double* dPara)
{

	float	fAxis_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_AXIO_RATIO];
	float	fArea_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_AREA_RATIO];
	long	nArea_Min = (long)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_AREA_MIN];
	long	nArea_Max = (long)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_AREA_MAX];
	long	nJudge1_Flag = (long)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_SPEC1_USE];
	long	nJudge1_Area_Min = (long)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_SPEC1_AREA_MIN];
	float	fJudge1_Axis_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_SPEC1_AXIO_RATIO];

	__int64 numOfLables;
	numOfLables = connectedComponentsWithStats(DstImg, matLabels, stats, centroids, 8, CV_32S, CCL_GRANA) - 1;

	double ratio = 0;
	double area_ratio = 0;
	int rect_count = 0;

	
	// Blob 개수
	for (int idx = 1; idx <= numOfLables; idx++)
	{
		// 객체 면적
		long nArea = stats.at<int>(idx, CC_STAT_AREA);

		// Blob 영역 Rect

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

		// 면적 제한
// 		if (
// 			ratio >= 3 || 
// 			area_ratio < 50 || 
// 			nArea <= 5 || nArea >= 100 ||
// 			(nArea >= 50 && ratio >= 2.2))
// 		{
		if (
			ratio >= fAxis_Ratio ||
			area_ratio < fArea_Ratio ||
			nArea <= nArea_Min || nArea >= nArea_Max ||
			(nJudge1_Flag > 0 && nArea >= nJudge1_Area_Min && ratio >= fJudge1_Axis_Ratio))
		{
			rect_count--;
			// 0GV 로 초기화 후, 넘어가기
			cv::Mat matTempROI = DstImg(rectTemp_Dust);
			cv::Mat matLabelROI = matLabels(rectTemp_Dust);

			for (int y = 0; y<rectTemp_Dust.height; y++)
			{
				int		*ptrLabel = (int *)matLabelROI.ptr(y);
				uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

				for (int x = 0; x<rectTemp_Dust.width; x++, ptrLabel++, ptrGray++)
				{
					// 해당 객체
					if (*ptrLabel == idx)	*ptrGray = 0;
				}
			}

			continue;
		}

	}
	
	return numOfLables;
}

int CInspectPoint::First_blob_Transparency_Back(cv::Mat Srcimg, cv::Mat DstImg, cv::Mat matLabels, cv::Mat stats, cv::Mat centroids, cv::Rect rectTemp_Dust, double* dPara)
{

	float	fAxis_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_AXIO_RATIO];
	float	fArea_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_AREA_RATIO];
	long	nArea_Min = (long)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_AREA_MIN];
	long	nArea_Max = (long)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_AREA_MAX];
	long	nJudge1_Flag = (long)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_SPEC1_USE];
	long	nJudge1_Area_Min = (long)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_SPEC1_AREA_MIN];
	float	fJudge1_Axis_Ratio = (float)dPara[E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_SPEC1_AXIO_RATIO];

	__int64 numOfLables;
	numOfLables = connectedComponentsWithStats(DstImg, matLabels, stats, centroids, 8, CV_32S, CCL_GRANA) - 1;

	double ratio = 0;
	double area_ratio = 0;
	int rect_count = 0;


	// Blob 개수
	for (int idx = 1; idx <= numOfLables; idx++)
	{
		// 객체 면적
		long nArea = stats.at<int>(idx, CC_STAT_AREA);

		// Blob 영역 Rect

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

		// 면적 제한
		// 		if (
		// 			ratio >= 3 || 
		// 			area_ratio < 50 || 
		// 			nArea <= 5 || nArea >= 100 ||
		// 			(nArea >= 50 && ratio >= 2.2))
		// 		{
		if (
			ratio >= fAxis_Ratio ||
			area_ratio < fArea_Ratio ||
			nArea <= nArea_Min || nArea >= nArea_Max ||
			(nJudge1_Flag > 0 && nArea >= nJudge1_Area_Min && ratio >= fJudge1_Axis_Ratio))
		{
			rect_count--;
			// 0GV 로 초기화 후, 넘어가기
			cv::Mat matTempROI = DstImg(rectTemp_Dust);
			cv::Mat matLabelROI = matLabels(rectTemp_Dust);

			for (int y = 0; y < rectTemp_Dust.height; y++)
			{
				int		*ptrLabel = (int *)matLabelROI.ptr(y);
				uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

				for (int x = 0; x < rectTemp_Dust.width; x++, ptrLabel++, ptrGray++)
				{
					// 해당 객체
					if (*ptrLabel == idx)	*ptrGray = 0;
				}
			}

			continue;
		}

	}

	return numOfLables;
}
// R, G, B 영상 검출 알고리즘
long CInspectPoint::LogicStart_RGB(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
	int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	double	dDarkDist				= dPara[E_PARA_POINT_RGB_COMMON_DARK_DIST];

	long	nDelLineBrightCntX		= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_CNT_X];		// 라인 삭제 x 방향 개수
	long	nDelLineBrightCntY		= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_CNT_Y];		// 라인 삭제 y 방향 개수
	long	nDelLineBrightThickX	= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_THICK_X];	// 라인 삭제 x 두께
	long	nDelLineBrightThickY	= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_THICK_Y];	// 라인 삭제 y 두께

	long	nDelLineDarkCntX		= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_CNT_X];		// 라인 삭제 x 방향 개수
	long	nDelLineDarkCntY		= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_CNT_Y];		// 라인 삭제 y 방향 개수
	long	nDelLineDarkThickX		= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_THICK_X];		// 라인 삭제 x 두께
	long	nDelLineDarkThickY		= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_THICK_Y];		// 라인 삭제 y 두께

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	bool	bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	int&	nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	// 검사 영역 외곽 축소
	CRect rectTemp(rectROI);

	int nOffSet = 100;

	cv::Rect rtInspROI;
	//rtInspROI.x = rectTemp.left - nOffSet;
	//rtInspROI.y = rectTemp.top - nOffSet;
	//rtInspROI.width = rectTemp.Width() + nOffSet * 2;
	//rtInspROI.height = rectTemp.Height() + nOffSet * 2;

	Insp_RectSet(rtInspROI, rectTemp, matSrcImage.cols, matSrcImage.rows, nOffSet);

	long	nWidth	= (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcImage.rows;	// 영상 세로 크기

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	// 범위 이진화
	nErrorCode = RangeAvgThreshold(matSrcImage, matDstImage, rectTemp, dPara, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("RangeAvgThreshold."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Dark_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

 	// Distance Transform & Threshold
	nErrorCode = DistanceTransformThreshold(matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), dDarkDist, cMem[0]);
 	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;
	
 	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DistanceTransformThreshold."));
 
 	// 중간 Image 확인
 	if( bImageSave )
 	{
 		CString strTemp;
 		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Dark_Dist.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
 		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
 
 		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Dist.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
 		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
 	}
	
	// Bright 작은 면적 불량 제거
	// Blob 시 속도 느림.
	nErrorCode = DeleteArea(matDstImage[E_DEFECT_COLOR_BRIGHT](rtInspROI), 3, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DeleteArea (BRIGHT)."));

	// Dark 작은 면적 불량 제거
	// Blob 시 속도 느림.
	nErrorCode = DeleteArea(matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), 0, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DeleteArea (DARK)."));

 	// line 불량 삭제하기
	if (nDelLineBrightCntX > 0 || nDelLineBrightCntY > 0)
 		ProjectionLineDelete(matDstImage[E_DEFECT_COLOR_BRIGHT], nDelLineBrightCntX, nDelLineBrightCntY, nDelLineBrightThickX, nDelLineBrightThickY);
	if (nDelLineDarkCntX > 0 || nDelLineDarkCntY > 0)
 		ProjectionLineDelete(matDstImage[E_DEFECT_COLOR_DARK], nDelLineDarkCntX, nDelLineDarkCntY, nDelLineDarkThickX, nDelLineDarkThickY);
 
 	// 불량 붙이기
 	//Morphology(matDstImage[E_DEFECT_COLOR_DARK],	matDstImage[E_DEFECT_COLOR_DARK],	5, 5, E_MORP_CLOSE, cMem[0]);
 	//Morphology(matDstImage[E_DEFECT_COLOR_BRIGHT],	matDstImage[E_DEFECT_COLOR_BRIGHT],	5, 5, E_MORP_CLOSE, cMem[0]);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Projection."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Dark_Delete.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Delete.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	return E_ERROR_CODE_TRUE;
}

// PNZ RGB Pattern 명점 미검 Test Alg.
long CInspectPoint::LogicStart_RGBTest(cv::Mat **matSrcBufferRGBAdd, cv::Mat* matDstImage, CRect rectROI, double* dPara,
int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, int Type, cv::Rect* rcCHoleROI, cv::Mat* matCholeBuffer)
{
	// 파라미터 NULL인 경우
	if( dPara == NULL )					return E_ERROR_CODE_EMPTY_PARA;
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	cv::Mat matSrcBufferRGB[3];

	//matSrcBufferRGB[0]	= *matSrcBufferRGBAdd[0];
	//matSrcBufferRGB[1]	= *matSrcBufferRGBAdd[1];
	//matSrcBufferRGB[2]	= *matSrcBufferRGBAdd[2];

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("RGB Buff Copy Start."));

 	// RGB 복사할때까지
 	int TimeCount = 0;
 	//while(	matSrcBufferRGB[0].empty()	|| matSrcBufferRGB[1].empty()	||	matSrcBufferRGB[2].empty()	)
	while (matSrcBufferRGBAdd[0]->empty() || matSrcBufferRGBAdd[1]->empty() || matSrcBufferRGBAdd[2]->empty())
 	{
 		TimeCount++;
 		Sleep(10);
 
 		// KSW 18.05.30
 		if (TimeCount >= 1000)
 			return E_ERROR_CODE_FALSE;
 	}

	matSrcBufferRGB[0] = *matSrcBufferRGBAdd[0];
	matSrcBufferRGB[1] = *matSrcBufferRGBAdd[1];
	matSrcBufferRGB[2] = *matSrcBufferRGBAdd[2];

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("RGB Buff Copy End."));

	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;
	
	// Parameter
	double	fApplyWeight		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_APPLYGV_WEIGHT];

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	bool	bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	int&	nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];


	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem[1]);

	// 검사 영역 외곽 축소
	CRect rectTemp(rectROI);

	int nOffSet = 10;

	cv::Rect rtInspROI;

	//rtInspROI.x = rectROI.left - nOffSet;
	//rtInspROI.y = rectROI.top - nOffSet;
	//rtInspROI.width = rectROI.Width() + nOffSet * 2;
	//rtInspROI.height = rectROI.Height() + nOffSet * 2;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	if ( Type == E_IMAGE_CLASSIFY_AVI_R && !matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].empty())
	{
		Insp_RectSet(rtInspROI, rectTemp, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].cols, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].rows, nOffSet);
		//Red Pattern Temp 만들기
		cv::Mat mat_Org			= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].type(), false);
		cv::Mat matTemp_1		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].type(), false);
		cv::Mat matTemp_2		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].type(), false);
		cv::Mat matTemp_3		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].type(), false);
		cv::Mat matTemp16_1		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].size(), CV_16UC1, false);
		cv::Mat matTemp16_2		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].size(), CV_16UC1, false);

		matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].copyTo(mat_Org);
		
		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Red Pattern MatCreate."));

		// Apply Enhancement
		ApplyEnhancement(mat_Org, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G], matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B], matTemp_1, matTemp_3, dPara, nCommonPara, strAlgPath, E_IMAGE_CLASSIFY_AVI_R, &cMatBufTemp);
		
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Red Pattern_ApplyEhance.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_1);
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Red Pattern_EnhancedOrg.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_3);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Red Pattern ApplyEnhancement."));

		cv::Mat tempMean, tempStd;
		cv::meanStdDev(mat_Org, tempMean, tempStd);

		double Meanvalue = tempMean.at<double>(0,0);	// 평균
		double Stdvalue  = tempStd.at<double>(0,0);		// 표준 편차

		ApplyMeanGV(matTemp_1,	Meanvalue*fApplyWeight);

		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Red Pattern_ApplyGV.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_1);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Red Pattern ApplyMeanGV."));

		//Sub 영상 만들기
		cv::blur(matTemp_3(rtInspROI), mat_Org(rtInspROI),	cv::Size(3, 3));
		cv::blur(matTemp_1(rtInspROI), matTemp_2(rtInspROI),	cv::Size(3, 3));

		matTemp_3(rtInspROI) = mat_Org(rtInspROI) - matTemp_2(rtInspROI);
		////////////////////////////////////////////////////////////////////////// choi 06.08
		for (int j = 0; j < MAX_MEM_SIZE_E_INSPECT_AREA; j++)
		{
			if (!matCholeBuffer[j].empty()) {
				cv::Mat matChole_Temp = cMatBufTemp.GetMat(matCholeBuffer[j].size() ,matCholeBuffer[j].type(),false);
				Scalar scMean, scStdev;
				matCholeBuffer[j].copyTo(matChole_Temp);
				cv::meanStdDev(matTemp_3(rtInspROI), scMean, scStdev);
				for (int i = 0; i < matChole_Temp.rows*matChole_Temp.cols; i++) {
					if (matChole_Temp.data[i] == 255) {
						matChole_Temp.data[i] = scMean[0];
					}
				}
				cv::Mat matTempRoi = matTemp_3(rcCHoleROI[j]);
				cv::max(matTempRoi, matChole_Temp, matTempRoi);
				matChole_Temp.release();
				matTempRoi.release();
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Red Pattern_Sub.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_3);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Red Pattern Sub."));

		matTemp_3.convertTo(matTemp16_1, CV_16UC1);
		
		Pow(matTemp16_1(rtInspROI), matTemp16_2(rtInspROI), 2.0, &cMatBufTemp);

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Red Pattern_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp16_2);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Red Pattern Pow."));

		// 범위 이진화
		nErrorCode = RangeAvgThreshold_RGB(matTemp16_2, matDstImage, rectTemp, dPara, &cMatBufTemp);
		if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Red Pattern_B_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);

			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Red Pattern_D_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Red Pattern Process."));
	}

	if ( Type == E_IMAGE_CLASSIFY_AVI_G && !matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].empty())
	{
		Insp_RectSet(rtInspROI, rectTemp, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].cols, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].rows, nOffSet);
		//Green Pattern Temp 만들기
		cv::Mat mat_Org			= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].type(), false);
		cv::Mat matTemp_1		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].type(), false);
		cv::Mat matTemp_2		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].type(), false);
		cv::Mat matTemp_3		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].type(), false);
		cv::Mat matTemp16_1		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].size(), CV_16UC1, false);
		cv::Mat matTemp16_2		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].size(), CV_16UC1, false);

		matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].copyTo(mat_Org);
		
		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Green Pattern MatCreate."));

		// Apply Enhancement
		ApplyEnhancement(mat_Org, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R], matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B], matTemp_1, matTemp_3, dPara, nCommonPara, strAlgPath, E_IMAGE_CLASSIFY_AVI_G, &cMatBufTemp);

		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Green Pattern_ApplyEhance.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_1);
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Green Pattern_EnhancedOrg.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_3);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Green Pattern ApplyEnhancement."));

		cv::Mat tempMean, tempStd;
		cv::meanStdDev(mat_Org, tempMean, tempStd);

		double Meanvalue = tempMean.at<double>(0,0);	// 평균
		double Stdvalue  = tempStd.at<double>(0,0);		// 표준 편차

		ApplyMeanGV(matTemp_1,	Meanvalue*fApplyWeight);

		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Green Pattern_ApplyGV.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_1);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Green Pattern ApplyMeanGV."));

		//Sub 영상 만들기
		//Sub 영상 만들기
		cv::blur(matTemp_3(rtInspROI), mat_Org(rtInspROI),	cv::Size(3,3));
		cv::blur(matTemp_1(rtInspROI), matTemp_2(rtInspROI),	cv::Size(3,3));

		matTemp_3(rtInspROI) = mat_Org(rtInspROI) - matTemp_2(rtInspROI);
		//////////////////////////////////////////////////////////////////////////
		for (int j = 0; j < MAX_MEM_SIZE_E_INSPECT_AREA; j++)
		{
			if (!matCholeBuffer[j].empty()) {
				cv::Mat matChole_Temp = cMatBufTemp.GetMat(matCholeBuffer[j].size(), matCholeBuffer[j].type(), false);
				Scalar scMean, scStdev;
				matCholeBuffer[j].copyTo(matChole_Temp);
				cv::meanStdDev(matTemp_3(rtInspROI), scMean, scStdev);
				for (int i = 0; i < matChole_Temp.rows*matChole_Temp.cols; i++) {
					if (matChole_Temp.data[i] == 255) {
						matChole_Temp.data[i] = scMean[0];
					}
				}
				cv::Mat matTempRoi = matTemp_3(rcCHoleROI[j]);
				cv::max(matTempRoi, matChole_Temp, matTempRoi);
				matChole_Temp.release();
				matTempRoi.release();
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Green Pattern_Sub.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_3);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Green Pattern Sub."));

		matTemp_3.convertTo(matTemp16_1, CV_16UC1);

		Pow(matTemp16_1(rtInspROI), matTemp16_2(rtInspROI), 2.0, &cMatBufTemp);

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Green Pattern_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp16_2);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Green Pattern Pow."));

		// 범위 이진화
		nErrorCode = RangeAvgThreshold_RGB(matTemp16_2, matDstImage, rectTemp, dPara, &cMatBufTemp);
		if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;



		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Green Pattern_B_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);

			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Green Pattern_D_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Green Pattern Process."));
	}

	if ( Type == E_IMAGE_CLASSIFY_AVI_B && !matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].empty())
	{
		Insp_RectSet(rtInspROI, rectTemp, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].cols, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].rows, nOffSet);
		//Blue Pattern Temp 만들기
		cv::Mat mat_Org			= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].type(), false);
		cv::Mat matTemp_1		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].type(), false);
		cv::Mat matTemp_2		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].type(), false);
		cv::Mat matTemp_3		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].size(), matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].type(), false);
		cv::Mat matTemp16_1		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].size(), CV_16UC1, false);
		cv::Mat matTemp16_2		= cMatBufTemp.GetMat(matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].size(), CV_16UC1, false);

		matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].copyTo(mat_Org);
		
		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Blue Pattern MatCreate."));
		
		// Apply Enhancement
		ApplyEnhancement(mat_Org, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R], matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G], matTemp_1, matTemp_3, dPara, nCommonPara, strAlgPath, E_IMAGE_CLASSIFY_AVI_B, &cMatBufTemp);

		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Blue Pattern_ApplyEhance.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_1);
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Blue Pattern_EnhancedOrg.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_3);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Blue Pattern ApplyEnhancement."));

		cv::Mat tempMean, tempStd;
		cv::meanStdDev(mat_Org, tempMean, tempStd);

		double Meanvalue = tempMean.at<double>(0,0);	// 평균
		double Stdvalue  = tempStd.at<double>(0,0);		// 표준 편차

		ApplyMeanGV(matTemp_1,	Meanvalue*fApplyWeight);

		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Blue Pattern_ApplyGV.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_1);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Blue Pattern ApplyMeanGV."));

		//Sub 영상 만들기
		cv::blur(matTemp_3(rtInspROI), mat_Org(rtInspROI),	cv::Size(3,3));
		cv::blur(matTemp_1(rtInspROI), matTemp_2(rtInspROI),	cv::Size(3,3));

		matTemp_3(rtInspROI) = mat_Org(rtInspROI) - matTemp_2(rtInspROI);

		//cv::GaussianBlur(matTemp_2,	matTemp_1, cv::Size(5, 5), 3);
		//////////////////////////////////////////////////////////////////////////
		for (int j = 0; j < MAX_MEM_SIZE_E_INSPECT_AREA; j++)
		{
			if (!matCholeBuffer[j].empty()) {
				cv::Mat matChole_Temp = cMatBufTemp.GetMat(matCholeBuffer[j].size(), matCholeBuffer[j].type(), false);
				Scalar scMean, scStdev;
				matCholeBuffer[j].copyTo(matChole_Temp);
				cv::meanStdDev(matTemp_3(rtInspROI), scMean, scStdev);
				for (int i = 0; i < matChole_Temp.rows*matChole_Temp.cols; i++) {
					if (matChole_Temp.data[i] == 255) {
						matChole_Temp.data[i] = scMean[0];
					}
				}
				cv::Mat matTempRoi = matTemp_3(rcCHoleROI[j]);
				cv::max(matTempRoi, matChole_Temp, matTempRoi);
				matChole_Temp.release();
				matTempRoi.release();
			}
		}
		//////////////////////////////////////////////////////////////////////////

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Blue Pattern_Sub.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp_3);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Blue Pattern Sub."));

		matTemp_3.convertTo(matTemp16_1, CV_16UC1);

		Pow(matTemp16_1(rtInspROI), matTemp16_2(rtInspROI), 2.0, &cMatBufTemp);

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Blue Pattern_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTemp16_2);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Blue Pattern Pow."));

		// 범위 이진화
		nErrorCode = RangeAvgThreshold_RGB(matTemp16_2, matDstImage, rectTemp, dPara, &cMatBufTemp);
		if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

		
		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Blue Pattern_B_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);

			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Blue Pattern_D_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
		}

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Blue Pattern Process."));
	}
	
// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}


	return E_ERROR_CODE_TRUE;
}



// Black 패턴 검출 알고리즘
long CInspectPoint::LogicStart_Black(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
	int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	long	nBlur1					= (long)dPara[E_PARA_POINT_BLACK_ACTIVE_BLUR_1];				// Blur1
	long	nBlur2					= (long)dPara[E_PARA_POINT_BLACK_ACTIVE_BLUR_2];				// Blur2
	long	nThresholdBright		= (long)dPara[E_PARA_POINT_BLACK_ACTIVE_THRESHOLD];

	long	nDelLineBrightCntX		= (long)dPara[E_PARA_POINT_BLACK_DEL_LINE_BRIGHT_CNT_X];		// 라인 삭제 x 방향 개수
	long	nDelLineBrightCntY		= (long)dPara[E_PARA_POINT_BLACK_DEL_LINE_BRIGHT_CNT_Y];		// 라인 삭제 y 방향 개수
	long	nDelLineBrightThickX	= (long)dPara[E_PARA_POINT_BLACK_DEL_LINE_BRIGHT_THICK_X];		// 라인 삭제 x 두께
	long	nDelLineBrightThickY	= (long)dPara[E_PARA_POINT_BLACK_DEL_LINE_BRIGHT_THICK_Y];		// 라인 삭제 y 두께

	long	nBigFlag				= (long)dPara[E_PARA_POINT_BLACK_BIG_FLAG];						// 큰불량 & 낮은 GV 불량 검출 사용 유/무
	long	nBigMinArea				= (long)dPara[E_PARA_POINT_BLACK_BIG_MIN_AREA];					// 큰 불량의 최소 면적
	long	nBigMaxGV				= (long)dPara[E_PARA_POINT_BLACK_BIG_MAX_GV];					// 낮은 GV의 최대 GV

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	bool	bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	int&	nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	int nOffSet = 50;

	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nOffSet;
	//rtInspROI.y = rectROI.top - nOffSet;
	//rtInspROI.width = rectROI.Width() + nOffSet * 2;
	//rtInspROI.height = rectROI.Height() + nOffSet * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nOffSet);

	// Blur
	cv::Mat matTempBuf1, matTempBuf2;
	matTempBuf1 = cMem[0]->GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matTempBuf2 = cMem[0]->GetMat(matSrcImage.size(), matSrcImage.type(), false);

	cv::blur(matSrcImage(rtInspROI), matTempBuf1(rtInspROI), cv::Size(nBlur1, nBlur1));
	cv::blur(matTempBuf1(rtInspROI), matTempBuf2(rtInspROI), cv::Size(nBlur2, nBlur2));

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("blur."));

	cv::subtract(matTempBuf1, matTempBuf2, matTempBuf1);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("subtract."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Sub.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf1);
	}

	// Threshold
	cv::Mat matROI1 = matTempBuf1( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matROI2 = matDstImage[E_DEFECT_COLOR_BRIGHT]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Mat Create."));

	// Threshold
	nErrorCode = Binary(matROI1, matROI2, nThresholdBright, false, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("threshold."));

	// line 불량 삭제하기
	if(nDelLineBrightCntX > 0 || nDelLineBrightCntY > 0)
		ProjectionLineDelete(matDstImage[E_DEFECT_COLOR_BRIGHT], nDelLineBrightCntX, nDelLineBrightCntY, nDelLineBrightThickX, nDelLineBrightThickY);

 	// 불량 붙이기
 	//Morphology(matDstImage[E_DEFECT_COLOR_BRIGHT],	matDstImage[E_DEFECT_COLOR_BRIGHT],	5, 5, E_MORP_CLOSE, cMem[0]);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Projection."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Delete.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	//////////////////////////////////////////////////////////////////////////
	// 영역이 넓으면서, GV값이 낮은 경우 찾기
	//////////////////////////////////////////////////////////////////////////
	if ( nBigFlag > 0 )
	{
		// Threshold
		nErrorCode = Binary(matSrcImage(rtInspROI), matTempBuf2(rtInspROI), 0, false, cMem[0]);
		if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

		// 불량 붙이기
		nErrorCode = Morphology(matTempBuf2(rtInspROI), matTempBuf1(rtInspROI), 5, 5, E_MORP_CLOSE);
		if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Big_Point.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTempBuf1);
		}
		
		nErrorCode = DeleteMinArea(matSrcImage(rtInspROI), matTempBuf1(rtInspROI), nBigMinArea, nBigMaxGV, cMem[0]);
		if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

		cv::bitwise_or(matTempBuf1, matDstImage[E_DEFECT_COLOR_BRIGHT], matDstImage[E_DEFECT_COLOR_BRIGHT]);

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Big_Point_MinArea.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matTempBuf1);

			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Big_Point_or.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
		}
	}
	//////////////////////////////////////////////////////////////////////////

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Find Big Area."));


	matROI1.release();
	matROI2.release();
	matTempBuf1.release();
	matTempBuf2.release();

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Release."));

	return E_ERROR_CODE_TRUE;
}

// Gray 패턴 검출 알고리즘
long CInspectPoint::LogicStart_Gray(cv::Mat& matSrcImage,cv::Mat **matSrcBufferRGBAdd,cv::Mat matBKBuffer, cv::Mat* matDstImage, CRect rectROI, double* dPara,
	int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Rect* rcCHoleROI, cv::Mat* matCholeBuffer)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;
	
	double	dPow							= dPara[E_PARA_POINT_RGB_COMMON_POW];
	double	dDarkDist						= dPara[E_PARA_POINT_RGB_COMMON_DARK_DIST];

	long	nDelLineBrightCntX				= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_CNT_X];		// 라인 삭제 x 방향 개수
	long	nDelLineBrightCntY				= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_CNT_Y];		// 라인 삭제 y 방향 개수
	long	nDelLineBrightThickX			= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_THICK_X];	// 라인 삭제 x 두께
	long	nDelLineBrightThickY			= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_THICK_Y];	// 라인 삭제 y 두께

	long	nDelLineDarkCntX				= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_CNT_X];		// 라인 삭제 x 방향 개수
	long	nDelLineDarkCntY				= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_CNT_Y];		// 라인 삭제 y 방향 개수
	long	nDelLineDarkThickX				= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_THICK_X];		// 라인 삭제 x 두께
	long	nDelLineDarkThickY				= (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_THICK_Y];		// 라인 삭제 y 두께

	double	bAdjustGrayR_AdjustRatio		= (double)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_R_ADJUST_RATIO];
	int		bAdjustGrayR_AdjustCutMinGV		= (int)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_R_CUT_MINGV];

	double	bAdjustGrayG_AdjustRatio		= (double)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_G_ADJUST_RATIO];
	int		bAdjustGrayG_AdjustCutMinGV		= (int)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_G_CUT_MINGV];

	double	bAdjustGrayB_AdjustRatio		= (double)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_B_ADJUST_RATIO];
	int		bAdjustGrayB_AdjustCutMinGV		= (int)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_B_CUT_MINGV];

	bool	bAdjustGrayR_Use				= ( dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_R_USE] > 0 ) ? true : false;
	bool	bAdjustGrayG_Use				= ( dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_G_USE] > 0 ) ? true : false;
	bool	bAdjustGrayB_Use				= ( dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_B_USE] > 0 ) ? true : false;


	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	bool	bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	int&	nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	int		nRatio				=  nCommonPara[E_PARA_COMMON_PS_MODE				];
	
	// 검사 영역 외곽 축소
	CRect rectTemp(rectROI);

	long	nWidth	= (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcImage.rows;	// 영상 세로 크기

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}
	
	cv::Mat matSrcBufferRGB[3];

	matSrcBufferRGB[0] = *matSrcBufferRGBAdd[0];
	matSrcBufferRGB[1] = *matSrcBufferRGBAdd[1];
	matSrcBufferRGB[2] = *matSrcBufferRGBAdd[2];
		

	// Adjust기능을 사용 안할때를 대비해 미리 복사
	cv::Mat matAdjustTempBuf = cMem[0]->GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matSrcImage.copyTo(matAdjustTempBuf);

	CRect rectTemp_For_Adj;
	rectTemp_For_Adj.left = 0;
	rectTemp_For_Adj.top = 0;
	rectTemp_For_Adj.right = nWidth;
	rectTemp_For_Adj.bottom = nHeight;

	//보정 전후 밝기가 약간 달라짐으로 외곽 영역도 달라진 만큼 보정치 추가
	double dblAve_BEF = GetAverage(matAdjustTempBuf);
	double dblAve_AFT, dblAve_ADJ_Ratio;
	
	// R 패턴 보정기능 사용
	if(bAdjustGrayR_Use && !matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].empty() )
	{
		AdjustImageWithRGB(matAdjustTempBuf, matAdjustTempBuf, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R], bAdjustGrayR_AdjustRatio, bAdjustGrayR_AdjustCutMinGV, rectTemp_For_Adj, cMem[0]);

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Adjust R."));

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Adjust_R.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matAdjustTempBuf);
		}
	}	

	// G 패턴 보정기능 사용
	if(bAdjustGrayG_Use && !matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].empty() )
	{
		AdjustImageWithRGB(matAdjustTempBuf, matAdjustTempBuf, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G], bAdjustGrayG_AdjustRatio, bAdjustGrayG_AdjustCutMinGV, rectTemp_For_Adj, cMem[0]);

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Adjust G."));

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Adjust_G.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matAdjustTempBuf);
		}
	}

	// B 패턴 보정기능 사용
	if(bAdjustGrayB_Use && !matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].empty() )
	{
		AdjustImageWithRGB(matAdjustTempBuf, matAdjustTempBuf, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B], bAdjustGrayB_AdjustRatio, bAdjustGrayB_AdjustCutMinGV, rectTemp_For_Adj, cMem[0]);

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Adjust B."));

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Adjust_B.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matAdjustTempBuf);
		}
	}

	//보정 전후 밝기가 약간 달라짐으로 외곽 영역도 달라진 만큼 보정치 추가
	dblAve_AFT = GetAverage(matAdjustTempBuf);
	dblAve_ADJ_Ratio = dblAve_AFT / dblAve_BEF;

	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// choi 06.08
	for (int j = 0; j < MAX_MEM_SIZE_E_INSPECT_AREA; j++)
	{
		if (!matBKBuffer.empty() && !matCholeBuffer[j].empty()) {
			cv::Mat matChole_Temp = cMem[0]->GetMat(matCholeBuffer[j].size(), matCholeBuffer[j].type(), false);
			Scalar scMean, scStdev;
			matCholeBuffer[j].copyTo(matChole_Temp);
			cv::Mat matTempRoi = matBKBuffer(rcCHoleROI[j]);
			cv::add(matTempRoi, matChole_Temp, matTempRoi);

			matChole_Temp.release();
			matTempRoi.release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//외곽 영역의 GV 를 맞추어 준다.
	AdjustBkGV(matAdjustTempBuf, matBKBuffer, dblAve_ADJ_Ratio, cMem[0]);

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_AdjustBK.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matAdjustTempBuf);
	}

	int nOffSet = 100;

	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nOffSet;
	//rtInspROI.y = rectROI.top - nOffSet;
	//rtInspROI.width = rectROI.Width() + nOffSet * 2;
	//rtInspROI.height = rectROI.Height() + nOffSet * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nOffSet);

	// KSW 17.09.05 - Pow 추가
	cv::Mat matTempBuf = cMem[0]->GetMat(matSrcImage.size(), CV_16U);
	nErrorCode = Pow(matAdjustTempBuf(rtInspROI), matTempBuf(rtInspROI), dPow, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Pow."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf);
	}
	
	// 범위 이진화
	nErrorCode = RangeAvgThreshold(matTempBuf, matDstImage, rectTemp, dPara, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	matTempBuf.release();

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("RangeAvgThreshold."));

	nOffSet = 10;

	//rtInspROI.x = rectROI.left - nOffSet;
	//rtInspROI.y = rectROI.top - nOffSet;
	//rtInspROI.width = rectROI.Width() + nOffSet * 2;
	//rtInspROI.height = rectROI.Height() + nOffSet * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nOffSet);

 	// Distance Transform & Threshold
 	nErrorCode = DistanceTransformThreshold(matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), dDarkDist, cMem[0]);
 	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;
 
 	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DistanceTransformThreshold."));
 
 	// 중간 Image 확인
 	if( bImageSave )
 	{
 		CString strTemp;
 		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Dark_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
 		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
 
 		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
 		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
 	}

	//writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DistanceTransformThreshold Off"));

	// Bright 작은 면적 불량 제거
	// Blob 시 속도 느림.
	// KSW 17.12.01 - 기포 명점 제거 시, White 패턴에서 검출해서 비교해야 함
	// P/S 모드가 아닌 경우, White 패턴에서 1Pixel로 검출되는 경우도 존재
	if( nRatio==2 || nImageNum != E_IMAGE_CLASSIFY_AVI_WHITE )
		nErrorCode = DeleteArea(matDstImage[E_DEFECT_COLOR_BRIGHT](rtInspROI), 2, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DeleteArea (BRIGHT)."));

	// Dark 작은 면적 불량 제거
	// Blob 시 속도 느림.
	nErrorCode = DeleteArea(matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), 2, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DeleteArea (DARK)."));

	// line 불량 삭제하기
	if(nDelLineBrightCntX > 0 || nDelLineBrightCntY > 0)
		ProjectionLineDelete(matDstImage[E_DEFECT_COLOR_BRIGHT], nDelLineBrightCntX, nDelLineBrightCntY, nDelLineBrightThickX, nDelLineBrightThickY);
	if (nDelLineDarkCntX > 0 || nDelLineDarkCntY > 0)
		ProjectionLineDelete(matDstImage[E_DEFECT_COLOR_DARK], nDelLineDarkCntX, nDelLineDarkCntY, nDelLineDarkThickX, nDelLineDarkThickY);
 
 	// 불량 붙이기
 	//Morphology(matDstImage[E_DEFECT_COLOR_DARK],	matDstImage[E_DEFECT_COLOR_DARK],	5, 5, E_MORP_CLOSE, cMem[0]);
 	//Morphology(matDstImage[E_DEFECT_COLOR_BRIGHT],	matDstImage[E_DEFECT_COLOR_BRIGHT],	5, 5, E_MORP_CLOSE, cMem[0]);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Projection."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Dark_Delete.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Delete.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	return E_ERROR_CODE_TRUE;
}

//외곽 영역의 GV 를 맞추어 준다.
void CInspectPoint::AdjustBkGV(cv::Mat matOri, cv::Mat matBKBuffer, double dblRatio, CMatBuf* cMemSub)
{
	CMatBuf cMatBufTemp;
	cv::Mat matImagePart, matImagePartBK, matBkPart;

	// 버퍼 할당 & 초기화
	cMatBufTemp.SetMem(cMemSub);

	matImagePart = cMatBufTemp.GetMat(matOri.size(), matOri.type(), false);
	matImagePartBK = cMatBufTemp.GetMat(matBKBuffer.size(), matBKBuffer.type(), false);
	matBkPart = cMatBufTemp.GetMat(matOri.size(), matOri.type(), false);


	// 반전
	cv::bitwise_not(matBKBuffer, matImagePartBK);

	// 이미지 영역
	cv::bitwise_and(matOri, matImagePartBK, matImagePart);

	// BK 영역
	cv::bitwise_and(matOri, matBKBuffer, matBkPart);

// 	cv::imwrite("C:\\Users\\Vision_1\\Desktop\\bitwise_part.jpg", matImagePart);
// 		cv::imwrite("C:\\Users\\Vision_1\\Desktop\\bitwisebk.jpg", matBkPart);
// 		cv::imwrite("C:\\Users\\Vision_1\\Desktop\\bitwise_matImagePartBK.jpg", matImagePartBK);
	


	//가중치를 준다.
	matBkPart *= dblRatio;

	//두이미지를 더한다.
	matImagePart += matBkPart;

	//원본이미지에 보정치를 준 이미지를 복사
	matImagePart.copyTo(matOri);

	matImagePart.release();
	matImagePartBK.release();
	matBkPart.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}
}

// Gray 패턴 검출 알고리즘
long CInspectPoint::LogicStart_Gray2(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGBAdd, cv::Mat* matDstImage, CRect rectROI, double* dPara,
	int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	double	dPow = dPara[E_PARA_POINT_RGB_COMMON_POW];
	double	dDarkDist = dPara[E_PARA_POINT_RGB_COMMON_DARK_DIST];

	long	nDelLineBrightCntX = (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_CNT_X];		// 라인 삭제 x 방향 개수
	long	nDelLineBrightCntY = (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_CNT_Y];		// 라인 삭제 y 방향 개수
	long	nDelLineBrightThickX = (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_THICK_X];	// 라인 삭제 x 두께
	long	nDelLineBrightThickY = (long)dPara[E_PARA_POINT_RGB_DEL_LINE_BRIGHT_THICK_Y];	// 라인 삭제 y 두께

	long	nDelLineDarkCntX = (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_CNT_X];		// 라인 삭제 x 방향 개수
	long	nDelLineDarkCntY = (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_CNT_Y];		// 라인 삭제 y 방향 개수
	long	nDelLineDarkThickX = (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_THICK_X];		// 라인 삭제 x 두께
	long	nDelLineDarkThickY = (long)dPara[E_PARA_POINT_RGB_DEL_LINE_DARK_THICK_Y];		// 라인 삭제 y 두께

	double	bAdjustGrayR_AdjustRatio = (double)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_R_ADJUST_RATIO];
	int		bAdjustGrayR_AdjustCutMinGV = (int)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_R_CUT_MINGV];

	double	bAdjustGrayG_AdjustRatio = (double)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_G_ADJUST_RATIO];
	int		bAdjustGrayG_AdjustCutMinGV = (int)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_G_CUT_MINGV];

	double	bAdjustGrayB_AdjustRatio = (double)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_B_ADJUST_RATIO];
	int		bAdjustGrayB_AdjustCutMinGV = (int)dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_B_CUT_MINGV];

	bool	bAdjustGrayR_Use = (dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_R_USE] > 0) ? true : false;
	bool	bAdjustGrayG_Use = (dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_G_USE] > 0) ? true : false;
	bool	bAdjustGrayB_Use = (dPara[E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_B_USE] > 0) ? true : false;


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
	int		nRatio = nCommonPara[E_PARA_COMMON_PS_MODE];

	// 검사 영역 외곽 축소
	CRect rectTemp(rectROI);

	long	nWidth = (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcImage.rows;	// 영상 세로 크기

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// 중간 Image 확인
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	cv::Mat matSrcBufferRGB[3];

	matSrcBufferRGB[0] = *matSrcBufferRGBAdd[0];
	matSrcBufferRGB[1] = *matSrcBufferRGBAdd[1];
	matSrcBufferRGB[2] = *matSrcBufferRGBAdd[2];


	// Adjust기능을 사용 안할때를 대비해 미리 복사
	cv::Mat matAdjustTempBuf = cMem[0]->GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matSrcImage.copyTo(matAdjustTempBuf);




	///////////////////////////////약시인성 test////////////////////////////////////////////////////////////////////////////////////

	cv::Mat weak_test_ori;
	cv::Mat weak_test_R;
	cv::Mat weak_test_G;
	cv::Mat weak_test_B;
	matSrcImage.copyTo(weak_test_ori);
	matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].copyTo(weak_test_R);
	matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].copyTo(weak_test_G);
	matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].copyTo(weak_test_B);


	// subtract gray에서 R/G/B 구하기



	// 	for (int i= rectROI.TopLeft().y; i< rectROI.BottomRight().y; i++)
	// 	{
	// 		for (int j= rectROI.TopLeft().x; j< rectROI.BottomRight().x; j++)
	// 		{
	// 			weak_test_ori.data[i*matSrcImage.cols + j]= 255;
	// 		}
	// 	}

	// 	if (bImageSave)
	// 	{
	// 		CString strTemp;
	// 		strTemp.Format(_T("%s\\1_ori.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
	// 		ImageSave(strTemp, weak_test_ori);
	// 
	// 		strTemp.Format(_T("%s\\1_R.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
	// 		ImageSave(strTemp, weak_test_R);
	// 	
	// 
	// 		strTemp.Format(_T("%s\\1_G.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
	// 		ImageSave(strTemp, weak_test_G);
	// 	
	// 
	// 		strTemp.Format(_T("%s\\1_B.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
	// 		ImageSave(strTemp, weak_test_B);
	// 	}


	/////////// 1차 threshold 원본에 바로 사용하여 점등영역 걸러내기
	for (int i = 0; i < weak_test_R.rows * weak_test_R.cols; i++)
	{
		if (weak_test_R.data[i] < (double)30.0)
		{
			weak_test_R.data[i] = (double)0.0;
		}

	}

	for (int i = 0; i < weak_test_G.rows * weak_test_G.cols; i++)
	{
		if (weak_test_G.data[i] < (double)25.0)
		{
			weak_test_G.data[i] = (double)0.0;
		}

	}

	for (int i = 0; i < weak_test_B.rows * weak_test_B.cols; i++)
	{
		if (weak_test_B.data[i] < (double)40.0)
		{
			weak_test_B.data[i] = (double)0.0;
		}

	}

	//	cv::threshold(weak_test_G,weak_test_G,30,255, THRESH_BINARY);


	if (bImageSave)
	{
		CString strTemp;

		strTemp.Format(_T("%s\\1_1_pre_threshold_R.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_R);

		strTemp.Format(_T("%s\\1_1_pre_threshold_G.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_G);

		strTemp.Format(_T("%s\\1_1_pre_threshold_B.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_B);

	}






	// R 구하기
	cv::subtract(matSrcImage, weak_test_B, weak_test_ori);
	cv::subtract(weak_test_ori, weak_test_G, weak_test_R);

	// G 구하기
	cv::subtract(matSrcImage, weak_test_R, weak_test_ori);
	cv::subtract(weak_test_ori, weak_test_B, weak_test_G);

	//B 구하기
	cv::subtract(matSrcImage, weak_test_R, weak_test_ori);
	cv::subtract(weak_test_ori, weak_test_G, weak_test_B);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\1_2_subtract_R.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_R);
		strTemp.Format(_T("%s\\1_2_subtract_G.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_G);
		strTemp.Format(_T("%s\\1_2_subtract_B.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_B);
	}





	/////////// 2차 threshold gray에서 추출된 R/G/B 점등영역 추출
	int bright_count = 0;

	for (int i = 0; i<weak_test_R.rows * weak_test_R.cols; i++)
	{
		if (weak_test_R.data[i] < (double)35.0)
		{
			weak_test_R.data[i] = (double)0.0;
		}

	}

	for (int i = 0; i < weak_test_G.rows * weak_test_G.cols; i++)
	{
		if (weak_test_G.data[i] < (double)25.0)
		{
			weak_test_G.data[i] = (double)0.0;
		}

	}

	for (int i = 0; i < weak_test_B.rows * weak_test_B.cols; i++)
	{
		if (weak_test_B.data[i] < (double)40.0)
		{
			weak_test_B.data[i] = (double)0.0;
		}

	}

	//	cv::threshold(weak_test_G,weak_test_G,30,255, THRESH_BINARY);


	if (bImageSave)
	{
		CString strTemp;

		strTemp.Format(_T("%s\\1_3_pre_threshold_R.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_R);

		strTemp.Format(_T("%s\\1_3_pre_threshold_G.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_G);

		strTemp.Format(_T("%s\\1_3_pre_threshold_B.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_B);

	}






	// 영역분할 시작
	int test_width = (rectROI.BottomRight().x - rectROI.TopLeft().x) / 100;
	int test_height = (rectROI.BottomRight().y - rectROI.TopLeft().y) / 100;



	// 	 	for (int i= rectROI.TopLeft().y; i< rectROI.BottomRight().y; i++)
	// 	 	{
	// 	 		for (int j= rectROI.TopLeft().x; j< rectROI.BottomRight().x; j++)
	// 	 		{
	// 				for (int x = 0; x < test_height; x++)
	// 				{
	// 					for (int y = 0; x < test_width; y++)
	// 					{
	// 						weak_test_ori.data[(i+x )*weak_test_ori.cols + (j+y)] = 255;
	// 					}
	// 				}
	// 	 		}
	// 		}


	// 	for (int i = rectROI.TopLeft().y; i < rectROI.BottomRight().y - test_height; i = i + test_height)
	// 	{
	// 		for (int j = rectROI.TopLeft().x; j < rectROI.BottomRight().x - test_width; j = j + test_width)
	// 		{
	// 			for (int x = 0; x < test_height; x++)
	// 			{
	// 				for (int y = 0; y < test_width; y++)
	// 				{
	// 					if (weak_test_ori.data[(x + i)*matSrcImage.cols + (y + j)] <= 50)
	// 						weak_test_ori.data[(x + i)*matSrcImage.cols + (y + j)] = 0;
	// 				}
	// 			}
	// 
	// 		}
	// 	}
	double test_count = 0;
	double test_mean = 0;
	double test_gv_sum = 0;
	int test_hist[256] = { 0 };

	for (int i = rectROI.TopLeft().y; i < rectROI.BottomRight().y - test_height; i = i + test_height)
	{
		for (int j = rectROI.TopLeft().x; j < rectROI.BottomRight().x - test_width; j = j + test_width)
		{

			//////1차 점등영역 걸러내기 -G 패턴
			for (int x = 0; x < test_height; x++)
			{
				for (int y = 0; y < test_width; y++)
				{
					if (weak_test_G.data[(x + i)*matSrcImage.cols + (y + j)] <= 25)
						weak_test_G.data[(x + i)*matSrcImage.cols + (y + j)] = 0;
					else
					{

						test_hist[weak_test_G.data[(x + i)*matSrcImage.cols + (y + j)]]++;
						test_count++;
						test_gv_sum = test_gv_sum + weak_test_G.data[(x + i)*matSrcImage.cols + (y + j)];
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			//choikwangil


			// 			cv::Mat histo_weak;
			// 		    cv::Point2i weak_Point_LT(j, i);
			// 			cv::Point2i weak_Point_RB(j + test_width, i + test_height);
			// 			cv::Rect weak_rect(weak_Point_LT,weak_Point_RB);
			// 
			// 			GetHistogram(weak_test_G(weak_rect), histo_weak);

			int h_count_sum = 0;

			for (int h = 0; h < 256; h++) {
				if (test_hist[h] != 0) {
					h_count_sum += test_hist[h];
				}
			}

			double high_sumGV = 0;
			double high_meanGV = 0;
			int com_count = 0;

			h_count_sum *= 0.1;

			for (int h = 255; h >= 0; h--) {
				if (test_hist[h] != 0) {
					com_count += test_hist[h];
					if (com_count <= h_count_sum) {
						high_sumGV += test_hist[h] * h;
						high_meanGV = high_sumGV / com_count;
					}
					else {
						break;
					}
				}
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////2차 threshold 시작 - 
			/*if (test_count > 0)
			test_mean = (test_gv_sum / test_count) + 20;
			else
			test_mean = 0;*/


			for (int x = 0; x < test_height; x++)
			{
				for (int y = 0; y < test_width; y++)
				{
					if (weak_test_G.data[(x + i) * matSrcImage.cols + (y + j)] <= high_meanGV + 15) //choikwangil 변경 test_mean -> high_meanGV
					{
						weak_test_G.data[(x + i) * matSrcImage.cols + (y + j)] = 0;
					}
					//weak_test_G.data[(x + i) * matSrcImage.cols + (y + j)] = 0;

				}
			}

			test_hist[256] = { 0 };
			test_gv_sum = 0;
			test_count = 0;


			///////////////////////////////////////////////////////////////////
			//////1차 점등영역 걸러내기 - R 패턴//////////////////////////////////
			for (int x = 0; x < test_height; x++)
			{
				for (int y = 0; y < test_width; y++)
				{
					if (weak_test_R.data[(x + i)*matSrcImage.cols + (y + j)] <= 30)
						weak_test_R.data[(x + i)*matSrcImage.cols + (y + j)] = 0;
					else
					{
						test_count++;
						test_gv_sum = test_gv_sum + weak_test_R.data[(x + i)*matSrcImage.cols + (y + j)];
					}
				}
			}

			///////////2차 threshold 시작 - R
			if (test_count > 0)
				test_mean = (test_gv_sum / test_count) + 25;
			else
				test_mean = 0;


			for (int x = 0; x < test_height; x++)
			{
				for (int y = 0; y < test_width; y++)
				{
					if (weak_test_R.data[(x + i) * matSrcImage.cols + (y + j)] <= test_mean)
					{
						weak_test_R.data[(x + i) * matSrcImage.cols + (y + j)] = 0;
					}
					//weak_test_G.data[(x + i) * matSrcImage.cols + (y + j)] = 0;

				}
			}

			test_gv_sum = 0;
			test_count = 0;


			///////////////////////////////////////////////////////////////////
			//////1차 점등영역 걸러내기 - B 패턴//////////////////////////////////
			for (int x = 0; x < test_height; x++)
			{
				for (int y = 0; y < test_width; y++)
				{
					if (weak_test_B.data[(x + i)*matSrcImage.cols + (y + j)] <= 40)
						weak_test_B.data[(x + i)*matSrcImage.cols + (y + j)] = 0;
					else
					{
						test_count++;
						test_gv_sum = test_gv_sum + weak_test_B.data[(x + i)*matSrcImage.cols + (y + j)];
					}
				}
			}

			///////////2차 threshold 시작 - B
			if (test_count > 0)
				test_mean = (test_gv_sum / test_count) + 30;
			else
				test_mean = 0;


			for (int x = 0; x < test_height; x++)
			{
				for (int y = 0; y < test_width; y++)
				{
					if (weak_test_B.data[(x + i) * matSrcImage.cols + (y + j)] <= test_mean)
					{
						weak_test_B.data[(x + i) * matSrcImage.cols + (y + j)] = 0;
					}
					//weak_test_G.data[(x + i) * matSrcImage.cols + (y + j)] = 0;

				}
			}

			test_gv_sum = 0;
			test_count = 0;


		}
	}


	if (bImageSave)
	{
		CString strTemp;

		strTemp.Format(_T("%s\\1_4_range_threshold_G.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_G);
	}

	if (bImageSave)
	{
		CString strTemp;

		strTemp.Format(_T("%s\\1_4_range_threshold_R.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_R);
	}

	if (bImageSave)
	{
		CString strTemp;

		strTemp.Format(_T("%s\\1_4_range_threshold_B.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_B);
	}

	//	cv::add(weak_test_G, weak_test_R, weak_test_G);
	//	cv::add(weak_test_G, weak_test_B, weak_test_G);

	if (bImageSave)
	{
		CString strTemp;

		strTemp.Format(_T("%s\\1_5_final.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, weak_test_G);
	}






	//////////////////////////////////약시인성 test 끝//////////////////////////////////////////////////////////////////////////////////////////////













	// R 패턴 보정기능 사용
	if(bAdjustGrayR_Use && !matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R].empty() )
	{
		AdjustImageWithRGB(matAdjustTempBuf, matAdjustTempBuf, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_R], bAdjustGrayR_AdjustRatio, bAdjustGrayR_AdjustCutMinGV, rectTemp, cMem[0]);

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Adjust R."));

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Adjust_R.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matAdjustTempBuf);
		}
	}	

	// G 패턴 보정기능 사용
	if(bAdjustGrayG_Use && !matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G].empty() )
	{
		AdjustImageWithRGB(matAdjustTempBuf, matAdjustTempBuf, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_G], bAdjustGrayG_AdjustRatio, bAdjustGrayG_AdjustCutMinGV, rectTemp, cMem[0]);

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Adjust G."));

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Adjust_G.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matAdjustTempBuf);
		}
	}

	// B 패턴 보정기능 사용
	if(bAdjustGrayB_Use && !matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B].empty() )
	{
		AdjustImageWithRGB(matAdjustTempBuf, matAdjustTempBuf, matSrcBufferRGB[E_IMAGE_CLASSIFY_AVI_B], bAdjustGrayB_AdjustRatio, bAdjustGrayB_AdjustCutMinGV, rectTemp, cMem[0]);

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Adjust B."));

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Adjust_B.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matAdjustTempBuf);
		}
	}

	int nOffSet = 100;

	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nOffSet;
	//rtInspROI.y = rectROI.top - nOffSet;
	//rtInspROI.width = rectROI.Width() + nOffSet * 2;
	//rtInspROI.height = rectROI.Height() + nOffSet * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nOffSet);

	// KSW 17.09.05 - Pow 추가
	cv::Mat matTempBuf = cMem[0]->GetMat(matSrcImage.size(), CV_16U);
	nErrorCode = Pow(matAdjustTempBuf(rtInspROI), matTempBuf(rtInspROI), dPow, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Pow."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf);
	}
	
	// 범위 이진화
	nErrorCode = RangeAvgThreshold(matTempBuf, matDstImage, rectTemp, dPara, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	matTempBuf.release();

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("RangeAvgThreshold."));

	nOffSet = 10;

	//rtInspROI.x = rectROI.left - nOffSet;
	//rtInspROI.y = rectROI.top - nOffSet;
	//rtInspROI.width = rectROI.Width() + nOffSet * 2;
	//rtInspROI.height = rectROI.Height() + nOffSet * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nOffSet);

 	// Distance Transform & Threshold
 	nErrorCode = DistanceTransformThreshold(matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), dDarkDist, cMem[0]);
 	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;
 
 	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DistanceTransformThreshold."));
 
 	// 중간 Image 확인
 	if( bImageSave )
 	{
 		CString strTemp;
 		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Dark_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
 		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
 
 		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Threshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
 		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
 	}

	//writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DistanceTransformThreshold Off"));

	// Bright 작은 면적 불량 제거
	// Blob 시 속도 느림.
	// KSW 17.12.01 - 기포 명점 제거 시, White 패턴에서 검출해서 비교해야 함
	// P/S 모드가 아닌 경우, White 패턴에서 1Pixel로 검출되는 경우도 존재
	if( nRatio==2 || nImageNum != E_IMAGE_CLASSIFY_AVI_WHITE )
		nErrorCode = DeleteArea(matDstImage[E_DEFECT_COLOR_BRIGHT](rtInspROI), 2, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DeleteArea (BRIGHT)."));

	// Dark 작은 면적 불량 제거
	// Blob 시 속도 느림.
	nErrorCode = DeleteArea(matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), 1, cMem[0]); // PWJ 12.10 원본-> nErrorCode = DeleteArea(matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), 2, cMem[0]);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DeleteArea (DARK)."));

	// line 불량 삭제하기
	if(nDelLineBrightCntX > 0 || nDelLineBrightCntY > 0)
		ProjectionLineDelete(matDstImage[E_DEFECT_COLOR_BRIGHT], nDelLineBrightCntX, nDelLineBrightCntY, nDelLineBrightThickX, nDelLineBrightThickY);
	if (nDelLineDarkCntX > 0 || nDelLineDarkCntY > 0)
		ProjectionLineDelete(matDstImage[E_DEFECT_COLOR_DARK], nDelLineDarkCntX, nDelLineDarkCntY, nDelLineDarkThickX, nDelLineDarkThickY);
 
 	// 불량 붙이기
 	//Morphology(matDstImage[E_DEFECT_COLOR_DARK],	matDstImage[E_DEFECT_COLOR_DARK],	5, 5, E_MORP_CLOSE, cMem[0]);
 	//Morphology(matDstImage[E_DEFECT_COLOR_BRIGHT],	matDstImage[E_DEFECT_COLOR_BRIGHT],	5, 5, E_MORP_CLOSE, cMem[0]);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Projection."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Dark_Delete.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Delete.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	return E_ERROR_CODE_TRUE;
}


// Dust 패턴 검출 알고리즘


long CInspectPoint::LogicStart_DustALL(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, cv::Rect* rcCHoleROI, double* dPara,
	int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{                                                                                                                                                                                                   
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// Dust Parameter
	int		nShiftRanage	= (int)		dPara[E_PARA_POINT_DUST_ENHANCMENT_SHIFT_RANGE		];
	int		nGaussianSize	= (int)		dPara[E_PARA_POINT_DUST_ENHANCMENT_GAUSSIAN_SIZE	];
	float	fGaussianSigma	= (float)	dPara[E_PARA_POINT_DUST_ENHANCMENT_GAUSSIAN_SIGMA	];
	int		nMinMaxSize		= (int)		dPara[E_PARA_POINT_DUST_ENHANCMENT_MINMAX_SIZE		];
	int		nDeletArea		= (int)		dPara[E_PARA_POINT_DUST_LOGIC_DELET_AREA			];
	int		nDustDilate		= (int)		dPara[E_PARA_POINT_DUST_LOGIC_MORP_RANGE			];
	int		nFindMinArea	= (int)		dPara[E_PARA_POINT_DUST_LOGIC_BIG_AREA				];

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	bool	bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	int&	nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
	}

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem[0]);

	// 버퍼에 복사
	cv::Mat matTempBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	cv::Mat matTempBuf2 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());
	cv::Mat matTempBuf3 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());
	matSrcImage.copyTo(matTempBuf1);

	// 검사 영역 외곽 축소
	CRect rectTemp(rectROI);
	
	int nOffSet = 100;

	cv::Rect rtInspROI;

	//rtInspROI.x = rectROI.left - nOffSet;
	//rtInspROI.y = rectROI.top - nOffSet;
	//rtInspROI.width = rectROI.Width() + nOffSet * 2;
	//rtInspROI.height = rectROI.Height() + nOffSet * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nOffSet);

//////////////////////////////////////////////////////////////////////////
// 전처리 시작

	// Image Enhancement
	ShiftCopy(matTempBuf1(rtInspROI), matTempBuf2(rtInspROI), nShiftRanage, 0, 1, 0, &cMatBufTemp);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("ShiftCopy."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_ShiftCopy.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf2);
	}

	//rtInspROI.x = rectROI.left - nGaussianSize;
	//rtInspROI.y = rectROI.top - nGaussianSize;
	//rtInspROI.width = rectROI.Width() + nGaussianSize * 2;
	//rtInspROI.height = rectROI.Height() + nGaussianSize * 2;

	Insp_RectSet(rtInspROI, rectROI, matTempBuf2.cols, matTempBuf2.rows, nGaussianSize);

	// Blur
	cv::GaussianBlur(matTempBuf2(rtInspROI), matTempBuf3(rtInspROI), cv::Size(nGaussianSize,nGaussianSize), fGaussianSigma);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("blur."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_GaussianBlur.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf3);
	}

	// KSW 18.05.21 예외처리
	if( nMinMaxSize < 3 )	nMinMaxSize = 3;

	//rtInspROI.x = rectROI.left - nMinMaxSize;
	//rtInspROI.y = rectROI.top - nMinMaxSize;
	//rtInspROI.width = rectROI.Width() + nMinMaxSize * 2;
	//rtInspROI.height = rectROI.Height() + nMinMaxSize * 2;
	
	Insp_RectSet(rtInspROI, rectROI, matTempBuf3.cols, matTempBuf3.rows, nMinMaxSize);

	matTempBuf1.setTo(0);
	matTempBuf2.setTo(0);
	// Min-Max Filtering
	cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, cv::Size(nMinMaxSize, nMinMaxSize), cv::Point(-1, -1));
	
	cv::morphologyEx(matTempBuf3(rtInspROI), matTempBuf1(rtInspROI), MORPH_ERODE, StructElem);	// MORPH_ERODE  (Min)
	cv::morphologyEx(matTempBuf3(rtInspROI), matTempBuf2(rtInspROI), MORPH_DILATE, StructElem);	// MORPH_DILATE (Max)

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Min-Max Filter."));

	StructElem.release();

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_MinFilter.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf1);
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_MaxFilter.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf2);
	}

	// Max - Min SubImage Calculation
	matTempBuf3 = matTempBuf2 - matTempBuf1;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Min-Max Substract."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Sub Image.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf3);
	}

//////////////////////////////////////////////////////////////////////////
// Threshold

	nErrorCode = RangeAvgThreshold_DUST(matTempBuf3, matDstImage, rectTemp, dPara, &cMatBufTemp);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DUST RangeAvgThreshold."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Threshold_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Threshold_Dark.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
	}

	// Memory 해재
	matTempBuf1.release();
	matTempBuf2.release();
	matTempBuf3.release();

	// CHole 제거
	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		cv::Point ptCHoleCenter;
		ptCHoleCenter.x = rcCHoleROI[i].x + rcCHoleROI[i].width / 2;
		ptCHoleCenter.y = rcCHoleROI[i].y + rcCHoleROI[i].height / 2;
		int nCHoleraius = (rcCHoleROI[i].width + rcCHoleROI[i].height) / 2;

		cv::circle(matDstImage[E_DEFECT_COLOR_BRIGHT], ptCHoleCenter, nCHoleraius, cv::Scalar(0), -1);
	}

//////////////////////////////////////////////////////////////////////////
// Dust Logic

	// 점등 역역만 남기기
	nErrorCode = DeleteOutArea(matDstImage[E_DEFECT_COLOR_BRIGHT], rectROI, &cMatBufTemp);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DeleteOutArea."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_DeletOutArea_Image.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}
	
	//rtInspROI.x = rectROI.left;
	//rtInspROI.y = rectROI.top;
	//rtInspROI.width = rectROI.Width();
	//rtInspROI.height = rectROI.Height();

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows);

	// Bright 작은 면적 불량 제거
	// Blob 시 속도 느림.
	nErrorCode = DeleteArea(matDstImage[E_DEFECT_COLOR_BRIGHT](rtInspROI), nDeletArea, &cMatBufTemp);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DeleteArea."));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Delete.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}

	// Dust 불량 많은 경우, E급 판정 체크 함수
	nErrorCode = JudgementCheckE(matDstImage[E_DEFECT_COLOR_BRIGHT], dPara, rectROI, &cMatBufTemp);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("E Check."));

	// 먼지 제거 시, 조금 크게 하여 제거.
	if( nDustDilate > 0)
	{
		//rtInspROI.x = rectROI.left - nDustDilate;
		//rtInspROI.y = rectROI.top - nDustDilate;
		//rtInspROI.width = rectROI.Width() + nDustDilate * 2;
		//rtInspROI.height = rectROI.Height() + nDustDilate * 2;

		Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nDustDilate);

		nErrorCode = Morphology(matDstImage[E_DEFECT_COLOR_BRIGHT](rtInspROI), matDstImage[E_DEFECT_COLOR_BRIGHT](rtInspROI), nDustDilate, nDustDilate, E_MORP_DILATE);
		if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Morphology."));

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Dilate.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
		}
	}

	// 먼지 큰 경우 --> 주변 암점 제거용
	// 먼지의 경우, 현재 Dust 조명으로 모두 검출 안됨
	{
		//rtInspROI.x = rectROI.left;
		//rtInspROI.y = rectROI.top;
		//rtInspROI.width = rectROI.Width();
		//rtInspROI.height = rectROI.Height();

		Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows);

		nErrorCode = FindBigAreaDust(matDstImage[E_DEFECT_COLOR_BRIGHT](rtInspROI), matDstImage[E_DEFECT_COLOR_DARK](rtInspROI), nFindMinArea, &cMatBufTemp);
		if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

		writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Find Big Area."));

		// 중간 Image 확인
		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Bright_Big_Area.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_DEFECT_COLOR_DARK]);
		}
	}

	return E_ERROR_CODE_TRUE;
}


long CInspectPoint::ShiftCopy(cv::Mat& matSrcImage, cv::Mat& matDstImage, int nShiftX, int nShiftY, int nShiftLoopX, int nShiftLoopY, CMatBuf* cMemSub)
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
	cMatBufTemp.SetMem(cMemSub);
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



// 8bit & 12bit 영상 저장
long CInspectPoint::ImageSave(CString strPath, cv::Mat matSrcBuf)
{
	if( matSrcBuf.type() == CV_8U )
	{
		cv::imwrite((cv::String)(CStringA)strPath, matSrcBuf);
	}
	else
	{
		cv::Mat matTemp = cv::Mat::zeros(matSrcBuf.size(), CV_8U);
		matSrcBuf.convertTo(matTemp, CV_8U, 1./16.);

		cv::imwrite((cv::String)(CStringA)strPath, matTemp);

		matTemp.release();
	}

	return E_ERROR_CODE_TRUE;
}

// 12bit 이진화
long CInspectPoint::Binary(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double thresh, bool bInv, CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if( matSrcBuf.type() == CV_8U )
	{
		if( bInv )	cv::threshold(matSrcBuf, matDstBuf, thresh, 255.0, THRESH_BINARY_INV);
		else		cv::threshold(matSrcBuf, matDstBuf, thresh, 255.0, THRESH_BINARY);
	}
	else
		nErrorCode = Binary_16bit(matSrcBuf, matDstBuf, thresh, bInv, cMemSub);

	return nErrorCode;
}

// 12bit 이진화
long CInspectPoint::Binary_16bit(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double thresh, bool bInv, CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼가 없는 경우.
	if( matDstBuf.empty() )
		matDstBuf = cv::Mat::zeros(matSrcBuf.size(), matSrcBuf.type());

	// 버퍼가 있으나, 8U인 경우
	if( matDstBuf.type() == CV_8U )
	{
		uchar LUT[4096] = {0, };
		memset(LUT, 0, sizeof(uchar) * 4096);

		// 반전
		if( bInv )
		{
			for (int i=0 ; i<thresh ; i++)
				LUT[i] = (uchar)255;
		}
		else
		{
			for (int i=thresh+1 ; i<4096 ; i++)
				LUT[i] = (uchar)255;
		}

		MatIterator_<ushort> itSrc, endSrc;
		itSrc	= matSrcBuf.begin<ushort>();
		endSrc	= matSrcBuf.end<ushort>();
		MatIterator_<uchar> itDst	= matDstBuf.begin<uchar>();

		for ( ; itSrc != endSrc; itSrc++, itDst++)
			*itDst = LUT[ (int)(*itSrc) ];
	}

	// 버퍼가 있으나, 16U인 경우
	else
	{
		ushort LUT[4096] = {0, };
		memset(LUT, 0, sizeof(ushort) * 4096);

		// 반전
		if( bInv )
		{
			for (int i=0 ; i<thresh ; i++)
				LUT[i] = (ushort)4095;
		}
		else
		{
			for (int i=thresh+1 ; i<4096 ; i++)
				LUT[i] = (ushort)4095;
		}

		MatIterator_<ushort> itSrc, endSrc, itDst;
		itSrc	= matSrcBuf.begin<ushort>();
		endSrc	= matSrcBuf.end<ushort>();
		itDst	= matDstBuf.begin<ushort>();

		for ( ; itSrc != endSrc; itSrc++, itDst++)
			*itDst = LUT[ (int)(*itSrc) ];
	}	

	return E_ERROR_CODE_TRUE;
}

// Pow ( 8bit & 12bit )
long CInspectPoint::Pow(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double dPow, CMatBuf* cMemSub)
{
	if( dPow < 1.0 )			return E_ERROR_CODE_POINT_WARNING_PARA;
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼가 없는 경우.
	if( matDstBuf.empty() )
		matDstBuf = cv::Mat::zeros(matSrcBuf.size(), matSrcBuf.type());

	// 원본 8U인 경우
	if( matSrcBuf.type() == CV_8U )
	{
		// 결과 8U인 경우
		if ( matDstBuf.type() == CV_8U )
		{
			uchar LUT[256] = {0, };
			
			for (int i = 0; i < 256; i++)
			{
				double dVal = pow( i, dPow );
				if( dVal < 0 )		dVal = 0;
				if( dVal > 255 )	dVal = 255;
				LUT[i] = (uchar)dVal;
			}
			
			MatIterator_<uchar> itSrc, endSrc, itDst;
			itSrc	= matSrcBuf.begin<uchar>();
			endSrc	= matSrcBuf.end<uchar>();
			itDst	= matDstBuf.begin<uchar>();
			
			for ( ; itSrc != endSrc; itSrc++, itDst++)
				*itDst = LUT[(*itSrc)];
		}
		// 결과 16U인 경우
		else
		{
			ushort LUT[4096] = {0, };

			for (int i = 0; i < 4096; i++)
			{
				double dVal = pow( i, dPow );
				if( dVal < 0 )		dVal = 0;
				if( dVal > 4095 )	dVal = 4095;
				LUT[i] = (ushort)dVal;
			}

			MatIterator_<uchar> itSrc, endSrc;
			itSrc	= matSrcBuf.begin<uchar>();
			endSrc	= matSrcBuf.end<uchar>();
			MatIterator_<ushort> itDst = matDstBuf.begin<ushort>();

			for ( ; itSrc != endSrc; itSrc++, itDst++)
				*itDst = LUT[(*itSrc)];
		}		
	}
	// 원본 16U인 경우
	else
	{
		ushort LUT[4096] = {0, };

		for (int i = 0; i < 4096; i++)
		{
			double dVal = pow( i, dPow );
			if( dVal < 0 )		dVal = 0;
			if( dVal > 4095 )	dVal = 4095;
			LUT[i] = (ushort)dVal;
		}

		MatIterator_<ushort> itSrc, endSrc, itDst;
		itSrc	= matSrcBuf.begin<ushort>();
		endSrc	= matSrcBuf.end<ushort>();
		itDst	= matDstBuf.begin<ushort>();

		for ( ; itSrc != endSrc; itSrc++, itDst++)
			*itDst = LUT[(*itSrc)];
	}

	return E_ERROR_CODE_TRUE;
}


// 분할 영역 이진화
long CInspectPoint::RangeAvgThreshold(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if( matSrcImage.type() == CV_8U )                                               
		nErrorCode = RangeAvgThreshold_8bit(matSrcImage, matDstImage, rectROI, dPara, cMemSub);
	else
		nErrorCode = RangeAvgThreshold_16bit(matSrcImage, matDstImage, rectROI, dPara, cMemSub);

	return nErrorCode;
}

// 분할 영역 이진화
long CInspectPoint::RangeAvgThreshold_8bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	long	nBlurLoop_5x5				= (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_BLUR_LOOP];
	long	nBlur_Size					= (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_BLUR_LOOP_SIZE];

	long	nSegX						= (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_SEG_X];
	long	nSegY						= (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_SEG_Y];
	long	nEdgeArea					= (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_EDGE_AREA];

	float	fActiveDarkRatio			= (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_ACTIVE_DARK_RATIO];
	float	fActiveBrightRatio			= (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_ACTIVE_BRIGHT_RATIO];
	float	fEdgeDarkRatio					 =(float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_EDGE_DARK_RATIO];
	float	fEdgeBrightRatio			= (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_EDGE_BRIGHT_RATIO];

	int		nMinThGV					= (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_MIN_THRESHOLD];
	int		nMedian						= (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_MEDIAN];

	// 설정 값이 10보다 작은 경우.
	if( nSegX <= 10 )		return E_ERROR_CODE_POINT_WARNING_PARA;
	if( nSegY <= 10 )		return E_ERROR_CODE_POINT_WARNING_PARA;

	// 이미지 크기
	long	nWidth	= (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcImage.rows;	// 영상 세로 크기

	long nStart_Y, nEnd_Y;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// Temp 메모리 할당
	cv::Mat matBlurBuf  = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	cv::Mat matBlurBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);

	// Range Avg
	cv::Mat matAvgBuf	= cMatBufTemp.GetMat(nRangeY, nRangeX, matSrcImage.type(), false);

	

	int nBlur = nBlur_Size;

	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nBlur;
	//rtInspROI.y = rectROI.top - nBlur;
	//rtInspROI.width = rectROI.Width() + nBlur * 2;
	//rtInspROI.height = rectROI.Height() + nBlur * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nBlur);

	if( nBlurLoop_5x5 > 0 )
	{
		cv::blur(matSrcImage(rtInspROI), matBlurBuf(rtInspROI), cv::Size(nBlur, nBlur));

		if( nBlurLoop_5x5 > 1 )
		{
			// Avg
			for(int i=1 ; i<nBlurLoop_5x5 ; i++)
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
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

		// y 범위 계산
		if ( y < nRangeY - nEdgeArea )
		{
			nStart_Y = y * nSegY + rectROI.top;

			if( y==nRangeY-1-nEdgeArea)		nEnd_Y = rectROI.bottom - nEdgeArea * nSegY;
			else							nEnd_Y = nStart_Y + nSegY;
		}
		else
		{
			nStart_Y = rectROI.bottom - (nRangeY - y) * nSegY;
			nEnd_Y	 = nStart_Y + nSegY;
		}

		for (long x=0 ; x<nRangeX ; x++)
		{
			long nStart_X, nEnd_X;

			// x 범위 계산
			if ( x < nRangeX - nEdgeArea )
			{				
				nStart_X = x * nSegX + rectROI.left;

				if( x==nRangeX-1-nEdgeArea)		nEnd_X = rectROI.right - nEdgeArea * nSegX;
				else							nEnd_X = nStart_X + nSegX;
			}
			else
			{
				nStart_X = rectROI.right - (nRangeX - x) * nSegX;
				nEnd_X	 = nStart_X + nSegX;
			}

			// 범위 설정
			rectPtr[x].x		= nStart_X;
			rectPtr[x].y		= nStart_Y;
			rectPtr[x].width	= nEnd_X - nStart_X + 1;
			rectPtr[x].height	= nEnd_Y - nStart_Y + 1;
		}
	}

	// 평균 계산
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Avg
		BYTE *ptr = (BYTE *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x=0 ; x<nRangeX ; x++)
		{
			// 영상 ROI
			cv::Mat matTempBuf = matBlurBuf( rectPtr[x] );

			// mean & stdDev 구하기
		//	cv::Scalar m, s;
		//	cv::meanStdDev(matTempBuf, m, s);

			// 히스토그램
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			double dblAverage;
			double dblStdev;
			GetMeanStdDev_From_Histo(matHisto,0,255,dblAverage,dblStdev);

			// 평균 범위 설정
		//	int nMinGV = (int)(m[0] - s[0]);
		//	int nMaxGV = (int)(m[0] + s[0]);
			int nMinGV = (int)(dblAverage - dblStdev);
			int nMaxGV = (int)(dblAverage + dblStdev);
			if( nMinGV < 0 )	nMinGV = 0;
			if( nMaxGV > 255 )	nMaxGV = 255;			

			// 초기화
			__int64 nPixelSum	= 0;
			__int64 nPixelCount	= 0;

			// 설정된 평균 범위만큼만 재평균
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m=nMinGV ; m<=nMaxGV ; m++, pVal++)
			{
				nPixelSum	+= (m * *pVal);
				nPixelCount	+= *pVal;
			}

			// 개수가 하나라도 있어야...
			if( nPixelCount > 0 )
				ptr[x] = (BYTE)(nPixelSum / (float)nPixelCount);
		}
	}

	// 주변 평균값 -> 중간값으로 설정
	MedianFilter(matAvgBuf, matAvgBuf, nMedian, &cMatBufTemp);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("MedianFilter."));

	// 이진화
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Avg
		BYTE *ptr = (BYTE *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x=0 ; x<nRangeX ; x++)
		{
			// 영상 ROI
			cv::Mat matTempBuf	= matBlurBuf( rectPtr[x] );

			// 평균 * Ratio
			long nDarkTemp, nBrightTemp;

			// Edge 설정 따로 하기
			if (x < nEdgeArea				||
				y < nEdgeArea				||
				x >= nRangeX - nEdgeArea	||
				y >= nRangeY - nEdgeArea	)
			{
				nDarkTemp	= (long)( ptr[x] * fEdgeDarkRatio );
				nBrightTemp	= (long)( ptr[x] * fEdgeBrightRatio );
			}
			else
			{
				nDarkTemp	= (long)( ptr[x] * fActiveDarkRatio );
				nBrightTemp	= (long)( ptr[x] * fActiveBrightRatio );
			}			

			// Gray 너무 어두운 경우 있음.
			// ( 평균 GV 2 ~ 3 * fBrightRatio -> 이진화 : 노이즈 다 올라옴 )
			if( nBrightTemp < nMinThGV )	nBrightTemp = nMinThGV;

			// 예외 처리
			if( nDarkTemp < 0 )		nDarkTemp	= 0;
			if( nBrightTemp > 255 )	nBrightTemp = 255;

			// 파라미터 0인경우 예외 처리
			if( fActiveDarkRatio == 0 )		nDarkTemp	= -1;
			if( fActiveBrightRatio == 0 )	nBrightTemp = 256;

			// E_DEFECT_COLOR_DARK Threshold
			cv::Mat matTempBufT = matDstImage[E_DEFECT_COLOR_DARK]( rectPtr[x] );
			cv::threshold(matTempBuf, matTempBufT, nDarkTemp, 255.0, THRESH_BINARY_INV);

			// E_DEFECT_COLOR_BRIGHT Threshold
			matTempBufT = matDstImage[E_DEFECT_COLOR_BRIGHT]( rectPtr[x] );
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

long CInspectPoint::RangeAvgThreshold_Bubble(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if (matSrcImage.type() == CV_8U)
		nErrorCode = RangeAvgThreshold_Bubble_8bit(matSrcImage, matDstImage, rectROI, dPara, cMemSub);
	else
		nErrorCode = RangeAvgThreshold_16bit(matSrcImage, matDstImage, rectROI, dPara, cMemSub);

	return nErrorCode;
}

// 분할 영역 이진화
long CInspectPoint::RangeAvgThreshold_Bubble_8bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	long	nBlurLoop_5x5 = (long)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_BLUR_LOOP];
	long	nBlur_Size = (long)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_BLUR_LOOP_SIZE];

	long	nSegX = (long)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_SEG_X];
	long	nSegY = (long)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_SEG_Y];
	long	nEdgeArea = (long)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_EDGE_AREA];

	float	fActiveDarkRatio = (float)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_ACTIVE_DARK_RATIO];
	float	fActiveBrightRatio = (float)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_ACTIVE_BRIGHT_RATIO];
	float	fEdgeDarkRatio = (float)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_EDGE_DARK_RATIO];
	float	fEdgeBrightRatio = (float)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_EDGE_BRIGHT_RATIO];

	int		nMinThGV = (long)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_MIN_THRESHOLD];
	int		nMedian = (long)dPara[E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_MEDIAN];

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
// 분할 영역 이진화
long CInspectPoint::RangeAvgThreshold_DUST(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
// 	long	nBlurLoop_5x5 = 1;//(long)dPara[E_PARA_POINT_DUST_BINARY_BLUR_LOOP];
// 	long	nSegX = 400;//(long)dPara[E_PARA_POINT_DUST_BINARY_SEG_X];
// 	long	nSegY = 400;//(long)dPara[E_PARA_POINT_DUST_BINARY_SEG_Y];
// 	long	nEdgeArea = 0;//(long)dPara[E_PARA_POINT_DUST_BINARY_EDGE_AREA];
// 
// 	float	fActiveDarkRatio = 0;// (float)dPara[E_PARA_POINT_DUST_BINARY_ACTIVE_DARK_RATIO];
// 	float	fActiveBrightRatio = 1.25;// (float)dPara[E_PARA_POINT_DUST_BINARY_ACTIVE_BRIGHT_RATIO];
// 	float	fEdgeDarkRatio = 0;// (float)dPara[E_PARA_POINT_DUST_BINARY_EDGE_DARK_RATIO];
// 	float	fEdgeBrightRatio = 0;//(float)dPara[E_PARA_POINT_DUST_BINARY_EDGE_BRIGHT_RATIO];
// 
// 	int		nMinThGV = 5;// (long)dPara[E_PARA_POINT_DUST_BINARY_MIN_THRESHOLD_GV];
// 	int		nMedian = 5;// (long)dPara[E_PARA_POINT_DUST_BINARY_MEDIAN];

	long	nBlurLoop_5x5 = (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_BLUR_LOOP];
	long	nBlur_Size = (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_BLUR_LOOP_SIZE];

	long	nSegX = (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_SEG_X];
	long	nSegY = (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_SEG_Y];
	long	nEdgeArea = (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_EDGE_AREA];

	float	fActiveDarkRatio = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_ACTIVE_DARK_RATIO];
	float	fActiveBrightRatio = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_ACTIVE_BRIGHT_RATIO];
	float	fEdgeDarkRatio = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_EDGE_DARK_RATIO];
	float	fEdgeBrightRatio = (float)dPara[E_PARA_LCP_SUBSTANCE_COMMON_EDGE_BRIGHT_RATIO];

	int		nMinThGV = (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_MIN_THRESHOLD];
	int		nMedian = (long)dPara[E_PARA_LCP_SUBSTANCE_COMMON_MEDIAN];

	// 설정 값이 10보다 작은 경우.
	if( nSegX <= 10 )		return E_ERROR_CODE_POINT_WARNING_PARA;
	if( nSegY <= 10 )		return E_ERROR_CODE_POINT_WARNING_PARA;



	// 이미지 크기
	long	nWidth	= (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcImage.rows;	// 영상 세로 크기

	long nStart_Y, nEnd_Y;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DUST Start."));

	// Temp 메모리 할당
	cv::Mat matBlurBuf  = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());
	cv::Mat matBlurBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());

	// Range Avg
	cv::Mat matAvgBuf	= cMatBufTemp.GetMat(nRangeY, nRangeX, matSrcImage.type(), false);

	int nBlur = nBlur_Size;

	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nBlur;
	//rtInspROI.y = rectROI.top - nBlur;
	//rtInspROI.width = rectROI.Width() + nBlur * 2;
	//rtInspROI.height = rectROI.Height() + nBlur * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nBlur);

	if( nBlurLoop_5x5 > 0 )
	{
		cv::blur(matSrcImage(rtInspROI), matBlurBuf(rtInspROI), cv::Size(nBlur, nBlur));

		if( nBlurLoop_5x5 > 1 )
		{
			// Avg
			for(int i=1 ; i<nBlurLoop_5x5 ; i++)
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

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DUST BlurLoop."));

	// Range Rect
	cv::Rect *rectRange = new cv::Rect[nRangeX * nRangeY];
	memset(rectRange, 0, sizeof(cv::Rect) * nRangeX * nRangeY);

	// 범위 계산
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

		// y 범위 계산
		if ( y < nRangeY - nEdgeArea )
		{
			nStart_Y = y * nSegY + rectROI.top;

			if( y==nRangeY-1-nEdgeArea)		nEnd_Y = rectROI.bottom - nEdgeArea * nSegY;
			else							nEnd_Y = nStart_Y + nSegY;
		}
		else
		{
			nStart_Y = rectROI.bottom - (nRangeY - y) * nSegY;
			nEnd_Y	 = nStart_Y + nSegY;
		}

		for (long x=0 ; x<nRangeX ; x++)
		{
			long nStart_X, nEnd_X;

			// x 범위 계산
			if ( x < nRangeX - nEdgeArea )
			{				
				nStart_X = x * nSegX + rectROI.left;

				if( x==nRangeX-1-nEdgeArea)		nEnd_X = rectROI.right - nEdgeArea * nSegX;
				else							nEnd_X = nStart_X + nSegX;
			}
			else
			{
				nStart_X = rectROI.right - (nRangeX - x) * nSegX;
				nEnd_X	 = nStart_X + nSegX;
			}

			// 범위 설정
			rectPtr[x].x		= nStart_X;
			rectPtr[x].y		= nStart_Y;
			rectPtr[x].width	= nEnd_X - nStart_X + 1;
			rectPtr[x].height	= nEnd_Y - nStart_Y + 1;
		}
	}

	// 평균 계산
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Avg
		BYTE *ptr = (BYTE *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x=0 ; x<nRangeX ; x++)
		{
			// 영상 ROI
			cv::Mat matTempBuf = matBlurBuf( rectPtr[x] );

			// mean & stdDev 구하기
			//	cv::Scalar m, s;
			//	cv::meanStdDev(matTempBuf, m, s);

			// 히스토그램
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			double dblAverage;
			double dblStdev;
			GetMeanStdDev_From_Histo(matHisto,0,255,dblAverage,dblStdev);

			// 평균 범위 설정
			//	int nMinGV = (int)(m[0] - s[0]);
			//	int nMaxGV = (int)(m[0] + s[0]);
			int nMinGV = (int)(dblAverage - dblStdev);
			int nMaxGV = (int)(dblAverage + dblStdev);
			if( nMinGV < 0 )	nMinGV = 0;
			if( nMaxGV > 255 )	nMaxGV = 255;			

			// 초기화
			__int64 nPixelSum	= 0;
			__int64 nPixelCount	= 0;

			// 설정된 평균 범위만큼만 재평균
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m=nMinGV ; m<=nMaxGV ; m++, pVal++)
			{
				nPixelSum	+= (m * *pVal);
				nPixelCount	+= *pVal;
			}

			// 개수가 하나라도 있어야...
			if( nPixelCount > 0 )
				ptr[x] = (BYTE)(nPixelSum / (float)nPixelCount);
		}
	}

	// 주변 평균값 -> 중간값으로 설정
	MedianFilter(matAvgBuf, matAvgBuf, nMedian, &cMatBufTemp);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("DUST MedianFilter."));

	// 이진화
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Avg
		BYTE *ptr = (BYTE *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x=0 ; x<nRangeX ; x++)
		{
			// 영상 ROI
			cv::Mat matTempBuf	= matBlurBuf( rectPtr[x] );

			// 평균 * Ratio
			long nDarkTemp, nBrightTemp;

			// Edge 설정 따로 하기
			if (x < nEdgeArea				||
				y < nEdgeArea				||
				x >= nRangeX - nEdgeArea	||
				y >= nRangeY - nEdgeArea	)
			{
				nDarkTemp	= (long)( ptr[x] * fEdgeDarkRatio );
				nBrightTemp	= (long)( ptr[x] * fEdgeBrightRatio );
			}
			else
			{
				nDarkTemp	= (long)( ptr[x] * fActiveDarkRatio );
				nBrightTemp	= (long)( ptr[x] * fActiveBrightRatio );
			}			

			// Gray 너무 어두운 경우 있음.
			// ( 평균 GV 2 ~ 3 * fBrightRatio -> 이진화 : 노이즈 다 올라옴 )
			// Dust에서 아주 약한 것을 찾을 수 도 있으니 0일 때 작동 안되게 함
			if ( nMinThGV != 0 )
			{
				if( nBrightTemp < nMinThGV )	nBrightTemp = nMinThGV;
			}
			
			// 예외 처리
			if( nDarkTemp < 0 )		nDarkTemp	= 0;
			if( nBrightTemp > 255 )	nBrightTemp = 255;

			// 파라미터 0인경우 예외 처리
			if( fActiveDarkRatio == 0 )		nDarkTemp	= -1;
			if( fActiveBrightRatio == 0 )	nBrightTemp = 256;

			// E_DEFECT_COLOR_DARK Threshold
			cv::Mat matTempBufT = matDstImage[E_DEFECT_COLOR_DARK]( rectPtr[x] );
			cv::threshold(matTempBuf, matTempBufT, nDarkTemp, 255.0, THRESH_BINARY_INV);

			// E_DEFECT_COLOR_BRIGHT Threshold
			matTempBufT = matDstImage[E_DEFECT_COLOR_BRIGHT]( rectPtr[x] );
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

//히스토그램으로 평균
long CInspectPoint::GetAverage_From_Histo(cv::Mat matHisto,int nMin,int nMax,double &dblAverage)
{
	double dblValSum	= 0;
	double dblCountSum	= 0;
	float *pVal = (float*)matHisto.data;

	// 평균을 구한다.
	for(int i = nMin ; i<nMax ; i++)
	{		
		dblValSum	+= (*pVal * i);
		dblCountSum	+= *pVal;
		pVal++;
	}

	dblAverage = dblValSum / dblCountSum;

	return E_ERROR_CODE_TRUE;
}

//히스토그램으로 표준편차
long CInspectPoint::GetMeanStdDev_From_Histo(cv::Mat matHisto,int nMin,int nMax,double &dblAverage,double &dblStdev)
{
	GetAverage_From_Histo(matHisto,nMin,nMax,dblAverage);
	
	double dblValSum	= 0;
	double dblCountSum	= 0;
	double dblTmp;
	float *pVal = (float*)matHisto.data;

	// 분산을 구한다.
	for(int i = nMin ; i<nMax ; i++)
	{	
		dblTmp = (i - dblAverage);
		dblTmp = dblTmp*dblTmp;
		dblValSum	+= (*pVal * dblTmp);
		dblCountSum	+= *pVal;
		pVal++;
	}

	dblStdev = dblValSum / dblCountSum;	;
	dblStdev = sqrt(dblStdev);

	return E_ERROR_CODE_TRUE;
}

// 분할 영역 이진화
long CInspectPoint::RangeAvgThreshold_16bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	long	nBlurLoop_5x5				= (long)dPara[E_PARA_POINT_RGB_COMMON_BLUR_LOOP];
	long	nSegX						= (long)dPara[E_PARA_POINT_RGB_COMMON_SEG_X];
	long	nSegY						= (long)dPara[E_PARA_POINT_RGB_COMMON_SEG_Y];
	long	nEdgeArea					= (long)dPara[E_PARA_POINT_RGB_EDGE_AREA];

	float	fActiveDarkRatio			= (float)dPara[E_PARA_POINT_RGB_ACTIVE_DARK_RATIO];
	float	fActiveBrightRatio			= (float)dPara[E_PARA_POINT_RGB_ACTIVE_BRIGHT_RATIO];
	float	fEdgeDarkRatio				= (float)dPara[E_PARA_POINT_RGB_EDGE_DARK_RATIO];
	float	fEdgeBrightRatio			= (float)dPara[E_PARA_POINT_RGB_EDGE_BRIGHT_RATIO];

	int		nMinThGV					= (long)dPara[E_PARA_POINT_RGB_COMMON_MIN_THRESHOLD];
	int		nMedian						= (long)dPara[E_PARA_POINT_RGB_COMMON_MEDIAN];

	// 설정 값이 10보다 작은 경우.
	if( nSegX <= 10 )		return E_ERROR_CODE_POINT_WARNING_PARA;
	if( nSegY <= 10 )		return E_ERROR_CODE_POINT_WARNING_PARA;

	// 이미지 크기
	long	nWidth	= (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcImage.rows;	// 영상 세로 크기

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
	cv::Mat matAvgBuf	= cMatBufTemp.GetMat(nRangeY, nRangeX, matSrcImage.type(), false);

	int nBlur = 5;

	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nBlur;
	//rtInspROI.y = rectROI.top - nBlur;
	//rtInspROI.width = rectROI.Width() + nBlur * 2;
	//rtInspROI.height = rectROI.Height() + nBlur * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nBlur);

	if( nBlurLoop_5x5 > 0 )
	{
		cv::blur(matSrcImage(rtInspROI), matBlurBuf(rtInspROI), cv::Size(nBlur, nBlur));

		if( nBlurLoop_5x5 > 1 )
		{
			// Avg
			for(int i=1 ; i<nBlurLoop_5x5 ; i++)
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
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

		// y 범위 계산
		if ( y < nRangeY - nEdgeArea )
		{
			nStart_Y = y * nSegY + rectROI.top;

			if( y==nRangeY-1-nEdgeArea)		nEnd_Y = rectROI.bottom - nEdgeArea * nSegY;
			else							nEnd_Y = nStart_Y + nSegY;
		}
		else
		{
			nStart_Y = rectROI.bottom - (nRangeY - y) * nSegY;
			nEnd_Y	 = nStart_Y + nSegY;
		}

		for (long x=0 ; x<nRangeX ; x++)
		{
			long nStart_X, nEnd_X;

			// x 범위 계산
			if ( x < nRangeX - nEdgeArea )
			{				
				nStart_X = x * nSegX + rectROI.left;

				if( x==nRangeX-1-nEdgeArea)		nEnd_X = rectROI.right - nEdgeArea * nSegX;
				else							nEnd_X = nStart_X + nSegX;
			}
			else
			{
				nStart_X = rectROI.right - (nRangeX - x) * nSegX;
				nEnd_X	 = nStart_X + nSegX;
			}

			// 범위 설정
			rectPtr[x].x		= nStart_X;
			rectPtr[x].y		= nStart_Y;
			rectPtr[x].width	= nEnd_X - nStart_X + 1;
			rectPtr[x].height	= nEnd_Y - nStart_Y + 1;
		}
	}

	// 평균 계산
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Avg
		ushort *ptr = (ushort *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x=0 ; x<nRangeX ; x++)
		{
			// 영상 ROI
			cv::Mat matTempBuf = matBlurBuf( rectPtr[x] );

			// mean & stdDev 구하기
			cv::Scalar m, s;
			cv::meanStdDev(matTempBuf, m, s);

			// 평균 범위 설정
			int nMinGV = (int)(m[0] - s[0]);
			int nMaxGV = (int)(m[0] + s[0]);
			if( nMinGV < 0 )	nMinGV = 0;
			if( nMaxGV > 4095 )	nMaxGV = 4095;

			// 히스토그램
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			// 초기화
			__int64 nPixelSum	= 0;
			__int64 nPixelCount	= 0;

			// 설정된 평균 범위만큼만 재평균
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m=nMinGV ; m<=nMaxGV ; m++, pVal++)
			{
				nPixelSum	+= (m * *pVal);
				nPixelCount	+= *pVal;
			}

			// 개수가 하나라도 있어야...
			if( nPixelCount > 0 )
				ptr[x] = (ushort)(nPixelSum / (float)nPixelCount);
		}
	}

	// 주변 평균값 -> 중간값으로 설정
	MedianFilter(matAvgBuf, matAvgBuf, nMedian, &cMatBufTemp);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("MedianFilter."));

	// 이진화
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Avg
		ushort *ptr = (ushort *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x=0 ; x<nRangeX ; x++)
		{
			// 영상 ROI
			cv::Mat matTempBuf	= matBlurBuf( rectPtr[x] );

			// 평균 * Ratio
			long nDarkTemp, nBrightTemp;

			// Edge 설정 따로 하기
			if (x < nEdgeArea				||
				y < nEdgeArea				||
				x >= nRangeX - nEdgeArea	||
				y >= nRangeY - nEdgeArea	)
			{
				nDarkTemp	= (long)( ptr[x] * fEdgeDarkRatio );
				nBrightTemp	= (long)( ptr[x] * fEdgeBrightRatio );
			}
			else
			{
				nDarkTemp	= (long)( ptr[x] * fActiveDarkRatio );
				nBrightTemp	= (long)( ptr[x] * fActiveBrightRatio );
			}			

			// Gray 너무 어두운 경우 있음.
			// ( 평균 GV 2 ~ 3 * fBrightRatio -> 이진화 : 노이즈 다 올라옴 )
			if( nBrightTemp < nMinThGV )	nBrightTemp = nMinThGV;

			// 예외 처리
			if( nDarkTemp < 0 )			nDarkTemp	= 0;
			if( nBrightTemp > 4095 )	nBrightTemp = 4095;

			// 파라미터 0인경우 예외 처리
			if( fActiveDarkRatio == 0 )		nDarkTemp	= -1;
			if( fActiveBrightRatio == 0 )	nBrightTemp = 4096;

			// E_DEFECT_COLOR_DARK Threshold
			cv::Mat matTempBufT = matDstImage[E_DEFECT_COLOR_DARK]( rectPtr[x] );
			Binary_16bit(matTempBuf, matTempBufT, nDarkTemp, true, &cMatBufTemp);

			// E_DEFECT_COLOR_BRIGHT Threshold
			matTempBufT = matDstImage[E_DEFECT_COLOR_BRIGHT]( rectPtr[x] );
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

// 분할 영역 이진화
long CInspectPoint::RangeAvgThreshold_RGB(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub)
{
	long	nBlurLoop_5x5				= (long)dPara[E_PARA_POINT_RGB_COMMON_BLUR_LOOP];
	long	nSegX						= (long)dPara[E_PARA_POINT_RGB_COMMON_SEG_X];
	long	nSegY						= (long)dPara[E_PARA_POINT_RGB_COMMON_SEG_Y];
	long	nEdgeArea					= (long)dPara[E_PARA_POINT_RGB_EDGE_AREA];

	float	fActiveDarkRatio			= (float)dPara[E_PARA_POINT_RGB_BRIGHT_INSP_DARK_RATIO_ACTIVE	];
	float	fActiveBrightRatio			= (float)dPara[E_PARA_POINT_RGB_BRIGHT_INSP_BRIGHT_RATIO_ACTIVE	];
	float	fEdgeDarkRatio				= (float)dPara[E_PARA_POINT_RGB_BRIGHT_INSP_DARK_RATIO_EDGE		];
	float	fEdgeBrightRatio			= (float)dPara[E_PARA_POINT_RGB_BRIGHT_INSP_BRIGHT_RATIO_EDGE	];

	int		nMinThGV					= (long)dPara[E_PARA_POINT_RGB_COMMON_MIN_THRESHOLD];
	int		nMedian						= (long)dPara[E_PARA_POINT_RGB_COMMON_MEDIAN];

	// 설정 값이 10보다 작은 경우.
	if( nSegX <= 10 )		return E_ERROR_CODE_POINT_WARNING_PARA;
	if( nSegY <= 10 )		return E_ERROR_CODE_POINT_WARNING_PARA;

	// 이미지 크기
	long	nWidth	= (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcImage.rows;	// 영상 세로 크기

	long nStart_Y, nEnd_Y;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Start."));

	// Temp 메모리 할당	
	cv::Mat matBlurBuf  = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	cv::Mat matBlurBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);

	// Range Avg
	cv::Mat matAvgBuf	= cMatBufTemp.GetMat(nRangeY, nRangeX, matSrcImage.type(), false);

	int nBlur = 5;

	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nBlur;
	//rtInspROI.y = rectROI.top - nBlur;
	//rtInspROI.width = rectROI.Width() + nBlur * 2;
	//rtInspROI.height = rectROI.Height() + nBlur * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nBlur);

	if( nBlurLoop_5x5 > 0 )
	{
		cv::blur(matSrcImage(rtInspROI), matBlurBuf(rtInspROI), cv::Size(nBlur, nBlur));

		if( nBlurLoop_5x5 > 1 )
		{
			// Avg
			for(int i=1 ; i<nBlurLoop_5x5 ; i++)
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
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

		// y 범위 계산
		if ( y < nRangeY - nEdgeArea )
		{
			nStart_Y = y * nSegY + rectROI.top;

			if( y==nRangeY-1-nEdgeArea)		nEnd_Y = rectROI.bottom - nEdgeArea * nSegY;
			else							nEnd_Y = nStart_Y + nSegY;
		}
		else
		{
			nStart_Y = rectROI.bottom - (nRangeY - y) * nSegY;
			nEnd_Y	 = nStart_Y + nSegY;
		}

		for (long x=0 ; x<nRangeX ; x++)
		{
			long nStart_X, nEnd_X;

			// x 범위 계산
			if ( x < nRangeX - nEdgeArea )
			{				
				nStart_X = x * nSegX + rectROI.left;

				if( x==nRangeX-1-nEdgeArea)		nEnd_X = rectROI.right - nEdgeArea * nSegX;
				else							nEnd_X = nStart_X + nSegX;
			}
			else
			{
				nStart_X = rectROI.right - (nRangeX - x) * nSegX;
				nEnd_X	 = nStart_X + nSegX;
			}

			// 범위 설정
			rectPtr[x].x		= nStart_X;
			rectPtr[x].y		= nStart_Y;
			rectPtr[x].width	= nEnd_X - nStart_X + 1;
			rectPtr[x].height	= nEnd_Y - nStart_Y + 1;
		}
	}

	// 평균 계산
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Avg
		ushort *ptr = (ushort *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x=0 ; x<nRangeX ; x++)
		{
			// 영상 ROI
			cv::Mat matTempBuf = matBlurBuf( rectPtr[x] );

			// mean & stdDev 구하기
			cv::Scalar m, s;
			cv::meanStdDev(matTempBuf, m, s);

			// 평균 범위 설정
			int nMinGV = (int)(m[0] - s[0]);
			int nMaxGV = (int)(m[0] + s[0]);
			if( nMinGV < 0 )	nMinGV = 0;
			if( nMaxGV > 4095 )	nMaxGV = 4095;

			// 히스토그램
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			// 초기화
			__int64 nPixelSum	= 0;
			__int64 nPixelCount	= 0;

			// 설정된 평균 범위만큼만 재평균
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m=nMinGV ; m<=nMaxGV ; m++, pVal++)
			{
				nPixelSum	+= (m * *pVal);
				nPixelCount	+= *pVal;
			}

			// 개수가 하나라도 있어야...
			if( nPixelCount > 0 )
				ptr[x] = (ushort)(nPixelSum / (float)nPixelCount);
		}
	}

	// 주변 평균값 -> 중간값으로 설정
	MedianFilter(matAvgBuf, matAvgBuf, nMedian, &cMatBufTemp);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("MedianFilter."));

	// 이진화
	for (long y=0 ; y<nRangeY ; y++)
	{
		// Range Avg
		ushort *ptr = (ushort *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x=0 ; x<nRangeX ; x++)
		{
			// 영상 ROI
			cv::Mat matTempBuf	= matBlurBuf( rectPtr[x] );

			// 평균 * Ratio
			long nDarkTemp, nBrightTemp;

			// Edge 설정 따로 하기
			if (x < nEdgeArea				||
				y < nEdgeArea				||
				x >= nRangeX - nEdgeArea	||
				y >= nRangeY - nEdgeArea	)
			{
				nDarkTemp	= (long)( ptr[x] * fEdgeDarkRatio );
				nBrightTemp	= (long)( ptr[x] * fEdgeBrightRatio );
			}
			else
			{
				nDarkTemp	= (long)( ptr[x] * fActiveDarkRatio );
				nBrightTemp	= (long)( ptr[x] * fActiveBrightRatio );
			}			

			// Gray 너무 어두운 경우 있음.
			// ( 평균 GV 2 ~ 3 * fBrightRatio -> 이진화 : 노이즈 다 올라옴 )
			if( nBrightTemp < nMinThGV )	nBrightTemp = nMinThGV;

			// 예외 처리
			if( nDarkTemp < 0 )			nDarkTemp	= 0;
			if( nBrightTemp > 4095 )	nBrightTemp = 4095;

			// 파라미터 0인경우 예외 처리
			if( fActiveDarkRatio == 0 )		nDarkTemp	= -1;
			if( fActiveBrightRatio == 0 )	nBrightTemp = 4096;

			// E_DEFECT_COLOR_DARK Threshold
			cv::Mat matTempBufT = matDstImage[E_DEFECT_COLOR_DARK]( rectPtr[x] );
			Binary_16bit(matTempBuf, matTempBufT, nDarkTemp, true, &cMatBufTemp);

			// E_DEFECT_COLOR_BRIGHT Threshold
			matTempBufT = matDstImage[E_DEFECT_COLOR_BRIGHT]( rectPtr[x] );
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


// Median
long CInspectPoint::MedianFilter(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nKSize, CMatBuf* cMemSub)
{
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if( matSrcBuf.type() == CV_8U )
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

		for (int yy=0 ; yy<matSrcBuf.rows ; yy++)
		{
			ushort *ptrDst = (ushort *)matTempBuf.ptr(yy);

			for (int xx=0 ; xx<matSrcBuf.cols ; xx++, ptrDst++)
			{
				// 범위 확인
				int nSX = xx - nHSize;
				int nEX = xx + nHSize;
				int nSY = yy - nHSize;
				int nEY = yy + nHSize;

				// 예외 처리
				nSX = ( nSX < 0 ) ? 0 : nSX ;
				nSY = ( nSY < 0 ) ? 0 : nSY ;
				nEX = ( nEX >= matSrcBuf.cols ) ? matSrcBuf.cols - 1 : nEX ;
				nEY = ( nEY >= matSrcBuf.rows ) ? matSrcBuf.rows - 1 : nEY ;

				int nCount = 0;

				// 데이터 넣기
				for (int nMy=nSY ; nMy<=nEY ; nMy++)
				{
					ushort *ptrSrc = (ushort *)matSrcBuf.ptr(nMy) + nSX;

					for (int nMx=nSX ; nMx<=nEX ; nMx++, ptrSrc++)
					{
						nArr[nCount++] = *ptrSrc;
					}
				}

				// 정렬
				for (int i=0 ; i<nCount-1 ; i++)
				{
					for (int j=i+1 ; j<nCount ; j++)
					{
						if( nArr[i] > nArr[j] )
						{
							ushort nTemp = nArr[i];
							nArr[i] = nArr[j];
							nArr[j] = nTemp;
						}
					}
				}

				// 결과
				*ptrDst = nArr[ nCount/2 ];
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

// Distance Transform & Threshold
long CInspectPoint::DistanceTransformThreshold(cv::Mat& matSrcImage, cv::Mat& matDstImage, double dThreshold, CMatBuf* cMemSub)
{
	if( dThreshold <= 0.0 )	return E_ERROR_CODE_TRUE;
	if( dThreshold >= 255.0 )	return E_ERROR_CODE_TRUE;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	cv::Mat matTempBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), CV_32FC1, false);
	cv::Mat matTempBuf2 = cMatBufTemp.GetMat(matSrcImage.size(), CV_32FC1, false);

	// 원본 8bit / 결과 32F
	if( matSrcImage.type() == CV_8U )
	{
		cv::distanceTransform(matSrcImage, matTempBuf1, CV_DIST_L2, 3);

		cv::threshold(matTempBuf1, matTempBuf2, dThreshold, 255.0, THRESH_BINARY);
	}
	else
	{
		cv::Mat matSrc8bit = cMatBufTemp.GetMat(matSrcImage.size(), CV_8UC1, false);
		matSrcImage.convertTo(matSrc8bit, CV_8UC1, 1./16.);

		cv::distanceTransform(matSrc8bit, matTempBuf1, CV_DIST_L2, 3);

		matSrc8bit.release();

		cv::threshold(matTempBuf1, matTempBuf2, dThreshold, 4095.0, THRESH_BINARY);
	}	

	// bit 변환
	matTempBuf2.convertTo(matDstImage, matSrcImage.type());

	matTempBuf1.release();
	matTempBuf2.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// 모폴로지
long CInspectPoint::Morphology(cv::Mat& matSrcImage, cv::Mat& matDstImage, long nSizeX, long nSizeY, ENUM_MORP nOperation)
{
	if( nSizeX < 1 )			return E_ERROR_CODE_POINT_WARNING_PARA;
	if( nSizeY < 1 )			return E_ERROR_CODE_POINT_WARNING_PARA;
	if( matSrcImage.empty() )	return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, Size(nSizeX, nSizeY), Point(nSizeX/2, nSizeY/2));

	switch (nOperation)
	{
	case E_MORP_ERODE:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_ERODE, StructElem);
		break;

	case E_MORP_DILATE:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_DILATE, StructElem);
		break;

	case E_MORP_OPEN:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_OPEN, StructElem);
		break;

	case E_MORP_CLOSE:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_CLOSE, StructElem);
		break;

	default:
		StructElem.release();
		return E_ERROR_CODE_POINT_WARNING_PARA;
		break;
	}

	StructElem.release();
	return E_ERROR_CODE_TRUE;
}

// 외각 제거
long CInspectPoint::DeleteOutArea(cv::Mat& matSrcImage, cv::Point* ptCorner, CMatBuf* cMemSub)
{

	CMatBuf cMatBufTemp;

	// Temp 메모리 할당
	cMatBufTemp.SetMem(cMemSub);

	// 마스크 초기화
	cv::Mat matTempBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);

	int npt[] = {E_CORNER_END };
	const cv::Point* ppt[1] = { ptCorner };

	// 점등 영역 마스크
	if( matSrcImage.type() == CV_8U )
		cv::fillPoly(matTempBuf, ppt, npt, 1, cv::Scalar(255, 255, 255));
	else
		cv::fillPoly(matTempBuf, ppt, npt, 1, cv::Scalar(4095, 4095, 4095));

	// AND
	cv::bitwise_and(matSrcImage, matTempBuf, matSrcImage);

	// 할당 해제
	matTempBuf.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// 외각 제거
long CInspectPoint::DeleteOutArea(cv::Mat& matSrcImage, CRect rectROI, CMatBuf* cMemSub)
{
	CMatBuf cMatBufTemp;

	// Temp 메모리 할당
	cMatBufTemp.SetMem(cMemSub);

	// 마스크 초기화
	cv::Mat matTempBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());

	// 점등 영역 마스크
	if( matSrcImage.type() == CV_8U )
		cv::rectangle(matTempBuf, cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()), cv::Scalar(255, 255, 255), -1);
	else
		cv::rectangle(matTempBuf, cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()), cv::Scalar(4095, 4095, 4095), -1);

	// AND
	cv::bitwise_and(matSrcImage, matTempBuf, matSrcImage);

	// 할당 해제
	matTempBuf.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// 작은 면적 제거
long CInspectPoint::DeleteArea(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if( matSrcImage.type() == CV_8U )
		nErrorCode = DeleteArea_8bit(matSrcImage, nCount, cMemSub);
	else
		nErrorCode = DeleteArea_16bit(matSrcImage, nCount, cMemSub);

	return nErrorCode;
}

// 작은 면적 제거
long CInspectPoint::DeleteArea_8bit(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우.
	if( matSrcImage.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;	

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// 메모리 할당
	cv::Mat matLabel, matStats, matCentroid;
	matLabel = cMatBufTemp.GetMat(matSrcImage.size(), CV_32SC1, false);

	// Blob 개수
	__int64 nTotalLabel = cv::connectedComponentsWithStats(matSrcImage, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

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
		if( nArea <= nCount )
		{
			// 0GV 로 초기화 후, 넘어가기
			cv::Mat matTempROI	= matSrcImage( rectTemp );
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
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}
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
long CInspectPoint::DeleteArea_16bit(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우.
	if( matSrcImage.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// 8bit 변환
	cv::Mat matSrc8bit = cMatBufTemp.GetMat(matSrcImage.size(), CV_8UC1, false);
	matSrcImage.convertTo(matSrc8bit, CV_8UC1, 1./16.);

	// 메모리 할당
	cv::Mat matLabel, matStats, matCentroid;
	matLabel = cMatBufTemp.GetMat(matSrc8bit.size(), CV_32SC1, false);

	// Blob 개수
	__int64 nTotalLabel = cv::connectedComponentsWithStats(matSrc8bit, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

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
		if( nArea <= nCount )
		{
			// 0GV 로 초기화 후, 넘어가기
			cv::Mat matTempROI	= matSrc8bit( rectTemp );
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
	}

	// 16bit 변환
	matSrc8bit.convertTo(matSrcImage, CV_16UC1, 16.);
	
// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}
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
			if( matSrcImage.at<ushort>(y, x) == 0)		continue;

			memset(bConnect, 0, sizeof(bool) * 8);
			nConnectCnt = 1;

			// 주변 갯수 파악
			for (int z=0 ; z<8 ; z++)
			{
				if( matSrcImage.at<ushort>(y + nConnectY[z], x + nConnectX[z]) != 0)
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

				DstBuffer.at<ushort>(y + nConnectY[z], x + nConnectX[z]) = (ushort)4095;
			}

			// 중심 그리기
			DstBuffer.at<ushort>(y, x) = (ushort)4095;
		}
	}

	matSrcImage = DstBuffer.clone();

	DstBuffer.release();
	************************************************************************/

	return E_ERROR_CODE_TRUE;
}


// Line Delete
long CInspectPoint::ProjectionLineDelete(cv::Mat& matSrcImage, int nCutCountX, int nCutCountY, int nThicknessX, int nThicknessY)
{
	// 버퍼가 없는 경우.
	if( matSrcImage.empty()	)		return E_ERROR_CODE_EMPTY_BUFFER;

	vector <int>	ptrX, ptrY;
	int nTemp;

	int nMaxGV = 0;
	if( matSrcImage.type() == CV_8U )
		nMaxGV = 255;
	else
		nMaxGV = 4095;

	// 벡터 초기화
	vector <int>().swap(ptrX);
	vector <int>().swap(ptrY);

	// 가로 방향
	if( nCutCountX > 0 )
	{
		// 가로방향 누적
		for (int y=0 ; y<matSrcImage.rows ; y++)
		{
			nTemp = (int)( cv::sum( matSrcImage.row(y) )[0] / nMaxGV );

			if( nTemp > nCutCountX )
			{
				ptrY.push_back( y );
			}
		}

		// 가로방향 삭제
		for (int y=0 ; y<ptrY.size() ; y++)
		{
			cv::line(matSrcImage, cv::Point(0, ptrY[y]), cv::Point(matSrcImage.cols, ptrY[y]), cv::Scalar(0, 0, 0), nThicknessX);
		}
	}

	// 세로 방향
	if( nCutCountY > 0 )
	{
		// 세로방향 누적
		for (int x=0 ; x<matSrcImage.cols ; x++)
		{
			nTemp = (short)( cv::sum( matSrcImage.col(x) )[0] / nMaxGV );

			if( nTemp > nCutCountY )
			{
				ptrX.push_back( x );
			}
		}

		// 세로방향 삭제
		for (int x=0 ; x<ptrX.size() ; x++)
		{
			cv::line(matSrcImage, cv::Point(ptrX[x], 0), cv::Point(ptrX[x], matSrcImage.rows), cv::Scalar(0, 0, 0), nThicknessX);
		}
	}	

	// 벡터 초기화
	vector <int>().swap(ptrX);
	vector <int>().swap(ptrY);

	return E_ERROR_CODE_TRUE;	
}


long CInspectPoint::GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto)
{
	// 버퍼가 없는 경우.
	if( matSrcImage.empty()	)		return E_ERROR_CODE_EMPTY_BUFFER;

	int nHistSize = 0;
	if( matSrcImage.type() == CV_8U )
		nHistSize = 256;
	else
		nHistSize = 4096;

	float fHistRange[] = {(float)0, (float)nHistSize-1};
	const float *ranges[] = { fHistRange };

	cv::calcHist(&matSrcImage,1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;
}

long CInspectPoint::JudgementCheckE(cv::Mat& matSrcBuf, double* dPara, CRect rectROI, CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 파라미터	
	int nFlag				= (int)dPara[E_PARA_POINT_DUST_GROUP_USE];
	int nDel				= (int)dPara[E_PARA_POINT_DUST_GROUP_EDGE_DEL_OUTLINE];
	int nEdgeArea			= (int)dPara[E_PARA_POINT_DUST_GROUP_EDGE_AREA];
	int nMinAreaEdge		= (int)dPara[E_PARA_POINT_DUST_GROUP_MIN_AREA_EDGE];
	int nMinAreaActive		= (int)dPara[E_PARA_POINT_DUST_GROUP_MIN_AREA_ACTIVE];
	int nSetEdgeCount		= (int)dPara[E_PARA_POINT_DUST_GROUP_EDGE_COUNT];
	int nSetActiveCount		= (int)dPara[E_PARA_POINT_DUST_GROUP_ACTIVE_COUNT];
	int nSetTotalCount		= (int)dPara[E_PARA_POINT_DUST_GROUP_TOTAL_COUNT];

	// 사용 안하는 경우
	//if( nFlag <= 0 )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( nFlag <= 0 )		return E_ERROR_CODE_TRUE;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// 미리 할당
	cv::Mat matStats, matCentroid;
	cv::Mat matLabel = cMatBufTemp.GetMat(matSrcBuf.size(), CV_32SC1, false);

	// Label 연산
	__int64 nTotalLabel = 0;

	if( matSrcBuf.type() == CV_8U )
	{
		nTotalLabel = cv::connectedComponentsWithStats(matSrcBuf, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA);
	}
	else
	{
		cv::Mat matSrc8bit = cMatBufTemp.GetMat(matSrcBuf.size(), CV_8UC1, false);
		matSrcBuf.convertTo(matSrc8bit, CV_8UC1, 1./16.);

		nTotalLabel = cv::connectedComponentsWithStats(matSrc8bit, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA);

		matSrc8bit.release();
	}

	// 영역 불량 갯수
	__int64 nEdgeCount		= 0;
	__int64 nActiveCount	= 0;

	// Total 영역 ( 무시 영역 Offset )
	CRect rectTotalArea(rectROI.left + nDel, rectROI.top + nDel, rectROI.right - nDel, rectROI.bottom - nDel);

	// Active 영역
	CRect rectActiveArea(rectTotalArea.left + nEdgeArea, rectTotalArea.top + nEdgeArea, rectTotalArea.right - nEdgeArea, rectTotalArea.bottom - nEdgeArea);

	// Dust 불량 좌표
	CPoint ptCenter;

	// Dust 갯수 만큼...
	for( int idx=1 ; idx<nTotalLabel ; idx++ )
	{
		// Center Point 구하기
		ptCenter.x	= (int)matCentroid.at<double>(idx, 0);
		ptCenter.y	= (int)matCentroid.at<double>(idx, 1);
		int nArea	= (int)matStats.at<int>(idx, CC_STAT_AREA);

		// Edge 외곽 무시 영역 밖 패스
		if ( !rectTotalArea.PtInRect(ptCenter) )
			continue;

		// Active 범위 안에 불량 존재?
		if ( rectActiveArea.PtInRect(ptCenter) )
		{
			// 설정한 불량 크기보다 작은 경우 패스
			if( nMinAreaActive > nArea )
				continue;

			// Active 영역 안에 존재하는 경우
			nActiveCount++;
		}
		// Edge 범위 안 불량
		else
		{
			// 설정한 불량 크기보다 작은 경우 패스
			if( nMinAreaEdge > nArea )
				continue;

			// Edge 영역 안에 존재하는 경우
			nEdgeCount++;
		}
	}

	// Active 설정 체크 ( E_DEFECT_JUDGEMENT_DUST_GROUP 불량 추가 )
	if (nActiveCount > nSetActiveCount)
		nErrorCode = E_ERROR_CODE_POINT_JUDEGEMENT_E;

	// Edge 설정 체크 ( E_DEFECT_JUDGEMENT_DUST_GROUP 불량 추가 )
	if (nEdgeCount > nSetEdgeCount)
		nErrorCode = E_ERROR_CODE_POINT_JUDEGEMENT_E;

	// Total 설정 체크 ( E_DEFECT_JUDGEMENT_DUST_GROUP 불량 추가 )
	if (nActiveCount + nEdgeCount > nSetTotalCount)
		nErrorCode = E_ERROR_CODE_POINT_JUDEGEMENT_E;

	//CString str;
	//str.Format(_T("E%d, A%d"), nEdgeCount, nActiveCount);
	//AfxMessageBox(str);

	// 해제
	matStats.release();
	matCentroid.release();
	matLabel.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}


// 원본 이미지에 RGB 패턴중 한개를 넣어서 보정치를 주는 기능을 한다 -> Gray 에서 사용
long CInspectPoint::AdjustImageWithRGB(cv::Mat& matSrcImage, cv::Mat& matDstImage, cv::Mat& matAdjustSrcImage, double dblAdjustRatio, int nCutMinGVForAdjust, CRect rectROI, CMatBuf* cMemSub)
{
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// 영상 크기가 다른 경우 사용 불가
	if( matSrcImage.rows != matAdjustSrcImage.rows )	return E_ERROR_CODE_TRUE;
	if( matSrcImage.cols != matAdjustSrcImage.cols )	return E_ERROR_CODE_TRUE;

	cv::Rect rcROI;
	rcROI.x			= rectROI.left;
	rcROI.y			= rectROI.top;
	rcROI.width		= rectROI.Width();
	rcROI.height	= rectROI.Height();

	// Temp 메모리 할당	
	cv::Mat matTempBuf			= cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);

	// ROI 조정
	cv::Mat matSrcROIImage		= matSrcImage(rcROI);
	cv::Mat matDstROIImage		= matTempBuf(rcROI);
	cv::Mat matAdjSrcROIImage	= matAdjustSrcImage(rcROI);

	cv::Mat matAdjImage			= cMatBufTemp.GetMat(matSrcROIImage.size(), matSrcROIImage.type(), false);
	cv::Mat matAdjImageBackup	= cMatBufTemp.GetMat(matSrcROIImage.size(), matSrcROIImage.type(), false);

	double dblAveSrc, dblAveAdjustSrc, dblApplyRatio;
	CString strTemp;	

	///////////////////////////////////////////////////////////////////
	// 참조 이미지를 구한다. 각 픽셀의 밝기가 입력된 Gray 이미지와 동일하게 맞추어 준다.
	////////////////////////////////////////////////////////////////////

	// 원본의 평균과 보정 시 참조할 이미지의 평균을 구한다.
	dblAveSrc		= GetAverage(matSrcROIImage);

	// 각 픽셀들의 평균에 근접한 평균을 구한다.
	dblAveAdjustSrc	= GetAverageForRGB(matAdjSrcROIImage);

	// 상대값을 구한다.
	dblApplyRatio	= dblAveSrc / dblAveAdjustSrc;

	// 보정치를 적용한 참조 이미지를 구한다. - 각 픽셀의 밝기가 입력된 Gray 이미지와 동일하게 맞추어 주는 기능
	matAdjImage		= matAdjSrcROIImage * dblApplyRatio;

	// 보정치 적용 이미지 백업 - 나중에 빈 구멍을 채울때 사용한다.
	matAdjImage.copyTo(matAdjImageBackup);

	/////////////////////////////////////////////////////////////////////
	// 최종 구해진 보정이미지에 사용자 보정치를 적용한다.
	/////////////////////////////////////////////////////////////////////

	// 사용자 보정치 적용
	matAdjImage *= dblAdjustRatio; 

	// 최소 GV 값 끊어내기 (픽셀 사이사이의 밝은 영역이 방해가됨)
	matAdjImage -= nCutMinGVForAdjust;

//  	/////////////////////////////
//  	// 보정치 이미지 적용
//  	/////////////////////////////
//  	matDstROIImage = matSrcROIImage - matAdjImage;
//  
//  	/////////////////////////////
//  	// 보정치 이미지 적용후 사이사이 검은 구멍을 메꾼다.
//  	/////////////////////////////
//  	cv::max(matDstROIImage, matAdjImageBackup, matDstROIImage);
//  

 	matDstROIImage = matSrcROIImage - matAdjImageBackup;
 	cv::max(matDstROIImage, matAdjImage, matDstROIImage);

	// 최종 결과 복사
	matTempBuf.copyTo(matDstImage);

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// PNZ Pattern Substraction Test
long CInspectPoint::PatternSubstraction(cv::Mat& matSrcImage, cv::Mat* matDstImage, int type, CMatBuf* cMemSub)
{
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// Temp 메모리 할당
	cv::Mat matTempBuf_Temp		= cMatBufTemp.GetMat(matDstImage[E_IMAGE_CLASSIFY_AVI_R].size(), matDstImage[E_IMAGE_CLASSIFY_AVI_R].type(), false);
	cv::Mat matTempBuf_Output	= cMatBufTemp.GetMat(matDstImage[E_IMAGE_CLASSIFY_AVI_B].size(), matDstImage[E_IMAGE_CLASSIFY_AVI_B].type(), false);
	


	if( type == E_IMAGE_CLASSIFY_AVI_R)
	{
		matTempBuf_Temp		= matDstImage[E_IMAGE_CLASSIFY_AVI_R] - matDstImage[E_IMAGE_CLASSIFY_AVI_G];
		matTempBuf_Output	= matTempBuf_Temp - matDstImage[E_IMAGE_CLASSIFY_AVI_B];

		

	}

	else if( type == E_IMAGE_CLASSIFY_AVI_G)
	{
		matTempBuf_Temp		= matDstImage[E_IMAGE_CLASSIFY_AVI_G] - matDstImage[E_IMAGE_CLASSIFY_AVI_R];
		matTempBuf_Output	= matTempBuf_Temp - matDstImage[E_IMAGE_CLASSIFY_AVI_B];
	}

	else if( type == E_IMAGE_CLASSIFY_AVI_B)
	{
		matTempBuf_Temp		= matDstImage[E_IMAGE_CLASSIFY_AVI_B] - matDstImage[E_IMAGE_CLASSIFY_AVI_G];
		matTempBuf_Output	= matTempBuf_Temp - matDstImage[E_IMAGE_CLASSIFY_AVI_R];
	}

	// 최종 결과 복사
	//matTempBuf.copyTo(matDstImage);
// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// 평균 GV값을 설정한 GV가 되도록 설정한다.
// 알고리즘 특성상 평균 GV 값이 많이 차이가 나게되면 Threshold 문제가 생김
void CInspectPoint::ApplyMeanGV(Mat &matSrcImage, double dblTargetGV)
{
	double dblMean;
	cv::Scalar m = cv::mean(matSrcImage);
	dblMean = m[0];

	matSrcImage *= dblTargetGV/dblMean;
}

void CInspectPoint::ApplyEnhancement(cv::Mat matSrcImage, cv::Mat matBuff1, cv::Mat matBuff2, cv::Mat& matDstImage1, cv::Mat& matDstImage2,
	double* dPara, int* nCommonPara, CString strAlgPath, int Type, CMatBuf* cMemSub)
{
	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	bool	bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	int&	nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	// 사용하는 Parameter
	double	dSubRatio1		;
	double	dMulRatio1		;
	double	dSubRatio2		;
	double	dMulRatio2		;
	double	dSubRatioTemp	;
	double	dMulRatioTemp	;

	if ( Type == E_IMAGE_CLASSIFY_AVI_R)
	{
		dSubRatio1		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_GREEN_SUB_RATIO	];
		dMulRatio1		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_GREEN_MLT_RATIO	];
		dSubRatio2		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_BLUE_SUB_RATIO	];
		dMulRatio2		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_BLUE_MLT_RATIO	];

		dSubRatioTemp	= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_RED_SUB_RATIO	];
		dMulRatioTemp	= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_RED_MLT_RATIO	];
	}

	if ( Type == E_IMAGE_CLASSIFY_AVI_G)
	{
		dSubRatio1		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_RED_SUB_RATIO	];
		dMulRatio1		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_RED_MLT_RATIO	];
		dSubRatio2		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_BLUE_SUB_RATIO	];
		dMulRatio2		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_BLUE_MLT_RATIO	];

		dSubRatioTemp	= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_GREEN_SUB_RATIO	];
		dMulRatioTemp	= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_GREEN_MLT_RATIO	];
	}

	if ( Type == E_IMAGE_CLASSIFY_AVI_B)
	{
		dSubRatio1		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_RED_SUB_RATIO	];
		dMulRatio1		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_RED_MLT_RATIO	];
		dSubRatio2		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_GREEN_SUB_RATIO	];
		dMulRatio2		= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_GREEN_MLT_RATIO	];

		dSubRatioTemp	= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_BLUE_SUB_RATIO	];
		dMulRatioTemp	= (double)	dPara[E_PARA_POINT_RGB_BRIGHT_INSP_BLUE_MLT_RATIO	];
	}

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// Temp 메모리 할당
	cv::Mat matTempBuf1	= cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	cv::Mat matTempBuf2	= cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);

	//Pattern Sub for Buff1, Buff2SubPatternEhancemnt(
	SubPatternEhancemnt(matBuff1, matTempBuf1, dSubRatio1, dMulRatio1, nCommonPara, strAlgPath, &cMatBufTemp);
	SubPatternEhancemnt(matBuff2, matTempBuf2, dSubRatio2, dMulRatio2, nCommonPara, strAlgPath, &cMatBufTemp);

	SubPatternEhancemnt(matSrcImage, matDstImage2, dSubRatioTemp, dMulRatioTemp, nCommonPara, strAlgPath, &cMatBufTemp);

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_SubPattern1.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf1);
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_SubPattern2.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf2);
	}

	matDstImage1 = matDstImage2 - matTempBuf1 - matTempBuf2;

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_SubPattern3.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage1);
	}

	matTempBuf1.release();
	matTempBuf2.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

}

void CInspectPoint::SubPatternEhancemnt(cv::Mat matSrcImage, cv::Mat& matDstImage, double dSubWeight, double dEnhanceWeight, int* nCommonPara, CString strAlgPath, CMatBuf* cMemSub)
{
	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	bool	bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	int&	nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// Temp 메모리 할당
	cv::Mat matTempBuf1	= cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);

	double ImagelMean;
	cv::Scalar m = cv::mean(matSrcImage);
	ImagelMean = m[0];

	matTempBuf1 = matSrcImage - ImagelMean * dSubWeight;
	matDstImage = matTempBuf1 * dEnhanceWeight;

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Enhancem1.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matTempBuf1);
		strTemp.Format(_T("%s\\%02d_%02d_%02d_POINT_%02d_Enhancem2.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstImage);
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}
}

// 실제 점등된 Cell 만 추출하여 평균값을 낸 것과 근접한 효과를 낸다.
double CInspectPoint::GetAverageForRGB(cv::Mat& matSrcImage)
{
	double dblRet			= 0;
	cv::Mat matHisto;
	int nMinGV				= 0;
	int nMaxGV				= 256;
	int nAveCutOffCount_Min	= 0;
	float *pVal				= NULL;
	double dblValSum		= 0;
	double dblCountSum		= 0;

	// Bit GV Max
	int nHistSize = 0;
	if( matSrcImage.type() == CV_8U )
		nHistSize = 256;
	else
		nHistSize = 4096;

	nMaxGV = nHistSize;

//	// 이미지의 전체 평균을 구한다.
//	cv::Scalar m = cv::mean(matSrcImage);
//	nMinGV = (int)m[0];

	// 히스토그램
	GetHistogram(matSrcImage, matHisto);	

	// 최소 평균을 구한다. - 이미지의 전체 평균을 구한다.	
	double tmpMinGV;
	GetAverage_From_Histo(matHisto,0,nHistSize,tmpMinGV);
	nMinGV = (int)tmpMinGV;

	// 최소 값이 적용된 평균을 구한다.
	dblValSum	= 0;
	dblCountSum	= 0;
	pVal = ((float*)matHisto.data) + nMinGV;

	// 시작범위를 이미지 전체 평균으로 대체한다.
	for(int i = nMinGV ; i<nMaxGV ; i++)
	{		
		dblValSum	+= (*pVal * i);
		dblCountSum	+= *pVal;
		pVal++;
	}

	dblRet = dblValSum / dblCountSum;

	return dblRet;
}

//이미지의 전체 평균을 구한다.
double CInspectPoint::GetAverage(Mat& matSrcImage)
{	
	cv::Scalar m = cv::mean(matSrcImage);

	return  (double)(m[0]);
}


// Dust에서 면적이 큰 경우 찾기
long CInspectPoint::FindBigAreaDust(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, long nFindArea, CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우.
	if( matSrcBuf.empty() )				return E_ERROR_CODE_EMPTY_BUFFER;

	// 8bit만 대응
	//if( matSrcBuf.type() != CV_8U )		return E_ERROR_CODE_TRUE;

	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;	

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	cv::Mat matLabelBuffer = cMatBufTemp.GetMat(matSrcBuf.size(), CV_32SC1, false);

	matSrcBuf.convertTo(matLabelBuffer, CV_32SC1);
	matDstBuf.setTo(0);

	// 검사 영역 Rect
	cv::Rect rectTemp;
	int nTempCount = 0;

	// 8 bit
	if( matDstBuf.type() == CV_8U )
	{
		for(int y=0 ; y<matLabelBuffer.rows ; y++)
		{
			int *row = (int*)matLabelBuffer.ptr(y);

			for(int x=0 ; x<matLabelBuffer.cols ; x++)
			{
				if(row[x] != 255)	continue;

				nTempCount++;

				// 객체 면적
				long nArea = cv::floodFill(matLabelBuffer, Point(x, y), nTempCount, &rectTemp);

				// 면적 제한
				if( nArea <  nFindArea )	continue;

				int nEndXX = rectTemp.x + rectTemp.width;
				int nEndYY = rectTemp.y + rectTemp.height;

				// 예외처리
				if( nEndYY >= matLabelBuffer.rows )	nEndYY = matLabelBuffer.rows - 1;
				if( nEndXX >= matLabelBuffer.cols )	nEndXX = matLabelBuffer.cols - 1;

				// 해당 라벨 각각 픽셀 좌표
				for(int j=rectTemp.y ; j<=nEndYY ; j++)
				{
					int		*row2 = (int *)matLabelBuffer.ptr(j);
					BYTE	*row3 = (BYTE *)matDstBuf.ptr(j);

					for(int i=rectTemp.x ; i<=nEndXX ; i++)
					{
						if(row2[i] != nTempCount)	continue;

						row3[i] = (BYTE)255;
					}
				}	
			}
		}
	}
	// 12 bit
	else
	{
		for(int y=0 ; y<matLabelBuffer.rows ; y++)
		{
			int *row = (int*)matLabelBuffer.ptr(y);

			for(int x=0 ; x<matLabelBuffer.cols ; x++)
			{
				if(row[x] != 4095)	continue;

				nTempCount++;

				// 객체 면적
				long nArea = cv::floodFill(matLabelBuffer, Point(x, y), nTempCount, &rectTemp);

				// 면적 제한
				if( nArea <  nFindArea )	continue;

				int nEndXX = rectTemp.x + rectTemp.width;
				int nEndYY = rectTemp.y + rectTemp.height;

				// 예외처리
				if( nEndYY >= matLabelBuffer.rows )	nEndYY = matLabelBuffer.rows - 1;
				if( nEndXX >= matLabelBuffer.cols )	nEndXX = matLabelBuffer.cols - 1;

				// 해당 라벨 각각 픽셀 좌표
				for(int j=rectTemp.y ; j<=nEndYY ; j++)
				{
					int		*row2 = (int *)matLabelBuffer.ptr(j);
					ushort	*row3 = (ushort *)matDstBuf.ptr(j);

					for(int i=rectTemp.x ; i<=nEndXX ; i++)
					{
						if(row2[i] != nTempCount)	continue;

						row3[i] = (ushort)4095;
					}
				}	
			}
		}
	}

	matLabelBuffer.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}

// 암점 - Dust 면적 큰 주변인 경우 제거  ( 8bit & 12bit )
long CInspectPoint::DeleteCompareDarkPoint(cv::Mat& matSrcBuffer, int nOffset, stDefectInfo* pResultBlob, int nModePS)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 파라미터 NULL인 경우
	if( matSrcBuffer.empty() )		return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )		return E_ERROR_CODE_EMPTY_PARA;

	// 검출된 불량 개수
	int nDefectCount = pResultBlob->nDefectCount;

	// 불량 개수 만큼...
	for (int i=0 ; i<nDefectCount ; )
	{
		// 암점 불량이 아닌경우 제외
		if( pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_POINT_DARK		&&
			pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_POINT_GROUP_DARK	)
		{
			i++;
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
		rect.x		*= nModePS;
		rect.y		*= nModePS;
		rect.width	*= nModePS;
		rect.height	*= nModePS;

		// 영상 크기에 맞게 조절하여 비교 ( nModePS )
		//rect.x		/= nModePS;
		//rect.y		/= nModePS;
		//rect.width	/= nModePS;
		//rect.height	/= nModePS;

		// nOffset 확장
		rect.x		-= nOffset;
		rect.y		-= nOffset;
		rect.width	+= (nOffset + nOffset);
		rect.height	+= (nOffset + nOffset);

		// 예외 처리
		if( rect.x < 0 )	rect.x = 0;
		if( rect.y < 0 )	rect.y = 0;
		if( rect.x + rect.width	 >= matSrcBuffer.cols )	rect.width  = matSrcBuffer.cols - rect.x - 1;
		if( rect.y + rect.height >= matSrcBuffer.rows )	rect.height = matSrcBuffer.rows - rect.y - 1;

		// 해당 ROI 따기
		cv::Mat matTempBuf = matSrcBuffer(rect);

		//////////////////////////////////////////////////////////////////////////
		// 암점 주변에서 Dust 존재하는지 확인
		//////////////////////////////////////////////////////////////////////////

		// 암점 주변 -> Dust 영상 Max GV 찾기
		double valMax;
		cv::minMaxLoc(matTempBuf, NULL, &valMax);

		// Dust 확인 시, 큰 불량이 없는 경우 ( 큰 불량 존재 시, 255 / 4096 값이 존재 )
		if( valMax == 0 )
		{
			i++;
			continue;
		}

		// 해당 암점 제거
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

		// 마지막 Index 초기화 필요한지는 확인 필요 ( 안해도 될듯??? )
	}

	// 최종 불량 개수 재설정
	pResultBlob->nDefectCount = nDefectCount;

	return nErrorCode;
}

// 어두운 Dust 면적 작은 경우만 찾기 ( 8bit )
long CInspectPoint::DarkDustMaxArea(cv::Mat matSrcBuffer[E_DEFECT_COLOR_COUNT], int nMaxArea, CMatBuf* cMemSub)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 파라미터 NULL인 경우
	if( matSrcBuffer[E_DEFECT_COLOR_BRIGHT].empty() )	return E_ERROR_CODE_EMPTY_PARA;
	if( matSrcBuffer[E_DEFECT_COLOR_DARK].empty() )		return E_ERROR_CODE_EMPTY_PARA;

	// 8bit만 대응
	//if( matSrcBuffer[E_DEFECT_COLOR_DARK].type() != CV_8U )		return E_ERROR_CODE_TRUE;

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	cv::Mat matLabelBuffer = cMatBufTemp.GetMat(matSrcBuffer[E_DEFECT_COLOR_DARK].size(), CV_32SC1, false);

	matSrcBuffer[E_DEFECT_COLOR_DARK].convertTo(matLabelBuffer, CV_32SC1);
	
	// 검사 영역 Rect
	cv::Rect rectTemp;
	int nTempCount = 0;

	// 8 bit
	if( matSrcBuffer[E_DEFECT_COLOR_BRIGHT].type() == CV_8U )
	{
		for(int y=0 ; y<matLabelBuffer.rows ; y++)
		{
			int *row = (int*)matLabelBuffer.ptr(y);

			for(int x=0 ; x<matLabelBuffer.cols ; x++)
			{
				if(row[x] != 255)	continue;

				nTempCount++;

				// 객체 면적
				long nArea = cv::floodFill(matLabelBuffer, Point(x, y), nTempCount, &rectTemp);

				// 면적 제한
				if( nArea > nMaxArea )	continue;

				int nEndXX = rectTemp.x + rectTemp.width;
				int nEndYY = rectTemp.y + rectTemp.height;

				// 예외처리
				if( nEndYY >= matLabelBuffer.rows )	nEndYY = matLabelBuffer.rows - 1;
				if( nEndXX >= matLabelBuffer.cols )	nEndXX = matLabelBuffer.cols - 1;

				// 해당 라벨 각각 픽셀 좌표
				for(int j=rectTemp.y ; j<=nEndYY ; j++)
				{
					int		*row2 = (int *)matLabelBuffer.ptr(j);
					BYTE	*row3 = (BYTE *)matSrcBuffer[E_DEFECT_COLOR_BRIGHT].ptr(j);

					for(int i=rectTemp.x ; i<=nEndXX ; i++)
					{
						if(row2[i] != nTempCount)	continue;

						row3[i] = (BYTE)255;
					}
				}
			}
		}
	}
	// 12 bit
	else
	{
		for(int y=0 ; y<matLabelBuffer.rows ; y++)
		{
			int *row = (int*)matLabelBuffer.ptr(y);

			for(int x=0 ; x<matLabelBuffer.cols ; x++)
			{
				if(row[x] != 4095)	continue;

				nTempCount++;

				// 객체 면적
				long nArea = cv::floodFill(matLabelBuffer, Point(x, y), nTempCount, &rectTemp);

				// 면적 제한
				if( nArea > nMaxArea )	continue;

				int nEndXX = rectTemp.x + rectTemp.width;
				int nEndYY = rectTemp.y + rectTemp.height;

				// 예외처리
				if( nEndYY >= matLabelBuffer.rows )	nEndYY = matLabelBuffer.rows - 1;
				if( nEndXX >= matLabelBuffer.cols )	nEndXX = matLabelBuffer.cols - 1;

				// 해당 라벨 각각 픽셀 좌표
				for(int j=rectTemp.y ; j<=nEndYY ; j++)
				{
					int		*row2 = (int *)matLabelBuffer.ptr(j);
					ushort	*row3 = (ushort *)matSrcBuffer[E_DEFECT_COLOR_BRIGHT].ptr(j);

					for(int i=rectTemp.x ; i<=nEndXX ; i++)
					{
						if(row2[i] != nTempCount)	continue;

						row3[i] = (ushort)4095;
					}
				}
			}
		}
	}	

	matLabelBuffer.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}

// 작은 면적 제거하기
long CInspectPoint::DeleteMinArea(cv::Mat matSrcBuffer, cv::Mat matThBuffer, int nMinArea, int nMaxGV, CMatBuf* cMemSub)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 파라미터 NULL인 경우
	if( matSrcBuffer.empty() )	return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThBuffer.empty() )	return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	cv::Mat matLabelBuffer = cMatBufTemp.GetMat(matThBuffer.size(), CV_32SC1, false);

	// 비트 변환
	matThBuffer.convertTo(matLabelBuffer, CV_32SC1);

	// 0으로 초기화
	matThBuffer.setTo(0);

	// 검사 영역 Rect
	cv::Rect rectTemp;
	int nTempCount = 0;

	// 8 bit
	if( matThBuffer.type() == CV_8U )
	{
		for(int y=0 ; y<matLabelBuffer.rows ; y++)
		{
			int *row = (int*)matLabelBuffer.ptr(y);

			for(int x=0 ; x<matLabelBuffer.cols ; x++)
			{
				if(row[x] != 255)	continue;

				nTempCount++;

				// 객체 면적
				long nArea = cv::floodFill(matLabelBuffer, Point(x, y), nTempCount, &rectTemp);

				// 면적 제한
				if( nArea < nMinArea )	continue;

				int nEndXX = rectTemp.x + rectTemp.width;
				int nEndYY = rectTemp.y + rectTemp.height;

				// 예외처리
				if( nEndYY >= matLabelBuffer.rows )	nEndYY = matLabelBuffer.rows - 1;
				if( nEndXX >= matLabelBuffer.cols )	nEndXX = matLabelBuffer.cols - 1;

				double dMax = 0;
				cv::Mat matTempBuf = matSrcBuffer( rectTemp );
				cv::minMaxLoc(matTempBuf, NULL, &dMax);
				if( dMax > nMaxGV )		continue;

				// 해당 라벨 각각 픽셀 좌표
				for(int j=rectTemp.y ; j<=nEndYY ; j++)
				{
					int		*row2 = (int *)matLabelBuffer.ptr(j);
					BYTE	*row3 = (BYTE *)matThBuffer.ptr(j);

					for(int i=rectTemp.x ; i<=nEndXX ; i++)
					{
						if(row2[i] != nTempCount)	continue;

						row3[i] = (BYTE)255;
					}
				}
			}
		}
	}
	// 12 bit
	else
	{
		for(int y=0 ; y<matLabelBuffer.rows ; y++)
		{
			int *row = (int*)matLabelBuffer.ptr(y);

			for(int x=0 ; x<matLabelBuffer.cols ; x++)
			{
				if(row[x] != 4095)	continue;

				nTempCount++;

				// 객체 면적
				long nArea = cv::floodFill(matLabelBuffer, Point(x, y), nTempCount, &rectTemp);

				// 면적 제한
				if( nArea > nMinArea )	continue;

				int nEndXX = rectTemp.x + rectTemp.width;
				int nEndYY = rectTemp.y + rectTemp.height;

				// 예외처리
				if( nEndYY >= matLabelBuffer.rows )	nEndYY = matLabelBuffer.rows - 1;
				if( nEndXX >= matLabelBuffer.cols )	nEndXX = matLabelBuffer.cols - 1;

				double dMax = 0;
				cv::Mat matTempBuf = matSrcBuffer( rectTemp );
				cv::minMaxLoc(matTempBuf, NULL, &dMax);
				if( dMax > nMaxGV )		continue;

				// 해당 라벨 각각 픽셀 좌표
				for(int j=rectTemp.y ; j<=nEndYY ; j++)
				{
					int		*row2 = (int *)matLabelBuffer.ptr(j);
					ushort	*row3 = (ushort *)matThBuffer.ptr(j);

					for(int i=rectTemp.x ; i<=nEndXX ; i++)
					{
						if(row2[i] != nTempCount)	continue;

						row3[i] = (ushort)4095;
					}
				}
			}
		}
	}	

	matLabelBuffer.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}

long CInspectPoint::FindBubble_DustImage(cv::Mat matSrcbuffer, cv::Mat& matBubbleResult, cv::Rect rtROI, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath)
{
	bool bImgSave = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0 ? true : false;

	// 파라미터
	int nBlurSize		= dPara[E_PARA_POINT_DUST_BUBBLE_SRC_BLUR_SIZE];
	int nBKBlurSize		= dPara[E_PARA_POINT_DUST_BUBBLE_BK_BLUR_SIZE];
	float fResize		= dPara[E_PARA_POINT_DUST_BUBBLE_RESIZE];
	int nThreshold		= dPara[E_PARA_POINT_DUST_BUBBLE_THRESHOLD];
	int nCloseSize		= dPara[E_PARA_POINT_DUST_BUBBLE_CLOSE_SIZE];

	// 예외처리 ( 홀수 )
	if(nBlurSize % 2 == 0)		nBlurSize++;
	if(nBKBlurSize % 2 == 0)	nBKBlurSize++;
	if(nCloseSize % 2 == 0)		nCloseSize++;

	//////////////////////////////////////////////////////////////////////////

	// 점등 영역 ROI
	cv::Mat matSrcROIBuf	= matSrcbuffer(rtROI);
	
	if(bImgSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Dust_Bubble_Org.jpg"), strAlgPath);
		ImageSave(strTemp, matSrcROIBuf);
	}
	
	double dPow = 1.4;
	cv::Mat matPow = cMem[0]->GetMat(matSrcROIBuf.size(), matSrcROIBuf.type(), false);

	Pow(matSrcROIBuf, matPow, dPow, cMem[0]);

	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Pow."));

	// 리사이즈 0.25
	cv::Mat matResizeBuf = cMem[0]->GetMat(matSrcROIBuf.rows * fResize, matSrcROIBuf.cols * fResize, matSrcbuffer.type(), false);
	cv::resize(matPow, matResizeBuf, matResizeBuf.size());
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Resize."));

	// 버블을 검출하기 위한 배경영상
	cv::Mat matResizeTemp1Buf = cMem[0]->GetMat(matResizeBuf.size(), matResizeBuf.type(), false);
	cv::Mat matResizeTemp2Buf = cMem[0]->GetMat(matResizeBuf.size(), matResizeBuf.type(), false);
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Create Resize Buf."));

	// Blur
	cv::blur(matResizeBuf, matResizeTemp1Buf, cv::Size(nBlurSize, nBlurSize));		// Src
	cv::blur(matResizeBuf, matResizeTemp2Buf, cv::Size(nBKBlurSize, nBKBlurSize));	// BackGround

	if(bImgSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Dust_Bubble_Src.jpg"), strAlgPath);
		ImageSave(strTemp, matResizeTemp1Buf);
		
		strTemp.Format(_T("%s\\Dust_Bubble_BackGround.jpg"), strAlgPath);
		ImageSave(strTemp, matResizeTemp2Buf);
	}

	// 기포가 어두움? ( 배경에서 원본 빼기 )
	cv::subtract(matResizeTemp2Buf, matResizeTemp1Buf, matResizeBuf);
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("subtract."));

	// 이진화
	cv::threshold(matResizeBuf, matResizeTemp1Buf, nThreshold, 255, THRESH_BINARY);
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("threshold."));

	// 채우기
	cv::morphologyEx(matResizeTemp1Buf, matResizeTemp2Buf, MORPH_CLOSE, cv::getStructuringElement(MORPH_RECT, cv::Size(nCloseSize, nCloseSize)));
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Morp."));

	// 리사이즈 원복
	cv::Mat matResROIBuf = cMem[0]->GetMat(matSrcROIBuf.size(), matSrcROIBuf.type(), false);
	cv::resize(matResizeTemp2Buf, matResROIBuf, matResROIBuf.size());
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Resize."));
	
	// CHole 제거
	for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
	{
		cv::Point ptCHoleCenter;
		ptCHoleCenter.x = (rcCHoleROI[i].x + rcCHoleROI[i].width / 2) - rtROI.x;
		ptCHoleCenter.y = (rcCHoleROI[i].y + rcCHoleROI[i].height / 2) - rtROI.y;
		int nCHoleraius = (rcCHoleROI[i].width + rcCHoleROI[i].height) / 2;

		cv::circle(matResROIBuf, ptCHoleCenter, nCHoleraius, cv::Scalar(0), -1);
	}

	if(bImgSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Dust_Bubble_TH.jpg"), strAlgPath);
		ImageSave(strTemp, matResROIBuf);
	}

	// 검출된 기포 200GV로 채움 ( 점등 영역만 )
	matBubbleResult(rtROI).setTo(200, matResROIBuf);
	writeInspectLog(E_ALG_TYPE_AVI_POINT, __FUNCTION__, _T("Bubble Set."));

	return E_ERROR_CODE_TRUE;
} 

void CInspectPoint::Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset)
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

void CInspectPoint::Lipschitz_Fillter(cv::Mat& ip)
{
	int slope, slope1, p, p1, p2, p3, p4, maxz;

	static boolean m_Down = false;     //  
	static boolean m_TopHat = true;  // lower Lipschitz cover  
	double m_Slope = 3.0;                 // slope

	int *destPixels = new int[ip.rows * ip.cols];
	int *srcPixels = new int[ip.rows * ip.cols];
	uchar *tmpBytePixels = new uchar[ip.rows * ip.cols];

	int ImageHeight = ip.rows;
	int ImageWidth = ip.cols;

	cv::Rect m_roi;
	m_roi.x = 0;
	m_roi.y = 0;
	m_roi.width = ip.cols;
	m_roi.height = ip.rows;
	// 	int destPixels[nChannels][ip.rows * ip.cols];
	// 	int srcPixels[ip.channels()][ip.rows * ip.cols];
	// 	uchar tmpBytePixels[ip.channels()][ip.rows * ip.cols];
	//	short tmpShortPixels[ip.channels()][ip.rows * ip.cols];



	tmpBytePixels = (uchar*)ip.data;



	int sign = (m_Down ? 1 : -1);
	int topdown = (m_Down ? 0 : 255);

	for (int ij = 0; ij < ip.rows * ip.cols; ij++)
	{
		srcPixels[ij] = sign *(tmpBytePixels[ij] & 0xff);
		destPixels[ij] = srcPixels[ij];

	}


	slope = (int)(m_Slope);
	slope1 = (int)(slope * sqrt(2.0));
	//	maxz = m_channels;


	for (int y = 0; y < ip.rows; y++)   // rows
	{


		p2 = sign * (topdown + (sign)* slope);
		p3 = sign * (topdown + (sign)* slope1);
		for (int x = m_roi.x; x < m_roi.x + m_roi.width; x++) // columns
		{
			p = (p2 - slope);
			p1 = (p3 - slope1);
			if (p1 > p) p = p1;
			p3 = destPixels[x + ImageWidth * (max(y - 1, 0))];
			p1 = p3 - slope;
			if (p1 > p) p = p1;

			p4 = destPixels[min(x + 1, ImageWidth - 1) + ImageWidth * (max(y - 1, 0))];
			p1 = p4 - slope1;
			if (p1 > p) p = p1;

			p2 = srcPixels[x + ImageWidth * y];
			if (p > p2) {
				destPixels[x + ImageWidth * y] = p;
				p2 = p;
			}
		}

	}

	for (int y = m_roi.y + m_roi.height - 1; y >= m_roi.y; y--)   // rows
	{
		p2 = sign * (topdown + (sign)* slope);
		p3 = sign * (topdown + (sign)* slope1);
		for (int x = m_roi.x + m_roi.width - 1; x >= m_roi.x; x--)  // columns
		{
			p = (p2 - slope);
			p1 = (p3 - slope1);
			if (p1 > p) p = p1;

			p3 = destPixels[x + ImageWidth * (min(y + 1, ImageHeight - 1))];
			p1 = p3 - slope;
			if (p1 > p)   p = p1;

			p4 = destPixels[max(x - 1, 0) + ImageWidth * (min(y + 1, ImageHeight - 1))];
			p1 = p4 - slope1;
			if (p1 > p) p = p1;

			p2 = destPixels[x + ImageWidth * y];
			if (p > p2)
			{
				destPixels[x + ImageWidth * y] = p;
				p2 = p;
			}
		}

	}



	for (int ij = 0; ij < ImageHeight * ImageWidth; ij++)
	{
		if (m_TopHat)
		{
			tmpBytePixels[ij] = (m_Down ? (byte)(srcPixels[ij] - destPixels[ij] + 255)
				: (byte)(destPixels[ij] - srcPixels[ij]));
		}
		else
		{

			tmpBytePixels[ij] = (byte)(sign * destPixels[ij]);
		}

	}


// 	cv::Mat matResult = cv::Mat::zeros(ip.size(), ip.type());
// 
// 	for (int i = 0; i < ImageWidth * ImageHeight; i++) {
// 		matResult.data[i] = tmpBytePixels[i];
// 	}

	delete[] destPixels;
	delete[] srcPixels;
	
// 	matResult.copyTo(ip);
// 	matResult.release();

	//delete[] tmpBytePixels;

	// 	ByteProcessor bip = (ByteProcessor)ip;
	// 	bip.setPixels(tmpBytePixels[0]);
	// 	Mat M(2, 3, CV_32FC1); // warpAffine 연산에 사용되는 2*3 행렬
	// 	cv::Point2f center;
	// 	center.x = (float)ptCenter.x;
	// 	center.y = (float)ptCenter.y;
	// 	M = getRotationMatrix2D(center, Theta, 1.0);
	// 	cv::warpAffine(matResult, matResult, M, matResult.size()); // 회전
	// 
	// 	imwrite("D:\\Lipschitz_Fillter.bmp", matResult);


}

long CInspectPoint::Image_Pow(int ImgType, double dpow, Mat& InPutImage, Mat& OutPutImage)
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

void CInspectPoint::KalMan_Filter(cv::Mat* stack, int nStack_size, double percentvar, double gain) {
	cv::Mat ip = stack[0];
	int bitDepth = 0;
	if (ip.type()==CV_8UC1)
		bitDepth = 8;
	else if (ip.type() == CV_16UC1)
		bitDepth = 16;
	else if (ip.type() == CV_32FC1)
		bitDepth = 32;
	else
		return;

	int width = stack[0].cols;
	int height = stack[0].rows;
	int dimension = width*height;
	int stacksize = nStack_size;

	double* stackslice = new double[dimension];
	double* filteredslice = new double[dimension];
	double* noisevar = new double[dimension];
	double* average = new double[dimension];
	double* predicted = new double[dimension];
	double* predictedvar = new double[dimension];
	double* observed = new double[dimension];
	double* Kalman = new double[dimension];
	double* corrected = new double[dimension];
	double* correctedvar = new double[dimension];

	for (int i = 0; i < dimension; ++i)
		noisevar[i] = percentvar;
	predicted = toDouble((double*)stack[0].data, dimension);
	predictedvar = noisevar;

	for (int i = 1; i < stacksize; ++i) {
		//IJ.showProgress(i, stacksize);
		stackslice = toDouble((double*)stack[i].data, dimension);
		observed = toDouble(stackslice, dimension);
		for (int k = 0; k < dimension; ++k)
			Kalman[k] = predictedvar[k] / (predictedvar[k] + noisevar[k]);
		for (int k = 0; k < dimension; ++k)
			corrected[k] = gain*predicted[k] + (1.0 - gain)*observed[k] + Kalman[k] * (observed[k] - predicted[k]);
		for (int k = 0; k < dimension; ++k)
			correctedvar[k] = predictedvar[k] * (1.0 - Kalman[k]);
		predictedvar = correctedvar;
		predicted = corrected;

		
		stack[0].data = (uchar*)fromDouble(corrected, dimension);
	}
}


byte* CInspectPoint::fromDouble(double* darray, int nlenght) {
	
		byte* bytes = new byte[nlenght];
		for (int i = 0; i < nlenght; i++)
			bytes[i] = (byte)darray[i];
		return bytes;
}

double* CInspectPoint::toDouble(double* darray, int nlenght) {
	double* doubles = NULL;
	
		byte* bytes = (byte*)darray;
		doubles = new double[nlenght];
		for (int i = 0; i < nlenght; i++)
			doubles[i] = (bytes[i] & 0xff);
		
	return doubles;
}