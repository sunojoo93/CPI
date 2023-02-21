/************************************************************************
Mura2 알고리즘 관련 소스
************************************************************************/


#include "StdAfx.h"
#include "InspectMura2.h"


CInspectMura2::CInspectMura2(void)
{
}


CInspectMura2::~CInspectMura2(void)
{
}


long CInspectMura2::FindMuraDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	///////////////////////////////////////////////////////
	//
	//	function	: Main 함수
	//	made by		: 김진영
	//				: 2017. 04. 17
	//
	//	[in]	param 1	:	원본 영상		/	CV_8UC1
	//
	//	[out]	param 2 :	바이너리 영상 출력 /	CV_8UC1
	//						사이즈 2개며 함수 호출전 다음 아래를 선언해야함.
	//
	//						vector<cv::Mat> matBins(3);
	//						matBins.at(0) = cv::Mat(matSrcImage.size(), CV_8UC1, Scalar(0));
	//						matBins.at(1) = cv::Mat(matSrcImage.size(), CV_8UC1, Scalar(0));
	//
	//						0 : Dark
	//						1 : Bright
	//
	//	[in]	param 3 :	검사영역 ROI
	//
	//	[in]	param 4	:	매개변수 ( 아직 사용 X )
	//
	//	[in]	param 5	:	이미지파일 저장 경로
	//
	//	[in]	param 6	:	이미지파일 저장 여부 선택
	//
	//	[return]		:	true / false
	//
	////////////////////////////////////////////////////////

	// 파라미터 NULL인 경우.
	if( dPara == NULL )					return E_ERROR_CODE_EMPTY_PARA;
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcBuffer.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

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

	long	nWidth	= (long)matSrcBuffer.cols;	// 영상 가로 크기
	long	nHeight	= (long)matSrcBuffer.rows;	// 영상 세로 크기

	// 임시로 범위 작게 설정
	CRect rectROI = new CRect(
		max(ptCorner[E_CORNER_LEFT_TOP].x,		ptCorner[E_CORNER_LEFT_BOTTOM].x	),
		max(ptCorner[E_CORNER_LEFT_TOP].y,		ptCorner[E_CORNER_RIGHT_TOP].y		),
		min(ptCorner[E_CORNER_RIGHT_TOP].x,		ptCorner[E_CORNER_RIGHT_BOTTOM].x	),
		min(ptCorner[E_CORNER_LEFT_BOTTOM].y,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	) );

	// 검사 영역이 영상 크기를 벗어난 경우
	if( rectROI.left	<	0		||
		rectROI.top		<	0		||
		rectROI.right	>= 	nWidth	||
		rectROI.bottom	>= 	nHeight	)	return E_ERROR_CODE_ROI_OVER;

	cv::Rect _rectROI(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height());	

	Mat matDarkResultImage_01;
	Mat matBrightResultImage_01;


	LogicStart_Gray(matSrcBuffer, matDarkResultImage_01,matBrightResultImage_01,rectROI, dPara,nCommonPara, strAlgPath, EngineerDefectJudgment);

	
	//Bright
	if( !matBrightResultImage_01.empty() )
	{
		// 라벨
		CFeatureExtraction cFeatureExtraction;

		// EMD 필터만 동작하도록...
		cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer, matBrightResultImage_01, matDrawBuffer, 
			nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("M2B_"), EngineerDefectJudgment, pResultBlob);

		// 결과 외곽선 그리기 ( Light SkyBlue )
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

		// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt);
	}


	//Dark
	if( !matDarkResultImage_01.empty() )
	{
		// 라벨
		CFeatureExtraction cFeatureExtraction;

		// EMD 필터만 동작하도록...
		cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer, matDarkResultImage_01, matDrawBuffer, 
			nCommonPara, E_DEFECT_COLOR_DARK, _T("M2D_"), EngineerDefectJudgment, pResultBlob);

		// 결과 외곽선 그리기 ( Light SkyBlue )
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

		// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt);
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectMura2::LogicStart_Gray(cv::Mat& matSrcImage, cv::Mat &matDarkResultImage_01, cv::Mat &matBrightResultImage_01,
									CRect rectROI, double* dPara,int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
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

	bImageSave =  true;

	//////////////////////////////////////////////////////////////////////////
	int nPIdx = 0;
	float fBrightThresholdRatio_M1 = (float)dPara[nPIdx++]/100.0;	// 주 검사 영역의 밝은불량 Threshold ratio
	float fDarkThresholdRatio_M1 = (float)dPara[nPIdx++]/100.0;	// 주 검사 영역의 어두운 불량 Threshold ratio

	float fBrightThresholdRatio_OutLine_M1 = (float)dPara[nPIdx++]/100.0; // 외곽 검사 영역의 밝은불량 Threshold ratio
	float fDarkThresholdRatio_OutLine_M1 = (float)dPara[nPIdx++]/100.0; // 외곽 검사 영역의 어두운 불량 Threshold ratio

	int nBirghtFilterSize_M1 = (int)dPara[nPIdx++]; // 1차 필터링할 불량 크기
	int nDarkFilterSize_M1 = (int)dPara[nPIdx++]; // 1차 필터링할 불량 크기
	
	nPIdx++;//  Dummy => --------------------------------------------

	float fBrightThresholdRatio_M2 = (float)dPara[nPIdx++]/100.0;	// 주 검사 영역의 밝은불량 Threshold ratio
	float fDarkThresholdRatio_M2 = (float)dPara[nPIdx++]/100.0;	// 주 검사 영역의 어두운 불량 Threshold ratio

	float fBrightThresholdRatio_OutLine_M2 = (float)dPara[nPIdx++]/100.0; // 외곽 검사 영역의 밝은불량 Threshold ratio
	float fDarkThresholdRatio_OutLine_M2 = (float)dPara[nPIdx++]/100.0; // 외곽 검사 영역의 어두운 불량 Threshold ratio

	int nBirghtFilterSize_M2 = (int)dPara[nPIdx++]; // 1차 필터링할 불량 크기
	int nDarkFilterSize_M2 = (int)dPara[nPIdx++]; // 1차 필터링할 불량 크기

	nPIdx++;//  Dummy => --------------------------------------------

	float fBrightThresholdRatio_L1 = (float)dPara[nPIdx++]/100.0;	// 주 검사 영역의 밝은불량 Threshold ratio
	float fDarkThresholdRatio_L1 = (float)dPara[nPIdx++]/100.0;	// 주 검사 영역의 어두운 불량 Threshold ratio

	float fBrightThresholdRatio_OutLine_L1 = (float)dPara[nPIdx++]/100.0; // 외곽 검사 영역의 밝은불량 Threshold ratio
	float fDarkThresholdRatio_OutLine_L1 = (float)dPara[nPIdx++]/100.0; // 외곽 검사 영역의 어두운 불량 Threshold ratio

	int nBirghtFilterSize_L1 = (int)dPara[nPIdx++]; // 1차 필터링할 불량 크기
	int nDarkFilterSize_L1 = (int)dPara[nPIdx++]; // 1차 필터링할 불량 크기

	nPIdx++;//  Dummy => --------------------------------------------

	float fBrightThresholdRatio_L2 = (float)dPara[nPIdx++]/100.0;	// 주 검사 영역의 밝은불량 Threshold ratio
	float fDarkThresholdRatio_L2 = (float)dPara[nPIdx++]/100.0;	// 주 검사 영역의 어두운 불량 Threshold ratio

	float fBrightThresholdRatio_OutLine_L2 = (float)dPara[nPIdx++]/100.0; // 외곽 검사 영역의 밝은불량 Threshold ratio
	float fDarkThresholdRatio_OutLine_L2 = (float)dPara[nPIdx++]/100.0; // 외곽 검사 영역의 어두운 불량 Threshold ratio

	int nBirghtFilterSize_L2 = (int)dPara[nPIdx++]; // 1차 필터링할 불량 크기
	int nDarkFilterSize_L2 = (int)dPara[nPIdx++]; // 1차 필터링할 불량 크기

	nPIdx++;//  Dummy => --------------------------------------------

	double dblTargetMeanGV = (double)dPara[nPIdx++]; // 현재 Parameter가 설정된 Cell의 기준 GV. 입력된 영상의 평균 GV가 해당 GV보다 높으면 전체적으로 밝기를 스케일 해준다.
	int nBlur_Base = (int)dPara[nPIdx++];			// 증폭하기 전에 실행할 Blur 크기
	int nBlur_Final = (int)dPara[nPIdx++];		// 증폭후 검사 타겟 이미지를 만들 실행할 Blur 크기
	int nBlur_Back = (int)dPara[nPIdx++];		// 증폭후 배경 이미지를 만들 실행할 Blur 크기

	nPIdx++;//  Dummy => --------------------------------------------

	int nAveMinStart = (int)dPara[nPIdx++];	//검사시 Threshold를 잡을때 평균 GV 대비 비율로 결정한다. 이때 히스토그램을 만들어 계산을 하게 되는데 이때 최소한 설정 GV보다 밝은 Pixel 들만 사용한다.
	double dblAveCutOffCountR_Min = (double)dPara[nPIdx++]/100.0; // 검사시 Threshold를 잡을때 평균 GV 대비 비율로 결정한다. 이때 히스토그램을 만들어 계산을 하게 되는데 히스토그램 전체 Pixel 개수가 설정한 수치 이상인 GV만 바탕으로서 유효하다고 판단한다.

	int nMaxGVAreaPartX = (int)dPara[nPIdx++]; // 증폭전 바탕 대비 너무 차이가 많이나는 Pixel 을 눌러줄때 사용하는 Parameter - 구역별 바탕 계산을 하게 되는데 이때 X 축 방향의 구역 을 몇 등분 할지 설정한다.
	int nMaxGVAreaPartY = (int)dPara[nPIdx++]; // 증폭전 바탕 대비 너무 차이가 많이나는 Pixel 을 눌러줄때 사용하는 Parameter - 구역별 바탕 계산을 하게 되는데 이때 Y 축 방향의 구역 을 몇 등분 할지 설정한다.
	double dblMinGVR = (double)dPara[nPIdx++]/100.0; // 증폭전 바탕 대비 너무 차이가 많이나는 Pixel 을 눌러줄때 사용하는 Parameter - 바탕 대비 몇 % 까지 차이가 나는 놈을 눌러 줄것이냐. 어두운 불량. 1.0이 최대
	double dblMaxGVR = (double)dPara[nPIdx++]/100.0; // 증폭전 바탕 대비 너무 차이가 많이나는 Pixel 을 눌러줄때 사용하는 Parameter - 바탕 대비 몇 % 까지 차이가 나는 놈을 눌러 줄것이냐. 밝은 불량. 1.0이 최대

	int nMaxGVAreaPartX_BGI = (int)dPara[nPIdx++];  // 증폭후 바탕 이미지 생성시 바탕 대비 너무 차이가 많이나는 Pixel 을 눌러줄때 사용하는 Parameter - 구역별 바탕 계산을 하게 되는데 이때 X 축 방향의 구역 을 몇 등분 할지 설정한다.
	int nMaxGVAreaPartY_BGI = (int)dPara[nPIdx++];  // 증폭후 바탕 이미지 생성시 바탕 대비 너무 차이가 많이나는 Pixel 을 눌러줄때 사용하는 Parameter - 구역별 바탕 계산을 하게 되는데 이때 Y 축 방향의 구역 을 몇 등분 할지 설정한다.
	double dblMinGVR_BGI = (double)dPara[nPIdx++]/100.0; // 증폭후 바탕 이미지 생성시 바탕 대비 너무 차이가 많이나는 Pixel 을 눌러줄때 사용하는 Parameter - 바탕 대비 몇 % 까지 차이가 나는 놈을 눌러 줄것이냐. 어두운 불량. 1.0이 최대
	double dblMaxGVR_BGI = (double)dPara[nPIdx++]/100.0; // 증폭후 바탕 이미지 생성시 바탕 대비 너무 차이가 많이나는 Pixel 을 눌러줄때 사용하는 Parameter - 바탕 대비 몇 % 까지 차이가 나는 놈을 눌러 줄것이냐. 밝은 불량. 1.0이 최대

	nPIdx++;//  Dummy => --------------------------------------------

	double dblResizeRatio = (double)dPara[nPIdx++];

	long	nOutLineForDelete	= (long)dPara[nPIdx++];		// 외곽 제거 Pixel	

	int nOutLineArea_01 = (int)dPara[nPIdx++];		// 외곽 검사 영역은 검사 Spec를 다르게 준다. 이때 외곽에서 얼마나 떨어진 지점까지 다르게 줄 것이냐를 결정한다.
	int nOutLineArea_02 = (int)dPara[nPIdx++];		// 외곽 검사 영역은 검사 Spec를 다르게 준다. 이때 외곽에서 얼마나 떨어진 지점까지 다르게 줄 것이냐를 결정한다.
	
	////////////////////////
	// 검사 영역 외곽 축소//
	////////////////////////

	CRect rectTemp(rectROI);
	rectTemp.left	+= nOutLineForDelete;
	rectTemp.top	+= nOutLineForDelete;
	rectTemp.right	-= nOutLineForDelete;
	rectTemp.bottom -= nOutLineForDelete;

	cv::Rect cropRect = cv::Rect(rectTemp.left, rectTemp.top, rectTemp.Width(), rectTemp.Height());	

	cv::Mat matSrcCrop =  matSrcImage(cropRect);
	cv::Mat matResizeTmp1;
	cv::Mat matResizeTmp2;
	cv::Mat matResizeTmp3;
	cv::Mat mat16Tmp1;
	cv::Mat mat16Tmp2;	
	cv::Mat mat16Tmp3;
	cv::Mat mat16Tmp4;	
	cv::Mat mat16InspImage;
	cv::Mat mat16BackgroundImage_M;
	cv::Mat mat16BackgroundImage_L;
	cv::Mat mat16BinBright_M1;
	cv::Mat mat16BinDark_M1;
	cv::Mat mat16BinBright_M2;
	cv::Mat mat16BinDark_M2;
	cv::Mat mat16BinBright_L1;
	cv::Mat mat16BinDark_L1;
	cv::Mat mat16BinBright_L2;
	cv::Mat mat16BinDark_L2;
	cv::Mat mat16BinResult_Bright;
	cv::Mat mat16BinResult_Dark;
	double dblBrightAverage_M;
	double dblDarkAverage_M;
	double dblBrightAverage_L;
	double dblDarkAverage_L;

	Size szBlur_Base = Size(nBlur_Base,nBlur_Base);
	Size szBlur_Final = Size(nBlur_Final,nBlur_Final);
	Size sznBlur_Back = Size(nBlur_Back,nBlur_Back);

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_Main_%02d_Src.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matSrcCrop);
	}

	//////////////////////
	//이미지 크기 재조정//
	//////////////////////

	cv::resize(matSrcCrop, matResizeTmp1, Size(matSrcCrop.cols/dblResizeRatio, matSrcCrop.rows/dblResizeRatio), CV_INTER_AREA);

	/////////////////////
	//타겟 평균 GV 적용//
	/////////////////////

	ApplyMeanGV(matResizeTmp1,dblTargetMeanGV);

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_Resize.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResizeTmp1);
	}

	//////////////////////
	// 기본 Blur를 준다.//
	//////////////////////
	cv::blur(matResizeTmp1,matResizeTmp2,szBlur_Base);
	cv::blur(matResizeTmp2,matResizeTmp3,szBlur_Base);

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_Filter_Base.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResizeTmp3);
	}

	////////////////////////////////////////////////////////////////////////
	//1차 주변대비 차이가 많이 나는 GV 눌러주기 - 증폭전				  //
	//구역별 평균보다 특정 GV 이상 높거나 낮은놈은 한계치를 설정하여 준다.//
	////////////////////////////////////////////////////////////////////////

	SetMinMax(matResizeTmp3,nMaxGVAreaPartX,nMaxGVAreaPartY,dblMinGVR,dblMaxGVR);

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_Filter_MinMax_01.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResizeTmp3);
	}

	/////////////
	//16bit변환//
	/////////////

	matResizeTmp3.convertTo(mat16Tmp1,CV_16U);

	/////////////////
	//제곱으로 증폭//
	/////////////////

	cv::pow(mat16Tmp1,2,mat16Tmp1);

	////////////////////
	//검사 이미지 생성//	
	////////////////////

	//작은 blur를 주어서 최종 이미지 생성 - 현재 25 mask
	cv::blur(mat16Tmp1,mat16InspImage,szBlur_Final);

	////////////////////
	//바탕 이미지 생성//
	////////////////////

	//구역별 평균보다 특정 GV 이상 높거나 낮은놈은 한계치를 설정하여 준다. - 여기서 바탕 만들기 전에 한번 더해준다.
	SetMinMax(mat16Tmp1,nMaxGVAreaPartX_BGI,nMaxGVAreaPartY_BGI,dblMinGVR_BGI,dblMaxGVR_BGI);

	//바탕화면 생성
	MakeBGImage(BG_METHOD_BLUR,mat16Tmp1,mat16BackgroundImage_M,sznBlur_Back); // 평균값 바탕화면 - 현재 150 mask
	MakeBGImage(BG_METHOD_PROFILE,mat16Tmp1,mat16BackgroundImage_L,sznBlur_Back); // lINE PROFILE 기반 바탕화면
	
	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_InspImage.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16InspImage/32);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_Back_M.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BackgroundImage_M/32);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_Back_L.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BackgroundImage_L/32);
	}

	/////////////
	//최종 결과//
	/////////////
		
	//Mean 필터 기반 바탕화면에 대한 불량 연산
	mat16Tmp1 = mat16InspImage - mat16BackgroundImage_M; //검사 이미지에서 바탕이미지를 빼준다. - 밝은 불량	
	mat16Tmp2 = mat16BackgroundImage_M - mat16InspImage; //바탕이미지에서 검사 이미지를 빼준다. - 어두운 불량

	//Line profile 기반 바탕화면에 대한 불량 연산
	mat16Tmp3 = mat16InspImage - mat16BackgroundImage_L; //검사 이미지에서 바탕이미지를 빼준다. - 밝은 불량	
	mat16Tmp4 = mat16BackgroundImage_L - mat16InspImage; //바탕이미지에서 검사 이미지를 빼준다. - 어두운 불량

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_FinBright_M.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16Tmp1);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_FinDark_M.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16Tmp2);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_FinBright_L.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16Tmp3);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_FinDark_L.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16Tmp4);
	}
	
	//Threshold 계산 - 상위 하위 특정 비율만큼 잘라내고 구한다. - Mean 필터 기반 바탕
	dblBrightAverage_M = GetAverage(mat16Tmp1,dblAveCutOffCountR_Min,nAveMinStart,255);
	dblDarkAverage_M = GetAverage(mat16Tmp2,dblAveCutOffCountR_Min,nAveMinStart,255);

	//Threshold 계산 - 상위 하위 특정 비율만큼 잘라내고 구한다. - Line profile 필터 기반 바탕
	dblBrightAverage_L = GetAverage(mat16Tmp3,dblAveCutOffCountR_Min,nAveMinStart,255);
	dblDarkAverage_L = GetAverage(mat16Tmp4,dblAveCutOffCountR_Min,nAveMinStart,255);


	//Mean 필터 기반 연산
	MakeThresholdImage(mat16Tmp1,mat16BinBright_M1,dblBrightAverage_M,fBrightThresholdRatio_M1,fBrightThresholdRatio_OutLine_M1,nOutLineArea_01); // 강시인 - 밝은 불량
	MakeThresholdImage(mat16Tmp1,mat16BinBright_M2,dblBrightAverage_M,fBrightThresholdRatio_M2,fBrightThresholdRatio_OutLine_M2,nOutLineArea_02); // 약시인 - 밝은 불량
	MakeThresholdImage(mat16Tmp2,mat16BinDark_M1,dblDarkAverage_M,fDarkThresholdRatio_M1,fDarkThresholdRatio_OutLine_M1,nOutLineArea_01); // 강시인 - 어두운 불량
	MakeThresholdImage(mat16Tmp2,mat16BinDark_M2,dblDarkAverage_M,fDarkThresholdRatio_M2,fDarkThresholdRatio_OutLine_M2,nOutLineArea_02); // 약시인 - 어두운 불량

	//Line profile 기반 연산	
	MakeThresholdImage(mat16Tmp3,mat16BinBright_L1,dblBrightAverage_L,fBrightThresholdRatio_L1,fBrightThresholdRatio_OutLine_L1,nOutLineArea_01); // 강시인 - 밝은 불량
	MakeThresholdImage(mat16Tmp3,mat16BinBright_L2,dblBrightAverage_L,fBrightThresholdRatio_L2,fBrightThresholdRatio_OutLine_L2,nOutLineArea_02); // 약시인 - 밝은 불량	
	MakeThresholdImage(mat16Tmp4,mat16BinDark_L1,dblDarkAverage_L,fDarkThresholdRatio_L1,fDarkThresholdRatio_OutLine_L1,nOutLineArea_01); // 강시인 - 어두운 불량
	MakeThresholdImage(mat16Tmp4,mat16BinDark_L2,dblDarkAverage_L,fDarkThresholdRatio_L2,fDarkThresholdRatio_OutLine_L2,nOutLineArea_02); // 약시인 - 어두운 불량

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinBright_M1.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinBright_M1);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinDark_M1.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinDark_M1);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinBright_M2.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinBright_M2);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinDark_M2.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinDark_M2);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinBright_L1.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinBright_L1);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinDark_L1.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinDark_L1);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinBright_L2.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinBright_L2);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinDark_L2.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinDark_L2);
	}

	///////////////////////////////////////////////
	//우선 1차 사이즈로 걸러낸다 -> 강시인,약시인//
	///////////////////////////////////////////////
	mat16BinResult_Bright = cv::Mat::zeros(mat16BinBright_M1.size(),CV_8UC1);
	mat16BinResult_Dark = cv::Mat::zeros(mat16BinDark_M1.size(),CV_8UC1);;

	CFeatureExtraction cFeatureExtraction;
	STRU_DEFECT_ITEM FilterJudgment;
	FilterJudgment.bDefectItemUse = TRUE;	
	FilterJudgment.Judgment[E_FEATURE_AREA].bUse = TRUE;
	FilterJudgment.Judgment[E_FEATURE_AREA].nSign = E_SIGN_GREATER;

	double dblScale_Area = dblResizeRatio*dblResizeRatio;
	//밝은 불량
	FilterJudgment.Judgment[E_FEATURE_AREA].dValue = nBirghtFilterSize_M1/dblScale_Area;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mat16BinBright_M1, mat16BinBright_M1, &FilterJudgment);
	cFeatureExtraction.DrawBlob(mat16BinResult_Bright, cv::Scalar(255,255,255), BLOB_DRAW_BLOBS, true);

	FilterJudgment.Judgment[E_FEATURE_AREA].dValue = nBirghtFilterSize_M2/dblScale_Area;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mat16BinBright_M2, mat16BinBright_M2, &FilterJudgment);
	cFeatureExtraction.DrawBlob(mat16BinResult_Bright, cv::Scalar(255,255,255), BLOB_DRAW_BLOBS, true);

	FilterJudgment.Judgment[E_FEATURE_AREA].dValue = nBirghtFilterSize_L1/dblScale_Area;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mat16BinBright_L1, mat16BinBright_L1, &FilterJudgment);
	cFeatureExtraction.DrawBlob(mat16BinResult_Bright, cv::Scalar(255,255,255), BLOB_DRAW_BLOBS, true);

	FilterJudgment.Judgment[E_FEATURE_AREA].dValue = nBirghtFilterSize_L2/dblScale_Area;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mat16BinBright_L2, mat16BinBright_L2, &FilterJudgment);
	cFeatureExtraction.DrawBlob(mat16BinResult_Bright, cv::Scalar(255,255,255), BLOB_DRAW_BLOBS, true);

	//어두운 불량
	FilterJudgment.Judgment[E_FEATURE_AREA].dValue = nDarkFilterSize_M1/dblScale_Area;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mat16BinDark_M1, mat16BinDark_M1, &FilterJudgment);
	cFeatureExtraction.DrawBlob(mat16BinResult_Dark, cv::Scalar(255,255,255), BLOB_DRAW_BLOBS, true);

	FilterJudgment.Judgment[E_FEATURE_AREA].dValue = nDarkFilterSize_M2/dblScale_Area;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mat16BinDark_M2, mat16BinDark_M2, &FilterJudgment);
	cFeatureExtraction.DrawBlob(mat16BinResult_Dark, cv::Scalar(255,255,255), BLOB_DRAW_BLOBS, true);

	FilterJudgment.Judgment[E_FEATURE_AREA].dValue = nDarkFilterSize_L1/dblScale_Area;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mat16BinDark_L1, mat16BinDark_L1, &FilterJudgment);
	cFeatureExtraction.DrawBlob(mat16BinResult_Dark, cv::Scalar(255,255,255), BLOB_DRAW_BLOBS, true);

	FilterJudgment.Judgment[E_FEATURE_AREA].dValue = nDarkFilterSize_L2/dblScale_Area;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mat16BinDark_L2, mat16BinDark_L2, &FilterJudgment);
	cFeatureExtraction.DrawBlob(mat16BinResult_Dark, cv::Scalar(255,255,255), BLOB_DRAW_BLOBS, true);

	// 중간 Image 확인
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinBright_01_FAfter.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinResult_Bright);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinDark_01_FAfter.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinResult_Dark);
	}

	// 원본 크기로 설정
	matBrightResultImage_01 = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);
	cv::resize(mat16BinResult_Bright, matBrightResultImage_01( cv::Rect(rectTemp.left, rectTemp.top, rectTemp.Width(), rectTemp.Height()) ), Size(rectTemp.Width(), rectTemp.Height()), 0, 0, CV_INTER_AREA);
	
	matDarkResultImage_01 = cv::Mat::zeros(matSrcImage.size(), CV_8UC1);
	cv::resize(mat16BinResult_Dark, matDarkResultImage_01( cv::Rect(rectTemp.left, rectTemp.top, rectTemp.Width(), rectTemp.Height()) ), Size(rectTemp.Width(), rectTemp.Height()), 0, 0, CV_INTER_AREA);
	
	matSrcCrop.release();
	matResizeTmp1.release();
	matResizeTmp2.release();
	matResizeTmp3.release();
	mat16Tmp1.release();
	mat16Tmp2.release();
	mat16Tmp3.release();
	mat16Tmp4.release();
	mat16InspImage.release();
	mat16BackgroundImage_M.release();
	mat16BinBright_M1.release();
	mat16BinDark_M1.release();
	mat16BinBright_M2.release();
	mat16BinDark_M2.release();
	mat16BinBright_L1.release();
	mat16BinDark_L1.release();
	mat16BinBright_L2.release();
	mat16BinDark_L2.release();
	mat16BinResult_Bright.release();;
	mat16BinResult_Dark.release();;

	return E_ERROR_CODE_TRUE;
}

