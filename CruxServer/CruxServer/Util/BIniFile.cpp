// BIniFile.cpp: implementation of the CBIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BIniFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBIniFile::CBIniFile()
{
	::memset(m_szFileName, 0x00, MAX_PATH);
}

CBIniFile::CBIniFile(LPCTSTR lpFileName)
{
	::lstrcpy(m_szFileName, lpFileName);
}

CBIniFile::~CBIniFile()
{

}

void CBIniFile::SetFileName(LPCTSTR lpFileName)
{
	::lstrcpy(m_szFileName, lpFileName);
}

BOOL CBIniFile::WriteProfileSection(LPCTSTR lpszSection, LPCTSTR lpString)
{
	return ::WritePrivateProfileSection(lpszSection, lpString, m_szFileName);
}


BOOL CBIniFile::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpKeyName, LPCTSTR lpString)
{
	return ::WritePrivateProfileString(lpszSection, lpKeyName, lpString, m_szFileName);
}

BOOL CBIniFile::WriteProfileStruct(LPCTSTR lpszSection, LPCTSTR lpKeyName, LPVOID lpStruct, UINT uSizeStruct)
{
	return ::WritePrivateProfileStruct(lpszSection, lpKeyName, lpStruct, uSizeStruct, m_szFileName);
}

DWORD CBIniFile::GetProfileSectionNames(LPTSTR lpszReturnBuffer, DWORD nSize)
{
	return ::GetPrivateProfileSectionNames(lpszReturnBuffer, nSize, m_szFileName);
}

DWORD CBIniFile::GetProfileSection(LPCTSTR lpszSection, LPTSTR lpReturnedString, DWORD nSize)
{
	return ::GetPrivateProfileSection(lpszSection, lpReturnedString, nSize, m_szFileName);
}

DWORD CBIniFile::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize)
{
	return ::GetPrivateProfileString(lpszSection, lpKeyName, lpDefault, lpReturnedString, nSize, m_szFileName);
}

UINT CBIniFile::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpKeyName, INT nDefault)
{
	return ::GetPrivateProfileInt(lpszSection, lpKeyName, nDefault, m_szFileName);
}

BOOL CBIniFile::GetProfileStruct(LPCTSTR lpszSection, LPCTSTR lpKeyName, LPVOID lpStruct, UINT uSizeStruct)
{
	return ::GetPrivateProfileStruct(lpszSection, lpKeyName, lpStruct, uSizeStruct, m_szFileName);
}
