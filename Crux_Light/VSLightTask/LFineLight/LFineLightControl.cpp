#include "stdafx.h"
#include "LFineLightControl.h"
#include "LFineLight/Library/ProtocolLibRegLFine.h"
#include "VSLightTask.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CVSLightTaskApp theApp;

void CallBackFunc(int nPort,char *pBuff)
{
	((CLFineLight*)theApp.m_pLight)->CallBackFunc_FromUnit(nPort, pBuff);
}


CLFineLight::CLFineLight()
	//: CDialog(CLFineLight::IDD)
	: m_bIsPortOpen(FALSE), m_nPort(0), m_nBaudRate(0), m_nControllerCnt(0), m_nChannelTotalCnt(0) ,m_nChannelType(0)
{
	memset(m_nCtrlChannelCnt, 0, sizeof(m_nCtrlChannelCnt));		// �ʱ�ȭ
}


CLFineLight::~CLFineLight()
{	
}



// Turn On
void CLFineLight::MultiTurnOn(STRU_LIGHT_INFO stLightInfo)
{

	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	CString strLog1 = NULL;
	CString strLog2 = NULL;
	for (UINT nCtrlIndex = 0; nCtrlIndex < m_nControllerCnt; nCtrlIndex++)
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////<Start>
		// 2. ��Ʈ�� ID�� ��ü ä�� �ѹ��� On 180512 YSS
		LFineDll::SetBrightness(m_nPort, nCtrlIndex+1, stLightInfo.stLight[nCtrlIndex].nLightVal, m_nCtrlChannelCnt[nCtrlIndex]);
		int nStartFlag = 1;
		for (UINT j = 0; j < m_nCtrlChannelCnt[nCtrlIndex]; j++)
		{
			if(nStartFlag == 1)
			{	
				strLog1.Format(_T("MultiTurnOn (Port : %d, Ctrl Num : %d, Value : "), m_nPort, nCtrlIndex+1);
				nStartFlag++;
			}
			strLog2.Format(_T("%d "), stLightInfo.stLight[nCtrlIndex].nLightVal[j]);
			strLog1.Append(strLog2);
		}
		strLog1.Append(_T(")"));
		theApp.WriteLog(strLog1);
		///////////////////////////////////////////////////////////////////////////////////////////////////<End>
	}
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// 17.06.15 UI ���� ������ Count �� ������� ��� Controller �� ������ ������ �ѵ��� ����
	//for (UINT i=0; i<stLightInfo.nControllerCount; i++)
// 	for (UINT i=0; i<m_nControllerCnt; i++)
// 	{
// 		//for (UINT j=0; j<stLightInfo.stLight[i].nChCnt; j++)
// 		for (UINT j=0; j<m_nChannelCnt; j++)
// 		{
// 			theApp.WriteLog(_T("MultiTurnOn (%d,%d) Val : %d"), m_nPort, j+1, stLightInfo.stLight[i].nLightVal[j]);
// 			LFineDll::SetBrightness(m_nPort, j+1, stLightInfo.stLight[i].nLightVal[j]);
// 		}
// 	}
// 	Sleep(10);



// LFine - GetC ����
// 	for(UINT i = 0 ; i < m_nChannelCnt ; i++)
// 		GetC(m_nPort,i);
	//RequestCurrentStatus(m_nPort);
}

void CLFineLight::TurnOn(UINT nController, UINT nChannel, UINT nIntensity)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
	//LFineDll::SetBrightness(m_nPort, nChannel, nIntensity);	
	
}

void CLFineLight::GetC(UINT nChannel)
{
// 	if (!m_bIsPortOpen)
// 	{
// 		theApp.WriteLog(TRUE, _T("MultiTurnOn Fail. Port is not open."));
// 		return;
// 	}
// 
// 	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
// 	GetC(m_nPort, nChannel);	
	return;
}

// Turn Off
void CLFineLight::MultiTurnOff()
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
		LFineDll::SetBrightness(m_nPort, n+1, LightVal, m_nChannelTotalCnt);
	}

	free(LightVal);
// 	for (UINT j=0; j<m_nChannelCnt; j++)
// 	{
// 		LFineDll::SetBrightness(m_nPort, j+1, 0);	
// 	}
}

void CLFineLight::TurnOff(UINT nController, UINT nChannel)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
	//LFineDll::SetBrightness(m_nPort, nChannel, 0);	
}

