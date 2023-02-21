// FtpClient.h: interface for the CFtpClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTPCLIENT_H__F196D430_806C_4A00_B5BE_04AC559B59A2__INCLUDED_)
#define AFX_FTPCLIENT_H__F196D430_806C_4A00_B5BE_04AC559B59A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxinet.h>	//ftp 를 위해 추가해야됨
#include "LogWriter.h"
#include "LogMng\FileMng.h"


class CFTPFileFind  
{
public:

	int GetErrorMessage();
	CString GetFileName();
	BOOL m_fnRemoteClose();	
	BOOL FindNextFile();
	BOOL FindFile(LPCTSTR RemoteFile = NULL, DWORD dwFlags = INTERNET_FLAG_RELOAD);
	//}}AFX_FindFile

	CFTPFileFind(CFtpConnection* pConnection);
	virtual ~CFTPFileFind();

	CFtpFileFind		*m_pRemoteFindFile;
	CString				m_strAddress;
};

class CFtpClient :private CLogWriterCruxServer
{
private:	
	HANDLE						    m_hThreadUpdateFtp;	
	HANDLE							m_hThreadConnectFtp;
	HANDLE							m_hThreadQmsDataDel;					// Thread Handle.
	HANDLE						    m_hThreadUpdateImgFtp;	
	CString  m_strNowDate;
	CString  m_strNowTime; 
	USHORT	m_uLogBlockLevel;			///< Log 를 기록할 Log Level 을 가진다.이것보다 낮은 레벨은 기입하지 않는다.
private:
	BOOL m_fnFTPConnect();
	//BOOL m_fnFTPConnect(STRU_FTP_SEVER_INFO stFtpInfo );
	//BOOL m_fnFtpConnectProc(int iIpCnt);
	BOOL m_fnFtpConnectProc(int iIpCnt, STRU_FTP_SEVER_INFO &m_stCurFtpInfo, STRU_FTP_PATH &m_stCurFtpPath);
	//BOOL m_fnFtpConnectImgProc(int iIpCnt);
	BOOL m_fnFtpConnectImgProc(int iIpCnt, STRU_FTP_SEVER_INFO &m_stCurFtpInfo, STRU_FTP_PATH &m_stCurFtpPath);
	BOOL m_fnFTPUploadProc(LPCTSTR strFileName);
	BOOL m_fnFTPUploadProcImg(LPCTSTR strFileName);
	BOOL m_fnInitUploadThread();
	BOOL m_fnInitUploadImgThread();
	BOOL m_fnInitConnectionThread();	//2015.11.20
	int  m_fnGetErrorMessage();
	int m_fnSetErrorMessage(CString strErrMsg, CString strGuestErrMsg, int LogFlag);
	int  m_fnDeleteCurFile(CString strDeleteFilePath);
	BOOL QmsFileUpload(LPVOID pArg );
	BOOL QmsImgUplaod(LPVOID pArg );
public:
	//CLogWriterCruxServer*						m_pFtpLogWriter;
	//CLogWriterCruxServer*						m_pFtpImgLogWriter;
	BOOL m_fnStartFTPConnect(); 	
	//BOOL m_fnFTPdUploadData(CString strFileName);	
	BOOL m_fnFTPdUploadData();	
	BOOL m_fnFTPdUploadImg();
	BOOL m_fnDownLoadDir(LPCTSTR rDir, LPCTSTR lDir);
	BOOL m_fnQMSUpLoadImg(LPCTSTR lDir, LPCTSTR rDir);	
	CString m_fnUpLoadDir(LPCTSTR lDir, LPCTSTR rDir);	
	BOOL m_fnChkFtpConnect(CString RemoteURL);
	BOOL m_fnLPWD();
	BOOL m_fnRPWD();
	BOOL m_fnLocalRename(CString LocalOldFile, CString LocalNewFile);
	BOOL m_fnRemoteRename(CString RemoteOldFile, CString RemoteNewFile);
	BOOL m_fnLocalDel(CString LocalFile);
	BOOL m_fnRemoteDel(CString RemoteFile);
	BOOL m_fnRemoveLocalDir(CString LocalDir);
	BOOL m_fnRemoveRemoteDir(CString RemoteDir);
	BOOL m_fnMakeLocalDir(CString LocalDir);
	BOOL m_fnMakeRemoteDir(CString RemoteDir);
	BOOL m_fnSetLocalCurDir(CString LocalDir);
	BOOL m_fnSetRemoteCurDir(CString RemoteDir);
	CString m_fnGetLocalCurDir();
	CString m_fnGetRemoteCurDir();

	int  m_fnSetAutoDeleteInfo(CString strFilePath, int nPeriod=30);

