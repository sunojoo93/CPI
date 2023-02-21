
// VSPGControllerTask.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.
#include "PGControllerInterface.h"
#include "PGControllerWorkManager.h"
#include <direct.h>
#include <afxstr.h>

// CVSPGControllerTaskApp:
// 이 클래스의 구현에 대해서는 VSPGControllerTask.cpp을 참조하십시오.
//

class CVSPGControllerTaskApp : public CWinApp
{
public:
	CVSPGControllerTaskApp();
	~CVSPGControllerTaskApp();
// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.
	// Get
	CString				GetInitFilePath()			{	return m_strInitFilePath	;};

	BOOL				IsManualMode()				{	return m_bManual			;};
	BOOL				GetIPCState()				{	return m_bIPCConnect		;};
	BOOL				GetSimulationMode()			{	return m_bSimulationMode	;};
	// Set
	void				SetIPCState(BOOL bState)	{	m_bIPCConnect = bState		;};
	void				SetSimulationMode(BOOL bState){	m_bSimulationMode = bState	;};

	// Visual Station Thread
	static UINT			ThreadConnectVSServer(LPVOID pParam);
	BOOL				m_bExec;				// IPC Trhead 실행 Flag

	// 전역으로 접근할 필요가 있는 변수 및 함수는 여기에 선언 후 theApp. 사용
	// Variable
	CPGControllerInterface*	m_pPgController;
	VSMessageProcessor*	m_pInternalServer;		// IPC Driver
 	//CLogWriter*			m_pLogWriter;								// 로그관리자 객체
// 
// 	// Function
 	void				m_fnWriteLog(const TCHAR* pszLog, ...);		// 로그 기록 함수	
	BOOL				SetPgInfo(ST_PG_INFO* stData)						{memcpy(&m_stPgInfo, stData,sizeof(ST_PG_INFO)); return TRUE;};
	ST_PG_INFO			GetPgInfo()											{return m_stPgInfo;};

	BOOL				bIsDust(int nPatternNo)								{if(nPatternNo < 0)	return TRUE;	return FALSE;		  	 };

// 	BOOL				bIsDust(int nPatternNo)							
// 	{
// 		int nGrabCnt = GetCurPgDataIndex(nPatternNo);
// 		if (!_tcsicmp(m_stPgInfo.stPgData[nGrabCnt].strPatternName, _T("DUST"))) return TRUE;		
// 		return FALSE;
// 	};

	ST_PG_DATA			GetCurPatternVoltageData(int nPatternNo/*TCHAR* strCurStepName*/)		;
																				
	int GetCurPgDataIndex(int nPatternNo)							
	{
		for (int nCnt = 0 ; nCnt < MAX_GRAB_STEP_COUNT + 1; nCnt++)
		{
			if(m_stPgInfo.stPgData[nCnt].nPtnNum == nPatternNo)		
				return nCnt;
		}		
		return FALSE;
	};


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

	DECLARE_MESSAGE_MAP()

private:
	CString				m_strInitFilePath;
	CWinThread*			m_pVSThread;

	BOOL				m_bManual;			// Auto - Manual 여부
	BOOL				m_bIPCConnect;
	HANDLE				m_hEventThreadAlive;
	BOOL				m_bSimulationMode;
	ST_PG_INFO			m_stPgInfo;
};

extern CVSPGControllerTaskApp theApp;