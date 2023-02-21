
/************************************************************************/
// Mura 불량 검출 관련 소스
// 수정일 : 18.05.31 - KSW
/************************************************************************/

#include "StdAfx.h"
#include "InspectMura.h"
#include <numeric>

// Pattern String
// E_IMAGE_CLASSIFY_SVI_COUNT 수정될 경우, 수정해야 함
CString gg_strPat[ E_IMAGE_CLASSIFY_AVI_COUNT ] = {
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
CString gg_strCam[ 2 ] = {
	_T("Coaxial"),
	_T("Side")
};


CInspectMura::CInspectMura(void)
{
	cMem				= NULL;
	m_cInspectLibLog	= NULL;
	m_strAlgLog			= NULL;
	m_tInitTime			= 0;
	m_tBeforeTime		= 0;
	sz					= 11;
}


CInspectMura::~CInspectMura(void)
{
}


// Main 검사 알고리즘
long CInspectMura::DoFindMuraDefect(cv::Mat matSrcBuffer,cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
	cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{
	// 파라미터 NULL인 경우
	if( dPara == NULL )					return E_ERROR_CODE_EMPTY_PARA;
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcBuffer.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;

	long	nWidth	= (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuffer.rows;	// 영상 세로 크기	

	// 범위 설정
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

	// 버퍼 할당 & 초기화
	cv::Mat matDstImage[E_DEFECT_COLOR_COUNT];

	// 알고리즘 영상 번호
	int nAlgImgNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];

	// 버퍼 할당
	matDstImage[E_DEFECT_COLOR_DARK]	= cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());
	matDstImage[E_DEFECT_COLOR_BRIGHT]	= cMem->GetMat(matSrcBuffer.size(), matSrcBuffer.type());

	// 결과 버퍼가 NULL이 아닌 경우 에러
	if( matDstImage[E_DEFECT_COLOR_DARK].empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstImage[E_DEFECT_COLOR_BRIGHT].empty() )	return E_ERROR_CODE_EMPTY_BUFFER;

	// 영상 마다 알고리즘 다름
	switch( nAlgImgNum )
	{
	case E_IMAGE_CLASSIFY_AVI_R:
	case E_IMAGE_CLASSIFY_AVI_G:
	case E_IMAGE_CLASSIFY_AVI_B:
		{
			// RGB Line Mura 검출
			nErrorCode = LogicStart_RGB_LINE_MURA(matSrcBuffer, matSrcBufferRGB, matDstImage, matBKBuffer, rectROI, dPara, nCommonPara, strAlgPath, EngineerDefectJudgment, pResultBlob);

			// KSW 18.09.21 - RGB 불량이 검출되어도, 누기 검사 해야 함
			// R, G, B Line 불량 검출한 경우
			//if ( nErrorCode == E_ERROR_CODE_MURA_RGB_LINE_DEFECT )
			//{
			//	// 할당 해제
			//	matDstImage[E_DEFECT_COLOR_BRIGHT].release();
			//	matDstImage[E_DEFECT_COLOR_DARK].release();
			//}
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
	case E_IMAGE_CLASSIFY_AVI_GRAY_128:
	{
		nErrorCode = LogicStart_MuraG3CM(matSrcBuffer, matBKBuffer, rectROI, dPara, nCommonPara,strAlgPath, EngineerDefectJudgment, pResultBlob);
	}
	break;
	case E_IMAGE_CLASSIFY_AVI_GRAY_32: 
	case E_IMAGE_CLASSIFY_AVI_GRAY_64:
	case E_IMAGE_CLASSIFY_AVI_GRAY_87:
	//case E_IMAGE_CLASSIFY_AVI_GRAY_128:
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
	if( !matDstImage[E_DEFECT_COLOR_BRIGHT].empty()		&&
		!matDstImage[E_DEFECT_COLOR_DARK].empty()	&&	
		nAlgImgNum != E_IMAGE_CLASSIFY_AVI_GRAY_128)
	{
		// 점등 영역 이외 검출 제거 ( 라운드 영역 제거 )
		if( !matBKBuffer.empty() )
		{
			cv::subtract(matDstImage[E_DEFECT_COLOR_BRIGHT]	, matBKBuffer, matBrightBuffer	);		//메모리 할당
			cv::subtract(matDstImage[E_DEFECT_COLOR_DARK]	, matBKBuffer, matDarkBuffer	);		//메모리 할당

			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Copy CV Sub Result."));
		}
		// 결과 넘기기
		else
		{			
// 			matBrightBuffer	= matDstImage[E_DEFECT_COLOR_BRIGHT].clone();		//메모리 할당
// 			matDarkBuffer	= matDstImage[E_DEFECT_COLOR_DARK].clone();			//메모리 할당

			matDstImage[E_DEFECT_COLOR_DARK].copyTo(matDarkBuffer);
			matDstImage[E_DEFECT_COLOR_BRIGHT].copyTo(matBrightBuffer);

			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Copy Clone Result."));
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


// Dust 제거 후, 결과 벡터 넘겨주기
long CInspectMura::GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
	double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
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

	// 사용 파라미터
	bool	bFlagW		= ( dPara[E_PARA_AVI_MURA_DUST_BRIGHT_FLAG					] > 0 ) ? true : false;
	bool	bFlagD		= ( dPara[E_PARA_AVI_MURA_DUST_DARK_FLAG					] > 0 ) ? true : false;
	int		nSize		= (int)dPara[E_PARA_AVI_MURA_DUST_BIG_AREA					];
	int		nRange		= (int)dPara[E_PARA_AVI_MURA_DUST_ADJUST_RANGE				];
	int     nWhiteMura_Judge_Flag = (int)dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_FLAG];
	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	//////////////////////////////////////////////////////////////////////////

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

	//////////////////////////////////////////////////////////////////////////

	// Dust 패턴 불량 없음
	if( nImageNum == E_IMAGE_CLASSIFY_AVI_DUST)
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
	if( !matDustBuffer[E_DEFECT_COLOR_BRIGHT].empty()	&&
		!matDustBuffer[E_DEFECT_COLOR_DARK].empty()		)
	{
		// 영상 비율 같은지 확인		
		if(	matSrcBuffer.rows == matDustBuffer[E_DEFECT_COLOR_DARK].rows	&&
			matSrcBuffer.cols == matDustBuffer[E_DEFECT_COLOR_DARK].cols	)
			nModePS = 1;
		else
			nModePS = 2;

		// 밝은 먼지 영상만
		//matDustBuffer[E_DEFECT_COLOR_BRIGHT].copyTo(matDustTemp);

		// 밝은 & 어두운 먼지 영상
		cv::add(matDustBuffer[E_DEFECT_COLOR_BRIGHT], matDustBuffer[E_DEFECT_COLOR_DARK], matDustTemp);

		// 큰 먼지만 남도록...
		nErrorCode = DeleteArea1(matDustTemp, nSize, &cMatBufTemp);
		if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DeleteArea."));
	}
	// Dust 영상 버퍼 없는 경우
	// 제거 로직 없이 불량 추출
	else
	{
		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Empty Dust Image."));
	}

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Dark_ResThreshold.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstBuffer[E_DEFECT_COLOR_DARK] );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bright_ResThreshold.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDstBuffer[E_DEFECT_COLOR_BRIGHT] );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bright_Dust.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDustTemp );
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
		cFeatureExtraction.SetMem( &cMatBufTemp );
		cFeatureExtraction.SetLog(m_cInspectLibLog, E_ALG_TYPE_AVI_MURA, m_tInitTime, m_tBeforeTime, m_strAlgLog);

		// E_DEFECT_COLOR_DARK 결과
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_DARK](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_DARK, _T("DM_"), EngineerDefectJudgment, pResultBlob);
		if( nErrorCode != E_ERROR_CODE_TRUE )
		{
			// 메모리 해제
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}
		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DoDefectBlobJudgment-Dark."));

		// Dust 면적 큰 주변인 경우 제거
		if( bFlagD )
		{
			nErrorCode = DeleteCompareDust(matDustTemp, nRange, pResultBlob, 0, nModePS);
			if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;
			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DeleteCompareDust-Dark."));
		}

		// Nugi 재분류
		JudgeNugi(matSrcBuffer, matDstBuffer[E_DEFECT_COLOR_DARK], rectROI, dPara, nCommonPara, strAlgPath, pResultBlob, &cMatBufTemp);
		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("JudgeNugi."));

		// Dark 불량 개수
		int nStartIndex = pResultBlob->nDefectCount;

		// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
		// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
		if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
			cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

		// 결과 외곽선 그리기
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

		//////////////////////////////////////////////////////////////////////////
// 
// 		BOOL bUse[MAX_MEM_SIZE_E_DEFECT_NAME_COUNT];
// 
// 		// 만약 Spot 검사 하는 경우
// 		if (EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse || EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse) //04.16 choi
// 		{
// 			// 백점 하기 위해 이진화 ( 백점 : 255 / 백무라 200 )
// 			cv::threshold(matDstBuffer[E_DEFECT_COLOR_BRIGHT], matResBuf, 220, 255.0, CV_THRESH_BINARY);
// 
// 			// 구분 파라미터 복사
// 			for (int p = 0; p < MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; p++)
// 			{
// 				bUse[p] = EngineerDefectJudgment[p].bDefectItemUse;
// 
// 				// 모든 파라미터 검사 해제
// 				EngineerDefectJudgment[p].bDefectItemUse = false;
// 			}
// 
// 			// Spot 검사만 설정
// 			EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse = true;
// 			EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse = true; //choi 04.16
// 
// 			// E_DEFECT_COLOR_BRIGHT 결과
// 			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matResBuf(rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
// 				nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("BM_"), EngineerDefectJudgment, pResultBlob);
// 			if (nErrorCode != E_ERROR_CODE_TRUE)
// 			{
// 				// 메모리 해제
// 				matSrcBuffer.release();
// 				matDstBuffer[E_DEFECT_COLOR_DARK].release();
// 				matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();
// 
// 				return nErrorCode;
// 			}
// 			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DoDefectBlobJudgment-Bright Spot."));
// 
// 			// White Spot 재분류
// 			JudgeWhiteSpot(matSrcBuffer, matDstBuffer[E_DEFECT_COLOR_BRIGHT], rectROI, dPara, nCommonPara, strAlgPath, pResultBlob, &cMatBufTemp);
// 			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("JudgeWhiteSpot."));
// 
// 			// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
// 			// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
// 			if (!USE_ALG_CONTOURS)	// 결과 외곽선 저장
// 				cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);
// 
// 			// 결과 외곽선 그리기
// 			cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);
// 
// 			// 원복
// 			for (int p = 0; p < MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; p++)
// 			{
// 				EngineerDefectJudgment[p].bDefectItemUse = bUse[p];
// 			}
// 		}

		//////////////////////////////////////////////////////////////////////////

		// 백무라 하기 위해 이진화 ( 백점 : 255 / 백무라 200 )
// 		cv::threshold(matDstBuffer[E_DEFECT_COLOR_BRIGHT], matResBuf, 190, 255.0, CV_THRESH_BINARY);
// 		
// 		// Spot 검사만 해제
// 		bUse[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT] = EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse;
// 		bUse[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT] = EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse; //04.16 choi
// 
// 		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse = false;
// 		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse = false;									//04.16 choi

		// E_DEFECT_COLOR_BRIGHT 결과
		nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer(rectBlobROI), matDstBuffer[E_DEFECT_COLOR_BRIGHT](rectBlobROI), matDrawBuffer(rectBlobROI), rectROI,
			nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("BM_"), EngineerDefectJudgment, pResultBlob);
		if( nErrorCode != E_ERROR_CODE_TRUE )
		{
			// 메모리 해제
			matSrcBuffer.release();
			matDstBuffer[E_DEFECT_COLOR_DARK].release();
			matDstBuffer[E_DEFECT_COLOR_BRIGHT].release();

			return nErrorCode;
		}
		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DoDefectBlobJudgment-Bright."));

		// 원복
// 		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT].bDefectItemUse = bUse[E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT];
// 		EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT].bDefectItemUse = bUse[E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT]; //04.16 choi

		// Dust 면적 큰 주변인 경우 제거
		if( bFlagW )
		{
			// nStartIndex : Dark 불량 개수 이후부터 시작
			nErrorCode = DeleteCompareDust(matDustTemp, nRange, pResultBlob, nStartIndex, nModePS);
			if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;
			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DeleteCompareDust-Bright."));
		}

		// White Mura 재분류
		if (nWhiteMura_Judge_Flag > 0) {
					JudgeWhiteMura(matSrcBuffer, matDstBuffer[E_DEFECT_COLOR_BRIGHT], rectROI, dPara, nCommonPara, strAlgPath, pResultBlob, &cMatBufTemp);
					writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("JudgeWhiteMura."));
		}
		// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
		// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
		if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
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


long CInspectMura::LogicStart_SPOT(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
	int* nCommonPara, CString strAlgPath)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;



	// 사용 파라미터
	int		nGauSize		= (int)dPara[E_PARA_AVI_MURA_COMMON_GAUSSIAN_SIZE				];
	double	dGauSig			= dPara[E_PARA_AVI_MURA_COMMON_GAUSSIAN_SIGMA					];

	//Inspect 사용 Flag
	int     nBright_Inpect_Flag		 = (int)dPara[E_PARA_AVI_MURA_GRAY_INSPECT_BRIGHT_FLAG];
	int     nDark_Inpect_Flag		 = (int)dPara[E_PARA_AVI_MURA_GRAY_INSPECT_DARK_FLAG];
	int     nBright_inspect_new_Flag = (int)dPara[E_PARA_AVI_MURA_GRAY_INSPECT_MID_BRIGHT_FLAG];

	int		nEdge_Area		= (int)dPara[E_PARA_AVI_MURA_GRAY_BRIGHT_THRESHOLD_WHITE_MURA_EDGE_AREA];
	int		nBrightTh1		= (int)dPara[E_PARA_AVI_MURA_GRAY_BRIGHT_THRESHOLD_WHITE_MURA_ACTIVE];
	int		nBrightTh2		= (int)dPara[E_PARA_AVI_MURA_GRAY_BRIGHT_THRESHOLD_WHITE_MURA_EDGE];
	int		nBrightMorp		= (int)dPara[E_PARA_AVI_MURA_GRAY_BRIGHT_MORP					];
	int		nBrightDelArea	= (int)dPara[E_PARA_AVI_MURA_GRAY_BRIGHT_DEL_AREA				];

	int		nDarkTh			= (int)dPara[E_PARA_AVI_MURA_GRAY_DARK_THRESHOLD				];
	int		nDarkMorp		= (int)dPara[E_PARA_AVI_MURA_GRAY_DARK_MORP						];
	int		nDarkDelArea	= (int)dPara[E_PARA_AVI_MURA_GRAY_DARK_DEL_AREA					];

	// 예외처리
	if( nBrightMorp % 2 == 0 )		nBrightMorp++;
	if( nDarkMorp % 2 == 0 )		nDarkMorp++;
	//if (nBrightTh1 < nBrightTh2)	nBrightTh2 = nBrightTh1;

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

	//////////////////////////////////////////////////////////////////////////
	//Mid Biright 용
	int nAdjust1_Min_GV = (int)dPara[E_PARA_AVI_MURA_GRAY_MID_BRIGHT_ADJUST1_MIN_GV]; //20
	int nResize_Loop_Cnt = (int)dPara[E_PARA_AVI_MURA_GRAY_MID_BRIGHT_RESIZE_LOOP_CNT]; //5

	double dContrast_Value = (double)dPara[E_PARA_AVI_MURA_GRAY_MID_BRIGHT_CONTRAST_VALUE]; //0.01
	int	   nAdjust2_Muti_Value = (int)dPara[E_PARA_AVI_MURA_GRAY_MID_BRIGHT_ADJUST2_MUTI_VALUE]; //3

	int    nCanny_th_min = (int)dPara[E_PARA_AVI_MURA_GRAY_MID_BRIGHT_CANNY_MIN]; //120
	int    nCanny_th_max = (int)dPara[E_PARA_AVI_MURA_GRAY_MID_BRIGHT_CANNY_MAX]; //255

	int    nEdge_Del_Bk_loop_cnt = (int)dPara[E_PARA_AVI_MURA_GRAY_MID_BRIGHT_EDGE_DEL_LOOP]; //10
	int    nEdge_Del_Bk_morp_size = (int)dPara[E_PARA_AVI_MURA_GRAY_MID_BRIGHT_EDGE_DEL_MORP_SIZE]; //21

	int    nMid_Bright_Del_Area = (int)dPara[E_PARA_AVI_MURA_GRAY_MID_BRIGHT_DEL_AREA]; //100

	int    nDefect_Morp_resize = (int)dPara[E_PARA_AVI_MURA_GRAY_MID_BRIGHT_DEFECT_MORP_RESIZE]; //80
	//////////////////////////////////////////////////////////////////////////


	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	// 검사 영역
	CRect rectTemp(rectROI);

	long	nWidth	= (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcImage.rows;	// 영상 세로 크기

	////////////////////////////////////////////////////////////////////////// choi 06.24 변경
	cv::Mat matSrcROIBuf_tmp = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), true);		 //Mid_Bright 용
	cv::Mat matSrcROIBuf_result = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);	 //Mid_Bright 용

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

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Src.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcROIBuf );
	}

	///////////////////////////////////////////////////////////////////////////*/
	// Blur
	cv::Mat matGauBuf = cMatBufTemp.GetMat( matSrcROIBuf.size(), matSrcROIBuf.type(), false);
	cv::GaussianBlur(matSrcROIBuf, matGauBuf, cv::Size(nGauSize, nGauSize), dGauSig, dGauSig);
	
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Gau.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matGauBuf );
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("GaussianBlur."));

	// 배경 영상
	cv::Mat matBGBuf = cMatBufTemp.GetMat( matGauBuf.size(), matGauBuf.type(), false);
	nErrorCode = Estimation_XY(matGauBuf, matBGBuf, dPara, &cMatBufTemp);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Esti.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBGBuf );
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Estimation_XY."));

	// 버퍼 할당
	cv::Mat matSubBrBuf	= cMatBufTemp.GetMat( matSrcROIBuf.size(), matSrcROIBuf.type(), false);
	cv::Mat matSubDaBuf	= cMatBufTemp.GetMat( matSrcROIBuf.size(), matSrcROIBuf.type(), false);

	cv::Mat matThBrBuf = cMatBufTemp.GetMat(matSrcROIBuf.size(), matSrcROIBuf.type(), false);
	cv::Mat matThBrBuf_Act	= cMatBufTemp.GetMat(matSrcROIBuf.size(), matSrcROIBuf.type(), false);
	cv::Mat matThBrBuf_Edge = cMatBufTemp.GetMat(matSrcROIBuf.size(), matSrcROIBuf.type(), false);

	cv::Mat matThDaBuf	= cMatBufTemp.GetMat(matSrcROIBuf.size(), matSrcROIBuf.type(), false);
	cv::Mat matTempBuf	= cMatBufTemp.GetMat(matSrcROIBuf.size(), matSrcROIBuf.type(), false);

