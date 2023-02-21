// _FileMng.cpp: implementation of the CFileMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileMng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileMng::CFileMng()
{

}

CFileMng::~CFileMng()
{

}

BOOL CFileMng::m_fnMakeDirectory(CString strPathName)
{
	CString		strTemp, strMakePath;
	int			nTemp, nPos, nLen;
	TCHAR		pBuff[MAX_PATH];
	BOOL		fTemp;
	
	strPathName.TrimLeft(' ');
	strPathName.TrimRight(' ');
	
	strPathName.TrimLeft('\\');
	strPathName.TrimRight('\\');
	
	nPos=m_fnInPos(strPathName,'\\');
	
	if(nPos<0)
	{
		GetCurrentDirectoryW(MAX_PATH, (LPWSTR)pBuff);
		strMakePath.Format(pBuff);
		strMakePath=strMakePath + "\\" + strPathName;
		fTemp=CreateDirectory(strMakePath.GetBuffer(strMakePath.GetLength()),NULL);
		strMakePath.ReleaseBuffer();
		
		if(fTemp)
			return TRUE;
		else
		{
			nTemp=GetLastError();

			if(nTemp == ERROR_ALREADY_EXISTS && nTemp != ERROR_ACCESS_DENIED)
				return TRUE;
			else
				return FALSE;
		}
	}
	
	do
	{
		nLen=strPathName.GetLength();
		if(strMakePath.GetLength()==0)
			strMakePath=strPathName.Left(nPos);
		else
			strMakePath=strMakePath + "\\" + strPathName.Left(nPos);
		strPathName=strPathName.Right(nLen-nPos-1);
		
		fTemp = CreateDirectory(strMakePath.GetBuffer(strMakePath.GetLength()),NULL);
		strMakePath.ReleaseBuffer();
		
		if(!fTemp)
		{
			nTemp = GetLastError();

			if(nTemp != ERROR_ALREADY_EXISTS && nTemp != ERROR_ACCESS_DENIED)
				return FALSE;
		}
		
		nPos = m_fnInPos(strPathName,'\\');
		if(nPos<0)
		{
			strMakePath=strMakePath + "\\" + strPathName;
			fTemp=CreateDirectory(strMakePath.GetBuffer(strMakePath.GetLength()),NULL);
			strMakePath.ReleaseBuffer();
			
			if(fTemp)
				return TRUE;
			else
			{
				nTemp = GetLastError();
				if(nTemp == ERROR_ALREADY_EXISTS && nTemp != ERROR_ACCESS_DENIED)
					return TRUE;
				else
					return FALSE;
			}
		}
	} while(nPos>=0);
	
	return TRUE;
}

int CFileMng::m_fnInPos(CString strBase, char cChar, int nStartPos)
{
	int		nLen, i;
	
	if(nStartPos<0)
		return -1;
	
	nLen=strBase.GetLength();
	
	if(nStartPos>(nLen-1))
		return -1;
	
	for(i=nStartPos;i<nLen;i++)
	{
		if(strBase.GetAt(i)==cChar)
			return i;
	}
	
	return -1;
}

BOOL CFileMng::m_fnFindFile(CString strFullPath)
{
	BOOL bRes = FALSE;

	CFileFind findFile;

	bRes = findFile.FindFile(strFullPath);

	return bRes;
}
///////////////////////////////////////////////////////////////////////
//	Function name	: m_fnDeleteFiles
//	Description	: 주어진 기간을 초과한 모든 폴더및 파일을 삭제한다.
//	Return type	: int
//	Argument	: CString strFilePath	: 삭제할 폴더이름.
//			: int nDayPeriod	: 비교할 기간.
//
//	comment		: 비교기간은 Day 단위임......................................

