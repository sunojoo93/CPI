////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare Algorithm DLL Interface Function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DEFINE_INTERFACE_H
#define DEFINE_INTERFACE_H

/***************************************************************************************************
 Algorithm DLL �� Algorithm Task �� ������ �ʿ��� ENUM, ����ü ���� ���⿡ ���� - AVI / SVI / APP ���� ����
***************************************************************************************************/

#pragma once

#include "Define.h"
#include "DefineInterface_SITE.h"

//////////////////////////////////////////////////////////////////////////
// ENUM
//////////////////////////////////////////////////////////////////////////
enum ENUM_DEFECT_COLOR
{
	E_DEFECT_COLOR_DARK			=0	,	// 00 ��ο� �ҷ�
	E_DEFECT_COLOR_BRIGHT			,	// 01 ���� �ҷ�

	E_DEFECT_COLOR_COUNT				// 02 Total
};

// �ε�ȣ ������ ( <, >, ==, <=, >= )
enum ENUM_SIGN_OF_INEQUALITY
{
	E_SIGN_EQUAL						= 0	,	// x == judgment value
	E_SIGN_NOT_EQUAL						,	// x != judgment value
	E_SIGN_GREATER							,	// x >  judgment value
	E_SIGN_LESS								,	// x <  judgment value
	E_SIGN_GREATER_OR_EQUAL					,	// x >= judgment value
	E_SIGN_LESS_OR_EQUAL						// x <= judgment value
};

// 2017.06.08 ���� �ڵ� AVI/SVI/APP �������� ����
// ���� �ڵ� ���� ��
enum ENUM_ERROR_CODE
{
	E_ERROR_CODE_FALSE					= -1	,	// ������ - ���� ���� �Լ� ����

	// ���� : 000����
	E_ERROR_CODE_TRUE					= 000	,	// ����

	E_ERROR_CODE_EMPTY_BUFFER			= 001	,	// ���� ���� ���� ���
	E_ERROR_CODE_EMPTY_PATH						,	// ���� ��ΰ� ���� ���.
	E_ERROR_CODE_EMPTY_PARA						,	// �˻� �Ķ���Ͱ� ���� ���.
	E_ERROR_CODE_EMPTY_SET_ROI					,	// ROI ������ ���� ���.
	E_ERROR_CODE_IMAGE_CLASSIFY_OVER			,	// ���� ��ȣ�� 0~9�� �ƴ� ���.
	E_ERROR_CODE_ROI_OVER						,	// �˻� ������ ���� ũ�⸦ ��� ���.
	E_ERROR_CODE_INCOMPATIBLE_BUFFER_TYPE		,	// Mat ���� Ÿ���� �ٸ� ���.
	E_ERROR_CODE_TIME_OUT						,	// ���� �ð� ������ ���
	E_ERROR_CODE_IMAGE_GRAY						,	// Gray ������ �ƴ� ���
	E_ERROR_CODE_IMAGE_COLOR					,	// Color ������ �ƴ� ���
	E_ERROR_CODE_IMAGE_SIZE						,	// ���� ũ�Ⱑ �ٸ� ���
	E_ERROR_CODE_EMPTY_RGB						,	// R,G,B ���� AD or ���� ���
	E_ERROR_CODE_NON_USE						,	// LCP �˻翡 �ʼ� �˻� üũ �ȵ��ִ� ��� �˻� �н�

	// Align : 100����
	E_ERROR_CODE_ALIGN_NO_DATA			= 101	,	// ������ ���� & ������ ���� ���.
	E_ERROR_CODE_ALIGN_WARNING_PARA				,	// �߸��� �Ķ���͸� ������ ���.
	E_ERROR_CODE_ALIGN_NO_FIND_CELL				,	// Cell�� ã�� �� ���� ���.
	E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR		,	// ������ ���� Ʋ���� ���. (ERROR - �˻� �Ұ�)
	E_ERROR_CODE_ALIGN_ANGLE_RANGE_WARNING		,	// ������ ���� Ʋ���� ���. (WARNING - �˻� ����)
	E_ERROR_CODE_ALIGN_CAN_NOT_CALC				,	// ���� ��� �Ұ��� ���.
	E_ERROR_CODE_ALIGN_DISPLAY					,	// ���÷��� �̻�.
	E_ERROR_CODE_ALIGN_IMAGE_OVER				,	// ���� ��ǥ ���.
	E_ERROR_CODE_ALIGN_WRONG_SLOPE				,	// ���� �߸��� �� ���� (SVI)
	E_ERROR_CODE_ALIGN_ROUND_SETTING			,	// Round ���� �÷��� On�� ��� �˻� ����
	E_ERROR_CODE_ALIGN_LENGTH_RANGE_ERROR		,	// Align �� Pannel Size Range ��� ��� �˻� ����.

	// Point : 200����
	E_ERROR_CODE_POINT_DEFECT_NO		= 201	,	// ����Ʈ �ҷ��� �ƴ� ���.
	E_ERROR_CODE_POINT_JUDEGEMENT_E				,	// Judgement E�� ���� ���Ͽ�
	E_ERROR_CODE_POINT_RES_VECTOR_NULL			,	// ����Ʈ �ҷ� ��� ���Ͱ� NULL�� ���.
	E_ERROR_CODE_POINT_NOT_USE_DUST_ALG			,	// Dust �˰��� ��� ���� ���.
	E_ERROR_CODE_POINT_WARNING_PARA				,	// �߸��� �Ķ���͸� ������ ���.
	E_ERROR_CODE_POINT_TIME_OVER				,	// ���� �ð� �� ���ð��� ���� �ɸ��� ���. ( ���� 10�� ���� )

