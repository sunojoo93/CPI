#pragma once
#include <afxmt.h>

#define SM_STEP_INIT				0
#define SM_STEP_START				1
#define SM_STEP_END					(-1)

#define SLEEP_COUNT				50


class CSequenceManager : public CWnd
{
public:
	CSequenceManager();
	virtual ~CSequenceManager();

	int				m_fnInit();
	int				m_fnDeInit();			

	void			m_fnInitStep(int nStepValue=SM_STEP_INIT);
	void			m_fnClearScenario();
	void			m_fnSetStep(int nStep);			
	void			m_fnSetOldStep(int nOldStep);	
	int				m_fnGetStep();
	int				m_fnGetOldStep();
	BOOL			m_fnCheckStep();
	int				m_fnRetryStep();

protected:

	int				m_fnStartScenario(BOOL bStart, BOOL nScenarioMode = TRUE);
	static UINT		m_fnCheckScenario(LPVOID lpThread);
	static UINT		m_fnCheckInput(LPVOID lpThread);
	virtual void	m_fnCheck() {}
	int				m_fnGetCurrentTime();
	int				m_fnGetCurrentTime2();
		
	HANDLE			m_hThread;
//	BOOL			m_bFlagThread;

	int				m_nCurStep;
	int				m_nOldStep;

	CWinThread* m_pThread; 
	volatile bool m_bFlagThread;    // 이벤트를 써도 됨
	CCriticalSection m_csExitThread;
};

