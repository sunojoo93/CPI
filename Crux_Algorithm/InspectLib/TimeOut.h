
/************************************************************************/
// Time Out ���� ���
// ������ : 18.02.26 - KSW
/************************************************************************/


#pragma once


class CTimeOut
{
public:
	CTimeOut(void);
	virtual ~CTimeOut(void);

public:
	long				TimeCheckStart();			// ���� ����
	BOOL				GetTimeOutFlag();			// Ÿ�� �ƿ� Ȯ�� �÷���
	void				SetTimeOut(int nTime_ms);	// Ÿ�� �ƿ� ����

protected:
	int					m_nSetTime;				// ���� �ð�
	int					m_nEndTime;				// ���� �ð�
};

