////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare Algorithm DLL Interface Function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DEFINE_INTERFACE_BOE7_H
#define DEFINE_INTERFACE_BOE7_H

/***************************************************************************************************
 Algorithm DLL �� Algorithm Task �� ������ �ʿ��� ENUM, ����ü ���� ���⿡ ���� - AVI / SVI / APP ���� ����
***************************************************************************************************/

#pragma once

#include "Define.h"


// KSW 17.09.07 - Image 8bit ���
#define IMAGE_MAX_GV		256

// KSW 17.09.07 - Image 12bit ��� ( 8bit������ ��� ���� ... �޸� ���� )
//#define IMAGE_MAX_GV		4096

// KSW 17.09.07 - Hist --> UI �ȳѰ��� ( ��� ���� ǥ�� �ȵ� )
// 12bit ��� ��... �ð� ���� �ɸ�...
#define USE_ALG_HIST		FALSE

// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
#define USE_ALG_CONTOURS	TRUE

// KSW 17.11.29 - �ܰ� ���� �ִ� ����
// KSW 18.02.05 - SVI �ҷ� ũ�Ⱑ ŭ / �ܰ� ���� �ø��� 100 -> 500
#define MAX_CONTOURS		500

#define  AREA_CAM_IMG_CLASSIFY_NUM 20
//////////////////////////////////////////////////////////////////////////
// ENUM
//////////////////////////////////////////////////////////////////////////

#define WARPING_TARGET_WIDTH 912 // SVI warping size
#define WARPING_TARGET_HEIGHT 513
// [����] ���� ID
// 2017.06.07 NDH : APP ���� ��ȣ �߰�
enum ENUM_IMAGE_CLASSIFY
{
	// AVI
	E_IMAGE_CLASSIFY_AVI_R = 0		,	// 00 R		
	E_IMAGE_CLASSIFY_AVI_G			,	// 01 G		
	E_IMAGE_CLASSIFY_AVI_B			,	// 02 B		
	E_IMAGE_CLASSIFY_AVI_BLACK		,	// 03 BLACK	
	E_IMAGE_CLASSIFY_AVI_WHITE		,	// 04 WHITE	
	E_IMAGE_CLASSIFY_AVI_GRAY_32	,	// 05 GRAY_32	
	E_IMAGE_CLASSIFY_AVI_GRAY_64	,	// 06 GRAY_64	
	E_IMAGE_CLASSIFY_AVI_GRAY_87	,	// 07 GRAY_87
	E_IMAGE_CLASSIFY_AVI_GRAY_128	,	// 08 GRAY_128	
	E_IMAGE_CLASSIFY_AVI_DUST		,	// 09 DUST
	E_IMAGE_CLASSIFY_AVI_VTH		,	// 10 VTH
	E_IMAGE_CLASSIFY_AVI_PCD		,	// 11 PCD
	E_IMAGE_CLASSIFY_AVI_COAXIAL	,	// 12 COAXIAL
	E_IMAGE_CLASSIFY_AVI_SIDE		,	// 13 SIDE
	E_IMAGE_CLASSIFY_AVI_BACK		,	// 14 BACK

	E_IMAGE_CLASSIFY_AVI_COUNT		,	// 10 AVI Total

	//////////////////////////////////////////////////////////////////////////

	// KSW 18.02.07 - AVI Dust�� �� ���� �ִٸ� �����ϰ� ����� ����
	// SVI
	E_IMAGE_CLASSIFY_SVI_R = 0		,	// 00 R
	E_IMAGE_CLASSIFY_SVI_G			,	// 01 G
	E_IMAGE_CLASSIFY_SVI_B			,	// 02 B
	E_IMAGE_CLASSIFY_SVI_BLACK		,	// 03 BLACK
	E_IMAGE_CLASSIFY_SVI_WHITE		,	// 04 WHITE	
	E_IMAGE_CLASSIFY_SVI_GRAY_32	,	// 06 GRAY_32
	E_IMAGE_CLASSIFY_SVI_GRAY_64	,	// 07 GRAY_64
	E_IMAGE_CLASSIFY_SVI_GRAY_87	,	// 08 GRAY_87
	E_IMAGE_CLASSIFY_SVI_GRAY_128	,	// 09 GRAY_128

	E_IMAGE_CLASSIFY_SVI_COUNT		,	// 09 SVI Total

	//////////////////////////////////////////////////////////////////////////

	// APP
	E_IMAGE_CLASSIFY_APP_ALL = 0		,	// 00 APP All Illum			��ü ���� �� 20		
	E_IMAGE_CLASSIFY_APP_RIGHT		,	// 01 APP Right Illum 2
	E_IMAGE_CLASSIFY_APP_LEFT		,	// 02 APP Left Illum 
	E_IMAGE_CLASSIFY_APP_BOTTOM		,	// 03 APP Bottom Illum 
	E_IMAGE_CLASSIFY_APP_TOP		,	// 04 APP Top Illum 
	E_IMAGE_CLASSIFY_APP_ALL2,	// 05 APP All  Illum		��ü ���� �� 70 
	E_IMAGE_CLASSIFY_APP_DUST			,	// 06 APP dust Illum

	E_IMAGE_CLASSIFY_APP_COUNT		// 07 Total
};

// 2017.06.08 Align ���� AVI/SVI/APP �������� ����
// [CInspectAlign] Align ����
enum ENUM_ALIGN_TYPE
{
	E_ALIGN_TYPE_LEFT = 0	,
	E_ALIGN_TYPE_TOP		,
	E_ALIGN_TYPE_RIGHT		,
	E_ALIGN_TYPE_BOTTOM		,
	E_ALIGN_TYPE_END
};

// 2017.06.07 NDH : GUI �ʿ��� XML ���� �ϳ��� AVL, ABI ���� ����Ͽ� Algorithm�ʵ� ���� ���
// [AVI ����] �ҷ� ���� ���
// 17.03.09 �ű� �ҷ� �׸� �߰�
enum ENUM_DEFECT_JUDGEMENT
{
	//////////////////////////////////////////////////////////////////////////
	// AVI �ҷ� - ���÷��� �̻�
	//////////////////////////////////////////////////////////////////////////

	// ���÷��� �̻� ����
	E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL_START = 0,

	E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL = E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL_START		,	// ���� �̻�
	E_DEFECT_JUDGEMENT_DISPLAY_OFF						,	// ���� �ƿ� �ȵ� ��� ( ���غҷ� & Cell �̻� )
	E_DEFECT_JUDGEMENT_DISPLAY_DARK						,	// ���� ��� ��ο�
	E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT					,	// ���� ��� ����	
	E_DEFECT_JUDGEMENT_DISPLAY_CHOLE_ABNORMAL			,	// CHole �ֺ� ��� �̻�
	E_DEFECT_JUDGEMENT_DUST_GROUP						,	// Group�� ������ �̹��� ���

	// ���÷��� �̻� ��
	E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL_END	= E_DEFECT_JUDGEMENT_DUST_GROUP,

	//////////////////////////////////////////////////////////////////////////
	// AVI �ҷ� - Point �ҷ�
	//////////////////////////////////////////////////////////////////////////

