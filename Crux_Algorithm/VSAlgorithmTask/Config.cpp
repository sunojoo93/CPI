// Config.cpp: implementation of the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Config.h"
#include "Define.h"
#include "VSAlgorithmTask.h"
#include <direct.h>
#include<Shlwapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfig::CConfig()
{

}

CConfig::~CConfig()
{
}


TCHAR* CConfig::GetItem(TCHAR *sec, TCHAR *key, TCHAR *def)
{
	GetPrivateProfileString(sec,key,def,buf,_MAX_PATH, m_strCfgPath);
	return buf;
}

CString CConfig::GetStr(TCHAR* Sec, TCHAR* key, CString def)
{
	return CString(GetItem(Sec,key,(TCHAR*)(LPCTSTR)(def)));
}

UINT CConfig::GetUINT(TCHAR *Sec, TCHAR *key, UINT def)
{
	CString ret = GetStr(Sec,key,_T("--"));
	return (ret==_T("--"))? def :  (UINT)_ttoi((TCHAR*)(LPCTSTR)(ret));
}

int CConfig::GetInt(TCHAR* Sec, TCHAR* key, int def)
{
	CString ret = GetStr(Sec,key,_T("--"));
	return (ret==_T("--"))? def :  (int)_ttoi((TCHAR*)(LPCTSTR)(ret));
}

double CConfig::GetDbl(TCHAR* Sec, TCHAR* key, double def)
{
	CString ret = GetStr(Sec,key,_T("--"));
	return (ret==_T("--"))? def :  (double)_ttof((TCHAR*)(LPCTSTR)(ret));
}

void CConfig::Write(TCHAR *sec, TCHAR *key, UINT val)
{
	CString str; str.Format(_T("%d"),val);
	WritePrivateProfileString(sec,key,(TCHAR*)(LPCTSTR)(str), m_strCfgPath);
}

void CConfig::Write(TCHAR *sec, TCHAR *key, int val)
{
	CString str; str.Format(_T("%d"),val);
	WritePrivateProfileString(sec,key,(TCHAR*)(LPCTSTR)(str), m_strCfgPath);
}

void CConfig::Write(TCHAR *sec, TCHAR *key, double val)
{
	CString str; str.Format(_T("%10.3f"),val);
	WritePrivateProfileString(sec,key,(TCHAR*)(LPCTSTR)(str), m_strCfgPath);
}

void CConfig::Write(TCHAR *sec, TCHAR *key, CString val)
{
	int n = WritePrivateProfileString(sec, key, (TCHAR*)(LPCTSTR)(val), m_strCfgPath);
}

