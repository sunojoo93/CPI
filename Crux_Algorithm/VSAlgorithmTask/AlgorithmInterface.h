/*****************************************************************************
  File Name		: InspectAlgorithmInterface.h
  Version		: ver 1.0
  Create Date	: 2015.03.12
  Author		: KYT
  Description	: �˻� �˰��� �������̽�
  Abbreviations	: 
 *****************************************************************************/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// stDefectInfo ����ü ����ϱ� ���� Include ��.
//#include "AviDefineInspect.h"
#include "DefineInterface.h"
#include "InspResultInfo.h"

#include <concurrent_queue.h>
#include "InspThrd.h"

// �˻� �˰��� ������ ��� �������� �ִ� ���� ������
// KSW 18.02.05 - SVI �ҷ� �߻� �� ���� 20000 -> 1000
#define	MAX_DEFECT_COUNT		2000

// Logic���� ����ϴ� �Ķ���� ����ü
struct tLogicPara
{
	cv::Mat				MatOrgImage;		// �ش� ���� ����
//	cv::Mat				MatOrgRGB[3];		// R, G, B ����
	cv::Mat				MatBKG;				// Mura���� ��� ( ������ ���� )
	cv::Mat				*MatOrgRGBAdd[3];		// 
	cv::Mat				MatDust;			// Dust ����
	int					nThreadLog;
	int					nCameraNum;
	CString				strPanelID;
	TCHAR				tszPanelID[MAX_PATH];

	// ����ü �ʱ�ȭ
	tLogicPara():
	nThreadLog(0),	nCameraNum(0)
	{	strPanelID.Format(_T(""));		memset(tszPanelID, 0, MAX_PATH * sizeof(TCHAR));	}
};

//----------------------------------------------------------------------------------------------------
//	 Class ��	:	ResultBlob_Total
//	 Class���	:	�� ��� ������ ����Ʈ
//   �ֿ���	:
//	 �� �� ��	:	2015/03/12
//	 �� �� ��	:	KYT
//	 �������	:	V.1.0 �ʱ� �ۼ�
//	 ��    ��	:	
//----------------------------------------------------------------------------------------------------
class ResultBlob_Total
{
public:
	ResultBlob_Total()
	{
		InitializeCriticalSection(&m_cs);
	}

	~ResultBlob_Total()
	{
		stDefectInfo* pTempResultBlob = NULL;
		DeleteCriticalSection(&m_cs);
		for(POSITION pos = m_ListDefectInfo.GetHeadPosition(); pos != NULL;)
		{
			pTempResultBlob = m_ListDefectInfo.GetNext(pos);

			if (pTempResultBlob != NULL)
			{
				delete pTempResultBlob;
				pTempResultBlob = NULL;
			}
		}

		m_ListDefectInfo.RemoveAll();
	}

	stDefectInfo* GetNext_ResultBlob(POSITION &ps)
	{
		EnterCriticalSection(&m_cs);
		stDefectInfo* rb = m_ListDefectInfo.GetNext(ps);
		LeaveCriticalSection(&m_cs);

		return rb;
	}

	void RemoveAll_ResultBlob()
	{
		stDefectInfo* pTempResultBlob = NULL;
	
		for(POSITION pos = m_ListDefectInfo.GetHeadPosition(); pos != NULL;)
		{
			pTempResultBlob = m_ListDefectInfo.GetNext(pos);

			if (pTempResultBlob != NULL)
			{
				delete pTempResultBlob;
				pTempResultBlob = NULL;
			}
		}

		m_ListDefectInfo.RemoveAll();
	}

