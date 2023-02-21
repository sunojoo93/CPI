// Config.h: interface for the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_CONFIG_H__89A008AB_6C8F_4C25_9509_8F34D97F8EEE__INCLUDED_
#define	AFX_CONFIG_H__89A008AB_6C8F_4C25_9509_8F34D97F8EEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\CommonHeaders\Structure.h"
#include "InspResultInfo.h"

// 문자열 구분에만 사용 분기문 사용하지 말 것
enum EQP_TYPE
{
	EQP_AVI = 0,
	EQP_SVI,
	EQP_APP,
	EQP_AMT,
	EQP_LCP,
	MAX_EQP_TYPE
};

const CString strEqpType[MAX_EQP_TYPE] = {_T("AVI"), _T("SVI"), _T("APP")};

class CConfig  
{
public:
	CConfig();
	virtual ~CConfig();

	bool			Load();
// 파일 형식 사용 안함 - IPC Message 활용
// 	bool			LoadModel(CString strModel = _T(""));
	int				GetPCNum()												{	return m_nPCNum														;};
	// 17.06.23 Manual 동작일 경우 결과 저장 경로를 D:\ 로 고정 -> 설정값 따르도록 수정
// 	CString			GetOriginPath()											{	return m_strNetworkDrive + GetCurrentDrive() + m_strOriginPath 		;}; // 2018.01.18 sgkim Define으로 변경 ORIGIN_PATH
// 	CString			GetResultPath()											{	return m_strNetworkDrive + GetCurrentDrive() + m_strResultPath		;}; // 2018.01.18 sgkim Define으로 변경 RESULT_PATH
	CString			GetOriginPath()											{	return m_strOriginPath 												;};
	CString			GetResultPath()											{	return m_strResultPath												;};

	CString			GetLogFilePath()										{	return m_strLogFilePath												;};
//	CString			GetInspPath()											{	return m_strNetworkDrive + GetCurrentDrive() + m_strInspPath		;}; // 2018.01.18 sgkim Define으로 변경 INSP_PATH
	CString			GetInspPath()											{	return m_strInspPath												;};
	CString			GetEqpTypeName()										{	return strEqpType[m_nEqpType]										;};
	CString			GetEqpName()										    {	return m_strEQPName													;};
	EQP_TYPE		GetEqpType()											{	return (EQP_TYPE)m_nEqpType											;};
	int				GetLogLevel()											{	return m_nLogLevel													;};	
	int				GetUseCamCount()										{	return m_nCamCount													;};
	CString			GetNetworkDrivePath()									{	return m_strNetworkDrive											;};
	BOOL			GetSimualtionMode()										{	return m_nSimualtionMode											;};
	CString			GetUseDrive()											{	return m_strUseDrive												;};		
	CString			GetSimulationDrive()									{	return m_strSimulationDrive											;};
	CString			GetCurrentDrive()										{	return m_strCurrentDrive											;}; 
	CString			GetINIDrive()											{	return m_strINIDrive												;}; // ini에 설정되어있는 드라이브 Last Used Drive
	int				GetDriveLimitSize()										{	return m_nDriveLimitSize											;};
	// CCD[0] / 공정[1] 불량 발생 판단 기준
	BOOL*			GetUseCCDAlarm()										{	return m_bUseRepeatAlarm											;};
	int*			GetCCDOffset()											{	return m_nRepeatCompareOffset										;};
	int*			GetCCDLightAlarmCount()									{	return m_nRepeatLightAlarmCount										;};
	int*			GetCCDHeavyAlarmCount()									{	return m_nRepeatHeavyAlarmCount										;};

	// Set Func
	void			SetCurrentDrive(CString strDrive)						{	m_strCurrentDrive = strDrive										;};
	void			SetINIDrive(CString strDrive)							{	m_strINIDrive = strDrive											;}; // ini에 설정되어있는 드라이브 Last Used Drive
	void			SetDriveLimitSize(int nSize)							{	m_nDriveLimitSize = nSize											;};
	
	
	void			Write(TCHAR* sec, TCHAR* key, UINT val);
	void			Write(TCHAR* sec, TCHAR* key, int val);
	void			Write(TCHAR* sec, TCHAR* key, double val);
	void			Write(TCHAR* sec, TCHAR* key, CString val);

	void			SetPath(CString str);	
	bool			OpenFile(CString sPath);

	CString			GETDRV();												// 현재 솔루션 실행 드라이브 판단 추가
	


private:
	TCHAR*			GetItem(TCHAR* sec, TCHAR* key, TCHAR* def);
	UINT			GetUINT(TCHAR* Sec, TCHAR* key, UINT def);
	int				GetInt(TCHAR* Sec, TCHAR* key, int def);
	double			GetDbl(TCHAR* Sec, TCHAR* key, double def);
	CString			GetStr(TCHAR* Sec, TCHAR* key, CString def);
	CString			GetPath()	{ return m_strCfgPath; }


	CString			m_strINIDrive;
	CString			m_strCfgPath;
	TCHAR			buf[128];	
	
	// Device Info
	int				m_nCamCount;
	int				m_nLightCount;

	// Initialize Info
	CString			m_strModelPath;
	int				m_nPCNum;

	// Model Info
	ST_MODEL_INFO	m_stModelInfo;
// 	BOOL			m_bUseStep;
// 	CString			m_strGrabName[MAX_GRAB_STEP_COUNT];
// 	CString			m_strLightValue[MAX_GRAB_STEP_COUNT][MAX_LIGHT_COUNT];
// 	int				m_nGrabCount;
// 	ST_CAMERA_COND  m_stCamCond[MAX_GRAB_STEP_COUNT][WriteResultPanelData];
// 	STRU_LIGHT_INFO m_stLightInfo[MAX_GRAB_STEP_COUNT];

	CString			m_strNetworkDrive;
	CString			m_strOriginPath;
	CString			m_strResultPath;
	CString			m_strLogFilePath;		// Log File Path
	CString			m_strInspPath;
	CString			m_strCurrentDrive;
	int				m_nEqpType;
	int				m_nLogLevel;
	BOOL			m_nSimualtionMode;
	CString			m_strUseDrive;
	CString			m_strSimulationDrive;
	int				m_nDriveLimitSize;
	CString			m_strEQPName;

	// 동일 좌표 중복 불량 (CCD/공정 불량)
	BOOL			m_bUseRepeatAlarm[eCOORD_KIND];
	int				m_nRepeatCompareOffset[eCOORD_KIND];
	int				m_nRepeatLightAlarmCount[eCOORD_KIND];
	int				m_nRepeatHeavyAlarmCount[eCOORD_KIND];
};

#endif // !defined(AFX_CONFIG_H__89A008AB_6C8F_4C25_9509_8F34D97F8EEE__INCLUDED_)