bool CConfig::Load()
{
	CString strSection, strKey;
	CString strModel;

#pragma region >> Loading Device.cfg
// 	if(OpenFile(PATH_DEVICE_FILE))
// 	{
// 	}
#pragma endregion

#pragma region >> Loading Initialize.ini
	if(OpenFile(INIT_FILE_PATH))
	{		
		m_nPCNum			= GetInt(_T("Common"), _T("VISION PC NUM"), 1);		
		CString sSection, sKey;
		sSection.Format(_T("NETWORK_DRIVE_PATH_%d"), m_nPCNum);
		if (GetStr(_T("Common"), _T("SIMULATION Mode"), _T("FALSE")).CompareNoCase(_T("FALSE")) == 0)
		{
			sKey = _T("DRIVE");
			m_nSimualtionMode = FALSE;
		}
		else
		{
			sKey = _T("DRIVE_SIMUL");
			m_nSimualtionMode = TRUE;
		}
		m_strNetworkDrive	= GetStr(sSection.GetBuffer(0), sKey.GetBuffer(0), _T("111"));
		m_strOriginPath		= GetStr(sSection.GetBuffer(0), _T("ORIGIN_PATH"), _T(""));
		m_strResultPath		= GetStr(sSection.GetBuffer(0), _T("RESULT_PATH"), _T(""));
		m_strLogFilePath	= GetStr(_T("LogFilePath"), _T("LogFilePath"), _T(""));
		m_strInspPath		= GetStr(sSection.GetBuffer(0), _T("INSPDATA_PATH"), _T(""));

		//m_strCurrentDrive	= GetStr(_T("Diskinformation"), _T("Last Used Drive"), _T("D_Drive"));
		m_strINIDrive		= GetStr(_T("Diskinformation"), _T("Last Used Drive"), _T("D_Drive")).Left(1) + _T("_Drive\\");		
		m_nDriveLimitSize   = GetInt(_T("Diskinformation"), _T("DriveLimitSize"), 80);	
		m_strUseDrive		= GetStr(_T("Diskinformation"), _T("Use Drive"), _T("C"));
		m_strSimulationDrive = GetStr(_T("Diskinformation"), _T("Simulation Drive"), _T("D_Drive")).Left(1) + _T("_Drive\\");
		m_strEQPName		= GetStr(_T("Common"), _T("EQP"), _T("UNKNOWN"));

		//GetPrivateProfileSection(_T("Pattern_Info"), strPatternInfo, 4096, strINIFilePath);
		// EqpType 은 해당 Type (AVI/SVI/APP) 에 따른 문자열 반환(GetEqpTypeName())으로만 사용하고 분기문 등에 사용하지 말 것.
		// 분기문이 필요하다고 판단되는 부분은 Virtual 로 구현하여 해당 Type 의 파일에만 작업해야함.
		m_nEqpType			= GetInt(_T("Common"), _T("TYPE"), 0);

		// 새로운 CCD 불량 발생
		m_bUseRepeatAlarm[ePIXEL]			= GetInt(_T("Repeat Defect Alarm - CCD"), _T("Use"), 0);
		m_nRepeatCompareOffset[ePIXEL]		= GetInt(_T("Repeat Defect Alarm - CCD"), _T("Compare Offset(Pixel)"), 0);
		m_nRepeatLightAlarmCount[ePIXEL]	= GetInt(_T("Repeat Defect Alarm - CCD"), _T("Light Alarm"), 5);
		m_nRepeatHeavyAlarmCount[ePIXEL]	= GetInt(_T("Repeat Defect Alarm - CCD"), _T("Heavy Alarm"), 10);

		// 공정 불량 발생
		m_bUseRepeatAlarm[eWORK]			= GetInt(_T("Repeat Defect Alarm - Work"), _T("Use"), 0);
		m_nRepeatCompareOffset[eWORK]		= GetInt(_T("Repeat Defect Alarm - Work"), _T("Compare Offset(um)"), 0);
		m_nRepeatLightAlarmCount[eWORK]		= GetInt(_T("Repeat Defect Alarm - Work"), _T("Light Alarm"), 5);
		m_nRepeatHeavyAlarmCount[eWORK]		= GetInt(_T("Repeat Defect Alarm - Work"), _T("Heavy Alarm"), 10);

		// 2018.09.21 MDJ APP Repeat Defect 장비 불량 발생
		m_bUseRepeatAlarm[eMACHINE] = GetInt(_T("Repeat Defect Alarm - Machine"), _T("Use"), 0);
		m_nRepeatCompareOffset[eMACHINE] = GetInt(_T("Repeat Defect Alarm - Machine"), _T("Compare Offset"), 0);
		m_nRepeatLightAlarmCount[eMACHINE] = GetInt(_T("Repeat Defect Alarm - Machine"), _T("Light Alarm"), 5);
		m_nRepeatHeavyAlarmCount[eMACHINE] = GetInt(_T("Repeat Defect Alarm - Machine"), _T("Heavy Alarm"), 10);
	}
#pragma endregion

#pragma region >> Loading Crux_Algorithm.ini
	if(OpenFile(VS_ALGORITHM_TASK_INI_FILE))
	{				
		m_nLogLevel			= GetInt(_T("General"),  _T("LogLevel"), 1);
	}
#pragma endregion

#pragma region >> Loading Device.cfg
	if(OpenFile(DEVICE_FILE_PATH))
	{
		m_nCamCount = 0;
		for (int nGrabberCnt=0; nGrabberCnt<MAX_FRAME_GRABBER_COUNT; nGrabberCnt++)
		{
			strSection.Format(_T("Frame Grabber_%d"), nGrabberCnt);
			for (int nCameraCnt=0; nCameraCnt<MAX_CAMERA_COUNT; nCameraCnt++)
			{
				strKey.Format(_T("Insp Camera_%d"), nCameraCnt);
				if (GetStr(strSection.GetBuffer(0), strKey.GetBuffer(0), _T("F"))==_T("T"))
					m_nCamCount++;
			}
		}
	}
#pragma endregion

	return true;
}

// set config file's self path
void CConfig::SetPath(CString str)
{
	m_strCfgPath = str;
}

bool CConfig::OpenFile(CString sPath)
{
	SetPath(sPath);
	//CFile conf;
	if (!PathFileExists(sPath))
	//if(!conf.Open(sPath.GetBuffer(0), CFile::modeRead)) 
	{
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Not exist file (%s)"), sPath);
		return false ;
	}
	//conf.Close();
	return true;
}

