#include "StdAfx.h"
#include ".\messageprocessor.h"
#include "snappy-c.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
*	Module name	:	CMessageProcessor
*	Parameter	:	Task No, 메시지큐 포인터, Application의 Callback 함수 Pointer, 메시지 처리자의 처리 모드
*	Return		:	없음
*	Function	:	생성자
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
CMessageProcessor::CMessageProcessor(USHORT uTaskNo, CMessageQueue<BYTE*>* pMessageQueue, CMessageQueue<BYTE*>* pAppPeekMessageQueue, USHORT uProcessMode, 
									 SOCKET hClientSocket, bool* pbSocketConnect /* = NULL */)
{
	//파라메터 정보를 참고하여 멤버 변수를 설정한다.
	m_uTaskNo				= uTaskNo;
	m_pMessageQueue			= pMessageQueue;
	m_uProcessMode			= uProcessMode;
	m_hClientSocket			= hClientSocket;
	m_pszBuffer				= NULL;
	m_pbSocketConnect		= pbSocketConnect;
	m_bRun					= true;
	m_pAppPeekMessageQueue	= pAppPeekMessageQueue;


	m_hWaitStop			= CreateEvent(NULL, TRUE, FALSE, NULL);
	
	InitializeCriticalSection(&m_csMessageSend);
}

