
// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.4
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.


#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.
#include <afxsock.h>            // MFC ���� Ȯ��


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT


#include ".\define\ColorDefine.h"
#include ".\define\IndexDefine.h"


#include <ppl.h>

using namespace Concurrency;

//GDI+ ���
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
extern void G_ADDLOG_TEMPERATURE( const wchar_t* chLog, ...); //2016.04.05 add by ikm - �µ��� ���� �α�
extern void G_AddLog_STG_SEQ(int nLogLevel, int nStageID, const wchar_t* chLog, ...);
extern void G_AddLog_ALARM( const wchar_t* chLog, ...); //2016.08.17 �˶� �α�
extern void G_AddLog_RESET( const wchar_t* chLog, ...); //2018.03.31 ���� �α�
extern void G_AddLog_QMS( const wchar_t* chLog, ...);	//2017.06.27 ����Panda QMS ���� Log
extern void G_AddLog_IMG_QMS( const wchar_t* chLog, ...);	//2017.06.27 ����Panda QMS ���� Log


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


