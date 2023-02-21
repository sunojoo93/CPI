/** 
@addtogroup	Client_Socket_Library
@{
*/ 

/**
@file		InterServerInterface.h
@brief		Client Socket DLL 을 이용하여 Visual Station 64 Server의 기능을 이용 할 수 있게한다.

@remark		Client Application 에서 본 헤더 파일을 Include 하고 Link 옵션에 ClientSockDll.lib 파일을 연결하여서 사용한다.
@author		김용태
@date		2007.05.08
*/
#pragma once

#include <Windows.h>
#include "..\Constant\CommonConstant.h"
#include "..\Structure\Define_Message.h"
#include "..\Class\MemPooler.h"
#include "..\Class\MessageQueue.h"
#include "..\Class\LogWriter.h"
#include "..\ImportClientDll.h"
#include "..\ErrorCode\ClientDllErrorCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#pragma warning( disable : 4996 )

/**
@brief		Client Socket DLL 과의 연결 기능을 제공한다.

@return		정상 종료 : OKAY, 이상 종료 : 그외
@remark		Client DLL 에서 부터 응답 받을 Callback function
@warning	Application 내에서는 VS64 메시지를 처리하고 나서 MemoryPool.Free() 함수를 이용하여 자원을 해지 해줘야 한다. 
@author		김용태
@date		2006.
*/
/*static int	g_fnAppCallBackFunc(CMDMSG *pCmdMsg,	///< Client Dll 에서 넘어온 메시지 구조체 포인터
								BYTE *cBuff			///< Client Dll 에서 넘어온 Data 포인터
								)	*/	
//{
//	//CMDMSG* cmdMsg	= NULL;
//	//UCHAR*	buff	= NULL;
//
//	//cmdMsg = new CMDMSG;
//	//buff = new UCHAR[cmdMsg->uMsgSize];
//
//	if(NULL == pCmdMsg && NULL == cBuff)
//	{
//		pCmdMsg	= (CMDMSG *)-1;		//QuitMessage....
//		//buff	= (UCHAR*)-1;
//	}
//	else
//	{
//		//cmdMsg	= (CMDMSG*)MemoryPool.Alloc();	// 2016.09.28 가변 사이즈 파라메터 전달 작업	
//		//cmdMsg	= new CMDMSG;						// 2016.09.28 가변 사이즈 파라메터 전달 작업
//
//		//memcpy(cmdMsg, pCmdMsg, sizeof(CMDMSG));
//		//cmdMsg->cMsgBuf = (INT64)cmdMsg + sizeof(CMDMSG);				// 2016.09.28 가변 사이즈 파라메터 전달 작업
//		//memcpy((void*)cmdMsg->cMsgBuf, cBuff, cmdMsg->uMsgSize);		// 2016.09.28 가변 사이즈 파라메터 전달 작업
//		//cmdMsg->cMsgBuf = (INT64)cBuff;									// 2016.09.28 가변 사이즈 파라메터 전달 작업
//	}
//
//	if(NULL != st_hWnd && NULL != st_nUserMessage)
//	{
//		if(NULL == pCmdMsg && NULL == cBuff)
//		{
////			st_nServerConnected = 0;
//
//			if(st_bAutoExitFlag)
//				PostMessage(st_hWnd, WM_QUIT, NULL, NULL);
//			else
//				PostMessage(st_hWnd, st_nUserMessage, -1, -1);	// Windows message Processor에서 VS Server Disconnect 처리를 해줘야 한다.
//		}
//		else
//		{
//			PostMessage(st_hWnd, st_nUserMessage, NULL, (LPARAM)pCmdMsg);
//		}
//	}
//	else
//	{
//		MessageQueue.m_fnPushMessage((BYTE*)pCmdMsg);
//	}
//
//	return OKAY;
//}