/*
*	Module name	:	~CMessageProcessor
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	생성자
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
CMessageProcessor::~CMessageProcessor(void)
{

	//스레스 수행 Flag 를 false 로 해서 쓰레드를 종료 하게 한다.
	m_bRun = false;
	SetEvent(m_hWaitStop);
	Sleep(10);

	CloseHandle(m_hWaitStop);

	SuspendThread(m_hThreadHandle);
	
	if(NULL != m_pszBuffer)
		//delete (PPER_PACKET_BUFFER)m_pszBuffer;
		//MemoryPool.Free((PER_PACKET_BUFFER*)m_pszBuffer);
		delete[] m_pszBuffer;

	TerminateThread(m_hThreadHandle,NULL);
	m_fnWaitForTermination();
	
	//Socket 연결 유무 Flag 를 False 로 한다.
	if(NULL != m_pbSocketConnect)
		*m_pbSocketConnect = false;

	DeleteCriticalSection(&m_csMessageSend);
}

/*
*	Module name	:	m_fnStartMessageProcessor
*	Parameter	:	없음
*	Return		:	정상 종료 : 생성된 Thread 의 Handle
*					이상 종료 : NULL 
*	Function	:	메시지 처리자를 실행 시킨다. Thread 생성
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
HANDLE CMessageProcessor::m_fnStartMessageProcessor()
{
	
	//Thread를 생성한다
	//만일 Thread 생성 핸들 값이 Null 이 아니라면 Thread 의 핸들 값을 반환한다.
	//그렇지 않고 만일 Null 이라면 Null을 반환한다.
	m_bRun = true;
	ResetEvent(m_hWaitStop);
	m_hThreadHandle = m_fnStartThread();
	return m_hThreadHandle;
}

/*
*	Module name	:	m_fnStopMessageProcessor
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	메시지 처리자 쓰레드를 종료 시킨다.
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
void CMessageProcessor::m_fnStopMessageProcessor()
{

	m_bRun = false;
	SetEvent(m_hWaitStop);
	Sleep(20);
	return;
}

/*
*	Module name	:	m_fnRunThread
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	메시지 처리자 Thread 기능을 담당한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
void CMessageProcessor::m_fnRunThread()
{
	int					nRet;
	PACKET_HEADER*		pPacketHeader;
	PACKET_DATA_TYPE	ePacketDataType;	
	HANDLE Event[2];

	Event[0] = m_pMessageQueue->m_hEventMessageExist;
	Event[1] = m_hWaitStop;

	//구문을 반복 한다. 쓰레드 수행 플레그가 False 일때 까지.
	while(m_bRun)
	{

		//Queue 에서 메시지를 하나 꺼낸다.
		m_pszBuffer = m_pMessageQueue->m_fnPopMessage();
		//TRACE("%d : 0\n", m_uProcessMode);
		//만일 메시지가 없다면
		if(m_pszBuffer == NULL )
		{
			//이벤트를 초기화 한다.
			//ResetEvent(m_pMessageQueue->m_hEventMessageExist);
			//TRACE("%d : 1\n", m_uProcessMode);
			//Thread 를 대기 모드로 진입 시킨다.
			//::WaitForSingleObject(m_pMessageQueue->m_hEventMessageExist, INFINITE);
			//대기 모드가 풀리면 Continue 한다.
			//TRACE("%d : 2\n", m_uProcessMode);
			nRet = WaitForMultipleObjects(2, Event, FALSE, INFINITE);
			continue;
		}
		//TRACE("%d : 3\n", m_uProcessMode);
		//Header 의 정보를 보고 메시지 Type 이 무엇인지 확인한다.
		{
			pPacketHeader = (PACKET_HEADER*)m_pszBuffer;
			ePacketDataType = pPacketHeader->PacketDataType;
		}

		//쓰레드 종료 메시지 인지 체크한다.
		{
			if(0 == pPacketHeader->uPacketSize)
			{
				/*if(m_pszBuffer)
				{
					delete (PPER_PACKET_BUFFER)m_pszBuffer;
					m_pszBuffer = NULL;
				}*/
				//MemoryPool.Free((PER_PACKET_BUFFER*)m_pszBuffer);
				if(m_pszBuffer)
				{
					delete[] m_pszBuffer;
					m_pszBuffer = NULL;
				}
				continue;
			}
		}
		
		//메시지 type 을 보고 그에 따른 처리를 한다.
		{
			switch(ePacketDataType)
			{
				//만일 Client Messag 라면
			case CLIENT_MESSAGE_DATA:
				{
					//Client Message 처리 요청 한다.
					nRet = m_fnProcessClientMessageData(m_pszBuffer);
					if(OKAY != nRet)
					{
						TRACE("CMessageProcessor::m_fnRunThread -> m_fnProcessClientMessageData error : %d\n", nRet);
					}
					break;
				}
				//만일 메모리 참조 메시지 라면
			case MEMORY_MESSAGE_DATA:
				{
					//메모리 참조 메시지 처리 요청 한다.
					nRet = m_fnProcessMemoryMessageData(m_pszBuffer);
					if(OKAY != nRet)
					{
						TRACE("CMessageProcessor::m_fnRunThread -> m_fnProcessMemoryMessageData error : %d\n", nRet);
					}
					break;
				}
				//만일 Visual Station 명령이라면
			case VS_COMMAND_DATA:
				{
					//Visual Station 명령 처리 요청 한다.
					nRet = m_fnProcessVsCommandData(m_pszBuffer);
					if(OKAY != nRet)
					{
						TRACE("CMessageProcessor::m_fnRunThread -> m_fnProcessVsCommandData error : %d\n", nRet);
					}
					break;
				}
				//만일 Visual Station 등록 메시지 라면
			case CLIENT_VS_REGIST_DATA:
				{
					//현재 구현 상으로 본 구문으로 집입할 이유가 없다. App supporter에서 이미 처리가 끝나기 때문
					nRet = m_fnProcessClientVsRegistData(m_pszBuffer);
					if(OKAY != nRet)
					{
						TRACE("CMessageProcessor::m_fnRunThread -> m_fnProcessClientVsRegistData error : %d\n", nRet);
					}
					break;
				}
				//만일 그외 메시지 라면 오류이다.
			default:
				{
					/*if(m_pszBuffer)
					{
						delete (PPER_PACKET_BUFFER)m_pszBuffer;
						m_pszBuffer = NULL;
					}*/
					//MemoryPool.Free((PER_PACKET_BUFFER*)m_pszBuffer);
					if(m_pszBuffer)
					{
						delete[] m_pszBuffer;
						m_pszBuffer = NULL;
					}
					TRACE("CMessageProcessor::m_fnRunThread -> PacketDataType is not exist.\n");
					break;
				}
			}
		}
		m_pszBuffer = NULL;
	}
}