	// Point �ҷ� ����
	E_DEFECT_JUDGEMENT_POINT_START,

	E_DEFECT_JUDGEMENT_POINT_DARK = E_DEFECT_JUDGEMENT_POINT_START		,	// ����
	E_DEFECT_JUDGEMENT_POINT_BRIGHT										,	// ����
	E_DEFECT_JUDGEMENT_POINT_WEAK_BRIGHT								,	// �����
	E_DEFECT_JUDGEMENT_POINT_BRIGHT_DARK								,	// ��/����
	E_DEFECT_JUDGEMENT_POINT_DARK_SP_1									,	// Red Pattern ����
	E_DEFECT_JUDGEMENT_POINT_DARK_SP_2									,	// Green Pattern ����
	E_DEFECT_JUDGEMENT_POINT_DARK_SP_3									,	// Blue Pattern ����
	E_DEFECT_JUDGEMENT_POINT_GROUP_DARK									,	// ���� ����
	E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT								,	// ���� ����
	E_DEFECT_JUDGEMENT_POINT_DIMPLE										,   // DIMPLE

	// Point �ҷ� ��
	E_DEFECT_JUDGEMENT_POINT_END = E_DEFECT_JUDGEMENT_POINT_DIMPLE,

	//////////////////////////////////////////////////////////////////////////
	// AVI �ҷ� - Line �ҷ�
	//////////////////////////////////////////////////////////////////////////

	// Line �ҷ� ����
	E_DEFECT_JUDGEMENT_LINE_START,

	E_DEFECT_JUDGEMENT_LINE_X_BRIGHT = E_DEFECT_JUDGEMENT_LINE_START,	// ���� X����
	E_DEFECT_JUDGEMENT_LINE_X_BRIGHT_MULT		,   // ���� X���� �ٹ�
	E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT			,	// ���� Y����
	E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT_MULT		,   // ���� Y���� �ٹ�
	E_DEFECT_JUDGEMENT_LINE_X_DARK				,	// ��ο� X����
	E_DEFECT_JUDGEMENT_LINE_X_DARK_MULT			,	// ��ο� X���� �ٹ�
	E_DEFECT_JUDGEMENT_LINE_Y_DARK				,	// ��ο� Y����
	E_DEFECT_JUDGEMENT_LINE_Y_DARK_MULT			,	// ��ο� Y���� �ٹ�
	E_DEFECT_JUDGEMENT_LINE_X_OPEN				,	// ���̰� ª�� X����
	E_DEFECT_JUDGEMENT_LINE_Y_OPEN_RIGHT		,	// ���� �� ���̰� ª�� Y����	
	E_DEFECT_JUDGEMENT_LINE_Y_OPEN_LEFT			,	// �� �� ���̰� ª�� Y����	
	E_DEFECT_JUDGEMENT_LINE_DGS					,	// X, Y ���ÿ� ����
	E_DEFECT_JUDGEMENT_LINE_DGS_X				,	// X, Y ���ÿ� ����
	E_DEFECT_JUDGEMENT_LINE_DGS_Y				,	// X, Y ���ÿ� ����
	E_DEFECT_JUDGEMENT_XLINE_SPOT				,	// X Line ���� Point
	E_DEFECT_JUDGEMENT_YLINE_SPOT				,	// Y Line ���� Point
	E_DEFECT_JUDGEMENT_LINE_X_EDGE_BRIGHT		,	// EDGE�� �ִ� ���� X LINE
	E_DEFECT_JUDGEMENT_LINE_Y_EDGE_BRIGHT		,	// EDGE�� �ִ� ���� Y LINE
	E_DEFECT_JUDGEMENT_LINE_CRACK_RIGHT			,	// Crack Pattern���� ������ Crack Line
	E_DEFECT_JUDGEMENT_LINE_CRACK_LEFT			,	// Crack Pattern���� ���� Crack Line
	E_DEFECT_JUDGEMENT_LINE_CRACK_BOTH			,	// Crack Pattern���� ���� Crack Line
	E_DEFECT_JUDGEMENT_LINE_X_DEFECT_BRIGHT		,	// ���� MURA�� X LINE DEFECT
	E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_BRIGHT		,	// ���� MURA�� Y LINE DEFECT
	E_DEFECT_JUDGEMENT_LINE_X_DEFECT_DARK		,	// ��ο� MURA�� LINE DEFECT
	E_DEFECT_JUDGEMENT_LINE_Y_DEFECT_DARK		,	// ��ο� MURA�� LINE DEFECT
	E_DEFECT_JUDGEMENT_LINE_PCD_CRACK			,	// 5.99 ���� ��ġ BLACK, RED, BLUE PATTERN���� ����� �ҷ�
	E_DEFECT_JUDGEMENT_LINE_X_VTH_BRIGHT		,	// Special PATTERN ���� X Line �ҷ�
	E_DEFECT_JUDGEMENT_LINE_Y_VTH_BRIGHT		,	// Special PATTERN ���� Y Line �ҷ�
	E_DEFECT_JUDGEMENT_LINE_X_VTH_DARK			,	// Special PATTERN ��ο� X Line �ҷ�
	E_DEFECT_JUDGEMENT_LINE_Y_VTH_DARK			,	// Special PATTERN ��ο� Y Line �ҷ�
	E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_BRIGHT	,	// Special PATTERN ���� X Line �ҷ�
	E_DEFECT_JUDGEMENT_LINE_Y_VTH_DEFECT_BRIGHT	,	// Special PATTERN ���� Y Line �ҷ�
	E_DEFECT_JUDGEMENT_LINE_X_VTH_DEFECT_DARK	,	// Special PATTERN ��ο� X Line �ҷ�
	E_DEFECT_JUDGEMENT_LINE_Y_VTH_DEFECT_DARK	,	// Special PATTERN ��ο� Y Line �ҷ�
	E_DEFECT_JUDGEMENT_LINE_VTH_DGS				,	// Special PATTERN DGS �ҷ�
	E_DEFECT_JUDGEMENT_LINE_VTH_DGS_X			,	// Special PATTERN DGS X Line �ҷ�
	E_DEFECT_JUDGEMENT_LINE_VTH_DGS_Y			,	// Special PATTERN DGS Y Line �ҷ�
	E_DEFECT_JUDGEMENT_LINE_ETC_4				,	// LINE DEFECT BUFFER4
	E_DEFECT_JUDGEMENT_MURANORMAL_TYPE2_F_GRADE	,	// LINE DEFECT BUFFER5
	E_DEFECT_JUDGEMENT_MURANORMAL_TYPE3_F_GRADE	,	// LINE DEFECT BUFFER6

	// Line �ҷ� ��
	E_DEFECT_JUDGEMENT_LINE_END = E_DEFECT_JUDGEMENT_MURANORMAL_TYPE3_F_GRADE,

	//////////////////////////////////////////////////////////////////////////
	// AVI & SVI �ҷ� - Mura �ҷ�
	//////////////////////////////////////////////////////////////////////////

	// Mura �ҷ� ����
	E_DEFECT_JUDGEMENT_MURA_START,

