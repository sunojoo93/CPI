
/************************************************************************
Line 알고리즘 관련 소스
************************************************************************/

#include "StdAfx.h"
#include "InspectLine.h"


#define round(fp) (int)((fp) >= 0 ? (fp) + 0.5 : (fp) - 0.5)

CInspectLine::CInspectLine(void)
{
	m_bProcess = false;

	cMem				= NULL;
	m_cInspectLibLog	= NULL;
	m_strAlgLog			= NULL;
	m_tInitTime			= 0;
	m_tBeforeTime		= 0;
}

CInspectLine::~CInspectLine(void)
{

}

long CInspectLine::FindLineDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Mat& matBKBuffer, vector<int> NorchIndex, CPoint OrgIndex, cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath,
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];
	//	int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];

	// 파라미터 NULL인 경우.
	if (dPara == NULL)					return E_ERROR_CODE_EMPTY_PARA;
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	long	nWidth = (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight = (long)matSrcBuffer.rows;	// 영상 세로 크기

	// 영상 번호
	long	nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Start."));
	
	// 초기화
	cv::Mat matDstImage[E_BINARY_TOTAL_COUNT];

	matDstImage[E_BINARY_BRIGHT_X] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type(), false);
	matDstImage[E_BINARY_BRIGHT_Y] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type(), false);
	matDstImage[E_BINARY_DARK_X] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type(), false);
	matDstImage[E_BINARY_DARK_Y] = cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type(), false);

	// 큰 외곽
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

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	m_bProcess = false;
	CString strPath;
	strPath.Format(_T("%s"), strAlgPath);

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Init."));

	//원본 백업
	cv::Mat matSrcBuf8bit = cMem->GetMat(matSrcBuffer.size(), CV_8U, false);
	if (matSrcBuffer.type() == CV_8U)
		matSrcBuffer.copyTo(matSrcBuf8bit);
	else
		matSrcBuffer.convertTo(matSrcBuf8bit, CV_8U, 1. / 16.);

	switch (nImageNum)	// 영상 마다 알고리즘 다름.
	{
	case E_IMAGE_CLASSIFY_AVI_R:
	case E_IMAGE_CLASSIFY_AVI_G:
	case E_IMAGE_CLASSIFY_AVI_B:
	case E_IMAGE_CLASSIFY_AVI_GRAY_87:
	case E_IMAGE_CLASSIFY_AVI_WHITE:  // White pattern Line 검사 추가 21.04.15 pwj
	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
	{
		nErrorCode = LogicStart_RGB5(matSrcBuf8bit, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment, ptCorner);
		nErrorCode = LogicStart_Weak(matSrcBuf8bit, matDstImage, NorchIndex, OrgIndex, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment, ptCorner);

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalResult_XB.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_BINARY_BRIGHT_X]);
			strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalResult_YB.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_BINARY_BRIGHT_Y]);
			strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalResult_XD.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_BINARY_DARK_X]);
			strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalResult_YD.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_BINARY_DARK_Y]);
		}
	}
	break;

	case E_IMAGE_CLASSIFY_AVI_BLACK:
	case E_IMAGE_CLASSIFY_AVI_PCD:
		nErrorCode = LogicStart_BlackWhite3(matSrcBuf8bit, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment, ptCorner);
		break;

	case E_IMAGE_CLASSIFY_AVI_GRAY_32:
		nErrorCode = LogicStart_Crack(matSrcBuf8bit, matDstImage, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment, ptCorner);
		break;

		//case E_IMAGE_CLASSIFY_AVI_GRAY_64:
	case E_IMAGE_CLASSIFY_AVI_GRAY_128:
	case E_IMAGE_CLASSIFY_AVI_VTH:
		//	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
	{
		nErrorCode = LogicStart_Weak(matSrcBuf8bit, matDstImage, NorchIndex, OrgIndex, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment, ptCorner);
		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalResult_XB.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_BINARY_BRIGHT_X]);
			strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalResult_YB.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_BINARY_BRIGHT_Y]);
			strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalResult_XD.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_BINARY_DARK_X]);
			strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalResult_YD.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_BINARY_DARK_Y]);
		}
	}
	break;

	// KSW 17.08.09 - PG List 수정
// 	case E_IMAGE_CLASSIFY_AVI_GRAY_87:
// 		break;

	default:
		return E_ERROR_CODE_TRUE;		// 영상 번호가 잘못 들어온 경우.
	}
	//////////////////////////////////////////////////////////////////////////
	// Back Ground Sub
	//////////////////////////////////////////////////////////////////////////


	if ( !matBKBuffer.empty() )
	{
		cv::subtract( matDstImage[E_BINARY_BRIGHT_X],	matBKBuffer , matDstImage[E_BINARY_BRIGHT_X]	);
		cv::subtract( matDstImage[E_BINARY_BRIGHT_Y],	matBKBuffer , matDstImage[E_BINARY_BRIGHT_Y]	);
		if (nImageNum != E_IMAGE_CLASSIFY_AVI_BLACK && nImageNum != E_IMAGE_CLASSIFY_AVI_PCD)
		{
			cv::subtract(matDstImage[E_BINARY_DARK_X], matBKBuffer, matDstImage[E_BINARY_DARK_X]);
			cv::subtract(matDstImage[E_BINARY_DARK_Y], matBKBuffer, matDstImage[E_BINARY_DARK_Y]);
		}
	}

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalSubResult_XB.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave( strTemp, matDstImage[E_BINARY_BRIGHT_X] );
		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalSubResult_YB.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave( strTemp, matDstImage[E_BINARY_BRIGHT_Y] );
		if (nImageNum != E_IMAGE_CLASSIFY_AVI_BLACK && nImageNum != E_IMAGE_CLASSIFY_AVI_PCD)
		{
			strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalSubResult_XD.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_BINARY_DARK_X]);
			strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_FinalSubResult_YD.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDstImage[E_BINARY_DARK_Y]);
		}
	}