// 181001 yskim
// Algorithm Task를 제외한 나머지 Task들은 GetModuleFileName 호출 방식으로 통일되어있음
// 따라서 Algorithm Task도 GetModuleFileName 호출 방식으로 변경
CString CConfig::GETDRV() 
{ 
	TCHAR buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileName(NULL, buff, sizeof(buff));
	CString strFolder = buff;
	CString strRet = strFolder.Left(1);
	strRet.MakeUpper();		//180919 YSS	// 181001 yskim Lower -> Upper로 변경
	return strRet;

	//int nDrive = 0;   nDrive = _getdrive();   
	//CString str;	
	//str.Format(_T("%c"), char(nDrive) + 96);
	//return str;   
};

// 파일 형식 사용 안함 - IPC Message 활용
// bool CConfig::LoadModel(CString strModel)
// {
// 	CString strKey = _T("");
// 	CString strCameraValue = _T("");
// 	CString strGrabStep = _T("");
// 
// 	int nCurPos = 0;
// 
// 	if(OpenFile(PATH_MODEL_FILE + strModel + _T("\\") + strModel + EXTENSION_MODEL_FILE))
// 	{		
// 		m_nGrabCount = GetInt(_T("Panel Info"), _T("Grab Step"), 1);
// 		
// 		for (int nGrabCnt=0; nGrabCnt<m_nGrabCount; nGrabCnt++)
// 		{
// 			strKey.Format(_T("Grab_%d"), nGrabCnt + 1);
// 			strGrabStep = GetStr(_T("Panel Info"), strKey.GetBuffer(0), _T(""));
// 			nCurPos = 0;
// 			m_bUseStep = strGrabStep.Tokenize(_T(","), nCurPos)==_T("T")?TRUE:FALSE;
// 			m_strGrabName[nGrabCnt]	= strGrabStep.Tokenize(_T(","), nCurPos);
// 
// 			// Get Light Value
// 			CString strLightValue;
// 
// 			if(m_stLightInfo[nGrabCnt].stLight == NULL)
// 				m_stLightInfo[nGrabCnt].stLight = new STRU_SERIAL_INFO[m_nLightControllerCount];
// 
// 			m_stLightInfo[nGrabCnt].bUse = TRUE;		// 임시 설정
// 			m_stLightInfo[nGrabCnt].nControllerCount = m_nLightControllerCount;
// 			
// 			for (int nLightCnt=0; nLightCnt<m_nLightControllerCount; nLightCnt++)
// 			{				
// 				
// 				m_stLightInfo[nGrabCnt].stLight[nLightCnt].bUse = TRUE;		// 임시설정
// 				strKey.Format(_T("LIGHT_%d"), nLightCnt + 1);
// 				strLightValue = GetStr(m_strGrabName[nGrabCnt].GetBuffer(0), strKey.GetBuffer(0), _T("0"));
// 				
// 				if (m_stLightInfo[nGrabCnt].stLight[nLightCnt].nLightVal == NULL)
// 					m_stLightInfo[nGrabCnt].stLight[nLightCnt].nLightVal = new int[m_nLightChannelCount];
// 
// 				m_stLightInfo[nGrabCnt].stLight[nLightCnt].nChCnt = m_nLightChannelCount;
// 
// 				// Parsing Channel Value
// 				nCurPos = 0;
// 				for (int nChCnt=0; nChCnt<m_nLightChannelCount; nChCnt++)
// 				{
// 					m_stLightInfo[nGrabCnt].stLight[nLightCnt].nLightVal[nChCnt]		= _ttoi(strLightValue.Tokenize(_T(","), nCurPos));
// 				}
// 			}
// 			for (int nCamCnt=0; nCamCnt<m_nCamCount; nCamCnt++)
// 			{
// 				strKey.Format(_T("CAMERA_%d"), nCamCnt + 1);
// 				strCameraValue = GetStr(m_strGrabName[nGrabCnt].GetBuffer(0), strKey.GetBuffer(0), _T("0"));
// 
// 				// Parsing Camera Conditions
// 				nCurPos = 0;
// 				m_stCamCond[nGrabCnt][nCamCnt].dExpTime		= _ttof(strCameraValue.Tokenize(_T(","), nCurPos));
// 				m_stCamCond[nGrabCnt][nCamCnt].nAnalogGain	= _ttoi(strCameraValue.Tokenize(_T(","), nCurPos));
// 				m_stCamCond[nGrabCnt][nCamCnt].nSeqMode		= _ttoi(strCameraValue.Tokenize(_T(","), nCurPos));
// 			}
// 		}		
// 	}
// 	return true;
// }