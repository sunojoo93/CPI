#include "StdAfx.h"
#include "InspectPad.h"
#include "Define.h"
#include "FeatureExtraction.h"
#include "ExportLibrary.h"

CString strMaskName[ROI_NUM] = {_T("PAD_CONTACT_T_MASK.bmp"), _T("PAD_MODULE_MASK.bmp"), _T("PAD_CONTACT_B_MASK.bmp")};

CInspectPad::CInspectPad(void)
{
	cMem=NULL;
}


CInspectPad::~CInspectPad(void)
{
}

bool CInspectPad::GetEnableMatching(Mat Src)
{
	// src : Sobel Image
	Mat matThres;
	Mat matX, matY;

	// 이진화
	cv::threshold(Src, matThres, 20, 255, THRESH_BINARY);

	Mat elementVer(3,3, CV_8U, Scalar(1));
	Mat elementHer(3,3, CV_8U, Scalar(1));

	uchar	bKernalY[3][3] = { 0,0,0, 1,1,1, 0,0,0};
	uchar	bKernalX[3][3] = { 0,1,0, 0,1,0, 0,1,0};

	for(long y = 0 ;  y < elementVer.rows ; y++ )
	{
		for(long x = 0; x < elementVer.cols ; x++ )
		{
			elementVer.at<uchar>(y,x) = bKernalX[y][x];
		}
	}

	for(long y = 0 ;  y < elementHer.rows ; y++ )
	{
		for(long x = 0; x < elementHer.cols ; x++ )
		{
			elementHer.at<uchar>(y,x) = bKernalY[y][x];
		}
	}

	// 수평 / 수직 Open 연산
	morphologyEx(matThres, matX, cv::MORPH_OPEN,  elementHer, Point(-1,-1), 3); 
	morphologyEx(matThres, matY, cv::MORPH_OPEN,  elementVer, Point(-1,-1), 3); 

	//레이블링
	vector< vector<Point> > contoursX, contoursY; // Vector for storing contour
	vector<Vec4i> hierarchyX, hierarchyY;

	findContours(matY, contoursY, hierarchyY,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

	findContours(matX, contoursX, hierarchyX,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

	if((int)contoursX.size() > 0 && (int)contoursY.size() > 0 )
		return true;
	else
		return false;
}


long CInspectPad::CalcSegmentArea(const int nSegSize, const int nImageW, const int nImageH, vector<Rect> &o_vecROI)
{

	o_vecROI.clear();

	int	nCntX, nCntY;
	if( nImageW > nSegSize )		nCntX = nImageW / nSegSize;
	else							nCntX = 1;
	if( nImageH > nSegSize )		nCntY = nImageH / nSegSize;
	else							nCntY = 1;


	for (int y=0 ; y < nCntY  ; y++)
	{
		for (int x=0 ; x < nCntX  ; x++)
		{
	
			int nPartSizeX, nPartSizeY;

			if( x == nCntX  - 1)
				nPartSizeX = nImageW - x * nSegSize;				// 마지막 조각은 이미지 끝까지 할당
			else					
				nPartSizeX = nSegSize;

			if( y == nCntY  - 1 )	
				nPartSizeY = nImageH - y * nSegSize;				// 마지막 조각은 이미지 끝까지 할당
			else					
				nPartSizeY = nSegSize;

			Rect rtTemp1( x * nSegSize , y * nSegSize , nPartSizeX  , nPartSizeY);

			o_vecROI.emplace_back(rtTemp1);

		}
	}

	return 0;
}


long	CInspectPad::GetPreMatchingInfo(Mat matSrcBuffer, Mat matReferBuffer,  vector<Rect >vecInspSrc,vector<Rect > vecInspTem, vector<sMatchingScore>		&o_vecMatchingInfo)
{
	vector<sMatchingScore>().swap(o_vecMatchingInfo);

	const int nPartCount = (int)vecInspTem.size();

	o_vecMatchingInfo.resize(nPartCount);


	Mat Draw;
	matSrcBuffer.copyTo(Draw);
	cv::cvtColor(Draw, Draw, COLOR_GRAY2RGB);
	bool bFirstSeg = true;
	double dMin, dMax; 
	Point ptMinLoc; Point ptMaxLoc;
	Rect ptMatching;

	for(long nCnt = 0; nCnt < nPartCount ; nCnt++)
	{
		Rect rtSource		=	vecInspSrc[nCnt];
		Rect rtTemplate		=	vecInspTem[nCnt];

		// 전처리를 할 경우 원본에서 깊은 복사
		Mat matSrcEdge; 
		Mat matTemEdge;
		matSrcBuffer(rtSource).copyTo(matSrcEdge);
		matReferBuffer(rtTemplate).copyTo(matTemEdge);

		Edge_BasicSobel(matSrcEdge, matSrcEdge);
		Edge_BasicSobel(matTemEdge, matTemEdge);
	
		// 첫 조각이 아니고서는 Matching 가능한 정보를 지녀야 한다.
		// X - Y 성분을 가져야 매칭 가능하다고 인정.
		if(!GetEnableMatching(matSrcEdge) && !bFirstSeg)
		{
			o_vecMatchingInfo[nCnt] = o_vecMatchingInfo[nCnt-1];

			int nX = vecInspSrc[nCnt].x -  vecInspSrc[nCnt- 1].x;
			int nY = vecInspSrc[nCnt].y -  vecInspSrc[nCnt- 1].y;
		}
		else
		{
			bFirstSeg = false; 

			// Score 버퍼 생성
			int result_cols = matSrcEdge.cols - matTemEdge.cols + 1;
			int result_rows = matSrcEdge.rows - matTemEdge.rows + 1;

			Mat	matScoreTemp, matScoreTemp2;
			matScoreTemp.create( result_rows, result_cols, CV_32FC1 );

			// 매칭
			matchTemplate(matSrcEdge, matTemEdge, matScoreTemp, CV_TM_CCOEFF_NORMED);

			// 정규화
			//normalize( matScore, matScore, 0, 1, NORM_MINMAX, -1, Mat() );

			// 최소값 및 최대값 구하기
			cv::minMaxLoc( matScoreTemp, &dMin, &dMax, &ptMinLoc, &ptMaxLoc, Mat() );

			ptMatching.x = ptMaxLoc.x ;
			ptMatching.y = ptMaxLoc.y ;
			ptMatching.width =  rtTemplate.width;
			ptMatching.height	=	rtTemplate.height;

			Mat matSrcPart = Mat::zeros(vecInspTem[nCnt].width, vecInspTem[nCnt].height, CV_8UC1); 
			matSrcEdge(ptMatching).copyTo(matSrcPart);

			o_vecMatchingInfo[nCnt].dScore		= dMax;
			o_vecMatchingInfo[nCnt].ptPosition	= ptMaxLoc;
		}

		//if(bImageSave)
		//{
		//	CString strPath1, strPath2;
		//	strPath1.Format(_T("D:\\%d.PAD_PART_Matching_%lf.jpg"), nCnt, dMax);
		//	strPath2.Format(_T("D:\\%d.PAD_PART_Temp.jpg"), nCnt);

		//	cv::cvtColor(matSrcEdge, matSrcEdge, COLOR_GRAY2RGB);

		//	// 직선 그리기
		//	cv::rectangle(matSrcEdge, ptMatching,	cv::Scalar(255, 0, 0), 1);

		//	// 영상 저장
		//	cv::imwrite((cv::String)(CStringA)strPath1, matSrcEdge);
		//	cv::imwrite((cv::String)(CStringA)strPath2, matTemEdge);

		//	Rect rtDraw;
		//	rtDraw = ptMatching;
		//	rtDraw.x = rtSource.x + ptMatching.x;
		//	rtDraw.y = rtSource.y + ptMatching.y;

		//	cv::rectangle(Draw, rtDraw,	cv::Scalar(255, 0, 0), 1);

		//}


	}

	//if(bImageSave)
	//{
	//	CString strPath1;
	//	strPath1.Format(_T("D:\\PAD_MatchingMap.jpg"));

	//	// 영상 저장
	//	cv::imwrite((cv::String)(CStringA)strPath1, Draw);
	//}

	return E_ERROR_CODE_TRUE;
}


long	CInspectPad::GetPreMatchingInfo2(Mat matSrcBuffer, Mat matReferBuffer,  vector<Rect >vecInspSrc,vector<Rect > vecInspTem, vector<sMatchingScore>		&o_vecMatchingInfo)
{
	vector<sMatchingScore>().swap(o_vecMatchingInfo);

	const int nPartCount = (int)vecInspTem.size();

	o_vecMatchingInfo.resize(nPartCount);

	Mat Draw;
	matSrcBuffer.copyTo(Draw);
	cv::cvtColor(Draw, Draw, COLOR_GRAY2RGB);

	for(long nCnt = 0; nCnt < nPartCount ; nCnt++)
	{
		Rect				rtSource		=	vecInspSrc[nCnt];
		Rect				rtTemplate		=	vecInspTem[nCnt];

		long nSizeX		= rtSource.width	- rtTemplate.width;
		long nSizeY		= rtSource.height	- rtTemplate.height;


		Mat matTemp( rtTemplate.height, rtTemplate.width, CV_8UC1, Scalar(255));
		Mat matSubResult = Mat::zeros(matTemp.size(), CV_8UC1);
		Mat matTemFinal =	matReferBuffer(rtTemplate);

		double dMinValue = 255 * matSubResult.rows * matSubResult.cols;

		int nPosX, nPosY;
		for (int y = 0 ; y < nSizeY; y++)
		{
			nPosY = rtSource.y + y;

			for (int x = 0; x < nSizeX ; x++)
			{
				nPosX = rtSource.x + x;

				Rect	rtSrcFinal;
				rtSrcFinal.y		=	 nPosY;
				rtSrcFinal.x		=	 nPosX;
				rtSrcFinal.width	=	rtTemplate.width;
				rtSrcFinal.height	=	rtTemplate.height;


				Mat matSrcFinal =	matSrcBuffer(rtSrcFinal);

				cv::absdiff(matTemFinal, matSrcFinal, matSubResult);


				double dMin = ( cv::sum(matSubResult)[0] ) / ( matSubResult.rows * matSubResult.cols);


				if( dMinValue > dMin )
				{
					dMinValue	= dMin;
					o_vecMatchingInfo[nCnt].dScore			= dMin;
					o_vecMatchingInfo[nCnt].ptPosition.x	= x;
					o_vecMatchingInfo[nCnt].ptPosition.y	= y;

				}
			}
		}

		if(false)
		{
// 실행 드라이브 D:\ 고정 아님 - 필요 시 InspectLibLog 의 GETDRV() 활용
// 			CString strPath1, strPath2;
// 			strPath1.Format(_T("D:\\%d.PAD_PART_Matching.jpg"), nCnt);
// 			strPath2.Format(_T("D:\\%d.PAD_PART_Temp.jpg"), nCnt);

			Mat matSrc;
			matSrcBuffer(rtSource).copyTo(matSrc);

			cv::cvtColor(matSrc, matSrc, COLOR_GRAY2RGB);

			// 직선 그리기
			Rect ptMatching(o_vecMatchingInfo[nCnt].ptPosition.x, o_vecMatchingInfo[nCnt].ptPosition.y, rtTemplate.width, rtTemplate.height);
			cv::rectangle(matSrc, ptMatching,	cv::Scalar(255, 0, 0), 1);

			// 영상 저장
// 			cv::imwrite((cv::String)(CStringA)strPath1, matSrc);
// 			cv::imwrite((cv::String)(CStringA)strPath2, matTemFinal);

			Rect rtDraw;
			rtDraw = ptMatching;
			rtDraw.x = rtSource.x + ptMatching.x;
			rtDraw.y = rtSource.y + ptMatching.y;

			cv::rectangle(Draw, rtDraw,	cv::Scalar(255, 0, 0), 1);

		}

	}

	if(false)
	{
// 실행 드라이브 D:\ 고정 아님 - 필요 시 InspectLibLog 의 GETDRV() 활용
// 		CString strPath1;
// 		strPath1.Format(_T("D:\\PAD_MatchingMap.jpg"));
// 
// 		// 영상 저장
// 		cv::imwrite((cv::String)(CStringA)strPath1, Draw);
	}

	return E_ERROR_CODE_TRUE;
}


// 남경 방식 : 템플릿 매칭으로 찾고, 찾은점 기준에서 Offset 만큼 이동시키며, 두 이미지의 차이값이 최소가 되는 부분을 찾는다. 그 부분에서의 나온게 불량 인정
// 장점 : 작은 불량은 자동으로 제거 된다.
// 단점 : 작은 불량을 잡지 못한다. 
// 보완점 : 밝기 보정 차이 필요. 두 이미지의 밝기 차이가 있을 경우 문제 발생. 
long CInspectPad::FindPadScratchDefect(Mat matMainBuffer, Mat matReferenceBuffer, Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	vector<Mat>		vecSrcBuffer;
	vector<Rect>	vecInspAreaROI;
	//파라미터 세팅

	bool	bImageSave			= (nCommonPara[1] > 0) ? true : false;	// 1 : 알고리즘 중간결과 Image Save 

	int nPos = 0;

	const	int		nSearchSizeX			=		(int)dPara[nPos++];
	const	int		nSearchSizeY			=		(int)dPara[nPos++];
	const	int		nMaskSizeX				=		(int)dPara[nPos++];
	const	int		nMaskSizeY				=		(int)dPara[nPos++];


	const	int		nUserPartition			=		(int)dPara[nPos++];
	const	int		nSegSize				=		(int)dPara[nPos++];
	const	int		nThresholdRtoS			=		(int)dPara[nPos++];
	const	int		nThresholdStoR			=		(int)dPara[nPos++];

	const	int		nOffsetSizeX			=		nSearchSizeX * 2;
	const	int		nOffsetSizeY			=		nSearchSizeY * 2;

	if(matMainBuffer.empty())
	{
		// 추후 에러 코드 추가 
		return 1;
	}

	if(matReferenceBuffer.empty())
	{
		// 추후 에러 코드 추가 
		return 1;
	}

	// 원본 이미지 확장 : 원본이미지는 실제 레퍼 크기보다 커야 한다.
	Rect rtSrcArea = rtInspArea;
	rtSrcArea.x			-= nSearchSizeX;
	rtSrcArea.y			-= nSearchSizeY;
	rtSrcArea.width		+= nOffsetSizeX;
	rtSrcArea.height	+= nOffsetSizeY;

	// 계산 원본 이미지 추출 
	Point	ptCalcOffset(rtSrcArea.tl() );		// 나중에 좌표 보정을 할 때, 사용 한다 

	Mat matSrcBuffer, matReferBuffer;
	matMainBuffer(rtSrcArea).copyTo(matSrcBuffer);
	matReferenceBuffer.copyTo(matReferBuffer);

	blur(matSrcBuffer, matSrcBuffer,cv::Size(3, 3));
	blur(matReferBuffer, matReferBuffer, cv::Size(3, 3));

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\PAD_SRC.bmp"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, matSrcBuffer);

		strTemp.Format(_T("%s\\PAD_TEM.bmp"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, matReferBuffer);

		//imwrite("D:\\PAD_SRC.bmp", matSrcBuffer);
		//imwrite("D:\\PAD_TEM.bmp", matReferBuffer);
	}

	Mat matResultBlobRtoS	 = Mat::zeros(rtInspArea.height, rtInspArea.width, CV_8UC1);
	Mat matResultBlobStoR	= Mat::zeros(rtInspArea.height, rtInspArea.width, CV_8UC1);
	Mat matResultGrayBuffer	= Mat::zeros(rtInspArea.height, rtInspArea.width, CV_8UC1);

	const	int		nSrcImageW		=	matSrcBuffer.cols;
	const	int		nSrcImageH		=	matSrcBuffer.rows;

	const	int		nTemImageW		=	matReferBuffer.cols;
	const	int		nTemImageH		=	matReferBuffer.rows;

	const	int		nMainImageW		=	matMainBuffer.cols;
	const	int		nMainImageH		=	matMainBuffer.rows;


	// 영역 분할
	vector<Rect >				vecInspTem;
	vector<sMatchingScore>		vecMatchingInfo;

	// 분할 영역 계산
	CalcSegmentArea(nSegSize,  nTemImageW, nTemImageH, vecInspTem);
	int nCount	= (int) vecInspTem.size();

	vector<Rect >				vecInspSrc(vecInspTem);

	for(long n=0; n < nCount ;n ++)
	{
		Rect rtTemp = vecInspSrc[n];
		rtTemp.width		+= nOffsetSizeX;
		rtTemp.height		+= nOffsetSizeY;

		vecInspSrc[n]		= rtTemp;
	}
	//GetPreMatchingInfo2(matSrcBuffer, matReferBuffer, vecInspSrc, vecInspTem, vecMatchingInfo);
	//// 분할 영역 : 매칭 점수 및 포인트 계산
	GetPreMatchingInfo(matSrcBuffer, matReferBuffer, vecInspSrc, vecInspTem, vecMatchingInfo);

	int nPartCount	= (int)vecMatchingInfo.size();

	for(long nCnt = 0; nCnt < nPartCount ; nCnt++)
	{
		Rect				rtSource		=	vecInspSrc[nCnt];
		Rect				rtTemplate		=	vecInspTem[nCnt];
		sMatchingScore		matchingInfo	=	vecMatchingInfo[nCnt];

		Mat matResultRtoS = matResultBlobRtoS(rtTemplate);
		Mat matResultStoR = matResultBlobStoR(rtTemplate);
		Mat matResultGray = matResultGrayBuffer(rtTemplate);

		Mat matTempRtoS = Mat::zeros(matResultRtoS.size(), CV_8UC1);
		Mat matTempStoR = Mat::zeros(matResultStoR.size(), CV_8UC1);
		Mat matSubResultRtoS (matResultRtoS.size(), CV_8UC1, Scalar(255));
		Mat matSubResultStoR( matResultStoR.size(), CV_8UC1, Scalar(255));



		for (int y= -nMaskSizeY ; y <= nMaskSizeY ; y++)
		{
			if( matchingInfo.ptPosition.y + y < 0 )															continue;
			if( matchingInfo.ptPosition.y + y  + rtTemplate.height  >= rtSource.y + rtSource.height )		continue;

			for (int x=-nMaskSizeX ; x <= nMaskSizeX ; x++)
			{
				if( matchingInfo.ptPosition.x + x < 0 )														continue;
				if( matchingInfo.ptPosition.x + x  + rtTemplate.width  >= rtSource.x + rtSource.width )		continue;
		
				Rect rtSrcFinal;

				rtSrcFinal.y		=	rtSource.y + matchingInfo.ptPosition.y + y;
				rtSrcFinal.x		=	rtSource.x + matchingInfo.ptPosition.x + x;
				rtSrcFinal.width	=	rtTemplate.width;
				rtSrcFinal.height	=	rtTemplate.height;
				if(matSrcBuffer.rows < rtSrcFinal.height + rtSrcFinal.y) matSrcBuffer.rows = rtSrcFinal.height + rtSrcFinal.y;


				Mat matTemFinal =	matReferBuffer(rtTemplate);
				Mat matSrcFinal =	matSrcBuffer(rtSrcFinal);

				if(bImageSave)
				{
					CString strTemp;
					strTemp.Format(_T("%s\\matRefer.jpg"), strAlgPath);
					cv::imwrite((cv::String)(CStringA)strTemp, matTemFinal);

					strTemp.Format(_T("%s\\matSrc.jpg"), strAlgPath);
					cv::imwrite((cv::String)(CStringA)strTemp, matSrcFinal);
				}

				cv::subtract(matTemFinal, matSrcFinal, matTempRtoS);
				cv::subtract(matSrcFinal, matTemFinal, matTempStoR);
				cv::min(matSubResultRtoS, matTempRtoS, matSubResultRtoS);
				cv::min(matSubResultStoR, matTempStoR, matSubResultStoR);

			}
		}

		matSubResultRtoS.copyTo(matResultRtoS);
		matSubResultStoR.copyTo(matResultStoR);

		Rect rtGrayArea;
		rtGrayArea.y		=	rtSource.y + matchingInfo.ptPosition.y;
		rtGrayArea.x		=	rtSource.x + matchingInfo.ptPosition.x;
		rtGrayArea.width	=	rtTemplate.width;
		rtGrayArea.height	=	rtTemplate.height;
		matSrcBuffer(rtGrayArea).copyTo(matResultGray);


		if(bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\Blob %d.PAD_PART_R-S.jpg"), strAlgPath);
			cv::imwrite((cv::String)(CStringA)strTemp, matResultRtoS);

			strTemp.Format(_T("%s\\Blob %d.PAD_PART_S-R.jpg"), strAlgPath);
			cv::imwrite((cv::String)(CStringA)strTemp, matResultStoR);

		}

	}

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\PadResult_Gray_R-S.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, matResultBlobRtoS);

		strTemp.Format(_T("%s\\PadResult_Gray_S-R.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, matResultBlobStoR);

		Mat SaveTemp = Mat::zeros(matResultBlobRtoS.size(), CV_8UC1);
		cv::add(matResultBlobRtoS, matResultBlobStoR, SaveTemp);
		strTemp.Format(_T("%s\\PadResult_Gray_ALL.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, SaveTemp);
	}

	cv::threshold(matResultBlobRtoS, matResultBlobRtoS, nThresholdRtoS, 255, THRESH_BINARY);

	cv::threshold(matResultBlobStoR, matResultBlobStoR, nThresholdStoR, 255, THRESH_BINARY);

	if(bImageSave)   
	{
		CString strTemp;
		strTemp.Format(_T("%s\\PadResult_Binary_R-S.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, matResultBlobRtoS);

		strTemp.Format(_T("%s\\PadResult_Binary_S-R.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, matResultBlobStoR);

		strTemp.Format(_T("%s\\PadResult_Binary_All.jpg"), strAlgPath);
		Mat SaveTemp = Mat::zeros(matResultBlobRtoS.size(), CV_8UC1);
		cv::bitwise_or(matResultBlobRtoS, matResultBlobStoR, SaveTemp);
		cv::imwrite((cv::String)(CStringA)strTemp, SaveTemp);

	}

	Mat matPartDraw = matDrawBuffer(rtInspArea);
	CFeatureExtraction cFeatureExtraction;
	int nPrevDefectCount = pResultBlob->nDefectCount;

	cFeatureExtraction.SetMem(cMem);
	cFeatureExtraction.DoDefectBlobJudgment(matResultGrayBuffer, matResultBlobRtoS, matPartDraw, 
		nCommonPara, E_DEFECT_COLOR_DARK, _T("PAD_SCRACTH"), EngineerDefectJudgment, pResultBlob, false);

	cFeatureExtraction.DrawBlob(matPartDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

	if (bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Pad_Draw_Image.jpg"), strAlgPath);
		cv::imwrite((cv::String)(CStringA)strTemp, matPartDraw);
	}

	int nCurrDefectCount = pResultBlob->nDefectCount;

	for(long n =nPrevDefectCount; n < nCurrDefectCount ;n++ )
	{
		pResultBlob->ptLT[n].x		+= rtSrcArea.x + vecMatchingInfo[0].ptPosition.x;
		pResultBlob->ptLT[n].y		+= rtSrcArea.y + vecMatchingInfo[0].ptPosition.y;
		pResultBlob->ptRT[n].x		+= rtSrcArea.x + vecMatchingInfo[0].ptPosition.x;	
		pResultBlob->ptRT[n].y		+= rtSrcArea.y + vecMatchingInfo[0].ptPosition.y;
		pResultBlob->ptRB[n].x		+= rtSrcArea.x + vecMatchingInfo[0].ptPosition.x;
		pResultBlob->ptRB[n].y		+= rtSrcArea.y + vecMatchingInfo[0].ptPosition.y;
		pResultBlob->ptLB[n].x		+= rtSrcArea.x + vecMatchingInfo[0].ptPosition.x;
		pResultBlob->ptLB[n].y		+= rtSrcArea.y + vecMatchingInfo[0].ptPosition.y;
	}

	return E_ERROR_CODE_TRUE;

}


long CInspectPad::FindPadScratchDefect2(Mat matMainBuffer, Mat matReferenceBuffer, Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{
	vector<Mat>		vecSrcBuffer;
	vector<Rect>	vecInspAreaROI;
	//파라미터 세팅

	bool	bImageSave			= (nCommonPara[1] > 0) ? true : false;	// 1 : 알고리즘 중간결과 Image Save 

	int nPos = 0;

	const	int		nSearchSizeX			=		(int)dPara[nPos++];
	const	int		nSearchSizeY			=		(int)dPara[nPos++];
	const	int		nMaskSizeX				=		(int)dPara[nPos++];
	const	int		nMaskSizeY				=		(int)dPara[nPos++];


	const	int		nUserPartition			=		(int)dPara[nPos++];
	const	int		nSegSize				=		(int)dPara[nPos++];
	const	int		nThresholdRtoS			=		(int)dPara[nPos++];
	const	int		nThresholdStoR			=		(int)dPara[nPos++];
	const	int		nROIArea				=		(int)dPara[nPos++];
	const	float	fGamma					=		(int)dPara[nPos++];


	if(matMainBuffer.empty())
	{
		// 추후 에러 코드 추가 
		return 1;
	}

	if(matReferenceBuffer.empty())
	{
		// 추후 에러 코드 추가 
		return 1;
	}

	Rect rtSrcArea = rtInspArea;
	Mat matSrcBuffer, matThrBuffer;
	matMainBuffer(rtSrcArea).copyTo(matSrcBuffer);

	matThrBuffer = Mat::zeros(matSrcBuffer.size(), CV_8UC1);

	CString strTemp;
	strTemp.Format(_T("%s\\Src_image.jpg"), strAlgPath);
	cv::imwrite((cv::String)(CStringA)strTemp, matSrcBuffer);

	//blur(matSrcBuffer, matSrcBuffer,cv::Size(3, 3));

	cv::GaussianBlur(matSrcBuffer, matSrcBuffer, Size(3,3), 0);

	strTemp.Format(_T("%s\\blur_image.jpg"), strAlgPath);
	cv::imwrite((cv::String)(CStringA)strTemp, matSrcBuffer);

	// Gamma
	GammaCorrection(matSrcBuffer, matThrBuffer, fGamma);

	strTemp.Format(_T("%s\\gamma_image.jpg"), strAlgPath);
	cv::imwrite((cv::String)(CStringA)strTemp, matThrBuffer);

	// Threshold

	cv::threshold(matThrBuffer,matThrBuffer, nSearchSizeY,255, THRESH_BINARY);

	strTemp.Format(_T("%s\\Thrs_image.jpg"), strAlgPath);
	cv::imwrite((cv::String)(CStringA)strTemp, matThrBuffer);

	//cv::threshold(matThrBuffer,matThrBuffer, nSearchSizeY,255, THRESH_BINARY);

	return true;
}

void CInspectPad::GammaCorrection(Mat matSrcBuffer,Mat matThrBuffer, float fGamma)
{

	register int i, j;

	// Get Image Size
	int		nWidth		= matSrcBuffer.cols;
	int		nHeight		= matSrcBuffer.rows;

	int		nMatSrcStep		= matSrcBuffer.step;

	int		nMatDstStep		= matThrBuffer.step;

	float fInputLevel = 0.0f;
	float fGammaCorrection = 1.0f / fGamma;
	int nTransLevel[256] = {0, };

	for ( i = 0 ; i < 256 ; i++)
	{
		nTransLevel[i] = (int)(255.0f * pow(fInputLevel, fGammaCorrection));
		fInputLevel += 0.00390625;
	}

	for( i = 0 ; i < nHeight ; i++ )
	{
		for( j = 0 ; j < nWidth ; j++ ) 
		{
			matThrBuffer.data[i * nMatDstStep + j] = (unsigned char)( nTransLevel[ matSrcBuffer.data[i * nMatSrcStep + j] ] );
		}
	}

}

bool CInspectPad::MARKPOSITION(cv::Mat Src, cv::Mat Mark, cv::Point2f& mPosition)
{
	//cv::Mat Match(Src.rows, Src.cols, CV_8UC1);
	Mat Match;
	cv::matchTemplate(Src, Mark, Match, CV_TM_CCOEFF_NORMED);
	//cv::normalize(Match, Match, 1, 0, CV_MINMAX);

	double minValue,maxValue;
	cv::Point2i minloc, ptMatch;
	cv::minMaxLoc(Match, &minValue, &maxValue, NULL, &ptMatch);
	cv::Rect rect(ptMatch.x, ptMatch.y, Mark.cols, Mark.rows); 

//   	if(maxValue < 0.7)
//   		return false;



// 	float* fMatchScore_arr = (float*)Match.ptr(ptMatch.y);
// 	double fMatchScore;
// 	//X 방향 Matching Rate Line Pitting
// 	vector<Point2f> ptMatchScore_X, ptMatchScore_Y;
// 
// 	int nSamplingNum = 5;
// 	for(int nX = ptMatch.x-nSamplingNum; nX <= ptMatch.x+nSamplingNum; nX++)
// 	{
// 
// 		if(nX < 0 || nX > Match.cols-1)	continue;
// 		else	fMatchScore = *(fMatchScore_arr+nX)*100;
// 
// 		ptMatchScore_X.push_back(Point2f(nX, fMatchScore));
// 	}
// 
// 	for(int nY = ptMatch.y-nSamplingNum; nY <= ptMatch.y+nSamplingNum; nY++)
// 	{
// 		if(nY < 0 || nY > Match.rows-1) continue;
// 		else{
// 			fMatchScore_arr = (float*)Match.ptr(nY);
// 			fMatchScore = *(fMatchScore_arr+ptMatch.x)*100;
// 
// 			ptMatchScore_Y.push_back(Point2f(nY, fMatchScore));
// 		}
// 	}

	//곡선 Pitting 상수
// 	long double dA,dB,dC;
// 	long double dX_Maxpt, dY_Maxpt;
// 
// 	if(calcCurveFit(ptMatchScore_X, dA, dB, dC)==-1)	return FALSE;
// 	dX_Maxpt = -1*(dB/(dA*2));
// 
// 	if(calcCurveFit(ptMatchScore_Y, dA, dB, dC)==-1)	return FALSE;
// 	dY_Maxpt = -1*(dB/(dA*2));

	mPosition = Point2f(ptMatch.x+Mark.cols/2, ptMatch.y+Mark.rows/2);

	return true;
}

//2017.11.13 Curve Fitting by LJM
//2차 방정식 Curve Fitting
//Y = AX^2+BX+C
//R[3][1]= {A},{B},{C};
long CInspectPad::calcCurveFit(std::vector <cv::Point2f>& ptSamples, long double& dA, long double& dB, long double& dC)
{
	if(ptSamples.size() < 3)
		return -1;

	Mat X(ptSamples.size(),3,CV_32FC1);//2차 방정식 X
	Mat Y(ptSamples.size(),1, CV_32FC1);//2차 방정식 Y
	Mat R(3,1,CV_32FC1);



	for(int nRow = 0; nRow < ptSamples.size(); nRow++)
	{
		X.at<float>(nRow, 0) = pow(ptSamples[nRow].x,2);
		X.at<float>(nRow, 1) = ptSamples[nRow].x;
		X.at<float>(nRow, 2) = 1;

		Y.at<float>(nRow, 0) = ptSamples[nRow].y;
	}

	Mat InvX(3,ptSamples.size(),CV_32FC1);
	InvX = X.inv(cv::DECOMP_SVD);

	R = InvX*Y;

	dA = (long double)R.at<float>(0,0);
	dB = (long double)R.at<float>(1,0);
	dC = (long double)R.at<float>(2,0);

	return 0;
}

//
//long CInspectPad::FindPadScratchDefect3(Mat matMainBuffer, Mat matReferenceBuffer, Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
//	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
//{
//	//파라미터 세팅
//	bool	bImageSave			= (nCommonPara[1] > 0) ? true : false;	// 1 : 알고리즘 중간결과 Image Save 
//
//	int nPos = 0;
//	const	int		nBlackThreshold				=		(int)dPara[nPos++];
//	const	int		nSubMaskThreshold			=		(int)dPara[nPos++];
//	int		nBlurlSize							=		(int)dPara[nPos++];
//	if((nBlurlSize%2)==0){	nBlurlSize++;	}
//	const	int		nMakeMask					=		(int)dPara[nPos++];
//
//	if(matMainBuffer.empty()) {
//		// 추후 에러 코드 추가 
//		return 1;
//	}
//
//	if(matReferenceBuffer.empty()) {
//		// 추후 에러 코드 추가 
//		return 1;
//	}
//
//	//Mark Load
//	cv::Mat Mark[PADMARK_NUM];
//	Mark[PADMARK_T] = cv::imread((LPCSTR) CStringA(MARKPATH_T), 0);
//	Mark[PADMARK_B] = cv::imread((LPCSTR) CStringA(MARKPATH_B), 0);
//
//
//	// 1. Refernce mark extract
//	Mat matReferBuffer;
//	matReferenceBuffer.copyTo(matReferBuffer);
//	
//	cv::Point2f mRefPosition[PADMARK_NUM];
//	cv::Rect mRefRect[PADMARK_NUM];
//
//	mRefRect[PADMARK_T] = cv::Rect(0, 0,						matReferBuffer.cols,	matReferBuffer.rows/2);
//	mRefRect[PADMARK_B] = cv::Rect(0, matReferBuffer.rows/2,	matReferBuffer.cols,	matReferBuffer.rows/2);
//
//	for(int i=0; i<PADMARK_NUM; i++){
//		MARKPOSITION(matReferBuffer(mRefRect[i]),	Mark[i],	mRefPosition[i]);
//		if(i == PADMARK_B) mRefPosition[PADMARK_B].y += matReferBuffer.rows/2;
//	}
//
//	//cv::circle( temp, mPosition[i], 3, -1, 1, 2, 0 );
//	/*cv::line(temp, mPosition[0], mPosition[1], cv::Scalar(0,0,255), 1,8,0);
//	cv::imwrite("mark1_result.bmp", temp );*/
//
//	// 2. Refernce mark align
//	double ang = atan2f((mRefPosition[1].y-mRefPosition[0].y),(mRefPosition[1].x-mRefPosition[0].x))*180/PI;
//
//	cv::Mat rRef;
//	cv::Mat mRotation = cv::getRotationMatrix2D(mRefPosition[0], (ang-90), 1 );
//	cv::warpAffine( matReferBuffer, rRef, mRotation, matReferBuffer.size() );
//	
//
//	// 3. Src mark extract
//	Mat matSrcBuffer;
//	matMainBuffer(rtInspArea).copyTo(matSrcBuffer);
//
//	cv::Point2f mSrcPosition[PADMARK_NUM];
//	cv::Rect mSrcRect[PADMARK_NUM];
//
//	mSrcRect[PADMARK_T] = cv::Rect(0, 0,					matSrcBuffer.cols,	matSrcBuffer.rows/2);
//	mSrcRect[PADMARK_B] = cv::Rect(0, matSrcBuffer.rows/2,	matSrcBuffer.cols,	matSrcBuffer.rows/2);
//
//	for(int i=0; i<PADMARK_NUM; i++){
//		MARKPOSITION(matSrcBuffer(mSrcRect[i]),	Mark[i],	mSrcPosition[i]);
//		if(i == PADMARK_B) mSrcPosition[PADMARK_B].y += matSrcBuffer.rows/2;
//	}
//	//cv::circle( temp, mPosition[i], 3, -1, 1, 2, 0 );
//	/*cv::line(temp, mPosition[0], mPosition[1], cv::Scalar(0,0,255), 1,8,0);
//	cv::imwrite("mark1_result.bmp", temp );*/
//
//	// 4. Src mark align
//	ang = atan2f((mSrcPosition[1].y-mSrcPosition[0].y),(mSrcPosition[1].x-mSrcPosition[0].x))*180/PI;
//
//	cv::Mat rSrc;
//	mRotation = cv::getRotationMatrix2D(mSrcPosition[0], (ang-90), 1 );
//	cv::warpAffine( matSrcBuffer, rSrc, mRotation, matSrcBuffer.size() );
//
//
//	// 5. SET Inspection area 
//	/*int rx = mRefPosition[0].x - mSrcPosition[0].x;	
//	int sx = 0;
//	if(rx<0) {
//		rx = 0;
//		sx = mSrcPosition[0].x - mRefPosition[0].x;
//	}
//
//	int ry = mRefPosition[0].y - mSrcPosition[0].y;	
//	int sy = 0;
//	if(ry<0) {
//		ry = 0;
//		sy = mSrcPosition[0].y - mRefPosition[0].y;
//	}
//
//	int pw;
//	if( (rRef.cols - rx) > (rSrc.cols - sx)) {
//		pw = rSrc.cols - sx;
//	} else {
//		pw = rRef.cols - rx;
//	}
//
//	int ph;
//	if( (rRef.rows - ry) > (rSrc.rows - sy)) {
//		ph = rSrc.rows - sy;
//	} else {
//		ph = rRef.rows - ry;
//	}
//
//	cv::Mat iReference;
//	cv::Rect iRefRect(rx, ry, pw, ph);
//	rRef(iRefRect).copyTo(iReference);
//
//	cv::Mat iSrc;
//	cv::Rect iSrcRect(sx, sy, pw, ph);
//	rSrc(iSrcRect).copyTo(iSrc);	*/
//
//
//	/*int rx = mRefPosition[0].x - mSrcPosition[0].x;	
//	int sx = 0;
//	if(rx<0) {
//		rx = 0;
//		sx = mSrcPosition[0].x - mRefPosition[0].x;
//	}
//
//	int ry = mRefPosition[0].y - mSrcPosition[0].y;	
//	int sy = 0;
//	if(ry<0) {
//		ry = 0;
//		sy = mSrcPosition[0].y - mRefPosition[0].y;
//	}
//
//	int pw;
//	if( (rRef.cols - rx) > (rSrc.cols - sx)) {
//		pw = rSrc.cols - sx;
//	} else {
//		pw = rRef.cols - rx;
//	}
//
//	int ph;
//	if( (rRef.rows - ry) > (rSrc.rows - sy)) {
//		ph = rSrc.rows - sy;
//	} else {
//		ph = rRef.rows - ry;
//	}*/
//	//55, 30, 190, 4350
//	//길이에 대해.. MASK, REF, SRC가 일치하도록.....
//	cv::Mat iReference;
//	cv::Rect iRefRect(mRefPosition[0].x - 50, mRefPosition[0].y , 180, 4300);
//	rRef(iRefRect).copyTo(iReference);
//
//	cv::Mat iSrc;
//	cv::Rect iSrcRect(mSrcPosition[0].x - 50,  mSrcPosition[0].y , 180, 4300);
//	rSrc(iSrcRect).copyTo(iSrc);	
//
//
//	Mat mBlack, mBurnt, mScratch;
//	mBlack = iReference - iSrc;
//
//	
//	// 6. Make Mask
//	Mat mMask;
//	if(nMakeMask>0){
//		cv::threshold(mBlack, mMask, nBlackThreshold, 255, THRESH_BINARY);
//		cv::imwrite((LPCSTR) CStringA(MASKPATH), mMask);
//		return E_ERROR_CODE_TRUE;
//	} 
//
//	// 6. Substract Mask
//	cv::threshold(mBlack, mBurnt, nBlackThreshold, 255, THRESH_BINARY);
//	mMask = cv::imread((LPCSTR) CStringA(MASKPATH), 0);
//
//	Mat mDefect;
//	//cv::Rect maskRect(0, 0, mBurnt.cols-10, mBurnt.rows-10);
//	//cv::subtract(mBurnt(maskRect), mMask(maskRect),test);
//	mDefect = mBurnt - mMask;
//
//	cv::GaussianBlur(mDefect, mDefect, Size(nBlurlSize,nBlurlSize), 0,0);
//	cv::threshold(mDefect, mDefect, nSubMaskThreshold, 255, THRESH_BINARY);
//
//
//	cv::imwrite("Ref.bmp",		iReference	);
//	cv::imwrite("Src.bmp",		iSrc		);
//	cv::imwrite("Sub.bmp",		mBlack		);
//	cv::imwrite("Result.bmp",	mBurnt		);
//	cv::imwrite("test.bmp",		mDefect		);
//	
//	/*Mat matResultBlob	 = Mat::zeros(matSrcBuffer.rows, matSrcBuffer.cols, CV_8UC1);
//	mDefect.copyTo(matResultBlob(iSrcRect));
//	Mat matPartDraw = matDrawBuffer(rtInspArea);
//
//	CFeatureExtraction cFeatureExtraction;
//	int nPrevDefectCount = pResultBlob->nDefectCount;
//
//	cFeatureExtraction.SetMem(cMem);
//	cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer, matResultBlob, matPartDraw, 
//		nCommonPara, E_DEFECT_COLOR_DARK, _T("PAD_SCRACTH"), EngineerDefectJudgment, pResultBlob, false);
//		cFeatureExtraction.DoDefectBlobJudgment(matSrcBuffer, matResultBlob, matPartDraw, 
//		nCommonPara, E_DEFECT_COLOR_DARK, _T("PAD_SCRACTH"), EngineerDefectJudgment, pResultBlob, false);
//
//	cFeatureExtraction.DrawBlob(matPartDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);
//*/
//
//	// CRUX Labeling
//	Mat ResultMat = Mat::zeros(matSrcBuffer.rows, matSrcBuffer.cols, CV_8UC1);
//	mDefect.copyTo(ResultMat(iSrcRect));
//	Mat matPartDraw = matDrawBuffer(rtInspArea);
//	CFeatureExtraction cFeatureExtraction;
//	cFeatureExtraction.SetMem(cMem);
//
//	int nPrevDefectCount = pResultBlob->nDefectCount;
//
//
//	cFeatureExtraction.DoDefectBlobSingleJudgment(matSrcBuffer, ResultMat, matPartDraw, nCommonPara,
//			E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob,  E_DEFECT_JUDGEMENT_APP_PAD_BURNT, false);
//	cFeatureExtraction.DrawBlob(matPartDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);
//
//	cFeatureExtraction.DoDefectBlobSingleJudgment(matSrcBuffer, ResultMat, matPartDraw, nCommonPara,
//			E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob,  E_DEFECT_JUDGEMENT_APP_PAD_SCRATCH, false);
//	cFeatureExtraction.DrawBlob(matPartDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);
//
//
//
//	int nCurrDefectCount = pResultBlob->nDefectCount;
//
//	for(long n =nPrevDefectCount; n < nCurrDefectCount ;n++ )
//	{
//		pResultBlob->ptLT[n].x		+= rtInspArea.x;
//		pResultBlob->ptLT[n].y		+= rtInspArea.y;
//		pResultBlob->ptRT[n].x		+= rtInspArea.x;	
//		pResultBlob->ptRT[n].y		+= rtInspArea.y;
//		pResultBlob->ptRB[n].x		+= rtInspArea.x;
//		pResultBlob->ptRB[n].y		+= rtInspArea.y;
//		pResultBlob->ptLB[n].x		+= rtInspArea.x;
//		pResultBlob->ptLB[n].y		+= rtInspArea.y;
//	}
//
//	return E_ERROR_CODE_TRUE;
//
//}



long CInspectPad::FindPadScratchDefect3(Mat matMainBuffer, Mat matReferenceBuffer, cv::Mat matDust, Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CString strCurRecipePath, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt)
{
	int		nImageClassify		=	nCommonPara[3];

	switch(nImageClassify)
	{
	case E_IMAGE_CLASSIFY_APP_ALL2:
		GoldenImageProcess( matMainBuffer, matReferenceBuffer, matDust, matDrawBuffer, rtInspArea, dPara, nCommonPara, strAlgPath, 
			EngineerDefectJudgment, pResultBlob, strCurRecipePath, stPadInfo, strContourTxt);
		break;
	default:
		ThresholdProcess( matMainBuffer, matReferenceBuffer, matDust, matDrawBuffer, rtInspArea, dPara, nCommonPara, strAlgPath, 
			EngineerDefectJudgment, pResultBlob, strCurRecipePath, stPadInfo, strContourTxt);
		break;
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectPad::GoldenImageProcess(Mat matMainBuffer, Mat matReferenceBuffer, cv::Mat matDust, Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CString strCurRecipePath, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt)
{
	PAD_DUST_FEATURE mPadDustFeature;
	int		nImageSaveInx		=	0;
	bool	bImageSave			=	(nCommonPara[1] > 0) ? true : false;	// 1 : 알고리즘 중간결과 Image Save 
	int		nImageClassify		=	nCommonPara[3];
	int		nCameraNum_			=	nCommonPara[4];						// 04 : Cam Number ( 카메라 번호 / SVI )

	long	nErrorCode = E_ERROR_CODE_TRUE;

	cv::Rect rtPadWork = stPadInfo[E_PAD_REF][nCameraNum_].GetRectCoord(0);

	//파라미터 세팅
	int nPos = 0;
	int nGV = 10;
	int nGVOnOff = 0;
	double	dBlackThreshold								=		(double)dPara[nPos++];
	const	int		nWhiteThreshold						=		(int)dPara[nPos++];
	int		nBlurlSize									=		(int)dPara[nPos++];
	if((nBlurlSize%2)==0){ nBlurlSize++; }
	const	int		nMakeRef2							=		(int)dPara[nPos++];		//Reference 1,2 만드는 Flag = 0: 정상진행, 1: Make Ref1, 2: Make Ref2
	const	int		nMakeMask							=		(int)dPara[nPos++];		//도금단자 부분 Mask 만드는 Flag
	const	int		nDivideNum							=		(int)dPara[nPos++];		//Pad 검사영역을 세로로 몇 등분해서 밝기 보정할 것인지
	const	double	dMaskGaus_Sigma						=		(double)dPara[nPos++];	
	const	int		nMaskBlur_Gaus						=		(int)dPara[nPos++];		//Mask 처리 Gausian Blur Size
	const	int		nMaskBlur							=		(int)dPara[nPos++];		
	const	int		nMaskDivde							=		(int)dPara[nPos++];		//Mask 처리하는 과정에서 몇 등분하여 처리 진행 할 것인지
	const	double	dMaskPrcTH							=		(double)dPara[nPos++];	//Mask 처리하는 부분의 Threshold
	const	double	dMaskMeanWeight						=		(double)dPara[nPos++];	//Mask 처리하는 부분에서 평균값으로 이진화시 평균값에 사용할 가중치
	const	int		nNone_R_Offset						=		(int)dPara[nPos++];		//비검영역 Offset Right쪽
	const	int		nCCutInterceptOffset				=		(int)dPara[nPos++];		//C-Cut Line 제거시 절편 값 Offset
	const	int		nCutGV								=		(int)dPara[nPos++];
	const	int		nTreshWidthOffset					=		(int)dPara[nPos++];		//Cutting 잔재물 제거 영역 Width Offset
	int		nDeleteMorph								=		(int)dPara[nPos++];		//짜잘한 작은 것들을 제거하기 위한 Morphology Size
	int		nResultMorph								=		(int)dPara[nPos++];		//결과 Defect 영상 근접불량 끼리 연결하기 위한 Morphology
	int		nResultMorphOffset							=		(int)dPara[nPos++];		//결과 Defect 영상 근접불량 끼리 연결한 팽창연산에서 다시 침식할 때 침식의 Mask를 얼마나 더 크게 할지의 Offset 값

	mPadDustFeature.fMeanGV								=		(float)dPara[nPos++];
	mPadDustFeature.fRoundness							=		(float)dPara[nPos++];
	mPadDustFeature.fSpecificGVRatio					=		(float)dPara[nPos++];
	mPadDustFeature.nMaxDustArea						=		(int)dPara[nPos++];	// Dust Area가 가질 수 있는 Maximum Size

	if(nDeleteMorph % 2 == 0)			nDeleteMorph++;
	if(nResultMorph % 2 == 0)			nResultMorph++;
	if(nResultMorphOffset % 2 != 0)		nResultMorphOffset++;	//Offset 값은 더하는 값임, 홀수+홀수 = 짝수 이기 때문에 홀수+짝수 형태로 만들어줌.

	if(matMainBuffer.empty()) {
		// 추후 에러 코드 추가 
		return E_ERROR_CODE_EMPTY_BUFFER;
	}

	if(matReferenceBuffer.empty()) {
		// 추후 에러 코드 추가 
		return E_ERROR_CODE_EMPTY_BUFFER;
	}

	// 1. Mark Load
	cv::Mat Mark[PADMARK_NUM];
	Rect rtMark[PADMARK_NUM] = {stPadInfo[E_PAD_ALIGN_MARK][0].GetRectCoord(0), stPadInfo[E_PAD_ALIGN_MARK][0].GetRectCoord(1)};
	Mark[PADMARK_T] = rtMark[0].y < rtMark[1].y ? cvarrToMat(stPadInfo[E_PAD_ALIGN_MARK][0].GetImage(0)) : cvarrToMat(stPadInfo[E_PAD_ALIGN_MARK][0].GetImage(1));
	Mark[PADMARK_B] = rtMark[0].y < rtMark[1].y ? cvarrToMat(stPadInfo[E_PAD_ALIGN_MARK][0].GetImage(1)) : cvarrToMat(stPadInfo[E_PAD_ALIGN_MARK][0].GetImage(0));

	if (Mark[PADMARK_T].empty() || Mark[PADMARK_B].empty())
	{
		return E_ERROR_CODE_EMPTY_BUFFER;
	}

	cv::Mat mImgBuffer[IMG_NUM];
	cv::Mat mRImgBuffer[IMG_NUM];
	cv::Point2f pPosition[IMG_NUM][PADMARK_NUM];

	cv::Mat mDustImg;

	// 2. Src mark extract
	GetCheckROIOver(rtInspArea, Rect(0,0,matMainBuffer.cols,matMainBuffer.rows), rtInspArea);
	matMainBuffer(rtInspArea).copyTo(mImgBuffer[SRC]);
	matDust(rtInspArea).copyTo(mDustImg);

	if(bImageSave)   
	{
		CString strTemp;
		strTemp.Format(_T("%s\\Pad_src.jpg"),		strAlgPath);	cv::imwrite((cv::String)(CStringA)strTemp, mImgBuffer[SRC]);
	}

	cv::Rect mSrcRect[PADMARK_NUM];
	mSrcRect[PADMARK_T] = cv::Rect(0, 0,						mImgBuffer[SRC].cols,	mImgBuffer[SRC].rows/2);
	mSrcRect[PADMARK_B] = cv::Rect(0, mImgBuffer[SRC].rows/2,	mImgBuffer[SRC].cols,	mImgBuffer[SRC].rows/2);

	for(int i=0; i<PADMARK_NUM; i++){
		MARKPOSITION(mImgBuffer[SRC](mSrcRect[i]),	Mark[i],	pPosition[SRC][i]);
		if(i == PADMARK_B) pPosition[SRC][PADMARK_B].y += mImgBuffer[SRC].rows/2;
	}


	// 3. Src mark align
	double ang = atan2f((pPosition[SRC][PADMARK_B].y-pPosition[SRC][PADMARK_T].y),(pPosition[SRC][PADMARK_B].x-pPosition[SRC][PADMARK_T].x))*180/CV_PI;

	cv::Mat rSrc;
	cv::Mat mRotation = cv::getRotationMatrix2D(pPosition[SRC][PADMARK_T], (ang-90), 1 );
	cv::warpAffine( mImgBuffer[SRC], mRImgBuffer[SRC], mRotation, mImgBuffer[SRC].size() );
	cv::warpAffine( mDustImg, mDustImg, mRotation, mDustImg.size());

	CString strRef2Path, strRefPath;
	strRef2Path.Format(_T("%s\\%d_Ref2.bmp"), strCurRecipePath, nImageClassify);
	strRefPath.Format(_T("%s\\%d_Ref.bmp"), strCurRecipePath, nImageClassify);

	if(nMakeRef2 == 1) {
		cv::imwrite((cv::String)(CStringA) strRefPath, mRImgBuffer[SRC]);
		return E_ERROR_CODE_TRUE;
	}
	else if (nMakeRef2 == 2) {
		cv::imwrite((cv::String)(CStringA)strRef2Path, mRImgBuffer[SRC]);
		return E_ERROR_CODE_TRUE;
	}

	mImgBuffer[REF2] = cv::imread((cv::String)(CStringA)strRefPath, 0);
	if (mImgBuffer[REF2].empty())		return E_ERROR_CODE_PAD_REF_IMG_EMPTY_BUFFER;

	mImgBuffer[REF3] = cv::imread((cv::String)(CStringA)strRef2Path, 0);
	if (mImgBuffer[REF3].empty())	return E_ERROR_CODE_PAD_REF_IMG_EMPTY_BUFFER;

	if(nImageClassify == E_IMAGE_CLASSIFY_APP_ALL2 ) matReferenceBuffer.copyTo(mImgBuffer[REF1]);
	else											 
	{
		mImgBuffer[REF2].copyTo(mImgBuffer[REF1]);
		mImgBuffer[REF3].copyTo(mImgBuffer[REF2]);
	}


	cv::Rect mRefRect[PADMARK_NUM];
	mRefRect[PADMARK_T] = cv::Rect(0, 0,						mImgBuffer[REF1].cols,	mImgBuffer[REF1].rows/2);
	mRefRect[PADMARK_B] = cv::Rect(0, mImgBuffer[REF1].rows/2,	mImgBuffer[REF1].cols,	mImgBuffer[REF1].rows/2);

	for(int i=0; i<PADMARK_NUM; i++){
		MARKPOSITION(mImgBuffer[REF1](mRefRect[i]),	Mark[i],	pPosition[REF1][i]);
		if(i == PADMARK_B) pPosition[REF1][PADMARK_B].y += mImgBuffer[REF1].rows/2;
	}

	// 5. REF1 mark align
	ang = atan2f((pPosition[REF1][PADMARK_B].y-pPosition[REF1][PADMARK_T].y),(pPosition[REF1][PADMARK_B].x-pPosition[REF1][PADMARK_T].x))*180/CV_PI;

	mRotation = cv::getRotationMatrix2D(pPosition[REF1][PADMARK_T], (ang-90), 1 );
	cv::warpAffine( mImgBuffer[REF1], mRImgBuffer[REF1], mRotation, mImgBuffer[REF1].size() );

	// 6. REF2 mark extract	
	cv::Rect mRefRect2[PADMARK_NUM];
	mRefRect2[PADMARK_T] = cv::Rect(0, 0,						mImgBuffer[REF2].cols,	mImgBuffer[REF2].rows/2);
	mRefRect2[PADMARK_B] = cv::Rect(0, mImgBuffer[REF2].rows/2,	mImgBuffer[REF2].cols,	mImgBuffer[REF2].rows/2);

	for(int i=0; i<PADMARK_NUM; i++){
		MARKPOSITION(mImgBuffer[REF2](mRefRect2[i]),	Mark[i],	pPosition[REF2][i]);
		if(i == PADMARK_B) pPosition[REF2][PADMARK_B].y += mImgBuffer[REF2].rows/2;
	}

	// 7. Refernce mark align
	ang = atan2f((pPosition[REF2][PADMARK_B].y-pPosition[REF2][PADMARK_T].y),(pPosition[REF2][PADMARK_B].x-pPosition[REF2][PADMARK_T].x))*180/CV_PI;

	mRotation = cv::getRotationMatrix2D(pPosition[REF2][0], (ang-90), 1 );
	cv::warpAffine( mImgBuffer[REF2], mRImgBuffer[REF2], mRotation, mImgBuffer[REF2].size() );

	/////////////////////////////////////////////
	//////Test  패드 밝기 높은 이미지 한번더 뺌
	cv::Rect mRefRect3[PADMARK_NUM];
	mRefRect3[PADMARK_T] = cv::Rect(0, 0,						mImgBuffer[REF3].cols,	mImgBuffer[REF3].rows/2);
	mRefRect3[PADMARK_B] = cv::Rect(0, mImgBuffer[REF3].rows/2,	mImgBuffer[REF3].cols,	mImgBuffer[REF3].rows/2);

	for(int i=0; i<PADMARK_NUM; i++){
		MARKPOSITION(mImgBuffer[REF3](mRefRect3[i]),	Mark[i],	pPosition[REF3][i]);
		if(i == PADMARK_B) pPosition[REF3][PADMARK_B].y += mImgBuffer[REF3].rows/2;
	}

	// 7. Refernce mark align
	ang = atan2f((pPosition[REF3][PADMARK_B].y-pPosition[REF3][PADMARK_T].y),(pPosition[REF3][PADMARK_B].x-pPosition[REF3][PADMARK_T].x))*180/CV_PI;

	mRotation = cv::getRotationMatrix2D(pPosition[REF3][0], (ang-90), 1 );
	cv::warpAffine( mImgBuffer[REF3], mRImgBuffer[REF3], mRotation, mImgBuffer[REF3].size() );

	//PAD에서 검사에 불필요한 부분 제거
	cv::Mat* mtNoneArea = NULL;
	DeleteNoneArea_Match(mRImgBuffer, IMG_NUM, mtNoneArea, stPadInfo);

	int nROICount = stPadInfo[E_PAD_INSP_AREA][0].GetRoiCount();

	Rect* rtROI						= new Rect[nROICount];	//ROI 저장된 좌표 값
	Mat* mtMatch					= new Mat[nROICount];	//ROI 저장된 영상
	Mat* mtMask						= new Mat[nROICount];	//Mask 영상

	CString strMatch;

	int nROI_OffsetX = 10; //원본영상에서 Matching을 진행할 영역의 확장 Offset 크기
	int nROI_OffsetY = 30;

	for(int nROIInx = 0; nROIInx < nROICount; nROIInx++)
	{
		mtMatch[nROIInx] = cvarrToMat(stPadInfo[E_PAD_INSP_AREA][nCameraNum_].GetImage(nROIInx));
		if(nMakeMask)
		{
			nErrorCode = MakeMask(mtMatch, mtMask, strCurRecipePath, nROIInx);
			if(nErrorCode != E_ERROR_CODE_TRUE)
				return nErrorCode;
			strMatch.Format(_T("%s\\PAD_INSP_INFO\\PadRoi\\PadRoiInsp_img_Mask_%02d_CAM%02d.bmp"), strCurRecipePath, nROIInx, nCameraNum_);	
			imwrite(CSTR2PCH(strMatch), mtMask[nROIInx]);
			return E_ERROR_CODE_TRUE;
		}
		else
		{
			strMatch.Format(_T("%s\\PAD_INSP_INFO\\PadRoi\\PadRoiInsp_img_Mask_%02d_CAM%02d.bmp"), strCurRecipePath, nROIInx, nCameraNum_);			
			mtMask[nROIInx]  = imread((cv::String)(CStringA)strMatch, IMREAD_GRAYSCALE);
		}
		rtROI[nROIInx] = stPadInfo[E_PAD_INSP_AREA][nCameraNum_].GetRectCoord(nROIInx);

		//PAD 검사 영역 좌표로 보정
		rtROI[nROIInx] = Rect(rtROI[nROIInx].x - rtPadWork.x - nROI_OffsetX, rtROI[nROIInx].y - rtPadWork.y - nROI_OffsetY, 
			rtROI[nROIInx].width + nROI_OffsetX * 2, rtROI[nROIInx].height + nROI_OffsetY * 2);

		GetCheckROIOver(rtROI[nROIInx], Rect(0, 0, mRImgBuffer[SRC].cols, mRImgBuffer[SRC].rows), rtROI[nROIInx]);
	}

	//ROI 영상 크기와 Mask 영상 크기가 다르다면 (Mask Teaching이 안 됬다는 얘기)
	if(!CheckMask(mtMatch, mtMask, nROICount))
	{
		delete[] rtROI;
		delete[] mtMatch;
		delete[] mtMask;

		return E_ERROR_CODE_PAD_MASK_SIZE;
	}

	cv::Mat* mtMatchROI;									//원본영상에서 Matching을 진행 할 영역
	cv::Mat* mtApartInsp;									//Matching 진행 하여 별도 분리 검사 진행하는 영역 영상
	cv::Rect* rtApartInsp;									//Matching 진행 하여 별도 분리 검사 진행하는 영역 좌표
	cv::Mat mtMatchRst;	
	cv::Mat	mtComImgMask[IMG_NUM];									//ROI 영역을 제외한 나머지 부분

	double dMatchRate;
	cv::Point ptMatch;		//Matching 해서 나온 정수 좌표

	mtMatchROI			= new cv::Mat[nROICount];
	mtApartInsp			= new cv::Mat[nROICount];
	rtApartInsp			= new cv::Rect[nROICount];

	for(int nROIInx = 0; nROIInx < nROICount; nROIInx++)
	{
		mtMatchROI[nROIInx] = mRImgBuffer[SRC](rtROI[nROIInx]);

		if(mtMatchROI[nROIInx].cols < mtMatch[nROIInx].cols || mtMatchROI[nROIInx].rows < mtMatch[nROIInx].rows)
			return E_ERROR_CODE_PAD_INSP_ROI_OVER;

		cv::matchTemplate(mtMatchROI[nROIInx], mtMatch[nROIInx], mtMatchRst, CV_TM_CCOEFF_NORMED);
		cv::minMaxLoc(mtMatchRst, NULL, &dMatchRate, NULL, &ptMatch);

		rtApartInsp[nROIInx] = cv::Rect(rtROI[nROIInx].x + ptMatch.x, rtROI[nROIInx].y + ptMatch.y, mtMatch[nROIInx].cols, mtMatch[nROIInx].rows);
		mtMatchROI[nROIInx](Rect(ptMatch.x, ptMatch.y, mtMatch[nROIInx].cols, mtMatch[nROIInx].rows)).copyTo(mtApartInsp[nROIInx]);
	}

	MaskProcess(mtApartInsp, mtMask, nROICount, dMaskGaus_Sigma, nMaskBlur_Gaus, nMaskBlur, dMaskPrcTH, dMaskMeanWeight, strAlgPath, mtMatch, nMaskDivde, bImageSave);

	/////////////////////////////////////////////
	// 8. Set inspection area
	vector<int> sx, sy, mw, mh;	//십자Mark Top
	vector<int> sx_b, sy_b;		//십자Mark Bottom

	for(int i=0; i<IMG_NUM; i++)
	{
		sx.push_back(pPosition[i][PADMARK_T].x);
		sy.push_back(pPosition[i][PADMARK_T].y);
		sx_b.push_back(pPosition[i][PADMARK_B].x);
		sy_b.push_back(pPosition[i][PADMARK_B].y);
		mw.push_back(mRImgBuffer[i].cols - pPosition[i][PADMARK_T].x);
		mh.push_back(mRImgBuffer[i].rows - pPosition[i][PADMARK_T].y);
	}
	sort(sx.begin(), sx.end());
	sort(sy.begin(), sy.end());
	sort(sx_b.begin(), sx_b.end());
	sort(sy_b.begin(), sy_b.end());
	sort(mw.begin(), mw.end());
	sort(mh.begin(), mh.end());

	cv::Mat comImg[IMG_NUM];

	//회전 시 생기는 검은 배경 제거 Offset값
	int nRotateBGOffset=1;												
	cv::Rect iRect[IMG_NUM];
	for(int i=0; i<IMG_NUM; i++)	{
		iRect[i] = cv::Rect(pPosition[i][PADMARK_T].x - sx.at(0)+nRotateBGOffset,
			pPosition[i][PADMARK_T].y - sy.at(0)+nRotateBGOffset,
			sx.at(0) + mw.at(0)-nRotateBGOffset,
			sy.at(0) + mh.at(0)-nRotateBGOffset);
		mRImgBuffer[i](iRect[i]).copyTo(comImg[i]);
		mtComImgMask[i] = cMem->GetMat(comImg[i].size(), comImg[i].type());
	}

	if(bImageSave)   
	{
		CString strTemp;
		strTemp.Format(_T("%s\\PAD_%d_ComImg.jpg"),	 strAlgPath, nImageSaveInx++);	cv::imwrite((cv::String)(CStringA)strTemp, comImg[SRC]);
	}

	Rect* rtFillmean;	//별도로 분리해서 검사하는 부분에 평균 값으로 채우는 영역, 별도 분리는 현재 도금단자 부분을 분리하고 있음
	rtFillmean = new Rect[nROICount];
	for(int nROIInx = 0; nROIInx < nROICount; nROIInx++)
	{			
		rtFillmean[nROIInx] = Rect(rtApartInsp[nROIInx].x - iRect[SRC].x, rtApartInsp[nROIInx].y - iRect[SRC].y, rtApartInsp[nROIInx].width, rtApartInsp[nROIInx].height);
		GetCheckROIOver(rtFillmean[nROIInx], Rect(0,0,comImg[SRC].cols,comImg[SRC].rows), rtFillmean[nROIInx]);
	}	

	Scalar Mean, Stdev;
	for(int i=0; i<IMG_NUM; i++)
	{
		cv::meanStdDev(comImg[i], Mean, Stdev);

		for(int nROIInx = 0; nROIInx < nROICount; nROIInx++)
		{
			comImg[i](rtFillmean[nROIInx]).setTo(Mean[0]);
			mtComImgMask[i](rtFillmean[nROIInx]).setTo(255);
		}
		cv::bitwise_not(mtComImgMask[i], mtComImgMask[i]);
	}


	CString strTest;
	Mat mtMarkDaraw;
	Rect MarkSearchRt;

	Mat comImg_St[IMG_NUM];

	float fStretchRatio;
	float fStretchRatio_Min = comImg[SRC].rows;
	for(int i = SRC; i < IMG_NUM; i++)
	{
		MarkSearchRt = Rect(0, comImg[i].rows/2, comImg[i].cols, comImg[i].rows/2);
		MARKPOSITION(comImg[i](MarkSearchRt), Mark[PADMARK_B], pPosition[i][PADMARK_B]);
		pPosition[i][PADMARK_B].y += comImg[i].rows/2;
		MarkSearchRt = Rect(0, 0, comImg[i].cols, comImg[i].rows/2);
		MARKPOSITION(comImg[i](MarkSearchRt), Mark[PADMARK_T], pPosition[i][PADMARK_T]);


		if(bImageSave)
		{
			cv::cvtColor(comImg[i], mtMarkDaraw, CV_GRAY2RGB);
			cv::circle(mtMarkDaraw, pPosition[i][PADMARK_B], 1, Scalar(0,0,255));
			cv::circle(mtMarkDaraw, pPosition[i][PADMARK_T], 1, Scalar(0,0,255));
			strTest.Format(_T("%s\\PAD_%d_CrossMark[%d].jpg"), strAlgPath, nImageSaveInx++, i);		cv::imwrite((cv::String)(CStringA)strTest, mtMarkDaraw);
		}
		if(i == SRC)
			continue;
		fStretchRatio = (pPosition[SRC][PADMARK_B].y - pPosition[i][PADMARK_B].y) / pPosition[i][PADMARK_B].y;			


		StretchBottom_V(comImg[i], comImg[i], fStretchRatio);
		if(fStretchRatio_Min > fStretchRatio)
			fStretchRatio_Min = fStretchRatio;
	}

	//영상을 줄이는 방향으로 Scale 보정 시 영상 아래쪽에 검은 영역이 생기기 때문에 제거
	if(fStretchRatio_Min < 0)
	{
		Rect rtDelete = Rect(0, comImg[SRC].rows - (int)fStretchRatio_Min, comImg[SRC].cols, (int)fStretchRatio_Min);
		for(int i = SRC; i < IMG_NUM; i++)
		{
			comImg[i](rtDelete).setTo(0);
		}
	}

	//C-Cut Line C-Cut 크기가 가장 작은 것 기준으로 제거
	//CCutLineDelete(comImg, IMG_NUM, pPosition[SRC][PADMARK_T], pPosition[SRC][PADMARK_B], nCCutInterceptOffset, nCutGV);

	PAD_Bright_MeanAdjust(comImg, IMG_NUM, nDivideNum);	

	/////////////////////test
	vector<Rect >				vecInspTem;
	vector<sMatchingScore>		vecMatchingInfo;
	vector<Rect >				vecInspSrc;

	//GetPreMatchingInfo(matSrcBuffer, matReferBuffer, vecInspSrc, vecInspTem, vecMatchingInfo);

	if(bImageSave)   
	{
		CString strTemp;
		strTemp.Format(_T("%s\\PAD_%d_Prc_ComImg.jpg"),		strAlgPath, nImageSaveInx++);	cv::imwrite((cv::String)(CStringA)strTemp, comImg[SRC]);
		strTemp.Format(_T("%s\\PAD_%d_Prc_ComImg.jpg"),		strAlgPath, nImageSaveInx++);	cv::imwrite((cv::String)(CStringA)strTemp, comImg[REF1]);
		strTemp.Format(_T("%s\\PAD_%d_Prc_ComImg.jpg"),		strAlgPath, nImageSaveInx++);	cv::imwrite((cv::String)(CStringA)strTemp, comImg[REF2]);
		strTemp.Format(_T("%s\\PAD_%d_Prc_ComImgMask.jpg"),		strAlgPath, nImageSaveInx++);	cv::imwrite((cv::String)(CStringA)strTemp, mtComImgMask[SRC]);
	}

	/////////////////////test
	// 9. Sub (REF1 - SRC) - (REF1 - REF2)
	// Black Threshold
	Mat mSub1, mSub2, mSub3, mSubResult, mSubThResult;

	//ROI영역들을 제외한 나머지 부분의 평균 값 차이 비교를 위함.
	Scalar meanSrc, meanRef1, StdSrc, StdRef1;
	meanStdDev(comImg[SRC], meanSrc, StdSrc, mtComImgMask[SRC]);
	meanStdDev(comImg[REF1], meanRef1, StdRef1, mtComImgMask[REF1]);

	Mat mtHistSrc, mtHistRef1;
	GetHistogram(comImg[SRC], mtHistSrc, mtComImgMask[SRC]);
	GetHistogram(comImg[REF1], mtHistRef1, mtComImgMask[SRC]);

	//표준편차 이하 값들 사용 안한 평균 값
	double dAvgSrc, dAvgRef1;
	GetAvgOfStdevRange(mtHistSrc, meanSrc, StdSrc, dAvgSrc);
	GetAvgOfStdevRange(mtHistRef1, meanRef1, StdRef1, dAvgRef1);



	for(int nImageInx=0; nImageInx < IMG_NUM; nImageInx++)
	{
		cv::GaussianBlur(comImg[nImageInx], comImg[nImageInx], cv::Size(nBlurlSize, nBlurlSize), 0, 0);
	}

	mSub1 = comImg[REF1] - comImg[SRC];
	mSub2 = comImg[REF1] - comImg[REF2];
	mSub3 = comImg[REF2] - comImg[REF3];
	mSubResult = mSub1 - mSub2;
	mSubThResult = mSubResult - mSub3;
	// White Threshold
	Mat mWhiteSub;
	mWhiteSub = comImg[SRC] - comImg[REF1];

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\PAD_%d_Dark_DefectResult.jpg"),	strAlgPath, nImageSaveInx++);	cv::imwrite((cv::String)(CStringA)strTemp, mSubResult);
	}	

	// 10. Defect
	Mat mBlur, mDefect;

	cv::GaussianBlur(mWhiteSub, mWhiteSub, Size(nBlurlSize,nBlurlSize), 0,0);

	double dSubMean = dAvgRef1 - dAvgSrc;

	//ROI영역을 제외한 나머지 부분의 평균 값이 원본 영상보다 크다면 과검 제거를 위해 Threshold 값에 가중치를 줌.
	if(dSubMean > 0)
		dBlackThreshold += dSubMean * 3;

	cv::threshold(mSubResult, mDefect, dBlackThreshold, 255, THRESH_BINARY);

	cv::threshold(mWhiteSub, mWhiteSub, nWhiteThreshold, 255, THRESH_BINARY);

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\PAD_%d_Dark_DefectResult_Tresh.jpg"),  strAlgPath, nImageSaveInx++);	cv::imwrite((cv::String)(CStringA)strTemp, mDefect);
	}	

	for(int nROIInx=0; nROIInx < nROICount; nROIInx++)
	{
		cv::add(mDefect(rtFillmean[nROIInx]), mtApartInsp[nROIInx](Rect(0,0,rtFillmean[nROIInx].width,rtFillmean[nROIInx].height)), mDefect(rtFillmean[nROIInx]));
	}

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\PAD_%d_Add_Mask_Prc.jpg"),	strAlgPath, nImageSaveInx++);	cv::imwrite((cv::String)(CStringA)strTemp, mDefect);
	}	

	delete[] rtROI;
	delete[] mtMatch;
	delete[] mtMask;
	delete[] mtMatchROI;
	delete[] mtApartInsp;
	delete[] rtApartInsp;
	delete[] rtFillmean;

	DeleteCuttingTresh(mDefect, nTreshWidthOffset);

	Rect rtNoneR = Rect(mDefect.cols - nNone_R_Offset, 0, nNone_R_Offset, mDefect.rows);
	mDefect(rtNoneR).setTo(0);

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\PAD_%d_Clean_Cutting_Line.jpg"),	 strAlgPath, nImageSaveInx++);	cv::imwrite((cv::String)(CStringA)strTemp, mDefect);
	}	


	if(nDeleteMorph >= 3)
	{
		//짜잘한 Blob들 제거용 Open 연산
		cv::morphologyEx(mDefect, mDefect, MORPH_OPEN, cv::getStructuringElement(MORPH_RECT, Size(nDeleteMorph, nDeleteMorph), cv::Point(-1,-1)));
	}

	if(nResultMorph >= 3)
	{
		//불량 끊긴 것 붙이기 위한 Close 연산, Erode을 Dilate보다 Offset만큼 크게 해줌
		cv::morphologyEx(mDefect, mDefect, MORPH_DILATE, cv::getStructuringElement(MORPH_RECT, Size(nResultMorph, nResultMorph), cv::Point(-1,-1)));
		cv::morphologyEx(mDefect, mDefect, MORPH_ERODE, cv::getStructuringElement(MORPH_RECT, Size(nResultMorph+nResultMorphOffset, nResultMorph+nResultMorphOffset), cv::Point(-1,-1)));
	}

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\PAD_%d_Result Morph.jpg"),	strAlgPath, nImageSaveInx++);	cv::imwrite((cv::String)(CStringA)strTemp, mDefect);
	}	

	cv::Scalar sGV = cv::mean(mSubResult);

	double dGV = (double)sGV[0];

	if (dGV > nGV)
	{
		return E_ERROR_CODE_TRUE;
	}

	
	ClassifyPadDust(mDefect, mDefect, mDustImg(iRect[SRC]), mDustImg(iRect[SRC]), mPadDustFeature, strAlgPath, bImageSave, nImageClassify);

	int nTemp = 0;

	// 11. CRUX Labeling
	Mat ResultMat		= Mat::zeros(mImgBuffer[SRC].rows, mImgBuffer[SRC].cols, CV_8UC1);
	Mat mResultPadDust	= Mat::zeros(mImgBuffer[SRC].rows, mImgBuffer[SRC].cols, CV_8UC1);

	mDefect.copyTo(ResultMat(iRect[SRC]));

	Mat matPartDraw = matDrawBuffer(rtInspArea);
	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem(cMem);

	int nPrevDefectCount = pResultBlob->nDefectCount;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mImgBuffer[SRC], ResultMat, matPartDraw, nCommonPara,
		E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob,  E_DEFECT_JUDGEMENT_APP_PAD_BURNT, false);
	cFeatureExtraction.DrawBlob(matPartDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

	for(long n = nTemp; n < pResultBlob->nDefectCount ;n++ )
	{
		pResultBlob->ptLT[n].x		+= rtInspArea.x;
		pResultBlob->ptLT[n].y		+= rtInspArea.y;
		pResultBlob->ptRT[n].x		+= rtInspArea.x;	
		pResultBlob->ptRT[n].y		+= rtInspArea.y;
		pResultBlob->ptRB[n].x		+= rtInspArea.x;
		pResultBlob->ptRB[n].y		+= rtInspArea.y;
		pResultBlob->ptLB[n].x		+= rtInspArea.x;	
		pResultBlob->ptLB[n].y		+= rtInspArea.y;
		pResultBlob->nCenterx[n]	+= rtInspArea.x;
		pResultBlob->nCentery[n]	+= rtInspArea.y;

		for (int nCt=0 ; nCt<MAX_CONTOURS ; nCt++)
		{
			if( pResultBlob->nContoursX[n][nCt] == 0	&&
				pResultBlob->nContoursY[n][nCt] == 0	)
				break;

			pResultBlob->nContoursX[n][nCt] += rtInspArea.x;
			pResultBlob->nContoursY[n][nCt] += rtInspArea.y;
		}				

		nTemp++;
	}

	// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
	// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
	if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

	cFeatureExtraction.DoDefectBlobSingleJudgment(mImgBuffer[SRC], ResultMat, matPartDraw, nCommonPara,
		E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob,  E_DEFECT_JUDGEMENT_APP_PAD_SCRATCH, false);
	cFeatureExtraction.DrawBlob(matPartDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

	for(long n = nTemp; n < pResultBlob->nDefectCount ;n++ )
	{
		pResultBlob->ptLT[n].x		+= rtInspArea.x;
		pResultBlob->ptLT[n].y		+= rtInspArea.y;
		pResultBlob->ptRT[n].x		+= rtInspArea.x;	
		pResultBlob->ptRT[n].y		+= rtInspArea.y;
		pResultBlob->ptRB[n].x		+= rtInspArea.x;
		pResultBlob->ptRB[n].y		+= rtInspArea.y;
		pResultBlob->ptLB[n].x		+= rtInspArea.x;	
		pResultBlob->ptLB[n].y		+= rtInspArea.y;
		pResultBlob->nCenterx[n]	+= rtInspArea.x;
		pResultBlob->nCentery[n]	+= rtInspArea.y;

		for (int nCt=0 ; nCt<MAX_CONTOURS ; nCt++)
		{
			if( pResultBlob->nContoursX[n][nCt] == 0	&&
				pResultBlob->nContoursY[n][nCt] == 0	)
				break;

			pResultBlob->nContoursX[n][nCt] += rtInspArea.x;
			pResultBlob->nContoursY[n][nCt] += rtInspArea.y;
		}				

		nTemp++;
	}

	// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
	// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
	if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);			

	cFeatureExtraction.DoDefectBlobSingleJudgment(mImgBuffer[SRC], mResultPadDust, matPartDraw, nCommonPara,
		E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob,  E_DEFECT_JUDGEMENT_APP_PAD_DUST, false);
	cFeatureExtraction.DrawBlob(matPartDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

	for(long n = nTemp; n < pResultBlob->nDefectCount ;n++ )
	{
		pResultBlob->ptLT[n].x		+= rtInspArea.x;
		pResultBlob->ptLT[n].y		+= rtInspArea.y;
		pResultBlob->ptRT[n].x		+= rtInspArea.x;	
		pResultBlob->ptRT[n].y		+= rtInspArea.y;
		pResultBlob->ptRB[n].x		+= rtInspArea.x;
		pResultBlob->ptRB[n].y		+= rtInspArea.y;
		pResultBlob->ptLB[n].x		+= rtInspArea.x;	
		pResultBlob->ptLB[n].y		+= rtInspArea.y;
		pResultBlob->nCenterx[n]	+= rtInspArea.x;
		pResultBlob->nCentery[n]	+= rtInspArea.y;

		for (int nCt=0 ; nCt<MAX_CONTOURS ; nCt++)
		{
			if( pResultBlob->nContoursX[n][nCt] == 0	&&
				pResultBlob->nContoursY[n][nCt] == 0	)
				break;

			pResultBlob->nContoursX[n][nCt] += rtInspArea.x;
			pResultBlob->nContoursY[n][nCt] += rtInspArea.y;
		}				

		nTemp++;
	}

	// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
	// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
	if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);			

	if(bImageSave)   
	{
		Mat mtResult;
		matPartDraw.copyTo(mtResult);	

		CString strTemp;
		strTemp.Format(_T("%s\\PadResult.jpg"),		strAlgPath);	cv::imwrite((cv::String)(CStringA)strTemp, mtResult);
	}

	return E_ERROR_CODE_TRUE;
}

long CInspectPad::ThresholdProcess(Mat matMainBuffer, Mat matReferBuffer, cv::Mat matDust, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CString strCurRecipePath, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt)
{
	PAD_DUST_FEATURE	mPadDustFeature;
	int		nImageSaveInx		=	0;
	bool	bImageSave			=	(nCommonPara[1] > 0) ? true : false;	// 1 : 알고리즘 중간결과 Image Save 
	int		nImageClassify		=	nCommonPara[3];
	int		nCameraNum_			=	nCommonPara[4];						// 04 : Cam Number ( 카메라 번호 / SVI )

	long	nErrorCode = E_ERROR_CODE_TRUE;

	int nPos = 0;

	int		nBlurSize									=	(int)dPara[nPos++];
	float	fThreshold									=	(float)dPara[nPos++];
	int		nCloseSize									=	(int)dPara[nPos++];
	int		nDeleteRect									=	(int)dPara[nPos++];
	int		nDeleteBackGroundRange						=	(int)dPara[nPos++];
	mPadDustFeature.fMeanGV								=	(float)dPara[nPos++];
	mPadDustFeature.fRoundness							=	(float)dPara[nPos++];
	mPadDustFeature.fSpecificGVRatio					=	(float)dPara[nPos++];
	mPadDustFeature.nMaxDustArea						=   (int)dPara[nPos++];
	BOOL	bUseContact									=   (BOOL)dPara[nPos++];
	int		nTBCuttingOffset							=   (int)dPara[nPos++];
	int		nLCuttingOffset								=   (int)dPara[nPos++];

	GetCheckMaskSize(nBlurSize);
	GetCheckMaskSize(nCloseSize);
	cv::Rect rtPadWork = stPadInfo[E_PAD_REF][nCameraNum_].GetRectCoord(0);

	Mat mSrcBuffer;			//원본 PAD ROI 영상
	Mat mImgBuffer;			//원본 PAD ROI 처리 영상
	Mat mDustBuffer;		//Dust Pattern PAD ROI영상

	rtInspArea = SetCuttingNonInspect(rtInspArea,nLCuttingOffset,nTBCuttingOffset,TRUE);

	// 2. Src mark extract
	GetCheckROIOver(rtInspArea, Rect(0,0,matMainBuffer.cols,matMainBuffer.rows), rtInspArea);
	matMainBuffer(rtInspArea).copyTo(mSrcBuffer);
	matMainBuffer(rtInspArea).copyTo(mImgBuffer);
	matDust(rtInspArea).copyTo(mDustBuffer);

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%d_Pad_Treshold_Prc_%d.jpg"),		strAlgPath, nImageClassify, nImageSaveInx++);	
		cv::imwrite((cv::String)(CStringA)strTemp, mImgBuffer);
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(bUseContact)			// Right Pattern 도금 단자부 검사 사용 유무
	{
		cv::Mat* mtNoneArea = NULL;

		int nROICount = stPadInfo[E_PAD_INSP_AREA][0].GetRoiCount();

		Rect* rtROI						= new Rect[nROICount];	//ROI 저장된 좌표 값
		Mat* mtMatch					= new Mat[nROICount];	//ROI 저장된 영상
		Mat* mtMask						= new Mat[nROICount];

		CString strMatch;	

		int nROI_OffsetX = 10; //원본영상에서 Matching을 진행할 영역의 확장 Offset 크기
		int nROI_OffsetY = 30;

		for(int nROIInx=0; nROIInx<nROICount; nROIInx++)
		{
			mtMatch[nROIInx] = cvarrToMat(stPadInfo[E_PAD_INSP_AREA][nCameraNum_].GetImage(nROIInx));

			strMatch.Format(_T("%s\\PAD_INSP_INFO\\PadRoi\\PadRoiInsp_img_Mask_%02d_CAM%02d.bmp"), strCurRecipePath, nROIInx, nCameraNum_);			
			mtMask[nROIInx]  = imread((cv::String)(CStringA)strMatch, IMREAD_GRAYSCALE);

			rtROI[nROIInx]  = stPadInfo[E_PAD_INSP_AREA][nCameraNum_].GetRectCoord(nROIInx);

			//PAD 검사 영역 좌표로 보정
			rtROI[nROIInx]  = Rect(rtROI[nROIInx] .x - rtPadWork.x - nROI_OffsetX, rtROI[nROIInx] .y - rtPadWork.y - nROI_OffsetY, 
				rtROI[nROIInx] .width + nROI_OffsetX * 2, rtROI[nROIInx] .height + nROI_OffsetY * 2);

			GetCheckROIOver(rtROI[nROIInx] , Rect(0, 0, mImgBuffer.cols, mImgBuffer.rows), rtROI[nROIInx] );
		}

		cv::Mat *mtMatchROI;									//원본영상에서 Matching을 진행 할 영역
		cv::Mat *mtApartInsp;									//Matching 진행 하여 별도 분리 검사 진행하는 영역 영상
		cv::Rect *rtApartInsp;									//Matching 진행 하여 별도 분리 검사 진행하는 영역 좌표
		cv::Mat mtMatchRst;	
		cv::Mat	mtComImgMask[IMG_NUM];									//ROI 영역을 제외한 나머지 부분

		double dMatchRate;
		cv::Point ptMatch;		//Matching 해서 나온 정수 좌표

		mtMatchROI			= new cv::Mat[nROICount];
		mtApartInsp			= new cv::Mat[nROICount];
		rtApartInsp			= new cv::Rect[nROICount];

		for(int nROIInx=0; nROIInx<nROICount; nROIInx++)
		{
			mtMatchROI[nROIInx] = mImgBuffer(rtROI[nROIInx] );

			if(mtMatchROI[nROIInx].cols < mtMatch[nROIInx].cols || mtMatchROI[nROIInx].rows < mtMatch[nROIInx] .rows)
				return E_ERROR_CODE_PAD_INSP_ROI_OVER;

			cv::matchTemplate(mtMatchROI[nROIInx] , mtMatch[nROIInx] , mtMatchRst, CV_TM_CCOEFF_NORMED);
			cv::minMaxLoc(mtMatchRst, NULL, &dMatchRate, NULL, &ptMatch);

			rtApartInsp[nROIInx]  = cv::Rect(rtROI[nROIInx] .x + ptMatch.x, rtROI[nROIInx] .y + ptMatch.y, mtMatch[nROIInx] .cols, mtMatch[nROIInx] .rows);
			mtMatchROI[nROIInx](Rect(ptMatch.x, ptMatch.y, mtMatch[nROIInx] .cols, mtMatch[nROIInx] .rows)).copyTo(mtApartInsp[nROIInx]);
		}

		Rect* rtFillmean;	//별도로 분리해서 검사하는 부분에 평균 값으로 채우는 영역, 별도 분리는 현재 도금단자 부분을 분리하고 있음

		rtFillmean = new Rect[nROICount];

		for(int i=0; i<nROICount; i++)
		{			
			rtFillmean[i] = Rect(rtApartInsp[i].x , rtApartInsp[i].y, rtApartInsp[i].width, rtApartInsp[i].height);
			GetCheckROIOver(rtFillmean[i], Rect(0,0,mImgBuffer.cols,mImgBuffer.rows), rtFillmean[i]);

			Scalar Mean, Stdev;

			cv::meanStdDev(mImgBuffer, Mean, Stdev);
			mImgBuffer(rtFillmean[i]).setTo(Mean[0]);
		}

		if(bImageSave)
		{
			CString strTemp;
			strTemp.Format(_T("%s\\Right_Contact_Non.jpg"),strAlgPath);
			cv::imwrite((cv::String)(CStringA)strTemp,mImgBuffer);
		}

		delete[] rtROI;
		delete[] mtMatch;
		delete[] mtMask;
		delete[] mtMatchROI;
		delete[] mtApartInsp;
		delete[] rtApartInsp;
		delete[] rtFillmean;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	MinimumFilter(mImgBuffer, mImgBuffer, nBlurSize);

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%d_Pad_Treshold_Prc_%d.jpg"),		strAlgPath, nImageClassify, nImageSaveInx++);	
		cv::imwrite((cv::String)(CStringA)strTemp, mImgBuffer);
	}

	cv::threshold(mImgBuffer, mImgBuffer, fThreshold, 255, CV_THRESH_BINARY_INV);

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%d_Pad_Treshold_Prc_%d.jpg"),		strAlgPath, nImageClassify, nImageSaveInx++);	
		cv::imwrite((cv::String)(CStringA)strTemp, mImgBuffer);
	}


	Rect rtDeleteRect(nDeleteRect, nDeleteRect, mImgBuffer.cols-nDeleteRect*2, mImgBuffer.rows-nDeleteRect*2);
	if(rtDeleteRect.x < 0 || rtDeleteRect.y < 0 || rtDeleteRect.width < 0 ||  rtDeleteRect.height < 0)
		return E_ERROR_CODE_ROI_OVER;

	Mat mtDeleteRectMask = cMem->GetMat(mImgBuffer.size(), mImgBuffer.type());
	mtDeleteRectMask.setTo(255);
	mtDeleteRectMask(rtDeleteRect).setTo(0);

	Mat mtDeleteRect;
	mtDeleteRect = mImgBuffer - mtDeleteRectMask;

	Mat mtFindContour;
	mtDeleteRect.copyTo(mtFindContour);

	Mat mtBiggestBlob = cMem->GetMat(mImgBuffer.size(), mImgBuffer.type());

	int largest_area=0;
	int largest_contour_index=0;

	vector< vector<Point> > contours; // Vector for storing contour
	vector<Vec4i> hierarchy;

	cv::findContours( mtFindContour, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
	cv::Rect bounding_rect;
	for( int i = 0; i< contours.size(); i++ ) {// iterate through each contour. 
		bounding_rect = cv::boundingRect(contours[i]);

		if(bounding_rect.tl().y < nDeleteBackGroundRange)
		{
			cv::drawContours(mtBiggestBlob, contours, i, Scalar(255), CV_FILLED);
		}
		if(bounding_rect.br().y > mImgBuffer.rows - nDeleteBackGroundRange)
		{
			cv::drawContours(mtBiggestBlob, contours, i, Scalar(255), CV_FILLED);
		}
	}

	Mat mtSubBiggestBlob;

	mtSubBiggestBlob = mtDeleteRect - mtBiggestBlob;

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%d_Pad_Treshold_Prc_%d.jpg"),		strAlgPath, nImageClassify, nImageSaveInx++);	
		cv::imwrite((cv::String)(CStringA)strTemp, mtSubBiggestBlob);
	}

	DeleteRectRecovery(mImgBuffer, mtSubBiggestBlob, nDeleteRect);


	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%d_Pad_Treshold_Prc_%d.jpg"),		strAlgPath, nImageClassify, nImageSaveInx++);	
		cv::imwrite((cv::String)(CStringA)strTemp, mtSubBiggestBlob);
	}

	Mat mtResult;
	mtResult = mtSubBiggestBlob;

	cv::morphologyEx(mtResult, mtResult, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(nCloseSize, nCloseSize)));
	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%d_Pad_Treshold_Prc_%d.jpg"),		strAlgPath, nImageClassify, nImageSaveInx++);	
		cv::imwrite((cv::String)(CStringA)strTemp, mtResult);
	}

	Mat mtDustResult;
	ClassifyPadDust(mtResult, mtResult, mDustBuffer, mtDustResult, mPadDustFeature, strAlgPath, bImageSave, nImageClassify);

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%d_Pad_Treshold_Prc_PadDust_%d.jpg"),		strAlgPath, nImageClassify, nImageSaveInx++);	
		cv::imwrite((cv::String)(CStringA)strTemp, mtDustResult);
	}

	if(bImageSave)
	{
		CString strTemp;
		strTemp.Format(_T("%s\\%d_Pad_Treshold_Prc_PadBurnt_%d.jpg"),		strAlgPath, nImageClassify, nImageSaveInx++);	
		cv::imwrite((cv::String)(CStringA)strTemp, mtResult);
	}

	int nTemp = 0;

	Mat matPartDraw = matDrawBuffer(rtInspArea);
	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem(cMem);

	int nPrevDefectCount = pResultBlob->nDefectCount;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mSrcBuffer, mtResult, matPartDraw, nCommonPara,
		E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob,  E_DEFECT_JUDGEMENT_APP_PAD_BURNT, false);
	cFeatureExtraction.DrawBlob(matPartDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

	for(long n = nTemp; n < pResultBlob->nDefectCount ;n++ )
	{
		pResultBlob->ptLT[n].x		+= rtInspArea.x;
		pResultBlob->ptLT[n].y		+= rtInspArea.y;
		pResultBlob->ptRT[n].x		+= rtInspArea.x;	
		pResultBlob->ptRT[n].y		+= rtInspArea.y;
		pResultBlob->ptRB[n].x		+= rtInspArea.x;
		pResultBlob->ptRB[n].y		+= rtInspArea.y;
		pResultBlob->ptLB[n].x		+= rtInspArea.x;	
		pResultBlob->ptLB[n].y		+= rtInspArea.y;
		pResultBlob->nCenterx[n]	+= rtInspArea.x;
		pResultBlob->nCentery[n]	+= rtInspArea.y;

		for (int nCt=0 ; nCt<MAX_CONTOURS ; nCt++)
		{
			if( pResultBlob->nContoursX[n][nCt] == 0	&&
				pResultBlob->nContoursY[n][nCt] == 0	)
				break;

			pResultBlob->nContoursX[n][nCt] += rtInspArea.x;
			pResultBlob->nContoursY[n][nCt] += rtInspArea.y;
		}				

		nTemp++;
	}

	// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
	// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
	if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

	matPartDraw = matDrawBuffer(rtInspArea);

	cFeatureExtraction.DoDefectBlobSingleJudgment(mSrcBuffer, mtDustResult, matPartDraw, nCommonPara,
		E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob,  E_DEFECT_JUDGEMENT_APP_PAD_DUST, false);
	cFeatureExtraction.DrawBlob(matPartDraw, cv::Scalar(135,206,250), BLOB_DRAW_BLOBS_CONTOUR, true);

	for(long n = nTemp; n < pResultBlob->nDefectCount ;n++ )
	{
		pResultBlob->ptLT[n].x		+= rtInspArea.x;
		pResultBlob->ptLT[n].y		+= rtInspArea.y;
		pResultBlob->ptRT[n].x		+= rtInspArea.x;	
		pResultBlob->ptRT[n].y		+= rtInspArea.y;
		pResultBlob->ptRB[n].x		+= rtInspArea.x;
		pResultBlob->ptRB[n].y		+= rtInspArea.y;
		pResultBlob->ptLB[n].x		+= rtInspArea.x;	
		pResultBlob->ptLB[n].y		+= rtInspArea.y;
		pResultBlob->nCenterx[n]	+= rtInspArea.x;
		pResultBlob->nCentery[n]	+= rtInspArea.y;

		for (int nCt=0 ; nCt<MAX_CONTOURS ; nCt++)
		{
			if( pResultBlob->nContoursX[n][nCt] == 0	&&
				pResultBlob->nContoursY[n][nCt] == 0	)
				break;

			pResultBlob->nContoursX[n][nCt] += rtInspArea.x;
			pResultBlob->nContoursY[n][nCt] += rtInspArea.y;
		}				

		nTemp++;
	}

	rtInspArea = SetCuttingNonInspect(rtInspArea,nLCuttingOffset,nTBCuttingOffset,FALSE);

	// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
	// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
	if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(nCommonPara, strContourTxt, true);

	return nErrorCode;
}

long CInspectPad::PAD_Bright_Adjust(cv::Mat* mtImg, int nImageNum)
{	
	int** nHist;
	nHist = new int*[nImageNum];

	for(int n=0; n < nImageNum; n++)
	{
		nHist[n] = new int[256];
		memset(nHist[n], 0, 256 * sizeof(int));
	}

	vector<double> dMax;
	vector<double> dMean;
	Scalar  s;
	Scalar* m;
	m = new Scalar[nImageNum];
	uchar* pImgdata;

	for(int n=0; n < nImageNum; n++)
	{
		//cv::minMaxIdx(mtImg[n], 0, &dM);
		cv::meanStdDev(mtImg[n], m[n], s);
		//dMax.push_back(dM);
		dMean.push_back(double(m[n][0]));
		for(int nY=0; nY < mtImg[n].rows; nY++)
		{
			pImgdata = mtImg[n].data+nY*mtImg[n].step;
			for(int nX=0; nX < mtImg[n].cols; nX++)
			{
				nHist[n][(int)*(pImgdata+nX)]++;
			}
		}
	}
	
	sort(dMean.begin(), dMean.end());

	int** nLut_Adjust;
	nLut_Adjust = new int*[nImageNum];
	for(int n=0; n < nImageNum; n++)
	{
		nLut_Adjust[n] = new int[256];
		memset(nLut_Adjust[n], 0, sizeof(int) * 256);
	}

	unsigned int nSum[256]={0,};

	for(int n=0; n < nImageNum; n++)
	{

		for(int i=0; i < 256; i++)
		{
			memset(nSum,0, sizeof(unsigned int)*256);
			for(int j=0; j <= i; j++)
			{
				nSum[i]				+= nHist[n][j];
			}
			nLut_Adjust[n][i]	= (float)nSum[i]/(mtImg[n].cols*mtImg[n].rows)*255;	
		}
	}

	Mat *mtDst;
	mtDst = new Mat[nImageNum];
	uchar *pDstData;

	for(int n=0; n < nImageNum; n++)
	{
		double dOffsetMean = dMean[dMean.size()-1] - m[n][0];
		mtDst[n] = Mat::zeros(mtImg[n].size(), CV_8UC1);
		for(int nY=0; nY < mtImg[n].rows; nY++)
		{
			pImgdata = mtImg[n].data + nY*mtImg[n].step;
			pDstData = mtDst[n].data + nY*mtDst[n].step;
			for(int nX=0; nX < mtImg[n].cols; nX++)
			{
				//*(pDstData+nX) = (uchar)nLut_Adjust[n][(int)*(pImgdata+nX)];
				if((int)*(pImgdata+nX)!=0)
					*(pDstData+nX) = (uchar)((int)*(pImgdata+nX)+(int)dOffsetMean);
			}
		}
	}

	for(int n=0; n < nImageNum; n++)
	{
		mtDst[n].copyTo(mtImg[n]);

		delete[] nHist[n];
		delete[] nLut_Adjust[n];
	}

	delete[] nHist;
	delete[] nLut_Adjust;
	delete[] mtDst;
	delete[] m;

	return 0;
}

long CInspectPad::PAD_Bright_MeanAdjust(cv::Mat* mtImg, int nImgNum, int nDivideNum)
{
	Scalar  s;
	Scalar*	m;
	vector<double> dMean;
	
	int nDivideRange = mtImg[0].rows/nDivideNum;
	int nHeight = nDivideRange;
	m = new Scalar[nImgNum];

	for(int nDivideInx = 0; nDivideInx < nDivideNum; nDivideInx++)
	{
		vector<double>().swap(dMean);
		if(nDivideInx == nDivideNum-1)
			nHeight = mtImg[0].rows - nDivideRange*nDivideInx;

		Rect rtDivide = Rect(0, nDivideRange*nDivideInx, mtImg[0].cols, nHeight);

		for(int n=0; n < nImgNum; n++)
		{
			Mat mtHist;
			cv::meanStdDev(mtImg[n](rtDivide), m[n], s);
			GetHistogram(mtImg[n](rtDivide), mtHist);
			double dAvg;
			GetAvgOfStdevRange(mtHist, m[n], s, dAvg);
			dMean.push_back(double(m[n][0]));
		}

		sort(dMean.begin(), dMean.end());

		uchar* pImgdata;
		int	   nAdjustVal;
		for(int n=0; n < nImgNum; n++)
		{
			double dOffsetMean = dMean[dMean.size()-1] / m[n][0];
			if(dOffsetMean < 1)
				dOffsetMean = 1;

			for(int nY = 0; nY < mtImg[n](rtDivide).rows; nY++)
			{
				pImgdata = mtImg[n](rtDivide).data + nY * mtImg[n](rtDivide).step;
				for(int nX = 0; nX < mtImg[n](rtDivide).cols; nX++)
				{
					if((int)*(pImgdata+nX) != 0)
					{
						nAdjustVal = ((int)*(pImgdata + nX)  * dOffsetMean);
						if(nAdjustVal < 0)
							nAdjustVal = 0;
						if(nAdjustVal > 255)
							nAdjustVal = 255;
						*(pImgdata + nX) = (uchar)nAdjustVal;
					}
				}
			}
		}
	}

	delete[] m;

	return E_ERROR_CODE_TRUE;
}

bool CInspectPad::GetAvgOfStdevRange(Mat matHisto, Scalar Avg, Scalar Stdev, double& dReturnAvg)
{
	int nMin = (int)(Avg[0] - Stdev[0]);
	int nMax = (int)(Avg[0] + Stdev[0]);

	if(nMin < 0)
		nMin = 0;
 	if(nMax > 255)
 		nMax = 255;

	double dSum = 0;
	int nSumCnt = 0;

	float* fHistData = (float*)matHisto.data + nMin;

	for(int n = nMin; n < nMax; n++, fHistData++)
	{
// 		if((int)*fHistData == 0)
// 			continue;
		dSum += *fHistData * n;
		nSumCnt += (int)*fHistData;
	}

	dReturnAvg = dSum / nSumCnt;

	return true;
}

//모서리로부터 가로, 세로로 가장 긴변을 사용하여 가로, 세로에서 찾은 두지점을 연결하여 Cutting
long CInspectPad::CCutLineDelete(cv::Mat* mtImg, int nImageNum, Point2f ptMark_T, Point2f ptMark_B, int nCutInterceptOffset, int nCutGV)
{
	vector<Point2f> ptCutLine_T, ptCutLine_B;

	uchar* pImagedata;
	int nY, nX;
	for(nY = 0; nY < mtImg[SRC].rows * 0.05; nY++)
	{
		pImagedata = mtImg[SRC].data + nY * mtImg[SRC].step;
		for(nX = mtImg[SRC].cols-1; nX >= 0 ; nX--)
		{
			if((int)*(pImagedata + nX) > nCutGV)
			{
				if(nX > mtImg[SRC].cols - 3)
					break;
				ptCutLine_T.push_back(Point2f(nX, nY));
				break;
			}
		}

	}

	for(nY = mtImg[SRC].rows-1; nY > mtImg[SRC].rows * 0.95; nY--)
	{
		pImagedata = mtImg[SRC].data + nY * mtImg[SRC].step;
		for(nX = mtImg[SRC].cols-1; nX >= 0 ; nX--)
		{
			if((int)*(pImagedata + nX) > nCutGV)
			{
				if(nX > mtImg[SRC].cols - 3)
					break;
				ptCutLine_B.push_back(Point2f(nX, nY));
				break;
			}
		}
	}

	long double dCutTOP_A, dCutTOP_B;//PAD Top 부분 C-Cut Line
	long double dCutBOTTOM_A, dCutBOTTOM_B;//PAD Bottom 부분 C-Cut Line

	if(ptCutLine_B.size() < 3 || ptCutLine_T.size() < 3)
		return 0;

	Mat mtCutT, mtCutB;
	cv::cvtColor(mtImg[SRC], mtCutT, CV_GRAY2RGB);
	cv::cvtColor(mtImg[SRC], mtCutB, CV_GRAY2RGB);

// 	for(int n = 0; n < ptCutLine_T.size(); n++)
// 	{
// 		circle(mtCutT, ptCutLine_T[n], 1, Scalar(0,255,0));
// 	}
// 	for(int n = 0; n < ptCutLine_B.size(); n++)
// 	{
// 		circle(mtCutB, ptCutLine_B[n], 1, Scalar(0,255,0));
// 	}
// 

	calcRANSAC(ptCutLine_T, dCutTOP_A, dCutTOP_B, 3, 5);
	calcRANSAC(ptCutLine_B, dCutBOTTOM_A, dCutBOTTOM_B, 3, 5);


	Mat mtTest;


	int nInterSept_Offset = nCutInterceptOffset;
	dCutTOP_B += nInterSept_Offset;
	dCutBOTTOM_B -= nInterSept_Offset;

	int nCutLine_X;

	for(int nImgInx = 0; nImgInx < nImageNum; nImgInx++)
	{
		for(nY = 0; nY < mtImg[SRC].rows * 0.05; nY++)
		{
			pImagedata = mtImg[nImgInx].data + nY * mtImg[nImgInx].step;
			nCutLine_X = ((float)nY - dCutTOP_B)/dCutTOP_A;
			for(nX= mtImg[nImgInx].cols-1; nX >= 0; nX--)
			{
				if(nX > nCutLine_X)
					*(pImagedata+nX) = 0;
			}
		}

		for(nY = mtImg[SRC].rows * 0.95; nY < mtImg[nImgInx].rows; nY++)
		{
			pImagedata = mtImg[nImgInx].data + nY * mtImg[nImgInx].step;
			nCutLine_X = ((float)nY - dCutBOTTOM_B)/dCutBOTTOM_A;
			for(nX= mtImg[nImgInx].cols-1; nX >= 0; nX--)
			{
 				if(nX > nCutLine_X)
 					*(pImagedata+nX) = 0;
			}
		}
	}
	

	return 0;
}

long CInspectPad::DeleteNoneArea_Match(cv::Mat* mtImg, int nImageNum, cv::Mat* mtNone,  STRU_INFO_PAD** stPadInfo)
{
	int nNoneNum = stPadInfo[E_PAD_NONE_AREA][0].GetRoiCount();

	mtNone = new cv::Mat[nNoneNum];

	Mat mtTemplate;
	double dMatchRate;
	cv::Point ptMatch;		//Matching 해서 나온 정수 좌표
	Rect rtDelete;
	Scalar m,s;

	for(int nImageInx=0; nImageInx < nImageNum; nImageInx++)
	{
		cv::meanStdDev(mtImg[nImageInx], m, s);
		for(int nNoneInx=0; nNoneInx < nNoneNum; nNoneInx++)
		{
			mtNone[nNoneInx] = cvarrToMat(stPadInfo[E_PAD_NONE_AREA][0].GetImage(nNoneInx));
			
			if(mtImg[nImageInx].cols < mtNone[nNoneInx].cols	||
			   mtImg[nImageInx].rows < mtNone[nNoneInx].rows)
			   continue;
			cv::matchTemplate(mtImg[nImageInx], mtNone[nNoneInx], mtTemplate, CV_TM_CCOEFF_NORMED);
			cv::minMaxLoc(mtTemplate, NULL, &dMatchRate, NULL, &ptMatch);
			if(dMatchRate < 0.6)
				continue;

			rtDelete = Rect(ptMatch.x, ptMatch.y, mtNone[nNoneInx].cols, mtNone[nNoneInx].rows);
			Mat mtDeleteMask = Mat::zeros(Size(rtDelete.width, rtDelete.height), mtImg[nImageInx].type());
			const cv::Point* ptNoneMask[1] = {stPadInfo[E_PAD_NONE_AREA][0].GetPolygonCoord(nNoneInx)};

			int npt[] = {stPadInfo[E_PAD_NONE_AREA][0].GetPolygonCount(nNoneInx)};
			fillPoly(mtDeleteMask, ptNoneMask, npt, 1, Scalar(255));
			mtImg[nImageInx](rtDelete).setTo(m[0]);

// 실행 드라이브 D:\ 고정 아님 - 필요 시 InspectLibLog 의 GETDRV() 활용
// 			CString strTest;
// 			strTest.Format(_T("D:\\IDB\\NoneMask_%d.bmp"), nNoneInx);
// 			imwrite((cv::String)(CStringA)strTest, mtDeleteMask);
		}
	}

	delete[] mtNone;

	return E_ERROR_CODE_TRUE;
}

long CInspectPad::MaskProcess(cv::Mat* mtROI, cv::Mat* mtMask, int nROINum, double dGaus_Sigma, int nMaskBlurSize_Guas, int nMaskBlurSize, double dTh , double dMeanWeight, CString strSavePath, Mat* mtMatch, int nMaskDivde, bool bImageSave)
{
	int nSaveImageInx = 1;	//중간 저장 영상 순서

	CString strSaveImage;

	//Mask = 도금단자 영역, Mask2 = 도금단자 아닌 영역
	Mat* mtPrcMask, *mtPrcMask2;
	mtPrcMask = new Mat[nROINum];
	mtPrcMask2 = new Mat[nROINum];

	Scalar m, s, m_Inv, s_Inv;
	Scalar Refmean1, RefStd1, Refmean2, RefStd2;

	int nDivide = 3;				//영상 분할 갯수
	cv::Rect rtDivide;					//영상 분할 ROI

	Mat mtGaus;	//가우시안 블러 영상
	Mat mtBG;		//배경영상
	Mat	mtDivideTh;			//영상분할하여 평균값 이진화로 검출할 영상

	Mat mtMask_INV;
	Mat mtMask_INV_Morph;
	Mat mtRef1, mtRef2;			//ROI Refernece영상

	for(int nROIInx=0; nROIInx < nROINum; nROIInx++)
	{
		mtPrcMask[nROIInx]	= Mat::zeros(mtROI[nROIInx].size(), CV_8UC1);
		mtPrcMask2[nROIInx] = Mat::zeros(mtROI[nROIInx].size(), CV_8UC1);
		mtMatch[nROIInx].copyTo(mtRef1);
		mtMatch[nROIInx].copyTo(mtRef2);

		cv::bitwise_not(mtMask[nROIInx], mtMask_INV);
		cv::meanStdDev(mtROI[nROIInx], m, s, mtMask[nROIInx]);
		mtPrcMask[nROIInx].setTo(m[0], mtMask_INV);

		cv::morphologyEx(mtMask_INV, mtMask_INV, MORPH_ERODE, cv::getStructuringElement(MORPH_RECT, Size(3,3)));
		cv::bitwise_not(mtMask_INV, mtMask_INV_Morph);

		//도금 단자 이외의 영역 평균값으로 채우기
		mtROI[nROIInx].copyTo(mtPrcMask[nROIInx], mtMask[nROIInx]);
		mtROI[nROIInx].copyTo(mtPrcMask2[nROIInx], mtMask_INV);

		int nDivideHeight = mtROI[nROIInx].rows / nMaskDivde;
		for(int nDivideInx = 0; nDivideInx < nMaskDivde; nDivideInx++)
		{
			if(nDivideInx == nMaskDivde-1)	rtDivide = cv::Rect(0, nDivideHeight * nDivideInx, mtROI[nROIInx].cols, mtROI[nROIInx].rows - (nDivideHeight * nDivideInx));
			else							rtDivide = cv::Rect(0, nDivideHeight * nDivideInx, mtROI[nROIInx].cols, nDivideHeight);
		
			cv::meanStdDev(mtROI[nROIInx](rtDivide), m_Inv, s_Inv, mtMask_INV(rtDivide));
			mtPrcMask2[nROIInx](rtDivide).setTo(m_Inv[0], mtMask_INV_Morph(rtDivide));			
		}

		

		cv::meanStdDev(mtRef1, Refmean1, RefStd1, mtMask[nROIInx]);	
		mtRef1.setTo(m[0], mtMask_INV);
		cv::meanStdDev(mtRef2, Refmean2, RefStd2, mtMask_INV);		
		mtRef2.setTo(m_Inv[0], mtMask_INV_Morph);
		
		
		float fSubMean;
		Mat mtSubstance1, mtSubstance2, mtSubstanceRst1, mtSubstanceRst2, mtSubstanceRst3;
	
		float fBlackTH;
		float fWhiteTH;

		//Mask None Inverse 영역 처리
		fSubMean = Refmean1[0] - m[0];
		if(fSubMean > 0)
		{
			fBlackTH = (float)dTh + fSubMean;
			fWhiteTH = (float)dTh;
		}
		else
		{
			fBlackTH = (float)dTh;
		}
		cv::blur(mtRef1, mtRef1, Size(3,3));
		cv::blur(mtPrcMask[nROIInx], mtPrcMask[nROIInx], Size(3,3));
		mtSubstance1 = mtRef1 - mtPrcMask[nROIInx];

		cv::threshold(mtSubstance1, mtSubstanceRst1, fBlackTH, 255, CV_THRESH_BINARY);

		//Mask Inverse 영역 처리
		fSubMean = Refmean2[0] - m_Inv[0];
		//과검 방지를 위해 Threshold 값에 가중치를 줌.
		if(fSubMean > 0)
		{
			fBlackTH = (float)dTh + fSubMean;
			fWhiteTH = (float)dTh;
		}
		else
		{
			fBlackTH = (float)dTh;
			fWhiteTH = (float)dTh + abs(fSubMean) * 1.5;
		}
		cv::blur(mtRef2, mtRef2, Size(3,3));
		cv::blur(mtPrcMask2[nROIInx], mtPrcMask2[nROIInx], Size(3,3));
		mtSubstance1 = mtRef2 - mtPrcMask2[nROIInx];

		cv::threshold(mtSubstance1, mtSubstanceRst2, fBlackTH, 255, CV_THRESH_BINARY);

		cv::add(mtSubstanceRst1, mtSubstanceRst2, mtSubstanceRst3);

		if(bImageSave)
		{
			strSaveImage.Format(_T("%s\\Apart_PAD_Substance_%d_%d_Ref.bmp"),	strSavePath, nROIInx+1, nSaveImageInx);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtRef1);

			strSaveImage.Format(_T("%s\\Apart_PAD_Substance_Inv_%d_%d_Ref.bmp"),	strSavePath, nROIInx+1, nSaveImageInx++);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtRef2);

			strSaveImage.Format(_T("%s\\Apart_PAD_Substance_%d_%d_Result.bmp"),	strSavePath, nROIInx+1, nSaveImageInx++);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtSubstanceRst1);

			strSaveImage.Format(_T("%s\\Apart_PAD_Substance_Inv_%d_%d_Result.bmp"),	strSavePath, nROIInx+1, nSaveImageInx++);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtSubstanceRst2);

			strSaveImage.Format(_T("%s\\Apart_PAD_White_%d_%d_MeanFill.bmp"),	strSavePath, nROIInx+1, nSaveImageInx++);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtPrcMask[nROIInx]);

			strSaveImage.Format(_T("%s\\Apart_PAD_Dark_%d_%d_MeanFill.bmp"),	strSavePath, nROIInx+1, nSaveImageInx++);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtPrcMask2[nROIInx]);
		}

		mtPrcMask[nROIInx].copyTo(mtGaus);
		mtPrcMask[nROIInx].copyTo(mtBG);

		if(nMaskBlurSize_Guas % 2 == 0)
			nMaskBlurSize_Guas += 1;
		if(nMaskBlurSize % 2 == 0)
			nMaskBlurSize += 1;

		cv::GaussianBlur(mtGaus, mtGaus, Size(nMaskBlurSize_Guas, nMaskBlurSize_Guas), dGaus_Sigma);
		cv::GaussianBlur(mtPrcMask2[nROIInx], mtPrcMask2[nROIInx], Size(nMaskBlurSize_Guas, nMaskBlurSize_Guas), dGaus_Sigma);

		if(bImageSave)
		{
			strSaveImage.Format(_T("%s\\Apart_PAD_White_%d_%d_Gaus.bmp"),	strSavePath, nROIInx+1, nSaveImageInx);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtGaus);

			strSaveImage.Format(_T("%s\\Apart_PAD_Dark_%d_%d_Gaus.bmp"),	strSavePath, nROIInx+1, nSaveImageInx++);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtPrcMask2[nROIInx]);
		}


		mtGaus.copyTo(mtDivideTh);

		int nDivideRange = (float)mtPrcMask[nROIInx].rows / nDivide;
		int nHeight		 = nDivideRange;
		for(int n = 0; n < nDivide; n++)
		{
			if(n == nDivide - 1)
				nHeight = mtDivideTh.rows - nDivideRange * n;
			rtDivide = Rect(0, 0 + n * nDivideRange, mtDivideTh.cols, nHeight);

			Scalar m, s;
			cv::meanStdDev( mtDivideTh(rtDivide), m, s);
			cv::threshold(mtDivideTh(rtDivide), mtDivideTh(rtDivide), (double)m[0] * dMeanWeight, 255, CV_THRESH_BINARY_INV);
			cv::meanStdDev( mtPrcMask2[nROIInx](rtDivide), m, s);

			double dThreshold2 = m[0] * (dMeanWeight - 0.1);
			if(dThreshold2 < 0)
				dThreshold2 = 0;
			cv::threshold(mtPrcMask2[nROIInx](rtDivide), mtPrcMask2[nROIInx](rtDivide), dThreshold2, 255, CV_THRESH_BINARY_INV);
		}

		if(bImageSave)
		{
			strSaveImage.Format(_T("%s\\Apart_PAD_White_%d_%d_Divide TH.bmp"),	strSavePath, nROIInx+1, nSaveImageInx);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtDivideTh);

			strSaveImage.Format(_T("%s\\Apart_PAD_Dark_%d_%d_Divide TH.bmp"),	strSavePath, nROIInx+1, nSaveImageInx++);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtPrcMask2[nROIInx]);
		}

		Mat mtResult;
		cv::add(mtPrcMask2[nROIInx], mtDivideTh, mtResult);
		cv::add(mtResult, mtSubstanceRst3, mtResult);

		mtResult.copyTo(mtROI[nROIInx]);

		if(bImageSave)
		{
			strSaveImage.Format(_T("%s\\Apart_PAD_%d_%d_Result Add.bmp"),	strSavePath, nROIInx+1, nSaveImageInx++);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtROI[nROIInx]);
		}

		cv::morphologyEx(mtROI[nROIInx], mtROI[nROIInx], MORPH_CLOSE, cv::getStructuringElement(MORPH_RECT, Size(5,5)));

		if(bImageSave)
		{
			strSaveImage.Format(_T("%s\\Apart_PAD_%d_%d_Result Morph.bmp"),	strSavePath, nROIInx+1, nSaveImageInx++);
			cv::imwrite((cv::String)(CStringA)strSaveImage, mtROI[nROIInx]);
		}
	}

	delete[] mtPrcMask;
	delete[] mtPrcMask2;

	return E_ERROR_CODE_TRUE;
}

