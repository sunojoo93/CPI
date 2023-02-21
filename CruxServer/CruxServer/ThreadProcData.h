#pragma once

// CThreadProcData

class CThreadProcData : public CWinThread
{
	DECLARE_DYNCREATE(CThreadProcData)

protected:
	CThreadProcData();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CThreadProcData();
private:

public:
	CRITICAL_SECTION m_csThreadData;
	HANDLE m_hThreadData;
	HANDLE m_hThreadDelDir;
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	static unsigned __stdcall   m_fnOverlapDefectData(LPVOID pArg);
	static unsigned __stdcall m_fnDeleteAllFolder(LPVOID pArg);
	CString m_strTheadData;
protected:

};


