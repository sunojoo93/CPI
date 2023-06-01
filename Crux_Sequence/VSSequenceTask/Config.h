// Config.h: interface for the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_H__89A008AB_6C8F_4C25_9509_8F34D97F8EEE__INCLUDED_)
#define AFX_CONFIG_H__89A008AB_6C8F_4C25_9509_8F34D97F8EEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\..\CommonHeaders\Structure.h"
#include <direct.h>
#include <array>



#define VS_SEQUENCE_TASK_INI_FILE			theApp.m_Config.GETDRV() + _T(":\\CRUX\\Data\\INI\\CRUX_Sequence.ini")
#define PATH_INIT_FILE						theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\INI\\Initialize.ini")
#define PATH_DalsaLineCamera_FILE						theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\INI\\DalsaLineCamera1.ini")
#define PATH_DEVICE_FILE					theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\INI\\Device.cfg")
#define PATH_MODEL_FILE						theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\MODEL\\")
#define PATH_PG_INIT_FILE					theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\INI\\PGController.ini")
#define PATH_PG_VOLTINFO_FILE				theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\INI\\PGVoltInfo.ini")
//KYH 2021.04.15 Patten_Delay
#define Patten_Delay_FILE					theApp.m_Config.GETDRV() + _T(":\\CRUX\\DATA\\INI\\Patten_Delay.ini")
#define EXTENSION_MODEL_FILE				_T(".pnlinf")

#define NUM_FRAME_GRABBER					4
#define NUM_INSP_CAMERA						theApp.m_Config.GetUseCamCount()
#define NUM_LIGHT_CONTROLLER				theApp.m_Config.GetUseLightControllerCount()
#define TYPE_AVI							0
#define TYPE_SVI							1
#define TYPE_APP							2
#define TYPE_AMT							3		// AMT 추가 180801 YSS
#define TYPE_BUMP							4	
#define TYPE_AOT_CHIPPING					5
#define TYPE_ALM							6

#define PG_RECIPE							_T("PG_RECIPE")
#define RGB_CH								_T("RGB_CH")//_T("RGB_CH_INDEX")
#define RGB_CH_INDEX						_T("INDEX")
#define PATTERN_INDEX						_T("PATTERN_INDEX")
#define PATTERN_MTP							_T("MTP")
#define JUDGE								_T("JUDGE")
#define RED_VOLTAGE							_T("RED_VOLTAGE")
#define GREEN_VOLTAGE						_T("GREEN_VOLTAGE")
#define BLUE_VOLTAGE						_T("BLUE_VOLTAGE")
#define COLOR_X								_T("COLOR_X")
#define COLOR_Y								_T("COLOR_Y")
#define LUMINANCE							_T("LUMINANCE")
#define DUST_PG_INDEX						-100

class CConfig  
{
public:
	CConfig();
	virtual ~CConfig();