void CInspectMura2::MakeThresholdImage(Mat &matOri,Mat &matDst,double dblAverage,float fThresholdR,double fThresholdR_Outline,int nOutLineArea)
{
	//Threshold 계산//
	int nThreshold = (int)(dblAverage*fThresholdR);
	int nThreshold_OutLine = (int)(dblAverage*fThresholdR_Outline);
		
	//16bit 이미지는 바로 이진화가 안되서 거치는 과정
	matDst = matOri.clone();
	MakeThresholdImageWithCenterAndOut(matDst,nOutLineArea,nThreshold,nThreshold_OutLine);

	//8비트로 변환
	matDst.convertTo(matDst,CV_8U);	

	//이진화
	cv::threshold(matDst, matDst, 0, 255.0, THRESH_BINARY);
}


void CInspectMura2::MakeBGImage(int nMethod,Mat &matSrcImage, Mat &matBGImage,Size szParam01)
{
	if(nMethod == BG_METHOD_BLUR)
	{
		MakeBGImage_Blur(matSrcImage, matBGImage,szParam01);
	}
	else if(nMethod == BG_METHOD_PROFILE)
	{
		MakeBGImage_Profile(matSrcImage, matBGImage);
	}
}

void CInspectMura2::MakeBGImage_Blur(Mat &matSrcImage, Mat &matBGImage,Size szParam01)
{
	cv::blur(matSrcImage,matBGImage,szParam01);
}
void CInspectMura2::MakeBGImage_Profile(Mat &matSrcImage, Mat &matBGImage)
{	
	int nRow,nCol;
	Mat matRow,matCol;

	nRow = matSrcImage.rows;
	nCol = matSrcImage.cols;

	MakeProfile(PROFILE_ROW,matSrcImage,matRow);
	MakeProfile(PROFILE_COL,matSrcImage,matCol);

	matBGImage.create(nRow,nCol,CV_16U);

	for(int i=0 ; i< nRow ; i++)
	{
		matCol.row(0).copyTo(matBGImage.row(i));
	}

	for(int i=0 ; i< nCol ; i++)
	{
		cv::add(matBGImage.col(i),matRow.col(0),matBGImage.col(i));
	}
	matBGImage /= 2.0;
}

