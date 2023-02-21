// DriveInfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SpaceManager.h"
#include "FileManagement.h"


// CDriveInfo

IMPLEMENT_DYNAMIC(CFileManagement, CWnd)

BEGIN_MESSAGE_MAP(CFileManagement, CWnd)
//	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CDriveInfo 메시지 처리기입니다.

CFileManagement::CFileManagement()
{
	m_nRatioUsed = 0;
	m_pDeleteThread = NULL;
	m_pFormatThread = NULL;
	m_bExec = TRUE;
	// Thread Event 대기로 변경
	m_hEventDeleteThreadAlive = NULL;
	m_hEventDeleteThreadAlive = CreateEvent( NULL, TRUE,  FALSE,  NULL );
	ResetEvent(m_hEventDeleteThreadAlive);

	m_hEventFormatThreadAlive = NULL;
	m_hEventFormatThreadAlive = CreateEvent( NULL, TRUE,  FALSE,  NULL );
	ResetEvent(m_hEventFormatThreadAlive);

	InitializeCriticalSection(&m_csUpdateDiskStatus);
	InitializeCriticalSection(&m_csGetDiskSpace);	
}

CFileManagement::~CFileManagement()
{
}

// 2015.08.28 Fixed Drive 명 및 용량
std::vector<DriveInfo> CFileManagement::m_fnGetDiskStatus(std::vector<DriveInfo>& vDriveInfo, bool bFormatDrive)
{
	DWORD   dwDiskInfo;
	CString strDrive;	

	TCHAR pTmp[256];
	memset(pTmp, '\0', 256);

	dwDiskInfo = GetLogicalDrives();

	TCHAR  szDrive[MAX_PATH];
	GetLogicalDriveStrings(MAX_PATH, szDrive);

	DriveInfo stDriveInfo;

	for(int i = 0 ; i < 256 ; i++) 
	{
		if(szDrive[i] == '\0' && szDrive[i+1] == '\0')
			break;

		strDrive.Empty();
		while(szDrive[i] != '\0')
			strDrive += szDrive[i++];

		switch(GetDriveType(strDrive)) 
		{
		case DRIVE_FIXED:
			if(bFormatDrive)
			{
				if(strDrive == _T("C:\\") || strDrive == _T("D:\\") || strDrive == _T("E:\\")) // Format drive list에 C,D,E는 표시 안함.
					break;
			}
			else
				if(strDrive == _T("C:\\"))
					break;

			stDriveInfo.strDriveName = strDrive;

			m_fnGetDiskSpace(stDriveInfo);

			vDriveInfo.push_back(stDriveInfo);
			break;
		default :
			break;
		}
	}
	return vDriveInfo;
}