	E_DEFECT_JUDGEMENT_MURA_MULT_BP = E_DEFECT_JUDGEMENT_MURA_START,	// Point �ҷ� Gray ���� �����ΰ�� ( ���� ��� )
	E_DEFECT_JUDGEMENT_MURA_BLACK_SPOT			,	// ���� ����
	E_DEFECT_JUDGEMENT_MURA_WHITE_SPOT			,	// ���� ����	
	E_DEFECT_JUDGEMENT_MURA_CORNER_CM			,	// �ڳ� ȥ��
	E_DEFECT_JUDGEMENT_MURA_UP_CM				,	// ���� ȥ��
	E_DEFECT_JUDGEMENT_MURA_FINGER_CM			,	// �հ��� ȥ��
	E_DEFECT_JUDGEMENT_MURA_LINE_X				,	// �ٹ���
	E_DEFECT_JUDGEMENT_MURA_LINE_Y				,	// �ٹ���
	E_DEFECT_JUDGEMENT_MURA_WHITE				,	// �鹫��
	E_DEFECT_JUDGEMENT_MURA_CLOUD				,	// 
	E_DEFECT_JUDGEMENT_MURA_E_WHITE_SPOT		,	// �Ϻ�����	 
	E_DEFECT_JUDGEMENT_MURA_TYPE4				,	// Tpye4 Mura Test
	E_DEFECT_JUDGEMENT_MURA_TYPE3_SMALL			,	// Type3 ���� ������
	E_DEFECT_JUDGEMENT_MURA_TYPE3_BIG			,	// Type3 ū ������
	E_DEFECT_JUDGEMENT_MURA_TYPE3_RING			,	// Type3 Ring Mura
	E_DEFECT_JUDGEMENT_MURA_TEMP7				,	
	E_DEFECT_JUDGEMENT_MURA_TEMP8				,	
	E_DEFECT_JUDGEMENT_MURA_EMP					,	// EMP
	E_DEFECT_JUDGEMENT_MURA_EMD_BRIGHT			,	// EMD
	E_DEFECT_JUDGEMENT_MURA_EMD_DARK			,	// EMD
	E_DEFECT_JUDGEMENT_MURA_BN_CORNER			,	// �ڳ� ��Ʈ
	E_DEFECT_JUDGEMENT_MURA_STAMPED				,	// �Ϻ� ����
	E_DEFECT_JUDGEMENT_MURA_NUGI				,	// ����
	E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI			,	// ���� ����
	E_DEFECT_JUDGEMENT_MURA_EDGE_NUGI_			,	// ���� ����
	E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT		,	// ���� ������
	E_DEFECT_JUDGEMENT_MURA_AMORPH_DARK			,	// ��ο� ������
	E_DEFECT_JUDGEMENT_MURA_AMORPH_SMALL_DARK	,	// 
	E_DEFECT_JUDGEMENT_MURA_BOX_SCRATCH			,	//
	E_DEFECT_JUDGEMENT_MURA_CM					,	// ȥ�� ( SVI ��� )
	E_DEFECT_JUDGEMENT_MURA_LINEMURA_X			,	// G64 TYPE2 XLINE
	E_DEFECT_JUDGEMENT_MURA_LINEMURA_Y			,	// G64 TYPE2 YLINE
	E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_X			,	// G64 TYPE1 XLINE
	E_DEFECT_JUDGEMENT_MURA_TYPE1MURA_Y			,	// G64 TYPE1 YLINE
	E_DEFECT_JUDGEMENT_MURANORMAL_NOTCH_DEFECT	,

	// Mura �ҷ� ��
	E_DEFECT_JUDGEMENT_MURA_END = E_DEFECT_JUDGEMENT_MURANORMAL_NOTCH_DEFECT,

	//////////////////////////////////////////////////////////////////////////
	// AVI �ҷ� - Bin2 ��� ������� ���� -> ReTest ����
	//////////////////////////////////////////////////////////////////////////

	// ReTest �ҷ� ����
	E_DEFECT_JUDGEMENT_RETEST_START,

	E_DEFECT_JUDGEMENT_RETEST_POINT_DARK = E_DEFECT_JUDGEMENT_RETEST_START,	// ReTest ����
	E_DEFECT_JUDGEMENT_RETEST_POINT_BRIGHT		,	// ReTest ����Point
	E_DEFECT_JUDGEMENT_RETEST_LINE_BRIGHT		,	// ReTest ���� ����
	E_DEFECT_JUDGEMENT_RETEST_LINE_DARK			,	// ReTest ��ο� ����
	E_DEFECT_JUDGEMENT_RETEST_MURA				,	// ReTest ����
	E_DEFECT_JUDGEMENT_RETEST_MURA_BRIGHT		,	// ReTest ���� ����

	// ReTest �ҷ� ��
	E_DEFECT_JUDGEMENT_RETEST_END = E_DEFECT_JUDGEMENT_RETEST_MURA_BRIGHT,

	//////////////////////////////////////////////////////////////////////////
	// APP ���� �ҷ�
	//////////////////////////////////////////////////////////////////////////

	// APP �ҷ� ����
	E_DEFECT_JUDGEMENT_APP_START,
	
	E_DEFECT_JUDGEMENT_APP_CUTTING	 = E_DEFECT_JUDGEMENT_APP_START	,	// Cutting  
	E_DEFECT_JUDGEMENT_APP_CUT_C_COME_OUT			,	// C-Cut ����
	E_DEFECT_JUDGEMENT_APP_CUT_C_COME_IN			,	//		 ����
	E_DEFECT_JUDGEMENT_APP_MEASURE					,
	E_DEFECT_JUDGEMENT_APP_PAD_BURNT				,
	E_DEFECT_JUDGEMENT_APP_PAD_SCRATCH				,
	E_DEFECT_JUDGEMENT_APP_PAD_DUST					,
	E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLE			,
	E_DEFECT_JUDGEMENT_APP_ACTIVE_SUBSTANCE			,
	E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLESUBSTANCE	,
	E_DEFECT_JUDGEMENT_APP_ACTIVE_SCRATCH			,
	E_DEFECT_JUDGEMENT_APP_ACTIVE_MURA				,
	E_DEFECT_JUDGEMENT_APP_ACTIVE_HILL				,
	E_DEFECT_JUDGEMENT_APP_ACTIVE_FILM_PEELING		,
	E_DEFECT_JUDGEMENT_APP_ACTIVE_DUST				,
	E_DEFECT_JUDGEMENT_APP_CURL                     ,
	

// APP �ҷ� ��
	E_DEFECT_JUDGEMENT_APP_END = E_DEFECT_JUDGEMENT_APP_CURL,

	//////////////////////////////////////////////////////////////////////////

	E_DEFECT_JUDGEMENT_COUNT						// �� ����
};

enum ENUM_PANEL_DEFECT_TREND
{
	E_DEFECT_BP_PLUS_DP					= E_DEFECT_JUDGEMENT_COUNT,
	E_DEFECT_BP_DP_WB_BD_GD_GB_PLUS		,
	E_DEFECT_RETEST						,
	E_DEFECT_BP_WB_GB					,

