// ClientSockVB.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "ClientSockCs.h"
#include "..\..\VisualStation\CommonHeader\Class\Interserverinterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CInterServerInterface* g_pInterServerInterface;					///< CInterServerInterface 의 객체

USHORT G_TaskNo;
// 유일한 응용 프로그램 개체입니다.

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 오류 코드를 필요에 따라 수정합니다.
			_tprintf(_T("심각한 오류: MFC를 초기화하지 못했습니다.\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: 응용 프로그램의 동작은 여기에서 코딩합니다.
		}
	}
	else
	{
		// TODO: 오류 코드를 필요에 따라 수정합니다.
		_tprintf(_T("심각한 오류: GetModuleHandle 실패\n"));
		nRetCode = 1;
	}

	return nRetCode;
}


/**
@brief		DLL을 초기화 한다.

@return		0
@remark		
@author		김용태
@date		2014.09.25
*/
CLIENTSOCKCS_API int _stdcall InitClientSockDLL(
	TCHAR* szLogFilePath,			///< 로그 파일 경로
	TCHAR* szLogFileName,			///< 로그 파일 이름
	HWND hWnd = NULL,				///< 메시지를 받을 윈도우 Handle
	UINT nUserMessageNo = NULL,		///< 메시지 전달에 사용될 사용자 메시지 번호
	bool bAutoExitFlag = true,		///< 서버가 종료 될때 Application 을 자동 종료 시킬지를 가지는 Flag, Windows handle 이 등록된경우는 자동종료안된다.
	int uPeriodDay = 30,			///< 로그 보존 기한일
	int uWriteTerm = 0,				///< 로그 파일을 몇시간 주기로 생성하는가..시간, 0 = 하루치의 로그가 파일 하나로 생성됨.
	bool bLogWriteEnable = true,	///< 로그를 실제로 기입하는가 플레그
	USHORT uLogBlockLevel = LOG_LEVEL_4
	)
{
	g_pInterServerInterface = new CInterServerInterface(szLogFilePath, szLogFileName, hWnd, nUserMessageNo, bAutoExitFlag, uPeriodDay, uWriteTerm, bLogWriteEnable, uLogBlockLevel );
	return 0;
}


/** 
@brief		VS64 메시지 전달을 위해 Application 에 제공한 메모리를 회수한다.

@return		없음
@remark		Application 에서는 메시지 처리가 끝난 후 본 함수를 사용하여 자원을 반환해 주어야 한다.
@author		김용태
@date		2014.09.25
*/
CLIENTSOCKCS_API int _stdcall FreeMemory(
	void* pMemory					///< 자원을 회수할 메모리 주소
	)
{

	g_pInterServerInterface->m_fnFreeMemory(pMemory);	

	return 0;
}


