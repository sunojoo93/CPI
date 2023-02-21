#pragma once

#include "Define.h"
#include "MatBuf.h"				// �޸� ����

enum {PADMARK_T, PADMARK_B, PADMARK_NUM};
enum {SRC, REF1, REF2, REF3, IMG_NUM};
enum {ROI1, ROI2, ROI3, ROI_NUM};

struct PAD_DUST_FEATURE
{
	float	fMeanGV;
	int		nArea;
	int		fMinGV;
	int		nMax;
	float	fRoundness;
	float	fSpecificGVRatio;
	int		nMaxDustArea;

	PAD_DUST_FEATURE()
	{
		fMeanGV				=0;
		nArea				=0;
		fMinGV				=0;
		nMax				=0;
		fRoundness			=0;
		fSpecificGVRatio	=0;
		nMaxDustArea		=0;
	}
};

class CInspectPad
{
// ���� ����̺� D:\ ���� �ƴ� - �ʿ� �� InspectLibLog �� GETDRV() Ȱ�� - ��� ������???
// 	#define MARKPATH_T	_T("D:\\CRUX\\DATA\\MODEL\\mark1.bmp")
// 	#define MARKPATH_B	_T("D:\\CRUX\\DATA\\MODEL\\mark2.bmp")
// 	/*#define MASKPATH	_T("D:\\CRUX\\DATA\\MODEL\\mask.bmp")*/
// 	#define REF2PATH	_T("D:\\CRUX\\DATA\\MODEL\\Ref2.bmp")
// 	#define REFPATH	_T("D:\\CRUX\\DATA\\MODEL\\Ref.bmp")
// 	#define SRCPATH	_T("D:\\CRUX\\DATA\\MODEL\\APP_TEST\\InspRecipe\\17-08-01_01-01-01.1\\Pad_SRC.bmp")

	struct sMatchingScore
	{
		double	dScore;
		Point	ptPosition;

		sMatchingScore()
		{
			dScore			= 0.0;
			ptPosition.x	= 0.0;
			ptPosition.y	= 0.0;
		}

	};
private : 
	//		��� ������ ���� �Ѵ�.
	long	CalcSegmentArea(const int nSegSize, const int nImageW, const int nImageH, vector<Rect> &o_vecROI);

	//		���ҵ� ������ ��Ī �������� ����. :  X - Y Offset�� ���� �� �ִ� ������ �ִ��� �Ǵ��Ѵ�.
	bool	GetEnableMatching(Mat Src);

	//		Src �� Tem�� ��Ī�Ͽ� ��Ī ����Ʈ�� ���� �Ѵ�.
	long	GetPreMatchingInfo(Mat matSrcBuffer, Mat matReferBuffer, vector<Rect >vecInspSrc, vector<Rect > vecInspTem, vector<sMatchingScore>		&o_vecMatchingInfo);

	long	GetPreMatchingInfo2(Mat matSrcBuffer, Mat matReferBuffer,  vector<Rect >vecInspSrc,vector<Rect > vecInspTem, vector<sMatchingScore>		&o_vecMatchingInfo);

	// PAD TEST
	bool MARKPOSITION(cv::Mat Src, cv::Mat Mark, cv::Point2f& mPosition);

	long calcCurveFit(std::vector <cv::Point2f>& ptSamples, long double& dA, long double& dB, long double& dC);

	long PAD_Bright_Adjust(cv::Mat* mtImg, int nImageNum);
	
	long PAD_Bright_MeanAdjust(cv::Mat* mtImg, int nImgNum, int nDivideNum);

	long CCutLineDelete(cv::Mat* mtImg, int nImageNum, Point2f ptMark_T, Point2f ptMark_B, int nCutInterceptOffset, int nCutGV);

	long DeleteNoneArea_Match(cv::Mat* mtImg, int nImageNum, cv::Mat* mtNone,  STRU_INFO_PAD** stPadInfo);
	