	E_PANEL_DEFECT_TREND_COUNT					// �� ����
};

enum ENUM_ALGORITHM_NUM
{
	E_ALGORITHM_POINT		= 0	,	// 00 POINT	
	E_ALGORITHM_LINE			,	// 01 LINE	
	E_ALGORITHM_MURA			,	// 02 MURA
	E_ALGORITHM_MURA2			,	// 03 RESERVE00
	E_ALGORITHM_MURA3			,
	E_ALGORITHM_MURA4			,
	E_ALGORITHM_MURA_CHOLE		,	// 2021.01.06 HGM
	E_ALGORITHM_MURA_SCRATCH	,	// 2021.01.06 HGM
	E_ALGORITHM_SVI				,	// 04 RESERVE01
	E_ALGORITHM_MURA_NORMAL		,	// 05 RESERVE00
	E_ALGORITHM_LCP_SUBSTANCE	,	// 06 RESERVE01
	E_ALGORITHM_LCP_SCRATCH		,	// 07 RESERVE05
	E_ALGORITHM_LCP_MOISTURE	,	// 08 RESERVE06
	E_ALGORITHM_LCP_VARIANCE	,	// 08 RESERVE06
	E_ALGORITHM_LCP_CONCEALMENT ,
	E_ALGORITHM_RESERVE07		,	// 09 RESERVE07
	E_ALGORITHM_RESERVE08		,	// 10 RESERVE08
	E_ALGORITHM_RESERVE09		,	// 11 RESERVE09
	E_ALGORITHM_RESERVE10		,	// 12 RESERVE10
	E_ALGORITHM_RESERVE11		,	// 13 RESERVE11
	E_ALGORITHM_RESERVE12		,	// 14 RESERVE12
	E_ALGORITHM_NUMBER				// 15 Total
};

// KSW 17.09.26 - �Ķ���� ����
enum ENUM_PARA_COMMON
{
	E_PARA_COMMON_MAX_DEFECT_COUNT		,	// 00 : �ҷ� �ִ� ���� ( MAX_DEFECT_COUNT ���� �۰� )
	E_PARA_COMMON_IMAGE_SAVE_FLAG		,	// 01 : �˰��� �߰���� Image Save flag
	E_PARA_COMMON_IMAGE_SAVE_COUNT		,	// 02 : ���� ���� ���� ī��Ʈ ( Point�� ��� �Լ��� �ΰ��� ���� �� )
	E_PARA_COMMON_ALG_IMAGE_NUMBER		,	// 03 : ���� Alg ���� ��ȣ
	E_PARA_COMMON_CAMERA_NUMBER			,	// 04 : Cam Number ( ī�޶� ��ȣ / SVI )
	E_PARA_COMMON_ROI_NUMBER			,	// 05 : ROI Number ( ROI ��ȣ )
	E_PARA_COMMON_ALG_NUMBER			,	// 06 : �˰��� ��ȣ ( nAlgorithmNumber_ )
	E_PARA_COMMON_THREAD_ID				,	// 07 : Thread ID
	E_PARA_COMMON_DRAW_DEFECT_NUM_FLAG	,	// 08 : Draw Defect Num ǥ��
	E_PARA_COMMON_DRAW_DUST_FLAG		,	// 09 : Draw Dust ǥ�� ( Point �˰����� ���ƾ� �� )
	E_PARA_COMMON_UI_IMAGE_NUMBER		,	// 10 : UI �� ���� ���� ���� ��ȣ
	E_PARA_COMMON_ROTATE_ANGLE			,	// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
	E_PARA_COMMON_ROTATE_CENTER_X		,	// 12 : Cell ȸ���� �߽� x��ǥ
	E_PARA_COMMON_ROTATE_CENTER_Y		,	// 13 : Cell ȸ���� �߽� y��ǥ
	E_PARA_COMMON_PS_MODE				,	// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	E_PARA_COMMON_TOTAL_COUNT				// Total
};


// AVI & APP ALIGN �Ķ���� ����
enum ENUM_PARA_ALIGN
{
	///////////////////////////// AVI

	// LCP Align Hole ����
	E_PARA_LCP_ALIGN_TEXT = 0,
	E_PARA_LCP_ALIGN_GAUSSIAN_SIZE,
	E_PARA_LCP_ALIGN_GAUSSIAN_SIGMA,
	E_PARA_LCP_ALIGN_TH,
	E_PARA_LCP_ALIGN_ROUND_IN_SZ,
	E_PARA_LCP_ALIGN_ROUND_OUT_SZ,
	E_PARA_LCP_ALIGN_MEDIAN_BLUR,
	E_PARA_LCP_ALIGN_BIGCIRCLE_MINAREA,
	E_PARA_LCP_ALIGN_HOUGHCIRCLE_TEXT,
	E_PARA_LCP_ALIGN_HOUGH_DP,
	E_PARA_LCP_ALIGN_HOUGH_MINDIST,
	E_PARA_LCP_ALIGN_HOUGH_PARAM1,
	E_PARA_LCP_ALIGN_HOUGH_PARAM2,
	E_PARA_LCP_ALIGN_HOUGH_MINRADIUD,
	E_PARA_LCP_ALIGN_HOUGH_MAXRADIUD,
	E_PARA_LCP_ALIGN_ACTIVEMASK_IN,
	E_PARA_LCP_ALIGN_HOLEMASK_OUT,

	// LCP Align Hole ���� (����)
	E_PARA_LCP_ALIGN_LIMPI_TEXT = 0,
	E_PARA_LCP_ALIGN_LIMPI_EDGE_TH,
	E_PARA_LCP_ALIGN_LIMPI_MEDIAN_BLUR,
	E_PARA_LCP_ALIGN_LIMPI_BIGCIRCLE_MINAREA,
	E_PARA_LCP_ALIGN_LIMPI_HOUGHCIRCLE_TEXT,
	E_PARA_LCP_ALIGN_LIMPI_HOUGH_DP,
	E_PARA_LCP_ALIGN_LIMPI_HOUGH_MINDIST,
	E_PARA_LCP_ALIGN_LIMPI_HOUGH_PARAM1,
	E_PARA_LCP_ALIGN_LIMPI_HOUGH_PARAM2,
	E_PARA_LCP_ALIGN_LIMPI_HOUGH_MINRADIUD,
	E_PARA_LCP_ALIGN_LIMPI_HOUGH_MAXRADIUD,
	E_PARA_LCP_ALIGN_LIMPI_ACTIVEMASK_IN,
	E_PARA_LCP_ALIGN_LIMPI_HOLEMASK_OUT,

	////////////////////////////////////////////////////////////////////////// ���� AVI��
	E_PARA_CHECK_MIN_GV = 0,	// ���Ϻ� ��� GV �ּ� ���
	E_PARA_CHECK_MAX_GV,	// ���Ϻ� ��� GV �ִ� ���
	E_PARA_CHECK_AVG_GV,	// ���Ϻ� ���߰��� �ϴ� ���
	E_PARA_CHECK_AREA_FLAG,	// ���� Ȯ�� Flag
	E_PARA_CHECK_AREA_GV,	// ������ GV �̻� ������
	E_PARA_CHECK_AREA_RATIO,	// ���� ���� �߿� ���� ���� �̻��̸� AD
	E_PARA_CHECK_NON_DEL_FLAG,	// ��˻� ���� ���� ��, AD �˻� ( Dust ���� ��� X )
	E_PARA_CHECK_NON_DEL_OFFSET,	// ��˻� ���� ������ ����
	E_PARA_CHECK_PATTERN_LABEL,	// Label Check

