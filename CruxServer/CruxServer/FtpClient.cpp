// FtpClient.cpp: implementation of the CFtpClient class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdafx.h"
#include "FtpClient.h"
#include "MainFrm.h"
#include<windows.h>
#include "Shlwapi.h"
#include "ViewSetup.h"
#include "Token.h"
#include "..\..\..\..\..\VisualStation\CommonHeader\Class\Interserverinterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


enum
{
	QMS_DAT_LOG,
	QMS_IMG_LOG
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFTPFileFind::CFTPFileFind(CFtpConnection* pConnection)
{	
	// yskim - 0712
	try
	{
		//if(m_pRemoteFindFile) delete m_pRemoteFindFile;
		m_pRemoteFindFile = new CFtpFileFind(pConnection);
		pConnection->GetCurrentDirectoryAsURL(m_strAddress);
	}
	catch(CException* ee)
	{
		m_pRemoteFindFile->Close();
		delete m_pRemoteFindFile;
		ee->Delete();
	}
	catch(CFTPFileFind* ee)
	{
		m_pRemoteFindFile->Close();
		delete m_pRemoteFindFile;
	}
	catch(	CInternetException* ee)
	{
		m_pRemoteFindFile->Close();
		delete m_pRemoteFindFile;
		ee->Delete();
	}
}

CFTPFileFind::~CFTPFileFind()
{

	m_pRemoteFindFile->Close();
	delete m_pRemoteFindFile;

}
BOOL CFTPFileFind::m_fnRemoteClose()
{

	m_pRemoteFindFile->Close();
	delete m_pRemoteFindFile;
	return TRUE;
}

BOOL CFTPFileFind::FindFile(LPCTSTR RemoteFile, DWORD dwFlags)
{
	try
	{
		BOOL brc;

		//2016.09.09 Mr.Chu  Bypass Panel 예외처리 
		CString strTemp = _T("");
		strTemp.Format(_T("%s"),RemoteFile);
		if( strTemp != _T("/") )
			return FALSE;
		
		brc = m_pRemoteFindFile->FindFile(RemoteFile, dwFlags);
		if(brc == FALSE){
			TRACE("RFindFile ERROR : %s -> %d \n", m_strAddress, ::GetLastError());

			G_AddLog(3, 2, _T("[FTP File Find ERROR] RFindFile ERROR : File: %s, %s -> %d \n"),RemoteFile,  m_strAddress, ::GetLastError() );

			GetErrorMessage();
			return FALSE;
		}	
	}
	catch(	CInternetException* ee)
	{
		m_pRemoteFindFile->Close();
		delete m_pRemoteFindFile;
		ee->Delete();
		return FALSE;
	}
	catch (CException* ee)
	{
		m_pRemoteFindFile->Close();
		delete m_pRemoteFindFile;
		ee->Delete();
		return FALSE;
	}
	catch(CFTPFileFind* ee)
	{
		m_pRemoteFindFile->Close();
		delete m_pRemoteFindFile;
		return FALSE;
	}
	return TRUE;
}

CString CFTPFileFind::GetFileName()
{
	return m_pRemoteFindFile->GetFileName();
}


BOOL CFTPFileFind::FindNextFile()
{
	try
	{
		BOOL brc;
		brc = m_pRemoteFindFile->FindNextFile();
		if(brc == FALSE){
			TRACE("RFindNextFile ERROR : %s -> %d \n", m_strAddress, ::GetLastError());
			GetErrorMessage();
			return FALSE;
		}
	}
	catch(	CInternetException* ee)
	{
		m_pRemoteFindFile->Close();
		delete m_pRemoteFindFile;
		ee->Delete();
		return FALSE;
	}
	catch (CException* ee)
	{
		m_pRemoteFindFile->Close();
		delete m_pRemoteFindFile;
		ee->Delete();
		return FALSE;
	}
	catch(CFTPFileFind* ee)
	{
		m_pRemoteFindFile->Close();
		delete m_pRemoteFindFile;
		return FALSE;
	}

	return TRUE;
}

int CFTPFileFind::GetErrorMessage()
{
	TCHAR *szRcode;
	int rCode;
	CString strError = _T("");

	rCode = ::GetLastError();
	switch(rCode) {
	case ERROR_ACCESS_DENIED          :    szRcode = _T("ERROR_ACCESS_DENIED          ");    break;
	case ERROR_ADAP_HDW_ERR           :    szRcode = _T("ERROR_ADAP_HDW_ERR           ");    break;
	case ERROR_ALREADY_ASSIGNED       :    szRcode = _T("ERROR_ALREADY_ASSIGNED       ");    break;
	case ERROR_ARENA_TRASHED          :    szRcode = _T("ERROR_ARENA_TRASHED          ");    break;
	case ERROR_BAD_COMMAND            :    szRcode = _T("ERROR_BAD_COMMAND            ");    break;
	case ERROR_BAD_DEV_TYPE           :    szRcode = _T("ERROR_BAD_DEV_TYPE           ");    break;
	case ERROR_BAD_ENVIRONMENT        :    szRcode = _T("ERROR_BAD_ENVIRONMENT        ");    break;
	case ERROR_BAD_FORMAT             :    szRcode = _T("ERROR_BAD_FORMAT             ");    break;
	case ERROR_BAD_LENGTH             :    szRcode = _T( "ERROR_BAD_LENGTH             ");    break;
	case ERROR_BAD_NET_NAME           :    szRcode = _T( "ERROR_BAD_NET_NAME           ");    break;
	case ERROR_BAD_NET_RESP           :    szRcode =_T( "ERROR_BAD_NET_RESP           ");    break;
	case ERROR_BAD_NETPATH            :    szRcode = _T("ERROR_BAD_NETPATH            ");    break;
	case ERROR_BAD_REM_ADAP           :    szRcode = _T("ERROR_BAD_REM_ADAP           ");    break;
	case ERROR_BAD_UNIT               :    szRcode = _T("ERROR_BAD_UNIT               ");    break;
	case ERROR_CANNOT_MAKE            :    szRcode = _T("ERROR_CANNOT_MAKE            ");    break;
	case ERROR_CRC                    :    szRcode = _T("ERROR_CRC                    ");    break;
	case ERROR_CURRENT_DIRECTORY      :    szRcode = _T("ERROR_CURRENT_DIRECTORY      ");    break;
	case ERROR_DEV_NOT_EXIST          :    szRcode = _T("ERROR_DEV_NOT_EXIST          ");    break;
	case ERROR_DUP_NAME               :    szRcode = _T("ERROR_DUP_NAME               ");    break;
	case ERROR_FAIL_I24               :    szRcode = _T("ERROR_FAIL_I24               ");    break;
	case ERROR_FILE_EXISTS            :    szRcode = _T("ERROR_FILE_EXISTS            ");    break;
	case ERROR_FILE_NOT_FOUND         :    szRcode = _T("ERROR_FILE_NOT_FOUND         ");    break;
	case ERROR_GEN_FAILURE            :    szRcode = _T("ERROR_GEN_FAILURE            ");    break;
	case ERROR_HANDLE_DISK_FULL       :    szRcode = _T("ERROR_HANDLE_DISK_FULL       ");    break;
	case ERROR_HANDLE_EOF             :    szRcode = _T("ERROR_HANDLE_EOF             ");    break;
	case ERROR_INVALID_ACCESS         :    szRcode = _T("ERROR_INVALID_ACCESS         ");    break;
	case ERROR_INVALID_BLOCK          :    szRcode = _T("ERROR_INVALID_BLOCK          ");    break;
	case ERROR_INVALID_DATA           :    szRcode = _T("ERROR_INVALID_DATA           ");    break;
	case ERROR_INVALID_DRIVE          :    szRcode = _T("ERROR_INVALID_DRIVE          ");    break;
	case ERROR_INVALID_FUNCTION       :    szRcode = _T("ERROR_INVALID_FUNCTION       ");    break;
	case ERROR_INVALID_HANDLE         :    szRcode = _T("ERROR_INVALID_HANDLE         ");    break;
	case ERROR_INVALID_PARAMETER      :    szRcode = _T("ERROR_INVALID_PARAMETER      ");    break;
	case ERROR_INVALID_PASSWORD       :    szRcode = _T("ERROR_INVALID_PASSWORD       ");    break;
	case ERROR_LOCK_VIOLATION         :    szRcode = _T("ERROR_LOCK_VIOLATION         ");    break;
	case ERROR_NET_WRITE_FAULT        :    szRcode = _T("ERROR_NET_WRITE_FAULT        ");    break;
	case ERROR_NETNAME_DELETED        :    szRcode = _T("ERROR_NETNAME_DELETED        ");    break;
	case ERROR_NETWORK_ACCESS_DENIED  :    szRcode = _T("ERROR_NETWORK_ACCESS_DENIED  ");    break;
	case ERROR_NETWORK_BUSY           :    szRcode = _T("ERROR_NETWORK_BUSY           ");    break;
	case ERROR_NO_MORE_FILES          :    szRcode = _T("ERROR_NO_MORE_FILES          ");    break;
	case ERROR_NO_PROC_SLOTS          :    szRcode = _T("ERROR_NO_PROC_SLOTS          ");    break;
	case ERROR_NO_SPOOL_SPACE         :    szRcode = _T("ERROR_NO_SPOOL_SPACE         ");    break;
	case ERROR_NOT_DOS_DISK           :    szRcode = _T("ERROR_NOT_DOS_DISK           ");    break;
	case ERROR_NOT_ENOUGH_MEMORY      :    szRcode = _T("ERROR_NOT_ENOUGH_MEMORY      ");    break;
	case ERROR_NOT_READY              :    szRcode = _T("ERROR_NOT_READY              ");    break;
	case ERROR_NOT_SAME_DEVICE        :    szRcode = _T("ERROR_NOT_SAME_DEVICE        ");    break;
	case ERROR_NOT_SUPPORTED          :    szRcode = _T("ERROR_NOT_SUPPORTED          ");    break;
	case ERROR_OUT_OF_PAPER           :    szRcode = _T("ERROR_OUT_OF_PAPER           ");    break;
	case ERROR_OUT_OF_STRUCTURES      :    szRcode = _T("ERROR_OUT_OF_STRUCTURES      ");    break;
	case ERROR_OUTOFMEMORY            :    szRcode = _T("ERROR_OUTOFMEMORY            ");    break;
	case ERROR_PATH_NOT_FOUND         :    szRcode = _T("ERROR_PATH_NOT_FOUND         ");    break;
	case ERROR_PRINT_CANCELLED        :    szRcode = _T("ERROR_PRINT_CANCELLED        ");    break;
	case ERROR_PRINTQ_FULL            :    szRcode = _T("ERROR_PRINTQ_FULL            ");    break;
	case ERROR_READ_FAULT             :    szRcode = _T("ERROR_READ_FAULT             ");    break;
	case ERROR_REDIR_PAUSED           :    szRcode = _T("ERROR_REDIR_PAUSED           ");    break;
	case ERROR_REM_NOT_LIST           :    szRcode = _T("ERROR_REM_NOT_LIST           ");    break;
	case ERROR_REQ_NOT_ACCEP          :    szRcode = _T("ERROR_REQ_NOT_ACCEP          ");    break;
	case ERROR_SECTOR_NOT_FOUND       :    szRcode = _T("ERROR_SECTOR_NOT_FOUND       ");    break;
	case ERROR_SEEK                   :    szRcode = _T("ERROR_SEEK                   ");    break;
	case ERROR_SHARING_BUFFER_EXCEEDED:    szRcode = _T("ERROR_SHARING_BUFFER_EXCEEDED");    break;
	case ERROR_SHARING_PAUSED         :    szRcode = _T("ERROR_SHARING_PAUSED         ");    break;
	case ERROR_SHARING_VIOLATION      :    szRcode = _T("ERROR_SHARING_VIOLATION      ");    break;
	case ERROR_TOO_MANY_CMDS          :    szRcode = _T("ERROR_TOO_MANY_CMDS          ");    break;
	case ERROR_TOO_MANY_NAMES         :    szRcode = _T("ERROR_TOO_MANY_NAMES         ");    break;
	case ERROR_TOO_MANY_OPEN_FILES    :    szRcode = _T("ERROR_TOO_MANY_OPEN_FILES    ");    break;
	case ERROR_TOO_MANY_SESS          :    szRcode = _T("ERROR_TOO_MANY_SESS          ");    break;
	case ERROR_UNEXP_NET_ERR          :    szRcode = _T("ERROR_UNEXP_NET_ERR          ");    break;
	case ERROR_WRITE_FAULT            :    szRcode = _T("ERROR_WRITE_FAULT            ");    break;
	case ERROR_WRITE_PROTECT          :    szRcode = _T("ERROR_WRITE_PROTECT          ");    break;
	case ERROR_WRONG_DISK             :    szRcode = _T("ERROR_WRONG_DISK             ");    break;

	default 			               :    szRcode = _T("DEFAULT                      ");    break;
	}           
	TRACE("CFTPFileFind GetLastError() Errocode = %s [%d] \n", szRcode, rCode);
	//strError = 
	return rCode;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CFtpClient::~CFtpClient()
{
	if(m_pConnection != NULL){
		m_pConnection->Close();
		delete m_pConnection;
	}
	//delete m_pFtpImgLogWriter;
	//delete m_pFtpLogWriter;
	m_Session.Close();

	if(m_pConnectionImg != NULL){
		m_pConnectionImg->Close();
		delete m_pConnectionImg;
	}
	//delete m_pFtpImgLogWriter;
	//delete m_pFtpLogWriter;
	m_SessionImg.Close();
}

BOOL CFtpClient::m_fnOpen(LPCTSTR strAddress, LPCTSTR strName, LPCTSTR strPassword, INTERNET_PORT nPort, int LogFlag)
{	
	TCHAR buff[256];
	CString strMsg;
	BOOL rc = FALSE;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	//이미 연결된 것이 있으면 해제
	if(m_pConnection != NULL){
			m_pConnection->Close();
		delete m_pConnection;		
		m_pConnection = NULL;
		//return TRUE;
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" [m_fnOpen] m_fnOpen m_pConnection Memory Free ! \n"),m_strRemoteDirText );
	}

