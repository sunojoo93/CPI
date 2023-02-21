
/************************************************************************/
// Align ���� ���� ���
// ������ : 18.02.07 - KSW
/************************************************************************/

#pragma once

#include "Define.h"
#include "InspectLibLog.h"
#include "MatBuf.h"				// �޸� ����
#include <algorithm>
#include "DefectCCD.h"
#include <stdlib.h>

enum 
{
	TOP=0, BOTTOM, LINE_END
};

class CInspectAlign
{
public:
	CInspectAlign(void);
	virtual ~CInspectAlign(void);

	// �޸� ����
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};

	// �α� ����
	InspectLibLog*		m_cInspectLibLog;
	clock_t				m_tInitTime;
	clock_t				m_tBeforeTime;
	wchar_t*			m_strAlgLog;

	void		SetLog(InspectLibLog* cLog, clock_t tTimeI, clock_t tTimeB, wchar_t* strLog)
	{
		m_tInitTime			= tTimeI;
		m_tBeforeTime		= tTimeB;
		m_cInspectLibLog	= cLog;
		m_strAlgLog			= strLog;
	};

	void		writeInspectLog(int nAlgType, char* strFunc, wchar_t* strTxt)
	{
		if( m_cInspectLibLog == NULL )
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, m_strAlgLog);
	};

	//////////////////////////////////////////////////////////////////////////

	void		writeInspectLog_Memory(int nAlgType, char* strFunc, wchar_t* strTxt, __int64 nMemory_Use_Value = 0)
	{
		if (m_cInspectLibLog == NULL)
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, nMemory_Use_Value, m_strAlgLog);
	};

	///////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// Top Line ���� ã�� ( ī�޶� ���� ������ ) ( 8bit & 12bit )
	long	DoFindTheta(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point& ptCenter, wchar_t* strID=NULL);

	// AVI �˻� ���� ã�� ( 8bit & 12bit )
	long	DoFindActive(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point* ptResCorner, int nRatio, cv::Point& ptCenter, wchar_t* strID=NULL);

	long	DoFindActive_LCP(cv::Mat& matSrcBuf, cv::Mat matDrawBuf, cv::Mat matBackStage, cv::Mat& matMask_Active, cv::Mat& matMask_Hole,double* dPara, double& dTheta, int nAlignImageClassify,int nCameraNum ,int nImageClassify, cv::Point* ptResCorner, cv::Rect2i& rcCellRoi, double dCamResolution, int nRatio, cv::Point& ptCenter, cv::Point& ptAlignCenter, wchar_t* strID = NULL, wchar_t* strAlgPath = NULL);

	long	DoFindActive_LCP_Limpidity(cv::Mat& matSrcBuf, cv::Mat matDrawBuf, cv::Mat matStage, cv::Mat& matMask_Active, cv::Mat& matMask_Hole,double* dPara, double& dTheta, int nAlignImageClassify, int nCameraNum, int nImageClassify, cv::Point* ptResCorner, cv::Rect2i& rcCellRoi, double dCamResolution, int nRatio, cv::Point& ptCenter, cv::Point& ptAlignCenter, wchar_t* strID = NULL, wchar_t* strAlgPath = NULL);
	//�� ��ã�� �Լ�
	//long	DoFindActive_LCP_Mask_D(cv::Mat matSrcBuf, double* dPara, cv::Mat& matDstActive, cv::Mat& matDstHole, int nCameraNum, int nImageClassify, double dCamResolution, int nRatio, cv::Point& ptCenter, wchar_t* strID = NULL, wchar_t* strAlgPath = NULL);

	//long	DoFindActive_LCP_Mask_B(cv::Mat matSrcBuf, double* dPara, cv::Mat& matDstActive, cv::Mat& matDstHole, int nCameraNum, int nImageClassify, double dCamResolution, int nRatio, cv::Point& ptCenter, wchar_t* strID = NULL, wchar_t* strAlgPath = NULL);
	//�� ã�� �Լ�
	long	DoFindActive_LCP_Mask(cv::Mat matSrcBuf, double* dPara, cv::Mat& matDstActive, cv::Mat& matDstLabel, cv::Mat& matDstHole, int nCameraNum, int nImageClassify, double dCamResolution, int nRatio, cv::Point& ptCenter, wchar_t* strID = NULL, wchar_t* strAlgPath = NULL);

	long	DoFindActive_LCP_BACK(cv::Mat& matSrcBuf, cv::Mat& matBackStageBuf, double* dPara, int nAlignImageClassify, int nCameraNum, int nImageClassify, cv::Rect2i& rcActiveRoi, double dCamResolution, int nRatio, cv::Point& ptStandard, wchar_t* strID = NULL, wchar_t* strAlgPath = NULL);

	long	DoFindActive_LCP_Vaccum(cv::Mat& matSrcBuf, cv::Mat& matVaccumHoleBuf, double* dPara, int nAlignImageClassify, int nCameraNum, int nImageClassify, cv::Rect2i& rcActiveRoi, double dCamResolution, int nRatio, cv::Point& ptStandard, wchar_t* strID = NULL, wchar_t* strAlgPath = NULL);

	long    DoFindActive_LCP_AutoCal_Mask(cv::Mat matBackBuf, cv::Mat matEmptyBuf, cv::Mat matSide, CString Path_Result, CString ColorCode, CString T_Judge_Code, double* dPara);

	long	DoFindActive_LCP_AutoCal(cv::Mat matSrcBuf,cv::Mat matActiveMask, cv::Mat matLabelMask, CString PattenName, CString ColorCode, CString T_Judge_Code, CString Path_Result, int nMinGV, int nMaxGV, double& MeanGV, bool& Judge_OK_NG, double* dPara);

	void	VarianceFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);
	// APP �˻� ���� ã��
	long	DoFindActive_APP(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point* ptResCorner, int nRatio, double dCamResolution, double dPannelSizeX, double dPannelSizeY, cv::Point& ptCenter, wchar_t* strID=NULL);

	// SVI �˻� ���� ã��
	long	DoFindActive_SVI(cv::Mat matSrcBuf, double* dPara, double& dTheta, cv::Point* ptResCorner, int nCameraNum, int nRatio, cv::Point& ptCenter, wchar_t* strID=NULL);

	long	SetFindContour(cv::Mat matSrcBuf, INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRoundROICnt, double* dPara, int nAlgImg, int nRatio, CString strPath);
	long	SetFindContour_APP(cv::Mat matSrcBuf, INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRoundROICnt, double* dPara, int nAlgImg, int nRatio, CString strPath, Point* ptAlignCorner, CStringA strImageName, double dAlignTH, bool bImageSave = false);

	// Round / Camera Hole ���� & ���� ���� ( 8bit )
	long	SetFindContour_(cv::Mat matSrcBuf, INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], INSP_AREA CHoleROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRoundROICnt, int nCHoleROICnt, double* dPara, int nAlgImg, int nRatio, CString strPath);

	// Round ���� & ���� ����
	void	SetFindRound(cv::Mat& matTempBuf, vector< vector< cv::Point2i > > contours, cv::Point ptCorner[E_CORNER_END], INSP_AREA RoundROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nRoundROICnt, int nContourIdx, int nAlgImg, int nRatio, CString strPath);

	// CHole ���� & ���� ����
	void	SetFindCHole(cv::Mat& matTempBuf, vector< vector< cv::Point2i > > contours, cv::Point ptCorner[E_CORNER_END], INSP_AREA CHoleROI[MAX_MEM_SIZE_E_INSPECT_AREA], int nCHoleROICnt, int nContourIdx, int nAlgImg, int nRatio, CString strPath);

	// �ܰ� �κ� ó�� ( 8bit & 12bit )
	long	DoFillOutArea(cv::Mat& matSrcBuf, cv::Mat& matBKGBuf, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], ROUND_SET tCHoleSet[MAX_MEM_SIZE_E_INSPECT_AREA], cv::Mat* matCHoleROIBuf, cv::Rect* rcCHoleROI, bool* bCHoleAD,
		double* dPara, int nAlgImg, int nRatio, wchar_t* strID=NULL);

	// SVI �ܰ� �κ� ó�� ( Color )
	long	DoFillOutArea_SVI(cv::Mat& matSrcBuf, cv::Mat& matBKGBuf, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA],
		double* dPara, int nAlgImg, int nCameraNum, int nRatio, wchar_t* strID=NULL, cv::Point* ptCorner=NULL);

	// APP �ܰ� �κ� ó�� ( 8bit )
	long	DoFillOutArea_APP(cv::Mat& matSrcBuf, cv::Mat& matBKGBuf, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA],
		double* dPara, int nAlgImg, int nCameraNum, int nRatio, wchar_t* strID=NULL, cv::Point* ptCorner=NULL, vector<vector<Point2i>> &ptActive = vector<vector<Point2i>>(), double dAlignTheta=0, CString strPath=NULL, bool bImageSave=false);

	// Dust Pattern �ܰ� �κ� ó�� ( 8bit & 12bit )
	long	DoFillOutAreaDust(cv::Mat& matSrcBuf, cv::Point ptResCorner[E_CORNER_END], double dAngle, cv::Rect* rcCHoleROI, ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], ROUND_SET tCHoleSet[MAX_MEM_SIZE_E_INSPECT_AREA],
		double* dPara, int nAlgImg, int nRatio, wchar_t* strID=NULL);

	// ���� ȸ��
	long	DoRotateImage(cv::Mat ptSrcBuffer, cv::Mat& ptDstBuffer, double dAngle);

	// ��ǥ ȸ��
	long	DoRotatePoint(cv::Point matSrcPoint, cv::Point& matDstPoint, cv::Point ptCenter, double dAngle);

	// AVI AD �˻� ( 8bit & 12bit )
	long	DoFindDefectAD(cv::Mat matSrcBuf, double* dPara, double* dResult, int nRatio);

	// SVI AD �˻� ( Color )
	long	DoFindDefectAD_SVI(cv::Mat matSrcBuf, double* dPara, double* dResult, int nCameraNum, int nRatio);

	// APP AD �˻� ( 8bit )
	long	DoFindDefectAD_APP(cv::Mat MatOrgImage, double* dAlgPara, double* dResult, int nRatio);
	long	Check_Abnormal_PADEdge(cv::Mat MatOrgImage, int nThreshold, double dCompare_Theta, Rect rtObject);

	// AVI AD GV �˻� ( 8bit & 12bit )
	long	DoFindDefectAD_GV(cv::Mat& matSrcBuf, double* dPara, double* dResult, cv::Point* ptCorner, CDefectCCD *cCCD=NULL);

	// Dust 4���� GV �˻�
	long	DoFindDefectAD_GV_DUST(cv::Mat& matSrcBuf, double* dPara, double* dResult, cv::Point* ptCorner);

	// SVI AD GV �˻� ( Color )
	long	DoFindDefectAD_GV_SVI(cv::Mat& matSrcBuf, double* dPara, double* dResult, cv::Point* ptCorner);

	// �� ����
	long CurlJudge(cv::Mat matSrcBuf, double* dpara, cv::Point* ptCorner, BOOL& bCurl, stMeasureInfo* stCurlMeasure, BOOL bSaveImage, CString strPath);

	void MakeLabelMask(cv::Mat& matSrc, cv::Mat& matDstActive);

	void MakeActiveMask(cv::Mat& matSrc, cv::Mat& matDstActive, int nThreshActive);

	void Mask_SizeUp(cv::Mat matSrc, int nUpSize);

	void Mask_SizeDown(cv::Mat matSrc, int nUpSize);