//라인 프로파일을 만들어준다.
void CInspectMura2::MakeProfile(int nType,Mat &matSrcImage,Mat &matDstImage)
{
	int nRow,nCol,depth;	

	depth = matSrcImage.depth();
	nRow = matSrcImage.rows;
	nCol = matSrcImage.cols;

	if(nType == PROFILE_ROW)
	{
		matDstImage.create(nRow,1,CV_64F);		

		for(int i=0 ; i< nRow ; i++)
		{
			matDstImage.row(i) = cv::sum(matSrcImage.row(i))[0];
		}		

		matDstImage /= nCol;		

		matDstImage.convertTo(matDstImage,depth);		
	}
	if(nType == PROFILE_COL)
	{
		matDstImage.create(1,nCol,CV_64F);

		for(int i=0 ; i< nCol ; i++)
		{
			matDstImage.col(i) = cv::sum(matSrcImage.col(i))[0];
		}

		matDstImage /= nRow;

		matDstImage.convertTo(matDstImage,depth);
	}
}
//평균 GV값을 설정한 GV가 되도록 설정한다. - 알고리즘 특성상 평균 GV 값이 많이 차이가 나게되면 Threshold 문제가 생김
void CInspectMura2::ApplyMeanGV(Mat &matSrcImage,double dblTargetGV)
{
	double dblMean;
	Scalar m;
	m = cv::mean(matSrcImage);
	dblMean = m[0];

	//입력된 이미지가 타겟 GV보다 밝을 경우만 한다.
	//입력된 이미지가 타겟 GV보다어두울 경우 해주면 노이즈성 성분들도 증폭이 되어서 과검의 원인이 된다.
	if(dblMean > dblTargetGV) 
	{
		matSrcImage *= dblTargetGV/dblMean;
	}
}

