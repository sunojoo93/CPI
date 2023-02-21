
/************************************************************************/
// Point 불량 검출 관련 소스
// 수정일 : 18.02.07 - KSW
/************************************************************************/

#include "StdAfx.h"
#include "InspectSVI.h"


// Pattern String
// E_IMAGE_CLASSIFY_SVI_COUNT 수정될 경우, 수정해야 함
CString g_strPat[ E_IMAGE_CLASSIFY_SVI_COUNT ] = {
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
CString g_strCam[ 2 ] = {
	_T("Coaxial"),
	_T("Side")
};


//////////////////////////////////////////////////////////////////////////


CInspectSVI::CInspectSVI(void)
{
	m_cInspectLibLog	= NULL;
	m_strAlgLog			= NULL;
	m_tInitTime			= 0;
	m_tBeforeTime		= 0;
}


CInspectSVI::~CInspectSVI(void)
{
}

//////////////////////////////////////////////////////////////////////////


// Main 검사 알고리즘
long CInspectSVI::DoFindMuraDefect(cv::Mat matSrcBuf, cv::Mat matBKBuf, cv::Mat& matDrawBuf, cv::Point* ptCorner, double* dPara,
	int* nCommonPara, wchar_t strPath[][1024], STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CColorCorrection *cColor)
{
	// 파라미터 NULL인 경우
	if( dPara == NULL )					return E_ERROR_CODE_EMPTY_PARA;
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcBuf.empty() )				return E_ERROR_CODE_EMPTY_BUFFER;

	long	nWidth	= (long)matSrcBuf.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuf.rows;	// 영상 세로 크기

	// 영상 번호
	long	nImageClassify = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];

	// 에러 코드
	long nErrorCode = E_ERROR_CODE_TRUE;

	// 범위 크게 설정
	CRect rectROI = CRect(
		min(ptCorner[E_CORNER_LEFT_TOP].x,		ptCorner[E_CORNER_LEFT_BOTTOM].x	),
		min(ptCorner[E_CORNER_LEFT_TOP].y,		ptCorner[E_CORNER_RIGHT_TOP].y		),
		max(ptCorner[E_CORNER_RIGHT_TOP].x,		ptCorner[E_CORNER_RIGHT_BOTTOM].x	),
		max(ptCorner[E_CORNER_LEFT_BOTTOM].y,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	) );

	// 에러인 경우 에러코드
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ROI_OVER;

	if( rectROI.left >= rectROI.right )	return E_ERROR_CODE_ROI_OVER;
	if( rectROI.top >= rectROI.bottom )	return E_ERROR_CODE_ROI_OVER;

	// 영상 마다 알고리즘 다름
	switch(nImageClassify)
	{
	case E_IMAGE_CLASSIFY_SVI_R:
	case E_IMAGE_CLASSIFY_SVI_G:
	case E_IMAGE_CLASSIFY_SVI_B:

		// HS 불량 검사
		//nErrorCode = LogicStart_HS(matSrcBuf, matBKBuf, matDrawBuf, rectROI, dPara, nCommonPara, strPath, EngineerDefectJudgment, pResultBlob, cColor);

		// EMD 불량 검사
		nErrorCode = LogicStart_EMD(matSrcBuf, matBKBuf, matDrawBuf, rectROI, dPara, nCommonPara, strPath, EngineerDefectJudgment, pResultBlob);
		break;

	case E_IMAGE_CLASSIFY_SVI_BLACK:
		break;

	case E_IMAGE_CLASSIFY_SVI_WHITE:
	case E_IMAGE_CLASSIFY_SVI_GRAY_32:
	case E_IMAGE_CLASSIFY_SVI_GRAY_64:
	case E_IMAGE_CLASSIFY_SVI_GRAY_87:
	case E_IMAGE_CLASSIFY_SVI_GRAY_128:
		// HS 불량 검사
		nErrorCode = LogicStart_HS(matSrcBuf, matBKBuf, matDrawBuf, rectROI, dPara, nCommonPara, strPath, EngineerDefectJudgment, pResultBlob, cColor);

		// EMD 불량 검사
		nErrorCode = LogicStart_EMD(matSrcBuf, matBKBuf, matDrawBuf, rectROI, dPara, nCommonPara, strPath, EngineerDefectJudgment, pResultBlob);
		break;

		// 영상 번호가 잘못 들어온 경우.
	default:
		break;
	}

	// 에러인 경우 에러코드
	if( nErrorCode != E_ERROR_CODE_TRUE )	return nErrorCode;

	return E_ERROR_CODE_TRUE;
}


