
// AFTS_SimplifiedExample_MFC.h: PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// 주 기호입니다.
#include "AFWorkManager.h"
#include "Library/AFTSClass.h"
#include "define.h"
#include "CPUTimer.h"

// CAFTSSimplifiedExampleMFCApp:
// 이 클래스의 구현에 대해서는 AFTS_SimplifiedExample_MFC.cpp을(를) 참조하세요.
//

class CAFTSSimplifiedExampleMFCApp : public CWinApp
{
public:
	CAFTSSimplifiedExampleMFCApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

	// Visual Station Thread
	static UINT			ThreadConnectVSServer(LPVOID pParam);
	BOOL				 m_bExec;				// IPC Trhead 실행 Flag
	VSMessageProcessor*	 m_pInternalServer;		// IPC Driver

	int					GetTaskNum() { return m_nTaskNo; };
	void				SetIPCState(BOOL bState) { m_bIPCConnect = bState; };

	int					m_Module_IPadress[4];
	
	CLogWriter*			m_pLogWriter;
	CString				m_strLog_AF_Position;
	CString				m_strLog_AF_Stage;

	CCPUTimer			m_AFTactTime;

	CString GETDRV()
	{
		TCHAR buff[MAX_PATH];
		memset(buff, 0, MAX_PATH);
		::GetModuleFileName(NULL, buff, sizeof(buff));
		CString strFolder = buff;
		CString strRet = strFolder.Left(1);
		strRet.MakeUpper();		//180919 YSS	// 181001 yskim Lower -> Upper로 변경
		return strRet;
	};
	///
	
	///

	DECLARE_MESSAGE_MAP()

private:
	CWinThread*			m_pVSThread;

	CString				m_strInitFilePath;


	int					m_nTaskNo;

	BOOL				m_bIPCConnect;
	HANDLE				m_hEventThreadAlive;

	
};

extern CAFTSSimplifiedExampleMFCApp theApp;