	try
	{				
		// 현재 Local 디렉토리
		memset(buff,0X00,sizeof(buff));
		GetCurrentDirectory(255, (LPTSTR)buff);
		m_Session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 10);    // 10 secs)
		m_Session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 10); // 10 secs)
		m_Session.SetOption(INTERNET_OPTION_CALLBACK, 10);		  // 10 secs)
		m_Session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10); // 10 secs)
		m_Session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 10); // 10 secs)
		//m_Session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 10); // 10 secs)

		m_pConnection = m_Session.GetFtpConnection((LPCTSTR)strAddress, (LPCTSTR)strName, (LPCTSTR)strPassword, nPort ,1);		
		m_strAddress = strAddress;				
		m_strLocalDirText = buff;

		//if(m_pConnection != NULL){
		//	// 현재 Remote 디렉토리
		//	m_pConnection->GetCurrentDirectory(m_strRemoteDirText);
		//
		//	//찾을 화일 지정
		//	if(m_strAddress != ""){
		//		CFTPFileFind	finder(m_pConnection);
		//		finder.FindFile(m_strRemoteDirText);
		//	}else{
		//		return rc;
		//	}			
		//}else{
		//	return rc;
		//}

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" [FTP Accessing] m_fnOpen GetCurrentDirectory 2: %s \n"),m_strRemoteDirText );
		//todo.. 로그 기입 연결 성공 정보
		if (LogFlag == 1)
		{
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[LogFlag 1] m_fnOpen OK P1 connection IP : %s / port : %d \n"),strAddress, nPort );
			G_AddLog_QMS(_T("OK,P1,%s:%d"),strAddress,nPort);
		}
		if (LogFlag == 2)
		{
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[LogFlag 2] m_fnOpen OK P1 connection IP : %s / port : %d \n"),strAddress, nPort );
			G_AddLog_QMS(_T("OK,P1,%s:%d"),strAddress,nPort);
		}

		pFrame->m_bQmsConnection  = TRUE;	//2015.11.20

		return TRUE;
	}
	catch(CInternetException* ee)
	{	

		//todo.. 로그 기입 연결 오류 정보
		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			_T("[catch(CInternetException* ee)] m_fnOpen NG P1 connection IP : %s / port : %d -530-Not Logged IN\n"),strAddress, nPort );	
		G_AddLog_QMS(_T("NG,P1,%s:%d"),strAddress,nPort);

		pFrame->m_bQmsConnection  = FALSE;
		ee->GetErrorMessage((LPTSTR)buff, 255);
		strMsg = buff;

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T(" GetErrorMessage : %s\n"),strMsg );	

		if(m_pConnection != NULL){
			//todo.. 로그 기입 연결 오류 정보
			//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			//	_T("[catch(CInternetException* ee)] m_fnOpen m_pConnection Init \n"));
			m_pConnection->Close();
			delete m_pConnection;		
			m_pConnection = NULL;
			//return TRUE;
		}
		ee->Delete();
	}

	catch(CFtpClient* ee)
	{		
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[catch(CFtpClient* ee)] m_fnOpen NG P1 connection IP : %s / port : %d -530-Not Logged IN\n"),strAddress, nPort );

		ee->m_fnGetErrorMessage();
		strMsg = buff;

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T(" GetErrorMessage : %s\n"),strMsg );	

		if(m_pConnection != NULL){
			//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			//	_T("[catch(CFtpClient* ee)] m_fnOpen m_pConnection Init \n"));
			m_pConnection->Close();
			delete m_pConnection;		
			m_pConnection = NULL;
			//return TRUE;
		}
		ee->m_fnClose();
	}
	if(m_iConnectCnt < QMS_IP_CNT_MAX) 
	{		
		
		//todo.. 로그 기입 연결 오류 정보
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
				_T("[Retry] m_fnOpen NG P1 connection IP : %s / port : %d -530-Not Logged IN\n"),strAddress, nPort );		
			G_AddLog_QMS(_T("NG,P1,%s:%d"),strAddress,nPort);
		m_iConnectCnt++;
		//rc = m_fnOpen(strAddress, strName, strPassword, nPort);
		//pFrame->m_bQmsConnection  = FALSE;	//2015.11.20
		pFrame->m_bQmsConnection  = FALSE;
 		if(m_pConnection != NULL){
			//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			//	_T("[Retry] m_fnOpen m_pConnection Init \n"));	
 			m_pConnection->Close();
 			delete m_pConnection;		
 			m_pConnection = NULL;
 			//return TRUE;
 		}
	}
	else
	{
		//AfxMessageBox(strMsg, MB_OK|MB_ICONSTOP);
		memset(buff,0,sizeof(buff));			
		wsprintf(buff, _T("[Retry End] m_fnOpen NG P1 connection IP : %s / port : %d -530-Not Logged IN\n"),strAddress, nPort );
		G_AddLog_QMS(_T("NG,P1,%s:%d"),strAddress,nPort);
		//AfxMessageBox(buff, MB_OK|MB_ICONSTOP);
		//todo.. 로그 기입 중복 테스트 실패 오류 정보		
		m_iConnectCnt = 0;
		pFrame->m_bQmsConnection  = FALSE;

		//2016.07.17 Mr.Chu
		if(m_pConnection != NULL){
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
				_T("[Retry End] m_fnOpen m_pConnection Init \n"));	
			m_pConnection->Close();
			delete m_pConnection;		
			m_pConnection = NULL;
			//return TRUE;
		}
	}

	return rc;	
}

BOOL CFtpClient::m_fnOpenImg(LPCTSTR strAddress, LPCTSTR strName, LPCTSTR strPassword, INTERNET_PORT nPort, int LogFlag)
{	
	TCHAR buff[256];
	CString strMsg;
	BOOL rc = FALSE;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	//이미 연결된 것이 있으면 해제
	if(m_pConnectionImg != NULL){
		m_pConnectionImg->Close();
		delete m_pConnectionImg;		
		m_pConnectionImg = NULL;
		//return TRUE;
		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" [m_fnOpenImg] m_fnOpenImg m_pConnection Memory Free ! \n"),m_strRemoteDirText );
	}

	try
	{				
		// 현재 Local 디렉토리
		memset(buff,0X00,sizeof(buff));
		GetCurrentDirectory(255, (LPTSTR)buff);
		m_SessionImg.SetOption(INTERNET_OPTION_SEND_TIMEOUT, 10);    // 10 secs)
		m_SessionImg.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 10); // 10 secs)
		m_SessionImg.SetOption(INTERNET_OPTION_CALLBACK, 10);		  // 10 secs)
		m_SessionImg.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10); // 10 secs)
		m_SessionImg.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 10); // 10 secs)
		//m_Session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 10); // 10 secs)

		m_pConnectionImg = m_SessionImg.GetFtpConnection((LPCTSTR)strAddress, (LPCTSTR)strName, (LPCTSTR)strPassword, nPort ,1);		
		m_strAddress = strAddress;			
		m_strPort.Format(_T("%d"),nPort);
		m_strLocalDirText = buff;

		if(m_pConnectionImg != NULL){
			// 현재 Remote 디렉토리
			m_pConnectionImg->GetCurrentDirectory(m_strRemoteDirText);

			//찾을 화일 지정
			if(m_strAddress != ""){
				CFTPFileFind	finder(m_pConnectionImg);
				finder.FindFile(m_strRemoteDirText);
			}else{
				return rc;
			}
		}else{
			return rc;
		}		
		
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" [FTP Accessing] m_fnOpenImg GetCurrentDirectory 2: %s \n"),m_strRemoteDirText );
		//todo.. 로그 기입 연결 성공 정보
		if (LogFlag == 1)
		{
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[LogFlag 1] m_fnOpenImg OK P1 connection IP : %s / port : %d \n"),strAddress, nPort );
			G_AddLog_IMG_QMS(_T("OK,P1,%s:%d"),strAddress,nPort);
		}
		if (LogFlag == 2)
		{
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[LogFlag 2] m_fnOpenImg OK P1 connection IP : %s / port : %d \n"),strAddress, nPort );
			G_AddLog_IMG_QMS(_T("OK,P1,%s:%d"),strAddress,nPort);
		}

		pFrame->m_bQmsConnection  = TRUE;	//2015.11.20

		return TRUE;
	}
	catch(CInternetException* ee)
	{	

		//todo.. 로그 기입 연결 오류 정보
		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			_T("[catch(CInternetException* ee)] m_fnOpenImg NG P1 connection IP : %s / port : %d -530-Not Logged IN\n"),strAddress, nPort );	
		G_AddLog_IMG_QMS(_T("NG,P1,%s:%d"),strAddress,nPort);

		pFrame->m_bQmsConnection  = FALSE;
		ee->GetErrorMessage((LPTSTR)buff, 255);
		strMsg = buff;

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T(" GetErrorMessage : %s\n"),strMsg );	

		if(m_pConnectionImg != NULL){
			//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			//	_T("[catch(CInternetException* ee)] m_fnOpenImg m_pConnectionImg Init \n"));	
			m_pConnectionImg->Close();
			delete m_pConnectionImg;		
			m_pConnectionImg = NULL;
			//return TRUE;
		}
		ee->Delete();
	}

	catch(CFtpClient* ee)
	{		
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[catch(CFtpClient* ee)] m_fnOpenImg NG P1 connection IP : %s / port : %d -530-Not Logged IN\n"),strAddress, nPort );

		ee->m_fnGetErrorMessage();
		strMsg = buff;

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T(" GetErrorMessage : %s\n"),strMsg );	

		if(m_pConnectionImg != NULL){
			//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			//	_T("[catch(CFtpClient* ee)] m_fnOpenImg m_pConnectionImg Init \n"));	
			m_pConnectionImg->Close();
			delete m_pConnectionImg;		
			m_pConnectionImg = NULL;
			//return TRUE;
		}
		ee->m_fnCloseImg();
	}
	if(m_iConnectCntImg < QMS_IP_CNT_MAX) 
	{		

		//todo.. 로그 기입 연결 오류 정보
		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			_T("[Retry] m_fnOpenImg NG P1 connection IP : %s / port : %d -530-Not Logged IN\n"),strAddress, nPort );
		G_AddLog_IMG_QMS(_T("NG,P1,%s:%d"),strAddress,nPort);

		m_iConnectCntImg++;
		//rc = m_fnOpen(strAddress, strName, strPassword, nPort);
		//pFrame->m_bQmsConnection  = FALSE;	//2015.11.20
		pFrame->m_bQmsConnection  = FALSE;
		if(m_pConnectionImg != NULL){
			//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			//	_T("[Retry] m_fnOpenImg m_pConnectionImg Init \n"));
			m_pConnectionImg->Close();
			delete m_pConnectionImg;		
			m_pConnectionImg = NULL;
			//return TRUE;
		}
	}
	else
	{
		//AfxMessageBox(strMsg, MB_OK|MB_ICONSTOP);
		memset(buff,0,sizeof(buff));			
		wsprintf(buff, _T("[Retry End] m_fnOpenImg NG P1 connection IP : %s / port : %d -530-Not Logged IN\n"),strAddress, nPort );
		G_AddLog_IMG_QMS(_T("NG,P1,%s:%d"),strAddress,nPort);
		//AfxMessageBox(buff, MB_OK|MB_ICONSTOP);
		//todo.. 로그 기입 중복 테스트 실패 오류 정보		
		m_iConnectCntImg = 0;
		pFrame->m_bQmsConnection  = FALSE;

		//2016.07.17 Mr.Chu
		if(m_pConnectionImg != NULL){
			//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			//	_T("[Retry End] m_fnOpenImg m_pConnectionImg Init \n"));
			m_pConnectionImg->Close();
			delete m_pConnectionImg;		
			m_pConnectionImg = NULL;
			//return TRUE;
		}
	}

	return rc;	
}

BOOL CFtpClient::m_fnOpen(CString strAddress, INTERNET_PORT nPort)
{
	TCHAR buff[256];
	//이미 연결된 것이 있으면 해제
	if(m_pConnection != NULL){
		m_pConnection->Close();
		if (m_pConnection!=NULL)
			delete m_pConnection;
		m_pConnection = NULL;		
	}

	try
	{
		m_pConnection = m_Session.GetFtpConnection((LPCTSTR)strAddress, NULL, NULL, nPort);
		m_strAddress = strAddress;

		// 현재 Local 디렉토리
		GetCurrentDirectory(255,(LPTSTR) buff);
		m_strLocalDirText = buff;
		// 현재 Remote 디렉토리
		m_pConnection->GetCurrentDirectory(m_strRemoteDirText);
	}	
	catch(	CFtpClient* ee)
	{
		ee->m_fnGetErrorMessage();				
		ee->m_fnClose();		
		return FALSE;
	}

	return TRUE;
}

BOOL CFtpClient::m_fnClose()
{
	if(m_pConnection == NULL) return FALSE;

	//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
	//	_T("[m_fnClose] FTP Closing . . .  \n") );	

 	TCHAR buff[256] = {0,};
 	try
 	{
		m_pConnection->Close();
		delete m_pConnection;
		m_pConnection = NULL;

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[m_fnClose] FTP TRY Closing . . .  \n") );	
 	}
 	catch(	CInternetException* ee)
 	{
 		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
 		//	_T("[m_fnClose] Close ERROR : %s \n"),m_strAddress );	

		ee->GetErrorMessage((LPTSTR)buff, 255);
		TRACE("Close ERROR : %s (%s)", m_strAddress, buff);
 		m_pConnection = NULL;
 		ee->Delete();
 		return FALSE;
 	}

	m_strLocalDirText.Empty();
	m_strRemoteDirText.Empty();
	m_strAddress.Empty();

	return TRUE;
}

BOOL CFtpClient::m_fnCloseImg()
{
	if(m_pConnectionImg == NULL) return FALSE;

	//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
	//	_T("[m_fnCloseImg] FTP Closing . . .  \n") );

	try
	{
		m_pConnectionImg->Close();
		delete m_pConnectionImg;
		m_pConnectionImg = NULL;

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[m_fnCloseImg] FTP TRY Closing . . .  \n") );
	}
	catch(	CInternetException* ee)
	{
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[m_fnCloseImg] Close ERROR 1: %s \n"),m_strAddress );
		char buff[256];
		ee->GetErrorMessage((LPTSTR)buff, 255);
		TRACE("Close ERROR : %s (%s)", m_strAddress, buff);
		m_pConnectionImg = NULL;
		ee->Delete();
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[m_fnCloseImg] Close ERROR 2: %s \n"),m_strAddress );

		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("m_fnCloseImg NG P4 Disconnection IP : %s / port : %d -%s\n"), m_strAddress, m_strPort, buff);
		G_AddLog_IMG_QMS(_T("NG,P4,%s:%s"),m_strAddress,m_strPort);

		return FALSE;
	}

	m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("m_fnCloseImg OK P4 Disconnection IP : %s / port : %d \n"), m_strAddress, m_strPort);
	G_AddLog_IMG_QMS(_T("OK,P4,%s:%s"),m_strAddress,m_strPort);

	m_strLocalDirText.Empty();
	m_strRemoteDirText.Empty();
	m_strAddress.Empty();

	return TRUE;
}

BOOL CFtpClient::m_fnUpLoadData(CString strFile)
{
	CString strPathName, strLocalName;

	if(m_pConnection == NULL) return FALSE;

	//디렉토리명과 파일명을 붙이기 
	if(m_strRemoteDirText != "/")	// "/" 이면 안 더해야함
	{
		strPathName = m_strRemoteDirText;
		strPathName +=  "/";
		strPathName += strFile;
	}
	else
	{
		strPathName = m_strRemoteDirText + strFile;
	}
	strLocalName = m_strLocalDirText;
	strLocalName +=  "\\";
	strLocalName += strFile;
	//strLocalName = m_strLocalDirText + "\\" + strFile;
	
	return TRUE;//m_fnUpLoadData(strLocalName, strPathName);
}