// 혼색 불량 검출 로직
long CInspectSVI::LogicStart_HS(cv::Mat& matSrcBuf, cv::Mat &matBKBuf, cv::Mat& matDrawBuf, CRect rectROI, double* dPara, int* nCommonPara, 
	wchar_t _strPath[][1024], STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CColorCorrection *cColor)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if( matSrcBuf.empty() )	return E_ERROR_CODE_EMPTY_BUFFER;
	
	//////////////////////////////////////////////////////////////////////////
	// 파라미터 설정
	//////////////////////////////////////////////////////////////////////////

	float	DEGINRAD	= PI / 180.0f;	

	int		nGBSize		= (int)dPara[E_PARA_SVI_MURA_HS_GB_SIZE			];	//31;				// 가우시안 마스크 크기
	float	fSigma		= (float)dPara[E_PARA_SVI_MURA_HS_GB_SIGMA		];	//3.0f;				// 가우시안 시그마
	float	fAxisX		= (float)dPara[E_PARA_SVI_MURA_HS_AXIS_X		];	//0.053938916f;		// 타원 x축
	float	fAxisY		= (float)dPara[E_PARA_SVI_MURA_HS_AXIS_Y		];	//0.12808323f;		// 타원 y축
	float	fSDAxisX	= (float)dPara[E_PARA_SVI_MURA_HS_STANDARD_AXIS_X		];	//0.053938916f;		// 기준 타원 x축
	float	fSDAxisY	= (float)dPara[E_PARA_SVI_MURA_HS_STANDARD_AXIS_Y		];	//0.12808323f;		// 기준 타원 y축
	float	fCenterX	= (float)dPara[E_PARA_SVI_MURA_HS_CENTER_X		];	//0.31271592f;		// 중심 영역 x좌표
	float	fCenterY	= (float)dPara[E_PARA_SVI_MURA_HS_CENTER_Y		];	//0.32900149f;		// 중심 영역 y좌표
	float	fCenterR	= (float)dPara[E_PARA_SVI_MURA_HS_CENTER_OFFSET	];	//0.02f;			// 중심 영역 최대 보정 영역 (원 반지름)
	float	fAngle		= (float)dPara[E_PARA_SVI_MURA_HS_ANGLE			];	//-35.135487f;		// 타원 회전 각도
	float	fRangeA		= (float)dPara[E_PARA_SVI_MURA_HS_ANGLE_OFFSET	];	//3.0f;				// 타원 회전 각도 보정
	float	fAngleAfter = fAngle;																// 보정 후, 각도

	long	nDeleteOutLine	= (int)dPara[E_PARA_SVI_MURA_DELETE_PIXEL	];	//0;				// 최 외곽 불량 제거

	// 예외 처리
	if( fAxisX < 0)		return E_ERROR_CODE_SVI_WRONG_PARAM;
	if( fAxisY < 0)		return E_ERROR_CODE_SVI_WRONG_PARAM;
	if( fCenterX < 0 )	return E_ERROR_CODE_SVI_WRONG_PARAM;
	if( fCenterY < 0 )	return E_ERROR_CODE_SVI_WRONG_PARAM;

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
	int		nRatio				=  nCommonPara[E_PARA_COMMON_PS_MODE				];


	int		nLineThreshold		=  (int)dPara[E_PARA_SVI_LINE_TH				];
	int		nLineThickness		=  (int)dPara[E_PARA_SVI_LINE_THICKNESS			];
	float	fLineLength		=  (float)dPara[E_PARA_SVI_LINE_LENGTH			];

	if( nLineThreshold <= 0 || nLineThreshold >= 255)
		nLineThreshold = 5;

	if( nLineThickness <= 0 || nLineThickness >= 255)
		nLineThickness = 20;
	
	if(fLineLength <= 0 || fLineLength > 1)
		fLineLength = 0.3;

    //////////////////////////////////////////////////////////////////////////
	// 혼색 검출 알고리즘
	//////////////////////////////////////////////////////////////////////////

	cv::Mat matGrayBuf;
	cv::Mat matResBuf	= cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	cv::cvtColor(matSrcBuf, matGrayBuf, CV_RGB2GRAY);

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_HS_Src.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matSrcBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_HS_BK.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matBKBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_HS_Gray.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matGrayBuf );
	}

	cv::Mat matGauss;
	if( fSigma > 1.0 && nGBSize >= 3 )
		cv::GaussianBlur(matSrcBuf, matGauss, cv::Size(nGBSize, nGBSize), fSigma, fSigma);
	else
		matGauss = matSrcBuf.clone();

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_HS_Gauss.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matGauss );
	}

	

	cv::Mat matSrcROIBuf = matGauss( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matResROIBuf = matResBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );

	cv::Mat matRGBChannels[4];
	cv::split(matSrcROIBuf, matRGBChannels);

	cv::Mat matGrayChanels;
	cv::cvtColor(matSrcROIBuf, matGrayChanels, CV_RGB2GRAY);

	// SVI 컬러 보정 계수 가져오기
	double *dK = NULL;
	if( cColor != NULL )
		dK = cColor->GetColorCorrection();

	// 할당
	cv::Mat matSplitBuf[3], matXYBuf[2];
	matSplitBuf[0]	= cv::Mat::zeros(matSrcROIBuf.size(), CV_32FC1);
	matSplitBuf[1]	= cv::Mat::zeros(matSrcROIBuf.size(), CV_32FC1);
	matSplitBuf[2]	= cv::Mat::zeros(matSrcROIBuf.size(), CV_32FC1);
	matXYBuf[0]		= cv::Mat::zeros(matSrcROIBuf.size(), CV_32FC1);
	matXYBuf[1]		= cv::Mat::zeros(matSrcROIBuf.size(), CV_32FC1);

	// 영상 크기
	int nWidth	= matSrcROIBuf.cols;
	int nHeight	= matSrcROIBuf.rows;

	// 정상 범위 중심 좌표
	float fSumX	= 0;
	float fSumY	= 0;
	float fCnt	= 0;

	// 타원 영역 회전 각도 & 길이
	float fDegInRad = fAngle * DEGINRAD;
	float fSin		= sin(-fDegInRad);
	float fCos		= cos(-fDegInRad);
	float fLengthX	= (fAxisX * fAxisX / 4.0f);
	float fLengthY	= (fAxisY * fAxisY / 4.0f);
	float fSDLengthX	= (fSDAxisX * fSDAxisX / 4.0f);
	float fSDLengthY	= (fSDAxisY * fSDAxisY / 4.0f);
	float fTempX, fTempY, fRotX, fRotY, fLength;

	// 타원 각도
	long double dMatrixA[3]		= {0.0, 0.0, 0.0};
	long double dMatrixB[2]		= {0.0, 0.0};
	long double dOffsetAngle	= 0.0;

	for (int y=0; y<nHeight ; y++)
	{
		uchar* ptrB = (uchar *)matRGBChannels[0].ptr(y);	// B
		uchar* ptrG = (uchar *)matRGBChannels[1].ptr(y);	// G
		uchar* ptrR = (uchar *)matRGBChannels[2].ptr(y);	// R
		float* ptrX = (float *)matXYBuf[0].ptr(y);			// x
		float* ptrY = (float *)matXYBuf[1].ptr(y);			// y

		for (int x=0 ; x<nWidth ; x++, ptrR++, ptrG++, ptrB++, ptrX++, ptrY++)
		{
			//////////////////////////////////////////////////////////////////////////
			// RGB 보정
			//////////////////////////////////////////////////////////////////////////

			double dR=0.0, dG=0.0, dB=0.0;

			// 보정된 RGB
			if( dK != NULL )
			{
				dR = dK[0] * (*ptrR) + dK[1] * (*ptrG) + dK[2] * (*ptrB);
				dG = dK[3] * (*ptrR) + dK[4] * (*ptrG) + dK[5] * (*ptrB);
				dB = dK[6] * (*ptrR) + dK[7] * (*ptrG) + dK[8] * (*ptrB);
			}
			// 원본 그대로의 RGB
			else
			{
				dR = (*ptrR);
				dG = (*ptrG);
				dB = (*ptrB);
			}

			// 예외처리
			if( dR < 0 )	dR = 0.0;
			if( dG < 0 )	dG = 0.0;
			if( dB < 0 )	dB = 0.0;
			if( dR > 255 )	dR = 255.0;
			if( dG > 255 )	dG = 255.0;
			if( dB > 255 )	dB = 255.0;

			//////////////////////////////////////////////////////////////////////////
			// XYZ 변환
			//////////////////////////////////////////////////////////////////////////

			double tmpBGR[3], tmpXYZ[3];

			dR /= 255.0;
			if( dR > 0.04045 )	tmpBGR[2] = (std::pow( (dR + 0.055) / 1.055 , 2.4 ) * 100.0);
			else				tmpBGR[2] = (dR / 12.92 * 100.0);

			dG /= 255.0;
			if( dG > 0.04045 )	tmpBGR[1] = (std::pow( (dG + 0.055) / 1.055 , 2.4 ) * 100.0);
			else				tmpBGR[1] = (dG / 12.92 * 100.0);

			dB /= 255.0;
			if( dB > 0.04045 )	tmpBGR[0] = (std::pow( (dB + 0.055) / 1.055 , 2.4 ) * 100.0);
			else				tmpBGR[0] = (dB / 12.92 * 100.0);

			tmpXYZ[0] = ( (tmpBGR[2] * 0.4124) + (tmpBGR[1] * 0.3576) + (tmpBGR[0] * 0.1805) );		// X
			tmpXYZ[1] = ( (tmpBGR[2] * 0.2126) + (tmpBGR[1] * 0.7152) + (tmpBGR[0] * 0.0722) );		// Y
			tmpXYZ[2] = ( (tmpBGR[2] * 0.0193) + (tmpBGR[1] * 0.1192) + (tmpBGR[0] * 0.9505) );		// Z

			//////////////////////////////////////////////////////////////////////////
			// Yxy 변환
			//////////////////////////////////////////////////////////////////////////

			float fX = (float)( tmpXYZ[0] / ( tmpXYZ[0] + tmpXYZ[1] + tmpXYZ[2]) );	// x
			float fY = (float)( tmpXYZ[1] / ( tmpXYZ[0] + tmpXYZ[1] + tmpXYZ[2]) );	// y

			(*ptrX) = fX;
			(*ptrY) = fY;

			//////////////////////////////////////////////////////////////////////////
			// 설정된 타원 영역안에 존재 -> 중심좌표 구하기
			//////////////////////////////////////////////////////////////////////////			

			// 중심 이동
			fTempX = fX - fCenterX;
			fTempY = fY - fCenterY;

			// 회전
			fRotX = (fCos * fTempX) - (fSin * fTempY);
			fRotY = (fSin * fTempX) + (fCos * fTempY);

			// 정상 범위 영역 안에 존재하는지 확인
			//fTempX = (fRotX*fRotX) / fLengthX;
			//fTempY = (fRotY*fRotY) / fLengthY;

			fTempX = (fRotX*fRotX) / fSDLengthX;
			fTempY = (fRotY*fRotY) / fSDLengthY;

			// 정상이 아닌 경우 넘어가기
			if( fTempX + fTempY > 1.0 )	continue;

			// 각도 찾기
			dMatrixA[0] += (fX * fX);
			dMatrixA[1] += fX;
			dMatrixA[2] ++;
			dMatrixB[0] += (fX * fY);
			dMatrixB[1] += fY;
			
			// 좌표 추가
			fSumX += fX;
			fSumY += fY;
			fCnt++;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 타원 중심 보정
	//////////////////////////////////////////////////////////////////////////

	// 중심 보정 사용하는 경우
	if( fCenterR > 0.0f )
	{
		// 정상 범위 안에 1개라도 있어야 함
		if( fCnt > 0 )
		{
			fSumX /= fCnt;
			fSumY /= fCnt;

			// 중심으로부터 거리 확인
			fTempX = fSumX - fCenterX;
			fTempY = fSumY - fCenterY;
			fLength = sqrtf( fTempX*fTempX + fTempY*fTempY );

			// 타원 중심 이동
			// 중심이동 길이가 fCenterR 보다 길면, 최대치 fCenterR 로 변경 ( 각도 유지 )
			if( fLength > fCenterR )
			{
				float fTempAngle = atan2f( fTempY,  fTempX );

				fSumX = fCenterX + fCenterR * cos( fTempAngle );
				fSumY = fCenterY + fCenterR * sin( fTempAngle );
			}
		}
		// 1개도 없는 경우, ( fCenterX, fCenterY ) 좌표
		else
		{
			fSumX = fCenterX;
			fSumY = fCenterY;
		}
	}
	// 사용 안하는 경우, ( fCenterX, fCenterY ) 좌표
	else
	{
		fSumX = fCenterX;
		fSumY = fCenterY;
	}


	//////////////////////////////////////////////////////////////////////////
	// 타원 각도 보정
	//////////////////////////////////////////////////////////////////////////

	// 역행렬
	long double dDet = dMatrixA[0]*dMatrixA[2] - dMatrixA[1]*dMatrixA[1];

	// 역행렬 계산 불가 & 각도 보정 사용하는 경우
	if( dDet != 0 && fRangeA > 0.0f )
	{
		long double	dInvDet = 1./(long double)dDet;

		// y = dValue_A * x + dValue_B ( E_ALIGN_TYPE_TOP / E_ALIGN_TYPE_BOTTOM )
		long double dValueA = dInvDet * (dMatrixA[2] * dMatrixB[0] - dMatrixA[1] * dMatrixB[1]);
		long double dValueB = dInvDet * (-dMatrixA[1] * dMatrixB[0] + dMatrixA[0] * dMatrixB[1]);

		// 계산된 각도 확인
		dOffsetAngle = atan( dValueA ) * 180.0 / PI;
		if( fAngle > 0 && dOffsetAngle < 0 )	dOffsetAngle += 90.0;
		if( fAngle < 0 && dOffsetAngle > 0 )	dOffsetAngle -= 90.0;

		// 각도 범위 이내인 경우
		if( fabs(fAngle - dOffsetAngle) < fRangeA )
			fAngleAfter = (float)dOffsetAngle;
		else
		{
			if( fAngle < dOffsetAngle )	fAngleAfter = fAngle + fRangeA;
			else						fAngleAfter = fAngle - fRangeA;
		}
	}

	// 라인검출 ////////////////////////////////////////////////

	// 강한 불량 검출
	int		nStepX			= (int)dPara[E_PARA_SVI_MURA_EMD_STEP_X		];	//10;		// 가로 배경 스텝
	int		nStepY			= (int)dPara[E_PARA_SVI_MURA_EMD_STEP_Y		];	//10;		// 세로 배경 스텝
	
	int nSum, nSumCount;
	float fAverage = 0.0;

	int nStepCols = (int) (matSrcROIBuf.cols / nStepX);
	int nStepRows = (int) (matSrcROIBuf.rows / nStepY);


	cv::Mat matBGSizeDarkOrg[4];
	cv::Mat matBGSizeBrightOrg[4];

	matBGSizeDarkOrg[0] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeDarkOrg[1] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeDarkOrg[2] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeDarkOrg[3] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);

	matBGSizeBrightOrg[0] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeBrightOrg[1] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeBrightOrg[2] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeBrightOrg[3] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);

	cv::Mat matBGSizeDark[4];
	cv::Mat matBGSizeBright[4];

	//matBGSizeDark[0] = matBGSizeDarkOrg[0]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	//matBGSizeDark[1] = matBGSizeDarkOrg[1]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	//matBGSizeDark[2] = matBGSizeDarkOrg[2]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	//matBGSizeDark[3] = matBGSizeDarkOrg[3]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );

	//matBGSizeBright[0] = matBGSizeBrightOrg[0]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	//matBGSizeBright[1] = matBGSizeBrightOrg[1]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	//matBGSizeBright[2] = matBGSizeBrightOrg[2]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	//matBGSizeBright[3] = matBGSizeBrightOrg[3]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );


	matBGSizeDark[0] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeDark[1] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeDark[2] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeDark[3] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);

	matBGSizeBright[0] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeBright[1] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeBright[2] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);
	matBGSizeBright[3] = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);


	cv::Mat matLineMask = cv::Mat::zeros(matBGSizeDark[0].size(), CV_8UC1);

	int nTh = nLineThreshold;
	for(int c = 0 ; c < 4; c++)
	{
		//cv::Mat reSize = cv::Mat::zeros( nStepRows, nStepCols, matGauss.type());
		cv::Mat reSize = cv::Mat::zeros(nStepRows, nStepCols, CV_8UC1);

		for(int j = 0 ; j < nStepRows ; j ++)
		{
			for(int i = 0 ; i < nStepCols ; i ++)
			{
				nSum = 0;
				fAverage = 0;
				nSumCount = 0;

				for(int y  = (j * nStepY) ; y < (j * nStepY + nStepY) ; y ++)    
				{
					for(int x  = (i * nStepX) ; x <= (i * nStepX + nStepX) ; x ++)
					{

						if( y >= matResROIBuf.rows || x >= matResROIBuf.cols )
							continue;
						nSumCount ++;
					
						if(c == 0)
							nSum += matGrayChanels.at<uchar>(y, x);
						else if(c == 1)
							nSum += matRGBChannels[2].at<uchar>(y, x);
						else if(c == 2)
							nSum += matRGBChannels[1].at<uchar>(y, x);
						else
							nSum += matRGBChannels[0].at<uchar>(y, x);
					}
				}
			
				if(nSumCount > 0)
					fAverage = nSum / nSumCount;

				reSize.at<uchar>(j, i) = (uchar)fAverage;
			}
		}

		cv::medianBlur(reSize, reSize, 9);


		//cv::Mat matBGSize = cv::Mat::zeros( matGauss.rows, matGauss.cols, matGauss.type());
		cv::Mat matBGSize = cv::Mat::zeros(matSrcROIBuf.size(), CV_8UC1);

		for(int y  = 0 ; y < matResROIBuf.rows; y ++)
		{
			for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
			{
				if(x/nStepX >= nStepCols && y/nStepY >= nStepRows)
				{
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY) - 1, int(x/nStepX) - 1);
				}else if(x/nStepX >= nStepCols)
				{
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY), int(x/nStepX) - 1);				
				}else if(y/nStepY >= nStepRows)
				{
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY)  -1, int(x/nStepX));		
				}else{	
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY), int(x/nStepX));
				}
			}
		}

		// dark	
		if(c == 0){
			cv::subtract(matBGSize, matGrayChanels, matBGSizeDark[0]);
			cv::subtract(matGrayChanels, matBGSize, matBGSizeBright[0]);

			cv::threshold(matBGSizeDark[0], matBGSizeDark[0], nTh, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[0], matBGSizeBright[0], nTh, 255.0, CV_THRESH_BINARY);
		}else if(c == 1){
			cv::subtract(matBGSize, matRGBChannels[2], matBGSizeDark[1]);
			cv::subtract(matRGBChannels[2], matBGSize, matBGSizeBright[1]);

			cv::threshold(matBGSizeDark[1], matBGSizeDark[1], nTh, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[1], matBGSizeBright[1], nTh, 255.0, CV_THRESH_BINARY);
		}else if(c == 2){
			cv::subtract(matBGSize, matRGBChannels[1], matBGSizeDark[2]);
			cv::subtract(matRGBChannels[1], matBGSize, matBGSizeBright[2]);

			cv::threshold(matBGSizeDark[2], matBGSizeDark[2], nTh, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[2], matBGSizeBright[2], nTh, 255.0, CV_THRESH_BINARY);
		}else{
			cv::subtract(matBGSize, matRGBChannels[0], matBGSizeDark[3]);
			cv::subtract(matRGBChannels[0], matBGSize, matBGSizeBright[3]);

			cv::threshold(matBGSizeDark[3], matBGSizeDark[3], nTh, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[3], matBGSizeBright[3], nTh, 255.0, CV_THRESH_BINARY);	
		}

		cv::add(matLineMask, matBGSizeBright[c], matLineMask);
		cv::add(matLineMask, matBGSizeDark[c], matLineMask);
	}

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_HS_HARD_DEFECT.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matLineMask);
	}


	cv::dilate(matLineMask, matLineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)) );
	cv::erode(matLineMask, matLineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)) );
	
	cv::Mat matLineMaskWidth = cv::Mat::zeros(matLineMask.size(), CV_8UC1);
	cv::Mat matLineMaskHeight = cv::Mat::zeros(matLineMask.size(), CV_8UC1);

	int nProfile = 0;
	float fRateLine = 0.3;

	int* nRowProfile= (int*) calloc(matResROIBuf.rows,sizeof(int));
	int* nColProfile= (int*) calloc(matResROIBuf.cols,sizeof(int));

	int nBeforeLineIndex = 0;
	bool bLineBefore = 0;

	//// 가로 라인 검출  ////
	for(int y  = 0 ; y < matResROIBuf.rows; y ++)
	{
		nProfile =0;
		for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
		{
			 if(matLineMask.at<uchar>(y, x) > 0)
				 nProfile ++;
		}

		if(nProfile > (matResROIBuf.cols * fRateLine))
		{
			if(y == 0)
			{
				bLineBefore = true;
				nRowProfile[y] = 1;
			}

		//	for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
		//	{
		//		matLineMaskWidth.at<uchar>(y, x) = matLineMask.at<uchar>(y, x);
		//	}

			if(y == 0)
			{
				nRowProfile[y] = 1;
			}else{
				nRowProfile[y] = nRowProfile[y-1] + 1;
			}
		}
	}


	bLineBefore = false;
	for(int y  = 0 ; y < matResROIBuf.rows; y ++)
	{
		//첫 라인이 아닐때
		if(y == 0)
		{
			if(nRowProfile[y] > 0)
				bLineBefore = true;
			else
				bLineBefore = false;
		}
		
		//전에 라인이고 현재 라인이 아니라면때
		if(bLineBefore && y != 0)
		{
			// 현재가 라인이 아닐때
			if(nRowProfile[y] == 0)
			{
				if(nRowProfile[y-1] > nLineThickness)
				{
					for(int y1 = y - 1 ; nRowProfile[y1] > 0 ; y1 --)
					{
						if(y1 < 0)
							break;

						nRowProfile[y1] = 0;
					}
				}
			}
		}

		if(bLineBefore && y == (matResROIBuf.rows-1))
		{
			if(nRowProfile[y] > nLineThickness)
			{
				for(int y1 = y - 1 ; nRowProfile[y1] > 0 ; y1 --)
				{
					if(y1 < 0)
						break;

					nRowProfile[y1] = 0;
				}
			}
		}
		
		// 비교완료 후
		if(nRowProfile[y] > 0)
		{
			bLineBefore = true;
		}else{
			bLineBefore = false;
		}
	}

	for(int y  = 0 ; y < matResROIBuf.rows; y ++)
	{
		if(nRowProfile[y] == 0)
		{
			for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
			{
				matLineMaskWidth.at<uchar>(y, x) = 0;
			}
		}else{

			for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
			{
				matLineMaskWidth.at<uchar>(y, x) = matLineMask.at<uchar>(y, x);
			}
		}
	}


	//// 세로 라인 검출  ////
	for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
	{	
		nProfile =0;
		for(int y  = 0 ; y < matResROIBuf.rows; y ++)
		{
			if(matLineMask.at<uchar>(y, x) > 0)
				nProfile ++;
		}

		if(nProfile > (matResROIBuf.rows * fRateLine))
		{
			if(x == 0)
			{
				bLineBefore = true;
				nColProfile[x] = 1;
			}

		//	for(int y  = 0 ; y < matResROIBuf.rows; y ++)
		//	{
		//		matLineMaskHeight.at<uchar>(y, x) = matLineMask.at<uchar>(y, x);
		//	}

			if(x == 0)
			{
				nColProfile[x] = 1;
			}else{
				nColProfile[x] = nColProfile[x-1] + 1;
			}
		}else{
			nColProfile[x] = 0;
		}
	}

	bLineBefore = false;
	for(int x  = 0 ; x < matResROIBuf.cols; x ++)
	{
		//첫 라인이 아닐때
		if(x == 0)
		{
			if(nColProfile[x] > 0)
				bLineBefore = true;
			else
				bLineBefore = false;
		}
		
		//전 라인이 라인이 아닐일때
		if(bLineBefore && x != 0)
		{
			// 현재가 라인이 아닐때
			if(nColProfile[x] == 0)
			{
				if(nColProfile[x-1] > nLineThickness)
				{
					for(int x1 = x - 1 ; nColProfile[x1] > 0 ; x1 --)
					{
						if(x1 < 0)
							break;

						nColProfile[x1] = 0;
					}
				}
			}
		}


		if(bLineBefore && x == (matResROIBuf.cols-1))
		{
			if(nColProfile[x] > nLineThickness)
			{
				for(int x1 = x - 1 ; nColProfile[x1] > 0 ; x1 --)
				{
					if(x1 < 0)
						break;

					nColProfile[x1] = 0;
				}
			}
		}
				
		// 비교완료 후
		if(nColProfile[x] > 0)
		{
			bLineBefore = true;
		}else{
			bLineBefore = false;
		}		
	}

	for(int x  = 0 ; x < matResROIBuf.cols; x ++)
	{
		if(nColProfile[x] == 0)
		{
			for(int y  = 0 ; y < matResROIBuf.rows ; y ++)
			{
				matLineMaskHeight.at<uchar>(y, x) = 0;
			}
		}else{
			for(int y  = 0 ; y < matResROIBuf.rows ; y ++)
			{
				matLineMaskHeight.at<uchar>(y, x) = matLineMask.at<uchar>(y, x);
			}
		}
	}
	
	cv::add(matLineMaskHeight, matLineMaskWidth, matLineMask);

	//cv::dilate(matLineMask, matLineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5)) );
	//cv::erode(matLineMask, matLineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)) );


	free(nRowProfile);
	free(nColProfile);

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_HS_LineMask.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matLineMask);
	}
	// 라인검출 종료 /////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 보정된 정상 범위로 불량 찾기
	//////////////////////////////////////////////////////////////////////////

	// 보정된 각도 재계산
	fDegInRad = fAngleAfter * DEGINRAD;
	fSin = sin(-fDegInRad);
	fCos = cos(-fDegInRad);

	for (int y=0 ; y<nHeight ; y++)
	{
		float *ptrx = (float *)matXYBuf[0].ptr(y);	// x
		float *ptry = (float *)matXYBuf[1].ptr(y);	// y
		uchar* ptr	= (uchar *)matResROIBuf.ptr(y);	// Res
		uchar* ptrLine =  (uchar *)matLineMask.ptr(y);
		for (int x=0 ; x<nWidth ; x++, ptrx++, ptry++, ptr++, ptrLine++)
		{
			// 중심 이동
			fTempX = (*ptrx) - fSumX;
			fTempY = (*ptry) - fSumY;

			// 회전
			fRotX = (fCos * fTempX) - (fSin * fTempY);
			fRotY = (fSin * fTempX) + (fCos * fTempY);

			// 정상 범위 영역 안에 존재하는지 확인
			fTempX = (fRotX*fRotX) / fLengthX;
			fTempY = (fRotY*fRotY) / fLengthY;

			// 정상인경우 넘어가기
			if( fTempX + fTempY <= 1.0 )	continue;

			// 불량 이진화
			(*ptr) = (uchar)255;

			if( (*ptrLine) > 0) // 라인은 혼색에서 제외함
				(*ptr) = (uchar)0;
		}
	}

	// 점등 영역 불량만 남기기
	cv::subtract(matResBuf, matBKBuf, matResBuf);

	// 최 외곽선 제거
	if( nDeleteOutLine > 0 )
		cv::rectangle(matResBuf, cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()), cv::Scalar(0, 0, 0), nDeleteOutLine);

	

	if( bImageSave )
	{
		// 그래프 버퍼 생성
		cv::Mat matGraphBuf = cv::Mat::zeros( cv::Size(1100, 1100), CV_8UC3 );

		// 좌표계
		for (int i=0 ; i<=10 ; i++)
		{
			cv::line(matGraphBuf, cv::Point(50, i*100+50), cv::Point(1100-51, i*100+50), cv::Scalar(255, 255, 255), 1);
			cv::line(matGraphBuf, cv::Point(i*100+50, 50), cv::Point(i*100+50, 1100-51), cv::Scalar(255, 255, 255), 1);
		}

		// x, y 데이터
		cv::Vec3b color(128, 128, 128);
		for (int y=0; y<nHeight ; y++)
		{
			float* ptrX = (float *)matXYBuf[0].ptr(y);			// x
			float* ptrY = (float *)matXYBuf[1].ptr(y);			// y

			for (int x=0 ; x<nWidth ; x++, ptrX++, ptrY++)
			{
				int nIndexX = (int)( (*ptrX) * 1000 + 50 );
				int nIndexY = (int)( (*ptrY) * 1000 + 50 );

				// 예외처리
				if( nIndexX < 0	)		continue;
				if( nIndexY < 0	)		continue;
				if( nIndexX >= 1100	)	continue;
				if( nIndexY >= 1100	)	continue;

				matGraphBuf.at<cv::Vec3b>(nIndexY, nIndexX) = color;
			}
		}

		// 기존 정상 범위 타원
		int nSizeXE		= (int)(fAxisX * 1000);
		int nSizeYE		= (int)(fAxisY * 1000);
		int nSDSizeXE		= (int)(fSDAxisX * 1000);
		int nSDSizeYE		= (int)(fSDAxisY * 1000);
		int nCenterXE	= (int)(fCenterX * 1000) + 50;
		int nCenterYE	= (int)(fCenterY * 1000) + 50;

		cv::RotatedRect rectE(cv::Point(nCenterXE, nCenterYE), cv::Size(nSizeXE, nSizeYE),  fAngle);
		cv::ellipse(matGraphBuf, rectE, cv::Scalar(255, 0, 255), 1);
		
		cv::RotatedRect SDrectE(cv::Point(nCenterXE, nCenterYE), cv::Size(nSDSizeXE, nSDSizeYE),  fAngle);
		cv::ellipse(matGraphBuf, SDrectE, cv::Scalar(0, 255, 255), 1);

		// 초기 중심 이동 범위
		cv::circle(matGraphBuf, cv::Point(nCenterXE, nCenterYE), (int)(fCenterR * 1000), cv::Scalar(255, 0, 0), 1);
		cv::circle(matGraphBuf, cv::Point(nCenterXE, nCenterYE), 2, cv::Scalar(255, 0, 255), -1);

		// 보정 중심 이동 위치
		nCenterXE	= (int)(fSumX * 1000) + 50;
		nCenterYE	= (int)(fSumY * 1000) + 50;
		cv::circle(matGraphBuf, cv::Point(nCenterXE, nCenterYE), 2, cv::Scalar(0, 255, 0), -1);

		// 보정 정상 범위 타원
		rectE = cv::RotatedRect(cv::Point(nCenterXE, nCenterYE), cv::Size(nSizeXE, nSizeYE),  fAngleAfter);
		cv::ellipse(matGraphBuf, rectE, cv::Scalar(0, 255, 0), 1);

	


		// y반전
		cv::flip(matGraphBuf, matGraphBuf, 0);

		// 중간 Image 확인
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_HS_Graph.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matGraphBuf );

		// 그래프 버퍼 해제
		matGraphBuf.release();

		// 정상 범위 벗어난 결과
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_HS_Res.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matResBuf );
	}

	// R,G,B 점등패턴의 그래프까지만 추출함
	//if( nImageNum  == E_IMAGE_CLASSIFY_SVI_R ||  nImageNum  == E_IMAGE_CLASSIFY_SVI_G || E_IMAGE_CLASSIFY_SVI_B )
	//{
	//	if( !matRGBChannels[0].empty() )	matRGBChannels[0].release();
	//	if( !matRGBChannels[1].empty() )	matRGBChannels[1].release();
	//	if( !matRGBChannels[2].empty() )	matRGBChannels[2].release();
	//	if( !matRGBChannels[3].empty() )	matRGBChannels[3].release();
	//	if( !matSplitBuf[0].empty() )		matSplitBuf[0].release();
	//	if( !matSplitBuf[1].empty() )		matSplitBuf[1].release();
	//	if( !matSplitBuf[2].empty() )		matSplitBuf[2].release();
	//	if( !matXYBuf[0].empty() )			matXYBuf[0].release();
	//	if( !matXYBuf[1].empty() )			matXYBuf[1].release();
	//	if( !matSrcROIBuf.empty() )			matSrcROIBuf.release();
	//	if( !matResROIBuf.empty() )			matResROIBuf.release();
	//	if( !matGrayBuf.empty() )			matGrayBuf.release();
	//	if( !matResBuf.empty() )			matResBuf.release();

	//	return E_ERROR_CODE_TRUE;
	//}


	//////////////////////////////////////////////////////////////////////////
	// 결과
	//////////////////////////////////////////////////////////////////////////	

	// 라벨
	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem( NULL );

	//// HS 결과
	nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matGrayBuf, matResBuf, matDrawBuf, 
		nCommonPara, E_DEFECT_COLOR_DARK, _T("HS_"), EngineerDefectJudgment, pResultBlob);

	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		// 메모리 해제
		if( !matSrcROIBuf.empty() )	matSrcROIBuf.release();
		if( !matDrawBuf.empty() )	matDrawBuf.release();
		if( !matResBuf.empty() )	matResBuf.release();
		if( !matGrayBuf.empty() )	matGrayBuf.release();

		return nErrorCode;
	}

	// 결과 외곽선 그리기 ( Light SkyBlue )
	cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

	// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
	// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
	if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(nCommonPara, _strPath[1], true);

	//////////////////////////////////////////////////////////////////////////
	// 할당 해제
	//////////////////////////////////////////////////////////////////////////

	if( !matRGBChannels[0].empty() )	matRGBChannels[0].release();
	if( !matRGBChannels[1].empty() )	matRGBChannels[1].release();
	if( !matRGBChannels[2].empty() )	matRGBChannels[2].release();
	if( !matRGBChannels[3].empty() )	matRGBChannels[3].release();
	if( !matSplitBuf[0].empty() )		matSplitBuf[0].release();
	if( !matSplitBuf[1].empty() )		matSplitBuf[1].release();
	if( !matSplitBuf[2].empty() )		matSplitBuf[2].release();
	if( !matXYBuf[0].empty() )			matXYBuf[0].release();
	if( !matXYBuf[1].empty() )			matXYBuf[1].release();
	if( !matSrcROIBuf.empty() )			matSrcROIBuf.release();
	if( !matResROIBuf.empty() )			matResROIBuf.release();
	if( !matGrayBuf.empty() )			matGrayBuf.release();
	if( !matResBuf.empty() )			matResBuf.release();

	return E_ERROR_CODE_TRUE;
}