//외곽과 중앙을 따로 Threshold 처리해주는 함수
void CInspectMura2::MakeThresholdImageWithCenterAndOut(Mat& matSrcImage,int nOutLine,int nThresholdCen,int nThresholdOut)
{
	Mat matImageTmp;
	int nImageSizeX = matSrcImage.cols;
	int nImageSizeY = matSrcImage.rows;

	matImageTmp = matSrcImage.colRange(nOutLine,nImageSizeX - nOutLine).rowRange(0,nOutLine); // 위
	matImageTmp = matImageTmp-nThresholdOut;

	matImageTmp = matSrcImage.colRange(nOutLine,nImageSizeX - nOutLine).rowRange(nImageSizeY - nOutLine,nImageSizeY); // 아래
	matImageTmp = matImageTmp-nThresholdOut;

	matImageTmp = matSrcImage.colRange(0,nOutLine); // 왼쪽
	matImageTmp = matImageTmp-nThresholdOut;

	matImageTmp = matSrcImage.colRange(nImageSizeX - nOutLine,nImageSizeX); // 오른쪽
	matImageTmp = matImageTmp-nThresholdOut;
	
	matImageTmp = matSrcImage.rowRange(nOutLine,nImageSizeY-nOutLine).colRange(nOutLine,nImageSizeX-nOutLine); // 중간
	matImageTmp = matImageTmp-nThresholdCen;
}