/*
*	Module name	:	m_fnProcessClientMessageData
*	Parameter	:	Buffer Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	메시지 큐에서 꺼낸 내용인 ClientMessageData 를 처리한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CMessageProcessor::m_fnProcessClientMessageData(BYTE* pszBuffer)
{
	int		nRet;
	int		nErrorCode = OKAY;

	
	//처리 구분에 따른 처리를 한다.
	switch(m_uProcessMode)
	{
		//만일 Send Mode 로 동작 하고 있다면
	case PROCESSOR_MODE_SEND:
		{
		/*	{
				PPACKET_HEADER			pPacketHeader;
				PPACKET_CLIENT_MESSAGE	pPacketClientMessage;
				CHAR*					pBuff;
				CMDMSG*					pCmdMsg;

				pPacketHeader			= (PPACKET_HEADER)pszBuffer;
				pPacketClientMessage	= (PPACKET_CLIENT_MESSAGE)(pszBuffer + sizeof(PACKET_HEADER));

				pCmdMsg = (CMDMSG*)pPacketClientMessage->cParamBuf;

				pBuff = pPacketClientMessage->cParamBuf + sizeof(CMDMSG);


				TRACE("MessageProcessor(Snend) : %s\n",pBuff);
			}*/

			//SYSTEMTIME systime;						//System 시간을 알기 위한 변수
			//::GetLocalTime(&systime);
			//TRACE("[%02d:%02d:%02d.%03d] Msg Send.  \n", systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);

			//메시지를 VS Server 로 전송한다.
			nRet = m_fnSendMessage(pszBuffer);
			if(OKAY != nRet)
			{
				nErrorCode = nRet;
				TRACE("CMessageProcessor::m_fnProcessClientMessageData -> m_fnSendMessage error : %d\n", nErrorCode);
			}

			//SYSTEMTIME systime;						//System 시간을 알기 위한 변수
			//::GetLocalTime(&systime);
			//TRACE("[%02d:%02d:%02d.%03d] Msg Send End.  \n", systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);

			if (pszBuffer != NULL)
			{
				delete[] pszBuffer;
				pszBuffer = NULL;
			}
			
			break;
		}
		//만일 Receive Mode 로 동작 하고 있다면
	case PROCESSOR_MODE_RECEIVE:
		{
			CMDMSG*					pCmdMsg;				
			PACKET_HEADER*			pPacketHeader;
			PACKET_CLIENT_MESSAGE*	pPacketClientMessage;

			//Application 에 전달 할 CmdMsg 구조체를 완성 시킨다.
			{
				
				pPacketHeader			= (PACKET_HEADER*)pszBuffer;
				pPacketClientMessage	= (PACKET_CLIENT_MESSAGE*)(pszBuffer + sizeof(PACKET_HEADER));
				pPacketClientMessage->pParamBuf = (INT64)pPacketClientMessage + sizeof(PACKET_CLIENT_MESSAGE);
				pCmdMsg					= (CMDMSG*)pPacketClientMessage->pParamBuf;
				pCmdMsg->cMsgBuf		= (INT64)pCmdMsg + sizeof(CMDMSG);
				pCmdMsg->pRcvPacket		= (INT64)pszBuffer;
			}
			// 만약 메시지가 커맨드라면
			if(pCmdMsg->uMsgOrigin == CMD_TYPE_CMD)
			{
				if (pCmdMsg->uCompressedMsgSize > 0)
				{
					char* pCompressedMsgBuf = (char*)pCmdMsg->cMsgBuf;
					pCmdMsg->cCompressedMsgBuf = NULL;
					pCmdMsg->cMsgBuf = NULL;

					size_t length;
					snappy_status status1 = snappy_uncompressed_length(pCompressedMsgBuf, pCmdMsg->uCompressedMsgSize, &length);


					char *uncompressed = new char[length];

					snappy_status status2 = snappy_uncompress(pCompressedMsgBuf, pCmdMsg->uCompressedMsgSize, uncompressed, &length);

					pCmdMsg->cMsgBuf = (INT64)uncompressed;
					pCmdMsg->uMsgSize = (ULONG)length;
				}

				//메시지를 Application 으로 전송한다.
				//nRet = m_fnAppCallBackFunc(pCmdMsg, (UCHAR*)pCmdMsg->cMsgBuf);
				nRet = m_pAppPeekMessageQueue->m_fnPushMessage((BYTE*)pCmdMsg);

				break;

			}
			// 그렇지 않고 만약 회신 커맨드 라면, 회신 결과 값과 파라메터를 근원 CmdMsg 에 세팅하고 회신이벤트를 발생시킨다. 
			else if(pCmdMsg->uMsgOrigin == CMD_TYPE_REPLY)
			{
				CMDMSG	*pSrcCmdMsg;
				try
				{
					pSrcCmdMsg = (CMDMSG*)pCmdMsg->pCmdMsg;

					if(pSrcCmdMsg->lUniqueNo != pCmdMsg->lUniqueNo) 
					{
						if (pszBuffer != NULL)
						{
							delete[] pszBuffer;
							pszBuffer = NULL;
						}
						break;
					}
					pSrcCmdMsg->uRetStatus = pCmdMsg->uRetStatus;
					pSrcCmdMsg->uMsgSize = pCmdMsg->uMsgSize;
				}
				catch (...)
				{
					//pSrcCmdMsg가 소멸된 경우 메모리 참조 오류발생, 에러처리 필요. Todo
					TRACE("CMessageProcessor::m_fnProcessClientMessageData -> pSrcCmdMsg is Deleted. error.\n");
					break;
				}
				try
				{
					if(pSrcCmdMsg->cMsgBuf != NULL)
					{
						if (pCmdMsg->uCompressedMsgSize > 0)
						{
							pCmdMsg->cCompressedMsgBuf = pCmdMsg->cMsgBuf;
							pCmdMsg->cMsgBuf = NULL;

							size_t length;
							snappy_status status1 = snappy_uncompressed_length((char*)pCmdMsg->cCompressedMsgBuf, pCmdMsg->uCompressedMsgSize, &length);

							if (length > pSrcCmdMsg->uMsgSize)
							{
								length = pSrcCmdMsg->uMsgSize;
							}
							//char *uncompressed = new char[length];
							
							snappy_status status2 = snappy_uncompress((char*)pCmdMsg->cCompressedMsgBuf, pCmdMsg->uCompressedMsgSize, (char*)pSrcCmdMsg->cMsgBuf, &length);

						}
						else
							memcpy((void*)pSrcCmdMsg->cMsgBuf, (void*)pCmdMsg->cMsgBuf, pSrcCmdMsg->uMsgSize);
					}
				}
				catch (...)
				{
					TRACE("CMessageProcessor::m_fnProcessClientMessageData -> cMsgBuf of pSrcCmdMsg is NULL. error.\n");
					pSrcCmdMsg->uRetStatus = ERR_CLIENT_PARAM_BUFFER_IS_NULL;

				}
				//MemoryPool.Free((PER_PACKET_BUFFER*)pBuff);
				if (pszBuffer != NULL)
				{
					delete[] pszBuffer;
					pszBuffer = NULL;
				}

				SetEvent((HANDLE)pSrcCmdMsg->hRspEvent);
				
			}
			else
			{
				//MemoryPool.Free((PER_PACKET_BUFFER*)pBuff);
				if (pszBuffer != NULL)
				{
					delete[] pszBuffer;
					pszBuffer = NULL;
				}

				nErrorCode = NG;
				TRACE("CMessageProcessor::m_fnProcessClientMessageData -> CmdMsg.uMsgOrigin is invalid. %d. \n", pCmdMsg->uMsgOrigin);
			}



			//결과 값 반환이 필요하면 처리가 필요하다.. Work Item
			break;
		}
		//만일 그외 라면
	default:
		{
			//이곳으로 진입 한다면 Error 이다.
			nErrorCode = NG;
			TRACE("CMessageProcessor::m_fnProcessClientMessageData -> m_uProcessMode is %d. error.\n", m_uProcessMode);
			break;
		}
	}

	/*if(pszBuffer)
	{
		delete (PPER_PACKET_BUFFER)pszBuffer;
		pszBuffer = NULL;
	}*/
	//MemoryPool.Free((PER_PACKET_BUFFER*)pszBuffer);
	//if (pszBuffer != NULL)
	//{
	//	delete[] pszBuffer;
	//	pszBuffer = NULL;
	//}

	return nErrorCode;
}