	BOOL m_fnDownLoad(CString strFile);
	int  m_fnDownLoad(CString RemoteFile, CString LocalFile, CString strRoot);
	BOOL m_fnUpLoadData(CString strFile);
	BOOL m_fnUpLoadData(CString LocalFile, CString RemoteFile, CString strRoot);

	BOOL m_fnClose();
	BOOL m_fnCloseImg();
	BOOL m_fnOpen(LPCTSTR strAddress, LPCTSTR strName, LPCTSTR strPassword, INTERNET_PORT nPort = INTERNET_DEFAULT_FTP_PORT, int LogFlag = 1);
	BOOL m_fnOpenImg(LPCTSTR strAddress, LPCTSTR strName, LPCTSTR strPassword, INTERNET_PORT nPort = INTERNET_DEFAULT_FTP_PORT, int LogFlag = 2);
	BOOL m_fnOpen(CString strAddress, INTERNET_PORT nPort = INTERNET_DEFAULT_FTP_PORT);
	BOOL m_fnSetLogStataus(
		TSTRING szLogFilePath,			///< 로그 파일 경로
		TSTRING szLogFileName,			///< 로그 파일 이름
		int uPeriodDay = 30,			///< 로그 보존 기한일
		int uWriteTerm = 0,				///< 로그 파일을 몇시간 주기로 생성하는가..시간, 0 = 하루치의 로그가 파일 하나로 생성됨.
		bool bLogWriteEnable = true,	///< 로그를 실제로 기입하는가 플레그
		USHORT uLogBlockLevel = 4);
	BOOL m_fnfileOpen(CString strDownFileName);	
	CFtpClient(		
		TSTRING szLogFilePath,			///< 로그 파일 경로
		TSTRING szLogFileName,			///< 로그 파일 이름
		int uPeriodDay = 30,			///< 로그 보존 기한일
		int uWriteTerm = 0,				///< 로그 파일을 몇시간 주기로 생성하는가..시간, 0 = 하루치의 로그가 파일 하나로 생성됨.
		bool bLogWriteEnable = true
		) : CLogWriterCruxServer(szLogFilePath, szLogFileName, uPeriodDay, uWriteTerm, bLogWriteEnable)
	{
		m_hThreadQmsDataDel = NULL;
		m_pConnection = NULL;
		m_pConnectionImg = NULL;
		m_iConnectCnt = 0;
		m_iConnectCntImg = 0;
		m_strLocalDirText.Empty();
		m_strRemoteDirText.Empty();
		m_strAddress.Empty();
		m_hThreadUpdateFtp = NULL;
		m_hThreadUpdateImgFtp = NULL;
		m_hThreadConnectFtp = NULL;
		//m_pFtpLogWriter==NULL;
		//m_pFtpImgLogWriter = NULL;
	}
	virtual ~CFtpClient();

	//{{AFX_DATA(FTP)
	CInternetSession	m_Session;
	CFtpConnection		*m_pConnection;	
	CInternetSession	m_SessionImg;
	CFtpConnection		*m_pConnectionImg;	
	CString		m_strFileName;
	CString		m_strLocalDirText;
	CString		m_strRemoteDirText;
	CString		m_strAddress;
	CString		m_strPort;
	CString		m_strFtpServer;
	CString		m_strFtpID;
	CString		m_strFtpPW;	
	int			m_iFtpPortNo;

	//STRU_FTP_CONDITION m_stCurFtpCond;
	int			m_iConnectCnt;
	int			m_iConnectCntImg;
	//CString		m_strIniIpNo;	
	BOOL m_fnPrintLogs(CString strLog);
	void m_fnQmsSentImgDelete();	//2015.07.01

protected:
	static unsigned __stdcall  m_fnFTPConnect(LPVOID pArg);
	static unsigned __stdcall  m_fnFTPUploadProc(LPVOID pArg );
	static unsigned __stdcall  m_fnFTPUploadProcImg(LPVOID pArg );
	static unsigned __stdcall  m_fnFTPConnectCheck(LPVOID pArg);	//2015.11.20
	virtual int m_fnPrintLog(const wchar_t* pszLogData, ...){return 0;}
	virtual int m_fnPrintLog(const char* pszLogData, ...){return 0;}