	// Line : 300�� ��
	E_ERROR_CODE_LINE_HISTO				= 301	,	// 

	// Mura : 400�� ��
	E_ERROR_CODE_MURA_WRONG_PARA		= 401	,	// Mura �Ķ���� �̻�
	E_ERROR_CODE_MURA_HISTO						,	//
	E_ERROR_CODE_MURA_RGB_LINE_DEFECT			,	// RGB Line Mura �ҷ� ����

	// SVI : 500�� ��
	E_ERROR_CODE_SVI_EMD_ESTMATION		= 501	,	// Cell ���� ���� ã�� ����
	E_ERROR_CODE_SVI_WRONG_PARAM				,

	// Empty
	E_ERROR_CODE_EMPTY					= 600	,	// 

	// APP : 700 ����
	E_ERROR_CODE_CUTTING_MARK_TYPE_PARA	= 701	,
	E_ERROR_CODE_PAD_TEACHING_ABNORMAL			,	// PAD Teaching�� ����������.
	E_ERROR_CODE_PAD_MASK_REF_EMPTY				,	// PAD MASK ���� ���� Ȥ�� ������ ����.
	E_ERROR_CODE_PAD_REF_IMG_EMPTY_BUFFER		,	// PAD Reference ���� ����
	E_ERROR_CODE_PAD_MASK_SIZE					,	// PAD MASK Size �̻�
	E_ERROR_CODE_MEASURE_BM_FITTING_FAIL		,	// ���� ���� �� BM Line Fitting ����
	E_ERROR_CODE_MEASURE_PANEL_FITTING_FAIL		,	// ���� ���� �� Panel Line Fitting ����
	E_ERROR_CODE_EMPTY_CROSS_MARK_BUFFER		,	// ���� Mark ���� ����
	E_ERROR_CODE_CROSS_MARK_SEARCH_FAIL			,	// ���� Mark ã�� ����
	E_ERROR_CODE_APP_HISTO						,
	E_ERROR_CODE_CCUT_REF_IMG_BUFFER			,	// CCut Reference ���� ����
	E_ERROR_CODE_CCUT_ROI_MATCHING_FAIL			,	// CCut ROI Matching ���� (Panel �ջ� Ȥ�� ���� �ջ�)
	E_ERROR_CODE_PAD_PARAM_IS_NULL				,	// Pad Area�� �Ķ���Ͱ� �������� ����
	E_ERROR_CODE_PAD_INSP_ROI_OVER				,	// PAD INSP ROI Size ����
	E_ERROR_CODE_ACTIVE_MASK					,	// Active Mask�� ���� ����
	E_ERROR_CODE_ACTIVE_FEATURE_SIZE			,	// Active Feature Size ����

	// CCD Defect : 800 ����
	E_ERROR_CODE_CCD_EMPTY_BUFFER		= 801	,	// ���� ���� ���� ���
	E_ERROR_CODE_CCD_PS_BUFFER					,	// PS ���� ������ ���
	E_ERROR_CODE_CCD_WARNING_PARA				,	// �߸��� �Ķ���͸� ������ ���.
	E_ERROR_CODE_CCD_NOT_LOAD					,	// Load �ȵ� ���
	E_ERROR_CODE_CCD_EMPTY_FILE					,	// ������ ���� ���

	E_ERROR_CODE_LCP_STAGE_DIRTY        = 1001  ,   //LCP Stage ���� ���� ���
	E_ERROR_CODE_LCP_ALIGN_IMAGE_NON_BUFFER		,
	E_ERROR_CODE_LCP_ALIGN_BACK_MASK_FAIL,
	E_ERROR_CODE_LCP_ALIGN_ACTIVE_MASK_FAIL,

};

enum ENUM_PAD_AREA_NAME
{
	E_PAD_REF					=0	, // �е� �� ���۷��� �̹��� & ��ǥ
	E_PAD_FIDUCIAL					, // Fiducial Mark & ���� ����Ʈ �̹��� �� ��ǥ	
	E_PAD_ALIGN_MARK				, // �е�� ���� ��ũ �̹��� �� ��ǥ
	E_PAD_INSP_AREA					, // �е� �˻� ���� �̹��� �� ��ǥ
	E_PAD_NONE_AREA					, // �е� ��˻� ���� �̹��� �� ��ǥ
	E_PAD_AREA_COUNT				
};

//////////////////////////////////////////////////////////////////////////
// ����ü
//////////////////////////////////////////////////////////////////////////

//���� Data ����� ����ü 2017.09.20 LJM
struct stMeasureInfo
{
	int		nMeasureValueSize;
	CString strPanelID;
	bool	bJudge;

	double* dMeasureValue;

	double  dCameraResolution;

	stMeasureInfo(int nMaxSize)
	{
		dMeasureValue = new double[nMaxSize];
		memset(dMeasureValue, 0, sizeof(double) * nMaxSize);
		nMeasureValueSize = nMaxSize;
		bJudge=true;
	}
	~stMeasureInfo()
	{
		delete[] dMeasureValue;
	}
};