/*
*	Module name	:	m_fnProcessMemoryMessageData
*	Parameter	:	Buffer Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	메시지 큐에서 꺼낸 내용인 MemoryMessageData 를 처리한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CMessageProcessor::m_fnProcessMemoryMessageData(BYTE* pszBuffer)
{
	if(m_pszBuffer)
	{
		delete[] m_pszBuffer;
		m_pszBuffer = NULL;
	}
	return OKAY;
	//int		nRet;
	//int		nErrorCode = OKAY;

	////처리 구분에 따른 처리를 한다.
	//switch(m_uProcessMode)
	//{
	//	//만일 Send Mode 로 동작 하고 있다면
	//case PROCESSOR_MODE_SEND:
	//	{
	//		//메시지를 VS Server 로 전송한다.  pszBuffer memory delete 는 되어지면 안된다.. Receive 시 이루어 진다.
	//		nRet = m_fnSendMessage(pszBuffer);
	//		if(OKAY != nRet)
	//		{
	//			nErrorCode = nRet;
	//			TRACE("CMessageProcessor::m_fnProcessMemoryMessageData -> m_fnSendMessage error : %d\n", nErrorCode);
	//		}
	//		break;
	//	}
	//	//만일 Receive Mode 로 동작 하고 있다면
	//case PROCESSOR_MODE_RECEIVE:
	//	{
	//		PACKET_HEADER*			pPacketHeader;
	//		PACKET_MEMORY_MESSAGE*	pPacketMemoryMessage, *pOriginalMemoryMessage;

	//		//Buffer 에서 Packet 의 Header 와 Body 의 시작 Pointer 를 구한다.
	//		pPacketHeader			= (PACKET_HEADER*)pszBuffer;
	//		pPacketMemoryMessage	= (PACKET_MEMORY_MESSAGE*)(pszBuffer + sizeof(PACKET_HEADER));
	//		
	//		//근원지에서 발생된 메시지의 Pointer 를 가져온다. 메시지 생성, 전송 후 응답을 위해 AppSupporter의 Thread는 대기 하고 있고 이후 메시지는 삭제된다.
	//		pOriginalMemoryMessage	= (PACKET_MEMORY_MESSAGE*)pPacketMemoryMessage->pthis;
	//		//SMP의 반환값을 설정한다.
	//		pOriginalMemoryMessage->uSMPReturnValue = pPacketMemoryMessage->uSMPReturnValue;
	//		//SMP의 실제 Read/Write 된 Size 를 설정 한다.
	//		pOriginalMemoryMessage->uSize = pPacketMemoryMessage->uSize;
	//		//메시지 발생시의 생성된 Buffer 에 실제 가지고온 파라메터 값을 넣는다.
	//		memcpy(pOriginalMemoryMessage->cParamBuf, pPacketMemoryMessage->cParamBuf, pPacketMemoryMessage->uSize);
	//		//응답 대기중인 Thread 를 깨운다.
	//		SetEvent(pOriginalMemoryMessage->hEvent);
	//		//서버에서 전송한 메시지를 담기위한 버퍼를 지운다.
	//		/*if(pszBuffer)
	//		{
	//			delete (PPER_PACKET_BUFFER)pszBuffer;
	//			pszBuffer = NULL;
	//		}*/
	//		MemoryPool.Free((PER_PACKET_BUFFER*)pszBuffer);
	//		break;
	//	}
	//	//만일 그외 라면
	//default:
	//	{
	//		//이곳으로 진입 한다면 Error 이다.
	//		/*if(pszBuffer)
	//		{
	//			delete (PPER_PACKET_BUFFER)pszBuffer;
	//			pszBuffer = NULL;
	//		}*/
	//		MemoryPool.Free((PER_PACKET_BUFFER*)pszBuffer);
	//		nErrorCode = NG;
	//		TRACE("CMessageProcessor::m_fnProcessMemoryMessageData -> m_uProcessMode is %d. error : %d\n", nErrorCode);
	//		break;
	//	}
	//}

	//return nErrorCode;
}

