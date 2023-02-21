
/************************************************************************/
// Point 불량 검출 관련 헤더
// 수정일 : 18.02.07 - KSW
/************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "ColorCorrection.h"
#include "InspectLibLog.h"

//////////////////////////////////////////////////////////////////////////

// 파라미터
enum ENUM_PARA_SVI_MURA
{
	E_PARA_SVI_MURA_OUTLINE_TEXT			= 0	,	// 텍스트 표시
	E_PARA_SVI_MURA_DELETE_PIXEL				,	// 외곽 라인 불량 제거

	E_PARA_SVI_MURA_HS_TEXT						,	// 텍스트 표시
	E_PARA_SVI_MURA_HS_GB_SIZE					,	// 가우시안 마스크 크기
	E_PARA_SVI_MURA_HS_GB_SIGMA					,	// 가우시안 시그마
	E_PARA_SVI_MURA_HS_AXIS_X					,	// 타원 x축
	E_PARA_SVI_MURA_HS_AXIS_Y					,	// 타원 y축

	E_PARA_SVI_MURA_HS_STANDARD_AXIS_X			,	// 기준 타원 x축
	E_PARA_SVI_MURA_HS_STANDARD_AXIS_Y			,	// 기준 타원 y축

	E_PARA_SVI_MURA_HS_CENTER_X					,	// 중심 영역 x좌표
	E_PARA_SVI_MURA_HS_CENTER_Y					,	// 중심 영역 y좌표
	E_PARA_SVI_MURA_HS_CENTER_OFFSET			,	// 중심 영역 최대 보정 영역 (원 반지름)
	E_PARA_SVI_MURA_HS_ANGLE					,	// 타원 회전 각도
	E_PARA_SVI_MURA_HS_ANGLE_OFFSET				,	// 타원 회전 각도 보정

	E_PARA_SVI_MURA_EMD_TEXT					,	// 텍스트 표시
	E_PARA_SVI_MURA_EMD_GB_SIZE					,	// 가우시안 마스크 크기
	E_PARA_SVI_MURA_EMD_GB_SIGMA				,	// 가우시안 시그마
	E_PARA_SVI_MURA_EMD_THRESHOLD				,	// EMD 불량 이진화
	E_PARA_SVI_MURA_EMD_DIM_X					,	// 가로 배경 만들기
	E_PARA_SVI_MURA_EMD_DIM_Y					,	// 세로 배경 만들기
	E_PARA_SVI_MURA_EMD_STEP_X					,	// 가로 배경 스텝
	E_PARA_SVI_MURA_EMD_STEP_Y					,	// 세로 배경 스텝
	E_PARA_SVI_MURA_EMD_JUDGM_USE				,	// 판정 사용 유/무
	E_PARA_SVI_MURA_EMD_JUDGM_SIZE				,	// Edge로 부터 범위 설정

	E_PARA_SVI_MURA_EMD_GB_AV_OFFSET			,   // 평균에서 배경추출옵셋

	E_PARA_SVI_MURA_EMD_LEFT_PIXEL			    ,   // 영상 기준 왼쪽 구분 지역
	E_PARA_SVI_MURA_EMD_LEFT_THRESHOLD			,   // 영상 기준 왼쪽 입계값

	E_PARA_SVI_MURA_EMD_EDGE_PIXEL			    ,   // EDGE 구분 지역
	E_PARA_SVI_MURA_EMD_EDGE_THRESHOLD			,   // EDGE 지역 입계값

	E_PARA_SVI_LINE_TH						    ,   // LINE 검출 입계값
	E_PARA_SVI_LINE_THICKNESS    			        ,   // LINE 검출 범위

	E_PARA_SVI_LINE_LENGTH    			        ,   // LINE 검출 길이

	E_PARA_SVI_MURA_TOTAL_COUNT						// Total
};


//////////////////////////////////////////////////////////////////////////


class CInspectSVI
{
public:
	CInspectSVI(void);
	virtual ~CInspectSVI(void);

	// 로그 관리
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

	// Main 검사 알고리즘
	long	DoFindMuraDefect(cv::Mat matSrcBuf, cv::Mat matBKBuf, cv::Mat& matDrawBuf, cv::Point* ptCorner, double* dPara, int* nCommonPara,
				wchar_t strPath[][1024], STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CColorCorrection *cColor=NULL);

	//////////////////////////////////////////////////////////////////////////

protected:
	
	// 혼색 불량 검출 로직
	long	LogicStart_HS(cv::Mat& matSrcBuf, cv::Mat &matBKBuf, cv::Mat& matDrawBuf, CRect rectROI, double* dPara, int* nCommonPara,
				wchar_t _strPath[][1024], STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, CColorCorrection *cColor);

	// EMD 불량 검출 로직
	long	LogicStart_EMD(cv::Mat& matSrcBuf, cv::Mat &matBKBuf, cv::Mat& matDrawBuf, CRect rectROI, double* dPara, int* nCommonPara,
		wchar_t _strPath[][1024], STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);

	//////////////////////////////////////////////////////////////////////////

protected:

	// x방향 피팅
	bool	Estimation_X(cv::Mat matSrcBuf, cv::Mat &matDstBuf, int nDimensionX, int nStepX, float dThBGOffset);

	// y방향 피팅
	bool	Estimation_Y(cv::Mat matSrcBuf, cv::Mat &matDstBuf, int nDimensionY, int nStepY, float dThBGOffset);

	// xy방향 피팅
	bool	Estimation_XY(cv::Mat matSrcBuf, cv::Mat &matDstBuf, int nDimensionX, int nDimensionY, int nStepX, int nStepY, float dThBGOffset);

	// 두개 영상 평균
	bool	TwoImg_Average(cv::Mat matSrc1Buf, cv::Mat matSrc2Buf, cv::Mat &matDstBuf);

	// 영역에 따른 불량 재판정
	bool	JudgementArea(stDefectInfo* pResultBlob, CRect rectROI, double* dPara);
};
