#include "stdafx.h"
#include "SuninLightControl.h"
#include "SuninLight/Library/ProtocolLibRegSunin.h"
#include "VSLightTask.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CVSLightTaskApp theApp;

void CallBackFunc(int nPort,char *pBuff)
{
	((CSuninLight*)theApp.m_pLight)->CallBackFunc_FromUnit(nPort, pBuff);
}


CSuninLight::CSuninLight()
	//: CDialog(CSuninLight::IDD)
	: m_bIsPortOpen(FALSE), m_nPort(0), m_nBaudRate(0), m_nControllerCnt(0), m_nChannelTotalCnt(0) ,m_nChannelType(0)
{
	memset(m_nCtrlChannelCnt, 0, sizeof(m_nCtrlChannelCnt));		// 초기화
}


CSuninLight::~CSuninLight()
{	
}



// Turn On
void CSuninLight::MultiTurnOn(STRU_LIGHT_INFO stLightInfo)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	CString strLog1 = NULL;
	CString strLog2 = NULL;
// 	for (UINT nCtrlIndex = 0; nCtrlIndex < m_nControllerCnt; nCtrlIndex++)
// 	{
		///////////////////////////////////////////////////////////////////////////////////////////////////<Start>
		// 1. 기존 방식(채널 하나씩 cmd 던짐)
//  		for (UINT j=0; j<m_nChannelTotalCnt; j++)
//  		{
//  			theApp.WriteLog(_T("MultiTurnOn (%d,%d) Val : %d"), m_nPort, j+1, stLightInfo.stLight[i].nLightVal[j]);
//  			SuninDll::SetBrightness(m_nPort, j+1, stLightInfo.stLight[i].nLightVal[j]);
//  		}
		///////////////////////////////////////////////////////////////////////////////////////////////////<End>



		///////////////////////////////////////////////////////////////////////////////////////////////////<Start>
		// 2. 컨트롤 ID별 전체 채널 한번에 On 180512 YSS
// 		SuninDll::SetBrightness(m_nPort, nCtrlIndex+1, stLightInfo.stLight[nCtrlIndex].nLightVal, m_nCtrlChannelCnt[nCtrlIndex]);
// 		int nStartFlag = 1;
// 		for (UINT j = 0; j < m_nCtrlChannelCnt[nCtrlIndex]; j++)
// 		{
// 			if(nStartFlag == 1)
// 			{	
// 				strLog1.Format(_T("MultiTurnOn (Port : %d, Ctrl Num : %d, Value : "), m_nPort, nCtrlIndex+1);
// 				nStartFlag++;
// 			}
// 			strLog2.Format(_T("%d "), stLightInfo.stLight[nCtrlIndex].nLightVal[j]);
// 			strLog1.Append(strLog2);
// 		}
// 		strLog1.Append(_T(")"));
// 		theApp.WriteLog(strLog1);
		///////////////////////////////////////////////////////////////////////////////////////////////////<End>
