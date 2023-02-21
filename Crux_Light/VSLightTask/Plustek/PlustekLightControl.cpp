#include "stdafx.h"
#include "PlustekLightControl.h"
#include "Plustek/ProtocolLibReg.h"
#include "VSLightTask.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CVSLightTaskApp theApp;

void CallBackFunc(int nPort,char *pBuff)
{
	((CPlustekLight*)theApp.m_pLight)->CallBackFunc_FromUnit(nPort, pBuff);
}


CPlustekLight::CPlustekLight()
	//: CDialog(CPlustekLight::IDD)
	: m_bIsPortOpen(FALSE), m_nPort(0), m_nBaudRate(0), m_nControllerCnt(0), m_nChannelCnt(0), m_nChannelType(0)
{
	//CPlustekLight::m_nChannelType = 0;
}


CPlustekLight::~CPlustekLight()
{	
}



// Turn On
void CPlustekLight::MultiTurnOn(STRU_LIGHT_INFO stLightInfo)
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
			plkSetBrightness(m_nPort, j+1, stLightInfo.stLight[i].nLightVal[j]);
		}
	}
	Sleep(10);
	for(UINT i = 0 ; i < m_nChannelCnt ; i++)
		plkGetC(m_nPort,i);
	//plkRequestCurrentStatus(m_nPort);
}

void CPlustekLight::TurnOn(UINT nController, UINT nChannel, UINT nIntensity)
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController 는 사용 않함.. 확인 필요. Todo..
	plkSetBrightness(m_nPort, nChannel, nIntensity);	
	
}

void CPlustekLight::GetC(UINT nChannel)
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController 는 사용 않함.. 확인 필요. Todo..
	plkGetC(m_nPort, nChannel);	

}

// Turn Off
void CPlustekLight::MultiTurnOff()
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	for (UINT j=0; j<m_nChannelCnt; j++)
	{
		plkSetBrightness(m_nPort, j+1, 0);	
	}
}

void CPlustekLight::TurnOff(UINT nController, UINT nChannel)
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController 는 사용 않함.. 확인 필요. Todo..
	plkSetBrightness(m_nPort, nChannel, 0);	
}

void CPlustekLight::CheckLight(int nControllerCount)
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	plkGetError(m_nPort);
	// nController 는 사용 않함.. 확인 필요. Todo..
// 	for(int i = 0 ; i < m_nChannelCnt ; i++)
// 		plkGetC(m_nPort,i);	
}

void CPlustekLight::GetBrightness()
{
	if (!m_bIsPortOpen)
	{
		theApp.m_fnWriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController 는 사용 않함.. 확인 필요. Todo..
	plkGetBrightness(0);	
}



BOOL CPlustekLight::InitializeLight(CString strInitFilePath)
{
	int nRet = APP_OK;

	TCHAR szSimulationFlag[6] = {0,};
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);

	if( !_tcscmp(szSimulationFlag, _T("TRUE")) )
		return nRet;

 	m_nPort				= GetPrivateProfileInt(_T("Settings"),_T("PORT"), 2, strInitFilePath);	
 	m_nBaudRate			= GetPrivateProfileInt(_T("Settings"),_T("BAUDRATE"), 19200, strInitFilePath);
	m_nControllerCnt	= GetPrivateProfileInt(_T("Settings"),_T("CONTROLLER_COUNT "), 4, strInitFilePath);
	m_nChannelCnt		= GetPrivateProfileInt(_T("Settings"),_T("CHANNEL_COUNT "), 16, strInitFilePath);

	if (m_nChannelCnt <= 2)
		m_nChannelType = PLK_MODULE_2CH;
	else if (m_nChannelCnt <= 4)
		m_nChannelType = PLK_MODULE_4CH;
	else if (m_nChannelCnt <= 8)
		m_nChannelType = PLK_MODULE_8CH;
	else if (m_nChannelCnt <= 16)
		m_nChannelType = PLK_MODULE_16CH;
	else
		m_nChannelType = PLK_MODULE_32CH;

	if (m_bIsPortOpen == TRUE)
	{
		nRet = plkClosePort(m_nPort);
		if (nRet != APP_OK)
		{
			theApp.m_fnWriteLog(_T("Port Close Fail"));
			return FALSE;
		}

		m_bIsPortOpen = FALSE;
	}

	switch(plkOpenPort(m_nPort, m_nChannelType, CallBackFunc))
	{
	case PLK_SUCCESS:
		theApp.m_fnWriteLog(_T("Port Open OK."));
		m_bIsPortOpen = TRUE;
		break;
	case PLK_INVALID_PARAMETER:
		theApp.m_fnWriteLog(_T("Invalid parameter"));
		break;
	case PLK_INVALID_HANDLE:
		theApp.m_fnWriteLog(_T("Invalid handle"));
		break;
	case PLK_INVALID_PORT:
		theApp.m_fnWriteLog(_T("Invalid port"));
		break;
	case PLK_INVALID_TYPE:
		theApp.m_fnWriteLog(_T("Invalid module"));
		break;
	case PLK_OPEN_ERR:
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

UINT CPlustekLight::ThreadCheckLight(LPVOID lParam)
{ 
	//theApp.m_pInternalServer->m_fnCmdEditSend()
	CPlustekLight* pThis = (CPlustekLight*)lParam;
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

void CPlustekLight::CallBackFunc_FromUnit(int nPort,char *pBuff)
{
	CString strTemp;
	CString strOrg=_T("");
	char* token = NULL;
	//static int nChannelCnt = 0;



	if (pBuff == NULL)
	{
		theApp.m_fnWriteLog(_T("Call Back data is NULL."));
		return;
	}

	PROTOCOL_4CH_INFO brightInfo4ch;
	memcpy(&brightInfo4ch,(char *)pBuff,sizeof(PROTOCOL_4CH_INFO));

	CString strLog = NULL;

	token = strtok(brightInfo4ch.strcmd," ");    

	while (token != NULL)
	{
		if( strstr(token,"getc") )
		{
			strLog.Format(_T("Current Value : "));
			strLog += strtok(NULL, " ");

			theApp.m_fnWriteLog(strLog);
			break;
		} 

		if( strstr(token,"geterror") )
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