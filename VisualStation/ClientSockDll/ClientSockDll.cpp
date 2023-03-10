#include "stdafx.h"
#include "ClientSocketDll.h"
#include "MessageProcessor.h"
#include "MessageReceiver.h"
#include "AppSupporter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//CAppSupporter					AppSupporter(&SendMessageQueue, &ReceiveMessageQueue, &g_bSocketConnection);							//Application Supporter pointer

int g_fnFreeMemory(INT64& AppSupporter, BYTE* rcvPacket)
{
	CAppSupporter* pAppSupporter = (CAppSupporter*)AppSupporter;

	if(NULL == AppSupporter)
	{
		ERROR_RETURN("main dll::g_fnCommandSend -> Invalid Parameter!! VS Server is not connected. : %d\n", ERR_CLIENT_INVALID_PARAMETER);
	}

	pAppSupporter->m_fnFreeMemory(rcvPacket);

	return APP_OK;
}


/*
*	Module name	:	g_fnRegisterTask
*	Parameter	:	TaskNo, Application Callback Function 주소
*	Return		:	정상 종료 : 0
*					이상 종료 : 0을 제외한 나머지 값
*	Function	:	Internal Server 에 Task 를 등록한다.
*	Create		:	2006.03.02 
*	Author		:	김용태
*	Version		:	1.0 
*/
int g_fnRegisterTask(INT64& AppSupporter, USHORT uTaskNo, CMessageQueue<BYTE*>* pMessageQueue, char* pszServerIp/* = "127.0.0.1"*/, WORD wPortNo/* = 5000*/)
{
	int nRet;
	int	nErrorCode = APP_OK;
	CAppSupporter *pAppSupporter;

	//파라메터 유효성 검사
	{
		if(0 >= uTaskNo || 60000 < uTaskNo)
		{
			//잘못된 Client ID임
			ERROR_RETURN("main dll::g_fnRegisterTask -> CLIENTID did not become well. : %d\n", ERR_CLIENT_REGIST_BAD_ID);		
		}

		if(pMessageQueue == NULL)
		{
			//Application cal back 함수가 없다.
			ERROR_RETURN("main dll::g_fnRegisterTask -> There is not APPCALLBACK. : %d\n", ERR_CLIENT_REGIST_CALL_BACK);	
		}
	}
	

	//만일 Client 등록 과정이 한번도 수행 되지 않았다면
	if(AppSupporter == NULL)
	{
		pAppSupporter = new CAppSupporter();

		//Client 등록한다.
		nRet = pAppSupporter->m_fnRegisterTask(uTaskNo, "", pMessageQueue, pszServerIp, wPortNo);

		//만일 결과가 OK 가 아니라면 Error 응답한다.
		if(WSAECONNREFUSED == nRet)	
		{
			SAFE_DELETE(pAppSupporter);
			ERROR_RETURN("main dll::g_fnRegisterTask -> m_fnRegisterTask error. Server was not prepared. : %d\n", ERR_CLIENT_REGIST_CONNREFUSED);
		}
		else if(OKAY != nRet)
		{
			SAFE_DELETE(pAppSupporter);
			ERROR_RETURN("main dll::g_fnRegisterTask -> m_fnRegisterTask error : %d\n", nRet);
		}
		//그렇지 않고 그외라면 정상 처리한다.
		else
		{
			nErrorCode = APP_OK;
		}

		//Client dll 수행 Flag 를 true 로 한다.
		pAppSupporter->m_bIsClientDLLStarted = true;

		AppSupporter = (INT64)pAppSupporter;
	}
	//그렇지 않고 만일 Client 등록 이 한번 이루어 졌다면
	else
	{
		pAppSupporter = (CAppSupporter*)AppSupporter;
		//만일 Socket 이 연결되어져 있지 않다면
		if(pAppSupporter->m_bSocketConnect == false)
		{
			//내부 동적 생성 객체를 소멸시킨다.
			nRet = g_fnUnRegisterTask(AppSupporter);

			if(APP_OK != nRet && ERR_CLIENT_NOT_REGISTERED != nRet)
			{
				ERROR_RETURN("main dll::g_fnRegisterTask -> m_fnUnRegisterTask error : %d\n", nRet);
			}

			//Client 등록한다.
			nRet = pAppSupporter->m_fnRegisterTask(uTaskNo, "", pMessageQueue, pszServerIp, wPortNo);

			//만일 결과가 OK 가 아니라면 Error 응답한다.
			if(WSAECONNREFUSED == nRet)	
			{
				//Server 가 준비 되지 않았다.
				ERROR_RETURN("main dll::g_fnRegisterTask -> m_fnRegisterTask Fail. Server was not prepared. : %d\n", ERR_CLIENT_REGIST_CONNREFUSED);
			}
			else if(OKAY != nRet)
			{
				ERROR_RETURN("main dll::g_fnRegisterTask -> m_fnRegisterTask Fail : %d\n", nRet);
			}
			else
			{
				pAppSupporter->m_bIsClientDLLStarted = true;
				nErrorCode = APP_OK;
			}

		}
		//그렇지 않고 만일 Socket 이 연결 되어 있다면
		else
		{
			//Client 가 이미 등록 되어있으므로 Error 응답한다.
			ERROR_RETURN("main dll::g_fnRegisterTask -> Currently Client is being registered already in Server. : %d\n", ERR_CLIENT_REGISTERED);
		}
	}

	return nErrorCode;
}

