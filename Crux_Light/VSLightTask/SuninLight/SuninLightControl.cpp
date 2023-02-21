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
	memset(m_nCtrlChannelCnt, 0, sizeof(m_nCtrlChannelCnt));		// �ʱ�ȭ
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
		// 1. ���� ���(ä�� �ϳ��� cmd ����)
//  		for (UINT j=0; j<m_nChannelTotalCnt; j++)
//  		{
//  			theApp.WriteLog(_T("MultiTurnOn (%d,%d) Val : %d"), m_nPort, j+1, stLightInfo.stLight[i].nLightVal[j]);
//  			SuninDll::SetBrightness(m_nPort, j+1, stLightInfo.stLight[i].nLightVal[j]);
//  		}
		///////////////////////////////////////////////////////////////////////////////////////////////////<End>



		///////////////////////////////////////////////////////////////////////////////////////////////////<Start>
		// 2. ��Ʈ�� ID�� ��ü ä�� �ѹ��� On 180512 YSS
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
	// 2018.12.01 ����(yskim)
	// CSOT : ������(�ܰ��˻�����) ��� ä�� 1,2,3,4,5,6,7,8 �̰�, �Ϻ� ä�� 1,3,5,7 �� (��� �̷��� ��)
	// CSOT : �̿� ���� ������ ȥ���� ������ �ֱ⿡ �Ʒ��� ���� ó���� ��
	// CSOT : 4ä��ó�� ����ϱ� ���� UI �� ä�� ������ 4���� �����ϰ�
	//        UI�� ��� 1ch �� => ���� �������� �� 1,2 ä��
	//        UI�� ��� 2ch �� => ���� �������� �� 3,4 ä��
	//        UI�� ��� 3ch �� => ���� �������� �� 5,6 ä��
	//        UI�� ��� 4ch �� => ���� �������� �� 7,8 ä��
	//        UI�� �Ϻ� 1ch �� => ���� �������� �� 1 ä��
	//        UI�� �Ϻ� 2ch �� => ���� �������� �� 3 ä��
	//        UI�� �Ϻ� 3ch �� => ���� �������� �� 5 ä��
	//        UI�� �Ϻ� 4ch �� => ���� �������� �� 7 ä��
	// ( ����� ��Ʈ�ѷ����� �Ŵ��� ����� 4ä���� )
	// Initialize.ini �� Site Type�� ���� ����� (0 = Default, 1 = B7, 2 = B11, 3 = GVO, 4 = CSOT)
	if ((theApp.GetSiteTypeNum() == 4 || theApp.GetSiteTypeNum() == 2) && theApp.GetInspType() == 2) // 2018.12.18 �ּ���(sojoo)
	{
		for (int nCtrlNum = 0; nCtrlNum < (int)m_nControllerCnt; nCtrlNum++)
		{
			// �������� �־�� ������ �ս� �߻����� ����
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

	//3. ��� ä�� �ѹ��� On (Sunin Light �� ���..)		180516 YSS
	int nTotalChCnt = 0;
	for(int n = 0; n < (int)m_nControllerCnt ;n++)
		nTotalChCnt += m_nCtrlChannelCnt[n];		// �� ä�� ���� ���

	// ���� ũ�� �Ҵ� �� �ʱ�ȭ
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

	// LightVal �迭�� ��� ä�� �� �Ľ�
	int nIndex = 0;
	for(int nCtrlNum = 0 ; nCtrlNum < (int)m_nControllerCnt ; nCtrlNum++)
	{
		for(int nChNum = 0 ; nChNum < (int)m_nCtrlChannelCnt[nCtrlNum]; nChNum++)
		{
			LightVal[nIndex] = stLightInfo.stLight[nCtrlNum].nLightVal[nChNum];

			// B7 Ph3 APP ���� ���� �� ä�κ� �⺻ �ɼ� �� ���� ���� ����  181211 YSS
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

	// 3. ��� ä�� �ѹ��� ���� ��� ����		180516 YSS
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

// ��� ����
void CSuninLight::TurnOn(UINT nController, UINT nChannel, UINT nIntensity)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
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
// 	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
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

// ��� ����
void CSuninLight::TurnOff(UINT nController, UINT nChannel)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
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

	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
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
	m_nControllerCnt	= GetPrivateProfileInt(_T("Settings"),_T("CONTROLLER_COUNT "), 4, strInitFilePath);		// ��Ʈ�ѷ� �� ����
	m_nChannelTotalCnt	= GetPrivateProfileInt(_T("Settings"),_T("CHANNEL_COUNT "), 16, strInitFilePath);		// ��� ��Ʈ�ѷ��� �� ä�� ����

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
	
	// ���� Sunin ���� �ΰ��� Ÿ�� ���(MODULE_12CH, MODULE_17CH)
	// 'MODULE_12CH' : ��Ʈ�ѷ��� �ִ� ä�� ���� 12��(Phase3 Tester ���� �� 4�� ��Ʈ�ѷ� 48ä�� ���. 12(CtrlID_1) + 12(CtrlID_2) + 12(CtrlID_3) + 12(CtrlID_4))
	// 'MODULE_17CH' : ��Ʈ�ѷ��� �ִ� ä�� ���� 17��(��� ���� �� 1�� ��Ʈ�ѷ� 17ä�� ���. 17(CtrlID_1))
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
			//18.10.18 Light Task ���� Count�� .ini ���Ͽ��� �ҷ������� ����
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