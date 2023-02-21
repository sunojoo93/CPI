// LogMng.h: interface for the CLogMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGMNG_H__CA6292D0_6D51_4170_B5FB_FC311AC39425__INCLUDED_)
#define AFX_LOGMNG_H__CA6292D0_6D51_4170_B5FB_FC311AC39425__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filemng.h"

class CLogMng
{
public:
	CLogMng();
	virtual ~CLogMng();

	//2015.07.09 Chu 
	int				m_nCompareStageNum;
	int				m_nCurrentLogPriority;
	int				m_fnSetAutoDeleteInfo(CString strFilePath, int nPeriod=30);
	void			m_fnSetInitInfo(CString strLogPath, CString strHeaderName, int nLogPeriod=2);
	BOOL			m_fnLogMngWriteLog(CString strLog, BOOL fIsAddNewLine=TRUE);
	//2015.07.03 Chu
	//BOOL			m_fnLogMngWriteLog(CString strLog, BOOL fIsDateFolder, BOOL fIsAddNewLine=TRUE, int WriteStageLog=FALSE);
	BOOL			m_fnLogMngWriteLog(CString strLog, BOOL fIsDateFolder, BOOL fIsAddNewLine=TRUE, int WriteStageLog=FALSE, int NoUseTotalLog = FALSE);
	BOOL			m_fnMakeDirectory(CString strPathName);
	//2015.07.03 Chu
	CString m_fnReturnLogFileName_Stage(BOOL fIsDateFolder, CString strFilePath, BOOL NoUseTotalLog = FALSE);
	CRITICAL_SECTION m_csLogMng;

// Attribute
protected:
	int				m_nLogPeriod;
	
	CString			m_strLogPath;
	CString			m_strHeaderName;
	CFileMng		m_fileMng;

	// Auto Delete..
	HANDLE			m_hThread;					// Thread Handle.
	int				m_nDeletePeriod;
	CString			m_strDeleteFilePath;
	BOOL			m_bFileAutoDelete;
	BOOL			m_bFileAutoDelete_Working;

// Operation
protected:
	CString			m_fnReturnLogTextHeader();
	CString			m_fnReturnLogFileName(BOOL fIsDateFolder);
	CString			m_fnGetNowString();
	CString			m_fnGetDateString();

	// Auto Delete..
	int				m_fnStartAutoDelete(BOOL bStart);
	static void		m_fnCheckAutoDelte(LPVOID pWnd);

};

#endif // !defined(AFX_LOGMNG_H__CA6292D0_6D51_4170_B5FB_FC311AC39425__INCLUDED_)