//////////////////////////////////////////////////////////////////////////
// Blob Judgment
//////////////////////////////////////////////////////////////////////////

	// 파라미터 NULL인 경우.
	if( dPara == NULL )					return E_ERROR_CODE_EMPTY_PARA;
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcBuf8bit.empty() )						return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstImage[E_BINARY_BRIGHT_X].empty() )	return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstImage[E_BINARY_BRIGHT_Y].empty() )	return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstImage[E_BINARY_DARK_X].empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstImage[E_BINARY_DARK_Y].empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Start."));

	cv::Rect rectInspROI;
	Insp_RectSet(rectInspROI, rectROI, matSrcBuf8bit.cols, matSrcBuf8bit.rows);

	// 불량 판정 & 결과 보내기
	{
		// 라벨
		CFeatureExtraction cFeatureExtraction;
		cFeatureExtraction.SetMem( cMem );
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_LINE, m_tInitTime, m_tBeforeTime, m_strAlgLog);

		// Bright X
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuf8bit(rectInspROI), matDstImage[E_BINARY_BRIGHT_X](rectInspROI), matDrawBuffer(rectInspROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("LBX"), EngineerDefectJudgment, pResultBlob);

		if( nErrorCode != E_ERROR_CODE_TRUE )
		{
			// 메모리 해제
			matSrcBuf8bit.release();
			matDstImage[E_BINARY_BRIGHT_X].release();
			matDstImage[E_BINARY_BRIGHT_Y].release();
			matDstImage[E_BINARY_DARK_X].release();
			matDstImage[E_BINARY_DARK_Y].release();

			return nErrorCode;
		}

		writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("BlobJudgment (BrightX)."));

		// Bright Y
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuf8bit(rectInspROI), matDstImage[E_BINARY_BRIGHT_Y](rectInspROI), matDrawBuffer(rectInspROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("LBY"), EngineerDefectJudgment, pResultBlob);

		if( nErrorCode != E_ERROR_CODE_TRUE )
		{
			// 메모리 해제
			matSrcBuf8bit.release();
			matDstImage[E_BINARY_BRIGHT_X].release();
			matDstImage[E_BINARY_BRIGHT_Y].release();
			matDstImage[E_BINARY_DARK_X].release();
			matDstImage[E_BINARY_DARK_Y].release();

			return nErrorCode;
		}

		writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("BlobJudgment (BrightY)."));

		if (  nImageNum != E_IMAGE_CLASSIFY_AVI_BLACK && nImageNum != E_IMAGE_CLASSIFY_AVI_GRAY_32 && nImageNum != E_IMAGE_CLASSIFY_AVI_PCD)
		{
			// Dark X
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuf8bit(rectInspROI), matDstImage[E_BINARY_DARK_X](rectInspROI), matDrawBuffer(rectInspROI), rectROI,
				nCommonPara, E_DEFECT_COLOR_DARK, _T("LDX"), EngineerDefectJudgment, pResultBlob);

			if( nErrorCode != E_ERROR_CODE_TRUE )
			{
				// 메모리 해제
				matSrcBuf8bit.release();
				matDstImage[E_BINARY_BRIGHT_X].release();
				matDstImage[E_BINARY_BRIGHT_Y].release();
				matDstImage[E_BINARY_DARK_X].release();
				matDstImage[E_BINARY_DARK_Y].release();

				return nErrorCode;
			}

			writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("BlobJudgment (DarkX)."));

			// Dark Y
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuf8bit(rectInspROI), matDstImage[E_BINARY_DARK_Y](rectInspROI), matDrawBuffer(rectInspROI), rectROI,
				nCommonPara, E_DEFECT_COLOR_DARK, _T("LDY"), EngineerDefectJudgment, pResultBlob);

			if( nErrorCode != E_ERROR_CODE_TRUE )
			{
				// 메모리 해제
				matSrcBuf8bit.release();
				matDstImage[E_BINARY_BRIGHT_X].release();
				matDstImage[E_BINARY_BRIGHT_Y].release();
				matDstImage[E_BINARY_DARK_X].release();
				matDstImage[E_BINARY_DARK_Y].release();

				return nErrorCode;
			}

			writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("BlobJudgment (DarkY)."));
		}
	}

	// 메모리 해제
	matSrcBuf8bit.release();
	matDstImage[E_BINARY_BRIGHT_X].release();
	matDstImage[E_BINARY_BRIGHT_Y].release();
	matDstImage[E_BINARY_DARK_X].release();
	matDstImage[E_BINARY_DARK_Y].release();

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Memory Release."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory());
// 	}

	return nErrorCode;
}


 long CInspectLine::LogicStart_BlackWhite3(Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
 	int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner)
 {
 	long	nErrorCode			=	E_ERROR_CODE_TRUE;		// 에러 코드
 
	int		nWindowSize			=	dPara[E_PARA_LINE_BLACK_WINDOW_SIZE		];		// 5
	float	fSigma				=	dPara[E_PARA_LINE_BLACK_SIGMA			];		// 3
	int		nResizesize			=	dPara[E_PARA_LINE_BLACK_RESIZE			];		// 2
	float	fThresholdRatio		=	dPara[E_PARA_LINE_BLACK_THRESHOLD_RATIO	];		// 1	
	int		nOutLine			=	dPara[E_PARA_LINE_BLACK_OUTLINE			];			 
	int		nRotationOnOff		=	dPara[E_PARA_LINE_BLACK_ROTATION_FLAG	];		// 0
 
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
 
	if (rectROI.left	<= 0				)	rectROI.left	= 0;
	if (rectROI. right	>= matSrcImage.cols	)	rectROI. right	= matSrcImage.cols-1;
	if (rectROI.top		<= 0				)	rectROI.top		= 0;
	if (rectROI.bottom	>= matSrcImage.rows	)	rectROI.bottom	= matSrcImage.rows-1;
 
 	if (rectTemp.left	<= 0				)	rectTemp.left	= 0;
 	if (rectTemp. right >= matSrcImage.cols	)	rectTemp.right	= matSrcImage.cols-1;
 	if (rectTemp.top	<= 0				)	rectTemp.top	= 0;
 	if (rectTemp.bottom >= matSrcImage.rows	)	rectTemp.bottom = matSrcImage.rows-1;

	// Resize 영상 크기
	int nResizeW = matSrcImage.cols / nResizesize;
	int nResizeH = matSrcImage.rows / nResizesize;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	// Temp Buf
	cv::Mat matTempBuf1				= cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());
	cv::Mat matTempBuf2				= cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	cv::Mat matResizeTempBuf1		= cMatBufTemp.GetMat(cv::Size(nResizeW, nResizeH), matSrcImage.type(), false);
	cv::Mat matResizeTempBuf2		= cMatBufTemp.GetMat(cv::Size(nResizeW, nResizeH), matSrcImage.type(), false);
	cv::Mat matResizeTempBuf3		= cMatBufTemp.GetMat(cv::Size(nResizeW, nResizeH), matSrcImage.type(), false);
	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Start."));

	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nWindowSize;
	//rtInspROI.y = rectROI.top - nWindowSize;
	//rtInspROI.width = rectROI.Width() + nWindowSize * 2;
	//rtInspROI.height = rectROI.Height() + nWindowSize * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nWindowSize);

	// 가우시안 블러
	cv::GaussianBlur(matSrcImage(rtInspROI), matTempBuf1(rtInspROI), cv::Size(nWindowSize, nWindowSize), fSigma);

	// 리싸이즈
	cv::resize(matTempBuf1, matResizeTempBuf1, cv::Size(nResizeW, nResizeH), 0, 0, INTER_LINEAR);

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Pre-processing."));

	// 표준편차 이진화?
	Sdtthreshold(matResizeTempBuf1, matResizeTempBuf2, fThresholdRatio);

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Thresholding."));

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_BlackThreshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResizeTempBuf2);
	}
	
	cv::Mat matProjection[4];

	matProjection[0] = cMatBufTemp.GetMat(matResizeTempBuf2.rows, 1, matResizeTempBuf2.type(), false);
	matProjection[1] = cMatBufTemp.GetMat(1, matResizeTempBuf2.cols, matResizeTempBuf2.type(), false);
	matProjection[2] = cMatBufTemp.GetMat(1, matResizeTempBuf2.cols, matResizeTempBuf2.type(), false);
	matProjection[3] = cMatBufTemp.GetMat(matResizeTempBuf2.rows, 1, matResizeTempBuf2.type(), false);

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
			//////////////////////////////////////////////////////////////////////////
			// X 방향
			//////////////////////////////////////////////////////////////////////////
			matResizeTempBuf1.setTo(0);
			LineMeasurement(matResizeTempBuf2, matResizeTempBuf1, matProjection, dPara, rectROI, 1, nOutLine, &cMatBufTemp);

			writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("LineMeasure X."));

			if (bImageSave)
			{
				CString strTemp;
				strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_MeasureX.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
				cv::imwrite((cv::String)(CStringA)strTemp, matResizeTempBuf1);
			}

			cv::resize(matResizeTempBuf1, matTempBuf1, matSrcImage.size(), 0, 0, INTER_LINEAR);

			writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Resize_Up X."));

			matTempBuf1.copyTo(matDstImage[E_BINARY_BRIGHT_X]);

			writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("X Copy."));
		}
		break;
		case 1:
		{
			//////////////////////////////////////////////////////////////////////////
			// Y 방향
			//////////////////////////////////////////////////////////////////////////
			matResizeTempBuf3.setTo(0);
			LineMeasurement(matResizeTempBuf2, matResizeTempBuf3, matProjection, dPara, rectROI, 2, nOutLine, &cMatBufTemp);

			writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("LineMeasure Y."));

			if (bImageSave)
			{
				CString strTemp;
				strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_MeasureY.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
				cv::imwrite((cv::String)(CStringA)strTemp, matResizeTempBuf3);
			}

			cv::resize(matResizeTempBuf3, matTempBuf2, matSrcImage.size(), 0, 0, INTER_LINEAR);

			writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Resize_Up Y."));

			matTempBuf2.copyTo(matDstImage[E_BINARY_BRIGHT_Y]);

			writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Y Copy."));
		}
		break;
		}

	}

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

 	return E_ERROR_CODE_TRUE;
 }
 
 long CInspectLine::LogicStart_Crack(Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
	 int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner)
 {
	 long	nErrorCode			=	E_ERROR_CODE_TRUE;		// 에러 코드

	 int		nWindowSize			=	dPara[E_PARA_LINE_BLACK_WINDOW_SIZE		];		// 5
	 float		fSigma				=	dPara[E_PARA_LINE_BLACK_SIGMA			];		// 3
	 int		nResizesize			=	dPara[E_PARA_LINE_BLACK_RESIZE			];		// 2
	 float		fThresholdRatio		=	dPara[E_PARA_LINE_BLACK_THRESHOLD_RATIO	];		// 1	
	 int		nOutLine			=	dPara[E_PARA_LINE_BLACK_OUTLINE			];			 
	 int		nRotationOnOff		=	dPara[E_PARA_LINE_BLACK_ROTATION_FLAG	];		// 0

	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	 // 검사 영역 외곽 축소
	 CRect rectTemp(rectROI);

	 if (rectROI.left	 <= 0					)	rectROI.left	= 0;
	 if (rectROI. right	 >= matSrcImage.cols	)	rectROI. right	= matSrcImage.cols-1;
	 if (rectROI.top	 <= 0					)	rectROI.top		= 0;
	 if (rectROI.bottom	 >= matSrcImage.rows	)	rectROI.bottom	= matSrcImage.rows-1;

	 if (rectTemp.left	 <= 0					)	rectTemp.left	= 0;
	 if (rectTemp. right >= matSrcImage.cols	)	rectTemp.right	= matSrcImage.cols-1;
	 if (rectTemp.top	 <= 0					)	rectTemp.top	= 0;
	 if (rectTemp.bottom >= matSrcImage.rows	)	rectTemp.bottom = matSrcImage.rows-1;

	 // Resize 영상 크기
	 int nResizeW = matSrcImage.cols / nResizesize;
	 int nResizeH = matSrcImage.rows / nResizesize;

	 CMatBuf cMatBufTemp;
	 cMatBufTemp.SetMem(cMem);

	 // Temp Buf
	 cv::Mat matTempBuf1		= cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	 cv::Mat matTempBuf2		= cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	 cv::Mat matResizeTempBuf1	= cMatBufTemp.GetMat(cv::Size(nResizeW, nResizeH), matSrcImage.type(), false);
	 cv::Mat matResizeTempBuf2	= cMatBufTemp.GetMat(cv::Size(nResizeW, nResizeH), matSrcImage.type(), false);
	 cv::Mat matResizeTempBuf3	= cMatBufTemp.GetMat(cv::Size(nResizeW, nResizeH), matSrcImage.type(), false);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Start"));

	 cv::Rect rtInspROI;
	 //rtInspROI.x = rectROI.left - nWindowSize;
	 //rtInspROI.y = rectROI.top - nWindowSize;
	 //rtInspROI.width = rectROI.Width() + nWindowSize * 2;
	 //rtInspROI.height = rectROI.Height() + nWindowSize * 2;

	 Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nWindowSize);

	 // 가우시안 블러
	 cv::GaussianBlur(matSrcImage(rtInspROI), matTempBuf1(rtInspROI), cv::Size(nWindowSize, nWindowSize), fSigma);

	 // 리싸이즈
	 cv::resize(matTempBuf1, matResizeTempBuf1, cv::Size(nResizeW, nResizeH), 0, 0, INTER_LINEAR);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Pre-processing."));

	cv::Mat tempMean, tempStd;
	double minvalue = 0;
	double maxvlaue = 0;
	cv::meanStdDev(matResizeTempBuf1, tempMean, tempStd);
	cv::minMaxIdx(matResizeTempBuf1, &minvalue, &maxvlaue, NULL, NULL);

	double Meanvalue = tempMean.at<double>(0,0);
	double Stdvalue  = tempStd.at<double>(0,0);	
	double dbthresh	 = 0;

	if ((Meanvalue <= 0.1) && (Stdvalue <= 5))
	{	dbthresh  = 1; }

	if (((Meanvalue > 0.1) && (Meanvalue <= 1) )&& ((Stdvalue > 5.0)  && (Stdvalue <= 10.0))) 
	{	dbthresh  = Stdvalue * fThresholdRatio; }

	if ((Meanvalue > 1.0) && (Stdvalue > 10))
	{	dbthresh  = Meanvalue * Stdvalue; }

	if((Meanvalue > 0.1) && (Stdvalue < 5) )
	{	dbthresh  = maxvlaue / 2;	}

	// 이진화
	cv::threshold(matResizeTempBuf1, matResizeTempBuf2, dbthresh, 255.0, THRESH_BINARY);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Thresholding."));

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_BlackThreshold.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 cv::imwrite((cv::String)(CStringA)strTemp, matResizeTempBuf2);
	 }

	 cv::Mat matProjection[4];

	 matProjection[0] = cMatBufTemp.GetMat(matResizeTempBuf2.rows, 1, matResizeTempBuf2.type(), false);
	 matProjection[1] = cMatBufTemp.GetMat(matResizeTempBuf2.cols, 1, matResizeTempBuf2.type(), false);
	 matProjection[2] = cMatBufTemp.GetMat(matResizeTempBuf2.cols, 1, matResizeTempBuf2.type(), false);
	 matProjection[3] = cMatBufTemp.GetMat(matResizeTempBuf2.rows, 1, matResizeTempBuf2.type(), false);

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
			 //////////////////////////////////////////////////////////////////////////
			 // X 방향
			 //////////////////////////////////////////////////////////////////////////
			 LineMeasurement(matResizeTempBuf2, matResizeTempBuf1, matProjection, dPara, rectROI, 1, nOutLine);

			 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("LineMeasure X."));

			 if (bImageSave)
			 {
				 CString strTemp;
				 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_MeasureX.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
				 cv::imwrite((cv::String)(CStringA)strTemp, matResizeTempBuf1);
			 }

			 cv::resize(matResizeTempBuf1, matTempBuf1, matSrcImage.size(), 0, 0, INTER_LINEAR);

			 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Resize_Up X."));

			 matTempBuf1.copyTo(matDstImage[E_BINARY_BRIGHT_X]);

			 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("X Copy."));
		 }
		 break;
		 case 1:
		 {
			 //////////////////////////////////////////////////////////////////////////
			 // Y 방향
			 //////////////////////////////////////////////////////////////////////////

			 LineMeasurement(matResizeTempBuf2, matResizeTempBuf3, matProjection, dPara, rectROI, 2, nOutLine);

			 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("LineMeasure Y."));

			 if (bImageSave)
			 {
				 CString strTemp;
				 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_MeasureY.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
				 cv::imwrite((cv::String)(CStringA)strTemp, matResizeTempBuf3);
			 }

			 cv::resize(matResizeTempBuf3, matTempBuf2, matSrcImage.size(), 0, 0, INTER_LINEAR);

			 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Resize_Up Y."));

			 matTempBuf2.copyTo(matDstImage[E_BINARY_BRIGHT_Y]);

			 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Y Copy."));
		 }
		 break;
		 }
	 }
	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("End."));

// 	 if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		 writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		 writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	 }

	 return E_ERROR_CODE_TRUE;
 }

long CInspectLine::LogicStart_RGB5(Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
	int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner )
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	double		dblTargetGV				=		(double)	dPara[E_PARA_LINE_RGB_TARGET_GV			];
	int			nBlurSize				=		(int)		dPara[E_PARA_LINE_RGB_BLUR_SIZE			];
	int			nMeanFilterSize			=		(int)		dPara[E_PARA_LINE_RGB_MEAN_FILTER_SIZE	];
	int			nBGFilterSize			=		(int)		dPara[E_PARA_LINE_RGB_BG_FILTER_SIZE	];
	int			nImageFilterSize		=		(int)		dPara[E_PARA_LINE_RGB_IMAGE_FILTER_SIZE	];
	int			nDeletArea				=		(int)		dPara[E_PARA_LINE_RGB_DELETE_AREA		];	

	// Weak Parameter
	int			nWeakResize				=		(int)		dPara[E_PARA_LINE_RGB_WEAK_RESIZE		];

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

	//////////////////////////////////////////////////////////////////////////

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave( strTemp, matSrcImage );
	}

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	// Resize 영상 크기
	int nResizeW = matSrcImage.cols / nWeakResize;
	int nResizeH = matSrcImage.rows / nWeakResize;

	// Input Image to Image Buffer
	cv::Mat matResizeTempBuf1 = cMatBufTemp.GetMat(cv::Size(nResizeW, nResizeH), matSrcImage.type(), false);
	cv::Mat matResizeTempBuf2 = cMatBufTemp.GetMat(cv::Size(nResizeW, nResizeH), matSrcImage.type(), false);

	// Resize 검사 영역
	CRect rectResize;
	rectResize.left		= rectROI.left	 / nWeakResize;
	rectResize.top		= rectROI.top	 / nWeakResize;
	rectResize.right	= rectROI.right	 / nWeakResize;
	rectResize.bottom	= rectROI.bottom / nWeakResize;

	//////////////////////////////////////////////////////////////////////////
	// Pre-processing
	//////////////////////////////////////////////////////////////////////////

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Start"));

	// 영상 축소 - Processing Time 감소
	cv::resize( matSrcImage, matResizeTempBuf1, cv::Size(nResizeW, nResizeH), 0, 0, INTER_LINEAR);	

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("resize."));

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ResiezeRGBImage.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResizeTempBuf1);
	}

	cv::Rect rtInspROI;
	//rtInspROI.x = rectResize.left - nBlurSize;
	//rtInspROI.y = rectResize.top - nBlurSize;
	//rtInspROI.width = rectResize.Width() + nBlurSize * 2;
	//rtInspROI.height = rectResize.Height() + nBlurSize * 2;

	Insp_RectSet(rtInspROI, rectResize, matResizeTempBuf1.cols, matResizeTempBuf1.rows, nBlurSize);

	// Noise 제거
	cv::blur(matResizeTempBuf1(rtInspROI), matResizeTempBuf2(rtInspROI), cv::Size(nBlurSize, nBlurSize));
	cv::blur(matResizeTempBuf2(rtInspROI), matResizeTempBuf1(rtInspROI), cv::Size(nBlurSize, nBlurSize));

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("blur."));

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_BlurImage.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResizeTempBuf1);
	}

	// 평균 밝기 보정
	ApplyMeanGV( matResizeTempBuf1, dblTargetGV, rectResize);
	////////////////////////////////////////////////////////////////////
	//choikwangil
	//BYTE* pmatActive = (BYTE*)matResizeTempBuf1.data;

	//filter8(pmatActive, pmatActive, -10000, 10000, nResizeW, nResizeH);
	////////////////////////////////////////////////////////////////////

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ApplyGV.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResizeTempBuf1);
	}

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("ApplyMeanGV"));

	// 버퍼 할당 & 초기화
	cv::Mat matDstBuff[2];
	matDstBuff[E_DEFECT_COLOR_DARK]		= cMatBufTemp.GetMat(matResizeTempBuf1.size(), matResizeTempBuf1.type());
	matDstBuff[E_DEFECT_COLOR_BRIGHT]	= cMatBufTemp.GetMat(matResizeTempBuf1.size(), matResizeTempBuf1.type());

	//////////////////////////////////////////////////////////////////////////
	// Main
	//////////////////////////////////////////////////////////////////////////

	nErrorCode = calcRGBMain(matSrcImage, matResizeTempBuf1, matDstBuff, matDstImage, dPara, nCommonPara,
		rectResize, ptCorner, strAlgPath, EngineerDefectJudgment, &cMatBufTemp);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("calcRGBMain."));

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////Functions///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CInspectLine::ApplyMeanGV(Mat &matSrcImage, double dblTargetGV, CRect rectTemp)
{
	double dblMean;
	cv::Scalar m = cv::mean(matSrcImage(Rect(rectTemp.left, rectTemp.top, rectTemp.Width(), rectTemp.Height())));
	//cv::Scalar m = cv::mean(matSrcImage);
	dblMean = m[0];

	matSrcImage *= dblTargetGV/dblMean;
}

