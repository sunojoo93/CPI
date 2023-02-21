
/************************************************************************/
// Mura 불량 검출 관련 소스
// 수정일 : 18.05.31 - KSW
/************************************************************************/

#include "StdAfx.h"
#include "InspectMura3.h"




CInspectMura3::CInspectMura3(void)
{
	cMem = NULL;
	m_cInspectLibLog = NULL;
	m_strAlgLog = NULL;
	m_tInitTime = 0;
	m_tBeforeTime = 0;

	//////////////////////////////////////////////////////////////////////////
	//sz = 11;
	//////////////////////////////////////////////////////////////////////////
}


CInspectMura3::~CInspectMura3(void)
{
}


// Main 검사 알고리즘
long CInspectMura3::DoFindMuraDefect(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
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
	{

	}
	break;

	case E_IMAGE_CLASSIFY_AVI_BLACK:
	case E_IMAGE_CLASSIFY_AVI_PCD:
	{
		// 검사 안함
	}
	break;

	case E_IMAGE_CLASSIFY_AVI_DUST:
	{
		// Point에서 검사한 결과 사용
	}
	break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_32:
	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
	case E_IMAGE_CLASSIFY_AVI_GRAY_87:
	case E_IMAGE_CLASSIFY_AVI_GRAY_128:
	case E_IMAGE_CLASSIFY_AVI_WHITE:
	{
		// 백점 검출
		nErrorCode = LogicStart_SPOT(matSrcBuffer, matSrcBufferRGB, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath);
	}
	break;

	// 영상 번호가 잘못 들어온 경우.
	default:
		return E_ERROR_CODE_TRUE;
	}

	// 빈 영상이 아닌 경우
	if (!matDstImage[E_DEFECT_COLOR_BRIGHT].empty() &&
		!matDstImage[E_DEFECT_COLOR_DARK].empty())
	{
		// 점등 영역 이외 검출 제거 ( 라운드 영역 제거 )
		if (!matBKBuffer.empty())
		{
			cv::subtract(matDstImage[E_DEFECT_COLOR_BRIGHT], matBKBuffer, matBrightBuffer);		//메모리 할당
			cv::subtract(matDstImage[E_DEFECT_COLOR_DARK], matBKBuffer, matDarkBuffer);		//메모리 할당

			writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Copy CV Sub Result."));
		}
		// 결과 넘기기
		else
		{
			// 			matBrightBuffer	= matDstImage[E_DEFECT_COLOR_BRIGHT].clone();		//메모리 할당
			// 			matDarkBuffer	= matDstImage[E_DEFECT_COLOR_DARK].clone();			//메모리 할당

			matDstImage[E_DEFECT_COLOR_DARK].copyTo(matDarkBuffer);
			matDstImage[E_DEFECT_COLOR_BRIGHT].copyTo(matBrightBuffer);

			writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Copy Clone Result."));
		}

		// 할당 해제
		matDstImage[E_DEFECT_COLOR_BRIGHT].release();
		matDstImage[E_DEFECT_COLOR_DARK].release();
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return nErrorCode;
}


long CInspectMura3::DoFindMuraDefect2(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, cv::Mat& matDrawBuffer, wchar_t* strContourTxt)
{
	// 파라미터 NULL인 경우
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Inspect start."));
	////////////////////////////////////////////////////////////////////////// choi 05.13
	int JudgeSpot_Flag = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_FLAG];
	//////////////////////////////////////////////////////////////////////////
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
	if (rectROI.left < 0 ||
		rectROI.top < 0 ||
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

	writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Dst Buf Set."));

	// 영상 마다 알고리즘 다름
	switch (nAlgImgNum)
	{
	case E_IMAGE_CLASSIFY_AVI_R:
	case E_IMAGE_CLASSIFY_AVI_G:
	case E_IMAGE_CLASSIFY_AVI_B:
	{

	}
	break;

	case E_IMAGE_CLASSIFY_AVI_BLACK:
	case E_IMAGE_CLASSIFY_AVI_PCD:
	{
		// 검사 안함
	}
	break;

	case E_IMAGE_CLASSIFY_AVI_DUST:
	{
		// Point에서 검사한 결과 사용
	}
	break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_32:
	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
	case E_IMAGE_CLASSIFY_AVI_GRAY_87:
	case E_IMAGE_CLASSIFY_AVI_GRAY_128:
	case E_IMAGE_CLASSIFY_AVI_WHITE:
	{
		// 백점 검출
		nErrorCode = LogicStart_SPOT(matSrcBuffer, matSrcBufferRGB, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath);
	}
	break;

	// 영상 번호가 잘못 들어온 경우.
	default:
		return E_ERROR_CODE_TRUE;
	}

	// 빈 영상이 아닌 경우
	if (!matDstImage[E_DEFECT_COLOR_BRIGHT].empty() &&
		!matDstImage[E_DEFECT_COLOR_DARK].empty())
	{
		// 점등 영역 이외 검출 제거 ( 라운드 영역 제거 )
		if (!matBKBuffer.empty())
		{
			cv::subtract(matDstImage[E_DEFECT_COLOR_BRIGHT], matBKBuffer, matDstImage[E_DEFECT_COLOR_BRIGHT]);		//메모리 할당
			cv::subtract(matDstImage[E_DEFECT_COLOR_DARK], matBKBuffer, matDstImage[E_DEFECT_COLOR_DARK]);		//메모리 할당

			writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Copy CV Sub Result."));
		}
		// 결과 넘기기
// 		else
// 		{
// 			// 			matBrightBuffer	= matDstImage[E_DEFECT_COLOR_BRIGHT].clone();		//메모리 할당
// 			// 			matDarkBuffer	= matDstImage[E_DEFECT_COLOR_DARK].clone();			//메모리 할당
// 
// 			matDstImage[E_DEFECT_COLOR_DARK].copyTo(matDarkBuffer);
// 			matDstImage[E_DEFECT_COLOR_BRIGHT].copyTo(matBrightBuffer);
// 
// 			writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Copy Clone Result."));
// 		}

		///////////////////////////// woojin 19.08.28
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
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_MURA_3, m_tInitTime, m_tBeforeTime, m_strAlgLog);

		// E_DEFECT_COLOR_bright 결과
		// choikwangil 12.03 black hole
		switch (nAlgImgNum)
		{
		case E_IMAGE_CLASSIFY_AVI_WHITE:
		{
			//////////////////////////////////////////////////////////////////////////
 			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstImage[E_DEFECT_COLOR_BRIGHT](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
 				nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("M3_B"), EngineerDefectJudgment, pResultBlob);

			if (JudgeSpot_Flag == 1) {
				// White Spot 재분류
				JudgeWhiteSpot(matSrcBuffer, matDstImage[E_DEFECT_COLOR_BRIGHT], rectROI, dPara, nCommonPara, strAlgPath, pResultBlob, &cMatBufTemp);
				writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("JudgeWhiteSpot."));
			}

			if (!USE_ALG_CONTOURS)	// 결과 외곽선 저장
				cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

			// 결과 외곽선 그리기
			cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstImage[E_DEFECT_COLOR_DARK](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
				nCommonPara, E_DEFECT_COLOR_DARK, _T("M3_D"), EngineerDefectJudgment, pResultBlob);

			if (!USE_ALG_CONTOURS)	// 결과 외곽선 저장
				cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

			// 결과 외곽선 그리기
			cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);
			//////////////////////////////////////////////////////////////////////////
		}
		break;
		case E_IMAGE_CLASSIFY_AVI_GRAY_64:
		{
			//////////////////////////////////////////////////////////////////////////
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstImage[E_DEFECT_COLOR_BRIGHT](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
				nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("M3_B"), EngineerDefectJudgment, pResultBlob);

			if (JudgeSpot_Flag == 1) {
				// White Spot 재분류
				JudgeWhiteSpot(matSrcBuffer, matDstImage[E_DEFECT_COLOR_BRIGHT], rectROI, dPara, nCommonPara, strAlgPath, pResultBlob, &cMatBufTemp);
				writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("JudgeWhiteSpot."));
			}
			
			if (!USE_ALG_CONTOURS)	// 결과 외곽선 저장
				cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);
			
			// 결과 외곽선 그리기
			cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstImage[E_DEFECT_COLOR_DARK](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
				nCommonPara, E_DEFECT_COLOR_DARK, _T("M3_D"), EngineerDefectJudgment, pResultBlob);

			if (!USE_ALG_CONTOURS)	// 결과 외곽선 저장
				cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

			// 결과 외곽선 그리기
			cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);
			//////////////////////////////////////////////////////////////////////////
		}
		break;
		}

	
		// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 ) //choikwangil 04.07 추가 draw 버그 수정
		// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
		
		


		// 할당 해제
		matDstImage[E_DEFECT_COLOR_BRIGHT].release();
		matDstImage[E_DEFECT_COLOR_DARK].release();

		writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 End."));

