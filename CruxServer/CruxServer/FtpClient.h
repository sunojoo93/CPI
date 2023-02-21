// FtpClient.h: interface for the CFtpClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTPCLIENT_H__F196D430_806C_4A00_B5BE_04AC559B59A2__INCLUDED_)
#define AFX_FTPCLIENT_H__F196D430_806C_4A00_B5BE_04AC559B59A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxinet.h>	//ftp �� ���� �߰��ؾߵ�
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
	USHORT	m_uLogBlockLevel;			///< Log �� ����� Log Level �� ������.�̰ͺ��� ���� ������ �������� �ʴ´�.
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
		TSTRING szLogFilePath,			///< �α� ���� ���
		TSTRING szLogFileName,			///< �α� ���� �̸�
		int uPeriodDay = 30,			///< �α� ���� ������
		int uWriteTerm = 0,				///< �α� ������ ��ð� �ֱ�� �����ϴ°�..�ð�, 0 = �Ϸ�ġ�� �αװ� ���� �ϳ��� ������.
		bool bLogWriteEnable = true,	///< �α׸� ������ �����ϴ°� �÷���
		USHORT uLogBlockLevel = 4);
	BOOL m_fnfileOpen(CString strDownFileName);	
	CFtpClient(		
		TSTRING szLogFilePath,			///< �α� ���� ���
		TSTRING szLogFileName,			///< �α� ���� �̸�
		int uPeriodDay = 30,			///< �α� ���� ������
		int uWriteTerm = 0,				///< �α� ������ ��ð� �ֱ�� �����ϴ°�..�ð�, 0 = �Ϸ�ġ�� �αװ� ���� �ϳ��� ������.
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
	@brief		�α׸� ����Ѵ�.

	@return		���� ���� : 0, �̻� ���� : �׿�
	@remark		Log Task �� Log ��� �޽����� �����Ѵ�. �����ڵ��
	@author		�����
	@date		2006.
	*/
	int m_fnPrintLog(
		USHORT LogLevel,				///< ��� �� Log �� Log Level, 1���� �Լ� ������  Block�� �����ϰ�, 2���� Log Task ���� Block ���ΰ� �����ȴ�.
		int	nLogTextSize,				///< �α׸� ������ ������ ������
		const wchar_t* pszLogData,		///< ���� ��� �� Log ����
		...								///< ���� ����
		)
	{
		int				nResultCode = APP_OK;

		if(LogLevel > m_uLogBlockLevel)//NetWork���ϸ� ���̱� ���ؼ� Log Data��ü�� ������ �ʴ´�. 
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
	@brief		�α׸� ����Ѵ�.

	@return		���� ���� : 0, �̻� ���� : �׿�
	@remark		Log Task �� Log ��� �޽����� �����Ѵ�.
	@author		�����
	@date		2006.
	*/
	int m_fnPrintLog(
		USHORT LogLevel,				///< ��� �� Log �� Log Level, 1���� �Լ� ������  Block�� �����ϰ�, 2���� Log Task ���� Block ���ΰ� �����ȴ�.
		int	nLogTextSize,				///< �α׸� ������ ������ ������
		const char* pszLogData,			///< ���� ��� �� Log ����
		...								///< ���� ����
		)
	{
		int				nResultCode = APP_OK;

		if(LogLevel > m_uLogBlockLevel)//NetWork���ϸ� ���̱� ���ؼ� Log Data��ü�� ������ �ʴ´�. 
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
	@brief		�α׸� ����Ѵ�.

	@return		���� ���� : 0, �̻� ���� : �׿�
	@remark		Log Task �� Log ��� �޽����� �����Ѵ�.
	@author		�����
	@date		2006.
	*/
	int m_fnPrintLog(
		USHORT LogLevel,				///< ��� �� Log �� Log Level, 1���� �Լ� ������  Block�� �����ϰ�, 2���� Log Task ���� Block ���ΰ� �����ȴ�.
//		int	nLogTextSize,				///< �α׸� ������ ������ ������
		const char* pszLogData			///< ���� ��� �� Log ����
		)
	{
		int				nResultCode = APP_OK;

		if(LogLevel > m_uLogBlockLevel)//NetWork���ϸ� ���̱� ���ؼ� Log Data��ü�� ������ �ʴ´�. 
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
	@brief		�α׸� ����Ѵ�.

	@return		���� ���� : 0, �̻� ���� : �׿�
	@remark		Log Task �� Log ��� �޽����� �����Ѵ�. �����ڵ��
	@author		�����
	@date		2006.
	*/
	int m_fnPrintLog(
		USHORT LogLevel,				///< ��� �� Log �� Log Level, 1���� �Լ� ������  Block�� �����ϰ�, 2���� Log Task ���� Block ���ΰ� �����ȴ�.
//		int	nLogTextSize,				///< �α׸� ������ ������ ������
		const wchar_t* pszLogData		///< ���� ��� �� Log ����
		)
	{
		int				nResultCode = APP_OK;

		if(LogLevel > m_uLogBlockLevel)//NetWork���ϸ� ���̱� ���ؼ� Log Data��ü�� ������ �ʴ´�. 
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