protected:

	// Cell ���� ã��
	long	FindCellArea(cv::Mat matThreshBuf, int nMinArea, cv::Rect& rectCell);

	// �ߺ� ������ ��
	bool	FindInSamples(vector <cv::Point2i>& ptSamples, cv::Point2i ptIndexs);

	// �����Ϳ��� �ߺ����� �ʰ� N���� ������ ������ ���
	long	GetRandomSamples(vector <cv::Point2i>& ptSrcIndexs, vector <cv::Point2i>& ptSamples, int nSampleCount);

	// ���� ���� ���
	long	calcLineFit(vector <cv::Point2i>& ptSamples, long double& dA, long double& dB);

	// ��ȿ�� ������ ���� �ľ�
	long	calcLineVerification(vector <cv::Point2i>& ptSrcIndexs, vector <cv::Point2i>& ptInliers, long double& dA, long double& dB, double distThreshold);

	// RANSAC �˰���
	long	calcRANSAC(vector <cv::Point2i>& ptSrcIndexs, long double& dA, long double& dB, int nMinSamples, double distThreshold);

	// ���⿡ ���� ������ ȹ��
	long	RobustFitLine(cv::Mat& matTempBuf, cv::Rect rectCell, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType, int nSamp=50);

	// Profile�� ����� ��ü�� �ν��Ͽ� ��ǥ Ž��
	long	ObjectOutAreaGetLine(cv::Mat& matTempBuf, cv::Rect rectCell, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType, int nThreshold_Theta, float fAvgOffset);

	// ����ȭ �ȿ��󿡼� ��ü�� ���ʿ��� �ٱ����� ��ǥŽ��
	long	ObjectInAreaGetLine(cv::Mat& matTempBuf, cv::Rect rectImgSize, long double& dA, long double& dB, int nMinSamples, double distThreshold, int nType);

	// ���� ū Blob ��ȯ APP
	long	FindBiggestBlob_APP(cv::Mat & src, cv::Mat & dst);

	// 4���� ������ ��ġ ã��
	long	FindCornerPoint(cv::Point2f ptSrc[E_CORNER_END], cv::Point ptDst[E_CORNER_END], long nWidth, long nHeight);

	// ���� ���� �ܰ��κ� �����
	long	FindEdgeArea(cv::Mat matSrcBuf, cv::Mat& matDstBuf, int nLength, CMatBuf* cMemSub=NULL);

	// ���� ���� �ܰ��κ� ����� ( Color )
	long	FindEdgeArea_SVI(cv::Mat matSrcBuf, cv::Mat& matDstBuf, int nLength);

	// ���� ���� ��� ä���
	long	FillAreaMeanX(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV);

	// ���� ���� ��� ä��� ( Color )
	long	FillAreaMeanX_SVI(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV);

	// ���� ���� ��� ä���
	long	FillAreaMeanY(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV);

	// ���� ���� ��� ä��� ( Color )
	long	FillAreaMeanY_SVI(cv::Mat& matMeanBuf, cv::Mat& matEdgeBuf, CRect rectROI, int nSegX, int nSegY, int nMinGV);

	// ����� �κи� �����
	long	FillMerge(cv::Mat& matSrcBuf, cv::Mat matMeanBuf, cv::Mat matMaskBuf, int nAlgImg , CMatBuf* cMemSub=NULL ); //choikwangil 04.06 b11 ����

	// ����� �κи� ����� ( Color )
	long	FillMerge_SVI(cv::Mat& matSrcBuf, cv::Mat matMeanBuf, cv::Mat matMaskBuf);

	// � �κ�, ����� �������� ��¦ �ֱ� ( ��Ȳ�� ���� �ٸ� )
	cv::Point	calcRoundIn(ROUND_SET tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA], int nIndex, int nRoundIn);

	// � �̿� ���� ����
	long	calcLineConnect(cv::Mat& matSrcBuf, cv::Point ptSE[2], cv::Point ptPoly[2], int& nSE, int nSetArea);

	// �ڳ� �κ� ���� ä��� ( Color )
	long	FillCorner(cv::Mat& matSrcROIBuf, cv::Mat& matMaskROIBuf, int nType);

	// �̹��� Shift ����
	long	ShiftCopy(cv::Mat& matSrcImage, cv::Mat& matDstImage, int nShiftX, int nShiftY, int nShiftLoopX = 1, int nShiftLoopY = 1 );
	long	ShiftCopy16Bit(cv::Mat& matSrcImage, cv::Mat& matDstImage, int nShiftX, int nShiftY, int nShiftLoopX = 1, int nShiftLoopY = 1 );
	long	ShiftCopyParaCheck(int ShiftValue, int& nCpyX, int& nCpyY, int& nLoopX, int& nLoopY);

	//���������� ���������� �ϱ����� Cell�� Edge�� ã�� - �ӵ� ����� ���Ͽ�
	long	CInspectAlign::FindCellEdge_For_Morphology(cv::Mat matSrc, int nThreshold, cv::Rect &rcFindCellROI); // �뷫���� Cell ��ġ�� ã�´�.
	long	CInspectAlign::MakeRoI_For_Morphology(cv::Rect rcFindCellROI, 
												  int nExtROI_Outer, int nExtROI_Inner_L, int nExtROI_Inner_R, int nExtROI_Inner_T, int nExtROI_Inner_B,
												  cv::Size rcLimit, cv::Rect  *prcMorpROI); //���������� �� ���� �����

	//�κ� ���������� �ϴ� ���
	long	CInspectAlign::Partial_Morphology(cv::Mat matSrc, cv::Mat matDst, int nMorpType, cv::Mat StructElem, cv::Rect  *prcMorpROI);

	//�κ� ���ö�þ�
	long	CInspectAlign::Partial_Laplacian(cv::Mat matSrc, cv::Mat matDst, cv::Rect  *prcMorpROI);

	//�ڽ��� ������ �����ϴ� ���
	void RecalRect(cv::Rect &rcRect, cv::Size szLimit);

	// ���� �̸� ��������
	CString		GetPatternString(int nPattern);
	CStringA	GetPatternStringA(int nPattern);

	//BM LT, RT �޾ƿ���
	long GetBMCorner(cv::Mat Src, double* dAlgPara, Point* ptPanelCorner, cv::Rect& rtBMCorner);
};