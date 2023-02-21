// _FileMng.h: interface for the CFileMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX__FILEMNG_H__3C362528_4BDF_4441_8A78_BC49BDB150F1__INCLUDED_)
#define AFX__FILEMNG_H__3C362528_4BDF_4441_8A78_BC49BDB150F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "back.h"

class CFileMng : public CBack
{
public:
	CFileMng();
	virtual ~CFileMng();


	BOOL				m_fnMakeDirectory(CString strPathName);
	BOOL				m_fnFindFile(CString strFullPath);
	int					m_fnDeleteFiles(CString strFilePath, int nPeriod, BOOL bFileOnly);
	//	BOOL				m_fnIsYunYear(int nYear);
	DWORD				m_fnGetTimeAmount(CTime& time);
	DWORD				m_fnGetTimeAmoutNow();
	BOOL				m_fnDeleteFolder(const CString &strFolder);

protected:
	int					m_fnInPos(CString strBase, char cChar, int nStartPos=0);
};

#endif // !defined(AFX__FILEMNG_H__3C362528_4BDF_4441_8A78_BC49BDB150F1__INCLUDED_)