BOOL CFtpClient::m_fnUpLoadData(CString LocalFile, CString RemoteFile, CString strRoot)
{
	CString strPathName, strCmpFileName, strFileName;
	BOOL bWorking;
	int iFileCnt = 1;
	//CFTPFileFind	finder(m_pConnection);
	BOOL brc;
	TCHAR buff[256] = {0,};

	if(m_pConnection == NULL)
		return FALSE;
	if(m_strAddress == ""){	
		return FALSE;
	}	
	try
	{
		//CFTPFileFind	finder(m_pConnection);
		//if(m_pConnection != NULL){
		//	// 현재 Remote 디토리
		//	m_pConnection->GetCurrentDirectory(m_strRemoteDirText);
		//}else{
		//	return FALSE;
		//}

		////m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" [m_fnUpLoadData] GetCurrentDirectory : %s \n"),m_strRemoteDirText );

		//bWorking = finder.FindFile(m_strRemoteDirText);	// 모든 파일

		//while(bWorking == TRUE)
		//{
		//	bWorking = finder.FindNextFile();	// 파일 하나를 찾음	
		//	strFileName = finder.GetFileName();	// 파일 이름 얻기			
		//	strCmpFileName = strRoot +   strFileName;

		//	if (!strCmpFileName.Compare(RemoteFile))
		//	{
		//		//todo.. 로그 기입 동일 문서명이 존재
		//		brc = m_fnRemoteDel(RemoteFile);
		//		if (brc == FALSE)	return FALSE;
		//		break;			
		//	}
		//	iFileCnt++;
		//	if (bWorking == FALSE) break;
		//}
		////brc = FALSE;

		//if (iFileCnt)
		//{
			/*iFileCnt =0 ;
			while (iFileCnt < QMS_UPLOAD_CNT_MAX) {*/
				//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[m_fnDownLoad] PutFile1 LocalFile : %s RemoteFile : %s\n"), LocalFile, RemoteFile);
				strPathName = m_pConnection->GetServerName();
				if(m_pConnection == NULL || m_strAddress != strPathName) return FALSE;
				brc = m_pConnection->PutFile(LocalFile, RemoteFile,FTP_TRANSFER_TYPE_ASCII);		//파일 업로드
				//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[m_fnDownLoad] PutFile2 LocalFile : %s RemoteFile : %s Return : %d \n"), LocalFile, RemoteFile, brc);
				if (brc) return TRUE;			    // 업로드 성공 시 TRUE를 RETRUN 한다.	
				if (!brc) return FALSE; // 2회쩨 업로드 실패 시 FALSE를 RETRUN 한다.
				//if (!brc && iFileCnt) return FALSE; // 2회쩨 업로드 실패 시 FALSE를 RETRUN 한다.
				//if(brc == FALSE && iFileCnt < 1){		
					//m_fnSetErrorMessage(_T(" NG P2 Upload Temporary failed-%d-%s \n"), FALSE);		
					//Sleep(2000);
				//}	
				//iFileCnt ++ ;
		//	}
		//}				
		//finder.m_fnRemoteClose();		
	}
	
	catch(	CInternetException* ee)
	{
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" [m_fnUpLoadData] FINISH : FAIL1 \n") );
		ee->GetErrorMessage((LPTSTR)buff, 255);
		TRACE("UpLoad ERROR : %s (%s)<==(%s) %s \n", m_strAddress, RemoteFile, LocalFile, buff);
		ee->Delete();	
		return FALSE;
	}
	
	catch(	CFtpConnection	* ee)
	{
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" [m_fnUpLoadData] FINISH : FAIL2 \n") );
		//GetErrorMessage();
		::GetLastError();
		TRACE("UpLoad ERROR : %s (%s)<==(%s) %s \n", m_strAddress, RemoteFile, LocalFile, buff);
		ee->Close();			
		
		return FALSE;
	}

	//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" [m_fnUpLoadData] FINISH : SUCCESS \n") );
	return TRUE;
}

BOOL CFtpClient::m_fnDownLoad(CString strFile)
{
	CString strPathName, strLocalName;

	if(m_pConnection == NULL) return FALSE;

	//디렉토리명과 파일명을 붙이기 
	if(m_strRemoteDirText != "/")	// "/" 이면 안 더해야함
	{
		strPathName = m_strRemoteDirText;
		strPathName +=  "/";
		strPathName += strFile;		
	}
	else
		strPathName = m_strRemoteDirText + strFile;

	strLocalName = m_strLocalDirText;
	strLocalName +=  "\\";
	strLocalName += strFile;
//	strLocalName = m_strLocalDirText + "\\" + strFile;
	
	return TRUE;// m_fnDownLoad(strPathName, strLocalName);
}

int CFtpClient::m_fnDownLoad(CString RemoteFile, CString LocalFile, CString strRoot)
{
	CString strPathName, strCmpFileName, strFileName;
	BOOL bWorking;
	int iFileCnt = 0;
	//CFTPFileFind	finder(m_pConnection);
	BOOL rc;
	int iErrCode = 0;
	CString strErrMsg;

	if(m_pConnection == NULL){	m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[m_fnDownLoad] m_pConnection NULL...\n") );		return FALSE;}
	if(m_strAddress == ""){	
		return FALSE;
	}

	try
	{
		CFTPFileFind	finder(m_pConnection);
		if(m_pConnection != NULL){
			// 현재 Remote 디렉토리
			m_pConnection->GetCurrentDirectory(m_strRemoteDirText);
		}else{
			return FALSE;
		}		
		
		bWorking = finder.FindFile(m_strRemoteDirText);	// 모든 파일

		if (bWorking == FALSE)
		{
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[m_fnDownLoad] FileFind Fail : %d \n"),m_strRemoteDirText );

			return FALSE;
		}

		while(bWorking == TRUE)
		{
			if (bWorking == FALSE)
			{
				m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[m_fnDownLoad] Ans File Find Fail \n") );

				return FALSE;
			}

			bWorking = finder.FindNextFile();	// 파일 하나를 찾음	
			strFileName = finder.GetFileName();	// 파일 이름 얻기			
			strCmpFileName = strRoot +   strFileName;

			if (!strCmpFileName.Compare(RemoteFile))
			{	
				m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[m_fnDownLoad] Ans File Find [ %s ] [ %s ]\n"),strCmpFileName,RemoteFile );

				iFileCnt = 1;
				break;			
			}
						
		}
		if (iFileCnt)
		{
			rc = m_pConnection->GetFile(RemoteFile, LocalFile,TRUE, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_ASCII);	//파일 다운로드
			if(rc == FALSE){
				iErrCode = GetLastError();
				m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[m_fnDownLoad] GetFile Fail...\n") );
				TRACE("DownLoad ERROR : %s (%s)==>(%s) %d \n", m_strAddress, RemoteFile, LocalFile, iErrCode);						
				
				return iErrCode;
			}
		}
	}
	catch(	CInternetException* ee)
	{
// 		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
// 			_T("[CInternetException] m_fnDownLoad m_pConnection Init \n"));	
		//m_fnInitUploadThread();		
		TCHAR buff[256];
		ee->GetErrorMessage((LPTSTR)buff, 255);
		//TRACE("DownLoad ERROR : %s (%s)==>(%s) %s \n", m_strAddress, RemoteFile, LocalFile, buff);

		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			_T("DownLoad ERROR : %s (%s)==>(%s) %s \n"), m_strAddress, RemoteFile, LocalFile, buff);	
		//finder.m_fnRemoteClose();
		ee->Delete();
		return FALSE;
	}
	catch(CFTPFileFind* eftp)
	{	
		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			_T("[CFTPFileFind] m_fnDownLoad m_pConnection Init \n"));
		//m_fnInitUploadThread();
		//finder.m_fnRemoteClose();
		return eftp->GetErrorMessage();
	}

	return TRUE;
}

BOOL CFtpClient::m_fnSetRemoteCurDir(CString RemoteDir)
{
	if(m_pConnection == NULL) return FALSE;
	try
	{
		BOOL brc;
		brc = m_pConnection->SetCurrentDirectory((LPCTSTR)RemoteDir);
		if(brc == FALSE){
			TRACE("RemoteCurDir ERROR : %s (%s) >< %d \n", m_strAddress, RemoteDir, ::GetLastError());
			m_fnGetErrorMessage();
			return FALSE;
		}
		else
			m_strRemoteDirText = RemoteDir;
	}
	catch(	CInternetException* ee)
	{
		TCHAR buff[256];
		ee->GetErrorMessage((LPTSTR)buff, 255);
		TRACE("SetRemoteCurDir ERROR : %s (%s) >< %s \n", m_strAddress, RemoteDir, buff);
		ee->Delete();
		return FALSE;
	}

	return TRUE;
}

BOOL CFtpClient::m_fnSetLocalCurDir(CString LocalDir)
{
	if(::SetCurrentDirectory(LocalDir) == FALSE)
	{
		m_fnGetErrorMessage();
		return FALSE;
	}
	else
		m_strLocalDirText = LocalDir;
	return TRUE;
}