	//////////////////////////////////////////////////////////////////////////

	//LCP Align && Mask && Insp Judge����


	E_PARA_LCP_AD_TEXT = 0,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_AD_IMAGE_NUM,	// AD �˻��� ���� ��ȣ ( E_IMAGE_CLASSIFY_AVI_WHITE : 4 )

	E_PARA_LCP_COLORCODE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_COLORCODE_NUM,	// AD �˻��� ���� ��ȣ ( E_IMAGE_CLASSIFY_AVI_WHITE : 4 )

	E_PARA_LCP_BACK_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_LCP_BACK_IMAGE_NUM,	// AD �˻��� ���� ��ȣ ( E_IMAGE_CLASSIFY_AVI_WHITE : 4 )
	E_PARA_LCP_BACK_THRESHOLD,
	E_PARA_LCP_BACK_BIGCIRCLE_MINAREA,
	E_PARA_LCP_BACK_ROUNDIN,

	E_PARA_LCP_MASK_LABEL_TEXT,	// ��ġ�� ( ��� ���ϴ� ��� : 0 )
	E_PARA_LCP_MASK_LABEL_IMAGE_NUM, //12.14
	E_PARA_LCP_MASK_LABEL_TH,	// �ּ� ��� GV
	E_PARA_LCP_MASK_LABEL_GAU_SZ,
	E_PARA_LCP_MASK_LABEL_GAU_SIG,
	E_PARA_LCP_MASK_LABEL_MAX,
	E_PARA_LCP_MASK_LABEL_MIN,
	E_PARA_LCP_MASK_LABEL_ROUND_IN,
	E_PARA_LCP_MASK_LABEL_ROUND_OUT,

	E_PARA_LCP_MASK_VACUUM_TEXT,
	E_PARA_LCP_MASK_VACUUM_IMAGE_NUM, //12.14
	E_PARA_LCP_MASK_VACUUM_IMAGE_TRESHOLD,
	E_PARA_LCP_MASK_VACUUM_IMAGE_MIN_AREA,
	E_PARA_LCP_MASK_VACUUM_IMAGE_ROUNDOUT,

	E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_TEXT,
	E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_MAIN,
	E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_COMPARE,
	E_PARA_LCP_INSPECT_SUBSTANCE_IMAGE_DUST,

	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_TEXT,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_CROP_OFFSET,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_SEG_X,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_SEG_Y,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_DEFECT_DARK_TH_RATIO,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_BG_DARK_TH_RATIO,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_AXIS_RATIO,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_AREA_RATIO,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_BG_DEFECT_SIZE,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_BG_CENTER_DEFECT_RATIO,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_CENTER_DIFF,
	E_PARA_LCP_INSPECT_SUBSTANCE_JUDGE_CENTER_DIFF_RATIO,

	E_PARA_LCP_INSPECT_MOISTURE_JUDGE_TEXT,
	E_PARA_LCP_INSPECT_MOISTURE_JUDGE_AREA_RATIO,

	E_PARA_LCP_INSPECT_VARIANCE_JUDGE_TEXT,
	E_PARA_LCP_INSPECT_VARIANCE_JUDGE_AREA_RATIO,

	E_PARA_LCP_AREA_IMG_TRANSFORM_TEXT,
	E_PARA_LCP_AREA_IMG_TRANSFORM_ORI_WIDTH,
	E_PARA_LCP_AREA_IMG_TRANSFORM_ORI_HEIGHT,
	E_PARA_LCP_AREA_IMG_TRANSFORM_X,
	E_PARA_LCP_AREA_IMG_TRANSFORM_Y,

	E_PARA_LCP_STAGE_SUBSTANCE_TEXT,
	E_PARA_LCP_STAGE_SUBSTANCE_USE_FLAG,
	E_PARA_LCP_STAGE_SUBSTANCE_BLUR_SIZE,
	E_PARA_LCP_STAGE_SUBSTANCE_THRESHOLD,
	E_PARA_LCP_STAGE_SUBSTANCE_JUDGE_RATIO,

	E_PARA_LCP_CANDIDATE_DELETE_TEXT,
	E_PARA_LCP_CANDIDATE_DELETE_FLAG,

	E_PARA_LCP_MOISTURE_SUBSTANCE_DELETE_TEXT,
	E_PARA_LCP_MOISTURE_SUBSTANCE_DELETE_FLAG,

	E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_TEXT,


		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_FLAG,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_DEGREE_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_DEGREE_MAX,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_LENGHT_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_LENGHT_MAX,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_L_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC1_L_MAX,

		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_FLAG,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_DEGREE_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_DEGREE_MAX,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_LENGHT_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_LENGHT_MAX,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_L_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_RED_SPEC2_L_MAX,

		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_FLAG,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_DEGREE_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_DEGREE_MAX,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_LENGHT_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_LENGHT_MAX,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_L_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC1_L_MAX,


		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_FLAG,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_DEGREE_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_DEGREE_MAX,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_LENGHT_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_LENGHT_MAX,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_L_MIN,
		E_PARA_LCP_SUBSTANCE_COLOR_CLASSIFY_BLUE_SPEC2_L_MAX,


	////////////////////////////////////////////////////////////////////////// ���� AVI
	E_PARA_AD_TEXT = 0,	// �ؽ�Ʈ ǥ��
	E_PARA_AD_IMAGE_NUM						,	// AD �˻��� ���� ��ȣ ( E_IMAGE_CLASSIFY_AVI_WHITE : 4 )
	E_PARA_AD_RATE							,	// ��ġ�� ( ��� ���ϴ� ��� : 0 )
	E_PARA_AD_ZOOM							,	// ���� ��� ����
	E_PARA_AD_MIN_GV						,	// �ּ� ��� GV
	E_PARA_AD_MAX_GV						,	// �ִ� ��� GV
	E_PARA_AD_MAX_STD						,	// �ִ� ǥ������
	E_PARA_AD_Y_LINE						,	// ������ ���� ����
	E_PARA_AD_BLOCK_X_GV					,	// ���� ���� GV ����
	E_PARA_AD_BLOCK_X_AREA					,	// ���� ���� ���� ����
	E_PARA_AD_BLOCK_Y_GV					,	// ���� ���� GV ����
	E_PARA_AD_BLOCK_Y_AREA					,	// ���� ���� ���� ����

	// Size
	E_PARA_CELL_SIZE_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_CELL_SIZE_X						,	// ����� Cell ���� ���� ( pixel )
	E_PARA_CELL_SIZE_Y						,	// ����� Cell ���� ���� ( pixel )