/*
*	Module name	:	m_fnProcessVsCommandData
*	Parameter	:	Buffer Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	메시지 큐에서 꺼낸 내용인 VsCommandData 를 처리한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CMessageProcessor::m_fnProcessVsCommandData(BYTE* pszBuffer)
{
//	int		nRet;
	int		nErrorCode = OKAY;

	//VS Command Message 처리, Link Test, 연결 해지 통보 처리 
	/*if(pszBuffer)
	{	
		delete (PPER_PACKET_BUFFER)pszBuffer;
		pszBuffer = NULL;
	}	*/
	// MemoryPool.Free((PER_PACKET_BUFFER*)pszBuffer);
	if(m_pszBuffer)
	{
		delete[] m_pszBuffer;
		m_pszBuffer = NULL;
	}

	return nErrorCode;
}

/*
*	Module name	:	m_fnProcessClientVsRegistData
*	Parameter	:	Buffer Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	메시지 큐에서 꺼낸 내용인 ClientVsRegistData 를 처리한다... 
*					현재 이 함수로 들어올 이유가 없다. 클라이언트 등록은 AppSupportor 에서 하기때문임
*					만약 있다면 할당되었던 메모리 공간을 지우고 끝낸다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CMessageProcessor::m_fnProcessClientVsRegistData(BYTE* pszBuffer)
{
	//int		nRet;
	int		nErrorCode = OKAY;

	nErrorCode = NG;

	/*if(pszBuffer)
	{
		delete (PPER_PACKET_BUFFER)pszBuffer;
		pszBuffer = NULL;
	}*/
	// MemoryPool.Free((PER_PACKET_BUFFER*)pszBuffer);
	if(m_pszBuffer)
	{
		delete[] m_pszBuffer;
		m_pszBuffer = NULL;
	}

	return nErrorCode;
}

