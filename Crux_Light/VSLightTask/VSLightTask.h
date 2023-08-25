
// VSLightTask.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.
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
// 이 클래스의 구현에 대해서는 VSLightTask.cpp을 참조하십시오.
#define	WM_TRAYICON_MSG					WM_USER + 1
#define WM_PRINT_UI_LOG_MSG_UNICODE		WM_USER + 2
#define	WM_PRINT_UI_LOG_MSG_MULTI_BYTE	WM_USER + 3

class CVSLightTaskApp : public CWinApp
{
public:
	CVSLightTaskApp();
	~CVSLightTaskApp();
// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.
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
	int					GetInspType()				{	return m_nInspType			;};// 2018.12.18 주순오(sojoo)
	// Set
	void				SetLightState(BOOL bState)	{	m_bLightState = bState		;};
	void				SetIPCState(BOOL bState)	{	m_bIPCConnect = bState		;};
	void				SetSMemState(BOOL bState)	{	m_bSMemState = bState		;};
	void				SetPCNum(int nPCNum)		{	m_nPCNum = nPCNum			;};
	void				SetVsTaskNum(int nVsTaskNum){	m_nVsTaskNum = nVsTaskNum	;};
	void				SetTaskNum(int nTaskNum)	{	m_nTaskNum = nTaskNum		;};
	void			SetSiteTypeNum(int nSiteTypeNum){	m_nSiteType = nSiteTypeNum	;};
	void				SetInspType(int nInspTypeNum){	m_nInspType = nInspTypeNum	;};// 2018.12.18 주순오(sojoo)

	// Visual Station Thread
	static UINT			ThreadConnectVSServer(LPVOID pParam);
	BOOL				m_bExec;				// IPC Trhead 실행 Flag

	// 전역으로 접근할 필요가 있는 변수 및 함수는 여기에 선언 후 theApp. 사용
	// Variable
	LightInterface*	m_pLight;				// 카메라
	VSMessageProcessor*	m_pInternalServer;		// IPC Driver
 	CLogWriter*			m_pLogWriter;								// 로그관리자 객체
	UINT32				OccuredAlaramCode = 0;
// 
// 	// Function
 	void				m_fnWriteLog(const TCHAR* pszLog, ...);		// 로그 기록 함수
	void				WriteLog(const TCHAR* pszLog, ...);			// Alg 테스크 Log 방식 변경 180417 YSS


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
	CVSLightTaskDlg*	m_pDlg;
	CString				m_strInitFilePath;
	CWinThread*			m_pVSThread;

	BOOL				m_bManual;			// Auto - Manual 여부
	BOOL				m_bIPCConnect;
	BOOL				m_bSMemState;
	BOOL				m_bLightState;

	HANDLE				m_hEventThreadAlive;

	UINT				m_nLightNo;
	int					m_nPCNum;
	int					m_nVsTaskNum;
	int					m_nTaskNum;	
	int					m_nSiteType;	// 2018.12.01 김양수(yskim)
	int					m_nInspType;	// 2018.12.18 주순오(sojoo)
};

extern CVSLightTaskApp theApp;