	POSITION GetHeadPosition_ResultBlob()
	{
		EnterCriticalSection(&m_cs);
		POSITION ps = m_ListDefectInfo.GetHeadPosition();
		LeaveCriticalSection(&m_cs);

		return ps;
	}

	
	POSITION AddTail_ResultBlobAndAddOffset(stDefectInfo* pResultBlob, cv::Point* ptCorner)
	{
		// AVI�� ��� ���� ���� �״�� ����ϹǷ�, Corner ���� ���ص� �� : NULL
		//if( ptCorner )
		//{
		//	for(int nDefectCnt = 0; nDefectCnt < pResultBlob->nDefectNumber; nDefectCnt++)
		//	{
		//		pResultBlob->Box_X_Max[nDefectCnt] += rectTeachingROI.left;
		//		pResultBlob->Box_X_Min[nDefectCnt] += rectTeachingROI.left;
		//		pResultBlob->Box_Y_Max[nDefectCnt] += rectTeachingROI.top;
		//		pResultBlob->Box_Y_Min[nDefectCnt] += rectTeachingROI.top;
		//	}
		//}		

		EnterCriticalSection(&m_cs);
		POSITION ps = m_ListDefectInfo.AddTail(pResultBlob);
		LeaveCriticalSection(&m_cs);

		return ps;
	}

	void ResultBlobAddOffset(stDefectInfo* pResultBlob, CRect& rectTeachingROI)
	{
// ���� Offset ��� ����
// 		for(int nDefectCnt = 0; nDefectCnt < pResultBlob->nDefectNumber; nDefectCnt++)
// 		{
// 			pResultBlob->Box_X_Max[nDefectCnt] += rectTeachingROI.left;
// 			pResultBlob->Box_X_Min[nDefectCnt] += rectTeachingROI.left;
// 			pResultBlob->Box_Y_Max[nDefectCnt] += rectTeachingROI.top;
// 			pResultBlob->Box_Y_Min[nDefectCnt] += rectTeachingROI.top;
// 		}
		return ;
	}


	POSITION AddTail_ResultBlob(stDefectInfo* pResultBlob)
	{
		EnterCriticalSection(&m_cs);
		POSITION ps = m_ListDefectInfo.AddTail(pResultBlob);
		LeaveCriticalSection(&m_cs);
		
		return ps;
	}

	// 2015.06.17 NG Flag ���� - By CWH
	BOOL IsEmpty_ResultBlob()	
	{
		BOOL bRet = TRUE;

		EnterCriticalSection(&m_cs);

		stDefectInfo* pTempResultBlob = NULL;

		for(POSITION pos = m_ListDefectInfo.GetHeadPosition(); pos != NULL;)
		{
			pTempResultBlob = m_ListDefectInfo.GetNext(pos);

			if (pTempResultBlob->nDefectCount != 0)
			{
				bRet = FALSE;
				break;
			}
		}

		LeaveCriticalSection(&m_cs);
		return bRet;
	}

	void	SetPanelID(CString strPanelID)		{ m_strPanelID = strPanelID; }
	CString GetPanelID()						{ return m_strPanelID; }
	void	SetModelID(CString strModelID)		{ m_strModelID = strModelID; }
	CString GetModelID()						{ return m_strModelID; }
private:
	CList<stDefectInfo*, stDefectInfo*>	m_ListDefectInfo;
	CRITICAL_SECTION	m_cs;
	CString				m_strPanelID;
	CString				m_strModelID;
};


//----------------------------------------------------------------------------------------------------
//	 Class ��	:	InspectAlgorithmInterface
//	 Class���	:	�˻� �˰��� �������̽�
//   �ֿ���	:
//	 �� �� ��	:	2015/03/12
//	 �� �� ��	:	KYT
//	 �������	:	V.1.0 �ʱ� �ۼ�
//	 ��    ��	:	
//----------------------------------------------------------------------------------------------------
class InspectAlgorithmInterface
{
public:
	InspectAlgorithmInterface();
	~InspectAlgorithmInterface(void);

protected:

	// �˰��� ��, Defect ��� ���� ����
	void			BlobFeatureSave(stDefectInfo *resultBlob, CString strPath, int* nImageDefectCount = NULL);	