BOOL CFtpClient::m_fnMakeRemoteDir(CString RemoteDir)
{
	if(m_pConnectionImg == NULL) return FALSE;
	try
	{
		BOOL brc;
		RemoteDir = RemoteDir+ _T("/");
		brc = m_pConnectionImg->CreateDirectory((LPCTSTR)RemoteDir);
		if(brc == FALSE){
			TRACE("MakeRemoteDir ERROR : %s (%s) >< %d \n", m_strAddress, RemoteDir, ::GetLastError());
			m_fnGetErrorMessage();
			return FALSE;
		}
	}
	catch(	CInternetException* ee)
	{
		TCHAR buff[256];
		ee->GetErrorMessage((LPTSTR)buff, 255);
		TRACE("MakeRemoteDir ERROR : %s (%s) >< %s \n", m_strAddress, RemoteDir, buff);
		ee->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CFtpClient::m_fnMakeLocalDir(CString LocalDir)
{
	if (GetFileAttributes( LocalDir) == -1)
	{
		CreateDirectory(LocalDir, NULL);
	}	
	return TRUE;
}

BOOL CFtpClient::m_fnRemoveRemoteDir(CString RemoteDir)
{
	if(m_pConnection == NULL) return FALSE;
	try
	{
		BOOL brc;
		brc = m_pConnection->RemoveDirectory((LPCTSTR)RemoteDir);
		if(brc == FALSE){
			TRACE("RemoveRemoteDir ERROR : %s (%s) >< %d \n", m_strAddress, RemoteDir, ::GetLastError());
			m_fnGetErrorMessage();
			return FALSE;
		}
	}
	catch(	CInternetException* ee)
	{
		TCHAR buff[256];
		ee->GetErrorMessage((LPTSTR)buff, 255);
		TRACE("RemoveRemoteDir ERROR : %s (%s) >< %s \n", m_strAddress, RemoteDir, buff);
		ee->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL CFtpClient::m_fnRemoveLocalDir(CString LocalDir)
{
	return ::RemoveDirectory(LocalDir);
}

BOOL CFtpClient::m_fnRemoteDel(CString RemoteFile)
{
	int iErrCode;
	if(m_pConnection == NULL) return FALSE;
	try
	{
		BOOL brc;
		brc = m_pConnection->Remove(RemoteFile);
		if(brc == FALSE){

			iErrCode = GetLastError();
			TRACE("RemoteDel ERROR : %s (%s) >< %d \n", m_strAddress, RemoteFile, ::GetLastError());	

			//m_fnPrintLog(
			//	LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE,
			//	_T("[Try] RemoteDel ERROR : %s (%s) >< %d \n"),m_strAddress,RemoteFile, ::GetLastError());

			return FALSE;
		}
	}
	catch(	CInternetException* ee)
	{
		TCHAR buff[256];
		ee->GetErrorMessage((LPTSTR)buff, 255);
		TRACE("RemoteDel ERROR : %s (%s) >< %s \n", m_strAddress, RemoteFile, buff);

		//m_fnPrintLog(
		//	LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE,
		//	_T("[Catch] RemoteDel ERROR : %s (%s) >< %s \n"),m_strAddress, RemoteFile, buff);

		ee->Delete();			
		return FALSE;
	}
	return TRUE;
}

BOOL CFtpClient::m_fnLocalDel(CString LocalFile)
{
	return ::DeleteFile(LocalFile);
}

BOOL CFtpClient::m_fnRemoteRename(CString RemoteOldFile, CString RemoteNewFile)
{
	if(m_pConnection == NULL) return FALSE;
	try
	{
		BOOL brc;
		brc = m_pConnection->Rename((LPCTSTR)RemoteOldFile,(LPCTSTR)RemoteNewFile);
		if(brc == FALSE){
			TRACE("RemoteRename ERROR : %s (%s) %s -> %d \n", m_strAddress, RemoteOldFile, RemoteNewFile, ::GetLastError());							
			return FALSE;
		}
	}
	catch(	CInternetException* ee)
	{
		TCHAR buff[256];
		ee->GetErrorMessage((LPTSTR)buff, 255);
		TRACE("RemoteRename ERROR : %s (%s) %s -> %s \n", m_strAddress, RemoteOldFile, RemoteNewFile, buff);
		ee->Delete();		
		return FALSE;
	}
	return TRUE;
}

BOOL CFtpClient::m_fnLocalRename(CString LocalOldFile, CString LocalNewFile)
{
	return ::MoveFile(LocalOldFile, LocalNewFile);
}

CString CFtpClient::m_fnGetRemoteCurDir()
{
	m_fnRPWD();
	return m_strRemoteDirText;
}

CString CFtpClient::m_fnGetLocalCurDir()
{
	m_fnLPWD();
	return m_strLocalDirText;
}

BOOL CFtpClient::m_fnRPWD()
{
	if(m_pConnection == NULL) return FALSE;
	try
	{
		BOOL brc;
		brc = m_pConnection->GetCurrentDirectory(m_strRemoteDirText);
		if(brc == FALSE){
			TRACE("RPWD ERROR : %s -> %d \n", m_strAddress, ::GetLastError());
			m_fnGetErrorMessage();
			return FALSE;
		}
	}
	catch(	CInternetException* ee)
	{
		TCHAR buff[256];
		ee->GetErrorMessage((LPTSTR)buff, 255);
		TRACE("RPWD ERROR : %s -> %s \n", m_strAddress, buff);
		ee->Delete();
		return FALSE;
	}

	return TRUE;
}

BOOL CFtpClient::m_fnLPWD()
{
	TCHAR buff[256];
	::GetCurrentDirectory(255, (LPTSTR)buff);
	m_strLocalDirText = buff;
	return TRUE;
}

BOOL CFtpClient::m_fnChkFtpConnect(CString RemoteURL)
{
	if(m_pConnection == NULL) return FALSE;
	try
	{
		BOOL brc;
		brc = m_pConnection->GetCurrentDirectoryAsURL(RemoteURL);
		if(brc == FALSE){
			TRACE("GetURL ERROR : %s -> %d \n", m_strAddress, ::GetLastError());
			m_fnGetErrorMessage();
			return FALSE;
		}
	}
	catch(	CInternetException* ee)
	{
		TCHAR buff[256];
		ee->GetErrorMessage((LPTSTR)buff, 255);
		TRACE("GetURL ERROR : %s -> %s \n", m_strAddress, buff);
		ee->Delete();
		return FALSE;
	}

	return TRUE;
}

int CFtpClient::m_fnGetErrorMessage()
{
	TCHAR *szRcode;
	int rCode;

	rCode = ::GetLastError();
	switch(rCode) {
		 case ERROR_ACCESS_DENIED          :    szRcode = _T("ERROR_ACCESS_DENIED          ");    break;
		 case ERROR_ADAP_HDW_ERR           :    szRcode = _T("ERROR_ADAP_HDW_ERR           ");    break;
		 case ERROR_ALREADY_ASSIGNED       :    szRcode = _T("ERROR_ALREADY_ASSIGNED       ");    break;
		 case ERROR_ARENA_TRASHED          :    szRcode = _T("ERROR_ARENA_TRASHED          ");    break;
		 case ERROR_BAD_COMMAND            :    szRcode = _T("ERROR_BAD_COMMAND            ");    break;
		 case ERROR_BAD_DEV_TYPE           :    szRcode = _T("ERROR_BAD_DEV_TYPE           ");    break;
		 case ERROR_BAD_ENVIRONMENT        :    szRcode = _T("ERROR_BAD_ENVIRONMENT        ");    break;
		 case ERROR_BAD_FORMAT             :    szRcode = _T("ERROR_BAD_FORMAT             ");    break;
		 case ERROR_BAD_LENGTH             :    szRcode = _T("ERROR_BAD_LENGTH             ");    break;
		 case ERROR_BAD_NET_NAME           :    szRcode = _T("ERROR_BAD_NET_NAME           ");    break;
		 case ERROR_BAD_NET_RESP           :    szRcode = _T("ERROR_BAD_NET_RESP           ");    break;
		 case ERROR_BAD_NETPATH            :    szRcode = _T("ERROR_BAD_NETPATH            ");    break;
		 case ERROR_BAD_REM_ADAP           :    szRcode = _T("ERROR_BAD_REM_ADAP           ");    break;
		 case ERROR_BAD_UNIT               :    szRcode = _T("ERROR_BAD_UNIT               ");    break;
		 case ERROR_CANNOT_MAKE            :    szRcode = _T("ERROR_CANNOT_MAKE            ");    break;
		 case ERROR_CRC                    :    szRcode = _T("ERROR_CRC                    ");    break;
		 case ERROR_CURRENT_DIRECTORY      :    szRcode = _T("ERROR_CURRENT_DIRECTORY      ");    break;
		 case ERROR_DEV_NOT_EXIST          :    szRcode = _T("ERROR_DEV_NOT_EXIST          ");    break;
		 case ERROR_DUP_NAME               :    szRcode = _T("ERROR_DUP_NAME               ");    break;
		 case ERROR_FAIL_I24               :    szRcode = _T("ERROR_FAIL_I24               ");    break;
		 case ERROR_FILE_EXISTS            :    szRcode = _T("ERROR_FILE_NOT_FOUND         ");    break;
		 case ERROR_GEN_FAILURE            :    szRcode = _T("ERROR_GEN_FAILURE            ");    break;
		 case ERROR_HANDLE_DISK_FULL       :    szRcode = _T("ERROR_HANDLE_DISK_FULL       ");    break;
		 case ERROR_HANDLE_EOF             :    szRcode = _T("ERROR_HANDLE_EOF             ");    break;
		 case ERROR_INVALID_ACCESS         :    szRcode = _T("ERROR_INVALID_ACCESS         ");    break;
		 case ERROR_INVALID_BLOCK          :    szRcode = _T("ERROR_INVALID_BLOCK          ");    break;
		 case ERROR_INVALID_DATA           :    szRcode = _T("ERROR_INVALID_DATA           ");    break;
		 case ERROR_INVALID_DRIVE          :    szRcode = _T("ERROR_INVALID_DRIVE          ");    break;
		 case ERROR_INVALID_FUNCTION       :    szRcode = _T("ERROR_INVALID_FUNCTION       ");    break;
		 case ERROR_INVALID_HANDLE         :    szRcode = _T("ERROR_INVALID_HANDLE         ");    break;
		 case ERROR_INVALID_PARAMETER      :    szRcode = _T("ERROR_INVALID_PARAMETER      ");    break;
		 case ERROR_INVALID_PASSWORD       :    szRcode = _T("ERROR_INVALID_PASSWORD       ");    break;
		 case ERROR_LOCK_VIOLATION         :    szRcode = _T("ERROR_LOCK_VIOLATION         ");    break;
		 case ERROR_NET_WRITE_FAULT        :    szRcode = _T("ERROR_NET_WRITE_FAULT        ");    break;
		 case ERROR_NETNAME_DELETED        :    szRcode = _T("ERROR_NETNAME_DELETED        ");    break;
		 case ERROR_NETWORK_ACCESS_DENIED  :    szRcode = _T("ERROR_NETWORK_ACCESS_DENIED  ");    break;
		 case ERROR_NETWORK_BUSY           :    szRcode = _T("ERROR_NETWORK_BUSY           ");    break;
		 case ERROR_NO_MORE_FILES          :    szRcode = _T("ERROR_NO_MORE_FILES          ");    break;
		 case ERROR_NO_PROC_SLOTS          :    szRcode = _T("ERROR_NO_PROC_SLOTS          ");    break;
		 case ERROR_NO_SPOOL_SPACE         :    szRcode = _T("ERROR_NO_SPOOL_SPACE         ");    break;
		 case ERROR_NOT_DOS_DISK           :    szRcode = _T("ERROR_NOT_DOS_DISK           ");    break;
		 case ERROR_NOT_ENOUGH_MEMORY      :    szRcode = _T("ERROR_NOT_ENOUGH_MEMORY      ");    break;
		 case ERROR_NOT_READY              :    szRcode = _T("ERROR_NOT_READY              ");    break;
		 case ERROR_NOT_SAME_DEVICE        :    szRcode = _T("ERROR_NOT_SAME_DEVICE        ");    break;
		 case ERROR_NOT_SUPPORTED          :    szRcode = _T("ERROR_NOT_SUPPORTED          ");    break;
		 case ERROR_OUT_OF_PAPER           :    szRcode = _T("ERROR_OUT_OF_PAPER           ");    break;
		 case ERROR_OUT_OF_STRUCTURES      :    szRcode = _T("ERROR_OUT_OF_STRUCTURES      ");    break;
		 case ERROR_OUTOFMEMORY            :    szRcode = _T("ERROR_OUTOFMEMORY            ");    break;
		 case ERROR_PATH_NOT_FOUND         :    szRcode = _T("ERROR_PATH_NOT_FOUND         ");    break;
		 case ERROR_PRINT_CANCELLED        :    szRcode = _T("ERROR_PRINT_CANCELLED        ");    break;
		 case ERROR_PRINTQ_FULL            :    szRcode = _T("ERROR_PRINTQ_FULL            ");    break;
		 case ERROR_READ_FAULT             :    szRcode = _T("ERROR_READ_FAULT             ");    break;
		 case ERROR_REDIR_PAUSED           :    szRcode = _T("ERROR_REDIR_PAUSED           ");    break;
		 case ERROR_REM_NOT_LIST           :    szRcode = _T("ERROR_REM_NOT_LIST           ");    break;
		 case ERROR_REQ_NOT_ACCEP          :    szRcode = _T("ERROR_REQ_NOT_ACCEP          ");    break;
		 case ERROR_SECTOR_NOT_FOUND       :    szRcode = _T("ERROR_SECTOR_NOT_FOUND       ");    break;
		 case ERROR_SEEK                   :    szRcode = _T("ERROR_SEEK                   ");    break;
		 case ERROR_SHARING_BUFFER_EXCEEDED:    szRcode = _T("ERROR_SHARING_BUFFER_EXCEEDED");    break;
		 case ERROR_SHARING_PAUSED         :    szRcode = _T("ERROR_SHARING_PAUSED         ");    break;
		 case ERROR_SHARING_VIOLATION      :    szRcode = _T("ERROR_SHARING_VIOLATION      ");    break;
		 case ERROR_TOO_MANY_CMDS          :    szRcode = _T("ERROR_TOO_MANY_CMDS          ");    break;
		 case ERROR_TOO_MANY_NAMES         :    szRcode = _T("ERROR_TOO_MANY_NAMES         ");    break;
		 case ERROR_TOO_MANY_OPEN_FILES    :    szRcode = _T("ERROR_TOO_MANY_OPEN_FILES    ");    break;
		 case ERROR_TOO_MANY_SESS          :    szRcode = _T("ERROR_TOO_MANY_SESS          ");    break;
		 case ERROR_UNEXP_NET_ERR          :    szRcode = _T("ERROR_UNEXP_NET_ERR          ");    break;
		 case ERROR_WRITE_FAULT            :    szRcode = _T("ERROR_WRITE_FAULT            ");    break;
		 case ERROR_WRITE_PROTECT          :    szRcode = _T("ERROR_WRITE_PROTECT          ");    break;
		 case ERROR_WRONG_DISK             :    szRcode = _T("ERROR_WRONG_DISK             ");    break;
          
		 default 			               :    szRcode = _T("DEFAULT                      ");    break;
	}        
	
	return rCode;
}

int CFtpClient::m_fnSetErrorMessage(CString strErrMsg, CString strGuestErrMsg, int LogFlag)
{
	TCHAR *szRcode;
	int rCode;	
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	rCode = ::GetLastError();
		 
	switch(rCode) {
	case ERROR_SUCCESS				  :    szRcode = _T("ERROR_SUCCESS                ");    break;
	case ERROR_ACCESS_DENIED          :    szRcode = _T("ERROR_ACCESS_DENIED          ");    break;
	case ERROR_ADAP_HDW_ERR           :    szRcode = _T("ERROR_ADAP_HDW_ERR           ");    break;
	case ERROR_ALREADY_ASSIGNED       :    szRcode = _T("ERROR_ALREADY_ASSIGNED       ");    break;
	case ERROR_ARENA_TRASHED          :    szRcode = _T("ERROR_ARENA_TRASHED          ");    break;
	case ERROR_BAD_COMMAND            :    szRcode = _T("ERROR_BAD_COMMAND            ");    break;
	case ERROR_BAD_DEV_TYPE           :    szRcode = _T("ERROR_BAD_DEV_TYPE           ");    break;
	case ERROR_BAD_ENVIRONMENT        :    szRcode = _T("ERROR_BAD_ENVIRONMENT        ");    break;
	case ERROR_BAD_FORMAT             :    szRcode = _T("ERROR_BAD_FORMAT             ");    break;
	case ERROR_BAD_LENGTH             :    szRcode = _T("ERROR_BAD_LENGTH             ");    break;
	case ERROR_BAD_NET_NAME           :    szRcode = _T("ERROR_BAD_NET_NAME           ");    break;
	case ERROR_BAD_NET_RESP           :    szRcode = _T("ERROR_BAD_NET_RESP           ");    break;
	case ERROR_BAD_NETPATH            :    szRcode = _T("ERROR_BAD_NETPATH            ");    break;
	case ERROR_BAD_REM_ADAP           :    szRcode = _T("ERROR_BAD_REM_ADAP           ");    break;
	case ERROR_BAD_UNIT               :    szRcode = _T("ERROR_BAD_UNIT               ");    break;
	case ERROR_CANNOT_MAKE            :    szRcode = _T("ERROR_CANNOT_MAKE            ");    break;
	case ERROR_CRC                    :    szRcode = _T("ERROR_CRC                    ");    break;
	case ERROR_CURRENT_DIRECTORY      :    szRcode = _T("ERROR_CURRENT_DIRECTORY      ");    break;
	case ERROR_DEV_NOT_EXIST          :    szRcode = _T("ERROR_DEV_NOT_EXIST          ");    break;
	case ERROR_DUP_NAME               :    szRcode = _T("ERROR_DUP_NAME               ");    break;
	case ERROR_FAIL_I24               :    szRcode = _T("ERROR_FAIL_I24               ");    break;
	case ERROR_FILE_EXISTS            :    szRcode = _T("ERROR_FILE_NOT_FOUND         ");    break;
	case ERROR_GEN_FAILURE            :    szRcode = _T("ERROR_GEN_FAILURE            ");    break;
	case ERROR_HANDLE_DISK_FULL       :    szRcode = _T("ERROR_HANDLE_DISK_FULL       ");    break;
	case ERROR_HANDLE_EOF             :    szRcode = _T("ERROR_HANDLE_EOF             ");    break;
	case ERROR_INVALID_ACCESS         :    szRcode = _T("ERROR_INVALID_ACCESS         ");    break;
	case ERROR_INVALID_BLOCK          :    szRcode = _T("ERROR_INVALID_BLOCK          ");    break;
	case ERROR_INVALID_DATA           :    szRcode = _T("ERROR_INVALID_DATA           ");    break;
	case ERROR_INVALID_DRIVE          :    szRcode = _T("ERROR_INVALID_DRIVE          ");    break;
	case ERROR_INVALID_FUNCTION       :    szRcode = _T("ERROR_INVALID_FUNCTION       ");    break;
	case ERROR_INVALID_HANDLE         :    szRcode = _T("ERROR_INVALID_HANDLE         ");    break;
	case ERROR_INVALID_PARAMETER      :    szRcode = _T("ERROR_INVALID_PARAMETER      ");    break;
	case ERROR_INVALID_PASSWORD       :    szRcode = _T("ERROR_INVALID_PASSWORD       ");    break;
	case ERROR_LOCK_VIOLATION         :    szRcode = _T("ERROR_LOCK_VIOLATION         ");    break;
	case ERROR_NET_WRITE_FAULT        :    szRcode = _T("ERROR_NET_WRITE_FAULT        ");    break;
	case ERROR_NETNAME_DELETED        :    szRcode = _T("ERROR_NETNAME_DELETED        ");    break;
	case ERROR_NETWORK_ACCESS_DENIED  :    szRcode = _T("ERROR_NETWORK_ACCESS_DENIED  ");    break;
	case ERROR_NETWORK_BUSY           :    szRcode = _T("ERROR_NETWORK_BUSY           ");    break;
	case ERROR_NO_MORE_FILES          :    szRcode = _T("ERROR_NO_MORE_FILES          ");    break;
	case ERROR_NO_PROC_SLOTS          :    szRcode = _T("ERROR_NO_PROC_SLOTS          ");    break;
	case ERROR_NO_SPOOL_SPACE         :    szRcode = _T("ERROR_NO_SPOOL_SPACE         ");    break;
	case ERROR_NOT_DOS_DISK           :    szRcode = _T("ERROR_NOT_DOS_DISK           ");    break;
	case ERROR_NOT_ENOUGH_MEMORY      :    szRcode = _T("ERROR_NOT_ENOUGH_MEMORY      ");    break;
	case ERROR_NOT_READY              :    szRcode = _T("ERROR_NOT_READY              ");    break;
	case ERROR_NOT_SAME_DEVICE        :    szRcode = _T("ERROR_NOT_SAME_DEVICE        ");    break;
	case ERROR_NOT_SUPPORTED          :    szRcode = _T("ERROR_NOT_SUPPORTED          ");    break;
	case ERROR_OUT_OF_PAPER           :    szRcode = _T("ERROR_OUT_OF_PAPER           ");    break;
	case ERROR_OUT_OF_STRUCTURES      :    szRcode = _T("ERROR_OUT_OF_STRUCTURES      ");    break;
	case ERROR_OUTOFMEMORY            :    szRcode = _T("ERROR_OUTOFMEMORY            ");    break;
	case ERROR_PATH_NOT_FOUND         :    szRcode = _T("ERROR_PATH_NOT_FOUND         ");    break;
	case ERROR_PRINT_CANCELLED        :    szRcode = _T("ERROR_PRINT_CANCELLED        ");    break;
	case ERROR_PRINTQ_FULL            :    szRcode = _T("ERROR_PRINTQ_FULL            ");    break;
	case ERROR_READ_FAULT             :    szRcode = _T("ERROR_READ_FAULT             ");    break;
	case ERROR_REDIR_PAUSED           :    szRcode = _T("ERROR_REDIR_PAUSED           ");    break;
	case ERROR_REM_NOT_LIST           :    szRcode = _T("ERROR_REM_NOT_LIST           ");    break;
	case ERROR_REQ_NOT_ACCEP          :    szRcode = _T("ERROR_REQ_NOT_ACCEP          ");    break;
	case ERROR_SECTOR_NOT_FOUND       :    szRcode = _T("ERROR_SECTOR_NOT_FOUND       ");    break;
	case ERROR_SEEK                   :    szRcode = _T("ERROR_SEEK                   ");    break;
	case ERROR_SHARING_BUFFER_EXCEEDED:    szRcode = _T("ERROR_SHARING_BUFFER_EXCEEDED");    break;
	case ERROR_SHARING_PAUSED         :    szRcode = _T("ERROR_SHARING_PAUSED         ");    break;
	case ERROR_SHARING_VIOLATION      :    szRcode = _T("ERROR_SHARING_VIOLATION      ");    break;
	case ERROR_TOO_MANY_CMDS          :    szRcode = _T("ERROR_TOO_MANY_CMDS          ");    break;
	case ERROR_TOO_MANY_NAMES         :    szRcode = _T("ERROR_TOO_MANY_NAMES         ");    break;
	case ERROR_TOO_MANY_OPEN_FILES    :    szRcode = _T("ERROR_TOO_MANY_OPEN_FILES    ");    break;
	case ERROR_TOO_MANY_SESS          :    szRcode = _T("ERROR_TOO_MANY_SESS          ");    break;
	case ERROR_UNEXP_NET_ERR          :    szRcode = _T("ERROR_UNEXP_NET_ERR          ");    break;
	case ERROR_WRITE_FAULT            :    szRcode = _T("ERROR_WRITE_FAULT            ");    break;
	case ERROR_WRITE_PROTECT          :    szRcode = _T("ERROR_WRITE_PROTECT          ");    break;
	case ERROR_WRONG_DISK             :    szRcode = _T("ERROR_WRONG_DISK             ");    break;
	case 12002L			              :    szRcode = _T("ERROR_INTERNET_TIMEOUT       ");    break;
	case 12003L			              :    szRcode = _T("ERROR_INTERNET_EXTENDED_ERROR");    break;
    case 12031L			              :    szRcode = _T("The connection with the server was reset");    break;
	default 			              :    szRcode = _T("DEFAULT                      ");    break;
	}        
	//m_pFtpLogWriter->m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE,
		//_T(" NG P5 Receive file failed-%d-%s \n"), rCode, szRcode);		
	if (LogFlag == QMS_DAT_LOG){
		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, strErrMsg, rCode, szRcode);
		G_AddLog_QMS(_T("%s,%d,%s"),strGuestErrMsg,rCode,szRcode);
	}
	else if (LogFlag == QMS_IMG_LOG){
		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, strErrMsg, rCode, szRcode);
		G_AddLog_IMG_QMS(_T("%s,%d,%s"),strGuestErrMsg,rCode,szRcode);
	}
	
	return rCode;
}

BOOL CFtpClient::m_fnQMSUpLoadImg(LPCTSTR lDir, LPCTSTR rDir)
{
	CFileFind finder;
	CString	cFileName, cFileDate, cLDIR, cRDIR,strVal;
	CString strtemp, resToken;	
	int curPos;
	/*CTime time = CTime::GetCurrentTime();
	CString strNowDate =  time.Format("%Y%m%d");
	CString strNowTime  = time.Format("%H%M%S"); */
//	unsigned long size;
	BOOL bWorking,brc;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strPanelID1, strPanelID2, testurl, strStepId;
	TCHAR chRead[255] = {0};
	cLDIR = lDir;

	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	//long lRet;
	bWorking = finder.FindFile(lDir);	// 모든화일

	while(bWorking == TRUE)
	{

		//todo.. 패널이 두장 짜리 일때 어떻게 처리 할지 생각할 필요가 있음
		// 아마 이미지는 별도의 스텝아이디와 패널명을 가진 디렉토리를 만들어서 업로드 하는게 바람직하다고 봄
		// 
		//lRet = pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STEPID, 5, strStepId);
		////lRet = pFrame->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STEPID, 20, stStepID);
		//if (strStepId == _T(""))
		//{
		//	strStepId = _T("AAAAA");
		//}

		memset(chRead,0X00,sizeof(chRead));	
		pFrame->m_iniConfigSet.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("51400"), (LPTSTR)chRead, 255);

		strStepId  = chRead;

		//strStepId=_T("51400");

		if(m_fnMakeRemoteDir(_T("/") + strStepId) == FALSE)	{}			
		//return FALSE;
		strVal = _T("/") + strStepId;

		strVal = _T("/") + m_strNowDate;
		//cFileDate = time.Format("%Y%m%d");
		if(m_fnMakeRemoteDir(_T("/") + m_strNowDate) == FALSE)	{}			
			//return FALSE;
		strVal = _T("/") + m_strNowDate;
		
		pFrame->m_fnGetPanelID_1(0);		
		//strPanelID1 = _T("PNL000001");//	
		if(m_fnMakeRemoteDir(strVal + _T("/") + strPanelID1) == FALSE) {}
			//return FALSE;
		strVal += _T("/") + strPanelID1;

		//cFileDate = time.Format("%H%M%S");
		if(m_fnMakeRemoteDir(strVal + _T("/") + m_strNowTime) == FALSE) {}
			//return FALSE;

		strVal += _T("/") + m_strNowTime;

		//m_fnUpLoadDir(cLDIR, cRDIR);
		//strVal + _T("/") + rDir;
		strtemp = lDir;
		curPos = 0;
		resToken= strtemp.Tokenize(_T("\\"),curPos);
		while (resToken != "")
		{
			if (curPos >= strtemp.GetLength())	break;
			resToken= strtemp.Tokenize(_T("\\"),curPos);
		}
		brc = m_pConnection->PutFile(lDir,strVal+_T("/") + resToken,FTP_TRANSFER_TYPE_BINARY);		//파일 업로드			
		if (brc) 
			return TRUE;			    // 업로드 성공 시 TRUE를 RETRUN 한다.	
		if (!brc) 
			return FALSE; // 2회쩨 업로드 실패 시 FALSE를 RETRUN한다.			
	}
	finder.Close();
	return TRUE;
}

CString CFtpClient::m_fnUpLoadDir(LPCTSTR lDir, LPCTSTR rDir)
{
	CFileFind finder;
	CString cTempDIR = _T(""), cTempFlder = _T("");	//2018.09.08 add by mby
	CString	cFileName, cFileDate, cLDIR, cRDIR, cCpyDir, cCpyLDir, resToken, cTemp, cTokenVal, strTemp, strGuestMsg;	
	CTime time;
	unsigned long size;
	BOOL bWorking;
	BOOL brc, brc2,bCpyFlg = FALSE;
	int nLoopCnt = 0,curPos =0 ;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	cTokenVal = lDir;
	resToken= cTokenVal.Tokenize(_T("\\"),curPos);
	
	try
	{
		while (resToken != "")
		{	
			if(nLoopCnt == 0)
			{				
				cTemp.Format(_T("%s"),_T("D:\\"));
				cTempDIR.Format(_T("%s"),_T("Z:\\"));//2018.09.08 add by mby				
			}
			else if (nLoopCnt == 2)
			{
				cTemp.Format(_T("%s"),_T("\\i_send"));
				cTempDIR.Format(_T("%s"),_T("\\i_send"));//2018.09.08 add by mby
			}
			else
			{
				cTemp.Format(_T("%s"),_T("\\") + resToken);
				cTempDIR.Format(_T("%s"),_T("\\") + resToken);//2018.09.08 add by mby
			}

			cCpyDir.Append(cTemp);
			cTempFlder.Append(cTempDIR);//2018.09.08 add by mby

			resToken = cTokenVal.Tokenize(_T("\\"),curPos);

			nLoopCnt++;
		}	

		m_fnMakeLocalDir(cCpyDir);

		cLDIR = lDir;
		cCpyLDir = lDir;

		if(cLDIR.Right(1) != "\\")
			cLDIR += "\\*.*";
		else
			cLDIR += "*.*";

		bWorking = finder.FindFile(cLDIR);	// 모든화일

		while(bWorking == TRUE)
		{
			bWorking = finder.FindNextFile();	// 화일하나를 찾음

			if(finder.IsDots()) continue;	// "."이나  ".." 이면 다음으로 넘어감

			// 화일이름
			cFileName = finder.GetFileName();	// 화일이름 얻기

			cLDIR = lDir;
			if(cLDIR.Right(1) != "\\")
				cLDIR += "\\";
			cLDIR += cFileName;

			cRDIR = rDir;
			if(cRDIR.Right(1) != "/")
				cRDIR += "/";
			cRDIR += cFileName;

			if(finder.IsDirectory())
			{
				if(m_fnMakeRemoteDir(cRDIR) == FALSE)
				{
					strGuestMsg.Format(_T("OK,P2,%s"),rDir);
					m_fnSetErrorMessage(_T(" NG P2 Make Remote Directory failed-%d-%s \n"), strGuestMsg, QMS_IMG_LOG);
					//return FALSE;
				}
				else
					m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" OK P2 Transfer Image Path - %s \n"), rDir);
					G_AddLog_IMG_QMS(_T("OK,P2,%s"),rDir);

					cTokenVal = m_fnUpLoadDir(cLDIR, cRDIR);
			}
			else 
			{
				// 화일크기
				size = finder.GetLength();
				// 화일날짜
				finder.GetLastWriteTime(time);
				cFileDate = time.Format("%y%m%d %H%M");
				bool a_bool = FALSE;
				 
				if(cFileName.Find(EXTENSION_JPG))
				{
					brc = m_pConnectionImg->PutFile(cLDIR, cRDIR,FTP_TRANSFER_TYPE_BINARY);		//파일 업로드	
				
 					if (!brc) 
					{
 						strTemp.Format(_T("NG P2 Image file tranfer failed - %s / -%d-%s \n"),cRDIR);
 						strGuestMsg.Format(_T("NG,P2,%s"),cRDIR);
 						m_fnSetErrorMessage(strTemp, strGuestMsg, QMS_IMG_LOG);

						//
// 						TCHAR* strWaitImgFolderPath = (LPTSTR)(LPCTSTR)cLDIR;
// 						CString strBuf = _T("");
// 						TCHAR strResPath[255] = {0,};
// 						int nCnt = 0;
// 
// 						for(int i = cLDIR.GetLength()-1; i>0; i--)
// 						{
// 							strBuf.Format(_T("%c"),strWaitImgFolderPath[i]);
// 							if( strBuf == _T("\\") )
// 								break;
// 							else
// 								nCnt++;
// 						}
// 						for(int i = 0; i< (cLDIR.GetLength()-1) - nCnt; i++ )
// 							strResPath[i] = strWaitImgFolderPath[i];
// 
// 						strBuf.Format(_T("%s"),strResPath);
// 
// 						pFrame->m_DataManager.m_fnMoveFolder(strBuf,cCpyDir);	//2017.09.15 SUMMARY UI 상 IMG를 보여주기 위한 TEST
						//

 						return FALSE;
 					}// 2회째 업로드 실패 시 FALSE를 RETRUN한다.

					brc = CopyFile(cLDIR , cCpyDir + _T("\\") + cFileName, 0);
					
					if(!cFileName.Compare(_T("ALLMAP_000.jpg")))	// 1.디버깅 후 cFileName값 확인.  // 2.strcmp()확인 필요. 																	
					{												// 3.if(!cFileName.Compare(_T("allmap_000.jpg")))이미지가 같으면 0을 반환.						
						a_bool = TRUE;
					}
					else if(!cFileName.Compare(_T("C1_270.jpg")))
					{
						a_bool = TRUE;
					}
					else if(!cFileName.Compare(_T("C2_270.jpg")))
					{
						a_bool = TRUE;
					}
					else if(!cFileName.Compare(_T("C3_090.jpg")))					
					{
						a_bool = TRUE;
					}
					else if(!cFileName.Compare(_T("C4_090.jpg")))					
					{
						a_bool = TRUE;
					}
					else
					{
				 		//20180912 #23호기 네트워크 이상으로 대기 _ryu
						//m_fnMakeLocalDir(cTempFlder);	 //2018.09.08 add by mby
						//pFrame->m_LogMng.m_fnMakeDirectory(cTempFlder);
				
					}

					if(a_bool == FALSE)
					{															
						//brc2 = CopyFile(cLDIR , cTempFlder + _T("\\") + cFileName, 0);		// 2018.09.08 add by mby
					}					
					
					if (brc)
					{
						DeleteFile(cLDIR);

						m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" OK P3 Transfer successful Image - %s \n"), cRDIR);
						G_AddLog_IMG_QMS(_T("OK,P3,%s"),cRDIR);
						//bCpyFlg =TRUE;
						bCpyFlg = FALSE;	//2017.09.15 SUMMARY UI 상 IMG를 보여주기 위한 TEST
					}					
				}
			}
		}

		finder.Close();
		
		if (bCpyFlg)
		{		
			//curPos = 0;
			cTokenVal = cCpyLDir;
			
			/*cTokenVal.Append( _T("\\"));
			while (cTokenVal != "")
			{	
			if (!cTokenVal.Compare(pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iUnsent))
			break;	

			brc = RemoveDirectory(cTokenVal);
			resToken = pFrame->m_DataManager.m_fnLastToken(cTokenVal, _T("\\"));		
			cTokenVal = cTokenVal.Left(cTokenVal.GetLength() - (resToken.GetLength() + 1));							
			}			*/

			//파일 업로드 완료
		}
		return cTokenVal;
	}
	catch (CFileException* e)
	{
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[CFileException] m_fnUpLoadDir  \n"));
		G_AddLog(2, 2, _T("[QMS FILE UPLOAD] m_fnUpLoadDir Failed !! - CFileException "));
		e->Delete();
		return cTokenVal;
	}
	catch (CException* e)
	{
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[CException] m_fnUpLoadDir  \n"));
		G_AddLog(2, 2, _T("[MS FILE UPLOAD] m_fnUpLoadDir Failed !! - CException "));
		e->Delete();
		return cTokenVal;
	}
}