struct stActiveBurntROI
{
	int	nMaxROINum;
	Rect *rtROI;
	
	stActiveBurntROI(int nROINum)
	{
		rtROI = new Rect[nROINum];
		nMaxROINum = nROINum;
	}
	~stActiveBurntROI()
	{
		delete[] rtROI;
	}
};

#pragma pack(push)
#pragma pack(1)

struct STRU_JUDGEMENT
{
	BOOL bUse;			// ���� �׸� ���� ��/��
	int nSign;			// ������ ( <, >, ==, <=, >= )
	double dValue;      // ��

	// ����ü �ʱ�ȭ
	struct STRU_JUDGEMENT()  
	{
		memset(this, 0, sizeof(STRU_JUDGEMENT));   
	}
};

struct STRU_DEFECT_ITEM
{
	BOOL bDefectItemUse;   // �˰��� ��� ����
	TCHAR strItemName[50];

	STRU_JUDGEMENT Judgment[2 * MAX_MEM_SIZE_E_DEFECT_JUDGMENT_COUNT]; // Defect ���� �׸� �� ���� 2��

	STRU_DEFECT_ITEM()
	{
		memset(this, 0, sizeof(STRU_DEFECT_ITEM));     
	}
};

struct STRU_PARAM_ALG
{
	BOOL bAlgorithmUse;   // �˰��� ��� ����
	TCHAR strAlgName[50];	
	double dPara[MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT];	// �˰��� �Ķ����  

	UINT nDefectItemCount;
	STRU_DEFECT_ITEM stDefectItem[MAX_MEM_SIZE_E_DEFECT_NAME_COUNT];	// Defect Item

	STRU_PARAM_ALG()
	{
		memset(this, 0, sizeof(STRU_PARAM_ALG));     
	}

};

struct STRU_INFO_ROI
{
	BOOL bUseROI;
	CRect rectROI;
	TCHAR strROIName[50]; 

	STRU_PARAM_ALG AlgorithmList[MAX_MEM_SIZE_E_ALGORITHM_NUMBER];   // �˰��� �Ķ����	

	STRU_INFO_ROI()
	{
		memset(this, 0, sizeof(STRU_INFO_ROI));     
	}
};


// KSW 17.10.30 [Round] - Alg ����
struct ROUND_SET
{
	// KSW 17.10.24 [Round] - Grab Step��ŭ �ڳ� �κп����� ����
	// Cell ������ �ִ� 30���� �������� Offset �����
	cv::Point2i ptContours[MAX_MEM_SIZE_ROUND_COUNT];		// ������ ������ � ��ǥ
	int			nContourCount;								// ������ ������ � ��ǥ ����
	int			nCornerInside[E_CORNER_END];				// ������ ������ 4���� ������ ��, Cell ���� �ȿ� �����ϴ��� ��/��
	int			nCornerMinLength;							// ������ �������� ���� ����� �ڳ��� [ E_CORNER_END ]
															// E_CORNER_LEFT_TOP �������� �Ϸ� ������, �־������� ���� ����
	ROUND_SET()
	{
		// KSW 17.10.30 [Round] - �ʱ�ȭ
		memset(ptContours, 0, sizeof(cv::Point2i) * MAX_MEM_SIZE_ROUND_COUNT);
		memset(nCornerInside, 0, sizeof(int) * E_CORNER_END);
		nContourCount		= 0;
		nCornerMinLength	= 0;
	}
};

struct INSP_AREA
{
	BOOL bUseROI;
	CRect rectROI; 
	TCHAR strROIName[50];

	INSP_AREA()
	{
		memset(this, 0, sizeof(INSP_AREA));
	}
};

struct STRU_INFO_CAMERA
{
	BOOL bUse;

	int nROICnt;
	STRU_INFO_ROI ROI[MAX_MEM_SIZE_ROI_COUNT];

	int nNonROICnt;
	INSP_AREA NonROI[MAX_MEM_SIZE_E_INSPECT_AREA];
	
	int nRndROICnt;
	INSP_AREA RndROI[MAX_MEM_SIZE_E_INSPECT_AREA];
	

	//2019.02.20 for Hole ROI
	int nHoleROICnt;
	INSP_AREA HoleROI[MAX_MEM_SIZE_E_INSPECT_AREA];

	BOOL bUseAD;
	double dADPara[MAX_MEM_SIZE_AD_PARA_TOTAL_COUNT];

	STRU_INFO_CAMERA()
	{
		memset(this, 0, sizeof(STRU_INFO_CAMERA));     
	}
};

struct STRU_INFO_GRAB_STEP
{
	BOOL	bUse;
	TCHAR	strGrabStepName[50];	
	int		eImgClassify;			// ENUM_IMAGE_CLASSIFY -> int
	int		nCamCnt;
	STRU_INFO_CAMERA stInfoCam[MAX_MEM_SIZE_CAM_COUNT];

	// KSW 17.10.30 [Round] - Alg ����
	ROUND_SET	tRoundSet[MAX_MEM_SIZE_E_INSPECT_AREA];

	ROUND_SET	tCHoleSet[MAX_MEM_SIZE_E_INSPECT_AREA];

	STRU_INFO_GRAB_STEP()
	{
		memset(this, 0, sizeof(STRU_INFO_GRAB_STEP));
	}
};

