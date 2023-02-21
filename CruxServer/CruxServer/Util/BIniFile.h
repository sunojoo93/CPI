// BIniFile.h: interface for the CBIniFile class.
// CBIniFile
// : INI ������ ���� ����ϱ� ���� Wrapper Class
// : ����ö
// : 2003. 3. 10
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINIFILE_H__695DB7A2_39EB_4476_A340_43842C207239__INCLUDED_)
#define AFX_BINIFILE_H__695DB7A2_39EB_4476_A340_43842C207239__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBIniFile  
{
public:
	inline LPCTSTR GetPathName() { return m_szFileName; };
	BOOL GetProfileStruct(LPCTSTR lpszSection, LPCTSTR lpKeyName, LPVOID lpStruct, UINT uSizeStruct);
	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpKeyName, INT nDefault);
	DWORD GetProfileString(LPCTSTR lpszSection, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize);
	DWORD GetProfileSection(LPCTSTR lpszSection, LPTSTR lpReturnedString, DWORD nSize);
	DWORD GetProfileSectionNames(LPTSTR lpszReturnBuffer, DWORD nSize);
	BOOL WriteProfileStruct(LPCTSTR lpszSection, LPCTSTR lpKeyName, LPVOID lpStruct, UINT uSizeStruct);
	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpKeyName, LPCTSTR lpString);
	BOOL WriteProfileSection(LPCTSTR lpszSection, LPCTSTR lpString);
	void SetFileName(LPCTSTR lpFileName);

	CBIniFile(LPCTSTR lpFileName);
	CBIniFile();
	virtual ~CBIniFile();

private:
	TCHAR m_szFileName[MAX_PATH];	//INI ������ ����Ǵ� Ǯ�н�.
							//NULL�� ��� win.ini�� ����ȴ�.
							//��ΰ� ���� ��� windows������ ����ȴ�.
};

#endif // !defined(AFX_BINIFILE_H__695DB7A2_39EB_4476_A340_43842C207239__INCLUDED_)