long CInspectLine::Morphology(Mat& matSrcImage, Mat& matDstImage, long nSizeX, long nSizeY, int nOperation, CMatBuf* cMemSub, int nIter)
{
	if( nSizeX < 1 )			return E_ERROR_CODE_POINT_WARNING_PARA;
	if( nSizeY < 1 )			return E_ERROR_CODE_POINT_WARNING_PARA;
	if( matSrcImage.empty() )	return E_ERROR_CODE_EMPTY_BUFFER;
	int nRep = nIter;

	cv::Point anchor(-1,-1);

	cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, Size(nSizeX, nSizeY));

	switch (nOperation)
	{
	case EP_MORP_ERODE:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_ERODE, StructElem, anchor,nRep);
		break;

	case EP_MORP_DILATE:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_DILATE, StructElem, anchor,nRep);
		break;

	case EP_MORP_OPEN:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_OPEN, StructElem, anchor,nRep);
		break;

	case EP_MORP_CLOSE:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_CLOSE, StructElem, anchor,nRep);
		break;

	case EP_MORP_GRADIENT:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_GRADIENT, StructElem, anchor,nRep);
		break;

	case EP_MORP_TOPHAT:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_TOPHAT, StructElem, anchor,nRep);
		break;

	case EP_MORP_BLACKHAT:
		cv::morphologyEx(matSrcImage, matDstImage, MORPH_BLACKHAT, StructElem, anchor,nRep);
		break;

	default:
		StructElem.release();
		return E_ERROR_CODE_POINT_WARNING_PARA;
		break;
	}

	StructElem.release();

	return E_ERROR_CODE_TRUE;
}

