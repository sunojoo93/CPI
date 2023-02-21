
/************************************************************************/
// Blob ���� �ҽ�
// ������ : 17.03.08 - KSW
/************************************************************************/

#include "StdAfx.h"

#include "FeatureExtraction.h"

// ���� ó��
#include <ppl.h>
using namespace Concurrency;

//////////////////////////////////////////////////////////////////////////

CFeatureExtraction::CFeatureExtraction(void)
{
	m_bComplete = false;

	// ���� �ʱ�ȭ
	vector<tBLOB_FEATURE>().swap(m_BlobResult);

	InitializeCriticalSectionAndSpinCount(&m_csCoordFile,4000);

	cMem				= NULL;
	m_cInspectLibLog	= NULL;
	m_strAlgLog			= NULL;
	m_tInitTime			= 0;
	m_tBeforeTime		= 0;

	// ���� �ð� �Ѿ��, Ŭ���� ���� ( 40�� )
	cTimeOut.SetTimeOut(40000);
}

CFeatureExtraction::~CFeatureExtraction(void)
{
	DeleteCriticalSection(&m_csCoordFile);

	Release();
}

bool CFeatureExtraction::Release()
{
	m_bComplete = false;

	// ���� �ʱ�ȭ
	if( m_BlobResult.size() != 0 )
	{
		for (int i=0 ; i<m_BlobResult.size() ; i++)
		{
			vector<cv::Point>().swap(m_BlobResult[i].ptIndexs);
			vector <cv::Point>().swap(m_BlobResult[i].ptContours);
		}
		vector<tBLOB_FEATURE>().swap(m_BlobResult);
	}

	return true;
}

