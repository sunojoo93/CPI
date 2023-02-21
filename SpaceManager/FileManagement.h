#pragma once

#include <vector>

// CFileManagement

#define errSpaceSetting -1
#define MINHOUR			0	// 파일 최소 보존 시간
#define MAX_PATH_NUM	10

#define FMIFM_FLOPPY 0x08
#define FMIFM_HARDDISK 0x0C

typedef struct _DriveInfo
{
	BOOL	bUseManagement;
	CString	strDriveName;
	float	nDriveTotalSpace;
	float	nDriveRemainingSpace;
	CTime	timeLastWrite;
	BOOL	bDeleteDoneFlag;
	BOOL	nFormatFlag;
	_DriveInfo()
	{
		bUseManagement = FALSE;
		strDriveName = _T("");
		nDriveTotalSpace = 0;
		nDriveRemainingSpace = 0;
		timeLastWrite = 0;
		bDeleteDoneFlag = FALSE;
		nFormatFlag = FALSE;
	}
}DriveInfo;

typedef enum
{
	PROGRESS,
	DONEWITHSTRUCTURE,
	UNKNOWN2,
	UNKNOWN3,
	UNKNOWN4,
	UNKNOWN5,
	INSUFFICIENTRIGHTS,
	FSNOTSUPPORTED,
	VOLUMEINUSE,
	UNKNOWN9,
	UNKNOWNA,
	DONE,
	UNKNOWNC,
	UNKNOWND,
	OUTPUT,
	STRUCTUREPROGRESS,
	CLUSTERSIZETOOSMALL,
} CALLBACKCOMMAND;


class CFileManagement	: public CWnd
{
	DECLARE_DYNAMIC(CFileManagement)

	typedef struct tagDELETETHREADPARAMS {
		CFileManagement*		pThis;
		std::vector<DriveInfo> vDriveInfo;
		std::vector<CString>	vstrFolder;
		int						nTimeLate;
		int						nMaxSpace;
		int						nMinSpace;
		int						nDeleteStep;
		BOOL					bFindSubFolder;
		int						nStepTimeLate;
		int						nDeleteSleep;
		int						nExTimeLate;
		std::vector<CString>	vstrExFolder;
		
	} DELETETHREADPARAMS;

	typedef struct tagFORMATTHREADPARAMS {
		CFileManagement*		pThis;
		std::vector<DriveInfo> vDriveInfo;
		int					   nMaxSpace;

	} FORMATTHREADPARAMS;

public:
	CFileManagement();
	virtual ~CFileManagement();	

	std::vector<DriveInfo>		m_fnGetDiskStatus(std::vector<DriveInfo>& vDriveInfo, bool bFormatDrive);
	void						m_fnUpdateDiskStatus(std::vector<DriveInfo>& vDriveInfo, const std::vector<CString>& strFolder); // 폴더 모니터링
	void						m_fnUpdateDiskStatus(std::vector<DriveInfo>& vDriveInfo); // 포맷드라이브 모니터링
	void						m_fnGetDiskSpace(DriveInfo& stDriveInfo);
	void						m_fnApplyDeleteRules(std::vector<DriveInfo>& vDriveInfo, const std::vector<CString>& strFolder, int nTimeLate, int nMaxSpace, int nMinSpace, int nStepTimeLate, int nDeleteStep, BOOL bFindSubFolder, int nDeleteSleep, int nExTimeLate, const std::vector<CString>& strExFolder);
	void						m_fnApplyFormatRules(std::vector<DriveInfo>& vDriveInfo, int nMaxSpace);

	BOOL						m_fnDeletePreviouslyDataRefTime(CString strFolder, int nHour, BOOL bFindSubFolder, int nDeleteSleep, BOOL bRecurse = FALSE);
	//void						m_fnRemoveDirectoryFiles(CString strFolder, int nHour, BOOL bFindSubFolder);
	CTime						m_fnGetCreationTime(CTime tmLastWriteTime, CString strSampleFolder);
	CTime						m_fnGetLastWriteTime(CTime tmLastWriteTime, CString strSampleFolder);
	int							m_fnGetOldestDriveIndex(std::vector<DriveInfo>& vDriveInfo);
	float						m_fnGetByte(ULARGE_INTEGER dwParam);
	void						m_fnClearDeleteDoneFlag(std::vector<DriveInfo>& vDriveInfo);
	void						m_fnApplyTimeLimitRule(std::vector<DriveInfo>& vDriveInfo, const std::vector<CString>& vstrFolder, int nTimeLate, BOOL bFindSubFolder, int nDeleteSleep);
	void						m_fnApplyExTimeLimitRule(std::vector<DriveInfo>& vDriveInfo, const std::vector<CString>& vstrExFolder, int nExTimeLate, BOOL bFindSubFolder, int nDeleteSleep);	//2016.04.08 add by mby 
	BOOL						m_fnApplySpaceLimitRule(std::vector<DriveInfo>& vDriveInfo, const std::vector<CString>& vstrFolder, int nStepTimeLate, BOOL bFindSubFolder, int nDeleteSleep, int nMaxSpace, int nMinSpace, int nDeleteStep);

	static UINT					ThreadApplyDeleteRules(LPVOID pParam);
	static UINT					ThreadApplyFormatRules(LPVOID pParam);
	void						m_fnSetDriveUseFlag(int nVIndex, BOOL bUseFlag);


	void						SetFormatMaxSpace(int nSpace) {m_nFormatMaxSpace = nSpace;};
	int							GetFormatMaxSpace()				{return m_nFormatMaxSpace;};

	void						SetTimeCheckPath(CString strPath) {m_strTimeCheckPath = strPath;};
	CString						GetTimeCheckPath()					{return m_strTimeCheckPath;};

	void						SetFormatCompleteFlag(std::vector<DriveInfo>& vDriveInfo,int nIndex, BOOL nFlag) { vDriveInfo[nIndex].nFormatFlag = nFlag; };
	BOOL						FormatDrive(CString strDrive);

	int							m_nRatioUsed;
	CString						m_strCurUseDriveName;	

	CWinThread*					m_pDeleteThread;
	CWinThread*					m_pFormatThread;
	BOOL						m_bExec;

	HANDLE						m_hEventDeleteThreadAlive;	// Thread Event 대기 방식으로 변경
	HANDLE						m_hEventFormatThreadAlive;	// Thread Event 대기 방식으로 변경

	

private:
	std::vector<DriveInfo>		m_stDriveInfo;
	CRITICAL_SECTION			m_csUpdateDiskStatus;
	CRITICAL_SECTION			m_csGetDiskSpace;	
	int							m_nFormatMaxSpace;
	CString						m_strTimeCheckPath;

protected:
	DECLARE_MESSAGE_MAP()

	__inline char* CSTR2PCH(CString strInput)
	{
		char * tmpch;
		int sLen = WideCharToMultiByte(CP_ACP, 0, strInput, -1, NULL, 0, NULL, NULL);       
		tmpch = new char[sLen + 1];
		WideCharToMultiByte(CP_ACP, 0, strInput, -1, tmpch, sLen, NULL, NULL);
		return tmpch;
	}
};