struct STRU_IMAGE_INFO
{
	CString	strPanelID;
	CString strLotID;
	int		nCameraNo;
	int		nImageNo;
	UINT	nRatio;		// Pixel Shift ���ο� ���� ������ ���� ����
	UINT	nInspType;
	UINT    nShareImgNum;
	int		nStageNo;
	double dPatternCIE[3];	// MTP ���� �� [0]:X, [1]:Y, [2]:L �˻� Ȱ��
	CString strPos;

	STRU_IMAGE_INFO()
	{
		strPanelID	= _T("");
		strLotID	= _T("");
		nCameraNo	= 0;
		nImageNo	= 0;
		nRatio		= 0;		// Pixel Shift ���ο� ���� ������ ���� ����
		nInspType	= 0;
		nStageNo	= 0;
		nShareImgNum= 0;
		for (int i=0; i<3; i++)
			dPatternCIE[i] = 0.0;
		strPos = _T("");
	}
};

// ���� ��� ����ü
struct stDefectInfo
{
	int			nImageNumber;		// ����� ���� ���� ��ȣ [ENUM_IMAGE_CLASSIFY]
	int			nCamNumber;			// ����� ī�޶� ��ȣ

	int			nDefectCount;		// �� �ҷ� ����
	int			nMaxDefect;			// ���� ������ �ҷ� ���� ����

	// �ҷ� �з�
	int*		nDefectJudge;		// �ҷ� ���� ��� [ENUM_DEFECT_JUDGEMENT]	
	int*		nDefectColor;		// ��� ��� �ҷ� ���� [ENUM_DEFECT_COLOR]	
	int*		nPatternClassify;	// ���� ���� (R, G, B...)

	int*		nArea;				// �ҷ� ����
	POINT*		ptLT;				// �ҷ� rect Left-Top
	POINT*		ptRT;				// �ҷ� rect Right-Top
	POINT*		ptRB;				// �ҷ� rect Right-Bottom
	POINT*		ptLB;				// �ҷ� rect Left-Bottom
	double*		dMeanGV;			// �ҷ� ��� ���
	double*		dSigma;				// ǥ�� ����
	int*		nMinGV;				// �ҷ� �ּ� ���
	int*		nMaxGV;				// �ҷ� �ִ� ���
	double*		dBackGroundGV;		// ��� ��� ���
	int*		nCenterx;			// �߽��� x
	int*		nCentery;			// �߽��� y

	double*		dBreadth;			// �ҷ� �β�
	double*		dCompactness;		// �ҷ� ����
	double*		dRoundness;			// ������ ����� ���� 1�̸� ���� ����
	double*		dF_Elongation;		// ��, ���� ����
	double*		dF_Min;				// ����
	double*		dF_Max;				// ����

	double*		dMuraObj;			// KSW 18.09.04 - Mura ��ü ���
	double*		dMuraBk;			// KSW 18.09.04 - Mura ��� ���
	bool*		bMuraActive;		// KSW 18.09.04 - Mura Active ��ġ?
	bool*		bMuraBright;		// KSW 18.09.04 - Mura �ȿ� ���� GV�� �ִ°�?

	double*		dF_MeanAreaRatio;	// choikwangil
	double*		dF_MinBoxRatio;	// choikwangil

	double*		dF_AreaPer;			// choikwangil 04.20
	int*		nJudge_GV;		// choikwangil 04.20
	int*		nIn_Count;			// choikwangil 04.20

	// Color ( SVI )
	double*		Lab_Degree;			// 	
	double*		Lab_Lenght;			// 
	double*		Lab_avg_L;			// 	
	double*		Lab_avg_a;			// 
	double*		Lab_avg_b;			// 
	double*		Lab_diff_L;			// 
	double*		Lab_diff_a;			// 
	double*		Lab_diff_b;			// 
	String*		strColor;

	////////////////////////////////////////////////////////////////////////// 0607 choi ���� ��ġ �������� ���� Data �߰�
	double*		dMV_Per;
	//////////////////////////////////////////////////////////////////////////
	long		nMallocCount;

#if USE_ALG_HIST
	// KSW 17.06.24 ��ü ������׷�
	__int64**	nHist;				//
#endif

#if USE_ALG_CONTOURS
	// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
	int**		nContoursX;
	int**		nContoursY;
#endif

	bool*		bUseResult;			// ���� �ҷ� ��� ��� ��/��

	// ����ü �ʱ�ȭ
	stDefectInfo(int MaxDefect, int ImageNumber = -1, int CamNumber = -1)
	{
		nImageNumber = ImageNumber;	
		nCamNumber = CamNumber;

		nMaxDefect = MaxDefect;
		nDefectCount = 0;		

		nDefectJudge	= (int *) malloc ( sizeof( int ) * MaxDefect );
		memset(nDefectJudge, 0, sizeof(int) * MaxDefect );

		nDefectColor	= (int *) malloc ( sizeof( int ) * MaxDefect );
		memset(nDefectColor, 0, sizeof(int) * MaxDefect );

		nPatternClassify= (int *) malloc ( sizeof( int ) * MaxDefect );
		memset(nPatternClassify, 0, sizeof(int) * MaxDefect );

		nArea			= (int *) malloc ( sizeof( int ) * MaxDefect );
		memset(nArea, 0, sizeof(int) * MaxDefect );

		ptLT			= (POINT *) malloc ( sizeof( POINT ) * MaxDefect );
		ptRT			= (POINT *) malloc ( sizeof( POINT ) * MaxDefect );
		ptRB			= (POINT *) malloc ( sizeof( POINT ) * MaxDefect );
		ptLB			= (POINT *) malloc ( sizeof( POINT ) * MaxDefect );

		dMeanGV			= (double *) malloc ( sizeof( double ) * MaxDefect );
		memset(dMeanGV, 0, sizeof(double) * MaxDefect );

		dSigma			= (double *) malloc ( sizeof( double ) * MaxDefect );
		memset(dSigma, 0, sizeof(double) * MaxDefect );

		nMinGV			= (int *) malloc ( sizeof( int ) * MaxDefect );	
		memset(nMinGV, 0, sizeof(int) * MaxDefect );

		nMaxGV			= (int *) malloc ( sizeof( int ) * MaxDefect );		
		memset(nMaxGV, 0, sizeof(int) * MaxDefect );

		dBackGroundGV	= (double *) malloc ( sizeof( double ) * MaxDefect );	
		memset(dBackGroundGV, 0, sizeof(double) * MaxDefect );

		nCenterx		= (int *) malloc ( sizeof( int ) * MaxDefect );		
		memset(nCenterx, 0, sizeof(int) * MaxDefect );

		nCentery		= (int *) malloc ( sizeof( int ) * MaxDefect );		
		memset(nCentery, 0, sizeof(int) * MaxDefect );
		
		dBreadth		= (double *) malloc ( sizeof( double ) * MaxDefect );	
		memset(dBreadth, 0, sizeof(double) * MaxDefect );

		dCompactness	= (double *) malloc ( sizeof( double ) * MaxDefect );	
		memset(dCompactness, 0, sizeof(double) * MaxDefect );

		dRoundness		= (double *) malloc (sizeof( double ) * MaxDefect );
		memset(dRoundness, 0, sizeof(double) * MaxDefect );

		dF_Elongation	= (double *) malloc ( sizeof( double ) * MaxDefect );	
		memset(dF_Elongation, 0, sizeof(double) * MaxDefect );

		dF_Min			= (double *) malloc ( sizeof( double ) * MaxDefect );
		memset(dF_Min, 0, sizeof(double) * MaxDefect );

		dF_Max			= (double *) malloc ( sizeof( double ) * MaxDefect );
		memset(dF_Max, 0, sizeof(double) * MaxDefect );

		dMuraObj		= (double *)malloc(sizeof(double) * MaxDefect);
		memset(dMuraObj, 0, sizeof(double) * MaxDefect);

		dMuraBk			= (double *)malloc(sizeof(double) * MaxDefect);
		memset(dMuraBk, 0, sizeof(double) * MaxDefect);

		bMuraActive		= (bool *)malloc(sizeof(bool) * MaxDefect);
		memset(bMuraActive, 0, sizeof(bool) * MaxDefect);

		bMuraBright		= (bool *)malloc(sizeof(bool) * MaxDefect);
		memset(bMuraBright, 0, sizeof(bool) * MaxDefect);

		Lab_Degree = (double *)malloc(sizeof(double) * MaxDefect);
		memset(Lab_Degree, 0, sizeof(double) * MaxDefect);

		Lab_Lenght = (double *)malloc(sizeof(double) * MaxDefect);
		memset(Lab_Lenght, 0, sizeof(double) * MaxDefect);

		Lab_avg_L		= (double *) malloc ( sizeof( double ) * MaxDefect );
		memset(Lab_avg_L, 0, sizeof(double) * MaxDefect );

		Lab_avg_a		= (double *) malloc ( sizeof( double ) * MaxDefect );
		memset(Lab_avg_a, 0, sizeof(double) * MaxDefect );

		Lab_avg_b		= (double *) malloc ( sizeof( double ) * MaxDefect );
		memset(Lab_avg_b, 0, sizeof(double) * MaxDefect );

		Lab_diff_L		= (double *) malloc ( sizeof( double ) * MaxDefect );
		memset(Lab_diff_L, 0, sizeof(double) * MaxDefect );

		Lab_diff_a		= (double *) malloc ( sizeof( double ) * MaxDefect );
		memset(Lab_diff_a, 0, sizeof(double) * MaxDefect );

		Lab_diff_b		= (double *) malloc ( sizeof( double ) * MaxDefect );
		memset(Lab_diff_b, 0, sizeof(double) * MaxDefect );

		dF_MeanAreaRatio = (double *)malloc(sizeof(double) * MaxDefect); //choikwangil
		memset(dF_MeanAreaRatio, 0, sizeof(double) * MaxDefect);

		dF_MinBoxRatio = (double *)malloc(sizeof(double) * MaxDefect); //choikwangil
		memset(dF_MinBoxRatio, 0, sizeof(double) * MaxDefect);

		dF_AreaPer = (double *)malloc(sizeof(double) * MaxDefect); //choikwangil 04.20
		memset(dF_AreaPer, 0, sizeof(double) * MaxDefect);

		nJudge_GV = (int *)malloc(sizeof(int) * MaxDefect); //choikwangil 04.20
		memset(nJudge_GV, 0, sizeof(int) * MaxDefect);

		nIn_Count = (int *)malloc(sizeof(int) * MaxDefect); //choikwangil 04.20
		memset(nIn_Count, 0, sizeof(int) * MaxDefect);

		strColor = (String *)malloc(sizeof(String) * MaxDefect);
		memset(strColor, 0, sizeof(String) * MaxDefect);

		dMV_Per = (double *)malloc(sizeof(double) * MaxDefect);
		memset(dMV_Per, 0, sizeof(double) * MaxDefect);


		nMallocCount = MaxDefect;

#if USE_ALG_HIST
		// KSW 17.06.24 ��ü ������׷�
		nHist			= (__int64 **) malloc ( sizeof( __int64 * ) * MaxDefect );
		for(int m=0 ; m<nMallocCount ; m++)
		{
			nHist[m] = (__int64 *) malloc ( sizeof( __int64 ) * IMAGE_MAX_GV );
			memset(nHist[m], 0, sizeof(__int64) * IMAGE_MAX_GV );
		}
#endif

#if USE_ALG_CONTOURS
		// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
		nContoursX		= (int **) malloc ( sizeof( int * ) * MaxDefect );
		nContoursY		= (int **) malloc ( sizeof( int * ) * MaxDefect );
		for(int m=0 ; m<nMallocCount ; m++)
		{
			nContoursX[m] = (int *) malloc ( sizeof( int ) * MAX_CONTOURS );
			nContoursY[m] = (int *) malloc ( sizeof( int ) * MAX_CONTOURS );

			memset(nContoursX[m], 0, sizeof(int) * MAX_CONTOURS );
			memset(nContoursY[m], 0, sizeof(int) * MAX_CONTOURS );
		}
#endif

		bUseResult		= (bool *) malloc ( sizeof( bool ) * MaxDefect );
		for (int nIndex =0; nIndex < MaxDefect; nIndex++)	bUseResult[nIndex] = true;
	}