//구간별 평균을 내어서 평균 GV의 특정 비율 이상 값을 Min, Max값으로 눌러준다.
void CInspectMura2::SetMinMax(Mat& matSrcImage,int nMaxGVAreaPartX,int nMaxGVAreaPartY,double dblMinGVR,double dblMaxGVR)
{
	int nImageSizeX = matSrcImage.cols;
	int nImageSizeY = matSrcImage.rows;
	int nImageTermX = nImageSizeX / nMaxGVAreaPartX;
	int nImageTermY = nImageSizeY / nMaxGVAreaPartY;
	int nAreaStartX,nAreaEndX;
	int nAreaStartY,nAreaEndY;
	double dblMean,dblMin,dblMax;	
	Mat tmpImage;

	for(int y=0 ; y<nMaxGVAreaPartY ; y++)
	{
		nAreaStartY = y*nImageTermY;
		nAreaEndY =  (y+1)*nImageTermY;

		if(nAreaEndY > nImageSizeY-1)
			nAreaEndY = nImageSizeY-1;

		for(int x=0 ; x<nMaxGVAreaPartX ; x++)
		{
			nAreaStartX = x*nImageTermX;
			nAreaEndX =  (x+1)*nImageTermX;

			if(nAreaEndX > nImageSizeX-1)
				nAreaEndX = nImageSizeX-1;

			tmpImage = matSrcImage.colRange(nAreaStartX,nAreaEndX).rowRange(nAreaStartY,nAreaEndY);

			Scalar m;
			m = cv::mean(tmpImage);
			dblMean = m[0];

			dblMin = dblMean-(dblMean*dblMinGVR);
			dblMax = dblMean+(dblMean*dblMaxGVR);

			cv::max(tmpImage,dblMin,tmpImage);
			cv::min(tmpImage,dblMax,tmpImage);
		}
	}
}


