
/************************************************************************/
// Point �ҷ� ���� ���� ���
// ������ : 17.03.08 - KSW
/************************************************************************/

#pragma once

#include "Define.h"
#include "FeatureExtraction.h"
#include "DefectCCD.h"
#include "MatBuf.h"				// �޸� ����
#include "InspectLibLog.h"

enum ENUM_PARA_LCP_SUBSTANCE_COMMON
{
	E_PARA_LCP_SUBSTANCE_COMMON_BLUR_LOOP = 0,	// Blur 5x5 �ݺ�
	E_PARA_LCP_SUBSTANCE_COMMON_BLUR_LOOP_SIZE,	// Blur 5x5 �ݺ�
	E_PARA_LCP_SUBSTANCE_COMMON_SEG_X,	// ���� ����ȭ ���� X
	E_PARA_LCP_SUBSTANCE_COMMON_SEG_Y,	// ���� ����ȭ ���� Y
	E_PARA_LCP_SUBSTANCE_COMMON_MIN_THRESHOLD,	// �ּ� ����ȭ �� ( ��ο� ����϶� ��� )
	E_PARA_LCP_SUBSTANCE_COMMON_MEDIAN,	// ū �ҷ��� ���, ���� ��հ��� �ֺ���� �ٸ�
	E_PARA_LCP_SUBSTANCE_COMMON_POW_USE,
	E_PARA_LCP_SUBSTANCE_COMMON_POW_VALUE,	// Pow
	E_PARA_LCP_SUBSTANCE_COMMON_ACTIVE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_SUBSTANCE_COMMON_ACTIVE_DARK_RATIO,	// Dark �ҷ� �ֺ� ��� ���
	E_PARA_LCP_SUBSTANCE_COMMON_ACTIVE_BRIGHT_RATIO,	// Bright �ҷ� �ֺ� ��� ���
	E_PARA_LCP_SUBSTANCE_COMMON_EDGE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_SUBSTANCE_COMMON_EDGE_AREA,	// �ܰ����κ��� ���� ����
	E_PARA_LCP_SUBSTANCE_COMMON_EDGE_DARK_RATIO,	// Dark �ҷ� �ֺ� ��� ���
	E_PARA_LCP_SUBSTANCE_COMMON_EDGE_BRIGHT_RATIO,	// Bright �ҷ� �ֺ� ��� ���

	E_PARA_LCP_SUBSTANCE_COMMON_TOTAL_COUNT
};

enum ENUM_PARA_LCP_SUBSTANCE_DUST
{

	E_PARA_LCP_SUBSTANCE_DUST_TEXT = E_PARA_LCP_SUBSTANCE_COMMON_TOTAL_COUNT,

	//Blur Dust Delete Alg
	E_PARA_LCP_SUBSTANCE_DUST_BL_TEXT,
	E_PARA_LCP_SUBSTANCE_DUST_BL_BLUR_SIZE,
	E_PARA_LCP_SUBSTANCE_DUST_BL_THRESHOLD,
	E_PARA_LCP_SUBSTANCE_DUST_BL_MAXIMUM_SIZE,

	//RangeThreshold ��ó�� Alg
	E_PARA_LCP_SUBSTANCE_DUST_RT_TEXT,
	E_PARA_LCP_SUBSTANCE_DUST_RT_RESIZE,
	E_PARA_LCP_SUBSTANCE_DUST_RT_MAXIMUM_SIZE,
	E_PARA_LCP_SUBSTANCE_DUST_RT_THRESHOLD,


	E_PARA_LCP_SUBSTANCE_DUST_TOTAL_COUNT								// Total
};

enum ENUM_PARA_LCP_SUBSTANCE_SIDE
{
	E_PARA_LCP_SUBSTANCE_SIDEL_TEXT = E_PARA_LCP_SUBSTANCE_COMMON_TOTAL_COUNT,

	//Line Mura Delete Alg
	E_PARA_LCP_SUBSTANCE_SIDE_LM_TEXT,
	E_PARA_LCP_SUBSTANCE_SIDE_LM_MAXIMUM_SIZE,
	E_PARA_LCP_SUBSTANCE_SIDE_LM_BLUR_SIZE,
	E_PARA_LCP_SUBSTANCE_SIDE_LM_GAUBLUR_SIZE,
	E_PARA_LCP_SUBSTANCE_SIDE_LM_GAUBLUR_SIG,
	E_PARA_LCP_SUBSTANCE_SIDE_LM_THRESHOLD,
	E_PARA_LCP_SUBSTANCE_SIDE_LM_MAXIMUM_SIZE_2,