// EMD 불량 검출 로직
/*
long CInspectSVI::LogicStart_EMD(cv::Mat& matSrcBuf, cv::Mat &matBKBuf, cv::Mat& matDrawBuf, CRect rectROI, double* dPara, int* nCommonPara, 
	wchar_t _strPath[][1024], STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if( matSrcBuf.empty() )	return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 파라미터 설정
	//////////////////////////////////////////////////////////////////////////

	int		nGBSize			= (int)dPara[E_PARA_SVI_MURA_EMD_GB_SIZE	];	//31;		// 가우시안 마스크 크기
	float	fSigma			= (float)dPara[E_PARA_SVI_MURA_EMD_GB_SIGMA	];	//3.0f;		// 가우시안 시그마
	int		nThreshold		= (int)dPara[E_PARA_SVI_MURA_EMD_THRESHOLD	];	//5;		// EMD 불량 이진화
	int		nDimensionX		= (int)dPara[E_PARA_SVI_MURA_EMD_DIM_X		];	//2;		// 가로 배경 만들기
	int		nDimensionY		= (int)dPara[E_PARA_SVI_MURA_EMD_DIM_Y		];	//2;		// 세로 배경 만들기
	int		nStepX			= (int)dPara[E_PARA_SVI_MURA_EMD_STEP_X		];	//10;		// 가로 배경 스텝
	int		nStepY			= (int)dPara[E_PARA_SVI_MURA_EMD_STEP_Y		];	//10;		// 세로 배경 스텝

	long	nDeleteOutLine	= (int)dPara[E_PARA_SVI_MURA_DELETE_PIXEL	];	//0;		// 최 외곽 불량 제거
	float   fThBGOffset		= (float)dPara[E_PARA_SVI_MURA_EMD_GB_AV_OFFSET	];	//      // 배경	설정 OFFSET

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
	int		nRatio				=  nCommonPara[E_PARA_COMMON_PS_MODE				];


	//////////////////////////////////////////////////////////////////////////
	// EMD 검출 알고리즘
	//////////////////////////////////////////////////////////////////////////

	cv::Mat matGrayBuf;

	cv::Mat matRGBChannels[4];
	cv::split(matSrcBuf, matRGBChannels);

	switch ( nImageNum )
	{
	case E_IMAGE_CLASSIFY_SVI_R:
		matGrayBuf = matRGBChannels[2];
		break;

	case E_IMAGE_CLASSIFY_SVI_G:
		matGrayBuf = matRGBChannels[1];
		break;

	case E_IMAGE_CLASSIFY_SVI_B:
		matGrayBuf = matRGBChannels[0];
		break;

	default:
		cv::cvtColor(matSrcBuf, matGrayBuf, CV_RGB2GRAY);
		break;
	}

	// 버퍼가 없는 경우
	if( matGrayBuf.empty() )	return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat matResBuf	= cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	cv::Mat matSrcROIBuf = matSrcBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matResROIBuf = matResBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matGrayROIBuf= matGrayBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Src.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matSrcBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_BK.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matBKBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Gray.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matGrayROIBuf );
	}

	cv::Mat matGauss;
	if( fSigma > 1.0 && nGBSize >= 3 )
		cv::GaussianBlur(matGrayROIBuf, matGauss, cv::Size(nGBSize, nGBSize), fSigma, fSigma);
	else
		matGauss = matGrayROIBuf.clone();

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Gauss.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matGauss );
	}

	cv::Mat matEstim;
	if( !Estimation_XY(matGauss, matEstim, nDimensionX, nDimensionY, nStepX, nStepY, fThBGOffset) )
	{
		// 메모리 해제
		if( !matGauss.empty() )			matGauss.release();
		if( !matEstim.empty() )			matEstim.release();
		if( !matSrcROIBuf.empty() )		matSrcROIBuf.release();
		if( !matResROIBuf.empty() )		matResROIBuf.release();
		if( !matGrayROIBuf.empty() )	matGrayROIBuf.release();
		if( !matGrayBuf.empty() )		matGrayBuf.release();
		if( !matResBuf.empty() )		matResBuf.release();

		return E_ERROR_CODE_SVI_EMD_ESTMATION;
	}

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Estim.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matEstim );
	}

	cv::Mat matSub;
	cv::subtract(matEstim, matGauss, matSub);

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Sub.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matSub );
	}
	
	cv::threshold(matSub, matResROIBuf, nThreshold, 255.0, CV_THRESH_BINARY);

	// 점등 영역 불량만 남기기
	cv::subtract(matResBuf, matBKBuf, matResBuf);

	// 최 외곽선 제거
	if( nDeleteOutLine > 0 )
		cv::rectangle(matResBuf, cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()), cv::Scalar(0, 0, 0), nDeleteOutLine);

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Res.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matResBuf );
	}

	//////////////////////////////////////////////////////////////////////////
	// 결과
	//////////////////////////////////////////////////////////////////////////

	// 라벨
	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem( NULL );

	// EMD 결과
	nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matGrayBuf, matResBuf, matDrawBuf, 
		nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		// 메모리 해제
		if( !matGauss.empty() )			matGauss.release();
		if( !matEstim.empty() )			matEstim.release();
		if( !matSub.empty() )			matSub.release();
		if( !matSrcROIBuf.empty() )		matSrcROIBuf.release();
		if( !matResROIBuf.empty() )		matResROIBuf.release();
		if( !matGrayROIBuf.empty() )	matGrayROIBuf.release();
		if( !matGrayBuf.empty() )		matGrayBuf.release();
		if( !matResBuf.empty() )		matResBuf.release();

		return nErrorCode;
	}

	// 결과 외곽선 그리기 ( Light SkyBlue )
	cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

	// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
	// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
	if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(nCommonPara, _strPath[1], true);

	//////////////////////////////////////////////////////////////////////////
	// 할당 해제
	//////////////////////////////////////////////////////////////////////////

	if( !matGauss.empty() )			matGauss.release();
	if( !matEstim.empty() )			matEstim.release();
	if( !matSub.empty() )			matSub.release();
	if( !matSrcROIBuf.empty() )		matSrcROIBuf.release();
	if( !matResROIBuf.empty() )		matResROIBuf.release();
	if( !matGrayROIBuf.empty() )	matGrayROIBuf.release();
	if( !matGrayBuf.empty() )		matGrayBuf.release();
	if( !matResBuf.empty() )		matResBuf.release();

	// 코너에 존재하는 경우, 제외...
	JudgementArea(pResultBlob, rectROI, dPara);

	return E_ERROR_CODE_TRUE;
}*/