//BOOL CFtpClient::m_fnFtpConnectProc(int iIpCnt)
//{	
//	int	iConctCnt = 0;
//	BOOL rc;
//	while(iConctCnt < iIpCnt )
//	{				
//		rc = m_fnFTPConnect();			
//		if(rc == FALSE)	{
//			m_fnInitUploadThread();		
//			return FALSE;
//		}
//		else
//			break;			
//		iConctCnt++;
//	}
//	return TRUE;
//}

BOOL CFtpClient::m_fnFtpConnectProc(int iIpCnt, STRU_FTP_SEVER_INFO &m_stCurFtpInfo, STRU_FTP_PATH &m_stCurFtpPath)
{
	int	iConctCnt = 0;
	BOOL bConnectFlag;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	wchar_t chRead[255] = {0};
	
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	m_stCurFtpPath.strDir_Default = pFrame->m_stFtpInfo.stPath->strDir_Default;	
	m_stCurFtpPath.strDir_Send = pFrame->m_stFtpInfo.stPath->strDir_Send;
	m_stCurFtpPath.strDir_Wait = pFrame->m_stFtpInfo.stPath->strDir_Wait;
	m_stCurFtpPath.strDir_iWait = pFrame->m_stFtpInfo.stPath->strDir_iWait;
	m_stCurFtpPath.strDir_iSend = pFrame->m_stFtpInfo.stPath->strDir_iSend;
	m_stCurFtpPath.strDir_iLog = pFrame->m_stFtpInfo.stPath->strDir_iLog;

	m_stCurFtpPath.strDir_Error =  pFrame->m_stFtpInfo.stPath->strDir_Error;
	m_stCurFtpPath.strDir_Log =  pFrame->m_stFtpInfo.stPath->strDir_Log;

	while(iConctCnt < iIpCnt )
	{				
		m_stCurFtpInfo.strServer = pFrame->m_stFtpInfo.stInfo[iConctCnt].strServer;	
		m_stCurFtpInfo.strID = pFrame->m_stFtpInfo.stInfo[iConctCnt].strID;
		m_stCurFtpInfo.strPW = pFrame->m_stFtpInfo.stInfo[iConctCnt].strPW;

		m_stCurFtpInfo.nFTP_Port =  pFrame->m_stFtpInfo.stInfo[iConctCnt].nFTP_Port;
		m_stCurFtpInfo.strRoot =  pFrame->m_stFtpInfo.stInfo[iConctCnt].strRoot;

		bConnectFlag = m_fnOpen(m_stCurFtpInfo.strServer, m_stCurFtpInfo.strID,
			m_stCurFtpInfo.strPW, m_stCurFtpInfo.nFTP_Port, 1);		
		if(bConnectFlag)
			break;			
		iConctCnt++;
	}

	if (!bConnectFlag)
	{		
		//todo.. 로그 기입 모든 오류
		m_fnPrintLog(
			LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE,
			_T(" m_fnFtpConnectProc NG P1 connection IP : %s / port : %d -530-Not Logged IN server list all failed to connect.\n")
			,m_stCurFtpInfo.strServer, m_stCurFtpInfo.nFTP_Port);

		memset(chRead,0,sizeof(chRead));			
		wsprintf(chRead, _T(" NG P1 connection IP : %s / port : %d -530-Not Logged IN server list all failed to connect.\n")
			,m_stCurFtpInfo.strServer, m_stCurFtpInfo.nFTP_Port );
	//	AfxMessageBox(chRead, MB_OK|MB_ICONSTOP);

		G_AddLog_QMS(_T("NG,P1,%s:%d -530-Not Logged IN server list all failed to connect."),m_stCurFtpInfo.strServer,m_stCurFtpInfo.nFTP_Port);

		//2015.11.03 Mr.Chu
		//QMS File Upload 실패 시 PLC Alarm 발생
		if(pFrame->m_stParamInfo.bUseCIM2){	//2021.03.04 add by mby
			if(pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)		pView->m_pDlgPLC->m_fnSetDevice(ADDR_QMS_UPLOAD_FAIL,1);
		}

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[bConnectFlag] m_fnFtpConnectProc m_pConnection Init \n"));
		m_fnInitUploadThread();	
		return FALSE;
	}
	return TRUE;
}

