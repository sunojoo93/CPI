
// VSLightTask.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.
#include "LightInterface.h"
#include "LightWorkManager.h"

#ifdef _NEEPSLIGHT
#include "NeepsLight/NeepsLightControl.h"
#include "NeepsLightTaskDlg.h"
#elif _PLUSTEK
#include "Plustek/PlustekLightControl.h"
#include "PlustekLightTaskDlg.h"
#elif _SUNINLIGHT
#include "SuninLight/SuninLightControl.h"
#include "SuninLightTaskDlg.h"
#elif _EROOMLIGHT
#include "EroomLight/EroomLightControl.h"
#include "EroomLightTaskDlg.h"
#elif _IPULSLIGHT
#include "IPulsLight/IPulsLightControl.h"
#include "IPulsLight/IPulsLightTaskDlg.h"
#elif _LFINELIGHT
#include "LFineLight/LFineLightControl.h"
#include "LFineLightTaskDlg.h"
#elif _LLIGHTLIGHT
#include "LLightLight/LLightLightControl.h"
#include "LLightLight\LLightLightTaskDlg.h"
#elif _TRIVISIONLIGHT
#include "TrivisionLight/TrivisionLightControl.h"
#include "TrivisionLightTaskDlg.h"
#else
#include "VSLightTaskDlg.h"
#endif
#include <direct.h>
#include <afxstr.h>

// CVSLightTaskApp:
// �� Ŭ������ ������ ���ؼ��� VSLightTask.cpp�� �����Ͻʽÿ�.
#define	WM_TRAYICON_MSG					WM_USER + 1
#define WM_PRINT_UI_LOG_MSG_UNICODE		WM_USER + 2
#define	WM_PRINT_UI_LOG_MSG_MULTI_BYTE	WM_USER + 3

class CVSLightTaskApp : public CWinApp
{
public:
	CVSLightTaskApp();
	~CVSLightTaskApp();
// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	// Get
	CString				GetInitFilePath()			{	return m_strInitFilePath	;};

	BOOL				IsManualMode()				{	return m_bManual			;};
	BOOL				GetIPCState()				{	return m_bIPCConnect		;};
	BOOL				GetSMemState()				{	return m_bSMemState			;};
	BOOL				GetLightState()				{	return m_bLightState		;};
	int					GetPCNum()					{	return m_nPCNum				;};
	int					GetVsTaskNum()				{	return m_nVsTaskNum			;};
	int					GetTaskNum()				{	return m_nTaskNum			;};
	int					GetSiteTypeNum()			{	return m_nSiteType			;};
	int					GetInspType()				{	return m_nInspType			;};// 2018.12.18 �ּ���(sojoo)
	// Set
	void				SetLightState(BOOL bState)	{	m_bLightState = bState		;};
	void				SetIPCState(BOOL bState)	{	m_bIPCConnect = bState		;};
	void				SetSMemState(BOOL bState)	{	m_bSMemState = bState		;};
	void				SetPCNum(int nPCNum)		{	m_nPCNum = nPCNum			;};
	void				SetVsTaskNum(int nVsTaskNum){	m_nVsTaskNum = nVsTaskNum	;};
	void				SetTaskNum(int nTaskNum)	{	m_nTaskNum = nTaskNum		;};
	void			SetSiteTypeNum(int nSiteTypeNum){	m_nSiteType = nSiteTypeNum	;};
	void				SetInspType(int nInspTypeNum){	m_nInspType = nInspTypeNum	;};// 2018.12.18 �ּ���(sojoo)

	// Visual Station Thread
	static UINT			ThreadConnectVSServer(LPVOID pParam);
	BOOL				m_bExec;				// IPC Trhead ���� Flag

	// �������� ������ �ʿ䰡 �ִ� ���� �� �Լ��� ���⿡ ���� �� theApp. ���
	// Variable
	LightInterface*	m_pLight;				// ī�޶�
	VSMessageProcessor*	m_pInternalServer;		// IPC Driver
 	CLogWriter*			m_pLogWriter;								// �αװ����� ��ü
	UINT32				OccuredAlaramCode = 0;
// 
// 	// Function
 	void				m_fnWriteLog(const TCHAR* pszLog, ...);		// �α� ��� �Լ�
	void				WriteLog(const TCHAR* pszLog, ...);			// Alg �׽�ũ Log ��� ���� 180417 YSS


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
	CVSLightTaskDlg*	m_pDlg;
	CString				m_strInitFilePath;
	CWinThread*			m_pVSThread;

	BOOL				m_bManual;			// Auto - Manual ����
	BOOL				m_bIPCConnect;
	BOOL				m_bSMemState;
	BOOL				m_bLightState;

	HANDLE				m_hEventThreadAlive;

	UINT				m_nLightNo;
	int					m_nPCNum;
	int					m_nVsTaskNum;
	int					m_nTaskNum;	
	int					m_nSiteType;	// 2018.12.01 ����(yskim)
	int					m_nInspType;	// 2018.12.18 �ּ���(sojoo)
};

extern CVSLightTaskApp theApp;