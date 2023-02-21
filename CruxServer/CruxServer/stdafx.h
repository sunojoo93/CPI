
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.4
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.
#include <afxsock.h>            // MFC 소켓 확장


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT


#include ".\define\ColorDefine.h"
#include ".\define\IndexDefine.h"


#include <ppl.h>

using namespace Concurrency;

//GDI+ 사용
#include <gdiplus.h>
#include <afxcontrolbars.h>
#pragma comment(lib, "gdiplus")
using namespace Gdiplus;

#define  PLC_TASK_MODE			0 //2015.11.27 add by IKM for PLC TASK MODE
#define  THREAD_MODE			1 //2015.11.27 add by IKM for PLC TASK MODE
#define	 AVL_MODE				0	
#define	 ABI_MODE				1
#define	 LARGE_SIZE_MODE        1
#define	 MONITOR_MODE		    0
extern void G_AddLog(int nLogLevel,int nLogPriority, const wchar_t* chLog, ...);
extern void G_AddLog_SEQ(int nStageID, const wchar_t* chLog, ...);
extern void G_ADDLOG_TEMPERATURE( const wchar_t* chLog, ...); //2016.04.05 add by ikm - 온도값 전용 로그
extern void G_AddLog_STG_SEQ(int nLogLevel, int nStageID, const wchar_t* chLog, ...);
extern void G_AddLog_ALARM( const wchar_t* chLog, ...); //2016.08.17 알람 로그
extern void G_AddLog_RESET( const wchar_t* chLog, ...); //2018.03.31 리셋 로그
extern void G_AddLog_QMS( const wchar_t* chLog, ...);	//2017.06.27 성도Panda QMS 고객사 Log
extern void G_AddLog_IMG_QMS( const wchar_t* chLog, ...);	//2017.06.27 성도Panda QMS 고객사 Log


#if LARGE_SIZE_MODE		
	#include "DefineVal_BIG.h"	
#else
	#if MONITOR_MODE
		#include "DefineVal_M.h"
	#else
		#include "DefineVal.h"
	#endif
#endif
#include "DefineCommon.h"
#include "Structure.h"
#include <io.h>
#pragma comment(lib,"..\\..\\..\\..\\VisualStation\\EXE\\ClientSockDll.lib")
#include "..\..\..\..\..\VisualStation\CommonHeader\Class\Interserverinterface.h"

//#define DEBUG_TEST

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


