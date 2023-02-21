////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declare Algorithm DLL Interface Function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DEFINE_INTERFACE_H
#define DEFINE_INTERFACE_H

/***************************************************************************************************
 Algorithm DLL 과 Algorithm Task 간 공유가 필요한 ENUM, 구조체 등은 여기에 정의 - AVI / SVI / APP 공통 사항
***************************************************************************************************/

#pragma once

#include "Define.h"
#include "DefineInterface_SITE.h"

//////////////////////////////////////////////////////////////////////////
// ENUM
//////////////////////////////////////////////////////////////////////////
enum ENUM_DEFECT_COLOR
{
	E_DEFECT_COLOR_DARK			=0	,	// 00 어두운 불량
	E_DEFECT_COLOR_BRIGHT			,	// 01 밝은 불량

	E_DEFECT_COLOR_COUNT				// 02 Total
};

// 부등호 연산자 ( <, >, ==, <=, >= )
enum ENUM_SIGN_OF_INEQUALITY
{
	E_SIGN_EQUAL						= 0	,	// x == judgment value
	E_SIGN_NOT_EQUAL						,	// x != judgment value
	E_SIGN_GREATER							,	// x >  judgment value
	E_SIGN_LESS								,	// x <  judgment value
	E_SIGN_GREATER_OR_EQUAL					,	// x >= judgment value
	E_SIGN_LESS_OR_EQUAL						// x <= judgment value
};

// 2017.06.08 에러 코드 AVI/SVI/APP 공용으로 변경
// 에러 코드 리턴 값
enum ENUM_ERROR_CODE
{
	E_ERROR_CODE_FALSE					= -1	,	// 비정상 - 실제 구현 함수 없음

	// 공통 : 000번대
	E_ERROR_CODE_TRUE					= 000	,	// 정상

	E_ERROR_CODE_EMPTY_BUFFER			= 001	,	// 영상 버퍼 없는 경우
	E_ERROR_CODE_EMPTY_PATH						,	// 저장 경로가 없는 경우.
	E_ERROR_CODE_EMPTY_PARA						,	// 검사 파라미터가 없는 경우.
	E_ERROR_CODE_EMPTY_SET_ROI					,	// ROI 설정이 없는 경우.
	E_ERROR_CODE_IMAGE_CLASSIFY_OVER			,	// 영상 번호가 0~9가 아닌 경우.
	E_ERROR_CODE_ROI_OVER						,	// 검사 영역이 영상 크기를 벗어난 경우.
	E_ERROR_CODE_INCOMPATIBLE_BUFFER_TYPE		,	// Mat 버퍼 타입이 다른 경우.
	E_ERROR_CODE_TIME_OUT						,	// 설정 시간 오버된 경우
	E_ERROR_CODE_IMAGE_GRAY						,	// Gray 영상이 아닌 경우
	E_ERROR_CODE_IMAGE_COLOR					,	// Color 영상이 아닌 경우
	E_ERROR_CODE_IMAGE_SIZE						,	// 영상 크기가 다른 경우
	E_ERROR_CODE_EMPTY_RGB						,	// R,G,B 영상 AD or 없는 경우
	E_ERROR_CODE_NON_USE						,	// LCP 검사에 필수 검사 체크 안되있는 경우 검사 패스

	// Align : 100번대
	E_ERROR_CODE_ALIGN_NO_DATA			= 101	,	// 데이터 부족 & 데이터 없는 경우.
	E_ERROR_CODE_ALIGN_WARNING_PARA				,	// 잘못된 파라미터를 설정한 경우.
	E_ERROR_CODE_ALIGN_NO_FIND_CELL				,	// Cell을 찾을 수 없을 경우.
	E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR		,	// 각도가 많이 틀어진 경우. (ERROR - 검사 불가)
	E_ERROR_CODE_ALIGN_ANGLE_RANGE_WARNING		,	// 각도가 많이 틀어진 경우. (WARNING - 검사 지장)
	E_ERROR_CODE_ALIGN_CAN_NOT_CALC				,	// 각도 계산 불가인 경우.
	E_ERROR_CODE_ALIGN_DISPLAY					,	// 디스플레이 이상.
	E_ERROR_CODE_ALIGN_IMAGE_OVER				,	// 영상 좌표 벗어남.
	E_ERROR_CODE_ALIGN_WRONG_SLOPE				,	// 기울기 잘못된 값 추출 (SVI)
	E_ERROR_CODE_ALIGN_ROUND_SETTING			,	// Round 셋팅 플래그 On인 경우 검사 안함
	E_ERROR_CODE_ALIGN_LENGTH_RANGE_ERROR		,	// Align 후 Pannel Size Range 벗어난 경우 검사 안함.

