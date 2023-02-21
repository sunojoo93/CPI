

/************************************************************************/
// SVI 컬러 보정 관련 헤더
// 수정일 : 18.02.20 - KSW
/************************************************************************/


#pragma once

#include "Define.h"

// Color 보정 수치 개수
#define COLOR_CORRECTION_COUNT	9



class CColorCorrection
{
public:
	CColorCorrection(void);
	virtual ~CColorCorrection(void);

public:
	// Color 보정 수치 Load
	long	ColorCorrectionLoad(CString strFileName);

	// Color 보정 수치 가져오기
	double*	GetColorCorrection();

protected:
	// Load 됐는지 확인 용
	bool	bLoad;

	// Color 보정 수치
	double	m_dCoefficient[COLOR_CORRECTION_COUNT];

};