// 		if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 		}

	}

	return nErrorCode;
}

void CInspectMura3::Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset)
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




// Dust 제거 후, 결과 벡터 넘겨주기
long CInspectMura3::GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
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
	if (matDustBuffer[E_DEFECT_COLOR_DARK].empty())	return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDustBuffer[E_DEFECT_COLOR_BRIGHT].empty())	return E_ERROR_CODE_EMPTY_BUFFER;


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



		writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("DeleteArea."));
	}
	// Dust 영상 버퍼 없는 경우
	// 제거 로직 없이 불량 추출
	else
	{
		writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Empty Dust Image."));
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
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_MURA_3, m_tInitTime, m_tBeforeTime, m_strAlgLog);

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
		writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("DoDefectBlobJudgment-Dark."));



		// Dark 불량 개수
		int nStartIndex = pResultBlob->nDefectCount;

		// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
		// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
		if (!USE_ALG_CONTOURS)	// 결과 외곽선 저장
			cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

		// 결과 외곽선 그리기
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

		//////////////////////////////////////////////////////////////////////////

		BOOL bUse[MAX_MEM_SIZE_E_DEFECT_NAME_COUNT];

		// 만약 Spot 검사 하는 경우
		if (EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse || EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse) // 04.16 choi
		{
			// 백점 하기 위해 이진화 ( 백점 : 255 / 백무라 200 )
			cv::threshold(matDstBuffer[E_DEFECT_COLOR_BRIGHT], matResBuf, 220, 255.0, CV_THRESH_BINARY);

			// 구분 파라미터 복사
			for (int p = 0; p < MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; p++)
			{
				bUse[p] = EngineerDefectJudgment[p].bDefectItemUse;

				// 모든 파라미터 검사 해제
				EngineerDefectJudgment[p].bDefectItemUse = false;
			}

			// Spot 검사만 설정
			EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse = true;
			EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse = true; // 04.16 choi

			// E_DEFECT_COLOR_BRIGHT 결과
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matResBuf(rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
				nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("BM_"), EngineerDefectJudgment, pResultBlob);
			if (nErrorCode != E_ERROR_CODE_TRUE)
			{
				// 메모리 해제
				matSrcBuffer.release();
				matDstBuffer[E_DEFECT_COLOR_DARK].release();
				matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

				return nErrorCode;
			}
			writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("DoDefectBlobJudgment-Bright Spot."));

			// White Spot 재분류
			//		JudgeWhiteSpot(matSrcBuffer, matDstBuffer[E_DEFECT_COLOR_BRIGHT], rectROI, dPara, nCommonPara, strAlgPath, pResultBlob, &cMatBufTemp);
			writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("JudgeWhiteSpot."));

			// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
			// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
			if (!USE_ALG_CONTOURS)	// 결과 외곽선 저장
				cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

			// 결과 외곽선 그리기
			cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

			// 원복
			for (int p = 0; p < MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; p++)
			{
				EngineerDefectJudgment[p].bDefectItemUse = bUse[p];
			}
		}

		//////////////////////////////////////////////////////////////////////////

		// 백무라 하기 위해 이진화 ( 백점 : 255 / 백무라 200 )
		cv::threshold(matDstBuffer[E_DEFECT_COLOR_BRIGHT], matResBuf, 190, 255.0, CV_THRESH_BINARY);

		// Spot 검사만 해제
		bUse[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT] = EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse;
		bUse[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT] = EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse; //04.16 choi

		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse = false;
		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse = false; //04.16 choi

		// E_DEFECT_COLOR_BRIGHT 결과
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matResBuf(rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("BM_"), EngineerDefectJudgment, pResultBlob);
		if (nErrorCode != E_ERROR_CODE_TRUE)
		{
			// 메모리 해제
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}
		writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("DoDefectBlobJudgment-Bright."));

		// 원복
		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse = bUse[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT];
		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse = bUse[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT]; // 04.16 choi


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

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}


long CInspectMura3::LogicStart_SPOT(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
	int* nCommonPara, CString strAlgPath)
{
	int nAlgImgNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];
	int nresize		 = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_RESIZE];
	int nblur_size = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_BLUR_SIZE];
	int nblur_sigma = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_BLUR_SIGMA];

	int	  nBright_Flag = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_BRIGHT_FLAG];							//choi_2104
	float fBrightRatio_RGB = (float)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_BRIGHT_RATIO];
	float fBrightRatio_RGB_Edge = (float)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_BRIGHT_EDGE_RATIO];

	int   nDark_Flag = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_DARK_FLAG];								//choi_2104
	float fDarkRatio_RGB = (float)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_DARK_RATIO];							//choi_2104
	float fDarkRatio_RGB_Edge = (float)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_DARK_EDGE_RATIO];				//choi_2104
	int   nDark_Minimum_Size = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_DARK_MINIMUM_SIZE];				//choi_2104

	int nSegX = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SEG_X];
	int nSegY = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SEG_Y];

	int nBright_Mexican_size = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_FILTER_SIZE];						//choi_2104
	int nBright_Mxblur_size = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_BLUR_SIZE];							//choi_2104
	int nBright_Mxblur_sigma = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_BLUR_SIGMA];						//choi_2104

	int nDark_Mexican_size = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_DARK_FILTER_SIZE];						//choi_2104
	int nDark_Mxblur_size = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_DARK_BLUR_SIZE];							//choi_2104
	int nDark_Mxblur_sigma = (int)dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MEXICAN_DARK_BLUR_SIGMA];						//choi_2104

	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;


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
	writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot Start."));
	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	// 검사 영역
	CRect rectTemp(rectROI);

	long	nWidth = (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcImage.rows;	// 영상 세로 크기

	////////////////////////////////////////////////////////////////////////// choi 06.24 변경
	cv::Mat matSrcROIBuf = cMatBufTemp.GetMat(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()).size(), matSrcImage.type(), false);
	cv::Mat matBrROIBuf = cMatBufTemp.GetMat(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()).size(), matDstImage[E_DEFECT_COLOR_BRIGHT].type(), false);
	cv::Mat matDaROIBuf = cMatBufTemp.GetMat(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()).size(), matDstImage[E_DEFECT_COLOR_DARK].type(), false);

	matSrcROIBuf = matSrcImage(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));
	matBrROIBuf = matDstImage[E_DEFECT_COLOR_BRIGHT](cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));
	matDaROIBuf = matDstImage[E_DEFECT_COLOR_DARK](cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));
	//////////////////////////////////////////////////////////////////////////
	

	// 배경 영역
	cv::Mat matBkROIBuf;
	if (!matBKBuffer.empty())
		matBkROIBuf = matBKBuffer(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA3_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcROIBuf);
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot Buf Set."));

	int nResizeWidth = matSrcROIBuf.cols / nresize;
	int nResizeHeight = matSrcROIBuf.rows / nresize;
	cv::Mat matResize = cMatBufTemp.GetMat(matSrcROIBuf.size(), matSrcROIBuf.type());

	cv::resize(matSrcROIBuf, matResize, cv::Size(nResizeWidth, nResizeHeight), 3, 3, INTER_AREA);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot Resize."));

	if (bImageSave)
	{

		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA3_%02d_resize.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResize);		//matResult 로 변경할것
	}


	cv::GaussianBlur(matResize, matResize, cv::Size(nblur_size, nblur_size), nblur_sigma, nblur_sigma);

	writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot Blur."));

	////////////////////////////////////////////////////////////////////////// choi 05.01 -> pwj 각 flag 안으로 이동
