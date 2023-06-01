// Config.cpp: implementation of the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Config.h"
#include "VSSequenceTask.h"
#include "stdlib.h"

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
	if(OpenFile(PATH_DEVICE_FILE))
	{
		m_nCamCount = 0;
		for (int nGrabberCnt=0; nGrabberCnt<NUM_FRAME_GRABBER; nGrabberCnt++)
		{
			strSection.Format(_T("Frame Grabber_%d"), nGrabberCnt);
			for (int nCameraCnt=0; nCameraCnt<MAX_CAMERA_COUNT; nCameraCnt++)
			{
				strKey.Format(_T("Insp Camera_%d"), nCameraCnt);
				if (GetStr(strSection.GetBuffer(0), strKey.GetBuffer(0), _T("F"))==_T("T"))
					m_nCamCount++;
			}
		}
		m_nLightCount = 0;
		for (int nLightCnt=1; nLightCnt<=MAX_LIGHT_PORT_COUNT; nLightCnt++)
		{
			strSection = _T("Light Controller");
			strKey.Format(_T("Port_%d"), nLightCnt);
			if (GetInt(strSection.GetBuffer(0), strKey.GetBuffer(0), 0) > 0)
				m_nLightCount++;
		}
	}
#pragma endregion

#pragma region >> Loading Initialize.ini
	if(OpenFile(PATH_INIT_FILE))
	{		
		m_nPCNum		= GetInt(_T("Common"), _T("VISION PC NUM"), 1);
		strModel		= GetStr(_T("Common"), _T("CURRENT RECIPE"), _T("TEST"));
		m_nType			= GetInt(_T("Common"), _T("Type"), 1);
		m_nPcCnt		= GetInt(_T("Common"), _T("VISION PC COUNT"), 1);
		m_nSiteNum		= GetInt(_T("Common"), _T("SITE TYPE"), 1);
	}
#pragma endregion

#pragma region >> Loading DalsaLineCamera.ini
	if (OpenFile(PATH_DalsaLineCamera_FILE))
	{
		m_nImgCount = GetInt(_T("Settings"), _T("ImageCount"), 1);
		
	}
#pragma endregion

// #pragma region >> Loading PGController.ini
// 	
// 		TCHAR strPatternInfo[256];
// 		int nSize = GetPrivateProfileSection(_T("Pattern_Info"), strPatternInfo, 4096, PATH_PG_INIT_FILE);
// 
// 		TCHAR szCurrent[200] = {0, };
// 		TCHAR* pOffset = strPatternInfo;
// 		TCHAR * szToken = NULL;
// 
// 		TCHAR szTotal[1024]= {0,};
// 		TCHAR szPatternName[100] = {0, };
// 		TCHAR szPatternNum[100] = {0, };
// 		for(int i = 0 ; i < nSize ; i++)
// 		{
// 			if(strPatternInfo[i]== 0x00 )
// 			{
// 				memset(szTotal,0,1024);
// 				memcpy(szTotal, pOffset,1024);
// 
// 				memcpy(szCurrent,szTotal,i * 2); // 현재 라인에 Key, Data 읽어와서 szCurrent에 저장
// 
// 				szToken = _tcstok(szCurrent,_T("="));
// 				memcpy(szPatternName,szToken,sizeof(szPatternName)); // szCurrent에서 = 기준으로 tokenize 하면 앞에께 key값(Pattern name)
// 
// 				szToken = _tcstok(NULL,_T("="));
// 				memcpy(szPatternNum,szToken,sizeof(szPatternNum)); // 한번더 tokenize 하면 뒤에께 value 값(Pattern num)
// 
// 				int nIndex = _ttoi(szPatternNum);
// 				
// 				m_stPgInfo.stPgData[nIndex].nPtnNum = nIndex;
// 					 
// 				memcpy(m_stPgInfo.stPgData[nIndex].strPatternName,szPatternName,sizeof(m_stPgInfo.stPgData[nIndex].strPatternName));
// 				pOffset+=(_tcslen(szTotal)+1); // 첫번째 라인에서 처리한 크기 만큼 뒤로 밀면 다음 라인 나옴.
// 				if(strPatternInfo[i+1] == 0x00)
// 					break;
// 			}
// 		}
// 
// #pragma endregion

// 파일 형식 사용 안함 - IPC Message 활용
// #pragma region >> Loading Model File
// 	LoadModel(strModel);
// #pragma endregion
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
		//theApp.m_SequenceTask->m_fnPrintLog(TRUE, _T("SEQLOG - Not exist file (%s)"), sPath);
		return false ;
	}
	//conf.Close();
	return true;
}