	// Point : 200번대
	E_ERROR_CODE_POINT_DEFECT_NO		= 201	,	// 포인트 불량이 아닌 경우.
	E_ERROR_CODE_POINT_JUDEGEMENT_E				,	// Judgement E급 판정 리턴용
	E_ERROR_CODE_POINT_RES_VECTOR_NULL			,	// 포인트 불량 결과 벡터가 NULL인 경우.
	E_ERROR_CODE_POINT_NOT_USE_DUST_ALG			,	// Dust 알고리즘 사용 안한 경우.
	E_ERROR_CODE_POINT_WARNING_PARA				,	// 잘못된 파라미터를 설정한 경우.
	E_ERROR_CODE_POINT_TIME_OVER				,	// 연산 시간 및 대기시간이 오래 걸리는 경우. ( 현재 10초 설정 )

	// Line : 300번 대
	E_ERROR_CODE_LINE_HISTO				= 301	,	// 

	// Mura : 400번 대
	E_ERROR_CODE_MURA_WRONG_PARA		= 401	,	// Mura 파라미터 이상
	E_ERROR_CODE_MURA_HISTO						,	//
	E_ERROR_CODE_MURA_RGB_LINE_DEFECT			,	// RGB Line Mura 불량 검출

	// SVI : 500번 대
	E_ERROR_CODE_SVI_EMD_ESTMATION		= 501	,	// Cell 점등 영역 찾지 못함
	E_ERROR_CODE_SVI_WRONG_PARAM				,

	// Empty
	E_ERROR_CODE_EMPTY					= 600	,	// 

	// APP : 700 번대
	E_ERROR_CODE_CUTTING_MARK_TYPE_PARA	= 701	,
	E_ERROR_CODE_PAD_TEACHING_ABNORMAL			,	// PAD Teaching이 비정상적임.
	E_ERROR_CODE_PAD_MASK_REF_EMPTY				,	// PAD MASK 기준 정보 혹은 영상이 없음.
	E_ERROR_CODE_PAD_REF_IMG_EMPTY_BUFFER		,	// PAD Reference 영상 없음
	E_ERROR_CODE_PAD_MASK_SIZE					,	// PAD MASK Size 이상
	E_ERROR_CODE_MEASURE_BM_FITTING_FAIL		,	// 측정 진행 시 BM Line Fitting 실패
	E_ERROR_CODE_MEASURE_PANEL_FITTING_FAIL		,	// 측정 진행 시 Panel Line Fitting 실패
	E_ERROR_CODE_EMPTY_CROSS_MARK_BUFFER		,	// 십자 Mark 영상 없음
	E_ERROR_CODE_CROSS_MARK_SEARCH_FAIL			,	// 십자 Mark 찾기 실패
	E_ERROR_CODE_APP_HISTO						,
	E_ERROR_CODE_CCUT_REF_IMG_BUFFER			,	// CCut Reference 영상 없음
	E_ERROR_CODE_CCUT_ROI_MATCHING_FAIL			,	// CCut ROI Matching 실패 (Panel 손상 혹은 영상 손상)
	E_ERROR_CODE_PAD_PARAM_IS_NULL				,	// Pad Area의 파라미터가 존재하지 않음
	E_ERROR_CODE_PAD_INSP_ROI_OVER				,	// PAD INSP ROI Size 문제
	E_ERROR_CODE_ACTIVE_MASK					,	// Active Mask에 문제 있음
	E_ERROR_CODE_ACTIVE_FEATURE_SIZE			,	// Active Feature Size 문제

