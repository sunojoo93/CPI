
// Crux_MainPcInterface.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.
#include <direct.h>


// CCrux_MainPcInterfaceApp:
// 이 클래스의 구현에 대해서는 Crux_MainPcInterface.cpp을 참조하십시오.
//

class CCrux_MainPcInterfaceApp : public CWinApp
{
public:
	CCrux_MainPcInterfaceApp();

// 재정의입니다.
public:


	CString GETDRV()
	{

		// 		TCHAR currentDir[MAX_PATH];
		// 		GetCurrentDirectory(MAX_PATH, currentDir);

		TCHAR buff[MAX_PATH];
		memset(buff, 0, MAX_PATH);
		::GetModuleFileName(NULL, buff, sizeof(buff));
		CString strFolder = buff;
		CString strRet = strFolder.Left(1);
		strRet.MakeUpper();		//180919 YSS	// 181001 yskim Lower -> Upper로 변경
		return strRet;

		// 		int nDrive = 0;	nDrive = _getdrive();
		// 		CString str;
		// 		str.Format(_T("%c"), char(nDrive) + 96);
		// 		return strRet;
	};

	virtual BOOL InitInstance();

	int GetPCNum()								{	return m_nPCNum;	};
	int GetInspType()							{	return m_nInspType;	};
	BOOL GetSimulationProgramMode()				{	return m_bSimulationProgramMode; }
	BOOL GetSimulationMode()					{	return m_bSimulationMode; }

	// YHS 18.03.12 - Merge Tool get 함수 추가
	BOOL GetMergeToolUse() {
		m_nInspStep = GetPrivateProfileInt(_T("MergeTool"), _T("USE_MERGE_TOOL"), 1, GETDRV() + _T(":\\Crux\\DATA\\INI\\Initialize.ini")) ? true : false; return m_nInspStep;
	};
	// yskim 19.12.03 - 그랩 제한 세마포어 개수
	int GetGrabLimitCount() {
		return m_nGrabLimitCount;
	};
	// yskim 19.12.03 - 그랩 제한 세마포어 대기 시간 (ms)
	int GetGrabLimitWaitTime() {
		return m_nGrabLimitWaitTime;
	};

private:
	int m_nPCNum;
	int m_nInspType;						//검사기 Type, AVI:0, SVi:1, APP:2
	BOOL m_bSimulationProgramMode;			//Program 이 Simultor로 구동 되어야 하는지 Flag , ARGV 로 실행 모드를 결정한다.
	BOOL m_bSimulationMode;					//Crux 가 Simulation Mode 로 구동되어야 하는지 Flag,

// 구현입니다.

	// YHS 18.03.12 - Merge Tool 변수 추가
	BOOL m_nInspStep;

	int m_nGrabLimitCount;		// yskim 19.12.03 - 그랩 제한 세마포어 개수
	int m_nGrabLimitWaitTime;	// yskim 19.12.03 - 그랩 제한 세마포어 대기 시간 (ms)

	DECLARE_MESSAGE_MAP()
};

extern CCrux_MainPcInterfaceApp theApp;