ST_PG_DATA* CConfig::GetVoltageInfo(TCHAR* strCurStepName)
{
	for(int i = 0 ; i < MAX_GRAB_STEP_COUNT  ; i++)
	{
		//if(!_tcsicmp(m_stModelInfo.stPgInfo.stPgData[i].strPatternName, strCurStepName))
		//	return &m_stModelInfo.stPgInfo.stPgData[i];
	}
	return NULL;
}

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


bool CConfig::UpdatePGVoltInfo(CString strMtpDrv, CString strPGInfoPath)
{
	//////////////////////////////////////////////////////////////////////////
	// 17.08.10
	// MTP 검사 결과 파일을 확인하여, 검사 성공한 패턴의 Voltage 정보를 'PGVoltInfo.ini' 파일에 갱신한다.
	//////////////////////////////////////////////////////////////////////////
	//bool bRet = false;

	//const int RETRYCOUNT = 10; // 기존 3초에서 1초로 변경 (p.s.. Tianma MTP 기본전압 점등)
	//const int RETRYINTERVAL = 100;

	//strPGInfoPath.Format(_T("%s\\%s"), strMtpDrv, strPGInfoPath);
	//CString strPGInfoPathDefualt = _T("D:\\CRUX\\DATA\\TXT\\default_Data\\PG_DEFAULT_DATA.ini");

	//CString strNetDrv;	
	//strNetDrv.Format(_T("%sNET_CONNECT.txt"), strMtpDrv);
	//
	//CStdioFile fileWrite;
	//int nRetryCntNetDrive = 0;
	//BOOL bRetNetDrive = FALSE;
	//try
	//{
	//	// Network Drive 연결 100ms씩 최대 3초까지 Retry 하는 구문 추가
	//	do{
	//		bRetNetDrive = fileWrite.Open(strNetDrv, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	//		if (bRetNetDrive)
	//		{
	//			fileWrite.Close();
	//			break;
	//		}
	//		else
	//		{
	//			nRetryCntNetDrive++;
	//			theApp.m_SequenceTask->m_fnPrintLog(false, _T("Retry Connect %s Drive (%d)"), strMtpDrv, nRetryCntNetDrive);
	//			Sleep(RETRYINTERVAL);				
	//		}
	//	} while (nRetryCntNetDrive < RETRYCOUNT);

	//	if (!bRetNetDrive)
	//	{
	//		theApp.m_SequenceTask->m_fnPrintLog(true, _T("PLEASE CHECK NETWORK DRIVE CONNECTED (%s) !!!"), strMtpDrv);
	//		return false;
	//	}
	//}
	//catch (CFileException* e)
	//{
	//	TCHAR strErr[256];
	//	e->GetErrorMessage(strErr, 256);
	//	theApp.m_SequenceTask->m_fnPrintLog(true, _T("ERROR OCCURED NETWORK DRIVE WRITE FILE : %s (%s) !!!"), strNetDrv, strErr);
	//	e->Delete();
	//	return false;
	//}	

	//// 검사 결과 원본 파일 존재 유무 확인(예외처리)
	//CFileFind ff;
	//int nRetryCntMTP = 0;
	//BOOL bRetMTP = FALSE;	
	//// MTP 파일 100ms씩 최대 3초까지 대기하는 구문 추가
	//do{
	//	bRetMTP = ff.FindFile(strPGInfoPath);
	//	if (bRetMTP)	break;
	//	else
	//	{
	//		nRetryCntMTP++;
	//		theApp.m_SequenceTask->m_fnPrintLog(false, _T("Retry Read MTP Result (%d)"), nRetryCntMTP);
	//		Sleep(RETRYINTERVAL);
	//	}
	//} while (nRetryCntMTP < RETRYCOUNT);

	//if(!bRetMTP)
	//{
	//	theApp.m_SequenceTask->m_fnPrintLog(true, _T("NOT EXIST PG RESULT FILE : %s !!!(PASS)"), strPGInfoPath);
	//	strPGInfoPath = strPGInfoPathDefualt;
	//	//return false;
	//}

	//// 1. MTP 원본 검사 파일을 읽는다.
	//// INI 파일내에 존재하는 Section명 리스트 저장
	//vector<CString>	SectionList;

	////TCHAR lpszReturnBuffer[1024];
	//TCHAR tmpDat[50] = {0,};
	//TCHAR szBuf[150] = {0,};
	//byte* bytes  = new byte[1024]; 
	//CString strRGBCH_Index = _T("");
	//CString strTemp = _T(""); 
	//int nCnt =0;
	//bool bMTP = false;
	//ST_PG_INFO stPgInfo = theApp.m_Config.GetPgInfo();	
	//try
	//{
	//	GetPrivateProfileSectionNames((LPWSTR)bytes, 1024, strPGInfoPath);	
	//			
	//	while (true)
	//	{
	//		TRACE((LPWSTR)bytes);
	//		CString tmp = _T("");

	//		tmp = (LPWSTR)bytes;
	//		int nlength = tmp.GetLength();
	//		if(nlength == 0) break;

	//		memset(tmpDat, 0x00, sizeof(tmpDat));
	//		memcpy(tmpDat, bytes, sizeof(TCHAR) * nlength);

	//		// Section 저장
	//		if(_tcsicmp(tmpDat, PG_RECIPE) && _tcsicmp(tmpDat, RGB_CH))
	//			SectionList.push_back(tmpDat);

	//		bytes+= (sizeof(TCHAR) * nlength + 2) ;
	//	}

	//	stPgInfo.Init();

	//	GetPrivateProfileString(RGB_CH, RGB_CH_INDEX,	_T("-1"), szBuf, sizeof(szBuf), strPGInfoPath);
	//	memcpy(stPgInfo.strChIndex, szBuf, sizeof(stPgInfo.strChIndex));

	//	int nCount = 0;
	//	strRGBCH_Index.Format(_T("%s"),	szBuf);
	//	while (AfxExtractSubString(strTemp, strRGBCH_Index, nCount++, ','))
	//		stPgInfo.nChIndexNum++;

	//	int ratio = theApp.m_Config.GetSiteNum() == 10 ? 1000 : 100;
	//	double ceil_value = theApp.m_Config.GetSiteNum() == 10 ? 0 : 0.005;

	//	// 2. 각 패턴 별 검사 결과 성공/실패 여부를 확인한다.
	//	int nSectionCount = SectionList.size();
	//	int nIndex = 0;
	//	CString strMTP = _T("");
	//	for (int nMTP = 0; nMTP < nSectionCount; nMTP++)
	//	{
	//		GetPrivateProfileString(SectionList[nMTP], PATTERN_MTP, _T(""), szBuf, sizeof(szBuf), strPGInfoPath);
	//		strMTP.Format(_T("%s"), szBuf);
	//		if (strMTP == "DISPLAY_ABNORMAL")
	//		{
	//			bMTP = true;
	//			break;
	//		}

	//	}
	//	if (!bMTP && theApp.m_Config.GetType() == TYPE_AVI)
	//	{

	//		for (int nPgCount = 0; nPgCount < MAX_GRAB_STEP_COUNT; nPgCount++)
	//		{
	//			CString a;
	//			a.Format(_T("%s"),stPgInfo.stPgData[nPgCount].strPatternName);
	//			
	//			if (a == _T("BLACK"))
	//			{
	//				stPgInfo.stPgData[nPgCount].wVoltage[0] = (WORD)(7500);	// 소수점 3째자리 반올림

 //					stPgInfo.stPgData[nPgCount].wVoltage[1] = (WORD)(7500);	// 소수점 3째자리 반올림

 //					stPgInfo.stPgData[nPgCount].wVoltage[2] = (WORD)(7500);	// 소수점 3째자리 반올림
	//				break;
	//			}
	//		}
	//	}

	//	while(nIndex < nSectionCount)
	//	{
	//		CString strSection = SectionList[nIndex];
	//		memset(szBuf,0,sizeof(szBuf));
	//		CString strJudge = _T("");
	//		
	//		// MTP 검사 결과 파일에서 검사 '성공', '실패' 여부를 확인한다.
	//		GetPrivateProfileString(strSection, JUDGE,	_T("SKIP"), szBuf, sizeof(szBuf), strPGInfoPath);
	//		strJudge.Format(_T("%s"),	szBuf);

	//		//if(strJudge.CompareNoCase(_T("OK")) == 0)			// MTP 검사 결과 '성공'인 경우,
	//		//{
	//			//int nPatternIndex = 0;
	//			//int nGrabIndex = 0;
	//			// MTP 검사 결과 파일에서 성공한 패턴의 Voltage 정보를 읽는다
	//			int nPatternIndex  = GetPrivateProfileInt(strSection, PATTERN_INDEX,-1, strPGInfoPath);

	//			for (int nGrabCnt = 0 ; nGrabCnt < MAX_GRAB_STEP_COUNT; nGrabCnt++)
	//			{
	//			  if(stPgInfo.stPgData[nGrabCnt].nPtnNum == nPatternIndex)
	//			  {
	//				//  nGrabIndex = nGrabCnt;				

	//				  if (nPatternIndex <= 0)
	//				  {
	//					  theApp.m_SequenceTask->m_fnPrintLog(true, _T("ABNORMAL PATTERN INDEX - %d (%s - [%s]) !!!"), nPatternIndex, strPGInfoPath, strSection);
	//					  return false;
	//				  }

	//				  stPgInfo.stPgData[nGrabCnt].bJudge = TRUE;

	//				  stPgInfo.stPgData[nGrabCnt].nPtnNum = nPatternIndex;
	//				  //memcpy(m_stPgInfo.stPgData[nPatternIndex-1].strPatternName, GetPatternName(nPatternIndex), 100);

	//				  if (bMTP)
	//				  {
	//					  theApp.m_SequenceTask->m_fnPrintLog(FALSE, _T("MTP : DISPLAY_ABNORMAL"));

	//					  GetPrivateProfileString(strSection, RED_VOLTAGE, _T("0"), szBuf, sizeof(szBuf), strPGInfoPath);
	//					  stPgInfo.stPgData[nGrabCnt].wVoltage[0] = (WORD)(7500);	// 소수점 3째자리 반올림

	//					  GetPrivateProfileString(strSection, GREEN_VOLTAGE, _T("0"), szBuf, sizeof(szBuf), strPGInfoPath);
	//					  stPgInfo.stPgData[nGrabCnt].wVoltage[1] = (WORD)(7500);	// 소수점 3째자리 반올림

	//					  GetPrivateProfileString(strSection, BLUE_VOLTAGE, _T("0"), szBuf, sizeof(szBuf), strPGInfoPath);
	//					  stPgInfo.stPgData[nGrabCnt].wVoltage[2] = (WORD)(7500);	// 소수점 3째자리 반올림
	//				  }
	//				  else 
	//				  {
	//					  GetPrivateProfileString(strSection, RED_VOLTAGE, _T("0"), szBuf, sizeof(szBuf), strPGInfoPath);
	//					  stPgInfo.stPgData[nGrabCnt].wVoltage[0] = (WORD)((_ttof(szBuf) + ceil_value) * ratio);	// 소수점 3째자리 반올림

	//					  GetPrivateProfileString(strSection, GREEN_VOLTAGE, _T("0"), szBuf, sizeof(szBuf), strPGInfoPath);
	//					  stPgInfo.stPgData[nGrabCnt].wVoltage[1] = (WORD)((_ttof(szBuf) + ceil_value) * ratio);	// 소수점 3째자리 반올림

	//					  GetPrivateProfileString(strSection, BLUE_VOLTAGE, _T("0"), szBuf, sizeof(szBuf), strPGInfoPath);
	//					  stPgInfo.stPgData[nGrabCnt].wVoltage[2] = (WORD)((_ttof(szBuf) + ceil_value) * ratio);	// 소수점 3째자리 반올림
	//				  }

	//				  GetPrivateProfileString(strSection, COLOR_X, _T("0"), szBuf, sizeof(szBuf), strPGInfoPath);
	//				  stPgInfo.stPgData[nGrabCnt].dColor_X = _ttof(szBuf);

	//				  GetPrivateProfileString(strSection, COLOR_Y, _T("0"), szBuf, sizeof(szBuf), strPGInfoPath);
	//				  stPgInfo.stPgData[nGrabCnt].dColor_Y = _ttof(szBuf);

	//				  GetPrivateProfileString(strSection, LUMINANCE, _T("0"), szBuf, sizeof(szBuf), strPGInfoPath);
	//				  stPgInfo.stPgData[nGrabCnt].dLuminance = _ttof(szBuf);
	//			
	//			  }
	//			}			
	//			
	//		//}
	//		//else	// MTP 검사 결과 '실패'인 경우,
	//		//{
	//		//	// Voltage 정보 저장하지 않고 skip
	//		//	nIndex++;
	//		//	continue;
	//		//}
	//		nIndex++;
	//	}
	//
	//	//set pg 데이터 
	//	theApp.m_Config.SetPgInfo(stPgInfo);
	//}
	//catch(...)
	//{
	//	theApp.m_SequenceTask->m_fnPrintLog(true, _T("ERROR OCCURED READ MTP FILE (%s) !!!"), strPGInfoPath);
	//	return false;
	//}
	//
	//return true;
return true;
}