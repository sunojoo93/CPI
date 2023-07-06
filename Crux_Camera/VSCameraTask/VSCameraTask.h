
// VSCameraTask.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.
#include "SharedMemWrapper.h"
#include "CameraInterface.h"
#include "CameraWorkManager.h"
#include "TriggerControl.h"
#include <direct.h>
#include <sysinfoapi.h>

// CVSCameraTaskApp:
// �� Ŭ������ ������ ���ؼ��� VSCameraTask.cpp�� �����Ͻʽÿ�.
//

class CVSCameraTaskApp : public CWinApp
{
public:
	CVSCameraTaskApp();
	~CVSCameraTaskApp();
// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
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
	BOOL				m_bExec;				// IPC Trhead ���� Flag

	// �������� ������ �ʿ䰡 �ִ� ���� �� �Լ��� ���⿡ ���� �� theApp. ���
	// Variable
	CSharedMemWrapper*	m_pSharedMemory;		// �����޸�
	CameraInterface*	m_pCamera;				// ī�޶�
	VSMessageProcessor*	m_pInternalServer;		// IPC Driver
	CLogWriter*			m_pLogWriter;								// �αװ����� ��ü	
	CLogWriter*			m_pLogLineScan;								// ���ν�ĵ��
	CLogWriter*			m_pLogMonitoring;							// ����͸� �α� �߰�
// 
// 	// Function
	void				m_fnWriteTactLog(const TCHAR* pszLog, ...);				// �α� ��� �Լ�
	void				m_fnWriteLineScanLog(const TCHAR* pszLog, ...);				// �α� ��� �Լ�
	void				m_fnWriteMonitoringLog(const TCHAR* pszLog, ...);		// �α� ��� �Լ�

	CString GETDRV()
	{
		TCHAR buff[MAX_PATH];
		memset(buff, 0, MAX_PATH);
		::GetModuleFileName(NULL, buff, sizeof(buff));
		CString strFolder = buff;
		CString strRet = strFolder.Left(1);
		strRet.MakeUpper();		//180919 YSS	// 181001 yskim Lower -> Upper�� ����
		return strRet;
	};


	DECLARE_MESSAGE_MAP()

private:
	CString				m_strInitFilePath;
	CWinThread*			m_pVSThread;
	
	BOOL				m_bManual;			// TRUEAuto - Manual ����
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