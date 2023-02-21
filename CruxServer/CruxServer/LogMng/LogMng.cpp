// LogMng.cpp: implementation of the CLogMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogMng.h"
#include "Share.h"
#include <stdio.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CLogMng::CLogMng()
{
	m_bFileAutoDelete = FALSE;
	m_bFileAutoDelete_Working = FALSE;
	m_nDeletePeriod = -1;
	m_strDeleteFilePath = "";
	m_hThread = NULL;
	m_nCompareStageNum = NULL;
	m_nCurrentLogPriority = 5;
	::InitializeCriticalSection(&m_csLogMng);
}

CLogMng::~CLogMng()
{
	if(m_bFileAutoDelete)
	{
		m_nDeletePeriod = -1;
		m_strDeleteFilePath = "";
		m_fnStartAutoDelete(FALSE);
	}
	::DeleteCriticalSection(&m_csLogMng);
}


//초기설정하는 함수 -> 로그화일이 생성되는 위치, 로그화일명, 생성되는 Timing분할
void CLogMng::m_fnSetInitInfo(CString strLogPath, CString strHeaderName, int nLogPeriod)
{
	m_strLogPath	= strLogPath;
	m_strHeaderName = strHeaderName;
	m_nLogPeriod	= nLogPeriod;

}

//생성되고 있는 로그화일명이 필요할 경우 호출하는 함수
CString CLogMng::m_fnReturnLogFileName(BOOL fIsDateFolder)
{
	CString		strRet, strTemp;

	if(m_strHeaderName.IsEmpty())
	{
		strTemp = m_fnGetDateString();
		if(fIsDateFolder == TRUE)
			strRet = m_strLogPath + _T("\\") + strTemp + _T(".Log");
		else
			strRet = m_strLogPath + _T("\\") + _T(".Log");
	}
	else
	{
		strTemp=m_fnGetDateString();
		if(fIsDateFolder == TRUE)
			strRet = m_strLogPath + _T("\\") + strTemp + _T("\\") + m_strHeaderName + _T("_") + strTemp + _T(".Log");
		else
			strRet = m_strLogPath + _T("\\") + m_strHeaderName + _T("_") + strTemp + _T(".Log");
	}

	return strRet;
}
//현재 시간정보 반환
CString CLogMng::m_fnGetNowString()
{
	CString			strRet;
	SYSTEMTIME		time;

	::GetLocalTime(&time);

	strRet.Format(_T("%4d%02d%02d%02d%02d%02d%03d"),
				  time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond,time.wMilliseconds);

	return strRet;
}
//Write하는 날짜정보의 구성요소 설정
CString CLogMng::m_fnGetDateString()
{
	CString			strRet;
	SYSTEMTIME		time;

	::GetLocalTime(&time);

	switch(m_nLogPeriod)
	{
	case 0:	// Minuter까지 보여줄지
		strRet.Format(_T("%4d%02d%02d%02d%02d"),time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute);
		break;
	case 1:	// Hour까지 보여줄지
		strRet.Format(_T("%4d%02d%02d%02d"),time.wYear,time.wMonth,time.wDay,time.wHour);
		break;
	case 2: // Day까지 보여줄지
	default:
		strRet.Format(_T("%4d%02d%02d"),time.wYear,time.wMonth,time.wDay);
		break;
	}

	return strRet;
}

