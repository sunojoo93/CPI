
/************************************************************************/
// Time Out 관련 소스
// 수정일 : 18.02.26 - KSW
/************************************************************************/


#include "StdAfx.h"
#include "TimeOut.h"



CTimeOut::CTimeOut(void)
{
	// 설정 시간 ( 기본 40초 )
	m_nSetTime	= 40000;
}


CTimeOut::~CTimeOut(void)
{
}


long CTimeOut::TimeCheckStart()
{
	// 설정한 시간
	m_nEndTime = clock() + m_nSetTime;

	return 0;
}


// 타임 아웃 확인 플래그
// 디버그 시, 무조건 False 반환 ( 실제 디버그 할 경우, 확인 해야 하므로... )
BOOL CTimeOut::GetTimeOutFlag()
{
	int nTime = clock();

#ifdef _DEBUG	//Debug
	return	FALSE;
#else			//Release
	return ( nTime >= m_nEndTime ) ? TRUE : FALSE;
#endif	
}

// 타임 아웃 설정
void CTimeOut::SetTimeOut(int nTime_ms)
{	
	m_nSetTime = nTime_ms;
}