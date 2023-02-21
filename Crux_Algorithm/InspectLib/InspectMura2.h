/************************************************************************
Mura2 알고리즘 관련 헤더
************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"

#define BG_METHOD_BLUR 0
#define BG_METHOD_PROFILE 1

#define PROFILE_ROW 0
#define PROFILE_COL 1

class CInspectMura2
{
public:
	CInspectMura2(void);
	virtual ~CInspectMura2(void);

	// Main 검사 알고리즘
	long		FindMuraDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

	long LogicStart_Gray(cv::Mat& matSrcImage, cv::Mat &matDarkResultImage_01, cv::Mat &matBrightResultImage_01,
						 CRect rectROI, double* dPara,int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

protected:

	//구간별 평균을 내어서 평균 GV의 특정 비율 이상 값을 Min, Max값으로 눌러준다.
	void SetMinMax(Mat& matSrcImage,int nMaxGVAreaPartX,int nMaxGVAreaPartY,double dblMinGVR,double dblMaxGVR);

	//외곽과 중앙을 따로 Threshold 처리해주는 함수
	void MakeThresholdImageWithCenterAndOut(Mat& matSrcImage,int nOutLine,int nThresholdCen,int nThresholdOut);

	//특정 GV 구간 평균 - 바탕에서 뺀 이미지에서 사용할 거라 최소 Pixel 개수 이외에는 전부 평균한다.
	double GetAverage(Mat& matSrcImage,double dblAveCutOffCountR_Min,int nAveMinStartGV,int nAveMaxEndGV);

	//히스토그램
	long GetHistogram(Mat& matSrcImage, Mat& matHisto);
	int GetBitFromImageDepth(int nDepth);

	//평균 GV값을 설정한 GV가 되도록 설정한다. - 알고리즘 특성상 평균 GV 값이 많이 차이가 나게되면 Threshold 문제가 생김
	void ApplyMeanGV(Mat &matSrcImage,double dblTargetGV);

	//Thresold 이미지를 만들어준다. (이진화 이미지 생성)
	void MakeThresholdImage(Mat &matOri,Mat &matDst,double dblAverage,float fThresholdR,double fThresholdR_Outline,int nOutLineArea);

	//바탕 이미지를 만든다.
	void MakeBGImage(int nMethod,Mat &matSrcImage, Mat &matBGImage,Size szParam01);
	void MakeBGImage_Blur(Mat &matSrcImage, Mat &matBGImage,Size szParam01);
	void MakeBGImage_Profile(Mat &matSrcImage, Mat &matBGImage);
	void MakeProfile(int nType,Mat &matSrcImage,Mat &matDstImage); // 라인 프로파일을 만들어준다.
};

