#ifndef AVI_DEFINE_INSPECT_H
#define AVI_DEFINE_INSPECT_H

/**********************************************************************************
 Algorithm DLL 과 Algorithm Task 간 공유가 필요한 ENUM, 구조체 등은 여기에 정의 - AVI 전용
**********************************************************************************/

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

// [CInspectAlign] Align 방향
// enum ENUM_ALIGN_TYPE
// {
// 	E_ALIGN_TYPE_LEFT = 0	,
// 	E_ALIGN_TYPE_TOP		,
// 	E_ALIGN_TYPE_RIGHT		,
// 	E_ALIGN_TYPE_BOTTOM
// };

// [AVIResult 구조체] 불량 색상
// enum ENUM_DEFECT_COLOR
// {
// 	E_DEFECT_COLOR_DARK			=0	,	// 00 어두운 불량
// 	E_DEFECT_COLOR_BRIGHT			,	// 01 밝은 불량
// 
// 	E_DEFECT_COLOR_COUNT				// 02 Total
// };

#endif