	// Round Cell ����
	E_PARA_ROUND_TEXT						,	// �ؽ�Ʈ ǥ��
	E_PARA_ROUND_SETTING					,	// ���� �÷���
	E_PARA_ROUND_USE						,	// Round �ڳʺ� ã�� �˰��� ��� ��/��
	E_PARA_ROUND_IN							,	// Round �ڳʺ� �������� �˻� ���� ����
	E_PARA_ROUND_FIND_OFFSET				,	// Round ���� ã�� ���� ���� ( ��,��,��,�� )
	E_PARA_ROUND_DUST_MIN_GV				,	// Dust���� - �ܰ� ��� ä�ﶧ, �ּ� ��� GV ����
	E_PARA_ROUND_OTHER_MIN_GV				,	// Dust �̿� ���� - �ܰ� ��� ä�ﶧ, �ּ� ��� GV ����

	// Camera Hole ����
	E_PARA_CHOLE_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_CHOLE_SETTING,	// ���� �÷���
	E_PARA_CHOLE_USE,	// CHole ã�� �˰��� ON/OFF
	E_PARA_CHOLE_IN,	// CHole �ٱ������� �˻� ���� ����
	E_PARA_CHOLE_FIND_OFFSET,	// CHole ���� ã�� ���� ���� ( ��, ��, ��, ��)
	E_PARA_CHOLE_ROI_MIN_GV,	// ���� ���� - CHole ROI �ּ� GV Ȯ��
	E_PARA_CHOLE_SMALL_DEFECT_SIZE,	// ���� �ҷ� Judgement �ҷ� ũ�� ����
	E_PARA_CHOLE_SMALL_DEFECT_OFFSET,	// ���� �ҷ� judgement ���� ���� Offset
	E_PARA_CHOLE_BIG_DEFECT_OFFSET,	// ū �ҷ� judgement ���� ���� Offset
	E_PARA_CHOLE_LINE_DELETE_FLAG,	// ����μ� ���� Defect ���� Flag (���� �ҷ��� �ȳ����� ���� ��� �ش� ��� OFF)

	// Shift Copy ����
	E_PARA_SHIFT_COPY_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_SHIFT_COPY_R						,	// x���� ? Pixel �̵�
	E_PARA_SHIFT_COPY_G						,	// y���� ? Pixel �̵�
	E_PARA_SHIFT_COPY_B						,	// x���� �ݺ� Ƚ��

	// Align ����
	E_PARA_ALIGN_TEXT						,	// �ؽ�Ʈ ǥ��
	E_PARA_ALIGN_THRESHOLD					,	// ����ȭ
	E_PARA_ALIGN_MORP						,	// Cell ���� ����� ä��� �뵵
	E_PARA_ALIGN_ANGLE_ERR					,	// �ش� ������ ����� �߾˶� �߻�
	E_PARA_ALIGN_ANGLE_WAR					,	// �ش� ������ ����� ��˶� �߻�	

	// Align ���� Morp ����
	E_PARA_ALIGN_PARTIAL_USE				,	// USE OFF : 0 / ON : 1
	E_PARA_ALIGN_PARTIAL_THRESHOLD			,	// ����� ã������ �뵵
	E_PARA_ALIGN_PARTIAL_OUTER				,	// ROI ���� OFFSET
	E_PARA_ALIGN_PARTIAL_LEFT_OFFSET		,	// ROI LEFT OFFSET
	E_PARA_ALIGN_PARTIAL_RIGHT_OFFSET		,	// ROI RIGHT OFFSET
	E_PARA_ALIGN_PARTIAL_TOP_OFFSET			,	// ROI TOP OFFSET
	E_PARA_ALIGN_PARTIAL_BOTTOM_OFFSET		,	// ROI BOTTOM OFFSET

	// Group ����
	E_PARA_GROUP_TEXT						,	// �ؽ�Ʈ ǥ��
	E_PARA_GROUP_FLAG						,	// �׷� ���� ��� ��/��
	E_PARA_GROUP1_COUNT						,	// �׷�1 ���� �ȼ� ����
	E_PARA_GROUP1_DIAMETER					,	// �׷�1 ���� ����
	E_PARA_GROUP1_DELETE					,	// �׷�1 ����
	E_PARA_GROUP2_COUNT						,	// �׷�2 ���� �ȼ� ����
	E_PARA_GROUP2_DIAMETER					,	// �׷�2 ���� ����
	E_PARA_GROUP2_DELETE					,	// �׷�2 ����
	E_PARA_GROUP3_COUNT						,	// �׷�3 ���� �ȼ� ����
	E_PARA_GROUP3_DIAMETER					,	// �׷�3 ���� ����
	E_PARA_GROUP3_DELETE					,	// �׷�3 ����

	// Pad �� ���� �ӽ� ����
	E_PARA_PAD_BRIGHT_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_PAD_BRIGHT_FLAG					,	// Pad �� ���� 
	E_PARA_PAD_BRIGHT_X						,	// Pad �� ���� ����
	E_PARA_PAD_BRIGHT_Y						,	// Pad �� ���� ����

	// ȥ�� ���� �߻� -> ������� ����� ������ ���� ( ���� ���� ��� ET ���� )
	E_PARA_HS_JUDGMENT_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_HS_JUDGMENT_COUNT_BRIGHT			,	// ���� ����
	E_PARA_HS_JUDGMENT_COUNT_DARK			,	// ���� ����

	// ���� ���� �����μ� �ҷ� ��ó�� ����Ʈ ������ ����
	E_PARA_STRONG_DEFECT_NOISE_TEXT			,	// �ؽ�Ʈ ǥ��
	E_PARA_STRONG_DEFECT_NOISE_ADJUST_RANGE	,	// ���� �ҷ��� ���� Ȯ�� ����

	// PCD_CRACK : ��˻� ���� �ȿ� ���� �ҷ� �����ϴ� ���, �ҷ��� ����
	E_PARA_PCD_CRACK_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_PCD_CRACK_FLAG					,	// PCD CRACK ��� ��/��

	// Non Insp Area
	E_PARA_NON_INSP_AREA_TEXT				,	// �ؽ�Ʈ ǥ��
	E_PARA_NON_INSP_AREA_ONLY_LINE			,	// ���� �ҷ��� ��츸 ����

	// Dimple ���� Ratio
	E_PARA_DIMPLE_RATIO_TEXT				,	// �ؽ�Ʈ ǥ��
	E_PARA_DIMPLE_RATIO_GRAY_Active			,	// DIMPLE ���� Ratio Active
	E_PARA_DIMPLE_RATIO_GRAY_Edge			,	// DIMPLE ���� Ratio Edge
	E_PARA_DIMPLE_RATIO_RGB					,	// DIMPLE ���� Ratio

	// �ѵ��� Spot ���� Ratio
	E_PARA_SISPOT_TEXT						,	// �ؽ�Ʈ ǥ��
	E_PARA_SISPOT_RATIO_ACTIVE				,	// ACTIVE SPOT ���� RATIO �� 1�� ����
	E_PARA_SISPOT_RATIO_EDGE				,	// EDGE SPOT ���� RATIO �� 1�� ����
	E_PARA_SISPOT_NUMBER_GVCOUNT			,	// GV COUNT
	E_PARA_SISPOT_AREA						,	// GV COUNT

