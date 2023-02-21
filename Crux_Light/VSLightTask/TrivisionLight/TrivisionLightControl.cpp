#include "stdafx.h"
#include "TrivisionLightControl.h"
#include "TrivisionLight/Library/ProtocolLibRegTrivisionLight.h"
#include "VSLightTask.h"
#include <regex>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CVSLightTaskApp theApp;

void CallBackFunc(int nPort,char *pBuff)
{
	((CTrivisionLight*)theApp.m_pLight)->CallBackFunc_FromUnit(nPort, pBuff);
}


CTrivisionLight::CTrivisionLight()
	//: CDialog(CTrivisionLight::IDD)
	: m_bIsPortOpen(FALSE), m_nPort(0), m_nBaudRate(0), m_nControllerCnt(0), m_nChannelTotalCnt(0) ,m_nChannelType(0)
{
	memset(m_nCtrlChannelCnt, 0, sizeof(m_nCtrlChannelCnt));		// �ʱ�ȭ
}


CTrivisionLight::~CTrivisionLight()
{	
}



// Turn On
void CTrivisionLight::MultiTurnOn(STRU_LIGHT_INFO stLightInfo)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	CString strLog1 = NULL;
	CString strLog2 = NULL;

	// Initialize.ini �� Site Type�� ���� ����� (0 = Default, 1 = B7, 2 = B11, 3 = GVO, 4 = CSOT)
	if (theApp.GetSiteTypeNum() == 4)
	{
		//for (int nCtrlNum = 0; nCtrlNum < (int)m_nControllerCnt; nCtrlNum++)
		//{
		//	// �������� �־�� ������ �ս� �߻����� ����
		//	stLightInfo.stLight[nCtrlNum].nLightVal[7] = stLightInfo.stLight[nCtrlNum].nLightVal[3];
		//	stLightInfo.stLight[nCtrlNum].nLightVal[6] = stLightInfo.stLight[nCtrlNum].nLightVal[3];

		//	stLightInfo.stLight[nCtrlNum].nLightVal[5] = stLightInfo.stLight[nCtrlNum].nLightVal[2];
		//	stLightInfo.stLight[nCtrlNum].nLightVal[4] = stLightInfo.stLight[nCtrlNum].nLightVal[2];

		//	stLightInfo.stLight[nCtrlNum].nLightVal[3] = stLightInfo.stLight[nCtrlNum].nLightVal[1];
		//	stLightInfo.stLight[nCtrlNum].nLightVal[2] = stLightInfo.stLight[nCtrlNum].nLightVal[1];

		//	stLightInfo.stLight[nCtrlNum].nLightVal[1] = stLightInfo.stLight[nCtrlNum].nLightVal[0];
		//	stLightInfo.stLight[nCtrlNum].nLightVal[0] = stLightInfo.stLight[nCtrlNum].nLightVal[0];
		//}
	}

	// ��Ʈ�ѷ��� ä�� ������ŭ �迭 ���� �� �ʱ�ȭ
	UINT *LightVal;
	LightVal = (UINT *)malloc(sizeof(UINT)*m_nChannelTotalCnt);
	for(int i = 0 ; i < (int)m_nChannelTotalCnt; i++)
		LightVal[i] = 0;
	
	// �迭�� �� ä��� STRU_LIGHT_INFO���κ���
	int nIndex = 0;
	for(int nCtrlNum = 0 ; nCtrlNum < (int)m_nControllerCnt ; nCtrlNum++)
	{
		for(int nChNum = 0 ; nChNum < (int)m_nCtrlChannelCnt[nCtrlNum]; nChNum++)
		{
			LightVal[nIndex] = stLightInfo.stLight[nCtrlNum].nLightVal[nChNum] * m_offSet;
			nIndex++;
		}
	}

	// 3. ��� ä�� �ѹ��� ���� ��� ����		180516 YSS
	TrivisionLightDll::SetBrightness(m_nPort, 0, LightVal, m_nChannelTotalCnt);
	
	// �α� ���
	int nStartFlag = 1;
	nIndex = 0;
	for (UINT nCtrlIndex = 0; nCtrlIndex < m_nControllerCnt; nCtrlIndex++)
	{
		for (UINT j = 0; j < m_nCtrlChannelCnt[nCtrlIndex]; j++)
		{
			if(nStartFlag == 1)
			{	
				//strLog1.Format(_T("MultiTurnOn (Port : %d, Ctrl Num : %d, Offset : %.3f, Value : "), m_nPort, nCtrlIndex + 1, m_offSet);
				strLog1.Format(_T("MultiTurnOn (Port : %d, Offset : %.3f, Value : "), m_nPort, m_offSet);	// �α��ߺ��̶�� CtrlIndex ����
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
	Sleep(10);
}

// ��� ����
void CTrivisionLight::TurnOn(UINT nController, UINT nChannel, UINT nIntensity)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
	//TrivisionLightDll::SetBrightness(m_nPort, nChannel, nIntensity);	
	
}

void CTrivisionLight::GetC(UINT nChannel)
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
void CTrivisionLight::MultiTurnOff()
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

	/*for(int n = 0; n < (int)m_nControllerCnt; n++)
	{
		TrivisionLightDll::SetBrightness(m_nPort, n+1, LightVal, m_nChannelTotalCnt);
	}*/

	// TrivisionLight�� ctrl no �Ⱦ�, 485 ��ſ����� �����ѵ� 485�� �Ⱦ�.
	TrivisionLightDll::SetBrightness(m_nPort, 0, LightVal, m_nChannelTotalCnt);
	theApp.WriteLog(_T("MultiTurnOff (Port : %d, Total Channel : %d)"), m_nPort, m_nChannelTotalCnt);

	free(LightVal);
}