	long MaskProcess(cv::Mat* mtROI, cv::Mat* mtMask, int nROINum, double dGaus_Sigma, int nMaskBlurSize_Guas, int nMaskBlurSize, double dTh , double dMeanWeight, CString strSavePath, Mat* mtMatch, int nMaskDivde, bool bImageSave=false);

	long StretchBottom_V(cv::Mat& Src, cv::Mat& Dst, double dStretchRatio);

	long DeleteCuttingTresh(cv::Mat& Src, int nWidthOffset);

	Rect SetCuttingNonInspect(Rect rtResizeCuttingArea, int nLTCuttingOffset, int nTBCuttingOffset, BOOL bResize);

	//Line Pitting
	long calcRANSAC(std::vector <cv::Point2f>& ptSrcIndexs, long double& dA, long double& dB, int nMinSamples, double distThreshold);
	long GetRandomSamples(std::vector <cv::Point2f>& ptSrcIndexs, std::vector <cv::Point2f>& ptSamples, int nSampleCount);
	long calcLineFit(std::vector <cv::Point2f>& ptSamples, long double& dA, long double& dB);
	long calcLineVerification(std::vector <cv::Point2f>& ptSrcIndexs, std::vector <cv::Point2f>& ptInliers, long double& dA, long double& dB, double distThreshold);
	bool FindInSamples(std::vector <cv::Point2f>& ptSamples, cv::Point2f ptIndexs);

	//Histogram
	long GetHistogram(Mat& matSrcImage, Mat& matHisto, Mat matMask = Mat());
	int GetBitFromImageDepth(int nDepth);

	bool GetAvgOfStdevRange(Mat matHisto, Scalar Avg, Scalar Stdev, double& dReturnAvg);

	long MakeMask(Mat* mtROI, Mat* mtMask, CString strPath, int nROIInx);	//���� ���� Mask �����
	bool CheckMask(Mat* mtROI, Mat* mtMask, int nROINum);					//���� ���� ROI ����� Mask ������� ũ�� ��, ũ�Ⱑ ���� ���� ���¿��� Mask�� ����ϸ� Program Down

	//Threshold ��� Pad Algorithm Sub �Լ�
	long DeleteRectRecovery(Mat DeleteBefore, Mat& DeleteAfter, int nDeleteRect);		

	//Cutting ���繰 �з� Algorithm
	long ClassifyPadDust(Mat mtSrcBinary, Mat& mtDst, Mat mtDustOrg, Mat& mtDustDst, PAD_DUST_FEATURE PadDustFeature, CString strAresultPath, bool bImageSave, int nImageInx);
	int	 UpwardOfSpecificGVCount(Mat mtSrc, int nGV);

public:
	CInspectPad(void);
	~CInspectPad(void);

	// �޸� ����
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};


	long FindPadScratchDefect(Mat matSrcBuffer, Mat matReferBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

	long FindPadScratchDefect2(Mat matSrcBuffer, Mat matReferBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt = NULL);

	long FindPadScratchDefect3(Mat matMainBuffer, Mat matReferBuffer, cv::Mat matDust, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CString strCurRecipePath, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt = NULL);

	long GoldenImageProcess(Mat matMainBuffer, Mat matReferBuffer, cv::Mat matDust, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CString strCurRecipePath, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt = NULL);

	long ThresholdProcess(Mat matMainBuffer, Mat matReferBuffer, cv::Mat matDust, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* nCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CString strCurRecipePath, STRU_INFO_PAD** stPadInfo, wchar_t* strContourTxt = NULL);

	void GammaCorrection(	Mat matSrcBuffer,Mat matThrBuffer, float fGamma );				// Gamma

	long FindPadDust(Mat matSrcBuffer, cv::Mat& matDrawBuffer, Rect rtInspArea, double* dPara, int* pCommonPara, wchar_t* strAlgPath, 
		STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);
};

