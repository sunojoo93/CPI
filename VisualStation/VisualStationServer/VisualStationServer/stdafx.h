// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include <afx.h>
#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <WinSock2.h>										//Winsock 2 정의 파일
#include <process.h>										//Thread 사용을 위함
#include <MMSystem.h>										//Multi Media Timer 사용을 위함
#include "..\..\CommonHeader\Constant\CommonConstant.h"				//공용으로 사용하는 상수를 정의함
#include "ServerConstant.h"									//Server에서 사용하는 상수를 정의함
#include "..\..\CommonHeader\Structure\CommonStructure.h"				//공용 패킷 구조체
#include "ServerStructure.h"								//Server에서 사용하는 구조체
#include "..\..\CommonHeader\ErrorCode\InternalServerErrorCode.h"		//Internal Server Error Code 정의

#pragma warning( disable : 4996 )	//컴파일 경고 메시지를 무시하게 한다. <~이(가) deprecated로 선언되었습니다.>메시지를 무시한다. .net2003 .net2005 환경 이후에는 지원되지 않을 가능성이 있다.