//화일안에 생성될 실제 로그내용 포맷
BOOL CLogMng::m_fnLogMngWriteLog(CString strLog, BOOL fIsAddNewLine)
{
	if(strLog == _T(""))
		return FALSE;

	CString			strFN, strLogText, strTemp;
	FILE			*pFile;

	if(m_strLogPath.IsEmpty()) return FALSE;
	
	m_fileMng.m_fnMakeDirectory(m_strLogPath + "\\" + m_fnGetDateString());

	strFN = m_fnReturnLogFileName(TRUE);

	if(strFN == "")
		return FALSE;

	strLogText	= m_fnReturnLogTextHeader();	

	if(fIsAddNewLine)						
		strLog += "\n";

	strLogText += strLog;					
	
	_wfopen_s(&pFile, strFN, (const wchar_t*)_T("a+"));
	if(!pFile)
	{
		CString strErr;
		strErr.Format(_T("Log File Open Error %d"), GetLastError());
		return(0);
	}

	fwprintf(pFile, strLogText);
	fclose(pFile);
	
	return TRUE;
}
// 
//2015.07.03 Chu WriteStageLog 인자 추가시킴
BOOL CLogMng::m_fnLogMngWriteLog(CString strLog, BOOL fIsDateFolder/*=TRUE*/, BOOL fIsAddNewLine/*=TRUE*/, int WriteStageLog, int NoUseTotalLog)
{
	BOOL bRes = TRUE;
	EnterCriticalSection(&m_csLogMng);
	if(strLog == _T("")){
		LeaveCriticalSection(&m_csLogMng);
		return FALSE;
	}
	CString			strFN, strLogText, strTemp;
	FILE			*pFile;

	if(m_strLogPath.IsEmpty()){
		LeaveCriticalSection(&m_csLogMng);
		return FALSE;
	}


	if(fIsDateFolder)
	{
		m_fileMng.m_fnMakeDirectory(m_strLogPath + "\\" + m_fnGetDateString());
		strFN = m_fnReturnLogFileName(fIsDateFolder);
	}
	else
	{
		m_fileMng.m_fnMakeDirectory(m_strLogPath + _T("\\"));
		strFN = m_fnReturnLogFileName(fIsDateFolder);
	}

	if(strFN == ""){
		LeaveCriticalSection(&m_csLogMng);
		return FALSE;
	}
	strLogText	= m_fnReturnLogTextHeader();	

	if(fIsAddNewLine)						
		strLog += "\n";

	strLogText += strLog;		
	// 2015.11.27 comment by ikm for Log Delay - S
	if (!NoUseTotalLog)
	{


		_wfopen_s(&pFile, strFN, (const wchar_t*)_T("a+"));
		if(!pFile)
		{
			CString strErr;
			strErr.Format(_T("Log File Open Error %d"), GetLastError());
			LeaveCriticalSection(&m_csLogMng);
			return(0);
		}

		fwprintf(pFile, strLogText);
		fclose(pFile);
	}
	// 2015.11.27 comment by ikm for Log Delay - E


	//2015.07.03 Chu
	if(WriteStageLog != -1)
	{
		//m_strLogPath
		CString strStageLogPath,strStageLog;
		strStageLogPath.Format(_T("%s\\Stage %d"),m_strLogPath,WriteStageLog+1);

		m_fileMng.m_fnMakeDirectory(strStageLogPath + _T("\\"));
		strStageLog = m_fnReturnLogFileName_Stage(fIsDateFolder,strStageLogPath, NoUseTotalLog);

		_wfopen_s(&pFile, strStageLog, (const wchar_t*)_T("a+"));
		if(!pFile)
		{
			CString strErr;
			strErr.Format(_T("Log File Open Error %d"), GetLastError());
			LeaveCriticalSection(&m_csLogMng);
			return(0);
		}

		fwprintf(pFile, strLogText);
		fclose(pFile);

	}
	LeaveCriticalSection(&m_csLogMng);
	return bRes;
}

CString CLogMng::m_fnReturnLogTextHeader()
{
	CString			strRet, strTemp;

	strTemp=m_fnGetNowString();

	switch(m_nLogPeriod)
	{
	case 0:		// Minute당 생성
		strRet = strTemp.Mid(0,4)  + "/" +
				 strTemp.Mid(4,2)  + "/" +
				 strTemp.Mid(6,2)  + " " +
				 strTemp.Mid(8,2)  + ":" +
				 strTemp.Mid(10,2) + ":" +
				 strTemp.Mid(12,2) + "." +
				 strTemp.Mid(14,3);
		break;
	case 1:		// Hour당 생성
	case 2:		// Day당 생성
	default:	
		strRet = strTemp.Mid(0,4)  + "/" +
				 strTemp.Mid(4,2)  + "/" +
				 strTemp.Mid(6,2)  + " " +
				 strTemp.Mid(8,2)  + ":" +
				 strTemp.Mid(10,2) + ":" +
				 strTemp.Mid(12,2) + "." +
				 strTemp.Mid(14,3);
		break;
	}

	if(m_strHeaderName.IsEmpty())
		strRet += _T(" ");
	else
		strRet += _T("| [") + m_strHeaderName + _T("] ");
	
	return strRet;
}

///////////////////////////////////////////////////////////////////////
//	Function name	: m_fnMakeDirectory
//	Description	    : By city7
//	Return type		: BOOL
//
//  Argument        : CString strPathName
//	comment			: 원하는 Local 드라이브에 폴더가 있는지 없는지 확인하고
//					  폴더를 만들어준다.
//

BOOL CLogMng::m_fnMakeDirectory(CString strPathName)
{
	BOOL bRes = TRUE;

	bRes = m_fileMng.m_fnMakeDirectory(strPathName);

	return bRes;
}