/*
*	Module name	:	g_fnRegisterTask
*	Parameter	:	TaskNo, Application Callback Function 주소
*	Return		:	정상 종료 : 0
*					이상 종료 : 0을 제외한 나머지 값
*	Function	:	Internal Server 에 Task 를 등록한다. 유니코드 대응
*	Create		:	2006.03.02 
*	Author		:	김용태
*	Version		:	1.0 
*/
int g_fnRegisterTask(INT64& AppSupporter, USHORT uTaskNo, CMessageQueue<BYTE*>* pMessageQueue, wchar_t* pszServerIp/* = "127.0.0.1"*/, WORD wPortNo/* = 5000*/)
{
	char* pStr;
	int	nRet = APP_OK;

	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, pszServerIp, -1, NULL, 0,NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];

	//형 변환 
	WideCharToMultiByte(CP_ACP, 0, pszServerIp, -1, pStr, strSize, 0,0);

	nRet = g_fnRegisterTask(AppSupporter, uTaskNo, pMessageQueue, pStr, wPortNo);

	delete[] pStr;

	return nRet;
}

/*
*	Module name	:	g_fnUnRegisterTask
*	Parameter	:	TaskNo
*	Return		:	정상 종료 : 0
*					이상 종료 : 0을 제외한 나머지 값
*	Function	:	Internal Server 에 Task 를 해제한다.
*	Create		:	2006.03.02
*	Author		:	김용태
*	Version		:	1.0
*/
int g_fnUnRegisterTask(INT64& AppSupporter)
{
	int nRet;
	int	nErrorCode = APP_OK;
	CAppSupporter* pAppSupporter = (CAppSupporter*)AppSupporter;

	//RegisterTask 가 수행되었다면 
	if(pAppSupporter != NULL && pAppSupporter->m_bIsClientDLLStarted)
	{
		nRet = pAppSupporter->m_fnUnRegisterTask();
		if(OKAY == nRet)
		{
			nErrorCode = APP_OK;
			pAppSupporter->m_bIsClientDLLStarted = false;
		}
		else
		{
			ERROR_RETURN("main dll::g_fnUnRegisterTask -> m_fnUnRegisterTask Fail!! : %d\n", nRet);
		}
	}
	else
	{
		ERROR_RETURN("main dll::g_fnUnRegisterTask -> It was not registered in Server. : %d\n", ERR_CLIENT_NOT_REGISTERED);
	}
	return nErrorCode;
}


