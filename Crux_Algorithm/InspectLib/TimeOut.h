
/************************************************************************/
// Time Out 관련 헤더
// 수정일 : 18.02.26 - KSW
/************************************************************************/


#pragma once


class CTimeOut
{
public:
	CTimeOut(void);
	virtual ~CTimeOut(void);

public:
	long				TimeCheckStart();			// 동작 실행
	BOOL				GetTimeOutFlag();			// 타임 아웃 확인 플래그
	void				SetTimeOut(int nTime_ms);	// 타임 아웃 설정

protected:
	int					m_nSetTime;				// 설정 시간
	int					m_nEndTime;				// 종료 시간
};