	// �ϳ��� Cell �˻� ����
	long			StartLogic(CString strPanelID, CString strDrive, CString strLcp_Bubble_Path ,tAlignInfo stThrdAlignInfo,
							cv::Mat MatOriginImage[][MAX_CAMERA_COUNT], cv::Mat& MatColorBuffer, cv::Mat& MatDrawBuffer, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
							ResultBlob_Total* pResultBlob_Total, const int nImageNum, const int nCameraNum, const int nThreadIndex, bool bpInspectEnd[][MAX_CAMERA_COUNT],bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER] ,int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtReusltInfo, const double* _mtp);

	void			DrawAdjustROI(cv::Mat& MatDrawBuffer, cv::Point* pPtCorner, LPCTSTR strROIName, int nCurCount, int nDrawMode);	// 2015.09.16 ROI �̸� ���� �߰�
		
	// �ڵ� ROI ��� �� Align Image
	long			AcquireAutoRoiAndAlignImage(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, int nRatio, int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult,double dCamResolution, double dPannelSizeX,double dPannelSizeY);

	long			PanelCurlJudge(cv::Mat& matSrcBuf, double* dPara, tAlignInfo* stCamAlignInfo, ResultBlob_Total* pResultBlobTotal, int nImageNum, stMeasureInfo* stCurlMeasure,CString strPath);

	long			AcquireAutoRoiAndAlignImage_LCP(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, cv::Mat MatDrawImage, int nRatio,int  nAlignImageClassify,int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult, double dCamResolution,int nColorCode ,double dPannelSizeX, double dPannelSizeY);

	long			AcquireAutoRoiAndAlignImage_LCP_Mask(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, int nRatio, int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult, double dCamResolution, double dPannelSizeX, double dPannelSizeY,int nColorCode);
	
	long			AcquireAutoBackRoiAndAlignImage_LCP(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, cv::Mat MatDrawImage, int nRatio, int  nAlignImageClassify, int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult, double dCamResolution, int nColorCode,double dPannelSizeX, double dPannelSizeY);
	
	long			AcquireAutoVaccumRoiAndAlignImage_LCP(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, cv::Mat MatDrawImage, int nRatio, int  nAlignImageClassify, int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult, double dCamResolution, int nColorCode, double dPannelSizeX, double dPannelSizeY);

	long			Empty_Stage_Dust_Judge(cv::Mat matSrc_Stage, cv::Mat matVaccumMask, CString strPanelID, CString strDrive, double& dStage_Mean_Gv,double* dAlignPara);
	////////////////// �����Լ�
	// �˰��� �˻� ����
	virtual long	StartLogicAlgorithm(const CString strDrive, const CString strLcp_Bubble_Path, const tLogicPara& LogicPara, cv::Mat& MatColorBuffer,
		cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat& MatDrawBuffer,
		const int nImageNum, const int nROINumber, const int nAlgorithmNumber, 
		tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, 
		bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp = 0) = 0;

	// AVI : Align ���� ���� - ȸ���� ������ �����ϰ� ��ǥ�� ����
	// SVI : Warping ��ǥ�� ����
	// AVI : Align ���� ���� - ȸ���� ������ �����ϰ� ��ǥ ���� - �� �� Rotate
	virtual bool	AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner) = 0;

	// ���� �̹��� ����
	// AVI : �ƹ��͵� ���� / SVI : Warping / APP : Rotate
	virtual bool	AdjustOriginImage(cv::Mat& MatOriginImage, cv::Mat& MatDrawImage, tAlignInfo* pStAlignInfo)		{	return false	;};

	// �ܰ� �κ� ó��
	virtual long	makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio) = 0;

	// Align �ϱ� �� AD �˻�
	virtual long	CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio)	{	return E_ERROR_CODE_FALSE	;};

	// ROI GV �˻�
	virtual long	CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult, 
								bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode)	{	return E_ERROR_CODE_TRUE		;};
};