	//Blur Dust Delete Alg
	E_PARA_LCP_SUBSTANCE_SIDE_BL_TEXT,
	E_PARA_LCP_SUBSTANCE_SIDE_BL_BLUR_SIZE,
	E_PARA_LCP_SUBSTANCE_SIDE_BL_GAUBLUR_SIZE,
	E_PARA_LCP_SUBSTANCE_SIDE_BL_GAUBLUR_SIG,
	E_PARA_LCP_SUBSTANCE_SIDE_BL_BLUR_THRESHOLD,

	//RangeThreshold ��ó�� Alg
	E_PARA_LCP_SUBSTANCE_SIDE_RT_TEXT,
	E_PARA_LCP_SUBSTANCE_SIDE_RT_RESIZE,
	E_PARA_LCP_SUBSTANCE_SIDE_RT_MAXIMUM_SIZE,
	E_PARA_LCP_SUBSTANCE_SIDE_RT_THRESHOLD,

	//Firts Judge
	E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_AXIO_RATIO,
	E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_AREA_RATIO,
	E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_AREA_MIN,
	E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_AREA_MAX,
	E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_SPEC1_TEXT,
	E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_SPEC1_USE,
	E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_SPEC1_AREA_MIN,
	E_PARA_LCP_SUBSTANCE_SIDE_FIRST_JUDGE_SPEC1_AXIO_RATIO,

	E_PARA_LCP_SUBSTANCE_SIDE_TOTAL_COUNT								// Total
};

enum ENUM_PARA_LCP_SUBSTANCE_COXIAL
{

	E_PARA_LCP_SUBSTANCE_COXIAL_TEXT = E_PARA_LCP_SUBSTANCE_COMMON_TOTAL_COUNT,

	//Blur Dust Delete Alg
	E_PARA_LCP_SUBSTANCE_COXIAL_BL_TEXT,
	E_PARA_LCP_SUBSTANCE_COXIAL_BL_BLUR_SIZE,
	E_PARA_LCP_SUBSTANCE_COXIAL_BL_GAUBLUR_SIZE,
	E_PARA_LCP_SUBSTANCE_COXIAL_BL_GAUBLUR_SIG,
	E_PARA_LCP_SUBSTANCE_COXIAL_BL_THRESHOLD,

	//Firts Judge
	E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_AXIO_RATIO,
	E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_AREA_RATIO,
	E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_AREA_MIN,
	E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_AREA_MAX,
	E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_SPEC1_TEXT,
	E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_SPEC1_USE,
	E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_SPEC1_AREA_MIN,
	E_PARA_LCP_SUBSTANCE_COXIAL_FIRST_JUDGE_SPEC1_AXIO_RATIO,

	E_PARA_LCP_SUBSTANCE_COXIAL_TOTAL_COUNT								// Total
};

enum ENUM_PARA_LCP_SUBSTANCE_BACK
{
	E_PARA_LCP_SUBSTANCE_BACK_TEXT = E_PARA_LCP_SUBSTANCE_COMMON_TOTAL_COUNT,

	//RangeThreshold ��ó�� Alg
	E_PARA_LCP_SUBSTANCE_BACK_RT_TEXT,
	E_PARA_LCP_SUBSTANCE_BACK_RT_MAXIMUM_SIZE,

	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_TEXT,
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_FLAG,

	//Back Bubble �� RangeTh
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_RANGE_TEXT,	// Blur 5x5 �ݺ�
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_BLUR_LOOP ,	// Blur 5x5 �ݺ�
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_BLUR_LOOP_SIZE,	// Blur 5x5 �ݺ�
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_SEG_X,	// ���� ����ȭ ���� X
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_SEG_Y,	// ���� ����ȭ ���� Y
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_MIN_THRESHOLD,	// �ּ� ����ȭ �� ( ��ο� ����϶� ��� )
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_MEDIAN,	// ū �ҷ��� ���, ���� ��հ��� �ֺ���� �ٸ�
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_ACTIVE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_ACTIVE_DARK_RATIO,	// Dark �ҷ� �ֺ� ��� ���
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_ACTIVE_BRIGHT_RATIO,	// Bright �ҷ� �ֺ� ��� ���
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_EDGE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_EDGE_AREA,	// �ܰ����κ��� ���� ����
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_EDGE_DARK_RATIO,	// Dark �ҷ� �ֺ� ��� ���
	E_PARA_LCP_SUBSTANCE_BACK_BUBBLE_EDGE_BRIGHT_RATIO,	// Bright �ҷ� �ֺ� ��� ���

	E_PARA_LCP_SUBSTANCE_BACK_TOTAL_COUNT								// Total
};

//////////////////////////////////////////////////////////////////////////
// ���� �÷� Para Index