long CInspectLine::RangeAvgThreshold_Gray(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI,
	long nLoop, long nSegX, long nSegY, float fDarkRatio, float fBrightRatio, float fDarkRatio_Edge, float fBrightRatio_Edge, CMatBuf* cMemSub)
{
	// 설정 값이 10보다 작은 경우.
	if( nSegX <= 10 )		return E_ERROR_CODE_POINT_WARNING_PARA;
	if( nSegY <= 10 )		return E_ERROR_CODE_POINT_WARNING_PARA;

	// 이미지 크기
	long	nWidth	= (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcImage.rows;	// 영상 세로 크기

	long x, y;
	long nStart_X, nStart_Y, nEnd_X, nEnd_Y;

	long nPixelSum, nPixelCount, nPixelAvg;

	// Active 영역만 검사
	int nRangeX = rectROI.Width() / nSegX + 1;
	int nRangeY = rectROI.Height() / nSegY + 1;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// Temp 메모리 할당	
	cv::Mat matBlurBuf  = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	cv::Mat matBlurBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);

	int nBlur = 5;

	//연산량 감소목적
	cv::Rect rtInspROI;
	//rtInspROI.x = rectROI.left - nBlur;
	//rtInspROI.y = rectROI.top - nBlur;
	//rtInspROI.width = rectROI.Width() + nBlur * 2;
	//rtInspROI.height = rectROI.Height() + nBlur * 2;

	Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nBlur);

	if( nLoop > 0 )
	{
		cv::blur(matSrcImage(rtInspROI), matBlurBuf(rtInspROI), cv::Size(nBlur, nBlur));

		if( nLoop > 1 )
		{
			// Avg
			for(int i=1 ; i<nLoop ; i++)
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

	//////////////////////////////////////////////////////////////////////////

	for (y=0 ; y<nRangeY ; y++)
	{
		// y 범위 계산
		nStart_Y = y * nSegY + rectROI.top;
		if( y==nRangeY-1)		nEnd_Y = rectROI.bottom;
		else					nEnd_Y = nStart_Y + nSegY;

		for (x=0 ; x<nRangeX ; x++)
		{
			double dbDarkRatio		= fDarkRatio;
			double dbBrightRatio	= fBrightRatio;

			//Edge 부분
			if ( x == 0 || y == 0 || x == nRangeX || y == nRangeY)
			{
				double dbDarkRatio		= fDarkRatio_Edge;
				double dbBrightRatio	= fBrightRatio_Edge;
			}
			// x 범위 계산
			nStart_X = x * nSegX + rectROI.left;
			if( x==nRangeX-1)		nEnd_X = rectROI.right;
			else					nEnd_X = nStart_X + nSegX;

			// 범위 설정
			cv::Rect rectTemp;
			rectTemp.x		= nStart_X;
			rectTemp.y		= nStart_Y;
			rectTemp.width	= nEnd_X - nStart_X + 1;
			rectTemp.height	= nEnd_Y - nStart_Y + 1;

			// 영상 ROI
			cv::Mat matTempBuf = matBlurBuf( rectTemp );

			// 히스토그램
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			double dblAverage;
			double dblStdev;
			GetMeanStdDev_From_Histo(matHisto,0,255,dblAverage,dblStdev);

			// 평균 범위 설정
			int nMinGV = (int)(dblAverage - dblStdev);
			int nMaxGV = (int)(dblAverage + dblStdev);
			if( nMinGV < 0 )	nMinGV = 0;
			if( nMaxGV > 255 )	nMaxGV = 255;			

			// 초기화
			nPixelSum	= 0;
			nPixelCount	= 0;
			nPixelAvg	= 0;

			// 설정된 평균 범위만큼만 재평균
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m=nMinGV ; m<=nMaxGV ; m++, pVal++)
			{
				nPixelSum	+= (m * *pVal);
				nPixelCount	+= *pVal;
			}

			if( nPixelCount == 0 )	continue;

			// 범위 내 평균
			nPixelAvg = (long)(nPixelSum / (double)nPixelCount);

			// 평균 * Ratio
			long nDarkTemp		= (long)( nPixelAvg * dbDarkRatio );
			long nBrightTemp	= (long)( nPixelAvg * dbBrightRatio );

			// Gray 너무 어두운 경우 있음.
			// ( 평균 GV 2 ~ 3 * fBrightRatio -> 이진화 : 노이즈 다 올라옴 )
			if( nBrightTemp < 15 )	nBrightTemp = 15;

			// 예외 처리
			if( nDarkTemp < 0 )		nDarkTemp = 0;
			if( nBrightTemp > 255 )	nBrightTemp = 255;

			// 파라미터 0인경우 예외 처리
			if( dbDarkRatio == 0 )	nDarkTemp	= -1;
			if( dbBrightRatio == 0 )	nBrightTemp = 256;

			// E_DEFECT_COLOR_DARK Threshold
			cv::Mat matTempBufT = matDstImage[E_DEFECT_COLOR_DARK]( rectTemp );
			cv::threshold(matTempBuf, matTempBufT, nDarkTemp, 255.0, THRESH_BINARY_INV);

			// E_DEFECT_COLOR_BRIGHT Threshold
			matTempBufT = matDstImage[E_DEFECT_COLOR_BRIGHT]( rectTemp );
			cv::threshold(matTempBuf, matTempBufT, nBrightTemp, 255.0, THRESH_BINARY);
		}
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

long CInspectLine::GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto)
{
	int nBit = GetBitFromImageDepth(matSrcImage.depth());
	if(nBit == -1)	return E_ERROR_CODE_LINE_HISTO;

	int nHistSize = (int)pow((double)256,(double)nBit);
	float fHiStronge[] = {0, (float) nHistSize-1};
	const float *ranges[] = { fHiStronge };

	cv::calcHist(&matSrcImage,1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;
}

int CInspectLine::GetBitFromImageDepth(int nDepth)
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


// Line Delete X Projection Y Projection
void CInspectLine::LineMeasurement(cv::Mat matSrcImage, cv::Mat& matDstImage, Mat* matProjection, double* dPara, CRect rectROI, int nDirection, int nOutLine, CMatBuf* cMemSub)
{
	int		nOffset					=		(int)		dPara[E_PARA_LINE_BLACK_OFFSET];			// 10
	int		nXYThreshold			=		(int)		dPara[E_PARA_LINE_BLACK_THRESHOLD_XY];		// 5
	int		nXXThreshold			=		(int)		dPara[E_PARA_LINE_BLACK_THRESHOLD_XX];		// 20
	int		nYXThreshold			=		(int)		dPara[E_PARA_LINE_BLACK_THRESHOLD_YX];		// 5
	int		nYYThreshold			=		(int)		dPara[E_PARA_LINE_BLACK_THRESHOLD_YY];		// 20
	int		nThickness				=		(int)		dPara[E_PARA_LINE_BLACK_THICKNESS];			// 1
	int		nPixelDistance			=		(int)		dPara[E_PARA_LINE_BLACK_PIXEL_DISTANCE];	// 10

	uchar PtrValue, Subvalue;

	cv::Mat tmpImage;	 

	// 벡터 초기화
	vector <int>	vecX, vecY;
	vector <int>().swap(vecX);
	vector <int>().swap(vecY);

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	if (nDirection == 1)
	{
		cv::Mat Profile = matProjection[0];
		MakeProfile(E_PROFILE_ROW, matSrcImage, Profile, &cMatBufTemp);

		int nRows = Profile.rows;

		uchar* BuffValue = (uchar*)Profile.data;

		for (int y=0 ; y < nRows ; y++)
		{
			PtrValue = BuffValue[y * Profile.cols + 0];

			if( PtrValue >= nXYThreshold )
				vecY.push_back( y );
		}

		for (int y = 0; y<vecY.size(); y++)
		{
			vector <int>().swap(vecX);
			// 범위 설정
			cv::Rect rectTemp;
			rectTemp.x		 = 0;
			rectTemp.y		 = vecY[y] - nOffset;
			rectTemp.width	 = matSrcImage.cols-1;
			rectTemp.height = nOffset*2;

			// 예외처리
			if (rectTemp.y < 0) rectTemp.y = 0;
			if (rectTemp.y + rectTemp.height >= matSrcImage.rows) rectTemp.height = matSrcImage.rows-rectTemp.y-1;			  

			// ROI
			tmpImage = matSrcImage(rectTemp);

			cv::Mat subProfile = matProjection[1];
			MakeProfile(E_PROFILE_COL, tmpImage, subProfile, &cMatBufTemp);

			int nCols	  = subProfile.cols;

			uchar* BuffV = (uchar*)subProfile.data;
			for (int x=0 ; x < nCols ; x++)
			{
				Subvalue = BuffV[x * subProfile.rows + 0];

				if( Subvalue > nXXThreshold )
					vecX.push_back( x );
			}

			if (vecX.size() == 0)
			{
				vecX.push_back(0);
				vecX.push_back(0);
			}
			// KSW17.09.30 - 버그 수정
			for (int x=0 ; x < vecX.size()-1 ; x++)
			{
				int nCom1 = vecX[x];
				int nCom2 = vecX[x+1];

				// 거리 차이가 별로 없으면 이어 붙이기
				if ( abs(nCom1 - nCom2) <= nPixelDistance ) 
					cv::line(matDstImage, cv::Point(nCom1, vecY[y]), cv::Point(nCom2, vecY[y]), cv::Scalar(255, 255, 255), 1);
			}
		}
	}
	else if (nDirection == 2)
	{
		cv::Mat Profile = matProjection[2];
		MakeProfile(E_PROFILE_COL, matSrcImage, Profile, &cMatBufTemp);

		int nCols = Profile.cols;

		uchar* BuffValue = (uchar*)Profile.data;

		for (int y=0 ; y < nCols ; y++)
		{
			PtrValue = BuffValue[y* Profile.rows + 0];

			if( PtrValue >= nYXThreshold )
				vecY.push_back( y );
		}

		for (int y = 0; y<vecY.size(); y++)
		{
			vector <int>().swap(vecX);
			//범위 설정
			cv::Rect rectTemp;
			rectTemp.x		 = vecY[y] - nOffset; 
			rectTemp.y		 = 0;
			rectTemp.width	 = nOffset*2;
			rectTemp.height = matSrcImage.rows-1;

			// 예외처리
			if (rectTemp.x < 0) rectTemp.x = 0;
			if (rectTemp.x + rectTemp.width >= matSrcImage.cols) rectTemp.width = matSrcImage.cols-rectTemp.x-1;

			tmpImage = matSrcImage(rectTemp);

			cv::Mat subProfile = matProjection[3];
			MakeProfile(E_PROFILE_ROW, tmpImage, subProfile, &cMatBufTemp);

			int nRow  = subProfile.rows;
			uchar* BuffV = (uchar*)subProfile.data;

			for (int x=0 ; x < nRow ; x++)
			{
				Subvalue = BuffV[x* subProfile.cols + 0];

				if( Subvalue > nYYThreshold) 
					vecX.push_back( x );
			}
			if (vecX.size() == 0)
			{
				vecX.push_back(0);
				vecX.push_back(0);
			}

			// KSW17.09.30 - 버그 수정
			for (int x=0 ; x < vecX.size()-1 ; x++)
			{
				int nCom1 = vecX[x];
				int nCom2 = vecX[x+1];

				// 거리 차이가 별로 없으면 이어 붙이기
				if ( abs(nCom1 - nCom2) <= nPixelDistance ) 
					cv::line(matDstImage, cv::Point(vecY[y], nCom1), cv::Point(vecY[y], nCom2), cv::Scalar(255, 255, 255), 1);
			}
		}
	}

	vector <int>().swap(vecX);
	vector <int>().swap(vecY);

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

}

//Profile
void CInspectLine::MakeProfile(int nType, Mat &matSrcImage, Mat &matDstProjection, CMatBuf* cMemSub)
{
	int nRow, nCol, ndepth;

	nRow	= matSrcImage.rows;
	nCol	= matSrcImage.cols;
	ndepth  = matSrcImage.depth();

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	if(nType == E_PROFILE_ROW)
	{
		cv::Mat matDstImageBuff = cMatBufTemp.GetMat(nRow, 1, CV_64F, false);

		for(int i=0 ; i< nRow ; i++)
		{
			matDstImageBuff.row(i) = cv::sum(matSrcImage.row(i))[0];
		}		

		matDstImageBuff /= nCol;

		matDstImageBuff.convertTo(matDstProjection, ndepth);
	}

	if(nType == E_PROFILE_COL)
	{
		cv::Mat matDstImageBuff = cMatBufTemp.GetMat(1, nCol, CV_64F, false);

		for(int i=0 ; i< nCol ; i++)
		{
			matDstImageBuff.col(i) = cv::sum(matSrcImage.col(i))[0];
		}

		matDstImageBuff /= nRow;

		matDstImageBuff.convertTo(matDstProjection, ndepth);
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

}

 void CInspectLine::Sdtthreshold(cv::Mat matSrcImage, cv::Mat& matDstImage, float fThresholdRatio)
 {
	cv::Mat tempMean, tempStd;
	double minvalue = 0;
	double maxvlaue = 0;
	cv::meanStdDev(matSrcImage, tempMean, tempStd);
	cv::minMaxIdx(matSrcImage, &minvalue, &maxvlaue, NULL, NULL);

	double Meanvalue = tempMean.at<double>(0,0);	// 평균
	double Stdvalue  = tempStd.at<double>(0,0);	// 표준 편차
	double dbthresh	 = 0;	

	if ((Meanvalue <= 0.1) && (Stdvalue <= 5))
	{ dbthresh  = 1; }

	else if (((Meanvalue > 0.1) && (Meanvalue <= 1) )&& (Stdvalue <= 5)) 
	{ dbthresh  = 1; }

	else if (((Meanvalue > 0.1) && (Meanvalue <= 1) )&& ((Stdvalue > 5.0)  && (Stdvalue <= 10.0))) 

	{ dbthresh  = Stdvalue * fThresholdRatio; }

	else if ((Meanvalue > 1.0) && (Stdvalue > 10))
	{ dbthresh  = Meanvalue * Stdvalue; }

	else
	{	dbthresh  = maxvlaue / 2;	}


	// 이진화
	cv::threshold(matSrcImage, matDstImage, dbthresh, 255.0, THRESH_BINARY);
 }
 
 long CInspectLine::calcLine_BrightX(cv::Mat& matSrcImage, cv::Mat& matDstImage, cv::Mat& matTempBuf1, cv::Mat& matTempBuf2, CRect rectResize, double* dPara, int* nCommonPara, CString strAlgPath)
 {
	 int	nMorpOpen	= (int)dPara[E_PARA_LINE_RGB_MORP_OPEN	];
	 int	nMorpClose	= (int)dPara[E_PARA_LINE_RGB_MORP_CLOSE	];

	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int			nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool			bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&			nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int			nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int			nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int			nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int			nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int			nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	 //////////////////////////////////////////////////////////////////////////


	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Start"));

	 cv::Rect rtInspROI;
	 //rtInspROI.x = rectResize.left - (nMorpOpen * 4 + 1);
	 //rtInspROI.y = rectResize.top - (nMorpOpen * 4 + 1);
	 //rtInspROI.width = rectResize.Width() + (nMorpOpen * 4 + 1) * 2;
	 //rtInspROI.height = rectResize.Height() + (nMorpOpen * 4 + 1) * 2;

	 Insp_RectSet(rtInspROI, rectResize, matSrcImage.cols, matSrcImage.rows, (nMorpOpen * 4 + 1));

	 // X라인과 Y라인만 남겨 놓기위한 Morphology
	 Morphology(matSrcImage(rtInspROI), matTempBuf1(rtInspROI), nMorpOpen*4+1, 1, EP_MORP_OPEN);

	 //rtInspROI.x = rectResize.left - nMorpClose;
	 //rtInspROI.y = rectResize.top - nMorpClose;
	 //rtInspROI.width = rectResize.Width() + nMorpClose * 2;
	 //rtInspROI.height = rectResize.Height() + nMorpClose * 2;

	 Insp_RectSet(rtInspROI, rectResize, matSrcImage.cols, matSrcImage.rows, nMorpClose);

	 // 라인 살짝 떨어져 있는거 붙이기
	 Morphology(matTempBuf1(rtInspROI), matTempBuf2(rtInspROI), nMorpClose, 1, EP_MORP_CLOSE);

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_Bright_Defect_Morphology_X.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 cv::imwrite((cv::String)(CStringA)strTemp, matTempBuf2);
	 }

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Morphology."));

	 // 이진화 영상을 Upsize 함
	 cv::resize( matTempBuf2, matDstImage, matDstImage.size(), 0, 0, 0);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("resize."));

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::calcLine_BrightY(cv::Mat& matSrcImage, cv::Mat& matDstImage, cv::Mat& matTempBuf1, cv::Mat& matTempBuf2, CRect rectResize, double* dPara, int* nCommonPara, CString strAlgPath)
 {
	 int	nMorpOpen	= (int)dPara[E_PARA_LINE_RGB_MORP_OPEN	];
	 int	nMorpClose	= (int)dPara[E_PARA_LINE_RGB_MORP_CLOSE	];

	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int			nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool			bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&			nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int			nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int			nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int			nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int			nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int			nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	 //////////////////////////////////////////////////////////////////////////

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Start"));

	 cv::Rect rtInspROI;
	 //rtInspROI.x = rectResize.left - (nMorpOpen * 4 + 1);
	 //rtInspROI.y = rectResize.top - (nMorpOpen * 4 + 1);
	 //rtInspROI.width = rectResize.Width() + (nMorpOpen * 4 + 1) * 2;
	 //rtInspROI.height = rectResize.Height() + (nMorpOpen * 4 + 1) * 2;

	 Insp_RectSet(rtInspROI, rectResize, matSrcImage.cols, matSrcImage.rows, (nMorpOpen * 4 + 1));

	 // X라인과 Y라인만 남겨 놓기위한 Morphology
	 Morphology(matSrcImage(rtInspROI), matTempBuf1(rtInspROI), 1, nMorpOpen*4+1, EP_MORP_OPEN);

	 //rtInspROI.x = rectResize.left - nMorpClose;
	 //rtInspROI.y = rectResize.top - nMorpClose;
	 //rtInspROI.width = rectResize.Width() + nMorpClose * 2;
	 //rtInspROI.height = rectResize.Height() + nMorpClose * 2;

	 Insp_RectSet(rtInspROI, rectResize, matSrcImage.cols, matSrcImage.rows, nMorpClose);

	 // 라인 살짝 떨어져 있는거 붙이기
	 Morphology(matTempBuf1(rtInspROI), matTempBuf2(rtInspROI), 1, nMorpClose, EP_MORP_CLOSE);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Morphology."));

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_Bright_Defect_Morphology_Y.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 cv::imwrite((cv::String)(CStringA)strTemp, matTempBuf2);
	 }

	 // 이진화 영상을 Upsize 함
	 cv::resize( matTempBuf2, matDstImage, matDstImage.size(), 0, 0, 0);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("resize."));

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::calcLine_DarkX(cv::Mat& matSrcImage, cv::Mat& matDstImage, cv::Mat& matTempBuf1, cv::Mat& matTempBuf2, CRect rectResize, double* dPara, int* nCommonPara, CString strAlgPath)
 {
	 int	nMorpOpen	= (int)dPara[E_PARA_LINE_RGB_MORP_OPEN	];
	 int	nMorpClose	= (int)dPara[E_PARA_LINE_RGB_MORP_CLOSE	];

	 //////////////////////////////////////////////////////////////////////////
	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int			nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool			bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&			nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int			nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int			nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int			nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int			nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int			nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	 //////////////////////////////////////////////////////////////////////////

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Start"));

	cv::Rect rtInspROI;
	//rtInspROI.x = rectResize.left - (nMorpOpen * 4 + 1);
	//rtInspROI.y = rectResize.top - (nMorpOpen * 4 + 1);
	//rtInspROI.width = rectResize.Width() + (nMorpOpen * 4 + 1) * 2;
	//rtInspROI.height = rectResize.Height() + (nMorpOpen * 4 + 1) * 2;

	Insp_RectSet(rtInspROI, rectResize, matSrcImage.cols, matSrcImage.rows, (nMorpOpen * 4 + 1));

	 // X라인과 Y라인만 남겨 놓기위한 Morphology
	 Morphology(matSrcImage(rtInspROI), matTempBuf1(rtInspROI), nMorpOpen*4+1, 1, EP_MORP_OPEN);

	 //rtInspROI.x = rectResize.left - nMorpClose;
	 //rtInspROI.y = rectResize.top - nMorpClose;
	 //rtInspROI.width = rectResize.Width() + nMorpClose * 2;
	 //rtInspROI.height = rectResize.Height() + nMorpClose * 2;

	 Insp_RectSet(rtInspROI, rectResize, matSrcImage.cols, matSrcImage.rows, nMorpClose);

	 // 라인 살짝 떨어져 있는거 붙이기
	 Morphology(matTempBuf1(rtInspROI), matTempBuf2(rtInspROI), nMorpClose, 1, EP_MORP_CLOSE);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Morphology."));

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_matBinaryMorphDrakX.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 cv::imwrite((cv::String)(CStringA)strTemp, matTempBuf2);
	 }

	 // 이진화 영상을 Upsize 함
	 cv::resize( matTempBuf2, matDstImage, matDstImage.size(), 0, 0, 0);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("resize."));

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::calcLine_DarkY(cv::Mat& matSrcImage, cv::Mat& matDstImage, cv::Mat& matTempBuf1, cv::Mat& matTempBuf2, CRect rectResize, double* dPara, int* nCommonPara, CString strAlgPath)
 {
	 int	nMorpOpen	= (int)dPara[E_PARA_LINE_RGB_MORP_OPEN	];
	 int	nMorpClose	= (int)dPara[E_PARA_LINE_RGB_MORP_CLOSE	];

	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	 //////////////////////////////////////////////////////////////////////////

	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Start"));
	
	cv::Rect rtInspROI;
	//rtInspROI.x = rectResize.left - (nMorpOpen * 4 + 1);
	//rtInspROI.y = rectResize.top - (nMorpOpen * 4 + 1);
	//rtInspROI.width = rectResize.Width() + (nMorpOpen * 4 + 1) * 2;
	//rtInspROI.height = rectResize.Height() + (nMorpOpen * 4 + 1) * 2;

	Insp_RectSet(rtInspROI, rectResize, matSrcImage.cols, matSrcImage.rows, (nMorpOpen * 4 + 1));

	 // X라인과 Y라인만 남겨 놓기위한 Morphology
	 Morphology(matSrcImage(rtInspROI), matTempBuf1(rtInspROI), 1, nMorpOpen*4+1, EP_MORP_OPEN);

	 //rtInspROI.x = rectResize.left - nMorpClose;
	 //rtInspROI.y = rectResize.top - nMorpClose;
	 //rtInspROI.width = rectResize.Width() + nMorpClose * 2;
	 //rtInspROI.height = rectResize.Height() + nMorpClose * 2;

	 Insp_RectSet(rtInspROI, rectResize, matSrcImage.cols, matSrcImage.rows, nMorpClose);

	 // 라인 살짝 떨어져 있는거 붙이기
	 Morphology(matTempBuf1(rtInspROI), matTempBuf2(rtInspROI), 1, nMorpClose, EP_MORP_CLOSE);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Morphology."));

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_Dark_Defect_Morphology_Y.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 cv::imwrite((cv::String)(CStringA)strTemp, matTempBuf2);
	 }

	 // 이진화 영상을 Upsize 함
	 cv::resize( matTempBuf2, matDstImage, matDstImage.size(), 0, 0, 0);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("resize."));

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::calcRGBMain(cv::Mat& matSrcImage, cv::Mat& matThImage, cv::Mat* matDstImage, cv::Mat* matBinaryMorp, double* dPara, int* nCommonPara,
	 CRect rectResize, cv::Point* ptCorner, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, CMatBuf* cMemSub)
 {
	 long		nSegX					=		(long)		dPara[E_PARA_LINE_RGB_SEG_X				];
	 long		nSegY					=		(long)		dPara[E_PARA_LINE_RGB_SEG_Y				];
	 float		fDarkRatio_RGB			=		(float)		dPara[E_PARA_LINE_RGB_DARK_RATIO_X		];
	 float		fBrightRatio_RGB		=		(float)		dPara[E_PARA_LINE_RGB_BRIGHT_RATIO_X	];
	 float		fDarkRatio_RGB_Edge		=		(float)		dPara[E_PARA_LINE_RGB_DARK_RATIO_Y		];
	 float		fBrightRatio_RGB_Edge	=		(float)		dPara[E_PARA_LINE_RGB_BRIGHT_RATIO_Y	];
	 long		nOutLine				=		(long)		dPara[E_PARA_LINE_RGB_OUTLINE			];
	 int		nWeakResize				=		(int)		dPara[E_PARA_LINE_RGB_WEAK_RESIZE		];

	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount			=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave				= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount			=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum				=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum					=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber				=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber		=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex				=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	 // 에러 코드
	 long nErrorCode = E_ERROR_CODE_TRUE;

	 CMatBuf cMatBufTemp;
	 cMatBufTemp.SetMem(cMemSub);	 

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Mat Create."));

	 // RangeThresholding Point용 Alg 사용
	 RangeAvgThreshold_Gray(matThImage, matDstImage, rectResize, 1, nSegX, nSegY, fDarkRatio_RGB, fBrightRatio_RGB, fDarkRatio_RGB_Edge, fBrightRatio_RGB_Edge, &cMatBufTemp);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("RangeAvgThreshold_Gray."));

	 // 최외곽 배열 제거
	 cv::line(matDstImage[E_DEFECT_COLOR_DARK]	, ptCorner[E_CORNER_LEFT_TOP]		/ nWeakResize, ptCorner[E_CORNER_RIGHT_TOP]		/ nWeakResize, cv::Scalar(0, 0, 0), nOutLine);
	 cv::line(matDstImage[E_DEFECT_COLOR_DARK]	, ptCorner[E_CORNER_RIGHT_TOP]		/ nWeakResize, ptCorner[E_CORNER_RIGHT_BOTTOM]	/ nWeakResize, cv::Scalar(0, 0, 0), nOutLine);
	 cv::line(matDstImage[E_DEFECT_COLOR_DARK]	, ptCorner[E_CORNER_RIGHT_BOTTOM]	/ nWeakResize, ptCorner[E_CORNER_LEFT_BOTTOM]	/ nWeakResize, cv::Scalar(0, 0, 0), nOutLine);
	 cv::line(matDstImage[E_DEFECT_COLOR_DARK]	, ptCorner[E_CORNER_LEFT_BOTTOM]	/ nWeakResize, ptCorner[E_CORNER_LEFT_TOP]		/ nWeakResize, cv::Scalar(0, 0, 0), nOutLine);
	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Out Line Delete."));

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_Dark.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 cv::imwrite((cv::String)(CStringA)strTemp, matDstImage[E_DEFECT_COLOR_DARK]);

		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_Bright.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 cv::imwrite((cv::String)(CStringA)strTemp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	 }

	 // 병렬 처리를 위한 Temp 메모리 할당
	 cv::Mat matTempBuf1[4];
	 cv::Mat matTempBuf2[4];
	 for (int i = 0; i < 4; i++)
	 {
		 matTempBuf1[i] = cMatBufTemp.GetMat(matThImage.size(), matSrcImage.type(), false);
		 matTempBuf2[i] = cMatBufTemp.GetMat(matThImage.size(), matSrcImage.type());
	 }