// 	C_Mexican_filter(matResize, nMexican_size,nMxblur_size,nMxblur_sigma);
// 
// 	writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot CM Filter."));
	//////////////////////////////////////////////////////////////////////////

// #pragma omp parallel
// 	{
// #pragma omp sections
// 		{
// #pragma omp section
// 			{
				if (nBright_Flag > 0) {

					cv::Mat mat_Br_result = cMatBufTemp.GetMat(matResize.size(), matResize.type());
					matResize.copyTo(mat_Br_result);

					nErrorCode = C_Mexican_filter(mat_Br_result, nBright_Mexican_size, nBright_Mxblur_size, nBright_Mxblur_sigma);

					writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot CM Filter."));

					if (bImageSave)
					{

						CString strTemp;
						strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA3_%02d_Br_Mexican_filter.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
						cv::imwrite((cv::String)(CStringA)strTemp, mat_Br_result);		//matResult 로 변경할것
					}

					CRect resize_Rect(0, 0, mat_Br_result.cols - 1, mat_Br_result.rows - 1);
					RangeAvgThreshold_Gray(mat_Br_result, mat_Br_result, resize_Rect, 1, nSegX, nSegY, fDarkRatio_RGB, fBrightRatio_RGB, fDarkRatio_RGB_Edge, fBrightRatio_RGB_Edge, 1, &cMatBufTemp); //choi 05.01

					writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot (Bright) RangeTH"));

					if (bImageSave)
					{

						CString strTemp;
						strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA3_%02d_WHITE_SPOT_Range_Threshold_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
						cv::imwrite((cv::String)(CStringA)strTemp, mat_Br_result);
					}


					cv::resize(mat_Br_result, matBrROIBuf, cv::Size(matSrcROIBuf.cols, matSrcROIBuf.rows), 3, 3, INTER_AREA);

					writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot (Bright) Resize"));

					if (bImageSave)
					{

						CString strTemp;
						strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA3_%02d_WHITE_SPOT_Bright_resize_INV.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
						cv::imwrite((cv::String)(CStringA)strTemp, matBrROIBuf);
					}

				}
//			}
// #pragma omp section
// 			{
				if (nDark_Flag > 0) {

					cv::Mat mat_Dark_result = cMatBufTemp.GetMat(matResize.size(), matResize.type());
					matResize.copyTo(mat_Dark_result);

					nErrorCode = C_Mexican_filter(mat_Dark_result, nDark_Mexican_size, nDark_Mxblur_size, nDark_Mxblur_sigma);

					writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot CM Filter."));



					if (bImageSave)
					{

						CString strTemp;
						strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA3_%02d_Da_Mexican_filter.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
						cv::imwrite((cv::String)(CStringA)strTemp, mat_Dark_result);		//matResult 로 변경할것
					}

					MinimumFilter(mat_Dark_result, mat_Dark_result, nDark_Minimum_Size);

					writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot (Dark) MinimumFilter"));


					if (bImageSave)
					{

						CString strTemp;
						strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA3_%02d_Da_Minimum_filter.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
						cv::imwrite((cv::String)(CStringA)strTemp, mat_Dark_result);		//matResult 로 변경할것
					}

					CRect resize_Rect(0, 0, mat_Dark_result.cols - 1, mat_Dark_result.rows - 1);
					RangeAvgThreshold_Gray(mat_Dark_result, mat_Dark_result, resize_Rect, 1, nSegX, nSegY, fDarkRatio_RGB, fBrightRatio_RGB, fDarkRatio_RGB_Edge, fBrightRatio_RGB_Edge, 0, &cMatBufTemp); //choi 05.01

					writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot (Dark) RangeTH"));

					if (bImageSave)
					{

						CString strTemp;
						strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA3_%02d_BLACK_SPOT_Range_Threshold_Dark.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
						cv::imwrite((cv::String)(CStringA)strTemp, mat_Dark_result);
					}


					cv::resize(mat_Dark_result, matDaROIBuf, cv::Size(matSrcROIBuf.cols, matSrcROIBuf.rows), 3, 3, INTER_AREA);

					writeInspectLog(E_ALG_TYPE_AVI_MURA_3, __FUNCTION__, _T("Mura3 Logic_Spot (Dark) Resize"));

					if (bImageSave)
					{

						CString strTemp;
						strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA3_%02d_BLACK_SPOT_Dark_resize_INV.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
						cv::imwrite((cv::String)(CStringA)strTemp, matDaROIBuf);
					}

				}
// 			}
// 		}
// 	}
// 				if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 					writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 					writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 				}

	return nErrorCode;
}


// 8bit & 12bit 영상 저장
long CInspectMura3::ImageSave(CString strPath, cv::Mat matSrcBuf)
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

bool CInspectMura3::OrientedBoundingBox(cv::RotatedRect& rect1, cv::RotatedRect& rect2)
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