	bool			Load();
// 파일 형식 사용 안함 - IPC Message 활용
// 	bool			LoadModel(CString strModel = _T(""));
	// Get Func
	BOOL	IsUseStep(CString AreaName, int nGrabCnt)									
	{	
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PatternList[nGrabCnt].Use;
			}
		}
		return FALSE;
	};		// 현재 Step 사용 여부
	BOOL	IsUseVacuum(CString AreaName, int nGrabCnt)
	{	
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PatternList[nGrabCnt].Vacuum;
			}
		}
		return FALSE;
	};		// 현재 Step Vaccum사용 여부
	BOOL	IsUseCamera(CString AreaName, int nGrabCnt, int nCamCnt)					
	{	
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PatternList[nGrabCnt].Cam_Condition[nCamCnt].Use;
			}
		}
		return FALSE;
	};		// 현재 Step 사용 여부
	ST_LIGHT_COND_AOT		GetLightInfo(CString AreaName, int nGrabCnt, int nLightNum)
	{	
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PatternList[nGrabCnt].Light_Condition[nLightNum];
			}
		}
	};
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

	BOOL	IsUseLight(CString AreaName, int nGrabCnt, int nLightNum)
	{
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PatternList[nGrabCnt].Light_Condition[nLightNum].Use;
			}
		}
		return FALSE;
	};

	//// 패턴 별로 조명 사용 유무를 설정 한다.
	//void SetUseLight()					
	//{
	//	for (int nGrabCnt = 0; nGrabCnt < MAX_GRAB_STEP_COUNT; nGrabCnt++)
	//	{
	//		m_bIsUseLight[nGrabCnt] = IsUseLight(nGrabCnt);			
	//	}		
	//};
	BOOL					IsDust(int nGrabCnt)									{	return m_bIsUseLight[nGrabCnt]											;};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ST_CAM_COND_AOT		GetCameraConditions(CString AreaName, int nGrabCnt, int nCamCnt)
	{	
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PatternList[nGrabCnt].Cam_Condition[nCamCnt];
			}
		}
	};
	int		GetGrabCount(CString AreaName)											
	{
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PtnCount;
			}
		}
	};
	TCHAR*		GetCurStepName(CString AreaName, int nGrabCnt)
	{
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PatternList[nGrabCnt].PatternName;
			}
		};
	};
	UINT	GetCamPSMode(CString AreaName, int nGrabCnt, int nCamCnt)					
	{	
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PatternList[nGrabCnt].Cam_Condition[nCamCnt].PS;
			}
		}
	};		// PixelShift Mode
	DOUBLE	GetCamExposeVal(CString AreaName, int nGrabCnt, int nCamCnt)
	{ 
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PatternList[nGrabCnt].Cam_Condition[nCamCnt].PS;
			}
		}
	};		// 2021.12.15~ MDJ Modify Camera Expose Time

	int						GetUseLightControllerCount()							{	return m_nLightCount													;};
	int						GetUseCamCount()										{	return m_nCamCount														;};
	int						GetPCNum()												{	return m_nPCNum															;};
	int						GetType()												{	return m_nType															;};		
	int						GetPCCnt()												{   return m_nPcCnt															;};
	int						GetSiteNum()											{	return m_nSiteNum														;};
	int						GetShareImgCount()                                      {   return m_nImgCount                                                      ;};
	// PG 레시피 처리
	ST_PG_DATA*				GetVoltageInfo(TCHAR* strCurStepName);
	//ST_PG_INFO				GetPgInfo()											
	//								{	
	//												int a = 0;
	//												return m_stModelInfo.stPgInfo									
	//								;};
	ST_CAM_COND_AOT		GetLineInfo(CString AreaName, int nGrabCnt, int nCamCnt )
	{
		for (int i = 0; i < m_stModelInfo.GrabCount; ++i)
		{
			if (m_stModelInfo.GrabArea[i].Name == AreaName)
			{
				return m_stModelInfo.GrabArea[i].PatternList[nGrabCnt].Cam_Condition[nCamCnt];
			}
		}
	};
	bool					UpdatePGVoltInfo(CString strMtpDrv, CString strOrgFilePath);		// 17.08.10 PG Voltage 값 갱신 함수
	//void					SetPgInfo(ST_PG_INFO stPgInfo)							{	m_stModelInfo.stPgInfo = stPgInfo										;};
	//int						GetCurPgIndex(int nGrabCnt)	{if (IsDust(nGrabCnt)) return DUST_PG_INDEX;	return m_stModelInfo.stPgInfo.stPgData[nGrabCnt].nPtnNum;};
	void					SetModelInfo(ST_RECIPE_INFO_AOT* pStModelInfo)				{	m_stModelInfo = *pStModelInfo;							 };

	//// AOT ////
	//void					SetModelInfo_AOT(ST_RECIPE_INFO_AOT* pStModelInfo) { m_stModelInfo = *pStModelInfo;	/*SetUseLight();*/ };



private:
	void					Write(TCHAR* sec, TCHAR* key, UINT val);
	void					Write(TCHAR* sec, TCHAR* key, int val);
	void					Write(TCHAR* sec, TCHAR* key, double val);
	void					Write(TCHAR* sec, TCHAR* key, CString val);

	TCHAR*					GetItem(TCHAR* sec, TCHAR* key, TCHAR* def);
	UINT					GetUINT(TCHAR* Sec, TCHAR* key, UINT def);
	int						GetInt(TCHAR* Sec, TCHAR* key, int def);
	double					GetDbl(TCHAR* Sec, TCHAR* key, double def);
	CString					GetStr(TCHAR* Sec, TCHAR* key, CString def);
	CString					GetPath()	{ return m_strCfgPath; }
	CString					TCHARToString(const TCHAR* ptsz);

	void					SetPath(CString str);
	bool					OpenFile(CString sPath);

	CString					m_strCfgPath;
	TCHAR					buf[128];	
	
	// Device Info
	int						m_nCamCount;
	int						m_nLightCount;

	// Initialize Info
	CString					m_strModelPath;
	int						m_nPCNum;
	int						m_nPcCnt;
	int						m_nType;
	int						m_nSiteNum;

	int						m_nImgCount;

	// Model Info
	ST_RECIPE_INFO_AOT			m_stModelInfo;
	//ST_RECIPE_INFO_AOT		m_stModelInfo_AOT;

	// 조명 사용 유무 (DUST 판단용)
	BOOL					m_bIsUseLight[MAX_GRAB_STEP_COUNT];
//	ST_PG_INFO				m_stPgInfo;
	// 	BOOL					m_bUseStep;
	// 	CString					m_strGrabName[MAX_GRAB_COUNT];
	// 	CString					m_strLightValue[MAX_GRAB_COUNT][MAX_LIGHT_COUNT];
	// 	int						m_nGrabCount;
	// 	ST_CAMERA_COND			m_stCamCond[MAX_GRAB_COUNT][MAX_CAMERA_COUNT];
	// 	STRU_LIGHT_INFO			m_stLightInfo[MAX_GRAB_COUNT];
};

#endif // !defined(AFX_CONFIG_H__89A008AB_6C8F_4C25_9509_8F34D97F8EEE__INCLUDED_)
