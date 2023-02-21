
/************************************************************************/
// Blob 관련 소스
// 수정일 : 17.03.08 - KSW
/************************************************************************/

#include "StdAfx.h"

#include "FeatureExtraction.h"

// 병렬 처리
#include <ppl.h>
using namespace Concurrency;

//////////////////////////////////////////////////////////////////////////

CFeatureExtraction::CFeatureExtraction(void)
{
	m_bComplete = false;

	// 벡터 초기화
	vector<tBLOB_FEATURE>().swap(m_BlobResult);

	InitializeCriticalSectionAndSpinCount(&m_csCoordFile,4000);

	cMem				= NULL;
	m_cInspectLibLog	= NULL;
	m_strAlgLog			= NULL;
	m_tInitTime			= 0;
	m_tBeforeTime		= 0;

	// 설정 시간 넘어가면, 클래스 종료 ( 40초 )
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

	// 벡터 초기화
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
	// Blob 완료 했는지 확인용.
	m_bComplete = false;

	// 메모리 해제
	Release();

	// 벡터 초기화
	if( m_BlobResult.size() != 0 )
	{
		for (int i=0 ; i<m_BlobResult.size() ; i++)
		{
			vector<cv::Point>().swap(m_BlobResult[i].ptIndexs);
			vector <cv::Point>().swap(m_BlobResult[i].ptContours);
		}
		vector<tBLOB_FEATURE>().swap(m_BlobResult);
	}

	// 영상이 없는 경우 리턴
	if( ThresholdBuffer.empty() )			return false;

	// 1채널이 아닌 경우
	if( ThresholdBuffer.channels() != 1 )	return false;

	// Gray 영상이 존재X  &  1채널 아닌 경우
	bool bGrayEmpty = false;
	if( GrayBuffer.empty() || GrayBuffer.channels() != 1)
	{
		GrayBuffer = ThresholdBuffer.clone();
		bGrayEmpty = true;
	}

	// 영상 변환
	//cv::Mat LabelBuffer;
	//ThresholdBuffer.convertTo(LabelBuffer, CV_32SC1);

	// Label 계산 (8방향)
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

	// 예외처리
	if( nTotalLabel < 0 )
	{
		// 메모리 해제
		if( bGrayEmpty )			GrayBuffer.release();
		if( !matLabel.empty() )		matLabel.release();
		if( !matStats.empty() )		matStats.release();
		if( !matCentroid.empty() )	matCentroid.release();

		return false;
	}

	// 예외처리
	if (nTotalLabel >= nMaxDefectCount)
		nTotalLabel = nMaxDefectCount-1;

	// 기본 Feature 계산 8bit
	if( GrayBuffer.type() == CV_8UC1 )
		DoFeatureBasic_8bit(matLabel, matStats, matCentroid, GrayBuffer, (int)nTotalLabel, &cMatBufTemp);

	// 기본 Feature 계산 16bit
	else
		DoFeatureBasic_16bit(matLabel, matStats, matCentroid, GrayBuffer, (int)nTotalLabel, &cMatBufTemp);

	writeInspectLog(__FUNCTION__, _T("FeatureBasic."));

	// 메모리 해제
	if( bGrayEmpty )			GrayBuffer.release();
	if( !matLabel.empty() )		matLabel.release();
	if( !matStats.empty() )		matStats.release();
	if( !matCentroid.empty() )	matCentroid.release();

	writeInspectLog(__FUNCTION__, _T("Release."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}
	// Blob 완료
	m_bComplete = true;

	return true;
}
bool CFeatureExtraction::DoBlobCalculate(cv::Mat ThresholdBuffer, CRect rectROI, cv::Mat GrayBuffer, int nMaxDefectCount)
{
	// Blob 완료 했는지 확인용.
	m_bComplete = false;

	// 메모리 해제
	Release();

	// 벡터 초기화
	if (m_BlobResult.size() != 0)
	{
		for (int i = 0; i < m_BlobResult.size(); i++)
		{
			vector<cv::Point>().swap(m_BlobResult[i].ptIndexs);
			vector <cv::Point>().swap(m_BlobResult[i].ptContours);
		}
		vector<tBLOB_FEATURE>().swap(m_BlobResult);
	}

	// 영상이 없는 경우 리턴
	if (ThresholdBuffer.empty())			return false;

	// 1채널이 아닌 경우
	if (ThresholdBuffer.channels() != 1)	return false;

	// Gray 영상이 존재X  &  1채널 아닌 경우
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

	// 예외처리
	if (nTotalLabel < 0)
	{
		// 메모리 해제
		if (bGrayEmpty)				GrayBuffer.release();
		if (!matLabel.empty())		matLabel.release();
		if (!matStats.empty())		matStats.release();
		if (!matCentroid.empty())	matCentroid.release();

		return false;
	}

	// 예외처리
	if (nTotalLabel >= nMaxDefectCount)
		nTotalLabel = nMaxDefectCount - 1;

	// 기본 Feature 계산 8bit
	if (GrayBuffer.type() == CV_8UC1)
		DoFeatureBasic_8bit(matLabel, matStats, matCentroid, GrayBuffer, (int)nTotalLabel, &cMatBufTemp);

	// 기본 Feature 계산 16bit
	else
		DoFeatureBasic_16bit(matLabel, matStats, matCentroid, GrayBuffer, (int)nTotalLabel, &cMatBufTemp);

	writeInspectLog(__FUNCTION__, _T("FeatureBasic."));

	CoordApply(rectROI, nTotalLabel);

	// 메모리 해제
	if (bGrayEmpty)			GrayBuffer.release();
	if (!matLabel.empty())		matLabel.release();
	if (!matStats.empty())		matStats.release();
	if (!matCentroid.empty())	matCentroid.release();

	writeInspectLog(__FUNCTION__, _T("Release."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}

	// Blob 완료
	m_bComplete = true;

	return true;
}

bool CFeatureExtraction::DoBlobCalculate_LCP(cv::Mat ThresholdBuffer, CRect rectROI, double* dPara, cv::Mat GrayBuffer, cv::Mat ColorBuffer,  int nMaxDefectCount)
{
	// Blob 완료 했는지 확인용.
	m_bComplete = false;

	// 메모리 해제
	Release();

	// 벡터 초기화
	if (m_BlobResult.size() != 0)
	{
		for (int i = 0; i < m_BlobResult.size(); i++)
		{
			vector<cv::Point>().swap(m_BlobResult[i].ptIndexs);
			vector <cv::Point>().swap(m_BlobResult[i].ptContours);
		}
		vector<tBLOB_FEATURE>().swap(m_BlobResult);
	}

	// 영상이 없는 경우 리턴
	if (ThresholdBuffer.empty())			return false;

	// 1채널이 아닌 경우
	if (ThresholdBuffer.channels() != 1)	return false;

	// Gray 영상이 존재X  &  1채널 아닌 경우
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

	// 예외처리
	if (nTotalLabel < 0)
	{
		// 메모리 해제
		if (bGrayEmpty)				GrayBuffer.release();
		if (!matLabel.empty())		matLabel.release();
		if (!matStats.empty())		matStats.release();
		if (!matCentroid.empty())	matCentroid.release();

		return false;
	}

	// 예외처리
	if (nTotalLabel >= nMaxDefectCount)
		nTotalLabel = nMaxDefectCount - 1;

	// 기본 Feature 계산 8bit
	if (GrayBuffer.type() == CV_8UC1)
		DoFeatureBasic_8bit_LCP(matLabel, matStats, matCentroid, GrayBuffer, ColorBuffer,dPara ,(int)nTotalLabel, &cMatBufTemp);

	// 기본 Feature 계산 16bit
	else
		DoFeatureBasic_16bit(matLabel, matStats, matCentroid, GrayBuffer, (int)nTotalLabel, &cMatBufTemp);

	writeInspectLog(__FUNCTION__, _T("FeatureBasic."));

	CoordApply(rectROI, nTotalLabel);

	// 메모리 해제
	if (bGrayEmpty)			GrayBuffer.release();
	if (!matLabel.empty())		matLabel.release();
	if (!matStats.empty())		matStats.release();
	if (!matCentroid.empty())	matCentroid.release();

	writeInspectLog(__FUNCTION__, _T("Release."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}

	// Blob 완료
	m_bComplete = true;

	return true;
}

// 좌표 보정
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

// Blob & 판정 결과
long CFeatureExtraction::DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType, bool bPtRotate)
{
	// 시간 제한 시작
	cTimeOut.TimeCheckStart();

	// 메모리 해제
	Release();

	// 파라미터 NULL인 경우.
	if (nCommonPara == NULL)						return E_ERROR_CODE_EMPTY_PARA;
	if (pResultBlob == NULL)						return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)				return E_ERROR_CODE_EMPTY_PARA;

	if (nDefectType < 0)							return E_ERROR_CODE_EMPTY_PARA;
	if (nDefectType >= E_DEFECT_JUDGEMENT_COUNT)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if (matSrcImage.empty())						return E_ERROR_CODE_EMPTY_BUFFER;
	if (matThresholdImage.empty())					return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];						// 00 : 불량 최대 갯수
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;	// 01 : 알고리즘 중간결과 Image Save 
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];						// 02 : 영상 저장 순서 카운트
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];						// 03 : 현재 영상 번호
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];						// 04 : Cam Number
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];						// 05 : ROI Number
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];						// 06 : 알고리즘 번호
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];						// 07 : Thread ID
	bool	bDefectNum = (nCommonPara[E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG] > 0) ? true : false;	// 08 : Draw Defect Num 표시
	bool	bDrawDust = (nCommonPara[E_PARA_COMMON_DRAW_DUST_FLAG] > 0) ? true : false;	// 09 : Draw Dust 표시
	int		nPatternImageNum = nCommonPara[E_PARA_COMMON_UI_IMAGE_NUMBER];						// 10 : UI 상 패턴 순서 영상 번호
	float	fAngle = nCommonPara[E_PARA_COMMON_ROTATE_ANGLE] / 1000.f;				// 11 : Cell 회전된 각도 ( Align 계산 값, 소수점 3자리 까지만... )
	int		nCx = nCommonPara[E_PARA_COMMON_ROTATE_CENTER_X];						// 12 : Cell 회전된 중심 x좌표
	int		nCy = nCommonPara[E_PARA_COMMON_ROTATE_CENTER_Y];						// 13 : Cell 회전된 중심 y좌표
	int		nPS = nCommonPara[E_PARA_COMMON_PS_MODE];						// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

																			// 불량 입력 된 갯수
	int& nDefectCount = pResultBlob->nDefectCount;

	// 최대 불량 갯수 넘은 경우 빠져 나오기
	if (nDefectCount >= nMaxDefectCount)
		return E_ERROR_CODE_TRUE;

	// 회전 좌표 계산 시, 사용
	double dTheta = -fAngle * PI / 180.;
	double	dSin = sin(dTheta);
	double	dCos = cos(dTheta);
	double	dSin_ = sin(-dTheta);
	double	dCos_ = cos(-dTheta);

	// 라벨 시작
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);


	// 선택한 Defect 리스트
	int nFork = nDefectType;

	// Defect 리스트 선택한 경우만...
	if (!EngineerDefectJudgment[nFork].bDefectItemUse)
		return E_ERROR_CODE_TRUE;

	// Blob 갯수
	for (int i = 0; i < m_BlobResult.size(); i++)
	{
		// 불량 색상 필터링
		if (!DoColorFilter(nFork, nDefectColor))
			continue;


		// 판정 항목당 범위 2개
		int nFeatureCount = E_FEATURE_COUNT * 2;
		bool bFilter = true;
		bool bInit = false;

		for (int nForj = 0; nForj < nFeatureCount; nForj++)
		{

			// 판정 항목 선택한 경우만
			if (!EngineerDefectJudgment[nFork].Judgment[nForj].bUse)
				continue;

			// 한번이라도 동작 한 경우.
			bInit = true;

			// 설정한 필터링 만족하면 true / 만족하지 않으면 false 반환
			if (!DoFiltering(
				m_BlobResult[i],										// Blob 결과
				nForj / 2,												// 비교 Feature
				EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// 연산자 ( <, >, ==, <=, >= )
				EngineerDefectJudgment[nFork].Judgment[nForj].dValue))	// 값
			{

				bFilter = false;
				break;
			}
		}



		// 설정한 모든 조건이 만족 하는 경우, 결과 입력
		if (bInit && bFilter)
		{
			m_BlobResult[i].bFiltering = true;

			// 꼭지점 정보
			if (bPtRotate)
			{
				int nL, nT, nR, nB;

				// 회전 시, 예측 좌표 계산
				int X = (int)(dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx);
				int Y = (int)(dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy);

				// 초기 설정
				nL = nR = X;
				nT = nB = Y;

				// 외곽선 갯수 만큼
				for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
				{
					// 회전 시, 예측 좌표 계산
					X = (int)(dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx);
					Y = (int)(dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy);

					// 갱신
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

				// 초기 설정
				nL = nR = X;
				nT = nB = Y;

				// 외곽선 갯수 만큼
				for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
				{
					X = m_BlobResult[i].ptContours[k].x;
					Y = m_BlobResult[i].ptContours[k].y;

					// 갱신
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

			// UI에 넘겨줄 결과 넣기
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
			pResultBlob->nDefectJudge[nDefectCount] = nFork;			// 해당 불량
			pResultBlob->nPatternClassify[nDefectCount] = nPatternImageNum;

#if USE_ALG_HIST
			// KSW 17.06.24 객체 히스토그램
			memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
			// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
			calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif


			// 불량 번호 그리기
			if (!matDrawBuffer.empty() && bDefectNum)
			{
				cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x - 2, pResultBlob->ptRT[nDefectCount].y - 10, 30, 12), cv::Scalar(0, 0, 0), -1);

				char str[256] = { 0, };
				sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
				cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
				cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
			}

			// 마지막 불량 카운트 증가
			nDefectCount++;
		}



		// 최대 불량 갯수 넘은 경우 빠져 나오기
		if (nDefectCount >= nMaxDefectCount)
			break;
	}

	return E_ERROR_CODE_TRUE;
}

