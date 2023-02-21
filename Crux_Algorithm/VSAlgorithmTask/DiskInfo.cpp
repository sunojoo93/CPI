#include "StdAfx.h"
#include "DiskInfo.h"
#include "VSAlgorithmTask.h"




CDiskInfo::CDiskInfo(void)
{
}


CDiskInfo::~CDiskInfo(void)
{
}

void CDiskInfo::CreateDriveInfoVector()
{  
	DWORD   dwDiskInfo;
	int		nCount = 0;
	CString strDrive;	
	BOOL	bExistRamDrive = FALSE;		// Ram Drive 존재 여부 확인을 위함
	char pTmp[256];      // local drive 이름의 나열 저장
	memset(pTmp, '\0', 256);
	//CString	ExDrive1 = _T("C:\\");
	//CString	ExDrive2 = _T("D:\\");
	//CString	ExDrive3 = _T("E:\\");

	dwDiskInfo = GetLogicalDrives();

	tDriveInfoParam tTempDriveInfo;


	wchar_t szDrive[MAX_PATH] = {0, };
	GetLogicalDriveStrings(MAX_PATH, szDrive);
	// szDrive를 파싱하여 드라이브 목록을 얻어 올 수 있다.
	for(int i=0 ; i<256 ; i++) 
	{
		if(szDrive[i] == '\0' && szDrive[i+1] == '\0')
			break;

		strDrive.Empty();
		while(szDrive[i] != '\0')
			strDrive += szDrive[i++];

		switch(GetDriveType(strDrive)) 
		{
		case DRIVE_CDROM:
			break;
		case DRIVE_REMOVABLE:
			break;
		case DRIVE_FIXED:
 			tTempDriveInfo.Drive_Name = strDrive;
			tTempDriveInfo.nDriveLimitSize = theApp.m_Config.GetDriveLimitSize();
			m_tDriveInfoParam.push_back(tTempDriveInfo);
			nCount++;			
			break;
		default :
			break;
		}
	}
}

void CDiskInfo::RenewalDiskInfo()
{
	float  TolalSize, TotalFree;
	float  dPrecent = 0.0;
	CString	Drive_name = _T("");

	ULARGE_INTEGER  lTotalBytes;
	ULARGE_INTEGER  lTotalFree;

	for(int i = 0 ; i < m_tDriveInfoParam.size() ; i++)
	{
		if( GetDriveType( m_tDriveInfoParam[i].Drive_Name ) == DRIVE_FIXED ) // 고정 드라이브만 계산
		{
			GetDiskFreeSpaceEx(m_tDriveInfoParam[i].Drive_Name , &m_lFreeBytesAvailable, &lTotalBytes, &lTotalFree);

			TolalSize = ((float)lTotalBytes.u.HighPart *4 + ((float)lTotalBytes.u.LowPart/(1024*1024*1024)));
			TotalFree = ((float)lTotalFree.u.HighPart *4 + ((float)lTotalFree.u.LowPart/(1024*1024*1024)));

			dPrecent = (float)(TotalFree * 1.0 / TolalSize * 100);
			m_tDriveInfoParam[i].Free_PerCent = dPrecent;

	
		}
	}

}

CString CDiskInfo::GetAvailableDrivePath(CString strCurrentDrive)
{	
	int	n_dCheck = 0;	
	// 2015.09.01 드라이브별 용량 Limit 기능 추가 및 전체 Sequence 수정	
	int nLastDriveIndex = 0;
	// 현재 사용 중인 드라이브 최대 용량까지 사용
	for (int nDriveIndex=0 ; nDriveIndex< m_tDriveInfoParam.size(); nDriveIndex++)
	{
		if ( m_tDriveInfoParam[nDriveIndex].Drive_Name == strCurrentDrive)
		{			
			if(m_tDriveInfoParam[nDriveIndex].bUseFlag == true && m_tDriveInfoParam[nDriveIndex].nDriveLimitSize > 100 - m_tDriveInfoParam[nDriveIndex].Free_PerCent)
				return strCurrentDrive;
			nLastDriveIndex = nDriveIndex;
		}
	}

	// 현재 사용 중 드라이브를 제외하고 순차적으로 다음 드라이브 검색
	//for (int i=0 ; i< DRIVE_PATH_COUNT ; i++)
	for (int i=0 ; i< m_tDriveInfoParam.size() - 1; i++)
	{
		// 드라이브 순차 접근
		if (++nLastDriveIndex > m_tDriveInfoParam.size() -1)	nLastDriveIndex = 0;

		if(m_tDriveInfoParam[nLastDriveIndex].bUseFlag == true && m_tDriveInfoParam[nLastDriveIndex].nDriveLimitSize > 100 - m_tDriveInfoParam[nLastDriveIndex].Free_PerCent)
		{
			n_dCheck = 1;
			break;	
		}
	}
	//Default
	if(n_dCheck == 0)
	{
		AfxMessageBox(_T("Please Check Free Size on Hard Disk!!!"));
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Please Check Free Size on Hard Disk!!!"));
	}
	return m_tDriveInfoParam[nLastDriveIndex].Drive_Name;
}

void CDiskInfo::SetUseDisk(CString strDrive)
{
	CString strTemp;
	strTemp.Format(_T("%s:\\"),strDrive);
	//strDrive = strDrive + _T("\:\\");
	for (int nDriveIndex=0 ; nDriveIndex< m_tDriveInfoParam.size(); nDriveIndex++)
	{
		if ( m_tDriveInfoParam[nDriveIndex].Drive_Name == strTemp )
			m_tDriveInfoParam[nDriveIndex].bUseFlag = true;
	}
}