long CInspectPad::StretchBottom_V(cv::Mat& Src, cv::Mat& Dst, double dStretchRatio)
{

	Mat mtPrc = Mat::zeros(Src.size(), CV_8UC1);

	uchar* pSrcData;
	uchar* pPrcdata;
	double dMove;
	
	if(dStretchRatio == 0)
		return E_ERROR_CODE_TRUE;

	if(dStretchRatio > 0)
	{
		for(int nY = 0; nY < Src.rows; nY++)
		{
			pSrcData = Src.data + nY * Src.step;
			pPrcdata = mtPrc.data + nY * mtPrc.step;
			dMove = dStretchRatio * nY;
			if(nY + (int)dMove >= Src.rows)
				continue;
			for(int nX = 0; nX < Src.cols; nX++)
			{
				for(int nMove = 0; nMove <= (int)dMove; nMove++)
				{
					if(nMove == (int)dMove)
						*(pPrcdata + (nMove * Src.step + nX)) = *(pSrcData + nX);
					else
					{
						if((int)*(pPrcdata + (nMove * Src.step + nX)) == 0)	//홀일 떄만 채워 준다.
						{
							if(nY == 0)
								*(pPrcdata + (nMove * Src.step + nX)) = *(pSrcData + nX);
							else
								*(pPrcdata + (nMove * Src.step + nX)) = *(pSrcData - Src.step + nX);
						}
					}
				}
			}
		}
	}
	else
	{
		for(int nY = 0; nY < Src.rows; nY++)
		{
			pSrcData = Src.data + nY * Src.step;
			pPrcdata = mtPrc.data + nY * mtPrc.step;
			dMove = dStretchRatio * nY;
			if(nY + (int)dMove <= 0)
				continue;
			for(int nX = 0; nX < Src.cols; nX++)
			{
				*(pPrcdata + ((int)dMove * Src.step + nX)) = *(pSrcData + nX);
			}
		}
	}
	
	
	mtPrc.copyTo(Dst);

	return E_ERROR_CODE_TRUE;
}

