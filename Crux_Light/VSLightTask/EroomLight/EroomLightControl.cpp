#include "stdafx.h"
#include "EroomLightControl.h"
#include "EroomLight/Library/ProtocolLibRegEroom.h"
#include "VSLightTask.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CVSLightTaskApp theApp;

void CallBackFunc(int nPort,char *pBuff)
{
	((CEroomLight*)theApp.m_pLight)->CallBackFunc_FromUnit(nPort, pBuff);
}


CEroomLight::CEroomLight()
	//: CDialog(CEroomLight::IDD)
	: m_bIsPortOpen(FALSE), m_nPort(0), m_nBaudRate(0), m_nControllerCnt(0), m_nChannelCnt(0)/*, m_nChannelType(0)*/
{
	//CEroomLight::m_nChannelType = 0;
}


CEroomLight::~CEroomLight()
{	
}



// Turn On
void CEroomLight::MultiTurnOn(STRU_LIGHT_INFO stLightInfo)
{

	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// 17.06.15 UI 에서 보내는 Count 와 상관없이 모든 Controller 를 설정된 값으로 켜도록 수정
	//for (UINT i=0; i<stLightInfo.nControllerCount; i++)
	for (UINT i=0; i<m_nControllerCnt; i++)
	{
		//for (UINT j=0; j<stLightInfo.stLight[i].nChCnt; j++)
		for (UINT j=0; j<m_nChannelCnt; j++)
		{
			theApp.m_fnWriteLog(_T("MultiTurnOn (%d,%d) Val : %d"), m_nPort, j+1, stLightInfo.stLight[i].nLightVal[j]);
			EroomDll::SetBrightness(m_nPort, j+1, stLightInfo.stLight[i].nLightVal[j]);
		}
	}
	Sleep(10);
// Eroom - GetC 없음
// 	for(UINT i = 0 ; i < m_nChannelCnt ; i++)
// 		GetC(m_nPort,i);
	//RequestCurrentStatus(m_nPort);
}

void CEroomLight::TurnOn(UINT nController, UINT nChannel, UINT nIntensity)
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController 는 사용 않함.. 확인 필요. Todo..
	EroomDll::SetBrightness(m_nPort, nChannel, nIntensity);	
	
}

void CEroomLight::GetC(UINT nChannel)
{
// 	if (!m_bIsPortOpen)
// 	{
// 		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
// 		return;
// 	}
// 
// 	// nController 는 사용 않함.. 확인 필요. Todo..
// 	GetC(m_nPort, nChannel);	
	return;
}

// Turn Off
void CEroomLight::MultiTurnOff()
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	for (UINT j=0; j<m_nChannelCnt; j++)
	{
		EroomDll::SetBrightness(m_nPort, j+1, 0);	
	}
}

void CEroomLight::TurnOff(UINT nController, UINT nChannel)
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController 는 사용 않함.. 확인 필요. Todo..
	EroomDll::SetBrightness(m_nPort, nChannel, 0);	
}

void CEroomLight::CheckLight(int nControllerCount)
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	EroomDll::GetError(m_nPort);
	// nController 는 사용 않함.. 확인 필요. Todo..
// 	for(int i = 0 ; i < m_nChannelCnt ; i++)
// 		plkGetC(m_nPort,i);	
}

void CEroomLight::GetBrightness()
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController 는 사용 않함.. 확인 필요. Todo..
	EroomDll::GetBrightness(0);	
}