void CInspectMura3::Filter8(BYTE* InImg, BYTE* OutImg,int nMin, int nMax , int width, int height)
{
	int i, j, k, m;
	long nmax, nmin, nsum;
	double dvalue;

		int coef[8]={13,14,20,14,7,-4,-7,-1};
	//	int coef[5]={48, 22,	20,	-2,	-8};
	//	int coef[3]={12, 1, -3};
	//	int coef[7]={6,	7,	9,	5,	2,	-3,	-2};
	//	int coef[4]={10, 18, -1, -3};
	//int coef[4] = { 42, 14, 7, -7 };
	//	int coef[16]={2, 6, 10, 15, 20, 13, 14, 18, 8, 2, -2, -3, -5,-5, -3, -2};
	//	int coef[26]={ 12,	22,	32,	41,	51,	60,	50,	45,	40,	42,	40,	35,	34,	20,	11,	11,	7,	-8,	-14,	-15,	-11,	-12,	-7,	-10,	-4,	-8};
		nmax = nMax;
		nmin = nMin;
	long* NewImg = new long[(width + 1)*(height + 1)];
	long* NewLoG = new long[(width + 1)*(height + 1)];

	//	memcpy(NewImg,InImg, width*height);
	memset(NewLoG, (long)0, width*height);


	for (i = 0;i < height;i++)
	{
		for (j = 0;j < width;j++)
		{
			NewImg[(i + 1)*(width + 1) + j + 1] = (long)InImg[i*width + j];
		}
		NewImg[(i + 1)*(width + 1)] = 0; // 이거 잘못 된것 같음. NewImg[i * width ] = 0;  이게 맞는듯? 
	}
	for (j = 0; j <= width; j++)
		NewImg[j] = 0;


	m = 8;

	width++; // width = width +1;
	height++; // height = height +1;
	for (j = 1;j < width;j++) NewImg[j] += NewImg[j - 1]; // 첫번째 가로줄에 값을 넣어주는데. 들어갈 값이 모두 0인데 왜 넣는 걸까? 바로 위에서 이미 0으로 초기화 하고있는데? 또 똑같은 행위를 하고 있음. 의미 없는 행위

	for (i = 1;i < height;i++) NewImg[i*width] += NewImg[(i - 1)*width]; // 이것도 상단에 NewImg[i * width ] = 0; 이렇게 변경해주면 의미 없는 코드임

	for (i = 1;i < height;i++)
		for (j = 1;j < width;j++)
			NewImg[i*width + j] += NewImg[(i - 1)*width + j] + NewImg[i*width + (j - 1)] - NewImg[(i - 1)*width + (j - 1)];

// 	nmax = -10000000; //필요 없는 내용
// 	nmin = 10000000;
	//	 	nmax=-12000; //필요 없는 내용
	//	 	nmin= 12000;


	for (i = m + 1;i < height - m;i++)
		for (j = m + 1;j < width - m;j++)
		{
			nsum = 0;

			//13,14,20,14,7,-4,-7,-1

			for (k = 0;k < m;k++)
				nsum += (long)(coef[k] * (NewImg[(i + k)*width + (j + k)] - NewImg[(i - (k + 1))*width + (j + k)] - NewImg[(i + k)*width + (j - (k + 1))] + NewImg[(i - (k + 1))*width + (j - (k + 1))]));
			//정사각형 모양에서 1,3 사분면 합에 2,4 사분면합을 빼준 값에다가 계수를 곱하여 3사분면 값으로 대체 함.

			NewLoG[i*width + j] = nsum;

			if (NewLoG[i*width + j] > nmax) nmax = NewLoG[i*width + j]; //NewLoG[i*width+j] = nmax; 
			if (NewLoG[i*width + j] < nmin) nmin = NewLoG[i*width + j];

		}
// 
// 	nmax = 8000; // Min, Max 값이 올라가면 평균 밝기가 내려감 (Gray scale)
// 	nmin = -8000;
	// 		nmax= 10000;
	// 		nmin=-10000;
	// min 값과 max 값은 다를 수 있다.
	for (i = 1;i < height;i++)
	{
		for (j = 1;j < width;j++)
		{
			if (i >= m && i < height - m && j >= m && j < width - m) {
				dvalue = ((double)((NewLoG[i*width + j] - nmin) * 255) / (double)(nmax - nmin));
				if (dvalue < 0.0f)			OutImg[(i - 1)*(width - 1) + (j - 1)] = (BYTE)0;
				else if (dvalue > 255.0f)	OutImg[(i - 1)*(width - 1) + (j - 1)] = (BYTE)255;
				else					OutImg[(i - 1)*(width - 1) + (j - 1)] = (BYTE)dvalue;
			}
			else						OutImg[(i - 1)*(width - 1) + (j - 1)] = (BYTE)127;

		}
	}

	delete[] NewImg;
	delete[] NewLoG;


}