#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(4)
#endif
	 for (int i = 0; i < 4; i++)
	 {
		 switch (i)
		 {
		 case 0:
		 {
			 // Bright X
			 nErrorCode |= calcLine_BrightX(matDstImage[E_DEFECT_COLOR_BRIGHT], matBinaryMorp[E_BINARY_BRIGHT_X], matTempBuf1[0], matTempBuf2[0], rectResize, dPara, nCommonPara, strAlgPath);

			 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("calcLine_BrightX."));

		 }
		 break;
		 case 1:
		 {
			 // Bright Y
			 nErrorCode |= calcLine_BrightY(matDstImage[E_DEFECT_COLOR_BRIGHT], matBinaryMorp[E_BINARY_BRIGHT_Y], matTempBuf1[1], matTempBuf2[1], rectResize, dPara, nCommonPara, strAlgPath);

			 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("calcLine_BrightY."));

		 }
		 break;
		 case 2:
		 {
			 // Dark X
			 nErrorCode |= calcLine_DarkX(matDstImage[E_DEFECT_COLOR_DARK], matBinaryMorp[E_BINARY_DARK_X], matTempBuf1[2], matTempBuf2[2], rectResize, dPara, nCommonPara, strAlgPath);

			 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("calcLine_DarkX."));

		 }
		 break;
		 case 3:
		 {
			 // Dark Y
			 nErrorCode |= calcLine_DarkY(matDstImage[E_DEFECT_COLOR_DARK], matBinaryMorp[E_BINARY_DARK_Y], matTempBuf1[3], matTempBuf2[3], rectResize, dPara, nCommonPara, strAlgPath);

			 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("calcLine_DarkY."));

		 }
		 break;
		 }
	 }
	 if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

// 	 if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		 writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		 writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
//	 }

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::LogicStart_Weak(cv::Mat& matSrcImage, cv::Mat* matDstImage, vector<int> NorchIndex, CPoint OrgIndex, CRect rectROI, double* dPara,
	 int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Point* ptCorner)
 {

	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	 //////////////////////////////////////////////////////////////////////////

	 const int	nMaxGV			= 4096;

	 int		nOnOff			= dPara[E_PARA_LINE_RGB_WEAK_FLAG];	//on/off
	 // 노이즈 제거
	 double		dPow			=  2;	//= dPara[2];	// 2; // 1.2
	 int		nWindowSize		=  dPara[E_PARA_LINE_RGB_WEAK_GAUSSIAN];	//= dPara[3];	// 31;
	 double		dSigma			=  dPara[E_PARA_LINE_RGB_WEAK_PROJ_SIGAM];	//= dPara[4];	// 4;
	 int		nReSizeValue	=  dPara[E_PARA_LINE_RGB_WEAK_RESIZE];

	 // 최외곽선 제거
	 int		nOutLineDark	= dPara[E_PARA_LINE_RGB_WEAK_OUTLINEBRIGHT];	// 61;
	 int		nOutLineBright	= dPara[E_PARA_LINE_RGB_WEAK_OUTLINEDARK];	// 61;

	 // 불량 검출	
	 int		dbTargateGV		= dPara[E_PARA_LINE_RGB_WEAK_TARGET_GV];	// 50;

	 double		dbThresholdBX	= dPara[E_PARA_LINE_RGB_WEAK_BRIGHT_RATIO_X];	// 50;
	 double		dbThresholdBY	= dPara[E_PARA_LINE_RGB_WEAK_BRIGHT_RATIO_Y];	// 50;
	 double		dbThresholdDX	= dPara[E_PARA_LINE_RGB_WEAK_DARK_RATIO_X];	// 50;
	 double		dbThresholdDY	= dPara[E_PARA_LINE_RGB_WEAK_DARK_RATIO_Y];	// 50;

	 // 배경 만들기
	 int		nBlur			= dPara[E_PARA_LINE_RGB_WEAK_PROJ_BLUR1];	// 101;
	 int		nBlur2			= dPara[E_PARA_LINE_RGB_WEAK_PROJ_BLUR2];	// 71;
	 int		nRange			= dPara[E_PARA_LINE_RGB_WEAK_PROJ_RANGE];	// 20;
	 int		nMorp			= dPara[E_PARA_LINE_RGB_WEAK_PROJ_MORPHOLOGY];	// 51;

	 //////////////////////////////////////////////////////////////////////////

	 // 에러 코드
	 long	nErrorCode = E_ERROR_CODE_TRUE;
	 if( nOnOff == 0 ) return nErrorCode;

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_Src.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 ImageSave( strTemp, matSrcImage );
	 }

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Start"));

	 CMatBuf cMatBufTemp;
	 cMatBufTemp.SetMem(cMem);

	 // Input Image to Image Buffer
	 cv::Mat matTempBuf0 = cMatBufTemp.GetMat(matSrcImage.size(), CV_8UC1, false);
	 cv::Mat matTempBuf1 = cMatBufTemp.GetMat(matSrcImage.size(), CV_16U);
	 cv::Mat matTempBuf2 = cMatBufTemp.GetMat(matSrcImage.size(), CV_16U);

	 matSrcImage.copyTo(matTempBuf0);

	 // 평균 밝기 보정
	 if ( dbTargateGV > 0)
	 {
		 ApplyMeanGV( matTempBuf0, dbTargateGV, rectROI);

		 if( bImageSave )
		 {
			 CString strTemp;
			 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ApplyGV.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
			 cv::imwrite((cv::String)(CStringA)strTemp, matTempBuf0);
		 }

	 }

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Image Buffer"));

	 int nOffSet = 100;

	 cv::Rect rtInspROI;
	 //rtInspROI.x = rectROI.left - nOffSet;
	 //rtInspROI.y = rectROI.top - nOffSet;
	 //rtInspROI.width = rectROI.Width() + nOffSet * 2;
	 //rtInspROI.height = rectROI.Height() + nOffSet * 2;

	 Insp_RectSet(rtInspROI, rectROI, matTempBuf0.cols, matTempBuf0.rows, nOffSet);

	 // Image 8bit Pow
	 Image_Pow(matSrcImage.type(), dPow, matTempBuf0(rtInspROI), matTempBuf2(rtInspROI));
	 matTempBuf0.release();
	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Pow"));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_Pow.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 ImageSave( strTemp, matTempBuf2 );
	 }

	// 노이즈 제거
	resizeGaussian(matTempBuf2(rtInspROI), matTempBuf1(rtInspROI), nReSizeValue, nWindowSize, dSigma, &cMatBufTemp);
	matTempBuf2.release();
	writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("resize Gaussian Blur"));

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_Gaus.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave( strTemp, matTempBuf1 );
	}

	
// 	int var_brightness1 = 0;
// 	double var_contrast1 = 0.3; // 변수로 바꿔야함
// 	double c, d;
// 	if (var_contrast1 > 0)
// 	{
// 	double delta1 = 127.0*var_contrast1;
// 	c = 255.0 / (255.0 - delta1);
// 	d = c*(var_brightness1 - delta1);
// 	}
// 	else
// 	{
// 	double delta1 = -128.0*var_contrast1;
// 	c = (256.0 - delta1 * 2) / 255.0;
// 	d = c*var_brightness1 + delta1;
// 	}
// 
// 	cv::Mat Mat_test = matTempBuf1.clone();
// 
// 	cv::Mat temp1 = cMatBufTemp.GetMat(Mat_test.size(), Mat_test.type());
// 	cv::Mat dst1 = cMatBufTemp.GetMat(Mat_test.size(), Mat_test.type());
// 	Mat_test.copyTo(temp1);
// 
// 	temp1.convertTo(matTempBuf1, CV_8U, c, d);
// 	
// 
// 	if (bImageSave)
// 	{
// 		CString strTemp;
// 		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_Contrast.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
// 		ImageSave(strTemp, matTempBuf1);
// 	}








	cv::Mat matROIBuf = matTempBuf1( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );



	// Result Image
	cv::Mat matDstImage_Weak[E_BINARY_TOTAL_COUNT];
	
	matDstImage_Weak[E_BINARY_BRIGHT_X] = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());
	matDstImage_Weak[E_BINARY_BRIGHT_Y] = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());
	matDstImage_Weak[E_BINARY_DARK_X] = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());
	matDstImage_Weak[E_BINARY_DARK_Y] = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type());

	cv::Mat matProjectionX[5];
	cv::Mat matProjectionY[5];

	for (int i = 0; i < 5; i++)
	{
		matProjectionX[i] = cMatBufTemp.GetMat(1, matROIBuf.rows, matROIBuf.type(), false);
		matProjectionY[i] = cMatBufTemp.GetMat(1, matROIBuf.cols, matROIBuf.type(), false);
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
			/////////////////////////// Y Line ////////////////////////////

			calcWeakLine_Y(matROIBuf, matDstImage[E_BINARY_BRIGHT_Y], matDstImage[E_BINARY_DARK_Y], matProjectionY, NorchIndex, OrgIndex, rectROI, dbThresholdBY, dbThresholdDY,
				nBlur, nBlur2, nRange, nMorp, nOutLineBright, nOutLineDark, nCommonPara, strAlgPath, dPara);

			cv::bitwise_or(matDstImage_Weak[E_BINARY_BRIGHT_Y], matDstImage[E_BINARY_BRIGHT_Y], matDstImage[E_BINARY_BRIGHT_Y]);
			cv::bitwise_or(matDstImage_Weak[E_BINARY_DARK_Y], matDstImage[E_BINARY_DARK_Y], matDstImage[E_BINARY_DARK_Y]);
			///////////////////////////////////////////////////////////////
		}
			break;
		case 1:
		{
			/////////////////////////// X Line ////////////////////////////

			calcWeakLine_X(matROIBuf, matDstImage[E_BINARY_BRIGHT_X], matDstImage[E_BINARY_DARK_X], matProjectionX, rectROI, dbThresholdBX, dbThresholdDX,
				nBlur, nBlur2, nRange, nMorp, nOutLineBright, nOutLineDark, nCommonPara, strAlgPath, dPara);

			cv::bitwise_or(matDstImage_Weak[E_BINARY_BRIGHT_X], matDstImage[E_BINARY_BRIGHT_X], matDstImage[E_BINARY_BRIGHT_X]);
			cv::bitwise_or(matDstImage_Weak[E_BINARY_DARK_X], matDstImage[E_BINARY_DARK_X], matDstImage[E_BINARY_DARK_X]);
			///////////////////////////////////////////////////////////////
		}
			break;
		}
	}

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("End"));

