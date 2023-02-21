#pragma once

// CThreadProcData

class CThreadProcData : public CWinThread
{
	DECLARE_DYNCREATE(CThreadProcData)

protected:
	CThreadProcData();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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