#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif
	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
		{
			//////////////////////////////////////////////////////////////////////////
			// 밝은 불량 찾기
			//////////////////////////////////////////////////////////////////////////
			// 빼기
			if (nBright_Inpect_Flag > 0) {
				cv::subtract(matGauBuf, matBGBuf, matSubBrBuf);

			if (bImageSave)
			{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bright_Sub.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, matSubBrBuf);
			}

			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("subtract - Bright."));

			//////////////////////////////////////////////////////////////////////////choi 06.04
			
			cv::Rect R_Mask(0 + nEdge_Area, 0 + nEdge_Area, matSubBrBuf.cols - (nEdge_Area * 2), matSubBrBuf.rows - (nEdge_Area * 2));

			cv::Mat matActive_Mask = cMatBufTemp.GetMat(matSubBrBuf.size(), matSubBrBuf.type(), false);
			cv::Mat matEdge_Mask = cMatBufTemp.GetMat(matSubBrBuf.size(), matSubBrBuf.type(), false);

			matActive_Mask(R_Mask).setTo(255);
			cv::bitwise_not(matActive_Mask, matEdge_Mask);

			cv::Mat matActive_Br_Temp = cMatBufTemp.GetMat(matSubBrBuf.size(), matSubBrBuf.type(), false);
			cv::Mat matEdge_Br_Temp = cMatBufTemp.GetMat(matSubBrBuf.size(), matSubBrBuf.type(), false);

			cv::bitwise_and(matSubBrBuf, matActive_Mask, matActive_Br_Temp);
			cv::bitwise_and(matSubBrBuf, matEdge_Mask, matEdge_Br_Temp);

			cv::threshold(matActive_Br_Temp, matThBrBuf_Act, nBrightTh1, 255.0, CV_THRESH_BINARY);
			cv::threshold(matEdge_Br_Temp, matThBrBuf_Edge, nBrightTh2, 255.0, CV_THRESH_BINARY);

			if (bImageSave)
			{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bright_Th_Active.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, matThBrBuf_Act);
			}

			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("threshold - Bright."));

			if (bImageSave)
			{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bright_Th_Edge.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, matThBrBuf_Edge);
			}

			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("threshold - Bright."));

			cv::bitwise_or(matThBrBuf_Act, matThBrBuf_Edge, matThBrBuf);

			matActive_Mask.release();
			matEdge_Mask.release();
			matActive_Br_Temp.release();
			matEdge_Br_Temp.release();
			matThBrBuf_Act.release();
			matThBrBuf_Edge.release();
			//////////////////////////////////////////////////////////////////////////

			// 이진화
// 			cv::threshold(matSubBrBuf, matThBrBuf, nBrightTh1, 255.0, CV_THRESH_BINARY);
// 			if (bImageSave)
// 			{
// 				CString strTemp;
// 				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bright_Th.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
// 				ImageSave(strTemp, matThBrBuf);
// 			}
// 
// 			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("threshold - Bright."));

			// 불량 붙이기
			if (nBrightMorp > 1)
				cv::morphologyEx(matThBrBuf, matBrROIBuf, MORPH_CLOSE, cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(nBrightMorp, nBrightMorp)));
			else
				matThBrBuf.copyTo(matBrROIBuf);

			if (bImageSave)
			{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bright_Morp.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, matBrROIBuf);
			}

			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("morphologyEx - Bright."));

			// 배경 빼고 안쪽것만...
			// Edge 쪽에 불량이 존재하는 경우 라인 이어버림
			if (!matBkROIBuf.empty())
				cv::subtract(matBrROIBuf, matBkROIBuf, matBrROIBuf);
			}
			else {
				matBrROIBuf.setTo(0);
				break;
			}
			// 백무라용 이진화  ( 백점 : 255 / 백무라 200 )
			//cv::threshold(matSubBrBuf, matTempBuf, nBrightTh2, 200.0, CV_THRESH_BINARY);

		}
			break;
		case 1:
		{
			//////////////////////////////////////////////////////////////////////////
			// 어두운 불량 찾기
			//////////////////////////////////////////////////////////////////////////
			// 빼기
			if (nDark_Inpect_Flag > 0) {
				cv::subtract(matBGBuf, matGauBuf, matSubDaBuf);
				if (bImageSave)
				{
					CString strTemp;
					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Dark_Sub.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
					ImageSave(strTemp, matSubDaBuf);
				}

			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("subtract - Dark."));

			// 이진화
			cv::threshold(matSubDaBuf, matThDaBuf, nDarkTh, 255.0, CV_THRESH_BINARY);
			if (bImageSave)
			{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Dark_Th.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, matThDaBuf);
			}

			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("threshold - Dark."));

			// 누기 불량임...
			if (nDarkMorp > 1)
				cv::morphologyEx(matThDaBuf, matDaROIBuf, MORPH_OPEN, cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(nDarkMorp, nDarkMorp)));
			else
				matThDaBuf.copyTo(matDaROIBuf);
			if (bImageSave)
			{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Dark_Morp.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, matDaROIBuf);
			}

			writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("morphologyEx - Dark."));
			}
			else {
				matDaROIBuf.setTo(0);
				break;
			}

		}
			break;
		case 2:
		{
			//////////////////////////////////////////////////////////////////////////
			// 중간 크기 밝은 불량 찾기
			//////////////////////////////////////////////////////////////////////////
			// 빼기
			if (nBright_inspect_new_Flag > 0) {
				/////////////////////////////////////////////////////////////////////////// 07.02 Test

				cv::Mat matResize = cMatBufTemp.GetMat(matSrcROIBuf_tmp.size(), matSrcROIBuf_tmp.type());
				matSrcImage.copyTo(matResize);

				cv::Mat matTmp = cMatBufTemp.GetMat(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()).size(), matSrcImage.type(), false);
				matResize(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height())).copyTo(matTmp);

				//////////////////////////////////////////////////////////////////////////
				//어두운 부분 평균 값으로 올리기
				//////////////////////////////////////////////////////////////////////////
				cv::Scalar m, s;
				cv::meanStdDev(matTmp, m, s);

				for (int i = 0; i < matTmp.cols*matTmp.rows; i++) {
				if (matTmp.data[i] < nAdjust1_Min_GV) {
				matTmp.data[i] = m[0];
				}
				}

				matTmp.copyTo(matResize(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height())));

				if (bImageSave)
				{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Adjust1.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, matResize);
				}

				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				//리사이즈
				//////////////////////////////////////////////////////////////////////////
				for (int i = 0; i < nResize_Loop_Cnt; i++) {
				cv::resize(matResize, matResize, matResize.size()/2, 3, 3, INTER_AREA);
				}


				if (bImageSave)
				{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_resize.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, matResize);
				}

				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				//콘트라스트
				//////////////////////////////////////////////////////////////////////////
				int var_brightness1 = 0;
				double var_contrast1 = dContrast_Value;
				double c, d;
				if (var_contrast1 > 0)
				{
				double delta1 = 127.0*var_contrast1;
				c = 255.0 / (255.0 - delta1 * 2);
				d = c*(var_brightness1 - delta1);
				}
				else
				{
				double delta1 = -128.0*var_contrast1;
				c = (256.0 - delta1 * 2) / 255.0;
				d = c*var_brightness1 + delta1;
				}

				cv::Mat temp1 = cMatBufTemp.GetMat(matResize.size(), matResize.type());
				cv::Mat dst1 = cMatBufTemp.GetMat(matResize.size(), matResize.type());
				matResize.copyTo(temp1);

				temp1.convertTo(dst1, CV_8U, c, d);

				if (bImageSave)
				{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_contrast.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, dst1);
				}
				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				//밝기 보정
				//////////////////////////////////////////////////////////////////////////
				for (int i = 0; i < dst1.cols*dst1.rows; i++)
				{

				if (dst1.data[i] * nAdjust2_Muti_Value >= 255)
				{
				dst1.data[i] = 255;
				}
				else
				dst1.data[i] = dst1.data[i] * nAdjust2_Muti_Value;
				}

				if (bImageSave)
				{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Adjust2.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, dst1);
				}
				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				//멕시칸 필터
				//////////////////////////////////////////////////////////////////////////
				C_Mexican_filter(dst1);

				if (bImageSave)
				{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Mexican.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, dst1);
				}

				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				//캐니 엣지
				//////////////////////////////////////////////////////////////////////////
				cv::Canny(dst1, dst1, nCanny_th_min, nCanny_th_max, 3, false);

				if (bImageSave)
				{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Canny.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, dst1);
				}

				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				//사이즈 원복
				//////////////////////////////////////////////////////////////////////////

				cv::resize(dst1, dst1, matSrcROIBuf_result.size(), 3, 3, INTER_AREA);

				if (bImageSave)
				{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Canny_inv_resize.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, dst1);
				}
				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				//사이드 과검 제거 
				//////////////////////////////////////////////////////////////////////////
				cv::Mat matBkTmp = cMatBufTemp.GetMat(matBKBuffer.size(), matBKBuffer.type(), false);
				matBKBuffer.copyTo(matBkTmp);

				for (int i = 0; i < nEdge_Del_Bk_loop_cnt; i++) {
					cv::morphologyEx(matBkTmp, matBkTmp, MORPH_DILATE, cv::getStructuringElement(MORPH_RECT, cv::Size(nEdge_Del_Bk_morp_size, nEdge_Del_Bk_morp_size)));
				}

				cv::subtract(dst1, matBkTmp, dst1);

				if (bImageSave)
				{
				CString strTemp;
				strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Result.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
				ImageSave(strTemp, dst1);
				}
				//////////////////////////////////////////////////////////////////////////

				dst1.copyTo(matSrcROIBuf_result);
			}
			else {
				matSrcROIBuf_tmp.setTo(0);
				break;
			}
		}

		break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 밝은 불량 찾기
	//////////////////////////////////////////////////////////////////////////
	if (nBright_Inpect_Flag > 0) 
	{
		// 작은 면적 제거 & 라인 잇기
		nErrorCode = DeleteArea3(matBrROIBuf, nBrightDelArea, 10, &cMatBufTemp);
		if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DeleteArea - Bright."));

// 		// 작은 면적 제거
// 		nErrorCode = DeleteArea1(matTempBuf, nBrightDelArea, &cMatBufTemp);
// 		if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;
// 
// 		// 불량 통합
// 		cv::add(matBrROIBuf, matTempBuf, matBrROIBuf);

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bright_Del.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matBrROIBuf);
		}

		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Th Add - Bright."));
	}

	//////////////////////////////////////////////////////////////////////////
	// 어두운 불량 찾기
	//////////////////////////////////////////////////////////////////////////
	if (nDark_Inpect_Flag > 0)
	{
		// 작은 면적 제거
		nErrorCode = DeleteArea1(matDaROIBuf, nDarkDelArea, &cMatBufTemp);
		if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Dark_Del.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matDaROIBuf );
		}

		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DeleteArea - Dark."));
	}

	//////////////////////////////////////////////////////////////////////////
	// 중간 밝은 불량 찾기
	//////////////////////////////////////////////////////////////////////////
	if (nBright_inspect_new_Flag > 0) {

		nErrorCode = DeleteArea3(matSrcROIBuf_result, nMid_Bright_Del_Area, 10, &cMatBufTemp);
		if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DeleteArea - Bright."));

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Result_Delete.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matSrcROIBuf_result);
		}

		vector<vector<cv::Point>>	contours;
		vector<vector<cv::Point>>().swap(contours);
		vector<cv::Point > hierarchy;
		vector<cv::Point >().swap(hierarchy);

		cv::findContours(matSrcROIBuf_result, contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

		// 	for (int m = 0; m < contours.size(); m++)
		// 	{
		// 		for (int k = 0; k < contours.at(m).size(); k++)
		// 			ptContours.push_back(contours[m][k]);
		// 	}

		// 	// convexHull
		// 	for (int i = 0; i < contours.size(); i++) {
		// 		cv::convexHull(contours[i], ptConvexHull[i]);
		// 		cv::fillConvexPoly(matDstImage[E_DEFECT_COLOR_BRIGHT], ptConvexHull[i], cv::Scalar(255, 255, 255));
		// 	}
		// 	vector<vector<cv::Point>>().swap(contours);
		// 	vector<vector<cv::Point>>().swap(ptConvexHull);

		for (int k = 0; k < contours.size(); k++)
		{
			drawContours(matSrcROIBuf_tmp, contours, k, Scalar(255), CV_FILLED, 8, hierarchy);
		}

		vector<vector<cv::Point>>().swap(contours);
		vector<cv::Point >().swap(hierarchy);

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Result_Contours.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matSrcROIBuf_tmp);
		}

		cv::resize(matSrcROIBuf_tmp, matSrcROIBuf_tmp, matSrcImage.size() / nDefect_Morp_resize, 3, 3, INTER_AREA);
		cv::resize(matSrcROIBuf_tmp, matSrcROIBuf_tmp, matSrcImage.size() , 3, 3, INTER_AREA);

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Result_Morp.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matSrcROIBuf_tmp);
		}

		cv::bitwise_or(matDstImage[E_DEFECT_COLOR_BRIGHT],matSrcROIBuf_tmp, matDstImage[E_DEFECT_COLOR_BRIGHT]);
	}
	return nErrorCode;
}