long CInspectPad::calcRANSAC(std::vector <cv::Point2f>& ptSrcIndexs, long double& dA, long double& dB, int nMinSamples, double distThreshold)
{
	long nMaxCost = 0;

	// 벡터 초기화
	vector <cv::Point2f> ptSamples, ptInliers;

	// 임시 라인 방정식 y = ax + b
	long double dAA = 0, dBB = 0;

	// 반복 횟수 정하기
	int nMaxIter = (int)(1 + log(1. - 0.99)/log(1. - pow(0.5, nMinSamples)));

	long nErrorCode = E_ERROR_CODE_TRUE ;
	for (int i = 0; i<nMaxIter; i++)
	{
		// 벡터 초기화
		vector<cv::Point2f>().swap(ptSamples);
		vector<cv::Point2f>().swap(ptInliers);

		// hypothesis
		// 원본 데이터에서 임의로 N개의 샘플 데이터를 취득
		GetRandomSamples(ptSrcIndexs, ptSamples, nMinSamples);

		// 현재 데이터를 정상적인 데이터로 보고 모델 파라미터 예측
		calcLineFit(ptSamples, dAA, dBB);

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
		vector<cv::Point2f>().swap(ptInliers);
	}

	// 벡터 데이터 제거
	vector<cv::Point2f>().swap(ptSamples);

	// 계산 불가
	if( nMaxCost <= 0 )		return E_ERROR_CODE_FALSE;

	return E_ERROR_CODE_TRUE;
}

