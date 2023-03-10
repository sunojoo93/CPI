#pragma once

//Task를 Visual Station Internal Server 에 등록 한다.
extern "C++" __declspec(dllexport) int g_fnRegisterTask(INT64& AppSupporter, USHORT uTaskNo, CMessageQueue<BYTE*>* pMessageQueue, 
														char* pszServerIp = "127.0.0.1", WORD wPortNo = 5000);

//Task를 Visual Station Internal Server 에 등록 한다. 유니코드 지원
extern "C++" __declspec(dllexport) int g_fnRegisterTask(INT64& AppSupporter, USHORT uTaskNo, CMessageQueue<BYTE*>* pMessageQueue, 
	wchar_t* pszServerIp = L"127.0.0.1", WORD wPortNo = 5000);


//Visual Station에서 등록 해지 한다.
extern "C++" __declspec(dllexport) int g_fnUnRegisterTask(INT64& AppSupporter);

//타 Cleint 로 메시지를 전송한다.
extern "C++" __declspec(dllexport) int g_fnCommandSend(INT64& AppSupporter, CMDMSG* pCmdMsg);

//타 Cleint 로 응답 메시지를 전송한다.
extern "C++" __declspec(dllexport) int g_fnSendResponseCommand(INT64& AppSupporter, CMDMSG* pCmdMsg);

extern "C++" __declspec(dllexport) int g_fnFreeMemory(INT64& AppSupporter, BYTE* rcvPacket);

extern "C++" __declspec(dllexport) int g_fnDeleteAppSupporter(INT64& AppSupporter);