/*
*	Module name	:	m_fnSendMessage
*	Parameter	:	Buffer Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Buffer에 있는 Message를 Server 로 전송한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CMessageProcessor::m_fnSendMessage(BYTE* buffer, int len)
{
	int						nRet			= 0;
	int						nSendOffset		= 0;
	int						nErrorCode		= OKAY;

	while(1)
	{
		//Server 로 Packet 전송을 한다.
		nRet = send(m_hClientSocket,(char*) buffer + nSendOffset, len - nSendOffset, 0);

		//만일 Socket Error 가 발생 되었다면
		if(SOCKET_ERROR == nRet)	
		{	
			nErrorCode = WSAGetLastError();
			//Application 으로 Socket 종료를 알린다.
			m_fnSendToAppSocketCloseMessage();

			//이상 종료 한다.
			TRACE("CMessageProcessor::m_fnSendMessage -> send error : %d\n", nErrorCode);
			return nErrorCode;
		}	

		//만일 송신한 Packet 이 실제 필요한 전체 Packet 크기 보다 작다면
		if(len > nSendOffset + nRet)
		{	
			TRACE("CMessageProcessor::m_fnSendMessage -> uSendSize > nSendOffset + nRet");

			//총 송신 크기를 저장한다.
			nSendOffset += nRet;
			//다시 송신 시도한다.
			continue;
		}
		//만일 송신한 Pacekt 이 실제 필요한 만큼 전송 되었다면
		//구간 반복을 빠져나온다.
		break;
	}	

	return nErrorCode;
}

/*
*	Module name	:	m_fnSendMessage
*	Parameter	:	Buffer Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Buffer에 있는 Message를 Server 로 전송한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CMessageProcessor::m_fnSendMessage(BYTE* pszBuffer)
{
	int						nRet;
	int						nErrorCode = OKAY;
	ULONG						nSendOffset	= 0;
	ULONG					uSendSize;
	PACKET_HEADER*			pPacketHeader;

	EnterCriticalSection(&m_csMessageSend);

	//총 패킷 사이즈를 구한다.
	pPacketHeader	= (PACKET_HEADER*)pszBuffer;
	uSendSize		= pPacketHeader->uPacketSize;

	if (pPacketHeader->PacketDataType != CLIENT_MESSAGE_DATA)
	{
		//구간 반복 한다.
		while(1)
		{
			//Server 로 Packet 전송을 한다.
			nRet = send(m_hClientSocket, (char*)pszBuffer + nSendOffset, uSendSize - nSendOffset, 0);

			//만일 Socket Error 가 발생 되었다면
			if(SOCKET_ERROR == nRet)	
			{	
				nErrorCode = WSAGetLastError();
				//Application 으로 Socket 종료를 알린다.
				m_fnSendToAppSocketCloseMessage();

				//이상 종료 한다.
				TRACE("CMessageProcessor::m_fnSendMessage -> send error : %d\n", nErrorCode);
				LeaveCriticalSection(&m_csMessageSend);
				return nErrorCode;
			}	
																
			//만일 송신한 Packet 이 실제 필요한 전체 Packet 크기 보다 작다면
			if(uSendSize > nSendOffset + nRet)
			{	
				TRACE("CMessageProcessor::m_fnSendMessage -> uSendSize > nSendOffset + nRet");

				//총 송신 크기를 저장한다.
				nSendOffset += nRet;
				//다시 송신 시도한다.
				continue;
			}
			//만일 송신한 Pacekt 이 실제 필요한 만큼 전송 되었다면
			//구간 반복을 빠져나온다.
			break;
		}		
	}
	else
	{
		//uSendSize = 0;
		PACKET_CLIENT_MESSAGE* pPacketClientMessage = (PACKET_CLIENT_MESSAGE*)(pszBuffer + sizeof(PACKET_HEADER));
		CMDMSG* pCmdmsg = (CMDMSG*)pPacketClientMessage->pParamBuf;

		if (pPacketClientMessage->uTaskNo2 == 0)
		{
			LeaveCriticalSection(&m_csMessageSend);
			return OKAY;
		}
		

		// PACKET_HEADER 와 PACKET_CLIENT_MESSAGE 를 보낸다
		nRet = m_fnSendMessage(pszBuffer, sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE));
		if (nRet != OKAY)
		{
			LeaveCriticalSection(&m_csMessageSend);
			return nRet;
		}

		// CMDMSG 를 보낸다.
		nRet = m_fnSendMessage((BYTE*)pCmdmsg, sizeof(CMDMSG));
		if (nRet != OKAY)
		{
			LeaveCriticalSection(&m_csMessageSend);
			return nRet;
		}

		// CMDMSG의 파라메터를 보낸다.
		if (pCmdmsg->uCompressedMsgSize > 0)
			nRet = m_fnSendMessage((BYTE*)pCmdmsg->cCompressedMsgBuf, pCmdmsg->uCompressedMsgSize);
		else if (pCmdmsg->uSendMsgSize > 0)
			nRet = m_fnSendMessage((BYTE*)pCmdmsg->cMsgBuf, pCmdmsg->uSendMsgSize);
		else
			nRet = m_fnSendMessage((BYTE*)pCmdmsg->cMsgBuf, pCmdmsg->uMsgSize);

		if (nRet != OKAY)
		{
			LeaveCriticalSection(&m_csMessageSend);
			return nRet;
		}
		
		SetEvent((HANDLE)pCmdmsg->hSentEvent);
		
	}

	LeaveCriticalSection(&m_csMessageSend);
	return nErrorCode;
}

/*
*	Module name	:	m_fnSendToAppSocketCloseMessage
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Application 으로 Socket 이 끊겼음을 알린다.
*	Create		:	2006.04.05
*	Author		:	김용태
*	Version		:	1.0
*/
int CMessageProcessor::m_fnSendToAppSocketCloseMessage()
{
	int nErrorCode = OKAY;

	//Socket 연결 유무 Flag 를 False 로 한다.
	if(NULL != m_pbSocketConnect)
		*m_pbSocketConnect = false;

	//Application Calback Function 을 파라메터 NUL,NULL 로 호출한다. APP에서는 Socket Close 처리가 필요하다.
	//m_fnAppCallBackFunc(NULL, NULL);		
	nErrorCode = m_pAppPeekMessageQueue->m_fnPushMessage((BYTE*)-1);

	return nErrorCode;
}