long CInspectMura::LogicStart_RGB_LINE_MURA(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
	int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 사용 파라미터
	int		nResize			= (int)dPara[E_PARA_AVI_MURA_RGB_RESIZE				];
	int		nGauSize		= (int)dPara[E_PARA_AVI_MURA_RGB_GAUSSIAN_SIZE		];
	double	dGauSig			= dPara[E_PARA_AVI_MURA_RGB_GAUSSIAN_SIGMA			];
	int		nEMD_Edge_Area	= (int)dPara[E_PARA_AVI_MURA_RGB_DARK_EDGE_AREA		];
	int		nEdge_DarkTh	= (int)dPara[E_PARA_AVI_MURA_RGB_DARK_EDGE_THRESHOLD];
	int		nAct_DarkTh		= (int)dPara[E_PARA_AVI_MURA_RGB_DARK_ACTIVE_THRESHOLD];
	int		nDarkMorp		= (int)dPara[E_PARA_AVI_MURA_RGB_DARK_MORP			];
	float	fPow			= (float)dPara[E_PARA_AVI_MURA_RGB_POW				];
	int		nBlurX			= (int)dPara[E_PARA_AVI_MURA_RGB_BLUR_X				];
	int		nBlurY			= (int)dPara[E_PARA_AVI_MURA_RGB_BLUR_Y1			];
	int		nBlurY2			= (int)dPara[E_PARA_AVI_MURA_RGB_BLUR_Y2			];
	int		nEdgeArea		= (int)dPara[E_PARA_AVI_MURA_RGB_EDGE_AREA			];
	int		nActiveTh		= (int)dPara[E_PARA_AVI_MURA_RGB_THRESHOLD			];
	int		nEdgeTh			= (int)dPara[E_PARA_AVI_MURA_RGB_EDGE_THRESHOLD		];
	int		nCut			= (int)dPara[E_PARA_AVI_MURA_RGB_INSIDE				];

	if( nResize <= 0 )	return E_ERROR_CODE_MURA_WRONG_PARA;


	int nNugi_Inspect_Flag		= (int)dPara[E_PARA_AVI_MURA_RGB_INSPECT_DARK_FLAG];
	int nMuraLine_Inspect_Flag	= (int)dPara[E_PARA_AVI_MURA_RGB_INSPECT_LINE_MURA_FLAG];

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
	int		nPS					=  nCommonPara[E_PARA_COMMON_PS_MODE				];

	//////////////////////////////////////////////////////////////////////////

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	long	nWidth	= (long)matSrcImage.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcImage.rows;	// 영상 세로 크기

	// 원본
	cv::Mat matSrcROIBuf	= matSrcImage(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));
	//cv::Mat matBrROIBuf		= matDstImage[E_DEFECT_COLOR_BRIGHT](cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));
	cv::Mat matDaROIBuf		= matDstImage[E_DEFECT_COLOR_DARK](cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Src.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcROIBuf );
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Src."));

	/************************************************************************
	기본적인 전처리
	************************************************************************/

	// 배경 영역
	cv::Mat matBkROIBuf;
	if (!matBKBuffer.empty())
		matBkROIBuf = matBKBuffer(cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()));

	//////////////////////////////////////////////////////////////////////////
	// 노이즈 제거 1
	cv::Mat matBlurBuf8 = cMatBufTemp.GetMat(matSrcROIBuf.size(), CV_8UC1, false);
	cv::blur(matSrcROIBuf, matBlurBuf8, cv::Size(5, 5));
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("blur 1."));

	//////////////////////////////////////////////////////////////////////////
	// Resize
	cv::Mat matReBuf8 = cMatBufTemp.GetMat(matBlurBuf8.size() / nResize, matBlurBuf8.type(), false);
	cv::resize(matBlurBuf8, matReBuf8, matReBuf8.size(), INTER_AREA);
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Re.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matReBuf8);
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Resize."));

	//////////////////////////////////////////////////////////////////////////
	// 16bit & 증폭
	cv::Mat matGauBuf16 = cMatBufTemp.GetMat(matReBuf8.size(), CV_16UC1, false);
	cv::Mat matPowBuf16 = cMatBufTemp.GetMat(matReBuf8.size(), CV_16UC1, false);
	matReBuf8.convertTo(matGauBuf16, CV_16UC1);

	if (fPow > 10.0)
	{
		// 자동으로 밝기 보정
		float fMean = cv::mean(matGauBuf16)[0];

		float fAutoPow = log(fPow * 16.0) / log(fMean);

		nErrorCode = Pow(matGauBuf16, matPowBuf16, fAutoPow, 4095, &cMatBufTemp);
	}
	else
	{
		// 예외 처리
		if (fPow <= 0)	fPow = 1.0f;

		// 설정 수치로 적용
		nErrorCode = Pow(matGauBuf16, matPowBuf16, fPow, 4095, &cMatBufTemp);
	}
	if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_P.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matPowBuf16);
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("P16."));


	//////////////////////////////////////////////////////////////////////////
	// Gaussian or Blur
	if (false)
	{
		cv::GaussianBlur(matPowBuf16, matGauBuf16, cv::Size(nGauSize, nGauSize), dGauSig);
	}
	else
	{
		// 버퍼 할당 & 초기화
		CMatBuf cMatBufTemp2;
		cMatBufTemp2.SetMem(&cMatBufTemp);

		// Blur
		cv::Mat matTempBuf16 = cMatBufTemp2.GetMat(matPowBuf16.size(), matPowBuf16.type(), false);

		// Avg
		cv::blur(matPowBuf16, matGauBuf16, cv::Size(nGauSize, nGauSize));
		for (int i = 1; i < dGauSig; i++)
		{
			cv::blur(matGauBuf16, matTempBuf16, cv::Size(nGauSize, nGauSize));

			matTempBuf16.copyTo(matGauBuf16);
		}

		matTempBuf16.release();

// 		if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp2.Get_FixMemory());
// 			writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp2.Get_AutoMemory());
// 		}
	}

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Gau.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matGauBuf16);
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("GaussianBlur."));

	////////////////////////////////////////////////////////////////////////// common
	cv::Mat matThBuf = cMatBufTemp.GetMat(matGauBuf16.size(), CV_8UC1, true);
	cv::Mat matMorpBuf = cMatBufTemp.GetMat(matThBuf.size(), matThBuf.type(), true);
	
	/************************************************************************
	어두운 불량 검출 ex) 누기
	************************************************************************/
	if (nNugi_Inspect_Flag > 0) {
		//////////////////////////////////////////////////////////////////////////
		// 16bit 이진화
		//cv::Mat matThBuf = cMatBufTemp.GetMat(matGauBuf16.size(), CV_8UC1, false);
	//////////////////////////////////////////////////////////////////////////
		if (nEMD_Edge_Area >= (matGauBuf16.rows / 2) || nEMD_Edge_Area >= (matGauBuf16.cols / 2)) nEMD_Edge_Area = 0;

		if (nEMD_Edge_Area > 0) {

			//////////////////////////////////////////////////////////////////////////
			cv::Mat matGauBuf16_Edge = cMatBufTemp.GetMat(matReBuf8.size(), CV_16UC1, false);
			//////////////////////////////////////////////////////////////////////////

			cv::Rect rtActiveArea;
			rtActiveArea.x = 0 + nEMD_Edge_Area;
			rtActiveArea.y = 0 + nEMD_Edge_Area;
			rtActiveArea.width = matGauBuf16.cols - (nEMD_Edge_Area * 2);
			rtActiveArea.height = matGauBuf16.rows - (nEMD_Edge_Area * 2);

			matGauBuf16.copyTo(matGauBuf16_Edge);
			matGauBuf16_Edge(rtActiveArea).setTo(4096);

			nErrorCode = Threshold16_INV(matGauBuf16_Edge, matThBuf, nEdge_DarkTh, 255);
			if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;



			nErrorCode = Threshold16_INV(matGauBuf16(rtActiveArea), matThBuf(rtActiveArea), nAct_DarkTh, 255);
			if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

		}
		else {

			nErrorCode = Threshold16_INV(matGauBuf16, matThBuf, nAct_DarkTh, 255);
			if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

		}
	//////////////////////////////////////////////////////////////////////////
	

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Dark_Th.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matThBuf);
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Dark Th."));

	//////////////////////////////////////////////////////////////////////////
	// Dark Defect 크게 ( 누기 불량 주변으로 제거하는 목적 )
	
	cv::morphologyEx(matThBuf, matMorpBuf, MORPH_DILATE, cv::getStructuringElement( MORPH_RECT, cv::Size(nDarkMorp, nDarkMorp) ) );
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_DarkM.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matMorpBuf);
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("DarkM."));

	//////////////////////////////////////////////////////////////////////////
	// Nugi 검출 결과용 ( 원래 영상 크기로 복원 )
	cv::resize(matMorpBuf, matDaROIBuf, matDaROIBuf.size(), INTER_AREA);
	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Nugi.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDaROIBuf);
	}

	//////////////////////////////////////////////////////////////////////////
	// 어두운 라인 불량 제거 ( 누기만 남기기 위함 )
	nErrorCode = DeleteDarkLine(matDaROIBuf, 1.15f, &cMatBufTemp);
	if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Nugi_Del.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matDaROIBuf);
	}
	}else{
		matDaROIBuf.setTo(0);
	}
	/************************************************************************
	RGB 라인 불량 검출
	************************************************************************/
	if (nMuraLine_Inspect_Flag > 0) {
		// E_DEFECT_JUDGEMENT_MURA_LINE_X 검사 시, RGB 라인 검출 알고리즘 동작
		if (EngineerDefectJudgment == NULL)
			return E_ERROR_CODE_TRUE;
		if (!EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_LINE_X].bDefectItemUse)
			return E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////
	// 가로 방향 Max GV 제한 : 명점 등등 밝은 불량 주변으로 검출되는 경우 발생 방지
	nErrorCode = LimitMaxGV16X(matGauBuf16, 1.15f);
	if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;	

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_MaxGV.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matGauBuf16);
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("P16 MaxGV."));


	//////////////////////////////////////////////////////////////////////////
	// 가로 방향으로만 Blur ( RGB 불량의 경우 가로 방향으로만 불량이 존재 )
	cv::Mat matBlurBuf16 = cMatBufTemp.GetMat(matGauBuf16.size(), matGauBuf16.type(), false);
	cv::blur(matGauBuf16, matBlurBuf16, cv::Size(nBlurX, nBlurY));

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Blur.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBlurBuf16);
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Blur."));

	//////////////////////////////////////////////////////////////////////////
	// 배경 만들기 ( 세로 방향으로 Blur 하여 배경 생성 )
	cv::Mat matBKBuf16 = cMatBufTemp.GetMat(matBlurBuf16.size(), matBlurBuf16.type(), false);
	cv::blur(matBlurBuf16, matBKBuf16, cv::Size(1, nBlurY2));

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Bk.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matBKBuf16 );
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("BK."));

	//////////////////////////////////////////////////////////////////////////
	// (배경 - 노이즈 제거) => 이진화  16bit
	// Active & Edge Spec. 따로

	// Active
	cv::Rect rectActive(nEdgeArea, nEdgeArea, matBKBuf16.cols - nEdgeArea - nEdgeArea, matBKBuf16.rows - nEdgeArea - nEdgeArea);

	// 전체 ( Edge )
	nErrorCode = SubThreshold16(matBKBuf16, matBlurBuf16, matThBuf, nEdgeTh, 255);
	if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

	// Active 영역 초기화
	matThBuf(rectActive).setTo(0);
	
	// Active 영역 이진화
	nErrorCode = SubThreshold16(matBKBuf16(rectActive), matBlurBuf16(rectActive), matThBuf(rectActive), nActiveTh, 255);
	if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_SubTh.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matThBuf);
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Sub Th."));

	//////////////////////////////////////////////////////////////////////////
	// 누기 불량 같은 어두운 부분 지우기
	cv::Mat matResBuf = cMatBufTemp.GetMat(matThBuf.size(), matThBuf.type(), false);
	cv::subtract(matThBuf, matMorpBuf, matResBuf);
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_SubD.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matResBuf);
	}
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Sub."));

	//////////////////////////////////////////////////////////////////////////
	// 배경 버퍼 있다면, 배경 영역 지우기 ( 점등 영역 이외 부분 지우기 )
	if (!matBkROIBuf.empty())
	{
		// 영상 크기 맞추기
		cv::resize(matBkROIBuf, matMorpBuf, matSrcROIBuf.size() / nResize, INTER_AREA);

		// 배경 영역 조금 크게 설정
		int nM = nCut;
		cv::morphologyEx(matMorpBuf, matThBuf, MORPH_DILATE, cv::getStructuringElement(MORPH_RECT, cv::Size(nM, nM)));

		// 데이터 복사
		matResBuf.copyTo(matMorpBuf);

		// 배경 영역 삭제
		cv::subtract(matMorpBuf, matThBuf, matResBuf);

		if (bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_SubD2.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
			ImageSave(strTemp, matResBuf);
		}
		writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Sub2."));
	}

	//////////////////////////////////////////////////////////////////////////
	// RGB Line Mura 면적 제한
	// 면적 제한하여, 불량 제거 ( RGB 불량의 경우, 선처럼 이어져 있지 않고 끊어져 있음 )
	nErrorCode = LimitArea(matResBuf, dPara, &cMatBufTemp);
	if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;
	
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Limit Area."));

	//////////////////////////////////////////////////////////////////////////
	// RGB Line Mura 찾기
	if (bImageSave)
	{
		// 엑셀 저장
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Res.csv"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);

		nErrorCode = JudgeRGBLineMuraSave(matResBuf, matBKBuf16, dPara, nCommonPara, rectROI, pResultBlob, strTemp, &cMatBufTemp);

		// 영상 저장
		strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Res.jpg"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matResBuf);
	}
	else
	{
		nErrorCode = JudgeRGBLineMura(matResBuf, matBKBuf16, dPara, nCommonPara, rectROI, pResultBlob, &cMatBufTemp);
	}
	
	if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Limit Area."));
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}


// 8bit & 12bit 영상 저장
long CInspectMura::ImageSave(CString strPath, cv::Mat matSrcBuf)
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

// Sub - Threshold ( 16bit )
long CInspectMura::SubThreshold16(cv::Mat& matSrc1Buf, cv::Mat& matSrc2Buf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV)
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
long CInspectMura::Threshold16(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV)
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
		*itDst = LUT[ *itSrc1 ];
	}

	return E_ERROR_CODE_TRUE;
}


