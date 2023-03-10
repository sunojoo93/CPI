// VisualStation64Server.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <conio.h>
#include "VisualStationServer.h"
#include "InternalServer.h"
#include <Windows.h>
#include <iostream>

//#include "..\..\CommonHeader\External\Lockey\spromeps.H"	//Lock Key

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 유일한 응용 프로그램 개체입니다.

CWinApp theApp;

using namespace std;
// 트레이 아이콘을 지원하기 위한 함수
// 작동하는 부분의 HWND를 넘겨줘야 한다.

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	HANDLE hMutex=CreateMutex(NULL,TRUE, _T("VST64Server")); 
	if(GetLastError()==ERROR_ALREADY_EXISTS) 
	{ 
		return FALSE; 
	} 

#ifndef _DEBUG
	//HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_HIDE);
#endif

	//Lock Key

	/*RB_SPRO_APIPACKET		ApiPacket;
	SP_STATUS						spStatus;
	RB_BYTE							queryData[SPRO_MAX_QUERY_SIZE];
	RB_BYTE							queryResponse[SPRO_MAX_QUERY_SIZE];
	RB_DWORD						query32Response;
	RB_WORD							Response;

	spStatus	=	RNBOsproFormatPacket(ApiPacket,sizeof(ApiPacket));
	if(SP_SUCCESS		!=	spStatus)
	{
		cout	<<	"Format Fail!!"<<endl<<"Closing Visual Station64!!"<<endl;
		return 0;
	}

	spStatus	=	RNBOsproInitialize(ApiPacket);
	if(SP_SUCCESS		 != spStatus)
	{
		cout	<<	"Initialize Fail!!"<<endl<<"Closing Visual Station64!!"<<endl;
		return 0;
	}

	spStatus	=	RNBOsproSetContactServer(ApiPacket,RNBO_STANDALONE);
	if(SP_SUCCESS		 != spStatus)
	{
		cout	<<	"STANDALONE	Set Error!!"<<endl<<"Closing Visual Station64!!"<<endl;
		return 0;
	}

	spStatus	=	RNBOsproFindFirstUnit(ApiPacket,0xF22F);
	if(SP_SUCCESS		 != spStatus)
	{
		cout	<<	"ID not Match!!"<<endl<<"Closing Visual Station64!!"<<endl;
		return 0;
	}
	else
	{

		spStatus = RNBOsproRead(ApiPacket,SP_VLAUE_POS,&Response);
		if(Response == 0xABCD)
			cout << "Login Success!!"<<endl;
		else
			cout << "Unlock Fail!!"<<endl<<"Closing Visual Station64!!"<<endl;

		*queryData	= 20061231;

		RNBOsproQuery(ApiPacket,SP_QUERY_POS,queryData,queryResponse,&query32Response,SP_QUERY_SIZE);
		if(query32Response == 0xD614BB8E)
            cout << "Unlock Success!!"<<endl<<"Starting Visual Station64!!"<<endl;
		else
		{
			cout << "Unlock Fail!!"<<endl<<"Closing Visual Station64!!"<<endl;
			return 0;
		}
	}*/
	//Lock Key

	//Command Prompt Quick Edit Mode Disable
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dwPreviousMode = 0;
	GetConsoleMode(hConsole, &dwPreviousMode);
	DWORD dwNewMode = dwPreviousMode | ENABLE_MOUSE_INPUT;
	//SetConsoleMode(hConsole, dwNewMode);
	dwNewMode &= ~ENABLE_QUICK_EDIT_MODE;
	SetConsoleMode(hConsole, dwNewMode);

	// MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 오류 코드를 필요에 따라 수정합니다.
		_tprintf(_T("Fatal Error: MFC initialization failed.\n"));
		nRetCode = 1;
	}
	else
	{
		int					nRet;
		BOOL				bEnd = FALSE;
		CInternalServer*	pInternalServer = NULL;
		int					ch;
		
		//Internal Server 생성.. 부가적인 모든 Class 들이 생성 된다.
		pInternalServer = new CInternalServer;
		
		//Internal Server 시작
		nRet = pInternalServer->m_fnStartInternalServer();
		if(OKAY == nRet)
		{
			printf("Internal Server Start \n");
			while(!bEnd)
			{
				// Server 죽이기
				ch = _getch(); 
				ch = toupper( ch ); 
				//만일 입력된 키가 ctrl+C 라면 종료한다.
				if(ch == 3)
					bEnd = TRUE;

				Sleep(100);
			}
		}
		
		SAFE_DELETE(pInternalServer);

		printf("Internal Server End \n");
	}

	//int ch;
	//ch = _getch();
	ReleaseMutex(hMutex); 

	return nRetCode;
}