///*
//*	Module name	:	m_fnSendResponsCommand
//*	Parameter	:	CMDMSG Pointer
//*	Return		:	정상 종료 : 1
//*					이상 종료 : 1을 제외한 나머지 값
//*	Function	:	Visual Station에 Client Message 전달을 하기 위한 기능을 담당한다.
//*					Packet 을 만들어 서버로 직접 전송한다. 응답 회신을 위함.
//*	Create		:	2014.08.29
//*	Author		:	김용태
//*	Version		:	1.0
//*/
//int CMessageProcessor::m_fnSendResponsCommand(CMDMSG* pCmdMsg)
//{
//	int						nRet;
//	int						nErrorCode = OKAY;
//	char*					pcBuffer;
//	PACKET_HEADER*			pPacketHeader;
//	PACKET_CLIENT_MESSAGE*	pPacketClientMessage;
//
//	pCmdMsg->uMsgOrigin = CMD_TYPE_REPLY;
//
//	pcBuffer = (char*)MemoryPool.Alloc();									
//
//	//Buffer 에서 Packet 의 Header 와 Body 의 시작 Pointer 를 구한다.
//	pPacketHeader			= (PACKET_HEADER*)pcBuffer;
//	pPacketClientMessage	= (PACKET_CLIENT_MESSAGE*)(pcBuffer + sizeof(PACKET_HEADER));
//
//	//Message Header 정보를 설정한다.
//	pPacketHeader->uPacketSize		= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE);
//	pPacketHeader->PacketDataType	= CLIENT_MESSAGE_DATA;
//	pPacketHeader->uDataSize		= sizeof(PACKET_CLIENT_MESSAGE);
//
//	//Message Body 정보를 설정한다.
//	pPacketClientMessage->uStationNo1	= 0;								//자신의 Station No.. 0으로 입력 되면 Server Receive 단에서 자동으로 자신의 Station No 로 바꿔준다.
//	pPacketClientMessage->uStationNo2	= pCmdMsg->uStation_Dest;
//	pPacketClientMessage->uTaskNo1		= pCmdMsg->uTask_Dest;
//	pPacketClientMessage->uTaskNo2		= pCmdMsg->uTask_Src;
//	pPacketClientMessage->dwTimeOut		= pCmdMsg->dwTimeOut;
//	pPacketClientMessage->uParamBufSize = (USHORT)(sizeof(CMDMSG) + pCmdMsg->uMsgSize);
//	pPacketClientMessage->uSendOkFlag	= OKAY;
//	memcpy(pPacketClientMessage->cParamBuf, pCmdMsg, sizeof(CMDMSG));		//CMDMSG 자체를 파라메터로쓰는 이유는 CMDMSG 구조체가 패킷구조체와 틀려서 
//																			//body로 쓸수 없기 때문이다. body 자체는 Server 와 Client dll 에서만 참고 되어진다.
//																			//현재 명령을 Receive 하는 Task 에서는 Application 으로 Callback 을 수행 되어 질때
//																			//넘기는 구조체는 pPacketClientMessage->cParamBuf 의 pCmdMsg 이다.
//																			//이후 실제 CMDMSG구조체에서 사용하는 Parameter 가 입력된다.
//	memcpy(pPacketClientMessage->cParamBuf + sizeof(CMDMSG), pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize);	//Application 에서 사용하는 파라메터 최대 크기를 알아야 오류를 방지 할 수 있다..Work Item.. 
//	//uMsgSize 가 MAX_BUFFER-1204 보다 클경우 예외처리 필요 Todo..
//
//	//TRACE("AppSupporter : %s\n",pCmdMsg->cMsgBuf);
//
//	//Send Message
//	nRet = m_fnSendMessage(pcBuffer);
//
//	MemoryPool.Free((PER_PACKET_BUFFER*)pcBuffer);
//
//	nErrorCode = nRet;
//
//	ERROR_RETURN("CMessageProcessor::m_fnSendResponsCommand -> m_pMessageQueue.AddMessage Error\n",nErrorCode);
//
//	return nErrorCode;
//}