/*
*	Module name	:	g_fnCommandSend
*	Parameter	:	CMDMSG 구조체 주소
*	Return		:	정상 종료 : 0
*					이상 종료 : 0을 제외한 나머지 값
*	Function	:	다른 Task로 CMD를 송신한다.
*	Create		:	2006.03.02
*	Author		:	김용태
*	Version		:	1.0
*/
int g_fnCommandSend(INT64& AppSupporter, CMDMSG* pCmdMsg)
{
	int nRet;
	int	nErrorCode = APP_OK;
	CAppSupporter* pAppSupporter = (CAppSupporter*)AppSupporter;

	//파라메터 유효성 검사
	{
		if(0 > pCmdMsg->uTask_Src)
		{
			ERROR_RETURN("main dll::g_fnCommandSend -> Invalid Parameter!! The SourceTaskNo escaped a scope. : %d\n", ERR_CLIENT_INVALID_PARAMETER);
		}
		if(0 > pCmdMsg->uTask_Dest)
		{
			ERROR_RETURN("main dll::g_fnCommandSend -> Invalid Parameter!! The DestTaskNo escaped a scope. : %d\n", ERR_CLIENT_INVALID_PARAMETER);
		}
		if(MAX_CLIENT_MESSAGE_BUFFER < pCmdMsg->uMsgSize)
		{
			ERROR_RETURN("main dll::g_fnCommandSend -> Invalid Parameter!! Transmission it exceeded the size which is possible. : %d\n", ERR_CLIENT_INVALID_PARAMETER);
		}
		if(NULL == AppSupporter)
		{
			ERROR_RETURN("main dll::g_fnCommandSend -> Invalid Parameter!! VS Server is not connected. : %d\n", ERR_CLIENT_INVALID_PARAMETER);
		}
	}

	//만일 Client 등록이 수행되었고 소켓이 연결된 상태라면 Client Command 전달기능을 요청한다.
	if(pAppSupporter->m_bIsClientDLLStarted && pAppSupporter->m_bSocketConnect)
	{
		nRet = pAppSupporter->m_fnSendCommand(pCmdMsg);
		ERROR_RETURN("main dll::g_fnCommandSend -> m_fnCommandSend error : %d \n",nRet);
	}
	else
	{
		ERROR_RETURN("main dll::g_fnCommandSend -> It was not registered in Server. : %d\n", ERR_CLIENT_NOT_REGISTERED);
	}

	nErrorCode = pCmdMsg->uRetStatus;

	return nErrorCode;
}


/*
*	Module name	:	g_fnSendResponsCommand
*	Parameter	:	CMDMSG 구조체 주소
*	Return		:	정상 종료 : 0
*					이상 종료 : 0을 제외한 나머지 값
*	Function	:	다른 Task로 Response CMD를 송신한다.
*	Create		:	2014.09.01
*	Author		:	김용태
*	Version		:	1.0
*/
int g_fnSendResponseCommand(INT64& AppSupporter, CMDMSG* pCmdMsg)
{
	int nRet;
	int	nErrorCode = APP_OK;
	CAppSupporter* pAppSupporter = (CAppSupporter*)AppSupporter;

	//파라메터 유효성 검사
	{
		if(0 > pCmdMsg->uTask_Src)
		{
			ERROR_RETURN("main dll::g_fnCommandSend -> Invalid Parameter!! The SourceTaskNo escaped a scope. : %d\n", ERR_CLIENT_INVALID_PARAMETER);
		}
		if(0 > pCmdMsg->uTask_Dest)
		{
			ERROR_RETURN("main dll::g_fnCommandSend -> Invalid Parameter!! The DestTaskNo escaped a scope. : %d\n", ERR_CLIENT_INVALID_PARAMETER);
		}
		if(MAX_CLIENT_MESSAGE_BUFFER < pCmdMsg->uMsgSize)
		{
			ERROR_RETURN("main dll::g_fnCommandSend -> Invalid Parameter!! Transmission it exceeded the size which is possible. : %d\n", ERR_CLIENT_INVALID_PARAMETER);
		}
		if(NULL == AppSupporter)
		{
			ERROR_RETURN("main dll::g_fnCommandSend -> Invalid Parameter!! VS Server is not connected. : %d\n", ERR_CLIENT_INVALID_PARAMETER);
		}
	}

	//만일 Client 등록이 수행되었고 소켓이 연결된 상태라면 Client Command 전달기능을 요청한다.
	if(pAppSupporter->m_bIsClientDLLStarted && pAppSupporter->m_bSocketConnect)
	{
		nRet = pAppSupporter->m_fnSendResponseCommand(pCmdMsg);
		ERROR_RETURN("main dll::g_fnCommandSend -> m_fnCommandSend error : %d \n",nRet);
	}
	else
	{
		ERROR_RETURN("main dll::g_fnCommandSend -> It was not registered in Server. : %d\n", ERR_CLIENT_NOT_REGISTERED);
	}

	return nErrorCode;
}

int g_fnDeleteAppSupporter(INT64& AppSupporter)
{
//	int nRet;
	int	nErrorCode = APP_OK;
	CAppSupporter* pAppSupporter = (CAppSupporter*)AppSupporter;

	if(NULL == AppSupporter)
	{
		ERROR_RETURN("main dll::g_fnCommandSend -> Invalid Parameter!! VS Server is not connected. : %d\n", ERR_CLIENT_INVALID_PARAMETER);
	}

	SAFE_DELETE(pAppSupporter);

	AppSupporter = NULL; 

	return nErrorCode;
}