bool CFeatureExtraction::DoBlobCalculate(cv::Mat ThresholdBuffer, cv::Mat GrayBuffer, int nMaxDefectCount)
{
	// Blob �Ϸ� �ߴ��� Ȯ�ο�.
	m_bComplete = false;

	// �޸� ����
	Release();

	// ���� �ʱ�ȭ
	if( m_BlobResult.size() != 0 )
	{
		for (int i=0 ; i<m_BlobResult.size() ; i++)
		{
			vector<cv::Point>().swap(m_BlobResult[i].ptIndexs);
			vector <cv::Point>().swap(m_BlobResult[i].ptContours);
		}
		vector<tBLOB_FEATURE>().swap(m_BlobResult);
	}

	// ������ ���� ��� ����
	if( ThresholdBuffer.empty() )			return false;

	// 1ä���� �ƴ� ���
	if( ThresholdBuffer.channels() != 1 )	return false;

	// Gray ������ ����X  &  1ä�� �ƴ� ���
	bool bGrayEmpty = false;
	if( GrayBuffer.empty() || GrayBuffer.channels() != 1)
	{
		GrayBuffer = ThresholdBuffer.clone();
		bGrayEmpty = true;
	}

	// ���� ��ȯ
	//cv::Mat LabelBuffer;
	//ThresholdBuffer.convertTo(LabelBuffer, CV_32SC1);

	// Label ��� (8����)
	//int nTotalLabel = cv::connectedComponents(ThresholdBuffer, LabelBuffer) - 1;

	writeInspectLog(__FUNCTION__, _T("Start."));

	cv::Mat matLabel, matStats, matCentroid;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);
	matLabel = cMatBufTemp.GetMat(ThresholdBuffer.size(), CV_32SC1);

	writeInspectLog(__FUNCTION__, _T("Mat Create."));

	__int64 nTotalLabel = 0;
	
	if( ThresholdBuffer.type() == CV_8U )
	{
		nTotalLabel = cv::connectedComponentsWithStats(ThresholdBuffer, matLabel, matStats, matCentroid, 8, CV_32S,CCL_GRANA) - 1;
	}
	else
	{
		cv::Mat matSrc8bit = cMatBufTemp.GetMat(ThresholdBuffer.size(), CV_8UC1);
		ThresholdBuffer.convertTo(matSrc8bit, CV_8UC1, 1./16.);

		nTotalLabel = cv::connectedComponentsWithStats(matSrc8bit, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

		matSrc8bit.release();
	}
	
	writeInspectLog(__FUNCTION__, _T("connectedComponents."));

	// ����ó��
	if( nTotalLabel < 0 )
	{
		// �޸� ����
		if( bGrayEmpty )			GrayBuffer.release();
		if( !matLabel.empty() )		matLabel.release();
		if( !matStats.empty() )		matStats.release();
		if( !matCentroid.empty() )	matCentroid.release();

		return false;
	}

	// ����ó��
	if (nTotalLabel >= nMaxDefectCount)
		nTotalLabel = nMaxDefectCount-1;

	// �⺻ Feature ��� 8bit
	if( GrayBuffer.type() == CV_8UC1 )
		DoFeatureBasic_8bit(matLabel, matStats, matCentroid, GrayBuffer, (int)nTotalLabel, &cMatBufTemp);

	// �⺻ Feature ��� 16bit
	else
		DoFeatureBasic_16bit(matLabel, matStats, matCentroid, GrayBuffer, (int)nTotalLabel, &cMatBufTemp);

	writeInspectLog(__FUNCTION__, _T("FeatureBasic."));

	// �޸� ����
	if( bGrayEmpty )			GrayBuffer.release();
	if( !matLabel.empty() )		matLabel.release();
	if( !matStats.empty() )		matStats.release();
	if( !matCentroid.empty() )	matCentroid.release();

	writeInspectLog(__FUNCTION__, _T("Release."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}
	// Blob �Ϸ�
	m_bComplete = true;

	return true;
}
bool CFeatureExtraction::DoBlobCalculate(cv::Mat ThresholdBuffer, CRect rectROI, cv::Mat GrayBuffer, int nMaxDefectCount)
{
	// Blob �Ϸ� �ߴ��� Ȯ�ο�.
	m_bComplete = false;

	// �޸� ����
	Release();

	// ���� �ʱ�ȭ
	if (m_BlobResult.size() != 0)
	{
		for (int i = 0; i < m_BlobResult.size(); i++)
		{
			vector<cv::Point>().swap(m_BlobResult[i].ptIndexs);
			vector <cv::Point>().swap(m_BlobResult[i].ptContours);
		}
		vector<tBLOB_FEATURE>().swap(m_BlobResult);
	}

	// ������ ���� ��� ����
	if (ThresholdBuffer.empty())			return false;

	// 1ä���� �ƴ� ���
	if (ThresholdBuffer.channels() != 1)	return false;

	// Gray ������ ����X  &  1ä�� �ƴ� ���
	bool bGrayEmpty = false;
	if (GrayBuffer.empty() || GrayBuffer.channels() != 1)
	{
		GrayBuffer = ThresholdBuffer.clone();
		bGrayEmpty = true;
	}

	writeInspectLog(__FUNCTION__, _T("Start."));

	cv::Mat matLabel, matStats, matCentroid;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);
	matLabel = cMatBufTemp.GetMat(ThresholdBuffer.size(), CV_32SC1, false);

	writeInspectLog(__FUNCTION__, _T("Mat Create."));

	__int64 nTotalLabel = 0;

	if (ThresholdBuffer.type() == CV_8U)
	{
		nTotalLabel = cv::connectedComponentsWithStats(ThresholdBuffer, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;
	}
	else
	{
		cv::Mat matSrc8bit = cMatBufTemp.GetMat(ThresholdBuffer.size(), CV_8UC1);
		ThresholdBuffer.convertTo(matSrc8bit, CV_8UC1, 1. / 16.);

		nTotalLabel = cv::connectedComponentsWithStats(matSrc8bit, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

		matSrc8bit.release();
	}

	writeInspectLog(__FUNCTION__, _T("connectedComponents."));

	// ����ó��
	if (nTotalLabel < 0)
	{
		// �޸� ����
		if (bGrayEmpty)				GrayBuffer.release();
		if (!matLabel.empty())		matLabel.release();
		if (!matStats.empty())		matStats.release();
		if (!matCentroid.empty())	matCentroid.release();

		return false;
	}

	// ����ó��
	if (nTotalLabel >= nMaxDefectCount)
		nTotalLabel = nMaxDefectCount - 1;

	// �⺻ Feature ��� 8bit
	if (GrayBuffer.type() == CV_8UC1)
		DoFeatureBasic_8bit(matLabel, matStats, matCentroid, GrayBuffer, (int)nTotalLabel, &cMatBufTemp);

	// �⺻ Feature ��� 16bit
	else
		DoFeatureBasic_16bit(matLabel, matStats, matCentroid, GrayBuffer, (int)nTotalLabel, &cMatBufTemp);

	writeInspectLog(__FUNCTION__, _T("FeatureBasic."));

	CoordApply(rectROI, nTotalLabel);

	// �޸� ����
	if (bGrayEmpty)			GrayBuffer.release();
	if (!matLabel.empty())		matLabel.release();
	if (!matStats.empty())		matStats.release();
	if (!matCentroid.empty())	matCentroid.release();

	writeInspectLog(__FUNCTION__, _T("Release."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}

	// Blob �Ϸ�
	m_bComplete = true;

	return true;
}

bool CFeatureExtraction::DoBlobCalculate_LCP(cv::Mat ThresholdBuffer, CRect rectROI, double* dPara, cv::Mat GrayBuffer, cv::Mat ColorBuffer,  int nMaxDefectCount)
{
	// Blob �Ϸ� �ߴ��� Ȯ�ο�.
	m_bComplete = false;

	// �޸� ����
	Release();

	// ���� �ʱ�ȭ
	if (m_BlobResult.size() != 0)
	{
		for (int i = 0; i < m_BlobResult.size(); i++)
		{
			vector<cv::Point>().swap(m_BlobResult[i].ptIndexs);
			vector <cv::Point>().swap(m_BlobResult[i].ptContours);
		}
		vector<tBLOB_FEATURE>().swap(m_BlobResult);
	}

	// ������ ���� ��� ����
	if (ThresholdBuffer.empty())			return false;

	// 1ä���� �ƴ� ���
	if (ThresholdBuffer.channels() != 1)	return false;

	// Gray ������ ����X  &  1ä�� �ƴ� ���
	bool bGrayEmpty = false;
	if (GrayBuffer.empty() || GrayBuffer.channels() != 1)
	{
		GrayBuffer = ThresholdBuffer.clone();
		bGrayEmpty = true;
	}

	writeInspectLog(__FUNCTION__, _T("Start."));

	cv::Mat matLabel, matStats, matCentroid;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMem);
	matLabel = cMatBufTemp.GetMat(ThresholdBuffer.size(), CV_32SC1, false);

	writeInspectLog(__FUNCTION__, _T("Mat Create."));

	__int64 nTotalLabel = 0;

	if (ThresholdBuffer.type() == CV_8U)
	{
		nTotalLabel = cv::connectedComponentsWithStats(ThresholdBuffer, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;
	}
	else
	{
		cv::Mat matSrc8bit = cMatBufTemp.GetMat(ThresholdBuffer.size(), CV_8UC1);
		ThresholdBuffer.convertTo(matSrc8bit, CV_8UC1, 1. / 16.);

		nTotalLabel = cv::connectedComponentsWithStats(matSrc8bit, matLabel, matStats, matCentroid, 8, CV_32S, CCL_GRANA) - 1;

		matSrc8bit.release();
	}

	writeInspectLog(__FUNCTION__, _T("connectedComponents."));

	// ����ó��
	if (nTotalLabel < 0)
	{
		// �޸� ����
		if (bGrayEmpty)				GrayBuffer.release();
		if (!matLabel.empty())		matLabel.release();
		if (!matStats.empty())		matStats.release();
		if (!matCentroid.empty())	matCentroid.release();

		return false;
	}

	// ����ó��
	if (nTotalLabel >= nMaxDefectCount)
		nTotalLabel = nMaxDefectCount - 1;

	// �⺻ Feature ��� 8bit
	if (GrayBuffer.type() == CV_8UC1)
		DoFeatureBasic_8bit_LCP(matLabel, matStats, matCentroid, GrayBuffer, ColorBuffer,dPara ,(int)nTotalLabel, &cMatBufTemp);

	// �⺻ Feature ��� 16bit
	else
		DoFeatureBasic_16bit(matLabel, matStats, matCentroid, GrayBuffer, (int)nTotalLabel, &cMatBufTemp);

	writeInspectLog(__FUNCTION__, _T("FeatureBasic."));

	CoordApply(rectROI, nTotalLabel);

	// �޸� ����
	if (bGrayEmpty)			GrayBuffer.release();
	if (!matLabel.empty())		matLabel.release();
	if (!matStats.empty())		matStats.release();
	if (!matCentroid.empty())	matCentroid.release();

	writeInspectLog(__FUNCTION__, _T("Release."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}

	// Blob �Ϸ�
	m_bComplete = true;

	return true;
}

// ��ǥ ����
void	CFeatureExtraction::CoordApply(CRect rectROI, int nTotalLabel)
{
	for (int nBlobNum = 0; nBlobNum < nTotalLabel; nBlobNum++)
	{
		m_BlobResult.at(nBlobNum).rectBox.x += rectROI.left;
		m_BlobResult.at(nBlobNum).rectBox.y += rectROI.top;

		m_BlobResult.at(nBlobNum).ptCenter.x += rectROI.left;
		m_BlobResult.at(nBlobNum).ptCenter.y += rectROI.top;

		for (int idx = 0; idx < m_BlobResult.at(nBlobNum).ptIndexs.size(); idx++)
		{
			m_BlobResult.at(nBlobNum).ptIndexs[idx].x += rectROI.left;
			m_BlobResult.at(nBlobNum).ptIndexs[idx].y += rectROI.top;
		}

		for (int idx = 0; idx < m_BlobResult.at(nBlobNum).ptContours.size(); idx++)
		{
			m_BlobResult.at(nBlobNum).ptContours[idx].x += rectROI.left;
			m_BlobResult.at(nBlobNum).ptContours[idx].y += rectROI.top;
		}
	}
}

// Blob & ���� ���
long CFeatureExtraction::DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType, bool bPtRotate)
{
	// �ð� ���� ����
	cTimeOut.TimeCheckStart();

	// �޸� ����
	Release();

	// �Ķ���� NULL�� ���.
	if (nCommonPara == NULL)						return E_ERROR_CODE_EMPTY_PARA;
	if (pResultBlob == NULL)						return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)				return E_ERROR_CODE_EMPTY_PARA;

	if (nDefectType < 0)							return E_ERROR_CODE_EMPTY_PARA;
	if (nDefectType >= E_DEFECT_JUDGEMENT_COUNT)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if (matSrcImage.empty())						return E_ERROR_CODE_EMPTY_BUFFER;
	if (matThresholdImage.empty())					return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];						// 00 : �ҷ� �ִ� ����
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;	// 01 : �˰��� �߰���� Image Save 
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];						// 02 : ���� ���� ���� ī��Ʈ
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];						// 03 : ���� ���� ��ȣ
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];						// 04 : Cam Number
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];						// 05 : ROI Number
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];						// 06 : �˰��� ��ȣ
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];						// 07 : Thread ID
	bool	bDefectNum = (nCommonPara[E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG] > 0) ? true : false;	// 08 : Draw Defect Num ǥ��
	bool	bDrawDust = (nCommonPara[E_PARA_COMMON_DRAW_DUST_FLAG] > 0) ? true : false;	// 09 : Draw Dust ǥ��
	int		nPatternImageNum = nCommonPara[E_PARA_COMMON_UI_IMAGE_NUMBER];						// 10 : UI �� ���� ���� ���� ��ȣ
	float	fAngle = nCommonPara[E_PARA_COMMON_ROTATE_ANGLE] / 1000.f;				// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
	int		nCx = nCommonPara[E_PARA_COMMON_ROTATE_CENTER_X];						// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy = nCommonPara[E_PARA_COMMON_ROTATE_CENTER_Y];						// 13 : Cell ȸ���� �߽� y��ǥ
	int		nPS = nCommonPara[E_PARA_COMMON_PS_MODE];						// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

																			// �ҷ� �Է� �� ����
	int& nDefectCount = pResultBlob->nDefectCount;

	// �ִ� �ҷ� ���� ���� ��� ���� ������
	if (nDefectCount >= nMaxDefectCount)
		return E_ERROR_CODE_TRUE;

	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = -fAngle * PI / 180.;
	double	dSin = sin(dTheta);
	double	dCos = cos(dTheta);
	double	dSin_ = sin(-dTheta);
	double	dCos_ = cos(-dTheta);

	// �� ����
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);


	// ������ Defect ����Ʈ
	int nFork = nDefectType;

	// Defect ����Ʈ ������ ��츸...
	if (!EngineerDefectJudgment[nFork].bDefectItemUse)
		return E_ERROR_CODE_TRUE;

	// Blob ����
	for (int i = 0; i < m_BlobResult.size(); i++)
	{
		// �ҷ� ���� ���͸�
		if (!DoColorFilter(nFork, nDefectColor))
			continue;


		// ���� �׸�� ���� 2��
		int nFeatureCount = E_FEATURE_COUNT * 2;
		bool bFilter = true;
		bool bInit = false;

		for (int nForj = 0; nForj < nFeatureCount; nForj++)
		{

			// ���� �׸� ������ ��츸
			if (!EngineerDefectJudgment[nFork].Judgment[nForj].bUse)
				continue;

			// �ѹ��̶� ���� �� ���.
			bInit = true;

			// ������ ���͸� �����ϸ� true / �������� ������ false ��ȯ
			if (!DoFiltering(
				m_BlobResult[i],										// Blob ���
				nForj / 2,												// �� Feature
				EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// ������ ( <, >, ==, <=, >= )
				EngineerDefectJudgment[nFork].Judgment[nForj].dValue))	// ��
			{

				bFilter = false;
				break;
			}
		}



		// ������ ��� ������ ���� �ϴ� ���, ��� �Է�
		if (bInit && bFilter)
		{
			m_BlobResult[i].bFiltering = true;

			// ������ ����
			if (bPtRotate)
			{
				int nL, nT, nR, nB;

				// ȸ�� ��, ���� ��ǥ ���
				int X = (int)(dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx);
				int Y = (int)(dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy);

				// �ʱ� ����
				nL = nR = X;
				nT = nB = Y;

				// �ܰ��� ���� ��ŭ
				for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
				{
					// ȸ�� ��, ���� ��ǥ ���
					X = (int)(dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx);
					Y = (int)(dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy);

					// ����
					if (nL > X)	nL = X;
					if (nR < X)	nR = X;
					if (nT > Y)	nT = Y;
					if (nB < Y)	nB = Y;
				}

				cv::Point ptTemp;

				ptTemp.x = (int)(dCos_ * (nL - nCx) - dSin_ * (nT - nCy) + nCx);
				ptTemp.y = (int)(dSin_ * (nL - nCx) + dCos_ * (nT - nCy) + nCy);
				pResultBlob->ptLT[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptLT[nDefectCount].y = (LONG)ptTemp.y;

				ptTemp.x = (int)(dCos_ * (nR - nCx) - dSin_ * (nT - nCy) + nCx);
				ptTemp.y = (int)(dSin_ * (nR - nCx) + dCos_ * (nT - nCy) + nCy);
				pResultBlob->ptRT[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptRT[nDefectCount].y = (LONG)ptTemp.y;

				ptTemp.x = (int)(dCos_ * (nR - nCx) - dSin_ * (nB - nCy) + nCx);
				ptTemp.y = (int)(dSin_ * (nR - nCx) + dCos_ * (nB - nCy) + nCy);
				pResultBlob->ptRB[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptRB[nDefectCount].y = (LONG)ptTemp.y;

				ptTemp.x = (int)(dCos_ * (nL - nCx) - dSin_ * (nB - nCy) + nCx);
				ptTemp.y = (int)(dSin_ * (nL - nCx) + dCos_ * (nB - nCy) + nCy);
				pResultBlob->ptLB[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptLB[nDefectCount].y = (LONG)ptTemp.y;
			}
			else
			{
				int nL, nT, nR, nB;
				int X = m_BlobResult[i].ptContours[0].x;
				int Y = m_BlobResult[i].ptContours[0].y;

				// �ʱ� ����
				nL = nR = X;
				nT = nB = Y;

				// �ܰ��� ���� ��ŭ
				for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
				{
					X = m_BlobResult[i].ptContours[k].x;
					Y = m_BlobResult[i].ptContours[k].y;

					// ����
					if (nL > X)	nL = X;
					if (nR < X)	nR = X;
					if (nT > Y)	nT = Y;
					if (nB < Y)	nB = Y;
				}

				pResultBlob->ptLT[nDefectCount].x = nL;
				pResultBlob->ptLT[nDefectCount].y = nT;

				pResultBlob->ptRT[nDefectCount].x = nR;
				pResultBlob->ptRT[nDefectCount].y = nT;

				pResultBlob->ptRB[nDefectCount].x = nR;
				pResultBlob->ptRB[nDefectCount].y = nB;

				pResultBlob->ptLB[nDefectCount].x = nL;
				pResultBlob->ptLB[nDefectCount].y = nB;
			}

			// UI�� �Ѱ��� ��� �ֱ�
			pResultBlob->nArea[nDefectCount] = m_BlobResult[i].nArea;
			pResultBlob->nMaxGV[nDefectCount] = m_BlobResult[i].nMaxGV;
			pResultBlob->nMinGV[nDefectCount] = m_BlobResult[i].nMinGV;
			pResultBlob->dMeanGV[nDefectCount] = m_BlobResult[i].fMeanGV;

			pResultBlob->nCenterx[nDefectCount] = m_BlobResult[i].ptCenter.x;
			pResultBlob->nCentery[nDefectCount] = m_BlobResult[i].ptCenter.y;

			pResultBlob->dBackGroundGV[nDefectCount] = m_BlobResult[i].fBKGV;

			pResultBlob->dCompactness[nDefectCount] = m_BlobResult[i].fCompactness;
			pResultBlob->dSigma[nDefectCount] = m_BlobResult[i].fStdDev;
			pResultBlob->dF_Min[nDefectCount] = m_BlobResult[i].fMajorAxis;
			pResultBlob->dF_Max[nDefectCount] = m_BlobResult[i].fMinorAxis;

			pResultBlob->dBreadth[nDefectCount] = m_BlobResult[i].fMajorAxis;
			pResultBlob->dF_Min[nDefectCount] = m_BlobResult[i].fMajorAxis;
			pResultBlob->dF_Max[nDefectCount] = m_BlobResult[i].fMinorAxis;	
			pResultBlob->dF_Elongation[nDefectCount] = m_BlobResult[i].fElongation;
			pResultBlob->dCompactness[nDefectCount] = m_BlobResult[i].fCompactness;
			pResultBlob->dRoundness[nDefectCount] = m_BlobResult[i].fRoundness;

			pResultBlob->nDefectColor[nDefectCount] = nDefectColor;
			pResultBlob->nDefectJudge[nDefectCount] = nFork;			// �ش� �ҷ�
			pResultBlob->nPatternClassify[nDefectCount] = nPatternImageNum;

#if USE_ALG_HIST
			// KSW 17.06.24 ��ü ������׷�
			memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
			// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
			calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif


			// �ҷ� ��ȣ �׸���
			if (!matDrawBuffer.empty() && bDefectNum)
			{
				cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x - 2, pResultBlob->ptRT[nDefectCount].y - 10, 30, 12), cv::Scalar(0, 0, 0), -1);

				char str[256] = { 0, };
				sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
				cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
				cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
			}

			// ������ �ҷ� ī��Ʈ ����
			nDefectCount++;
		}



		// �ִ� �ҷ� ���� ���� ��� ���� ������
		if (nDefectCount >= nMaxDefectCount)
			break;
	}

	return E_ERROR_CODE_TRUE;
}

// Blob & ���� ��� ROI
long CFeatureExtraction::DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType,  bool bPtRotate)
{
	// �ð� ���� ����
	cTimeOut.TimeCheckStart();

	// �޸� ����
	Release();

	// �Ķ���� NULL�� ���.
	if( nCommonPara == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)				return E_ERROR_CODE_EMPTY_PARA;

	if( nDefectType < 0	)							return E_ERROR_CODE_EMPTY_PARA;
	if( nDefectType >= E_DEFECT_JUDGEMENT_COUNT	)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if( matSrcImage.empty() )						return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )					return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount		= nCommonPara[	E_PARA_COMMON_MAX_DEFECT_COUNT			];						// 00 : �ҷ� �ִ� ����
	bool	bImageSave			= (nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_FLAG			] > 0) ? true : false;	// 01 : �˰��� �߰���� Image Save 
	int&	nSaveImageCount		= nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_COUNT			];						// 02 : ���� ���� ���� ī��Ʈ
	int		nImageNum			= nCommonPara[	E_PARA_COMMON_ALG_IMAGE_NUMBER			];						// 03 : ���� ���� ��ȣ
	int		nCamNum				= nCommonPara[	E_PARA_COMMON_CAMERA_NUMBER				];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[	E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[	E_PARA_COMMON_ALG_NUMBER				];						// 06 : �˰��� ��ȣ
	int		nThrdIndex			= nCommonPara[	E_PARA_COMMON_THREAD_ID					];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG		] > 0) ? true : false;	// 08 : Draw Defect Num ǥ��
	bool	bDrawDust			= (nCommonPara[	E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust ǥ��
	int		nPatternImageNum	= nCommonPara[	E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI �� ���� ���� ���� ��ȣ
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell ȸ���� �߽� y��ǥ
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
																
	// �ҷ� �Է� �� ����
	int& nDefectCount = pResultBlob->nDefectCount;

	// �ִ� �ҷ� ���� ���� ��� ���� ������
	if( nDefectCount >= nMaxDefectCount )
		return E_ERROR_CODE_TRUE;

	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );
	double	dSin_	= sin( -dTheta );
	double	dCos_	= cos( -dTheta );

	// �� ����
	DoBlobCalculate(matThresholdImage, rectROI, matSrcImage, nMaxDefectCount);


	// ������ Defect ����Ʈ
	int nFork = nDefectType;

	// Defect ����Ʈ ������ ��츸...
	if( !EngineerDefectJudgment[nFork].bDefectItemUse )
		return E_ERROR_CODE_TRUE;

	// Blob ����
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// �ҷ� ���� ���͸�
		if( !DoColorFilter(nFork, nDefectColor) )
			continue;
	

		// ���� �׸�� ���� 2��
		int nFeatureCount = E_FEATURE_COUNT * 2;
		bool bFilter	= true;
		bool bInit		= false;

		for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
		{

			// ���� �׸� ������ ��츸
			if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
				continue;

			// �ѹ��̶� ���� �� ���.
			bInit = true;

			// ������ ���͸� �����ϸ� true / �������� ������ false ��ȯ
			if(	!DoFiltering(
				m_BlobResult[i],										// Blob ���
				nForj/2,												// �� Feature
				EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// ������ ( <, >, ==, <=, >= )
				EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// ��
			{

				bFilter = false;
				break;
			}
		}

		

		// ������ ��� ������ ���� �ϴ� ���, ��� �Է�
		if( bInit && bFilter )
		{
			m_BlobResult[i].bFiltering = true;

			// ������ ����
			if(bPtRotate)
			{
				int nL, nT, nR, nB;

				// ȸ�� ��, ���� ��ǥ ���
				int X = (int)( dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx );
				int Y = (int)( dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy );

				// �ʱ� ����
				nL = nR = X;
				nT = nB = Y;

				// �ܰ��� ���� ��ŭ
				for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
				{
					// ȸ�� ��, ���� ��ǥ ���
					X = (int)( dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx );
					Y = (int)( dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy );

					// ����
					if( nL > X )	nL = X;
					if( nR < X )	nR = X;
					if( nT > Y )	nT = Y;
					if( nB < Y )	nB = Y;
				}

				cv::Point ptTemp;

				ptTemp.x = (int)( dCos_ * (nL - nCx) - dSin_ * (nT - nCy) + nCx );
				ptTemp.y = (int)( dSin_ * (nL - nCx) + dCos_ * (nT - nCy) + nCy );
				pResultBlob->ptLT		[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptLT		[nDefectCount].y = (LONG)ptTemp.y;

				ptTemp.x = (int)( dCos_ * (nR - nCx) - dSin_ * (nT - nCy) + nCx );
				ptTemp.y = (int)( dSin_ * (nR - nCx) + dCos_ * (nT - nCy) + nCy );
				pResultBlob->ptRT		[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptRT		[nDefectCount].y = (LONG)ptTemp.y;

				ptTemp.x = (int)( dCos_ * (nR - nCx) - dSin_ * (nB - nCy) + nCx );
				ptTemp.y = (int)( dSin_ * (nR - nCx) + dCos_ * (nB - nCy) + nCy );
				pResultBlob->ptRB		[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptRB		[nDefectCount].y = (LONG)ptTemp.y;

				ptTemp.x = (int)( dCos_ * (nL - nCx) - dSin_ * (nB - nCy) + nCx );
				ptTemp.y = (int)( dSin_ * (nL - nCx) + dCos_ * (nB - nCy) + nCy );
				pResultBlob->ptLB		[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptLB		[nDefectCount].y = (LONG)ptTemp.y;
			}
			else
			{
				int nL, nT, nR, nB;
				int X = m_BlobResult[i].ptContours[0].x;
				int Y = m_BlobResult[i].ptContours[0].y;

				// �ʱ� ����
				nL = nR = X;
				nT = nB = Y;

				// �ܰ��� ���� ��ŭ
				for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
				{
					X = m_BlobResult[i].ptContours[k].x; 
					Y = m_BlobResult[i].ptContours[k].y;

					// ����
					if( nL > X )	nL = X;
					if( nR < X )	nR = X;
					if( nT > Y )	nT = Y;
					if( nB < Y )	nB = Y;
				}

				pResultBlob->ptLT		[nDefectCount].x = nL	;
				pResultBlob->ptLT		[nDefectCount].y = nT	;

				pResultBlob->ptRT		[nDefectCount].x = nR	;
				pResultBlob->ptRT		[nDefectCount].y = nT	;

				pResultBlob->ptRB		[nDefectCount].x = nR	;
				pResultBlob->ptRB		[nDefectCount].y = nB	;

				pResultBlob->ptLB		[nDefectCount].x =  nL	;
				pResultBlob->ptLB		[nDefectCount].y =	nB	;
			}

			// UI�� �Ѱ��� ��� �ֱ�
			pResultBlob->nArea			[nDefectCount] = m_BlobResult[i].nArea;
			pResultBlob->nMaxGV			[nDefectCount] = m_BlobResult[i].nMaxGV;
			pResultBlob->nMinGV			[nDefectCount] = m_BlobResult[i].nMinGV;
			pResultBlob->dMeanGV		[nDefectCount] = m_BlobResult[i].fMeanGV;

			pResultBlob->nCenterx		[nDefectCount] = m_BlobResult[i].ptCenter.x;
			pResultBlob->nCentery		[nDefectCount] = m_BlobResult[i].ptCenter.y;

			pResultBlob->dBackGroundGV	[nDefectCount] = m_BlobResult[i].fBKGV;

			pResultBlob->dCompactness	[nDefectCount] = m_BlobResult[i].fCompactness;
			pResultBlob->dSigma			[nDefectCount] = m_BlobResult[i].fStdDev;
			pResultBlob->dF_Min			[nDefectCount] = m_BlobResult[i].fMajorAxis;
			pResultBlob->dF_Max			[nDefectCount] = m_BlobResult[i].fMinorAxis;

			pResultBlob->dBreadth		[nDefectCount] = m_BlobResult[i].fMajorAxis;
			pResultBlob->dF_Min			[nDefectCount] = m_BlobResult[i].fMajorAxis;
			pResultBlob->dF_Max			[nDefectCount] = m_BlobResult[i].fMinorAxis;
			pResultBlob->dF_Elongation	[nDefectCount] = m_BlobResult[i].fAxisRatio;
			pResultBlob->dCompactness	[nDefectCount] = m_BlobResult[i].fCompactness;
			pResultBlob->dRoundness		[nDefectCount] = m_BlobResult[i].fRoundness;

			pResultBlob->nDefectColor	[nDefectCount] = nDefectColor;
			pResultBlob->nDefectJudge	[nDefectCount] = nFork;			// �ش� �ҷ�
			pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

#if USE_ALG_HIST
			// KSW 17.06.24 ��ü ������׷�
			memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
			// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
			calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

			
			// �ҷ� ��ȣ �׸���
			if( !matDrawBuffer.empty() && bDefectNum )
			{
				cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x-2, pResultBlob->ptRT[nDefectCount].y-10, 30, 12), cv::Scalar(0, 0, 0), -1);

				char str[256]={0,};
				sprintf_s(str, sizeof(str),"%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
				cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
				cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
			}

			// ������ �ҷ� ī��Ʈ ����
			nDefectCount++;
		}

		

		// �ִ� �ҷ� ���� ���� ��� ���� ������
		if( nDefectCount >= nMaxDefectCount )
			break;
	}

	return E_ERROR_CODE_TRUE;
}

// Blob & ���� ���
long CFeatureExtraction::DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate)
{
	writeInspectLog(__FUNCTION__, _T("Start."));

	// �ð� ���� ����
	cTimeOut.TimeCheckStart();

	// �޸� ����
	Release();

	// �Ķ���� NULL�� ���.
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if( matSrcImage.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount		= nCommonPara[	E_PARA_COMMON_MAX_DEFECT_COUNT			];						// 00 : �ҷ� �ִ� ����
	bool	bImageSave			= (nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_FLAG			] > 0) ? true : false;	// 01 : �˰��� �߰���� Image Save 
	int&	nSaveImageCount		= nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_COUNT			];						// 02 : ���� ���� ���� ī��Ʈ
	int		nImageNum			= nCommonPara[	E_PARA_COMMON_ALG_IMAGE_NUMBER			];						// 03 : ���� ���� ��ȣ
	int		nCamNum				= nCommonPara[	E_PARA_COMMON_CAMERA_NUMBER				];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[	E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[	E_PARA_COMMON_ALG_NUMBER				];						// 06 : �˰��� ��ȣ
	int		nThrdIndex			= nCommonPara[	E_PARA_COMMON_THREAD_ID					];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG		] > 0) ? true : false;	// 08 : Draw Defect Num ǥ��
	bool	bDrawDust			= (nCommonPara[	E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust ǥ��
	int		nPatternImageNum	= nCommonPara[	E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI �� ���� ���� ���� ��ȣ
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell ȸ���� �߽� y��ǥ
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� �Է� �� ����
	int& nDefectCount = pResultBlob->nDefectCount;

	// �ִ� �ҷ� ���� ���� ��� ���� ������
	if( nDefectCount >= nMaxDefectCount )
		return E_ERROR_CODE_TRUE;
	
	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );
	double	dSin_	= sin( -dTheta );
	double	dCos_	= cos( -dTheta );

	// �� ����
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);

	writeInspectLog(__FUNCTION__, _T("BlobCalculate."));

	// Blob ����
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// �ð� ���� Ȯ��
		if( cTimeOut.GetTimeOutFlag() )	continue;

		// KSW 17.10.16 [�ӽ�] - E_DEFECT_JUDGEMENT_MURA_MULT_BP �켱����
		bool	bMultFlag		= false;
		bool	bMultCalcFlag	= false;
		if( EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_MULT_BP].bDefectItemUse )
			bMultFlag = true;

		// Defect ����Ʈ ��ŭ
		for (int nFork = 0 ; nFork < E_DEFECT_JUDGEMENT_COUNT ; nFork++)
		{
			// KSW 17.10.16 [�ӽ�] - E_DEFECT_JUDGEMENT_MURA_MULT_BP ���� ��, ó������ ����
			if( bMultFlag && bMultCalcFlag )
			{
				bMultFlag	= false;
				nFork		= 0;
			}

			// KSW 17.10.16 [�ӽ�] - E_DEFECT_JUDGEMENT_MURA_MULT_BP �켱 ����
			else if( bMultFlag )
			{
				nFork = E_DEFECT_JUDGEMENT_MURA_MULT_BP;
				bMultCalcFlag = true;
			}						

			// Defect ����Ʈ ������ ��츸...
			else if( !EngineerDefectJudgment[nFork].bDefectItemUse )
				continue;

			// �ҷ� ���� ���͸�
			if( !DoColorFilter(nFork, nDefectColor) )
				continue;

			// ���� �׸�� ���� 2��
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter	= true;
			bool bInit		= false;
			for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
			{
				// ���� �׸� ������ ��츸
				if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
					continue;

				// �ѹ��̶� ���� �� ���.
				bInit = true;

				// ������ ���͸� �����ϸ� true / �������� ������ false ��ȯ
				if(	!DoFiltering(
						m_BlobResult[i],										// Blob ���
						nForj/2,												// �� Feature
						EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// ������ ( <, >, ==, <=, >= )
						EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// ��
				{
					bFilter = false;
					break;
				}
			}

			// ������ ��� ������ ���� �ϴ� ���, ��� �Է�
			if( bInit && bFilter )
			{
				m_BlobResult[i].bFiltering = true;				
				
				// ������ ����
				if(bPtRotate)
				{
					int nL, nT, nR, nB;

					// ȸ�� ��, ���� ��ǥ ���
					int X = (int)( dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx );
					int Y = (int)( dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy );
					
					// �ʱ� ����
					nL = nR = X;
					nT = nB = Y;
					
					// �ܰ��� ���� ��ŭ
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						// ȸ�� ��, ���� ��ǥ ���
						X = (int)( dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx );
						Y = (int)( dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy );

						// ����
						if( nL > X )	nL = X;
						if( nR < X )	nR = X;
						if( nT > Y )	nT = Y;
						if( nB < Y )	nB = Y;
					}

					cv::Point ptTemp;

					ptTemp.x = (int)( dCos_ * (nL - nCx) - dSin_ * (nT - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nL - nCx) + dCos_ * (nT - nCy) + nCy );
					pResultBlob->ptLT		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptLT		[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)( dCos_ * (nR - nCx) - dSin_ * (nT - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nR - nCx) + dCos_ * (nT - nCy) + nCy );
					pResultBlob->ptRT		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptRT		[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)( dCos_ * (nR - nCx) - dSin_ * (nB - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nR - nCx) + dCos_ * (nB - nCy) + nCy );
					pResultBlob->ptRB		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptRB		[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)( dCos_ * (nL - nCx) - dSin_ * (nB - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nL - nCx) + dCos_ * (nB - nCy) + nCy );
					pResultBlob->ptLB		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptLB		[nDefectCount].y = (LONG)ptTemp.y;
				}
				else
				{
					int nL, nT, nR, nB;
					int X = m_BlobResult[i].ptContours[0].x;
					int Y = m_BlobResult[i].ptContours[0].y;

					// �ʱ� ����
					nL = nR = X;
					nT = nB = Y;

					// �ܰ��� ���� ��ŭ
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						X = m_BlobResult[i].ptContours[k].x; 
						Y = m_BlobResult[i].ptContours[k].y;

						// ����
						if( nL > X )	nL = X;
						if( nR < X )	nR = X;
						if( nT > Y )	nT = Y;
						if( nB < Y )	nB = Y;
					}	

					pResultBlob->ptLT		[nDefectCount].x = nL	;
					pResultBlob->ptLT		[nDefectCount].y = nT	;

					pResultBlob->ptRT		[nDefectCount].x = nR	;
					pResultBlob->ptRT		[nDefectCount].y = nT	;

					pResultBlob->ptRB		[nDefectCount].x = nR	;
					pResultBlob->ptRB		[nDefectCount].y = nB	;

					pResultBlob->ptLB		[nDefectCount].x =  nL	;
					pResultBlob->ptLB		[nDefectCount].y =	nB	;
				}

				// UI�� �Ѱ��� ��� �ֱ�
				pResultBlob->nArea			[nDefectCount] = m_BlobResult[i].nArea;
				pResultBlob->nMaxGV			[nDefectCount] = m_BlobResult[i].nMaxGV;
				pResultBlob->nMinGV			[nDefectCount] = m_BlobResult[i].nMinGV;
				pResultBlob->dMeanGV		[nDefectCount] = m_BlobResult[i].fMeanGV;

				pResultBlob->nCenterx		[nDefectCount] = m_BlobResult[i].ptCenter.x;
				pResultBlob->nCentery		[nDefectCount] = m_BlobResult[i].ptCenter.y;

				pResultBlob->dBackGroundGV	[nDefectCount] = m_BlobResult[i].fBKGV;

				pResultBlob->dCompactness	[nDefectCount] = m_BlobResult[i].fCompactness;
				pResultBlob->dSigma			[nDefectCount] = m_BlobResult[i].fStdDev;
				pResultBlob->dF_Min			[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Max			[nDefectCount] = m_BlobResult[i].fMinorAxis;

				pResultBlob->dBreadth		[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Min			[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Max			[nDefectCount] = m_BlobResult[i].fMinorAxis;
				pResultBlob->dF_Elongation	[nDefectCount] = m_BlobResult[i].fAxisRatio;
				pResultBlob->dCompactness	[nDefectCount] = m_BlobResult[i].fCompactness;
				pResultBlob->dRoundness		[nDefectCount] = m_BlobResult[i].fRoundness;

				pResultBlob->nDefectColor	[nDefectCount] = nDefectColor;
				pResultBlob->nDefectJudge	[nDefectCount] = nFork;			// �ش� �ҷ�
				pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

#if USE_ALG_HIST
				// KSW 17.06.24 ��ü ������׷�
				memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
				// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
				calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

				// �ҷ� ��ȣ �׸���
				if( !matDrawBuffer.empty() && bDefectNum )
				{
					cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x-2, pResultBlob->ptRT[nDefectCount].y-10, 30, 12), cv::Scalar(0, 0, 0), -1);

					char str[256]={0,};
					sprintf_s(str, sizeof(str),"%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
					cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
					cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
				}

				// ������ �ҷ� ī��Ʈ ����
				nDefectCount++;

				break;
			}
		}

		// �ִ� �ҷ� ���� ���� ��� ���� ������
		if( nDefectCount >= nMaxDefectCount )
			break;
	}

	writeInspectLog(__FUNCTION__, _T("Filtering & Result."));

	// �ð� ���� Ȯ��
	if( cTimeOut.GetTimeOutFlag() )	return E_ERROR_CODE_TIME_OUT;

	return E_ERROR_CODE_TRUE;
}
// Blob & ���� ��� ROI
long CFeatureExtraction::DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate)
{
	writeInspectLog(__FUNCTION__, _T("Start."));

	// �ð� ���� ����
	cTimeOut.TimeCheckStart();

	// �޸� ����
	Release();

	// �Ķ���� NULL�� ���.
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (pResultBlob == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if (matSrcImage.empty())			return E_ERROR_CODE_EMPTY_BUFFER;
	if (matThresholdImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount		= nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];						// 00 : �ҷ� �ִ� ����
	bool	bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;	// 01 : �˰��� �߰���� Image Save 
	int&	nSaveImageCount		= nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];						// 02 : ���� ���� ���� ī��Ʈ
	int		nImageNum			= nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];						// 03 : ���� ���� ��ȣ
	int		nCamNum				= nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[E_PARA_COMMON_ALG_NUMBER				];						// 06 : �˰��� ��ȣ
	int		nThrdIndex			= nCommonPara[E_PARA_COMMON_THREAD_ID				];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG	] > 0) ? true : false;	// 08 : Draw Defect Num ǥ��
	bool	bDrawDust			= (nCommonPara[E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust ǥ��
	int		nPatternImageNum	= nCommonPara[E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI �� ���� ���� ���� ��ȣ
	float	fAngle				= nCommonPara[E_PARA_COMMON_ROTATE_ANGLE			] / 1000.f;				// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
	int		nCx					= nCommonPara[E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy					= nCommonPara[E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell ȸ���� �߽� y��ǥ
	int		nPS					= nCommonPara[E_PARA_COMMON_PS_MODE					];						// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� �Է� �� ����
	int& nDefectCount = pResultBlob->nDefectCount;

	// �ִ� �ҷ� ���� ���� ��� ���� ������
	if (nDefectCount >= nMaxDefectCount)
		return E_ERROR_CODE_TRUE;

	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = -fAngle * PI / 180.;
	double	dSin = sin(dTheta);
	double	dCos = cos(dTheta);
	double	dSin_ = sin(-dTheta);
	double	dCos_ = cos(-dTheta);

	// �� ����
	DoBlobCalculate(matThresholdImage, rectROI, matSrcImage, nMaxDefectCount);

	writeInspectLog(__FUNCTION__, _T("BlobCalculate."));

	//cv::Mat Defect_map = matSrcImage.clone(); // pwj

	// Blob ����
	for (int i = 0; i < m_BlobResult.size(); i++)
	{
		// �ð� ���� Ȯ��
		if (cTimeOut.GetTimeOutFlag())	continue;

		// KSW 17.10.16 [�ӽ�] - E_DEFECT_JUDGEMENT_MURA_MULT_BP �켱����
		bool	bMultFlag		= false;
		bool	bMultCalcFlag	= false;
		if (EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_MULT_BP].bDefectItemUse)
			bMultFlag = true;

		// Defect ����Ʈ ��ŭ
		for (int nFork = 0; nFork < E_DEFECT_JUDGEMENT_COUNT; nFork++)
		{
			// KSW 17.10.16 [�ӽ�] - E_DEFECT_JUDGEMENT_MURA_MULT_BP ���� ��, ó������ ����
			if (bMultFlag && bMultCalcFlag)
			{
				bMultFlag = false;
				nFork = 0;
			}

			// KSW 17.10.16 [�ӽ�] - E_DEFECT_JUDGEMENT_MURA_MULT_BP �켱 ����
			else if (bMultFlag)
			{
				nFork = E_DEFECT_JUDGEMENT_MURA_MULT_BP;
				bMultCalcFlag = true;
			}

			// Defect ����Ʈ ������ ��츸...
			else if (!EngineerDefectJudgment[nFork].bDefectItemUse)
				continue;

			// �ҷ� ���� ���͸�
			if (!DoColorFilter(nFork, nDefectColor))
				continue;

			// ���� �׸�� ���� 2��
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter = true;
			bool bInit = false;
			for (int nForj = 0; nForj < nFeatureCount; nForj++)
			{
				// ���� �׸� ������ ��츸
				if (!EngineerDefectJudgment[nFork].Judgment[nForj].bUse)
					continue;

				////////////////////////////////////////////////////////////////////////// 04.20 choi
				if (nForj == E_FEATURE_GVAREA_RATIO_TEST * 2 || nForj == (E_FEATURE_GVAREA_RATIO_TEST * 2) + 1) {

					double dValue = EngineerDefectJudgment[nFork].Judgment[nForj].dValue;

					int nTmp = (int)dValue % 10000;
					double nPer = ((double)dValue-(double)nTmp) / 10000.0;
					double nRatio = nTmp / 1000;
					

					double Mean_GV = m_BlobResult[i].fBKGV * nRatio;


					if (Mean_GV < 0)				Mean_GV = 0;
					if (Mean_GV > IMAGE_MAX_GV)	Mean_GV = IMAGE_MAX_GV - 1;

					__int64 nHist = 0;
					for (int m = Mean_GV; m <= 255; m++)
						nHist += m_BlobResult[i].nHist[m];

					double Area_per = nHist / m_BlobResult[i].nBoxArea;
					Area_per *= 100;

					m_BlobResult[i].fAreaPer = Area_per;
					m_BlobResult[i].nJudge_GV = Mean_GV;
					m_BlobResult[i].nIn_Count = nHist;
				}

				//////////////////////////////////////////////////////////////////////////
				// �ѹ��̶� ���� �� ���.
				bInit = true;

				// ������ ���͸� �����ϸ� true / �������� ������ false ��ȯ
				if (!DoFiltering(
					m_BlobResult[i],										// Blob ���
					nForj / 2,												// �� Feature
					EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// ������ ( <, >, ==, <=, >= )
					EngineerDefectJudgment[nFork].Judgment[nForj].dValue))	// ��
				{
					bFilter = false;
					break;
				}
			}

			// ������ ��� ������ ���� �ϴ� ���, ��� �Է�
			if (bInit && bFilter)
			{
				m_BlobResult[i].bFiltering = true;

				// ������ ����
				//if (bPtRotate)
				if (0)
				{
					int nL, nT, nR, nB;

					// ȸ�� ��, ���� ��ǥ ���
					int X = (int)(dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx);
					int Y = (int)(dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy);

					// �ʱ� ����
					nL = nR = X;
					nT = nB = Y;

					// �ܰ��� ���� ��ŭ
					for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
					{
						// ȸ�� ��, ���� ��ǥ ���
						X = (int)(dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx);
						Y = (int)(dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy);

						// ����
						if (nL > X)	nL = X;
						if (nR < X)	nR = X;
						if (nT > Y)	nT = Y;
						if (nB < Y)	nB = Y;
					}

					cv::Point ptTemp;

					ptTemp.x							= (int)(dCos_ * (nL - nCx) - dSin_ * (nT - nCy) + nCx);
					ptTemp.y							= (int)(dSin_ * (nL - nCx) + dCos_ * (nT - nCy) + nCy);
					pResultBlob->ptLT[nDefectCount].x	= (LONG)ptTemp.x;
					pResultBlob->ptLT[nDefectCount].y	= (LONG)ptTemp.y;

					ptTemp.x							= (int)(dCos_ * (nR - nCx) - dSin_ * (nT - nCy) + nCx);
					ptTemp.y							= (int)(dSin_ * (nR - nCx) + dCos_ * (nT - nCy) + nCy);
					pResultBlob->ptRT[nDefectCount].x	= (LONG)ptTemp.x;
					pResultBlob->ptRT[nDefectCount].y	= (LONG)ptTemp.y;

					ptTemp.x							= (int)(dCos_ * (nR - nCx) - dSin_ * (nB - nCy) + nCx);
					ptTemp.y							= (int)(dSin_ * (nR - nCx) + dCos_ * (nB - nCy) + nCy);
					pResultBlob->ptRB[nDefectCount].x	= (LONG)ptTemp.x;
					pResultBlob->ptRB[nDefectCount].y	= (LONG)ptTemp.y;

					ptTemp.x							= (int)(dCos_ * (nL - nCx) - dSin_ * (nB - nCy) + nCx);
					ptTemp.y							= (int)(dSin_ * (nL - nCx) + dCos_ * (nB - nCy) + nCy);
					pResultBlob->ptLB[nDefectCount].x	= (LONG)ptTemp.x;
					pResultBlob->ptLB[nDefectCount].y	= (LONG)ptTemp.y;
				}
				else
				{
					int nL, nT, nR, nB;
					int X = m_BlobResult[i].ptContours[0].x;
					int Y = m_BlobResult[i].ptContours[0].y;

					// �ʱ� ����
					nL = nR = X;
					nT = nB = Y;

					// �ܰ��� ���� ��ŭ
					for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
					{
						X = m_BlobResult[i].ptContours[k].x;
						Y = m_BlobResult[i].ptContours[k].y;

						// ����
						if (nL > X)	nL = X;
						if (nR < X)	nR = X;
						if (nT > Y)	nT = Y;
						if (nB < Y)	nB = Y;
					}

					pResultBlob->ptLT[nDefectCount].x = nL;
					pResultBlob->ptLT[nDefectCount].y = nT;

					pResultBlob->ptRT[nDefectCount].x = nR;
					pResultBlob->ptRT[nDefectCount].y = nT;

					pResultBlob->ptRB[nDefectCount].x = nR;
					pResultBlob->ptRB[nDefectCount].y = nB;

					pResultBlob->ptLB[nDefectCount].x = nL;
					pResultBlob->ptLB[nDefectCount].y = nB;
				}

				// UI�� �Ѱ��� ��� �ֱ�
				pResultBlob->nArea[nDefectCount]			= m_BlobResult[i].nArea;
				pResultBlob->nMaxGV[nDefectCount]			= m_BlobResult[i].nMaxGV;
				pResultBlob->nMinGV[nDefectCount]			= m_BlobResult[i].nMinGV;
				pResultBlob->dMeanGV[nDefectCount]			= m_BlobResult[i].fMeanGV;

				pResultBlob->nCenterx[nDefectCount]			= m_BlobResult[i].ptCenter.x;
				pResultBlob->nCentery[nDefectCount]			= m_BlobResult[i].ptCenter.y;

				pResultBlob->dBackGroundGV[nDefectCount]	= m_BlobResult[i].fBKGV;

				pResultBlob->dCompactness[nDefectCount]		= m_BlobResult[i].fCompactness;
				pResultBlob->dSigma[nDefectCount]			= m_BlobResult[i].fStdDev;
				pResultBlob->dF_Min[nDefectCount]			= m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Max[nDefectCount]			= m_BlobResult[i].fMinorAxis;

				pResultBlob->dBreadth[nDefectCount]			= m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Min[nDefectCount]			= m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Max[nDefectCount]			= m_BlobResult[i].fMinorAxis;
				pResultBlob->dF_Elongation[nDefectCount]	= m_BlobResult[i].fAxisRatio;
				pResultBlob->dCompactness[nDefectCount]		= m_BlobResult[i].fCompactness;
				pResultBlob->dRoundness[nDefectCount]		= m_BlobResult[i].fRoundness;
				pResultBlob->dF_MeanAreaRatio[nDefectCount] = m_BlobResult[i].fMeanAreaRatio; //choikwangil
			




				pResultBlob->dF_AreaPer[nDefectCount] = m_BlobResult[i].fAreaPer; //choikwangil
				pResultBlob->nJudge_GV[nDefectCount] = m_BlobResult[i].nJudge_GV; //choikwangil
				pResultBlob->nIn_Count[nDefectCount] = m_BlobResult[i].nIn_Count; //choikwangil

				pResultBlob->nDefectColor[nDefectCount]		= nDefectColor;
				pResultBlob->nDefectJudge[nDefectCount]		= nFork;			// �ش� �ҷ�
				pResultBlob->nPatternClassify[nDefectCount] = nPatternImageNum;


#if USE_ALG_HIST
				// KSW 17.06.24 ��ü ������׷�
				memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
				// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
				calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

				// �ҷ� ��ȣ �׸���
				if (!matDrawBuffer.empty() && bDefectNum)
				{
					cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x - 2, pResultBlob->ptRT[nDefectCount].y - 10, 30, 12), cv::Scalar(0, 0, 0), -1);

					char str[256] = { 0, };
					sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
					cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
					cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
				}

				//defect Map �ҷ� ���� �׸��� pwj
				//cv::rectangle(Defect_map, cv::Rect(pResultBlob->ptRT[nDefectCount].x - 2, pResultBlob->ptRT[nDefectCount].y - 10, 30, 12), cv::Scalar(0, 0, 0), 1);

				//cv::imwrite("D:\\Defect map.jpg", Defect_map);

				// ������ �ҷ� ī��Ʈ ����
				nDefectCount++;

				break;
			}
		}

		// �ִ� �ҷ� ���� ���� ��� ���� ������
		if (nDefectCount >= nMaxDefectCount)
			break;
	}

	writeInspectLog(__FUNCTION__, _T("Filtering & Result."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory(), m_nAlgType);
// 	}

	// �ð� ���� Ȯ��
	if (cTimeOut.GetTimeOutFlag())	return E_ERROR_CODE_TIME_OUT;

	return E_ERROR_CODE_TRUE;
}

long CFeatureExtraction::DoDefectBlobJudgment_LCP(cv::Mat& matSrcImage, cv::Mat& matColorImage ,cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI,
	int* nCommonPara, long nDefectColor, double* dPara, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate)
{
	writeInspectLog(__FUNCTION__, _T("Start."));

	// �ð� ���� ����
	cTimeOut.TimeCheckStart();

	// �޸� ����
	Release();

	// �Ķ���� NULL�� ���.
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (pResultBlob == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if (matSrcImage.empty())			return E_ERROR_CODE_EMPTY_BUFFER;
	if (matThresholdImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];						// 00 : �ҷ� �ִ� ����
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;	// 01 : �˰��� �߰���� Image Save 
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];						// 02 : ���� ���� ���� ī��Ʈ
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];						// 03 : ���� ���� ��ȣ
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];						// 04 : Cam Number
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];						// 05 : ROI Number
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];						// 06 : �˰��� ��ȣ
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];						// 07 : Thread ID
	bool	bDefectNum = (nCommonPara[E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG] > 0) ? true : false;	// 08 : Draw Defect Num ǥ��
	bool	bDrawDust = (nCommonPara[E_PARA_COMMON_DRAW_DUST_FLAG] > 0) ? true : false;	// 09 : Draw Dust ǥ��
	int		nPatternImageNum = nCommonPara[E_PARA_COMMON_UI_IMAGE_NUMBER];						// 10 : UI �� ���� ���� ���� ��ȣ
	float	fAngle = nCommonPara[E_PARA_COMMON_ROTATE_ANGLE] / 1000.f;				// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
	int		nCx = nCommonPara[E_PARA_COMMON_ROTATE_CENTER_X];						// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy = nCommonPara[E_PARA_COMMON_ROTATE_CENTER_Y];						// 13 : Cell ȸ���� �߽� y��ǥ
	int		nPS = nCommonPara[E_PARA_COMMON_PS_MODE];						// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

																			// �ҷ� �Է� �� ����
	int& nDefectCount = pResultBlob->nDefectCount;

	// �ִ� �ҷ� ���� ���� ��� ���� ������
	if (nDefectCount >= nMaxDefectCount)
		return E_ERROR_CODE_TRUE;

	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = -fAngle * PI / 180.;
	double	dSin = sin(dTheta);
	double	dCos = cos(dTheta);
	double	dSin_ = sin(-dTheta);
	double	dCos_ = cos(-dTheta);

	// �� ����
	DoBlobCalculate_LCP(matThresholdImage, rectROI, dPara, matSrcImage, matColorImage ,nMaxDefectCount);

	writeInspectLog(__FUNCTION__, _T("BlobCalculate."));

	//cv::Mat Defect_map = matSrcImage.clone(); // pwj

	// Blob ����
	for (int i = 0; i < m_BlobResult.size(); i++)
	{
		// �ð� ���� Ȯ��
		if (cTimeOut.GetTimeOutFlag())	continue;

		// KSW 17.10.16 [�ӽ�] - E_DEFECT_JUDGEMENT_MURA_MULT_BP �켱����
		bool	bMultFlag = false;
		bool	bMultCalcFlag = false;
		if (EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_MULT_BP].bDefectItemUse)
			bMultFlag = true;

		// Defect ����Ʈ ��ŭ
		for (int nFork = 0; nFork < E_DEFECT_JUDGEMENT_COUNT; nFork++)
		{
			// KSW 17.10.16 [�ӽ�] - E_DEFECT_JUDGEMENT_MURA_MULT_BP ���� ��, ó������ ����
			if (bMultFlag && bMultCalcFlag)
			{
				bMultFlag = false;
				nFork = 0;
			}

			// KSW 17.10.16 [�ӽ�] - E_DEFECT_JUDGEMENT_MURA_MULT_BP �켱 ����
			else if (bMultFlag)
			{
				nFork = E_DEFECT_JUDGEMENT_MURA_MULT_BP;
				bMultCalcFlag = true;
			}

			// Defect ����Ʈ ������ ��츸...
			else if (!EngineerDefectJudgment[nFork].bDefectItemUse)
				continue;

			// �ҷ� ���� ���͸�
			if (!DoColorFilter(nFork, nDefectColor))
				continue;

			// ���� �׸�� ���� 2��
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter = true;
			bool bInit = false;
			for (int nForj = 0; nForj < nFeatureCount; nForj++)
			{
				// ���� �׸� ������ ��츸
				if (!EngineerDefectJudgment[nFork].Judgment[nForj].bUse)
					continue;

				////////////////////////////////////////////////////////////////////////// 04.20 choi
				if (nForj == E_FEATURE_GVAREA_RATIO_TEST * 2 || nForj == (E_FEATURE_GVAREA_RATIO_TEST * 2) + 1) {

					double dValue = EngineerDefectJudgment[nFork].Judgment[nForj].dValue;

					int nTmp = (int)dValue % 10000;
					double nPer = ((double)dValue - (double)nTmp) / 10000.0;
					double nRatio = nTmp / 1000;


					double Mean_GV = m_BlobResult[i].fBKGV * nRatio;


					if (Mean_GV < 0)				Mean_GV = 0;
					if (Mean_GV > IMAGE_MAX_GV)	Mean_GV = IMAGE_MAX_GV - 1;

					__int64 nHist = 0;
					for (int m = Mean_GV; m <= 255; m++)
						nHist += m_BlobResult[i].nHist[m];

					double Area_per = nHist / m_BlobResult[i].nBoxArea;
					Area_per *= 100;

					m_BlobResult[i].fAreaPer = Area_per;
					m_BlobResult[i].nJudge_GV = Mean_GV;
					m_BlobResult[i].nIn_Count = nHist;
				}

				//////////////////////////////////////////////////////////////////////////
				// �ѹ��̶� ���� �� ���.
				bInit = true;

				// ������ ���͸� �����ϸ� true / �������� ������ false ��ȯ
				if (!DoFiltering(
					m_BlobResult[i],										// Blob ���
					nForj / 2,												// �� Feature
					EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// ������ ( <, >, ==, <=, >= )
					EngineerDefectJudgment[nFork].Judgment[nForj].dValue))	// ��
				{
					bFilter = false;
					break;
				}
			}

			// ������ ��� ������ ���� �ϴ� ���, ��� �Է�
			if (bInit && bFilter)
			{
				m_BlobResult[i].bFiltering = true;

				// ������ ����
				//if (bPtRotate)
				if (0)
				{
					int nL, nT, nR, nB;

					// ȸ�� ��, ���� ��ǥ ���
					int X = (int)(dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx);
					int Y = (int)(dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy);

					// �ʱ� ����
					nL = nR = X;
					nT = nB = Y;

					// �ܰ��� ���� ��ŭ
					for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
					{
						// ȸ�� ��, ���� ��ǥ ���
						X = (int)(dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx);
						Y = (int)(dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy);

						// ����
						if (nL > X)	nL = X;
						if (nR < X)	nR = X;
						if (nT > Y)	nT = Y;
						if (nB < Y)	nB = Y;
					}

					cv::Point ptTemp;

					ptTemp.x = (int)(dCos_ * (nL - nCx) - dSin_ * (nT - nCy) + nCx);
					ptTemp.y = (int)(dSin_ * (nL - nCx) + dCos_ * (nT - nCy) + nCy);
					pResultBlob->ptLT[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptLT[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)(dCos_ * (nR - nCx) - dSin_ * (nT - nCy) + nCx);
					ptTemp.y = (int)(dSin_ * (nR - nCx) + dCos_ * (nT - nCy) + nCy);
					pResultBlob->ptRT[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptRT[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)(dCos_ * (nR - nCx) - dSin_ * (nB - nCy) + nCx);
					ptTemp.y = (int)(dSin_ * (nR - nCx) + dCos_ * (nB - nCy) + nCy);
					pResultBlob->ptRB[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptRB[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)(dCos_ * (nL - nCx) - dSin_ * (nB - nCy) + nCx);
					ptTemp.y = (int)(dSin_ * (nL - nCx) + dCos_ * (nB - nCy) + nCy);
					pResultBlob->ptLB[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptLB[nDefectCount].y = (LONG)ptTemp.y;
				}
				else
				{
					int nL, nT, nR, nB;
					int X = m_BlobResult[i].ptContours[0].x;
					int Y = m_BlobResult[i].ptContours[0].y;

					// �ʱ� ����
					nL = nR = X;
					nT = nB = Y;

					// �ܰ��� ���� ��ŭ
					for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
					{
						X = m_BlobResult[i].ptContours[k].x;
						Y = m_BlobResult[i].ptContours[k].y;

						// ����
						if (nL > X)	nL = X;
						if (nR < X)	nR = X;
						if (nT > Y)	nT = Y;
						if (nB < Y)	nB = Y;
					}

					pResultBlob->ptLT[nDefectCount].x = nL;
					pResultBlob->ptLT[nDefectCount].y = nT;

					pResultBlob->ptRT[nDefectCount].x = nR;
					pResultBlob->ptRT[nDefectCount].y = nT;

					pResultBlob->ptRB[nDefectCount].x = nR;
					pResultBlob->ptRB[nDefectCount].y = nB;

					pResultBlob->ptLB[nDefectCount].x = nL;
					pResultBlob->ptLB[nDefectCount].y = nB;
				}

				// UI�� �Ѱ��� ��� �ֱ�
				pResultBlob->nArea[nDefectCount] = m_BlobResult[i].nArea;
				pResultBlob->nMaxGV[nDefectCount] = m_BlobResult[i].nMaxGV;
				pResultBlob->nMinGV[nDefectCount] = m_BlobResult[i].nMinGV;
				pResultBlob->dMeanGV[nDefectCount] = m_BlobResult[i].fMeanGV;

				pResultBlob->nCenterx[nDefectCount] = m_BlobResult[i].ptCenter.x;
				pResultBlob->nCentery[nDefectCount] = m_BlobResult[i].ptCenter.y;

				pResultBlob->dBackGroundGV[nDefectCount] = m_BlobResult[i].fBKGV;

				pResultBlob->dCompactness[nDefectCount] = m_BlobResult[i].fCompactness;
				pResultBlob->dSigma[nDefectCount] = m_BlobResult[i].fStdDev;
				pResultBlob->dF_Min[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Max[nDefectCount] = m_BlobResult[i].fMinorAxis;

				pResultBlob->dBreadth[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Min[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Max[nDefectCount] = m_BlobResult[i].fMinorAxis;
				pResultBlob->dF_Elongation[nDefectCount] = m_BlobResult[i].fAxisRatio;
				pResultBlob->dCompactness[nDefectCount] = m_BlobResult[i].fCompactness;
				pResultBlob->dRoundness[nDefectCount] = m_BlobResult[i].fRoundness;
				pResultBlob->dF_MeanAreaRatio[nDefectCount] = m_BlobResult[i].fMeanAreaRatio; //choikwangil
				pResultBlob->dF_MinBoxRatio[nDefectCount] = m_BlobResult[i].fMinBoxRatio;

				pResultBlob->Lab_Degree[nDefectCount] = m_BlobResult[i].fLab_Degree; //choikwangil
				pResultBlob->Lab_Lenght[nDefectCount] = m_BlobResult[i].fLab_Lenght; //choikwangil

				pResultBlob->Lab_avg_L[nDefectCount] = m_BlobResult[i].fLab_avg_L; //choikwangil
				pResultBlob->Lab_avg_a[nDefectCount] = m_BlobResult[i].fLab_avg_a; //choikwangil
				pResultBlob->Lab_avg_b[nDefectCount] = m_BlobResult[i].fLab_avg_b; //choikwangil
				pResultBlob->Lab_diff_L[nDefectCount] = m_BlobResult[i].fLab_diff_L; //choikwangil
				pResultBlob->Lab_diff_a[nDefectCount] = m_BlobResult[i].fLab_diff_a; //choikwangil
				pResultBlob->Lab_diff_b[nDefectCount] = m_BlobResult[i].fLab_diff_b; //choikwangil
				pResultBlob->strColor[nDefectCount] = m_BlobResult[i].strColor; //choikwangil

				pResultBlob->dF_AreaPer[nDefectCount] = m_BlobResult[i].fAreaPer; //choikwangil
				pResultBlob->nJudge_GV[nDefectCount] = m_BlobResult[i].nJudge_GV; //choikwangil
				pResultBlob->nIn_Count[nDefectCount] = m_BlobResult[i].nIn_Count; //choikwangil

				pResultBlob->nDefectColor[nDefectCount] = nDefectColor;

				if (m_BlobResult[i].strColor == "Red" && (	nFork == E_DEFECT_JUDGEMENT_POINT_DARK ||
															nFork == E_DEFECT_JUDGEMENT_POINT_DARK_SP_1 ||
															nFork == E_DEFECT_JUDGEMENT_POINT_DARK_SP_2 ||
															nFork == E_DEFECT_JUDGEMENT_POINT_DARK_SP_3 || 
															nFork == E_DEFECT_JUDGEMENT_POINT_GROUP_DARK ||
															nFork == E_DEFECT_JUDGEMENT_LINE_X_DARK ||
															nFork == E_DEFECT_JUDGEMENT_LINE_Y_DARK)
				)
				{
					pResultBlob->nDefectJudge[nDefectCount] = E_DEFECT_JUDGEMENT_POINT_DIMPLE;
				}
				else {
					pResultBlob->nDefectJudge[nDefectCount] = nFork;			// �ش� �ҷ�
				}

				pResultBlob->nPatternClassify[nDefectCount] = nPatternImageNum;

#if USE_ALG_HIST
				// KSW 17.06.24 ��ü ������׷�
				memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
				// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
				calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

				// �ҷ� ��ȣ �׸���
				if (!matDrawBuffer.empty() && bDefectNum)
				{
					cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x - 2, pResultBlob->ptRT[nDefectCount].y - 10, 30, 12), cv::Scalar(0, 0, 0), -1);

					char str[256] = { 0, };
					sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
					cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
					cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
				}

				//defect Map �ҷ� ���� �׸��� pwj
				//cv::rectangle(Defect_map, cv::Rect(pResultBlob->ptRT[nDefectCount].x - 2, pResultBlob->ptRT[nDefectCount].y - 10, 30, 12), cv::Scalar(0, 0, 0), 1);

				//cv::imwrite("D:\\Defect map.jpg", Defect_map);

				// ������ �ҷ� ī��Ʈ ����
				nDefectCount++;

				break;
			}
		}

		// �ִ� �ҷ� ���� ���� ��� ���� ������
		if (nDefectCount >= nMaxDefectCount)
			break;
	}

	writeInspectLog(__FUNCTION__, _T("Filtering & Result."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory(), m_nAlgType);
// 	}

	// �ð� ���� Ȯ��
	if (cTimeOut.GetTimeOutFlag())	return E_ERROR_CODE_TIME_OUT;

	return E_ERROR_CODE_TRUE;
}

// Blob & ���� ���
long CFeatureExtraction::DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, STRU_DEFECT_ITEM* EngineerDefectJudgment, int nDefectType, int nMaxDefectCount)
{
	// �ð� ���� ����
	cTimeOut.TimeCheckStart();

	// �޸� ����
	Release();

	// �Ķ���� NULL�� ���.
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if( matSrcImage.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	if( nDefectType < 0	)							return E_ERROR_CODE_EMPTY_PARA;
	if( nDefectType >= E_DEFECT_JUDGEMENT_COUNT	)	return E_ERROR_CODE_EMPTY_PARA;

	// �� ����
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);

	// ������ Defect ����Ʈ
	int nFork = nDefectType;

	// Defect ����Ʈ ������ ��츸...
	if( !EngineerDefectJudgment[nFork].bDefectItemUse )
		return E_ERROR_CODE_TRUE;

	// Blob ����
#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// Defect ����Ʈ ��ŭ
		//for (int nFork = 0 ; nFork < E_DEFECT_JUDGEMENT_COUNT ; nFork++)		
		{
			// ���� �׸�� ���� 2��
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter	= true;
			bool bInit		= false;
			for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
			{
				// ���� �׸� ������ ��츸
				if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
					continue;

				// �ѹ��̶� ���� �� ���.
				bInit = true;

				// ������ ���͸� �����ϸ� true / �������� ������ false ��ȯ
				if(	!DoFiltering(
					m_BlobResult[i],										// Blob ���
					nForj/2,												// �� Feature
					EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// ������ ( <, >, ==, <=, >= )
					EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// ��
				{
					bFilter = false;
					break;
				}
			}

			// ������ ��� ������ ���� �ϴ� ���
			if( bInit && bFilter )
			{
				m_BlobResult[i].bFiltering = true;
			}
		}
	}

	return E_ERROR_CODE_TRUE;
}

// Blob & ���� ���
long CFeatureExtraction::DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, STRU_DEFECT_ITEM* EngineerDefectJudgment, int nMaxDefectCount)
{
	// �ð� ���� ����
	cTimeOut.TimeCheckStart();

	// �޸� ����
	Release();

	// �Ķ���� NULL�� ���.
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if( matSrcImage.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// �� ����
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);

	// Blob ����
#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// �ð� ���� Ȯ��
		if( cTimeOut.GetTimeOutFlag() )	continue;

		// Defect ����Ʈ ��ŭ
		for (int nFork = 0 ; nFork < E_DEFECT_JUDGEMENT_COUNT ; nFork++)		
		{
			// Defect ����Ʈ ������ ��츸...
			if( !EngineerDefectJudgment[nFork].bDefectItemUse )
				continue;

			// ���� �׸�� ���� 2��
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter	= true;
			bool bInit		= false;
			for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
			{
				// ���� �׸� ������ ��츸
				if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
					continue;

				// �ѹ��̶� ���� �� ���.
				bInit = true;

				// ������ ���͸� �����ϸ� true / �������� ������ false ��ȯ
				if(	!DoFiltering(
					m_BlobResult[i],										// Blob ���
					nForj/2,												// �� Feature
					EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// ������ ( <, >, ==, <=, >= )
					EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// ��
				{
					bFilter = false;
					break;
				}
			}

			// ������ ��� ������ ���� �ϴ� ���
			if( bInit && bFilter )
			{
				m_BlobResult[i].bFiltering = true;

				break;
			}
		}
	}

	// �ð� ���� Ȯ��
	if( cTimeOut.GetTimeOutFlag() )	return E_ERROR_CODE_TIME_OUT;

	return E_ERROR_CODE_TRUE;
}

////////ssy 0718 �ܰ��� ��ġ�ϴ��� �˻�
// Blob & ���� ���
long CFeatureExtraction::DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType,  bool bPtRotate, CRect prerectROI, int offset)
{
	// �ð� ���� ����
	cTimeOut.TimeCheckStart();

	// �޸� ����
	Release();

	// �Ķ���� NULL�� ���.
	if( nCommonPara == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)				return E_ERROR_CODE_EMPTY_PARA;

	if( nDefectType < 0	)							return E_ERROR_CODE_EMPTY_PARA;
	if( nDefectType >= E_DEFECT_JUDGEMENT_COUNT	)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if( matSrcImage.empty() )						return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )					return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount		= nCommonPara[	E_PARA_COMMON_MAX_DEFECT_COUNT			];						// 00 : �ҷ� �ִ� ����
	bool	bImageSave			= (nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_FLAG			] > 0) ? true : false;	// 01 : �˰��� �߰���� Image Save 
	int&	nSaveImageCount		= nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_COUNT			];						// 02 : ���� ���� ���� ī��Ʈ
	int		nImageNum			= nCommonPara[	E_PARA_COMMON_ALG_IMAGE_NUMBER			];						// 03 : ���� ���� ��ȣ
	int		nCamNum				= nCommonPara[	E_PARA_COMMON_CAMERA_NUMBER				];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[	E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[	E_PARA_COMMON_ALG_NUMBER				];						// 06 : �˰��� ��ȣ
	int		nThrdIndex			= nCommonPara[	E_PARA_COMMON_THREAD_ID					];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG		] > 0) ? true : false;	// 08 : Draw Defect Num ǥ��
	bool	bDrawDust			= (nCommonPara[	E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust ǥ��
	int		nPatternImageNum	= nCommonPara[	E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI �� ���� ���� ���� ��ȣ
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell ȸ���� �߽� y��ǥ
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� �Է� �� ����
	int& nDefectCount = pResultBlob->nDefectCount;

	// �ִ� �ҷ� ���� ���� ��� ���� ������
	if( nDefectCount >= nMaxDefectCount )
		return E_ERROR_CODE_TRUE;

	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );
	double	dSin_	= sin( -dTheta );
	double	dCos_	= cos( -dTheta );

	// �� ����
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);

	// ������ Defect ����Ʈ
	int nFork = nDefectType;

	// Defect ����Ʈ ������ ��츸...
	if( !EngineerDefectJudgment[nFork].bDefectItemUse )
		return E_ERROR_CODE_TRUE;

	// Blob ����
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// �ҷ� ���� ���͸�
		if( !DoColorFilter(nFork, nDefectColor) )
			continue;

		// ���� �׸�� ���� 2��
		int nFeatureCount = E_FEATURE_COUNT * 2;
		bool bFilter	= true;
		bool bInit		= false;
		for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
		{
			// ���� �׸� ������ ��츸
			if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
				continue;

			// �ѹ��̶� ���� �� ���.
			bInit = true;
			if (nForj >= E_FEATURE_IS_EDGE_C * 2)
				{
					//m_BlobResult[i].
					if(is_edge(m_BlobResult[i],nForj/2,prerectROI,offset))
					{
						bFilter = true;
					}
					else
					{
						bFilter = false;
						break;
					}
				}
			else
			{
				// ������ ���͸� �����ϸ� true / �������� ������ false ��ȯ
				if(	!DoFiltering(
					m_BlobResult[i],										// Blob ���
					nForj/2,												// �� Feature
					EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// ������ ( <, >, ==, <=, >= )
					EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// ��
				{
					bFilter = false;
					break;
				}
			}
		}

		// ������ ��� ������ ���� �ϴ� ���, ��� �Է�
		if( bInit && bFilter )
		{
			m_BlobResult[i].bFiltering = true;

			// ������ ����
			if(bPtRotate)
			{
				int nL, nT, nR, nB;

				// ȸ�� ��, ���� ��ǥ ���
				int X = (int)( dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx );
				int Y = (int)( dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy );

				// �ʱ� ����
				nL = nR = X;
				nT = nB = Y;

				// �ܰ��� ���� ��ŭ
				for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
				{
					// ȸ�� ��, ���� ��ǥ ���
					X = (int)( dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx );
					Y = (int)( dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy );

					// ����
					if( nL > X )	nL = X;
					if( nR < X )	nR = X;
					if( nT > Y )	nT = Y;
					if( nB < Y )	nB = Y;
				}

				cv::Point ptTemp;

				ptTemp.x = (int)( dCos_ * (nL - nCx) - dSin_ * (nT - nCy) + nCx );
				ptTemp.y = (int)( dSin_ * (nL - nCx) + dCos_ * (nT - nCy) + nCy );
				pResultBlob->ptLT		[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptLT		[nDefectCount].y = (LONG)ptTemp.y;

				ptTemp.x = (int)( dCos_ * (nR - nCx) - dSin_ * (nT - nCy) + nCx );
				ptTemp.y = (int)( dSin_ * (nR - nCx) + dCos_ * (nT - nCy) + nCy );
				pResultBlob->ptRT		[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptRT		[nDefectCount].y = (LONG)ptTemp.y;

				ptTemp.x = (int)( dCos_ * (nR - nCx) - dSin_ * (nB - nCy) + nCx );
				ptTemp.y = (int)( dSin_ * (nR - nCx) + dCos_ * (nB - nCy) + nCy );
				pResultBlob->ptRB		[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptRB		[nDefectCount].y = (LONG)ptTemp.y;

				ptTemp.x = (int)( dCos_ * (nL - nCx) - dSin_ * (nB - nCy) + nCx );
				ptTemp.y = (int)( dSin_ * (nL - nCx) + dCos_ * (nB - nCy) + nCy );
				pResultBlob->ptLB		[nDefectCount].x = (LONG)ptTemp.x;
				pResultBlob->ptLB		[nDefectCount].y = (LONG)ptTemp.y;
			}
			else
			{
				int nL, nT, nR, nB;
				int X = m_BlobResult[i].ptContours[0].x;
				int Y = m_BlobResult[i].ptContours[0].y;

				// �ʱ� ����
				nL = nR = X;
				nT = nB = Y;

				// �ܰ��� ���� ��ŭ
				for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
				{
					X = m_BlobResult[i].ptContours[k].x; 
					Y = m_BlobResult[i].ptContours[k].y;

					// ����
					if( nL > X )	nL = X;
					if( nR < X )	nR = X;
					if( nT > Y )	nT = Y;
					if( nB < Y )	nB = Y;
				}

				pResultBlob->ptLT		[nDefectCount].x = nL	;
				pResultBlob->ptLT		[nDefectCount].y = nT	;

				pResultBlob->ptRT		[nDefectCount].x = nR	;
				pResultBlob->ptRT		[nDefectCount].y = nT	;

				pResultBlob->ptRB		[nDefectCount].x = nR	;
				pResultBlob->ptRB		[nDefectCount].y = nB	;

				pResultBlob->ptLB		[nDefectCount].x =  nL	;
				pResultBlob->ptLB		[nDefectCount].y =	nB	;
			}

			// UI�� �Ѱ��� ��� �ֱ�
			pResultBlob->nArea			[nDefectCount] = m_BlobResult[i].nArea;
			pResultBlob->nMaxGV			[nDefectCount] = m_BlobResult[i].nMaxGV;
			pResultBlob->nMinGV			[nDefectCount] = m_BlobResult[i].nMinGV;
			pResultBlob->dMeanGV		[nDefectCount] = m_BlobResult[i].fMeanGV;

			pResultBlob->nCenterx		[nDefectCount] = m_BlobResult[i].ptCenter.x;
			pResultBlob->nCentery		[nDefectCount] = m_BlobResult[i].ptCenter.y;

			pResultBlob->dBackGroundGV	[nDefectCount] = m_BlobResult[i].fBKGV;

			pResultBlob->dCompactness	[nDefectCount] = m_BlobResult[i].fCompactness;
			pResultBlob->dSigma			[nDefectCount] = m_BlobResult[i].fStdDev;
			pResultBlob->dF_Min			[nDefectCount] = m_BlobResult[i].fMajorAxis;
			pResultBlob->dF_Max			[nDefectCount] = m_BlobResult[i].fMinorAxis;

			pResultBlob->dBreadth		[nDefectCount] = m_BlobResult[i].fMajorAxis;
			pResultBlob->dF_Min			[nDefectCount] = m_BlobResult[i].fMajorAxis;
			pResultBlob->dF_Max			[nDefectCount] = m_BlobResult[i].fMinorAxis;
			pResultBlob->dF_Elongation	[nDefectCount] = m_BlobResult[i].fAxisRatio;
			pResultBlob->dCompactness	[nDefectCount] = m_BlobResult[i].fCompactness;
			pResultBlob->dRoundness		[nDefectCount] = m_BlobResult[i].fRoundness;

			pResultBlob->nDefectColor	[nDefectCount] = nDefectColor;
			pResultBlob->nDefectJudge	[nDefectCount] = nFork;			// �ش� �ҷ�
			pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

#if USE_ALG_HIST
			// KSW 17.06.24 ��ü ������׷�
			memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
			// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
			calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

			// �ҷ� ��ȣ �׸���
			if( !matDrawBuffer.empty() && bDefectNum )
			{
				cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x-2, pResultBlob->ptRT[nDefectCount].y-10, 30, 12), cv::Scalar(0, 0, 0), -1);

				char str[256]={0,};
				sprintf_s(str, sizeof(str),"%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
				cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
				cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
			}

			// ������ �ҷ� ī��Ʈ ����
			nDefectCount++;
		}

		// �ִ� �ҷ� ���� ���� ��� ���� ������
		if( nDefectCount >= nMaxDefectCount )
			break;
	}

	return E_ERROR_CODE_TRUE;
}

////////SSY 1124 ������ �ҷ��鸸 �˻��¿�� ���ͷ� �Ѱ���
// Blob & ���� ���
long CFeatureExtraction::DoDefectBlobMultiJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, vector<int> nDefectType,  bool bPtRotate, CRect prerectROI, int offset)
{
	// �ð� ���� ����
	cTimeOut.TimeCheckStart();

	// �޸� ����
	Release();

	// �Ķ���� NULL�� ���.
	if( nCommonPara == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)				return E_ERROR_CODE_EMPTY_PARA;

	if( nDefectType.size() == 0 )					return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if( matSrcImage.empty() )						return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )					return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount		= nCommonPara[	E_PARA_COMMON_MAX_DEFECT_COUNT			];						// 00 : �ҷ� �ִ� ����
	bool	bImageSave			= (nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_FLAG			] > 0) ? true : false;	// 01 : �˰��� �߰���� Image Save 
	int&	nSaveImageCount		= nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_COUNT			];						// 02 : ���� ���� ���� ī��Ʈ
	int		nImageNum			= nCommonPara[	E_PARA_COMMON_ALG_IMAGE_NUMBER			];						// 03 : ���� ���� ��ȣ
	int		nCamNum				= nCommonPara[	E_PARA_COMMON_CAMERA_NUMBER				];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[	E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[	E_PARA_COMMON_ALG_NUMBER				];						// 06 : �˰��� ��ȣ
	int		nThrdIndex			= nCommonPara[	E_PARA_COMMON_THREAD_ID					];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG		] > 0) ? true : false;	// 08 : Draw Defect Num ǥ��
	bool	bDrawDust			= (nCommonPara[	E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust ǥ��
	int		nPatternImageNum	= nCommonPara[	E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI �� ���� ���� ���� ��ȣ
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell ȸ���� �߽� y��ǥ
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� �Է� �� ����
	int& nDefectCount = pResultBlob->nDefectCount;

	// �ִ� �ҷ� ���� ���� ��� ���� ������
	if( nDefectCount >= nMaxDefectCount )
		return E_ERROR_CODE_TRUE;

	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );
	double	dSin_	= sin( -dTheta );
	double	dCos_	= cos( -dTheta );

	// �� ����
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);

	// ������ Defect ����Ʈ
	vector<int> nForks = nDefectType;
	int nFork=0;

	// Blob ����
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		for (int j=0; j<nForks.size(); j++)
		{
			nFork = nForks[j];
			
			// �ҷ� ���� ���͸�
			if( !DoColorFilter(nFork, nDefectColor) )
				continue;
			
			// Defect ����Ʈ ������ ��츸...
			if( !EngineerDefectJudgment[nFork].bDefectItemUse )
				continue;

			// ���� �׸�� ���� 2��
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter	= true;
			bool bInit		= false;
			for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
			{
				// ���� �׸� ������ ��츸
				if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
					continue;

				// �ѹ��̶� ���� �� ���.
				bInit = true;
				if (nForj >= E_FEATURE_IS_EDGE_C * 2)
				{
					if(is_edge(m_BlobResult[i],nForj/2,prerectROI,offset))
					{
						bFilter = true;
					}
					else
					{
						bFilter = false;
						break;
					}
				}
				else
				{
					// ������ ���͸� �����ϸ� true / �������� ������ false ��ȯ
					if(	!DoFiltering(
						m_BlobResult[i],										// Blob ���
						nForj/2,												// �� Feature
						EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// ������ ( <, >, ==, <=, >= )
						EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// ��
					{
						bFilter = false;
						break;
					}
				}
			}

			// ������ ��� ������ ���� �ϴ� ���, ��� �Է�
			if( bInit && bFilter )
			{
				m_BlobResult[i].bFiltering = true;

				// ������ ����
				if(bPtRotate)
				{
					int nL, nT, nR, nB;

					// ȸ�� ��, ���� ��ǥ ���
					int X = (int)( dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx );
					int Y = (int)( dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy );

					// �ʱ� ����
					nL = nR = X;
					nT = nB = Y;

					// �ܰ��� ���� ��ŭ
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						// ȸ�� ��, ���� ��ǥ ���
						X = (int)( dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx );
						Y = (int)( dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy );

						// ����
						if( nL > X )	nL = X;
						if( nR < X )	nR = X;
						if( nT > Y )	nT = Y;
						if( nB < Y )	nB = Y;
					}

					cv::Point ptTemp;

					ptTemp.x = (int)( dCos_ * (nL - nCx) - dSin_ * (nT - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nL - nCx) + dCos_ * (nT - nCy) + nCy );
					pResultBlob->ptLT		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptLT		[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)( dCos_ * (nR - nCx) - dSin_ * (nT - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nR - nCx) + dCos_ * (nT - nCy) + nCy );
					pResultBlob->ptRT		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptRT		[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)( dCos_ * (nR - nCx) - dSin_ * (nB - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nR - nCx) + dCos_ * (nB - nCy) + nCy );
					pResultBlob->ptRB		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptRB		[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)( dCos_ * (nL - nCx) - dSin_ * (nB - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nL - nCx) + dCos_ * (nB - nCy) + nCy );
					pResultBlob->ptLB		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptLB		[nDefectCount].y = (LONG)ptTemp.y;
				}
				else
				{
					int nL, nT, nR, nB;
					int X = m_BlobResult[i].ptContours[0].x;
					int Y = m_BlobResult[i].ptContours[0].y;

					// �ʱ� ����
					nL = nR = X;
					nT = nB = Y;

					// �ܰ��� ���� ��ŭ
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						X = m_BlobResult[i].ptContours[k].x; 
						Y = m_BlobResult[i].ptContours[k].y;

						// ����
						if( nL > X )	nL = X;
						if( nR < X )	nR = X;
						if( nT > Y )	nT = Y;
						if( nB < Y )	nB = Y;
					}

					pResultBlob->ptLT		[nDefectCount].x = nL	;
					pResultBlob->ptLT		[nDefectCount].y = nT	;

					pResultBlob->ptRT		[nDefectCount].x = nR	;
					pResultBlob->ptRT		[nDefectCount].y = nT	;

					pResultBlob->ptRB		[nDefectCount].x = nR	;
					pResultBlob->ptRB		[nDefectCount].y = nB	;

					pResultBlob->ptLB		[nDefectCount].x =  nL	;
					pResultBlob->ptLB		[nDefectCount].y =	nB	;
				}

				// UI�� �Ѱ��� ��� �ֱ�
				pResultBlob->nArea			[nDefectCount] = m_BlobResult[i].nArea;
				pResultBlob->nMaxGV			[nDefectCount] = m_BlobResult[i].nMaxGV;
				pResultBlob->nMinGV			[nDefectCount] = m_BlobResult[i].nMinGV;
				pResultBlob->dMeanGV		[nDefectCount] = m_BlobResult[i].fMeanGV;

				pResultBlob->nCenterx		[nDefectCount] = m_BlobResult[i].ptCenter.x;
				pResultBlob->nCentery		[nDefectCount] = m_BlobResult[i].ptCenter.y;

				pResultBlob->dBackGroundGV	[nDefectCount] = m_BlobResult[i].fBKGV;

				pResultBlob->dCompactness	[nDefectCount] = m_BlobResult[i].fCompactness;
				pResultBlob->dSigma			[nDefectCount] = m_BlobResult[i].fStdDev;
				pResultBlob->dF_Min			[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Max			[nDefectCount] = m_BlobResult[i].fMinorAxis;

				pResultBlob->dBreadth		[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Min			[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Max			[nDefectCount] = m_BlobResult[i].fMinorAxis;
				pResultBlob->dF_Elongation	[nDefectCount] = m_BlobResult[i].fAxisRatio;
				pResultBlob->dCompactness	[nDefectCount] = m_BlobResult[i].fCompactness;
				pResultBlob->dRoundness		[nDefectCount] = m_BlobResult[i].fRoundness;

				pResultBlob->nDefectColor	[nDefectCount] = nDefectColor;
				pResultBlob->nDefectJudge	[nDefectCount] = nFork;			// �ش� �ҷ�
				pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

#if USE_ALG_HIST
				// KSW 17.06.24 ��ü ������׷�
				memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif
	
#if USE_ALG_CONTOURS
				// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
				calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

				// �ҷ� ��ȣ �׸���
				if( !matDrawBuffer.empty() && bDefectNum )
				{
					cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x-2, pResultBlob->ptRT[nDefectCount].y-10, 30, 12), cv::Scalar(0, 0, 0), -1);

					char str[256]={0,};
					sprintf_s(str, sizeof(str),"%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
					cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
					cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
				}

				// ������ �ҷ� ī��Ʈ ����
				nDefectCount++;
			}

			// �ִ� �ҷ� ���� ���� ��� ���� ������
			if( nDefectCount >= nMaxDefectCount )
				break;
		}		
	}

	return E_ERROR_CODE_TRUE;
}

// Blob & ���� ���
long CFeatureExtraction::DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate, CRect prerectROI, int offset)
{
	// �ð� ���� ����
	cTimeOut.TimeCheckStart();

	// �޸� ����
	Release();

	// �Ķ���� NULL�� ���.
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// ���� ���۰� NULL�� ���
	if( matSrcImage.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	int		nMaxDefectCount		= nCommonPara[	E_PARA_COMMON_MAX_DEFECT_COUNT			];						// 00 : �ҷ� �ִ� ����
	bool	bImageSave			= (nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_FLAG			] > 0) ? true : false;	// 01 : �˰��� �߰���� Image Save 
	int&	nSaveImageCount		= nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_COUNT			];						// 02 : ���� ���� ���� ī��Ʈ
	int		nImageNum			= nCommonPara[	E_PARA_COMMON_ALG_IMAGE_NUMBER			];						// 03 : ���� ���� ��ȣ
	int		nCamNum				= nCommonPara[	E_PARA_COMMON_CAMERA_NUMBER				];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[	E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[	E_PARA_COMMON_ALG_NUMBER				];						// 06 : �˰��� ��ȣ
	int		nThrdIndex			= nCommonPara[	E_PARA_COMMON_THREAD_ID					];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG		] > 0) ? true : false;	// 08 : Draw Defect Num ǥ��
	bool	bDrawDust			= (nCommonPara[	E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust ǥ��
	int		nPatternImageNum	= nCommonPara[	E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI �� ���� ���� ���� ��ȣ
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell ȸ���� �߽� y��ǥ
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	
	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );
	double	dSin_	= sin( -dTheta );
	double	dCos_	= cos( -dTheta );

	// �� ����
	DoBlobCalculate(matThresholdImage, matSrcImage);

	// �ҷ� �Է� �� ����
	int& nDefectCount = pResultBlob->nDefectCount;

	// Blob ����
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// �ð� ���� Ȯ��
		if( cTimeOut.GetTimeOutFlag() )	continue;

		// Defect ����Ʈ ��ŭ
		for (int nFork = 0 ; nFork < E_DEFECT_JUDGEMENT_COUNT ; nFork++)
		{
			// Defect ����Ʈ ������ ��츸...
			if( !EngineerDefectJudgment[nFork].bDefectItemUse )
				continue;

			// �ҷ� ���� ���͸�
			if( !DoColorFilter(nFork, nDefectColor) )
				continue;

			// ���� �׸�� ���� 2��
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter	= true;
			bool bInit		= false;
			for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
			{
				// ���� �׸� ������ ��츸

				if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
					continue;

				// �ѹ��̶� ���� �� ���.
				bInit = true;
				if (nForj >= E_FEATURE_IS_EDGE_C * 2)
				{
					//m_BlobResult[i].
					if(is_edge(m_BlobResult[i],nForj/2,prerectROI,offset))
					{
						bFilter = true;
					}
					else
					{
						bFilter = false;
						break;
					}
				}
				else
				{
					// ������ ���͸� �����ϸ� true / �������� ������ false ��ȯ
					if(	!DoFiltering(
							m_BlobResult[i],										// Blob ���
							nForj/2,												// �� Feature
							EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// ������ ( <, >, ==, <=, >= )
							EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// ��
					{
						bFilter = false;
						break;
					}
				}

			}

			// ������ ��� ������ ���� �ϴ� ���, ��� �Է�
			if( bInit && bFilter )
			{
				m_BlobResult[i].bFiltering = true;				
				
				// ������ ����
				if(bPtRotate)
				{
					int nL, nT, nR, nB;

					// ȸ�� ��, ���� ��ǥ ���
					int X = (int)( dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx );
					int Y = (int)( dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy );
					
					// �ʱ� ����
					nL = nR = X;
					nT = nB = Y;
					
					// �ܰ��� ���� ��ŭ
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						// ȸ�� ��, ���� ��ǥ ���
						X = (int)( dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx );
						Y = (int)( dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy );

						// ����
						if( nL > X )	nL = X;
						if( nR < X )	nR = X;
						if( nT > Y )	nT = Y;
						if( nB < Y )	nB = Y;
					}

					cv::Point ptTemp;

					ptTemp.x = (int)( dCos_ * (nL - nCx) - dSin_ * (nT - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nL - nCx) + dCos_ * (nT - nCy) + nCy );
					pResultBlob->ptLT		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptLT		[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)( dCos_ * (nR - nCx) - dSin_ * (nT - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nR - nCx) + dCos_ * (nT - nCy) + nCy );
					pResultBlob->ptRT		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptRT		[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)( dCos_ * (nR - nCx) - dSin_ * (nB - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nR - nCx) + dCos_ * (nB - nCy) + nCy );
					pResultBlob->ptRB		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptRB		[nDefectCount].y = (LONG)ptTemp.y;

					ptTemp.x = (int)( dCos_ * (nL - nCx) - dSin_ * (nB - nCy) + nCx );
					ptTemp.y = (int)( dSin_ * (nL - nCx) + dCos_ * (nB - nCy) + nCy );
					pResultBlob->ptLB		[nDefectCount].x = (LONG)ptTemp.x;
					pResultBlob->ptLB		[nDefectCount].y = (LONG)ptTemp.y;
				}
				else
				{
					int nL, nT, nR, nB;
					int X = m_BlobResult[i].ptContours[0].x;
					int Y = m_BlobResult[i].ptContours[0].y;

					// �ʱ� ����
					nL = nR = X;
					nT = nB = Y;

					// �ܰ��� ���� ��ŭ
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						X = m_BlobResult[i].ptContours[k].x; 
						Y = m_BlobResult[i].ptContours[k].y;

						// ����
						if( nL > X )	nL = X;
						if( nR < X )	nR = X;
						if( nT > Y )	nT = Y;
						if( nB < Y )	nB = Y;
					}	

					pResultBlob->ptLT		[nDefectCount].x = nL	;
					pResultBlob->ptLT		[nDefectCount].y = nT	;

					pResultBlob->ptRT		[nDefectCount].x = nR	;
					pResultBlob->ptRT		[nDefectCount].y = nT	;

					pResultBlob->ptRB		[nDefectCount].x = nR	;
					pResultBlob->ptRB		[nDefectCount].y = nB	;

					pResultBlob->ptLB		[nDefectCount].x =  nL	;
					pResultBlob->ptLB		[nDefectCount].y =	nB	;
				}

				// UI�� �Ѱ��� ��� �ֱ�
				pResultBlob->nArea			[nDefectCount] = m_BlobResult[i].nArea;
				pResultBlob->nMaxGV			[nDefectCount] = m_BlobResult[i].nMaxGV;
				pResultBlob->nMinGV			[nDefectCount] = m_BlobResult[i].nMinGV;
				pResultBlob->dMeanGV		[nDefectCount] = m_BlobResult[i].fMeanGV;

				pResultBlob->nCenterx		[nDefectCount] = m_BlobResult[i].ptCenter.x;
				pResultBlob->nCentery		[nDefectCount] = m_BlobResult[i].ptCenter.y;

				pResultBlob->dBackGroundGV	[nDefectCount] = m_BlobResult[i].fBKGV;

				pResultBlob->dCompactness	[nDefectCount] = m_BlobResult[i].fCompactness;
				pResultBlob->dSigma			[nDefectCount] = m_BlobResult[i].fStdDev;
				pResultBlob->dF_Min			[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Max			[nDefectCount] = m_BlobResult[i].fMinorAxis;

				pResultBlob->dBreadth		[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Min			[nDefectCount] = m_BlobResult[i].fMajorAxis;
				pResultBlob->dF_Max			[nDefectCount] = m_BlobResult[i].fMinorAxis;
				pResultBlob->dF_Elongation	[nDefectCount] = m_BlobResult[i].fAxisRatio;
				pResultBlob->dCompactness	[nDefectCount] = m_BlobResult[i].fCompactness;
				pResultBlob->dRoundness		[nDefectCount] = m_BlobResult[i].fRoundness;


				pResultBlob->nDefectColor	[nDefectCount] = nDefectColor;
				pResultBlob->nDefectJudge	[nDefectCount] = nFork;			// �ش� �ҷ�
				pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

#if USE_ALG_HIST
				// KSW 17.06.24 ��ü ������׷�
				memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
				// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
				calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

				// �ҷ� ��ȣ �׸���
				if( !matDrawBuffer.empty() && bDefectNum )
				{
					cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x-2, pResultBlob->ptRT[nDefectCount].y-10, 30, 12), cv::Scalar(0, 0, 0), -1);

					char str[256]={0,};
					sprintf_s(str, sizeof(str),"%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
					cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
					cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
				}

				// ������ �ҷ� ī��Ʈ ����
				nDefectCount++;

				break;
			}
		}

		// �ִ� �ҷ� ���� ���� ��� ���� ������
		if( nDefectCount >= nMaxDefectCount )
			break;
	}

	// �ð� ���� Ȯ��
	if( cTimeOut.GetTimeOutFlag() )	return E_ERROR_CODE_TIME_OUT;

	return E_ERROR_CODE_TRUE;
}

//ssy 0718 �ܰ��� ��ġ�ϴ��� �˻�
bool CFeatureExtraction::is_edge(tBLOB_FEATURE& tBlobResult, int nBlobFilter, CRect prerectROI, int offset)
{
	// ���͸� �Ȱ�� ����
	if( tBlobResult.bFiltering )	return false;
	//int offset = 5;
	bool bRes = false;
	int left = tBlobResult.rectBox.x;
	int top = tBlobResult.rectBox.y;
	int width = tBlobResult.rectBox.width;
	int height = tBlobResult.rectBox.height;

	int ori_left = prerectROI.left;
	int ori_top = prerectROI.top;
	int ori_width = prerectROI.Width();
	int ori_height = prerectROI.Height();

	bool is_c = false;
	bool is_v = false;
	bool is_h = false;
	bool is_cen = false;

	if ((left<=ori_left+offset && top<=ori_top+offset) || ((left+width) >= (ori_left+ori_width)-offset && top<=ori_top+offset) || (left<=ori_left+offset && (top+height) >= (ori_top+ori_height)-offset) || ((left+width)>= (ori_left+ori_width)-offset&& (top+height) >= (ori_top+ori_height)-offset))
	{
		is_c = true;
	}
	else if(top<=ori_top+offset || (top+height) >= (ori_top+ori_height)-offset)
	{
		is_v = true;
	}
	else if(left<=ori_left+offset || (left+width) >= (ori_left+ori_width)-offset)
	{
		is_h = true;
	}
	else
	{
		is_cen = true;
	}
	switch( nBlobFilter )
	{
		case E_FEATURE_IS_EDGE_C:
			if (is_c) bRes = true;
			break;

		case E_FEATURE_IS_EDGE_V:
			if (is_v) bRes = true;
			break;	

		case E_FEATURE_IS_EDGE_H:
			if (is_h) bRes = true;
			break;

		case E_FEATURE_IS_EDGE_CENTER:
			if (is_cen) bRes = true;
			break;
	}
	return bRes;
}

bool CFeatureExtraction::DrawBlob(cv::Mat& DrawBuffer, CvScalar DrawColor, long nOption, bool bSelect, float fFontSize)
{
	// ������ ���� ��� ����
	if( DrawBuffer.empty() )		return false;

	// Blob ���� ���Ѱ�� ����
	if( !m_bComplete )				return false;

	// Blob ��� 0�� ����
	if( m_BlobResult.size() == 0 )	return true;

	// �ɼ��� ���� ��� ����
	if( nOption == 0 )				return true;

	int i, j;

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (i=0 ; i<m_BlobResult.size() ; i++)
	{
		// �����Ѱ͸� �׸���
		if( !m_BlobResult[i].bFiltering && bSelect)	continue;

		// ȸ�� �ڽ� �׸���
		if( nOption & BLOB_DRAW_ROTATED_BOX )
		{
			cv::RotatedRect rRect = cv::RotatedRect(m_BlobResult[i].ptCenter, m_BlobResult[i].BoxSize, m_BlobResult[i].fAngle);

			cv::Point2f vertices[4];
			rRect.points(vertices);

			cv::line(DrawBuffer, vertices[0], vertices[1], DrawColor);
			cv::line(DrawBuffer, vertices[1], vertices[2], DrawColor);
			cv::line(DrawBuffer, vertices[2], vertices[3], DrawColor);
			cv::line(DrawBuffer, vertices[3], vertices[0], DrawColor);
		}

		// �ܰ� �ڽ� �׸���
		if( nOption & BLOB_DRAW_BOUNDING_BOX )
		{
			cv::Rect rect(m_BlobResult[i].rectBox);
			rect.x -= 5;
			rect.y -= 5;
			rect.width += 10;
			rect.height += 10;

			cv::rectangle(DrawBuffer, rect, DrawColor);
		}

		// Blob ��ü �׸���
		if( nOption & BLOB_DRAW_BLOBS )
		{
			// Gray �� ���
			if( DrawBuffer.channels() == 1 )
			{
				int nGrayColor = (int)(DrawColor.val[0] + DrawColor.val[1] + DrawColor.val[2]) / 3;

				for (j=0 ; j<m_BlobResult[i].ptIndexs.size() ; j++)
				{
					DrawBuffer.at<uchar>(m_BlobResult[i].ptIndexs[j].y, m_BlobResult[i].ptIndexs[j].x) = nGrayColor;
				}
			}
			// RGB �� ���
			else
			{
				for (j=0 ; j<m_BlobResult[i].ptIndexs.size() ; j++)
				{
					DrawBuffer.at<cv::Vec3b>(m_BlobResult[i].ptIndexs[j].y, m_BlobResult[i].ptIndexs[j].x)[0] = (int)DrawColor.val[0];
					DrawBuffer.at<cv::Vec3b>(m_BlobResult[i].ptIndexs[j].y, m_BlobResult[i].ptIndexs[j].x)[1] = (int)DrawColor.val[1];
					DrawBuffer.at<cv::Vec3b>(m_BlobResult[i].ptIndexs[j].y, m_BlobResult[i].ptIndexs[j].x)[2] = (int)DrawColor.val[2];
				}
			}
		}

		// Blob �ܰ��� �׸���
		if( nOption & BLOB_DRAW_BLOBS_CONTOUR )
		{
			// Gray �� ���
			if( DrawBuffer.channels() == 1 )
			{
				int nGrayColor = (int)(DrawColor.val[0] + DrawColor.val[1] + DrawColor.val[2]) / 3;

				for (j=0 ; j<m_BlobResult[i].ptContours.size() ; j++)
				{
					DrawBuffer.at<uchar>(m_BlobResult[i].ptContours[j].y, m_BlobResult[i].ptContours[j].x) = nGrayColor;
				}
			}
			// RGB �� ���
			else
			{
				for (j=0 ; j<m_BlobResult[i].ptContours.size() ; j++)
				{
					DrawBuffer.at<cv::Vec3b>(m_BlobResult[i].ptContours[j].y, m_BlobResult[i].ptContours[j].x)[0] = (int)DrawColor.val[0];
					DrawBuffer.at<cv::Vec3b>(m_BlobResult[i].ptContours[j].y, m_BlobResult[i].ptContours[j].x)[1] = (int)DrawColor.val[1];
					DrawBuffer.at<cv::Vec3b>(m_BlobResult[i].ptContours[j].y, m_BlobResult[i].ptContours[j].x)[2] = (int)DrawColor.val[2];
				}
			}
		}
	}

	return true;
}

bool CFeatureExtraction::DoColorFilter(int nDefectName, int nDefectColor)
{
	// ���� ó��
	if( nDefectName<0 )								return false;
	if( nDefectName>=E_DEFECT_JUDGEMENT_COUNT )		return false;

	int nColor = -1;

	switch( nDefectName )
	{
	case E_DEFECT_JUDGEMENT_POINT_DARK:				// ����
	case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
	case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
	case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
	case E_DEFECT_JUDGEMENT_POINT_GROUP_DARK:		// ���� ����
	case E_DEFECT_JUDGEMENT_LINE_X_DARK:			// ��ο� X����
	case E_DEFECT_JUDGEMENT_LINE_X_DARK_MULT:		// ��ο� X����
	case E_DEFECT_JUDGEMENT_LINE_Y_DARK:			// ��ο� Y����
	case E_DEFECT_JUDGEMENT_LINE_Y_DARK_MULT:		// ��ο� Y����
	case E_DEFECT_JUDGEMENT_MURA_AMORPH_DARK:		// ��ο� ������
	case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:		// ��ο� ����μ� ����
	case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:		// ��ο� ����μ� ����
	case E_DEFECT_JUDGEMENT_RETEST_POINT_DARK:		// ��˻� ����
	case E_DEFECT_JUDGEMENT_RETEST_LINE_DARK:		// ��˻� ��ο� X����
	case E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT:
	case E_DEFECT_JUDGEMENT_MURA_EMD_DARK:
	case E_DEFECT_JUDGEMENT_MURA_NUGI:
	case E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI:
	case E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_:
		nColor = E_DEFECT_COLOR_DARK;
		break;

	case E_DEFECT_JUDGEMENT_POINT_BRIGHT:			// ����
	case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:		// �����
	case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:		// ���� ����
	case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:			// ���� X����
	case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT_MULT:		// ���� X����
	case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:			// ���� Y����
	case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT_MULT:		// ���� Y����
	case E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT:		// ���� ������
	case E_DEFECT_JUDGEMENT_LINE_X_EDGE_BRIGHT:
	case E_DEFECT_JUDGEMENT_LINE_Y_EDGE_BRIGHT:
	case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
	case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:	// ���� ����μ� ����
	case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:	// ��˻� ����
	case E_DEFECT_JUDGEMENT_RETEST_LINE_BRIGHT:		// ��˻� ���� X����
	case E_DEFECT_JUDGEMENT_MURA_MULT_BP:			// KSW 17.09.27 - MURA_MULT_BP
	case E_DEFECT_JUDGEMENT_LINE_PCD_CRACK:			// PNZ 17.12.01 - �� �ҷ�
	case E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT:
	case E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT:      // 04.16 choi
	case E_DEFECT_JUDGEMENT_MURA_EMD_BRIGHT:
	case E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH:
		nColor = E_DEFECT_COLOR_BRIGHT;
		break;

	default:
		nColor = -1;
		break;
	}
	
	// �ҷ� ���� ������ ���
	if( nColor != -1 )
	{
		// ������ ����� �����Ѱ�?
		if( nColor == nDefectColor )
			return true;

		// ������ ����� �������� ������?
		else
			return false;
	}		

	// �ҷ� ���� �������� ���� ���.
	return true;
}

bool CFeatureExtraction::Compare(double dFeatureValue, int nSign, double dValue)
{
	bool bRes = false;

	// ������ ( <, >, ==, <=, >= )
	switch( nSign )
	{
	case	E_SIGN_EQUAL:				// x == judgment value
		bRes = ( dFeatureValue == dValue ) ? true : false;
		break;

	case	E_SIGN_NOT_EQUAL:			// x != judgment value
		bRes = ( dFeatureValue != dValue ) ? true : false;
		break;

	case	E_SIGN_GREATER:				// x >  judgment value
		bRes = ( dFeatureValue > dValue ) ? true : false;
		break;

	case	E_SIGN_LESS:				// x <  judgment value
		bRes = ( dFeatureValue < dValue ) ? true : false;
		break;

	case	E_SIGN_GREATER_OR_EQUAL:	// x >= judgment value
		bRes = ( dFeatureValue >= dValue ) ? true : false;
		break;

	case	E_SIGN_LESS_OR_EQUAL:		// x <= judgment value
		bRes = ( dFeatureValue <= dValue ) ? true : false;
		break;
	}

	return bRes;
}

bool CFeatureExtraction::DoFiltering(tBLOB_FEATURE& tBlobResult, int nBlobFilter, int nSign, double dValue)
{
	// ���͸� �Ȱ�� ����
	if( tBlobResult.bFiltering )	return false;

	bool bRes = false;

	switch( nBlobFilter )
	{
	case E_FEATURE_AREA:
		bRes = Compare( (double)tBlobResult.nArea, nSign, dValue);
		break;

	case E_FEATURE_BOX_AREA:
		bRes = Compare( (double)tBlobResult.nBoxArea, nSign, dValue);
		break;	

	case E_FEATURE_BOX_RATIO:
		bRes = Compare( (double)tBlobResult.fBoxRatio, nSign, dValue);
		break;

	case E_FEATURE_BOX_X:
		bRes = Compare( (double)tBlobResult.rectBox.width, nSign, dValue);
		break;

	case E_FEATURE_BOX_Y:
		bRes = Compare( (double)tBlobResult.rectBox.height, nSign, dValue);
		break;

	case E_FEATURE_SUM_GV:
		bRes = Compare( (double)tBlobResult.nSumGV, nSign, dValue);
		break;

	case E_FEATURE_MIN_GV:
		bRes = Compare( (double)tBlobResult.nMinGV, nSign, dValue);
		break;
	

	case E_FEATURE_MAX_GV:
		bRes = Compare( (double)tBlobResult.nMaxGV, nSign, dValue);
		break;

	case E_FEATURE_MEAN_GV:
		bRes = Compare( (double)tBlobResult.fMeanGV, nSign, dValue);
		break;

	case E_FEATURE_DIFF_GV:
		bRes = Compare( (double)tBlobResult.fDiffGV, nSign, dValue);
		break;

	case E_FEATURE_BK_GV:
		bRes = Compare( (double)tBlobResult.fBKGV, nSign, dValue);
		break;

	case E_FEATURE_STD_DEV:
		bRes = Compare( (double)tBlobResult.fStdDev, nSign, dValue);
		break;

	case E_FEATURE_SEMU:
		bRes = Compare( (double)tBlobResult.fSEMU, nSign, dValue);
		break;

	case E_FEATURE_COMPACTNESS:
		bRes = Compare( (double)tBlobResult.fCompactness, nSign, dValue);
		break;

	case E_FEATURE_MIN_GV_RATIO:
		bRes = Compare( (double)tBlobResult.nMinGVRatio, nSign, dValue);
		break;

	case E_FEATURE_MAX_GV_RATIO:
		bRes = Compare( (double)tBlobResult.nMaxGVRatio, nSign, dValue);
		break;

	case E_FEATURE_DIFF_GV_RATIO:
		bRes = Compare( (double)tBlobResult.fDiffGVRatio, nSign, dValue);
		break;

	case E_FEATURE_PERIMETER:
		bRes = Compare( (double)tBlobResult.fPerimeter, nSign, dValue);
		break;

	case E_FEATURE_ROUNDNESS:
		bRes = Compare( (double)tBlobResult.fRoundness, nSign, dValue);
		break;

	case E_FEATURE_ELONGATION:
		bRes = Compare( (double)tBlobResult.fElongation, nSign, dValue);
		break;

	case E_FEATURE_MIN_BOX_AREA:
		bRes = Compare( (double)tBlobResult.fMinBoxArea, nSign, dValue);
		break;

	case E_FEATURE_MINOR_AXIS:
		bRes = Compare( (double)tBlobResult.fMinorAxis, nSign, dValue);
		break;

	case E_FEATURE_MAJOR_AXIS:
		bRes = Compare( (double)tBlobResult.fMajorAxis, nSign, dValue);
		break;

	case E_FEATURE_AXIS_RATIO:
		bRes = Compare( (double)tBlobResult.fAxisRatio, nSign, dValue);
		break;

	case E_FEATURE_MIN_BOX_RATIO:
		bRes = Compare( (double)tBlobResult.fMinBoxRatio, nSign, dValue);
		break;

	case E_FEATURE_GV_UP_COUNT_0:
	case E_FEATURE_GV_UP_COUNT_1:
	case E_FEATURE_GV_UP_COUNT_2:
		{
			int nCount	= (int)dValue / 10000;
			int nGV		= (int)dValue % 10000;

			if( nGV < 0 )				nGV = 0;
			if( nGV > IMAGE_MAX_GV )	nGV = IMAGE_MAX_GV-1;

			__int64 nHist = 0;
			for (int m=nGV ; m<IMAGE_MAX_GV ; m++)
				nHist += tBlobResult.nHist[m];

			bRes = Compare( (double)nHist, nSign, (double)nCount);
		}
		break;

	case E_FEATURE_GV_DOWN_COUNT_0:
	case E_FEATURE_GV_DOWN_COUNT_1:
	case E_FEATURE_GV_DOWN_COUNT_2:
		{
			int nCount	= (int)dValue / 10000;
			int nGV		= (int)dValue % 10000;

			if( nGV < 0 )				nGV = 0;
			if( nGV > IMAGE_MAX_GV )	nGV = IMAGE_MAX_GV-1;

			__int64 nHist = 0;
			for (int m=0 ; m<=nGV ; m++)
				nHist += tBlobResult.nHist[m];

			bRes = Compare( (double)nHist, nSign, (double)nCount);
		}
		break;


	case E_FEATURE_MEANAREA_RATIO: //choikwangil
		bRes = Compare((double)tBlobResult.fMeanAreaRatio, nSign, dValue);
		break;

	case E_FEATURE_LAB_DEGREE: //choikwangil
		bRes = Compare((double)tBlobResult.fLab_Degree, nSign, dValue);
		break;
	case E_FEATURE_LAB_LENGHT: //choikwangil
		bRes = Compare((double)tBlobResult.fLab_Lenght, nSign, dValue);
		break;

	case E_FEATURE_LAB_L: //choikwangil
		bRes = Compare((double)tBlobResult.fLab_avg_L, nSign, dValue);
		break;

	case E_FEATURE_LAB_A: //choikwangil
		bRes = Compare((double)tBlobResult.fLab_avg_a, nSign, dValue);
		break;
	case E_FEATURE_LAB_B: //choikwangil
		bRes = Compare((double)tBlobResult.fLab_avg_b, nSign, dValue);
		break;

	case E_FEATURE_MAX_MIN:
		bRes = Compare((double)tBlobResult.fMax_Min, nSign, dValue);
		break;

	case E_FEATURE_RsubC:
		bRes = Compare((double)tBlobResult.fRsubC, nSign, dValue);
		break;

	case E_FEATURE_Mean_Min:
		bRes = Compare((double)tBlobResult.fMean_Min, nSign, dValue);
		break;



	case E_FEATURE_GVAREA_RATIO_TEST: //04.20 choi
	{

		int nTmp = (int)dValue % 10000;
		double nPer = ((double)dValue - (double)nTmp) / 10000.0;
		double nRatio = nTmp / 1000;


		double Mean_GV = tBlobResult.fBKGV * nRatio;


		if (Mean_GV < 0)				Mean_GV = 0;
		if (Mean_GV > IMAGE_MAX_GV)  	Mean_GV = IMAGE_MAX_GV - 1;

		__int64 nHist = 0;
		for (int m = Mean_GV; m <= 255; m++)
			nHist += tBlobResult.nHist[m];

		double Area_per = nHist / tBlobResult.nBoxArea;
		Area_per *= 100;

		bRes = Compare((double)Area_per, nSign, (double)nPer);
	}
	break;

	default:
		bRes = false;
		break;
	}

	return bRes;
}

// �ܰ��� ��ǥ �ѱ��
// P/S ����� ��� : ����Ͽ� ��ǥ ����
// Align ���� ��� �������� �����Ͽ� ��ǥ ����
bool CFeatureExtraction::SaveTxt(int* nCommonPara, wchar_t* strContourTxt, bool bUse)
{
	// �ܰ��� ���� -> AviInspection::JudgeSaveMuraContours() �̵�
	// �ҷ� ���� ��, ǥ�� �ȵǵ���...
	if( !bUse )		return	true;

	// �Ķ���� NULL�� ���.
	if( nCommonPara == NULL )			return false;

	// Blob ���� ���Ѱ�� ����
	if( !m_bComplete )					return false;

	// Blob ��� 0�� ����
	if( m_BlobResult.size() == 0 )		return true;

	// �ܰ��� ���� ��� ���� ���
	if( strContourTxt == NULL )			return false;

	//////////////////////////////////////////////////////////////////////////
	// ���� �Ķ����
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell ȸ���� �߽� y��ǥ
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );

	EnterCriticalSection(&m_csCoordFile);

	// TXT ����
	CStdioFile	fileWriter;
	CString		strLine;

	// ���� ����
	if( fileWriter.Open( strContourTxt, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite ) )
	{
		for (int i=0 ; i<m_BlobResult.size() ; i++)
		{
			// �����Ѱ͸�... ( ���� �ҷ� ���� ���͸�... )
			if( !m_BlobResult[i].bFiltering )	continue;

			// �ܰ��� ����
			for (int j=0 ; j<m_BlobResult[i].ptContours.size() ; j++)
			{
				// ȸ�� ��, ���� ��ǥ ���
				int X = (int)( dCos * (m_BlobResult[i].ptContours[j].x - nCx) - dSin * (m_BlobResult[i].ptContours[j].y - nCy) + nCx );
				int Y = (int)( dSin * (m_BlobResult[i].ptContours[j].x - nCx) + dCos * (m_BlobResult[i].ptContours[j].y - nCy) + nCy );

				// P/S ��忡 ���� ��ǥ ���� ( 1�� ��ǥ�� ���� )
				strLine.Format(_T("%d, %d\n"), (int)X/nPS, (int)Y/nPS);

				fileWriter.SeekToEnd();
				fileWriter.WriteString(strLine);
			}
		}

		// ���� ���� ��츸 �ݱ�
		fileWriter.Close();
	}

	LeaveCriticalSection(&m_csCoordFile);

	return true;
}