// Threshold ( 16bit )
long CInspectMura::Threshold16_INV(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV)
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
long CInspectMura::Pow(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double dPow, int nMaxGV, CMatBuf* cMemSub)
{
	if( dPow < 1.0 )			return E_ERROR_CODE_MURA_WRONG_PARA;
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

			if (nMaxGV > 4095)	nMaxGV = 4095;

			for (int i = 0; i < 4096; i++)
			{
				double dVal = pow( i, dPow );
				if( dVal < 0 )		dVal = 0;
				if( dVal > nMaxGV)	dVal = nMaxGV;

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


// 작은 면적 제거
long CInspectMura::DeleteArea1(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub)
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

	// 개수 없는 경우 나가기
	if( nTotalLabel <= 0 )	return E_ERROR_CODE_TRUE;

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
long CInspectMura::DeleteArea2(cv::Mat& matSrcImage, int nCount, int nLength, CMatBuf* cMemSub)
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

	// 개수 없는 경우 나가기
	if( nTotalLabel <= 0 )	return E_ERROR_CODE_TRUE;

	// 길이 제한
	int nX = (int)(matSrcImage.cols * 0.7);
	int nY = (int)(matSrcImage.rows * 0.7);

	// 개수만큼 메모리 할당
	cv::RotatedRect *rectRo = NULL;
	rectRo = new cv::RotatedRect[nTotalLabel+1];
	memset( rectRo, 0, sizeof(cv::RotatedRect) * (nTotalLabel+1) );

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
		
		if( nArea <= nCount			||	// 면적 제한
			rectTemp.width	>= nX	||	// 가로 길이 제한 ( 라인 불량... )
			rectTemp.height >= nY	)	// 세로 길이 제한 ( 라인 불량... )
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

		// 면적 만족하는 경우
		{
			// ROI 자르기
			cv::Mat matLabelROI = matLabel( rectTemp );

			// 초기화
			vector<cv::Point>	ptIndexs;
			vector<cv::Point>().swap(ptIndexs);

			for(int  y=0 ; y<rectTemp.height ; y++)
			{
				int *ptrLabel = (int *)matLabelROI.ptr(y);

				for(int x=0 ; x<rectTemp.width ; x++, ptrLabel++)
				{
					// 해당 객체
					if( *ptrLabel == idx )
					{
						// 원본 좌표 넣기
						ptIndexs.push_back( cv::Point(x + rectTemp.x, y + rectTemp.y) );
					}
				}
			}

			// 최소 박스 찾기
			rectRo[idx] = cv::minAreaRect(ptIndexs);

			// 확장
			rectRo[idx].size.width  += (nLength + nLength);
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
	for( int k=1 ; k<=nTotalLabel ; k++ )
	{
		// 값이 없는 경우
		if( rectRo[k].size.width <= 0 )	continue;

		for (int m=1 ; m<=nTotalLabel ; m++)
		{
			// 같은 불량 제외
			if( k == m )	continue;

			// 값이 없는 경우
			if( rectRo[m].size.width <= 0 )	continue;

			// 범위가 곂친 경우 선 그어서 합치기
			if( OrientedBoundingBox(rectRo[k], rectRo[m]) )
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

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

long CInspectMura::DeleteArea3(cv::Mat& matSrcImage, int nCount, int nLength, CMatBuf* cMemSub)
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

		if (nArea <= nCount )	
		{
			// 0GV 로 초기화 후, 넘어가기
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

			continue;
		}

		// 면적 만족하는 경우
		{
			// ROI 자르기
			cv::Mat matLabelROI = matLabel(rectTemp);

			// 초기화
			vector<cv::Point>	ptIndexs;
			vector<cv::Point>().swap(ptIndexs);

			for (int y = 0; y < rectTemp.height; y++)
			{
				int *ptrLabel = (int *)matLabelROI.ptr(y);

				for (int x = 0; x < rectTemp.width; x++, ptrLabel++)
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


// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

// x방향 피팅
long CInspectMura::Estimation_X(cv::Mat matSrcBuf, cv::Mat &matDstBuf, double* dPara)
{
	// 예외 처리
	if( matSrcBuf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matSrcBuf.channels() != 1 )	return E_ERROR_CODE_IMAGE_GRAY;
	if( matDstBuf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;

	int		nDimensionX		= (int)dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_DIM_X	];
	int		nEstiStepX		= (int)dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_STEP_X	];
	double	dEstiBright		= dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_BRIGHT		];
	double	dEstiDark		= dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_DARK			];

	int nStepX = matSrcBuf.cols / nEstiStepX;

	int nStepCols = matSrcBuf.cols / nStepX;
	int nHalfCols = matSrcBuf.cols / 2;

	cv::Mat matM = cv::Mat_<double>(nStepCols, nDimensionX+1);
	cv::Mat matL = cv::Mat_<double>(nStepCols, 1);
	cv::Mat matQ;

	double x, quad, dTemp;
	int i, j, k, m;

	// 밝기 값 제한
	cv::Scalar mean = cv::mean( matSrcBuf );
	int nMinGV = (int)(mean[0] * dEstiDark);
	int nMaxGV = (int)(mean[0] * dEstiBright);

	// 원본 8U인 경우
	if( matSrcBuf.type() == CV_8U )
	{
		for (i=0 ; i<matSrcBuf.rows ; i++)
		{
			for (j=0 ; j<nStepCols ; j++)
			{
				x = (j*nStepX - nHalfCols) / double(matSrcBuf.cols);

				matM.at<double>(j, 0) = 1.0;
				dTemp = 1.0;
				for(k=1 ; k<=nDimensionX ; k++)
				{
					dTemp *= x;
					matM.at<double>(j, k) = dTemp;
				}

				//I.at<double>(j, 0) = matSrcBuf.at<uchar>(i, j*nStepX);
				m = matSrcBuf.at<uchar>(i, j*nStepX);

				if( m < nMinGV )	m = nMinGV;
				if( m > nMaxGV )	m = nMaxGV;

				matL.at<double>(j, 0) = m;
			}

			cv::SVD svd(matM);
			svd.backSubst(matL, matQ);

			for (j=0 ; j<matDstBuf.cols ; j++)
			{
				x = (j - nHalfCols) / double(matSrcBuf.cols);

				quad = matQ.at<double>(0, 0);
				dTemp = 1.0;
				for(k=1 ; k<=nDimensionX ; k++)
				{
					dTemp *= x;
					quad += (matQ.at<double>(k, 0) * dTemp);
				}

				matDstBuf.at<uchar>(i, j) = saturate_cast<uchar>(quad);
			}
		}
	}
	// 원본 16U인 경우
	else
	{
		for (i=0 ; i<matSrcBuf.rows ; i++)
		{
			for (j=0 ; j<nStepCols ; j++)
			{
				x = (j*nStepX - nHalfCols) / double(matSrcBuf.cols);

				matM.at<double>(j, 0) = 1.0;
				dTemp = 1.0;
				for(k=1 ; k<=nDimensionX ; k++)
				{
					dTemp *= x;
					matM.at<double>(j, k) = dTemp;
				}

				//I.at<double>(j, 0) = matSrcBuf.at<ushort>(i, j*nStepX);
				m = matSrcBuf.at<ushort>(i, j*nStepX);

				if( m < nMinGV )	m = nMinGV;
				if( m > nMaxGV )	m = nMaxGV;

				matL.at<double>(j, 0) = m;
			}

			cv::SVD svd(matM);
			svd.backSubst(matL, matQ);

			for (j=0 ; j<matDstBuf.cols ; j++)
			{
				x = (j - nHalfCols) / double(matSrcBuf.cols);

				quad = matQ.at<double>(0, 0);
				dTemp = 1.0;
				for(k=1 ; k<=nDimensionX ; k++)
				{
					dTemp *= x;
					quad += (matQ.at<double>(k, 0) * dTemp);
				}

				matDstBuf.at<ushort>(i, j) = saturate_cast<ushort>(quad);
			}
		}
	}

	matM.release();
	matL.release();
	matQ.release();

	return E_ERROR_CODE_TRUE;
}


// y방향 피팅
long CInspectMura::Estimation_Y(cv::Mat matSrcBuf, cv::Mat &matDstBuf, double* dPara)
{
	// 예외 처리
	if( matSrcBuf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matSrcBuf.channels() != 1 )	return E_ERROR_CODE_IMAGE_GRAY;
	if( matDstBuf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;

	int		nDimensionY		= (int)dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_DIM_Y	];
	int		nEstiStepY		= (int)dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_STEP_Y	];	
	double	dEstiBright		= dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_BRIGHT		];
	double	dEstiDark		= dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_DARK			];

	int nStepY = matSrcBuf.rows / nEstiStepY;

	int nStepRows = matSrcBuf.rows / nStepY;
	int nHalfRows = matSrcBuf.rows / 2;

	cv::Mat matM = cv::Mat_<double>(nStepRows, nDimensionY+1);
	cv::Mat matL = cv::Mat_<double>(nStepRows, 1);
	cv::Mat matQ;

	double y, quad, dTemp;
	int i, j, k, m;

	// 밝기 값 제한
	cv::Scalar mean = cv::mean( matSrcBuf );
	int nMinGV = (int)(mean[0] * dEstiDark);
	int nMaxGV = (int)(mean[0] * dEstiBright);

	// 원본 8U인 경우
	if( matSrcBuf.type() == CV_8U )
	{
		for (j=0 ; j<matSrcBuf.cols ; j++)
		{
			for (i=0 ; i<nStepRows ; i++)
			{
				y = (i*nStepY - nHalfRows) / double(matSrcBuf.rows);

				matM.at<double>(i, 0) = 1.0;
				dTemp = 1.0;
				for(k=1 ; k<=nDimensionY ; k++)
				{
					dTemp *= y;
					matM.at<double>(i, k) = dTemp;
				}

				//I.at<double>(i, 0) = matSrcBuf.at<uchar>(i*nStepY, j);
				m = matSrcBuf.at<uchar>(i*nStepY, j);

				if( m < nMinGV )	m = nMinGV;
				if( m > nMaxGV )	m = nMaxGV;

				matL.at<double>(i, 0) = m;
			}

			cv::SVD svd(matM);
			svd.backSubst(matL, matQ);

			for (i=0 ; i<matSrcBuf.rows ; i++)
			{
				y = (i - nHalfRows) / double(matSrcBuf.rows);

				quad = matQ.at<double>(0, 0);
				dTemp = 1.0;
				for(k=1 ; k<=nDimensionY ; k++)
				{
					dTemp *= y;
					quad += (matQ.at<double>(k, 0) * dTemp);
				}

				matDstBuf.at<uchar>(i, j) = saturate_cast<uchar>(quad);
			}
		}
	}
	// 원본 16U인 경우
	else
	{
		for (j=0 ; j<matSrcBuf.cols ; j++)
		{
			for (i=0 ; i<nStepRows ; i++)
			{
				y = (i*nStepY - nHalfRows) / double(matSrcBuf.rows);

				matM.at<double>(i, 0) = 1.0;
				dTemp = 1.0;
				for(k=1 ; k<=nDimensionY ; k++)
				{
					dTemp *= y;
					matM.at<double>(i, k) = dTemp;
				}

				//I.at<double>(i, 0) = matSrcBuf.at<ushort>(i*nStepY, j);
				m = matSrcBuf.at<ushort>(i*nStepY, j);

				if( m < nMinGV )	m = nMinGV;
				if( m > nMaxGV )	m = nMaxGV;

				matL.at<double>(i, 0) = m;
			}

			cv::SVD svd(matM);
			svd.backSubst(matL, matQ);

			for (i=0 ; i<matSrcBuf.rows ; i++)
			{
				y = (i - nHalfRows) / double(matSrcBuf.rows);

				quad = matQ.at<double>(0, 0);
				dTemp = 1.0;
				for(k=1 ; k<=nDimensionY ; k++)
				{
					dTemp *= y;
					quad += (matQ.at<double>(k, 0) * dTemp);
				}

				matDstBuf.at<ushort>(i, j) = saturate_cast<ushort>(quad);
			}
		}
	}

	matM.release();
	matL.release();
	matQ.release();

	return E_ERROR_CODE_TRUE;
}


// y방향 피팅 & 평균
long CInspectMura::Estimation_Y_N_Average(cv::Mat matSrc1Buf, cv::Mat matSrc2Buf, cv::Mat &matDstBuf, double* dPara)
{
	if( matSrc1Buf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matSrc2Buf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstBuf.empty() )				return E_ERROR_CODE_EMPTY_BUFFER;

	if( matSrc1Buf.channels() != 1 )	return E_ERROR_CODE_IMAGE_GRAY;
	if( matSrc2Buf.channels() != 1 )	return E_ERROR_CODE_IMAGE_GRAY;

	if(	matSrc1Buf.rows != matSrc2Buf.rows	||		// 가로 크기 같아야 함
		matSrc1Buf.cols != matSrc2Buf.cols	)		// 세로 크기 같아야 함
		return E_ERROR_CODE_IMAGE_SIZE;

	int		nDimensionY		= (int)dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_DIM_Y	];
	int		nEstiStepY		= (int)dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_STEP_Y	];	
	double	dEstiBright		= dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_BRIGHT		];
	double	dEstiDark		= dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_DARK			];

	int nStepY = matSrc1Buf.rows / nEstiStepY;

	int nStepRows = matSrc1Buf.rows / nStepY;
	int nHalfRows = matSrc1Buf.rows / 2;

	cv::Mat matM = cv::Mat_<double>(nStepRows, nDimensionY+1);
	cv::Mat matL = cv::Mat_<double>(nStepRows, 1);
	cv::Mat matQ;

	double y, quad, dTemp;
	int i, j, k, m;

	// 밝기 값 제한
	cv::Scalar mean = cv::mean( matSrc1Buf );
	int nMinGV = (int)(mean[0] * dEstiDark);
	int nMaxGV = (int)(mean[0] * dEstiBright);

	for (j=0 ; j<matSrc1Buf.cols ; j++)
	{
		for (i=0 ; i<nStepRows ; i++)
		{
			y = (i*nStepY - nHalfRows) / double(matSrc1Buf.rows);

			matM.at<double>(i, 0) = 1.0;
			dTemp = 1.0;
			for(k=1 ; k<=nDimensionY ; k++)
			{
				dTemp *= y;
				matM.at<double>(i, k) = dTemp;
			}

			//I.at<double>(i, 0) = matSrc1Buf.at<uchar>(i*nStepY, j);
			m = matSrc1Buf.at<uchar>(i*nStepY, j);

			if( m < nMinGV )	m = nMinGV;
			if( m > nMaxGV )	m = nMaxGV;

			matL.at<double>(i, 0) = m;
		}

		cv::SVD svd(matM);
		svd.backSubst(matL, matQ);

		for (i=0 ; i<matSrc1Buf.rows ; i++)
		{
			y = (i - nHalfRows) / double(matSrc1Buf.rows);

			quad = matQ.at<double>(0, 0);
			dTemp = 1.0;
			for(k=1 ; k<=nDimensionY ; k++)
			{
				dTemp *= y;
				quad += (matQ.at<double>(k, 0) * dTemp);
			}
			
			// 평균값
			int nVal = (quad + matSrc2Buf.at<uchar>(i, j)) / 2;

			//matDstBuf.at<uchar>(i, j) = saturate_cast<uchar>(quad);
			matDstBuf.at<uchar>(i, j) = saturate_cast<uchar>(nVal);
		}
	}

	matM.release();
	matL.release();
	matQ.release();

	return E_ERROR_CODE_TRUE;
}


// xy방향 피팅
long CInspectMura::Estimation_XY(cv::Mat matSrcBuf, cv::Mat &matDstBuf, double* dPara, CMatBuf* cMemSub)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 예외 처리
	if( matSrcBuf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstBuf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matSrcBuf.channels() != 1 )	return E_ERROR_CODE_IMAGE_GRAY;

	int		nEstiDimX		= (int)dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_DIM_X	];
	int		nEstiDimY		= (int)dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_DIM_Y	];
	int		nEstiStepX		= (int)dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_STEP_X	];
	int		nEstiStepY		= (int)dPara[E_PARA_AVI_MURA_COMMON_ESTIMATION_STEP_Y	];

	// 예외 처리
	if( nEstiDimX <= 0 )	return E_ERROR_CODE_MURA_WRONG_PARA;
	if( nEstiDimY <= 0 )	return E_ERROR_CODE_MURA_WRONG_PARA;
	if( nEstiStepX <= 0 )	return E_ERROR_CODE_MURA_WRONG_PARA;
	if( nEstiStepY <= 0 )	return E_ERROR_CODE_MURA_WRONG_PARA;

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	cv::Mat matDstBufX = cMatBufTemp.GetMat(matSrcBuf.size(), matSrcBuf.type(), false);
	cv::Mat matDstBufY = cMatBufTemp.GetMat(matSrcBuf.size(), matSrcBuf.type(), false);

#ifdef _DEBUG
#else
#pragma omp parallel for num_threads(2)
#endif

	for (int i = 0; i < 2; i++)
	{
		switch (i)
		{
		case 0:
			nErrorCode |= Estimation_X(matSrcBuf, matDstBufX, dPara);
			break;
		case 1:
			nErrorCode |= Estimation_Y(matSrcBuf, matDstBufY, dPara);
			break;
		}
	}
	if (nErrorCode != E_ERROR_CODE_TRUE)	return nErrorCode;

	nErrorCode = TwoImg_Average(matDstBufX, matDstBufY, matDstBuf);
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	matDstBufX.release();
	matDstBufY.release();

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}