enum ENUM_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON
{
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_BLUR_LOOP = 0,	// Blur 5x5 �ݺ�
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_BLUR_LOOP_SIZE,	// Blur 5x5 �ݺ�
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_SEG_X,	// ���� ����ȭ ���� X
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_SEG_Y,	// ���� ����ȭ ���� Y
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_MIN_THRESHOLD,	// �ּ� ����ȭ �� ( ��ο� ����϶� ��� )
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_MEDIAN,	// ū �ҷ��� ���, ���� ��հ��� �ֺ���� �ٸ�
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_POW_USE,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_POW_VALUE,	// Pow
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_ACTIVE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_ACTIVE_DARK_RATIO,	// Dark �ҷ� �ֺ� ��� ���
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_ACTIVE_BRIGHT_RATIO,	// Bright �ҷ� �ֺ� ��� ���
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_EDGE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_EDGE_AREA,	// �ܰ����κ��� ���� ����
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_EDGE_DARK_RATIO,	// Dark �ҷ� �ֺ� ��� ���
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_EDGE_BRIGHT_RATIO,	// Bright �ҷ� �ֺ� ��� ���

	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_TOTAL_COUNT
};

enum ENUM_PARA_LCP_SUBSTANCE_TRANSPARENCY_DUST
{

	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_DUST_TEXT = E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_TOTAL_COUNT,

	//Blur Dust Delete Alg
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_DUST_BL_TEXT,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_DUST_BL_GAUBLUR_SIZE,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_DUST_BL_GAUBLUR_SIG,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_DUST_BL_THRESHOLD,

	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_DUST_TOTAL_COUNT								// Total
};

enum ENUM_PARA_LCP_SUBSTANCE_TRANSPARENCY_SIDE
{
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_SIDEL_TEXT = E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_TOTAL_COUNT,

	//RangeThreshold ��ó�� Alg
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_SIDE_RT_TEXT,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_SIDE_RT_RESIZE,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_SIDE_RT_MAXIMUM_SIZE,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_SIDE_RT_THRESHOLD,

	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_SIDE_TOTAL_COUNT								// Total
};

enum ENUM_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL
{

	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_TEXT = E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_TOTAL_COUNT,

	//Blur Dust Delete Alg
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_BL_TEXT,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_BL_BLUR_SIZE,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_BL_GAUBLUR_SIZE,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_BL_GAUBLUR_SIG,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_BL_THRESHOLD,

	//Firts Judge
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_AXIO_RATIO,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_AREA_RATIO,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_AREA_MIN,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_AREA_MAX,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_SPEC1_TEXT,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_SPEC1_USE,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_SPEC1_AREA_MIN,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_FIRST_JUDGE_SPEC1_AXIO_RATIO,

	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COXIAL_TOTAL_COUNT								// Total
};

enum ENUM_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK
{

	//Firts Judge
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_TEXT = E_PARA_LCP_SUBSTANCE_TRANSPARENCY_COMMON_TOTAL_COUNT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_AXIO_RATIO,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_AREA_RATIO,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_AREA_MIN,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_AREA_MAX,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_SPEC1_TEXT,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_SPEC1_USE,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_SPEC1_AREA_MIN,
	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_FIRST_JUDGE_SPEC1_AXIO_RATIO,

	E_PARA_LCP_SUBSTANCE_TRANSPARENCY_BACK_TOTAL_COUNT								// Total
};

// R, G, B, Gray, White
enum ENUM_PARA_POINT_RGB
{
	E_PARA_POINT_RGB_OUTLINE_TEXT						= 0	,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_RGB_DELETE_PIXEL							,	// �ܰ� ���� �ҷ� ����

	E_PARA_POINT_RGB_COMMON_TEXT							,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_RGB_COMMON_DARK_DIST						,	// 
	E_PARA_POINT_RGB_COMMON_BLUR_LOOP						,	// Blur 5x5 �ݺ�
	E_PARA_POINT_RGB_COMMON_SEG_X							,	// ���� ����ȭ ���� X
	E_PARA_POINT_RGB_COMMON_SEG_Y							,	// ���� ����ȭ ���� Y
	E_PARA_POINT_RGB_COMMON_MIN_THRESHOLD					,	// �ּ� ����ȭ �� ( ��ο� ����϶� ��� )
	E_PARA_POINT_RGB_COMMON_MEDIAN							,	// ū �ҷ��� ���, ���� ��հ��� �ֺ���� �ٸ�
	E_PARA_POINT_RGB_COMMON_POW								,	// Pow

	E_PARA_POINT_RGB_ACTIVE_TEXT							,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_RGB_ACTIVE_DARK_RATIO						,	// Dark �ҷ� �ֺ� ��� ���
	E_PARA_POINT_RGB_ACTIVE_BRIGHT_RATIO					,	// Bright �ҷ� �ֺ� ��� ���

	E_PARA_POINT_RGB_EDGE_TEXT								,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_RGB_EDGE_AREA								,	// �ܰ����κ��� ���� ����
	E_PARA_POINT_RGB_EDGE_DARK_RATIO						,	// Dark �ҷ� �ֺ� ��� ���
	E_PARA_POINT_RGB_EDGE_BRIGHT_RATIO						,	// Bright �ҷ� �ֺ� ��� ���

	E_PARA_POINT_RGB_DEL_LINE_TEXT							,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_RGB_DEL_LINE_BRIGHT_CNT_X					,	// ���� ���� x ���� ����
	E_PARA_POINT_RGB_DEL_LINE_BRIGHT_CNT_Y					,	// ���� ���� y ���� ����
	E_PARA_POINT_RGB_DEL_LINE_BRIGHT_THICK_X				,	// ���� ���� �β� x
	E_PARA_POINT_RGB_DEL_LINE_BRIGHT_THICK_Y				,	// ���� ���� �β� y
	E_PARA_POINT_RGB_DEL_LINE_DARK_CNT_X					,	// ���� ���� x ���� ����
	E_PARA_POINT_RGB_DEL_LINE_DARK_CNT_Y					,	// ���� ���� y ���� ����
	E_PARA_POINT_RGB_DEL_LINE_DARK_THICK_X					,	// ���� ���� �β� x
	E_PARA_POINT_RGB_DEL_LINE_DARK_THICK_Y					,	// ���� ���� �β� y

	E_PARA_POINT_RGB_DEL_CCD_TEXT							,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_RGB_DEL_CCD_DELETE_FLAG					,	// ���� ��� ��/��

	E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_TEXT				,	// Gray pattern ������ RGB ������� ��� Text
	E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_R_USE				,	// R ���� ���� ��뿩��
	E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_R_ADJUST_RATIO	,	// R ���� ���� Ratio
	E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_R_CUT_MINGV		,	// R ���� ������ ����� �ּ� Cutting GV
	E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_G_USE				,	// G ���� ���� ��뿩��
	E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_G_ADJUST_RATIO	,	// G ���� ���� Ratio
	E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_G_CUT_MINGV		,	// G ���� ������ ����� �ּ� Cutting GV
	E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_B_USE				,	// B ���� ���� ��뿩��
	E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_B_ADJUST_RATIO	,	// B ���� ���� Ratio
	E_PARA_POINT_RGB_ADJUST_GRAY_WITH_RGB_B_CUT_MINGV		,	// B ���� ������ ����� �ּ� Cutting GV

	E_PARA_POINT_RGB_BRIGHT_INSP_TEXT						,	// ���� ���� ǥ��
	E_PARA_POINT_RGB_BRIGHT_INSP_APPLYGV_WEIGHT				,	// ���� ��� ���� Weight����
	E_PARA_POINT_RGB_BRIGHT_INSP_RED_SUB_RATIO				,	// ���� ��� ���� Weight����
	E_PARA_POINT_RGB_BRIGHT_INSP_RED_MLT_RATIO				,	// ���� ��� ���� Weight����
	E_PARA_POINT_RGB_BRIGHT_INSP_GREEN_SUB_RATIO			,	// ���� ��� ���� Weight����
	E_PARA_POINT_RGB_BRIGHT_INSP_GREEN_MLT_RATIO			,	// ���� ��� ���� Weight����
	E_PARA_POINT_RGB_BRIGHT_INSP_BLUE_SUB_RATIO				,	// ���� ��� ���� Weight����
	E_PARA_POINT_RGB_BRIGHT_INSP_BLUE_MLT_RATIO				,	// ���� ��� ���� Weight����
	E_PARA_POINT_RGB_BRIGHT_INSP_DARK_RATIO_ACTIVE			,	// ���� ���� DARK RATIO ACTIVE
	E_PARA_POINT_RGB_BRIGHT_INSP_BRIGHT_RATIO_ACTIVE		,	// ���� ���� BRIGHT RATIO ACTIVE
	E_PARA_POINT_RGB_BRIGHT_INSP_DARK_RATIO_EDGE			,	// ���� ���� DARK RATIO EDGE
	E_PARA_POINT_RGB_BRIGHT_INSP_BRIGHT_RATIO_EDGE			,	// ���� ���� BRIGHT RATIO EDGE

	E_PARA_POINT_RGB_TOTAL_COUNT								// Total
};

// Black
enum ENUM_PARA_POINT_BLACK
{
	E_PARA_POINT_BLACK_OUTLINE_TEXT				= 0	,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_BLACK_DELETE_PIXEL					,	// �ܰ� ���� �ҷ� ����

	E_PARA_POINT_BLACK_ACTIVE_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_BLACK_ACTIVE_BLUR_1				,	// ������ ����
	E_PARA_POINT_BLACK_ACTIVE_BLUR_2				,	// ��� ����� �뵵
	E_PARA_POINT_BLACK_ACTIVE_THRESHOLD				,	// ����ȭ