bool CFeatureExtraction::GetResultblob(vector<tBLOB_FEATURE>& OutBlob)
{
	// ���� �ʱ�ȭ
	if( OutBlob.size() != 0 )
	{
		for (int i=0 ; i<OutBlob.size() ; i++)
		{
			vector<cv::Point>().swap(OutBlob[i].ptIndexs);
			vector <cv::Point>().swap(m_BlobResult[i].ptContours);
		}
		vector<tBLOB_FEATURE>().swap(OutBlob);
	}

	// Blob ���� ���Ѱ�� ����
	if( !m_bComplete )				return false;

	// Blob ��� 0�� ����
	if( m_BlobResult.size() == 0 )	return true;

	OutBlob.resize( m_BlobResult.size() );

	copy( m_BlobResult.begin(), m_BlobResult.end(), OutBlob.begin() );

	return true;
}

bool CFeatureExtraction::DoFeatureBasic_8bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel, CMatBuf* cMemSub)
{
	// ����� 1���� �ִ� ���
	if( nTotalLabel <= 0 )	return true;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	float fVal = 4.f * PI;

	//m_BlobResult = vector<tBLOB_FEATURE>(nTotalLabel);
	m_BlobResult.resize( nTotalLabel );

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for( int idx=1 ; idx<=nTotalLabel ; idx++ )
	{
		// �ð� ���� Ȯ��
		if( cTimeOut.GetTimeOutFlag() )	continue;

		int nBlobNum = idx - 1;

		m_BlobResult.at(nBlobNum).rectBox.x			= matStats.at<int>(idx, CC_STAT_LEFT);
		m_BlobResult.at(nBlobNum).rectBox.y			= matStats.at<int>(idx, CC_STAT_TOP);
		m_BlobResult.at(nBlobNum).rectBox.width		= matStats.at<int>(idx, CC_STAT_WIDTH);
		m_BlobResult.at(nBlobNum).rectBox.height	= matStats.at<int>(idx, CC_STAT_HEIGHT);

		// ��ü �ֺ� ( ��� GV�뵵 )
		int nOffSet = 10;

		int nSX = m_BlobResult.at(nBlobNum).rectBox.x - nOffSet;
		int nSY = m_BlobResult.at(nBlobNum).rectBox.y - nOffSet;
		int nEX = m_BlobResult.at(nBlobNum).rectBox.x + m_BlobResult.at(nBlobNum).rectBox.width + nOffSet + nOffSet;
		int nEY = m_BlobResult.at(nBlobNum).rectBox.y + m_BlobResult.at(nBlobNum).rectBox.height + nOffSet + nOffSet;

		if( nSX < 0 )	nSX = 0;
		if( nSY < 0 )	nSY = 0;
		if (nSX >= GrayBuffer.cols)		continue;
		if (nSY >= GrayBuffer.rows)		continue;

		if( nEX >= GrayBuffer.cols )	nEX = GrayBuffer.cols - 1;
		if( nEY >= GrayBuffer.rows )	nEY = GrayBuffer.rows - 1;

		cv::Rect rectTemp(nSX, nSY, nEX-nSX+1, nEY-nSY+1);

		__int64 nCount_in	= 0;
		__int64 nCount_out	= 0;
		__int64 nSum_in		= 0;	//	defect ����
		__int64 nSum_out	= 0;	//	defect ���ܿ���

		cv::Mat matTmp_src		= GrayBuffer( rectTemp );		//	������ ROI
		cv::Mat matTmp_label	= matLabel( rectTemp );			//	Label �� ROI
		cv::Mat matTemp			= cMatBufTemp.GetMat(rectTemp.height, rectTemp.width, CV_8UC1);

		for(int  y=0 ; y<rectTemp.height ; y++)
		{
			int		*ptrLabel	= (int *)matTmp_label.ptr(y);
			uchar	*ptrGray	= (uchar *)matTmp_src.ptr(y);
			uchar	*ptrTemp	= (uchar *)matTemp.ptr(y);

			for(int x=0 ; x<rectTemp.width ; x++, ptrLabel++, ptrGray++, ptrTemp++)
			{
				// �ش� ��ü
				if( *ptrLabel == idx )
				{
					nSum_in += *ptrGray;
					nCount_in++;

					// �ش� �� ���Ϳ� �ȼ� ��ǥ ����
					m_BlobResult.at(nBlobNum).ptIndexs.push_back( cv::Point(nSX+x, nSY+y) );

					*ptrTemp = (uchar)255;

					m_BlobResult.at(nBlobNum).nHist[ *ptrGray ]++;
				}
				// �ٸ���� ���
				else
				{
					// �� ��ȣ�� 0�� ���...
					// �ٸ� ��ü �����ɼ� �����Ƿ�.... ���� �߰�
					if( *ptrLabel == 0 )
					{
						nSum_out += *ptrGray;
						nCount_out++;
					}					
				}
			}
		}

		// ��� ���� ��
		m_BlobResult.at(nBlobNum).nSumGV = nSum_in;

		// ��ü ����
		m_BlobResult.at(nBlobNum).nArea = nCount_in;	//matStats.at<int>(idx, CC_STAT_AREA);

		// Box Area
		m_BlobResult.at(nBlobNum).nBoxArea = m_BlobResult.at(nBlobNum).rectBox.width * m_BlobResult.at(nBlobNum).rectBox.height;

		// Bounding Box ���� ���� / ��ü ���� ( Rectangularity(= Extent) )
		m_BlobResult.at(nBlobNum).fBoxRatio	= m_BlobResult.at(nBlobNum).nArea / (float)m_BlobResult.at(nBlobNum).nBoxArea;

		// Elongation ���ϱ�
		m_BlobResult.at(nBlobNum).fElongation = m_BlobResult.at(nBlobNum).rectBox.width / (float)m_BlobResult.at(nBlobNum).rectBox.height;

		// stdDev ���ϱ�
		cv::Scalar m, s;
		cv::meanStdDev(matTmp_src, m, s, matTemp);
		m_BlobResult.at(nBlobNum).fStdDev = float(s[0]);

		// Contours ���ϱ�
		vector<vector<cv::Point>>	ptContours;
		vector<vector<cv::Point>>().swap(ptContours);
		cv::findContours(matTemp, ptContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		// Perimeter ���ϱ�
		if( ptContours.size() != 0 )
		{
			// ROI �����̹Ƿ�, ���� �ʿ�
			// �ܰ� ��ǥ ��� ����
			for (int m=0 ; m<ptContours.size() ; m++)
			{
				for(int k=0 ; k<ptContours.at(m).size() ; k++)
					m_BlobResult.at(nBlobNum).ptContours.push_back( cv::Point(ptContours.at(m)[k].x + nSX, ptContours.at(m)[k].y + nSY) );
			}
		}
		else
		{
			// ���� ��ǥ�̹Ƿ�, ���� ���ص� ��.
			// �ܰ� ��ǥ ��� ����
			m_BlobResult.at(nBlobNum).ptContours.resize( (int)m_BlobResult.at(nBlobNum).ptIndexs.size() );
			std::copy( m_BlobResult.at(nBlobNum).ptIndexs.begin(), m_BlobResult.at(nBlobNum).ptIndexs.end(), m_BlobResult.at(nBlobNum).ptContours.begin() );
		}
		m_BlobResult.at(nBlobNum).fPerimeter = float( cv::arcLength(m_BlobResult.at(nBlobNum).ptContours, true) );
		vector<vector<cv::Point>>().swap(ptContours);

		// Roundness ���ϱ�
		m_BlobResult.at(nBlobNum).fRoundness = (fVal * m_BlobResult.at(nBlobNum).nArea )
			/ (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter);

		// ��ü�� �� ��翡 �󸶳� ����? ( �ѷ�^2 / 4 * Pi * ���� )
		m_BlobResult.at(nBlobNum).fCompactness = (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter)
			/ ( fVal * float( m_BlobResult.at(nBlobNum).nArea ) );

		// Defect GV ���ϱ�
		m_BlobResult.at(nBlobNum).fMeanGV = nSum_in / (float)nCount_in;

		//	��� GV ���ϱ�
		m_BlobResult.at(nBlobNum).fBKGV = nSum_out / (float)nCount_out;

		// GV ���̰� ���ϱ� ( ��� - ��ü )
		m_BlobResult.at(nBlobNum).fDiffGV = m_BlobResult.at(nBlobNum).fBKGV - m_BlobResult.at(nBlobNum).fMeanGV;

		// min, max GV ���ϱ�
		double valMin, valMax;
		cv::minMaxLoc(matTmp_src, &valMin, &valMax, 0, 0, matTemp);
		m_BlobResult.at(nBlobNum).nMinGV = (long)valMin;
		m_BlobResult.at(nBlobNum).nMaxGV = (long)valMax;

		// ��ü �ּ� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).nMinGVRatio = m_BlobResult.at(nBlobNum).nMinGV / m_BlobResult.at(nBlobNum).fBKGV;

		// ��ü �ִ� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).nMaxGVRatio = m_BlobResult.at(nBlobNum).nMaxGV / m_BlobResult.at(nBlobNum).fBKGV;

		//      ��� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).fDiffGVRatio = m_BlobResult.at(nBlobNum).fMeanGV / m_BlobResult.at(nBlobNum).fBKGV;

		// Center Point ���ϱ�
		m_BlobResult.at(nBlobNum).ptCenter.x = (int)matCentroid.at<double>(idx, 0);
		m_BlobResult.at(nBlobNum).ptCenter.y = (int)matCentroid.at<double>(idx, 1);

		// SEMU ���ϱ�
		if( m_BlobResult.at(nBlobNum).fDiffGV == 0.0 )
		{
			if( m_BlobResult.at(nBlobNum).fBKGV == 0 )
			{
				m_BlobResult.at(nBlobNum).fSEMU = 1.0
					/ ( 1.97f / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
			else
			{
				m_BlobResult.at(nBlobNum).fSEMU = (0.000001 / m_BlobResult.at(nBlobNum).fBKGV)
					/ ( 1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
		}
		else
		{
			if( m_BlobResult.at(nBlobNum).fBKGV == 0 )
			{
				m_BlobResult.at(nBlobNum).fSEMU = (fabs(m_BlobResult.at(nBlobNum).fMeanGV - m_BlobResult.at(nBlobNum).fBKGV) / 0.000001)
					/ ( 1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
			else
			{
				m_BlobResult.at(nBlobNum).fSEMU = (fabs(m_BlobResult.at(nBlobNum).fMeanGV - m_BlobResult.at(nBlobNum).fBKGV) / m_BlobResult.at(nBlobNum).fBKGV )
					/ ( 1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
		}

		cv::RotatedRect BoundingBox = cv::minAreaRect(m_BlobResult.at(nBlobNum).ptIndexs);

		// ȸ���� �簢�� ������ 4��
		//cv::Point2f vertices[4];
		//BoundingBox.points(vertices);

		// Box width and length
		m_BlobResult.at(nBlobNum).BoxSize		= BoundingBox.size;

		// Angle between the horizontal axis
		m_BlobResult.at(nBlobNum).fAngle		= BoundingBox.angle;

		// Minor Axis & Major Axis
		if( BoundingBox.size.width > BoundingBox.size.height )
		{
			m_BlobResult.at(nBlobNum).fMinorAxis	= BoundingBox.size.width;
			m_BlobResult.at(nBlobNum).fMajorAxis	= BoundingBox.size.height;
		}
		else
		{
			m_BlobResult.at(nBlobNum).fMinorAxis	= BoundingBox.size.height;
			m_BlobResult.at(nBlobNum).fMajorAxis	= BoundingBox.size.width;
		}

		// Feret��s area
		m_BlobResult.at(nBlobNum).fMinBoxArea = m_BlobResult.at(nBlobNum).fMinorAxis * m_BlobResult.at(nBlobNum).fMajorAxis;

		// Axis Ratio
		if( m_BlobResult.at(nBlobNum).fMajorAxis > 0 )
			m_BlobResult.at(nBlobNum).fAxisRatio		= m_BlobResult.at(nBlobNum).fMinorAxis / m_BlobResult.at(nBlobNum).fMajorAxis;
		else
			m_BlobResult.at(nBlobNum).fAxisRatio		= 0.f;

		// Min Bounding Box ���� ���� / ��ü ���� ( Area porosity )
		m_BlobResult.at(nBlobNum).fMinBoxRatio	= m_BlobResult.at(nBlobNum).fMinBoxArea / (float)m_BlobResult.at(nBlobNum).nArea;
		//choikwangil
		m_BlobResult.at(nBlobNum).fMeanAreaRatio = m_BlobResult.at(nBlobNum).fMeanGV / (float)m_BlobResult.at(nBlobNum).nArea;
		// �Ҵ� ����
		matTmp_src.release();
		matTmp_label.release();
		matTemp.release();
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}

	// �ð� ���� Ȯ��
	if( cTimeOut.GetTimeOutFlag() )	return false;

	return true;
}

bool CFeatureExtraction::DoFeatureBasic_8bit_LCP(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer,cv::Mat& ColorBuffer, double* dPara, int nTotalLabel, CMatBuf* cMemSub)
{
	// ����� 1���� �ִ� ���
	if (nTotalLabel <= 0)	return true;

	//////////////////////////////////////////////////////////////////////////
	//dpara
	//Red Spec1
	int dRed_Spec1_Flag = (int)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_FLAG];

	double dRed_Spec1_Degree_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_DEGREE_MIN];
	double dRed_Spec1_Degree_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_DEGREE_MAX];
	double dRed_Spec1_Lenght_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_LENGHT_MIN];
	double dRed_Spec1_Lenght_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_LENGHT_MAX];
	double dRed_Spec1_L_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_L_MIN];
	double dRed_Spec1_L_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_L_MAX];

	//Red Spec2
	int dRed_Spec2_Flag = (int)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_FLAG];
	
	double dRed_Spec2_Degree_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_DEGREE_MIN];
	double dRed_Spec2_Degree_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_DEGREE_MAX];
	double dRed_Spec2_Lenght_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_LENGHT_MIN];
	double dRed_Spec2_Lenght_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_LENGHT_MAX];
	double dRed_Spec2_L_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_L_MIN];
	double dRed_Spec2_L_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_L_MAX];


	//Blue Spec1
	int dBlue_Spec1_Flag = (int)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_FLAG];

	double dBlue_Spec1_Degree_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_DEGREE_MIN];
	double dBlue_Spec1_Degree_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_DEGREE_MAX];
	double dBlue_Spec1_Lenght_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_LENGHT_MIN];
	double dBlue_Spec1_Lenght_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_LENGHT_MAX];
	double dBlue_Spec1_L_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_L_MIN];
	double dBlue_Spec1_L_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_L_MAX];

	//Blue Spec2
	int dBlue_Spec2_Flag = (int)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_FLAG];

	double dBlue_Spec2_Degree_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_DEGREE_MIN];
	double dBlue_Spec2_Degree_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_DEGREE_MAX];
	double dBlue_Spec2_Lenght_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_LENGHT_MIN];
	double dBlue_Spec2_Lenght_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_LENGHT_MAX];
	double dBlue_Spec2_L_Min = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_L_MIN];
	double dBlue_Spec2_L_Max = (double)dPara[E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_L_MAX];


	//////////////////////////////////////////////////////////////////////////
	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	float fVal = 4.f * PI;

	//m_BlobResult = vector<tBLOB_FEATURE>(nTotalLabel);
	m_BlobResult.resize(nTotalLabel);

	//////////////////////////////////////////////////////////////////////////
	//LAB �˻� ���
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matThresh2 = cv::Mat::zeros(GrayBuffer.size(), CV_8UC1);

	cv::Mat matRGB[3];

	cv::Mat matDefectBuff_Color2 = ColorBuffer.clone();

	split(matDefectBuff_Color2, matRGB);

	for (int i = 0; i < 3; i++)
	{
		cv::GaussianBlur(matRGB[i], matRGB[i], cv::Size(31, 31), 1.0);

		// �̹����� 1/5 ũ��� ROI �����
		int nROIWidth = matDefectBuff_Color2.cols / 5 / 2;
		int nROIHeight = matDefectBuff_Color2.rows / 5 / 2;

		Rect rectRGBROI;
		rectRGBROI.x = matDefectBuff_Color2.cols / 2 - nROIWidth;
		rectRGBROI.y = matDefectBuff_Color2.rows / 2 - nROIHeight;
		rectRGBROI.width = nROIWidth * 2;
		rectRGBROI.height = nROIHeight * 2;

		cv::Mat matMask = matRGB[i](rectRGBROI).clone();
		cv::Mat matMask2 = matRGB[i](rectRGBROI).clone();
		// �� �� ����� ���ϴµ� ���صǴ� ��� ���� �� �����Ƿ� ����
		cv::threshold(matMask, matMask, 200, 255, THRESH_BINARY_INV);
		cv::threshold(matMask2, matMask2, 100, 255, THRESH_BINARY);

		cv::bitwise_and(matMask, matMask2, matMask);

		double dMean = cv::mean(matRGB[i](rectRGBROI), matMask)[0];

		int nValue = 126 - dMean;

		matRGB[i] += nValue;
	}

	cv::merge(matRGB, 3, matDefectBuff_Color2);
	//cv::imwrite("D:\\�̹��� ����.jpg", matDefectBuff_Color2);

	//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for( int idx=1 ; idx<=nTotalLabel ; idx++ )
	{
		// �ð� ���� Ȯ��
		if( cTimeOut.GetTimeOutFlag() )	continue;

		int nBlobNum = idx - 1;

		m_BlobResult.at(nBlobNum).rectBox.x			= matStats.at<int>(idx, CC_STAT_LEFT);
		m_BlobResult.at(nBlobNum).rectBox.y			= matStats.at<int>(idx, CC_STAT_TOP);
		m_BlobResult.at(nBlobNum).rectBox.width		= matStats.at<int>(idx, CC_STAT_WIDTH);
		m_BlobResult.at(nBlobNum).rectBox.height	= matStats.at<int>(idx, CC_STAT_HEIGHT);

		// ��ü �ֺ� ( ��� GV�뵵 )
		int nOffSet = 20;

		int nSX = m_BlobResult.at(nBlobNum).rectBox.x - nOffSet;
		int nSY = m_BlobResult.at(nBlobNum).rectBox.y - nOffSet;
		int nEX = m_BlobResult.at(nBlobNum).rectBox.x + m_BlobResult.at(nBlobNum).rectBox.width + nOffSet;
		int nEY = m_BlobResult.at(nBlobNum).rectBox.y + m_BlobResult.at(nBlobNum).rectBox.height + nOffSet;

		if (nSX < 0)	nSX = 0;
		if (nSY < 0)	nSY = 0;
		if (nSX >= GrayBuffer.cols)		continue;
		if (nSY >= GrayBuffer.rows)		continue;

		if (nEX >= GrayBuffer.cols)	nEX = GrayBuffer.cols - 1;
		if (nEY >= GrayBuffer.rows)	nEY = GrayBuffer.rows - 1;

		cv::Rect rectTemp(nSX, nSY, nEX-nSX+1, nEY-nSY+1);

		__int64 nCount_in	= 0;
		__int64 nCount_out	= 0;
		__int64 nSum_in		= 0;	//	defect ����
		__int64 nSum_out	= 0;	//	defect ���ܿ���

		cv::Mat matTmp_src		= GrayBuffer( rectTemp );		//	������ ROI
		cv::Mat matTmp_label	= matLabel( rectTemp );			//	Label �� ROI
		cv::Mat matTemp			= cMatBufTemp.GetMat(rectTemp.height, rectTemp.width, CV_8UC1);

		for(int  y=0 ; y<rectTemp.height ; y++)
		{
			int		*ptrLabel = (int *)matTmp_label.ptr(y);
			uchar	*ptrGray = (uchar *)matTmp_src.ptr(y);
			uchar	*ptrTemp = (uchar *)matTemp.ptr(y);

			for(int x=0 ; x<rectTemp.width ; x++, ptrLabel++, ptrGray++, ptrTemp++)
			{
				// �ش� ��ü
				if( *ptrLabel == idx )
				{
					nSum_in += *ptrGray;
					nCount_in++;

					// �ش� �� ���Ϳ� �ȼ� ��ǥ ����
					m_BlobResult.at(nBlobNum).ptIndexs.push_back( cv::Point(nSX+x, nSY+y) );

					*ptrTemp = (uchar)255;

					m_BlobResult.at(nBlobNum).nHist[*ptrGray]++;
				}
				// �ٸ���� ���
				else
				{
					// �� ��ȣ�� 0�� ���...
					// �ٸ� ��ü �����ɼ� �����Ƿ�.... ���� �߰�
					if( *ptrLabel == 0 )
					{
						nSum_out += *ptrGray;
						nCount_out++;
					}
				}
			}
		}

		// ��� ���� ��
		m_BlobResult.at(nBlobNum).nSumGV = nSum_in;

		// ��ü ����
		m_BlobResult.at(nBlobNum).nArea = nCount_in;	//matStats.at<int>(idx, CC_STAT_AREA);

		// Box Area
		m_BlobResult.at(nBlobNum).nBoxArea = m_BlobResult.at(nBlobNum).rectBox.width * m_BlobResult.at(nBlobNum).rectBox.height;

		// Bounding Box ���� ���� / ��ü ���� ( Rectangularity(= Extent) )
		m_BlobResult.at(nBlobNum).fBoxRatio	= m_BlobResult.at(nBlobNum).nArea / (float)m_BlobResult.at(nBlobNum).nBoxArea;

		// Elongation ���ϱ�
		m_BlobResult.at(nBlobNum).fElongation = m_BlobResult.at(nBlobNum).rectBox.width / (float)m_BlobResult.at(nBlobNum).rectBox.height;

		// stdDev ���ϱ�
		cv::Scalar m, s;
		cv::meanStdDev(matTmp_src, m, s, matTemp);
		m_BlobResult.at(nBlobNum).fStdDev = float(s[0]);

		// Contours ���ϱ�
		vector<vector<cv::Point>>	ptContours;
		vector<vector<cv::Point>>().swap(ptContours);
		cv::findContours(matTemp, ptContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		// Perimeter ���ϱ�
		if( ptContours.size() != 0 )
		{
			// ROI �����̹Ƿ�, ���� �ʿ�
			// �ܰ� ��ǥ ��� ����
			for (int m=0 ; m<ptContours.size() ; m++)
			{
				for(int k=0 ; k<ptContours.at(m).size() ; k++)
					m_BlobResult.at(nBlobNum).ptContours.push_back( cv::Point(ptContours.at(m)[k].x + nSX, ptContours.at(m)[k].y + nSY) );
			}
		}
		else
		{
			// ���� ��ǥ�̹Ƿ�, ���� ���ص� ��.
			// �ܰ� ��ǥ ��� ����
			m_BlobResult.at(nBlobNum).ptContours.resize( (int)m_BlobResult.at(nBlobNum).ptIndexs.size() );
			std::copy( m_BlobResult.at(nBlobNum).ptIndexs.begin(), m_BlobResult.at(nBlobNum).ptIndexs.end(), m_BlobResult.at(nBlobNum).ptContours.begin() );
		}
		m_BlobResult.at(nBlobNum).fPerimeter = float( cv::arcLength(m_BlobResult.at(nBlobNum).ptContours, true) );
		vector<vector<cv::Point>>().swap(ptContours);

		// Roundness ���ϱ�
		m_BlobResult.at(nBlobNum).fRoundness = (fVal * m_BlobResult.at(nBlobNum).nArea )
			/ (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter);

		// ��ü�� �� ��翡 �󸶳� ����? ( �ѷ�^2 / 4 * Pi * ���� )
		m_BlobResult.at(nBlobNum).fCompactness = (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter)
			/ ( fVal * float( m_BlobResult.at(nBlobNum).nArea ) );

		// Defect GV ���ϱ�
		m_BlobResult.at(nBlobNum).fMeanGV = nSum_in / (float)nCount_in;

		//	��� GV ���ϱ�
		m_BlobResult.at(nBlobNum).fBKGV = nSum_out / (float)nCount_out;

		// GV ���̰� ���ϱ� ( ��� - ��ü )
		m_BlobResult.at(nBlobNum).fDiffGV = m_BlobResult.at(nBlobNum).fBKGV - m_BlobResult.at(nBlobNum).fMeanGV;

		// min, max GV ���ϱ�
		double valMin, valMax;
		cv::minMaxLoc(matTmp_src, &valMin, &valMax, 0, 0, matTemp);
		m_BlobResult.at(nBlobNum).nMinGV = (long)valMin;
		m_BlobResult.at(nBlobNum).nMaxGV = (long)valMax;

		// ��ü �ּ� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).nMinGVRatio = m_BlobResult.at(nBlobNum).nMinGV / m_BlobResult.at(nBlobNum).fBKGV;

		// ��ü �ִ� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).nMaxGVRatio = m_BlobResult.at(nBlobNum).nMaxGV / m_BlobResult.at(nBlobNum).fBKGV;

		//      ��� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).fDiffGVRatio = m_BlobResult.at(nBlobNum).fMeanGV / m_BlobResult.at(nBlobNum).fBKGV;

		// Center Point ���ϱ�
		m_BlobResult.at(nBlobNum).ptCenter.x = (int)matCentroid.at<double>(idx, 0);
		m_BlobResult.at(nBlobNum).ptCenter.y = (int)matCentroid.at<double>(idx, 1);

		// SEMU ���ϱ�
		if( m_BlobResult.at(nBlobNum).fDiffGV == 0.0 )
		{
			if( m_BlobResult.at(nBlobNum).fBKGV == 0 )
			{
				m_BlobResult.at(nBlobNum).fSEMU = 1.0
					/ ( 1.97f / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
			else
			{
				m_BlobResult.at(nBlobNum).fSEMU = (0.000001 / m_BlobResult.at(nBlobNum).fBKGV)
					/ ( 1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
		}
		else
		{
			if( m_BlobResult.at(nBlobNum).fBKGV == 0 )
			{
				m_BlobResult.at(nBlobNum).fSEMU = (fabs(m_BlobResult.at(nBlobNum).fMeanGV - m_BlobResult.at(nBlobNum).fBKGV) / 0.000001)
					/ ( 1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
			else
			{
				m_BlobResult.at(nBlobNum).fSEMU = (fabs(m_BlobResult.at(nBlobNum).fMeanGV - m_BlobResult.at(nBlobNum).fBKGV) / m_BlobResult.at(nBlobNum).fBKGV )
					/ ( 1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
		}

		cv::RotatedRect BoundingBox = cv::minAreaRect(m_BlobResult.at(nBlobNum).ptIndexs);

	//	cv::Point2f varPoint[E_CORNER_END];
	//	BoundingBox.points(varPoint);

	//	varPoint[0].x - varPoint[1].x
		
		// ȸ���� �簢�� ������ 4��
		//cv::Point2f vertices[4];
		//BoundingBox.points(vertices);

		// Box width and length
		m_BlobResult.at(nBlobNum).BoxSize		= BoundingBox.size;

		// Angle between the horizontal axis
		m_BlobResult.at(nBlobNum).fAngle		= BoundingBox.angle;

		// minAreaRect�� ��� ������ ���� Width�� Height�� ����Ǵ� ������ �־�
		// ����� ��������� �����ϴ� ���� ����
		// ������ �̻��ϱ��ѵ�.. �̰� ���� ������� �ʾƼ� ���߿� ���濹��
		// ���� blog https://namkeenman.wordpress.com/tag/minarearect/
		// CIH 20200425 
		
		if (abs(m_BlobResult.at(nBlobNum).rectBox.width - BoundingBox.size.width) > abs(m_BlobResult.at(nBlobNum).rectBox.width - BoundingBox.size.height))
		{
			float fTemp = BoundingBox.size.width;
			BoundingBox.size.width = BoundingBox.size.height;
			BoundingBox.size.height = fTemp;
		}
		
		// Minor Axis & Major Axis
		if( BoundingBox.size.width > BoundingBox.size.height )
		{
			m_BlobResult.at(nBlobNum).fMinorAxis	= BoundingBox.size.width ;
			m_BlobResult.at(nBlobNum).fMajorAxis	= BoundingBox.size.height ;
		}
		else
		{
			m_BlobResult.at(nBlobNum).fMinorAxis	= BoundingBox.size.height ;
			m_BlobResult.at(nBlobNum).fMajorAxis	= BoundingBox.size.width ;
		}

		// Feret��s area
		m_BlobResult.at(nBlobNum).fMinBoxArea = m_BlobResult.at(nBlobNum).fMinorAxis * m_BlobResult.at(nBlobNum).fMajorAxis;

		// Axis Ratio
		if( m_BlobResult.at(nBlobNum).fMajorAxis > 0 )
			m_BlobResult.at(nBlobNum).fAxisRatio		= m_BlobResult.at(nBlobNum).fMinorAxis / m_BlobResult.at(nBlobNum).fMajorAxis;
		else
			m_BlobResult.at(nBlobNum).fAxisRatio		= 0.f;

		// Min Bounding Box ���� ���� / ��ü ���� ( Area porosity )
		m_BlobResult.at(nBlobNum).fMinBoxRatio	= ((float)m_BlobResult.at(nBlobNum).nArea / (float)m_BlobResult.at(nBlobNum).fMinBoxArea) *100;
		//choikwangil
		m_BlobResult.at(nBlobNum).fMeanAreaRatio = m_BlobResult.at(nBlobNum).fMeanGV / (float)m_BlobResult.at(nBlobNum).nArea;
		
		// �ҷ� �ִ� �ּҰ� ����
		m_BlobResult.at(nBlobNum).fMax_Min = m_BlobResult.at(nBlobNum).nMaxGV - m_BlobResult.at(nBlobNum).nMinGV;

		m_BlobResult.at(nBlobNum).fRsubC = abs(m_BlobResult.at(nBlobNum).fRoundness - m_BlobResult.at(nBlobNum).fCompactness);

		m_BlobResult.at(nBlobNum).fMean_Min = m_BlobResult.at(nBlobNum).fMeanGV - (float)m_BlobResult.at(nBlobNum).nMinGV;


		
		
		//////////////////////////////////////////////////////////////////////////
		int nOffSet_Color = 1;			// �ҷ����� ũ�� ROI�� �����Ҷ� �� Pixel �� ũ�� ������ �������ִ� ��

		Rect rectROI;

		// ���� x��ǥ
		rectROI.x = m_BlobResult.at(nBlobNum).rectBox.x - nOffSet_Color;
		rectROI.y = m_BlobResult.at(nBlobNum).rectBox.y - nOffSet_Color;
		rectROI.width = m_BlobResult.at(nBlobNum).rectBox.width + nOffSet_Color * 2;
		rectROI.height = m_BlobResult.at(nBlobNum).rectBox.height + nOffSet_Color * 2;

		if (rectROI.x < 0) rectROI.x = 0;
		if (rectROI.y < 0) rectROI.y = 0;
		if (rectROI.width + rectROI.x > matDefectBuff_Color2.cols) rectROI.width = (matDefectBuff_Color2.cols)-rectROI.x;
		if (rectROI.height + rectROI.y > matDefectBuff_Color2.rows) rectROI.height = (matDefectBuff_Color2.rows)-rectROI.y;
		// �ش� ��ġ �̹��� �߶󳻱�
		cv::Mat matDefectBuff_Color = matDefectBuff_Color2(rectROI).clone();			// �ش� �κ� �޸𸮸� ���Ҵ� �ؾ� �ϴ°�??
		cv::Mat matGray = GrayBuffer(rectROI).clone();								// Gray ����
		cv::Mat matDefectBuff_Thresh = matThresh2(rectROI);

		double dMeanGV = m_BlobResult.at(nBlobNum).fMeanGV;


		cv::threshold(matGray, matDefectBuff_Thresh, dMeanGV - 5, 255, THRESH_BINARY_INV);



		// �̹��� Lab ��ȯ -> Vector ������ �̿��ϸ� matLab ��� ���ص� �ɼ� ���� �޸𸮸� �����ؼ� �����غ�����
		cv::Mat matLab[3];

		matDefectBuff_Color.convertTo(matDefectBuff_Color, CV_32FC3);			// float ������ ��ȯ
		matDefectBuff_Color *= 1.0 / 255.0;										// �Ҽ������� ��ȯ

		cv::cvtColor(matDefectBuff_Color, matDefectBuff_Color, CV_BGR2Lab);		// Lab ��ȯ
		cv::split(matDefectBuff_Color, matLab);									// Lab �и�



																				// �ҷ��� ��� Lab�� ���ϱ�
		double dMeanL = cv::mean(matLab[0], matDefectBuff_Thresh)[0];
		double dMeanA = cv::mean(matLab[1], matDefectBuff_Thresh)[0];
		double dMeanB = cv::mean(matLab[2], matDefectBuff_Thresh)[0];


		// ������
		double Deg = atan2f(dMeanB, dMeanA) * 180 / 3.141592;
		String strColorName;

		double dLangth = sqrt(pow(dMeanA, 2) + pow(dMeanB, 2));

// 		if (Deg > -15 && Deg < 90 && dLangth > 10) // if (Deg > -15 && Deg < 85 && dLangth>10)
// 			strColorName = "Red";
// 
// 		else if (Deg<-50 && Deg>-180 && dLangth > 10)
// 			strColorName = "Blue";

// 		if (Deg > -68.0 && Deg < 93.0 && dLangth > 6.0 && dMeanL >= 42.0 && dMeanA > 0.0) 
// 			strColorName = "Red";
// 
// 		else if (Deg > -60.0 && Deg < 90.0 && dLangth > 8.0 && dMeanL < 42.0 && dMeanA > 0.0)
// 			strColorName = "Red";
// 
// 		else if (Deg<-80.0 && Deg>-180.0 && dLangth > 6.0 && dMeanL > 40.0)
// 			strColorName = "Blue";
// 
// 		else
// 			strColorName = "Dark";

		if (dRed_Spec1_Flag > 0 &&( Deg >= dRed_Spec1_Degree_Min && Deg < dRed_Spec1_Degree_Max) && (dLangth >= dRed_Spec1_Lenght_Min && dLangth < dRed_Spec1_Lenght_Max) && (dMeanL >= dRed_Spec1_L_Min && dMeanL < dRed_Spec1_L_Max) && dMeanA > 0.0)
			strColorName = "Red";

		else if (dRed_Spec2_Flag > 0 && (Deg >= dRed_Spec2_Degree_Min && Deg < dRed_Spec2_Degree_Max) && (dLangth >= dRed_Spec2_Lenght_Min && dLangth < dRed_Spec2_Lenght_Max) && (dMeanL >= dRed_Spec2_L_Min && dMeanL < dRed_Spec2_L_Max) && dMeanA > 0.0)
			strColorName = "Red";

		else if (dBlue_Spec1_Flag >0 && (Deg >= dBlue_Spec1_Degree_Min && Deg < dBlue_Spec1_Degree_Max) && (dLangth >= dBlue_Spec1_Lenght_Min && dLangth < dBlue_Spec1_Lenght_Max) && (dMeanL >= dBlue_Spec1_L_Min && dMeanL < dBlue_Spec1_L_Max))
			strColorName = "Blue";

		else if (dBlue_Spec2_Flag > 0 && (Deg >= dBlue_Spec2_Degree_Min && Deg < dBlue_Spec2_Degree_Max) && (dLangth >= dBlue_Spec2_Lenght_Min && dLangth < dBlue_Spec2_Lenght_Max) && (dMeanL >= dBlue_Spec2_L_Min && dMeanL < dBlue_Spec2_L_Max))
			strColorName = "Blue";

		else
			strColorName = "Dark";

		//string *matString;

		//matString[1] = strColorName;
		m_BlobResult.at(nBlobNum).fLab_Degree = Deg;
		m_BlobResult.at(nBlobNum).fLab_Lenght = dLangth;
		m_BlobResult.at(nBlobNum).fLab_avg_L = dMeanL;
		m_BlobResult.at(nBlobNum).fLab_avg_a = dMeanA;
		m_BlobResult.at(nBlobNum).fLab_avg_b = dMeanB;
		m_BlobResult.at(nBlobNum).strColor = strColorName;
		//////////////////////////////////////////////////////////////////////////
		
		// �Ҵ� ����
		matTmp_src.release();
		matTmp_label.release();
		matTemp.release();
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}

	// �ð� ���� Ȯ��
	if (cTimeOut.GetTimeOutFlag())	return false;

	return true;
}


bool CFeatureExtraction::DoFeatureBasic_16bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel, CMatBuf* cMemSub)
{
	// ����� 1���� �ִ� ���
	if( nTotalLabel <= 0 )	return true;

	CMatBuf cMatBufTemp;
	cMatBufTemp.SetMem(cMemSub);

	float fVal = 4.f * PI;

	//m_BlobResult = vector<tBLOB_FEATURE>(nTotalLabel);
	m_BlobResult.resize( nTotalLabel );

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for( int idx=1 ; idx<=nTotalLabel ; idx++ )
	{
		// �ð� ���� Ȯ��
		if( cTimeOut.GetTimeOutFlag() )	continue;

		int nBlobNum = idx - 1;

		m_BlobResult.at(nBlobNum).rectBox.x			= matStats.at<int>(idx, CC_STAT_LEFT);
		m_BlobResult.at(nBlobNum).rectBox.y			= matStats.at<int>(idx, CC_STAT_TOP);
		m_BlobResult.at(nBlobNum).rectBox.width		= matStats.at<int>(idx, CC_STAT_WIDTH);
		m_BlobResult.at(nBlobNum).rectBox.height	= matStats.at<int>(idx, CC_STAT_HEIGHT);

		// ��ü �ֺ� ( ��� GV�뵵 )
		int nOffSet = 20;

		int nSX = m_BlobResult.at(nBlobNum).rectBox.x - nOffSet;
		int nSY = m_BlobResult.at(nBlobNum).rectBox.y - nOffSet;
		int nEX = m_BlobResult.at(nBlobNum).rectBox.x + m_BlobResult.at(nBlobNum).rectBox.width + nOffSet + nOffSet;
		int nEY = m_BlobResult.at(nBlobNum).rectBox.y + m_BlobResult.at(nBlobNum).rectBox.height + nOffSet + nOffSet;

		if( nSX < 0 )	nSX = 0;
		if( nSY < 0 )	nSY = 0;
		if( nEX >= GrayBuffer.cols )	nEX = GrayBuffer.cols - 1;
		if( nEY >= GrayBuffer.rows )	nEY = GrayBuffer.rows - 1;

		cv::Rect rectTemp(nSX, nSY, nEX-nSX+1, nEY-nSY+1);

		__int64 nCount_in	= 0;
		__int64 nCount_out	= 0;
		__int64 nSum_in		= 0;	//	defect ����
		__int64 nSum_out	= 0;	//	defect ���ܿ���

		cv::Mat matTmp_src		= GrayBuffer( rectTemp );		//	������ ROI
		cv::Mat matTmp_label	= matLabel( rectTemp );			//	Label �� ROI
		cv::Mat matTemp			= cMatBufTemp.GetMat(rectTemp.height, rectTemp.width, CV_8UC1);

		for(int  y=0 ; y<rectTemp.height ; y++)
		{
			int		*ptrLabel	= (int *)matTmp_label.ptr(y);
			ushort	*ptrGray	= (ushort *)matTmp_src.ptr(y);
			uchar	*ptrTemp	= (uchar *)matTemp.ptr(y);

			for(int x=0 ; x<rectTemp.width ; x++, ptrLabel++, ptrGray++, ptrTemp++)
			{
				// �ش� ��ü
				if( *ptrLabel == idx )
				{
					nSum_in += *ptrGray;
					nCount_in++;

					// �ش� �� ���Ϳ� �ȼ� ��ǥ ����
					m_BlobResult.at(nBlobNum).ptIndexs.push_back( cv::Point(nSX+x, nSY+y) );

					*ptrTemp = (uchar)255;

					m_BlobResult.at(nBlobNum).nHist[ (int)(*ptrGray) ]++;					
				}
				// �ٸ���� ���
				else
				{
					// �� ��ȣ�� 0�� ���...
					// �ٸ� ��ü �����ɼ� �����Ƿ�.... ���� �߰�
					if( *ptrLabel == 0 )
					{
						nSum_out += *ptrGray;
						nCount_out++;
					}
				}
			}
		}

		// ��� ���� ��
		m_BlobResult.at(nBlobNum).nSumGV = nSum_in;

		// ��ü ����
		m_BlobResult.at(nBlobNum).nArea = nCount_in;	//matStats.at<int>(idx, CC_STAT_AREA);

		// Box Area
		m_BlobResult.at(nBlobNum).nBoxArea = m_BlobResult.at(nBlobNum).rectBox.width * m_BlobResult.at(nBlobNum).rectBox.height;

		// Bounding Box ���� ���� / ��ü ���� ( Rectangularity(= Extent) )
		m_BlobResult.at(nBlobNum).fBoxRatio	= m_BlobResult.at(nBlobNum).nArea / (float)m_BlobResult.at(nBlobNum).nBoxArea;

		// Elongation ���ϱ�
		m_BlobResult.at(nBlobNum).fElongation = m_BlobResult.at(nBlobNum).rectBox.width / (float)m_BlobResult.at(nBlobNum).rectBox.height;

		// stdDev ���ϱ�
		cv::Scalar m, s;
		cv::meanStdDev(matTmp_src, m, s, matTemp);
		m_BlobResult.at(nBlobNum).fStdDev = float(s[0]);

		// Contours ���ϱ�
		vector<vector<cv::Point>>	ptContours;
		vector<vector<cv::Point>>().swap(ptContours);
		cv::findContours(matTemp, ptContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		// Perimeter ���ϱ�
		if( ptContours.size() != 0 )
		{
			// ROI �����̹Ƿ�, ���� �ʿ�
			// �ܰ� ��ǥ ��� ����
			for (int m=0 ; m<ptContours.size() ; m++)
			{
				for(int k=0 ; k<ptContours.at(m).size() ; k++)
					m_BlobResult.at(nBlobNum).ptContours.push_back( cv::Point(ptContours.at(m)[k].x + nSX, ptContours.at(m)[k].y + nSY) );
			}
		}
		else
		{
			// ���� ��ǥ�̹Ƿ�, ���� ���ص� ��.
			// �ܰ� ��ǥ ��� ����
			m_BlobResult.at(nBlobNum).ptContours.resize( (int)m_BlobResult.at(nBlobNum).ptIndexs.size() );
			std::copy( m_BlobResult.at(nBlobNum).ptIndexs.begin(), m_BlobResult.at(nBlobNum).ptIndexs.end(), m_BlobResult.at(nBlobNum).ptContours.begin() );
		}
		m_BlobResult.at(nBlobNum).fPerimeter = float( cv::arcLength(m_BlobResult.at(nBlobNum).ptContours, true) );
		vector<vector<cv::Point>>().swap(ptContours);

		// Roundness ���ϱ�
		m_BlobResult.at(nBlobNum).fRoundness = (fVal * m_BlobResult.at(nBlobNum).nArea )
			/ (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter);

		// ��ü�� �� ��翡 �󸶳� ����? ( �ѷ�^2 / 4 * Pi * ���� )
		m_BlobResult.at(nBlobNum).fCompactness = (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter)
			/ ( fVal * float( m_BlobResult.at(nBlobNum).nArea ) );

		// Defect GV ���ϱ�
		m_BlobResult.at(nBlobNum).fMeanGV = nSum_in / (float)nCount_in;

		//	��� GV ���ϱ�
		m_BlobResult.at(nBlobNum).fBKGV = nSum_out / (float)nCount_out;

		// GV ���̰� ���ϱ� ( ��� - ��ü )
		m_BlobResult.at(nBlobNum).fDiffGV = m_BlobResult.at(nBlobNum).fBKGV - m_BlobResult.at(nBlobNum).fMeanGV;

		// min, max GV ���ϱ�
		double valMin, valMax;
		cv::minMaxLoc(matTmp_src, &valMin, &valMax, 0, 0, matTemp);
		m_BlobResult.at(nBlobNum).nMinGV = (long)valMin;
		m_BlobResult.at(nBlobNum).nMaxGV = (long)valMax;

		// ��ü �ּ� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).nMinGVRatio = m_BlobResult.at(nBlobNum).nMinGV / m_BlobResult.at(nBlobNum).fBKGV;

		// ��ü �ִ� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).nMaxGVRatio = m_BlobResult.at(nBlobNum).nMaxGV / m_BlobResult.at(nBlobNum).fBKGV;

		//      ��� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).fDiffGVRatio = m_BlobResult.at(nBlobNum).fMeanGV / m_BlobResult.at(nBlobNum).fBKGV;

		// Center Point ���ϱ�
		m_BlobResult.at(nBlobNum).ptCenter.x = (int)matCentroid.at<double>(idx, 0);
		m_BlobResult.at(nBlobNum).ptCenter.y = (int)matCentroid.at<double>(idx, 1);

		// SEMU ���ϱ�
		if( m_BlobResult.at(nBlobNum).fDiffGV == 0.0 )
		{
			if( m_BlobResult.at(nBlobNum).fBKGV == 0 )
			{
				m_BlobResult.at(nBlobNum).fSEMU = 1.0
					/ ( 1.97f / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
			else
			{
				m_BlobResult.at(nBlobNum).fSEMU = (0.000001 / m_BlobResult.at(nBlobNum).fBKGV)
					/ ( 1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
		}
		else
		{
			if( m_BlobResult.at(nBlobNum).fBKGV == 0 )
			{
				m_BlobResult.at(nBlobNum).fSEMU = (fabs(m_BlobResult.at(nBlobNum).fMeanGV - m_BlobResult.at(nBlobNum).fBKGV) / 0.000001)
					/ ( 1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
			else
			{
				m_BlobResult.at(nBlobNum).fSEMU = (fabs(m_BlobResult.at(nBlobNum).fMeanGV - m_BlobResult.at(nBlobNum).fBKGV) / m_BlobResult.at(nBlobNum).fBKGV )
					/ ( 1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f ) );
			}
		}

		cv::RotatedRect BoundingBox = cv::minAreaRect(m_BlobResult.at(nBlobNum).ptIndexs);

		// ȸ���� �簢�� ������ 4��
		//cv::Point2f vertices[4];
		//BoundingBox.points(vertices);

		// Box width and length
		m_BlobResult.at(nBlobNum).BoxSize		= BoundingBox.size;

		// Angle between the horizontal axis
		m_BlobResult.at(nBlobNum).fAngle		= BoundingBox.angle;

		// Minor Axis & Major Axis
		if( BoundingBox.size.width > BoundingBox.size.height )
		{
			m_BlobResult.at(nBlobNum).fMinorAxis	= BoundingBox.size.width;
			m_BlobResult.at(nBlobNum).fMajorAxis	= BoundingBox.size.height;
		}
		else
		{
			m_BlobResult.at(nBlobNum).fMinorAxis	= BoundingBox.size.height;
			m_BlobResult.at(nBlobNum).fMajorAxis	= BoundingBox.size.width;
		}

		// Feret��s area
		m_BlobResult.at(nBlobNum).fMinBoxArea = m_BlobResult.at(nBlobNum).fMinorAxis * m_BlobResult.at(nBlobNum).fMajorAxis;

		// Axis Ratio
		if( m_BlobResult.at(nBlobNum).fMajorAxis > 0 )
			m_BlobResult.at(nBlobNum).fAxisRatio		= m_BlobResult.at(nBlobNum).fMinorAxis / m_BlobResult.at(nBlobNum).fMajorAxis;
		else
			m_BlobResult.at(nBlobNum).fAxisRatio		= 0.f;

		// Min Bounding Box ���� ���� / ��ü ���� ( Area porosity )
		m_BlobResult.at(nBlobNum).fMinBoxRatio = m_BlobResult.at(nBlobNum).fMinBoxArea / (float)m_BlobResult.at(nBlobNum).nArea;
		//choikwangil
		m_BlobResult.at(nBlobNum).fMeanAreaRatio = m_BlobResult.at(nBlobNum).fMeanGV / (float)m_BlobResult.at(nBlobNum).nArea;
		// �Ҵ� ����
		matTmp_src.release();
		matTmp_label.release();
		matTemp.release();
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}
	// �ð� ���� Ȯ��
	if( cTimeOut.GetTimeOutFlag() )	return false;

	return true;
}

// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
bool CFeatureExtraction::calcContours(int* nContoursX, int* nContoursY, int nDefectIndex, float fAngle, int nCx, int nCy, int nPs)
{
	// Blob ���� ���Ѱ�� ����
	if( !m_bComplete )					return false;

	// Blob ��� 0�� ����
	if( m_BlobResult.size() == 0 )		return true;

	// �߸��� �Ķ���� �Էµ� ���
	if( nDefectIndex < 0 )						return false;
	if( nDefectIndex >= m_BlobResult.size() )	return false;

	//////////////////////////////////////////////////////////////////////////

	// �ܰ��� ����
	int nContoursCount = m_BlobResult[nDefectIndex].ptContours.size();

	// ȸ�� ��ǥ ��� ��, ���
	double dTheta = -0.0 * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );

	float fRatio = 1.0;

	// ���� �������� ���� ���
	if( nContoursCount >= MAX_CONTOURS )
	{
		fRatio = nContoursCount / (float)MAX_CONTOURS;
		nContoursCount = MAX_CONTOURS;
	}

	// �ܰ��� ����
	for (int j=0 ; j<nContoursCount; j++)
	{
		// ���� ����� �ܰ��� Index
		int i = (int)(j * fRatio);

		// ȸ�� ��, ���� ��ǥ ���
		//int X = (int)( dCos * (m_BlobResult[nDefectIndex].ptContours[i].x - nCx) - dSin * (m_BlobResult[nDefectIndex].ptContours[i].y - nCy) + nCx );
		//int Y = (int)( dSin * (m_BlobResult[nDefectIndex].ptContours[i].x - nCx) + dCos * (m_BlobResult[nDefectIndex].ptContours[i].y - nCy) + nCy );

		int X = (int)(dCos * (m_BlobResult[nDefectIndex].ptContours[i].x - nCx) - dSin * (m_BlobResult[nDefectIndex].ptContours[i].y - nCy) + nCx);
		int Y = (int)(dSin * (m_BlobResult[nDefectIndex].ptContours[i].x - nCx) + dCos * (m_BlobResult[nDefectIndex].ptContours[i].y - nCy) + nCy);

		// P/S ��忡 ���� ��ǥ ���� ( 1�� ��ǥ�� ���� )
		nContoursX[j] = (int)(X / nPs);
		nContoursY[j] = (int)(Y / nPs);
	}

	return true;
}