// 두개 영상 평균
long CInspectMura::TwoImg_Average(cv::Mat matSrc1Buf, cv::Mat matSrc2Buf, cv::Mat &matDstBuf)
{	
	if( matSrc1Buf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matSrc2Buf.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matDstBuf.empty() )				return E_ERROR_CODE_EMPTY_BUFFER;

	if( matSrc1Buf.channels() != 1 )	return E_ERROR_CODE_IMAGE_GRAY;
	if( matSrc2Buf.channels() != 1 )	return E_ERROR_CODE_IMAGE_GRAY;

	if(	matSrc1Buf.rows != matSrc2Buf.rows	||		// 가로 크기 같아야 함
		matSrc1Buf.cols != matSrc2Buf.cols	)		// 세로 크기 같아야 함
		return E_ERROR_CODE_IMAGE_SIZE;

	// 원본 8U인 경우
	if( matSrc1Buf.type() == CV_8U )
	{
		cv::MatIterator_<uchar> itSrc1, itSrc2, itDst, endDst;
		itSrc1	= matSrc1Buf.begin<uchar>();
		itSrc2	= matSrc2Buf.begin<uchar>();
		itDst	= matDstBuf.begin<uchar>();
		endDst	= matDstBuf.end<uchar>();

		for ( ; itDst != endDst; itSrc1++, itSrc2++, itDst++)
			*itDst = (uchar)((*itSrc1 + *itSrc2) / 2);
	}
	// 원본 16U인 경우
	else
	{
		cv::MatIterator_<ushort> itSrc1, itSrc2, itDst, endDst;
		itSrc1	= matSrc1Buf.begin<ushort>();
		itSrc2	= matSrc2Buf.begin<ushort>();
		itDst	= matDstBuf.begin<ushort>();
		endDst	= matDstBuf.end<ushort>();

		for ( ; itDst != endDst; itSrc1++, itSrc2++, itDst++)
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
long CInspectMura::DeleteCompareDust(cv::Mat& matSrcBuffer, int nOffset, stDefectInfo* pResultBlob, int nStartIndex, int nModePS)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 파라미터 NULL인 경우
	if( matSrcBuffer.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;
	if( pResultBlob == NULL )		return E_ERROR_CODE_EMPTY_PARA;

	// 검출된 불량 개수
	int nDefectCount = pResultBlob->nDefectCount;

	// 불량 개수 만큼...
	for (int i=nStartIndex ; i<nDefectCount ; i++)
	{
		// Mura 불량이 아닌경우 제외
		if( pResultBlob->nDefectJudge[i] <= E_DEFECT_JUDGEMENT_MURA_START	&&
			pResultBlob->nDefectJudge[i] > E_DEFECT_JUDGEMENT_MURA_END		)
		{
			//i++;
			continue;
		}

		// KSW 18.06.13 - 누기, 무정형 무라, 등등... 제외...
		if( pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_MURA_LINE_X			||
			pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_MURA_NUGI			||
			pResultBlob->nDefectJudge[i] == E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH		)
		{
			//i++;
			continue;
		}

		// KSW 18.06.13 - 면적 큰거는 제외...?
		if( pResultBlob->nArea[i] > 1000 )
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
		rect.x		*= nModePS;
		rect.y		*= nModePS;
		rect.width	*= nModePS;
		rect.height	*= nModePS;

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
		// 불량 주변에서 Dust 존재하는지 확인
		//////////////////////////////////////////////////////////////////////////

		// 불량 주변 -> Dust 영상 Max GV 찾기
		double valMax;
		cv::minMaxLoc(matTempBuf, NULL, &valMax);

		// Dust 확인 시, 큰 불량이 없는 경우 ( 큰 불량 존재 시, 255 / 4096 값이 존재 )
		if( valMax == 0 )
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
long CInspectMura::DeleteDarkLine(cv::Mat& matSrcBuffer, float fMajorAxisRatio, CMatBuf* cMemSub)
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

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}

// 가로 방향 Max GV 제한 : 명점 등등 밝은 불량 주변으로 검출되는 경우 발생 방지
long CInspectMura::LimitMaxGV16X(cv::Mat& matSrcBuffer, float fOffset)
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


// RGB Line Mura 면적 제한
// 면적 제한하여, 불량 제거 ( RGB 불량의 경우, 선처럼 이어져 있지 않고 끊어져 있음 )
long CInspectMura::LimitArea(cv::Mat& matSrcBuffer, double* dPara, CMatBuf* cMemSub)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 예외 처리
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// 파라미터
	int		nCount			= (int)dPara[E_PARA_AVI_MURA_RGB_AREA				];
	int		nCut			= (int)dPara[E_PARA_AVI_MURA_RGB_INSIDE				];

	// 외곽 부분 제거
	cv::Mat matROIBuf = matSrcBuffer(cv::Rect(nCut, nCut, matSrcBuffer.cols - nCut - nCut, matSrcBuffer.rows - nCut - nCut));

	// 메모리 할당
	cv::Mat matLabel, matStats, matCentroid;
	matLabel = cMatBufTemp.GetMat(matROIBuf.size(), CV_32SC1, false);

	// Blob 개수
	__int64 nTotalLabel = cv::connectedComponentsWithStats(matROIBuf, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

	// 장축 / 단축 비율 ( 라인 제거시 사용 )
	float	fLineRatio = 30;

	// 라인 제거 시, 주변으로 확장하여 제거
	int		nLineOffset = 7;

	for (int idx = 1; idx <= nTotalLabel; idx++)
	{
		// 객체 면적
		long nArea = matStats.at<int>(idx, CC_STAT_AREA);

		// 면적 너무 작은것 제외
		// 너무 큰것 제외 ( RGB 불량의 경우, 선처럼 이어져 있지 않고 끊어져 있음 )
		if (30 < nArea && nArea <= nCount)	continue;

		// Blob 영역 Rect
		cv::Rect rectTemp;
		rectTemp.x = matStats.at<int>(idx, CC_STAT_LEFT);
		rectTemp.y = matStats.at<int>(idx, CC_STAT_TOP);
		rectTemp.width = matStats.at<int>(idx, CC_STAT_WIDTH);
		rectTemp.height = matStats.at<int>(idx, CC_STAT_HEIGHT);

		// 가로 라인처럼 긴 경우 그 줄 삭제
		if (rectTemp.width / rectTemp.height > fLineRatio)
		{
			// 가로 방향
			cv::rectangle(matROIBuf, cv::Rect(0, rectTemp.y - nLineOffset, matROIBuf.cols, rectTemp.height + nLineOffset + nLineOffset), cv::Scalar(0), -1);
		}
		// 세로 라인처럼 긴 경우 그 줄 삭제
		else if (rectTemp.height / rectTemp.width > fLineRatio)
		{
			// 세로 방향
			cv::rectangle(matROIBuf, cv::Rect(rectTemp.x - nLineOffset, 0, rectTemp.width + nLineOffset + nLineOffset, matROIBuf.rows), cv::Scalar(0), -1);
		}
		// 그냥 면적이 작거나, 큰경우라면...
		else
		{
			// 0GV 로 초기화
			cv::Mat matTempROI = matROIBuf(rectTemp);
			cv::Mat matLabelROI = matLabel(rectTemp);

			for (int y = 0; y < rectTemp.height; y++)
			{
				int		*ptrLabel = (int *)matLabelROI.ptr(y);
				uchar	*ptrGray = (uchar *)matTempROI.ptr(y);

				for (int x = 0; x < rectTemp.width; x++, ptrLabel++, ptrGray++)
				{
					// 해당 객체 0GV 로 제거
					if (*ptrLabel == idx)	*ptrGray = 0;
				}
			}
		}
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}

// RGB Line Mura 찾기
long CInspectMura::JudgeRGBLineMura(cv::Mat& matSrcBuffer, cv::Mat& matBKBuf16, double* dPara, int* nCommonPara, CRect rectROI, stDefectInfo* pResultBlob, CMatBuf* cMemSub)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 예외 처리
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// 파라미터
	int		nCut			= (int)dPara[E_PARA_AVI_MURA_RGB_INSIDE				];
	float	fSegX			= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_SEG_X		];
	float	fSegY			= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_SEG_Y		];
	float	fArea1Ratio		= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_1_RATIO		];
	float	fArea2Cnt		= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_2_COUNT		];
	float	fArea2Ratio		= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_2_RATIO		];
	float	fAreaMinGV		= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_MIN_GV		];

	// 예외처리
	if (fSegX <= 0)	fSegX = 1;
	if (fSegY <= 0)	fSegY = 1;

	// 외곽 부분 제거
	cv::Mat matROIBuf = matSrcBuffer(cv::Rect(nCut, nCut, matSrcBuffer.cols - nCut - nCut, matSrcBuffer.rows - nCut - nCut));
	cv::Mat matROIBkBuf = matBKBuf16(cv::Rect(nCut, nCut, matSrcBuffer.cols - nCut - nCut, matSrcBuffer.rows - nCut - nCut));

	// 총 평균
	float dMean = cv::mean(matROIBkBuf)[0];
	float dMinGV = dMean;

	// 한 영역의 pixel 개수
	float nRangeX = matROIBuf.cols / fSegX;
	float nRangeY = matROIBuf.rows / fSegY;

	cv::Rect rectRange;
	float fTotalMax = 0;
	int nCnt = 0;

	__int64 nSumCount = 0;

	// 데이터 초기화 ( Defect Map 외곽선 용도 )
	cv::Mat matContoursBuf = cMatBufTemp.GetMat(matSrcBuffer.size(), CV_8UC1);

	int nSegX = fSegX;
	int nSegY = fSegY;

	for (int nY = 0; nY < fSegY; nY++)
	{
		rectRange.y = (int)(nRangeY * nY);
		rectRange.height = (int)(nRangeY * (nY + 1)) - rectRange.y;

		for (int nX = 0; nX < fSegX; nX++)
		{
			rectRange.x = (int)(nRangeX * nX);
			rectRange.width = (int)(nRangeX * (nX + 1)) - rectRange.x;

			// 평균
			float dTemp = cv::mean(matROIBkBuf(rectRange))[0];

			// 영역의 불량 면적
			nSumCount = (int)(cv::sum(matROIBuf(rectRange))[0] / 255);

			// 점등 면적
			float fTotal = nSumCount / (float)(rectRange.width * rectRange.height) * 100.f;

			// 최대값 확인
			if (fTotalMax < fTotal)
				fTotalMax = fTotal;

			// 약하고 넓게 퍼진 경우...
			// 영역 개수 파악
			if (fTotal > fArea2Ratio)
			{
				// 불량 영역 그리기 ( Defect Map 외곽선 용도 )
				cv::rectangle(matContoursBuf, cv::Rect(rectRange.x + nCut, rectRange.y + nCut, rectRange.width, rectRange.height), cv::Scalar(2558), -1);

				nCnt++;

				//////////////////////////////////////////////////////////////////////////

				// 모서리 제외
				if ((nX == 0 && nY == 0) ||
					(nX == 0 && nY == nSegY - 1) ||
					(nX == nSegX - 1 && nY == 0) ||
					(nX == nSegX - 1 && nY == nSegY - 1))
				{
					// 모서리인 경우...
				}
				else
				{
					// 최소 GV 찾기
					if (dMinGV > dTemp)
						dMinGV = dTemp;
				}
			}
		}
	}

	// RGB Line Mura 리스트 추가
	if (fTotalMax > fArea1Ratio	||	// 강한 RGB 불량인 경우
		nCnt > fArea2Cnt		)	// 여러개 존재하는 경우
	{
		// 어두운 영역이 하나라도 존재 하는 경우
		if (dMinGV < fAreaMinGV)
			AddRGBLineMuraDefect(matContoursBuf, dPara, nCommonPara, rectROI, pResultBlob);
	}

	// Log
	wchar_t wcLogTemp[MAX_PATH] = { 0 };
	swprintf_s(wcLogTemp, _T("RGB Value : %.5f, Count :%d, MinGV : %.3f,"), fTotalMax, nCnt, dMinGV);
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, wcLogTemp);

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}


// RGB Line Mura 찾기 ( 영상 저장 용 )
long CInspectMura::JudgeRGBLineMuraSave(cv::Mat& matSrcBuffer, cv::Mat& matBKBuf16, double* dPara, int* nCommonPara, CRect rectROI, stDefectInfo* pResultBlob, CString strAlgPath, CMatBuf* cMemSub)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 예외 처리
	if (matSrcBuffer.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	// 파라미터
	int		nCut			= (int)dPara[E_PARA_AVI_MURA_RGB_INSIDE				];
	float	fSegX			= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_SEG_X		];
	float	fSegY			= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_SEG_Y		];
	float	fArea1Ratio		= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_1_RATIO		];
	float	fArea2Cnt		= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_2_COUNT		];
	float	fArea2Ratio		= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_2_RATIO		];
	float	fAreaMinGV		= (float)dPara[E_PARA_AVI_MURA_RGB_AREA_MIN_GV		];

	// 예외처리
	if (fSegX <= 0)	fSegX = 1;
	if (fSegY <= 0)	fSegY = 1;

	// 외곽 부분 제거
	cv::Mat matROIBuf = matSrcBuffer(cv::Rect(nCut, nCut, matSrcBuffer.cols - nCut - nCut, matSrcBuffer.rows - nCut - nCut));
	cv::Mat matROIBkBuf = matBKBuf16(cv::Rect(nCut, nCut, matSrcBuffer.cols - nCut - nCut, matSrcBuffer.rows - nCut - nCut));

	// 총 평균
	float dMean = cv::mean(matROIBkBuf)[0];
	float dMinGV = dMean;

	// 한 영역의 pixel 개수
	float nRangeX = matROIBuf.cols / fSegX;
	float nRangeY = matROIBuf.rows / fSegY;

	cv::Rect rectRange;
	float fTotalMax = 0;
	int nCnt = 0;

	__int64 nSumCount = 0;

	// 데이터 초기화 ( Defect Map 외곽선 용도 )
	cv::Mat matContoursBuf = cMatBufTemp.GetMat(matSrcBuffer.size(), CV_8UC1);

	// 텍스트 좌표
	cv::Point ptTxT;

	int nSegX = fSegX;
	int nSegY = fSegY;
	
	// 파일 쓰기
	char szFileName[MAX_PATH] = { 0, };
	WideCharToMultiByte(CP_ACP, 0, strAlgPath, -1, szFileName, sizeof(szFileName), NULL, NULL);

	FILE *out = NULL;
	fopen_s(&out, szFileName, "wt");

	if (out)
	{
		fprintf_s(out, "nX,nY,Area Ratio,Min GV\n");
	}

	for (int nY = 0; nY < nSegY; nY++)
	{
		rectRange.y = (int)(nRangeY * nY);
		rectRange.height = (int)(nRangeY * (nY + 1)) - rectRange.y;

		// 텍스트 좌표
		ptTxT.y = rectRange.y + 25 + nCut;

		for (int nX = 0; nX < nSegX; nX++)
		{
			rectRange.x = (int)(nRangeX * nX);
			rectRange.width = (int)(nRangeX * (nX + 1)) - rectRange.x;

			// 평균
			float dTemp = cv::mean(matROIBkBuf(rectRange))[0];

			// 텍스트 좌표
			ptTxT.x = rectRange.x + 5 + nCut;

			// 영역의 불량 면적
			nSumCount = (int)(cv::sum(matROIBuf(rectRange))[0] / 255);

			// 점등 면적
			float fTotal = nSumCount / (float)(rectRange.width * rectRange.height) * 100.f;

			// 영역 표시
			cv::rectangle(matROIBuf(rectRange), cv::Rect(0, 0, rectRange.width, rectRange.height), cv::Scalar(128, 128, 128));

			// 약하고 넓게 퍼진 경우...
			// 영역 개수 파악
			if (fTotal > fArea2Ratio)
			{
				cv::Mat matGray1 = cv::Mat(rectRange.size(), matROIBuf.type(), 128);
				cv::Mat matGray2 = cv::Mat(rectRange.size(), matROIBuf.type(), 128);
				matROIBuf(rectRange).copyTo(matGray1);

				cv::addWeighted(matGray1, 0.5, matGray2, 0.5, 1.0, matROIBuf(rectRange));

				// 불량 영역 그리기
				cv::rectangle(matContoursBuf, cv::Rect(rectRange.x + nCut, rectRange.y + nCut, rectRange.width, rectRange.height), cv::Scalar(2558), -1);

				nCnt++;

				//////////////////////////////////////////////////////////////////////////

				// 모서리 제외
				if ((nX == 0 && nY == 0) ||
					(nX == 0 && nY == nSegY - 1) ||
					(nX == nSegX - 1 && nY == 0) ||
					(nX == nSegX - 1 && nY == nSegY - 1))
				{
					// 모서리인 경우...
				}
				else
				{
					// 최소 GV 찾기
					if (dMinGV > dTemp)
						dMinGV = dTemp;

					// 설정 GV 보다 아래인 영역
					if (fAreaMinGV > dTemp)
					{
						int nTemp = 10;

						// 영역 표시
						cv::rectangle(matROIBuf(rectRange), cv::Rect(nTemp, nTemp, rectRange.width - nTemp - nTemp, rectRange.height - nTemp - nTemp), cv::Scalar(255));
					}
				}
			}

			// 텍스트 표시
			CString strFont;
			strFont.Format(_T("(%02d, %02d)"), nX+1, nY+1);
			cv::putText(matSrcBuffer, (cv::String)(CStringA)strFont, ptTxT, CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2, LINE_AA);

			ptTxT.y += 40;
			strFont.Format(_T("Ratio : %.3f %%"), fTotal);
			cv::putText(matSrcBuffer, (cv::String)(CStringA)strFont, ptTxT, CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2, LINE_AA);
			ptTxT.y -= 40;

			ptTxT.y += 80;
			strFont.Format(_T("Average : %.3f"), dTemp);
			cv::putText(matSrcBuffer, (cv::String)(CStringA)strFont, ptTxT, CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2, LINE_AA);
			ptTxT.y -= 80;

			// 파일 쓰기
			if( out )
			{
				fprintf_s(out, "%d,%d,%.3f,%.3f\n", nX+1, nY+1, fTotal, dTemp);
			}

			// 최대값 확인
			if (fTotalMax < fTotal)
				fTotalMax = fTotal;
		}
	}

	// 파일 쓰기
	if (out)
	{
		fclose(out);
	}

	// 총 평균
	CString strFont2;
	strFont2.Format(_T("Total Max Ratio : %.3f %%"), fTotalMax);
	cv::putText(matSrcBuffer, (cv::String)(CStringA)strFont2, cv::Point(nCut + 5, nCut + 145), CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2, LINE_AA);
	strFont2.Format(_T("Total MinGV : %.3f"), dMinGV);
	cv::putText(matSrcBuffer, (cv::String)(CStringA)strFont2, cv::Point(nCut + 5, nCut + 185), CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255), 2, LINE_AA);

	// RGB Line Mura 리스트 추가
	if (fTotalMax > fArea1Ratio	||	// 강한 RGB 불량인 경우
		nCnt > fArea2Cnt		)	// 여러개 존재하는 경우
	{
		// 어두운 영역이 하나라도 존재 하는 경우
		if(dMinGV < fAreaMinGV)
			AddRGBLineMuraDefect(matContoursBuf, dPara, nCommonPara, rectROI, pResultBlob);
	}

	// Log
	wchar_t wcLogTemp[MAX_PATH] = { 0 };
	swprintf_s(wcLogTemp, _T("RGB Value : %.5f, Count :%d, MinGV : %.3f,"), fTotalMax, nCnt, dMinGV);
	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, wcLogTemp);

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode;
}

