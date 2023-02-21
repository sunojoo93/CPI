#ifndef APP_DEFINE_INSPECT_H
#define APP_DEFINE_INSPECT_H

/**********************************************************************************
 Algorithm DLL 과 Algorithm Task 간 공유가 필요한 ENUM, 구조체 등은 여기에 정의 - AVI 전용
**********************************************************************************/
// TEST 후 상세 내용 수정

#pragma once

#include "DefineInterface.h"
#include "DefineInterface_SITE.h"

//////////////////////////////////////////////////////////////////////////
// DEFINE
//////////////////////////////////////////////////////////////////////////

#define PI acos(-1.)


//////////////////////////////////////////////////////////////////////////
// ENUM
//////////////////////////////////////////////////////////////////////////

enum ENUM_ALGORITHM_APP_NUM
{
	E_ALGORITHM_APP_CUTTING	= 0		,	
	E_ALGORITHM_APP_CUT_C			,	
	E_ALGORITHM_APP_PANEL_MEASURE	,	
	E_ALGORITHM_APP_PAD_SCRATCH		,	
	E_ALGORITHM_APP_ACTIVE			,	
	E_ALGORITHM_APP_ACTIVE_BURNT	,	
	E_ALGORITHM_APP_ACTIVE_DUST		,	
	E_ALGORITHM_APP_PAD_DUST		,	// 07 RESERVE07
	E_ALGORITHM_APP_RESERVE08		,	// 08 RESERVE08
	E_ALGORITHM_APP_RESERVE09		,	// 09 RESERVE09
	E_ALGORITHM_APP_RESERVE10		,	// 10 RESERVE10
	E_ALGORITHM_APP_RESERVE11		,	// 11 RESERVE11
	E_ALGORITHM_APP_RESERVE12		,	// 12 RESERVE12
	E_ALGORITHM_APP_RESERVE13		,	// 13 RESERVE13
	E_ALGORITHM_APP_RESERVE14		,	// 14 RESERVE14
	E_ALGORITHM_APP_NUMBER				// 15 Total
};

enum ENUM_APP_MEASURE_PARAM_NUM
{
	E_MEASURE_PARA_BLUR_SIZE =0			,
	E_MEASURE_PARA_THRESHOLD			,
	E_MEASURE_PARA_MINSAMPLE			,
	E_MEASURE_PARA_DIST_TH				,
	E_MEASURE_PARA_HORIZONTAL_STANDARD	,
	E_MEASURE_PARA_VERTICAL_STANDARD,
};

#endif