BOOL CFtpClient::m_fnFtpConnectImgProc(int iIpCnt, STRU_FTP_SEVER_INFO &m_stCurFtpInfo, STRU_FTP_PATH &m_stCurFtpPath)
{	
	int	iConctCnt = 0;
	BOOL bConnectFlag;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	wchar_t chRead[255] = {0};
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	m_stCurFtpPath.strDir_Default = pFrame->m_stFtpInfo.stPath->strDir_Default;	
	m_stCurFtpPath.strDir_Send = pFrame->m_stFtpInfo.stPath->strDir_Send;
	m_stCurFtpPath.strDir_Wait = pFrame->m_stFtpInfo.stPath->strDir_Wait;
	m_stCurFtpPath.strDir_iWait = pFrame->m_stFtpInfo.stPath->strDir_iWait;
	m_stCurFtpPath.strDir_iSend = pFrame->m_stFtpInfo.stPath->strDir_iSend;
	m_stCurFtpPath.strDir_iLog = pFrame->m_stFtpInfo.stPath->strDir_iLog;

	m_stCurFtpPath.strDir_Error =  pFrame->m_stFtpInfo.stPath->strDir_Error;
	m_stCurFtpPath.strDir_Log =  pFrame->m_stFtpInfo.stPath->strDir_Log;

	while(iConctCnt < iIpCnt )
	{				
		m_stCurFtpInfo.strServer = pFrame->m_stFtpInfo.stInfo[iConctCnt].strServer;	
		m_stCurFtpInfo.strImgID = pFrame->m_stFtpInfo.stInfo[iConctCnt].strImgID;
		m_stCurFtpInfo.strImgPW = pFrame->m_stFtpInfo.stInfo[iConctCnt].strImgPW;

		m_stCurFtpInfo.nFTP_Port =  pFrame->m_stFtpInfo.stInfo[iConctCnt].nFTP_Port;
		m_stCurFtpInfo.strRoot =  pFrame->m_stFtpInfo.stInfo[iConctCnt].strRoot;

		bConnectFlag = m_fnOpenImg(m_stCurFtpInfo.strServer, m_stCurFtpInfo.strImgID,
			m_stCurFtpInfo.strImgPW, m_stCurFtpInfo.nFTP_Port, 2);		
		if(bConnectFlag)
			break;			
		iConctCnt++;
	}

	if (!bConnectFlag)
	{		
		//todo.. 로그 기입 모든 오류
		m_fnPrintLog(
			LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE,
			_T(" m_fnFtpConnectImgProc NG P1 connection IP : %s / port : %d -530-Not Logged IN server list all failed to connect.\n")
			,m_stCurFtpInfo.strServer, m_stCurFtpInfo.nFTP_Port);

		memset(chRead,0,sizeof(chRead));			
		wsprintf(chRead, _T(" NG P1 connection IP : %s / port : %d -530-Not Logged IN server list all failed to connect.\n")
			,m_stCurFtpInfo.strServer, m_stCurFtpInfo.nFTP_Port );
		AfxMessageBox(chRead, MB_OK|MB_ICONSTOP);

		G_AddLog_IMG_QMS(_T("NG,P1,%s:%d"),m_stCurFtpInfo.strServer,m_stCurFtpInfo.nFTP_Port);

		//2015.11.03 Mr.Chu
		//QMS File Upload 실패 시 PLC Alarm 발생
		if(pFrame->m_stParamInfo.bUseCIM2){	//2021.03.04 add by mby
			if(pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)		pView->m_pDlgPLC->m_fnSetDevice(ADDR_QMS_UPLOAD_FAIL,1);
		}

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[bConnectFlag] m_fnFtpConnectImgProc m_pConnectionImg Init"));
		m_fnInitUploadImgThread();	
		return FALSE;
	}
	return TRUE;
}

unsigned __stdcall CFtpClient::m_fnFTPConnect(LPVOID pArg) 
{

		return TRUE;
}

//2015.11.20
unsigned __stdcall CFtpClient::m_fnFTPConnectCheck(LPVOID pArg) 
{
	BOOL bConnectFlag = FALSE;	
	CString strLocalTmp, strRemoteTmp ,  strLocalDat, strRemoteDat , strLocalAns, strRemoteAns ,strRename;
	int ipCnt = 0;	
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	CFtpClient *pFtp = (CFtpClient *)pArg;
	wchar_t chRead[255] = {0};

	STRU_FTP_SEVER_INFO stCurFtpInfo;// = new STRU_FTP_SEVER_INFO;

	while (ipCnt < pFrame->m_stCurFtpCond.iIP_Cnt)
	{		
		stCurFtpInfo.strServer = pFrame->m_stFtpInfo.stInfo[ipCnt].strServer;	
		stCurFtpInfo.strID =  pFrame->m_stFtpInfo.stInfo[ipCnt].strID;
		stCurFtpInfo.strPW = pFrame->m_stFtpInfo.stInfo[ipCnt].strPW;
		stCurFtpInfo.nFTP_Port =  pFrame->m_stFtpInfo.stInfo[ipCnt].nFTP_Port;		
		stCurFtpInfo.strRoot =  pFrame->m_stFtpInfo.stInfo[ipCnt].strRoot;

		ipCnt++;

		bConnectFlag = pFtp->m_fnOpen(stCurFtpInfo.strServer, stCurFtpInfo.strID 
			, stCurFtpInfo.strPW, stCurFtpInfo.nFTP_Port, 1);
		if(bConnectFlag) break;								
	}

	ipCnt = 0;

	pFtp->m_fnInitConnectionThread();

	if (!bConnectFlag)
		return FALSE;
	else
		return TRUE;
}


unsigned __stdcall  CFtpClient::m_fnFTPUploadProc(LPVOID pArg )
{	
	BOOL rc = FALSE;
	CFtpClient *pFtp = (CFtpClient *)pArg;	
	//CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	//int iLogWriteTerm = 0, iPeriodDay = 0;
	//TSTRING strPath = _T(""), strLogName = _T("");
	//wchar_t chRead[255] = {0};

	////pFtp->m_fnSetLogStataus(strPath, strLogName, iPeriodDay, iLogWriteTerm );
	//iLogWriteTerm = pFrame->m_iniConfigFtp.GetProfileIntW(FTP_LOG_STATE, LOG_WRITE_TERM, 0);	
	//iPeriodDay = pFrame->m_iniConfigFtp.GetProfileIntW(FTP_LOG_STATE, KEY_FTP_DIR_PERIOD_DAY, 30);
	//strPath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Log;
	//pFrame->m_iniConfigFtp.GetProfileStringW(FTP_LOG_STATE, KEY_FTP_DIR_LOG_NAME, _T(""), (LPTSTR)chRead, 255);
	//
	//strLogName = chRead;

	rc = pFtp->QmsFileUpload(pArg);
	//rc = pFtp->QmsImgUplaod(pArg);
	return rc;
}