long  CInspectMura3::C_Mexican_filter(cv::Mat& ip , int sz, int nMx_blur_sz, int nMx_blur_sigma) {
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

// float*  CInspectMura3::gauss1D(int r) {
// 	sz = 2 * r + 1;
// 	//final double sigma2=2*((double)r/3.5 +1/7.0)*((double)r/3.5 +1/7.0);
// 	double sigma2 = ((double)r / 3.0 + 1 / 6)*((double)r / 3.0 + 1 / 6.0);
// 	float* kernel;
// 	kernel = (float *)malloc(sz * sizeof(float));
// 	float sum = 0;
// 	double PIs = 1 / sqrt(2 * PI*sigma2);
// 	for (int u = -r; u <= r; u++) {
// 		double x2 = u*u;
// 		int idx = u + r;
// 		kernel[idx] = (float)(exp(-0.5*x2 / sigma2)*PIs);
// 
// 	}
// 	sum = abs(sum);
// 	if (sum < 1e-5) sum = 1;
// 	if (sum != 1) {
// 		for (int i = 0; i < sz; i++) {
// 			kernel[i] /= sum;
// 			//System.out.print(kernel[i] +" ");
// 		}
// 	}
// 	return kernel;
// }

float*  CInspectMura3::diff2Gauss1D(int r) {
	int sz = 2 * r + 1;
	double sigma2 = ((double)r / 3.0 + 1 / 6)*((double)r / 3.0 + 1 / 6.0);
	//int *num;

	float* kernel;
	kernel = (float *)malloc(sz * sizeof(float));
	//((w^2-r^2)*%e^(-r^2/(2*w^2)))/(2^(3/2)*sqrt(%pi)*w^4*abs(w))
	float sum = 0;
	double PIs = 1 / sqrt(2 * PI*sigma2);
	for (int u = -r; u <= r; u++) {
		double x2 = u*u;
		int idx = u + r;
		kernel[idx] = (float)((x2 - sigma2)*exp(-0.5*x2 / sigma2)*PIs);

	}
	sum = abs(sum);
	if (sum < 1e-5) sum = 1;
	if (sum != 1) {
		for (int i = 0; i < sz; i++) {
			kernel[i] /= sum;
			//System.out.print(kernel[i] +" ");
		}
	}
	return kernel;
}

float*  CInspectMura3::computeKernel2D(int r) {
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

boolean  CInspectMura3::convolveFloat(cv::Mat& ip, float* kernel, int kw, int kh , int sz) {

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
	pixels=ip;
	ip.copyTo(pixels2);

	double scale = getScale(kernel ,sz);
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

double CInspectMura3::getScale(float* kernel, int sz) {
	double scale = 1.0;
	double sum = 0.0;
	for (int i = 0; i < sz*sz; i++)
		sum += kernel[i];
	if (sum != 0.0)
		scale = 1.0 / sum;
	return scale;
}

long CInspectMura3::RangeAvgThreshold_Gray(cv::Mat& matSrcImage, cv::Mat& matDstImage, CRect rectROI,
	long nLoop, long nSegX, long nSegY, float fDarkRatio, float fBrightRatio, float fDarkRatio_Edge, float fBrightRatio_Edge,int Defect_Color_mode, CMatBuf* cMemSub)
{
	// 설정 값이 10보다 작은 경우.
	if (nSegX <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;
	if (nSegY <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;

	// 이미지 크기
	long	nWidth = (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcImage.rows;	// 영상 세로 크기

	long x, y;
	long nStart_X, nStart_Y, nEnd_X, nEnd_Y;

	long nPixelSum, nPixelCount, nPixelAvg;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// Temp 메모리 할당	
	cv::Mat matBlurBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	cv::Mat matBlurBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);

	int nBlur = 5;

	//연산량 감소목적
	//cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nBlur;
	//rtInspROI.y = rectROI.top - nBlur;
	//rtInspROI.width = rectROI.Width() + nBlur * 2;
	//rtInspROI.height = rectROI.Height() + nBlur * 2;

	//Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nBlur);

	if (nLoop > 0)
	{
		cv::blur(matSrcImage, matBlurBuf, cv::Size(nBlur, nBlur));

		if (nLoop > 1)
		{
			// Avg
			for (int i = 1; i < nLoop; i++)
			{
				cv::blur(matBlurBuf, matBlurBuf1, cv::Size(nBlur, nBlur));

				matBlurBuf1.copyTo(matBlurBuf);
			}
		}
	}
	else
	{
		matSrcImage.copyTo(matBlurBuf);
	}
	matBlurBuf1.release();

	//////////////////////////////////////////////////////////////////////////

	for (y = 0; y < nRangeY; y++)
	{
		// y 범위 계산
		nStart_Y = y * nSegY + rectROI.top;
		if (y == nRangeY - 1)		nEnd_Y = rectROI.bottom;
		else					nEnd_Y = nStart_Y + nSegY;

		for (x = 0; x < nRangeX; x++)
		{
			double dbDarkRatio = fDarkRatio;
			double dbBrightRatio = fBrightRatio;

		
			// x 범위 계산
			nStart_X = x * nSegX + rectROI.left;
			if (x == nRangeX - 1)		nEnd_X = rectROI.right;
			else					nEnd_X = nStart_X + nSegX;

			//Edge 부분
			if (nStart_X == rectROI.left || y == rectROI.top || nEnd_X == rectROI.right || nEnd_Y == rectROI.bottom)
			{
				dbDarkRatio = fDarkRatio_Edge;
				dbBrightRatio = fBrightRatio_Edge;
			}

			// 범위 설정
			cv::Rect rectTemp;
			rectTemp.x = nStart_X;
			rectTemp.y = nStart_Y;
			rectTemp.width = nEnd_X - nStart_X + 1;
			rectTemp.height = nEnd_Y - nStart_Y + 1;

			// 영상 ROI
			cv::Mat matTempBuf = matBlurBuf(rectTemp);

			// 히스토그램
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			double dblAverage;
			double dblStdev;

			if (Defect_Color_mode == 1) {
				GetMeanStdDev_From_Histo(matHisto, 0, 255, dblAverage, dblStdev);
			}
			if (Defect_Color_mode == 0) {
				GetMeanStdDev_From_Histo(matHisto, 0, 256, dblAverage, dblStdev);
			}

			// 평균 범위 설정
			int nMinGV = (int)(dblAverage - dblStdev);
			int nMaxGV = (int)(dblAverage + dblStdev);
			if (nMinGV < 0)	nMinGV = 0;
			if (nMaxGV > 255)	nMaxGV = 255;

			// 초기화
			nPixelSum = 0;
			nPixelCount = 0;
			nPixelAvg = 0;

			// 설정된 평균 범위만큼만 재평균
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m = nMinGV; m <= nMaxGV; m++, pVal++)
			{
				nPixelSum += (m * *pVal);
				nPixelCount += *pVal;
			}

			if (nPixelCount == 0)	continue;

			// 범위 내 평균
			nPixelAvg = (long)(nPixelSum / (double)nPixelCount);

			// 평균 * Ratio
			long nDarkTemp = (long)(nPixelAvg * dbDarkRatio);
			long nBrightTemp = (long)(nPixelAvg * dbBrightRatio);

			// Gray 너무 어두운 경우 있음.
			// ( 평균 GV 2 ~ 3 * fBrightRatio -> 이진화 : 노이즈 다 올라옴 )
			if (nBrightTemp < 15)	nBrightTemp = 15;

			// 예외 처리
			if (nDarkTemp < 0)		nDarkTemp = 0;
			if (nBrightTemp > 255)	nBrightTemp = 255;

			// 파라미터 0인경우 예외 처리
			if (dbDarkRatio == 0)	nDarkTemp = -1;
			if (dbBrightRatio == 0)	nBrightTemp = 256;

			// E_DEFECT_COLOR_DARK Threshold
			if (Defect_Color_mode == 1) {
				cv::Mat matTempBufT = matDstImage(rectTemp);
				cv::threshold(matTempBuf, matTempBufT, nBrightTemp, 255.0, THRESH_BINARY);
			}

			if (Defect_Color_mode == 0) {
				cv::Mat matTempBufT = matDstImage(rectTemp);
				cv::threshold(matTempBuf, matTempBufT, nDarkTemp, 255.0, THRESH_BINARY_INV);
			}
		}
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}


// void CInspectMura3::Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset)
// {
// 	rectInspROI.x = rectROI.left - nOffset;
// 	rectInspROI.y = rectROI.top - nOffset;
// 	rectInspROI.width = rectROI.Width() + nOffset * 2;
// 	rectInspROI.height = rectROI.Height() + nOffset * 2;
// 
// 	if (rectInspROI.x < 0) rectInspROI.x = 0;
// 	if (rectInspROI.y < 0) rectInspROI.y = 0;
// 	if (rectInspROI.width > nWidth - rectInspROI.x) rectInspROI.width = nWidth - rectInspROI.x;
// 	if (rectInspROI.height > nHeight - rectInspROI.y) rectInspROI.height = nHeight - rectInspROI.y;
// }

long CInspectMura3::GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto)
{
	int nBit = GetBitFromImageDepth(matSrcImage.depth());
	if (nBit == -1)	return E_ERROR_CODE_LINE_HISTO;

	int nHistSize = (int)pow((double)256, (double)nBit);
	float fHiStronge[] = { 0, (float)nHistSize };
	const float *ranges[] = { fHiStronge };

	cv::calcHist(&matSrcImage, 1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;
}


int CInspectMura3::GetBitFromImageDepth(int nDepth)
{
	int nRet = -1;

	switch (nDepth)
	{
	case CV_8U:
	case CV_8S:
		nRet = 1;
		break;

	case CV_16U:
	case CV_16S:
		nRet = 2;
		break;

	case CV_32S:
	case CV_32F:
		nRet = 4;
		break;

	case CV_64F:
		nRet = 8;
		break;

	default:
		nRet = -1;
		break;
	}

	return nRet;
}

long CInspectMura3::GetMeanStdDev_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage, double &dblStdev)
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

long CInspectMura3::GetAverage_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage)
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

void CInspectMura3::MinimumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize)
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
	int nMin;

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

			nMin = 255;

			nMaskY_ST = nMask_ST;
			for (int nMaskY = 0; nMaskY < nMaskSize; nMaskY++)
			{
				int nY_ = nY + nMaskY_ST;
				ucSrcdata = src.ptr(nY_);

				nMaskX_ST = nMask_ST;
				for (int nMaskX = 0; nMaskX < nMaskSize; nMaskX++)
				{
					int nX_ = nX + nMaskX_ST;
					if ((int)ucSrcdata[nX_] < nMin)
						nMin = (int)ucSrcdata[nX_];
					nMaskX_ST++;
				}
				nMaskY_ST++;
			}
			ucDstdata[nX] = (uchar)nMin;
		}
	}

	prc.copyTo(dst);
}

