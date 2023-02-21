/************************************************************************
Mura2 �˰��� ���� �ҽ�
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
	//	function	: Main �Լ�
	//	made by		: ������
	//				: 2017. 04. 17
	//
	//	[in]	param 1	:	���� ����		/	CV_8UC1
	//
	//	[out]	param 2 :	���̳ʸ� ���� ��� /	CV_8UC1
	//						������ 2���� �Լ� ȣ���� ���� �Ʒ��� �����ؾ���.
	//
	//						vector<cv::Mat> matBins(3);
	//						matBins.at(0) = cv::Mat(matSrcImage.size(), CV_8UC1, Scalar(0));
	//						matBins.at(1) = cv::Mat(matSrcImage.size(), CV_8UC1, Scalar(0));
	//
	//						0 : Dark
	//						1 : Bright
	//
	//	[in]	param 3 :	�˻翵�� ROI
	//
	//	[in]	param 4	:	�Ű����� ( ���� ��� X )
	//
	//	[in]	param 5	:	�̹������� ���� ���
	//
	//	[in]	param 6	:	�̹������� ���� ���� ����
	//
	//	[return]		:	true / false
	//
	////////////////////////////////////////////////////////

	// �Ķ���� NULL�� ���.
	if( dPara == NULL )					return E_ERROR_CODE_EMPTY_PARA;
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if( matSrcBuffer.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
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

	long	nWidth	= (long)matSrcBuffer.cols;	// ���� ���� ũ��
	long	nHeight	= (long)matSrcBuffer.rows;	// ���� ���� ũ��

	// �ӽ÷� ���� �۰� ����
	CRect rectROI = new CRect(
		max(ptCorner[E_CORNER_LEFT_TOP].x,		ptCorner[E_CORNER_LEFT_BOTTOM].x	),
		max(ptCorner[E_CORNER_LEFT_TOP].y,		ptCorner[E_CORNER_RIGHT_TOP].y		),
		min(ptCorner[E_CORNER_RIGHT_TOP].x,		ptCorner[E_CORNER_RIGHT_BOTTOM].x	),
		min(ptCorner[E_CORNER_LEFT_BOTTOM].y,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	) );

	// �˻� ������ ���� ũ�⸦ ��� ���
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
		// ��
		CFeatureExtraction cFeatureExtraction;

		// EMD ���͸� �����ϵ���...
		cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer, matBrightResultImage_01, matDrawBuffer, 
			nCommonPara, E_DEFECT_COLOR_BRIGHT, _T("M2B_"), EngineerDefectJudgment, pResultBlob);

		// ��� �ܰ��� �׸��� ( Light SkyBlue )
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

		// ��� �ܰ��� ����
		cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt);
	}


	//Dark
	if( !matDarkResultImage_01.empty() )
	{
		// ��
		CFeatureExtraction cFeatureExtraction;

		// EMD ���͸� �����ϵ���...
		cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer, matDarkResultImage_01, matDrawBuffer, 
			nCommonPara, E_DEFECT_COLOR_DARK, _T("M2D_"), EngineerDefectJudgment, pResultBlob);

		// ��� �ܰ��� �׸��� ( Light SkyBlue )
		cFeatureExtraction.DrawBlob(matDrawBuffer, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

		// ��� �ܰ��� ����
		cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt);
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectMura2::LogicStart_Gray(cv::Mat& matSrcImage, cv::Mat &matDarkResultImage_01, cv::Mat &matBrightResultImage_01,
									CRect rectROI, double* dPara,int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment)
{
	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
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
	float fBrightThresholdRatio_M1 = (float)dPara[nPIdx++]/100.0;	// �� �˻� ������ �����ҷ� Threshold ratio
	float fDarkThresholdRatio_M1 = (float)dPara[nPIdx++]/100.0;	// �� �˻� ������ ��ο� �ҷ� Threshold ratio

	float fBrightThresholdRatio_OutLine_M1 = (float)dPara[nPIdx++]/100.0; // �ܰ� �˻� ������ �����ҷ� Threshold ratio
	float fDarkThresholdRatio_OutLine_M1 = (float)dPara[nPIdx++]/100.0; // �ܰ� �˻� ������ ��ο� �ҷ� Threshold ratio

	int nBirghtFilterSize_M1 = (int)dPara[nPIdx++]; // 1�� ���͸��� �ҷ� ũ��
	int nDarkFilterSize_M1 = (int)dPara[nPIdx++]; // 1�� ���͸��� �ҷ� ũ��
	
	nPIdx++;//  Dummy => --------------------------------------------

	float fBrightThresholdRatio_M2 = (float)dPara[nPIdx++]/100.0;	// �� �˻� ������ �����ҷ� Threshold ratio
	float fDarkThresholdRatio_M2 = (float)dPara[nPIdx++]/100.0;	// �� �˻� ������ ��ο� �ҷ� Threshold ratio

	float fBrightThresholdRatio_OutLine_M2 = (float)dPara[nPIdx++]/100.0; // �ܰ� �˻� ������ �����ҷ� Threshold ratio
	float fDarkThresholdRatio_OutLine_M2 = (float)dPara[nPIdx++]/100.0; // �ܰ� �˻� ������ ��ο� �ҷ� Threshold ratio

	int nBirghtFilterSize_M2 = (int)dPara[nPIdx++]; // 1�� ���͸��� �ҷ� ũ��
	int nDarkFilterSize_M2 = (int)dPara[nPIdx++]; // 1�� ���͸��� �ҷ� ũ��

	nPIdx++;//  Dummy => --------------------------------------------

	float fBrightThresholdRatio_L1 = (float)dPara[nPIdx++]/100.0;	// �� �˻� ������ �����ҷ� Threshold ratio
	float fDarkThresholdRatio_L1 = (float)dPara[nPIdx++]/100.0;	// �� �˻� ������ ��ο� �ҷ� Threshold ratio

	float fBrightThresholdRatio_OutLine_L1 = (float)dPara[nPIdx++]/100.0; // �ܰ� �˻� ������ �����ҷ� Threshold ratio
	float fDarkThresholdRatio_OutLine_L1 = (float)dPara[nPIdx++]/100.0; // �ܰ� �˻� ������ ��ο� �ҷ� Threshold ratio

	int nBirghtFilterSize_L1 = (int)dPara[nPIdx++]; // 1�� ���͸��� �ҷ� ũ��
	int nDarkFilterSize_L1 = (int)dPara[nPIdx++]; // 1�� ���͸��� �ҷ� ũ��

	nPIdx++;//  Dummy => --------------------------------------------

	float fBrightThresholdRatio_L2 = (float)dPara[nPIdx++]/100.0;	// �� �˻� ������ �����ҷ� Threshold ratio
	float fDarkThresholdRatio_L2 = (float)dPara[nPIdx++]/100.0;	// �� �˻� ������ ��ο� �ҷ� Threshold ratio

	float fBrightThresholdRatio_OutLine_L2 = (float)dPara[nPIdx++]/100.0; // �ܰ� �˻� ������ �����ҷ� Threshold ratio
	float fDarkThresholdRatio_OutLine_L2 = (float)dPara[nPIdx++]/100.0; // �ܰ� �˻� ������ ��ο� �ҷ� Threshold ratio

	int nBirghtFilterSize_L2 = (int)dPara[nPIdx++]; // 1�� ���͸��� �ҷ� ũ��
	int nDarkFilterSize_L2 = (int)dPara[nPIdx++]; // 1�� ���͸��� �ҷ� ũ��

	nPIdx++;//  Dummy => --------------------------------------------

	double dblTargetMeanGV = (double)dPara[nPIdx++]; // ���� Parameter�� ������ Cell�� ���� GV. �Էµ� ������ ��� GV�� �ش� GV���� ������ ��ü������ ��⸦ ������ ���ش�.
	int nBlur_Base = (int)dPara[nPIdx++];			// �����ϱ� ���� ������ Blur ũ��
	int nBlur_Final = (int)dPara[nPIdx++];		// ������ �˻� Ÿ�� �̹����� ���� ������ Blur ũ��
	int nBlur_Back = (int)dPara[nPIdx++];		// ������ ��� �̹����� ���� ������ Blur ũ��

	nPIdx++;//  Dummy => --------------------------------------------

	int nAveMinStart = (int)dPara[nPIdx++];	//�˻�� Threshold�� ������ ��� GV ��� ������ �����Ѵ�. �̶� ������׷��� ����� ����� �ϰ� �Ǵµ� �̶� �ּ��� ���� GV���� ���� Pixel �鸸 ����Ѵ�.
	double dblAveCutOffCountR_Min = (double)dPara[nPIdx++]/100.0; // �˻�� Threshold�� ������ ��� GV ��� ������ �����Ѵ�. �̶� ������׷��� ����� ����� �ϰ� �Ǵµ� ������׷� ��ü Pixel ������ ������ ��ġ �̻��� GV�� �������μ� ��ȿ�ϴٰ� �Ǵ��Ѵ�.

	int nMaxGVAreaPartX = (int)dPara[nPIdx++]; // ������ ���� ��� �ʹ� ���̰� ���̳��� Pixel �� �����ٶ� ����ϴ� Parameter - ������ ���� ����� �ϰ� �Ǵµ� �̶� X �� ������ ���� �� �� ��� ���� �����Ѵ�.
	int nMaxGVAreaPartY = (int)dPara[nPIdx++]; // ������ ���� ��� �ʹ� ���̰� ���̳��� Pixel �� �����ٶ� ����ϴ� Parameter - ������ ���� ����� �ϰ� �Ǵµ� �̶� Y �� ������ ���� �� �� ��� ���� �����Ѵ�.
	double dblMinGVR = (double)dPara[nPIdx++]/100.0; // ������ ���� ��� �ʹ� ���̰� ���̳��� Pixel �� �����ٶ� ����ϴ� Parameter - ���� ��� �� % ���� ���̰� ���� ���� ���� �ٰ��̳�. ��ο� �ҷ�. 1.0�� �ִ�
	double dblMaxGVR = (double)dPara[nPIdx++]/100.0; // ������ ���� ��� �ʹ� ���̰� ���̳��� Pixel �� �����ٶ� ����ϴ� Parameter - ���� ��� �� % ���� ���̰� ���� ���� ���� �ٰ��̳�. ���� �ҷ�. 1.0�� �ִ�

	int nMaxGVAreaPartX_BGI = (int)dPara[nPIdx++];  // ������ ���� �̹��� ������ ���� ��� �ʹ� ���̰� ���̳��� Pixel �� �����ٶ� ����ϴ� Parameter - ������ ���� ����� �ϰ� �Ǵµ� �̶� X �� ������ ���� �� �� ��� ���� �����Ѵ�.
	int nMaxGVAreaPartY_BGI = (int)dPara[nPIdx++];  // ������ ���� �̹��� ������ ���� ��� �ʹ� ���̰� ���̳��� Pixel �� �����ٶ� ����ϴ� Parameter - ������ ���� ����� �ϰ� �Ǵµ� �̶� Y �� ������ ���� �� �� ��� ���� �����Ѵ�.
	double dblMinGVR_BGI = (double)dPara[nPIdx++]/100.0; // ������ ���� �̹��� ������ ���� ��� �ʹ� ���̰� ���̳��� Pixel �� �����ٶ� ����ϴ� Parameter - ���� ��� �� % ���� ���̰� ���� ���� ���� �ٰ��̳�. ��ο� �ҷ�. 1.0�� �ִ�
	double dblMaxGVR_BGI = (double)dPara[nPIdx++]/100.0; // ������ ���� �̹��� ������ ���� ��� �ʹ� ���̰� ���̳��� Pixel �� �����ٶ� ����ϴ� Parameter - ���� ��� �� % ���� ���̰� ���� ���� ���� �ٰ��̳�. ���� �ҷ�. 1.0�� �ִ�

	nPIdx++;//  Dummy => --------------------------------------------

	double dblResizeRatio = (double)dPara[nPIdx++];

	long	nOutLineForDelete	= (long)dPara[nPIdx++];		// �ܰ� ���� Pixel	

	int nOutLineArea_01 = (int)dPara[nPIdx++];		// �ܰ� �˻� ������ �˻� Spec�� �ٸ��� �ش�. �̶� �ܰ����� �󸶳� ������ �������� �ٸ��� �� ���̳ĸ� �����Ѵ�.
	int nOutLineArea_02 = (int)dPara[nPIdx++];		// �ܰ� �˻� ������ �˻� Spec�� �ٸ��� �ش�. �̶� �ܰ����� �󸶳� ������ �������� �ٸ��� �� ���̳ĸ� �����Ѵ�.
	
	////////////////////////
	// �˻� ���� �ܰ� ���//
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

	// �߰� Image Ȯ��
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_Main_%02d_Src.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matSrcCrop);
	}

	//////////////////////
	//�̹��� ũ�� ������//
	//////////////////////

	cv::resize(matSrcCrop, matResizeTmp1, Size(matSrcCrop.cols/dblResizeRatio, matSrcCrop.rows/dblResizeRatio), CV_INTER_AREA);

	/////////////////////
	//Ÿ�� ��� GV ����//
	/////////////////////

	ApplyMeanGV(matResizeTmp1,dblTargetMeanGV);

	// �߰� Image Ȯ��
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_Resize.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResizeTmp1);
	}

	//////////////////////
	// �⺻ Blur�� �ش�.//
	//////////////////////
	cv::blur(matResizeTmp1,matResizeTmp2,szBlur_Base);
	cv::blur(matResizeTmp2,matResizeTmp3,szBlur_Base);

	// �߰� Image Ȯ��
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_Filter_Base.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResizeTmp3);
	}

	////////////////////////////////////////////////////////////////////////
	//1�� �ֺ���� ���̰� ���� ���� GV �����ֱ� - ������				  //
	//������ ��պ��� Ư�� GV �̻� ���ų� �������� �Ѱ�ġ�� �����Ͽ� �ش�.//
	////////////////////////////////////////////////////////////////////////

	SetMinMax(matResizeTmp3,nMaxGVAreaPartX,nMaxGVAreaPartY,dblMinGVR,dblMaxGVR);

	// �߰� Image Ȯ��
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_Filter_MinMax_01.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, matResizeTmp3);
	}

	/////////////
	//16bit��ȯ//
	/////////////

	matResizeTmp3.convertTo(mat16Tmp1,CV_16U);

	/////////////////
	//�������� ����//
	/////////////////

	cv::pow(mat16Tmp1,2,mat16Tmp1);

	////////////////////
	//�˻� �̹��� ����//	
	////////////////////

	//���� blur�� �־ ���� �̹��� ���� - ���� 25 mask
	cv::blur(mat16Tmp1,mat16InspImage,szBlur_Final);

	////////////////////
	//���� �̹��� ����//
	////////////////////

	//������ ��պ��� Ư�� GV �̻� ���ų� �������� �Ѱ�ġ�� �����Ͽ� �ش�. - ���⼭ ���� ����� ���� �ѹ� �����ش�.
	SetMinMax(mat16Tmp1,nMaxGVAreaPartX_BGI,nMaxGVAreaPartY_BGI,dblMinGVR_BGI,dblMaxGVR_BGI);

	//����ȭ�� ����
	MakeBGImage(BG_METHOD_BLUR,mat16Tmp1,mat16BackgroundImage_M,sznBlur_Back); // ��հ� ����ȭ�� - ���� 150 mask
	MakeBGImage(BG_METHOD_PROFILE,mat16Tmp1,mat16BackgroundImage_L,sznBlur_Back); // lINE PROFILE ��� ����ȭ��
	
	// �߰� Image Ȯ��
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
	//���� ���//
	/////////////
		
	//Mean ���� ��� ����ȭ�鿡 ���� �ҷ� ����
	mat16Tmp1 = mat16InspImage - mat16BackgroundImage_M; //�˻� �̹������� �����̹����� ���ش�. - ���� �ҷ�	
	mat16Tmp2 = mat16BackgroundImage_M - mat16InspImage; //�����̹������� �˻� �̹����� ���ش�. - ��ο� �ҷ�

	//Line profile ��� ����ȭ�鿡 ���� �ҷ� ����
	mat16Tmp3 = mat16InspImage - mat16BackgroundImage_L; //�˻� �̹������� �����̹����� ���ش�. - ���� �ҷ�	
	mat16Tmp4 = mat16BackgroundImage_L - mat16InspImage; //�����̹������� �˻� �̹����� ���ش�. - ��ο� �ҷ�

	// �߰� Image Ȯ��
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
	
	//Threshold ��� - ���� ���� Ư�� ������ŭ �߶󳻰� ���Ѵ�. - Mean ���� ��� ����
	dblBrightAverage_M = GetAverage(mat16Tmp1,dblAveCutOffCountR_Min,nAveMinStart,255);
	dblDarkAverage_M = GetAverage(mat16Tmp2,dblAveCutOffCountR_Min,nAveMinStart,255);

	//Threshold ��� - ���� ���� Ư�� ������ŭ �߶󳻰� ���Ѵ�. - Line profile ���� ��� ����
	dblBrightAverage_L = GetAverage(mat16Tmp3,dblAveCutOffCountR_Min,nAveMinStart,255);
	dblDarkAverage_L = GetAverage(mat16Tmp4,dblAveCutOffCountR_Min,nAveMinStart,255);


	//Mean ���� ��� ����
	MakeThresholdImage(mat16Tmp1,mat16BinBright_M1,dblBrightAverage_M,fBrightThresholdRatio_M1,fBrightThresholdRatio_OutLine_M1,nOutLineArea_01); // ������ - ���� �ҷ�
	MakeThresholdImage(mat16Tmp1,mat16BinBright_M2,dblBrightAverage_M,fBrightThresholdRatio_M2,fBrightThresholdRatio_OutLine_M2,nOutLineArea_02); // ����� - ���� �ҷ�
	MakeThresholdImage(mat16Tmp2,mat16BinDark_M1,dblDarkAverage_M,fDarkThresholdRatio_M1,fDarkThresholdRatio_OutLine_M1,nOutLineArea_01); // ������ - ��ο� �ҷ�
	MakeThresholdImage(mat16Tmp2,mat16BinDark_M2,dblDarkAverage_M,fDarkThresholdRatio_M2,fDarkThresholdRatio_OutLine_M2,nOutLineArea_02); // ����� - ��ο� �ҷ�

	//Line profile ��� ����	
	MakeThresholdImage(mat16Tmp3,mat16BinBright_L1,dblBrightAverage_L,fBrightThresholdRatio_L1,fBrightThresholdRatio_OutLine_L1,nOutLineArea_01); // ������ - ���� �ҷ�
	MakeThresholdImage(mat16Tmp3,mat16BinBright_L2,dblBrightAverage_L,fBrightThresholdRatio_L2,fBrightThresholdRatio_OutLine_L2,nOutLineArea_02); // ����� - ���� �ҷ�	
	MakeThresholdImage(mat16Tmp4,mat16BinDark_L1,dblDarkAverage_L,fDarkThresholdRatio_L1,fDarkThresholdRatio_OutLine_L1,nOutLineArea_01); // ������ - ��ο� �ҷ�
	MakeThresholdImage(mat16Tmp4,mat16BinDark_L2,dblDarkAverage_L,fDarkThresholdRatio_L2,fDarkThresholdRatio_OutLine_L2,nOutLineArea_02); // ����� - ��ο� �ҷ�

	// �߰� Image Ȯ��
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
	//�켱 1�� ������� �ɷ����� -> ������,�����//
	///////////////////////////////////////////////
	mat16BinResult_Bright = cv::Mat::zeros(mat16BinBright_M1.size(),CV_8UC1);
	mat16BinResult_Dark = cv::Mat::zeros(mat16BinDark_M1.size(),CV_8UC1);;

	CFeatureExtraction cFeatureExtraction;
	STRU_DEFECT_ITEM FilterJudgment;
	FilterJudgment.bDefectItemUse = TRUE;	
	FilterJudgment.Judgment[E_FEATURE_AREA].bUse = TRUE;
	FilterJudgment.Judgment[E_FEATURE_AREA].nSign = E_SIGN_GREATER;

	double dblScale_Area = dblResizeRatio*dblResizeRatio;
	//���� �ҷ�
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

	//��ο� �ҷ�
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

	// �߰� Image Ȯ��
	if( bImageSave )
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinBright_01_FAfter.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinResult_Bright);

		strTemp.Format(_T("%s\\%02d_%02d_%02d_MURA2_%02d_BinDark_01_FAfter.bmp"), strAlgPath, nImageNum, nCamNum, nROINumber, nSaveImageCount++);
		cv::imwrite((cv::String)(CStringA)strTemp, mat16BinResult_Dark);
	}

	// ���� ũ��� ����
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
	//Threshold ���//
	int nThreshold = (int)(dblAverage*fThresholdR);
	int nThreshold_OutLine = (int)(dblAverage*fThresholdR_Outline);
		
	//16bit �̹����� �ٷ� ����ȭ�� �ȵǼ� ��ġ�� ����
	matDst = matOri.clone();
	MakeThresholdImageWithCenterAndOut(matDst,nOutLineArea,nThreshold,nThreshold_OutLine);

	//8��Ʈ�� ��ȯ
	matDst.convertTo(matDst,CV_8U);	

	//����ȭ
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

//���� ���������� ������ش�.
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
//��� GV���� ������ GV�� �ǵ��� �����Ѵ�. - �˰��� Ư���� ��� GV ���� ���� ���̰� ���ԵǸ� Threshold ������ ����
void CInspectMura2::ApplyMeanGV(Mat &matSrcImage,double dblTargetGV)
{
	double dblMean;
	Scalar m;
	m = cv::mean(matSrcImage);
	dblMean = m[0];

	//�Էµ� �̹����� Ÿ�� GV���� ���� ��츸 �Ѵ�.
	//�Էµ� �̹����� Ÿ�� GV���پ�ο� ��� ���ָ� ����� ���е鵵 ������ �Ǿ ������ ������ �ȴ�.
	if(dblMean > dblTargetGV) 
	{
		matSrcImage *= dblTargetGV/dblMean;
	}
}

//�ܰ��� �߾��� ���� Threshold ó�����ִ� �Լ�
void CInspectMura2::MakeThresholdImageWithCenterAndOut(Mat& matSrcImage,int nOutLine,int nThresholdCen,int nThresholdOut)
{
	Mat matImageTmp;
	int nImageSizeX = matSrcImage.cols;
	int nImageSizeY = matSrcImage.rows;

	matImageTmp = matSrcImage.colRange(nOutLine,nImageSizeX - nOutLine).rowRange(0,nOutLine); // ��
	matImageTmp = matImageTmp-nThresholdOut;

	matImageTmp = matSrcImage.colRange(nOutLine,nImageSizeX - nOutLine).rowRange(nImageSizeY - nOutLine,nImageSizeY); // �Ʒ�
	matImageTmp = matImageTmp-nThresholdOut;

	matImageTmp = matSrcImage.colRange(0,nOutLine); // ����
	matImageTmp = matImageTmp-nThresholdOut;

	matImageTmp = matSrcImage.colRange(nImageSizeX - nOutLine,nImageSizeX); // ������
	matImageTmp = matImageTmp-nThresholdOut;
	
	matImageTmp = matSrcImage.rowRange(nOutLine,nImageSizeY-nOutLine).colRange(nOutLine,nImageSizeX-nOutLine); // �߰�
	matImageTmp = matImageTmp-nThresholdCen;
}

//������ ����� ��� ��� GV�� Ư�� ���� �̻� ���� Min, Max������ �����ش�.
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


//Ư�� GV ���� ��� - �������� �� �̹������� ����� �Ŷ� �ּ� Pixel ���� �̿ܿ��� ���� ����Ѵ�.
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

	//�ּ� Threshold (dblAveCutOffCountR_Min)�� ���Ѵ�. - ������ ������ GV�� ������ ī��Ʈ�ؼ� ����
	dblCountSum = 0;
	pVal = ((float*)matHisto.data)+nMinGV;
	for(int i = nMinGV ; i<nMaxGV ; i++)
	{
		dblCountSum += *pVal;
		pVal++;
	}

	nAveCutOffCount_Min = dblCountSum*dblAveCutOffCountR_Min;

	//����� ���Ѵ�.
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