BOOL CFtpClient::QmsFileUpload(LPVOID pArg )
{
	BOOL rc = FALSE;
	int iAnsCnt=0 ,iConctCnt=0, nRetryCnt=0;
	int szCode =0, nLoopCnt = 0;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	CFtpClient *pFtp = (CFtpClient *)pArg;	
	CFileStatus status;
	TCHAR	sWk[128];			
	CFile pFile;
	CString strLocalTmp, strRemoteTmp ,  strLocalDat, strRemoteDat , strLocalAns, strRemoteAns ,strRename, strGuestMsg = _T("");
	CString strLocalUnsent, strLocalError, strErrorCode;
	CString strErrPath;

	STRU_FTP_SEVER_INFO stCurFtpInfo;// = new STRU_FTP_SEVER_INFO;
	STRU_FTP_PATH		stCurFtpPath;// = new STRU_FTP_PATH;

	wchar_t cErrPathTmp[255] = {0};
	try
	{		
		CFileFind finder;
		CString	 strTmp, strFileName, strVal, strPathTmp;
		CTime time;
		int curPos = 0;
		//unsigned long size;
		BOOL bWorking;		
		//strLocalTmp =  pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Unsent; 
		//strLocalTmp += _T("\\*.*");

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[QmsFileUpload] Finding QMS Data in Unsent Folder... \n"));

		while(TRUE)
		{
			//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[QmsFileUpload] Unsent Folder Checking... \n"));
			BOOL bFtpFail = FALSE;		//2017.07.13 add by mby
			//strLocalTmp =  pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Unsent; 
			strLocalTmp = _T("D:\\QMS\\wait");
			strLocalTmp += _T("\\*.*");
			bWorking = finder.FindFile(strLocalTmp);	

			if (bWorking == FALSE)
			{
				//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[QmsFileUpload] Unsent Folder is Empty ! ! ! \n"));
				break;
			}

			while(bWorking == TRUE)
			{
				//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[QmsFileUpload] Finding File in Unsent Folder... \n"));

				bWorking = finder.FindNextFile();
				if(finder.IsDots()) continue;
				strVal = finder.GetFileName();	
				if (strVal.Find(EXTENSION_TMP))
				{
					//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[QmsFileUpload] Find File OK... [ %s ] \n"),strVal);

					stCurFtpInfo.init();
					stCurFtpPath.init();
					iConctCnt = 0;

					// Connect to QMS Server
					rc = FALSE;
					while(nRetryCnt < RETRY_COUNT)	{
						rc = pFtp->m_fnFtpConnectProc(pFrame->m_stCurFtpCond.iIP_Cnt,stCurFtpInfo, stCurFtpPath);
						if(!rc) 
						{
							if( nRetryCnt == RETRY_COUNT - 1 )
								return FALSE;
							Sleep(3 * 1000);	//Sleep(10 * 1000);
							nRetryCnt++;
						}
						else
						{
							nRetryCnt = NULL;
							break;
						}
					}
					//

					strLocalTmp =  stCurFtpPath.strDir_Default +  stCurFtpPath.strDir_Wait + strVal;    //pFtp->m_strFileName
					strRemoteTmp = stCurFtpInfo.strRoot + strVal;    //pFtp->m_strFileName

					if(!CFile::GetStatus(strLocalTmp, status)) {
						memset(sWk,0,sizeof(sWk));			
						wsprintf(sWk,  _T("Not Exist File Name : %s "), strVal);//pFtp->m_strFileName);
					//	AfxMessageBox(sWk, MB_OK|MB_ICONSTOP);
						//pFtp->m_fnInitUploadThread();		
						break;
					}
					iAnsCnt =0 ;

					//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" Uploading Data... \n") );
					while (iAnsCnt < QMS_UPLOAD_CNT_MAX) {
						rc = pFtp->m_fnUpLoadData(strLocalTmp, strRemoteTmp, stCurFtpInfo.strRoot);		//파일 업로드		
						if (rc) break;			    //  성공 시 TRUE를 RETRUN 한다.	
						if(rc == FALSE && iAnsCnt < QMS_UPLOAD_CNT_MAX - 1 )
						{
							//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
							//	_T("[OK] QmsFileUpload Upload Retry1 \n"));
							pFtp->m_fnSetErrorMessage(_T(" NG P2 Upload Temporary failed-%d-%s \n"), _T("NG,P2,Upload Temporary failed"), QMS_DAT_LOG);								
							rc = pFtp->m_fnFtpConnectProc(pFrame->m_stCurFtpCond.iIP_Cnt,stCurFtpInfo, stCurFtpPath);										
						}
						if(rc == FALSE && iAnsCnt >= QMS_UPLOAD_CNT_MAX - 1)
						{			
							//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
							//	_T("[OK] QmsFileUpload Upload Retry2 \n"));
							pFtp->m_fnSetErrorMessage(_T(" NG P2 Upload Temporary failed-%d-%s \n"), _T("NG,P2,Upload Temporary failed"), QMS_DAT_LOG);	
							//pFtp->m_fnInitUploadThread();	
							break;			
						}
						iAnsCnt ++ ;
					}
					//2017.07.13 add by mby
					if(bFtpFail){ 
						break;
					}
					//2017.07.13 add by mby
					// FTP 업로드 파일 확장자 tmp -> dat로 바꿈
					strRemoteDat = strRemoteTmp;
					strRemoteDat.Replace(strRemoteDat.Right(4),EXTENSION_DAT);	
					rc = pFtp->m_fnRemoteRename(strRemoteTmp, strRemoteDat);
					if(rc == FALSE){
																									//if (!pFtp->ChkFtpConnect(pFtp->m_stCurFtpInfo.strFTP_Server))
			//{
			//	// todo..로그 기입 socket 실패 또는 오류 초과				
			//}
			//else
			//{
			//	
			//	/*m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE,
			//		_T(" NG P2 Temporary rename failed-%s \n"), strErrorCode);*/
			//}	
			//
						pFtp->m_fnSetErrorMessage(_T(" NG P2 Temporary rename failed-%d-%s \n"), _T("NG,P2,Temporary rename failed"), QMS_DAT_LOG);
						//pFtp->m_fnInitUploadThread();	
						break;
					}
		
					// 파일 업로드 완료
					m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" OK P2 Transfer successful Data %s \n"), strVal);// pFtp->m_strFileName);
					G_AddLog_QMS(_T("OK,P2,Transfer successful,%s"),strVal);

		
					// FTP 업로드 된 dat파일을 로컬 발송 폴더로 이동
					strLocalDat =  stCurFtpPath.strDir_Default +  stCurFtpPath.strDir_Send + strVal;    //pFtp->m_strFileName
					strLocalDat.Replace(strLocalDat.Right(4),EXTENSION_DAT);	
					rc = MoveFile(strLocalTmp, strLocalDat);

					if (GetFileAttributes( stCurFtpPath.strDir_Default + _T("sent_bak")) == -1)
					{
						CreateDirectory(stCurFtpPath.strDir_Default + _T("sent_bak"), NULL);
					}	
					CopyFile(strLocalDat, stCurFtpPath.strDir_Default + _T("sent_bak\\") + strVal,FALSE);

					strRemoteAns = strRemoteDat;
					strRemoteAns.Replace(strRemoteDat.Right(4),EXTENSION_ANS);	
					strLocalAns =  stCurFtpPath.strDir_Default +  stCurFtpPath.strDir_Send + strVal;    //pFtp->m_strFileName
					strLocalAns.Replace(strLocalAns.Right(4),EXTENSION_ANS);	

					rc = pFtp->m_fnLocalDel(strLocalAns);
					while(iAnsCnt < pFrame->m_stCurFtpCond.iAns_Rename_Wait_cnt )
					{ // FTP에 업로드 된 데이터 파일이 ans파일로 변경 되었는지 확인 					
			
						Sleep( pFrame->m_stCurFtpCond.iAns_Rename_Chk_Timeout * 1000);	// FTP 서버에서 ans 파일이 만들어 지기 까지 3초간 대기한다.

					
						szCode = pFtp->m_fnDownLoad(strRemoteAns, strLocalAns , stCurFtpInfo.strRoot);	 // FTP 업로드 된 ans파일을 로컬 발송 폴더로 다운로드	

						m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("[m_fnDownLoad] Ans File Download Condition : %d \n"),szCode);
				
						if (szCode == OK) break;	
				
						iAnsCnt ++;	

						if(szCode != OK && iAnsCnt >= pFrame->m_stCurFtpCond.iAns_Rename_Wait_cnt)
						{
																				//if (!pFtp->ChkFtpConnect(pFtp->m_stCurFtpInfo.strFTP_Server))			
				//{// todo..로그 기입 socket 실패 또는 오류 초과		
				//	strLocalUnsent = pFtp->m_stCurFtpInfo.strFTP_Dir_Default +  pFtp->m_stCurFtpInfo.strFTP_Dir_Unsent + strVal;    //pFtp->m_strFileName
				//	strLocalUnsent.Replace(strLocalUnsent.Right(4),EXTENSION_DAT);	
				//	rc = MoveFile(strLocalDat, strLocalUnsent);	
				//	return FALSE;
				//}
				//else{	}	
							pFtp->m_fnSetErrorMessage(_T(" NG P5 Receive file failed-%d-%s \n"), _T("NG,P5,Receive file filed"), QMS_DAT_LOG);

							pFrame->m_iniConfigSet.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_DEFAULT, _T(""), (LPTSTR)cErrPathTmp, 255);	
							strErrPath.Append(cErrPathTmp);
							pFrame->m_iniConfigSet.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_UNSENT, _T(""), (LPTSTR)cErrPathTmp, 255);
							strErrPath.Append(cErrPathTmp);

							strLocalUnsent = /*pFtp->m_stCurFtpPath.strDir_Default +  pFtp->m_stCurFtpPath.strDir_Unsent*/strErrPath + strVal;    //pFtp->m_strFileName
							strErrPath = _T("");

							rc = pFtp->m_fnRemoteDel(strRemoteDat);
							strLocalUnsent.Replace(strLocalUnsent.Right(4),EXTENSION_TMP);	
							rc = MoveFile(strLocalDat, strLocalUnsent);		
							//pFtp->m_fnInitUploadThread();		
							break;
						}								
					}
		
					if(szCode != OK) 
					{
						//strLocalUnsent =  stCurFtpPath.strDir_Default +  stCurFtpPath.strDir_Unsent + strVal;    //pFtp->m_strFileName
						//strLocalUnsent.Replace(strLocalUnsent.Right(4),EXTENSION_TMP);	
						//rc = MoveFile(strLocalDat, strLocalUnsent);
						////pFtp->m_fnInitUploadThread();	
						break;
					}
					iAnsCnt =0 ;
					while (iAnsCnt < QMS_DOWNLOAD_CNT_MAX) {
						rc = pFtp->m_fnRemoteDel(strRemoteAns);	
						if (rc) break;			    //  성공 시 TRUE를 RETRUN 한다.	
						if(rc == FALSE && iAnsCnt < QMS_DOWNLOAD_CNT_MAX - 1 )
						{
							strGuestMsg.Format(_T("NG,P5,delete download file failed:%s"),strRemoteAns);
							pFtp->m_fnSetErrorMessage(_T(" NG P5 delete download file failed-%d-%s \n"), strGuestMsg, QMS_DAT_LOG);		
							rc = pFtp->m_fnFtpConnectProc(pFrame->m_stCurFtpCond.iIP_Cnt,stCurFtpInfo, stCurFtpPath);								
					
											/*		strLocalUnsent =  pFtp->m_stCurFtpPath.strDir_Default +  pFtp->m_stCurFtpPath.strDir_Unsent + strVal;    //pFtp->m_strFileName
					strLocalUnsent.Replace(strLocalUnsent.Right(4),EXTENSION_TMP);	
					rc = MoveFile(strLocalDat, strLocalUnsent);	*/
							//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
							//	_T("[OK] QmsFileUpload Download Retry1 \n"));
						}	
						if(rc == FALSE && iAnsCnt >= QMS_DOWNLOAD_CNT_MAX - 1)
						{		
							strGuestMsg.Format(_T("NG,P5,delete download file failed:%s"),strRemoteAns);
							pFtp->m_fnSetErrorMessage(_T(" NG P5 delete download file failed-%d-%s \n"), strGuestMsg, QMS_DAT_LOG);

							pFrame->m_iniConfigSet.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_DEFAULT, _T(""), (LPTSTR)cErrPathTmp, 255);	
							strErrPath.Append(cErrPathTmp);
							pFrame->m_iniConfigSet.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_UNSENT, _T(""), (LPTSTR)cErrPathTmp, 255);
							strErrPath.Append(cErrPathTmp);

							strLocalUnsent = /*pFtp->m_stCurFtpPath.strDir_Default +  pFtp->m_stCurFtpPath.strDir_Unsent*/strErrPath + strVal;    //pFtp->m_strFileName
							strErrPath = _T("");
							
							strLocalUnsent.Replace(strLocalUnsent.Right(4),EXTENSION_TMP);	
							rc = MoveFile(strLocalDat, strLocalUnsent);	
							//pFtp->m_fnInitUploadThread();
							//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
							//	_T("[OK] QmsFileUpload Download Retry1 \n"));
							break;
						}	
						iAnsCnt ++ ;
					}			
		
					m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE,
						_T(" OK P5 receive file successful : %s \n"), strRemoteAns);
					G_AddLog_QMS(_T("OK,P5,receive file successful:%s"),strRemoteAns);
					strLocalError =  stCurFtpPath.strDir_Default +  stCurFtpPath.strDir_Error + strVal;    //pFtp->m_strFileName
					// 응답 문서가 OK일 경우 dat 파일을 삭제 한다, NG일 경우 dat, ans 파일을 에러 폴더로 이동		
					if (pFtp->m_fnfileOpen(strLocalAns))
					{
																					//todo.. 로그 기입 업로드 성공
			//응답 문서가 OK일 경우 발송 폴더의 Dat 파일을 삭제
			//dat 파일을 로컬에서 삭제
			/*	rc = pFtp->LocalDel(strLocalDat);		
			if(rc == FALSE){	
			pFtp->InitUploadThread();	
			return FALSE;
			}*/	
			//ans 파일을 로컬에서 삭제
						rc = pFtp->m_fnLocalDel(strLocalDat);
						//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
						//	_T("[OK] QmsFileUpload Ans OK \n"));
												/*	if(rc == FALSE){	
					pFtp->InitUploadThread();		
					return FALSE;
				}*/
					}
					else
					{	
						//todo.. 로그 기입 업로드 실패
						//strLocalError =  pFtp->m_stCurFtpPath->strDir_Default + pFtp->m_stCurFtpPath->strDir_Error + strVal;    //pFtp->m_strFileName //_T("D:\\QMS\\error\\") +  /*
						strLocalError.Replace(strLocalError.Right(4),EXTENSION_DAT);	
			
						//error 폴더에 같은 이름의 dat 파일이 있으면 삭제
						rc = pFtp->m_fnLocalDel(strLocalError);					
						rc = MoveFile(strLocalDat, strLocalError);
						if(rc == FALSE){	
						//	pFtp->m_fnInitUploadThread();		
							break;
						}	
						strLocalError.Replace(strLocalError.Right(4),EXTENSION_ANS);	
						//error 폴더에 같은 이름의 ans 파일이 있으면 삭제
						rc = pFtp->m_fnLocalDel(strLocalError);			
						rc = MoveFile(strLocalAns, strLocalError);
						rc = pFtp->m_fnLocalDel(strLocalDat);
						//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
						//	_T("[OK] QmsFileUpload Ans Fail \n"));
						if(rc == FALSE){	
							strGuestMsg.Format(_T("NG,P5,delete download file failed:%s"),strVal);
							pFtp->m_fnSetErrorMessage(_T(""), strGuestMsg, QMS_DAT_LOG);		
							break;
						}
						else{
							strGuestMsg.Format(_T("NG P5 Receive File Failed")); 
							pFtp->m_fnSetErrorMessage(_T(""), strGuestMsg, QMS_DAT_LOG);
						}
					}				

				}
				nLoopCnt ++;
			}	
		}
		/*if (nLoopCnt)
		{*/

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[OK] QmsFileUpload m_pConnection Init [Upload Data Complete] \n"));
			pFtp->m_fnInitUploadThread();
			finder.Close();
		//}
		
		//break;	
	}
	catch (CFtpClient* ee)
	{
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[CFtpClient] QmsFileUpload m_pConnection Init \n"));
		pFtp->m_fnInitUploadThread();				
	}
	catch (CMemoryException* ee)
	{
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[CMemoryException] QmsFileUpload m_pConnection Init \n"));
		pFtp->m_fnInitUploadThread();				
	}
	catch (CException* ee)
	{
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[CException] QmsFileUpload m_pConnection Init \n"));
		pFtp->m_fnInitUploadThread();				
	}
	return rc;
}

