
/************************************************************************/
// Time Out ���� �ҽ�
// ������ : 18.02.26 - KSW
/************************************************************************/


#include "StdAfx.h"
#include "TimeOut.h"



CTimeOut::CTimeOut(void)
{
	// ���� �ð� ( �⺻ 40�� )
	m_nSetTime	= 40000;
}


CTimeOut::~CTimeOut(void)
{
}


long CTimeOut::TimeCheckStart()
{
	// ������ �ð�
	m_nEndTime = clock() + m_nSetTime;

	return 0;
}


// Ÿ�� �ƿ� Ȯ�� �÷���
// ����� ��, ������ False ��ȯ ( ���� ����� �� ���, Ȯ�� �ؾ� �ϹǷ�... )
BOOL CTimeOut::GetTimeOutFlag()
{
	int nTime = clock();

#ifdef _DEBUG	//Debug
	return	FALSE;
#else			//Release
	return ( nTime >= m_nEndTime ) ? TRUE : FALSE;
#endif	
}

// Ÿ�� �ƿ� ����
void CTimeOut::SetTimeOut(int nTime_ms)
{	
	m_nSetTime = nTime_ms;
}