// 2018.3.24 kjb
long CInspectSVI::LogicStart_EMD(cv::Mat& matSrcBuf, cv::Mat &matBKBuf, cv::Mat& matDrawBuf, CRect rectROI, double* dPara, int* nCommonPara, 
	wchar_t _strPath[][1024], STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob)
{
	// 에러 코드
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if( matSrcBuf.empty() )	return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 파라미터 설정
	//////////////////////////////////////////////////////////////////////////

	int		nGBSize			= (int)dPara[E_PARA_SVI_MURA_EMD_GB_SIZE	];	//31;		// 가우시안 마스크 크기
	float	fSigma			= (float)dPara[E_PARA_SVI_MURA_EMD_GB_SIGMA	];	//3.0f;		// 가우시안 시그마
	int		nThreshold		= (int)dPara[E_PARA_SVI_MURA_EMD_THRESHOLD	];	//5;		// EMD 불량 이진화
	int		nDimensionX		= (int)dPara[E_PARA_SVI_MURA_EMD_DIM_X		];	//2;		// 가로 배경 만들기
	int		nDimensionY		= (int)dPara[E_PARA_SVI_MURA_EMD_DIM_Y		];	//2;		// 세로 배경 만들기
	int		nStepX			= (int)dPara[E_PARA_SVI_MURA_EMD_STEP_X		];	//10;		// 가로 배경 스텝
	int		nStepY			= (int)dPara[E_PARA_SVI_MURA_EMD_STEP_Y		];	//10;		// 세로 배경 스텝

	long	nDeleteOutLine	= (int)dPara[E_PARA_SVI_MURA_DELETE_PIXEL	];	//0;		// 최 외곽 불량 제거
	float   fThBGOffset		= (float)dPara[E_PARA_SVI_MURA_EMD_GB_AV_OFFSET	];	//      // 배경	설정 OFFSET
	
	int		nLeftPiexl	= (int)dPara[E_PARA_SVI_MURA_EMD_LEFT_PIXEL		];			// 영상 왼쪽 Threshold 구분지역
	int		nLeftThreshold	= (int)dPara[E_PARA_SVI_MURA_EMD_LEFT_THRESHOLD		];	// 영상 왼쪽 Threshold 구분지역

	int		nEdgePiexl	= (int)dPara[E_PARA_SVI_MURA_EMD_EDGE_PIXEL		];			// EDGE 구분지역
	int		nEdgeThreshold	= (int)dPara[E_PARA_SVI_MURA_EMD_EDGE_THRESHOLD		];	// EDGE Threshold



	
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
	int		nRatio				=  nCommonPara[E_PARA_COMMON_PS_MODE				];

	int		nLineThreshold		=  (int)dPara[E_PARA_SVI_LINE_TH				];
	int		nLineThickness		=  (int)dPara[E_PARA_SVI_LINE_THICKNESS			];
	float	fLineLength		=  (float)dPara[E_PARA_SVI_LINE_LENGTH			];

	if( nLineThreshold <= 0 || nLineThreshold >= 255)
		nLineThreshold = 5;

	if( nLineThickness <= 0 || nLineThickness >= 255)
		nLineThickness = 20;

	if(fLineLength <= 0 || fLineLength > 1)
		fLineLength = 0.3;
	
	//////////////////////////////////////////////////////////////////////////
	// EMD 검출 알고리즘
	//////////////////////////////////////////////////////////////////////////

	cv::Mat matGrayBuf;
	cv::Mat matRedBuf;
	cv::Mat matGreenBuf;
	cv::Mat matBlueBuf;

	cv::Mat matRGBChannels[4];
	cv::split(matSrcBuf, matRGBChannels);


	matRedBuf = matRGBChannels[2];
	matGreenBuf = matRGBChannels[1];
	matBlueBuf = matRGBChannels[0];
	cv::cvtColor(matSrcBuf, matGrayBuf, CV_RGB2GRAY);

	// 버퍼가 없는 경우
	if( matGrayBuf.empty() || matRedBuf.empty() || matGreenBuf.empty() || matBlueBuf.empty() )	
		return E_ERROR_CODE_EMPTY_BUFFER;

	cv::Mat matResBuf	= cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	cv::Mat matResRedBuf	= cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	cv::Mat matResGreenBuf	= cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	cv::Mat matResBlueBuf	= cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);

	cv::Mat matSrcROIBuf = matSrcBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matResROIBuf = matResBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matResRedROIBuf = matResRedBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matResGreenROIBuf = matResGreenBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matResBlueROIBuf = matResBlueBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );

	cv::Mat matGrayROIBuf= matGrayBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matRedROIBuf= matRedBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matGreenROIBuf= matGreenBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matBlueROIBuf= matBlueBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );


	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Src.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matSrcBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_BK.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matBKBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Gray.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matGrayROIBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Red.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matRedBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Green.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matGreenBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Blue.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matBlueBuf );
	}




	cv::Mat matGauss;
	cv::Mat matRedGauss;
	cv::Mat matGreenGauss;
	cv::Mat matBlueGauss;

	cv::Mat matGaussTemp;
	cv::Mat matRedGaussTemp;
	cv::Mat matGreenGaussTemp;
	cv::Mat matBlueGaussTemp;


	if( fSigma > 1.0 && nGBSize >= 3 )
	{
		cv::GaussianBlur(matGrayROIBuf, matGauss, cv::Size(nGBSize, nGBSize), fSigma, fSigma);
		cv::GaussianBlur(matRedROIBuf, matRedGauss, cv::Size(nGBSize, nGBSize), fSigma, fSigma);
		cv::GaussianBlur(matGreenROIBuf, matGreenGauss, cv::Size(nGBSize, nGBSize), fSigma, fSigma);
		cv::GaussianBlur(matBlueROIBuf, matBlueGauss, cv::Size(nGBSize, nGBSize), fSigma, fSigma);
	}else{
		matGauss = matGrayROIBuf.clone();
		matRedGauss = matRedROIBuf.clone();
		matGreenGauss = matGreenROIBuf.clone();
		matBlueGauss = matBlueROIBuf.clone();
	}

	matGaussTemp = matGauss.clone();
	matRedGaussTemp = matRedGauss.clone();
	matGreenGaussTemp = matGreenGauss.clone();
	matBlueGaussTemp = matBlueGauss.clone();


	// 라인검출 ////////////////////////////////////////////////
	// 강한 불량 검출
	int nSum, nSumCount;
	float fAverage = 0.0;

	int nStepCols = (int) (matGauss.cols / nStepX);
	int nStepRows = (int) (matGauss.rows / nStepY);

	// 라벨
	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem( NULL );


	cv::Mat matBGSizeDarkOrg[4];
	cv::Mat matBGSizeBrightOrg[4];

	matBGSizeDarkOrg[0] = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	matBGSizeDarkOrg[1] = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	matBGSizeDarkOrg[2] = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	matBGSizeDarkOrg[3] = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);

	matBGSizeBrightOrg[0] = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	matBGSizeBrightOrg[1] = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	matBGSizeBrightOrg[2] = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	matBGSizeBrightOrg[3] = cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);

	cv::Mat matBGSizeDark[4];
	cv::Mat matBGSizeBright[4];

	matBGSizeDark[0] = matBGSizeDarkOrg[0]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	matBGSizeDark[1] = matBGSizeDarkOrg[1]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	matBGSizeDark[2] = matBGSizeDarkOrg[2]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	matBGSizeDark[3] = matBGSizeDarkOrg[3]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );

	matBGSizeBright[0] = matBGSizeBrightOrg[0]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	matBGSizeBright[1] = matBGSizeBrightOrg[1]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	matBGSizeBright[2] = matBGSizeBrightOrg[2]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	matBGSizeBright[3] = matBGSizeBrightOrg[3]( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );

	cv::Mat matLineMask = cv::Mat::zeros(matBGSizeDark[0].size(), CV_8UC1);
	

	int nTh = 3;
	for(int c = 0 ; c < 4; c++)
	{
		cv::Mat reSize = cv::Mat::zeros( nStepRows, nStepCols, matGauss.type());

		for(int j = 0 ; j < nStepRows ; j ++)
		{
			for(int i = 0 ; i < nStepCols ; i ++)
			{
				nSum = 0;
				fAverage = 0;
				nSumCount = 0;

				for(int y  = (j * nStepY) ; y < (j * nStepY + nStepY) ; y ++)
				{
					for(int x  = (i * nStepX) ; x <= (i * nStepX + nStepX) ; x ++)
					{

						if( y >= matResROIBuf.rows || x >= matResROIBuf.cols )
							continue;
						nSumCount ++;
					
						if(c == 0)
							nSum += matGauss.at<uchar>(y, x);
						else if(c == 1)
							nSum += matRedGauss.at<uchar>(y, x);
						else if(c == 2)
							nSum += matGreenGauss.at<uchar>(y, x);
						else
							nSum += matBlueGauss.at<uchar>(y, x);
					}
				}
			
				if(nSumCount > 0)
					fAverage = nSum / nSumCount;

				reSize.at<uchar>(j, i) = (uchar)fAverage;
			}
		}

		cv::medianBlur(reSize, reSize, 9);


		cv::Mat matBGSize = cv::Mat::zeros( matGauss.rows, matGauss.cols, matGauss.type());

		for(int y  = 0 ; y < matResROIBuf.rows; y ++)
		{
			for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
			{
				if(x/nStepX >= nStepCols && y/nStepY >= nStepRows)
				{
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY) - 1, int(x/nStepX) - 1);
				}else if(x/nStepX >= nStepCols)
				{
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY), int(x/nStepX) - 1);				
				}else if(y/nStepY >= nStepRows)
				{
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY)  -1, int(x/nStepX));		
				}else{	
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY), int(x/nStepX));
				}
			}
		}

		// dark		
		if(c == 0){
			cv::subtract(matBGSize, matGaussTemp, matBGSizeDark[0]);
			cv::subtract(matGaussTemp, matBGSize, matBGSizeBright[0]);

			cv::threshold(matBGSizeDark[0], matBGSizeDark[0], nLineThreshold, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[0], matBGSizeBright[0], nLineThreshold, 255.0, CV_THRESH_BINARY);
		}else if(c == 1){
			cv::subtract(matBGSize, matRedGaussTemp, matBGSizeDark[1]);
			cv::subtract(matRedGaussTemp, matBGSize, matBGSizeBright[1]);

			cv::threshold(matBGSizeDark[1], matBGSizeDark[1], nLineThreshold, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[1], matBGSizeBright[1], nLineThreshold, 255.0, CV_THRESH_BINARY);
		}else if(c == 2){
			cv::subtract(matBGSize, matGreenGaussTemp, matBGSizeDark[2]);
			cv::subtract(matGreenGaussTemp, matBGSize, matBGSizeBright[2]);

			cv::threshold(matBGSizeDark[2], matBGSizeDark[2], nLineThreshold, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[2], matBGSizeBright[2], nLineThreshold, 255.0, CV_THRESH_BINARY);
		}else{
			cv::subtract(matBGSize, matBlueGaussTemp, matBGSizeDark[3]);
			cv::subtract(matBlueGaussTemp, matBGSize, matBGSizeBright[3]);

			cv::threshold(matBGSizeDark[3], matBGSizeDark[3], nLineThreshold, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[3], matBGSizeBright[3], nLineThreshold, 255.0, CV_THRESH_BINARY);	
		}

		cv::add(matLineMask, matBGSizeBright[c], matLineMask);
		cv::add(matLineMask, matBGSizeDark[c], matLineMask);

	}

	cv::dilate(matLineMask, matLineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)) );
	cv::erode(matLineMask, matLineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)) );

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_HARD_DEFECT.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matLineMask);
	}
	

	int nProfile = 0;
	float fRateLine = 0.3;

	cv::Mat matLineMaskWidth = cv::Mat::zeros(matLineMask.size(), CV_8UC1);
	cv::Mat matLineMaskHeight = cv::Mat::zeros(matLineMask.size(), CV_8UC1);

	int* nRowProfile= (int*) calloc(matResROIBuf.rows,sizeof(int));
	int* nColProfile= (int*) calloc(matResROIBuf.cols,sizeof(int));

	int nBeforeLineIndex = 0;
	bool bLineBefore = 0;

	//// 가로 라인 검출  ////
	for(int y  = 0 ; y < matResROIBuf.rows; y ++)
	{
		nProfile =0;
		for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
		{
			if(matLineMask.at<uchar>(y, x) > 0)
				nProfile ++;
		}

		if(nProfile > (matResROIBuf.cols * fRateLine))
		{
			if(y == 0)
			{
				bLineBefore = true;
				nRowProfile[y] = 1;
			}

			//	for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
			//	{
			//		matLineMaskWidth.at<uchar>(y, x) = matLineMask.at<uchar>(y, x);
			//	}

			if(y == 0)
			{
				nRowProfile[y] = 1;
			}else{
				nRowProfile[y] = nRowProfile[y-1] + 1;
			}
		}
	}


	bLineBefore = false;
	for(int y  = 0 ; y < matResROIBuf.rows; y ++)
	{
		//첫 라인이 아닐때
		if(y == 0)
		{
			if(nRowProfile[y] > 0)
				bLineBefore = true;
			else
				bLineBefore = false;
		}

		//전에 라인이고 현재 라인이 아니라면때
		if(bLineBefore && y != 0)
		{
			// 현재가 라인이 아닐때
			if(nRowProfile[y] == 0)
			{
				if(nRowProfile[y-1] > nLineThickness)
				{
					for(int y1 = y - 1 ; nRowProfile[y1] > 0 ; y1 --)
					{
						if(y1 < 0)
							break;

						nRowProfile[y1] = 0;
					}
				}
			}
		}

		if(bLineBefore && y == (matResROIBuf.rows-1))
		{
			if(nRowProfile[y] > nLineThickness)
			{
				for(int y1 = y - 1 ; nRowProfile[y1] > 0 ; y1 --)
				{
					if(y1 < 0)
						break;

					nRowProfile[y1] = 0;
				}
			}
		}

		// 비교완료 후
		if(nRowProfile[y] > 0)
		{
			bLineBefore = true;
		}else{
			bLineBefore = false;
		}
	}

	for(int y  = 0 ; y < matResROIBuf.rows; y ++)
	{
		if(nRowProfile[y] == 0)
		{
			for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
			{
				matLineMaskWidth.at<uchar>(y, x) = 0;
			}
		}else{

			for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
			{
				matLineMaskWidth.at<uchar>(y, x) = matLineMask.at<uchar>(y, x);
			}
		}
	}


	//// 세로 라인 검출  ////
	for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
	{	
		nProfile =0;
		for(int y  = 0 ; y < matResROIBuf.rows; y ++)
		{
			if(matLineMask.at<uchar>(y, x) > 0)
				nProfile ++;
		}

		if(nProfile > (matResROIBuf.rows * fRateLine))
		{
			if(x == 0)
			{
				bLineBefore = true;
				nColProfile[x] = 1;
			}

			//	for(int y  = 0 ; y < matResROIBuf.rows; y ++)
			//	{
			//		matLineMaskHeight.at<uchar>(y, x) = matLineMask.at<uchar>(y, x);
			//	}

			if(x == 0)
			{
				nColProfile[x] = 1;
			}else{
				nColProfile[x] = nColProfile[x-1] + 1;
			}
		}else{
			nColProfile[x] = 0;
		}
	}

	bLineBefore = false;
	for(int x  = 0 ; x < matResROIBuf.cols; x ++)
	{
		//첫 라인이 아닐때
		if(x == 0)
		{
			if(nColProfile[x] > 0)
				bLineBefore = true;
			else
				bLineBefore = false;
		}

		//전 라인이 라인이 아닐일때
		if(bLineBefore && x != 0)
		{
			// 현재가 라인이 아닐때
			if(nColProfile[x] == 0)
			{
				if(nColProfile[x-1] > nLineThickness)
				{
					for(int x1 = x - 1 ; nColProfile[x1] > 0 ; x1 --)
					{
						if(x1 < 0)
							break;

						nColProfile[x1] = 0;
					}
				}
			}
		}


		if(bLineBefore && x == (matResROIBuf.cols-1))
		{
			if(nColProfile[x] > nLineThickness)
			{
				for(int x1 = x - 1 ; nColProfile[x1] > 0 ; x1 --)
				{
					if(x1 < 0)
						break;

					nColProfile[x1] = 0;
				}
			}
		}

		// 비교완료 후
		if(nColProfile[x] > 0)
		{
			bLineBefore = true;
		}else{
			bLineBefore = false;
		}		
	}

	for(int x  = 0 ; x < matResROIBuf.cols; x ++)
	{
		if(nColProfile[x] == 0)
		{
			for(int y  = 0 ; y < matResROIBuf.rows ; y ++)
			{
				matLineMaskHeight.at<uchar>(y, x) = 0;
			}
		}else{
			for(int y  = 0 ; y < matResROIBuf.rows ; y ++)
			{
				matLineMaskHeight.at<uchar>(y, x) = matLineMask.at<uchar>(y, x);
			}
		}
	}

	cv::add(matLineMaskHeight, matLineMaskWidth, matLineMask);

	//cv::dilate(matLineMask, matLineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5)) );
	//cv::erode(matLineMask, matLineMask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)) );


	free(nRowProfile);
	free(nColProfile);

	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_LineMask.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matLineMask);
	}
	// 라인검출 종료 /////////////////////////////////////////////////////////////


	// 강 입계값 배경검출 제거함
	nTh = 30;

	matBGSizeDark[0].zeros(matLineMask.size(),CV_8UC1);
	matBGSizeDark[1].zeros(matLineMask.size(),CV_8UC1);
	matBGSizeDark[2].zeros(matLineMask.size(),CV_8UC1);
	matBGSizeDark[3].zeros(matLineMask.size(),CV_8UC1);

	matBGSizeBright[0].zeros(matLineMask.size(),CV_8UC1);
	matBGSizeBright[1].zeros(matLineMask.size(),CV_8UC1);
	matBGSizeBright[2].zeros(matLineMask.size(),CV_8UC1);
	matBGSizeBright[3].zeros(matLineMask.size(),CV_8UC1);

	for(int c = 0 ; c < 4; c++)
	{
		cv::Mat reSize = cv::Mat::zeros( nStepRows, nStepCols, matGauss.type());

		for(int j = 0 ; j < nStepRows ; j ++)
		{
			for(int i = 0 ; i < nStepCols ; i ++)
			{
				nSum = 0;
				fAverage = 0;
				nSumCount = 0;

				for(int y  = (j * nStepY) ; y < (j * nStepY + nStepY) ; y ++)
				{
					for(int x  = (i * nStepX) ; x <= (i * nStepX + nStepX) ; x ++)
					{

						if( y >= matResROIBuf.rows || x >= matResROIBuf.cols )
							continue;
						nSumCount ++;
					
						if(c == 0)
							nSum += matGauss.at<uchar>(y, x);
						else if(c == 1)
							nSum += matRedGauss.at<uchar>(y, x);
						else if(c == 2)
							nSum += matGreenGauss.at<uchar>(y, x);
						else
							nSum += matBlueGauss.at<uchar>(y, x);
					}
				}
			
				if(nSumCount > 0)
					fAverage = nSum / nSumCount;

				reSize.at<uchar>(j, i) = (uchar)fAverage;
			}
		}

		cv::medianBlur(reSize, reSize, 9);


		cv::Mat matBGSize = cv::Mat::zeros( matGauss.rows, matGauss.cols, matGauss.type());

		for(int y  = 0 ; y < matResROIBuf.rows; y ++)
		{
			for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
			{
				if(x/nStepX >= nStepCols && y/nStepY >= nStepRows)
				{
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY) - 1, int(x/nStepX) - 1);
				}else if(x/nStepX >= nStepCols)
				{
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY), int(x/nStepX) - 1);				
				}else if(y/nStepY >= nStepRows)
				{
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY)  -1, int(x/nStepX));		
				}else{	
					matBGSize.at<uchar>(y, x) = reSize.at<uchar>(int(y/nStepY), int(x/nStepX));
				}
			}
		}

		// dark		
		if(c == 0){
			cv::subtract(matBGSize, matGaussTemp, matBGSizeDark[0]);
			cv::subtract(matGaussTemp, matBGSize, matBGSizeBright[0]);

			cv::threshold(matBGSizeDark[0], matBGSizeDark[0], nTh, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[0], matBGSizeBright[0], nTh, 255.0, CV_THRESH_BINARY);
		}else if(c == 1){
			cv::subtract(matBGSize, matRedGaussTemp, matBGSizeDark[1]);
			cv::subtract(matRedGaussTemp, matBGSize, matBGSizeBright[1]);

			cv::threshold(matBGSizeDark[1], matBGSizeDark[1], nTh, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[1], matBGSizeBright[1], nTh, 255.0, CV_THRESH_BINARY);
		}else if(c == 2){
			cv::subtract(matBGSize, matGreenGaussTemp, matBGSizeDark[2]);
			cv::subtract(matGreenGaussTemp, matBGSize, matBGSizeBright[2]);

			cv::threshold(matBGSizeDark[2], matBGSizeDark[2], nTh, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[2], matBGSizeBright[2], nTh, 255.0, CV_THRESH_BINARY);
		}else{
			cv::subtract(matBGSize, matBlueGaussTemp, matBGSizeDark[3]);
			cv::subtract(matBlueGaussTemp, matBGSize, matBGSizeBright[3]);

			cv::threshold(matBGSizeDark[3], matBGSizeDark[3], nTh, 255.0, CV_THRESH_BINARY);
			cv::threshold(matBGSizeBright[3], matBGSizeBright[3], nTh, 255.0, CV_THRESH_BINARY);	
		}
		

		/*if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Res_Dark_WRGB%d.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++,c);
			cv::imwrite( (cv::String)(CStringA)strTemp, matBGSizeDark[c]);

			strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Res_Bright_WRGB%d.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++,c);
			cv::imwrite( (cv::String)(CStringA)strTemp, matBGSizeBright[c]);
		}*/

		for(int y  = 0 ; y < matResROIBuf.rows; y ++)
		{
			for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
			{
				if(matBGSizeDark[c].at<uchar>(y, x) > 0)
				{
					if(c==0)
						matGaussTemp.at<uchar>(y, x) = matBGSize.at<uchar>(y, x);
					else if(c==1)
						matRedGaussTemp.at<uchar>(y, x) = matBGSize.at<uchar>(y, x);
					else if(c==2)
						matGreenGaussTemp.at<uchar>(y, x) = matBGSize.at<uchar>(y, x);
					else
					   matBlueGaussTemp.at<uchar>(y, x) = matBGSize.at<uchar>(y, x);
				}

				if(matBGSizeBright[c].at<uchar>(y, x) > 0)
				{
					if(c==0)
						matGaussTemp.at<uchar>(y, x) = matBGSize.at<uchar>(y, x);
					else if(c==1)
						matRedGaussTemp.at<uchar>(y, x) = matBGSize.at<uchar>(y, x);
					else if(c==2)
						matGreenGaussTemp.at<uchar>(y, x) = matBGSize.at<uchar>(y, x);
					else
					   matBlueGaussTemp.at<uchar>(y, x) = matBGSize.at<uchar>(y, x);
				}
			}
		}

		/*if( bImageSave )
		{
			CString strTemp;
			strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_SizeSubBinay_%d.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++,c);
			if(c==0)
			{
				cv::imwrite( (cv::String)(CStringA)strTemp, matGauss);
			}else if(c==1)
			{
				cv::imwrite( (cv::String)(CStringA)strTemp, matRedGauss);
			}else if(c==2)
			{				
				cv::imwrite( (cv::String)(CStringA)strTemp, matGreenGauss);
			}else 
			{
				cv::imwrite( (cv::String)(CStringA)strTemp, matBlueGauss);
			}
		}*/

		//if( nDeleteOutLine > 0 ){
		//	cv::rectangle(matBGSizeDarkOrg[c], cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()), cv::Scalar(0, 0, 0), nDeleteOutLine);
		//	cv::rectangle(matBGSizeBrightOrg[c], cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()), cv::Scalar(0, 0, 0), nDeleteOutLine);
		//}


		// 강불량
		/*
		if(c == 0){
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matGrayBuf, matBGSizeDarkOrg[c], matDrawBuf, 
				nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

			cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(100, 100, 100), BLOB_DRAW_BLOBS_CONTOUR, true);

			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matGrayBuf, matBGSizeBrightOrg[c], matDrawBuf, 
				nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

			cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(100, 100, 100), BLOB_DRAW_BLOBS_CONTOUR, true);
		}else if(c == 1){
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matRedBuf, matBGSizeDarkOrg[c], matDrawBuf, 
				nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

			cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(100, 100, 255), BLOB_DRAW_BLOBS_CONTOUR, true);

			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matRedBuf, matBGSizeBrightOrg[c], matDrawBuf, 
				nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

			cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(100, 100, 255), BLOB_DRAW_BLOBS_CONTOUR, true);
		}else if(c == 2){
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matGreenBuf, matBGSizeDarkOrg[c], matDrawBuf, 
				nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

			cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(100, 255, 100), BLOB_DRAW_BLOBS_CONTOUR, true);

			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matGreenBuf, matBGSizeBrightOrg[c], matDrawBuf, 
				nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

			cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(100, 255, 100), BLOB_DRAW_BLOBS_CONTOUR, true);
		}else{
			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matBlueBuf, matBGSizeDarkOrg[c], matDrawBuf, 
				nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

			cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(255, 100, 100), BLOB_DRAW_BLOBS_CONTOUR, true);

			nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matBlueBuf, matBGSizeBrightOrg[c], matDrawBuf, 
				nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

			cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(255, 100, 100), BLOB_DRAW_BLOBS_CONTOUR, true);			
		}*/

	

		if( !reSize.empty() )			reSize.release();
		if( !matBGSize.empty() )		matBGSize.release();
		//if( !matBGSizeDark.empty() )	matBGSizeDark.release();
		//if( !matBGSizeBright.empty() )	matBGSizeBright.release();
	}

	// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
	// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
	//if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
	//	cFeatureExtraction.SaveTxt(nCommonPara, _strPath[1], true);


	//cv::medianBlur(matGaussTemp, matGaussTemp, 5);
	//cv::medianBlur(matRedGaussTemp, matRedGaussTemp, 5);
	//cv::medianBlur(matGreenGaussTemp, matGreenGaussTemp, 5);
	//cv::medianBlur(matBlueGaussTemp, matBlueGaussTemp, 5);

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_GrayCorrectAfter.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matGaussTemp );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_RedCorrectAfters.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matRedGaussTemp );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_GreenCorrectAfter.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matGreenGaussTemp );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_BlueCorrectAfter.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matBlueGaussTemp );
	}


	////////////////////////////////////////////////////////////////////
	// 배경검출

	cv::Mat matEstim;
	cv::Mat matEstimRed;
	cv::Mat matEstimGreen;
	cv::Mat matEstimBlue;


	if( !Estimation_XY(matGaussTemp, matEstim, nDimensionX, nDimensionY, nStepX, nStepY, fThBGOffset) ||
		!Estimation_XY(matRedGaussTemp, matEstimRed, nDimensionX, nDimensionY, nStepX, nStepY, fThBGOffset) ||
		!Estimation_XY(matGreenGaussTemp, matEstimGreen, nDimensionX, nDimensionY, nStepX, nStepY, fThBGOffset) ||
		!Estimation_XY(matBlueGaussTemp, matEstimBlue, nDimensionX, nDimensionY, nStepX, nStepY, fThBGOffset)  )
	{
		// 메모리 해제
		if( !matGauss.empty() )			matGauss.release();
		if( !matRedGauss.empty() )		matRedGauss.release();
		if( !matGreenGauss.empty() )	matGreenGauss.release();
		if( !matBlueGauss.empty() )		matBlueGauss.release();

		if( !matEstim.empty() )			matEstim.release();
		if( !matEstimRed.empty() )		matEstimRed.release();
		if( !matEstimGreen.empty() )	matEstimGreen.release();
		if( !matEstimBlue.empty() )		matEstimBlue.release();

		if( !matSrcROIBuf.empty() )		matSrcROIBuf.release();

		if( !matResROIBuf.empty() )		matResROIBuf.release();
		if( !matResRedROIBuf.empty() )		matResRedROIBuf.release();
		if( !matResGreenROIBuf.empty() )	matResGreenROIBuf.release();
		if( !matResBlueROIBuf.empty() )		matResBlueROIBuf.release();

		if( !matGrayROIBuf.empty() )	matGrayROIBuf.release();
		if( !matRedROIBuf.empty() )		matRedROIBuf.release();
		if( !matGreenROIBuf.empty() )	matGreenROIBuf.release();
		if( !matBlueROIBuf.empty() )	matBlueROIBuf.release();
		
		if( !matGrayBuf.empty() )		matGrayBuf.release();
		if( !matRedBuf.empty() )		matRedBuf.release();
		if( !matGreenBuf.empty() )		matGreenBuf.release();
		if( !matBlueBuf.empty() )		matBlueBuf.release();


		if( !matResBuf.empty() )		matResBuf.release();
		if( !matResRedBuf.empty() )		matResRedBuf.release();
		if( !matResGreenBuf.empty() )	matResGreenBuf.release();
		if( !matResBlueBuf.empty() )	matResBlueBuf.release();

		return E_ERROR_CODE_SVI_EMD_ESTMATION;
	}

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Estim.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matEstim );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_EstimRed.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matEstimRed );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_EstimGreen.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matEstimGreen );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_EstimBlue.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matEstimBlue );
	}
	///////////////////////////////////////////////////////////////////

	cv::Mat matSub;
	cv::Mat matRedSub;
	cv::Mat matGreenSub;
	cv::Mat matBlueSub;

	cv::subtract(matEstim, matGauss, matSub);
	cv::subtract(matEstimRed, matRedGauss, matRedSub);
	cv::subtract(matEstimGreen, matGreenGauss, matGreenSub);
	cv::subtract(matEstimBlue, matBlueGauss, matBlueSub);


	cv::threshold(matBGSizeDark[0], matBGSizeDark[0], nTh, 255.0, CV_THRESH_BINARY_INV);
	cv::threshold(matBGSizeDark[1], matBGSizeDark[1], nTh, 255.0, CV_THRESH_BINARY_INV);
	cv::threshold(matBGSizeDark[2], matBGSizeDark[2], nTh, 255.0, CV_THRESH_BINARY_INV);
	cv::threshold(matBGSizeDark[3], matBGSizeDark[3], nTh, 255.0, CV_THRESH_BINARY_INV);

	cv::threshold(matBGSizeBright[0], matBGSizeBright[0], nTh, 255.0, CV_THRESH_BINARY_INV);
	cv::threshold(matBGSizeBright[1], matBGSizeBright[1], nTh, 255.0, CV_THRESH_BINARY_INV);
	cv::threshold(matBGSizeBright[2], matBGSizeBright[2], nTh, 255.0, CV_THRESH_BINARY_INV);
	cv::threshold(matBGSizeBright[3], matBGSizeBright[3], nTh, 255.0, CV_THRESH_BINARY_INV);


	// 강 EMD DARK, 강EMD BRIGHT는 LINE 제거에서 제외됨
	cv::add(matBGSizeDark[0], matBGSizeBright[0], matBGSizeDark[0]);
	cv::add(matBGSizeDark[1], matBGSizeBright[1], matBGSizeDark[0]);
	cv::add(matBGSizeDark[2], matBGSizeBright[2], matBGSizeDark[0]);
	cv::add(matBGSizeDark[3], matBGSizeBright[3], matBGSizeDark[0]);
	
	cv::subtract(matSub, matLineMask, matSub, matBGSizeDark[0]);
	cv::subtract(matRedSub, matLineMask, matRedSub,matBGSizeDark[1]);
	cv::subtract(matGreenSub, matLineMask, matGreenSub, matBGSizeDark[2]);
	cv::subtract(matBlueSub, matLineMask, matBlueSub, matBGSizeDark[3]);

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Sub.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matSub );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_SubRed.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matRedSub );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_SubGreen.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matGreenSub );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_SubBlue.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matBlueSub );
	}
	
	//기존
	//cv::threshold(matSub, matResROIBuf, nThreshold, 255.0, CV_THRESH_BINARY);
	//cv::threshold(matRedSub, matResRedROIBuf, nThreshold, 255.0, CV_THRESH_BINARY);
	//cv::threshold(matGreenSub, matResGreenROIBuf, nThreshold, 255.0, CV_THRESH_BINARY);
	//cv::threshold(matBlueSub, matResBlueROIBuf, nThreshold, 255.0, CV_THRESH_BINARY);


	cv::Mat matAtiveBGOrg = matBKBuf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );
	cv::Mat matAtiveBG = matAtiveBGOrg.clone();
	cv::Mat matAtiveEdge = matAtiveBGOrg.clone();

	//if( bImageSave )
	//{
	//	CString strTemp;
	//	strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_ActiveOrg.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
	//	cv::imwrite( (cv::String)(CStringA)strTemp, matAtiveBGOrg );
	//}

	//Active 영역 분리
	if(nEdgePiexl > 0 &&  nEdgeThreshold > 0)
		cv::morphologyEx(matAtiveBG, matAtiveBG, cv::MORPH_DILATE, getStructuringElement(MORPH_RECT, Size(nEdgePiexl, nEdgePiexl)));
	
	//if( bImageSave )
	//{
	//	CString strTemp;
	//	strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_ActiveOrgDilate.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
	//	cv::imwrite( (cv::String)(CStringA)strTemp, matAtiveBG );
	//}

	cv::threshold(matAtiveBG, matAtiveBG, 1, 255.0, CV_THRESH_BINARY_INV);
	
	if(nLeftPiexl > 0)
	{
		for(int y  = 0 ; y < matResROIBuf.rows; y ++)
		{
			for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
			{
				if(x < nLeftPiexl)
				{
					matAtiveBG.at<uchar>(y,x) = 0;

					if(matSub.at<uchar>(y,x) > nLeftThreshold)
						matResROIBuf.at<uchar>(y,x) = 255;
					else
						matResROIBuf.at<uchar>(y,x) = 0;

					if(matRedSub.at<uchar>(y,x) > nLeftThreshold)
						matResRedROIBuf.at<uchar>(y,x) = 255;
					else
						matResRedROIBuf.at<uchar>(y,x) = 0;

					if(matGreenSub.at<uchar>(y,x) > nLeftThreshold)
						matResGreenROIBuf.at<uchar>(y,x) = 255;
					else
						matResGreenROIBuf.at<uchar>(y,x) = 0;

					if(matBlueSub.at<uchar>(y,x) > nLeftThreshold)
						matResBlueROIBuf.at<uchar>(y,x) = 255;
					else
						matResBlueROIBuf.at<uchar>(y,x) = 0;
				}

				if(matAtiveBG.at<uchar>(y,x) > 0)
				{
					matAtiveEdge.at<uchar>(y,x) = 0;
				}else{
					matAtiveEdge.at<uchar>(y,x) = 255;
				}

				if(x < nLeftPiexl)
					matAtiveEdge.at<uchar>(y,x) = 0;
			}
		}
	}else{
		cv::threshold(matAtiveBG, matAtiveEdge, 1, 255.0, CV_THRESH_BINARY_INV);
	}

	//if( bImageSave )
	//{
	//	CString strTemp;

	//	strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_ActiveInspectionRegion.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
	//	cv::imwrite( (cv::String)(CStringA)strTemp, matAtiveBG );

	//	strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_EdgeInspectionRegion.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
	//	cv::imwrite( (cv::String)(CStringA)strTemp, matAtiveEdge );
	//}

	cv::Mat matResBlue1Buf	= cv::Mat::zeros(matSrcBuf.size(), CV_8UC1);
	cv::Mat matBlue1ROIBuf = matResBlue1Buf( cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()) );

	for(int y  = 0 ; y < matResROIBuf.rows; y ++)
	{
		for(int x  = 0 ; x < matResROIBuf.cols ; x ++)
		{
			if(matAtiveBG.at<uchar>(y,x) > 0)
			{
				if(matSub.at<uchar>(y,x) > nThreshold)
					matResROIBuf.at<uchar>(y,x) = 255;
				else
					matResROIBuf.at<uchar>(y,x) = 0;

				if(matRedSub.at<uchar>(y,x) > nThreshold)
					matResRedROIBuf.at<uchar>(y,x) = 255;
				else
					matResRedROIBuf.at<uchar>(y,x) = 0;

				if(matGreenSub.at<uchar>(y,x) > nThreshold)
					matResGreenROIBuf.at<uchar>(y,x) = 255;
				else
					matResGreenROIBuf.at<uchar>(y,x) = 0;

				if(matBlueSub.at<uchar>(y,x) > nThreshold)
					matResBlueROIBuf.at<uchar>(y,x) = 255;
				else
					matResBlueROIBuf.at<uchar>(y,x) = 0;
			}

			if(matAtiveEdge.at<uchar>(y,x) > 0 && nEdgeThreshold > 0)
			{
				if(matSub.at<uchar>(y,x) > nEdgeThreshold)
					matResROIBuf.at<uchar>(y,x) = 255;
				else
					matResROIBuf.at<uchar>(y,x) = 0;

				if(matRedSub.at<uchar>(y,x) > nEdgeThreshold)
					matResRedROIBuf.at<uchar>(y,x) = 255;
				else
					matResRedROIBuf.at<uchar>(y,x) = 0;

				if(matGreenSub.at<uchar>(y,x) > nEdgeThreshold)
					matResGreenROIBuf.at<uchar>(y,x) = 255;
				else
					matResGreenROIBuf.at<uchar>(y,x) = 0;

				if(matBlueSub.at<uchar>(y,x) > nEdgeThreshold)
					matResBlueROIBuf.at<uchar>(y,x) = 255;
				else
					matResBlueROIBuf.at<uchar>(y,x) = 0;
			}

			// 결과 크기를 동일하게 만듬
			if(matResROIBuf.at<uchar>(y,x) == 255  ||
				matResRedROIBuf.at<uchar>(y,x) == 255 ||
				matResGreenROIBuf.at<uchar>(y,x) == 255 ||
				matResBlueROIBuf.at<uchar>(y,x) == 255)
			{
				matResROIBuf.at<uchar>(y,x) = 255;
				matResRedROIBuf.at<uchar>(y,x) = 255;
				matResGreenROIBuf.at<uchar>(y,x) = 255;
				matResBlueROIBuf.at<uchar>(y,x) = 255;
			}

		}
	}


	/*if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_ActiveOrg.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matAtiveBGOrg );
	}*/
		


	// 점등 영역 불량만 남기기
	cv::subtract(matResBuf, matBKBuf, matResBuf);
	cv::subtract(matResRedBuf, matBKBuf, matResRedBuf);
	cv::subtract(matResGreenBuf, matBKBuf, matResGreenBuf);
	cv::subtract(matResBlueBuf, matBKBuf, matResBlueBuf);


	// 최 외곽선 제거
	if( nDeleteOutLine > 0 ){
		cv::rectangle(matResBuf, cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()), cv::Scalar(0, 0, 0), nDeleteOutLine);
		cv::rectangle(matResRedBuf, cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()), cv::Scalar(0, 0, 0), nDeleteOutLine);
		cv::rectangle(matResGreenBuf, cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()), cv::Scalar(0, 0, 0), nDeleteOutLine);
		cv::rectangle(matResBlueBuf, cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height()), cv::Scalar(0, 0, 0), nDeleteOutLine);
	}

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_Res.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matResBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_ResRed.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matResRedBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_ResGreen.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matResGreenBuf );

		strTemp.Format(_T("%s%s_%s_%02d_%02d_SVI_EMD_ResBlue.jpg"), _strPath[0], g_strPat[nImageNum], g_strCam[nCamNum], nROINumber, nSaveImageCount++);
		cv::imwrite( (cv::String)(CStringA)strTemp, matResBlueBuf );
	}



	//////////////////////////////////////////////////////////////////////////
	// 결과
	//////////////////////////////////////////////////////////////////////////

	// 라벨
	//CFeatureExtraction cFeatureExtraction;
	//cFeatureExtraction.SetMem( NULL );
	

	// EMD 결과
