
/************************************************************************/
// Mura 불량 검출 관련 헤더
// 수정일 : 18.05.31 - KSW
/************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "InspectLibLog.h"
#include "MatBuf.h"				// 메모리 관리


// R, G, B, Gray, White 공통
enum ENUM_PARA_AVI_MURA_COMMON
{
	E_PARA_AVI_MURA_DUST_TEXT							= 0	,	// 텍스트 표시
	E_PARA_AVI_MURA_DUST_BRIGHT_FLAG						,	// 먼지 제거 사용
	E_PARA_AVI_MURA_DUST_DARK_FLAG							,	// 먼지 제거 사용
	E_PARA_AVI_MURA_DUST_BIG_AREA							,	// 큰 면적 불량만 남기기
	E_PARA_AVI_MURA_DUST_ADJUST_RANGE						,	// 불량 주변으로 먼지가 있는 경우 불량 제거

	E_PARA_AVI_MURA_JUDGE_EDGE_NUGI_TEXT					,	// 텍스트 표시
	E_PARA_AVI_MURA_JUDGE_EDGE_NUGI_USE						,	// 면적 조건1

	E_PARA_AVI_MURA_COMMON_TEXT								,	// 텍스트 표시
	E_PARA_AVI_MURA_COMMON_GAUSSIAN_SIZE					,	//
	E_PARA_AVI_MURA_COMMON_GAUSSIAN_SIGMA					,	//
	E_PARA_AVI_MURA_COMMON_ESTIMATION_DIM_X					,	//
	E_PARA_AVI_MURA_COMMON_ESTIMATION_DIM_Y					,	//
	E_PARA_AVI_MURA_COMMON_ESTIMATION_STEP_X				,	// 
	E_PARA_AVI_MURA_COMMON_ESTIMATION_STEP_Y				,	//	
	E_PARA_AVI_MURA_COMMON_ESTIMATION_BRIGHT				,	//
	E_PARA_AVI_MURA_COMMON_ESTIMATION_DARK					,	//

	E_PARA_AVI_MURA_COMMON_TOTAL_COUNT							// Total
};

// R, G, B
enum ENUM_PARA_AVI_MURA_RGB
{
	E_PARA_AVI_MURA_RGB_INSPECT_FLAG_TEXT = E_PARA_AVI_MURA_COMMON_TOTAL_COUNT,
	E_PARA_AVI_MURA_RGB_INSPECT_DARK_FLAG					,	// 어두운 불량 flag
	E_PARA_AVI_MURA_RGB_INSPECT_LINE_MURA_FLAG				,	// 라인 무라 불량 flag

	E_PARA_AVI_MURA_RGB_EMD_DARK_TEXT						,
	E_PARA_AVI_MURA_RGB_EMD_DARK_THRESHOLD					,	// 이진화
	E_PARA_AVI_MURA_RGB_EMD_DARK_MORP						,	// 불량 붙이는 용도
	E_PARA_AVI_MURA_RGB_EMD_DARK_DEL_AREA					,	// 작은 면적 불량 제거

	E_PARA_AVI_MURA_RGB_TEXT								,
	E_PARA_AVI_MURA_RGB_RESIZE								,	//
	E_PARA_AVI_MURA_RGB_GAUSSIAN_SIZE						,	//
	E_PARA_AVI_MURA_RGB_GAUSSIAN_SIGMA						,	//
	E_PARA_AVI_MURA_RGB_DARK_EDGE_AREA						,	//
	E_PARA_AVI_MURA_RGB_DARK_EDGE_THRESHOLD					,	//
	E_PARA_AVI_MURA_RGB_DARK_ACTIVE_THRESHOLD				,	//
	E_PARA_AVI_MURA_RGB_DARK_MORP							,	//
	E_PARA_AVI_MURA_RGB_POW									,	// 증폭
	E_PARA_AVI_MURA_RGB_BLUR_X								,	//
	E_PARA_AVI_MURA_RGB_BLUR_Y1								,	//
	E_PARA_AVI_MURA_RGB_BLUR_Y2								,	//
	E_PARA_AVI_MURA_RGB_EDGE_AREA							,	//
	E_PARA_AVI_MURA_RGB_THRESHOLD							,	//
	E_PARA_AVI_MURA_RGB_EDGE_THRESHOLD						,	//
	E_PARA_AVI_MURA_RGB_AREA								,	// 큰 면적 제거
	E_PARA_AVI_MURA_RGB_INSIDE								,	// 안쪽
	E_PARA_AVI_MURA_RGB_AREA_SEG_X							,	// 분할 영역 X
	E_PARA_AVI_MURA_RGB_AREA_SEG_Y							,	// 분할 영역 Y
	E_PARA_AVI_MURA_RGB_AREA_1_RATIO						,	// 비율 1
	E_PARA_AVI_MURA_RGB_AREA_2_COUNT						,	// 비율 2
	E_PARA_AVI_MURA_RGB_AREA_2_RATIO						,	// 비율 2 영역 개수
	E_PARA_AVI_MURA_RGB_AREA_MIN_GV							,	// 분할 영역 최소 GV

	E_PARA_AVI_MURA_RGB_TOTAL_COUNT								// Total
};

// Gray, White
enum ENUM_PARA_AVI_MURA_GRAY
{

	E_PARA_AVI_MURA_GRAY_INSPECT_FLAG_TEXT = E_PARA_AVI_MURA_COMMON_TOTAL_COUNT,
	E_PARA_AVI_MURA_GRAY_INSPECT_BRIGHT_FLAG,	// 밝은 불량 flag
	E_PARA_AVI_MURA_GRAY_INSPECT_DARK_FLAG,	// 어두운 불량 flag
	E_PARA_AVI_MURA_GRAY_INSPECT_MID_BRIGHT_FLAG,	// 중간 밝은 불량 flag

	E_PARA_AVI_MURA_GRAY_BRIGHT_TEXT,
	E_PARA_AVI_MURA_GRAY_BRIGHT_THRESHOLD_WHITE_MURA_EDGE_AREA,	// 외곽 범위
	E_PARA_AVI_MURA_GRAY_BRIGHT_THRESHOLD_WHITE_MURA_ACTIVE,	// 백무라 이진화
	E_PARA_AVI_MURA_GRAY_BRIGHT_THRESHOLD_WHITE_MURA_EDGE,	// 백무라 이진화
	E_PARA_AVI_MURA_GRAY_BRIGHT_MORP,	// 불량 붙이는 용도
	E_PARA_AVI_MURA_GRAY_BRIGHT_DEL_AREA,	// 작은 면적 불량 제거

	E_PARA_AVI_MURA_GRAY_DARK_TEXT,	// 텍스트 표시
	E_PARA_AVI_MURA_GRAY_DARK_THRESHOLD,	// 이진화
	E_PARA_AVI_MURA_GRAY_DARK_MORP,	// 불량 붙이는 용도
	E_PARA_AVI_MURA_GRAY_DARK_DEL_AREA,	// 작은 면적 불량 제거

	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_TEXT,
	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_ADJUST1_MIN_GV,	// 
	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_RESIZE_LOOP_CNT,	// 
	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_CONTRAST_VALUE,	// 
	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_ADJUST2_MUTI_VALUE,	// 
	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_CANNY_MIN,	//
	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_CANNY_MAX,	// 
	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_EDGE_DEL_LOOP,	// 
	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_EDGE_DEL_MORP_SIZE,	// 
	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_DEL_AREA,	// 
	E_PARA_AVI_MURA_GRAY_MID_BRIGHT_DEFECT_MORP_RESIZE,	// 

	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_TEXT							,	// 텍스트 표시
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_MORP_OBJ						,	// 불량 크게 설정
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_MORP_BKG						,	// 밝은 부분 크게 설정  ( ex:명점 )
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_THRESHOLD						,	// 밝은 명점 확인

	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_TEXT						,	// 텍스트 표시
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_BRIGHT_RATIO		,	// 밝은 경우 조건
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_1		,	// 어두운 경우 조건1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA_1			,	// 어두운 경우 면적 조건1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_1			,	// 어두운 경우 Diff 조건1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_2		,	// 어두운 경우 조건2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA_2			,	// 어두운 경우 면적 조건2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_2			,	// 어두운 경우 Diff 조건2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_3		,	// 어두운 경우 조건3
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA_3			,	// 어두운 경우 면적 조건3
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_3			,	// 어두운 경우 Diff 조건3

	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_TEXT						,	// 텍스트 표시
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_LEFT					,	// Edge 영역 설정
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_TOP					,	// Edge 영역 설정
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_RIGHT					,	// Edge 영역 설정
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_BOTTOM				,	// Edge 영역 설정
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_BRIGHT_RATIO			,	// 밝은 경우 조건
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_1			,	// 어두운 경우 조건1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA_1			,	// 어두운 경우 면적 조건1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_1			,	// 어두운 경우 Diff 조건1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_2			,	// 어두운 경우 조건2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA_2			,	// 어두운 경우 면적 조건2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_2			,	// 어두운 경우 Diff 조건2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_3			,	// 어두운 경우 조건3
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA_3			,	// 어두운 경우 면적 조건3
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_3			,	// 어두운 경우 Diff 조건3

	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_TEXT						,	// 텍스트 표시
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_FLAG						,	// FLAG
	//E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_BRIGHT_RATIO,	// 밝은 경우 조건
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC1_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_1,	// 어두운 경우 조건1
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA1_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA1_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_1,	// 어두운 경우 Diff 조건1
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC2_FLAG, //SPEC2 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_2,	// 어두운 경우 조건2
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA2_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA2_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_2,	// 어두운 경우 Diff 조건2
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC3_FLAG, //SPEC3 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_3,	// 어두운 경우 조건3
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA3_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA3_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_3,	// 어두운 경우 Diff 조건3
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC4_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_4,	// 어두운 경우 조건4
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA4_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA4_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_4,	// 어두운 경우 Diff 조건4

	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_TEXT,	// 텍스트 표시
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_LEFT,	// Edge 영역 설정
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_TOP,	// Edge 영역 설정
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_RIGHT,	// Edge 영역 설정
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_BOTTOM,	// Edge 영역 설정
	//E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_BRIGHT_RATIO,	// 밝은 경우 조건
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC1_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_1,	// 어두운 경우 조건1
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA1_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA1_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_1,	// 어두운 경우 Diff 조건1
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC2_FLAG, //SPEC2 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_2,	// 어두운 경우 조건2
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA2_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA2_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_2,	// 어두운 경우 Diff 조건2
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC3_FLAG, //SPEC3 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_3,	// 어두운 경우 조건3
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA3_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA3_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_3,	// 어두운 경우 Diff 조건3
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC4_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_4,	// 어두운 경우 조건4
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA4_MIN,	// 어두운 경우 면적 범위 MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA4_MAX,	// 어두운 경우 면적 범위 MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_4,	// 어두운 경우 Diff 조건4

	E_PARA_AVI_MURA_TOTAL_COUNT											// Total
};

// Gray, White
enum ENUM_PARA_AVI_MURA_G3
{
	E_PARA_AVI_MURA_G3_TEXT,

	E_PARA_AVI_MURA_G3_PREPROCESS_SHIFTCOPY,
	E_PARA_AVI_MURA_G3_PREPROCESS_RESIZEUNIT,
	E_PARA_AVI_MURA_G3_PREPROCESS_LIMITLENGTH,
	E_PARA_AVI_MURA_G3_PREPROCESS_BLUR_L01,
	E_PARA_AVI_MURA_G3_PREPROCESS_BLUR_L02,
	E_PARA_AVI_MURA_G3_PREPROCESS_BLUR_L03,
	E_PARA_AVI_MURA_G3_MAIN_LOWAREA_TH,
	E_PARA_AVI_MURA_G3_MAIN_TOPAREA_TH,
	E_PARA_AVI_MURA_G3_JUDGE_DIFF_GV,

	E_PARA_AVI_MURA_G3_TOTAL_COUNT
};

class CInspectMura
{
public:
	CInspectMura(void);
	virtual ~CInspectMura(void);

	// 메모리 관리
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};

	int sz;
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

	void		writeInspectLog_Memory(int nAlgType, char* strFunc, wchar_t* strTxt, __int64 nMemory_Use_Value = 0)
	{
		if (m_cInspectLibLog == NULL)
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, nMemory_Use_Value, m_strAlgLog);
	};
	//////////////////////////////////////////////////////////////////////////
	
	// Main 검사 알고리즘
	long		DoFindMuraDefect(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);

	// Dust 제거 후, 결과 벡터 넘겨주기
	long		GetDefectList(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer, cv::Point* ptCorner,
		double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob, wchar_t* strContourTxt);

protected:
	long		LogicStart_SPOT(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath);

	long		LogicStart_RGB_LINE_MURA(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);

	////////////////////////////////////////////////////////////////////////// choi
	long        LogicStart_MuraG3CM(cv::Mat& matSrcImage, cv::Mat& matBKBuffer, CRect rectROI, double* dPara, int* nCommonPara,
		CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);
	//////////////////////////////////////////////////////////////////////////

protected:

	// 영상 저장 ( 8bit & 12bit )
	long		ImageSave(CString strPath, cv::Mat matSrcBuf);

	// Sub - Threshold ( 16bit )
	long		SubThreshold16(cv::Mat& matSrc1Buf, cv::Mat& matSrc2Buf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV);

	// Threshold ( 16bit )
	long		Threshold16(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV);
	long		Threshold16_INV(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV);

	// Pow ( 8bit & 12bit )
	long		Pow(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double dPow, int nMaxGV, CMatBuf* cMemSub=NULL);

	// 작은 면적 제거 & 근처 불량 잇기
	long		DeleteArea1(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub=NULL);

	// 작은 면적 제거 & 근처 불량 잇기
	long		DeleteArea2(cv::Mat& matSrcImage, int nCount, int nLength, CMatBuf* cMemSub=NULL);

	long		DeleteArea3(cv::Mat& matSrcImage, int nCount, int nLength, CMatBuf* cMemSub = NULL);
	// x방향 피팅
	long		Estimation_X(cv::Mat matSrcBuf, cv::Mat &matDstBuf, double* dPara);

	// y방향 피팅
	long		Estimation_Y(cv::Mat matSrcBuf, cv::Mat &matDstBuf, double* dPara);

	// y방향 피팅 & 평균
	long		Estimation_Y_N_Average(cv::Mat matSrc1Buf, cv::Mat matSrc2Buf, cv::Mat &matDstBuf, double* dPara);

	// xy방향 피팅
	long		Estimation_XY(cv::Mat matSrcBuf, cv::Mat &matDstBuf, double* dPara, CMatBuf* cMemSub);

	// 두개 영상 평균
	long		TwoImg_Average(cv::Mat matSrc1Buf, cv::Mat matSrc2Buf, cv::Mat &matDstBuf);

	// Dust 면적 큰 주변인 경우 제거
	long		DeleteCompareDust(cv::Mat& matSrcBuffer, int nOffset, stDefectInfo* pResultBlob, int nStartIndex, int nModePS);

	// 어두운 라인 불량 제거 ( 누기만 남기기 위함 )
	long		DeleteDarkLine(cv::Mat& matSrcBuffer, float fMajorAxisRatio, CMatBuf* cMemSub);

	// 가로 방향 Max GV 제한 : 명점 등등 밝은 불량 주변으로 검출되는 경우 발생 방지
	long		LimitMaxGV16X(cv::Mat& matSrcBuffer, float fOffset);

	// RGB Line Mura 면적 제한
	// 면적 제한하여, 불량 제거 ( RGB 불량의 경우, 선처럼 이어져 있지 않고 끊어져 있음 )
	long		LimitArea(cv::Mat& matSrcBuffer, double* dPara, CMatBuf* cMemSub);

	// RGB Line Mura 찾기
	long		JudgeRGBLineMura(cv::Mat& matSrcBuffer, cv::Mat& matBKBuf16, double* dPara, int* nCommonPara, CRect rectROI, stDefectInfo* pResultBlob, CMatBuf* cMemSub);

	// RGB Line Mura 찾기 ( 영상 저장 용 )
	long		JudgeRGBLineMuraSave(cv::Mat& matSrcBuffer, cv::Mat& matBKBuf16, double* dPara, int* nCommonPara, CRect rectROI, stDefectInfo* pResultBlob, CString strAlgPath, CMatBuf* cMemSub);

	// RGB Line Mura 리스트 추가
	long		AddRGBLineMuraDefect(cv::Mat& matContoursBuf, double* dPara, int* nCommonPara, CRect rectROI, stDefectInfo* pResultBlob);

	// Spot 재분류
	long		JudgeWhiteSpot(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
		stDefectInfo* pResultBlob, CMatBuf* cMemSub);

	// White Mura 재분류
	long		JudgeWhiteMura(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
		stDefectInfo* pResultBlob, CMatBuf* cMemSub);

	// Nugi 재분류
	long		JudgeNugi(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
		stDefectInfo* pResultBlob, CMatBuf* cMemSub);

	// 회전 사각형 곂치는지(충돌) 확인
	bool		OrientedBoundingBox(cv::RotatedRect& rect1, cv::RotatedRect& rect2);

	//////////////////////////////////////////////////////////////////////////choi
	void  C_Mexican_filter(cv::Mat& ip);
	float* computeKernel2D(int r);
	boolean convolveFloat(cv::Mat& ip, float* kernel, int kw, int kh);
	double getScale(float* kernel);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////choi G3
	long  ShiftCopyParaCheck(int ShiftValue, int& nCpyX, int& nCpyY, int& nLoopX, int& nLoopY);
	long  ShiftCopy(cv::Mat& matSrcImage, cv::Mat& matDstImage, int nShiftX, int nShiftY, int nShiftLoopX, int nShiftLoopY, CMatBuf* cMemSub);
	long  AveragingReducer(cv::Mat& matSrcImage, cv::Mat& matDstImage, CMatBuf* cMemSub);
	long  GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto);
	long  HistAreaCalc(cv::Mat& matSrcImage, int& nLowerIndex, int& nUpperIndex, int& nLowUpDiff, int& nLowValueArea, int& nTopValueArea, double* dPara);

	//////////////////////////////////////////////////////////////////////////
};
