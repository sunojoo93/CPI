
/************************************************************************/
// SVI �÷� ���� ���� �ҽ�
// ������ : 18.02.20 - KSW
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


// Color ���� ��ġ Load
long CColorCorrection::ColorCorrectionLoad(CString strFileName)
{
	// �̹� �ε� �� ���
	if( bLoad )	return E_ERROR_CODE_TRUE;

	// ���� ���� ��/�� Ȯ�� �ʿ�
	CFileFind find;
	BOOL bFindFile = FALSE;

	bFindFile = find.FindFile(strFileName);
	find.Close();

	// ���� ���� ���
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
		// �о����
		fscanf_s(out, "%lf\n", &dTemp);

		m_dCoefficient[m] = dTemp;
	}

	fclose(out);
	out = NULL;

	bLoad = true;

	return E_ERROR_CODE_TRUE;
}


// Color ���� ��ġ ��������
double* CColorCorrection::GetColorCorrection()
{
	// Load �ȵ� ���
	if( !bLoad )	return NULL;

	return m_dCoefficient;
}