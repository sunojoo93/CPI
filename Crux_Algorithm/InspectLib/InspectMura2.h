/************************************************************************
Mura2 �˰��� ���� ���
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

	// Main �˻� �˰���
	long		FindMuraDefect(cv::Mat matSrcBuffer, cv::Mat& matDrawBuffer, cv::Point* ptCorner, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

	long LogicStart_Gray(cv::Mat& matSrcImage, cv::Mat &matDarkResultImage_01, cv::Mat &matBrightResultImage_01,
						 CRect rectROI, double* dPara,int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

protected:

	//������ ����� ��� ��� GV�� Ư�� ���� �̻� ���� Min, Max������ �����ش�.
	void SetMinMax(Mat& matSrcImage,int nMaxGVAreaPartX,int nMaxGVAreaPartY,double dblMinGVR,double dblMaxGVR);

	//�ܰ��� �߾��� ���� Threshold ó�����ִ� �Լ�
	void MakeThresholdImageWithCenterAndOut(Mat& matSrcImage,int nOutLine,int nThresholdCen,int nThresholdOut);

	//Ư�� GV ���� ��� - �������� �� �̹������� ����� �Ŷ� �ּ� Pixel ���� �̿ܿ��� ���� ����Ѵ�.
	double GetAverage(Mat& matSrcImage,double dblAveCutOffCountR_Min,int nAveMinStartGV,int nAveMaxEndGV);

	//������׷�
	long GetHistogram(Mat& matSrcImage, Mat& matHisto);
	int GetBitFromImageDepth(int nDepth);

	//��� GV���� ������ GV�� �ǵ��� �����Ѵ�. - �˰��� Ư���� ��� GV ���� ���� ���̰� ���ԵǸ� Threshold ������ ����
	void ApplyMeanGV(Mat &matSrcImage,double dblTargetGV);

	//Thresold �̹����� ������ش�. (����ȭ �̹��� ����)
	void MakeThresholdImage(Mat &matOri,Mat &matDst,double dblAverage,float fThresholdR,double fThresholdR_Outline,int nOutLineArea);

	//���� �̹����� �����.
	void MakeBGImage(int nMethod,Mat &matSrcImage, Mat &matBGImage,Size szParam01);
	void MakeBGImage_Blur(Mat &matSrcImage, Mat &matBGImage,Size szParam01);
	void MakeBGImage_Profile(Mat &matSrcImage, Mat &matBGImage);
	void MakeProfile(int nType,Mat &matSrcImage,Mat &matDstImage); // ���� ���������� ������ش�.
};