void CLFineLight::CheckLight(int nControllerCount)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	LFineDll::GetError(m_nPort);
	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
// 	for(int i = 0 ; i < m_nChannelCnt ; i++)
// 		plkGetC(m_nPort,i);	
}

void CLFineLight::GetBrightness()
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
	LFineDll::GetBrightness(0);	
}



BOOL CLFineLight::InitializeLight(CString strInitFilePath)
{
	int nRet = APP_OK;

	TCHAR szSimulationFlag[6] = {0,};
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);

	if( !_tcscmp(szSimulationFlag, _T("TRUE")) )
		return nRet;

 	m_nPort				= GetPrivateProfileInt(_T("Settings"),_T("PORT"), 2, strInitFilePath);	
 	m_nBaudRate			= GetPrivateProfileInt(_T("Settings"),_T("BAUDRATE"), 38400, strInitFilePath);
	m_nControllerCnt	= GetPrivateProfileInt(_T("Settings"),_T("CONTROLLER_COUNT "), 4, strInitFilePath);
	m_nChannelTotalCnt	= GetPrivateProfileInt(_T("Settings"),_T("CHANNEL_COUNT "), 16, strInitFilePath);

	int nMaxChCnt = 0;	// �ϳ��� ��Ʈ�ѷ��� ������ �ִ� �ִ� ä�� ���� ����(��� Ÿ���� �����ϴµ� ���)

	// ��Ʈ�ѷ� ID �� ä�� ����		180515 YSS
	for(int nIndex = 0 ; nIndex < (int)m_nControllerCnt; nIndex++)		// ��Ʈ�ѷ� ������ŭ �˻��Ͽ� ��Ʈ�ѷ��� ä�� ������ ����	180711 YSS
	{
		CString strKey = _T("CHANNEL_COUNT_CTRL_");
		CString strNum;
		strNum.Format(_T("%d"), nIndex + 1);
		strKey += strNum;

		m_nCtrlChannelCnt[nIndex] = GetPrivateProfileInt(_T("Settings"), strKey, 16, strInitFilePath);

		if (nIndex == 0)	nMaxChCnt = m_nCtrlChannelCnt[0];
		else				nMaxChCnt = nMaxChCnt > (int)m_nCtrlChannelCnt[nIndex] ? nMaxChCnt : m_nCtrlChannelCnt[nIndex];
	}

	// ���� LFine ���� �Ѱ��� Ÿ�� ���(MODULE_3CH), �ٸ� Ÿ�� �߰��Ǹ� ���� �߰� ó���ؾ� ��.
	if(nMaxChCnt == MODULE_3CH)		m_nChannelType = MODULE_3CH;
	else{}

	if (m_bIsPortOpen == TRUE)
	{
		nRet = LFineDll::ClosePort(m_nPort);
		if (nRet != APP_OK)
		{
			theApp.WriteLog(_T("Port Close Fail"));
			return FALSE;
		}

		m_bIsPortOpen = FALSE;
	}

	switch(LFineDll::OpenPort(m_nPort, m_nControllerCnt, m_nBaudRate, m_nCtrlChannelCnt, CallBackFunc))
	{
	case SUCCESS:
		theApp.WriteLog(_T("Port Open OK."));
		m_bIsPortOpen = TRUE;
		m_pThreadCheckLight = AfxBeginThread(ThreadCheckLight, LPVOID(this));
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

	
	if (!m_bIsPortOpen)
	{
		return FALSE;
	}

	return TRUE;
}

UINT CLFineLight::ThreadCheckLight(LPVOID lParam)
{ 
	//theApp.m_pInternalServer->m_fnCmdEditSend()
	CLFineLight* pThis = (CLFineLight*)lParam;
	int nControllerNum = 0;
	while(1)
	{
		for(int i = 0 ; i < pThis->GetControllerCount() ; i++)
		{
			Sleep(500);
			nControllerNum = i + 1;
			//18.10.18 Light Task ���� Count�� .ini ���Ͽ��� �ҷ������� ����
			theApp.m_pInternalServer->m_fnCmdEditSend(VS_SEND_CHECK_LIGHT, 0, (USHORT)sizeof(int), theApp.GetVsTaskNum(), (BYTE*)&nControllerNum, CMD_TYPE_NORES);
		}
		Sleep(5000);
	}


	return 0;
}

void CLFineLight::CallBackFunc_FromUnit(int nPort,char *pBuff)
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