//  	nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matGrayBuf, matResBuf, matDrawBuf, 
//  		nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob);
	
	
	//by cyh EMD 결과
	nErrorCode = cFeatureExtraction.DoDefectBlobSingleJudgment(matGrayBuf, matResBuf, matDrawBuf, nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob, 
		E_DEFECT_JUDGEMENT_MURA_EMD_DARK, FALSE);

	//vector<tBLOB_FEATURE> GrayBlob;
	//cFeatureExtraction.GetResultblob(GrayBlob);

	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		// 메모리 해제
		if( !matGauss.empty() )			matGauss.release();
		if( !matRedGauss.empty() )		matRedGauss.release();
		if( !matGreenGauss.empty() )	matGreenGauss.release();
		if( !matBlueGauss.empty() )		matBlueGauss.release();

		if( !matEstim.empty() )			matEstim.release();
		if( !matEstimRed.empty() )		matEstimRed.release();
		if( !matEstimGreen.empty() )	matEstimGreen.release();
		if( !matEstimBlue.empty() )		matEstimBlue.release();

		if( !matSrcROIBuf.empty() )		matSrcROIBuf.release();

		if( !matResROIBuf.empty() )		matResROIBuf.release();
		if( !matResRedROIBuf.empty() )		matResRedROIBuf.release();
		if( !matResGreenROIBuf.empty() )	matResGreenROIBuf.release();
		if( !matResBlueROIBuf.empty() )		matResBlueROIBuf.release();

		if( !matGrayROIBuf.empty() )	matGrayROIBuf.release();
		if( !matRedROIBuf.empty() )		matRedROIBuf.release();
		if( !matGreenROIBuf.empty() )	matGreenROIBuf.release();
		if( !matBlueROIBuf.empty() )	matBlueROIBuf.release();
		
		if( !matGrayBuf.empty() )		matGrayBuf.release();
		if( !matRedBuf.empty() )		matRedBuf.release();
		if( !matGreenBuf.empty() )		matGreenBuf.release();
		if( !matBlueBuf.empty() )		matBlueBuf.release();


		if( !matResBuf.empty() )		matResBuf.release();
		if( !matResRedBuf.empty() )		matResRedBuf.release();
		if( !matResGreenBuf.empty() )	matResGreenBuf.release();
		if( !matResBlueBuf.empty() )	matResBlueBuf.release();

		if( !matSub.empty() )			matSub.release();
		if( !matRedSub.empty() )			matRedSub.release();
		if( !matGreenSub.empty() )			matGreenSub.release();
		if( !matBlueSub.empty() )			matBlueSub.release();

		if( !matAtiveBGOrg.empty() )		matAtiveBGOrg.release();
		if( !matAtiveBG.empty() )			matAtiveBG.release();
		if( !matAtiveEdge.empty() )			matAtiveEdge.release();


		return nErrorCode;
	}

	// 결과 외곽선 그리기 ( Light SkyBlue )
	cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