// RGB Line Mura 리스트 추가
long CInspectMura::AddRGBLineMuraDefect(cv::Mat& matContoursBuf, double* dPara, int* nCommonPara, CRect rectROI, stDefectInfo* pResultBlob)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	// 예외 처리
	if (matContoursBuf.empty())			return E_ERROR_CODE_EMPTY_BUFFER;

	// 사용 파라미터
	int		nResize			= (int)dPara[E_PARA_AVI_MURA_RGB_RESIZE				];
	if( nResize <= 0 )	return E_ERROR_CODE_MURA_WRONG_PARA;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nPS					=  nCommonPara[E_PARA_COMMON_PS_MODE			];

	// KSW 18.09.25 - RGB Line Mura 추가시 필요
	int		nPatUI				= nCommonPara[E_PARA_COMMON_UI_IMAGE_NUMBER		];

	if (pResultBlob != NULL)
	{
		pResultBlob->nArea[0] = 0;
		pResultBlob->nMaxGV[0] = 255;
		pResultBlob->nMinGV[0] = 0;
		pResultBlob->dMeanGV[0] = 0;

		pResultBlob->ptLT[0].x = 0;
		pResultBlob->ptLT[0].y = 0;
		pResultBlob->ptRT[0].x = 0;
		pResultBlob->ptRT[0].y = 0;
		pResultBlob->ptRB[0].x = 0;
		pResultBlob->ptRB[0].y = 0;
		pResultBlob->ptLB[0].x = 0;
		pResultBlob->ptLB[0].y = 0;

		pResultBlob->dBackGroundGV[0] = 0;
		pResultBlob->dCompactness[0] = 0;
		pResultBlob->dSigma[0] = 0;
		pResultBlob->dBreadth[0] = 0;
		pResultBlob->dF_Min[0] = 0;
		pResultBlob->dF_Max[0] = 0;
		pResultBlob->dF_Elongation[0] = 0;
		pResultBlob->dCompactness[0] = 0;

		// 밝기
		pResultBlob->nDefectColor[0] = E_DEFECT_COLOR_DARK;

		pResultBlob->nDefectJudge[0] = E_DEFECT_JUDGEMENT_MURA_LINE_X;
		pResultBlob->nPatternClassify[0] = nPatUI;

		// 카운트 증가
		pResultBlob->nDefectCount = 1;

		// Contours 구하기
		vector <cv::Point>	ptContoursArr;
		vector <cv::Point>().swap(ptContoursArr);

		vector<vector<cv::Point>>	ptContours;
		vector<vector<cv::Point>>().swap(ptContours);
		cv::findContours(matContoursBuf, ptContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		// Contours 개수
		for (int m = 0; m < ptContours.size(); m++)
		{
			// Resize 원본 좌표로 변경
			for (int k = 0; k < ptContours.at(m).size(); k++)
				ptContoursArr.push_back(cv::Point(ptContours.at(m)[k].x * nResize + rectROI.left, ptContours.at(m)[k].y * nResize + rectROI.top));
		}

		// 설정 개수보다 많은 경우
		int nContoursCount = ptContoursArr.size();
		float fRatio = 1.0;
		if (nContoursCount >= MAX_CONTOURS)
		{
			fRatio = nContoursCount / (float)MAX_CONTOURS;
			nContoursCount = MAX_CONTOURS;
		}

		// 외곽선 저장
		for (int j = 0; j < nContoursCount; j++)
		{
			// 실제 사용할 외곽선 Index
			int i = (int)(j * fRatio);

			// P/S 모드에 따라 좌표 수정 ( 1샷 좌표로 변경 )
			pResultBlob->nContoursX[0][j] = (int)(ptContoursArr[i].x / nPS);
			pResultBlob->nContoursY[0][j] = (int)(ptContoursArr[i].y / nPS);
		}

		// 초기화
		vector <cv::Point>().swap(ptContoursArr);
		vector<vector<cv::Point>>().swap(ptContours);
	}

	return nErrorCode;
}


// White Spot 재분류
long CInspectMura::JudgeWhiteSpot(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
	stDefectInfo* pResultBlob, CMatBuf* cMemSub)
{
	// 불량이 없는 경우 나가기
	int nCount = pResultBlob->nDefectCount;
	if( nCount<= 0)		return E_ERROR_CODE_TRUE;

	// 사용 파라미터
	int		nMorpObj			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_MORP_OBJ							];
	int		nMorpBKG			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_MORP_BKG							];
	int		nThreshold			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_THRESHOLD							];

	// Active Spec
	double	dSpecActiveBrightRatio	= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_BRIGHT_RATIO		];

	double	dSpecActiveDarkRatio1		= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_1	];
	double	dSpecActiveDarkArea1		= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA_1		];
	double	dSpecActiveDarkDiff1		= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_1		];

	double	dSpecActiveDarkRatio2		= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_2	];
	double	dSpecActiveDarkArea2		= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA_2		];
	double	dSpecActiveDarkDiff2		= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_2		];

	double	dSpecActiveDarkRatio3		= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_3	];
	double	dSpecActiveDarkArea3		= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA_3		];
	double	dSpecActiveDarkDiff3		= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_3		];

	// Edge 영역 설정
	double	dSpecEdgeAreaL				= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_LEFT				];
	double	dSpecEdgeAreaT				= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_TOP				];
	double	dSpecEdgeAreaR				= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_RIGHT				];
	double	dSpecEdgeAreaB				= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_BOTTOM			];

	// 예외처리
	if (dSpecEdgeAreaL < 0)				dSpecEdgeAreaL = 0;
	if (dSpecEdgeAreaT < 0)				dSpecEdgeAreaT = 0;
	if (dSpecEdgeAreaR < 0)				dSpecEdgeAreaR = 0;
	if (dSpecEdgeAreaB < 0)				dSpecEdgeAreaB = 0;

	// Edge Spec
	double	dSpecEdgeBrightRatio		= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_BRIGHT_RATIO		];

	double	dSpecEdgeDarkRatio1			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_1		];
	double	dSpecEdgeDarkArea1			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA_1		];
	double	dSpecEdgeDarkDiff1			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_1		];

	double	dSpecEdgeDarkRatio2			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_2		];
	double	dSpecEdgeDarkArea2			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA_2		];
	double	dSpecEdgeDarkDiff2			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_2		];

	double	dSpecEdgeDarkRatio3			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_3		];
	double	dSpecEdgeDarkArea3			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA_3		];
	double	dSpecEdgeDarkDiff3			= dPara[E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_3		];

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

	// 버퍼 할당 & 초기화
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	long	nWidth	= (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuffer.rows;	// 영상 세로 크기

	int nOffset = 200;
	cv::Mat matDefectMorp1Buf	= cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);
	cv::Mat matDefectMorp2Buf	= cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);
	cv::Mat matDefectBKBuf		= cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);
	cv::Mat matDefectThBuf		= cMatBufTemp.GetMat(cv::Size(nOffset, nOffset), matSrcBuffer.type(), false);

	// 불량 개수 만큼
	for (int i=0 ; i<nCount ; i++)
	{
		// Spot 불량이 아닌 경우 넘어가기
		if( pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT || pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT) //04.16 choi
			continue;

		// 불량 주변 영역
		cv::Rect rectTempROI(pResultBlob->nCenterx[i]-nOffset/2, pResultBlob->nCentery[i]-nOffset/2, nOffset, nOffset);
		if( rectTempROI.x < 0 )						rectTempROI.x = 0;
		if( rectTempROI.y < 0 )						rectTempROI.y = 0;
		if( rectTempROI.x + nOffset >= nWidth )		rectTempROI.x = nWidth  - nOffset - 1;
		if( rectTempROI.y + nOffset >= nHeight )	rectTempROI.y = nHeight - nOffset - 1;

		// 불량 영역
		cv::Mat matDefectSrcBuf = matSrcBuffer(rectTempROI);
		cv::Mat matTempBuf = matDstBuffer(rectTempROI);

		// 백점 하기 위해 이진화 ( 백점 : 255 / 백무라 200 )
		cv::Mat matDefectResBuf = cMatBufTemp.GetMat(matTempBuf.size(), matTempBuf.type(), false);
		cv::threshold(matTempBuf, matDefectResBuf, 220, 255.0, CV_THRESH_BINARY);

		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Src_%02d.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i);
			ImageSave(strTemp, matDefectSrcBuf );

			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Res_%02d.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i);
			ImageSave(strTemp, matDefectResBuf );
		}
		
		// 불량 조금 크게 설정
		int nValue = nMorpObj * 2 + 1;
		if( nMorpObj >= 1 )
			cv::morphologyEx( matDefectResBuf, matDefectMorp1Buf, MORPH_DILATE, cv::getStructuringElement( MORPH_ELLIPSE, cv::Size(nValue, nValue) ) );
		else
			matDefectResBuf.copyTo(matDefectMorp1Buf);

		// 배경 부분
		cv::bitwise_not(matDefectMorp1Buf, matDefectBKBuf);

		// 어두운 부분 찾기
		cv::threshold(matDefectSrcBuf, matDefectThBuf, 20, 255.0, THRESH_BINARY_INV);

		// 어두운 부분 제거
		cv::subtract(matDefectMorp1Buf,	matDefectThBuf,	matDefectMorp1Buf	);	// 검출
		cv::subtract(matDefectBKBuf,	matDefectThBuf,	matDefectBKBuf		);	// 배경

		// 배경 부분 mean & stdDev 구하기
		cv::Scalar meanBK, stdBK;
		cv::meanStdDev(matDefectSrcBuf, meanBK, stdBK, matDefectBKBuf);

		// 밝은 부분 찾기
		cv::threshold(matDefectSrcBuf, matDefectThBuf, nThreshold, 255.0, THRESH_BINARY);

		// 밝은 부분 조금 크게 설정
		nValue = nMorpBKG * 2 + 1;
		if( nMorpBKG >= 1 )
			cv::morphologyEx( matDefectThBuf, matDefectMorp2Buf, MORPH_DILATE, cv::getStructuringElement( MORPH_RECT, cv::Size(nValue, nValue) ) );
		else
			matDefectThBuf.copyTo(matDefectMorp2Buf);
			
		// 검출된 불량 안에 밝은 부분 있는지 확인
		double valMax=0;
		cv::bitwise_and(matDefectMorp1Buf, matDefectMorp2Buf, matDefectThBuf);
		cv::minMaxLoc(matDefectThBuf, NULL, &valMax);

		if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Bri_%02d.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i);
			ImageSave(strTemp, matDefectThBuf );
		}

		// 밝은 영역이 있는지 확인
		bool bPoint = (valMax > 100) ? true : false;

		// Active 영역만 설정
		CRect rectTemp;
		rectTemp.left	= rectROI.left		+ dSpecEdgeAreaL;
		rectTemp.top	= rectROI.top		+ dSpecEdgeAreaT;
		rectTemp.right	= rectROI.right		- dSpecEdgeAreaR;
		rectTemp.bottom	= rectROI.bottom	- dSpecEdgeAreaB;

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
					CString strTemp;
					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_Src_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanObj[0]);
					ImageSave(strTemp, matDefectMorp2Buf);

					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_BK_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanBK[0]);
					ImageSave(strTemp, matDefectBKBuf);
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

				// 무라 정보
				double	dArea = pResultBlob->nArea[i];
				double	dSub = meanObj[0] - meanBK[0];
				double	dDiff = pResultBlob->dMeanGV[i] / pResultBlob->dBackGroundGV[i];

				// Spec1
				if (dArea >= dSpecActiveDarkArea1		&&
					dSpecActiveDarkArea1 > 0			&&
					dSub > dSpecActiveDarkRatio1		&&
					dDiff > dSpecActiveDarkDiff1		)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec2
				else if (dArea >= dSpecActiveDarkArea2	&&
					dSpecActiveDarkArea2 > 0			&&
					dSub > dSpecActiveDarkRatio2		&& 
					dSub <= dSpecActiveDarkRatio1		&&
					dDiff > dSpecActiveDarkDiff2		)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec3
				else if (dArea >= dSpecActiveDarkArea3	&&
					dSpecActiveDarkArea3 > 0			&&
					dSub > dSpecActiveDarkRatio3		&&  
					dSub <= dSpecActiveDarkRatio2		&&
					dDiff > dSpecActiveDarkDiff3		)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				else
				{
					pResultBlob->bUseResult[i] = false;
				}

				if (bImageSave)
				{
					CString strTemp;
					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_Src_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanObj[0]);
					ImageSave(strTemp, matDefectMorp2Buf);

					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Active_BK_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanBK[0]);
					ImageSave(strTemp, matDefectBKBuf);
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
					CString strTemp;
					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_Src_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanObj[0]);
					ImageSave(strTemp, matDefectMorp2Buf);

					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_BK_O_%02d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, meanBK[0]);
					ImageSave(strTemp, matDefectBKBuf);
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
				if (dArea >= dSpecEdgeDarkArea1		&&
					dSpecEdgeDarkArea1 > 0			&&
					dSub > dSpecEdgeDarkRatio1		&&
					dDiff > dSpecEdgeDarkDiff1		)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec2
				else if (dArea >= dSpecEdgeDarkArea2	&&
					dSpecEdgeDarkArea2 > 0				&&
					dSub > dSpecEdgeDarkRatio2			&&
					dSub <= dSpecEdgeDarkRatio1			&&
					dDiff > dSpecEdgeDarkDiff2			)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				// Spec3
				else if (dArea >= dSpecEdgeDarkArea3	&&
					dSpecEdgeDarkArea3 > 0				&&
					dSub > dSpecEdgeDarkRatio3			&& 
					dSub <= dSpecEdgeDarkRatio2			&&
					dDiff > dSpecEdgeDarkDiff3			)
				{
					pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT;
				}
				else
				{
					pResultBlob->bUseResult[i] = false;
				}

				if (bImageSave)
				{
					CString strTemp;
					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_Src_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanObj[0]);
					ImageSave(strTemp, matDefectMorp2Buf);

					strTemp.Format(_T("%s%s_%s_%02d_%02d_AVI_MURA_Spot_Edge_BK_X_%02d_Area%d_%.3f.bmp"), strAlgPath, gg_strPat[nImageNum], gg_strCam[nCamNum], nROINumber, nSaveImageCount++, i, pResultBlob->nArea[i], meanBK[0]);
					ImageSave(strTemp, matDefectBKBuf);
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


