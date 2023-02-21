// Back.h: interface for the CBack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACK_H__4548F324_FF1E_4B96_8795_B1EDF4A5A166__INCLUDED_)
#define AFX_BACK_H__4548F324_FF1E_4B96_8795_B1EDF4A5A166__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBack  
{
public:
	CBack();
	virtual ~CBack();

	int				m_fnGetIntFromIniFile(CString strSN, CString strKN, CString strFN, int nDefault=0);
	CString			m_fnGetStringFromIniFile(CString strSN, CString strKN, CString strFN, CString strDefault=_T(""));
	BOOL			m_fnPutDataToIniFile(CString strSN, CString strKN, CString strFN, CString strValue);
	BOOL			m_fnPutDataToIniFile(CString strSN, CString strKN, CString strFN, char* szValue);
	BOOL			m_fnPutDataToIniFile(CString strSN, CString strKN, CString strFN, int nValue);

	CString			m_fnReplace(CString strSrc, CString szSrc=_T("'"), CString szRep=_T("''"));
	void			m_fnGetLineData(CString strSrc, CString strSeparator, short* nSrcData, int nCount);
	void			m_fnGetLineData(CString strSrc, CString strSeparator, TCHAR** szSrcData, int nCount);

	BOOL			m_fnIsYunYear(int nYear);
	CString			m_fnGetDateStringSecond();	/* year/month/day/hour/minute/second */
	CString			m_fnGetDateStringDay();	/* year/month/day */										
};

#endif // !defined(AFX_BACK_H__4548F324_FF1E_4B96_8795_B1EDF4A5A166__INCLUDED_)