//특정 GV 구간 평균 - 바탕에서 뺀 이미지에서 사용할 거라 최소 Pixel 개수 이외에는 전부 평균한다.
double CInspectMura2::GetAverage(Mat& matSrcImage,double dblAveCutOffCountR_Min,int nAveMinStartGV,int nAveMaxEndGV)
{
	double dblRet = 0;
	Mat matHisto;
	int nHistSize = (int)pow((double)256,(double)matSrcImage.depth());
	int nMinGV = nAveMinStartGV;
	int nMaxGV= nAveMaxEndGV;
	int nAveCutOffCount_Min = 0;
	float *pVal;
	double dblValSum = 0;
	double dblCountSum = 0;

	GetHistogram(matSrcImage,matHisto);	

	//최소 Threshold (dblAveCutOffCountR_Min)을 구한다. - 설정된 영역의 GV만 개수를 카운트해서 구함
	dblCountSum = 0;
	pVal = ((float*)matHisto.data)+nMinGV;
	for(int i = nMinGV ; i<nMaxGV ; i++)
	{
		dblCountSum += *pVal;
		pVal++;
	}

	nAveCutOffCount_Min = dblCountSum*dblAveCutOffCountR_Min;

	//평균을 구한다.
	dblValSum = 0;
	dblCountSum = 0;
	pVal = ((float*)matHisto.data)+nMinGV;

	for(int i = nMinGV ; i<nMaxGV ; i++)
	{
		if(*pVal > nAveCutOffCount_Min)
		{
			dblValSum += *pVal*i;
			dblCountSum += *pVal;
		}
		pVal++;
	}

	dblRet = dblValSum/dblCountSum;

	return dblRet;
}


long CInspectMura2::GetHistogram(Mat& matSrcImage, Mat& matHisto)
{
	int nBit = GetBitFromImageDepth(matSrcImage.depth());
	if(nBit == -1)	return E_ERROR_CODE_MURA_HISTO;

	int nHistSize = (int)pow((double)256,(double)nBit);
	float fHistRange[] = {(float)0, (float)nHistSize-1};
	const float *ranges[] = { fHistRange };

	cv::calcHist(&matSrcImage,1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;
}

int CInspectMura2::GetBitFromImageDepth(int nDepth)
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