	// CCD Defect : 800 번대
	E_ERROR_CODE_CCD_EMPTY_BUFFER		= 801	,	// 영상 버퍼 없는 경우
	E_ERROR_CODE_CCD_PS_BUFFER					,	// PS 영상 버퍼인 경우
	E_ERROR_CODE_CCD_WARNING_PARA				,	// 잘못된 파라미터를 설정한 경우.
	E_ERROR_CODE_CCD_NOT_LOAD					,	// Load 안된 경우
	E_ERROR_CODE_CCD_EMPTY_FILE					,	// 파일이 없는 경우

	E_ERROR_CODE_LCP_STAGE_DIRTY        = 1001  ,   //LCP Stage 먼지 많은 경우
	E_ERROR_CODE_LCP_ALIGN_IMAGE_NON_BUFFER		,
	E_ERROR_CODE_LCP_ALIGN_BACK_MASK_FAIL,
	E_ERROR_CODE_LCP_ALIGN_ACTIVE_MASK_FAIL,

};

enum ENUM_PAD_AREA_NAME
{
	E_PAD_REF					=0	, // 패드 부 레퍼런스 이미지 & 좌표
	E_PAD_FIDUCIAL					, // Fiducial Mark & 고정 포인트 이미지 및 좌표	
	E_PAD_ALIGN_MARK				, // 패드부 십자 마크 이미지 및 좌표
	E_PAD_INSP_AREA					, // 패드 검사 영역 이미지 및 좌표
	E_PAD_NONE_AREA					, // 패드 비검사 영역 이미지 및 좌표
	E_PAD_AREA_COUNT				
};

//////////////////////////////////////////////////////////////////////////
// 구조체
//////////////////////////////////////////////////////////////////////////

//측정 Data 사용할 구조체 2017.09.20 LJM
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
	BOOL bUse;			// 판정 항목 선택 유/무
	int nSign;			// 연산자 ( <, >, ==, <=, >= )
	double dValue;      // 값

	// 구조체 초기화
	struct STRU_JUDGEMENT()  
	{
		memset(this, 0, sizeof(STRU_JUDGEMENT));   
	}
};

struct STRU_DEFECT_ITEM
{
	BOOL bDefectItemUse;   // 알고리즘 사용 여부
	TCHAR strItemName[50];

	STRU_JUDGEMENT Judgment[2 * MAX_MEM_SIZE_E_DEFECT_JUDGMENT_COUNT]; // Defect 판정 항목 당 범위 2개

	STRU_DEFECT_ITEM()
	{
		memset(this, 0, sizeof(STRU_DEFECT_ITEM));     
	}
};

struct STRU_PARAM_ALG
{
	BOOL bAlgorithmUse;   // 알고리즘 사용 여부
	TCHAR strAlgName[50];	
	double dPara[MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT];	// 알고리즘 파라메터  

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

	STRU_PARAM_ALG AlgorithmList[MAX_MEM_SIZE_E_ALGORITHM_NUMBER];   // 알고리즘 파라메터	

	STRU_INFO_ROI()
	{
		memset(this, 0, sizeof(STRU_INFO_ROI));     
	}
};