	// �޸� ����
	~stDefectInfo()
	{
		free(nDefectJudge		);	nDefectJudge		= NULL;
		free(nDefectColor		);	nDefectColor		= NULL;
		free(nPatternClassify	);	nPatternClassify	= NULL;

		free(nArea				);	nArea				= NULL;
		free(ptLT				);	ptLT				= NULL;
		free(ptRT				);	ptRT				= NULL;
		free(ptRB				);	ptRB				= NULL;
		free(ptLB				);	ptLB				= NULL;
		free(dMeanGV			);	dMeanGV				= NULL;
		free(dSigma				);	dSigma				= NULL;
		free(nMinGV				);	nMinGV				= NULL;
		free(nMaxGV				);	nMaxGV				= NULL;
		free(dBackGroundGV		);	dBackGroundGV		= NULL;
		free(nCenterx			);	nCenterx			= NULL;
		free(nCentery			);	nCentery			= NULL;

		free(dBreadth			);	dBreadth			= NULL;
		free(dCompactness		);	dCompactness		= NULL;
		free(dRoundness			);  dRoundness			= NULL;
		free(dF_Elongation		);	dF_Elongation		= NULL;
		free(dF_Min				);	dF_Min				= NULL;
		free(dF_Max				);	dF_Max				= NULL;

		free(dMuraObj);				dMuraObj			= NULL;
		free(dMuraBk);				dMuraBk				= NULL;
		free(bMuraActive);			bMuraActive			= NULL;
		free(bMuraBright);			bMuraBright			= NULL;

		free(Lab_avg_L			);	Lab_avg_L			= NULL;
		free(Lab_avg_a			);	Lab_avg_a			= NULL;
		free(Lab_avg_b			);	Lab_avg_b			= NULL;
		free(Lab_diff_L			);	Lab_diff_L			= NULL;
		free(Lab_diff_a			);	Lab_diff_a			= NULL;
		free(Lab_diff_b			);	Lab_diff_b			= NULL;

		free(dF_MeanAreaRatio	);	dF_MeanAreaRatio	= NULL; //choikwangil

		free(dF_AreaPer			);	dF_AreaPer			= NULL; //choikwangil 04.20
		free(nJudge_GV			);	nJudge_GV			= NULL; //choikwangil 04.20
		free(nIn_Count			);	nIn_Count			= NULL; //choikwangil 04.20
																// �޸� ����
		free(strColor);   strColor = NULL;

		free(dMV_Per);	dMV_Per = NULL;

		free(bUseResult			);	bUseResult			= NULL;

#if USE_ALG_HIST
		// KSW 17.06.24 ��ü ������׷�
		for(int m=0 ; m<nMallocCount ; m++)
		{
			free( nHist[m] );
			nHist[m] = NULL;
		}
		free( nHist );
		nHist = NULL;
#endif

#if USE_ALG_CONTOURS
		// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
		for(int m=0 ; m<nMallocCount ; m++)
		{
			free( nContoursX[m] );
			free( nContoursY[m] );

			nContoursX[m] = NULL;
			nContoursY[m] = NULL;
		}
		free( nContoursX );
		free( nContoursY );
		nContoursX = NULL;
		nContoursY = NULL;
#endif
	}
};

struct stDefectInfo_Substance
{
	int			nImageNumber;		// ����� ���� ���� ��ȣ [ENUM_IMAGE_CLASSIFY]
	int			nCamNumber;			// ����� ī�޶� ��ȣ

