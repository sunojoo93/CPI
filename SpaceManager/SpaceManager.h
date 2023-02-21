
// SpaceManager.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.
#include "LogWriter.h"		// 로그 추가

#define SM_LOG_PATH		_T("D:\\CRUX\\DATA\\LOG\\")

// CSpaceManagerApp:
// 이 클래스의 구현에 대해서는 SpaceManager.cpp을 참조하십시오.
//

class CSpaceManagerApp : public CWinApp
{
public:
	CSpaceManagerApp();

	CLogWriter*				m_pLogWriter;								// 로그관리자 객체
	void					m_fnWriteLog(const TCHAR* pszLog, ...);		// 로그 기록 함수

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()
};

extern CSpaceManagerApp theApp;