// ��� ����
void CTrivisionLight::TurnOff(UINT nController, UINT nChannel)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
	//TrivisionLightDll::SetBrightness(m_nPort, nChannel, 0);	
}

void CTrivisionLight::CheckLight(int nControllerCount)
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	TrivisionLightDll::GetError(m_nPort);
}

void CTrivisionLight::GetBrightness()
{
	if (!m_bIsPortOpen)
	{
		theApp.WriteLog(_T("MultiTurnOn Fail. Port is not open."));
		return;
	}

	// nController �� ��� ����.. Ȯ�� �ʿ�. Todo..
	TrivisionLightDll::GetBrightness(0);	
}



BOOL CTrivisionLight::InitializeLight(CString strInitFilePath)
{
	int nRet = APP_OK;

	/*	���� �ʿ䰡 ���� �Ͱ��Ƽ� ����... yskim
	TCHAR szSimulationFlag[6] = {0,};
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);
	if( !_tcscmp(szSimulationFlag, _T("TRUE")) )
		return nRet;
	*/

 	m_nPort				= GetPrivateProfileInt(_T("Settings"),_T("PORT"), 2, strInitFilePath);	
 	m_nBaudRate			= GetPrivateProfileInt(_T("Settings"),_T("BAUDRATE"), 38400, strInitFilePath);
	m_nControllerCnt	= GetPrivateProfileInt(_T("Settings"),_T("CONTROLLER_COUNT "), 4, strInitFilePath);		// ��Ʈ�ѷ� �� ����
	m_nChannelTotalCnt	= GetPrivateProfileInt(_T("Settings"),_T("CHANNEL_COUNT "), 16, strInitFilePath);		// ��� ��Ʈ�ѷ��� �� ä�� ����

	TCHAR szOffset[6] = { 0, };
	GetPrivateProfileString(_T("Settings"), _T("OFFSET"), _T("1.0"), szOffset, sizeof(szOffset), strInitFilePath);
	m_offSet = _ttof(szOffset);

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
	
	// ���� TrivisionLight ���� �Ѱ��� Ÿ�� ���(MODULE_4CH)
	if(nMaxChCnt == MODULE_4CH)			m_nChannelType = MODULE_4CH;

	if (m_bIsPortOpen == TRUE)
	{
		nRet = TrivisionLightDll::ClosePort(m_nPort);
		if (nRet != APP_OK)
		{
			theApp.WriteLog(_T("Port Close Fail"));
			return FALSE;
		}

		m_bIsPortOpen = FALSE;
	}

	switch(TrivisionLightDll::OpenPort(m_nPort, m_nControllerCnt, m_nBaudRate, m_nCtrlChannelCnt, CallBackFunc))
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

UINT CTrivisionLight::ThreadCheckLight(LPVOID lParam)
{ 
	//theApp.m_pInternalServer->m_fnCmdEditSend()
	CTrivisionLight* pThis = (CTrivisionLight*)lParam;
	int nControllerNum = 0;
	while(1)
	{
		//for(int i = 0 ; i < pThis->GetControllerCount() ; i++)
		//{			
		//	Sleep(500);
		//	nControllerNum = i + 1;
		//	//18.10.18 Light Task ���� Count�� .ini ���Ͽ��� �ҷ������� ����
		//	theApp.m_pInternalServer->m_fnCmdEditSend(VS_SEND_CHECK_LIGHT, 0, (USHORT)sizeof(int), theApp.GetVsTaskNum(), (BYTE*)&nControllerNum, CMD_TYPE_NORES);		
		//}
		theApp.m_pInternalServer->m_fnCmdEditSend(VS_SEND_CHECK_LIGHT, 0, (USHORT)sizeof(int), theApp.GetVsTaskNum(), (BYTE*)&nControllerNum, CMD_TYPE_NORES);
		Sleep(10000);
	}
	return 0;
}

void CTrivisionLight::CallBackFunc_FromUnit(int nPort,char *pBuff)
{
	CString strTemp;
	CString strOrg=_T("");
	char* token = NULL;

	if (pBuff == NULL)
	{
		theApp.WriteLog(_T("Call Back data is NULL."));
		return;
	}

	PROTOCOL_INFO brightInfo;
	memcpy(&brightInfo,(char *)pBuff,sizeof(PROTOCOL_INFO));

	string rcvMsg(brightInfo.strcmd);	// ������ �޽���
	rcvMsg = rcvMsg.substr(1, rcvMsg.size() - 2);

	smatch match;
	static regex regMode("MODEW(..)");	// MODE ���� ���� �޽����� ����.. (ex, ���� �޽����� MODEWOK or MODEWNG)
	static regex regBrightness("CALLS(..)");	// ��ü ä�� ��� ���� ���� �޽����� ����..
	static regex regOnoff("CHONS(..)");	// ��ü ä�� �¿��� ���� ���� �޽����� ����..
	static regex regReadLimitBrightness("CLMTR([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})([0-9]{4})");	// ��ü ä�� ��� ���Ѱ� �б� ���� �޽����� ����..
	static regex regReadOffset("COFSR([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})([-0-9]{4})");	// ��ü ä�� �����°� �б� ���� �޽����� ����..
	static regex regReadLimitTemperature("TPALR([-0-9]{3})([-0-9]{3})([-0-9]{3})([-0-9]{3})");	// �µ� ���Ѱ� �б� ���� �޽����� ����..
	static regex regReadCurrentTemperature("TPCRR([-0-9]{3})([-0-9]{3})([-0-9]{3})([-0-9]{3})");	// ���� �µ� ���� �޽����� ����..
	static regex regAlarmState("OPSTA(..)");	// �˶� ���� ���� �޽����� ����..

	if (std::regex_search(rcvMsg, match, regMode))
	{
		CString retPacket(rcvMsg.c_str());
		CString retValue(match[1].str().c_str());
		theApp.WriteLog(_T("Change mode %s (RX Packet : %s)"), retValue, retPacket);
	}
	else if (std::regex_search(rcvMsg, match, regBrightness))
	{
		CString retPacket(rcvMsg.c_str());
		CString retValue(match[1].str().c_str());
		theApp.WriteLog(_T("Set Brightness %s (RX Packet : %s)"), retValue, retPacket);
	}
	else if (std::regex_search(rcvMsg, match, regOnoff))
	{
		CString retPacket(rcvMsg.c_str());
		CString retValue(match[1].str().c_str());
		theApp.WriteLog(_T("Set OnOff %s (RX Packet : %s)"), retValue, retPacket);
	}
	else if (std::regex_search(rcvMsg, match, regReadLimitBrightness))
	{
		CString retPacket(rcvMsg.c_str());
		CString retValue;
		for (int i = 1; i < match.size(); i++)
		{
			CString temp(match[i].str().c_str());
			retValue.AppendFormat(_T(" %d"), _ttoi(temp));
		}
		theApp.WriteLog(_T("Limit Brightness :%s (RX Packet : %s)"), retValue, retPacket);
	}
	else if (std::regex_search(rcvMsg, match, regReadOffset))
	{
		CString retPacket(rcvMsg.c_str());
		CString retValue;
		for (int i = 1; i < match.size(); i++)
		{
			CString temp(match[i].str().c_str());
			retValue.AppendFormat(_T(" %d"), _ttoi(temp));
		}
		theApp.WriteLog(_T("Offset :%s (RX Packet : %s)"), retValue, retPacket);
	}
	else if (std::regex_search(rcvMsg, match, regReadLimitTemperature))
	{
		CString retPacket(rcvMsg.c_str());
		CString retValue;
		for (int i = 1; i < match.size(); i++)
		{
			CString temp(match[i].str().c_str());
			retValue.AppendFormat(_T(" %d"), _ttoi(temp));
		}
		theApp.WriteLog(_T("Limit Temperature :%s (RX Packet : %s)"), retValue, retPacket);
	}
	else if (std::regex_search(rcvMsg, match, regReadCurrentTemperature))
	{
		CString retPacket(rcvMsg.c_str());
		CString retValue;
		for (int i = 1; i < match.size(); i++)
		{
			CString temp(match[i].str().c_str());
			retValue.AppendFormat(_T(" %d"), _ttoi(temp));
		}
		theApp.WriteLog(_T("Current Temperature :%s (RX Packet : %s)"), retValue, retPacket);
	}
	else if (std::regex_search(rcvMsg, match, regAlarmState))
	{
		CString retPacket(rcvMsg.c_str());
		CString retValue(match[1].str().c_str());
		theApp.WriteLog(_T("Alarm State %s (RX Packet : %s)"), retValue, retPacket);
	}
	else
	{
		CString retPacket(rcvMsg.c_str());
		theApp.WriteLog(_T("Unknown Msg (RX Packet : %s)"), retPacket);
	}

	return ;
}