//	}

	
	///////////////////////////////////////////////////////////////////////////////////////////////////<Start>
	// 2018.12.01 김양수(yskim)
	// CSOT : 면조명(외관검사조명) 상부 채널 1,2,3,4,5,6,7,8 이고, 하부 채널 1,3,5,7 임 (제어가 이렇게 됨)
	// CSOT : 이에 고객사 측에서 혼동할 요지가 있기에 아래와 같이 처리를 함
	// CSOT : 4채널처럼 사용하기 위해 UI 상 채널 개수를 4개로 설정하고
	//        UI상 상부 1ch 값 => 제어 프로토콜 상 1,2 채널
	//        UI상 상부 2ch 값 => 제어 프로토콜 상 3,4 채널
	//        UI상 상부 3ch 값 => 제어 프로토콜 상 5,6 채널
	//        UI상 상부 4ch 값 => 제어 프로토콜 상 7,8 채널
	//        UI상 하부 1ch 값 => 제어 프로토콜 상 1 채널
	//        UI상 하부 2ch 값 => 제어 프로토콜 상 3 채널
	//        UI상 하부 3ch 값 => 제어 프로토콜 상 5 채널
	//        UI상 하부 4ch 값 => 제어 프로토콜 상 7 채널
	// ( 참고로 컨트롤러에서 매뉴얼 제어는 4채널임 )
	// Initialize.ini 에 Site Type에 의해 실행됨 (0 = Default, 1 = B7, 2 = B11, 3 = GVO, 4 = CSOT)
	if ((theApp.GetSiteTypeNum() == 4 || theApp.GetSiteTypeNum() == 2) && theApp.GetInspType() == 2) // 2018.12.18 주순오(sojoo)
	{
		for (int nCtrlNum = 0; nCtrlNum < (int)m_nControllerCnt; nCtrlNum++)
		{
			// 역순으로 넣어야 데이터 손실 발생하지 않음
			stLightInfo.stLight[nCtrlNum].nLightVal[7] = stLightInfo.stLight[nCtrlNum].nLightVal[3];
			stLightInfo.stLight[nCtrlNum].nLightVal[6] = stLightInfo.stLight[nCtrlNum].nLightVal[3];

			stLightInfo.stLight[nCtrlNum].nLightVal[5] = stLightInfo.stLight[nCtrlNum].nLightVal[2];
			stLightInfo.stLight[nCtrlNum].nLightVal[4] = stLightInfo.stLight[nCtrlNum].nLightVal[2];

			stLightInfo.stLight[nCtrlNum].nLightVal[3] = stLightInfo.stLight[nCtrlNum].nLightVal[1];
			stLightInfo.stLight[nCtrlNum].nLightVal[2] = stLightInfo.stLight[nCtrlNum].nLightVal[1];

			stLightInfo.stLight[nCtrlNum].nLightVal[1] = stLightInfo.stLight[nCtrlNum].nLightVal[0];
			stLightInfo.stLight[nCtrlNum].nLightVal[0] = stLightInfo.stLight[nCtrlNum].nLightVal[0];
		}
	}

	//3. 모든 채널 한번에 On (Sunin Light 만 사용..)		180516 YSS
	int nTotalChCnt = 0;
	for(int n = 0; n < (int)m_nControllerCnt ;n++)
		nTotalChCnt += m_nCtrlChannelCnt[n];		// 총 채널 개수 취득

	// 동적 크기 할당 및 초기화
	UINT *LightVal;
	LightVal = (UINT *)malloc(sizeof(UINT)*nTotalChCnt);
	for(int i = 0 ; i < (int)nTotalChCnt; i++)
		LightVal[i] = 0;
	

	//  181211 YSS
	bool bState = false;
	if (GetPrivateProfileInt(_T("Settings"), _T("Light info"), 16, theApp.GETDRV() + _T(":\\Crux\\DATA\\INI\\SuninLight.ini")) != 0)
	{
		bState = true;
	}

	// YWS 181212

	TCHAR szDefaultGain[5] = { 0, };
	GetPrivateProfileString(_T("Settings"), _T("Default Light Offset"), _T("1.0"), szDefaultGain, sizeof(szDefaultGain), theApp.GETDRV() + _T(":\\Crux\\DATA\\INI\\SuninLight.ini"));
	double dLightOffset = 0.0;
	dLightOffset = _ttof(szDefaultGain);

	// LightVal 배열에 모든 채널 값 파싱
	int nIndex = 0;
	for(int nCtrlNum = 0 ; nCtrlNum < (int)m_nControllerCnt ; nCtrlNum++)
	{
		for(int nChNum = 0 ; nChNum < (int)m_nCtrlChannelCnt[nCtrlNum]; nChNum++)
		{
			LightVal[nIndex] = stLightInfo.stLight[nCtrlNum].nLightVal[nChNum];

			// B7 Ph3 APP 선인 조명 각 채널별 기본 옵셋 값 곱셈 연산 적용  181211 YSS
			if (theApp.GetSiteTypeNum() == 1 && bState == true)	// (0 = Default, 1 = B7, 2 = B11, 3 = GVO, 4 = CSOT)
			{				
				int nAllValue	= GetPrivateProfileInt(_T("Settings"), _T("Default All Value"), 16, theApp.GETDRV() + _T(":\\Crux\\DATA\\INI\\SuninLight.ini"));
				int nRightValue = GetPrivateProfileInt(_T("Settings"), _T("Default Right Value"), 16, theApp.GETDRV() + _T(":\\Crux\\DATA\\INI\\SuninLight.ini"));
				int nAll2Value	= GetPrivateProfileInt(_T("Settings"), _T("Default All2 Value"), 16, theApp.GETDRV() + _T(":\\Crux\\DATA\\INI\\SuninLight.ini"));
				if (LightVal[nIndex] == nAllValue)
				{
					double dAllValue = GetLightOffset(_T("Default All Offset"));
					double dTotal = 0.0;

					dTotal = LightVal[nIndex] * dAllValue;
					LightVal[nIndex] = int(dTotal);
				}
				else if (LightVal[nIndex] == nRightValue)
				{
					double dRightValue = GetLightOffset(_T("Default Right Offset"));
					double dTotal = 0.0;

					dTotal = LightVal[nIndex] * dRightValue;
					LightVal[nIndex] = int(dTotal);
				}
				else if (LightVal[nIndex] == nAll2Value)
				{
					double dAll2Value = GetLightOffset(_T("Default All2 Offset"));
					double dTotal = 0.0;

					dTotal = LightVal[nIndex] * dAll2Value;
					LightVal[nIndex] = int(dTotal);
				}
			}
			else
			{
				LightVal[nIndex] *= dLightOffset;
			}

			nIndex++;
		}
	}

	// 3. 모든 채널 한번에 점등 방식 설정		180516 YSS
	SuninDll::SetBrightness(m_nPort, 0, LightVal, nTotalChCnt);
	
	int nStartFlag = 1;
	nIndex = 0;
	for (UINT nCtrlIndex = 0; nCtrlIndex < m_nControllerCnt; nCtrlIndex++)
	{
		for (UINT j = 0; j < m_nCtrlChannelCnt[nCtrlIndex]; j++)
		{
			if(nStartFlag == 1)
			{	
				strLog1.Format(_T("MultiTurnOn (Port : %d, Ctrl Num : %d, Value : "), m_nPort, nCtrlIndex+1);
				nStartFlag++;
			}
			strLog2.Format(_T("%d "), LightVal[nIndex]);
			strLog1.Append(strLog2);
			nIndex++;
		}
	}
	strLog1.Append(_T(")"));
	theApp.WriteLog(strLog1);

	free(LightVal);
	///////////////////////////////////////////////////////////////////////////////////////////////////<End>

	Sleep(10);

}