void CInspectMura3::MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize)
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

//////////////////////////////////////////////////////////////////////////choi 04.26
bool CInspectMura3::cMeanFilte(cv::Mat matActiveImage, cv::Mat& matDstImage)
{
	try
	{
		cv::Scalar mean, std;
		cv::meanStdDev(matActiveImage, mean, std);
		double sub;
		double com;
#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
		for (int i = 0; i < matActiveImage.rows*matActiveImage.cols; i++) {

			sub = mean[0] - matActiveImage.data[i];
			sub /= 2.0;
			com = matActiveImage.data[i] + sub;
			if (com < 0) { matActiveImage.data[i] = 0; }
			else if (com > 254) { matActiveImage.data[i] = 255; }
			else { matActiveImage.data[i] = com; }
		}
		matActiveImage.copyTo(matDstImage);

		//cv::medianBlur(matActiveImage, matActiveImage, 5)
	}
	catch (cv::Exception& e)
	{
		const char* err_msg = e.what();
		cout << "Exception Caught: " << err_msg << endl;

		return false;
	}

	return true;
}

long CInspectMura3::JudgeWhiteSpot(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
	stDefectInfo* pResultBlob, CMatBuf* cMemSub)
{
	// 불량이 없는 경우 나가기
	int nCount = pResultBlob->nDefectCount;
	if (nCount <= 0)		return E_ERROR_CODE_TRUE;

	// 사용 파라미터
	int		nMorpObj = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MORP_OBJ];
	int		nMorpBKG = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_MORP_BKG];
	double  dThreshold_Ratio = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_THRESHOLD];

	// Active Spec
	double	dSpecActiveBrightRatio = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_BRIGHT_RATIO];

	//////////////////////////////////////////////////////////////////////////spec1
	int		nSpec1_Act_Flag  =  dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC1_FLAG];
	double	dSpecActiveDarkRatio1 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_1];
	double	dSpecActiveDarkArea1_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA1_MIN];
	double	dSpecActiveDarkArea1_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA1_MAX];
	double	dSpecActiveDarkDiff1 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_1];

	//////////////////////////////////////////////////////////////////////////spec2
	int		nSpec2_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC2_FLAG];
	double	dSpecActiveDarkRatio2 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_2];
	double	dSpecActiveDarkArea2_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA2_MIN];
	double	dSpecActiveDarkArea2_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA2_MAX];
	double	dSpecActiveDarkDiff2 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_2];

	//////////////////////////////////////////////////////////////////////////spec3
	int		nSpec3_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC3_FLAG];
	double	dSpecActiveDarkRatio3 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_3];
	double	dSpecActiveDarkArea3_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA3_MIN];
	double	dSpecActiveDarkArea3_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA3_MAX];
	double	dSpecActiveDarkDiff3 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_3];

	//////////////////////////////////////////////////////////////////////////spec4
	int		nSpec4_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC4_FLAG];
	double	dSpecActiveDarkRatio4 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_4];
	double	dSpecActiveDarkArea4_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA4_MIN];
	double	dSpecActiveDarkArea4_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA4_MAX];
	double	dSpecActiveDarkDiff4 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_4];

	//////////////////////////////////////////////////////////////////////////spec5
	int		nSpec5_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC5_FLAG];
	double	dSpecActiveDarkRatio5 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_5];
	double	dSpecActiveDarkArea5_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA5_MIN];
	double	dSpecActiveDarkArea5_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA5_MAX];
	double	dSpecActiveDarkDiff5 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_5];

	//////////////////////////////////////////////////////////////////////////spec6
	int		nSpec6_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC6_FLAG];
	double	dSpecActiveDarkRatio6 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_6];
	double	dSpecActiveDarkArea6_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA6_MIN];
	double	dSpecActiveDarkArea6_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA6_MAX];
	double	dSpecActiveDarkDiff6 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_6];

	//////////////////////////////////////////////////////////////////////////spec7
	int		nSpec7_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC7_FLAG];
	double	dSpecActiveDarkRatio7 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_7];
	double	dSpecActiveDarkArea7_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA7_MIN];
	double	dSpecActiveDarkArea7_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA7_MAX];
	double	dSpecActiveDarkDiff7 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_7];

	//////////////////////////////////////////////////////////////////////////spec8
	int		nSpec8_Act_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_SPEC8_FLAG];
	double	dSpecActiveDarkRatio8 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_8];
	double	dSpecActiveDarkArea8_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA8_MIN];
	double	dSpecActiveDarkArea8_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA8_MAX];
	double	dSpecActiveDarkDiff8 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_8];

	// Edge 영역 설정
	double	dSpecEdgeAreaL = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_LEFT];
	double	dSpecEdgeAreaT = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_TOP];
	double	dSpecEdgeAreaR = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_RIGHT];
	double	dSpecEdgeAreaB = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_AREA_BOTTOM];

	// 예외처리
	if (dSpecEdgeAreaL < 0)				dSpecEdgeAreaL = 0;
	if (dSpecEdgeAreaT < 0)				dSpecEdgeAreaT = 0;
	if (dSpecEdgeAreaR < 0)				dSpecEdgeAreaR = 0;
	if (dSpecEdgeAreaB < 0)				dSpecEdgeAreaB = 0;

	// Edge Spec
	double	dSpecEdgeBrightRatio = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_BRIGHT_RATIO];

	//////////////////////////////////////////////////////////////////////////SPEC1
	int		nSpec1_Edge_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC1_FLAG];
	double	dSpecEdgeDarkRatio1 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_1];
	double	dSpecEdgeDarkArea1_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA1_MIN];
	double	dSpecEdgeDarkArea1_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA1_MAX];
	double	dSpecEdgeDarkDiff1 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_1];

	//////////////////////////////////////////////////////////////////////////SPEC2
	int		nSpec2_Edge_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC2_FLAG];
	double	dSpecEdgeDarkRatio2 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_2];
	double	dSpecEdgeDarkArea2_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA2_MIN];
	double	dSpecEdgeDarkArea2_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA2_MAX];
	double	dSpecEdgeDarkDiff2 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_2];

	//////////////////////////////////////////////////////////////////////////SPEC3
	int		nSpec3_Edge_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC3_FLAG];
	double	dSpecEdgeDarkRatio3 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_3];
	double	dSpecEdgeDarkArea3_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA3_MIN];
	double	dSpecEdgeDarkArea3_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA3_MAX];
	double	dSpecEdgeDarkDiff3 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_3];

	//////////////////////////////////////////////////////////////////////////SPEC4
	int		nSpec4_Edge_Flag = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_SPEC4_FLAG];
	double	dSpecEdgeDarkRatio4 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_4];
	double	dSpecEdgeDarkArea4_MIN = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA4_MIN];
	double	dSpecEdgeDarkArea4_MAX = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA4_MAX];
	double	dSpecEdgeDarkDiff4 = dPara[E_PARA_AVI_MURA3_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_4];
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

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	long	nWidth = (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcBuffer.rows;	// 영상 세로 크기

	int nOffset = 200;
	cv::Mat matDefectMorp1Buf = cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);
	cv::Mat matDefectMorp2Buf = cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);
	cv::Mat matDefectBKBuf = cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);
	cv::Mat matDefectThBuf = cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);

	// 불량 개수 만큼
	for (int i = 0; i < nCount; i++)
	{
		// Spot 불량이 아닌 경우 넘어가기
		if (pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT && pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT) //04.16 choi
			continue;

		// 불량 주변 영역
		cv::Rect rectTempROI(pResultBlob->nCenterx[i] - nOffset / 2, pResultBlob->nCentery[i] - nOffset / 2, nOffset, nOffset);
		if (rectTempROI.x < 0)						rectTempROI.x = 0;
		if (rectTempROI.y < 0)						rectTempROI.y = 0;
		if (rectTempROI.x + nOffset >= nWidth)		rectTempROI.x = nWidth - nOffset - 1;
		if (rectTempROI.y + nOffset >= nHeight)	rectTempROI.y = nHeight - nOffset - 1;

		// 불량 영역
		cv::Mat matDefectSrcBuf = matSrcBuffer(rectTempROI);
		cv::Mat matTempBuf = matDstBuffer(rectTempROI);

		// 백점 하기 위해 이진화 ( 백점 : 255 / 백무라 200 )
		cv::Mat matDefectResBuf = cMatBufTemp.GetMat(matTempBuf.size(), matTempBuf.type(), false);
		cv::threshold(matTempBuf, matDefectResBuf, 220, 255.0, CV_THRESH_BINARY);

		if (bImageSave)
		{
// 			CString strTemp;
// 			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Src_%02d.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i);
// 			ImageSave(strTemp, matDefectSrcBuf);
// 
// 			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Res_%02d.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i);
// 			ImageSave(strTemp, matDefectResBuf);
		}

		// 불량 조금 크게 설정
		int nValue = nMorpObj * 2 + 1;
		if (nMorpObj >= 1)
			cv::morphologyEx(matDefectResBuf, matDefectMorp1Buf, MORPH_DILATE, cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(nValue, nValue)));
		else
			matDefectResBuf.copyTo(matDefectMorp1Buf);

		// 배경 부분
		cv::bitwise_not(matDefectMorp1Buf, matDefectBKBuf);

		// 어두운 부분 찾기
		cv::threshold(matDefectSrcBuf, matDefectThBuf, 20, 255.0, THRESH_BINARY_INV);

		// 어두운 부분 제거
		cv::subtract(matDefectMorp1Buf, matDefectThBuf, matDefectMorp1Buf);	// 검출
		cv::subtract(matDefectBKBuf, matDefectThBuf, matDefectBKBuf);	// 배경

																		// 배경 부분 mean & stdDev 구하기
		cv::Scalar meanBK, stdBK;
		cv::meanStdDev(matDefectSrcBuf, meanBK, stdBK, matDefectBKBuf);

		////////////////////////////////////////////////////////////////////////// 21 06 23 choi 추가
		cv::Scalar meanORI, stdORI;
		cv::meanStdDev(matDefectSrcBuf, meanORI, stdORI, matDefectMorp1Buf);
		

		int nTH = meanORI[0] * dThreshold_Ratio;
		//////////////////////////////////////////////////////////////////////////

		// 밝은 부분 찾기
		cv::threshold(matDefectSrcBuf, matDefectThBuf, nTH, 255.0, THRESH_BINARY);

		// 밝은 부분 조금 크게 설정
		nValue = nMorpBKG * 2 + 1;
		if (nMorpBKG >= 1)
			cv::morphologyEx(matDefectThBuf, matDefectMorp2Buf, MORPH_DILATE, cv::getStructuringElement(MORPH_RECT, cv::Size(nValue, nValue)));
		else
			matDefectThBuf.copyTo(matDefectMorp2Buf);

		// 검출된 불량 안에 밝은 부분 있는지 확인
		double valMax = 0;
		cv::bitwise_and(matDefectMorp1Buf, matDefectMorp2Buf, matDefectThBuf);
		cv::minMaxLoc(matDefectThBuf, NULL, &valMax);

		if (bImageSave)
		{
// 			CString strTemp;
// 			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Bri_%02d.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i);
// 			ImageSave(strTemp, matDefectThBuf);
		}

		// 밝은 영역이 있는지 확인
		bool bPoint = (valMax > 100) ? true : false;

		// Active 영역만 설정
		CRect rectTemp;
		rectTemp.left = rectROI.left + dSpecEdgeAreaL;
		rectTemp.top = rectROI.top + dSpecEdgeAreaT;
		rectTemp.right = rectROI.right - dSpecEdgeAreaR;
		rectTemp.bottom = rectROI.bottom - dSpecEdgeAreaB;

		// 불량 중심 좌표
		CPoint ptSrc(pResultBlob->nCenterx[i], pResultBlob->nCentery[i]);

		// Active 범위 안에 Spot 불량 존재
		if (rectTemp.PtInRect(ptSrc))
		{
			// Mura Active 불량
			pResultBlob->bMuraActive[i] = true;

			// 밝은 명점같은 경우 존재 할 경우 Spec
			if (bPoint)
			{
				// Mura 밝은 GV 존재
				pResultBlob->bMuraBright[i] = true;

				// 검출된 불량 안에 밝은 부분 제거된 영상
				cv::subtract(matDefectMorp1Buf, matDefectThBuf, matDefectMorp2Buf);

				// 검출 부분 mean & stdDev 구하기
				cv::Scalar meanObj, stdObj;
				cv::meanStdDev(matDefectSrcBuf, meanObj, stdObj, matDefectMorp2Buf);

				// Mura 밝기 정보 입력
				pResultBlob->dMuraObj[i] = meanObj[0];
				pResultBlob->dMuraBk[i] = meanBK[0];

				if (meanObj[0] - meanBK[0] > dSpecActiveBrightRatio) { pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT; }
				else													pResultBlob->bUseResult[i] = false;

				if (bImageSave)
				{
// 					CString strTemp;
// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_Src_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanObj[0]);
// 					ImageSave(strTemp, matDefectMorp2Buf);
// 
// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_BK_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanBK[0]);
// 					ImageSave(strTemp, matDefectBKBuf);
				}
			}
			// 밝은 명점같은 경우 존재 안할 경우 Spec
			else
			{
				// Mura 밝은 GV 존재 안함
				pResultBlob->bMuraBright[i] = false;

				// 검출된 불량 안에 밝은 부분 제거된 영상
				cv::subtract(matDefectMorp1Buf, matDefectThBuf, matDefectMorp2Buf);

// 				//////////////////////////////////////////////////////////////////////////
// 				cv:Mat test;
// 				cv::bitwise_and(matDefectSrcBuf, matDefectMorp2Buf, test);
// 				cv::threshold(test, matDefectMorp2Buf, pResultBlob->dMeanGV[i]*2.0, 255, THRESH_BINARY);
				//////////////////////////////////////////////////////////////////////////
				// 검출 부분 mean & stdDev 구하기
				cv::Scalar meanObj, stdObj;
				cv::meanStdDev(matDefectSrcBuf, meanObj, stdObj, matDefectMorp2Buf);

				// Mura 밝기 정보 입력
				pResultBlob->dMuraObj[i] = meanObj[0];
				pResultBlob->dMuraBk[i] = meanBK[0];

				// 무라 정보
				double	dArea = pResultBlob->nArea[i];
				double	dSub = meanObj[0] - meanBK[0];
				double	dDiff = pResultBlob->dMeanGV[i] / pResultBlob->dBackGroundGV[i];

				// Spec1
				if (dArea >= dSpecActiveDarkArea1_MIN		&&
					dArea < dSpecActiveDarkArea1_MAX	&&
					nSpec1_Act_Flag > 0 &&
					dSub > dSpecActiveDarkRatio1		&&
					dDiff > dSpecActiveDarkDiff1)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec2
				else if (dArea >= dSpecActiveDarkArea2_MIN	&&
					dArea < dSpecActiveDarkArea2_MAX	&&
					nSpec2_Act_Flag > 0 &&
					dSub > dSpecActiveDarkRatio2		&&
					dDiff > dSpecActiveDarkDiff2)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec3
				else if (dArea >= dSpecActiveDarkArea3_MIN	&&
					dArea < dSpecActiveDarkArea3_MAX	&&
					nSpec3_Act_Flag > 0 &&
					dSub > dSpecActiveDarkRatio3		&&
					dDiff > dSpecActiveDarkDiff3)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec4
				else if (dArea >= dSpecActiveDarkArea4_MIN	&&
					dArea < dSpecActiveDarkArea4_MAX	&&
					nSpec4_Act_Flag > 0 &&
					dSub > dSpecActiveDarkRatio4		&&
					dDiff > dSpecActiveDarkDiff4)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec5
				else if (dArea >= dSpecActiveDarkArea5_MIN	&&
					dArea < dSpecActiveDarkArea5_MAX	&&
					nSpec5_Act_Flag > 0 &&
					dSub > dSpecActiveDarkRatio5		&&
					dDiff > dSpecActiveDarkDiff5)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec6
				else if (dArea >= dSpecActiveDarkArea6_MIN	&&
					dArea < dSpecActiveDarkArea6_MAX	&&
					nSpec6_Act_Flag > 0 &&
					dSub > dSpecActiveDarkRatio6		&&
					dDiff > dSpecActiveDarkDiff6)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec7
				else if (dArea >= dSpecActiveDarkArea7_MIN	&&
					dArea < dSpecActiveDarkArea7_MAX	&&
					nSpec7_Act_Flag > 0 &&
					dSub > dSpecActiveDarkRatio7		&&
					dDiff > dSpecActiveDarkDiff7)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec7
				else if (dArea >= dSpecActiveDarkArea8_MIN	&&
					dArea < dSpecActiveDarkArea8_MAX	&&
					nSpec8_Act_Flag > 0 &&
					dSub > dSpecActiveDarkRatio8		&&
					dDiff > dSpecActiveDarkDiff8)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				else
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT;
				}

				if (bImageSave)
				{
// 					CString strTemp;
// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_Src_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanObj[0]);
// 					ImageSave(strTemp, matDefectMorp2Buf);
// 
// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_BK_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanBK[0]);
// 					ImageSave(strTemp, matDefectBKBuf);
				}
			}
		}
		// Edge 범위 안에 Spot 불량 존재
		else
		{
			// Mura Active 불량 아님
			pResultBlob->bMuraActive[i] = false;

			// 밝은 명점같은 경우 존재 할 경우 Spec
			if (bPoint)
			{
				// Mura 밝은 GV 존재
				pResultBlob->bMuraBright[i] = true;

				// 검출된 불량 안에 밝은 부분 제거된 영상
				cv::subtract(matDefectMorp1Buf, matDefectThBuf, matDefectMorp2Buf);

				// 검출 부분 mean & stdDev 구하기
				cv::Scalar meanObj, stdObj;
				cv::meanStdDev(matDefectSrcBuf, meanObj, stdObj, matDefectMorp2Buf);

				// Mura 밝기 정보 입력
				pResultBlob->dMuraObj[i] = meanObj[0];
				pResultBlob->dMuraBk[i] = meanBK[0];

				if (meanObj[0] - meanBK[0] > dSpecEdgeBrightRatio)	pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				else												pResultBlob->bUseResult[i] = false;

				if (bImageSave)
				{
// 					CString strTemp;
// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_Src_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanObj[0]);
// 					ImageSave(strTemp, matDefectMorp2Buf);
// 
// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_BK_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanBK[0]);
// 					ImageSave(strTemp, matDefectBKBuf);
				}
			}
			// 밝은 명점같은 경우 존재 안할 경우 Spec
			else
			{
				// Mura 밝은 GV 존재 안함
				pResultBlob->bMuraBright[i] = false;

				// 검출된 불량 안에 밝은 부분 제거된 영상
				cv::subtract(matDefectMorp1Buf, matDefectThBuf, matDefectMorp2Buf);

				// 검출 부분 mean & stdDev 구하기
				cv::Scalar meanObj, stdObj;
				cv::meanStdDev(matDefectSrcBuf, meanObj, stdObj, matDefectMorp2Buf);

				// Mura 밝기 정보 입력
				pResultBlob->dMuraObj[i] = meanObj[0];
				pResultBlob->dMuraBk[i] = meanBK[0];

				// Mura 정보
				double	dArea = pResultBlob->nArea[i];
				double	dSub = meanObj[0] - meanBK[0];
				double	dDiff = pResultBlob->dMeanGV[i] / pResultBlob->dBackGroundGV[i];

				// Spec1
				if (dArea >= dSpecEdgeDarkArea1_MIN		&&
					dArea < dSpecEdgeDarkArea1_MAX		&&
					nSpec1_Edge_Flag > 0 &&
					dSub > dSpecEdgeDarkRatio1		&&
					dDiff > dSpecEdgeDarkDiff1)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec2
				else if (dArea >= dSpecEdgeDarkArea2_MIN		&&
					dArea < dSpecEdgeDarkArea2_MAX		&&
					nSpec2_Edge_Flag > 0 &&
					dSub > dSpecEdgeDarkRatio2			&&
					dDiff > dSpecEdgeDarkDiff2)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec3
				else if (dArea >= dSpecEdgeDarkArea3_MIN		&&
					dArea < dSpecEdgeDarkArea3_MAX		&&
					nSpec3_Edge_Flag > 0 &&
					dSub > dSpecEdgeDarkRatio3			&&
					dDiff > dSpecEdgeDarkDiff3)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec4
				else if (dArea >= dSpecEdgeDarkArea4_MIN		&&
					dArea < dSpecEdgeDarkArea4_MAX		&&
					nSpec4_Edge_Flag > 0 &&
					dSub > dSpecEdgeDarkRatio4			&&
					dDiff > dSpecEdgeDarkDiff4)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				else
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT;
				}

				if (bImageSave)
				{
// 					CString strTemp;
// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_Src_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanObj[0]);
// 					ImageSave(strTemp, matDefectMorp2Buf);
// 
// 					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_BK_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanBK[0]);
// 					ImageSave(strTemp, matDefectBKBuf);
				}
			}
		}
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

/*8bit & 12bit 영상 저장
long CInspectMura3::ImageSave(CString strPath, cv::Mat matSrcBuf)
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
}*/