	E_PARA_POINT_BLACK_DEL_LINE_TEXT				,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_BLACK_DEL_LINE_BRIGHT_CNT_X		,	// ���� ���� x ���� ����
	E_PARA_POINT_BLACK_DEL_LINE_BRIGHT_CNT_Y		,	// ���� ���� y ���� ����
	E_PARA_POINT_BLACK_DEL_LINE_BRIGHT_THICK_X		,	// ���� ���� �β� x
	E_PARA_POINT_BLACK_DEL_LINE_BRIGHT_THICK_Y		,	// ���� ���� �β� y

	E_PARA_POINT_BLACK_BIG_TEXT						,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_BLACK_BIG_FLAG						,	// ū�ҷ� & ���� GV �ҷ� ���� ��� ��/��
	E_PARA_POINT_BLACK_BIG_MIN_AREA					,	// ū �ҷ��� �ּ� ����
	E_PARA_POINT_BLACK_BIG_MAX_GV					,	// ���� GV�� �ִ� GV

	E_PARA_POINT_BLACK_DEL_CCD_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_BLACK_DEL_CCD_OFFSET_FLAG			,	// ���� ��� ��/��
	E_PARA_POINT_BLACK_DEL_CCD_DELETE_FLAG			,	// ���� ��� ��/��
	E_PARA_POINT_BLACK_DEL_CCD_AUTO_FLAG			,	// �ڵ� ���� ��� ��/��
	E_PARA_POINT_BLACK_DEL_CCD_AUTO_GV				,	// �ڵ� ���� CCD 4�� ���
	E_PARA_POINT_BLACK_DEL_CCD_AUTO_BKGV			,	// �ڵ� ���� CCD �ֺ� ��� ���

	E_PARA_POINT_BLACK_ROI_OUTLINE_TEXT				,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_BLACK_ROI_OUTLINE_OFFSET			,	// Black pattern ROI���� offset

	E_PARA_POINT_BLACK_TOTAL_COUNT						// Total
};

// Dust
enum ENUM_PARA_POINT_DUST
{
	E_PARA_POINT_DUST_ENHANCMENT_TEXT			= 0	,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_DUST_ENHANCMENT_SHIFT_RANGE		,	// SHIFT COPY ����
	E_PARA_POINT_DUST_ENHANCMENT_GAUSSIAN_SIZE		,	// WINDOW SIZE
	E_PARA_POINT_DUST_ENHANCMENT_GAUSSIAN_SIGMA		,	// SIGMA ũ��
	E_PARA_POINT_DUST_ENHANCMENT_MINMAX_SIZE		,	// MINMAX FILTER SIZE

	E_PARA_POINT_DUST_BINARY_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_DUST_BINARY_BLUR_LOOP				,	// Dust ũ���Ͽ� ����
	E_PARA_POINT_DUST_BINARY_SEG_X					,	// Dust ũ���Ͽ� ����
	E_PARA_POINT_DUST_BINARY_SEG_Y					,	// Dust ũ���Ͽ� ����
	E_PARA_POINT_DUST_BINARY_MIN_THRESHOLD_GV		,	// Dust ũ���Ͽ� ����
	E_PARA_POINT_DUST_BINARY_MEDIAN					,	// Dust ũ���Ͽ� ����

	E_PARA_POINT_DUST_BINARY_ACTIVE_TEXT			,	// Dust ũ���Ͽ� ����
	E_PARA_POINT_DUST_BINARY_ACTIVE_DARK_RATIO		,	// Dust ũ���Ͽ� ����
	E_PARA_POINT_DUST_BINARY_ACTIVE_BRIGHT_RATIO	,	// Dust ũ���Ͽ� ����

	E_PARA_POINT_DUST_BINARY_EDGE_TEXT				,	// Dust ũ���Ͽ� ����
	E_PARA_POINT_DUST_BINARY_EDGE_AREA				,	// Dust ũ���Ͽ� ����
	E_PARA_POINT_DUST_BINARY_EDGE_DARK_RATIO		,	// Dust ũ���Ͽ� ����
	E_PARA_POINT_DUST_BINARY_EDGE_BRIGHT_RATIO		,	// Dust ũ���Ͽ� ����

	E_PARA_POINT_DUST_GROUP_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_DUST_GROUP_USE						,	// E ���� ��� ��/��
	E_PARA_POINT_DUST_GROUP_EDGE_DEL_OUTLINE		,	// Edge �ܰ� ���� ���� ����		
	E_PARA_POINT_DUST_GROUP_EDGE_AREA				,	// Edge �κ� ���� ����	
	E_PARA_POINT_DUST_GROUP_MIN_AREA_EDGE			,	// Edge �κ� ���� �ּ� ũ��
	E_PARA_POINT_DUST_GROUP_MIN_AREA_ACTIVE			,	// Active �κ� ���� �ּ� ũ��
	E_PARA_POINT_DUST_GROUP_EDGE_COUNT				,	// Edge ���� Dust ����
	E_PARA_POINT_DUST_GROUP_ACTIVE_COUNT			,	// Active ���� Dust ����
	E_PARA_POINT_DUST_GROUP_TOTAL_COUNT				,	// �� Dust ����

	E_PARA_POINT_DUST_LOGIC_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_DUST_LOGIC_DELET_AREA				,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_DUST_LOGIC_MORP_RANGE				,	// �ؽ�Ʈ ǥ��
	E_PARA_POINT_DUST_LOGIC_BIG_AREA				,	// �ؽ�Ʈ ǥ��

	E_PARA_POINT_DUST_BUBBLE_TEXT					,	// �ؽ�Ʈ ǥ�� Bubble �����
	E_PARA_POINT_DUST_BUBBLE_RESIZE					,	// ���� Resize
	E_PARA_POINT_DUST_BUBBLE_SRC_BLUR_SIZE			,	// ������ Blur Size
	E_PARA_POINT_DUST_BUBBLE_BK_BLUR_SIZE			,	// ��� ���� Blur Size
	E_PARA_POINT_DUST_BUBBLE_THRESHOLD				,	// ��� ���� ��� ���� Threshold_Value
	E_PARA_POINT_DUST_BUBBLE_CLOSE_SIZE				,	// �ҷ� ��� ������ ���� CLOSE���� ũ��

	E_PARA_POINT_DUST_TOTAL_COUNT						// Total
};

// Morphology - Operation
enum ENUM_MORP
{
	E_MORP_ERODE	= 0	,
	E_MORP_DILATE		,
	E_MORP_OPEN			,
	E_MORP_CLOSE
};

//////////////////////////////////////////////////////////////////////////

class CInspectPoint
{
public:
	CInspectPoint(void);
	virtual ~CInspectPoint(void);

	// �޸� ����
	CMatBuf*	cMem[2];
	void		SetMem_Multi(int nCnt, CMatBuf** data)
	{
		for (int i = 0; i < nCnt; i++)
		{
			cMem[i] = data[i];
		}
	};
	//CMatBuf*	GetMem()				{	return	cMem1	;};

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

	CString		GETDRV()
	{
		return m_cInspectLibLog->GETDRV();
	}

	//////////////////////////////////////////////////////////////////////////

	// Main �˻� �˰���
	long		DoFindPointDefect(cv::Mat matSrcBuffer, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
		cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, CDefectCCD *cCCD , cv::Mat* matCholeBuffer);

	long		DoFindPointDefect_Transparency(cv::Mat matSrcBuffer, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
		cv::Point* ptCorner, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, CDefectCCD *cCCD, cv::Mat* matCholeBuffer, cv::Mat matSrc_Stage);

	long		DoFindPointDefect_Black(cv::Mat matSrcBuffer, cv::Mat& matBKBuffer, cv::Mat& matDarkBuffer, cv::Mat& matBrightBuffer,
		cv::Point* ptCorner, cv::Rect rcAlignCellROI, double* dAlignPara, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment, CDefectCCD *cCCD, cv::Mat* matCholeBuffer);

	// Dust ���� ��, ��� ���� �Ѱ��ֱ�
	long		GetDefectList(cv::Mat matSrcBuffer, cv::Mat MatColorBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);

	// ���� ���� ��, ��� ���� �Ѱ��ֱ� ( ���� : 255GV�� ���� // ���� : 200GV�� ���� )
	long		GetDefectList_Bubble(cv::Mat matSrcBuffer, cv::Mat matDstBuffer[2], cv::Mat matDustBuffer[2], cv::Mat& matDrawBuffer,
		cv::Point* ptCorner, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, stDefectInfo* pResultBlob);

protected:
	void CInspectPoint::Flattening(int nFlatteningType, BYTE *pImage, CSize szImage);
	void CInspectPoint::FlattenMeanHorizontal(BYTE *pImage, CSize szImage);
	void CInspectPoint::FlattenMeanVertical(BYTE *pImage, CSize szImage);