long CInspectPad::GetRandomSamples(std::vector <cv::Point2f>& ptSrcIndexs, std::vector <cv::Point2f>& ptSamples, int nSampleCount)
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

	return true;
}

long  CInspectPad::calcLineFit(std::vector <cv::Point2f>& ptSamples, long double& dA, long double& dB)
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
long  CInspectPad::calcLineVerification(std::vector <cv::Point2f>& ptSrcIndexs, std::vector <cv::Point2f>& ptInliers, long double& dA, long double& dB, double distThreshold)
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
bool CInspectPad::FindInSamples(std::vector <cv::Point2f>& ptSamples, cv::Point2f ptIndexs)
{
	for (int i=0 ; i<ptSamples.size() ; i++)
	{
		if ( ptSamples[i].x == ptIndexs.x &&
			ptSamples[i].y == ptIndexs.y )
			return true;
	}

	return false;
}

long CInspectPad::DeleteCuttingTresh(cv::Mat& Src, int nWidthOffset)
{
	Rect rtTreshArea = Rect(0,0,nWidthOffset,Src.rows);

	Src(rtTreshArea).setTo(0);

	return E_ERROR_CODE_TRUE;
}

long CInspectPad::FindPadDust(Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* pCommonPara, wchar_t* strAlgPath, 
	STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt)
{

	int nImageSaveInx = 0;

	//영상 버퍼, ROI 이상 Check
	if(matSrcBuffer.empty() || matDrawBuffer.empty() || 
		rtInspArea.x < 0 || rtInspArea.y < 0 || rtInspArea.width < 1 || rtInspArea.height < 1 
		|| rtInspArea.width > matSrcBuffer.cols || rtInspArea.height > matSrcBuffer.rows)
		return E_ERROR_CODE_EMPTY_BUFFER;

	//중간 결과 이미지
	CString strAresult;

	bool	bImageSave			= (pCommonPara[1] > 0) ? true : false;	// 1 : 알고리즘 중간결과 Image Save 

	//Parameter
	int nPos = 0;
	int		nMaximumFilterSize			= (int)dPara[nPos++];			//Maximum Filter Size
	float	fGausSigma					= (float)dPara[nPos++];			//가우시안 블러 시그마 크기
	int		nThreshold					= (int)dPara[nPos++];			//이진화 값
	int		nMorphkernel				= (int)dPara[nPos++];			//Morphlogy Mask 사이즈
	int		nDustLT_Position			= (int)dPara[nPos++];			//Cutting Dust Left Top 위치
	int		nTBDustCuttingOffset		= (int)dPara[nPos++];			//Cutting Dust 비검사 영역 절단

	GetCheckMaskSize(nMaximumFilterSize);
	GetCheckMaskSize(nMorphkernel);

	// Pad Cutting 잔재물 비 검출 영역 절단
	rtInspArea = SetCuttingNonInspect(rtInspArea,0,nTBDustCuttingOffset,TRUE);

	Mat mtOrgROI;
	Mat mtSrc;
	matSrcBuffer(rtInspArea).copyTo(mtSrc);
	matSrcBuffer(rtInspArea).copyTo(mtOrgROI);

	//0.필터처리
	MaximumFilter(mtSrc, mtSrc, nMaximumFilterSize);
	if(bImageSave)
	{
		strAresult.Format(_T("%s\\PadDust_%d.bmp"), strAlgPath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strAresult, mtSrc);
	}

	//1.이진화
	cv::threshold(mtSrc, mtSrc, nThreshold, 255, CV_THRESH_BINARY);

	if(bImageSave)
	{
		strAresult.Format(_T("%s\\PadDust_%d.bmp"), strAlgPath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strAresult, mtSrc);
	}

	//2.Blob 하기 전에 Blob 갯수 줄이기 위한 모폴로지
	if(nMorphkernel > 2)
	{
		Mat StructElem = cv::getStructuringElement(MORPH_RECT, Size(nMorphkernel, nMorphkernel));
		cv::morphologyEx(mtSrc, mtSrc, MORPH_OPEN, StructElem);
	}

	if(bImageSave)
	{
		strAresult.Format(_T("%s\\PadDust_%d.bmp"), strAlgPath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strAresult, mtSrc);
	}
	
	//3.PAD ROI 영상 좌측에 붙어있는 Contour들은 Cutting 잔재물로 판단
	// 2018.10.02 Cutting 잔재물을 Bounding Rect 하였을 경우 좌측 상단 x좌표가 Parameter보다 작을 경우 Defect 처리
	vector<vector<Point>> ptContours;
	cv::findContours(mtSrc, ptContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cv::Point(0,0));

	Mat mtCuttingDust = Mat::zeros(mtSrc.size(), CV_8UC1);

	for(int n = 0; n < ptContours.size(); n++)
	{
		cv::Rect rtBound = cv::boundingRect(ptContours[n]);
		if(rtBound.tl().x < nDustLT_Position)
		{
			cv::drawContours(mtCuttingDust, ptContours, n, Scalar(255), -1);
		}
	}

	if(bImageSave)
	{
		strAresult.Format(_T("%s\\PadDust_%d.bmp"), strAlgPath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strAresult, mtCuttingDust);
	}

	if(nMorphkernel > 2)
	{
		Mat StructElem = cv::getStructuringElement(MORPH_RECT, Size(nMorphkernel, nMorphkernel));
		cv::morphologyEx(mtCuttingDust, mtCuttingDust, MORPH_CLOSE, StructElem);
	}

	if(bImageSave)
	{
		strAresult.Format(_T("%s\\PadDust_%d.bmp"), strAlgPath, nImageSaveInx++);
		imwrite((cv::String)(CStringA)strAresult, mtCuttingDust);
	}

	Mat mtPartDraw = matDrawBuffer(rtInspArea);

	CFeatureExtraction cFeatureExtraction;
	cFeatureExtraction.SetMem(cMem);

	int nPrevDefectCount = pResultBlob->nDefectCount;
	cFeatureExtraction.DoDefectBlobSingleJudgment(mtOrgROI, mtCuttingDust, mtPartDraw, pCommonPara,
		E_DEFECT_COLOR_BRIGHT, strContourTxt, EngineerDefectJudgment, pResultBlob,  E_DEFECT_JUDGEMENT_APP_PAD_DUST, false);
	cFeatureExtraction.DrawBlob(mtPartDraw, cv::Scalar(0,0,255), BLOB_DRAW_BLOBS_CONTOUR, true);


	int nCurrDefectCount = pResultBlob->nDefectCount;

	for(long n =nPrevDefectCount; n < nCurrDefectCount ;n++ )
	{
		pResultBlob->ptLT[n].x		+= rtInspArea.x;
		pResultBlob->ptLT[n].y		+= rtInspArea.y;
		pResultBlob->ptRT[n].x		+= rtInspArea.x;	
		pResultBlob->ptRT[n].y		+= rtInspArea.y;
		pResultBlob->ptRB[n].x		+= rtInspArea.x;
		pResultBlob->ptRB[n].y		+= rtInspArea.y;
		pResultBlob->ptLB[n].x		+= rtInspArea.x;
		pResultBlob->ptLB[n].y		+= rtInspArea.y;
	}

	if( !USE_ALG_CONTOURS )	// 결과 외곽선 저장
		cFeatureExtraction.SaveTxt(pCommonPara, strContourTxt, true);
	
	rtInspArea = SetCuttingNonInspect(rtInspArea,0,nTBDustCuttingOffset,FALSE);

	return E_ERROR_CODE_TRUE;
}