// KSW 17.10.30 [Round] - Alg 설정
struct ROUND_SET
{
	// KSW 17.10.24 [Round] - Grab Step만큼 코너 부분영역이 존재
	// Cell 꼭지점 최대 30개를 기준으로 Offset 사용함
	cv::Point2i ptContours[MAX_MEM_SIZE_ROUND_COUNT];		// 설정한 영역의 곡선 좌표
	int			nContourCount;								// 설정한 영역의 곡선 좌표 개수
	int			nCornerInside[E_CORNER_END];				// 설정한 영역의 4개의 꼭지점 중, Cell 영역 안에 존재하는지 유/무
	int			nCornerMinLength;							// 설정한 영역에서 제일 가까운 코너점 [ E_CORNER_END ]
															// E_CORNER_LEFT_TOP 기준으로 하려 했으나, 멀어질수록 오차 존재
	ROUND_SET()
	{
		// KSW 17.10.30 [Round] - 초기화
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

	// KSW 17.10.30 [Round] - Alg 설정
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
	UINT	nRatio;		// Pixel Shift 여부에 따른 원본에 대한 비율
	UINT	nInspType;
	UINT    nShareImgNum;
	int		nStageNo;
	double dPatternCIE[3];	// MTP 보정 후 [0]:X, [1]:Y, [2]:L 검사 활용
	CString strPos;

	STRU_IMAGE_INFO()
	{
		strPanelID	= _T("");
		strLotID	= _T("");
		nCameraNo	= 0;
		nImageNo	= 0;
		nRatio		= 0;		// Pixel Shift 여부에 따른 원본에 대한 비율
		nInspType	= 0;
		nStageNo	= 0;
		nShareImgNum= 0;
		for (int i=0; i<3; i++)
			dPatternCIE[i] = 0.0;
		strPos = _T("");
	}
};

// 공통 결과 구조체
struct stDefectInfo
{
	int			nImageNumber;		// 검출된 패턴 영상 번호 [ENUM_IMAGE_CLASSIFY]
	int			nCamNumber;			// 검출된 카메라 번호

	int			nDefectCount;		// 총 불량 갯수
	int			nMaxDefect;			// 저장 가능한 불량 정보 개수

	// 불량 분류
	int*		nDefectJudge;		// 불량 판정 결과 [ENUM_DEFECT_JUDGEMENT]	
	int*		nDefectColor;		// 배경 대비 불량 색상 [ENUM_DEFECT_COLOR]	
	int*		nPatternClassify;	// 패턴 종류 (R, G, B...)

	int*		nArea;				// 불량 면적
	POINT*		ptLT;				// 불량 rect Left-Top
	POINT*		ptRT;				// 불량 rect Right-Top
	POINT*		ptRB;				// 불량 rect Right-Bottom
	POINT*		ptLB;				// 불량 rect Left-Bottom
	double*		dMeanGV;			// 불량 평균 밝기
	double*		dSigma;				// 표준 편차
	int*		nMinGV;				// 불량 최소 밝기
	int*		nMaxGV;				// 불량 최대 밝기
	double*		dBackGroundGV;		// 배경 평균 밝기
	int*		nCenterx;			// 중심정 x
	int*		nCentery;			// 중심정 y

	double*		dBreadth;			// 불량 두께
	double*		dCompactness;		// 불량 원형
	double*		dRoundness;			// 원형에 가까운 정도 1이면 완전 원형
	double*		dF_Elongation;		// 장, 단축 비율
	double*		dF_Min;				// 단축
	double*		dF_Max;				// 장축

	double*		dMuraObj;			// KSW 18.09.04 - Mura 객체 밝기
	double*		dMuraBk;			// KSW 18.09.04 - Mura 배경 밝기
	bool*		bMuraActive;		// KSW 18.09.04 - Mura Active 위치?
	bool*		bMuraBright;		// KSW 18.09.04 - Mura 안에 밝은 GV가 있는가?

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

	////////////////////////////////////////////////////////////////////////// 0607 choi 판정 위치 변경으로 인한 Data 추가
	double*		dMV_Per;
	//////////////////////////////////////////////////////////////////////////
	long		nMallocCount;

#if USE_ALG_HIST
	// KSW 17.06.24 객체 히스토그램
	__int64**	nHist;				//
#endif

#if USE_ALG_CONTOURS
	// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
	int**		nContoursX;
	int**		nContoursY;
#endif

	bool*		bUseResult;			// 최종 불량 결과 사용 유/무

	// 구조체 초기화
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
		// KSW 17.06.24 객체 히스토그램
		nHist			= (__int64 **) malloc ( sizeof( __int64 * ) * MaxDefect );
		for(int m=0 ; m<nMallocCount ; m++)
		{
			nHist[m] = (__int64 *) malloc ( sizeof( __int64 ) * IMAGE_MAX_GV );
			memset(nHist[m], 0, sizeof(__int64) * IMAGE_MAX_GV );
		}
#endif

#if USE_ALG_CONTOURS
		// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
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