// White Mura 재분류
long CInspectMura::JudgeWhiteMura(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
	stDefectInfo* pResultBlob, CMatBuf* cMemSub)
{
	// 불량이 없는 경우 나가기
	int nCount = pResultBlob->nDefectCount;
	if (nCount <= 0)		return E_ERROR_CODE_TRUE;

	// 사용 파라미터
	//////////////////////////////////////////////////////////////////////////spec1
	int		nSpec1_Act_Flag = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC1_FLAG];
	double	dSpecActiveDarkRatio1 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_1];
	double	dSpecActiveDarkArea1_MIN = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA1_MIN];
	double	dSpecActiveDarkArea1_MAX = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA1_MAX];
	double	dSpecActiveDarkDiff1 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_1];

	//////////////////////////////////////////////////////////////////////////spec2
	int		nSpec2_Act_Flag = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC2_FLAG];
	double	dSpecActiveDarkRatio2 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_2];
	double	dSpecActiveDarkArea2_MIN = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA2_MIN];
	double	dSpecActiveDarkArea2_MAX = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA2_MAX];
	double	dSpecActiveDarkDiff2 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_2];

	//////////////////////////////////////////////////////////////////////////spec3
	int		nSpec3_Act_Flag = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC3_FLAG];
	double	dSpecActiveDarkRatio3 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_3];
	double	dSpecActiveDarkArea3_MIN = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA3_MIN];
	double	dSpecActiveDarkArea3_MAX = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA3_MAX];
	double	dSpecActiveDarkDiff3 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_3];

	//////////////////////////////////////////////////////////////////////////spec4
	int		nSpec4_Act_Flag = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC4_FLAG];
	double	dSpecActiveDarkRatio4 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_4];
	double	dSpecActiveDarkArea4_MIN = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA4_MIN];
	double	dSpecActiveDarkArea4_MAX = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA4_MAX];
	double	dSpecActiveDarkDiff4 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_4];

	// Edge 영역 설정
	double	dSpecEdgeAreaL = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_LEFT];
	double	dSpecEdgeAreaT = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_TOP];
	double	dSpecEdgeAreaR = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_RIGHT];
	double	dSpecEdgeAreaB = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_BOTTOM];

	// 예외처리
	if (dSpecEdgeAreaL < 0)				dSpecEdgeAreaL = 0;
	if (dSpecEdgeAreaT < 0)				dSpecEdgeAreaT = 0;
	if (dSpecEdgeAreaR < 0)				dSpecEdgeAreaR = 0;
	if (dSpecEdgeAreaB < 0)				dSpecEdgeAreaB = 0;

	// Edge Spec
	

	//////////////////////////////////////////////////////////////////////////SPEC1
	int		nSpec1_Edge_Flag = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC1_FLAG];
	double	dSpecEdgeDarkRatio1 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_1];
	double	dSpecEdgeDarkArea1_MIN = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA1_MIN];
	double	dSpecEdgeDarkArea1_MAX = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA1_MAX];
	double	dSpecEdgeDarkDiff1 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_1];

	//////////////////////////////////////////////////////////////////////////SPEC2
	int		nSpec2_Edge_Flag = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC2_FLAG];
	double	dSpecEdgeDarkRatio2 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_2];
	double	dSpecEdgeDarkArea2_MIN = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA2_MIN];
	double	dSpecEdgeDarkArea2_MAX = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA2_MAX];
	double	dSpecEdgeDarkDiff2 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_2];

	//////////////////////////////////////////////////////////////////////////SPEC3
	int		nSpec3_Edge_Flag = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC3_FLAG];
	double	dSpecEdgeDarkRatio3 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_3];
	double	dSpecEdgeDarkArea3_MIN = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA3_MIN];
	double	dSpecEdgeDarkArea3_MAX = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA3_MAX];
	double	dSpecEdgeDarkDiff3 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_3];

	//////////////////////////////////////////////////////////////////////////SPEC4
	int		nSpec4_Edge_Flag = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC4_FLAG];
	double	dSpecEdgeDarkRatio4 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_4];
	double	dSpecEdgeDarkArea4_MIN = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA4_MIN];
	double	dSpecEdgeDarkArea4_MAX = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA4_MAX];
	double	dSpecEdgeDarkDiff4 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_4];
	
	//////////////////////////////////////////////////////////////////////////
	// 불량 개수 만큼
	for (int i = 0; i < nCount; i++)
	{
		// White Mura 불량이 아닌 경우 넘어가기
		if (pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT)
			continue;

		// Active 영역만 설정
		CRect rectTemp;
		rectTemp.left = rectROI.left + dSpecEdgeAreaL;
		rectTemp.top = rectROI.top + dSpecEdgeAreaT;
		rectTemp.right = rectROI.right - dSpecEdgeAreaR;
		rectTemp.bottom = rectROI.bottom - dSpecEdgeAreaB;

		// 불량 중심 좌표
		CPoint ptSrc(pResultBlob->nCenterx[i], pResultBlob->nCentery[i]);

		// Active 범위 안에 WM 불량 존재
		if (rectTemp.PtInRect(ptSrc))
		{

			// 무라 정보
			double	dArea = pResultBlob->nArea[i];
			double	dSub = pResultBlob->dMeanGV[i] - pResultBlob->dBackGroundGV[i];
			double	dDiff = pResultBlob->dMeanGV[i] / pResultBlob->dBackGroundGV[i];

			// Spec1
			if (dArea >= dSpecActiveDarkArea1_MIN		&&
				dArea < dSpecActiveDarkArea1_MAX	&&
				nSpec1_Act_Flag > 0 &&
				dSub > dSpecActiveDarkRatio1		&&
				dDiff > dSpecActiveDarkDiff1)
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT;
			}
			// Spec2
			else if (dArea >= dSpecActiveDarkArea2_MIN	&&
				dArea < dSpecActiveDarkArea2_MAX	&&
				nSpec2_Act_Flag > 0 &&
				dSub > dSpecActiveDarkRatio2		&&
				dDiff > dSpecActiveDarkDiff2)
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT;
			}
			// Spec3
			else if (dArea >= dSpecActiveDarkArea3_MIN	&&
				dArea < dSpecActiveDarkArea3_MAX	&&
				nSpec3_Act_Flag > 0 &&
				dSub > dSpecActiveDarkRatio3		&&
				dDiff > dSpecActiveDarkDiff3)
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT;
			}
			// Spec4
			else if (dArea >= dSpecActiveDarkArea4_MIN	&&
				dArea < dSpecActiveDarkArea4_MAX	&&
				nSpec4_Act_Flag > 0 &&
				dSub > dSpecActiveDarkRatio4		&&
				dDiff > dSpecActiveDarkDiff4)
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT;
			}
			else
			{
				pResultBlob->bUseResult[i] = false;
			}
		}
		//Edge 범위 안에 WM 불량 존재
		else {
			// 무라 정보
			double	dArea = pResultBlob->nArea[i];
			double	dSub = pResultBlob->dMeanGV[i] - pResultBlob->dBackGroundGV[i];
			double	dDiff = pResultBlob->dMeanGV[i] / pResultBlob->dBackGroundGV[i];

			// Spec1
			if (dArea >= dSpecActiveDarkArea1_MIN		&&
				dArea < dSpecActiveDarkArea1_MAX	&&
				nSpec1_Act_Flag > 0 &&
				dSub > dSpecActiveDarkRatio1		&&
				dDiff > dSpecActiveDarkDiff1)
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT;
			}
			// Spec2
			else if (dArea >= dSpecActiveDarkArea2_MIN	&&
				dArea < dSpecActiveDarkArea2_MAX	&&
				nSpec2_Act_Flag > 0 &&
				dSub > dSpecActiveDarkRatio2		&&
				dDiff > dSpecActiveDarkDiff2)
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT;
			}
			// Spec3
			else if (dArea >= dSpecActiveDarkArea3_MIN	&&
				dArea < dSpecActiveDarkArea3_MAX	&&
				nSpec3_Act_Flag > 0 &&
				dSub > dSpecActiveDarkRatio3		&&
				dDiff > dSpecActiveDarkDiff3)
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT;
			}
			// Spec4
			else if (dArea >= dSpecActiveDarkArea4_MIN	&&
				dArea < dSpecActiveDarkArea4_MAX	&&
				nSpec4_Act_Flag > 0 &&
				dSub > dSpecActiveDarkRatio4		&&
				dDiff > dSpecActiveDarkDiff4)
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT;
			}
			else
			{
				pResultBlob->bUseResult[i] = false;
			}
		}
	}

	return E_ERROR_CODE_TRUE;
}

// White Mura 재분류
/*long CInspectMura::JudgeWhiteMura(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
	stDefectInfo* pResultBlob, CMatBuf* cMemSub)
{
	// 불량이 없는 경우 나가기
	int nCount = pResultBlob->nDefectCount;
	if (nCount <= 0)		return E_ERROR_CODE_TRUE;

	// 사용 파라미터
	double	dSpecMuraRatio1 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_DIFF_1];
	double	dSpecMuraArea1 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_AREA_1];

	double	dSpecMuraRatio2 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_DIFF_2];
	double	dSpecMuraArea2 = dPara[E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_AREA_2];

	//////////////////////////////////////////////////////////////////////////

	// 불량 개수 만큼
	for (int i = 0; i < nCount; i++)
	{
		// White Mura 불량이 아닌 경우 넘어가기
		if (pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT)
			continue;

		// 무라 정보
		double	dArea = pResultBlob->nArea[i];
		double	dDiff = pResultBlob->dMeanGV[i] / pResultBlob->dBackGroundGV[i];

		// Spec1
		if (dArea >= dSpecMuraArea1		&&
			dSpecMuraArea1 > 0 &&
			dDiff > dSpecMuraRatio1)
		{
			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT;
		}
		// Spec2
		else if (dArea >= dSpecMuraArea2	&&
			dSpecMuraArea2 > 0 &&
			dDiff > dSpecMuraRatio2)
		{
			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT;
		}
		else
		{
			pResultBlob->bUseResult[i] = false;
		}
	}

	return E_ERROR_CODE_TRUE;
}*/

// Nugi 재분류
// SCJ 20.02.18 - B11 曹文 요청 EDGE 영역 // ACTIVE 영역 NUGI 불량 분류 요청
long CInspectMura::JudgeNugi(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
	stDefectInfo* pResultBlob, CMatBuf* cMemSub)
{
	// 불량이 없는 경우 나가기
	int nCount = pResultBlob->nDefectCount;
	if (nCount <= 0)		return E_ERROR_CODE_TRUE;

	// 사용 할것인가?
	double	dUse = dPara[E_PARA_AVI_MURA_JUDGE_EDGE_NUGI_USE];
	if(dUse <= 0)			return E_ERROR_CODE_TRUE;

	//////////////////////////////////////////////////////////////////////////

	// 불량 개수 만큼
	for (int i = 0; i < nCount; i++)
	{
		// Nugi 불량이 아닌 경우 넘어가기
		if (pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_MURA_NUGI)
			continue;

		// 귀 부분 영역이 큼
		//if (pResultBlob->nArea[i] < 450000)
		//	continue;
		//
		//// 귀 부분 영역은 길쭉함
		//if (pResultBlob->dF_Elongation[i] < 3.0)
		//	continue;

		// 귀 영역만 설정
		//CRect rectTemp;
		//rectTemp.left = rectROI.left;
		//rectTemp.top = rectROI.top;
		//rectTemp.right = rectROI.left + 400;
		//rectTemp.bottom = rectROI.bottom;

		// ACTIVE 영역 전체 기준 EDGE 설정
		int nRectInX = 150, nRectInY = 150;
		CRect rectTemp;
		rectTemp.left = rectROI.left + nRectInX;
		rectTemp.top = rectROI.top + nRectInY;
		rectTemp.right = rectROI.right - nRectInX;
		rectTemp.bottom = rectROI.bottom - nRectInY;

		// 불량 중심 좌표
		CPoint ptSrc(pResultBlob->nCenterx[i], pResultBlob->nCentery[i]);

		// 귀 부분 범위 안에 불량 존재
		//if (!rectTemp.PtInRect(ptSrc))
		//	continue;

		if (rectTemp.PtInRect(ptSrc))
			continue;

		pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI;
	}

	return E_ERROR_CODE_TRUE;
}


bool CInspectMura::OrientedBoundingBox(cv::RotatedRect& rect1, cv::RotatedRect& rect2)
{
	// Distance Vector
	cv::Point2d dist, unit, vec[4];
	dist.x = rect1.center.x - rect2.center.x;
	dist.y = rect1.center.y - rect2.center.y;

	// rect1 Height Vector
	vec[0].x = rect1.size.height * cos( (rect1.angle - 90.0) / 180.0 * PI ) / 2.0;
	vec[0].y = rect1.size.height * sin( (rect1.angle - 90.0) / 180.0 * PI ) / 2.0;

	// rect2 Height Vector
	vec[1].x = rect2.size.height * cos( (rect2.angle - 90.0) / 180.0 * PI ) / 2.0;
	vec[1].y = rect2.size.height * sin( (rect2.angle - 90.0) / 180.0 * PI ) / 2.0;

	// rect1 Width Vector
	vec[2].x = rect1.size.width * cos( rect1.angle / 180.0 * PI ) / 2.0;
	vec[2].y = rect1.size.width * sin( rect1.angle / 180.0 * PI ) / 2.0;

	// rect2 Width Vector
	vec[3].x = rect2.size.width * cos( rect2.angle / 180.0 * PI ) / 2.0;
	vec[3].y = rect2.size.width * sin( rect2.angle / 180.0 * PI ) / 2.0;

	// 4개 벡터 중...
	bool bRes = true;
	for(int i=0; i<4; i++)
	{
		double dSum = 0;

		double dSize = sqrt( vec[i].x * vec[i].x + vec[i].y * vec[i].y );
		unit.x = (double)(vec[i].x / dSize);
		unit.y = (double)(vec[i].y / dSize);

		// 4개 내적 중...
		for(int j=0; j<4; j++)
			dSum += abs(unit.x * vec[j].x + unit.y * vec[j].y);

		// 하나라도 분리할수 있는 경우가 존재하는 경우
		if( abs(unit.x * dist.x + unit.y * dist.y) > dSum )
		{
			bRes = false;
			break;
		}
	}

	return bRes;
}
//////////////////////////////////////////////////////////////////////////
void  CInspectMura::C_Mexican_filter(cv::Mat& ip) {
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
	convolveFloat(ip, kernel2, sz, sz);
	//}
	double sigma2 = (sz - 1) / 10.0;
	sigma2 *= sigma2;
	ip *= sigma2;

	//cv::GaussianBlur(ip, ip, cv::Size(21, 21), 5, 5);


}

float*  CInspectMura::computeKernel2D(int r) {
	sz = 2 * r + 1;
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

boolean  CInspectMura::convolveFloat(cv::Mat& ip, float* kernel, int kw, int kh) {

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

	cv::Mat pixels;
	cv::Mat pixels2;
	pixels = ip;
	ip.copyTo(pixels2);

	double scale = getScale(kernel);
	// 	Thread thread = Thread.currentThread();
	// 	boolean isMainThread = thread == mainThread || thread.getName().indexOf("Preview") != -1;
	// 	if (isMainThread) pass++;
	double sum;
	int offset, i;
	boolean edgePixel;
	int xedge = width - uc;
	int yedge = height - vc;
	//long lastTime = System.currentTimeMillis();


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
						if (i >= sz*sz) return false;

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

	}
	return true;
}

double CInspectMura::getScale(float* kernel) {
	double scale = 1.0;
	double sum = 0.0;
	for (int i = 0; i < sz*sz; i++)
		sum += kernel[i];
	if (sum != 0.0)
		scale = 1.0 / sum;
	return scale;
}

//////////////////////////////////////////////////////////////////////////choi 21.02.05
//////////////////////////////////////////////////////////////////////////
// G3 Pattern G3 / L3 혼색 불량 검사 알고리즘
// 20.07.13
// PNZ
//////////////////////////////////////////////////////////////////////////