int	CLogMng::m_fnStartAutoDelete(BOOL bStart)
{
	int nRes = 0;
	
	if(bStart)
	{
		if(m_hThread != NULL)
		{
			DWORD  exitcode = 0;

			m_bFileAutoDelete = FALSE;

			::WaitForSingleObject(m_hThread, INFINITE);
			::GetExitCodeThread(m_hThread, &exitcode);

			if(exitcode ==  STILL_ACTIVE)
				::TerminateThread(m_hThread, exitcode);

			::CloseHandle(m_hThread);

			m_hThread   = NULL;
		}

		m_bFileAutoDelete = TRUE;

		DWORD dwThreadID;
		m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)m_fnCheckAutoDelte, (LPVOID)this, CREATE_SUSPENDED, &dwThreadID);
		ResumeThread(m_hThread);

		if(m_hThread == NULL) 
			return -1;
	}
	else
	{
		DWORD  exitcode = 0;

		m_bFileAutoDelete = FALSE;

		::WaitForSingleObject(m_hThread, INFINITE);
		::GetExitCodeThread(m_hThread, &exitcode);

		if(exitcode ==  STILL_ACTIVE)
			::TerminateThread(m_hThread, exitcode);

		::CloseHandle(m_hThread);

		m_hThread   = NULL;
	}

	return nRes;
}

void CLogMng::m_fnCheckAutoDelte(LPVOID pWnd)
{
	CLogMng*		pCtrlWnd = (CLogMng*)pWnd;

	CFileFind		fileFind;
	CTime			fileTime;
	BOOL			bStart = FALSE, bFindFile = FALSE;
	DWORD			dwFileDayAmount, dwNowDayAmount;

	CString			strDeleteFilePath, strFindFilePath, strTemp, strTemp2;
	//2015.07.09 Chu  Stage 별 Log 저장 변수
	static int nStage;
	static bool bStage;
	CString strStage,strStageCompare;

	//strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\*.*");
	//2015.07.09 Chu  Stage 별 Log 저장 경로
	switch( nStage )
	{
	case 0:
		strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\*.*");
		bStage = true;
		break;
	case 1:
		strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\Stage 1\\*.*");
		bStage = false;
		break;
	case 2:
		strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\Stage 2\\*.*");
		bStage = false;
		break;
	case 3:
		strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\Stage 3\\*.*");
		bStage = false;
	}
	nStage = 0;

	while(1)
	{
		Sleep(1000);

		if(!pCtrlWnd->m_bFileAutoDelete) 
		{
			fileFind.Close();
			break;
		}

		if(pCtrlWnd == NULL) continue;

		if(!bStart)
		{
			bFindFile = fileFind.FindFile(strDeleteFilePath);
			bStart = TRUE;
		}

		if(bFindFile)		   
		{
			BOOL bFind = fileFind.FindNextFile();
			strFindFilePath = fileFind.GetFilePath();

			if(fileFind.IsReadOnly()) continue;

			if(fileFind.IsDots())
			{
				if(!bFind)
				{
					// Dots이고 bFind가 FALSE이면 다음 파일이 존재하지 않음. 즉, 빈폴더!
					// 빈폴더 삭제!

					strFindFilePath.Replace(_T("\\.."), _T(""));
					
					strTemp2	= strFindFilePath;
					strTemp		= pCtrlWnd->m_strDeleteFilePath;
					
					if(strTemp.Find(_T("\\\\")) >= 0)
						strTemp2.Replace(_T("\\"), _T("\\\\"));
					
					strTemp.MakeLower();
					strTemp2.MakeLower();
					
					
					// 지정된 디렉토리와 일치하면 삭제 안함.
					if(strTemp2.Compare(strTemp))
					{
						// 현재 열려져있는 걸 닫아야 아래 폴더를 삭제할 수 있으니깐..
						fileFind.Close();	
						
						pCtrlWnd->m_fileMng.m_fnDeleteFolder(strFindFilePath);
					}

					strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\*.*");
					bStart = FALSE;
				}

				continue;
			}

			if(fileFind.IsDirectory())
			{
				strDeleteFilePath = strFindFilePath + _T("\\*.*");
				bStart = FALSE;
				continue;
			}

			fileFind.GetLastWriteTime(fileTime);

			dwFileDayAmount	= pCtrlWnd->m_fileMng.m_fnGetTimeAmount(fileTime);
			dwNowDayAmount	= pCtrlWnd->m_fileMng.m_fnGetTimeAmoutNow();
			
			if(dwNowDayAmount >= (dwFileDayAmount + pCtrlWnd->m_nDeletePeriod))
			{
				strFindFilePath = strFindFilePath.Right(4);

				if(!strFindFilePath.Compare(_T(".Log")) || !strFindFilePath.Compare(_T(".log")))
				{
					CString strFilePath = fileFind.GetFilePath();
					_wunlink(strFilePath);

					if(!bFind)
					{
						// 폴더안에 남아있던 마지막 파일을 삭제한 후 빈폴더 삭제!
						strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\*.*");
						bStart = FALSE;
					}
				}
				else
				{
					if(!bFind)
					{
						// 폴더안에 .log 파일이 아닌 다른 파일이 남아있슴.
						// 자동삭제 중지!
						pCtrlWnd->m_bFileAutoDelete = FALSE;
					}
				}

			}
		}
		else
		{
			// 지정된 폴더 다시 확인.. 다시확인..
			strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\*.*");
			bStart = FALSE;
		}
		//2015.07.09 Chu Stage 별 로그 저장 재귀함수 예외처리
		if(bStage == true)
		{
			nStage = 1;
			m_fnCheckAutoDelte(pWnd);
			nStage = 2;
			m_fnCheckAutoDelte(pWnd);
			nStage = 3;
			m_fnCheckAutoDelte(pWnd);
			strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\*.*");
			bStage = true;
		}
		else
			break;
	}
}

