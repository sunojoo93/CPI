#pragma once

// CThreadProcCIM

class CThreadProcCIM : public CWinThread
{
	DECLARE_DYNCREATE(CThreadProcCIM)

protected:
	CThreadProcCIM();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThreadProcCIM();
private:
	
public:
	CRITICAL_SECTION m_csThreadCim;
	HANDLE m_hThreadCim;
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	static unsigned __stdcall   m_fnResultJudgeProc(LPVOID lParam);
	//CString m_strTheadData;
	STRU_NG_FLAG_INFO m_stNgFlagTheadData[2];
protected:
};