// 	nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matRedBuf, matResRedBuf, matDrawBuf, 
// 		nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

	nErrorCode = cFeatureExtraction.DoDefectBlobSingleJudgment(matRedBuf, matResRedBuf, matDrawBuf, nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob, 
		E_DEFECT_JUDGEMENT_MURA_EMD_DARK, FALSE);

	//vector<tBLOB_FEATURE> RedBlob;
	//cFeatureExtraction.GetResultblob(RedBlob);


	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		// 메모리 해제
		if( !matGauss.empty() )			matGauss.release();
		if( !matRedGauss.empty() )		matRedGauss.release();
		if( !matGreenGauss.empty() )	matGreenGauss.release();
		if( !matBlueGauss.empty() )		matBlueGauss.release();

		if( !matEstim.empty() )			matEstim.release();
		if( !matEstimRed.empty() )		matEstimRed.release();
		if( !matEstimGreen.empty() )	matEstimGreen.release();
		if( !matEstimBlue.empty() )		matEstimBlue.release();

		if( !matSrcROIBuf.empty() )		matSrcROIBuf.release();

		if( !matResROIBuf.empty() )		matResROIBuf.release();
		if( !matResRedROIBuf.empty() )		matResRedROIBuf.release();
		if( !matResGreenROIBuf.empty() )	matResGreenROIBuf.release();
		if( !matResBlueROIBuf.empty() )		matResBlueROIBuf.release();

		if( !matGrayROIBuf.empty() )	matGrayROIBuf.release();
		if( !matRedROIBuf.empty() )		matRedROIBuf.release();
		if( !matGreenROIBuf.empty() )	matGreenROIBuf.release();
		if( !matBlueROIBuf.empty() )	matBlueROIBuf.release();
		
		if( !matGrayBuf.empty() )		matGrayBuf.release();
		if( !matRedBuf.empty() )		matRedBuf.release();
		if( !matGreenBuf.empty() )		matGreenBuf.release();
		if( !matBlueBuf.empty() )		matBlueBuf.release();


		if( !matResBuf.empty() )		matResBuf.release();
		if( !matResRedBuf.empty() )		matResRedBuf.release();
		if( !matResGreenBuf.empty() )	matResGreenBuf.release();
		if( !matResBlueBuf.empty() )	matResBlueBuf.release();

		if( !matSub.empty() )			matSub.release();
		if( !matRedSub.empty() )			matRedSub.release();
		if( !matGreenSub.empty() )			matGreenSub.release();
		if( !matBlueSub.empty() )			matBlueSub.release();

		if( !matAtiveBGOrg.empty() )		matAtiveBGOrg.release();
		if( !matAtiveBG.empty() )			matAtiveBG.release();
		if( !matAtiveEdge.empty() )			matAtiveEdge.release();

		return nErrorCode;
	}

	// 결과 외곽선 그리기 ( Light SkyBlue )
	cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