long CInspectMura::LogicStart_MuraG3CM(cv::Mat& matSrcImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{

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
	int		nPS = nCommonPara[E_PARA_COMMON_PS_MODE];
	int		nImageUI = nCommonPara[E_PARA_COMMON_UI_IMAGE_NUMBER];

	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("G3-CM Start."));

	// Parameters
	int		nShiftCopy = (int)dPara[E_PARA_AVI_MURA_G3_PREPROCESS_SHIFTCOPY];		// 5010;
	int		nResizeUnitPara = (int)dPara[E_PARA_AVI_MURA_G3_PREPROCESS_RESIZEUNIT];		// 100;
	int		nLimitLength = (int)dPara[E_PARA_AVI_MURA_G3_PREPROCESS_LIMITLENGTH];		// 10000;	
	int		nBlurLevel01 = (int)dPara[E_PARA_AVI_MURA_G3_PREPROCESS_BLUR_L01];		// 15;
	int		nBlurLevel02 = (int)dPara[E_PARA_AVI_MURA_G3_PREPROCESS_BLUR_L02];		// 9;
	int		nBlurLevel03 = (int)dPara[E_PARA_AVI_MURA_G3_PREPROCESS_BLUR_L03];		// 3;
	int		nJudge_DiffGV = (int)dPara[E_PARA_AVI_MURA_G3_JUDGE_DIFF_GV];		// 13;

																				// Memory Reset
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);

	cv::Rect rectTemp;

	int nRatio = 0;
	int nResizeUnit = 0;
	int nCpyX = 0;
	int nCpyY = 0;
	int nLoopX = 0;
	int nLoopY = 0;

	if (matSrcImage.cols > nLimitLength) { nRatio = 2; nResizeUnit = nResizeUnitPara; }
	else { nRatio = 1; nResizeUnit = nResizeUnitPara / 2; }

	rectTemp.x = rectROI.left		* nRatio;
	rectTemp.y = rectROI.top		* nRatio;
	rectTemp.width = rectROI.right	* nRatio - rectROI.left	* nRatio;
	rectTemp.height = rectROI.bottom	* nRatio - rectROI.top	* nRatio;

	cv::Mat matSrcTemp = cMatBufTemp.GetMat(rectTemp.height, rectTemp.width, CV_8UC1, false);
	cv::Mat matSubTemp_SC = cMatBufTemp.GetMat(rectTemp.height, rectTemp.width, CV_8UC1, false);
	cv::Mat matSubTemp_Blur1 = cMatBufTemp.GetMat(rectTemp.height / nResizeUnit, rectTemp.width / nResizeUnit, CV_8UC1, false);
	cv::Mat matSubTemp_Blur2 = cMatBufTemp.GetMat(rectTemp.height / nResizeUnit, rectTemp.width / nResizeUnit, CV_8UC1, false);
	cv::Mat matSubTemp_RE = cMatBufTemp.GetMat(rectTemp.height / nResizeUnit, rectTemp.width / nResizeUnit, CV_8UC1, false);

	matSrcImage(rectTemp).copyTo(matSrcTemp);

	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("G3-CM Get Memory & Set Para."));

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MN_%02d_G3CM_Input.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcImage);
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MN_%02d_G3CM_Input_Sub.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSrcTemp);
	}

	//////////////////////////////////////////////////////////////////////////
	// Pre-Process

	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("G3-CM Pre-Process Start."));

	ShiftCopyParaCheck(nShiftCopy, nCpyX, nCpyY, nLoopX, nLoopY);
	ShiftCopy(matSrcTemp, matSubTemp_SC, nCpyX, nCpyY, nLoopX, nLoopY, &cMatBufTemp);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MN_%02d_G3CM_ShiftCopy.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSubTemp_SC);
	}

	cv::blur(matSubTemp_SC, matSubTemp_Blur1, cv::Size(nBlurLevel01, nBlurLevel01));

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MN_%02d_G3CM_Blur1.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSubTemp_Blur1);
	}

	AveragingReducer(matSubTemp_Blur1, matSubTemp_Blur2, &cMatBufTemp);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MN_%02d_G3CM_AR.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSubTemp_Blur2);
	}

	cv::blur(matSubTemp_Blur2, matSubTemp_Blur2, cv::Size(nBlurLevel02, nBlurLevel02));

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MN_%02d_G3CM_Blur2.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSubTemp_Blur2);
	}

	cv::resize(matSubTemp_Blur2, matSubTemp_RE, matSubTemp_SC.size() / nResizeUnit, INTER_CUBIC);

	cv::blur(matSubTemp_RE, matSubTemp_RE, cv::Size(nBlurLevel03, nBlurLevel03));

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MN_%02d_G3CM_Resize.jpg"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		ImageSave(strTemp, matSubTemp_RE);
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("G3-CM Pre-Process End."));

	// Main Process

	cv::Mat matHist_Total, matHist_Resize;

	GetHistogram(matSubTemp_Blur1, matHist_Total);
	GetHistogram(matSubTemp_RE, matHist_Resize);

	int		nLowerIndex = 0;
	int		nUpperIndex = 0;
	int		nLowUpDiff = 0;
	int		nLowVArea = 0;
	int		nTopVArea = 0;

	HistAreaCalc(matHist_Resize, nLowerIndex, nUpperIndex, nLowUpDiff, nLowVArea, nTopVArea, dPara);

	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("Main Process End."));

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MN_%02d_G3CM_DataHIST.csv"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);

		char szPath[MAX_PATH] = { 0, };
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		FILE *out = NULL;
		fopen_s(&out, szPath, "wt");

		if (out)
		{

			fprintf_s(out, "LowIndex, %d\n", nLowerIndex);
			fprintf_s(out, "LowArea	, %d\n", nLowVArea);
			fprintf_s(out, "TopIndex, %d\n", nUpperIndex);
			fprintf_s(out, "TopArea	, %d\n", nTopVArea);
			fprintf_s(out, "LTDiff	, %d\n", nLowUpDiff);

			fprintf_s(out, "\n");

			fprintf_s(out, "Hist, Total, Resize\n");

			for (int j = 0; j < 256; j++)
			{

				fprintf_s(out, "%d,%f,%f\n", j, matHist_Total.at<float>(j, 0), matHist_Resize.at<float>(j, 0));
			}

			fclose(out);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// G3 Judgement

	int  nCountNumber = pResultBlob->nDefectCount;
	int  nDefectIndex = 0;
	bool bOnlyG3CM = true;

	if (nCountNumber == 0)	bOnlyG3CM = true;
	else					bOnlyG3CM = false;

	// Defect Index Calc.
	if (bOnlyG3CM == true)	 nDefectIndex = 0;
	else if (bOnlyG3CM == false) nDefectIndex = nCountNumber;

	if (nLowUpDiff == 0) return false;

	// Data Check
	if (nLowUpDiff >= nJudge_DiffGV)
	{
		if (pResultBlob != NULL)
		{
			pResultBlob->nArea[nDefectIndex] = 0;
			pResultBlob->nMaxGV[nDefectIndex] = 255;
			pResultBlob->nMinGV[nDefectIndex] = 0;
			pResultBlob->dMeanGV[nDefectIndex] = 0;

			pResultBlob->ptLT[nDefectIndex].x = 0;
			pResultBlob->ptLT[nDefectIndex].y = 0;
			pResultBlob->ptRT[nDefectIndex].x = 0;
			pResultBlob->ptRT[nDefectIndex].y = 0;
			pResultBlob->ptRB[nDefectIndex].x = 0;
			pResultBlob->ptRB[nDefectIndex].y = 0;
			pResultBlob->ptLB[nDefectIndex].x = 0;
			pResultBlob->ptLB[nDefectIndex].y = 0;

			pResultBlob->dBackGroundGV[nDefectIndex] = 0;
			pResultBlob->dCompactness[nDefectIndex] = 0;
			pResultBlob->dSigma[nDefectIndex] = 0;
			pResultBlob->dBreadth[nDefectIndex] = 0;
			pResultBlob->dF_Min[nDefectIndex] = 0;
			pResultBlob->dF_Max[nDefectIndex] = 0;
			pResultBlob->dF_Elongation[nDefectIndex] = 0;
			pResultBlob->dCompactness[nDefectIndex] = 0;

			// 밝기
			pResultBlob->nDefectColor[nDefectIndex] = E_DEFECT_COLOR_DARK;
			pResultBlob->nDefectJudge[nDefectIndex] = E_DEFECT_JUDGEMENT_MURA_CLOUD;
			pResultBlob->nPatternClassify[nDefectIndex] = nImageUI;

			// 카운트 증가
			pResultBlob->nDefectCount = nDefectIndex + 1;
		}
	}

	writeInspectLog(E_ALG_TYPE_AVI_MURA, __FUNCTION__, _T("G3 Judgement End."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return nErrorCode = E_ERROR_CODE_TRUE;
}

// PNZ ShiftCopy Parameter Check ( 18.10.18 )
long	CInspectMura::ShiftCopyParaCheck(int ShiftValue, int& nCpyX, int& nCpyY, int& nLoopX, int& nLoopY)
{
	if (ShiftValue == 0) return false;

	nCpyX = (int)(ShiftValue / 1000 % 10);	// X 방향 Unit
	nCpyY = (int)(ShiftValue / 100 % 10);	// Y 방향 Unit
	nLoopX = (int)(ShiftValue / 10 % 10);	// X 방향 Loop
	nLoopY = (int)(ShiftValue / 1 % 10);	// Y 방향 Loop

	return E_ERROR_CODE_TRUE;
}


long CInspectMura::ShiftCopy(cv::Mat& matSrcImage, cv::Mat& matDstImage, int nShiftX, int nShiftY, int nShiftLoopX, int nShiftLoopY, CMatBuf* cMemSub)
{
	// 버퍼가 없는 경우.
	if (matSrcImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	// 예외처리
	if (nShiftX < 0)		nShiftX = 0;
	if (nShiftY < 0)		nShiftY = 0;

	nShiftLoopX++;
	nShiftLoopY++;

	//////////////////////////////////////////////////////////////////////////

	// 원본 & 결과
	cv::Mat matSrcBuf, matDstBuf;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	matSrcBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matDstBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matSrcBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matDstBuf = cMatBufTemp.GetMat(matSrcImage.size(), matSrcImage.type(), false);
	matDstImage = cMatBufTemp.GetMat(matDstBuf.size(), matDstBuf.type());

	//matDstImage = cv::Mat::zeros(matDstBuf.size(), matDstBuf.type());
	matSrcImage.copyTo(matSrcBuf);
	matSrcImage.copyTo(matDstBuf);

	// 영상 크기
	int nImageSizeX = matSrcBuf.cols;
	int nImageSizeY = matSrcBuf.rows;

	// 임시 버퍼
	cv::Mat matTempBuf1, matTempBuf2;

	// x 방향
	int nOffsetX = 0;
	for (int x = 1; x < nShiftLoopX; x++)
	{
		nOffsetX = x * nShiftX;

		matTempBuf1 = matDstBuf(cv::Rect(0, 0, nImageSizeX - nOffsetX, nImageSizeY));
		matTempBuf2 = matSrcBuf(cv::Rect(nOffsetX, 0, nImageSizeX - nOffsetX, nImageSizeY));

		// 포인트 불량의 경우 점등 안되어야 할 부분의 배열이 켜질수 있음
		// 덮어쓰면 불량이 사라지므로 사용 불가
		cv::add(matTempBuf1, matTempBuf2, matTempBuf1);
	}

	// y 방향
	int nOffsetY = 0;
	matDstBuf.copyTo(matSrcBuf);
	for (int y = 1; y < nShiftLoopY; y++)
	{
		nOffsetY = y * nShiftY;

		matTempBuf1 = matDstBuf(cv::Rect(0, 0, nImageSizeX, nImageSizeY - nOffsetY));
		matTempBuf2 = matSrcBuf(cv::Rect(0, nOffsetY, nImageSizeX, nImageSizeY - nOffsetY));

		// 포인트 불량의 경우 점등 안되어야 할 부분의 배열이 켜질수 있음
		// 덮어쓰면 불량이 사라지므로 사용 불가
		cv::add(matTempBuf1, matTempBuf2, matTempBuf1);
	}

	// 좌, 상 방향으로만 복사하였으므로, 중앙으로 오도록 이동
	nOffsetX /= 2;
	nOffsetY /= 2;

	matDstBuf(cv::Rect(0, 0, matDstBuf.cols - nOffsetX, matDstBuf.rows - nOffsetY)).copyTo(matDstImage(cv::Rect(nOffsetX, nOffsetY, matDstBuf.cols - nOffsetX, matDstBuf.rows - nOffsetY)));

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


long	CInspectMura::AveragingReducer(cv::Mat& matSrcImage, cv::Mat& matDstImage, CMatBuf* cMemSub)
{
	if (matSrcImage.empty()) return false;

	int nMatWidth = matSrcImage.cols;
	int nMatHeight = matSrcImage.rows;

	int nMatWidth_New = (int)nMatWidth / 2;
	int nMatHeight_New = (int)nMatHeight / 2;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	cv::Mat matTemp = cMatBufTemp.GetMat(matSrcImage.size() / 2, CV_8UC1, false);

	matTemp.setTo(0);

	for (int j = 0; j < nMatHeight_New; j++)
	{
		for (int i = 0; i < nMatWidth_New; i++)
		{
			int nData0 = matSrcImage.at<uchar>(j * 2, i * 2);
			int nData1 = matSrcImage.at<uchar>(j * 2 + 1, i * 2);
			int nData2 = matSrcImage.at<uchar>(j * 2, i * 2 + 1);
			int nData3 = matSrcImage.at<uchar>(j * 2 + 1, i * 2 + 1);

			matTemp.at<uchar>(j, i) = (int)((nData0 + nData1 + nData2 + nData3) / (4 + 0.5));

		}
	}

	matTemp.copyTo(matDstImage);

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory());
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory());
// 	}

	return E_ERROR_CODE_TRUE;
}

long CInspectMura::GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto)
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


long CInspectMura::HistAreaCalc(cv::Mat& matSrcImage, int& nLowerIndex, int& nUpperIndex, int& nLowUpDiff, int& nLowValueArea, int& nTopValueArea, double* dPara)
{
	// 버퍼가 없는 경우.
	if (matSrcImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	// Parameter
	double	dbLowTH = (int)dPara[E_PARA_AVI_MURA_G3_MAIN_LOWAREA_TH]; //77;
	double	dbTopTH = (int)dPara[E_PARA_AVI_MURA_G3_MAIN_TOPAREA_TH]; //41;

	double	dbminValue = 0;
	double	dbmaxValue = 0;

	int		nMaxIndex = 0;
	int		nLowIndex = 0;
	int		nTopIndex = 0;
	float	fLowValue = 0;
	float	fTopValue = 0;

	int		nZeroCount = 0;
	int		nLT_Diff = 0;

	cv::minMaxLoc(matSrcImage, &dbminValue, &dbmaxValue);

	float *ptrIndexValue = (float *)matSrcImage.ptr(0);

	for (int i = 0; i < 256; i++, ptrIndexValue++)
	{
		if (*ptrIndexValue == dbmaxValue) { nMaxIndex = i; break; }
	}

	// Low Calc
	float *ptrLowValue = (float *)matSrcImage.ptr(0) + nMaxIndex;

	for (int i = nMaxIndex; i > 0; i--, ptrLowValue--)
	{
		if (*ptrLowValue <= dbLowTH) { nLowIndex = i + 1; fLowValue = matSrcImage.at<float>(nLowIndex, 0); break; }
	}

	// Top Calc
	float *ptrTopValue = (float *)matSrcImage.ptr(0) + nMaxIndex;

	for (int i = nMaxIndex; i < 256; i++, ptrTopValue++)
	{
		if (*ptrTopValue <= dbTopTH) { nTopIndex = i - 1; fTopValue = matSrcImage.at<float>(nTopIndex, 0); break; }
	}

	nLowerIndex = nLowIndex;
	nUpperIndex = nTopIndex;
	nLowUpDiff = nUpperIndex - nLowerIndex;
	nLowValueArea = (int)fLowValue;
	nTopValueArea = (int)fTopValue;

	return E_ERROR_CODE_TRUE;
}

//////////////////////////////////////////////////////////////////////////