
// VSCameraTask.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.
#include "SharedMemWrapper.h"
#include "CameraInterface.h"
#include "CameraWorkManager.h"
#include "TriggerControl.h"
#include <direct.h>
#include <sysinfoapi.h>

// CVSCameraTaskApp:
// 이 클래스의 구현에 대해서는 VSCameraTask.cpp을 참조하십시오.
//

class CVSCameraTaskApp : public CWinApp
{
public:
	CVSCameraTaskApp();
	~CVSCameraTaskApp();
// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.
	// Get
	CString				GetInitFilePath()			{	return m_strInitFilePath	;};

	BOOL				IsManualMode()				{	return m_bManual			;};
	BOOL				GetIPCState()				{	return m_bIPCConnect		;};
	BOOL				GetSMemState()				{	return m_bSMemState			;};
	BOOL				GetCameraState()			{	return m_bCamState			;};
	BOOL				GetSimulationMode()			{	return m_bSimulationMode	;};
	int					GetPcNum()					{	return m_nPcNum				;};
	int					GetTaskNum()				{	return m_nTaskNo			;};
	CString				GetPcName()					{	return m_strPcName; };
	// Set
	void				SetCameraState(BOOL bState)	{	m_bCamState = bState		;};
	void				SetIPCState(BOOL bState)	{	m_bIPCConnect = bState		;};
	void				SetSMemState(BOOL bState)	{	m_bSMemState = bState		;};
	void				SetSimulationMode(BOOL bState){	m_bSimulationMode = bState	;};
	// Visual Station Thread
	static UINT			ThreadConnectVSServer(LPVOID pParam);
	BOOL				m_bExec;				// IPC Trhead 실행 Flag

	// 전역으로 접근할 필요가 있는 변수 및 함수는 여기에 선언 후 theApp. 사용
	// Variable
	CSharedMemWrapper*	m_pSharedMemory;		// 공유메모리
	CameraInterface*	m_pCamera;				// 카메라
	VSMessageProcessor*	m_pInternalServer;		// IPC Driver
	CLogWriter*			m_pLogWriter;								// 로그관리자 객체	
	CLogWriter*			m_pLogLineScan;								// 라인스캔용
	CLogWriter*			m_pLogMonitoring;							// 모니터링 로그 추가
// 
// 	// Function
	void				m_fnWriteTactLog(const TCHAR* pszLog, ...);				// 로그 기록 함수
	void				m_fnWriteLineScanLog(const TCHAR* pszLog, ...);				// 로그 기록 함수
	void				m_fnWriteMonitoringLog(const TCHAR* pszLog, ...);		// 로그 기록 함수

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


	DECLARE_MESSAGE_MAP()

private:
	CString				m_strInitFilePath;
	CWinThread*			m_pVSThread;
	
	BOOL				m_bManual;			// TRUEAuto - Manual 여부
	BOOL				m_bIPCConnect;
	BOOL				m_bSMemState;
	BOOL				m_bCamState;

	HANDLE				m_hEventThreadAlive;
	BOOL				m_bSimulationMode;
	int					m_nPcNum;
	int					m_nTaskNo;
	CString				m_strPcName;

	

};

extern CVSCameraTaskApp theApp;