// 	nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matGreenBuf, matResGreenBuf, matDrawBuf, 
// 		nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

	nErrorCode = cFeatureExtraction.DoDefectBlobSingleJudgment(matGreenBuf, matResGreenBuf, matDrawBuf, nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob, 
		E_DEFECT_JUDGEMENT_MURA_EMD_DARK, FALSE);

	//vector<tBLOB_FEATURE> GBlob;
	//cFeatureExtraction.GetResultblob(GBlob);


	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		// 메모리 해제
		if( !matGauss.empty() )			matGauss.release();
		if( !matRedGauss.empty() )		matRedGauss.release();
		if( !matGreenGauss.empty() )	matGreenGauss.release();
		if( !matBlueGauss.empty() )		matBlueGauss.release();

		if( !matEstim.empty() )			matEstim.release();
		if( !matEstimRed.empty() )		matEstimRed.release();
		if( !matEstimGreen.empty() )	matEstimGreen.release();
		if( !matEstimBlue.empty() )		matEstimBlue.release();

		if( !matSrcROIBuf.empty() )		matSrcROIBuf.release();

		if( !matResROIBuf.empty() )		matResROIBuf.release();
		if( !matResRedROIBuf.empty() )		matResRedROIBuf.release();
		if( !matResGreenROIBuf.empty() )	matResGreenROIBuf.release();
		if( !matResBlueROIBuf.empty() )		matResBlueROIBuf.release();

		if( !matGrayROIBuf.empty() )	matGrayROIBuf.release();
		if( !matRedROIBuf.empty() )		matRedROIBuf.release();
		if( !matGreenROIBuf.empty() )	matGreenROIBuf.release();
		if( !matBlueROIBuf.empty() )	matBlueROIBuf.release();
		
		if( !matGrayBuf.empty() )		matGrayBuf.release();
		if( !matRedBuf.empty() )		matRedBuf.release();
		if( !matGreenBuf.empty() )		matGreenBuf.release();
		if( !matBlueBuf.empty() )		matBlueBuf.release();


		if( !matResBuf.empty() )		matResBuf.release();
		if( !matResRedBuf.empty() )		matResRedBuf.release();
		if( !matResGreenBuf.empty() )	matResGreenBuf.release();
		if( !matResBlueBuf.empty() )	matResBlueBuf.release();

		if( !matSub.empty() )			matSub.release();
		if( !matRedSub.empty() )			matRedSub.release();
		if( !matGreenSub.empty() )			matGreenSub.release();
		if( !matBlueSub.empty() )			matBlueSub.release();

		if( !matAtiveBGOrg.empty() )		matAtiveBGOrg.release();
		if( !matAtiveBG.empty() )			matAtiveBG.release();
		if( !matAtiveEdge.empty() )			matAtiveEdge.release();

		return nErrorCode;
	}


	// 결과 외곽선 그리기 ( Light SkyBlue )
	cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

// 	nErrorCode = cFeatureExtraction.DoDefectBlobJudgment(matBlueBuf, matResBlueBuf, matDrawBuf, 
// 		nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob);

	nErrorCode = cFeatureExtraction.DoDefectBlobSingleJudgment(matBlueBuf, matResBlueBuf, matDrawBuf, nCommonPara, E_DEFECT_COLOR_DARK, _T("EMD_"), EngineerDefectJudgment, pResultBlob, 
		E_DEFECT_JUDGEMENT_MURA_EMD_DARK, FALSE);

	//vector<tBLOB_FEATURE> BBlob;
	//cFeatureExtraction.GetResultblob(BBlob);


	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		// 메모리 해제
		if( !matGauss.empty() )			matGauss.release();
		if( !matRedGauss.empty() )		matRedGauss.release();
		if( !matGreenGauss.empty() )	matGreenGauss.release();
		if( !matBlueGauss.empty() )		matBlueGauss.release();

		if( !matEstim.empty() )			matEstim.release();
		if( !matEstimRed.empty() )		matEstimRed.release();
		if( !matEstimGreen.empty() )	matEstimGreen.release();
		if( !matEstimBlue.empty() )		matEstimBlue.release();

		if( !matSrcROIBuf.empty() )		matSrcROIBuf.release();

		if( !matResROIBuf.empty() )		matResROIBuf.release();
		if( !matResRedROIBuf.empty() )		matResRedROIBuf.release();
		if( !matResGreenROIBuf.empty() )	matResGreenROIBuf.release();
		if( !matResBlueROIBuf.empty() )		matResBlueROIBuf.release();

		if( !matGrayROIBuf.empty() )	matGrayROIBuf.release();
		if( !matRedROIBuf.empty() )		matRedROIBuf.release();
		if( !matGreenROIBuf.empty() )	matGreenROIBuf.release();
		if( !matBlueROIBuf.empty() )	matBlueROIBuf.release();
		
		if( !matGrayBuf.empty() )		matGrayBuf.release();
		if( !matRedBuf.empty() )		matRedBuf.release();
		if( !matGreenBuf.empty() )		matGreenBuf.release();
		if( !matBlueBuf.empty() )		matBlueBuf.release();


		if( !matResBuf.empty() )		matResBuf.release();
		if( !matResRedBuf.empty() )		matResRedBuf.release();
		if( !matResGreenBuf.empty() )	matResGreenBuf.release();
		if( !matResBlueBuf.empty() )	matResBlueBuf.release();

		if( !matSub.empty() )			matSub.release();
		if( !matRedSub.empty() )			matRedSub.release();
		if( !matGreenSub.empty() )			matGreenSub.release();
		if( !matBlueSub.empty() )			matBlueSub.release();

		if( !matAtiveBGOrg.empty() )		matAtiveBGOrg.release();
		if( !matAtiveBG.empty() )			matAtiveBG.release();
		if( !matAtiveEdge.empty() )			matAtiveEdge.release();

		return nErrorCode;
	}
		
	// 결과 외곽선 그리기 ( Light SkyBlue )
	cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);

	
	//vector<tBLOB_FEATURE> BBlob;
	//cFeatureExtraction.GetResultblob(BBlob);


	if( nErrorCode != E_ERROR_CODE_TRUE )
	{
		// 메모리 해제
		if( !matGauss.empty() )			matGauss.release();
		if( !matRedGauss.empty() )		matRedGauss.release();
		if( !matGreenGauss.empty() )	matGreenGauss.release();
		if( !matBlueGauss.empty() )		matBlueGauss.release();

		if( !matEstim.empty() )			matEstim.release();
		if( !matEstimRed.empty() )		matEstimRed.release();
		if( !matEstimGreen.empty() )	matEstimGreen.release();
		if( !matEstimBlue.empty() )		matEstimBlue.release();

		if( !matSrcROIBuf.empty() )		matSrcROIBuf.release();

		if( !matResROIBuf.empty() )		matResROIBuf.release();
		if( !matResRedROIBuf.empty() )		matResRedROIBuf.release();
		if( !matResGreenROIBuf.empty() )	matResGreenROIBuf.release();
		if( !matResBlueROIBuf.empty() )		matResBlueROIBuf.release();

		if( !matGrayROIBuf.empty() )	matGrayROIBuf.release();
		if( !matRedROIBuf.empty() )		matRedROIBuf.release();
		if( !matGreenROIBuf.empty() )	matGreenROIBuf.release();
		if( !matBlueROIBuf.empty() )	matBlueROIBuf.release();
		
		if( !matGrayBuf.empty() )		matGrayBuf.release();
		if( !matRedBuf.empty() )		matRedBuf.release();
		if( !matGreenBuf.empty() )		matGreenBuf.release();
		if( !matBlueBuf.empty() )		matBlueBuf.release();


		if( !matResBuf.empty() )		matResBuf.release();
		if( !matResRedBuf.empty() )		matResRedBuf.release();
		if( !matResGreenBuf.empty() )	matResGreenBuf.release();
		if( !matResBlueBuf.empty() )	matResBlueBuf.release();

		if( !matSub.empty() )			matSub.release();
		if( !matRedSub.empty() )			matRedSub.release();
		if( !matGreenSub.empty() )			matGreenSub.release();
		if( !matBlueSub.empty() )			matBlueSub.release();

		if( !matAtiveBGOrg.empty() )		matAtiveBGOrg.release();
		if( !matAtiveBG.empty() )			matAtiveBG.release();
		if( !matAtiveEdge.empty() )			matAtiveEdge.release();

		return nErrorCode;
	}
		
	// 결과 외곽선 그리기 ( Light SkyBlue )
	cFeatureExtraction.DrawBlob(matDrawBuf, cv::Scalar(135, 206, 250), BLOB_DRAW_BLOBS_CONTOUR, true);



	// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
	// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
	if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(nCommonPara, _strPath[1], true);

	//////////////////////////////////////////////////////////////////////////j
	// 할당 해제
	//////////////////////////////////////////////////////////////////////////

	if( !matGauss.empty() )			matGauss.release();
	if( !matRedGauss.empty() )		matRedGauss.release();
	if( !matGreenGauss.empty() )	matGreenGauss.release();
	if( !matBlueGauss.empty() )		matBlueGauss.release();

	if( !matEstim.empty() )			matEstim.release();
	if( !matEstimRed.empty() )		matEstimRed.release();
	if( !matEstimGreen.empty() )	matEstimGreen.release();
	if( !matEstimBlue.empty() )		matEstimBlue.release();

	if( !matSrcROIBuf.empty() )		matSrcROIBuf.release();

	if( !matResROIBuf.empty() )		matResROIBuf.release();
	if( !matResRedROIBuf.empty() )		matResRedROIBuf.release();
	if( !matResGreenROIBuf.empty() )	matResGreenROIBuf.release();
	if( !matResBlueROIBuf.empty() )		matResBlueROIBuf.release();

	if( !matGrayROIBuf.empty() )	matGrayROIBuf.release();
	if( !matRedROIBuf.empty() )		matRedROIBuf.release();
	if( !matGreenROIBuf.empty() )	matGreenROIBuf.release();
	if( !matBlueROIBuf.empty() )	matBlueROIBuf.release();
		
	if( !matGrayBuf.empty() )		matGrayBuf.release();
	if( !matRedBuf.empty() )		matRedBuf.release();
	if( !matGreenBuf.empty() )		matGreenBuf.release();
	if( !matBlueBuf.empty() )		matBlueBuf.release();


	if( !matResBuf.empty() )		matResBuf.release();
	if( !matResRedBuf.empty() )		matResRedBuf.release();
	if( !matResGreenBuf.empty() )	matResGreenBuf.release();
	if( !matResBlueBuf.empty() )	matResBlueBuf.release();

	if( !matSub.empty() )			matSub.release();
	if( !matRedSub.empty() )			matRedSub.release();
	if( !matGreenSub.empty() )			matGreenSub.release();
	if( !matBlueSub.empty() )			matBlueSub.release();

	if( !matAtiveBGOrg.empty() )		matAtiveBGOrg.release();
	if( !matAtiveBG.empty() )			matAtiveBG.release();
	if( !matAtiveEdge.empty() )			matAtiveEdge.release();


	// 불량 개수 만큼
	//int a1 = pResultBlob->nMaxGV[0];
	//int b1 = pResultBlob->nMinGV[0];
	//int a2 = pResultBlob->nMaxGV[1];
	//int a3 = pResultBlob->nMaxGV[2];

	// 코너에 존재하는 경우, 제외...
	JudgementArea(pResultBlob, rectROI, dPara);

	return E_ERROR_CODE_TRUE;
}