void CFileManagement::m_fnUpdateDiskStatus(std::vector<DriveInfo>& vDriveInfo, const std::vector<CString>& vstrFolder) 
{
	EnterCriticalSection(&m_csUpdateDiskStatus);
	{
		// 각 용량
		for (int nVIndex = 0; nVIndex < (int)vDriveInfo.size(); nVIndex++)
		{
			m_fnGetDiskSpace(vDriveInfo[nVIndex]);
		}

		float nTotalSpace = 0, nTotalRemmainingSpace = 0;

		// 전체 용량
		for (int nDriveIndex = 0; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
		{
			if (vDriveInfo[nDriveIndex].bUseManagement == TRUE)
			{
				nTotalSpace += vDriveInfo[nDriveIndex].nDriveTotalSpace;
				nTotalRemmainingSpace += vDriveInfo[nDriveIndex].nDriveRemainingSpace;
			}
		}
		if (nTotalSpace > 0)
			m_nRatioUsed = 100 - (int)(nTotalRemmainingSpace * 1.0 / nTotalSpace * 100);
		else
			m_nRatioUsed = 0;

		// 드라이브별 최종 기입 시각
		for (int nDriveIndex = 0; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
		{
			if (vDriveInfo[nDriveIndex].bUseManagement == TRUE)
			{
				vDriveInfo[nDriveIndex].timeLastWrite = 0;
				for (int nFolderIndex = 0; nFolderIndex < (int)vstrFolder.size(); nFolderIndex++)
				{
					vDriveInfo[nDriveIndex].timeLastWrite = m_fnGetCreationTime(vDriveInfo[nDriveIndex].timeLastWrite, 
																							vDriveInfo[nDriveIndex].strDriveName + vstrFolder[nFolderIndex]);
				}
			}
		}
	}
	LeaveCriticalSection(&m_csUpdateDiskStatus);
}

void CFileManagement::m_fnUpdateDiskStatus(std::vector<DriveInfo>& vDriveInfo)
{
	EnterCriticalSection(&m_csUpdateDiskStatus);
	{
		// 각 용량
		for (int nVIndex = 0; nVIndex < (int)vDriveInfo.size(); nVIndex++)
		{
			m_fnGetDiskSpace(vDriveInfo[nVIndex]);
		}

		float nTotalSpace = 0, nTotalRemmainingSpace = 0, RemainRatio = 0;

		// 전체 용량
		for (int nDriveIndex = 0; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
		{
			if (vDriveInfo[nDriveIndex].bUseManagement == TRUE)
			{
// 				nTotalSpace += vDriveInfo[nDriveIndex].nDriveTotalSpace;
// 				nTotalRemmainingSpace += vDriveInfo[nDriveIndex].nDriveRemainingSpace;
				RemainRatio = 100 -  (float)(vDriveInfo[nDriveIndex].nDriveRemainingSpace * 1.0 / vDriveInfo[nDriveIndex].nDriveTotalSpace * 100);
				if( RemainRatio > GetFormatMaxSpace())
					vDriveInfo[nDriveIndex].nFormatFlag = TRUE;
				else 
					vDriveInfo[nDriveIndex].nFormatFlag = FALSE;
			}
		}
// 		if (nTotalSpace > 0)
// 			remain=  100 - (float)(nTotalRemmainingSpace * 1.0 / nTotalSpace * 100);
// 			//m_nRatioUsed = 100 - (float)(nTotalRemmainingSpace * 1.0 / nTotalSpace * 100);
// 		else
// 			m_nRatioUsed = 0;

		// 드라이브별 최종 기입 시각
		for (int nDriveIndex = 0; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
		{
			if (vDriveInfo[nDriveIndex].bUseManagement == TRUE)
			{
				vDriveInfo[nDriveIndex].timeLastWrite = 0;
				vDriveInfo[nDriveIndex].timeLastWrite = m_fnGetCreationTime(vDriveInfo[nDriveIndex].timeLastWrite,
																			vDriveInfo[nDriveIndex].strDriveName + GetTimeCheckPath());
			}
		}
	}
	LeaveCriticalSection(&m_csUpdateDiskStatus);
}

void CFileManagement::m_fnGetDiskSpace(DriveInfo& stDriveInfo)
{
	EnterCriticalSection(&m_csGetDiskSpace);
	{
		ULARGE_INTEGER  lFreeBytesAvailable;
		ULARGE_INTEGER  lTotalBytes;
		ULARGE_INTEGER  lTotalFree;

		GetDiskFreeSpaceEx(stDriveInfo.strDriveName, &lFreeBytesAvailable, &lTotalBytes, &lTotalFree);
		stDriveInfo.nDriveTotalSpace = m_fnGetByte(lTotalBytes);
		stDriveInfo.nDriveRemainingSpace = m_fnGetByte(lTotalFree);
	}
	LeaveCriticalSection(&m_csGetDiskSpace);
}

void CFileManagement::m_fnApplyDeleteRules(std::vector<DriveInfo>& vDriveInfo, const std::vector<CString>& vstrFolder, int nTimeLate, int nMaxSpace, int nMinSpace, int nStepTimeLate, int nDeleteStep, BOOL bFindSubFolder, int nDeleteSleep, int nExTimeLate, const std::vector<CString>& vstrExFolder)
{
	if(m_pDeleteThread != NULL) 
	{
		// Thread Event 대기로 변경
		m_bExec = FALSE;
		theApp.m_fnWriteLog(_T("======= Wait Delete Ex-Thread Complete ======="));
		::WaitForSingleObject(m_hEventDeleteThreadAlive, INFINITE);
		ResetEvent(m_hEventDeleteThreadAlive);
		m_pDeleteThread = NULL;
	} 

	m_bExec = TRUE;

	DELETETHREADPARAMS *pThreadParams = new DELETETHREADPARAMS;

	pThreadParams->pThis = this;
	pThreadParams->vDriveInfo = vDriveInfo;
	pThreadParams->vstrFolder = vstrFolder;
	pThreadParams->nTimeLate = nTimeLate;
	pThreadParams->nMaxSpace = nMaxSpace;
	pThreadParams->nMinSpace = nMinSpace;
	pThreadParams->nDeleteStep = nDeleteStep;
	pThreadParams->bFindSubFolder = bFindSubFolder;
	pThreadParams->nStepTimeLate = nStepTimeLate;
	pThreadParams->nDeleteSleep = nDeleteSleep;
	pThreadParams->nExTimeLate = nExTimeLate;
	pThreadParams->vstrExFolder = vstrExFolder;

	theApp.m_fnWriteLog(_T("======= Delete Thread Start ======="));

	// 2015.10.12 Thread Priority 변경 - Normal -> Below_Normal
	m_pDeleteThread = AfxBeginThread(ThreadApplyDeleteRules, pThreadParams, THREAD_PRIORITY_BELOW_NORMAL);
}

void CFileManagement::m_fnApplyFormatRules(std::vector<DriveInfo>& vDriveInfo, int nMaxSpace)
{
 	if(m_pFormatThread != NULL) 
 	{
 		// Thread Event 대기로 변경
 		m_bExec = FALSE;
 		theApp.m_fnWriteLog(_T("======= Wait Format Ex-Thread Complete ======="));
 		::WaitForSingleObject(m_hEventFormatThreadAlive, INFINITE);
 		ResetEvent(m_hEventFormatThreadAlive);
 		m_pFormatThread = NULL;
 	} 
 
 	m_bExec = TRUE;
 
 	FORMATTHREADPARAMS *pThreadParams = new FORMATTHREADPARAMS;
	pThreadParams->pThis = this;
	pThreadParams->vDriveInfo = vDriveInfo;
	pThreadParams->nMaxSpace = nMaxSpace;


 	theApp.m_fnWriteLog(_T("======= Format Thread Start ======="));

 	m_pFormatThread = AfxBeginThread(ThreadApplyFormatRules, pThreadParams, THREAD_PRIORITY_BELOW_NORMAL);
}

float CFileManagement::m_fnGetByte(ULARGE_INTEGER dwParam)
{
	const int ByteVariable = 1024;

	return ((float)dwParam.u.HighPart *4 + ((float)dwParam.u.LowPart/(ByteVariable*ByteVariable*ByteVariable)));
}

// 파일 및 폴더 삭제 재귀 함수
BOOL CFileManagement::m_fnDeletePreviouslyDataRefTime(CString strFolder, int nHour, BOOL bFindSubFolder, int nDeleteSleep, BOOL bRecurse)
{
	BOOL bRet = FALSE;
	CFileFind finder;
	
	CString strWildcard = strFolder + _T("\\*.*");

	BOOL bWorking = finder.FindFile(strWildcard);

	CTime tmCurTime = CTime::GetCurrentTime();

	while (bWorking && m_bExec)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		CString strPath = finder.GetFilePath();
		CTime tmCreatedTIme;
		finder.GetCreationTime(tmCreatedTIme);
		//finder.GetLastWriteTime(tmLastWriteTime);

		if(finder.IsDirectory())
		{
			if (bFindSubFolder)
			{
				bRet = m_fnDeletePreviouslyDataRefTime(finder.GetFilePath(), nHour, bFindSubFolder, nDeleteSleep, TRUE);
			}
			//else
			//{
				//if (!bRecurse)
				//{
				//	bRet = m_fnDeletePreviouslyDataRefTime(finder.GetFilePath(), nHour, bFindSubFolder, nDeleteSleep, TRUE);
				//}
			//}
		}
		else						
		{
			if (tmCurTime - tmCreatedTIme > nHour * 60 * 60)
			{
				if (!finder.IsSystem())
				{
					//SetFileAttributes(finder.GetFilePath(), FILE_ATTRIBUTE_NORMAL);	// 읽기 전용 삭제
					bRet = ::DeleteFile(finder.GetFilePath());
					if (!bRet)
						theApp.m_fnWriteLog(_T("%s File Delete Error (Code : %d)"), finder.GetFilePath(), ::GetLastError());
					Sleep(nDeleteSleep);
					bRet = TRUE;
				}				
			}
		}
	}
	finder.Close();
	if (bRecurse)
		RemoveDirectory(strFolder);

	return bRet;
}

// 드라이브 모니터링 위치 최종 기입 날짜 반환
CTime CFileManagement::m_fnGetCreationTime(CTime tmLastWriteTime, CString strFolderPath)
{
	CFileFind finder;

	BOOL bWorking = finder.FindFile(strFolderPath + _T("\\*.*"));

	CTime tmCreateTime;
	CTime tmResult;

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// Delete 도 Write 로 취급하여 지운 폴더 수정 날짜 갱신하여 불합리 있음
		// 최종 생성일 기준
		if (finder.IsDots())
		{
			//finder.GetLastWriteTime(tmLastWriteTime);
			//break;
			continue;			
		}

		// 생성 날짜
		finder.GetCreationTime(tmCreateTime);
		if (tmLastWriteTime < tmCreateTime)
			tmLastWriteTime = tmCreateTime;
	}
	finder.Close();
	return tmLastWriteTime;
}

CTime CFileManagement::m_fnGetLastWriteTime(CTime tmLastWriteTime, CString strFolderPath)
{
	CFileFind finder;

	BOOL bWorking = finder.FindFile(strFolderPath + _T("\\*.*"));

	CTime tmCreateTime;

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// Delete 도 Write 로 취급하여 지운 폴더 수정 날짜 갱신하여 불합리 있음
		// 최종 생성일 기준
		if (finder.IsDots())
		{
			//finder.GetLastWriteTime(tmLastWriteTime);
			//break;
			continue;			
		}
		// 생성 날짜

		finder.GetLastWriteTime(tmCreateTime);
		if (tmLastWriteTime < tmCreateTime)
			tmLastWriteTime = tmCreateTime;
	}
	finder.Close();
	return tmLastWriteTime;
}

// 이미 삭제한 드라이브 제외한 가장 오래된 드라이브 반환
int CFileManagement::m_fnGetOldestDriveIndex(std::vector<DriveInfo>& vDriveInfo)
{	
 	int nOldestDriveIndex = errSpaceSetting;
 
 	// 기준 드라이브 인덱스
 	for (int nDriveIndex = 0; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
 	{
 		if (vDriveInfo[nDriveIndex].bUseManagement == TRUE
 			&& vDriveInfo[nDriveIndex].bDeleteDoneFlag == FALSE)
 		{
 			nOldestDriveIndex = nDriveIndex;
 			break;
 		}
 	}
 
 	// 삭제할 드라이브 없을 경우
 	if (nOldestDriveIndex == errSpaceSetting)
 		return errSpaceSetting;
 
 	for (int nDriveIndex = nOldestDriveIndex + 1; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
 	{
 		if (vDriveInfo[nDriveIndex].bUseManagement == TRUE
 			&& vDriveInfo[nDriveIndex].bDeleteDoneFlag == FALSE
 			&& vDriveInfo[nDriveIndex].timeLastWrite < vDriveInfo[nOldestDriveIndex].timeLastWrite
 			&& vDriveInfo[nDriveIndex].timeLastWrite != 0)
 			nOldestDriveIndex = nDriveIndex;
 	}
 	return nOldestDriveIndex;
}
void CFileManagement::m_fnClearDeleteDoneFlag(std::vector<DriveInfo>& vDriveInfo)
{
	for (int nIndex = 0; nIndex < (int)vDriveInfo.size(); nIndex++)
	{
		vDriveInfo[nIndex].bDeleteDoneFlag = FALSE;
	}
}

void CFileManagement::m_fnApplyTimeLimitRule(std::vector<DriveInfo>& vDriveInfo, const std::vector<CString>& vstrFolder, int nTimeLate, BOOL bFindSubFolder, int nDeleteSleep)
{
	BOOL bRet = FALSE;

	theApp.m_fnWriteLog(_T(">> Time Limit Delete - Start"));

	for (int nDriveIndex = 0; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
	{
		if (vDriveInfo[nDriveIndex].bUseManagement == TRUE)
		{
			for (int nFolderIndex = 0; nFolderIndex < (int)vstrFolder.size(); nFolderIndex++)
			{			
				bRet = m_fnDeletePreviouslyDataRefTime(vDriveInfo[nDriveIndex].strDriveName + vstrFolder[nFolderIndex], nTimeLate, bFindSubFolder, nDeleteSleep);
			}
		}
	}

	theApp.m_fnWriteLog(_T("<< Time Limit Delete - End"));
}
//2016.04.08 add by mby 
void CFileManagement::m_fnApplyExTimeLimitRule(std::vector<DriveInfo>& vDriveInfo, const std::vector<CString>& vstrExFolder, int nExTimeLate, BOOL bFindSubFolder, int nDeleteSleep)
{
	BOOL bRet = FALSE;

	theApp.m_fnWriteLog(_T(">> ExTime Limit Delete - Start"));

	for (int nDriveIndex = 0; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
	{
		if (vDriveInfo[nDriveIndex].bUseManagement == TRUE)
		{
			for (int nFolderIndex = 0; nFolderIndex < (int)vstrExFolder.size(); nFolderIndex++)
			{			
				bRet = m_fnDeletePreviouslyDataRefTime(vDriveInfo[nDriveIndex].strDriveName + vstrExFolder[nFolderIndex], nExTimeLate, bFindSubFolder, nDeleteSleep);
			}
		}
	}

	theApp.m_fnWriteLog(_T("<< ExTime Limit Delete - End"));
}


BOOL CFileManagement::m_fnApplySpaceLimitRule(std::vector<DriveInfo>& vDriveInfo, const std::vector<CString>& vstrFolder, int nStepTimeLate, BOOL bFindSubFolder, int nDeleteSleep, int nMaxSpace, int nMinSpace, int nDeleteStep)
{
	BOOL bRet = FALSE;
	
	// 2015.10.12 로그 추가
	theApp.m_fnWriteLog(_T("== UsedSpace : %d, MaxSpace : %d =="), m_nRatioUsed, nMaxSpace);

	if (m_nRatioUsed > nMaxSpace)
	{
		theApp.m_fnWriteLog(_T(">> Space Limit Delete - Start"));
		int nOldestDriveIndex = m_fnGetOldestDriveIndex(vDriveInfo);
		if (nOldestDriveIndex == errSpaceSetting)
		{
			theApp.m_fnWriteLog(_T("<< Space Limit Delete - Abnormal End (Drive Setting)"), m_nRatioUsed, nMinSpace);
			return FALSE;
		}

		int nChangeTimeLate;
		int nCount = 0;

		do
		{
			nCount++;
			// 모니터링하는 모든 폴더
			for (int nFolderIndex = 0; nFolderIndex < (int)vstrFolder.size(); nFolderIndex++)
			{
				nChangeTimeLate = nStepTimeLate - (nDeleteStep * nCount);
				if (nChangeTimeLate < MINHOUR)	// 최소 1시간 이전 파일은 남겨둠
					nChangeTimeLate = MINHOUR;
				// 기간 변경 후 삭제 실행
				bRet = m_fnDeletePreviouslyDataRefTime(vDriveInfo[nOldestDriveIndex].strDriveName + vstrFolder[nFolderIndex], nChangeTimeLate, bFindSubFolder, nDeleteSleep);								
			}			
			// 현재 드라이브 모든 파일 삭제했을 경우
			if (/*bRet == FALSE && */nChangeTimeLate == MINHOUR)
			{				
				vDriveInfo[nOldestDriveIndex].bDeleteDoneFlag = TRUE;
				theApp.m_fnWriteLog(_T("%s Complete !!"), vDriveInfo[nOldestDriveIndex].strDriveName);
				nOldestDriveIndex = m_fnGetOldestDriveIndex(vDriveInfo);

				// 모든 파일 삭제했으나, MinSpace 조건 충족하지 못했을 경우
				if (nOldestDriveIndex == errSpaceSetting)
				{
					m_fnClearDeleteDoneFlag(vDriveInfo);
					m_fnUpdateDiskStatus(vDriveInfo, vstrFolder);
					if (m_nRatioUsed > nMinSpace)
						theApp.m_fnWriteLog(_T("<< Space Limit Delete - Abnormal End (UsedSpace : %d, MinSpace : %d)"), m_nRatioUsed, nMinSpace);
					return FALSE;
				}
				nCount = 0;
			}
			m_fnUpdateDiskStatus(vDriveInfo, vstrFolder);
		} while (m_nRatioUsed > nMinSpace && m_bExec);
		m_fnClearDeleteDoneFlag(vDriveInfo);
		theApp.m_fnWriteLog(_T("<< Space Limit Delete - End (UsedSpace : %d, MinSpace : %d)"), m_nRatioUsed, nMinSpace);
	}
	return TRUE;
}


UINT CFileManagement::ThreadApplyDeleteRules(LPVOID pParam)
{
	BOOL bRet = FALSE;
	DELETETHREADPARAMS *pThreadParams = (DELETETHREADPARAMS *)pParam;

	CFileManagement*		pThis			= (CFileManagement*)pThreadParams->pThis;
	std::vector<DriveInfo> vDriveInfo		= pThreadParams->vDriveInfo;
	std::vector<CString>	vstrFolder		= pThreadParams->vstrFolder;
	int						nTimeLate		= pThreadParams->nTimeLate;
	int						nMaxSpace		= pThreadParams->nMaxSpace;
	int						nMinSpace		= pThreadParams->nMinSpace;
	int						nDeleteStep		= pThreadParams->nDeleteStep;
	BOOL					bFindSubFolder	= pThreadParams->bFindSubFolder;
	int						nStepTimeLate	= pThreadParams->nStepTimeLate;
	int						nDeleteSleep	= pThreadParams->nDeleteSleep;
	int						nExTimeLate		= pThreadParams->nExTimeLate;
	std::vector<CString>	vstrExFolder	= pThreadParams->vstrExFolder;

	delete pThreadParams;	

	//삭제경로가 있을경우에만 실행 
	if(vstrFolder.size() != 0){
		pThis->
			// 기간 - 모든 드라이브 / 폴더 검색 후 삭제
			m_fnApplyTimeLimitRule(vDriveInfo, vstrFolder, nTimeLate, bFindSubFolder, nDeleteSleep);
	}		
	
	//삭제경로가 있을경우에만 실행 
	if(vstrExFolder.size() != 0){
		pThis->
			// 단일 조건 기간 - 모든 드라이브 / 폴더 검색 후 삭제
			m_fnApplyExTimeLimitRule(vDriveInfo, vstrExFolder, nExTimeLate, bFindSubFolder, nDeleteSleep);
	}
		
	pThis->
		// 정보 갱신
		m_fnUpdateDiskStatus(pThis->m_stDriveInfo, vstrFolder);	

	pThis->
		// 용량 - 가장 오래된 드라이브부터 삭제 / 설정값만큼 기간 조절하면서
		m_fnApplySpaceLimitRule(vDriveInfo, vstrFolder, nStepTimeLate, bFindSubFolder, nDeleteSleep, nMaxSpace, nMinSpace, nDeleteStep);		

	// Thread Event 대기로 변경
	SetEvent(pThis->m_hEventDeleteThreadAlive);

	theApp.m_fnWriteLog(_T("======= Delete Thread End ======="));

	return 0;
}

UINT CFileManagement::ThreadApplyFormatRules(LPVOID pParam)
{
	BOOL bRet = FALSE;
	FORMATTHREADPARAMS *pThreadParams = (FORMATTHREADPARAMS *)pParam;

	CFileManagement*		pThis			= (CFileManagement*)pThreadParams->pThis;
	std::vector<DriveInfo> &vDriveInfo		= pThreadParams->vDriveInfo;
	int						nMaxSpace		= pThreadParams->nMaxSpace;

	WCHAR wcDrive[8];
	CString str;
	memset( wcDrive, 0x00, sizeof(wcDrive) );
	CString strDrive;

	CTime tMintime;
	int nOldestDriveIndex =0;
	CTime timeLastWrite = 0;

	// 모든 하드가 가득 차지 않았다면 return
	int nUseCount = 0;
	for (int nDriveIndex = 0 ; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
	{
		if (vDriveInfo[nDriveIndex].bUseManagement == TRUE && vDriveInfo[nDriveIndex].nFormatFlag == FALSE) 
		{
			SetEvent(pThis->m_hEventFormatThreadAlive);
			return TRUE;
		}
		else if(vDriveInfo[nDriveIndex].bUseManagement == FALSE)
			nUseCount++;
	}
	if(nUseCount == (int)vDriveInfo.size())
	{
		SetEvent(pThis->m_hEventFormatThreadAlive); // 모든 드라이브 사용 안할때 리턴
		return TRUE;
	}
	////////////////////////가장 오래된 드라이브 검색
	for (int nDriveIndex = 0 ; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
	{
		if (vDriveInfo[nDriveIndex].bUseManagement == TRUE)
		{
			nOldestDriveIndex = nDriveIndex;
			timeLastWrite = vDriveInfo[nOldestDriveIndex].timeLastWrite;
			break;
		}
	}

	for (int nDriveIndex = nOldestDriveIndex ; nDriveIndex < (int)vDriveInfo.size(); nDriveIndex++)
	{
		if (vDriveInfo[nDriveIndex].bUseManagement == TRUE
			&& timeLastWrite > vDriveInfo[nDriveIndex].timeLastWrite && vDriveInfo[nDriveIndex].timeLastWrite != 0)
			{	
				nOldestDriveIndex = nDriveIndex;
			}
	}
	////////////////////////가장 오래된 드라이브 검색


	 int nSpaceRatio = 100 - (int)( vDriveInfo[nOldestDriveIndex].nDriveRemainingSpace * 1.0 / vDriveInfo[nOldestDriveIndex].nDriveTotalSpace * 100 );
 	if( vDriveInfo[nOldestDriveIndex].bUseManagement && nSpaceRatio > nMaxSpace )
 	{

		strDrive = vDriveInfo[nOldestDriveIndex].strDriveName;
 		if( (strDrive == _T("C:\\")) || (strDrive == _T("D:\\")) || (strDrive == _T("E:\\")) ) // 최종 드라이브가 C,D,E일 경우 return
 		{
 			theApp.m_fnWriteLog(_T("Critical Error - ThreadFormatDrive Return - Drive : %s"), strDrive);
			SetEvent(pThis->m_hEventFormatThreadAlive);
 			return FALSE;
 		}
		int RETRYCOUNT = 10;
		int RETRYINTERVAL = 100;
		int nRetryFormatCnt = 0;		
		do{

			theApp.m_fnWriteLog(_T("Format Target Drive - %s"),strDrive);

			strDrive = strDrive.Left(2);
			pThis->FormatDrive(strDrive);

			ULARGE_INTEGER  lFreeBytesAvailable;
			ULARGE_INTEGER  lTotalBytes;
			ULARGE_INTEGER  lTotalFree;
			strDrive += _T("\\");
			GetDiskFreeSpaceEx(strDrive, &lFreeBytesAvailable, &lTotalBytes, &lTotalFree);

			if( pThis->m_fnGetByte(lTotalBytes) - pThis->m_fnGetByte(lTotalFree) < 1 )
			{
				vDriveInfo[nOldestDriveIndex].nFormatFlag = FALSE;
				theApp.m_fnWriteLog(_T("Format Complete - %s"),strDrive);
				break;
			}
			else
			{
				theApp.m_fnWriteLog(_T("ThreadFormatDrive format fail "));
				Sleep(RETRYINTERVAL);	
				theApp.m_fnWriteLog(_T("Retry FormatDrive count : (%d)"), nRetryFormatCnt);							
				nRetryFormatCnt++;
			}
		} while (nRetryFormatCnt < RETRYCOUNT);

	}
	// Thread Event 대기로 변경
	theApp.m_fnWriteLog(_T("======= Delete Thread End ======="));
	SetEvent(pThis->m_hEventFormatThreadAlive);
	return 0;
}
void CFileManagement::m_fnSetDriveUseFlag(int nVIndex, BOOL bUseFlag)
{
	//m_stDriveInfo[nVIndex].bUseManagement = bUseFlag;
} 

BOOL CFileManagement::FormatDrive(CString strDrive)
{
	SECURITY_ATTRIBUTES secattr; 
	ZeroMemory(&secattr,sizeof(secattr));
	secattr.nLength = sizeof(secattr);
	secattr.bInheritHandle = TRUE;

	HANDLE rPipe, wPipe;

	//Create pipes to write and read data
	CreatePipe(&rPipe,&wPipe,&secattr,0);

	STARTUPINFO sInfo; 
	ZeroMemory(&sInfo,sizeof(sInfo));
	PROCESS_INFORMATION pInfo; 
	ZeroMemory(&pInfo,sizeof(pInfo));
	sInfo.cb=sizeof(sInfo);
	sInfo.dwFlags=STARTF_USESTDHANDLES;
	sInfo.hStdInput=NULL; 
	sInfo.hStdOutput=wPipe; 
	sInfo.hStdError=wPipe;
	
	CString strExecute = _T("");
	strExecute = _T("/k format ") + strDrive + _T(" /q /y");

	CreateProcess(_T("C:\\Windows\\System32\\cmd.exe"), (LPWSTR)(LPCWSTR)strExecute, 0,0,TRUE, NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW,0,0,&sInfo,&pInfo);
	CloseHandle(wPipe);

	//now read the output pipe here.
// 	char buf[1000];
// 	DWORD reDword; 
// 	CString m_csOutput,csTemp;
// 	BOOL res;
// 	do
// 	{
// 		res=::ReadFile(rPipe,buf,1000,&reDword,0);
// 		csTemp=buf;
// 		m_csOutput+=csTemp.Left(reDword);
// 	}while(res);
	return TRUE;
}