	// �ѵ��� ������Spot ���� Ratio											
	E_PARA_STRONG_SISPOT_TEXT,	// �ؽ�Ʈ ǥ��
	E_PARA_STRONG_SISPOT_RATIO_ACTIVE,	// ACTIVE SPOT ���� RATIO �� 1�� ����
	E_PARA_STRONG_SISPOT_RATIO_EDGE,	// EDGE SPOT ���� RATIO �� 1�� ����
	E_PARA_STRONG_SISPOT_NUMBER_GVCOUNT,	// GV COUNT
	E_PARA_STRONG_SISPOT_AREA,	// GV COUNT

	// ���� Ȯ�� Logic
	E_PARA_DUST_DARKPOINT_TEXT				,	// �ؽ�Ʈ ǥ��
	E_PARA_DUST_DARKPOINT_ONOFF				,	// ACTIVE SPOT ���� RATIO �� 1�� ����

	// Camera Tap Overkill Delet
	E_PARA_CAMERA_TAP_OVERKILL_TEXT			,	// �ؽ�Ʈ ǥ��
	E_PARA_CAMERA_TAP_OVERKILL_ONOFF		,	// �ؽ�Ʈ ǥ��
	E_PARA_CAMERA_TAP_OVERKILL_CAMERATYPE	,	// ����
	E_PARA_CAMERA_TAP_OVERKILL_OFFSET		,	// �ؽ�Ʈ ǥ��

	// Norch �� Overkill Delet
	E_PARA_NORCH_OVERKILL_TEXT				,	// 
	E_PARA_NORCH_OVERKILL_ONOFF				,	// 
	E_PARA_NORCH_OVERKILL_UNIT_X			,	// 
	E_PARA_NORCH_OVERKILL_UNIT_Y			,	// 
	E_PARA_NORCH_OVERKILL_THGV_GREEN		,	// 
	E_PARA_NORCH_OVERKILL_THGV_RB			,	// 
	E_PARA_NORCH_OVERKILL_ABS_GREEN			,	// 
	E_PARA_NORCH_OVERKILL_ABS_RB			,	// 
	E_PARA_NORCH_OVERKILL_OFFSET			,	//

	E_PARA_LASTGROUP_JUDGE_TEXT				,
	E_PARA_LASTGROUP_JUDGE_ALL_COUNT		,
	E_PARA_LASTGROUP_JUDGE_BRIGHT_COUNT		,
	E_PARA_LASTGROUP_JUDGE_DARK_COUNT		,


	// Mura Normal Classification Parameter
	E_PARA_MURANORMAL_CLASSPARA_TEXT,	//
	E_PARA_MURANORMAL_CLASSPARA_TYPE1,	//
	E_PARA_MURANORMAL_CLASSPARA_TYPE2,	//
	E_PARA_MURANORMAL_CLASSPARA_TYPE3,	//
	E_PARA_MURANORMAL_CLASSPARA_TYPE4,	//

	// Mura Normal 2nd Judgement OverKill Reduction (T3)
	E_PARA_MURANORMAL_OR_TEXT,	//
	E_PARA_MURANORMAL_OR_TYPE3_BBS_DIFFGV,	//
	E_PARA_MURANORMAL_OR_TYPE3_SBS_DIFFGV,	//
	E_PARA_MURANORMAL_OR_TYPE3_CH_DIFFGV,	//
	E_PARA_MURANORMAL_OR_TYPE3_BUBBLEMERGE,	//

	// Point - point �ߺ� ���� offset
	E_PARA_Duplicate_offset,

	// 2021.01.12 HGM
	// Image ȸ�� ����
	E_PARA_IMAGE_ROTATE_TEXT				,	// �ؽ�Ʈ ǥ��
	E_PARA_IMAGE_ROTATE						,

	///////////////////////////////// SVI

	// AD GV ���� ( ��� üũ )
	E_PARA_SVI_CHECK_MIN_GV			= 0	,	// ���Ϻ� ��� GV �ּ� ���
	E_PARA_SVI_CHECK_MAX_GV				,	// ���Ϻ� ��� GV �ִ� ���

	///////////////////////////////////////////////////////////////////////////////////////////////

	// AD ����
	E_PARA_SVI_AD_TEXT				= 0	,	// �ؽ�Ʈ ǥ��
	E_PARA_SVI_AD_IMAGE_NUM				,	// AD �˻��� ���� ��ȣ ( E_IMAGE_CLASSIFY_AVI_WHITE : 4 )
	E_PARA_SVI_AD_RATE					,	// ��ġ�� ( ��� ���ϴ� ��� : 0 )
	E_PARA_SVI_AD_ZOOM					,	// ���� ��� ����
	E_PARA_SVI_AD_MIN_GV				,	// �ּ� ��� GV
	E_PARA_SVI_AD_MAX_GV				,	// �ִ� ��� GV
	E_PARA_SVI_AD_MAX_STD				,	// �ִ� ǥ������

	// Size
	E_PARA_SVI_CELL_SIZE_TEXT			,	// �ؽ�Ʈ ǥ��
	E_PARA_SVI_CELL_COAX_SIZE_X			,	// ���� ī�޶� ����� Cell ���� ���� ( pixel )
	E_PARA_SVI_CELL_COAX_SIZE_Y			,	// ���� ī�޶� ����� Cell ���� ���� ( pixel )
	E_PARA_SVI_CELL_SIDE_SIZE_X			,	// ���̵� ī�޶� ����� Cell ���� ���� ( pixel )
	E_PARA_SVI_CELL_SIDE_SIZE_Y			,	// ���̵� ī�޶� ����� Cell ���� ���� ( pixel )

	// Align ����
	E_PARA_SVI_ALIGN_TEXT				,	// �ؽ�Ʈ ǥ��
	E_PARA_SVI_ALIGN_THRESHOLD			,	// ����ȭ

	// Round Cell ����
	E_PARA_SVI_ROUND_TEXT				,	// �ؽ�Ʈ ǥ��
	E_PARA_SVI_ROUND_USE				,	// Round �ڳʺ� ã�� �˰��� ��� ��/��
	E_PARA_SVI_ROUND_IN					,	// Round �ڳʺ� �������� �˻� ���� ����
	E_PARA_SVI_ROUND_SIZE				,	// Round ���� ���� ����
	E_PARA_SVI_ROUND_DIAGONAL			,	// Round ���� �ڳ� ����

	E_PARA_SVI_CROP_RATIO				,


	E_PARA_SVI_TOTAL_COUNT				,	// Total


	///////////////////////////// APP