BOOL CEroomLight::InitializeLight(CString strInitFilePath)
{
	int nRet = APP_OK;

	TCHAR szSimulationFlag[6] = {0,};
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);

	if( !_tcscmp(szSimulationFlag, _T("TRUE")) )
		return nRet;

 	m_nPort				= GetPrivateProfileInt(_T("Settings"),_T("PORT"), 2, strInitFilePath);	
 	m_nBaudRate			= GetPrivateProfileInt(_T("Settings"),_T("BAUDRATE"), 38400, strInitFilePath);
	m_nControllerCnt	= GetPrivateProfileInt(_T("Settings"),_T("CONTROLLER_COUNT "), 4, strInitFilePath);
	m_nChannelCnt		= GetPrivateProfileInt(_T("Settings"),_T("CHANNEL_COUNT "), 16, strInitFilePath);

	// 현재 Eroom 조명 한가지 Protocol 만 사용
	// 채널 개수만 6, 24로 차이
	//m_nChannelType = MODULE_12CH;

	if (m_bIsPortOpen == TRUE)
	{
		nRet = EroomDll::ClosePort(m_nPort);
		if (nRet != APP_OK)
		{
			theApp.m_fnWriteLog(_T("Port Close Fail"));
			return FALSE;
		}

		m_bIsPortOpen = FALSE;
	}

	switch(EroomDll::OpenPort(m_nPort, m_nChannelCnt, CallBackFunc))
	{
	case SUCCESS:
		theApp.m_fnWriteLog(_T("Port Open OK."));
		m_bIsPortOpen = TRUE;
		break;
	case INVALID_PARAMETER:
		theApp.m_fnWriteLog(_T("Invalid parameter"));
		break;
	case INVALID_HANDLE:
		theApp.m_fnWriteLog(_T("Invalid handle"));
		break;
	case INVALID_PORT:
		theApp.m_fnWriteLog(_T("Invalid port"));
		break;
	case INVALID_TYPE:
		theApp.m_fnWriteLog(_T("Invalid module"));
		break;
	case OPEN_ERR:
		theApp.m_fnWriteLog(_T("open failed"));
		break;
	}

	m_pThreadCheckLight = AfxBeginThread(ThreadCheckLight, LPVOID(this)); 
	if (!m_bIsPortOpen)
	{
		return FALSE;
	}

	return TRUE;
}

UINT CEroomLight::ThreadCheckLight(LPVOID lParam)
{ 
	//theApp.m_pInternalServer->m_fnCmdEditSend()
	CEroomLight* pThis = (CEroomLight*)lParam;
	int nControllerNum = 0;
	while(1)
	{
		for(int i = 0 ; i < pThis->GetControllerCount() ; i++)
		{

			Sleep(500);
			nControllerNum = i + 1;
			// 18.10.18 Light Task 조명 Count를 .ini파일로 읽어오도록 수정
			theApp.m_pInternalServer->m_fnCmdEditSend(VS_SEND_CHECK_LIGHT, 0, (USHORT)sizeof(int), theApp.GetVsTaskNum(), (BYTE*)&nControllerNum, CMD_TYPE_NORES);
		}
		Sleep(5000);
	}


	return 0;
}

void CEroomLight::CallBackFunc_FromUnit(int nPort,char *pBuff)
{
// 	CString strTemp;
// 	CString strOrg=_T("");
// 	char* token = NULL;
// 	//static int nChannelCnt = 0;
// 
// 
// 
// 	if (pBuff == NULL)
// 	{
// 		theApp.m_fnWriteLog(_T("Call Back data is NULL."));
// 		return;
// 	}
// 
// 	PROTOCOL_INFO brightInfo;
// 	memcpy(&brightInfo,(char *)pBuff,sizeof(PROTOCOL_INFO));
// 
// 	CString strLog = NULL;
// 
// 	token = strtok(brightInfo.strcmd," ");    
// 
// 	while (token != NULL)
// 	{
// 		if( strstr(token,"getc") )
// 		{
// 			strLog.Format(_T("Current Value : "));
// 			strLog += strtok(NULL, " ");
// 
// 			theApp.m_fnWriteLog(strLog);
// 			break;
// 		} 
// 
// 		if( strstr(token,"geterrstatus") )
// 		{
// 			strLog.Format(_T("Error - 0"));
// 			theApp.m_pLogWriter->m_fnWriteLog(strLog);
// 			break;
// 		}
// 		if( strstr(token,"error") )
// 		{
// 			strLog += _T("Error - ");
// 			strLog += strtok(NULL, " ");
// 
// 			theApp.m_pLogWriter->m_fnWriteLog(strLog);
// 
// 			break;
// 		}
// 		if( strstr(token,"setbrightness") )
// 		{
// 			break;
// 		}
// 		break;
// 	}
	return ;
}