/** 
@brief		InterServerInterface

@remark		
-			Application 에 Client DLL 의 기능을 제공한다.
-			Client로 메시지 전달 방법은 2가지방식으로 제공한다. 
-			생성자에 파라메터가 입력이 되면 Window Message 를 이용한 VS 64 메시지 전달이 되고, 아니면 메시지 큐를 이용한 전달이 된다.
@warning	Application 내에서는 VS64 메시지를 처리하고 나서 MemoryPool.Free() 함수를 이용하여 자원을 해지 해줘야 한다. 
@author		김용태
@date		2006.
*/ 
class CInterServerInterface
	: private CLogWriter
{
public:
	USHORT	m_uTaskNo;								///< 자신의 Task NO 를 가진다.
	TCHAR*	m_szServerIP;
	WORD	m_wPortNo;
	TCHAR*	m_szServerName;
private:
	USHORT	m_uLogBlockLevel;						///< Log 를 기록할 Log Level 을 가진다.이것보다 낮은 레벨은 기입하지 않는다.
	bool	m_bAutoExitFlag;						///< Server 와의 연결이 끊기면 자동으로 Process 가 종료할지 여부. Default 는 true 이다.	
	bool	m_bServerConnected;
	CMessageQueue<BYTE*>	MessageQueue;			///< Application 에서 VS 64 메시지를 직접 Peek 할때 사용되는 Message Queue
	INT64	m_pAppSupporter;
public:
	/** 
	@brief		생성자

	@return		없음
	@remark		Class 생성시 파라메터가 입력이되면 Window Message 를 이용한 VS 64 메시지 전달이 되고, 아니면 메시지 큐를 이용한 전달이 된다.
	@author		김용태
	@date		2006.
	*/
	CInterServerInterface(
		TSTRING szLogFilePath,			///< 로그 파일 경로
		TSTRING szLogFileName,			///< 로그 파일 이름
		HWND hWnd = NULL,				///< 메시지를 받을 윈도우 Handle				//2017.03.13 기능 삭제함
		UINT nUserMessageNo = NULL,		///< 메시지 전달에 사용될 사용자 메시지 번호	//2017.03.13 기능 삭제함
		bool bAutoExitFlag = true,		///< 서버가 종료 될때 Application 을 자동 종료 시킬지를 가지는 Flag, Windows handle 이 등록된경우는 자동종료안된다.
		int uPeriodDay = 30,			///< 로그 보존 기한일
		int uWriteTerm = 0,				///< 로그 파일을 몇시간 주기로 생성하는가..시간, 0 = 하루치의 로그가 파일 하나로 생성됨.
		bool bLogWriteEnable = true,	///< 로그를 실제로 기입하는가 플레그
		USHORT uLogBlockLevel = LOG_LEVEL_4
		):
	//m_uLogTaskNo(DEFAULT_LOG_TASK),
		m_uLogBlockLevel(uLogBlockLevel),
		m_bAutoExitFlag(bAutoExitFlag),
		m_szServerIP(NULL),
		m_wPortNo(0),
		m_uTaskNo(0),
		m_bServerConnected(false),
		m_pAppSupporter(0),
		m_szServerName(NULL),
		CLogWriter(szLogFilePath, szLogFileName, uPeriodDay, uWriteTerm, bLogWriteEnable)
	{
		//st_hWnd			= hWnd;
		//st_nUserMessage	= nUserMessageNo;
		//st_bAutoExitFlag = bAutoExitFlag;
		//MemoryPool		= new CMemPooler<UCHAR[MAX_CLIENT_MESSAGE_BUFFER]>(300);
	}

	/** 
	@brief		소멸자

	@return		없음
	@remark		
	@author		김용태
	@date		2006.
	*/
	~CInterServerInterface(void)
	{
		delete[] m_szServerIP;
		delete[] m_szServerName;
		//delete MemoryPool;
		g_fnDeleteAppSupporter(m_pAppSupporter);
	}

	/** 
	@brief		VS64 메시지 전달을 위해 Application 에 제공한 메모리를 회수한다.

	@return		없음
	@remark		Application 에서는 메시지 처리가 끝난 후 본 함수를 사용하여 자원을 반환해 주어야 한다.
	@author		김용태
	@date		2006.
	*/
	void m_fnFreeMemory(
		void* pMemory					///< 자원을 회수할 메모리 주소
		)
	{
		//BYTE*	bTemp = (BYTE*)((CMDMSG*)pMemory)->pRcvPacket;
		//delete[] bTemp;		// 2016.09.28 가변 사이즈 파라메터 전달 작업
		//free(bTemp);
		g_fnFreeMemory(m_pAppSupporter, (BYTE*)((CMDMSG*)pMemory)->pRcvPacket);
	}

	/** 
	@brief		서버에서 전달된 메시지를 큐에서 하나 꺼내어 Application 에 전달 한다.

	@return		VS64 메시지
	@remark		꺼내진 메시지는 Application 내에서 사용 후 반환 되어야 한다.
	@author		김용태
	@date		2006.
	*/
	BYTE* m_fnPeekMessage()
	{	
		BYTE*	pMsg = NULL; 
		int		nErrorCode = APP_OK;

		while(TRUE)
		{
			pMsg = MessageQueue.m_fnPopMessage();

			if((BYTE*)-1 == pMsg)
			{
//				st_nServerConnected = 0;
				m_bServerConnected = false;

				if(m_bAutoExitFlag)
				{			
					m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Disconnected! Client END.."));
					exit(1);
				}
				else
				{
					m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Disconnected!"));
					nErrorCode = g_fnUnRegisterTask(m_pAppSupporter);
					do 
					{
						Sleep(500);
						//자동 종료를 하지 않는 경우 서버에 자동 재연결을 시도한다.
						nErrorCode = g_fnRegisterTask(m_pAppSupporter, m_uTaskNo, &MessageQueue, m_szServerIP, m_wPortNo);
						//만약 서버에 이미 등록 되어 있다면 에러임
						if(nErrorCode == ERR_CLIENT_REGISTERED)
						{
							m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Reconnet Error!"));
							return (BYTE*)-1;
						}
					} while (nErrorCode != APP_OK);
//					st_nServerConnected = 1;
					m_bServerConnected = true;
					m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Reconnected!"));
					continue;
				}
			}
			else if(NULL == pMsg)
			{
				//메시지 큐에서 메시지를 Pop 했지만 없을 경우 대기 모드로 진입한다.
				::WaitForSingleObject(MessageQueue.m_hEventMessageExist, INFINITE);
				continue;
			}
			break;
		}

		return pMsg;
	}

	/** 
	@brief		서버에서 전달된 메시지를 큐에서 하나 꺼내어 Application 에 전달 한다. Non Block Mode

	@return		VS64 메시지
	@remark		꺼내진 메시지는 Application 내에서 사용 후 반환 되어야 한다. Blocking 이 되지 않는다.
	@author		김용태
	@date		2006.
	*/
	BYTE* m_fnPeekMessageForNonBlock()
	{	
		BYTE*	pMsg = NULL; 
		int		nErrorCode = APP_OK;

		pMsg = MessageQueue.m_fnPopMessage();

		if((BYTE*)-1 == pMsg)
		{
//			st_nServerConnected = 0;
			m_bServerConnected = false;

			if(m_bAutoExitFlag)
			{
				m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Disconnected! Client END.."));
				exit(1);
			}
			else
			{
				m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Disconnected!"));
				nErrorCode = g_fnUnRegisterTask(m_pAppSupporter);
				do 
				{
					Sleep(500);
					//자동 종료를 하지 않는 경우 서버에 자동 재연결을 시도한다.
					nErrorCode = g_fnRegisterTask(m_pAppSupporter, m_uTaskNo, &MessageQueue, m_szServerIP, m_wPortNo);
					//만약 서버에 이미 등록 되어 있다면 에러임
					if(nErrorCode == ERR_CLIENT_REGISTERED)
					{
						m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Reconnet Error!"));
						return (BYTE*)-1;
					}
				} while (nErrorCode != APP_OK);
//				st_nServerConnected = 1;
				m_bServerConnected = true;
				m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Reconnected!"));
			}

		}
		else if(pMsg != NULL)
		{
			pMsg = pMsg;
		}

		return pMsg;
	}

	/** 
	@brief		서버에 등록한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		등록 할 서버가 같은 PC 내에 있지 않다면 IP 주소와 Port 번호를 입력받아야 한다.
	@author		김용태
	@date		2006.
	*/
	int m_fnRegisterTask(
		USHORT uTaskNo,							///< 서버로 등록시 사용할 Task 번호
		TCHAR* pszServerIp = _T("127.0.0.1"),	///< 등록 할 서버의 IP 주소
		WORD wPortNo = 5000,					///< 등록 할 서버의 Port No
		TCHAR* pszServerName = _T("")
		)
	{
		int	nRet;

		m_uTaskNo = uTaskNo;

		if(m_uTaskNo == 0)
		{
			return ERR_CLIENT_REGIST_BAD_ID;
		}

		if(m_szServerIP != NULL)
			delete[] m_szServerIP;

		m_szServerIP = new TCHAR[_tcslen(pszServerIp)+1];
		_tcsncpy(m_szServerIP, pszServerIp, _tcslen(pszServerIp)+1);

		m_szServerName = new TCHAR[_tcslen(pszServerName)+1];
		_tcsncpy(m_szServerName, pszServerName, _tcslen(pszServerName)+1);

		m_wPortNo = wPortNo;

		nRet = g_fnRegisterTask(m_pAppSupporter, uTaskNo, &MessageQueue, m_szServerIP, m_wPortNo);

		if(APP_OK == nRet)
		{
//			st_nServerConnected = 1;
			m_bServerConnected = true;
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Connected."));
		}
		else
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Connect Error! ErrorCode = %d."), nRet);
		return nRet;
	}

		/** 
	@brief		서버에 재등록한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		등록 할 서버가 같은 PC 내에 있지 않다면 IP 주소와 Port 번호를 입력받아야 한다.
	@author		김용태
	@date		2006.
	*/
	int m_fnReRegisterTask(
		)
	{
		int	nRet;
		if(m_uTaskNo == 0)
			return ERR_CLIENT_REGIST_BAD_ID;

		if(m_szServerIP == NULL)
			return ERR_CLIENT_REGIST_CONNREFUSED;

		nRet = g_fnRegisterTask(m_pAppSupporter, m_uTaskNo, &MessageQueue, m_szServerIP, m_wPortNo);

		if(APP_OK == nRet)
		{
//			st_nServerConnected = 1;
			m_bServerConnected = true;
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER ReConnected."));
		}
		else
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER ReConnect Error! ErrorCode = %d."), nRet);
		return nRet;
	}

	/** 
	@brief		서버에 등록 해지한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		
	@author		김용태
	@date		2006.
	*/
	int m_fnUnRegisterTask()
	{
		int	nRet;

		nRet = g_fnUnRegisterTask(m_pAppSupporter);
		m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SERVER Unregited!"));

//		st_nServerConnected = 0;
		m_bServerConnected = false;

		return nRet;
	}

	/** 
	@brief		다른 Task 로 메시지를 전달한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		
	@author		김용태
	@date		2006.
	*/
	int	m_fnSendCommand(
		CMDMSG* sCmdMsg,						///< 전송할 정보를 담은 CMDMSG 구조체 포인터
		USHORT nLogLevel = LOG_LEVEL_4
		)
	{
		int		nResultCode;

		//m_fnPrintLog(nLogLevel, DEFAULT_LOG_SIZE, _T("Send Message. To %d Task, %d, %d Function CALL."),	sCmdMsg->uTask_Dest,
		//																									sCmdMsg->uFunID_Dest,
		//																									sCmdMsg->uSeqID_Dest );
		nResultCode = g_fnCommandSend(m_pAppSupporter, sCmdMsg);

		if(nResultCode != APP_OK)
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("CommandSend Error. To %d Task, %d, %d Function. Error Status is %d."),
																											sCmdMsg->uTask_Dest,
																											sCmdMsg->uFunID_Dest,
																											sCmdMsg->uSeqID_Dest,
																											nResultCode);
		//else
		//	m_fnPrintLog(nLogLevel, DEFAULT_LOG_SIZE, _T("Send Message. To %d Task, %d, %d Function Returned. Return Status is %d."),
		//																									sCmdMsg->uTask_Dest,
		//																									sCmdMsg->uFunID_Dest,
		//																									sCmdMsg->uSeqID_Dest,
		//																									sCmdMsg->uRetStatus);
		return nResultCode;//OK;
	}

	/** 
	@brief		다른 Task 로 응답 메시지를 전달한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		
	@author		김용태
	@date		2006.
	*/
	int	m_fnSendResponseCommand(
		CMDMSG* sCmdMsg,						///< 전송할 정보를 담은 CMDMSG 구조체 포인터
		USHORT nLogLevel = LOG_LEVEL_NONE
		)
	{
		int		nResultCode;

		m_fnPrintLog(nLogLevel, DEFAULT_LOG_SIZE, _T("Response Message. To %d Task, %d, %d Function Returned. Return Status is %d."),
																											sCmdMsg->uTask_Src,
																											sCmdMsg->uFunID_Dest,
																											sCmdMsg->uSeqID_Dest,
																											sCmdMsg->uRetStatus);
		nResultCode = g_fnSendResponseCommand(m_pAppSupporter, sCmdMsg);
		
		if(nResultCode != APP_OK)
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("SendResponseCommand Error. To %d Task, %d, %d Function. Error Status is %d."),
																											sCmdMsg->uTask_Src,
																											sCmdMsg->uFunID_Dest,
																											sCmdMsg->uSeqID_Dest,
																											nResultCode);
		return nResultCode;//OK;
	}

	/** 
	@brief		다른 Task 로 메시지를 전달한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		
	@author		김용태
	@date		2006.
	*/
	int	m_fnSendCommand(
		USHORT uTask_Dest,					///< 목적지 Task 번호
		USHORT uFunID,						///< 호출 할 목적지의 Funtion ID
		USHORT uSeqID,						///< 호출 할 목적지의 Sequence ID
		USHORT uUnitID,						///< 호출 할 목적지의 Unit ID
		USHORT nLogLevel = LOG_LEVEL_4
		)
	{
		int		nResultCode;
		CMDMSG	sCmdMsg;

		//ZeroMemory((void*)&sCmdMsg,sizeof(CMDMSG));

		sCmdMsg.uStation_Src	= 0;
		sCmdMsg.uStation_Dest	= sCmdMsg.uStation_Src;
		sCmdMsg.uTask_Src		= m_uTaskNo;
		sCmdMsg.uTask_Dest		= uTask_Dest;
		sCmdMsg.uFunID_Dest		= uFunID;
		sCmdMsg.uSeqID_Dest		= uSeqID;
		sCmdMsg.uUnitID_Dest	= uUnitID;
		//sCmdMsg.uRetStatus=0;
		sCmdMsg.uFunID_Src		= 0;
		sCmdMsg.uSeqID_Src		= 0;
		sCmdMsg.uUnitID_Src		= 0;
		sCmdMsg.uMsgType		= CMD_TYPE_NORES;
		sCmdMsg.uMsgOrigin		= CMD_TYPE_CMD;

		//m_fnPrintLog(nLogLevel, DEFAULT_LOG_SIZE, _T("Send Message. To %d Task, %d, %d Function CALL."),	sCmdMsg.uTask_Dest,
		//																									sCmdMsg.uFunID_Dest,
		//																									sCmdMsg.uSeqID_Dest );
		nResultCode = g_fnCommandSend(m_pAppSupporter, &sCmdMsg);
		if(nResultCode != APP_OK)
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("CommandSend Error. To %d Task, %d, %d Function. Error Status is %d."),
																											sCmdMsg.uTask_Dest,
																											sCmdMsg.uFunID_Dest,
																											sCmdMsg.uSeqID_Dest,
																											nResultCode);
		//else
		//	m_fnPrintLog(nLogLevel, DEFAULT_LOG_SIZE, _T("Send Message. To %d Task, %d, %d Function Returned. Return Status is %d."),
		//																									sCmdMsg.uTask_Dest,
		//																									sCmdMsg.uFunID_Dest,
		//																									sCmdMsg.uSeqID_Dest,
		//																									sCmdMsg.uRetStatus);
		return nResultCode;//OK;
	}

	/** 
	@brief		다른 Task 로 메시지를 전달한다.

	@return		정상 종료 : 0, 이상 종료 : 그외
	@remark		
	@author		김용태
	@date		2006.
	*/
	int	m_fnSendCommand(
		USHORT uTask_Dest,						///< 목적지 Task 번호
		USHORT uFunID_Dest,						///< 호출 할 목적지의 Funtion ID
		USHORT uSeqID_Dest,						///< 호출 할 목적지의 Sequence ID
		USHORT uUnitID_Dest,					///< 호출 할 목적지의 Unit ID
		ULONG uMsgSize,							///< 전달할 메시지의 Size	// 2016.09.28 가변 사이즈 파라메터 전달 작업
		BYTE *cMsgBuf,							///< 전달할 메시지
		USHORT uFunID_Src = 0,					///< 근원지의 Function ID
		USHORT uSeqID_Src = 0,					///< 근원지의 Sequence ID
		USHORT uUnitID_Src = 0,					///< 근원지의 Unit ID
		USHORT uMsgType = CMD_TYPE_NORES,		///< 메시지가 Response 가 필요한지 여부, CMD_TYPE_NORES/CMD_TYPE_RES
		USHORT uMsgOrigin = CMD_TYPE_CMD,		///< 메시지가 처리에 대한 응답인지 여부, CMD_TYPE_REPLY/CMD_TYPE_CMD
		USHORT uMsgCompressMode = 0,			///< 파라메터 버퍼를 압축할지 여부, 0:압축안함, 1:압축, 2:10MB 이상 자동 압축
		ULONG  uSendMsgSize = 0,				///< uMsgSize 만큼 전송하지 않고 uSendMsgSize 만큼만 전송하면 되는 경우 사용한다. 회신에 데이터를 받는 경우 주로 사용
		USHORT nLogLevel = LOG_LEVEL_4
		)
	{
		int		nResultCode;
		CMDMSG	sCmdMsg;

		//ZeroMemory((void*)&sCmdMsg,sizeof(CMDMSG));

		sCmdMsg.uStation_Src		= 0;
		sCmdMsg.uStation_Dest		= sCmdMsg.uStation_Src;
		sCmdMsg.uTask_Src			= m_uTaskNo;
		sCmdMsg.uTask_Dest			= uTask_Dest;
		sCmdMsg.uFunID_Dest			= uFunID_Dest;
		sCmdMsg.uSeqID_Dest			= uSeqID_Dest;
		sCmdMsg.uUnitID_Dest		= uUnitID_Dest;
		sCmdMsg.uFunID_Src			= uFunID_Src;
		sCmdMsg.uSeqID_Src			= uSeqID_Src;
		sCmdMsg.uUnitID_Src			= uUnitID_Src;
		//sCmdMsg.uRetStatus=0;
		sCmdMsg.uMsgType			= uMsgType;
		sCmdMsg.uMsgOrigin			= uMsgOrigin;
		sCmdMsg.uMsgSize			= uMsgSize;
		sCmdMsg.cMsgBuf				= (INT64)cMsgBuf;
		sCmdMsg.uMsgCompressMode	= uMsgCompressMode;
		sCmdMsg.uSendMsgSize		= uSendMsgSize;

		//m_fnPrintLog(nLogLevel, DEFAULT_LOG_SIZE, _T("Send Message. To %d Task, %d, %d Function CALL."),	sCmdMsg.uTask_Dest,
		//																									sCmdMsg.uFunID_Dest,
		//																									sCmdMsg.uSeqID_Dest );
		nResultCode = g_fnCommandSend(m_pAppSupporter, &sCmdMsg);
		if(nResultCode != APP_OK)
			m_fnPrintLog(LOG_LEVEL_ALWAYS, DEFAULT_LOG_SIZE, _T("CommandSend Error. To %d Task, %d, %d Function. Error Status is %d."),
																											sCmdMsg.uTask_Dest,
																											sCmdMsg.uFunID_Dest,
																											sCmdMsg.uSeqID_Dest,
																											nResultCode);
		//else
		//	m_fnPrintLog(nLogLevel, DEFAULT_LOG_SIZE, _T("Send Message. To %d Task, %d, %d Function Returned. Return Status is %d."),
		//																									sCmdMsg.uTask_Dest,
		//																									sCmdMsg.uFunID_Dest,
		//																									sCmdMsg.uSeqID_Dest,
		//																									sCmdMsg.uRetStatus);

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

	bool m_fnGetServerConnected()
	{
		return m_bServerConnected;
	}

};

/** 
@}
*/ 