	int			nDefectCount;		// �� �ҷ� ����
	int			nMaxDefect;			// ���� ������ �ҷ� ���� ����

									// �ҷ� �з�
	int*		nDefectJudge;		// �ҷ� ���� ��� [ENUM_DEFECT_JUDGEMENT]	
	int*		nDefectColor;		// ��� ��� �ҷ� ���� [ENUM_DEFECT_COLOR]	

 	int*		nDefect_Size;		// 2�� ó�� �ҷ� ����
	double*		dDiff_Sub;			// 2�� ó�� ���� �ҷ��� ��� GV ����
	double*		dDiff_Ratio;		// 2�� ó�� ���� �ҷ��� ��� GV ���� ����

	double*		dAxis_Ratio;		// 2�� ó�� �ҷ� �ڽ��� ���� ������ ����
	double*		dArea_Ratio;		// 2�� ó�� �ҷ� �ڽ� ������ ���� �ҷ� ������ �����ϴ� ����

	int*		nDefect_Box_Width;	// 2�� ó�� �ҷ� �ڽ� ����
	int*		nDefect_Box_Height;	// 2�� ó�� �ҷ� �ڽ� ����

	int*		nBG_Size;			// 2�� ó�� ����� �ȼ� ����
	int*		nCenterBK_Size;		// �ҷ��� �����ִ� 2�� ó�� ����� �ȼ� ����

	double*		dDiff_Center_Sub;		//�ҷ��� �����ִ� 2�� ó�� ���� �ҷ��� ��� GV ����
	double*		dDiff_Center_Ratio;		//�ҷ��� �����ִ� 2�� ó�� ���� �ҷ��� ��� GV ���� ����

	String*		strColor;

	long		nMallocCount;

	bool*		bUseResult;			// ���� �ҷ� ��� ��� ��/��
							// ����ü �ʱ�ȭ
	stDefectInfo_Substance(int MaxDefect, int ImageNumber = -1, int CamNumber = -1)
	{
		nImageNumber = ImageNumber;
		nCamNumber = CamNumber;

		nMaxDefect = MaxDefect;
		nDefectCount = 0;

		nDefectJudge = (int *)malloc(sizeof(int) * MaxDefect);
		memset(nDefectJudge, 0, sizeof(int) * MaxDefect);

		nDefectColor = (int *)malloc(sizeof(int) * MaxDefect);
		memset(nDefectColor, 0, sizeof(int) * MaxDefect);

		nDefect_Size = (int *)malloc(sizeof(int) * MaxDefect);
		memset(nDefect_Size, 0, sizeof(int) * MaxDefect);

		dDiff_Sub = (double *)malloc(sizeof(double) * MaxDefect);
		memset(dDiff_Sub, 0, sizeof(double) * MaxDefect);

		dDiff_Ratio = (double *)malloc(sizeof(double) * MaxDefect);
		memset(dDiff_Ratio, 0, sizeof(double) * MaxDefect);

		dAxis_Ratio = (double *)malloc(sizeof(double) * MaxDefect);
		memset(dAxis_Ratio, 0, sizeof(double) * MaxDefect);

		dArea_Ratio = (double *)malloc(sizeof(double) * MaxDefect);
		memset(dArea_Ratio, 0, sizeof(double) * MaxDefect);

		nDefect_Box_Width = (int *)malloc(sizeof(int) * MaxDefect);
		memset(nDefect_Box_Width, 0, sizeof(int) * MaxDefect);

		nDefect_Box_Height = (int *)malloc(sizeof(int) * MaxDefect);
		memset(nDefect_Box_Height, 0, sizeof(int) * MaxDefect);

		nBG_Size = (int *)malloc(sizeof(int) * MaxDefect);
		memset(nBG_Size, 0, sizeof(int) * MaxDefect);

		nCenterBK_Size = (int *)malloc(sizeof(int) * MaxDefect);
		memset(nCenterBK_Size, 0, sizeof(int) * MaxDefect);

		dDiff_Center_Sub = (double *)malloc(sizeof(double) * MaxDefect);
		memset(dDiff_Center_Sub, 0, sizeof(double) * MaxDefect);

		dDiff_Center_Ratio = (double *)malloc(sizeof(double) * MaxDefect);
		memset(dDiff_Center_Ratio, 0, sizeof(double) * MaxDefect);

		strColor = (String *)malloc(sizeof(String) * MaxDefect);
		memset(strColor, 0, sizeof(String) * MaxDefect);

		nMallocCount = MaxDefect;

		bUseResult = (bool *)malloc(sizeof(bool) * MaxDefect);
		for (int nIndex = 0; nIndex < MaxDefect; nIndex++)	bUseResult[nIndex] = true;


	}
	// �޸� ����
	~stDefectInfo_Substance()
	{
		free(nDefectJudge);	nDefectJudge = NULL;
		free(nDefectColor);	nDefectColor = NULL;
 
		free(nDefect_Size);	nDefect_Size = NULL;
		free(dDiff_Sub);	dDiff_Sub = NULL;
		free(dDiff_Ratio);	dDiff_Ratio = NULL;
		free(dAxis_Ratio);	dAxis_Ratio = NULL;
		free(dArea_Ratio);	dArea_Ratio = NULL;
		free(nDefect_Box_Width);	nDefect_Box_Width = NULL;
		free(nDefect_Box_Height);	nDefect_Box_Height = NULL;
		free(nBG_Size);	nBG_Size = NULL;
		free(nCenterBK_Size);	nCenterBK_Size = NULL;
		free(dDiff_Center_Sub);	dDiff_Center_Sub = NULL;
		free(dDiff_Center_Ratio);	dDiff_Center_Ratio = NULL;
		
											  // �޸� ����
		free(strColor);   strColor = NULL;

		free(bUseResult);	bUseResult = NULL;


	}
};