// 사용 안함
void CSuninLight::TurnOn(UINT nController, UINT nChannel, UINT nIntensity)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController 는 사용 않함.. 확인 필요. Todo..
	//SuninDll::SetBrightness(m_nPort, nChannel, nIntensity);	
	
}

double CSuninLight::GetLightOffset(CString strLightinfo)
{
	TCHAR szDefaultGain[5] = { 0, };
	GetPrivateProfileString(_T("Settings"), strLightinfo, _T("1.0"), szDefaultGain, sizeof(szDefaultGain), theApp.GETDRV() + _T(":\\Crux\\DATA\\INI\\SuninLight.ini"));

	double dLightOffset = 0.0;
	dLightOffset = _ttof(szDefaultGain);

	return dLightOffset;
}

void CSuninLight::GetC(UINT nChannel)
{
// 	if (!m_bIsPortOpen)
// 	{
// 		theApp.WriteLog(TRUE, _T("MultiTurnOn Fail. Port is not open."));
// 		return;
// 	}
// 
// 	// nController 는 사용 않함.. 확인 필요. Todo..
// 	GetC(m_nPort, nChannel);	
	return;
}

// Turn Off
void CSuninLight::MultiTurnOff()
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	UINT *LightVal;
	LightVal = (UINT *)malloc(sizeof(UINT) * m_nChannelTotalCnt);
	for(int i = 0 ; i < (int)m_nChannelTotalCnt ; i++)
		LightVal[i] = 0;

	for(int n = 0; n < (int)m_nControllerCnt; n++)
	{
		SuninDll::SetBrightness(m_nPort, n+1, LightVal, m_nChannelTotalCnt);
	}

	free(LightVal);
}

// 사용 안함
void CSuninLight::TurnOff(UINT nController, UINT nChannel)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController 는 사용 않함.. 확인 필요. Todo..
	//SuninDll::SetBrightness(m_nPort, nChannel, 0);	
}

void CSuninLight::CheckLight(int nControllerCount)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	SuninDll::GetError(m_nPort);
}

void CSuninLight::GetBrightness()
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController 는 사용 않함.. 확인 필요. Todo..
	SuninDll::GetBrightness(0);	
}