	// 메모리 해제
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
																// 메모리 해제
		free(strColor);   strColor = NULL;

		free(dMV_Per);	dMV_Per = NULL;

		free(bUseResult			);	bUseResult			= NULL;

#if USE_ALG_HIST
		// KSW 17.06.24 객체 히스토그램
		for(int m=0 ; m<nMallocCount ; m++)
		{
			free( nHist[m] );
			nHist[m] = NULL;
		}
		free( nHist );
		nHist = NULL;
#endif

#if USE_ALG_CONTOURS
		// KSW 17.11.29 - 외곽 정보 ( AVI & SVI 머지 툴 )
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
	int			nImageNumber;		// 검출된 패턴 영상 번호 [ENUM_IMAGE_CLASSIFY]
	int			nCamNumber;			// 검출된 카메라 번호

	int			nDefectCount;		// 총 불량 갯수
	int			nMaxDefect;			// 저장 가능한 불량 정보 개수

									// 불량 분류
	int*		nDefectJudge;		// 불량 판정 결과 [ENUM_DEFECT_JUDGEMENT]	
	int*		nDefectColor;		// 배경 대비 불량 색상 [ENUM_DEFECT_COLOR]	

 	int*		nDefect_Size;		// 2차 처리 불량 면적
	double*		dDiff_Sub;			// 2차 처리 배경과 불량의 평균 GV 차이
	double*		dDiff_Ratio;		// 2차 처리 배경과 불량의 평균 GV 차이 비율

	double*		dAxis_Ratio;		// 2차 처리 불량 박스의 장축 단축의 비율
	double*		dArea_Ratio;		// 2차 처리 불량 박스 면적에 실제 불량 면적이 차지하는 비율

	int*		nDefect_Box_Width;	// 2차 처리 불량 박스 가로
	int*		nDefect_Box_Height;	// 2차 처리 불량 박스 세로

	int*		nBG_Size;			// 2차 처리 배경의 픽셀 개수
	int*		nCenterBK_Size;		// 불량과 겹쳐있는 2차 처리 배경의 픽셀 개수

	double*		dDiff_Center_Sub;		//불량과 겹쳐있는 2차 처리 배경과 불량의 평균 GV 차이
	double*		dDiff_Center_Ratio;		//불량과 겹쳐있는 2차 처리 배경과 불량의 평균 GV 차이 비율

	String*		strColor;

	long		nMallocCount;

	bool*		bUseResult;			// 최종 불량 결과 사용 유/무
							// 구조체 초기화
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
	// 메모리 해제
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
		
											  // 메모리 해제
		free(strColor);   strColor = NULL;

		free(bUseResult);	bUseResult = NULL;


	}
};

// 패널 판정 기준값 / 부호
struct stPanelJudgeInfo
{
	int nRefVal;
	int nSign;			// 연산자 ( <, >, ==, <=, >= )

	stPanelJudgeInfo()
	{
		nRefVal	= 0;
		nSign	= 0;
	}
};

/// 판정 우선순위. 인덱스가 낮을 수록 최우선 순위
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

/// 상위 보고 시 필터링 기준
struct stReportFilter
{
	BOOL bUse;	// 기능 사용 여부
	stPanelJudgeInfo stJudgeInfo[E_PANEL_DEFECT_TREND_COUNT];

	stReportFilter()
	{
		bUse = false;
		memset(stJudgeInfo, 0, sizeof(stPanelJudgeInfo) * E_PANEL_DEFECT_TREND_COUNT);
	}
};


/// 상위 보고 시 필터링 기준
struct stUserDefinedFilter
{
	BOOL bUse;	// 기능 사용 여부
	TCHAR strGrade[10];					// 필터를 적용 할 Grade
	int nFilterItemsCount;				// Defect Num 갯수				ex) 1+4 면 2개 
	int nFilterItems[100];				// 필터를 사용할 DEFECT_NUM,	ex) 5+6

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