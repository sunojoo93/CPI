
/************************************************************************/
// Mura �ҷ� ���� ���� ���
// ������ : 18.05.31 - KSW
/************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "InspectLibLog.h"
#include "MatBuf.h"				// �޸� ����


// R, G, B, Gray, White ����
enum ENUM_PARA_AVI_MURA_COMMON
{
	E_PARA_AVI_MURA_DUST_TEXT							= 0	,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA_DUST_BRIGHT_FLAG						,	// ���� ���� ���
	E_PARA_AVI_MURA_DUST_DARK_FLAG							,	// ���� ���� ���
	E_PARA_AVI_MURA_DUST_BIG_AREA							,	// ū ���� �ҷ��� �����
	E_PARA_AVI_MURA_DUST_ADJUST_RANGE						,	// �ҷ� �ֺ����� ������ �ִ� ��� �ҷ� ����

	E_PARA_AVI_MURA_JUDGE_EDGE_NUGI_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA_JUDGE_EDGE_NUGI_USE						,	// ���� ����1

	E_PARA_AVI_MURA_COMMON_TEXT								,	// �ؽ�Ʈ ǥ��
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
	E_PARA_AVI_MURA_RGB_INSPECT_DARK_FLAG					,	// ��ο� �ҷ� flag
	E_PARA_AVI_MURA_RGB_INSPECT_LINE_MURA_FLAG				,	// ���� ���� �ҷ� flag

	E_PARA_AVI_MURA_RGB_EMD_DARK_TEXT						,
	E_PARA_AVI_MURA_RGB_EMD_DARK_THRESHOLD					,	// ����ȭ
	E_PARA_AVI_MURA_RGB_EMD_DARK_MORP						,	// �ҷ� ���̴� �뵵
	E_PARA_AVI_MURA_RGB_EMD_DARK_DEL_AREA					,	// ���� ���� �ҷ� ����

	E_PARA_AVI_MURA_RGB_TEXT								,
	E_PARA_AVI_MURA_RGB_RESIZE								,	//
	E_PARA_AVI_MURA_RGB_GAUSSIAN_SIZE						,	//
	E_PARA_AVI_MURA_RGB_GAUSSIAN_SIGMA						,	//
	E_PARA_AVI_MURA_RGB_DARK_EDGE_AREA						,	//
	E_PARA_AVI_MURA_RGB_DARK_EDGE_THRESHOLD					,	//
	E_PARA_AVI_MURA_RGB_DARK_ACTIVE_THRESHOLD				,	//
	E_PARA_AVI_MURA_RGB_DARK_MORP							,	//
	E_PARA_AVI_MURA_RGB_POW									,	// ����
	E_PARA_AVI_MURA_RGB_BLUR_X								,	//
	E_PARA_AVI_MURA_RGB_BLUR_Y1								,	//
	E_PARA_AVI_MURA_RGB_BLUR_Y2								,	//
	E_PARA_AVI_MURA_RGB_EDGE_AREA							,	//
	E_PARA_AVI_MURA_RGB_THRESHOLD							,	//
	E_PARA_AVI_MURA_RGB_EDGE_THRESHOLD						,	//
	E_PARA_AVI_MURA_RGB_AREA								,	// ū ���� ����
	E_PARA_AVI_MURA_RGB_INSIDE								,	// ����
	E_PARA_AVI_MURA_RGB_AREA_SEG_X							,	// ���� ���� X
	E_PARA_AVI_MURA_RGB_AREA_SEG_Y							,	// ���� ���� Y
	E_PARA_AVI_MURA_RGB_AREA_1_RATIO						,	// ���� 1
	E_PARA_AVI_MURA_RGB_AREA_2_COUNT						,	// ���� 2
	E_PARA_AVI_MURA_RGB_AREA_2_RATIO						,	// ���� 2 ���� ����
	E_PARA_AVI_MURA_RGB_AREA_MIN_GV							,	// ���� ���� �ּ� GV

	E_PARA_AVI_MURA_RGB_TOTAL_COUNT								// Total
};

// Gray, White
enum ENUM_PARA_AVI_MURA_GRAY
{

	E_PARA_AVI_MURA_GRAY_INSPECT_FLAG_TEXT = E_PARA_AVI_MURA_COMMON_TOTAL_COUNT,
	E_PARA_AVI_MURA_GRAY_INSPECT_BRIGHT_FLAG,	// ���� �ҷ� flag
	E_PARA_AVI_MURA_GRAY_INSPECT_DARK_FLAG,	// ��ο� �ҷ� flag
	E_PARA_AVI_MURA_GRAY_INSPECT_MID_BRIGHT_FLAG,	// �߰� ���� �ҷ� flag

	E_PARA_AVI_MURA_GRAY_BRIGHT_TEXT,
	E_PARA_AVI_MURA_GRAY_BRIGHT_THRESHOLD_WHITE_MURA_EDGE_AREA,	// �ܰ� ����
	E_PARA_AVI_MURA_GRAY_BRIGHT_THRESHOLD_WHITE_MURA_ACTIVE,	// �鹫�� ����ȭ
	E_PARA_AVI_MURA_GRAY_BRIGHT_THRESHOLD_WHITE_MURA_EDGE,	// �鹫�� ����ȭ
	E_PARA_AVI_MURA_GRAY_BRIGHT_MORP,	// �ҷ� ���̴� �뵵
	E_PARA_AVI_MURA_GRAY_BRIGHT_DEL_AREA,	// ���� ���� �ҷ� ����

	E_PARA_AVI_MURA_GRAY_DARK_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA_GRAY_DARK_THRESHOLD,	// ����ȭ
	E_PARA_AVI_MURA_GRAY_DARK_MORP,	// �ҷ� ���̴� �뵵
	E_PARA_AVI_MURA_GRAY_DARK_DEL_AREA,	// ���� ���� �ҷ� ����

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

	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_TEXT							,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_MORP_OBJ						,	// �ҷ� ũ�� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_MORP_BKG						,	// ���� �κ� ũ�� ����  ( ex:���� )
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_THRESHOLD						,	// ���� ���� Ȯ��

	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_TEXT						,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_BRIGHT_RATIO		,	// ���� ��� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_1		,	// ��ο� ��� ����1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA_1			,	// ��ο� ��� ���� ����1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_1			,	// ��ο� ��� Diff ����1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_2		,	// ��ο� ��� ����2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA_2			,	// ��ο� ��� ���� ����2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_2			,	// ��ο� ��� Diff ����2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_RATIO_3		,	// ��ο� ��� ����3
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_AREA_3			,	// ��ο� ��� ���� ����3
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_ACTIVE_SPEC_DARK_DIFF_3			,	// ��ο� ��� Diff ����3

	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_TEXT						,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_LEFT					,	// Edge ���� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_TOP					,	// Edge ���� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_RIGHT					,	// Edge ���� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_AREA_BOTTOM				,	// Edge ���� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_BRIGHT_RATIO			,	// ���� ��� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_1			,	// ��ο� ��� ����1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA_1			,	// ��ο� ��� ���� ����1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_1			,	// ��ο� ��� Diff ����1
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_2			,	// ��ο� ��� ����2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA_2			,	// ��ο� ��� ���� ����2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_2			,	// ��ο� ��� Diff ����2
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_RATIO_3			,	// ��ο� ��� ����3
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_AREA_3			,	// ��ο� ��� ���� ����3
	E_PARA_AVI_MURA_GRAY_JUDGE_SPOT_EDGE_SPEC_DARK_DIFF_3			,	// ��ο� ��� Diff ����3

	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_TEXT						,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_FLAG						,	// FLAG
	//E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_BRIGHT_RATIO,	// ���� ��� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC1_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_1,	// ��ο� ��� ����1
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA1_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA1_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_1,	// ��ο� ��� Diff ����1
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC2_FLAG, //SPEC2 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_2,	// ��ο� ��� ����2
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA2_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA2_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_2,	// ��ο� ��� Diff ����2
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC3_FLAG, //SPEC3 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_3,	// ��ο� ��� ����3
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA3_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA3_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_3,	// ��ο� ��� Diff ����3
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_SPEC4_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_RATIO_4,	// ��ο� ��� ����4
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA4_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_AREA4_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_ACTIVE_SPEC_DARK_DIFF_4,	// ��ο� ��� Diff ����4

	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_LEFT,	// Edge ���� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_TOP,	// Edge ���� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_RIGHT,	// Edge ���� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_AREA_BOTTOM,	// Edge ���� ����
	//E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_BRIGHT_RATIO,	// ���� ��� ����
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC1_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_1,	// ��ο� ��� ����1
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA1_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA1_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_1,	// ��ο� ��� Diff ����1
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC2_FLAG, //SPEC2 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_2,	// ��ο� ��� ����2
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA2_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA2_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_2,	// ��ο� ��� Diff ����2
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC3_FLAG, //SPEC3 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_3,	// ��ο� ��� ����3
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA3_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA3_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_3,	// ��ο� ��� Diff ����3
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_SPEC4_FLAG, //SPEC1 FLAG
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_RATIO_4,	// ��ο� ��� ����4
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA4_MIN,	// ��ο� ��� ���� ���� MIN
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_AREA4_MAX,	// ��ο� ��� ���� ���� MAX
	E_PARA_AVI_MURA_GRAY_JUDGE_WHITE_MURA_EDGE_SPEC_DARK_DIFF_4,	// ��ο� ��� Diff ����4

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

	// �޸� ����
	CMatBuf*	cMem;
	void		SetMem(CMatBuf* data)	{	cMem = data		;};
	CMatBuf*	GetMem()				{	return	cMem	;};

	int sz;
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

	void		writeInspectLog_Memory(int nAlgType, char* strFunc, wchar_t* strTxt, __int64 nMemory_Use_Value = 0)
	{
		if (m_cInspectLibLog == NULL)
			return;

		m_tBeforeTime = m_cInspectLibLog->writeInspectLogTime(nAlgType, m_tInitTime, m_tBeforeTime, strFunc, strTxt, nMemory_Use_Value, m_strAlgLog);
	};
	//////////////////////////////////////////////////////////////////////////
	
	// Main �˻� �˰���
	long		DoFindMuraDefect(cv::Mat matSrcBuffer, cv::Mat **matSrcBufferRGB, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);

	// Dust ���� ��, ��� ���� �Ѱ��ֱ�
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

	// ���� ���� ( 8bit & 12bit )
	long		ImageSave(CString strPath, cv::Mat matSrcBuf);

	// Sub - Threshold ( 16bit )
	long		SubThreshold16(cv::Mat& matSrc1Buf, cv::Mat& matSrc2Buf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV);

	// Threshold ( 16bit )
	long		Threshold16(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV);
	long		Threshold16_INV(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nThreshold, int nMaxGV);

	// Pow ( 8bit & 12bit )
	long		Pow(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double dPow, int nMaxGV, CMatBuf* cMemSub=NULL);

	// ���� ���� ���� & ��ó �ҷ� �ձ�
	long		DeleteArea1(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub=NULL);

	// ���� ���� ���� & ��ó �ҷ� �ձ�
	long		DeleteArea2(cv::Mat& matSrcImage, int nCount, int nLength, CMatBuf* cMemSub=NULL);

	long		DeleteArea3(cv::Mat& matSrcImage, int nCount, int nLength, CMatBuf* cMemSub = NULL);
	// x���� ����
	long		Estimation_X(cv::Mat matSrcBuf, cv::Mat &matDstBuf, double* dPara);

	// y���� ����
	long		Estimation_Y(cv::Mat matSrcBuf, cv::Mat &matDstBuf, double* dPara);

	// y���� ���� & ���
	long		Estimation_Y_N_Average(cv::Mat matSrc1Buf, cv::Mat matSrc2Buf, cv::Mat &matDstBuf, double* dPara);

	// xy���� ����
	long		Estimation_XY(cv::Mat matSrcBuf, cv::Mat &matDstBuf, double* dPara, CMatBuf* cMemSub);

	// �ΰ� ���� ���
	long		TwoImg_Average(cv::Mat matSrc1Buf, cv::Mat matSrc2Buf, cv::Mat &matDstBuf);

	// Dust ���� ū �ֺ��� ��� ����
	long		DeleteCompareDust(cv::Mat& matSrcBuffer, int nOffset, stDefectInfo* pResultBlob, int nStartIndex, int nModePS);

	// ��ο� ���� �ҷ� ���� ( ���⸸ ����� ���� )
	long		DeleteDarkLine(cv::Mat& matSrcBuffer, float fMajorAxisRatio, CMatBuf* cMemSub);

	// ���� ���� Max GV ���� : ���� ��� ���� �ҷ� �ֺ����� ����Ǵ� ��� �߻� ����
	long		LimitMaxGV16X(cv::Mat& matSrcBuffer, float fOffset);

	// RGB Line Mura ���� ����
	// ���� �����Ͽ�, �ҷ� ���� ( RGB �ҷ��� ���, ��ó�� �̾��� ���� �ʰ� ������ ���� )
	long		LimitArea(cv::Mat& matSrcBuffer, double* dPara, CMatBuf* cMemSub);

	// RGB Line Mura ã��
	long		JudgeRGBLineMura(cv::Mat& matSrcBuffer, cv::Mat& matBKBuf16, double* dPara, int* nCommonPara, CRect rectROI, stDefectInfo* pResultBlob, CMatBuf* cMemSub);

	// RGB Line Mura ã�� ( ���� ���� �� )
	long		JudgeRGBLineMuraSave(cv::Mat& matSrcBuffer, cv::Mat& matBKBuf16, double* dPara, int* nCommonPara, CRect rectROI, stDefectInfo* pResultBlob, CString strAlgPath, CMatBuf* cMemSub);

	// RGB Line Mura ����Ʈ �߰�
	long		AddRGBLineMuraDefect(cv::Mat& matContoursBuf, double* dPara, int* nCommonPara, CRect rectROI, stDefectInfo* pResultBlob);

	// Spot ��з�
	long		JudgeWhiteSpot(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
		stDefectInfo* pResultBlob, CMatBuf* cMemSub);

	// White Mura ��з�
	long		JudgeWhiteMura(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
		stDefectInfo* pResultBlob, CMatBuf* cMemSub);

	// Nugi ��з�
	long		JudgeNugi(cv::Mat& matSrcBuffer, cv::Mat& matDstBuffer, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath,
		stDefectInfo* pResultBlob, CMatBuf* cMemSub);

	// ȸ�� �簢�� ��ġ����(�浹) Ȯ��
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