long CInspectPad::GetHistogram(Mat& matSrcImage, Mat& matHisto, Mat matMask)
{
	int nBit = GetBitFromImageDepth(matSrcImage.depth());
	if(nBit == -1)	return E_ERROR_CODE_APP_HISTO;

	int nHistSize = (int)pow((double)256,(double)nBit);
	float fHistRange[] = {(float)0, (float)nHistSize-1};
	const float *ranges[] = { fHistRange };

	cv::calcHist(&matSrcImage,1, 0, matMask, matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;

}

int CInspectPad::GetBitFromImageDepth(int nDepth)
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

long CInspectPad::MakeMask(Mat* mtROI, Mat* mtMask, CString strPath, int nROIInx)
{
	Mat mtPrc;	//처리용 영상 버퍼

	CString str;


	if(mtROI[nROIInx].empty())		return E_ERROR_CODE_PAD_MASK_REF_EMPTY;

	mtMask[nROIInx] = Mat::zeros(mtROI[nROIInx].size(), mtROI[nROIInx].type());

	Edge_BasicSobel(mtROI[nROIInx], mtPrc);

	Scalar m,s;
	cv::meanStdDev(mtPrc, m, s);

	cv::threshold(mtPrc, mtPrc, m[0]*0.98, 255, CV_THRESH_BINARY);

	vector<vector<Point2i>> ptContours;

	cv::findContours(mtPrc, ptContours, RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	cv::fillPoly(mtMask[nROIInx], ptContours, Scalar(255));

	cv::morphologyEx(mtMask[nROIInx], mtMask[nROIInx], MORPH_ERODE, getStructuringElement(MORPH_RECT, Size(7,7)));

	cv::morphologyEx(mtMask[nROIInx], mtMask[nROIInx], MORPH_DILATE, getStructuringElement(MORPH_RECT, Size(3,3)));

	cv::bitwise_and(mtROI[nROIInx], mtMask[nROIInx], mtPrc);

	cv::meanStdDev(mtPrc, m, s);
	
	cv::threshold(mtPrc, mtMask[nROIInx], m[0], 255, CV_THRESH_BINARY);


	return E_ERROR_CODE_TRUE;
}

bool CInspectPad::CheckMask(Mat* mtROI, Mat* mtMask, int nROINum)
{
	bool bCheck = true;
	
	for(int n=0; n < nROINum; n++)
	{
		//영상이 없다면
		if(mtROI[n].empty() || mtMask[n].empty())
			return false;

		//ROI와 Mask의 크기가 서로 다르다면
		if(mtROI[n].cols != mtMask[n].cols)
			bCheck = false;
		if(mtROI[n].rows != mtMask[n].rows)
			bCheck = false;
	}

	return bCheck;
}

long CInspectPad::DeleteRectRecovery(Mat DeleteBefore, Mat& DeleteAfter, int nDeleteRect)
{
	cv::Mat AfterContour;
	DeleteAfter.copyTo(AfterContour);
	
	vector<vector<cv::Point>> ptContours;
	vector<Vec4i> hierarchy;

	cv::findContours( AfterContour, ptContours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image

	int nRecovery_Range = nDeleteRect + 2;

	uchar* BeforeData;
	uchar* AfterData;

	for(int i = 0 ; i < ptContours.size(); i++)
	{
		Rect rt = cv::boundingRect(ptContours[i]);
		if(rt.tl().x < nRecovery_Range)
		{
			for(int nY = rt.tl().y; nY <= rt.br().y; nY++)
			{
				BeforeData = DeleteBefore.ptr(nY);
				AfterData = DeleteAfter.ptr(nY);
				for(int nX = rt.tl().x; nX >= 0; nX--)
				{
					if(rt.tl().x + 1 > DeleteBefore.cols - 1)
						break;

					if((int)AfterData[rt.tl().x + 1] == 255)
					{
						if((int)BeforeData[nX] == 255)
						{
							AfterData[nX] = 255;
						}
					}
					else
						break;
				}
			}
		}
	}	

	return E_ERROR_CODE_TRUE;
}

/*
[Input]
mtSrcBinary = Pad 불량 후보군 이진 영상
mtDustOrg	= Dust 원본 영상

[Output]
mtDst		= Pad 불량 후보군에서 Dust 후보군을 제거한 이진영상
mtDustDst	= Dust 후보군 이진 영상
*/

long CInspectPad::ClassifyPadDust(Mat mtSrcBinary, Mat& mtDst, Mat mtDustOrg, Mat& mtDustDst, PAD_DUST_FEATURE PadDustFeature, CString strAresultPath, bool bImageSave, int nImageInx)
{
	CString strSavePath;

	if(bImageSave)
	{
		strSavePath.Format(_T("%s\\PAD_DUST_FT"), strAresultPath);
		//경로 Check
		DWORD result;
		if (((result=GetFileAttributes(strSavePath)) == -1 ) || !(result&FILE_ATTRIBUTE_DIRECTORY) ) {
			CreateDirectory(strSavePath, NULL);
		}
	}

	cv::Mat matLabel, matStats, matCentroid;

	Mat mtPrc			=	Mat::zeros(mtSrcBinary.size(), mtSrcBinary.type());
	Mat mtDustPrc		=	Mat::zeros(mtDustOrg.size(), mtDustOrg.type());;	

	vector<vector<cv::Point>> ptContours;

	cv::findContours(mtSrcBinary, ptContours, RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	Rect rtBound;
	Scalar m, s;
 	float fPerimeter;
 	float fRoundness;
	float fArea;


	for(int nInx =0; nInx < ptContours.size(); nInx++)
	{
		fArea	= cv::contourArea(ptContours[nInx]);
 		fPerimeter = cv::arcLength(ptContours[nInx], true);
 		fRoundness = (4* CV_PI* fArea)/ pow(fPerimeter,2);
		rtBound = cv::boundingRect(ptContours[nInx]);
		Mat mtMask = Mat::zeros(Size(rtBound.width,rtBound.height), mtSrcBinary.type());
		cv::drawContours(mtMask, ptContours, nInx, Scalar(255), CV_FILLED, 8, vector<Vec4i>(), 0, Point(-rtBound.tl().x, -rtBound.tl().y)); //원영상에 그리는 멍청한 짓은하지 않겠지? 이것이 비법이다....
		cv::meanStdDev(mtDustOrg(rtBound), m, s, mtMask);

		Mat mtAnd = Mat::zeros(Size(rtBound.width, rtBound.height), mtSrcBinary.type());
		cv::bitwise_and(mtDustOrg(rtBound), mtMask, mtAnd);

 		float fSpecificGV = UpwardOfSpecificGVCount(mtAnd, 150);

		if(fArea == 0)
			continue;
		float fSpecificGVRatio = fSpecificGV / fArea;

		if(m[0] > PadDustFeature.fMeanGV||
		fSpecificGVRatio > PadDustFeature.fSpecificGVRatio||
		fRoundness < PadDustFeature.fRoundness)	// Cutting Dust는 선형성을 가짐  Parameter는 1에 가까워질수록 원형성을 띈다.
			cv::drawContours(mtDustPrc, ptContours, nInx, Scalar(255), CV_FILLED);
		else
			cv::drawContours(mtPrc, ptContours, nInx, Scalar(255), CV_FILLED);


		if(bImageSave)
		{
			CString strTest;
			strTest.Format(_T("%s\\%d_%d_Mask(GVRatio_%f).bmp"), strSavePath, nImageInx, nInx, fSpecificGVRatio);
			imwrite((cv::String)(CStringA)strTest, mtMask);
			strTest.Format(_T("%s\\%d_%d_Dust(MEAN_%f).bmp"),strSavePath, nImageInx, nInx, (float)m[0]);
			imwrite((cv::String)(CStringA)strTest, mtDustOrg(rtBound));
		}

	}

	mtPrc.copyTo(mtDst);
	mtDustPrc.copyTo(mtDustDst);

	return E_ERROR_CODE_TRUE;
}

int	CInspectPad::UpwardOfSpecificGVCount(Mat mtSrc, int nGV)
{
	uchar* ucImgdata;
	int nCount = 0;
	for(int nY = 0; nY < mtSrc.rows; nY++)
	{
		ucImgdata = mtSrc.ptr(nY);
		for(int nX = 0; nX < mtSrc.cols; nX++, ucImgdata++)
		{
			if(nGV < (int)*ucImgdata)
				nCount++;
		}
	}

	return nCount;
}

// 2018.08.01 Cutting 잔재물 검출 영역 재 설정
Rect CInspectPad::SetCuttingNonInspect(Rect rtResizeCuttingArea, int nLTCuttingOffset, int nTBCuttingOffset, BOOL bResize)
{
	if(bResize)
	{
		rtResizeCuttingArea.x += nLTCuttingOffset;
		rtResizeCuttingArea.y += nTBCuttingOffset;
		rtResizeCuttingArea.width -= nLTCuttingOffset;
		rtResizeCuttingArea.height -= (nTBCuttingOffset * 2);
	}

	else
	{
		rtResizeCuttingArea.x -= nLTCuttingOffset;
		rtResizeCuttingArea.y -= nTBCuttingOffset;
		rtResizeCuttingArea.width += nLTCuttingOffset;
		rtResizeCuttingArea.height += (nTBCuttingOffset * 2);
	}

	return rtResizeCuttingArea;
}