// x방향 피팅
bool CInspectSVI::Estimation_X(cv::Mat matSrcBuf, cv::Mat &matDstBuf, int nDimensionX, int nStepX, float fThBGOffset)
{
	if( matSrcBuf.empty() )			return false;

	if( matSrcBuf.channels() != 1 )	return false;

	if( !matDstBuf.empty() )
		matDstBuf.release();

	if( nStepX <= 0 )				return false;

	matDstBuf = cv::Mat::zeros(matSrcBuf.size(), matSrcBuf.type());

	int nStepCols = matSrcBuf.cols / nStepX;
	int nHalfCols = matSrcBuf.cols / 2;

	cv::Mat M = cv::Mat_<double>(nStepCols, nDimensionX+1);
	cv::Mat I = cv::Mat_<double>(nStepCols, 1);
	cv::Mat q;

	double x, quad, dTemp;
	int i, j, k, m;

	// 최소 밝기 값
	cv::Scalar mean = cv::mean( matSrcBuf );
	int nMinGV = (int)(mean[0] * fThBGOffset);

	for (i=0 ; i<matSrcBuf.rows ; i++)
	{
		for (j=0 ; j<nStepCols ; j++)
		{
			x = (j*nStepX - nHalfCols) / double(matSrcBuf.cols);

			M.at<double>(j, 0) = 1.0;
			dTemp = 1.0;
			for(k=1 ; k<=nDimensionX ; k++)
			{
				dTemp *= x;
				M.at<double>(j, k) = dTemp;
			}

			//I.at<double>(j, 0) = matSrcBuf.at<uchar>(i, j*nStepX);
			m = matSrcBuf.at<uchar>(i, j*nStepX);
			I.at<double>(j, 0) = ( m < nMinGV ) ? nMinGV : m;
		}

		cv::SVD s(M);
		s.backSubst(I, q);

		for (j=0 ; j<matDstBuf.cols ; j++)
		{
			x = (j - nHalfCols) / double(matSrcBuf.cols);

			quad = q.at<double>(0, 0);
			dTemp = 1.0;
			for(k=1 ; k<=nDimensionX ; k++)
			{
				dTemp *= x;
				quad += (q.at<double>(k, 0) * dTemp);
			}

			matDstBuf.at<uchar>(i, j) = saturate_cast<uchar>(quad);
		}
	}

	M.release();
	I.release();
	q.release();

	return true;
}


// y방향 피팅
bool CInspectSVI::Estimation_Y(cv::Mat matSrcBuf, cv::Mat &matDstBuf, int nDimensionY, int nStepY, float fThBGOffset)
{
	if( matSrcBuf.empty() )			return false;

	if( matSrcBuf.channels() != 1 )	return false;

	if( !matDstBuf.empty() )
		matDstBuf.release();

	if( nStepY <= 0 )				return false;

	matDstBuf = cv::Mat::zeros(matSrcBuf.size(), matSrcBuf.type());

	int nStepRows = matSrcBuf.rows / nStepY;
	int nHalfRows = matSrcBuf.rows / 2;

	cv::Mat M = cv::Mat_<double>(nStepRows, nDimensionY+1);
	cv::Mat I = cv::Mat_<double>(nStepRows, 1);
	cv::Mat q;

	double y, quad, dTemp;
	int i, j, k, m;

	// 최소 밝기 값
	cv::Scalar mean = cv::mean( matSrcBuf );
	int nMinGV = (int)(mean[0] * fThBGOffset);

	for (j=0 ; j<matSrcBuf.cols ; j++)
	{
		for (i=0 ; i<nStepRows ; i++)
		{
			y = (i*nStepY - nHalfRows) / double(matSrcBuf.rows);

			M.at<double>(i, 0) = 1.0;
			dTemp = 1.0;
			for(k=1 ; k<=nDimensionY ; k++)
			{
				dTemp *= y;
				M.at<double>(i, k) = dTemp;
			}

			//I.at<double>(i, 0) = matSrcBuf.at<uchar>(i*nStepY, j);
			m = matSrcBuf.at<uchar>(i*nStepY, j);
			I.at<double>(i, 0) = ( m < nMinGV ) ? nMinGV : m;
		}

		cv::SVD s(M);
		s.backSubst(I, q);

		for (i=0 ; i<matSrcBuf.rows ; i++)
		{
			y = (i - nHalfRows) / double(matSrcBuf.rows);

			quad = q.at<double>(0, 0);
			dTemp = 1.0;
			for(k=1 ; k<=nDimensionY ; k++)
			{
				dTemp *= y;
				quad += (q.at<double>(k, 0) * dTemp);
			}

			matDstBuf.at<uchar>(i, j) = saturate_cast<uchar>(quad);
		}
	}

	M.release();
	I.release();
	q.release();

	return true;
}


// xy방향 피팅
bool CInspectSVI::Estimation_XY(cv::Mat matSrcBuf, cv::Mat &matDstBuf, int nDimensionX, int nDimensionY, int nStepX, int nStepY, float fThBGOffset)
{
	if( matSrcBuf.empty() )			return false;

	if( matSrcBuf.channels() != 1 )	return false;

	if( !matDstBuf.empty() )
		matDstBuf.release();

	if( nStepX <= 0 )	return false;
	if( nStepY <= 0 )	return false;

	bool bRes = true;

	cv::Mat matDstBufX, matDstBufY;

	bRes = Estimation_X(matSrcBuf, matDstBufX, nDimensionX, nStepX, fThBGOffset);
	if( !bRes )			return false;

	bRes = Estimation_Y(matSrcBuf, matDstBufY, nDimensionY, nStepY, fThBGOffset);
	if( !bRes )			return false;

	bRes = TwoImg_Average(matDstBufX, matDstBufY, matDstBuf);
	if( !bRes )			return false;

	matDstBufX.release();
	matDstBufY.release();

	return bRes;
}


// 두개 영상 평균
bool CInspectSVI::TwoImg_Average(cv::Mat matSrc1Buf, cv::Mat matSrc2Buf, cv::Mat &matDstBuf)
{	
	if( matSrc1Buf.empty() )			return false;
	if( matSrc2Buf.empty() )			return false;

	if( matSrc1Buf.channels() != 1 )	return false;
	if( matSrc2Buf.channels() != 1 )	return false;

	if(	matSrc1Buf.rows != matSrc2Buf.rows	||		// 가로 크기 같아야 함
		matSrc1Buf.cols != matSrc2Buf.cols	)		// 세로 크기 같아야 함
		return false;

	if( !matDstBuf.empty() )
		matDstBuf.release();

	matDstBuf = cv::Mat::zeros(matSrc1Buf.rows, matSrc1Buf.cols, matSrc1Buf.type());

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

	return true;
}


// 영역에 따른 불량 재판정
bool CInspectSVI::JudgementArea(stDefectInfo* pResultBlob, CRect rectROI, double* dPara)
{
	// 파라미터 NULL인 경우
	if( dPara == NULL )				return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )		return E_ERROR_CODE_EMPTY_PARA;

	// 파라미터
	int	nJudgmUse	= (int)dPara[E_PARA_SVI_MURA_EMD_JUDGM_USE	];	//1;		// 판정 사용 유/무
	int	nJudgmSize	= (int)dPara[E_PARA_SVI_MURA_EMD_JUDGM_SIZE	];	//60;		// Edge로 부터 범위 설정

	//  사용 유/무
	if( nJudgmUse <= 0 )		return true;

	// 검출된 불량 개수
	int nCount = pResultBlob->nDefectCount;

	// 불량 개수가 0보다 클 경우만...
	if( nCount > 0 )
	{
		// 점등 영역 축소하여 설정
		CRect rectCell[7];

		// 축소된 Active 영역 ( EMD )
		rectCell[0] = CRect( 
			rectROI.left	+ nJudgmSize	,
			rectROI.top		+ nJudgmSize	,
			rectROI.right	- nJudgmSize	,
			rectROI.bottom	- nJudgmSize	);

		// LT 코너 ( 코너 혼색 )
		rectCell[1] = CRect(
			rectROI.left					,
			rectROI.top						,
			rectROI.left	+ nJudgmSize	,
			rectROI.top		+ nJudgmSize	);

		// RT 코너 ( 코너 혼색 )
		rectCell[2] = CRect( 
			rectROI.right	- nJudgmSize	,
			rectROI.top						,
			rectROI.right					,
			rectROI.top		- nJudgmSize	);

		// RB 코너 ( 코너 혼색 )
		rectCell[3] = CRect( 
			rectROI.right	- nJudgmSize	,
			rectROI.bottom	- nJudgmSize	,
			rectROI.right					,
			rectROI.bottom					);

		// LB 코너 ( 코너 혼색 )
		rectCell[4] = CRect( 
			rectROI.left					,
			rectROI.bottom	- nJudgmSize	,
			rectROI.left	+ nJudgmSize	,
			rectROI.bottom					);

		// 왼쪽 ( 상하 혼색 )
		rectCell[5] = CRect( 
			rectROI.left					,
			rectROI.top						,
			rectROI.left	+ nJudgmSize	,
			rectROI.bottom					);

		// 오른쪽 ( 상하 혼색 )
		rectCell[6] = CRect( 
			rectROI.right	- nJudgmSize	,
			rectROI.top						,
			rectROI.right					,
			rectROI.bottom					);

		// 불량 개수 만큼
		for (int i=0 ; i<nCount ; i++)
		{
			// EMD 불량이 아닌 경우 제외
			if( pResultBlob->nDefectJudge[i] != E_DEFECT_JUDGEMENT_MURA_EMD_DARK )
				continue;

			// 불량 줌심 좌표 확인
			CPoint ptP = CPoint( pResultBlob->nCenterx[i], pResultBlob->nCentery[i] );

			// 축소된 Active 영역 안에 불량 존재하면 EMD
			if ( rectCell[0].PtInRect( ptP ) )
				continue;

			// LT 코너 영역 안에 불량 존재하면 코너 혼색
			if ( rectCell[1].PtInRect( ptP ) )
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_CORNER_CM;
				continue;
			}

			// RT 코너 영역 안에 불량 존재하면 코너 혼색
			if ( rectCell[2].PtInRect( ptP ) )
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_CORNER_CM;
				continue;
			}

			// RB 코너 영역 안에 불량 존재하면 코너 혼색
			if ( rectCell[3].PtInRect( ptP ) )
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_CORNER_CM;
				continue;
			}

			// LB 코너 영역 안에 불량 존재하면 코너 혼색
			if ( rectCell[4].PtInRect( ptP ) )
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_CORNER_CM;
				continue;
			}

			// 왼쪽 영역 안에 불량 존재하면 상하 혼색
			if ( rectCell[5].PtInRect( ptP ) )
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_UP_CM;
				continue;
			}

			// 오른쪽 영역 안에 불량 존재하면 상하 혼색
			if ( rectCell[6].PtInRect( ptP ) )
			{
				pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_UP_CM;
				continue;
			}

			// 나머지 불량 -> 손가락 혼색
			pResultBlob->nDefectJudge[i] = E_DEFECT_JUDGEMENT_MURA_FINGER_CM;
		}
	}

	return true;
}