	//Curl
	E_PARA_APP_CURL_TEXT				=0	,	// �ؽ�Ʈ ǥ��
	E_PARA_APP_CURL_USE						,	// Curl Algorithm ���
	E_PARA_APP_CURL_RANGE_X				    ,	// Profile�� � �� ��� �� ������
	E_PARA_APP_CURL_RANGE_Y_TOP			    ,	// Profile�� ������ ���� ������ TOP
	E_PARA_APP_CURL_RANGE_Y_BOTTOM			,   // Profile�� ������ ���� ������ BOTTOM
	E_PARA_APP_CURL_IMAGE_NUM				,	// Curl Algorithm���� ����� Image number
	E_PARA_APP_CURL_CONDITION_COUNT         ,	// ��� Size ���� ū Size�� � �̻��� �� Curl�� �Ǵ��� ������
	E_PARA_APP_CURL_SEARCH_RATIO        	,	// ��� Size�� �� �踦 Curl�� ����
	E_PARA_APP_CURL_STANDARD_GV_RATIO       ,	// Curl�� ���� �Ǵ� �κ��̶�� ���� ���ؿ��� �� % �����ͼ� ������ ������
	E_PARA_APP_CURL_CORNER_SEARCH_COUNT	 	,	// Corner �κ��� Curl Parameter, ��� Size ���� ū Size�� � �̻��� �� Curl�� �Ǵ��� ������
	E_PARA_APP_CURL_CORNER_SEARCH_RATIO	    ,	// Corner �κ��� Curl Parameter, ��� Size�� �� �踦 Curl�� ����
	E_PARA_APP_CURL_BLUR_SIZE				,	// Blur Mask Size
	E_PARA_APP_CURL_GAUSSIGMA				,	// ����� Cell ���� ���� ( pixel )
	E_PARA_APP_CURL_START_OFFSET_LEFT		,	// C-Cut �κ��� �����ϱ� ���� ���� ������ Offset 
	E_PARA_APP_CURL_END_OFFSET_RIGHT		,   // C-Cut �κ��� �����ϱ� ���� ���� ���� Offset
	E_PARA_APP_CURL_PROFILE_SEARCH_OFFSET	,	// Profile���� Min �� ���ķ� Offset ũ�⸦ �ʺ�� ����� ��ŭ ���� ����
	E_PARA_APP_RESERVE2						,
	E_PARA_APP_RESERVE3						,
	E_PARA_APP_PAD_EDGE_TEXT				,
	E_PARA_APP_PAD_EDGE_THETA				,

	//AD Check
	E_PARA_APP_AD_TEXT						,	// �ؽ�Ʈ ǥ��
	E_PARA_APP_AD_IMAGE_NUM					,	// AD Check ����� Image
	E_PARA_APP_AD_BLUR_SIZE					,	// Blur Size
	E_PARA_APP_AD_THRESHOLD					,	// ����ȭ ��
	E_PARA_APP_AD_LIMIT_AREA				,	// ���� ���󿡼� �ּҷ� �����ؾ��ϴ� ũ�� ����
	E_PARA_APP_AD_GV						,	// AD Check GV ��
	
	// Align ����
	E_PARA_APP_ALIGN_TEXT					,	// �ؽ�Ʈ ǥ��
	E_PARA_APP_ALIGN_THRESHOLD				,	// ����ȭ ��
	E_PARA_APP_ALIGN_DILATE					,	// ����ȭ �� �� ���� ū Blob ���� �Ϳ� ����ϴ� ��â
	E_PARA_APP_ALIGN_PANEL_EDGE_THETA		,	// Cell �ܰ� Edge Detection ���� Theta	
	E_PARA_APP_ALIGN_ANGLE_ERR				,	// �ش� ������ ����� �߾˶� �߻�
	E_PARA_APP_ALIGN_ANGLE_WAR				,	// �ش� ������ ����� ��˶� �߻�
	E_PARA_APP_ALIGN_AVG_OFFSET				,
	E_PARA_APP_ALIGN_USE_OVERRANGE			,	// Align �� Pannel Size ���� ��� ��� ���� ó��
	E_PARA_APP_ALIGN_RANGE_OVER				,	// Align �� Pannel Size ���� ����� AD �ҷ�ó��

	//Polygon ���� ó�� ����
	E_PARA_APP_POLYGON_TEXT					,	//�ؽ�Ʈ ǥ��
	E_PARA_APP_MAKE_ACTIVE_MASK				,	//Polygon Mask ����� Flag
	E_PARA_APP_POLYGON_IMAGE_NUM			,	//ROUND ó�� �� ����� �̹���
	E_PARA_APP_POLYGON_RESIZE				,	//Resize�ϴ� ���� ũ��
	E_PARA_APP_POLYGON_GAUS_SIZE			,	//Gaussian Blur Mask ũ��
	E_PARA_APP_POLYGON_GAUS_SIGMA			,	//Gaussian Blur Sigma ũ��
	E_PARA_APP_POLYGON_THRESHOLD			,	//Edge ���� �� ����ϴ� ����ȭ ��
	E_PARA_APP_POLYGON_OPEN_SIZE			,	//Open ���� Mask ũ��	
	E_PARA_APP_POLYGON_SELECT_BLOB			,	//���������� Blob �� �� ��°�� ū ���� ������ ������
	E_PARA_APP_POLYGON_DILATE_SIZE			,	//Dilate ���� Mask ũ��
	E_PARA_APP_POLYGON_ERODE_SIZE			,	//Erode ���� Mask ũ��
	E_PARA_APP_POLYGON_CORNER_ROI_SIZE		,	//Corner ���� ����� ROI ũ��
	E_PARA_APP_POLYGON_BM_THETA_LT			,	//BM Line Search�� ���� ����
	E_PARA_APP_POLYGON_BM_THETA_RB			,	
	E_PARA_APP_POLYGON_BM_IGNORE_GV			,	//BM Line Search�� ���� ���� ������ �ּ� GV �� ����
	E_PARA_APP_POLYGON_BM_PRE_THRESH_RATIO	,	//BM Line Blob ����� ���� �ٸ� ¥���� Blob�� ���ֱ� ���� Threshold �������� ����ϴ� ��� ���� ����� ����
	E_PARA_APP_ADGV_TEXT					,	//ADGV TEXT
	E_PARA_APP_ADGV_MEAN_GV					,	//ADGV ���� ��� GV
	E_PARA_APP_RESERVE27					,
	E_PARA_APP_RESERVE28					,	
	E_PARA_APP_RESERVE29					,
	E_PARA_APP_RESERVE30					,
	E_PARA_APP_RESERVE31					,
	E_PARA_APP_RESERVE32					,
	E_PARA_APP_RESERVE33					,
	E_PARA_APP_RESERVE34					,

	// Align Area
	E_PARA_ALIGN_AREA_ACTIVE_L				,	// 
	E_PARA_ALIGN_AREA_ACTIVE_R				,	// 
	E_PARA_ALIGN_AREA_ACTIVE_T				,	// 
	E_PARA_ALIGN_AREA_ACTIVE_B				,	// 
	E_PARA_ALIGN_AREA_PAD_WIDTH				,	// 
	E_PARA_ALIGN_AREA_PAD_LOCATION			,	// 

	E_PARA_TOTAL_COUNT							// Total
};

enum ENUM_COLOR_CODE
{
	E_COLOR_CODE_WHITE = 0,
	E_COLOR_CODE_BLACK ,
	E_COLOR_CODE_MP ,
	E_COLOR_CODE_TRANSPARENCY ,
	E_COLOR_CODE_GRAY
};
#endif