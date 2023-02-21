// Protocol8CH.cpp: implementation of the Protocol8CH class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TrivisionLightController.h"
#include "stdlib.h"
#include "stdio.h"
#include "Rs232CommSTD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern void CallBackFunc(int nPort,char *pBuff);

TrivisionLightController::TrivisionLightController()
{ 
	m_CallBack	= NULL;
	m_hWnd		= NULL;
	m_pCommTrivisionLight= new CRs232CommSTD;
	memset(&m_stLightStatus,NULL,sizeof(m_stLightStatus));
}

TrivisionLightController::~TrivisionLightController()
{
	if(m_pCommTrivisionLight) delete ((CRs232CommSTD *)m_pCommTrivisionLight);
	m_pCommTrivisionLight= NULL;
	m_CallBack	= NULL;
	m_hWnd		= NULL;
}

void TrivisionLightController::SetOnOff(LIGHT_TrivisionLight_STATUS stLightInfo)
{
	bool bChanged = false;
	CString strOnOff = _T("");

	for (int i = 0; i < MAX_LIGHT_CHANNEL_COUNT; i++)
	{
		if (m_stLightStatus.bChannelOn[i] != stLightInfo.bChannelOn[i])
		{
			m_stLightStatus.bChannelOn[i] = stLightInfo.bChannelOn[i];
			bChanged = true;
		}
		strOnOff.Append(stLightInfo.bChannelOn[i]? _T("1") : _T("0"));
	}

	if (bChanged)
	{
		char pBuff[PROTOCOL_TrivisionLight_LENGTH];
		sprintf(pBuff,"setonex %d %s%c", GetCtrID(), strOnOff, '\r');
		((CRs232CommSTD *)m_pCommTrivisionLight)->WriteData((unsigned char *)pBuff,strlen(pBuff));
		Sleep(PROTOCOL_TrivisionLight_WAIT);
	}
}

void TrivisionLightController::GetOnOffAll()
{
	char pBuff[PROTOCOL_TrivisionLight_LENGTH];
	sprintf(pBuff,"getonex %d%c", GetCtrID(),'\r');
	((CRs232CommSTD *)m_pCommTrivisionLight)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_TrivisionLight_WAIT);
}

void TrivisionLightController::GetError()
{
	char pBuff[PROTOCOL_TrivisionLight_LENGTH];
	sprintf(pBuff,"geterror%c",'\r');
	((CRs232CommSTD *)m_pCommTrivisionLight)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_TrivisionLight_WAIT);
}

void TrivisionLightController::SetBrightness(LIGHT_TrivisionLight_STATUS stLightInfo)
{
	bool bChanged = false;
	CString strBrightness = _T(" ");
	CString strTemp = _T("");

	strBrightness.Format(_T("setbrightness %d 999"), GetCtrID());
	for (int i = 0; i < MAX_LIGHT_CHANNEL_COUNT ; i++)
	{
		if (m_stLightStatus.nBrightness[i] != stLightInfo.nBrightness[i])
		{
			m_stLightStatus.nBrightness[i] = stLightInfo.nBrightness[i];
			bChanged = true;
		}

		strTemp.Format(_T(" %d"), stLightInfo.nBrightness[i]);
		strBrightness.Append(strTemp);
	}
	strBrightness.AppendChar('\r');

	if (bChanged)
	{
		char pBuff[PROTOCOL_TrivisionLight_LENGTH];
		strcpy_s(pBuff, PROTOCOL_TrivisionLight_LENGTH, CW2A(strBrightness));
		//sprintf(pBuff,"setbrightness 1 %s%c", &strBrightness, '\r');
		((CRs232CommSTD *)m_pCommTrivisionLight)->WriteData((unsigned char *)pBuff,strlen(pBuff));
		Sleep(PROTOCOL_TrivisionLight_WAIT);
	}
}

void TrivisionLightController::GetBrightnessAll()
{
	char pBuff[PROTOCOL_TrivisionLight_LENGTH];
	sprintf(pBuff,"getbrightness %d 0%c", GetCtrID(), '\r');
	((CRs232CommSTD *)m_pCommTrivisionLight)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_TrivisionLight_WAIT);
}

void TrivisionLightController::GetCurrentAll()
{
	char pBuff[PROTOCOL_TrivisionLight_LENGTH];
	sprintf(pBuff,"getc %d 999%c", GetCtrID(), '\r');
	((CRs232CommSTD *)m_pCommTrivisionLight)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_TrivisionLight_WAIT);
}

void TrivisionLightController::GetTemperatureAll()
{
	char pBuff[PROTOCOL_TrivisionLight_LENGTH];
	sprintf(pBuff,"gett %d 999%c",GetCtrID(), '\r');
	((CRs232CommSTD *)m_pCommTrivisionLight)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_TrivisionLight_WAIT);
}

int TrivisionLightController::Create(int nPort, int nBaudRate)
{
	int ret;
	char buff[10];
	sprintf(buff,"COM%d",nPort);
	
	ret = ((CRs232CommSTD *)m_pCommTrivisionLight)->Create(buff,nBaudRate,nPort,false,CallBackFunc);
	return ret;
}

int TrivisionLightController::Create(int nPort,int nBaudRate,void (*callback)(int nPort,char *pBuff))
{
	int ret;
	char buff[10];
	sprintf(buff,"COM%d",nPort);

	ret = ((CRs232CommSTD *)m_pCommTrivisionLight)->Create(buff,nBaudRate,nPort,false,CallBackFunc);
	if(ret)
		m_CallBack = callback;

	return ret;
}

int TrivisionLightController::Create(int nPort,int nBaudRate,HWND hWnd)
{
	int ret;
	char buff[10];
	sprintf(buff,"COM%d",nPort);

	ret = ((CRs232CommSTD *)m_pCommTrivisionLight)->Create(buff,nBaudRate,nPort,false,CallBackFunc);
	return ret;
}