/** 
@brief		서버에서 전달된 메시지를 큐에서 하나 꺼내어 Application 에 전달 한다.

@return		VS64 메시지
@remark		꺼내진 메시지는 Application 내에서 사용 후 반환 되어야 한다.
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API BYTE* _stdcall PeekMessageBlocking()
{
	return g_pInterServerInterface->m_fnPeekMessage();
}


/** 
@brief		서버에서 전달된 메시지를 큐에서 하나 꺼내어 Application 에 전달 한다. Non Block Mode

@return		VS64 메시지
@remark		꺼내진 메시지는 Application 내에서 사용 후 반환 되어야 한다. Blocking 이 되지 않는다.
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API BYTE* _stdcall PeekMessageForNonBlock()
{
	return g_pInterServerInterface->m_fnPeekMessageForNonBlock();
}

/** 
@brief		서버에 등록한다.

@return		정상 종료 : 0, 이상 종료 : 그외
@remark		등록 할 서버가 같은 PC 내에 있지 않다면 IP 주소와 Port 번호를 입력받아야 한다.
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API int _stdcall RegisterTask(
	USHORT uTaskNo,							///< 서버로 등록시 사용할 Task 번호
	TCHAR* pszServerIp = _T("127.0.0.1"),	///< 등록 할 서버의 IP 주소
	WORD wPortNo = 5000						///< 등록 할 서버의 Port No
	)
{
	G_TaskNo = uTaskNo;
	return g_pInterServerInterface->m_fnRegisterTask(uTaskNo, pszServerIp, wPortNo);
}

	/** 
@brief		서버에 재등록한다.

@return		정상 종료 : 0, 이상 종료 : 그외
@remark		등록 할 서버가 같은 PC 내에 있지 않다면 IP 주소와 Port 번호를 입력받아야 한다.
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API int _stdcall ReRegisterTask(
	)
{
	return g_pInterServerInterface->m_fnReRegisterTask();
}

/** 
@brief		서버에 등록 해지한다.

@return		정상 종료 : 0, 이상 종료 : 그외
@remark		
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API int _stdcall UnRegisterTask()
{
	return g_pInterServerInterface->m_fnUnRegisterTask();
}

/** 
@brief		다른 Task 로 메시지를 전달한다.

@return		정상 종료 : 0, 이상 종료 : 그외
@remark		
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API int _stdcall SendCommand1(
	CMDMSG* sCmdMsg						///< 전송할 정보를 담은 CMDMSG 구조체 포인터
	)
{
	return g_pInterServerInterface->m_fnSendCommand(sCmdMsg);
}

/** 
@brief		다른 Task 로 메시지를 전달한다.

@return		정상 종료 : 0, 이상 종료 : 그외
@remark		
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API int _stdcall SendCommand2(
	USHORT uTask_Dest,					///< 목적지 Task 번호
	USHORT uFunID,						///< 호출 할 목적지의 Funtion ID
	USHORT uSeqID,						///< 호출 할 목적지의 Sequence ID
	USHORT uUnitID						///< 호출 할 목적지의 Unit ID
	)
{
	return g_pInterServerInterface->m_fnSendCommand(uTask_Dest, uFunID, uSeqID, uUnitID);
}

/** 
@brief		다른 Task 로 메시지를 전달한다.

@return		정상 종료 : 0, 이상 종료 : 그외
@remark		
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API int _stdcall SendCommand3(
	USHORT uTask_Dest,						///< 목적지 Task 번호
	USHORT uFunID_Dest,						///< 호출 할 목적지의 Funtion ID
	USHORT uSeqID_Dest,						///< 호출 할 목적지의 Sequence ID
	USHORT uUnitID_Dest,					///< 호출 할 목적지의 Unit ID
	ULONG  uMsgSize,						///< 전달할 메시지의 Size
	BYTE *cMsgBuf,							///< 전달할 메시지
	USHORT uFunID_Src = 0,					///< 근원지의 Function ID
	USHORT uSeqID_Src = 0,					///< 근원지의 Sequence ID
	USHORT uUnitID_Src = 0,					///< 근원지의 Unit ID
	USHORT uMsgType = CMD_TYPE_NORES,		///< 메시지가 Response 가 필요한지 여부, CMD_TYPE_NORES/CMD_TYPE_RES
	USHORT uMsgOrigin = CMD_TYPE_CMD,		///< 메시지가 처리에 대한 응답인지 여부, CMD_TYPE_REPLY/CMD_TYPE_CMD
	USHORT uMsgCompressMode = 0,			///< 파라메터 버퍼를 압축할지 여부, 0:압축안함, 1:압축, 2:10MB 이상 자동 압축
	ULONG  uSendMsgSize = 0					///< uMsgSize 만큼 전송하지 않고 uSendMsgSize 만큼만 전송하면 되는 경우 사용한다. 회신에 데이터를 받는 경우 주로 사용
	)
{
	return g_pInterServerInterface->m_fnSendCommand(uTask_Dest, uFunID_Dest, uSeqID_Dest, uUnitID_Dest, uMsgSize, cMsgBuf,
		uFunID_Src, uSeqID_Src, uUnitID_Src, uMsgType, uMsgOrigin, uMsgCompressMode, uSendMsgSize);
}


/** 
@brief		다른 Task 로 메시지를 전달한다.

@return		정상 종료 : 0, 이상 종료 : 그외
@remark		
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API int _stdcall SendCommand(
	USHORT uTask_Dest,						///< 목적지 Task 번호
	USHORT uFunID_Dest,						///< 호출 할 목적지의 Funtion ID
	USHORT uSeqID_Dest,						///< 호출 할 목적지의 Sequence ID
	USHORT uUnitID_Dest,					///< 호출 할 목적지의 Unit ID
	USHORT uMsgType,
	UINT uiTimeOut,
	ULONG uMsgSize,							///< 전달할 메시지의 Size
	BYTE *cMsgBuf,
	USHORT uMsgCompressMode = 0,			///< 파라메터 버퍼를 압축할지 여부, 0:압축안함, 1:압축, 2:10MB 이상 자동 압축
	ULONG  uSendMsgSize = 0					///< uMsgSize 만큼 전송하지 않고 uSendMsgSize 만큼만 전송하면 되는 경우 사용한다. 회신에 데이터를 받는 경우 주로 사용
	)
{
	int nRet = APP_OK;	
	CMDMSG tempCmdMsg;
	tempCmdMsg.uTask_Src		= G_TaskNo;
	tempCmdMsg.uTask_Dest		= uTask_Dest;
	tempCmdMsg.uFunID_Dest		= uFunID_Dest; 
	tempCmdMsg.uSeqID_Dest		= uSeqID_Dest;
	tempCmdMsg.uUnitID_Dest		= uUnitID_Dest;
	tempCmdMsg.uMsgType			= uMsgType;
	tempCmdMsg.dwTimeOut		= uiTimeOut;
	tempCmdMsg.uMsgSize			= uMsgSize;
	tempCmdMsg.cMsgBuf			= (INT64)cMsgBuf; 
	tempCmdMsg.uMsgCompressMode = uMsgCompressMode;
	tempCmdMsg.uSendMsgSize		= uSendMsgSize;
	nRet = g_pInterServerInterface->m_fnSendCommand(&tempCmdMsg);

	//*uRtnMsgSize = tempCmdMsg.uMsgSize;

	return nRet;
}

/** 
@brief		다른 Task 로 응답 메시지를 전달한다.

@return		정상 종료 : 0, 이상 종료 : 그외
@remark		
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API int _stdcall SendResponseCommand(
	CMDMSG* sCmdMsg						///< 전송할 정보를 담은 CMDMSG 구조체 포인터
	)
{
	return g_pInterServerInterface->m_fnSendResponseCommand(sCmdMsg);
}


/** 
@brief		로그를 기록한다.

@return		정상 종료 : 0, 이상 종료 : 그외
@remark		Log Task 로 Log 기록 메시지를 전달한다. 유니코드용
@author		김용태
@date		2006.
*/
CLIENTSOCKCS_API int _stdcall PrintLog(
	USHORT LogLevel,				///< 기록 할 Log 의 Log Level, 1차로 함수 내에서  Block을 결정하고, 2차로 Log Task 에서 Block 여부가 결정된다.
	int	nLogTextSize,				///< 로그를 기입할 내용의 사이즈
	const wchar_t* pszLogData		///< 실제 기록 할 Log 내용
	)

{
	return g_pInterServerInterface->m_fnPrintLog(LogLevel, nLogTextSize, pszLogData);
}

/** 
@brief		서버의 컨넥션 유무를 반환한다.

@return		
@remark		
@author		김용태
@date		2014.9.30
*/
CLIENTSOCKCS_API int _stdcall GetVSServerConnected()
{
	if(g_pInterServerInterface->m_fnGetServerConnected() == true)
		return 1;
	else
		return 0;
}
CLIENTSOCKCS_API int _stdcall test()
{
	if(g_pInterServerInterface->m_fnGetServerConnected() == true)
		return 1;
	else
		return 0;
}