/*
void CLogMng::m_fnCheckAutoDelte(LPVOID pWnd)
{
	CLogMng*		pCtrlWnd = (CLogMng*)pWnd;
	
	CFileFind		fileFind;
	CString			strFileName;
	CString         strFindFileName;
	CString			strTemp;
	
	CTime			fileTime;
	char			szFileName[512];
	char			szTemp[512];
	int				nTemp=0, nNumOfDelete=0, nStart = 0;
	BOOL			fTemp1 = FALSE, fTemp = FALSE;
	DWORD			dwFileDayAmount, dwNowDayAmount, dwTemp;
	
	memset(szFileName, 0x00, sizeof(szFileName));
	
	while(1)
	{
		Sleep(500);
		
		if(!pCtrlWnd->m_bFileAutoDelete) break;
		if(pCtrlWnd == NULL) continue;
		
		if(!nStart)
		{
			strFileName	= pCtrlWnd->m_strDeleteFilePath + "\\*.*";
			sprintf(szFileName, strFileName);
			
			fTemp1 = fileFind.FindFile(szFileName);
			nStart = 1;
		}
		
		if(!fTemp1)
		{
			nStart = 0;
			fileFind.Close();
			
			continue;
		}
		
		fTemp = fileFind.FindNextFile();
		if(!fTemp)
		{
			nStart = 0;
			fileFind.Close();
			
			continue;
		}
		
		fileFind.GetLastWriteTime(fileTime);
		
		dwFileDayAmount	= pCtrlWnd->m_fileMng.m_fnGetTimeAmount(fileTime);
		dwNowDayAmount	= pCtrlWnd->m_fileMng.m_fnGetTimeAmoutNow();
		strFindFileName	= fileFind.GetFileName();
		
		if(!fileFind.IsDots())
		{
			if(dwNowDayAmount >= (dwFileDayAmount + pCtrlWnd->m_nDeletePeriod))
			{
				strFindFileName = fileFind.GetFileName();
				strFindFileName = pCtrlWnd->m_strDeleteFilePath + "\\" + strFindFileName;
				
				if(fileFind.IsDirectory())
				{
					pCtrlWnd->m_fileMng.m_fnDeleteFolder(strFindFileName);
					nNumOfDelete++;
				}
				else
				{
					memset(szTemp, 0x00, sizeof(szTemp));
					
					sprintf(szTemp, strFindFileName);
					
					if(!DeleteFile(szTemp))
					{
						dwTemp = GetLastError();
					}
					else
					{
						nNumOfDelete++;
					}
				}
			}
			
			if(nNumOfDelete)
			{
				nStart = 0;
				nNumOfDelete = 0;
				
				fileFind.Close();
			}
		}
	}
}
*/

int CLogMng::m_fnSetAutoDeleteInfo(CString strFilePath, int nPeriod/*=30*/)
{
	int nRes = 0;

	m_strDeleteFilePath = strFilePath;
	m_nDeletePeriod		= nPeriod;

	if(strFilePath.IsEmpty() || m_nDeletePeriod <= 0)
		return -1;

	if(!strFilePath.Compare(_T("c:\\")))
		return -1;

	nRes = m_fnStartAutoDelete(TRUE);

	return nRes;
}

//2015.07.03 Chu
//기능 : 생성되고 있는 로그화일명이 필요할 경우 호출하는 함수
CString CLogMng::m_fnReturnLogFileName_Stage(BOOL fIsDateFolder, CString strFilePath, BOOL NoUseTotalLog /*= FALSE*/)
{
	CString		strRet, strTemp;

	if(m_strHeaderName.IsEmpty())
	{
		strTemp = m_fnGetDateString();
		if(fIsDateFolder == TRUE)
			strRet = strFilePath + _T("\\") + strTemp + _T(".Log");
		else
			strRet = strFilePath + _T("\\") + _T(".Log");
	}
	else
	{
		strTemp=m_fnGetDateString();
		if(fIsDateFolder == TRUE)
			strRet = strFilePath + _T("\\") + strTemp + _T("\\") + m_strHeaderName + _T("_") + strTemp + _T(".Log");
		else
			strRet = strFilePath + _T("\\") + m_strHeaderName + _T("_") + strTemp + _T(".Log");
	}

	return strRet;
}