	int	m_fnStartAutoDelete(BOOL bStart);	
	static void	m_fnCheckAutoDelte(LPVOID pWnd);	
protected:
	// Auto Delete..	
	CFileMng		m_fileMng;
	int				m_nDeletePeriod;
	CString			m_strDeleteFilePath;
	BOOL			m_bFileAutoDelete;
	BOOL			m_bFileAutoDelete_Working;


public:
		/** 
	@brief		로그를 기록한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		Log Task 로 Log 기록 메시지를 전달한다. 유니코드용
	@author		김용태
	@date		2006.
	*/
	int m_fnPrintLog(
		USHORT LogLevel,				///< 기록 할 Log 의 Log Level, 1차로 함수 내에서  Block을 결정하고, 2차로 Log Task 에서 Block 여부가 결정된다.
		int	nLogTextSize,				///< 로그를 기입할 내용의 사이즈
		const wchar_t* pszLogData,		///< 실제 기록 할 Log 내용
		...								///< 가변 인자
		)
	{
		int				nResultCode = APP_OK;

		if(LogLevel > m_uLogBlockLevel)//NetWork부하를 줄이기 위해서 Log Data자체를 보내지 않는다. 
		{
			return OKAY;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////
		{
			wchar_t* cpLogBuffer = new wchar_t[nLogTextSize+50];
			memset(cpLogBuffer, 0, sizeof(wchar_t) * nLogTextSize+50);
			va_list vaList;
			va_start(vaList, pszLogData);
			vswprintf_s(cpLogBuffer, nLogTextSize+50, pszLogData, (va_list)vaList);
			va_end(vaList);
			nResultCode = m_fnWriteLog(cpLogBuffer);
			delete[] cpLogBuffer;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////

		

		return nResultCode;//OK;
	}
	
	/** 
	@brief		로그를 기록한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		Log Task 로 Log 기록 메시지를 전달한다.
	@author		김용태
	@date		2006.
	*/
	int m_fnPrintLog(
		USHORT LogLevel,				///< 기록 할 Log 의 Log Level, 1차로 함수 내에서  Block을 결정하고, 2차로 Log Task 에서 Block 여부가 결정된다.
		int	nLogTextSize,				///< 로그를 기입할 내용의 사이즈
		const char* pszLogData,			///< 실제 기록 할 Log 내용
		...								///< 가변 인자
		)
	{
		int				nResultCode = APP_OK;

		if(LogLevel > m_uLogBlockLevel)//NetWork부하를 줄이기 위해서 Log Data자체를 보내지 않는다. 
		{
			return OKAY;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////
		{
			char* cpLogBuffer = new char[nLogTextSize+50];
			memset(cpLogBuffer, 0, sizeof(char) * nLogTextSize+50);
			va_list vaList;
			va_start(vaList, pszLogData);
			vsprintf_s(cpLogBuffer, nLogTextSize+50, pszLogData, (va_list)vaList);
			va_end(vaList);
			nResultCode = m_fnWriteLog(cpLogBuffer);
			delete[] cpLogBuffer;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////
		
		//nResultCode = m_fnWriteLog(pszLogData);

		return nResultCode;//OK;
	}

			/** 
	@brief		로그를 기록한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		Log Task 로 Log 기록 메시지를 전달한다.
	@author		김용태
	@date		2006.
	*/
	int m_fnPrintLog(
		USHORT LogLevel,				///< 기록 할 Log 의 Log Level, 1차로 함수 내에서  Block을 결정하고, 2차로 Log Task 에서 Block 여부가 결정된다.
//		int	nLogTextSize,				///< 로그를 기입할 내용의 사이즈
		const char* pszLogData			///< 실제 기록 할 Log 내용
		)
	{
		int				nResultCode = APP_OK;

		if(LogLevel > m_uLogBlockLevel)//NetWork부하를 줄이기 위해서 Log Data자체를 보내지 않는다. 
		{
			return OKAY;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////
		{
			nResultCode = m_fnWriteLog(pszLogData);
		}
		/////////////////////////////////////////////////////////////////////////////////////////////
		
		//nResultCode = m_fnWriteLog(pszLogData);

		return nResultCode;//OK;
	}

		/** 
	@brief		로그를 기록한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		Log Task 로 Log 기록 메시지를 전달한다. 유니코드용
	@author		김용태
	@date		2006.
	*/
	int m_fnPrintLog(
		USHORT LogLevel,				///< 기록 할 Log 의 Log Level, 1차로 함수 내에서  Block을 결정하고, 2차로 Log Task 에서 Block 여부가 결정된다.
//		int	nLogTextSize,				///< 로그를 기입할 내용의 사이즈
		const wchar_t* pszLogData		///< 실제 기록 할 Log 내용
		)
	{
		int				nResultCode = APP_OK;

		if(LogLevel > m_uLogBlockLevel)//NetWork부하를 줄이기 위해서 Log Data자체를 보내지 않는다. 
		{
			return OKAY;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////
		{
			nResultCode = m_fnWriteLog(pszLogData);
		}
		/////////////////////////////////////////////////////////////////////////////////////////////

		

		return nResultCode;//OK;
	}


};


#endif // !defined(AFX_FTPCLIENT_H__F196D430_806C_4A00_B5BE_04AC559B59A2__INCLUDED_)