// �г� ���� ���ذ� / ��ȣ
struct stPanelJudgeInfo
{
	int nRefVal;
	int nSign;			// ������ ( <, >, ==, <=, >= )

	stPanelJudgeInfo()
	{
		nRefVal	= 0;
		nSign	= 0;
	}
};

/// ���� �켱����. �ε����� ���� ���� �ֿ켱 ����
struct stPanelJudgePriority
{
	TCHAR strGrade[50];
	stPanelJudgeInfo stJudgeInfo[E_PANEL_DEFECT_TREND_COUNT];
	stPanelJudgeInfo stFilterInfo[E_PANEL_DEFECT_TREND_COUNT];
	stPanelJudgePriority()
	{
		memset(this, 0, sizeof(stPanelJudgePriority));
	}
};

/// ���� ���� �� ���͸� ����
struct stReportFilter
{
	BOOL bUse;	// ��� ��� ����
	stPanelJudgeInfo stJudgeInfo[E_PANEL_DEFECT_TREND_COUNT];

	stReportFilter()
	{
		bUse = false;
		memset(stJudgeInfo, 0, sizeof(stPanelJudgeInfo) * E_PANEL_DEFECT_TREND_COUNT);
	}
};


/// ���� ���� �� ���͸� ����
struct stUserDefinedFilter
{
	BOOL bUse;	// ��� ��� ����
	TCHAR strGrade[10];					// ���͸� ���� �� Grade
	int nFilterItemsCount;				// Defect Num ����				ex) 1+4 �� 2�� 
	int nFilterItems[100];				// ���͸� ����� DEFECT_NUM,	ex) 5+6

	stPanelJudgeInfo stFilterInfo;

	stUserDefinedFilter()
	{
		bUse = false;
		nFilterItemsCount = 0;
		memset(nFilterItems, 0, sizeof(int) * 100);
		memset(this, 0, sizeof(stUserDefinedFilter));
	}
};

struct stDefClassification
{
	TCHAR strDefectName[50];
	TCHAR strDefectCode[10];

	stDefClassification()
	{
		memset(this, 0, sizeof(stDefClassification));
	}
};


//2018.04.13 add by ikm for pad area
struct STRU_PAD_AREA
{
	int			 nPolygonCnt;
	IplImage*	 ipImg;	
	cv::Rect	 cvRect;
	cv::Point*   cvPoint;

	STRU_PAD_AREA()
	{		
		nPolygonCnt = 0;		
		cvPoint = new Point[1];
	}
	void Point_malloc(int nPntCnt = 1000)
	{
		nPolygonCnt = nPntCnt;
		cvPoint = new Point[nPntCnt];
	}	
	IplImage* GetImage()
	{
		return ipImg;		
	}
 	Rect GetRectCoord()
 	{
 		return cvRect;		
 	}
	Point* GetPolygonCoord()
	{
		return cvPoint;		
	}
	int GetPolygonCount()
	{
		return nPolygonCnt;
		return (int)_msize(cvPoint) / sizeof(*cvPoint);				
	}
	~STRU_PAD_AREA()
	{		
		delete[] cvPoint; cvPoint = NULL;
	}
};

struct STRU_INFO_PAD
{
	int nRectCnt;
	STRU_PAD_AREA* tPadInfo;
	
	STRU_INFO_PAD()
	{
		tPadInfo = new STRU_PAD_AREA[1];
	}
	void _malloc(int nMaxCnt = 100)
	{	
		nRectCnt = nMaxCnt;
		tPadInfo = new STRU_PAD_AREA[nMaxCnt];	
	}
	int GetRoiCount()
	{
		return nRectCnt;
		return (int)_msize(this->tPadInfo) / sizeof(*this->tPadInfo);				
	} 	
 	Rect GetRectCoord( int nRoiCnt)
 	{
 		return tPadInfo[nRoiCnt].GetRectCoord();			
 	}
	Point* GetPolygonCoord(int nRoiCnt)
	{
		return this->tPadInfo[nRoiCnt].GetPolygonCoord();	
	}
	int GetPolygonCount(int nRoiCnt)
	{
		return this->tPadInfo[nRoiCnt].GetPolygonCount();	
	}
	IplImage* GetImage(int nRoiCnt)
	{
		return tPadInfo[nRoiCnt].GetImage();		
	}
	void ReleaseImage()
	{
		for (int nRoiCnt = 0 ; nRoiCnt < GetRoiCount(); nRoiCnt++)
			cvReleaseImage(&tPadInfo[nRoiCnt].ipImg);	
	}
	~STRU_INFO_PAD()
	{
		ReleaseImage();	
		delete[] tPadInfo; tPadInfo = NULL; 
	}
};
//2018.04.13 add by ikm for pad area
#pragma pack(pop)

#endif