	// LCP ����˻� �˰���
	long LCP_COAXIAL(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara,int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	long LCP_COAXIAL_Black(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	long LCP_COAXIAL_Transparency(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);
	// LCP ���̵�˻� �˰���
	long LCP_SIDE(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	long LCP_SIDE_Transparency(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	// LCP DUST�˻� �˰���
	long LCP_DUST(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	long LCP_DUST_Black(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	long LCP_DUST_Transparency(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	// LCP BackLight�˻� �˰���
	long LCP_BACK(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, cv::Rect rcAlignCellROI, double* dPara,
		int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	long LCP_BACK_Black(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, cv::Rect rcAlignCellROI, double* dPara,
		int* nCommonPara, CString strAlgPath, CString strBubblePath, CString strBubblePath_Menual, ENUM_INSPECT_MODE eInspMode, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	long LCP_BACK_Transparency(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Mat matSrc_Stage);

	void LCP_Dark_Bright_Compare_Del(cv::Mat Gray_Side_Dark, cv::Mat Gray_Side_Bright);

	int First_blob_Coxial(cv::Mat Srcimg, cv::Mat DstImg, cv::Mat matLabels, cv::Mat stats, cv::Mat centroids, cv::Rect rectTemp_Dust,double* dPara);
	int First_blob_Side(cv::Mat Srcimg, cv::Mat DstImg, cv::Mat matLabels, cv::Mat stats, cv::Mat centroids, cv::Rect rectTemp_Dust, double* dPara);

	int First_blob_Transparency_Coxial(cv::Mat Srcimg, cv::Mat DstImg, cv::Mat matLabels, cv::Mat stats, cv::Mat centroids, cv::Rect rectTemp_Dust, double* dPara);

	int First_blob_Transparency_Back(cv::Mat Srcimg, cv::Mat DstImg, cv::Mat matLabels, cv::Mat stats, cv::Mat centroids, cv::Rect rectTemp_Dust, double* dPara);

	void MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize);

	// R, G, B ���� ���� �˰���
	long		LogicStart_RGB(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	// R, G, B ���� ���� �˰���
	long		LogicStart_RGBTest(cv::Mat** matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, int Type , cv::Rect* rcCHoleROI, cv::Mat* matCholeBuffer);

	// Black ���� ���� �˰���
	long		LogicStart_Black(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, 
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	// Gray ���� ���� �˰���
	long		LogicStart_Gray(cv::Mat& matSrcImage,cv::Mat **matSrcBufferRGB,cv::Mat matBKBuffer, cv::Mat* matDstImage, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment, cv::Rect* rcCHoleROI, cv::Mat* matCholeBuffer);

	long		LogicStart_Gray2(cv::Mat& matSrcImage, cv::Mat **matSrcBufferRGB, cv::Mat* matDstImage, CRect rectROI, double* dPara,
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

	// PNZ DUST ���� ��
	long		LogicStart_DustALL(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, cv::Rect* rcCHoleROI, double* dPara,
		int* nCommonPara, CString strAlgPath, STRU_DEFECT_ITEM* EngineerDefectJudgment);

protected:

	// ���� ���� ( 8bit & 12bit )
	long		ImageSave(CString strPath, cv::Mat matSrcBuf);

	// 12bit ����ȭ ( 8bit & 12bit )
	long		Binary(cv::Mat& matSrcImage, cv::Mat& matDstImage, double thresh, bool bInv=false, CMatBuf* cMemSub=NULL);
	long		Binary_16bit(cv::Mat& matSrcImage, cv::Mat& matDstImage, double thresh, bool bInv=false, CMatBuf* cMemSub=NULL);

	// Pow ( 8bit & 12bit )
	long		Pow(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, double dPow, CMatBuf* cMemSub);

	// ���� ���� ����ȭ ( 8bit & 12bit )
	long		RangeAvgThreshold(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub=NULL);
	long		RangeAvgThreshold_8bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub=NULL);
	long		RangeAvgThreshold_16bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub=NULL);
	long		RangeAvgThreshold_RGB(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub=NULL);
	long		RangeAvgThreshold_DUST(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub=NULL);
	long		RangeAvgThreshold_Bubble(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub = NULL);
	long		RangeAvgThreshold_Bubble_8bit(cv::Mat& matSrcImage, cv::Mat* matDstImage, CRect rectROI, double* dPara, CMatBuf* cMemSub = NULL);

	long		ShiftCopy(cv::Mat& matSrcImage, cv::Mat& matDstImage, int nShiftX, int nShiftY, int nShiftLoopX, int nShiftLoopY, CMatBuf* cMemSub = NULL);

	// Median ( 8bit & 12bit )
	long		MedianFilter(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nKSize, CMatBuf* cMemSub=NULL);

	// Distance Transform & Threshold ( 8bit & 12bit )
	long		DistanceTransformThreshold(cv::Mat& matSrcImage, cv::Mat& matDstImage, double dThreshold, CMatBuf* cMemSub=NULL);

	// �������� ( 8bit & 12bit )
	long		Morphology(cv::Mat& matSrcImage, cv::Mat& matDstImage, long nSizeX, long nSizeY, ENUM_MORP nOperation);

	// �ܰ� ���� ( 8bit & 12bit )
	long		DeleteOutArea(cv::Mat& matSrcImage, cv::Point* ptCorner, CMatBuf* cMemSub=NULL);
	long		DeleteOutArea(cv::Mat& matSrcImage, CRect rectROI, CMatBuf* cMemSub=NULL);

	// ���� ���� ���� ( 8bit & 12bit )
	long		DeleteArea(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub=NULL);
	long		DeleteArea_8bit(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub=NULL);
	long		DeleteArea_16bit(cv::Mat& matSrcImage, int nCount, CMatBuf* cMemSub=NULL);

	// Line Delete ( 8bit & 12bit )
	long		ProjectionLineDelete(cv::Mat& matSrcImage, int nCutCountX, int nCutCountY, int nThicknessX=1, int nThicknessY=1);	

	// ������׷� ( 8bit & 12bit )
	long		GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto);

	// Dust �ҷ� ���� ���, E�� ���� üũ �Լ� ( 8bit & 12bit )
	long		JudgementCheckE(cv::Mat& matSrcBuf, double* dPara, CRect rectROI, CMatBuf* cMemSub=NULL);

	// ���� �̹����� RGB ������ �Ѱ��� �־ ����ġ�� �ִ� ����� �Ѵ� -> Gray ���� ��� ( 8bit & 12bit )
	long		AdjustImageWithRGB(cv::Mat& matSrcImage, cv::Mat& matDstImage, cv::Mat& matAdjustSrcImage, double dblAdjustRatio, int nCutMinGVForAdjust, CRect rectROI, CMatBuf* cMemSub=NULL);

	// PNZ Pattern Substraction Test
	long		PatternSubstraction(cv::Mat& matSrcImage, cv::Mat* matDstImage, int type, CMatBuf* cMemSub=NULL);
	
	// PNZ ��� ���߱�
	void		ApplyMeanGV(Mat &matSrcImage,double dblTargetGV);

	void		ApplyEnhancement(cv::Mat matSrcImage, cv::Mat matBuff1, cv::Mat matBuff2, cv::Mat& matDstImage1, cv::Mat& matDstImage2, double *dPara, int* nCommonPara, CString strAlgPath, int Type, CMatBuf* cMemSub=NULL);

	void		SubPatternEhancemnt(cv::Mat matSrcImage, cv::Mat& matDstImage, double dSubWeight, double dEnhanceWeight, int* nCommonPara, CString strAlgPath, CMatBuf* cMemSub=NULL);

	// ���� ����� Cell �� �����Ͽ� ��հ��� �� �Ͱ� ������ ȿ���� ����. ( 8bit & 12bit )
	double		GetAverageForRGB(cv::Mat& matSrcImage);

	// ���� ������ ��� ( 8bit & 12bit )
	double		GetAverage(cv::Mat& matSrcImage);

	// Dust���� ������ ū ��� ã�� ( 8bit & 12bit )
	long		FindBigAreaDust(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, long nFindMinArea, CMatBuf* cMemSub=NULL);

	// ���� - Dust ���� ū �ֺ��� ��� ���� ( 8bit & 12bit )
	long		DeleteCompareDarkPoint(cv::Mat& matSrcBuffer, int nOffset, stDefectInfo* pResultBlob, int nModePS);

	// ��ο� Dust ���� ���� ��츸 ã�� ( 8bit & 12bit )
	long		DarkDustMaxArea(cv::Mat matSrcBuffer[E_DEFECT_COLOR_COUNT], int nMaxArea, CMatBuf* cMemSub=NULL);

	// ���� ���� �����ϱ� ( 8bit & 12bit )
	long		DeleteMinArea(cv::Mat matSrcBuffer, cv::Mat matThBuffer, int nMinArea, int nMaxGV, CMatBuf* cMemSub=NULL);

	// ������׷����� ���
	long		GetAverage_From_Histo(cv::Mat matHisto,int nMin,int nMax,double &dblAverage);

	// ������׷����� ǥ������
	long		GetMeanStdDev_From_Histo(cv::Mat matHisto,int nMin,int nMax,double &dblAverage,double &dblStdev);

	// Dust ���󿡼� ���� ã��
	long		FindBubble_DustImage(cv::Mat matSrcbuffer, cv::Mat& matBubbleResult, cv::Rect rtROI, cv::Rect* rcCHoleROI, double* dPara, int* nCommonPara, CString strAlgPath);

	void		Insp_RectSet(cv::Rect& rectInspROI, CRect& rectROI, int nWidth, int nHeight, int nOffset = 0);

	//�ܰ� ������ GV �� ���߾� �ش�.
	void		AdjustBkGV(cv::Mat matOri, cv::Mat matBKBuffer, double dblRatio, CMatBuf* cMemSub = NULL);

	void		Lipschitz_Fillter(cv::Mat& ip); //choi

	long		Image_Pow(int ImgType, double dpow, Mat& InPutImage, Mat& OutPutImage);

	//template <typename Object>
	void		KalMan_Filter(cv::Mat* stack,int nStack_size, double percentvar, double gain);

	byte*		fromDouble(double* darray, int nlenght);

	double*		toDouble(double* darray, int nlenght);
};