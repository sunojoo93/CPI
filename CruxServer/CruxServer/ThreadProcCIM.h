#pragma once

// CThreadProcCIM

class CThreadProcCIM : public CWinThread
{
	DECLARE_DYNCREATE(CThreadProcCIM)

protected:
	CThreadProcCIM();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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