BOOL CFtpClient::QmsImgUplaod(LPVOID pArg )
{
	BOOL rc = FALSE;
	int iAnsCnt=0 ,iConctCnt=0;
	int szCode =0, nLoopCnt = 0;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	CFtpClient *pFtp = (CFtpClient *)pArg;	
	CFileStatus status;

	CTime time = CTime::GetCurrentTime();
	pFtp->m_strNowDate =  time.Format("%Y%m%d");
	pFtp->m_strNowTime  = time.Format("%H%M%S"); 

	TCHAR	sWk[128];			
	CFile pFile;
	CString strLocalTmp, strRemoteTmp ,  strLocalDat, strRemoteDat , strLocalAns, strRemoteAns ,strRename;
	CString strLocalUnsent, strLocalError, strErrorCode;

	CString cTokenVal, resToken;
	try
	{		
		CFileFind finder;
		CString	 strTmp, strFileName, strVal, strPathTmp;
		CTime time;
		int curPos = 0;
		//unsigned long size;
		BOOL bWorking/*, brc*/;

		STRU_FTP_SEVER_INFO stCurFtpInfo;// = new STRU_FTP_SEVER_INFO;
		STRU_FTP_PATH stCurFtpPath;// = new STRU_FTP_PATH;

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[Process 1] QmsImgUplaod \n"));
		rc = pFtp->m_fnFtpConnectImgProc(pFrame->m_stCurFtpCond.iIP_Cnt,stCurFtpInfo, stCurFtpPath);
		if(!rc) return FALSE;
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[Process 2] QmsImgUplaod \n"));
		
		strLocalTmp =  pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iWait; 
		strLocalTmp += _T("\\*.*");
		while(TRUE)
		{
			//strLocalTmp =  pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iUnsent; 
			//strLocalTmp += _T("\\*.*");
			bWorking = finder.FindFile(strLocalTmp);
			if (bWorking == FALSE)
			{
				break;
			}
			while(bWorking == TRUE)
			{
				bWorking = finder.FindNextFile();
				if(finder.IsDots()) continue;
				strVal = finder.GetFileName();	

				// TEST 15.05.03
				//strLocalTmp = pFrame->m_DataManager.m_strDefImgSavePath ;//=  pFtp->m_stCurFtpPath->strDir_Default +  pFtp->m_stCurFtpPath->strDir_iUnsent ;//+ strVal;    //pFtp->m_strFileName
				strLocalTmp = stCurFtpPath.strDir_Default +  stCurFtpPath.strDir_iWait ;//+ strVal;    //pFtp->m_strFileName
				strRemoteTmp = stCurFtpInfo.strRoot;// + strVal;    //pFtp->m_strFileName
			//	G_AddLog(3, _T("QmsImgUplaod local path = %s"), strLocalTmp);

				if(!CFile::GetStatus(strLocalTmp, status)) {
					memset(sWk,0,sizeof(sWk));			
					wsprintf(sWk,  _T("Not Exist File Name : %s "), strVal);//pFtp->m_strFileName);
					//AfxMessageBox(sWk, MB_OK|MB_ICONSTOP);
				//	pFtp->m_fnInitUploadImgThread();		
					break;
				}
				iAnsCnt =0 ;

				if (finder.IsDirectory())
				{
					cTokenVal = pFtp->m_fnUpLoadDir(strLocalTmp, strRemoteTmp);		//파일 업로드		
					if (rc)
					{
						curPos = 0;
						//cTokenVal = cCpyLDir;

						cTokenVal.Append( _T("\\"));
						/*while (cTokenVal != "")
						{	
						if (!cTokenVal.Compare(pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iUnsent))
						break;	
						if (_T("i_unsent") == pFrame->m_DataManager.m_fnLastToken(cTokenVal, _T("\\")))
						{
						break;	
						}

						if (RemoveDirectory(cTokenVal))
						{
						resToken = pFrame->m_DataManager.m_fnLastToken(cTokenVal, _T("\\"));		
						cTokenVal = cTokenVal.Left(cTokenVal.GetLength() - (resToken.GetLength() + 1));		
						}
						else
						{
						break;
						}
						}		*/
						break;
					}
									    //  성공 시 TRUE를 RETRUN 한다.	
				}

				if(rc == FALSE && iAnsCnt < QMS_UPLOAD_CNT_MAX - 1 )
				{
					//pFtp->m_fnSetErrorMessage(_T(" NG P2 Upload Temporary failed-%d-%s \n"), TRUE, QMS_DAT_LOG);
					//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
					//	_T("[OK] QmsImgUplaod Retry1 \n"));
					pFtp->m_fnFtpConnectImgProc(pFrame->m_stCurFtpCond.iIP_Cnt, stCurFtpInfo, stCurFtpPath);										
				}
				if(rc == FALSE && iAnsCnt >= QMS_UPLOAD_CNT_MAX - 1)
				{	
					//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
					//	_T("[OK] QmsImgUplaod Retry2 \n"));
					//pFtp->m_fnSetErrorMessage(_T(" NG P2 Upload Temporary failed-%d-%s \n"), TRUE, QMS_DAT_LOG);	
					//pFtp->m_fnInitUploadImgThread();	
					break;			
				}
				iAnsCnt ++ ;

				// 파일 업로드 완료
				m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T(" OK P3 Transfer Image Complete %s \n"), strVal);// pFtp->m_strFileName);
			}
			//break;
		}	

		m_fnQmsSentImgDelete();//2015.07.01

		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[OK] QmsImgUplaod m_pConnectionImg Init \n"));
		pFtp->m_fnInitUploadImgThread();
		finder.Close();

	}
	catch (CFtpClient* ee)
	{
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
		//	_T("[CFtpClient] QmsImgUplaod m_pConnectionImg Init \n"));
		pFtp->m_fnInitUploadImgThread();				
	}
	return rc;
}

unsigned __stdcall  CFtpClient::m_fnFTPUploadProcImg(LPVOID pArg )
{	

	BOOL rc = FALSE;
	CFtpClient *pFtp = (CFtpClient *)pArg;	
	//CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	//int iLogWriteTerm = 0, iPeriodDay = 0;
	//TSTRING strPath = _T(""), strLogName = _T("");
	//wchar_t chRead[255] = {0};

	////pFtp->m_fnSetLogStataus(strPath, strLogName, iPeriodDay, iLogWriteTerm );
	//iLogWriteTerm = pFrame->m_iniConfigFtp.GetProfileIntW(FTP_LOG_STATE, LOG_WRITE_TERM, 0);	
	//iPeriodDay = pFrame->m_iniConfigFtp.GetProfileIntW(FTP_LOG_STATE, KEY_FTP_DIR_PERIOD_DAY, 30);
	//strPath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iLog;
	//pFrame->m_iniConfigFtp.GetProfileStringW(FTP_LOG_STATE, KEY_FTP_DIR_LOG_NAME, _T(""), (LPTSTR)chRead, 255);
	//strLogName = chRead;

	
	rc = pFtp->QmsImgUplaod(pArg);
	return rc;
}



BOOL CFtpClient::m_fnInitUploadThread()
{
	if(m_hThreadUpdateFtp)
	{
		//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE,_T(" [m_fnInitUploadThread] Thread Initializing... \n"));
		//delete  m_pFtpLogWriter;
		//m_pFtpLogWriter = NULL;
		CloseHandle(m_hThreadUpdateFtp);
		m_hThreadUpdateFtp = NULL;
		//delete  m_strFileName;
		m_fnClose();
	}
	return TRUE;
}

BOOL CFtpClient::m_fnInitUploadImgThread()
{
	if(m_hThreadUpdateImgFtp)
	{
		//delete  m_pFtpImgLogWriter;
		//m_pFtpImgLogWriter=NULL;
		CloseHandle(m_hThreadUpdateImgFtp);
		m_hThreadUpdateImgFtp = NULL;
	

		//delete  m_strFileName;
		m_fnCloseImg();
	}
	return TRUE;
}

BOOL CFtpClient::m_fnInitConnectionThread()
{
	if(m_hThreadConnectFtp)
	{
		CloseHandle(m_hThreadConnectFtp);
		m_hThreadConnectFtp = NULL;

		m_fnClose();
	}
	return TRUE;
}

//BOOL CFtpClient::m_fnFTPdUploadData(CString strFileName) 
BOOL CFtpClient::m_fnFTPdUploadData() 
{
//	BOOL rc;	
	
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
//	m_strFileName= new CString[MAX_PANEL];
		if(m_hThreadUpdateFtp == NULL && m_pConnection == NULL)
		{
			//m_strFileName =  strFileName;
			/*
			m_strFileName[PANEL_ONE] =  strFileName[PANEL_ONE];
			m_strFileName[PANEL_TWO] =  strFileName[PANEL_TWO];*/
			UINT uiThreadID;

			//m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, 
			//	_T("[m_fnFTPdUploadData] (m_fnFTPUploadProc) Beginning Thread...  \n"));

			m_hThreadUpdateFtp = (HANDLE)::_beginthreadex(NULL, 0, m_fnFTPUploadProc, (LPVOID)this, 0, &uiThreadID);

//			break;
		}	
	return 1;
}
BOOL CFtpClient::m_fnFTPdUploadImg() 
{
//	BOOL rc;	

	if(m_hThreadUpdateImgFtp == NULL)
	{		
		UINT uiThreadID;
		m_hThreadUpdateImgFtp = (HANDLE)::_beginthreadex(NULL, 0, m_fnFTPUploadProcImg, (LPVOID)this, 0, &uiThreadID);
	}	
	return 1;
}

BOOL CFtpClient::m_fnStartFTPConnect() 
{
//	BOOL rc;	

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if(m_hThreadConnectFtp == NULL)
	{
		UINT uiThreadID;
		//m_hThreadConnectFtp = (HANDLE)::_beginthreadex(NULL, 0, m_fnFTPConnect, (LPVOID)this, 0, &uiThreadID);
		m_hThreadConnectFtp = (HANDLE)::_beginthreadex(NULL, 0, m_fnFTPConnectCheck, (LPVOID)this, 0, &uiThreadID);	//2015.11.20
				
	}	
	return 1;
}

BOOL CFtpClient::m_fnfileOpen(CString strDownFileName)
{
	CString pszFileName = strDownFileName.MakeLower();
	int iNgCheck= 0, nCheckCnt = 0;
	CString strReadLine = _T("");
	CStdioFile file;
	CFileException e;
	

	if (!file.Open(strDownFileName, CFile::modeRead|CFile::typeText, NULL))
	{		
		return FALSE;
	}
	while(file.ReadString(strReadLine))
	{
		strReadLine.MakeUpper();
		iNgCheck = strReadLine.Find(_T("NG")) ;
		//2016.03.02 Mr.Chu		 패널 아이디 내 NG 가 들어간 경우 예외처리
		if(iNgCheck != -1 && nCheckCnt != 1)
		{
			file.Close();
			return FALSE;
		}
		nCheckCnt++;
	}

	file.Close();

	return TRUE;
}

BOOL CFtpClient::m_fnSetLogStataus(	TSTRING szLogFilePath,			///< 로그 파일 경로
								TSTRING szLogFileName,			///< 로그 파일 이름
								int uPeriodDay,			///< 로그 보존 기한일
								int uWriteTerm,				///< 로그 파일을 몇시간 주기로 생성하는가..시간, 0 = 하루치의 로그가 파일 하나로 생성됨.
								bool bLogWriteEnable,	///< 로그를 실제로 기입하는가 플레그
								USHORT uLogBlockLevel)
{

	return TRUE;
}

BOOL CFtpClient::m_fnPrintLogs(CString strLog)
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, strLog);

	return TRUE;
}

int CFtpClient::m_fnSetAutoDeleteInfo(CString strFilePath, int nPeriod/*=30*/)
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

int	CFtpClient::m_fnStartAutoDelete(BOOL bStart)
{
	int nRes = 0;

	if(bStart)
	{
		if(m_hThreadQmsDataDel != NULL)
		{
			DWORD  exitcode = 0;

			m_bFileAutoDelete = FALSE;

			::WaitForSingleObject(m_hThreadQmsDataDel, INFINITE);
			::GetExitCodeThread(m_hThreadQmsDataDel, &exitcode);

			if(exitcode ==  STILL_ACTIVE)
				::TerminateThread(m_hThreadQmsDataDel, exitcode);

			::CloseHandle(m_hThreadQmsDataDel);

			m_hThreadQmsDataDel   = NULL;
		}

		m_bFileAutoDelete = TRUE;

		DWORD dwThreadID;
		
		m_hThreadQmsDataDel = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)m_fnCheckAutoDelte, (LPVOID)this, CREATE_SUSPENDED, &dwThreadID);
		ResumeThread(m_hThreadQmsDataDel);

		if(m_hThreadQmsDataDel == NULL) 
			return -1;
	}
	else
	{
		DWORD  exitcode = 0;

		m_bFileAutoDelete = FALSE;

		::WaitForSingleObject(m_hThreadQmsDataDel, INFINITE);
		::GetExitCodeThread(m_hThreadQmsDataDel, &exitcode);

		if(exitcode ==  STILL_ACTIVE)
			::TerminateThread(m_hThreadQmsDataDel, exitcode);

		::CloseHandle(m_hThreadQmsDataDel);

		m_hThreadQmsDataDel   = NULL;
	}

	return nRes;
}

void CFtpClient::m_fnCheckAutoDelte(LPVOID pWnd)
{
	CFtpClient*		pCtrlWnd = (CFtpClient*)pWnd;

	CFileFind		fileFind;
	CTime			fileTime;
	BOOL			bStart = FALSE, bFindFile = FALSE , bFind = FALSE;	
	//DWORD			dwFileDayAmount, dwNowDayAmount;
	CString			strDeleteFilePath, strFindFilePath;

	strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\*.*");

	while(1)
	{
		Sleep(5000);

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
			bFind = fileFind.FindNextFile();	
			strFindFilePath = fileFind.GetFilePath();
			
			if(fileFind.IsReadOnly()) continue;

			if(fileFind.IsDots()) continue;

			if(fileFind.IsDirectory())
			{				
				pCtrlWnd->m_fnDeleteCurFile(strFindFilePath + _T("\\*.*"));	
				continue;
			}

			if(!bFind)
			{
				strDeleteFilePath = pCtrlWnd->m_strDeleteFilePath + _T("\\*.*");		
				bStart = FALSE; continue;
			}
		}
	}
}
int CFtpClient::m_fnDeleteCurFile(CString strDeleteFilePath)
{
	int val = 0;

	CFileFind		fileFind;
	CTime			fileTime;
	BOOL			bFind = FALSE;
	DWORD			dwFileDayAmount, dwNowDayAmount;
	CString			strFindFilePath, strTemp, strTemp2;
		
	bFind = fileFind.FindFile(strDeleteFilePath);	
	bFind = TRUE;
	try
	{
		while(bFind)		   
		{		

			bFind = fileFind.FindNextFile();	
			strFindFilePath = fileFind.GetFilePath();

			if(fileFind.IsReadOnly()) continue;
			if(fileFind.IsDots()) continue;

			fileFind.GetLastWriteTime(fileTime);

			dwFileDayAmount	= m_fileMng.m_fnGetTimeAmount(fileTime);
			dwNowDayAmount	= m_fileMng.m_fnGetTimeAmoutNow();

			if(dwNowDayAmount >= (dwFileDayAmount + m_nDeletePeriod))
			{
				strFindFilePath = strFindFilePath.Right(4);
				strFindFilePath.MakeLower();
				if(!strFindFilePath.Compare(EXTENSION_ANS) || !strFindFilePath.Compare(EXTENSION_DAT)
					|| !strFindFilePath.Compare(EXTENSION_TMP) || !strFindFilePath.Compare(EXTENSION_JPG) )
				{
					CString strFilePath = fileFind.GetFilePath();
					_wunlink(strFilePath);				
				}			
			}

			if (!bFind) break;		
		}	


	}
	catch (CMemoryException* e)
	{
		e->Delete();
	}
	catch (CFileException* e)
	{
		e->Delete();
	}
	catch (CException* e)
	{
		e->Delete();
	}

	return val;
}

//2015.07.01
void CFtpClient::m_fnQmsSentImgDelete()
{
		int			iDirDelDay;
//		SYSTEMTIME	systime;
		CFileFind	fileFind;
		BOOL		bWorking;
		DWORD		dwFileDayAmount, dwNowDayAmount;
		CString		strSentDir, strDirName;
		CTime		fileTime;
		CMainFrame* pFrame =  (CMainFrame*)(AfxGetApp()->GetMainWnd());

		iDirDelDay = pFrame->m_iniConfigFtp.GetProfileIntW(FTP_LOCAL_STATE, KEY_FTP_DIR_DELETE, 10); 
		strSentDir = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iSend;
		//strSentDir = _T("E:\\i_sent\\");
		bWorking = fileFind.FindFile(strSentDir + _T("\\*.*"));

		while(bWorking)
		{
			bWorking = fileFind.FindNextFile();

			if(fileFind.IsDirectory() && !fileFind.IsDots())
			{
				fileFind.GetCreationTime(fileTime);
				dwFileDayAmount	= pFrame->m_DataManager.m_fnGetTimeAmount(fileTime);
				dwNowDayAmount	= pFrame->m_DataManager.m_fnGetTimeAmoutNow();

				if(dwNowDayAmount > (dwFileDayAmount + iDirDelDay))
				{
					strDirName = fileFind.GetFileName();
					ShellExecute(0, _T("open"), _T("cmd"), _T("/C rd /s /q ") + strDirName, strSentDir, SW_HIDE);  
				}
			}
		}
}
