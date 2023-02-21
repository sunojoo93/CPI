
// VSPGControllerTask.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.
#include "PGControllerInterface.h"
#include "PGControllerWorkManager.h"
#include <direct.h>
#include <afxstr.h>

// CVSPGControllerTaskApp:
// �� Ŭ������ ������ ���ؼ��� VSPGControllerTask.cpp�� �����Ͻʽÿ�.
//

class CVSPGControllerTaskApp : public CWinApp
{
public:
	CVSPGControllerTaskApp();
	~CVSPGControllerTaskApp();
// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
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
	BOOL				m_bExec;				// IPC Trhead ���� Flag

	// �������� ������ �ʿ䰡 �ִ� ���� �� �Լ��� ���⿡ ���� �� theApp. ���
	// Variable
	CPGControllerInterface*	m_pPgController;
	VSMessageProcessor*	m_pInternalServer;		// IPC Driver
 	//CLogWriter*			m_pLogWriter;								// �αװ����� ��ü
// 
// 	// Function
 	void				m_fnWriteLog(const TCHAR* pszLog, ...);		// �α� ��� �Լ�	
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
		strRet.MakeUpper();		//180919 YSS	// 181001 yskim Lower -> Upper�� ����
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

	BOOL				m_bManual;			// Auto - Manual ����
	BOOL				m_bIPCConnect;
	HANDLE				m_hEventThreadAlive;
	BOOL				m_bSimulationMode;
	ST_PG_INFO			m_stPgInfo;
};

extern CVSPGControllerTaskApp theApp;