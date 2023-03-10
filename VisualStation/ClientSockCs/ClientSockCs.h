// 다음 ifdef 블록은 DLL에서 내보내기하는 작업을 쉽게 해 주는 매크로를 만드는 
// 표준 방식입니다. 이 DLL에 들어 있는 파일은 모두 명령줄에 정의된 _EXPORTS 기호로
// 컴파일되며, 다른 프로젝트에서는 이 기호를 정의할 수 없습니다.
// 이렇게 하면 소스 파일에 이 파일이 들어 있는 다른 모든 프로젝트에서는 
// CLIENTSOCKVB_API 함수를 DLL에서 가져오는 것으로 보고, 이 DLL은
// 이 DLL은 해당 매크로로 정의된 기호가 내보내지는 것으로 봅니다.
#ifdef CLIENTSOCKVB_EXPORTS
#define CLIENTSOCKCS_API __declspec(dllexport)
#else
#define CLIENTSOCKCS_API __declspec(dllimport)
#endif

#define	SEQUENCE_TABLE_CS(FUNCTION_NO,SEQUENCE_NO,FUNCTION_NAME)									\
	if ( pCmdMsg->uFunID_Dest == FUNCTION_NO && pCmdMsg->uSeqID_Dest == SEQUENCE_NO )				\
	{																								\
	isFunctionRuned = true;																			\
	nRet = FUNCTION_NAME((BYTE*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize);								\
	}

// 이 클래스는 ClientSockVB.dll에서 내보낸 것입니다.
class CLIENTSOCKCS_API CClientSockCs {
public:
	CClientSockCs(void);
	// TODO: 여기에 메서드를 추가합니다.
};

extern CLIENTSOCKCS_API int nClientSockVB;

CLIENTSOCKCS_API int fnClientSockCs(void);


CLIENTSOCKCS_API int _stdcall InitClientSockDLL(
	TCHAR* szLogFilePath,			///< 로그 파일 경로
	TCHAR* szLogFileName,			///< 로그 파일 이름
	HWND hWnd ,					///< 메시지를 받을 윈도우 Handle
	UINT nUserMessageNo ,		///< 메시지 전달에 사용될 사용자 메시지 번호
	bool bAutoExitFlag ,		///< 서버가 종료 될때 Application 을 자동 종료 시킬지를 가지는 Flag, Windows handle 이 등록된경우는 자동종료안된다.
	int uPeriodDay ,			///< 로그 보존 기한일
	int uWriteTerm ,				///< 로그 파일을 몇시간 주기로 생성하는가..시간, 0 = 하루치의 로그가 파일 하나로 생성됨.
	bool bLogWriteEnable ,	///< 로그를 실제로 기입하는가 플레그
	USHORT uLogBlockLevel 
	);

CLIENTSOCKCS_API int _stdcall FreeMemory(
	void* pMemory					///< 자원을 회수할 메모리 주소
	);

CLIENTSOCKCS_API BYTE* _stdcall PeekMessageBlocking();

CLIENTSOCKCS_API BYTE* _stdcall PeekMessageForNonBlock();


CLIENTSOCKCS_API int _stdcall RegisterTask(
	USHORT uTaskNo,							///< 서버로 등록시 사용할 Task 번호
	TCHAR* pszServerIp,						///< 등록 할 서버의 IP 주소
	WORD wPortNo							///< 등록 할 서버의 Port No
	);

CLIENTSOCKCS_API int _stdcall ReRegisterTask();

CLIENTSOCKCS_API int _stdcall UnRegisterTask();

CLIENTSOCKCS_API int _stdcall SendCommand1(
	BYTE* sCmdMsg						///< 전송할 정보를 담은 CMDMSG 구조체 포인터
	);

CLIENTSOCKCS_API int _stdcall SendCommand2(
	USHORT uTask_Dest,					///< 목적지 Task 번호
	USHORT uFunID,						///< 호출 할 목적지의 Funtion ID
	USHORT uSeqID,						///< 호출 할 목적지의 Sequence ID
	USHORT uUnitID						///< 호출 할 목적지의 Unit ID
	);

CLIENTSOCKCS_API int _stdcall SendCommand3(
	USHORT uTask_Dest,						///< 목적지 Task 번호
	USHORT uFunID_Dest,						///< 호출 할 목적지의 Funtion ID
	USHORT uSeqID_Dest,						///< 호출 할 목적지의 Sequence ID
	USHORT uUnitID_Dest,					///< 호출 할 목적지의 Unit ID
	ULONG uMsgSize,							///< 전달할 메시지의 Size
	BYTE *cMsgBuf,							///< 전달할 메시지
	USHORT uFunID_Src ,						///< 근원지의 Function ID
	USHORT uSeqID_Src ,						///< 근원지의 Sequence ID
	USHORT uUnitID_Src ,					///< 근원지의 Unit ID
	USHORT uMsgType ,						///< 메시지가 Response 가 필요한지 여부, CMD_TYPE_NORES/CMD_TYPE_RES
	USHORT uMsgOrigin, 						///< 메시지가 처리에 대한 응답인지 여부, CMD_TYPE_REPLY/CMD_TYPE_CMD
	USHORT uMsgCompressMode,				///< 파라메터 버퍼를 압축할지 여부, 0:압축안함, 1:압축, 2:10MB 이상 자동 압축
	ULONG  uSendMsgSize						///< uMsgSize 만큼 전송하지 않고 uSendMsgSize 만큼만 전송하면 되는 경우 사용한다. 회신에 데이터를 받는 경우 주로 사용
	);

CLIENTSOCKCS_API int _stdcall SendCommand(
	USHORT uTask_Dest,						///< 목적지 Task 번호
	USHORT uFunID_Dest,						///< 호출 할 목적지의 Funtion ID
	USHORT uSeqID_Dest,						///< 호출 할 목적지의 Sequence ID
	USHORT uUnitID_Dest,					///< 호출 할 목적지의 Unit ID
	USHORT uMsgType,
	UINT uiTimeOut,
	ULONG uMsgSize,							///< 전달할 메시지의 Size
	BYTE *cMsgBuf,							///< 전달할 메시지
	USHORT uMsgCompressMode,				///< 파라메터 버퍼를 압축할지 여부, 0:압축안함, 1:압축, 2:10MB 이상 자동 압축
	ULONG  uSendMsgSize 					///< uMsgSize 만큼 전송하지 않고 uSendMsgSize 만큼만 전송하면 되는 경우 사용한다. 회신에 데이터를 받는 경우 주로 사용
	);

CLIENTSOCKCS_API int _stdcall SendResponseCommand(
	BYTE* sCmdMsg						///< 전송할 정보를 담은 CMDMSG 구조체 포인터
	);

CLIENTSOCKCS_API int _stdcall PrintLog(
	USHORT LogLevel,				///< 기록 할 Log 의 Log Level, 1차로 함수 내에서  Block을 결정하고, 2차로 Log Task 에서 Block 여부가 결정된다.
	int	nLogTextSize,				///< 로그를 기입할 내용의 사이즈
	const wchar_t* pszLogData		///< 실제 기록 할 Log 내용
	);

CLIENTSOCKCS_API int _stdcall GetVSServerConnected();
CLIENTSOCKCS_API int _stdcall test();