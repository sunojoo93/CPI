
/************************************************************************/
// SVI 컬러 보정 관련 소스
// 수정일 : 18.02.20 - KSW
/************************************************************************/


#include "StdAfx.h"
#include "ColorCorrection.h"


CColorCorrection::CColorCorrection(void)
{
	bLoad = false;
	memset(m_dCoefficient, 0, sizeof(double) * COLOR_CORRECTION_COUNT);
}


CColorCorrection::~CColorCorrection(void)
{
	bLoad = false;
	memset(m_dCoefficient, 0, sizeof(double) * COLOR_CORRECTION_COUNT);
}


// Color 보정 수치 Load
long CColorCorrection::ColorCorrectionLoad(CString strFileName)
{
	// 이미 로드 된 경우
	if( bLoad )	return E_ERROR_CODE_TRUE;

	// 파일 존재 유/무 확인 필요
	CFileFind find;
	BOOL bFindFile = FALSE;

	bFindFile = find.FindFile(strFileName);
	find.Close();

	// 파일 없는 경우
	if (!bFindFile)
	{
		return E_ERROR_CODE_CCD_EMPTY_FILE;
	}

	//////////////////////////////////////////////////////////////////////////

	char szFileName[256] = {0, };
	WideCharToMultiByte(CP_ACP, 0, strFileName, -1, szFileName, sizeof(szFileName), NULL, NULL);

	FILE *out = NULL;
	fopen_s(&out, szFileName, "r");

	if( !out )	return E_ERROR_CODE_TRUE;

	double dTemp;

	for (int m=0 ; m<COLOR_CORRECTION_COUNT  ; m++)
	{
		// 읽어오기
		fscanf_s(out, "%lf\n", &dTemp);

		m_dCoefficient[m] = dTemp;
	}

	fclose(out);
	out = NULL;

	bLoad = true;

	return E_ERROR_CODE_TRUE;
}


// Color 보정 수치 가져오기
double* CColorCorrection::GetColorCorrection()
{
	// Load 안된 경우
	if( !bLoad )	return NULL;

	return m_dCoefficient;
}