// Blob & 판정 결과 ROI
long CFeatureExtraction::DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType,  bool bPtRotate)
{
	// 시간 제한 시작
	cTimeOut.TimeCheckStart();

	// 메모리 해제
	Release();

	// 파라미터 NULL인 경우.
	if( nCommonPara == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)				return E_ERROR_CODE_EMPTY_PARA;

	if( nDefectType < 0	)							return E_ERROR_CODE_EMPTY_PARA;
	if( nDefectType >= E_DEFECT_JUDGEMENT_COUNT	)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcImage.empty() )						return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )					return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		= nCommonPara[	E_PARA_COMMON_MAX_DEFECT_COUNT			];						// 00 : 불량 최대 갯수
	bool	bImageSave			= (nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_FLAG			] > 0) ? true : false;	// 01 : 알고리즘 중간결과 Image Save 
	int&	nSaveImageCount		= nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_COUNT			];						// 02 : 영상 저장 순서 카운트
	int		nImageNum			= nCommonPara[	E_PARA_COMMON_ALG_IMAGE_NUMBER			];						// 03 : 현재 영상 번호
	int		nCamNum				= nCommonPara[	E_PARA_COMMON_CAMERA_NUMBER				];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[	E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[	E_PARA_COMMON_ALG_NUMBER				];						// 06 : 알고리즘 번호
	int		nThrdIndex			= nCommonPara[	E_PARA_COMMON_THREAD_ID					];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG		] > 0) ? true : false;	// 08 : Draw Defect Num 표시
	bool	bDrawDust			= (nCommonPara[	E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust 표시
	int		nPatternImageNum	= nCommonPara[	E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI 상 패턴 순서 영상 번호
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell 회전된 각도 ( Align 계산 값, 소수점 3자리 까지만... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell 회전된 중심 x좌표
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell 회전된 중심 y좌표
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
																
	// 불량 입력 된 갯수
	int& nDefectCount = pResultBlob->nDefectCount;

	// 최대 불량 갯수 넘은 경우 빠져 나오기
	if( nDefectCount >= nMaxDefectCount )
		return E_ERROR_CODE_TRUE;

	// 회전 좌표 계산 시, 사용
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );
	double	dSin_	= sin( -dTheta );
	double	dCos_	= cos( -dTheta );

	// 라벨 시작
	DoBlobCalculate(matThresholdImage, rectROI, matSrcImage, nMaxDefectCount);


	// 선택한 Defect 리스트
	int nFork = nDefectType;

	// Defect 리스트 선택한 경우만...
	if( !EngineerDefectJudgment[nFork].bDefectItemUse )
		return E_ERROR_CODE_TRUE;

	// Blob 갯수
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// 불량 색상 필터링
		if( !DoColorFilter(nFork, nDefectColor) )
			continue;
	

		// 판정 항목당 범위 2개
		int nFeatureCount = E_FEATURE_COUNT * 2;
		bool bFilter	= true;
		bool bInit		= false;

		for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
		{

			// 판정 항목 선택한 경우만
			if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
				continue;

			// 한번이라도 동작 한 경우.
			bInit = true;

			// 설정한 필터링 만족하면 true / 만족하지 않으면 false 반환
			if(	!DoFiltering(
				m_BlobResult[i],										// Blob 결과
				nForj/2,												// 비교 Feature
				EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// 연산자 ( <, >, ==, <=, >= )
				EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// 값
			{

				bFilter = false;
				break;
			}
		}

		

		// 설정한 모든 조건이 만족 하는 경우, 결과 입력
		if( bInit && bFilter )
		{
			m_BlobResult[i].bFiltering = true;

			// 꼭지점 정보
			if(bPtRotate)
			{
				int nL, nT, nR, nB;

				// 회전 시, 예측 좌표 계산
				int X = (int)( dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx );
				int Y = (int)( dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy );

				// 초기 설정
				nL = nR = X;
				nT = nB = Y;

				// 외곽선 갯수 만큼
				for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
				{
					// 회전 시, 예측 좌표 계산
					X = (int)( dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx );
					Y = (int)( dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy );

					// 갱신
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

				// 초기 설정
				nL = nR = X;
				nT = nB = Y;

				// 외곽선 갯수 만큼
				for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
				{
					X = m_BlobResult[i].ptContours[k].x; 
					Y = m_BlobResult[i].ptContours[k].y;

					// 갱신
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

			// UI에 넘겨줄 결과 넣기
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
			pResultBlob->nDefectJudge	[nDefectCount] = nFork;			// 해당 불량
			pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

#if USE_ALG_HIST
			// KSW 17.06.24 객체 히스토그램
			memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
			// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
			calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

			
			// 불량 번호 그리기
			if( !matDrawBuffer.empty() && bDefectNum )
			{
				cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x-2, pResultBlob->ptRT[nDefectCount].y-10, 30, 12), cv::Scalar(0, 0, 0), -1);

				char str[256]={0,};
				sprintf_s(str, sizeof(str),"%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
				cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
				cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
			}

			// 마지막 불량 카운트 증가
			nDefectCount++;
		}

		

		// 최대 불량 갯수 넘은 경우 빠져 나오기
		if( nDefectCount >= nMaxDefectCount )
			break;
	}

	return E_ERROR_CODE_TRUE;
}

// Blob & 판정 결과
long CFeatureExtraction::DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate)
{
	writeInspectLog(__FUNCTION__, _T("Start."));

	// 시간 제한 시작
	cTimeOut.TimeCheckStart();

	// 메모리 해제
	Release();

	// 파라미터 NULL인 경우.
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcImage.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		= nCommonPara[	E_PARA_COMMON_MAX_DEFECT_COUNT			];						// 00 : 불량 최대 갯수
	bool	bImageSave			= (nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_FLAG			] > 0) ? true : false;	// 01 : 알고리즘 중간결과 Image Save 
	int&	nSaveImageCount		= nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_COUNT			];						// 02 : 영상 저장 순서 카운트
	int		nImageNum			= nCommonPara[	E_PARA_COMMON_ALG_IMAGE_NUMBER			];						// 03 : 현재 영상 번호
	int		nCamNum				= nCommonPara[	E_PARA_COMMON_CAMERA_NUMBER				];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[	E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[	E_PARA_COMMON_ALG_NUMBER				];						// 06 : 알고리즘 번호
	int		nThrdIndex			= nCommonPara[	E_PARA_COMMON_THREAD_ID					];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG		] > 0) ? true : false;	// 08 : Draw Defect Num 표시
	bool	bDrawDust			= (nCommonPara[	E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust 표시
	int		nPatternImageNum	= nCommonPara[	E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI 상 패턴 순서 영상 번호
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell 회전된 각도 ( Align 계산 값, 소수점 3자리 까지만... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell 회전된 중심 x좌표
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell 회전된 중심 y좌표
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 입력 된 갯수
	int& nDefectCount = pResultBlob->nDefectCount;

	// 최대 불량 갯수 넘은 경우 빠져 나오기
	if( nDefectCount >= nMaxDefectCount )
		return E_ERROR_CODE_TRUE;
	
	// 회전 좌표 계산 시, 사용
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );
	double	dSin_	= sin( -dTheta );
	double	dCos_	= cos( -dTheta );

	// 라벨 시작
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);

	writeInspectLog(__FUNCTION__, _T("BlobCalculate."));

	// Blob 갯수
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// 시간 제한 확인
		if( cTimeOut.GetTimeOutFlag() )	continue;

		// KSW 17.10.16 [임시] - E_DEFECT_JUDGEMENT_MURA_MULT_BP 우선순위
		bool	bMultFlag		= false;
		bool	bMultCalcFlag	= false;
		if( EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_MULT_BP].bDefectItemUse )
			bMultFlag = true;

		// Defect 리스트 만큼
		for (int nFork = 0 ; nFork < E_DEFECT_JUDGEMENT_COUNT ; nFork++)
		{
			// KSW 17.10.16 [임시] - E_DEFECT_JUDGEMENT_MURA_MULT_BP 진행 후, 처음부터 시작
			if( bMultFlag && bMultCalcFlag )
			{
				bMultFlag	= false;
				nFork		= 0;
			}

			// KSW 17.10.16 [임시] - E_DEFECT_JUDGEMENT_MURA_MULT_BP 우선 진행
			else if( bMultFlag )
			{
				nFork = E_DEFECT_JUDGEMENT_MURA_MULT_BP;
				bMultCalcFlag = true;
			}						

			// Defect 리스트 선택한 경우만...
			else if( !EngineerDefectJudgment[nFork].bDefectItemUse )
				continue;

			// 불량 색상 필터링
			if( !DoColorFilter(nFork, nDefectColor) )
				continue;

			// 판정 항목당 범위 2개
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter	= true;
			bool bInit		= false;
			for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
			{
				// 판정 항목 선택한 경우만
				if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
					continue;

				// 한번이라도 동작 한 경우.
				bInit = true;

				// 설정한 필터링 만족하면 true / 만족하지 않으면 false 반환
				if(	!DoFiltering(
						m_BlobResult[i],										// Blob 결과
						nForj/2,												// 비교 Feature
						EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// 연산자 ( <, >, ==, <=, >= )
						EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// 값
				{
					bFilter = false;
					break;
				}
			}

			// 설정한 모든 조건이 만족 하는 경우, 결과 입력
			if( bInit && bFilter )
			{
				m_BlobResult[i].bFiltering = true;				
				
				// 꼭지점 정보
				if(bPtRotate)
				{
					int nL, nT, nR, nB;

					// 회전 시, 예측 좌표 계산
					int X = (int)( dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx );
					int Y = (int)( dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy );
					
					// 초기 설정
					nL = nR = X;
					nT = nB = Y;
					
					// 외곽선 갯수 만큼
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						// 회전 시, 예측 좌표 계산
						X = (int)( dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx );
						Y = (int)( dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy );

						// 갱신
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

					// 초기 설정
					nL = nR = X;
					nT = nB = Y;

					// 외곽선 갯수 만큼
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						X = m_BlobResult[i].ptContours[k].x; 
						Y = m_BlobResult[i].ptContours[k].y;

						// 갱신
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

				// UI에 넘겨줄 결과 넣기
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
				pResultBlob->nDefectJudge	[nDefectCount] = nFork;			// 해당 불량
				pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

#if USE_ALG_HIST
				// KSW 17.06.24 객체 히스토그램
				memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
				// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
				calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

				// 불량 번호 그리기
				if( !matDrawBuffer.empty() && bDefectNum )
				{
					cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x-2, pResultBlob->ptRT[nDefectCount].y-10, 30, 12), cv::Scalar(0, 0, 0), -1);

					char str[256]={0,};
					sprintf_s(str, sizeof(str),"%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
					cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
					cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
				}

				// 마지막 불량 카운트 증가
				nDefectCount++;

				break;
			}
		}

		// 최대 불량 갯수 넘은 경우 빠져 나오기
		if( nDefectCount >= nMaxDefectCount )
			break;
	}

	writeInspectLog(__FUNCTION__, _T("Filtering & Result."));

	// 시간 제한 확인
	if( cTimeOut.GetTimeOutFlag() )	return E_ERROR_CODE_TIME_OUT;

	return E_ERROR_CODE_TRUE;
}
// Blob & 판정 결과 ROI
long CFeatureExtraction::DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate)
{
	writeInspectLog(__FUNCTION__, _T("Start."));

	// 시간 제한 시작
	cTimeOut.TimeCheckStart();

	// 메모리 해제
	Release();

	// 파라미터 NULL인 경우.
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (pResultBlob == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if (matSrcImage.empty())			return E_ERROR_CODE_EMPTY_BUFFER;
	if (matThresholdImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		= nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT		];						// 00 : 불량 최대 갯수
	bool	bImageSave			= (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG		] > 0) ? true : false;	// 01 : 알고리즘 중간결과 Image Save 
	int&	nSaveImageCount		= nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT		];						// 02 : 영상 저장 순서 카운트
	int		nImageNum			= nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER		];						// 03 : 현재 영상 번호
	int		nCamNum				= nCommonPara[E_PARA_COMMON_CAMERA_NUMBER			];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[E_PARA_COMMON_ALG_NUMBER				];						// 06 : 알고리즘 번호
	int		nThrdIndex			= nCommonPara[E_PARA_COMMON_THREAD_ID				];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG	] > 0) ? true : false;	// 08 : Draw Defect Num 표시
	bool	bDrawDust			= (nCommonPara[E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust 표시
	int		nPatternImageNum	= nCommonPara[E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI 상 패턴 순서 영상 번호
	float	fAngle				= nCommonPara[E_PARA_COMMON_ROTATE_ANGLE			] / 1000.f;				// 11 : Cell 회전된 각도 ( Align 계산 값, 소수점 3자리 까지만... )
	int		nCx					= nCommonPara[E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell 회전된 중심 x좌표
	int		nCy					= nCommonPara[E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell 회전된 중심 y좌표
	int		nPS					= nCommonPara[E_PARA_COMMON_PS_MODE					];						// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 입력 된 갯수
	int& nDefectCount = pResultBlob->nDefectCount;

	// 최대 불량 갯수 넘은 경우 빠져 나오기
	if (nDefectCount >= nMaxDefectCount)
		return E_ERROR_CODE_TRUE;

	// 회전 좌표 계산 시, 사용
	double dTheta = -fAngle * PI / 180.;
	double	dSin = sin(dTheta);
	double	dCos = cos(dTheta);
	double	dSin_ = sin(-dTheta);
	double	dCos_ = cos(-dTheta);

	// 라벨 시작
	DoBlobCalculate(matThresholdImage, rectROI, matSrcImage, nMaxDefectCount);

	writeInspectLog(__FUNCTION__, _T("BlobCalculate."));

	//cv::Mat Defect_map = matSrcImage.clone(); // pwj

	// Blob 갯수
	for (int i = 0; i < m_BlobResult.size(); i++)
	{
		// 시간 제한 확인
		if (cTimeOut.GetTimeOutFlag())	continue;

		// KSW 17.10.16 [임시] - E_DEFECT_JUDGEMENT_MURA_MULT_BP 우선순위
		bool	bMultFlag		= false;
		bool	bMultCalcFlag	= false;
		if (EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_MULT_BP].bDefectItemUse)
			bMultFlag = true;

		// Defect 리스트 만큼
		for (int nFork = 0; nFork < E_DEFECT_JUDGEMENT_COUNT; nFork++)
		{
			// KSW 17.10.16 [임시] - E_DEFECT_JUDGEMENT_MURA_MULT_BP 진행 후, 처음부터 시작
			if (bMultFlag && bMultCalcFlag)
			{
				bMultFlag = false;
				nFork = 0;
			}

			// KSW 17.10.16 [임시] - E_DEFECT_JUDGEMENT_MURA_MULT_BP 우선 진행
			else if (bMultFlag)
			{
				nFork = E_DEFECT_JUDGEMENT_MURA_MULT_BP;
				bMultCalcFlag = true;
			}

			// Defect 리스트 선택한 경우만...
			else if (!EngineerDefectJudgment[nFork].bDefectItemUse)
				continue;

			// 불량 색상 필터링
			if (!DoColorFilter(nFork, nDefectColor))
				continue;

			// 판정 항목당 범위 2개
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter = true;
			bool bInit = false;
			for (int nForj = 0; nForj < nFeatureCount; nForj++)
			{
				// 판정 항목 선택한 경우만
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
				// 한번이라도 동작 한 경우.
				bInit = true;

				// 설정한 필터링 만족하면 true / 만족하지 않으면 false 반환
				if (!DoFiltering(
					m_BlobResult[i],										// Blob 결과
					nForj / 2,												// 비교 Feature
					EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// 연산자 ( <, >, ==, <=, >= )
					EngineerDefectJudgment[nFork].Judgment[nForj].dValue))	// 값
				{
					bFilter = false;
					break;
				}
			}

			// 설정한 모든 조건이 만족 하는 경우, 결과 입력
			if (bInit && bFilter)
			{
				m_BlobResult[i].bFiltering = true;

				// 꼭지점 정보
				//if (bPtRotate)
				if (0)
				{
					int nL, nT, nR, nB;

					// 회전 시, 예측 좌표 계산
					int X = (int)(dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx);
					int Y = (int)(dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy);

					// 초기 설정
					nL = nR = X;
					nT = nB = Y;

					// 외곽선 갯수 만큼
					for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
					{
						// 회전 시, 예측 좌표 계산
						X = (int)(dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx);
						Y = (int)(dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy);

						// 갱신
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

					// 초기 설정
					nL = nR = X;
					nT = nB = Y;

					// 외곽선 갯수 만큼
					for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
					{
						X = m_BlobResult[i].ptContours[k].x;
						Y = m_BlobResult[i].ptContours[k].y;

						// 갱신
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

				// UI에 넘겨줄 결과 넣기
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
				pResultBlob->nDefectJudge[nDefectCount]		= nFork;			// 해당 불량
				pResultBlob->nPatternClassify[nDefectCount] = nPatternImageNum;


#if USE_ALG_HIST
				// KSW 17.06.24 객체 히스토그램
				memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
				// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
				calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

				// 불량 번호 그리기
				if (!matDrawBuffer.empty() && bDefectNum)
				{
					cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x - 2, pResultBlob->ptRT[nDefectCount].y - 10, 30, 12), cv::Scalar(0, 0, 0), -1);

					char str[256] = { 0, };
					sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
					cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
					cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
				}

				//defect Map 불량 전부 그리기 pwj
				//cv::rectangle(Defect_map, cv::Rect(pResultBlob->ptRT[nDefectCount].x - 2, pResultBlob->ptRT[nDefectCount].y - 10, 30, 12), cv::Scalar(0, 0, 0), 1);

				//cv::imwrite("D:\\Defect map.jpg", Defect_map);

				// 마지막 불량 카운트 증가
				nDefectCount++;

				break;
			}
		}

		// 최대 불량 갯수 넘은 경우 빠져 나오기
		if (nDefectCount >= nMaxDefectCount)
			break;
	}

	writeInspectLog(__FUNCTION__, _T("Filtering & Result."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory(), m_nAlgType);
// 	}

	// 시간 제한 확인
	if (cTimeOut.GetTimeOutFlag())	return E_ERROR_CODE_TIME_OUT;

	return E_ERROR_CODE_TRUE;
}

long CFeatureExtraction::DoDefectBlobJudgment_LCP(cv::Mat& matSrcImage, cv::Mat& matColorImage ,cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer, CRect rectROI,
	int* nCommonPara, long nDefectColor, double* dPara, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate)
{
	writeInspectLog(__FUNCTION__, _T("Start."));

	// 시간 제한 시작
	cTimeOut.TimeCheckStart();

	// 메모리 해제
	Release();

	// 파라미터 NULL인 경우.
	if (nCommonPara == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (pResultBlob == NULL)			return E_ERROR_CODE_EMPTY_PARA;
	if (EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if (matSrcImage.empty())			return E_ERROR_CODE_EMPTY_BUFFER;
	if (matThresholdImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount = nCommonPara[E_PARA_COMMON_MAX_DEFECT_COUNT];						// 00 : 불량 최대 갯수
	bool	bImageSave = (nCommonPara[E_PARA_COMMON_IMAGE_SAVE_FLAG] > 0) ? true : false;	// 01 : 알고리즘 중간결과 Image Save 
	int&	nSaveImageCount = nCommonPara[E_PARA_COMMON_IMAGE_SAVE_COUNT];						// 02 : 영상 저장 순서 카운트
	int		nImageNum = nCommonPara[E_PARA_COMMON_ALG_IMAGE_NUMBER];						// 03 : 현재 영상 번호
	int		nCamNum = nCommonPara[E_PARA_COMMON_CAMERA_NUMBER];						// 04 : Cam Number
	int		nROINumber = nCommonPara[E_PARA_COMMON_ROI_NUMBER];						// 05 : ROI Number
	int		nAlgorithmNumber = nCommonPara[E_PARA_COMMON_ALG_NUMBER];						// 06 : 알고리즘 번호
	int		nThrdIndex = nCommonPara[E_PARA_COMMON_THREAD_ID];						// 07 : Thread ID
	bool	bDefectNum = (nCommonPara[E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG] > 0) ? true : false;	// 08 : Draw Defect Num 표시
	bool	bDrawDust = (nCommonPara[E_PARA_COMMON_DRAW_DUST_FLAG] > 0) ? true : false;	// 09 : Draw Dust 표시
	int		nPatternImageNum = nCommonPara[E_PARA_COMMON_UI_IMAGE_NUMBER];						// 10 : UI 상 패턴 순서 영상 번호
	float	fAngle = nCommonPara[E_PARA_COMMON_ROTATE_ANGLE] / 1000.f;				// 11 : Cell 회전된 각도 ( Align 계산 값, 소수점 3자리 까지만... )
	int		nCx = nCommonPara[E_PARA_COMMON_ROTATE_CENTER_X];						// 12 : Cell 회전된 중심 x좌표
	int		nCy = nCommonPara[E_PARA_COMMON_ROTATE_CENTER_Y];						// 13 : Cell 회전된 중심 y좌표
	int		nPS = nCommonPara[E_PARA_COMMON_PS_MODE];						// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

																			// 불량 입력 된 갯수
	int& nDefectCount = pResultBlob->nDefectCount;

	// 최대 불량 갯수 넘은 경우 빠져 나오기
	if (nDefectCount >= nMaxDefectCount)
		return E_ERROR_CODE_TRUE;

	// 회전 좌표 계산 시, 사용
	double dTheta = -fAngle * PI / 180.;
	double	dSin = sin(dTheta);
	double	dCos = cos(dTheta);
	double	dSin_ = sin(-dTheta);
	double	dCos_ = cos(-dTheta);

	// 라벨 시작
	DoBlobCalculate_LCP(matThresholdImage, rectROI, dPara, matSrcImage, matColorImage ,nMaxDefectCount);

	writeInspectLog(__FUNCTION__, _T("BlobCalculate."));

	//cv::Mat Defect_map = matSrcImage.clone(); // pwj

	// Blob 갯수
	for (int i = 0; i < m_BlobResult.size(); i++)
	{
		// 시간 제한 확인
		if (cTimeOut.GetTimeOutFlag())	continue;

		// KSW 17.10.16 [임시] - E_DEFECT_JUDGEMENT_MURA_MULT_BP 우선순위
		bool	bMultFlag = false;
		bool	bMultCalcFlag = false;
		if (EngineerDefectJudgment[E_DEFECT_JUDGEMENT_MURA_MULT_BP].bDefectItemUse)
			bMultFlag = true;

		// Defect 리스트 만큼
		for (int nFork = 0; nFork < E_DEFECT_JUDGEMENT_COUNT; nFork++)
		{
			// KSW 17.10.16 [임시] - E_DEFECT_JUDGEMENT_MURA_MULT_BP 진행 후, 처음부터 시작
			if (bMultFlag && bMultCalcFlag)
			{
				bMultFlag = false;
				nFork = 0;
			}

			// KSW 17.10.16 [임시] - E_DEFECT_JUDGEMENT_MURA_MULT_BP 우선 진행
			else if (bMultFlag)
			{
				nFork = E_DEFECT_JUDGEMENT_MURA_MULT_BP;
				bMultCalcFlag = true;
			}

			// Defect 리스트 선택한 경우만...
			else if (!EngineerDefectJudgment[nFork].bDefectItemUse)
				continue;

			// 불량 색상 필터링
			if (!DoColorFilter(nFork, nDefectColor))
				continue;

			// 판정 항목당 범위 2개
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter = true;
			bool bInit = false;
			for (int nForj = 0; nForj < nFeatureCount; nForj++)
			{
				// 판정 항목 선택한 경우만
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
				// 한번이라도 동작 한 경우.
				bInit = true;

				// 설정한 필터링 만족하면 true / 만족하지 않으면 false 반환
				if (!DoFiltering(
					m_BlobResult[i],										// Blob 결과
					nForj / 2,												// 비교 Feature
					EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// 연산자 ( <, >, ==, <=, >= )
					EngineerDefectJudgment[nFork].Judgment[nForj].dValue))	// 값
				{
					bFilter = false;
					break;
				}
			}

			// 설정한 모든 조건이 만족 하는 경우, 결과 입력
			if (bInit && bFilter)
			{
				m_BlobResult[i].bFiltering = true;

				// 꼭지점 정보
				//if (bPtRotate)
				if (0)
				{
					int nL, nT, nR, nB;

					// 회전 시, 예측 좌표 계산
					int X = (int)(dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx);
					int Y = (int)(dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy);

					// 초기 설정
					nL = nR = X;
					nT = nB = Y;

					// 외곽선 갯수 만큼
					for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
					{
						// 회전 시, 예측 좌표 계산
						X = (int)(dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx);
						Y = (int)(dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy);

						// 갱신
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

					// 초기 설정
					nL = nR = X;
					nT = nB = Y;

					// 외곽선 갯수 만큼
					for (int k = 1; k < m_BlobResult[i].ptContours.size(); k++)
					{
						X = m_BlobResult[i].ptContours[k].x;
						Y = m_BlobResult[i].ptContours[k].y;

						// 갱신
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

				// UI에 넘겨줄 결과 넣기
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
					pResultBlob->nDefectJudge[nDefectCount] = nFork;			// 해당 불량
				}

				pResultBlob->nPatternClassify[nDefectCount] = nPatternImageNum;

#if USE_ALG_HIST
				// KSW 17.06.24 객체 히스토그램
				memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
				// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
				calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

				// 불량 번호 그리기
				if (!matDrawBuffer.empty() && bDefectNum)
				{
					cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x - 2, pResultBlob->ptRT[nDefectCount].y - 10, 30, 12), cv::Scalar(0, 0, 0), -1);

					char str[256] = { 0, };
					sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
					cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
					cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
				}

				//defect Map 불량 전부 그리기 pwj
				//cv::rectangle(Defect_map, cv::Rect(pResultBlob->ptRT[nDefectCount].x - 2, pResultBlob->ptRT[nDefectCount].y - 10, 30, 12), cv::Scalar(0, 0, 0), 1);

				//cv::imwrite("D:\\Defect map.jpg", Defect_map);

				// 마지막 불량 카운트 증가
				nDefectCount++;

				break;
			}
		}

		// 최대 불량 갯수 넘은 경우 빠져 나오기
		if (nDefectCount >= nMaxDefectCount)
			break;
	}

	writeInspectLog(__FUNCTION__, _T("Filtering & Result."));

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMem->Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMem->Get_AutoMemory(), m_nAlgType);
// 	}

	// 시간 제한 확인
	if (cTimeOut.GetTimeOutFlag())	return E_ERROR_CODE_TIME_OUT;

	return E_ERROR_CODE_TRUE;
}

// Blob & 판정 결과
long CFeatureExtraction::DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, STRU_DEFECT_ITEM* EngineerDefectJudgment, int nDefectType, int nMaxDefectCount)
{
	// 시간 제한 시작
	cTimeOut.TimeCheckStart();

	// 메모리 해제
	Release();

	// 파라미터 NULL인 경우.
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcImage.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	if( nDefectType < 0	)							return E_ERROR_CODE_EMPTY_PARA;
	if( nDefectType >= E_DEFECT_JUDGEMENT_COUNT	)	return E_ERROR_CODE_EMPTY_PARA;

	// 라벨 시작
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);

	// 선택한 Defect 리스트
	int nFork = nDefectType;

	// Defect 리스트 선택한 경우만...
	if( !EngineerDefectJudgment[nFork].bDefectItemUse )
		return E_ERROR_CODE_TRUE;

	// Blob 갯수
#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// Defect 리스트 만큼
		//for (int nFork = 0 ; nFork < E_DEFECT_JUDGEMENT_COUNT ; nFork++)		
		{
			// 판정 항목당 범위 2개
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter	= true;
			bool bInit		= false;
			for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
			{
				// 판정 항목 선택한 경우만
				if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
					continue;

				// 한번이라도 동작 한 경우.
				bInit = true;

				// 설정한 필터링 만족하면 true / 만족하지 않으면 false 반환
				if(	!DoFiltering(
					m_BlobResult[i],										// Blob 결과
					nForj/2,												// 비교 Feature
					EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// 연산자 ( <, >, ==, <=, >= )
					EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// 값
				{
					bFilter = false;
					break;
				}
			}

			// 설정한 모든 조건이 만족 하는 경우
			if( bInit && bFilter )
			{
				m_BlobResult[i].bFiltering = true;
			}
		}
	}

	return E_ERROR_CODE_TRUE;
}

// Blob & 판정 결과
long CFeatureExtraction::DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, STRU_DEFECT_ITEM* EngineerDefectJudgment, int nMaxDefectCount)
{
	// 시간 제한 시작
	cTimeOut.TimeCheckStart();

	// 메모리 해제
	Release();

	// 파라미터 NULL인 경우.
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcImage.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	// 라벨 시작
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);

	// Blob 갯수
#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// 시간 제한 확인
		if( cTimeOut.GetTimeOutFlag() )	continue;

		// Defect 리스트 만큼
		for (int nFork = 0 ; nFork < E_DEFECT_JUDGEMENT_COUNT ; nFork++)		
		{
			// Defect 리스트 선택한 경우만...
			if( !EngineerDefectJudgment[nFork].bDefectItemUse )
				continue;

			// 판정 항목당 범위 2개
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter	= true;
			bool bInit		= false;
			for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
			{
				// 판정 항목 선택한 경우만
				if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
					continue;

				// 한번이라도 동작 한 경우.
				bInit = true;

				// 설정한 필터링 만족하면 true / 만족하지 않으면 false 반환
				if(	!DoFiltering(
					m_BlobResult[i],										// Blob 결과
					nForj/2,												// 비교 Feature
					EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// 연산자 ( <, >, ==, <=, >= )
					EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// 값
				{
					bFilter = false;
					break;
				}
			}

			// 설정한 모든 조건이 만족 하는 경우
			if( bInit && bFilter )
			{
				m_BlobResult[i].bFiltering = true;

				break;
			}
		}
	}

	// 시간 제한 확인
	if( cTimeOut.GetTimeOutFlag() )	return E_ERROR_CODE_TIME_OUT;

	return E_ERROR_CODE_TRUE;
}

////////ssy 0718 외곽에 위치하는지 검사
// Blob & 판정 결과
long CFeatureExtraction::DoDefectBlobSingleJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, int nDefectType,  bool bPtRotate, CRect prerectROI, int offset)
{
	// 시간 제한 시작
	cTimeOut.TimeCheckStart();

	// 메모리 해제
	Release();

	// 파라미터 NULL인 경우.
	if( nCommonPara == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)				return E_ERROR_CODE_EMPTY_PARA;

	if( nDefectType < 0	)							return E_ERROR_CODE_EMPTY_PARA;
	if( nDefectType >= E_DEFECT_JUDGEMENT_COUNT	)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcImage.empty() )						return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )					return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		= nCommonPara[	E_PARA_COMMON_MAX_DEFECT_COUNT			];						// 00 : 불량 최대 갯수
	bool	bImageSave			= (nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_FLAG			] > 0) ? true : false;	// 01 : 알고리즘 중간결과 Image Save 
	int&	nSaveImageCount		= nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_COUNT			];						// 02 : 영상 저장 순서 카운트
	int		nImageNum			= nCommonPara[	E_PARA_COMMON_ALG_IMAGE_NUMBER			];						// 03 : 현재 영상 번호
	int		nCamNum				= nCommonPara[	E_PARA_COMMON_CAMERA_NUMBER				];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[	E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[	E_PARA_COMMON_ALG_NUMBER				];						// 06 : 알고리즘 번호
	int		nThrdIndex			= nCommonPara[	E_PARA_COMMON_THREAD_ID					];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG		] > 0) ? true : false;	// 08 : Draw Defect Num 표시
	bool	bDrawDust			= (nCommonPara[	E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust 표시
	int		nPatternImageNum	= nCommonPara[	E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI 상 패턴 순서 영상 번호
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell 회전된 각도 ( Align 계산 값, 소수점 3자리 까지만... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell 회전된 중심 x좌표
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell 회전된 중심 y좌표
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 입력 된 갯수
	int& nDefectCount = pResultBlob->nDefectCount;

	// 최대 불량 갯수 넘은 경우 빠져 나오기
	if( nDefectCount >= nMaxDefectCount )
		return E_ERROR_CODE_TRUE;

	// 회전 좌표 계산 시, 사용
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );
	double	dSin_	= sin( -dTheta );
	double	dCos_	= cos( -dTheta );

	// 라벨 시작
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);

	// 선택한 Defect 리스트
	int nFork = nDefectType;

	// Defect 리스트 선택한 경우만...
	if( !EngineerDefectJudgment[nFork].bDefectItemUse )
		return E_ERROR_CODE_TRUE;

	// Blob 갯수
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// 불량 색상 필터링
		if( !DoColorFilter(nFork, nDefectColor) )
			continue;

		// 판정 항목당 범위 2개
		int nFeatureCount = E_FEATURE_COUNT * 2;
		bool bFilter	= true;
		bool bInit		= false;
		for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
		{
			// 판정 항목 선택한 경우만
			if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
				continue;

			// 한번이라도 동작 한 경우.
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
				// 설정한 필터링 만족하면 true / 만족하지 않으면 false 반환
				if(	!DoFiltering(
					m_BlobResult[i],										// Blob 결과
					nForj/2,												// 비교 Feature
					EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// 연산자 ( <, >, ==, <=, >= )
					EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// 값
				{
					bFilter = false;
					break;
				}
			}
		}

		// 설정한 모든 조건이 만족 하는 경우, 결과 입력
		if( bInit && bFilter )
		{
			m_BlobResult[i].bFiltering = true;

			// 꼭지점 정보
			if(bPtRotate)
			{
				int nL, nT, nR, nB;

				// 회전 시, 예측 좌표 계산
				int X = (int)( dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx );
				int Y = (int)( dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy );

				// 초기 설정
				nL = nR = X;
				nT = nB = Y;

				// 외곽선 갯수 만큼
				for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
				{
					// 회전 시, 예측 좌표 계산
					X = (int)( dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx );
					Y = (int)( dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy );

					// 갱신
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

				// 초기 설정
				nL = nR = X;
				nT = nB = Y;

				// 외곽선 갯수 만큼
				for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
				{
					X = m_BlobResult[i].ptContours[k].x; 
					Y = m_BlobResult[i].ptContours[k].y;

					// 갱신
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

			// UI에 넘겨줄 결과 넣기
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
			pResultBlob->nDefectJudge	[nDefectCount] = nFork;			// 해당 불량
			pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

#if USE_ALG_HIST
			// KSW 17.06.24 객체 히스토그램
			memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
			// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
			calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

			// 불량 번호 그리기
			if( !matDrawBuffer.empty() && bDefectNum )
			{
				cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x-2, pResultBlob->ptRT[nDefectCount].y-10, 30, 12), cv::Scalar(0, 0, 0), -1);

				char str[256]={0,};
				sprintf_s(str, sizeof(str),"%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
				cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
				cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
			}

			// 마지막 불량 카운트 증가
			nDefectCount++;
		}

		// 최대 불량 갯수 넘은 경우 빠져 나오기
		if( nDefectCount >= nMaxDefectCount )
			break;
	}

	return E_ERROR_CODE_TRUE;
}

////////SSY 1124 선택한 불량들만 검사태우기 백터로 넘겨줌
// Blob & 판정 결과
long CFeatureExtraction::DoDefectBlobMultiJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, vector<int> nDefectType,  bool bPtRotate, CRect prerectROI, int offset)
{
	// 시간 제한 시작
	cTimeOut.TimeCheckStart();

	// 메모리 해제
	Release();

	// 파라미터 NULL인 경우.
	if( nCommonPara == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )						return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)				return E_ERROR_CODE_EMPTY_PARA;

	if( nDefectType.size() == 0 )					return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcImage.empty() )						return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )					return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		= nCommonPara[	E_PARA_COMMON_MAX_DEFECT_COUNT			];						// 00 : 불량 최대 갯수
	bool	bImageSave			= (nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_FLAG			] > 0) ? true : false;	// 01 : 알고리즘 중간결과 Image Save 
	int&	nSaveImageCount		= nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_COUNT			];						// 02 : 영상 저장 순서 카운트
	int		nImageNum			= nCommonPara[	E_PARA_COMMON_ALG_IMAGE_NUMBER			];						// 03 : 현재 영상 번호
	int		nCamNum				= nCommonPara[	E_PARA_COMMON_CAMERA_NUMBER				];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[	E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[	E_PARA_COMMON_ALG_NUMBER				];						// 06 : 알고리즘 번호
	int		nThrdIndex			= nCommonPara[	E_PARA_COMMON_THREAD_ID					];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG		] > 0) ? true : false;	// 08 : Draw Defect Num 표시
	bool	bDrawDust			= (nCommonPara[	E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust 표시
	int		nPatternImageNum	= nCommonPara[	E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI 상 패턴 순서 영상 번호
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell 회전된 각도 ( Align 계산 값, 소수점 3자리 까지만... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell 회전된 중심 x좌표
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell 회전된 중심 y좌표
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 입력 된 갯수
	int& nDefectCount = pResultBlob->nDefectCount;

	// 최대 불량 갯수 넘은 경우 빠져 나오기
	if( nDefectCount >= nMaxDefectCount )
		return E_ERROR_CODE_TRUE;

	// 회전 좌표 계산 시, 사용
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );
	double	dSin_	= sin( -dTheta );
	double	dCos_	= cos( -dTheta );

	// 라벨 시작
	DoBlobCalculate(matThresholdImage, matSrcImage, nMaxDefectCount);

	// 선택한 Defect 리스트
	vector<int> nForks = nDefectType;
	int nFork=0;

	// Blob 갯수
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		for (int j=0; j<nForks.size(); j++)
		{
			nFork = nForks[j];
			
			// 불량 색상 필터링
			if( !DoColorFilter(nFork, nDefectColor) )
				continue;
			
			// Defect 리스트 선택한 경우만...
			if( !EngineerDefectJudgment[nFork].bDefectItemUse )
				continue;

			// 판정 항목당 범위 2개
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter	= true;
			bool bInit		= false;
			for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
			{
				// 판정 항목 선택한 경우만
				if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
					continue;

				// 한번이라도 동작 한 경우.
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
					// 설정한 필터링 만족하면 true / 만족하지 않으면 false 반환
					if(	!DoFiltering(
						m_BlobResult[i],										// Blob 결과
						nForj/2,												// 비교 Feature
						EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// 연산자 ( <, >, ==, <=, >= )
						EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// 값
					{
						bFilter = false;
						break;
					}
				}
			}

			// 설정한 모든 조건이 만족 하는 경우, 결과 입력
			if( bInit && bFilter )
			{
				m_BlobResult[i].bFiltering = true;

				// 꼭지점 정보
				if(bPtRotate)
				{
					int nL, nT, nR, nB;

					// 회전 시, 예측 좌표 계산
					int X = (int)( dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx );
					int Y = (int)( dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy );

					// 초기 설정
					nL = nR = X;
					nT = nB = Y;

					// 외곽선 갯수 만큼
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						// 회전 시, 예측 좌표 계산
						X = (int)( dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx );
						Y = (int)( dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy );

						// 갱신
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

					// 초기 설정
					nL = nR = X;
					nT = nB = Y;

					// 외곽선 갯수 만큼
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						X = m_BlobResult[i].ptContours[k].x; 
						Y = m_BlobResult[i].ptContours[k].y;

						// 갱신
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

				// UI에 넘겨줄 결과 넣기
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
				pResultBlob->nDefectJudge	[nDefectCount] = nFork;			// 해당 불량
				pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

#if USE_ALG_HIST
				// KSW 17.06.24 객체 히스토그램
				memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif
	
#if USE_ALG_CONTOURS
				// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
				calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

				// 불량 번호 그리기
				if( !matDrawBuffer.empty() && bDefectNum )
				{
					cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x-2, pResultBlob->ptRT[nDefectCount].y-10, 30, 12), cv::Scalar(0, 0, 0), -1);

					char str[256]={0,};
					sprintf_s(str, sizeof(str),"%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
					cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
					cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
				}

				// 마지막 불량 카운트 증가
				nDefectCount++;
			}

			// 최대 불량 갯수 넘은 경우 빠져 나오기
			if( nDefectCount >= nMaxDefectCount )
				break;
		}		
	}

	return E_ERROR_CODE_TRUE;
}

// Blob & 판정 결과
long CFeatureExtraction::DoDefectBlobJudgment(cv::Mat& matSrcImage, cv::Mat& matThresholdImage, cv::Mat& matDrawBuffer,
	int* nCommonPara, long nDefectColor, CString strTxt, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, bool bPtRotate, CRect prerectROI, int offset)
{
	// 시간 제한 시작
	cTimeOut.TimeCheckStart();

	// 메모리 해제
	Release();

	// 파라미터 NULL인 경우.
	if( nCommonPara == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( pResultBlob == NULL )			return E_ERROR_CODE_EMPTY_PARA;
	if( EngineerDefectJudgment == NULL)	return E_ERROR_CODE_EMPTY_PARA;

	// 영상 버퍼가 NULL인 경우
	if( matSrcImage.empty() )			return E_ERROR_CODE_EMPTY_BUFFER;
	if( matThresholdImage.empty() )		return E_ERROR_CODE_EMPTY_BUFFER;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	int		nMaxDefectCount		= nCommonPara[	E_PARA_COMMON_MAX_DEFECT_COUNT			];						// 00 : 불량 최대 갯수
	bool	bImageSave			= (nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_FLAG			] > 0) ? true : false;	// 01 : 알고리즘 중간결과 Image Save 
	int&	nSaveImageCount		= nCommonPara[	E_PARA_COMMON_IMAGE_SAVE_COUNT			];						// 02 : 영상 저장 순서 카운트
	int		nImageNum			= nCommonPara[	E_PARA_COMMON_ALG_IMAGE_NUMBER			];						// 03 : 현재 영상 번호
	int		nCamNum				= nCommonPara[	E_PARA_COMMON_CAMERA_NUMBER				];						// 04 : Cam Number
	int		nROINumber			= nCommonPara[	E_PARA_COMMON_ROI_NUMBER				];						// 05 : ROI Number
	int		nAlgorithmNumber	= nCommonPara[	E_PARA_COMMON_ALG_NUMBER				];						// 06 : 알고리즘 번호
	int		nThrdIndex			= nCommonPara[	E_PARA_COMMON_THREAD_ID					];						// 07 : Thread ID
	bool	bDefectNum			= (nCommonPara[	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG		] > 0) ? true : false;	// 08 : Draw Defect Num 표시
	bool	bDrawDust			= (nCommonPara[	E_PARA_COMMON_DRAW_DUST_FLAG			] > 0) ? true : false;	// 09 : Draw Dust 표시
	int		nPatternImageNum	= nCommonPara[	E_PARA_COMMON_UI_IMAGE_NUMBER			];						// 10 : UI 상 패턴 순서 영상 번호
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell 회전된 각도 ( Align 계산 값, 소수점 3자리 까지만... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell 회전된 중심 x좌표
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell 회전된 중심 y좌표
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	
	// 회전 좌표 계산 시, 사용
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );
	double	dSin_	= sin( -dTheta );
	double	dCos_	= cos( -dTheta );

	// 라벨 시작
	DoBlobCalculate(matThresholdImage, matSrcImage);

	// 불량 입력 된 갯수
	int& nDefectCount = pResultBlob->nDefectCount;

	// Blob 갯수
	for (int i=0 ; i<m_BlobResult.size() ; i++)
	{
		// 시간 제한 확인
		if( cTimeOut.GetTimeOutFlag() )	continue;

		// Defect 리스트 만큼
		for (int nFork = 0 ; nFork < E_DEFECT_JUDGEMENT_COUNT ; nFork++)
		{
			// Defect 리스트 선택한 경우만...
			if( !EngineerDefectJudgment[nFork].bDefectItemUse )
				continue;

			// 불량 색상 필터링
			if( !DoColorFilter(nFork, nDefectColor) )
				continue;

			// 판정 항목당 범위 2개
			int nFeatureCount = E_FEATURE_COUNT * 2;
			bool bFilter	= true;
			bool bInit		= false;
			for (int nForj = 0 ; nForj < nFeatureCount ; nForj++)
			{
				// 판정 항목 선택한 경우만

				if( !EngineerDefectJudgment[nFork].Judgment[nForj].bUse )
					continue;

				// 한번이라도 동작 한 경우.
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
					// 설정한 필터링 만족하면 true / 만족하지 않으면 false 반환
					if(	!DoFiltering(
							m_BlobResult[i],										// Blob 결과
							nForj/2,												// 비교 Feature
							EngineerDefectJudgment[nFork].Judgment[nForj].nSign,	// 연산자 ( <, >, ==, <=, >= )
							EngineerDefectJudgment[nFork].Judgment[nForj].dValue) )	// 값
					{
						bFilter = false;
						break;
					}
				}

			}

			// 설정한 모든 조건이 만족 하는 경우, 결과 입력
			if( bInit && bFilter )
			{
				m_BlobResult[i].bFiltering = true;				
				
				// 꼭지점 정보
				if(bPtRotate)
				{
					int nL, nT, nR, nB;

					// 회전 시, 예측 좌표 계산
					int X = (int)( dCos * (m_BlobResult[i].ptContours[0].x - nCx) - dSin * (m_BlobResult[i].ptContours[0].y - nCy) + nCx );
					int Y = (int)( dSin * (m_BlobResult[i].ptContours[0].x - nCx) + dCos * (m_BlobResult[i].ptContours[0].y - nCy) + nCy );
					
					// 초기 설정
					nL = nR = X;
					nT = nB = Y;
					
					// 외곽선 갯수 만큼
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						// 회전 시, 예측 좌표 계산
						X = (int)( dCos * (m_BlobResult[i].ptContours[k].x - nCx) - dSin * (m_BlobResult[i].ptContours[k].y - nCy) + nCx );
						Y = (int)( dSin * (m_BlobResult[i].ptContours[k].x - nCx) + dCos * (m_BlobResult[i].ptContours[k].y - nCy) + nCy );

						// 갱신
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

					// 초기 설정
					nL = nR = X;
					nT = nB = Y;

					// 외곽선 갯수 만큼
					for (int k=1 ; k < m_BlobResult[i].ptContours.size() ; k++)
					{
						X = m_BlobResult[i].ptContours[k].x; 
						Y = m_BlobResult[i].ptContours[k].y;

						// 갱신
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

				// UI에 넘겨줄 결과 넣기
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
				pResultBlob->nDefectJudge	[nDefectCount] = nFork;			// 해당 불량
				pResultBlob->nPatternClassify[nDefectCount]= nPatternImageNum;

#if USE_ALG_HIST
				// KSW 17.06.24 객체 히스토그램
				memcpy(pResultBlob->nHist[nDefectCount], m_BlobResult[i].nHist, sizeof(__int64) * IMAGE_MAX_GV);
#endif

#if USE_ALG_CONTOURS
				// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
				calcContours(pResultBlob->nContoursX[nDefectCount], pResultBlob->nContoursY[nDefectCount], i, fAngle, nCx, nCy, nPS);
#endif

				// 불량 번호 그리기
				if( !matDrawBuffer.empty() && bDefectNum )
				{
					cv::rectangle(matDrawBuffer, cv::Rect(pResultBlob->ptRT[nDefectCount].x-2, pResultBlob->ptRT[nDefectCount].y-10, 30, 12), cv::Scalar(0, 0, 0), -1);

					char str[256]={0,};
					sprintf_s(str, sizeof(str),"%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
					cv::Point ptRT(pResultBlob->ptRT[nDefectCount].x, pResultBlob->ptRT[nDefectCount].y);
					cv::putText(matDrawBuffer, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
				}

				// 마지막 불량 카운트 증가
				nDefectCount++;

				break;
			}
		}

		// 최대 불량 갯수 넘은 경우 빠져 나오기
		if( nDefectCount >= nMaxDefectCount )
			break;
	}

	// 시간 제한 확인
	if( cTimeOut.GetTimeOutFlag() )	return E_ERROR_CODE_TIME_OUT;

	return E_ERROR_CODE_TRUE;
}

//ssy 0718 외곽에 위치하는지 검사
bool CFeatureExtraction::is_edge(tBLOB_FEATURE& tBlobResult, int nBlobFilter, CRect prerectROI, int offset)
{
	// 필터링 된경우 제외
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
	// 영상이 없는 경우 리턴
	if( DrawBuffer.empty() )		return false;

	// Blob 실행 안한경우 리턴
	if( !m_bComplete )				return false;

	// Blob 결과 0개 리턴
	if( m_BlobResult.size() == 0 )	return true;

	// 옵션이 없는 경우 리턴
	if( nOption == 0 )				return true;

	int i, j;

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for (i=0 ; i<m_BlobResult.size() ; i++)
	{
		// 선택한것만 그리기
		if( !m_BlobResult[i].bFiltering && bSelect)	continue;

		// 회전 박스 그리기
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

		// 외곽 박스 그리기
		if( nOption & BLOB_DRAW_BOUNDING_BOX )
		{
			cv::Rect rect(m_BlobResult[i].rectBox);
			rect.x -= 5;
			rect.y -= 5;
			rect.width += 10;
			rect.height += 10;

			cv::rectangle(DrawBuffer, rect, DrawColor);
		}

		// Blob 객체 그리기
		if( nOption & BLOB_DRAW_BLOBS )
		{
			// Gray 인 경우
			if( DrawBuffer.channels() == 1 )
			{
				int nGrayColor = (int)(DrawColor.val[0] + DrawColor.val[1] + DrawColor.val[2]) / 3;

				for (j=0 ; j<m_BlobResult[i].ptIndexs.size() ; j++)
				{
					DrawBuffer.at<uchar>(m_BlobResult[i].ptIndexs[j].y, m_BlobResult[i].ptIndexs[j].x) = nGrayColor;
				}
			}
			// RGB 인 경우
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

		// Blob 외곽선 그리기
		if( nOption & BLOB_DRAW_BLOBS_CONTOUR )
		{
			// Gray 인 경우
			if( DrawBuffer.channels() == 1 )
			{
				int nGrayColor = (int)(DrawColor.val[0] + DrawColor.val[1] + DrawColor.val[2]) / 3;

				for (j=0 ; j<m_BlobResult[i].ptContours.size() ; j++)
				{
					DrawBuffer.at<uchar>(m_BlobResult[i].ptContours[j].y, m_BlobResult[i].ptContours[j].x) = nGrayColor;
				}
			}
			// RGB 인 경우
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
	// 예외 처리
	if( nDefectName<0 )								return false;
	if( nDefectName>=E_DEFECT_JUDGEMENT_COUNT )		return false;

	int nColor = -1;

	switch( nDefectName )
	{
	case E_DEFECT_JUDGEMENT_POINT_DARK:				// 암점
	case E_DEFECT_JUDGEMENT_POINT_DARK_SP_1:
	case E_DEFECT_JUDGEMENT_POINT_DARK_SP_2:
	case E_DEFECT_JUDGEMENT_POINT_DARK_SP_3:
	case E_DEFECT_JUDGEMENT_POINT_GROUP_DARK:		// 군짐 암점
	case E_DEFECT_JUDGEMENT_LINE_X_DARK:			// 어두운 X라인
	case E_DEFECT_JUDGEMENT_LINE_X_DARK_MULT:		// 어두운 X라인
	case E_DEFECT_JUDGEMENT_LINE_Y_DARK:			// 어두운 Y라인
	case E_DEFECT_JUDGEMENT_LINE_Y_DARK_MULT:		// 어두운 Y라인
	case E_DEFECT_JUDGEMENT_MURA_AMORPH_DARK:		// 어두운 무정형
	case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK:		// 어두운 약시인성 라인
	case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK:		// 어두운 약시인성 라인
	case E_DEFECT_JUDGEMENT_RETEST_POINT_DARK:		// 재검사 암점
	case E_DEFECT_JUDGEMENT_RETEST_LINE_DARK:		// 재검사 어두운 X라인
	case E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT:
	case E_DEFECT_JUDGEMENT_MURA_EMD_DARK:
	case E_DEFECT_JUDGEMENT_MURA_NUGI:
	case E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI:
	case E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_:
		nColor = E_DEFECT_COLOR_DARK;
		break;

	case E_DEFECT_JUDGEMENT_POINT_BRIGHT:			// 명점
	case E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT:		// 약명점
	case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:		// 군집 명점
	case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:			// 밝은 X라인
	case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT_MULT:		// 밝은 X라인
	case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:			// 밝은 Y라인
	case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT_MULT:		// 밝은 Y라인
	case E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT:		// 밝은 무정형
	case E_DEFECT_JUDGEMENT_LINE_X_EDGE_BRIGHT:
	case E_DEFECT_JUDGEMENT_LINE_Y_EDGE_BRIGHT:
	case E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT:
	case E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT:	// 밝은 약시인성 라인
	case E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT:	// 재검사 명점
	case E_DEFECT_JUDGEMENT_RETEST_LINE_BRIGHT:		// 재검사 밝은 X라인
	case E_DEFECT_JUDGEMENT_MURA_MULT_BP:			// KSW 17.09.27 - MURA_MULT_BP
	case E_DEFECT_JUDGEMENT_LINE_PCD_CRACK:			// PNZ 17.12.01 - 신 불량
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
	
	// 불량 색상 정해진 경우
	if( nColor != -1 )
	{
		// 설정한 색상과 동일한가?
		if( nColor == nDefectColor )
			return true;

		// 설정한 색상과 동일하지 않은가?
		else
			return false;
	}		

	// 불량 색상 정해지지 않은 경우.
	return true;
}

bool CFeatureExtraction::Compare(double dFeatureValue, int nSign, double dValue)
{
	bool bRes = false;

	// 연산자 ( <, >, ==, <=, >= )
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
	// 필터링 된경우 제외
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

// 외곽선 좌표 넘기기
// P/S 모드인 경우 : 축소하여 좌표 저장
// Align 했을 경우 영상으로 예측하여 좌표 저장
bool CFeatureExtraction::SaveTxt(int* nCommonPara, wchar_t* strContourTxt, bool bUse)
{
	// 외곽선 정보 -> AviInspection::JudgeSaveMuraContours() 이동
	// 불량 삭제 시, 표현 안되도록...
	if( !bUse )		return	true;

	// 파라미터 NULL인 경우.
	if( nCommonPara == NULL )			return false;

	// Blob 실행 안한경우 리턴
	if( !m_bComplete )					return false;

	// Blob 결과 0개 리턴
	if( m_BlobResult.size() == 0 )		return true;

	// 외곽선 저장 경로 없는 경우
	if( strContourTxt == NULL )			return false;

	//////////////////////////////////////////////////////////////////////////
	// 공통 파라미터
	float	fAngle				= nCommonPara[	E_PARA_COMMON_ROTATE_ANGLE				] / 1000.f;				// 11 : Cell 회전된 각도 ( Align 계산 값, 소수점 3자리 까지만... )
	int		nCx					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_X			];						// 12 : Cell 회전된 중심 x좌표
	int		nCy					= nCommonPara[	E_PARA_COMMON_ROTATE_CENTER_Y			];						// 13 : Cell 회전된 중심 y좌표
	int		nPS					= nCommonPara[	E_PARA_COMMON_PS_MODE					];						// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 회전 좌표 계산 시, 사용
	double dTheta = -fAngle * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );

	EnterCriticalSection(&m_csCoordFile);

	// TXT 저장
	CStdioFile	fileWriter;
	CString		strLine;

	// 파일 열기
	if( fileWriter.Open( strContourTxt, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite ) )
	{
		for (int i=0 ; i<m_BlobResult.size() ; i++)
		{
			// 선택한것만... ( 실제 불량 판정 난것만... )
			if( !m_BlobResult[i].bFiltering )	continue;

			// 외곽선 저장
			for (int j=0 ; j<m_BlobResult[i].ptContours.size() ; j++)
			{
				// 회전 시, 예측 좌표 계산
				int X = (int)( dCos * (m_BlobResult[i].ptContours[j].x - nCx) - dSin * (m_BlobResult[i].ptContours[j].y - nCy) + nCx );
				int Y = (int)( dSin * (m_BlobResult[i].ptContours[j].x - nCx) + dCos * (m_BlobResult[i].ptContours[j].y - nCy) + nCy );

				// P/S 모드에 따라 좌표 수정 ( 1샷 좌표로 변경 )
				strLine.Format(_T("%d, %d\n"), (int)X/nPS, (int)Y/nPS);

				fileWriter.SeekToEnd();
				fileWriter.WriteString(strLine);
			}
		}

		// 파일 열린 경우만 닫기
		fileWriter.Close();
	}

	LeaveCriticalSection(&m_csCoordFile);

	return true;
}


bool CFeatureExtraction::GetResultblob(vector<tBLOB_FEATURE>& OutBlob)
{
	// 벡터 초기화
	if( OutBlob.size() != 0 )
	{
		for (int i=0 ; i<OutBlob.size() ; i++)
		{
			vector<cv::Point>().swap(OutBlob[i].ptIndexs);
			vector <cv::Point>().swap(m_BlobResult[i].ptContours);
		}
		vector<tBLOB_FEATURE>().swap(OutBlob);
	}

	// Blob 실행 안한경우 리턴
	if( !m_bComplete )				return false;

	// Blob 결과 0개 리턴
	if( m_BlobResult.size() == 0 )	return true;

	OutBlob.resize( m_BlobResult.size() );

	copy( m_BlobResult.begin(), m_BlobResult.end(), OutBlob.begin() );

	return true;
}

bool CFeatureExtraction::DoFeatureBasic_8bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel, CMatBuf* cMemSub)
{
	// 결과가 1개라도 있는 경우
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
		// 시간 제한 확인
		if( cTimeOut.GetTimeOutFlag() )	continue;

		int nBlobNum = idx - 1;

		m_BlobResult.at(nBlobNum).rectBox.x			= matStats.at<int>(idx, CC_STAT_LEFT);
		m_BlobResult.at(nBlobNum).rectBox.y			= matStats.at<int>(idx, CC_STAT_TOP);
		m_BlobResult.at(nBlobNum).rectBox.width		= matStats.at<int>(idx, CC_STAT_WIDTH);
		m_BlobResult.at(nBlobNum).rectBox.height	= matStats.at<int>(idx, CC_STAT_HEIGHT);

		// 객체 주변 ( 배경 GV용도 )
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
		__int64 nSum_in		= 0;	//	defect 영역
		__int64 nSum_out	= 0;	//	defect 제외영역

		cv::Mat matTmp_src		= GrayBuffer( rectTemp );		//	원본의 ROI
		cv::Mat matTmp_label	= matLabel( rectTemp );			//	Label 의 ROI
		cv::Mat matTemp			= cMatBufTemp.GetMat(rectTemp.height, rectTemp.width, CV_8UC1);

		for(int  y=0 ; y<rectTemp.height ; y++)
		{
			int		*ptrLabel	= (int *)matTmp_label.ptr(y);
			uchar	*ptrGray	= (uchar *)matTmp_src.ptr(y);
			uchar	*ptrTemp	= (uchar *)matTemp.ptr(y);

			for(int x=0 ; x<rectTemp.width ; x++, ptrLabel++, ptrGray++, ptrTemp++)
			{
				// 해당 객체
				if( *ptrLabel == idx )
				{
					nSum_in += *ptrGray;
					nCount_in++;

					// 해당 라벨 벡터에 픽셀 좌표 저장
					m_BlobResult.at(nBlobNum).ptIndexs.push_back( cv::Point(nSX+x, nSY+y) );

					*ptrTemp = (uchar)255;

					m_BlobResult.at(nBlobNum).nHist[ *ptrGray ]++;
				}
				// 다른경우 배경
				else
				{
					// 라벨 번호가 0인 경우...
					// 다른 객체 참조될수 있으므로.... 조건 추가
					if( *ptrLabel == 0 )
					{
						nSum_out += *ptrGray;
						nCount_out++;
					}					
				}
			}
		}

		// 밝기 누적 값
		m_BlobResult.at(nBlobNum).nSumGV = nSum_in;

		// 객체 면적
		m_BlobResult.at(nBlobNum).nArea = nCount_in;	//matStats.at<int>(idx, CC_STAT_AREA);

		// Box Area
		m_BlobResult.at(nBlobNum).nBoxArea = m_BlobResult.at(nBlobNum).rectBox.width * m_BlobResult.at(nBlobNum).rectBox.height;

		// Bounding Box 면적 비율 / 객체 면적 ( Rectangularity(= Extent) )
		m_BlobResult.at(nBlobNum).fBoxRatio	= m_BlobResult.at(nBlobNum).nArea / (float)m_BlobResult.at(nBlobNum).nBoxArea;

		// Elongation 구하기
		m_BlobResult.at(nBlobNum).fElongation = m_BlobResult.at(nBlobNum).rectBox.width / (float)m_BlobResult.at(nBlobNum).rectBox.height;

		// stdDev 구하기
		cv::Scalar m, s;
		cv::meanStdDev(matTmp_src, m, s, matTemp);
		m_BlobResult.at(nBlobNum).fStdDev = float(s[0]);

		// Contours 구하기
		vector<vector<cv::Point>>	ptContours;
		vector<vector<cv::Point>>().swap(ptContours);
		cv::findContours(matTemp, ptContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		// Perimeter 구하기
		if( ptContours.size() != 0 )
		{
			// ROI 영상이므로, 보정 필요
			// 외곽 좌표 결과 복사
			for (int m=0 ; m<ptContours.size() ; m++)
			{
				for(int k=0 ; k<ptContours.at(m).size() ; k++)
					m_BlobResult.at(nBlobNum).ptContours.push_back( cv::Point(ptContours.at(m)[k].x + nSX, ptContours.at(m)[k].y + nSY) );
			}
		}
		else
		{
			// 원본 좌표이므로, 보정 안해도 됨.
			// 외곽 좌표 결과 복사
			m_BlobResult.at(nBlobNum).ptContours.resize( (int)m_BlobResult.at(nBlobNum).ptIndexs.size() );
			std::copy( m_BlobResult.at(nBlobNum).ptIndexs.begin(), m_BlobResult.at(nBlobNum).ptIndexs.end(), m_BlobResult.at(nBlobNum).ptContours.begin() );
		}
		m_BlobResult.at(nBlobNum).fPerimeter = float( cv::arcLength(m_BlobResult.at(nBlobNum).ptContours, true) );
		vector<vector<cv::Point>>().swap(ptContours);

		// Roundness 구하기
		m_BlobResult.at(nBlobNum).fRoundness = (fVal * m_BlobResult.at(nBlobNum).nArea )
			/ (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter);

		// 객체가 원 모양에 얼마나 가까운가? ( 둘레^2 / 4 * Pi * 면적 )
		m_BlobResult.at(nBlobNum).fCompactness = (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter)
			/ ( fVal * float( m_BlobResult.at(nBlobNum).nArea ) );

		// Defect GV 구하기
		m_BlobResult.at(nBlobNum).fMeanGV = nSum_in / (float)nCount_in;

		//	배경 GV 구하기
		m_BlobResult.at(nBlobNum).fBKGV = nSum_out / (float)nCount_out;

		// GV 차이값 구하기 ( 배경 - 객체 )
		m_BlobResult.at(nBlobNum).fDiffGV = m_BlobResult.at(nBlobNum).fBKGV - m_BlobResult.at(nBlobNum).fMeanGV;

		// min, max GV 구하기
		double valMin, valMax;
		cv::minMaxLoc(matTmp_src, &valMin, &valMax, 0, 0, matTemp);
		m_BlobResult.at(nBlobNum).nMinGV = (long)valMin;
		m_BlobResult.at(nBlobNum).nMaxGV = (long)valMax;

		// 객체 최소 밝기 / 객체 평균 밝기
		m_BlobResult.at(nBlobNum).nMinGVRatio = m_BlobResult.at(nBlobNum).nMinGV / m_BlobResult.at(nBlobNum).fBKGV;

		// 객체 최대 밝기 / 객체 평균 밝기
		m_BlobResult.at(nBlobNum).nMaxGVRatio = m_BlobResult.at(nBlobNum).nMaxGV / m_BlobResult.at(nBlobNum).fBKGV;

		//      배경 밝기 / 객체 평균 밝기
		m_BlobResult.at(nBlobNum).fDiffGVRatio = m_BlobResult.at(nBlobNum).fMeanGV / m_BlobResult.at(nBlobNum).fBKGV;

		// Center Point 구하기
		m_BlobResult.at(nBlobNum).ptCenter.x = (int)matCentroid.at<double>(idx, 0);
		m_BlobResult.at(nBlobNum).ptCenter.y = (int)matCentroid.at<double>(idx, 1);

		// SEMU 구하기
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

		// 회전된 사각형 꼭지점 4개
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

		// Feret’s area
		m_BlobResult.at(nBlobNum).fMinBoxArea = m_BlobResult.at(nBlobNum).fMinorAxis * m_BlobResult.at(nBlobNum).fMajorAxis;

		// Axis Ratio
		if( m_BlobResult.at(nBlobNum).fMajorAxis > 0 )
			m_BlobResult.at(nBlobNum).fAxisRatio		= m_BlobResult.at(nBlobNum).fMinorAxis / m_BlobResult.at(nBlobNum).fMajorAxis;
		else
			m_BlobResult.at(nBlobNum).fAxisRatio		= 0.f;

		// Min Bounding Box 면적 비율 / 객체 면적 ( Area porosity )
		m_BlobResult.at(nBlobNum).fMinBoxRatio	= m_BlobResult.at(nBlobNum).fMinBoxArea / (float)m_BlobResult.at(nBlobNum).nArea;
		//choikwangil
		m_BlobResult.at(nBlobNum).fMeanAreaRatio = m_BlobResult.at(nBlobNum).fMeanGV / (float)m_BlobResult.at(nBlobNum).nArea;
		// 할당 해제
		matTmp_src.release();
		matTmp_label.release();
		matTemp.release();
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}

	// 시간 제한 확인
	if( cTimeOut.GetTimeOutFlag() )	return false;

	return true;
}

bool CFeatureExtraction::DoFeatureBasic_8bit_LCP(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer,cv::Mat& ColorBuffer, double* dPara, int nTotalLabel, CMatBuf* cMemSub)
{
	// 결과가 1개라도 있는 경우
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
	//LAB 검사 사용
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matThresh2 = cv::Mat::zeros(GrayBuffer.size(), CV_8UC1);

	cv::Mat matRGB[3];

	cv::Mat matDefectBuff_Color2 = ColorBuffer.clone();

	split(matDefectBuff_Color2, matRGB);

	for (int i = 0; i < 3; i++)
	{
		cv::GaussianBlur(matRGB[i], matRGB[i], cv::Size(31, 31), 1.0);

		// 이미지의 1/5 크기로 ROI 만들기
		int nROIWidth = matDefectBuff_Color2.cols / 5 / 2;
		int nROIHeight = matDefectBuff_Color2.rows / 5 / 2;

		Rect rectRGBROI;
		rectRGBROI.x = matDefectBuff_Color2.cols / 2 - nROIWidth;
		rectRGBROI.y = matDefectBuff_Color2.rows / 2 - nROIHeight;
		rectRGBROI.width = nROIWidth * 2;
		rectRGBROI.height = nROIHeight * 2;

		cv::Mat matMask = matRGB[i](rectRGBROI).clone();
		cv::Mat matMask2 = matRGB[i](rectRGBROI).clone();
		// 라벨 등 평균을 구하는데 방해되는 요소 있을 수 있으므로 제거
		cv::threshold(matMask, matMask, 200, 255, THRESH_BINARY_INV);
		cv::threshold(matMask2, matMask2, 100, 255, THRESH_BINARY);

		cv::bitwise_and(matMask, matMask2, matMask);

		double dMean = cv::mean(matRGB[i](rectRGBROI), matMask)[0];

		int nValue = 126 - dMean;

		matRGB[i] += nValue;
	}

	cv::merge(matRGB, 3, matDefectBuff_Color2);
	//cv::imwrite("D:\\이미지 보정.jpg", matDefectBuff_Color2);

	//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
	for( int idx=1 ; idx<=nTotalLabel ; idx++ )
	{
		// 시간 제한 확인
		if( cTimeOut.GetTimeOutFlag() )	continue;

		int nBlobNum = idx - 1;

		m_BlobResult.at(nBlobNum).rectBox.x			= matStats.at<int>(idx, CC_STAT_LEFT);
		m_BlobResult.at(nBlobNum).rectBox.y			= matStats.at<int>(idx, CC_STAT_TOP);
		m_BlobResult.at(nBlobNum).rectBox.width		= matStats.at<int>(idx, CC_STAT_WIDTH);
		m_BlobResult.at(nBlobNum).rectBox.height	= matStats.at<int>(idx, CC_STAT_HEIGHT);

		// 객체 주변 ( 배경 GV용도 )
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
		__int64 nSum_in		= 0;	//	defect 영역
		__int64 nSum_out	= 0;	//	defect 제외영역

		cv::Mat matTmp_src		= GrayBuffer( rectTemp );		//	원본의 ROI
		cv::Mat matTmp_label	= matLabel( rectTemp );			//	Label 의 ROI
		cv::Mat matTemp			= cMatBufTemp.GetMat(rectTemp.height, rectTemp.width, CV_8UC1);

		for(int  y=0 ; y<rectTemp.height ; y++)
		{
			int		*ptrLabel = (int *)matTmp_label.ptr(y);
			uchar	*ptrGray = (uchar *)matTmp_src.ptr(y);
			uchar	*ptrTemp = (uchar *)matTemp.ptr(y);

			for(int x=0 ; x<rectTemp.width ; x++, ptrLabel++, ptrGray++, ptrTemp++)
			{
				// 해당 객체
				if( *ptrLabel == idx )
				{
					nSum_in += *ptrGray;
					nCount_in++;

					// 해당 라벨 벡터에 픽셀 좌표 저장
					m_BlobResult.at(nBlobNum).ptIndexs.push_back( cv::Point(nSX+x, nSY+y) );

					*ptrTemp = (uchar)255;

					m_BlobResult.at(nBlobNum).nHist[*ptrGray]++;
				}
				// 다른경우 배경
				else
				{
					// 라벨 번호가 0인 경우...
					// 다른 객체 참조될수 있으므로.... 조건 추가
					if( *ptrLabel == 0 )
					{
						nSum_out += *ptrGray;
						nCount_out++;
					}
				}
			}
		}

		// 밝기 누적 값
		m_BlobResult.at(nBlobNum).nSumGV = nSum_in;

		// 객체 면적
		m_BlobResult.at(nBlobNum).nArea = nCount_in;	//matStats.at<int>(idx, CC_STAT_AREA);

		// Box Area
		m_BlobResult.at(nBlobNum).nBoxArea = m_BlobResult.at(nBlobNum).rectBox.width * m_BlobResult.at(nBlobNum).rectBox.height;

		// Bounding Box 면적 비율 / 객체 면적 ( Rectangularity(= Extent) )
		m_BlobResult.at(nBlobNum).fBoxRatio	= m_BlobResult.at(nBlobNum).nArea / (float)m_BlobResult.at(nBlobNum).nBoxArea;

		// Elongation 구하기
		m_BlobResult.at(nBlobNum).fElongation = m_BlobResult.at(nBlobNum).rectBox.width / (float)m_BlobResult.at(nBlobNum).rectBox.height;

		// stdDev 구하기
		cv::Scalar m, s;
		cv::meanStdDev(matTmp_src, m, s, matTemp);
		m_BlobResult.at(nBlobNum).fStdDev = float(s[0]);

		// Contours 구하기
		vector<vector<cv::Point>>	ptContours;
		vector<vector<cv::Point>>().swap(ptContours);
		cv::findContours(matTemp, ptContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		// Perimeter 구하기
		if( ptContours.size() != 0 )
		{
			// ROI 영상이므로, 보정 필요
			// 외곽 좌표 결과 복사
			for (int m=0 ; m<ptContours.size() ; m++)
			{
				for(int k=0 ; k<ptContours.at(m).size() ; k++)
					m_BlobResult.at(nBlobNum).ptContours.push_back( cv::Point(ptContours.at(m)[k].x + nSX, ptContours.at(m)[k].y + nSY) );
			}
		}
		else
		{
			// 원본 좌표이므로, 보정 안해도 됨.
			// 외곽 좌표 결과 복사
			m_BlobResult.at(nBlobNum).ptContours.resize( (int)m_BlobResult.at(nBlobNum).ptIndexs.size() );
			std::copy( m_BlobResult.at(nBlobNum).ptIndexs.begin(), m_BlobResult.at(nBlobNum).ptIndexs.end(), m_BlobResult.at(nBlobNum).ptContours.begin() );
		}
		m_BlobResult.at(nBlobNum).fPerimeter = float( cv::arcLength(m_BlobResult.at(nBlobNum).ptContours, true) );
		vector<vector<cv::Point>>().swap(ptContours);

		// Roundness 구하기
		m_BlobResult.at(nBlobNum).fRoundness = (fVal * m_BlobResult.at(nBlobNum).nArea )
			/ (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter);

		// 객체가 원 모양에 얼마나 가까운가? ( 둘레^2 / 4 * Pi * 면적 )
		m_BlobResult.at(nBlobNum).fCompactness = (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter)
			/ ( fVal * float( m_BlobResult.at(nBlobNum).nArea ) );

		// Defect GV 구하기
		m_BlobResult.at(nBlobNum).fMeanGV = nSum_in / (float)nCount_in;

		//	배경 GV 구하기
		m_BlobResult.at(nBlobNum).fBKGV = nSum_out / (float)nCount_out;

		// GV 차이값 구하기 ( 배경 - 객체 )
		m_BlobResult.at(nBlobNum).fDiffGV = m_BlobResult.at(nBlobNum).fBKGV - m_BlobResult.at(nBlobNum).fMeanGV;

		// min, max GV 구하기
		double valMin, valMax;
		cv::minMaxLoc(matTmp_src, &valMin, &valMax, 0, 0, matTemp);
		m_BlobResult.at(nBlobNum).nMinGV = (long)valMin;
		m_BlobResult.at(nBlobNum).nMaxGV = (long)valMax;

		// 객체 최소 밝기 / 객체 평균 밝기
		m_BlobResult.at(nBlobNum).nMinGVRatio = m_BlobResult.at(nBlobNum).nMinGV / m_BlobResult.at(nBlobNum).fBKGV;

		// 객체 최대 밝기 / 객체 평균 밝기
		m_BlobResult.at(nBlobNum).nMaxGVRatio = m_BlobResult.at(nBlobNum).nMaxGV / m_BlobResult.at(nBlobNum).fBKGV;

		//      배경 밝기 / 객체 평균 밝기
		m_BlobResult.at(nBlobNum).fDiffGVRatio = m_BlobResult.at(nBlobNum).fMeanGV / m_BlobResult.at(nBlobNum).fBKGV;

		// Center Point 구하기
		m_BlobResult.at(nBlobNum).ptCenter.x = (int)matCentroid.at<double>(idx, 0);
		m_BlobResult.at(nBlobNum).ptCenter.y = (int)matCentroid.at<double>(idx, 1);

		// SEMU 구하기
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
		
		// 회전된 사각형 꼭지점 4개
		//cv::Point2f vertices[4];
		//BoundingBox.points(vertices);

		// Box width and length
		m_BlobResult.at(nBlobNum).BoxSize		= BoundingBox.size;

		// Angle between the horizontal axis
		m_BlobResult.at(nBlobNum).fAngle		= BoundingBox.angle;

		// minAreaRect의 경우 각도에 따라 Width와 Height가 변경되는 문제가 있어
		// 사람이 통상적으로 생각하는 경우로 변경
		// 각도도 이상하긴한데.. 이건 지금 사용하지 않아서 나중에 변경예정
		// 참고 blog https://namkeenman.wordpress.com/tag/minarearect/
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

		// Feret’s area
		m_BlobResult.at(nBlobNum).fMinBoxArea = m_BlobResult.at(nBlobNum).fMinorAxis * m_BlobResult.at(nBlobNum).fMajorAxis;

		// Axis Ratio
		if( m_BlobResult.at(nBlobNum).fMajorAxis > 0 )
			m_BlobResult.at(nBlobNum).fAxisRatio		= m_BlobResult.at(nBlobNum).fMinorAxis / m_BlobResult.at(nBlobNum).fMajorAxis;
		else
			m_BlobResult.at(nBlobNum).fAxisRatio		= 0.f;

		// Min Bounding Box 면적 비율 / 객체 면적 ( Area porosity )
		m_BlobResult.at(nBlobNum).fMinBoxRatio	= ((float)m_BlobResult.at(nBlobNum).nArea / (float)m_BlobResult.at(nBlobNum).fMinBoxArea) *100;
		//choikwangil
		m_BlobResult.at(nBlobNum).fMeanAreaRatio = m_BlobResult.at(nBlobNum).fMeanGV / (float)m_BlobResult.at(nBlobNum).nArea;
		
		// 불량 최대 최소값 차이
		m_BlobResult.at(nBlobNum).fMax_Min = m_BlobResult.at(nBlobNum).nMaxGV - m_BlobResult.at(nBlobNum).nMinGV;

		m_BlobResult.at(nBlobNum).fRsubC = abs(m_BlobResult.at(nBlobNum).fRoundness - m_BlobResult.at(nBlobNum).fCompactness);

		m_BlobResult.at(nBlobNum).fMean_Min = m_BlobResult.at(nBlobNum).fMeanGV - (float)m_BlobResult.at(nBlobNum).nMinGV;


		
		
		//////////////////////////////////////////////////////////////////////////
		int nOffSet_Color = 1;			// 불량보다 크게 ROI을 설정할때 몇 Pixel 더 크게 잡을지 설정해주는 값

		Rect rectROI;

		// 시작 x좌표
		rectROI.x = m_BlobResult.at(nBlobNum).rectBox.x - nOffSet_Color;
		rectROI.y = m_BlobResult.at(nBlobNum).rectBox.y - nOffSet_Color;
		rectROI.width = m_BlobResult.at(nBlobNum).rectBox.width + nOffSet_Color * 2;
		rectROI.height = m_BlobResult.at(nBlobNum).rectBox.height + nOffSet_Color * 2;

		if (rectROI.x < 0) rectROI.x = 0;
		if (rectROI.y < 0) rectROI.y = 0;
		if (rectROI.width + rectROI.x > matDefectBuff_Color2.cols) rectROI.width = (matDefectBuff_Color2.cols)-rectROI.x;
		if (rectROI.height + rectROI.y > matDefectBuff_Color2.rows) rectROI.height = (matDefectBuff_Color2.rows)-rectROI.y;
		// 해당 위치 이미지 잘라내기
		cv::Mat matDefectBuff_Color = matDefectBuff_Color2(rectROI).clone();			// 해당 부분 메모리를 선할당 해야 하는가??
		cv::Mat matGray = GrayBuffer(rectROI).clone();								// Gray 영상
		cv::Mat matDefectBuff_Thresh = matThresh2(rectROI);

		double dMeanGV = m_BlobResult.at(nBlobNum).fMeanGV;


		cv::threshold(matGray, matDefectBuff_Thresh, dMeanGV - 5, 255, THRESH_BINARY_INV);



		// 이미지 Lab 변환 -> Vector 접근을 이용하면 matLab 사용 안해도 될수 있음 메모리를 생각해서 조사해봐야함
		cv::Mat matLab[3];

		matDefectBuff_Color.convertTo(matDefectBuff_Color, CV_32FC3);			// float 형으로 변환
		matDefectBuff_Color *= 1.0 / 255.0;										// 소수점으로 변환

		cv::cvtColor(matDefectBuff_Color, matDefectBuff_Color, CV_BGR2Lab);		// Lab 변환
		cv::split(matDefectBuff_Color, matLab);									// Lab 분리



																				// 불량의 평균 Lab값 구하기
		double dMeanL = cv::mean(matLab[0], matDefectBuff_Thresh)[0];
		double dMeanA = cv::mean(matLab[1], matDefectBuff_Thresh)[0];
		double dMeanB = cv::mean(matLab[2], matDefectBuff_Thresh)[0];


		// 색구분
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
		
		// 할당 해제
		matTmp_src.release();
		matTmp_label.release();
		matTemp.release();
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}

	// 시간 제한 확인
	if (cTimeOut.GetTimeOutFlag())	return false;

	return true;
}


bool CFeatureExtraction::DoFeatureBasic_16bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel, CMatBuf* cMemSub)
{
	// 결과가 1개라도 있는 경우
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
		// 시간 제한 확인
		if( cTimeOut.GetTimeOutFlag() )	continue;

		int nBlobNum = idx - 1;

		m_BlobResult.at(nBlobNum).rectBox.x			= matStats.at<int>(idx, CC_STAT_LEFT);
		m_BlobResult.at(nBlobNum).rectBox.y			= matStats.at<int>(idx, CC_STAT_TOP);
		m_BlobResult.at(nBlobNum).rectBox.width		= matStats.at<int>(idx, CC_STAT_WIDTH);
		m_BlobResult.at(nBlobNum).rectBox.height	= matStats.at<int>(idx, CC_STAT_HEIGHT);

		// 객체 주변 ( 배경 GV용도 )
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
		__int64 nSum_in		= 0;	//	defect 영역
		__int64 nSum_out	= 0;	//	defect 제외영역

		cv::Mat matTmp_src		= GrayBuffer( rectTemp );		//	원본의 ROI
		cv::Mat matTmp_label	= matLabel( rectTemp );			//	Label 의 ROI
		cv::Mat matTemp			= cMatBufTemp.GetMat(rectTemp.height, rectTemp.width, CV_8UC1);

		for(int  y=0 ; y<rectTemp.height ; y++)
		{
			int		*ptrLabel	= (int *)matTmp_label.ptr(y);
			ushort	*ptrGray	= (ushort *)matTmp_src.ptr(y);
			uchar	*ptrTemp	= (uchar *)matTemp.ptr(y);

			for(int x=0 ; x<rectTemp.width ; x++, ptrLabel++, ptrGray++, ptrTemp++)
			{
				// 해당 객체
				if( *ptrLabel == idx )
				{
					nSum_in += *ptrGray;
					nCount_in++;

					// 해당 라벨 벡터에 픽셀 좌표 저장
					m_BlobResult.at(nBlobNum).ptIndexs.push_back( cv::Point(nSX+x, nSY+y) );

					*ptrTemp = (uchar)255;

					m_BlobResult.at(nBlobNum).nHist[ (int)(*ptrGray) ]++;					
				}
				// 다른경우 배경
				else
				{
					// 라벨 번호가 0인 경우...
					// 다른 객체 참조될수 있으므로.... 조건 추가
					if( *ptrLabel == 0 )
					{
						nSum_out += *ptrGray;
						nCount_out++;
					}
				}
			}
		}

		// 밝기 누적 값
		m_BlobResult.at(nBlobNum).nSumGV = nSum_in;

		// 객체 면적
		m_BlobResult.at(nBlobNum).nArea = nCount_in;	//matStats.at<int>(idx, CC_STAT_AREA);

		// Box Area
		m_BlobResult.at(nBlobNum).nBoxArea = m_BlobResult.at(nBlobNum).rectBox.width * m_BlobResult.at(nBlobNum).rectBox.height;

		// Bounding Box 면적 비율 / 객체 면적 ( Rectangularity(= Extent) )
		m_BlobResult.at(nBlobNum).fBoxRatio	= m_BlobResult.at(nBlobNum).nArea / (float)m_BlobResult.at(nBlobNum).nBoxArea;

		// Elongation 구하기
		m_BlobResult.at(nBlobNum).fElongation = m_BlobResult.at(nBlobNum).rectBox.width / (float)m_BlobResult.at(nBlobNum).rectBox.height;

		// stdDev 구하기
		cv::Scalar m, s;
		cv::meanStdDev(matTmp_src, m, s, matTemp);
		m_BlobResult.at(nBlobNum).fStdDev = float(s[0]);

		// Contours 구하기
		vector<vector<cv::Point>>	ptContours;
		vector<vector<cv::Point>>().swap(ptContours);
		cv::findContours(matTemp, ptContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		// Perimeter 구하기
		if( ptContours.size() != 0 )
		{
			// ROI 영상이므로, 보정 필요
			// 외곽 좌표 결과 복사
			for (int m=0 ; m<ptContours.size() ; m++)
			{
				for(int k=0 ; k<ptContours.at(m).size() ; k++)
					m_BlobResult.at(nBlobNum).ptContours.push_back( cv::Point(ptContours.at(m)[k].x + nSX, ptContours.at(m)[k].y + nSY) );
			}
		}
		else
		{
			// 원본 좌표이므로, 보정 안해도 됨.
			// 외곽 좌표 결과 복사
			m_BlobResult.at(nBlobNum).ptContours.resize( (int)m_BlobResult.at(nBlobNum).ptIndexs.size() );
			std::copy( m_BlobResult.at(nBlobNum).ptIndexs.begin(), m_BlobResult.at(nBlobNum).ptIndexs.end(), m_BlobResult.at(nBlobNum).ptContours.begin() );
		}
		m_BlobResult.at(nBlobNum).fPerimeter = float( cv::arcLength(m_BlobResult.at(nBlobNum).ptContours, true) );
		vector<vector<cv::Point>>().swap(ptContours);

		// Roundness 구하기
		m_BlobResult.at(nBlobNum).fRoundness = (fVal * m_BlobResult.at(nBlobNum).nArea )
			/ (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter);

		// 객체가 원 모양에 얼마나 가까운가? ( 둘레^2 / 4 * Pi * 면적 )
		m_BlobResult.at(nBlobNum).fCompactness = (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter)
			/ ( fVal * float( m_BlobResult.at(nBlobNum).nArea ) );

		// Defect GV 구하기
		m_BlobResult.at(nBlobNum).fMeanGV = nSum_in / (float)nCount_in;

		//	배경 GV 구하기
		m_BlobResult.at(nBlobNum).fBKGV = nSum_out / (float)nCount_out;

		// GV 차이값 구하기 ( 배경 - 객체 )
		m_BlobResult.at(nBlobNum).fDiffGV = m_BlobResult.at(nBlobNum).fBKGV - m_BlobResult.at(nBlobNum).fMeanGV;

		// min, max GV 구하기
		double valMin, valMax;
		cv::minMaxLoc(matTmp_src, &valMin, &valMax, 0, 0, matTemp);
		m_BlobResult.at(nBlobNum).nMinGV = (long)valMin;
		m_BlobResult.at(nBlobNum).nMaxGV = (long)valMax;

		// 객체 최소 밝기 / 객체 평균 밝기
		m_BlobResult.at(nBlobNum).nMinGVRatio = m_BlobResult.at(nBlobNum).nMinGV / m_BlobResult.at(nBlobNum).fBKGV;

		// 객체 최대 밝기 / 객체 평균 밝기
		m_BlobResult.at(nBlobNum).nMaxGVRatio = m_BlobResult.at(nBlobNum).nMaxGV / m_BlobResult.at(nBlobNum).fBKGV;

		//      배경 밝기 / 객체 평균 밝기
		m_BlobResult.at(nBlobNum).fDiffGVRatio = m_BlobResult.at(nBlobNum).fMeanGV / m_BlobResult.at(nBlobNum).fBKGV;

		// Center Point 구하기
		m_BlobResult.at(nBlobNum).ptCenter.x = (int)matCentroid.at<double>(idx, 0);
		m_BlobResult.at(nBlobNum).ptCenter.y = (int)matCentroid.at<double>(idx, 1);

		// SEMU 구하기
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

		// 회전된 사각형 꼭지점 4개
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

		// Feret’s area
		m_BlobResult.at(nBlobNum).fMinBoxArea = m_BlobResult.at(nBlobNum).fMinorAxis * m_BlobResult.at(nBlobNum).fMajorAxis;

		// Axis Ratio
		if( m_BlobResult.at(nBlobNum).fMajorAxis > 0 )
			m_BlobResult.at(nBlobNum).fAxisRatio		= m_BlobResult.at(nBlobNum).fMinorAxis / m_BlobResult.at(nBlobNum).fMajorAxis;
		else
			m_BlobResult.at(nBlobNum).fAxisRatio		= 0.f;

		// Min Bounding Box 면적 비율 / 객체 면적 ( Area porosity )
		m_BlobResult.at(nBlobNum).fMinBoxRatio = m_BlobResult.at(nBlobNum).fMinBoxArea / (float)m_BlobResult.at(nBlobNum).nArea;
		//choikwangil
		m_BlobResult.at(nBlobNum).fMeanAreaRatio = m_BlobResult.at(nBlobNum).fMeanGV / (float)m_BlobResult.at(nBlobNum).nArea;
		// 할당 해제
		matTmp_src.release();
		matTmp_label.release();
		matTemp.release();
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}
	// 시간 제한 확인
	if( cTimeOut.GetTimeOutFlag() )	return false;

	return true;
}

// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
bool CFeatureExtraction::calcContours(int* nContoursX, int* nContoursY, int nDefectIndex, float fAngle, int nCx, int nCy, int nPs)
{
	// Blob 실행 안한경우 리턴
	if( !m_bComplete )					return false;

	// Blob 결과 0개 리턴
	if( m_BlobResult.size() == 0 )		return true;

	// 잘못된 파라미터 입력된 경우
	if( nDefectIndex < 0 )						return false;
	if( nDefectIndex >= m_BlobResult.size() )	return false;

	//////////////////////////////////////////////////////////////////////////

	// 외곽선 개수
	int nContoursCount = m_BlobResult[nDefectIndex].ptContours.size();

	// 회전 좌표 계산 시, 사용
	double dTheta = -0.0 * PI / 180.;
	double	dSin	= sin( dTheta );
	double	dCos	= cos( dTheta );

	float fRatio = 1.0;

	// 설정 개수보다 많은 경우
	if( nContoursCount >= MAX_CONTOURS )
	{
		fRatio = nContoursCount / (float)MAX_CONTOURS;
		nContoursCount = MAX_CONTOURS;
	}

	// 외곽선 저장
	for (int j=0 ; j<nContoursCount; j++)
	{
		// 실제 사용할 외곽선 Index
		int i = (int)(j * fRatio);

		// 회전 시, 예측 좌표 계산
		//int X = (int)( dCos * (m_BlobResult[nDefectIndex].ptContours[i].x - nCx) - dSin * (m_BlobResult[nDefectIndex].ptContours[i].y - nCy) + nCx );
		//int Y = (int)( dSin * (m_BlobResult[nDefectIndex].ptContours[i].x - nCx) + dCos * (m_BlobResult[nDefectIndex].ptContours[i].y - nCy) + nCy );

		int X = (int)(dCos * (m_BlobResult[nDefectIndex].ptContours[i].x - nCx) - dSin * (m_BlobResult[nDefectIndex].ptContours[i].y - nCy) + nCx);
		int Y = (int)(dSin * (m_BlobResult[nDefectIndex].ptContours[i].x - nCx) + dCos * (m_BlobResult[nDefectIndex].ptContours[i].y - nCy) + nCy);

		// P/S 모드에 따라 좌표 수정 ( 1샷 좌표로 변경 )
		nContoursX[j] = (int)(X / nPs);
		nContoursY[j] = (int)(Y / nPs);
	}

	return true;
}