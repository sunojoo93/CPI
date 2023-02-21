// Back.cpp: implementation of the CBack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Back.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#define MAX_BUFFER		1024

CBack::CBack()
{

}

CBack::~CBack()
{

}

int CBack::m_fnGetIntFromIniFile(CString strSN, CString strKN, CString strFN, int nDefault/*=0*/)
{
	char			szResData[MAX_BUFFER];
	char			szDefault[MAX_BUFFER];

	memset(szResData, 0x00, sizeof(szResData));
	memset(szDefault, 0x00, sizeof(szDefault));

	sprintf_s((char*)szDefault, MAX_BUFFER, "%d", nDefault);

	DWORD dwRes = ::GetPrivateProfileStringW(strSN, strKN, (LPCWSTR)szDefault, (LPWSTR)szResData, MAX_BUFFER, strFN);

	if(dwRes <= 0)		return nDefault;

	return atoi(szResData);
}

CString CBack::m_fnGetStringFromIniFile(CString strSN, CString strKN, CString strFN, CString strDefault/*=""*/)
{
	int				nLen = 0;
	TCHAR			szResData[MAX_BUFFER];
	TCHAR			szDefault[MAX_BUFFER];

	memset(szResData, 0x00, sizeof(szResData));
	memset(szDefault, 0x00, sizeof(szDefault));

	swprintf_s(szDefault, MAX_BUFFER,  strDefault);

	DWORD dwRes = ::GetPrivateProfileStringW(strSN, strKN, szDefault, szResData, MAX_BUFFER, strFN);

	if(dwRes <= 0)		return strDefault;
	
	return CString(szResData);
}

BOOL CBack::m_fnPutDataToIniFile(CString strSN, CString strKN, CString strFN, CString strValue)
{
	return ::WritePrivateProfileString(strSN,strKN,strValue,strFN);
}

BOOL CBack::m_fnPutDataToIniFile(CString strSN, CString strKN, CString strFN, int nValue)
{
	CString		strTemp;

	strTemp.Format(_T("%d"),nValue);
	return m_fnPutDataToIniFile(strSN,strKN,strFN,strTemp);
}

BOOL CBack::m_fnPutDataToIniFile(CString strSN, CString strKN, CString strFN, char *szValue)
{
	if(!szValue) return FALSE;

	CString		strTemp;

	strTemp=szValue;
	return m_fnPutDataToIniFile(strSN,strKN,strFN,strTemp);
}

CString	CBack::m_fnReplace(CString strSrc, CString szSrc/*="'"*/, CString szRep/*="''"*/)
{
	if(strSrc.IsEmpty()) return strSrc;

	strSrc.Replace(szSrc, szRep);
	strSrc.Replace(_T("\\"), _T("\\\\"));

	return strSrc;
}

CString	CBack::m_fnGetDateStringSecond()
{
	CString			strRet;
	SYSTEMTIME		time;

	::GetLocalTime(&time);

	strRet.Format(_T("%4d%02d%02d%02d%02d%02d"),time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);

	return strRet;
}

CString	CBack::m_fnGetDateStringDay()
{
	CString			strRet;
	SYSTEMTIME		time;

	::GetLocalTime(&time);

	strRet.Format(_T("%4d%02d%02d"),time.wYear,time.wMonth,time.wDay);

	return strRet;
}

void CBack::m_fnGetLineData(CString strSrc, CString strSeparator, short* nSrcData, int nCount)
{
	CString strTemp;
	int		nTemp = 0, nLength = 0;
	
	nLength = strSrc.GetLength();

	for(int i=0; i<nLength; i++)
	{
		char cGet = (char)strSrc.GetAt(i);

		if(CString(cGet) == strSeparator)
		{
			nSrcData[nTemp] = (short)_ttoi(strTemp);
			strTemp.Empty();

			nTemp++;
		}
		else
		{
			strTemp += CString(cGet);

			if(i == nLength - 1)
			{
				if(!strTemp.IsEmpty())
					nSrcData[nTemp] = (short)_ttoi(strTemp);
				break;
			}
		}

		if(nTemp == nCount) break;
	}
}

void CBack::m_fnGetLineData(CString strSrc, CString strSeparator, TCHAR** szSrcData, int nCount)
{
	CString strTemp;
	int		nTemp = 0, nLength = 0;
	
	nLength = strSrc.GetLength();

	for(int i=0; i<nLength; i++)
	{
		char cGet = (char)strSrc.GetAt(i);
		strTemp = cGet;

		if(CString(cGet) == strSeparator)
		{
			swprintf_s(szSrcData[nTemp], strTemp.GetLength(), strTemp);
			strTemp.Empty();

			nTemp++;
		}
		else
		{
			strTemp += CString(cGet);

			if(i == nLength - 1)
			{
				if(!strTemp.IsEmpty())
					swprintf_s(szSrcData[nTemp],strTemp.GetLength(), strTemp);
				break;
			}
		}

		if(nTemp == nCount) break;
	}
}

BOOL CBack::m_fnIsYunYear(int nYear)
{
	BOOL	fTemp;

	if(nYear%4==0)
	{
		if(nYear%100==0)	
		{	
			if(nYear%400==0)	{	fTemp=TRUE;		}
			else				{	fTemp=FALSE;	}
		}
		else					{	fTemp=FALSE;	}
	}
	else						{	fTemp=FALSE;	}


	return fTemp;
}