BOOL CSuninLight::InitializeLight(CString strInitFilePath)
{
	int nRet = APP_OK;

	TCHAR szSimulationFlag[6] = {0,};
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);

	if( !_tcscmp(szSimulationFlag, _T("TRUE")) )
		return nRet;

 	m_nPort				= GetPrivateProfileInt(_T("Settings"),_T("PORT"), 2, strInitFilePath);	
 	m_nBaudRate			= GetPrivateProfileInt(_T("Settings"),_T("BAUDRATE"), 38400, strInitFilePath);
	m_nControllerCnt	= GetPrivateProfileInt(_T("Settings"),_T("CONTROLLER_COUNT "), 4, strInitFilePath);		// 컨트롤러 총 갯수
	m_nChannelTotalCnt	= GetPrivateProfileInt(_T("Settings"),_T("CHANNEL_COUNT "), 16, strInitFilePath);		// 모든 컨트롤러의 총 채널 갯수

	int nMaxChCnt = 0;	// 하나의 컨트롤러가 가지고 있는 최대 채널 개수 저장(모듈 타입을 정의하는데 사용)

	// 컨트롤러 ID 당 채널 갯수		180515 YSS
	for(int nIndex = 0 ; nIndex < (int)m_nControllerCnt; nIndex++)		// 컨트롤러 개수만큼 검사하여 컨트롤러당 채널 개수를 저장	180711 YSS
	{
		CString strKey = _T("CHANNEL_COUNT_CTRL_");
		CString strNum;
		strNum.Format(_T("%d"), nIndex + 1);
		strKey += strNum;

		m_nCtrlChannelCnt[nIndex] = GetPrivateProfileInt(_T("Settings"), strKey, 16, strInitFilePath); 
		
		if (nIndex == 0)	nMaxChCnt = m_nCtrlChannelCnt[0];
		else				nMaxChCnt = nMaxChCnt > (int)m_nCtrlChannelCnt[nIndex] ? nMaxChCnt : m_nCtrlChannelCnt[nIndex];
	}
	
	// 현재 Sunin 조명 두가지 타입 사용(MODULE_12CH, MODULE_17CH)
	// 'MODULE_12CH' : 컨트롤러당 최대 채널 개수 12개(Phase3 Tester 설비 총 4개 컨트롤러 48채널 사용. 12(CtrlID_1) + 12(CtrlID_2) + 12(CtrlID_3) + 12(CtrlID_4))
	// 'MODULE_17CH' : 컨트롤러당 최대 채널 개수 17개(모듈 설비 총 1개 컨트롤러 17채널 사용. 17(CtrlID_1))
	if(nMaxChCnt == MODULE_12CH)			m_nChannelType = MODULE_12CH;
	else if(nMaxChCnt == MODULE_17CH)		m_nChannelType = MODULE_17CH;

	if (m_bIsPortOpen == TRUE)
	{
		nRet = SuninDll::ClosePort(m_nPort);
		if (nRet != APP_OK)
		{
			theApp.WriteLog(_T("Port Close Fail"));
			return FALSE;
		}

		m_bIsPortOpen = FALSE;
	}

	switch(SuninDll::OpenPort(m_nPort, m_nControllerCnt, m_nBaudRate, m_nCtrlChannelCnt, CallBackFunc))
	{
	case SUCCESS:
		theApp.WriteLog(_T("Port Open OK."));
		m_bIsPortOpen = TRUE;
		break;
	case INVALID_PARAMETER:
		theApp.WriteLog(_T("Invalid parameter"));
		break;
	case INVALID_HANDLE:
		theApp.WriteLog(_T("Invalid handle"));
		break;
	case INVALID_PORT:
		theApp.WriteLog(_T("Invalid port"));
		break;
	case INVALID_TYPE:
		theApp.WriteLog(_T("Invalid module"));
		break;
	case OPEN_ERR:
		theApp.WriteLog(_T("open failed"));
		break;
	}

	m_pThreadCheckLight = AfxBeginThread(ThreadCheckLight, LPVOID(this)); 
	if (!m_bIsPortOpen)
	{
		return FALSE;
	}

	return TRUE;
}

UINT CSuninLight::ThreadCheckLight(LPVOID lParam)
{ 
	//theApp.m_pInternalServer->m_fnCmdEditSend()
	CSuninLight* pThis = (CSuninLight*)lParam;
	int nControllerNum = 0;
	while(1)
	{
		for(int i = 0 ; i < pThis->GetControllerCount() ; i++)
		{			
			Sleep(500);
			nControllerNum = i + 1;
			//18.10.18 Light Task 조명 Count를 .ini 파일에서 불러오도록 수정
			theApp.m_pInternalServer->m_fnCmdEditSend(VS_SEND_CHECK_LIGHT, 0, (USHORT)sizeof(int), theApp.GetVsTaskNum(), (BYTE*)&nControllerNum, CMD_TYPE_NORES);		
		}
		Sleep(5000);
	}
	return 0;
}

void CSuninLight::CallBackFunc_FromUnit(int nPort,char *pBuff)
{
	CString strTemp;
	CString strOrg=_T("");
	char* token = NULL;
	//static int nChannelCnt = 0;



	if (pBuff == NULL)
	{
		theApp.WriteLog(_T("Call Back data is NULL."));
		return;
	}

	PROTOCOL_INFO brightInfo;
	memcpy(&brightInfo,(char *)pBuff,sizeof(PROTOCOL_INFO));

	CString strLog = NULL;

	token = strtok(brightInfo.strcmd," ");    

	while (token != NULL)
	{
		if( strstr(token,"getc") )
		{
			strLog.Format(_T("Current Value : "));
			strLog += strtok(NULL, " ");

			theApp.WriteLog(strLog);
			break;
		} 

		if( strstr(token,"geterrstatus") )
		{
			strLog.Format(_T("Error - 0"));
			theApp.m_pLogWriter->m_fnWriteLog(strLog);
			break;
		}
		if( strstr(token,"error") )
		{
			strLog += _T("Error - ");
			strLog += strtok(NULL, " ");

			theApp.m_pLogWriter->m_fnWriteLog(strLog);

			break;
		}
		if( strstr(token,"setbrightness") )
		{
			break;
		}
		break;
	}
	return ;
}