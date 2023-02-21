

/************************************************************************/
// SVI �÷� ���� ���� ���
// ������ : 18.02.20 - KSW
/************************************************************************/


#pragma once

#include "Define.h"

// Color ���� ��ġ ����
#define COLOR_CORRECTION_COUNT	9



class CColorCorrection
{
public:
	CColorCorrection(void);
	virtual ~CColorCorrection(void);

public:
	// Color ���� ��ġ Load
	long	ColorCorrectionLoad(CString strFileName);

	// Color ���� ��ġ ��������
	double*	GetColorCorrection();

protected:
	// Load �ƴ��� Ȯ�� ��
	bool	bLoad;

	// Color ���� ��ġ
	double	m_dCoefficient[COLOR_CORRECTION_COUNT];

};