int CFileMng::m_fnDeleteFiles(CString strFilePath, int nDayPeriod, BOOL bFileOnly)
{
	CString		strFileName, strFindFileName, strTemp;

	CTime			fileTime;
	CFileFind	fileFind;
	char*			szFileName;
	int				nTemp=0, nNumOfDelete=0;
	BOOL			fTemp;
	DWORD		dwFileDayAmount, dwNowDayAmount, dwTemp;


	strFileName	= strFilePath + _T("\\*.*");
	szFileName	= (char*)strFileName.GetBuffer(strFileName.GetLength());
	strFileName.ReleaseBuffer();

	fTemp=fileFind.FindFile((LPCTSTR)(LPCSTR)szFileName);

	if(!fTemp)
		return nNumOfDelete;


	//	1. 우선, 현재위치한 폴더내의 파일을 모두 지운다.
	//	2. 그다음, 서브 폴더내로 들어가서 반복작업(재귀호출)한다.
	while(fTemp)
	{
		fTemp=fileFind.FindNextFile();

		fileFind.GetLastWriteTime(fileTime);
		
		dwFileDayAmount	= m_fnGetTimeAmount(fileTime);
		dwNowDayAmount	= m_fnGetTimeAmoutNow();
		strFindFileName	= fileFind.GetFileName();

		if(dwNowDayAmount>=(dwFileDayAmount+nDayPeriod))
		{
			strFindFileName = fileFind.GetFileName();
			strFindFileName = strFilePath + "\\" + strFindFileName;
			if(fileFind.IsDirectory())
			{
				if(!fileFind.IsDots())
				{
					if(!bFileOnly)
					{
						m_fnDeleteFolder(strFindFileName);
					}
//					if(!m_fnDeleteFolder(strFindFileName))
//						m_fnDeleteFiles(strFindFileName, nDayPeriod);
				}
			}
			else
			{
				if( !fileFind.IsDirectory() && !fileFind.IsDots() )
				{
					if(fileFind.IsReadOnly())
					{
						//SHINY 추가 
						//						strTemp="Read-Only File - FileName : " + strFindFileName;
						//						m_pApp->m_fnSendLogToFile("__AutoDelete", strTemp);
					}
					if(!DeleteFile(strFindFileName.GetBuffer(strFindFileName.GetLength())))
					{
						strFindFileName.ReleaseBuffer();
						dwTemp=GetLastError();
					}
					else
					{
						strFindFileName.ReleaseBuffer();
						nNumOfDelete++;
						//						strTemp="Delete file - FileName : " + strFindFileName;
						//						m_pApp->m_fnSendLogToFile("__AutoDelete", strTemp);
					}
				}
			}
		}
	}
	fileFind.Close();

	return nNumOfDelete;
}

DWORD CFileMng::m_fnGetTimeAmount(CTime& time)
{
	int		nYear, nMonth, nDay;
	DWORD	dwAmount=0, dwTemp=0;
	int		i;

	nYear	= time.GetYear();
	nMonth	= time.GetMonth();
	nDay	= time.GetDay();

	for(i=2001;i<nYear;i++)
	{
		if(m_fnIsYunYear(i))
			dwTemp+=366;
		else
			dwTemp+=365;
	}

	dwAmount+=dwTemp;
	dwTemp=0;

	for(i=1; i<nMonth; i++)
	{
		switch(i)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			dwTemp+=31;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			dwTemp+=30;
			break;
		case 2:
			if(m_fnIsYunYear(nYear))
				dwTemp+=29;
			else
				dwTemp+=28;
			break;
		default:
			dwTemp+=30;
			break;
		}
	}

	dwAmount+=dwTemp;
	dwTemp=0;

	dwTemp=nDay;
	dwAmount+=dwTemp;

	return dwAmount;
}
DWORD CFileMng::m_fnGetTimeAmoutNow()
{
	CTime		time;
	time=CTime::GetCurrentTime();
	return m_fnGetTimeAmount(time);
}
//BOOL CFileMng::m_fnIsYunYear(int nYear)
//{
//	BOOL	fTemp;
//
//	if(nYear%4==0)
//	{
//		if(nYear%100==0)	
//		{	
//			if(nYear%400==0)	{	fTemp=TRUE;		}
//			else				{	fTemp=FALSE;	}
//		}
//		else					{	fTemp=FALSE;	}
//	}
//	else						{	fTemp=FALSE;	}
//
//
//	return fTemp;
//}

BOOL CFileMng::m_fnDeleteFolder(const CString &strFolder)
{
	SHFILEOPSTRUCT FileOp = {0};
	TCHAR szTemp[2048 * 2];

	wcscpy_s(szTemp, strFolder.GetLength() + 1, strFolder);
	szTemp[strFolder.GetLength() + 1] = NULL;				// NULL문자가 두개 들어가야 한다.

	FileOp.hwnd						= NULL;
	FileOp.wFunc					= FO_DELETE;
	FileOp.pFrom					= (LPCWSTR)szTemp;
	FileOp.pTo						= NULL;
	FileOp.fFlags					= FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT ;	// 확인메시지가 안 뜨도록 설정
	FileOp.fAnyOperationsAborted	= false;
	FileOp.hNameMappings			= NULL;
	FileOp.lpszProgressTitle		= NULL;

	int n = SHFileOperation(&FileOp);

	return TRUE;
}