// 	 if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		 writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		 writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	 }
 ///////////////////////////////////////////////////////////////
	 return E_ERROR_CODE_TRUE;
 }

 //YDS Image 증폭
 long CInspectLine::Image_Pow(int ImgType, double dpow, Mat& InPutImage, Mat& OutPutImage)
 {
	 double MaxValue = 4096;
	 if(ImgType == CV_8UC1)
	 {
		 ushort LUT[256] = {0};
		 double val = 0;
		 for(int i = 0; i < 256; i++)
		 {
			 val = pow(i, dpow);

			 if(val >= MaxValue) val = MaxValue - 1;
			 if(val < 0) val = 0;
			 LUT[i] = (ushort)val;
		 }
			 cv::MatIterator_<uchar> itSrc, endSrc;
			 itSrc	= InPutImage.begin<uchar>();
			 endSrc	= InPutImage.end<uchar>();
			 cv::MatIterator_<ushort> itDst = OutPutImage.begin<ushort>();

			 for ( ; itSrc != endSrc; itSrc++, itDst++)
				 *itDst = LUT[(*itSrc)];
	 }
	 else
	 {
		 InPutImage.copyTo(OutPutImage);
	 }

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::resizeGaussian(Mat& InPutImage, Mat& OutPutImage, int reSizeValue, int MaskSize, double dSigma, CMatBuf* cMemSub)
 {
	 
	 if (reSizeValue > 1)
	 {
		 CMatBuf cMatBufTemp;
		 cMatBufTemp.SetMem(cMemSub);

		 int reSizeWidth = InPutImage.cols / reSizeValue;
		 int reSizeHeight = InPutImage.rows / reSizeValue;

		 cv::Mat matreSizeBuf1 = cMatBufTemp.GetMat(cv::Size(reSizeWidth, reSizeHeight), CV_16U, false);
		 cv::Mat matreSizeBuf2 = cMatBufTemp.GetMat(cv::Size(reSizeWidth, reSizeHeight), CV_16U, false);

		 cv::resize(InPutImage, matreSizeBuf1, cv::Size(reSizeWidth, reSizeHeight), 0, 0, INTER_LINEAR);
		 cv::GaussianBlur(matreSizeBuf1, matreSizeBuf2, cv::Size(MaskSize / reSizeValue, MaskSize / reSizeValue), dSigma / reSizeValue);
		 cv::resize(matreSizeBuf2, OutPutImage, InPutImage.size(), 0, 0, INTER_LINEAR);

// 		 if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 			 writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 			 writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 		 }
	 }
	 else
		cv::GaussianBlur(InPutImage, OutPutImage, cv::Size(MaskSize, MaskSize), dSigma);


	 return E_ERROR_CODE_TRUE;
 }
 
 long CInspectLine::calcWeakLine_Y(Mat& InPutImage, Mat& OutPutImage1, Mat& OutPutImage2, Mat *matProjectionY, vector<int> NorchIndex, CPoint OrgIndex, CRect rectROI, double dbThresholdBY, double dbThresholdDY,
	 int nBlur, int nBlur2, int nRange, int nMorp, int nOutLineBright, int nOutLineDark, int* nCommonPara, CString strAlgPath, double* dPara)
 {
	 // Norch 사용 유무
	 int		nNorchOnOff			= dPara[E_PARA_LINE_RGB_NORCH_ONOFF];	// 51;
	 int		nNorchUnit=0;
	 if(nNorchOnOff > 0)
		nNorchUnit					= (NorchIndex[0] + NorchIndex[2]) / 2;

	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	 //////////////////////////////////////////////////////////////////////////

	 int width	= InPutImage.cols;
	 int height = InPutImage.rows;

	 cv::Mat MatproYBuf = matProjectionY[0];
	 MakeProjection(InPutImage, MatproYBuf, width, height, 1, nNorchOnOff, nNorchUnit, NorchIndex, OrgIndex);

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProY.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatproYBuf.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatproYBuf.at<ushort>(0, i) );
			 }

			 fclose(out);
		 }
	 }

	 cv::Mat MatproYBuf1 = matProjectionY[1];
	 calcProjection(MatproYBuf, MatproYBuf1, matProjectionY, width, nBlur, nBlur2, nRange, nMorp, 1, nCommonPara, strAlgPath);

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProY_Blur2.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatproYBuf1.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatproYBuf1.at<ushort>(0, i));
			 }

			 fclose(out);
		 }
	 }
	 calcWeakLine_BrigtY(MatproYBuf, MatproYBuf1, matProjectionY, OutPutImage1, rectROI, nNorchUnit, width, dbThresholdBY, nOutLineBright, nCommonPara, strAlgPath, dPara);
	 calcWeakLine_DarkY(MatproYBuf1, MatproYBuf, matProjectionY, OutPutImage2, rectROI, nNorchUnit, width,  dbThresholdDY, nOutLineDark, nCommonPara, strAlgPath, dPara);

	 MatproYBuf.release();
	 MatproYBuf1.release();
	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::calcWeakLine_X(Mat& InPutImage, Mat& OutPutImage1, Mat& OutPutImage2, Mat* matProjectionX, CRect rectROI, double dbThresholdBX, double dbThresholdDX,
	 int nBlur, int nBlur2, int nRange, int nMorp, int nOutLineBright, int nOutLineDark, int* nCommonPara, CString strAlgPath, double* dPara)
 {
	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	 //////////////////////////////////////////////////////////////////////////

	 int width = InPutImage.cols;
	 int height = InPutImage.rows;

	 cv::Mat MatproXBuf = matProjectionX[0];

	 vector<int> ValueBuff;

	 MakeProjection(InPutImage, MatproXBuf, width, height, 0, 0, 0, ValueBuff, 0);



	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProX.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatproXBuf.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatproXBuf.at<ushort>(0, i) );
			 }

			 fclose(out);
		 }
	 }

	 cv::Mat MatproXBuf1 = matProjectionX[1];
	 calcProjection(MatproXBuf, MatproXBuf1, matProjectionX, height, nBlur, nBlur2, nRange, nMorp, 0, nCommonPara, strAlgPath);

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProX_Blur2.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatproXBuf1.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatproXBuf1.at<ushort>(0, i));
			 }

			 fclose(out);
		 }
	 }

	 calcWeakLine_BrigtX(MatproXBuf, MatproXBuf1, matProjectionX, OutPutImage1, rectROI, height, dbThresholdBX, nOutLineBright, nCommonPara, strAlgPath, dPara);
	 calcWeakLine_DarkX(MatproXBuf1, MatproXBuf, matProjectionX, OutPutImage2, rectROI, height, dbThresholdDX, nOutLineDark, nCommonPara, strAlgPath, dPara);

	 MatproXBuf.release();
	 MatproXBuf1.release();
	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::MakeProjection(Mat& InPutImage, Mat&MatproSrc, int width, int height, int Type, int UseNorch, int NorchUnit, vector<int> NorchIndex, CPoint OrgIndex)
 {
	 if ( UseNorch == NULL ) UseNorch = 0;

	 // Y 방향 Projection Norch Projection Off
	 if( Type == 1 && UseNorch == 0 )
	 {
		 ushort *ptr = (ushort *)MatproSrc.ptr(0);

		 for (int i=0 ; i<width ; i++, ptr++)
		 {
			 *ptr = (ushort)( cv::sum( InPutImage.col(i) )[0] / (double)height );
		 }
	 }

	 // Y 방향 Projection Norch Projection On
	 else if( Type == 1 && UseNorch == 1 )
	 {
		 ushort *ptr = (ushort *)MatproSrc.ptr(0);

		 // Norch 부 ROI 정보 받아 오기
 		 int TopLength	  = NorchIndex[0];
 		 int TopHeight	  = NorchIndex[1];
 		 int BottomLength = NorchIndex[2];
 		 int BottomHeight = NorchIndex[3];

		 int BottomStartY = OrgIndex.y;

		 cv::Mat NorchTop			 = InPutImage( cv::Rect(0 , 0,				 TopLength,		TopHeight ));
		 cv::Mat NorchBottom		 = InPutImage( cv::Rect(0 , BottomStartY,	 BottomLength,	BottomHeight ));

		 // 예외처리 19.04.02 PNZ
		 int nNorchLengthUnit = min(TopLength, BottomLength);

		  for (int i=0 ; i<nNorchLengthUnit; i++, ptr++)
		  {
			 *ptr = (ushort)( ( cv::sum( NorchTop.col(i) )[0] + cv::sum( NorchBottom.col(i) )[0] ) / ( TopHeight + BottomHeight ) );
		  }


		 for (int i= nNorchLengthUnit; i<width ; i++, ptr++)
		 {
			 *ptr = (ushort)( cv::sum( InPutImage.col(i) )[0] / (double)height );
		 }
	 }

	 // X 방향 Projection
	 else if( Type == 0 )
	 {
		 ushort *ptr = (ushort *)MatproSrc.ptr(0);

		 for (int i=0 ; i<height ; i++, ptr++)
		 {
			 *ptr = (ushort)( cv::sum( InPutImage.row(i) )[0] / (double)width );
		 }
	 }

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Image Projection"));

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::calcProjection(Mat& MatproSrc, Mat& MatproDst, Mat* matProjection, int size, int nBlur, int nBlur2,
	 int nRange, int nMorp, int Type, int* nCommonPara, CString strAlgPath)
 {
	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	 //////////////////////////////////////////////////////////////////////////

	 cv::blur(MatproSrc, MatproDst, cv::Size(nBlur, 1));

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("pro blur"));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 if(Type == 1)		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProY_Blur.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 else if(Type == 0)	strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProX_Blur.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatproDst.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatproDst.at<ushort>(0, i) );
			 }

			 fclose(out);
		 }
	 }

	 cv::Mat MatproMax = matProjection[2];
	 cv::Mat MatproMin = matProjection[3];

	 cv::Mat	StructElem = cv::getStructuringElement( MORPH_RECT, Size(nMorp, 1), Point((nMorp/2) + 1, 0));
	 // MORPH_ERODE
	 cv::morphologyEx(MatproSrc, MatproMin, MORPH_ERODE, StructElem);
	 // MORPH_DILATE
	 cv::morphologyEx(MatproSrc, MatproMax, MORPH_DILATE, StructElem);
	 StructElem.release();

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Min/Max Filterilng"));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 if(Type == 1)		strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProY_Morp.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 else if(Type == 0)	strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProX_Morp.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 fprintf_s(out, "No,Src,Blur,Min,Max\n");

			 for (int i=0 ; i<MatproSrc.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d,%d,%d,%d\n", i, MatproSrc.at<ushort>(0, i), MatproDst.at<ushort>(0, i), MatproMin.at<ushort>(0, i), MatproMax.at<ushort>(0, i) );
			 }

			 fclose(out);
		 }
	 }

	 ushort *ptrBlur	= (ushort *)MatproDst.ptr(0);
	 ushort *ptrErode	= (ushort *)MatproMin.ptr(0);
	 ushort *ptrDilate	= (ushort *)MatproMax.ptr(0);

	 for (int i=0 ; i<MatproDst.cols ; i++, ptrBlur++, ptrErode++, ptrDilate++)
	 {
		 if( *ptrDilate - *ptrErode < nRange)
		 {
			 *ptrErode = *ptrBlur;
			 continue;
		 }

		 // 편차 적은 GV 넣기
		 if( *ptrBlur - *ptrErode > *ptrDilate - *ptrBlur )
			 *ptrErode = *ptrDilate;
	 }

	 cv::blur(MatproMin, MatproDst, cv::Size(nBlur2, 1));

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("MainProcessilng"));
	 
	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::calcWeakLine_BrigtY(Mat& MatproSrc, Mat& MatproDst, Mat* matProjectionY, Mat& OutPutImage, CRect rectROI, int nNorchUnit, int size,
	 double dbThresholdBY, int nOutLineBright, int* nCommonPara, CString strAlgPath, double* dPara)
 {
	 //////////////////////////////////////////////////////////////////////////
	 // 검사 Parameter
	 double		dInspRatio			= dPara[E_PARA_LINE_RGB_NORCH_INSP_RATIO_BRIGHT		];

	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	 //////////////////////////////////////////////////////////////////////////

	 cv::Mat MatproSub = matProjectionY[2];
	 cv::subtract(MatproSrc, MatproDst, MatproSub);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Subtract"));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProY_B_Sub.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatproSub.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatproSub.at<ushort>(0, i) );
			 }

			 fclose(out);
		 }
	 }

	 // RANSAC Proceed Data
	 cv::Mat MatSubRANSAC_NR	= matProjectionY[3];
	 cv::Mat MatSubRANSAC_Max	= matProjectionY[4];

	 MatSubRANSAC_NR.setTo(0);
	 MatSubRANSAC_Max.setTo(0);

	 ProfileMaxFilter(MatproSub, MatSubRANSAC_Max, size, nCommonPara, strAlgPath, dPara, nOutLineBright);

	 // Norch Value Enhancement
	 NorchValueProcess(MatproSub, MatSubRANSAC_Max, size, nCommonPara, strAlgPath, dPara, nOutLineBright, nNorchUnit);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Y Bright Norch Process."));

	 RangRANSACProcess(MatSubRANSAC_Max, MatSubRANSAC_NR, size, nCommonPara, strAlgPath, dPara);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Y Bright RANSAC Process."));

	 // Rang Threshold
	 RRM_Thresholding(MatproSub, MatSubRANSAC_NR, OutPutImage, rectROI, nNorchUnit, size, dbThresholdBY, nOutLineBright, 1, nCommonPara, strAlgPath, dPara, dInspRatio);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Y Bright RANSAC Threshold."));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_BY_Th.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 ImageSave( strTemp, OutPutImage );
	 }

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::calcWeakLine_DarkY(Mat& MatproSrc, Mat& MatproDst, Mat* matProjectionY, Mat& OutPutImage, CRect rectROI, int nNorchUnit, int size,
	 double dbThresholdDY, int nOutLineDark, int* nCommonPara, CString strAlgPath, double* dPara)
 {
	 //////////////////////////////////////////////////////////////////////////
	 // 검사 Parameter
	 double		dInspRatio			= dPara[E_PARA_LINE_RGB_NORCH_INSP_RATIO_DARK		];

	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	 //////////////////////////////////////////////////////////////////////////

	 cv::Mat MatproSub = matProjectionY[2];
	 cv::subtract(MatproSrc, MatproDst, MatproSub);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Subtract"));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProY_D_Sub.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatproSub.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatproSub.at<ushort>(0, i) );
			 }

			 fclose(out);
		 }
	 }

	 // RANSAC Proceed Data
	 cv::Mat MatSubRANSAC_NR = matProjectionY[3];
	 cv::Mat MatSubRANSAC_Max = matProjectionY[4];

	 MatSubRANSAC_NR.setTo(0);
	 MatSubRANSAC_Max.setTo(0);

	 ProfileMaxFilter(MatproSub, MatSubRANSAC_Max, size, nCommonPara, strAlgPath, dPara, nOutLineDark);

	 // Norch Value Enhancement
	 NorchValueProcess(MatproSub, MatSubRANSAC_Max, size, nCommonPara, strAlgPath, dPara, nOutLineDark, nNorchUnit);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Y Dark RANSAC Norch Process."));

	 RangRANSACProcess(MatSubRANSAC_Max, MatSubRANSAC_NR, size, nCommonPara, strAlgPath, dPara);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Y Dark RANSAC Process."));

	 // Rang Threshold
	 RRM_Thresholding(MatproSub, MatSubRANSAC_NR, OutPutImage, rectROI, nNorchUnit, size,  dbThresholdDY, nOutLineDark, 1, nCommonPara, strAlgPath, dPara, dInspRatio);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Y Dark RANSAC Threshold"));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_DY_Th.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 ImageSave( strTemp, OutPutImage );
	 }

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::calcWeakLine_BrigtX(Mat& MatproSrc, Mat& MatproDst, Mat* matProjectionX, Mat& OutPutImage, CRect rectROI, int size,
	 double dbThresholdBX, int nOutLineBright, int* nCommonPara, CString strAlgPath, double* dPara)
 {
	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	 //////////////////////////////////////////////////////////////////////////

	 cv::Mat MatproSub = matProjectionX[2];
	 cv::subtract(MatproSrc, MatproDst, MatproSub);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Subtract"));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProX_B_Sub.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatproSub.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatproSub.at<ushort>(0, i) );
			 }

			 fclose(out);
		 }
	 }

	 // RANSAC Proceed Data
	 cv::Mat MatSubRANSAC_NR = matProjectionX[3];
	 cv::Mat MatSubRANSAC_Max	= matProjectionX[4];

	 MatSubRANSAC_NR.setTo(0);
	 MatSubRANSAC_Max.setTo(0);

	 ProfileMaxFilter(MatproSub, MatSubRANSAC_Max, size, nCommonPara, strAlgPath, dPara, nOutLineBright);

	 RangRANSACProcess(MatSubRANSAC_Max, MatSubRANSAC_NR, size, nCommonPara, strAlgPath, dPara);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("X Bright RANSAC Process."));

	 // Rang Threshold
	 RRM_Thresholding(MatproSub, MatSubRANSAC_NR, OutPutImage, rectROI, 0, size, dbThresholdBX, nOutLineBright, 0, nCommonPara, strAlgPath, dPara, 0);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("X Bright RANSAC Threshold."));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_BX_Th.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 ImageSave( strTemp, OutPutImage );
	 }

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::calcWeakLine_DarkX(Mat& MatproSrc, Mat& MatproDst, Mat* matProjectionX, Mat& OutPutImage, CRect rectROI, int size,
	 double dbThresholdDX, int nOutLineDark, int* nCommonPara, CString strAlgPath, double* dPara)
 {
	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	 //////////////////////////////////////////////////////////////////////////

	 cv::Mat MatproSub = matProjectionX[2];
	 cv::subtract(MatproSrc, MatproDst, MatproSub);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("Subtract"));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_ProX_D_Sub.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatproSub.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatproSub.at<ushort>(0, i) );
			 }

			 fclose(out);
		 }
	 }

	 // RANSAC Proceed Data
	 cv::Mat MatSubRANSAC_NR = matProjectionX[3];
	 cv::Mat MatSubRANSAC_Max	= matProjectionX[4];

	 MatSubRANSAC_NR.setTo(0);
	 MatSubRANSAC_Max.setTo(0);

	 ProfileMaxFilter(MatproSub, MatSubRANSAC_Max, size, nCommonPara, strAlgPath, dPara, nOutLineDark);

	 RangRANSACProcess(MatSubRANSAC_Max, MatSubRANSAC_NR, size, nCommonPara, strAlgPath, dPara);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("X Dark RANSAC Process."));

	 // Rang Threshold
	 RRM_Thresholding(MatproSub, MatSubRANSAC_NR, OutPutImage, rectROI, 0, size, dbThresholdDX, nOutLineDark, 0, nCommonPara, strAlgPath, dPara, 0);

	 writeInspectLog(E_ALG_TYPE_AVI_LINE, __FUNCTION__, _T("X Dark RANSAC Threshold."));

	 // 중간 Image 확인
	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_DX_Th.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		 ImageSave( strTemp, OutPutImage );
	 }

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::RangeThreshold_Weak(Mat& MatproSub, Mat& OutPutImage, CRect rectROI, int size, 
	 double dbThreshold, int nOutLine, int Type, int* nCommonPara, CString strAlgPath)
 {
	 int SubUnite		= 300;
	 int nPixelCount	= 0;
	 int nPixelSum		= 0;
	 int SubStart		= 0;
	 int SubEnd			= 0;
	 double SubAverage	= 0;

	 int Subrange	 = size/SubUnite;

	 ushort *ptr;

	 for (int i=0 ; i<Subrange; i++)
	 {
		 nPixelSum = 0;
		 nPixelCount = 0;

		 SubStart = i*SubUnite;
		 if(i==Subrange-1)	SubEnd = size;
		 else				SubEnd = SubStart+SubUnite;

		 ptr = (ushort *)MatproSub.ptr(0) + SubStart;

		 for (int j=SubStart ; j<SubEnd; j++, ptr++)
		 {
			 nPixelSum += *ptr;
			 nPixelCount++;
		 }

		 SubAverage	= (double) nPixelSum/nPixelCount;

		 if(SubStart < nOutLine			)		SubStart	=	nOutLine;
		 if(SubEnd	 > size - nOutLine	)		SubEnd		=	size - nOutLine;

		 ptr = (ushort *)MatproSub.ptr(0) + SubStart;

		 if(Type == 1)
		 {
			 for (int j=SubStart ; j<SubEnd; j++, ptr++)
			 {
				 // 값이 일정 이상 작으면 넘어가기
				 if( *ptr < 5 || *ptr < SubAverage*dbThreshold )	continue;

				 // 값이 존재하면 불량 존재 ( 라인 그리기 )
				 cv::line(OutPutImage, cv::Point(rectROI.left + j, rectROI.top), cv::Point(rectROI.left + j, rectROI.bottom), cv::Scalar(255), 1);
			 }
		 }

		 else if(Type == 0)
		 {
			 for (int j=SubStart ; j<SubEnd; j++, ptr++)
			 {
				 // 값이 일정 이상 작으면 넘어가기
				 if( *ptr < 5 || *ptr < SubAverage*dbThreshold )	continue;

				 // 값이 존재하면 불량 존재 ( 라인 그리기 )
				 cv::line(OutPutImage, cv::Point(rectROI.left, rectROI.top + j), cv::Point(rectROI.right, rectROI.top + j), cv::Scalar(255), 1);
			 }
		 }
	 }

	 return E_ERROR_CODE_TRUE;
 }

 long CInspectLine::RangRANSACProcess(Mat& MatproSub, Mat& MatSubRANSAC, int size, int* nCommonPara, CString strAlgPath, double* dPara)
 {
	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];
	
	 //////////////////////////////////////////////////////////////////////////
	 // Parameter
	 int		SubUnite			= dPara[E_PARA_LINE_RGB_RANSAC_TH_UNIT					];	

	 // Data 초기화
	 int SubStart	= 0;
	 int SubEnd		= 0;
	 int Subrange	= size/SubUnite;

	 long double TestValueA;
	 long double TestValueB;

	 vector<Point2i> TestVector;
	 vector<Point2i> TestVector_Unit;

	 ushort *ptr;

	 // Range Process
	 for (int i=0 ; i<Subrange; i++)
	 {
		 SubStart = i*SubUnite;
		 if(i==Subrange-1)	SubEnd = size;
		 else				SubEnd = SubStart+SubUnite;

		 ptr = (ushort *)MatproSub.ptr(0) + SubStart;

		 // Parameter 초기화
		 int nDataPosition	= 0; 
		 int nDataIndex		= 0;

		 vector <Point2i>().swap(TestVector_Unit);

		 for (int j=SubStart ; j<SubEnd; j++, ptr++)
		 {
			 int nDataValue		= *ptr;
			 TestVector_Unit.push_back(Point2f(nDataPosition, nDataValue));
			 nDataPosition++;
		 }

		 // Fitting 값 구하기
		 calcRANSAC(TestVector_Unit, TestValueA, TestValueB);

		 // Org Data에 처리한 Data 빼기
		 ptr = (ushort *)MatproSub.ptr(0) + SubStart;

		 // 값 출력
		 for (int j=SubStart ; j<SubEnd; j++, ptr++)
		 {
			 double FitValue = (TestValueA * nDataIndex + TestValueB);
			 if (FitValue < 0) FitValue = 0; 
			 FitValue += 1;
			 TestVector.push_back(Point2f(j, FitValue));
			 nDataIndex++;
			 MatSubRANSAC.at<ushort>(0, j) = FitValue;
		 }
	 }

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_RANSAC_Data.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<TestVector.size() ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, TestVector[i].y );
			 }

			 fclose(out);
		 }
	 }

	 return E_ERROR_CODE_TRUE;
 }

 // MaxFilter Process
 long CInspectLine::ProfileMaxFilter(Mat& MatproSub, Mat& MatSubRANSAC, int size, int* nCommonPara, CString strAlgPath, double* dPara, int nOutLine)
 {
	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	 //////////////////////////////////////////////////////////////////////////
	 // Parameter
	 int		SubUnite			= dPara[E_PARA_LINE_RGB_RANSAC_MAXFILTER_SIZE		];		
	 double		dbMeanRatio			= dPara[E_PARA_LINE_RGB_RANSAC_AVG_TH_RATIO			];

	 // Norch 검사 사용한지 확인
	 if (SubUnite == 0) return false;


	 // Data 초기화
	 int SubStart	= 0;
	 int SubEnd		= 0;
	 int Subrange	= size/SubUnite + 1;

	 vector<int>	TestVector;
	 vector<int>	TestVector_Unit;
	 vector<int>	TestVector_MC; // Max-Filter Cut data

	 ushort *ptr;

	 // Range Process
	 for (int i=0 ; i<Subrange; i++)
	 {
		 SubStart = i*SubUnite;
		 if(i==Subrange-1)	SubEnd = size;
		 else				SubEnd = SubStart+SubUnite;

		 ptr = (ushort *)MatproSub.ptr(0) + SubStart;

		 vector <int>().swap(TestVector_Unit);

		 for (int j=SubStart ; j<SubEnd; j++, ptr++)
		 {
			 int nDataValue		= *ptr;
			 TestVector_Unit.push_back(nDataValue);
		 }

		 double minvalue = 0;
		 double maxvlaue = 0;

		 cv::minMaxIdx(TestVector_Unit, &minvalue, &maxvlaue, NULL, NULL);

		 for (int j=SubStart ; j<SubEnd; j++ )
		 {
			 int nDataValue	= maxvlaue;
			 TestVector.push_back(nDataValue);
		 }
	 }

	//////////////////////////////////////////////////////////////////////////
	// MaxFilter Data PostProcess
	 
	 // Max Data Info
	 double dblMean;
	 cv::Scalar m = cv::mean(TestVector);
	 dblMean = m[0];
	 int nMaxFiterData;

	 int nGVSum	  = 0;
	 int nGVCount = 0;

	 for( int i = 0 ; i < TestVector.size() ; i ++ )
	 {
		 if ( i < nOutLine || i > TestVector.size() - nOutLine )
		 {
			 nMaxFiterData = dblMean;
		 }

		 else
		 {
			nMaxFiterData = TestVector[i];
			if ( nMaxFiterData > dblMean*dbMeanRatio ) nMaxFiterData = dblMean;
		 }

		 TestVector_MC.push_back(nMaxFiterData);
		 MatSubRANSAC.at<ushort>(0, i) = nMaxFiterData;
	 }

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_MaxFilter_Data.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<TestVector.size() ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, TestVector[i] );
			 }

			 fclose(out);
		 }
	 }

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_MaxFilter_CutData.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<TestVector_MC.size() ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, TestVector_MC[i] );
			 }

			 fclose(out);
		 }
	 }

	 return E_ERROR_CODE_TRUE;
 }

 // MaxFilter Process
 long CInspectLine::NorchValueProcess(Mat& MatproSub,  Mat& MatNorchMaxADD, int size, int* nCommonPara, CString strAlgPath, double* dPara, int nOutLine, int nNorchLength)
 {
	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	 //////////////////////////////////////////////////////////////////////////
	 // Parameter
	 int		nNorchOnOff			= dPara[E_PARA_LINE_RGB_NORCH_ONOFF					];
	 int		nNorchMaxRange		= dPara[E_PARA_LINE_RGB_NORCH_MAXFILTER_SIZE		];
	 int		nNorchAvgRange		= dPara[E_PARA_LINE_RGB_NORCH_AVGFILTER_SIZE		];
	 int		nNorchMinGV			= dPara[E_PARA_LINE_RGB_NORCH_MIN_GV				];
	 double		dbNorchMeanRatio	= dPara[E_PARA_LINE_RGB_NORCH_AVG_TH_RATIO			];
	 

	 // Norch 검사 사용한지 확인 X 방향인지 확인
	 if ( nNorchOnOff == 0 || nNorchLength == 0 ) return false;

	 // Parameter 초기화
	 int		nNorchUnit_Max = nNorchLength / nNorchMaxRange;
	 int		nNorchUnit_Avg = nNorchLength / nNorchAvgRange;
	 int		nStart, nEnd, nMaxCut_Value, nDataValue, nDataCount;
	 ushort		*ptr, *ptr2;
	 
	 vector<int>  SubData, NorchData, vMaxFilterData, vAvgFilterData;

	 // Norch Data Average
	 ptr  = (ushort *)MatproSub.ptr(0);

	 nDataValue = 0;
	 nDataCount = 0;

	 for ( int i = 0 ; i < nNorchLength ; i++, ptr++)
	 {
		 int nValue = *ptr;
		 nDataValue += nValue;
		 nDataCount++;
	 }

	 double dbDataMean = ( double )	 nDataValue / nDataCount; 

	 // Norch Max 25
	 for ( int i = 0 ; i < nNorchUnit_Max ; i++)
	 {
		 nStart 	= i*nNorchMaxRange;
		 if(i==nNorchUnit_Max-1)		nEnd 	= nNorchLength;
		 else							nEnd 	= nStart+nNorchMaxRange;

		 vector <int>().swap(SubData);
		 ptr = (ushort *)MatproSub.ptr(0) + nStart;

		 for (int j=nStart ; j<nEnd; j++, ptr++)
		 {
			 SubData.push_back(*ptr);
		 }

		 double minvalue = 0;
		 double maxvlaue = 0;

		 cv::minMaxIdx(SubData, &minvalue, &maxvlaue, NULL, NULL);

		 for (int j=nStart ; j<nEnd; j++ )
		 {
			 vMaxFilterData.push_back(maxvlaue);
		 }
	 }

	 // Norch Avg 75
	 for ( int i = 0 ; i < nNorchUnit_Avg ; i++)
	 {
		 nStart 	= i*nNorchAvgRange;
		 if(i==nNorchUnit_Avg-1)		nEnd 	= nNorchLength;
		 else							nEnd 	= nStart+nNorchAvgRange;

		 vector <int>().swap(SubData);
		 ptr = (ushort *)MatproSub.ptr(0) + nStart;

		 for (int j=nStart ; j<nEnd; j++, ptr++)
		 {
			 SubData.push_back(*ptr);
		 }

		 cv::Scalar sSubMean = cv::mean(SubData);
		 double dbSubMean  = sSubMean[0];

		 for (int j=nStart ; j<nEnd; j++ )
		 {
			 vAvgFilterData.push_back(dbSubMean);
		 }
	 }

	 // Norch Max Cut & Enhancement
	 ptr  = (ushort *)MatproSub.ptr(0) + nOutLine;
	 ptr2 = (ushort *)MatNorchMaxADD.ptr(0) + nOutLine;

	 for ( int i = nOutLine ; i < nNorchLength; i ++, ptr++, ptr2++)
	 {
		 int nMF_Value	= vMaxFilterData[i];
		 int nAVG_Value = vAvgFilterData[i];

		 if ( nMF_Value > nAVG_Value * dbNorchMeanRatio )	 nMaxCut_Value = dbDataMean;
		 else												 nMaxCut_Value = nMF_Value;

		 *ptr2 = nMaxCut_Value;

		 if ( nMaxCut_Value <= *ptr)
		 {
			 int nOrg_Value = *ptr;
			 if ( nOrg_Value <= nNorchMinGV ) continue;
			 else *ptr = nOrg_Value * nOrg_Value;
		 }
	 }


	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_NorchEnhancement_Data.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatproSub.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatproSub.at<ushort>(0, i) );
			 }

			 fclose(out);
		 }
	 }

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_NorchMaxCut_Data.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<MatNorchMaxADD.cols ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, MatNorchMaxADD.at<ushort>(0, i) );
			 }

			 fclose(out);
		 }
	 }

	 return E_ERROR_CODE_TRUE;
 }


 // Range RANSAC MaxFilter Thresholding
 long	CInspectLine:: RRM_Thresholding(Mat& MatproSub, Mat& MatSubRANSAC, Mat& OutPutImage, CRect rectROI, int nNorchUnit,
	 int size, double dbThreshold, int nOutLine, int Type, int* nCommonPara, CString strAlgPath, double* dPara, double dInspRatio)
 {
	 //////////////////////////////////////////////////////////////////////////
	 // 공통 파라미터
	 int		nMaxDefectCount		=  nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];
	 bool		bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;
	 int&		nSaveImageCount		=  nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];
	 int		nImageNum			=  nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];
	 int		nCamNum				=  nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];
	 int		nROINumber			=  nCommonPara[E_PARA_COMMON_ROI_NUMBER				];
	 int		nAlgorithmNumber	=  nCommonPara[E_PARA_COMMON_ALG_NUMBER				];
	 int		nThrdIndex			=  nCommonPara[E_PARA_COMMON_THREAD_ID				];

	 // Parameter
	 int		nUseNorch			= dPara[E_PARA_LINE_RGB_NORCH_ONOFF					];  // Norch 검사 On/Off

	 // Data 초기화
	 ushort *ptr_Org, *prt_RANSAC;

	 vector<int> SubData;

	 int nDataStart = nOutLine;
	 int nDataEnd	= size - nOutLine;

	 // Profile Data
	 ptr_Org	= (ushort *)MatproSub.ptr(0);
	 prt_RANSAC = (ushort *)MatSubRANSAC.ptr(0);

	 for ( int i = 0; i < size ; i++, ptr_Org++, prt_RANSAC++ )
	 {
		 int nOrgData		= *ptr_Org;
		 int nRANSACData	= *prt_RANSAC;

		 int nSubValue		= nOrgData - nRANSACData;

		 if ( nSubValue < 0 ) nSubValue = 0;

		 SubData.push_back(nSubValue);
	 }

	 if( bImageSave )
	 {
		 CString strTemp;
		 strTemp.Format(_T("%s\\%02d_%02d_%02d_LINE_%02d_RRMData.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		 char szPath[MAX_PATH] = {0, };
		 memset(szPath, 0, sizeof(char) * MAX_PATH);
		 WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		 FILE *out = NULL;
		 fopen_s(&out, szPath, "wt");

		 if ( out )
		 {
			 for (int i=0 ; i<SubData.size() ; i++)
			 {
				 fprintf_s(out, "%d,%d\n", i, SubData[i] );
			 }

			 fclose(out);
		 }
	 }

	 // Thresholding
 	 for ( int i = nDataStart ; i < nDataEnd ; i++ )
 	 {
		// SubData[i] = SubData[i] + SubData[i];

 		 if ( Type == 1 && nUseNorch == 0 )
 		 {
 			if( SubData[i] < dbThreshold )	continue;
 
 			cv::line(OutPutImage, cv::Point(rectROI.left + i, rectROI.top), cv::Point(rectROI.left + i, rectROI.bottom), cv::Scalar(255), 1);
 		 }
 
 		 else if ( Type == 1 && nUseNorch == 1 )
 		 {
 			if ( i <= nNorchUnit )
 			{
 				if( SubData[i] < dInspRatio )	continue;
 
 				cv::line(OutPutImage, cv::Point(rectROI.left + i, rectROI.top), cv::Point(rectROI.left + i, rectROI.bottom), cv::Scalar(255), 1);
 			}
 
 			else if ( i > nNorchUnit )
 			{
 				if( SubData[i] < dbThreshold )	continue;
 
 				cv::line(OutPutImage, cv::Point(rectROI.left + i, rectROI.top), cv::Point(rectROI.left + i, rectROI.bottom), cv::Scalar(255), 1);
 			}
 		 }
 
 		 else if ( Type == 0 )
 		 {
 			if( SubData[i] < dbThreshold )	continue;
 
 			cv::line(OutPutImage, cv::Point(rectROI.left, rectROI.top + i), cv::Point(rectROI.right, rectROI.top + i), cv::Scalar(255), 1);
 		 }
 	 }


	 return E_ERROR_CODE_TRUE;
 }


 // 중복 데이터 비교
 bool CInspectLine::FindInSamples(vector <cv::Point2i>& ptSamples, cv::Point2i ptIndexs)
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
 long CInspectLine::GetRandomSamples(vector <cv::Point2i>& ptSrcIndexs, vector <cv::Point2i>& ptSamples, int nSampleCount)
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
 long CInspectLine::calcLineFit(vector <cv::Point2i>& ptSamples, long double& dA, long double& dB)
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
 long CInspectLine::calcLineVerification(vector <cv::Point2i>& ptSrcIndexs, vector <cv::Point2i>& ptInliers, long double& dA, long double& dB, double distThreshold)
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
 long CInspectLine::calcRANSAC(vector <cv::Point2i>& ptSrcIndexs, long double& dA, long double& dB)
 {
	 int	nMinSamples		= 5;
	 double	distThreshold	= 10;

	 // 갯수가 적을 경우 사용 안함
	 // 2개면 그냥 방정식으로 구하면 됨
	 // 원본 갯수보다 적어아야 함
	 if( nMinSamples < 2 )						return E_ERROR_CODE_ALIGN_WARNING_PARA;
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

	 return E_ERROR_CODE_TRUE;
 }

 // 8bit & 12bit 영상 저장
 long CInspectLine::ImageSave(CString strPath, cv::Mat matSrcBuf)
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

 //히스토그램으로 평균
 long CInspectLine::GetAverage_From_Histo(cv::Mat matHisto,int nMin,int nMax,double &dblAverage)
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
 long CInspectLine::GetMeanStdDev_From_Histo(cv::Mat matHisto,int nMin,int nMax,double &dblAverage,double &dblStdev)
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

 void CInspectLine::Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset)
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
 
 //choikwangil
 void CInspectLine::filter8(BYTE* pbyInImg, BYTE* pbyOutImg, int nMin, int nMax, int nWidth, int nHeight)
 {
	 long nSum;
	 double dvalue;

	 int coef[8] = { 13,14,20,14,7,-4,-7,-1 };

	 int nImagesize = nWidth * nHeight;

	 long* plImg = new long[nImagesize];
	 long* plLoG = new long[nImagesize];

	 //	memcpy(NewImg,InImg, width*height);
	 memset(plLoG, (long)0, nImagesize);


	 for (int i = 0; i < nImagesize; i++)
	 {
		 plImg[i] = (long)pbyInImg[i];

	 }

	 int nMaskSize = 8;


	 for (int x = 1; x < nWidth; x++)
	 {
		 plImg[x] += plImg[x - 1];
	 }

	 for (int y = 1; y < nHeight; y++)
	 {
		 plImg[y * nWidth] += plImg[(y - 1) * nWidth];
	 }
	 for (int y = 2; y < nHeight; y++)
	 {
		 for (int x = 2; x < nWidth; x++)
		 {
			 plImg[y * nWidth + x] += plImg[(y - 1) * nWidth + x] + plImg[y * nWidth + (x - 1)] - plImg[(y - 1) * nWidth + (x - 1)];
		 }
	 }

	 //nMax =-10000000;
	 //nMin = 10000000;
#ifndef _DEBUG
#pragma omp parallel for
#endif
	 for (int y = nMaskSize; y < nHeight - nMaskSize; y++)
	 {
		 for (int x = nMaskSize; x < nWidth - nMaskSize; x++)
		 {
			 nSum = 0;

			 for (int m = 0; m < nMaskSize; m++)
			 {
				 nSum += (long)(coef[m] * (plImg[(y + m) * nWidth + (x + m)]
					 - plImg[(y - (m + 1)) * nWidth + (x + m)]
					 - plImg[(y + m) * nWidth + (x - (m + 1))]
					 + plImg[(y - (m + 1)) * nWidth + (x - (m + 1))]));
			 }
			 plLoG[y * nWidth + x] = nSum;

			 /*if (plLoG[y * nWidth + x] > nMax)
			 {
			 nMax=plImg[y * nWidth + x];
			 }

			 if (plLoG[y * nWidth + x] < nMin)
			 {
			 nMin=plImg[y * nWidth + x];
			 }*/
		 }
	 }
	 //nMax = 8000;
	 //nMin =-8000;
#ifndef _DEBUG
#pragma omp parallel for
#endif
	 for (int y = 0; y < nHeight; y++)
	 {
		 for (int x = 0; x < nWidth; x++)
		 {
			 dvalue = ((double)((plLoG[y * nWidth + x] - nMin) * 255) / (double)(nMax - nMin));
			 if (dvalue < 0.0f)
			 {
				 pbyOutImg[y * nWidth + x] = (BYTE)0;
			 }
			 else if (dvalue>255.0f)
			 {
				 pbyOutImg[y * nWidth + x] = (BYTE)255;
			 }
			 else
			 {
				 pbyOutImg[y * nWidth + x] = (BYTE)dvalue;
			 }
		 }
	 }

	 delete[] plImg;
	 delete[] plLoG;
 }
