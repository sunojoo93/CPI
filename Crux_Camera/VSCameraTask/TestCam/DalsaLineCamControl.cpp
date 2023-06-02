// SerialCtrl.cpp: implementation of the CXMVSerialCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CPUTimer.h"
#include "mil.h"
#include "VSCameraTask.h"
#include "DalsaLineCamControl.h"
#include "DalsaLineParam.hpp"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDalsaLineCamControl::CDalsaLineCamControl()
{
	m_CamSerial = new CDalsaLineSerial;

	// 카메라 중복 컨트롤 제거
	InitializeCriticalSection(&m_csCameraCtrl);
}

CDalsaLineCamControl::~CDalsaLineCamControl()
{
	if (m_CamSerial != NULL)
	{
		delete m_CamSerial;
		m_CamSerial = NULL;
	}
	// 카메라 중복 컨트롤 제거
	DeleteCriticalSection(&m_csCameraCtrl);
}

BOOL CDalsaLineCamControl::ConnectCameraSerial(int ComPort, int nBaudrate)
{
	CString PortName;
	PortName.Format(_T("COM%d"), ComPort);

	if(!m_CamSerial->OpenComPort(nBaudrate,PortName))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}	
}

// 16.03.11 Camera 초기화
BOOL CDalsaLineCamControl::ResetCamera(eCamModel eModel)
{
	BOOL bRet = FALSE;

	switch(eModel)
	{
	case eNUCLi7370AT6:
		bRet = m_fnSendCommand<bool>("sta") ? TRUE : FALSE;
		DalsaLineParam::Instance().SetParams(m_CamSerial->m_strReturnMsg);
		break;
	default:
		break;
	}
	return bRet;
}

BOOL CDalsaLineCamControl::IsMultiShot(eCamModel eModel)
{
	BOOL bRet = FALSE;	
	CString retmsg, tempstr;

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		bRet = -1;
		break;

	}
	return bRet;
}

///  Send serial command to camera
template<> int CDalsaLineCamControl::m_fnSendCommand<int>(char* data, int nDigit)
{
	CString retmsg, tempstr;	
	int nLength = (int)strlen(data);
	int nResponse = 0;

	char* cCommand = new char[nLength+1];
	memset(cCommand, 0, nLength+1);
	memcpy(cCommand, data, nLength);

	EnterCriticalSection(&m_csCameraCtrl);

	int nRet = m_CamSerial->SetCommand(cCommand, 1000);

	if (nRet == 0 && nDigit > 0)
	{
		retmsg = m_CamSerial->m_strReturnMsg;
		tempstr = retmsg.Mid(nLength+2,max(nDigit, retmsg.GetLength()-(nLength+2)));
		nResponse = _ttoi(tempstr);
	}
	else
	{
		nResponse = -1;
	}

	LeaveCriticalSection(&m_csCameraCtrl);

	delete[] cCommand;

	return nResponse;
}

template<> double CDalsaLineCamControl::m_fnSendCommand<double>(char* data, int nDigit)
{
	CString retmsg, tempstr;	
	int nLength = (int)strlen(data);
	double dResponse = 0;

	char* cCommand = new char[nLength+1];
	memset(cCommand, 0, nLength+1);
	memcpy(cCommand, data, nLength);

	EnterCriticalSection(&m_csCameraCtrl);

	int nRet = m_CamSerial->SetCommand(cCommand, 1000);

	if (nRet == 0 && nDigit > 0)
	{
		retmsg = m_CamSerial->m_strReturnMsg;
		tempstr = retmsg.Mid(nLength+2,max(nDigit, retmsg.GetLength()-(nLength+2)));
		dResponse = _ttof(tempstr);
	}
	else
	{
		dResponse = -1.0;
	}

	LeaveCriticalSection(&m_csCameraCtrl);

	delete[] cCommand;

	return dResponse;
}

template<> bool CDalsaLineCamControl::m_fnSendCommand<bool>(char* data, int nDigit)
{
	CString retmsg, tempstr;
	int nLength = (int)strlen(data);
	int nResponse = 0;
	bool bResponse = 0;

	char* cCommand = new char[nLength+1];
	memset(cCommand, 0, nLength+1);
	memcpy(cCommand, data, nLength);
	
	EnterCriticalSection(&m_csCameraCtrl);

	int nRet = m_CamSerial->SetCommand(cCommand, 2000);

	if (nRet == 0 && nDigit == 1)
	{
		retmsg = m_CamSerial->m_strReturnMsg;
		tempstr = retmsg.Mid(nLength+2,1);
		nResponse = _ttoi(tempstr);

		nResponse == 1 ? bResponse = true : bResponse = false;
	}
	else
	{
		nRet == 0 ?	bResponse = true : bResponse = false;	
	}	

	LeaveCriticalSection(&m_csCameraCtrl);

	delete[] cCommand;

	return bResponse;
}

BOOL CDalsaLineCamControl::SetExpTime(eCamModel eModel, double dExpTime)
{
	BOOL bRet = FALSE;
	char strTemp[256];
	int exp = 0;

	switch(eModel)
	{
	case eNUCLi7370AT6:
		exp = dExpTime * 1000;	// ms -> us
		if (exp < 57) exp = 57;
		if (exp > 32767) exp = 32767;
		sprintf_s(strTemp, "i%d", exp);
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		break;

	default:
		break;
	}

	return bRet;
}

// Camera Tab Stitch Mode 설정		 180809 YSS
BOOL CDalsaLineCamControl::SetStitch(eCamModel eModel, int nMode)
{
	BOOL bRet = FALSE;

	switch (eModel)
	{
	case eNUCLi7370AT6:
	default:
		break;
	}

	return bRet;
}

BOOL CDalsaLineCamControl::SetCameraSequence(eCamModel eModel, int nMode)
{
	BOOL bRet = FALSE;

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		break;
	}

	return bRet;
}

BOOL CDalsaLineCamControl::SetMultishotEnable(eCamModel eModel, BOOL bFlag)
{
	BOOL bRet = FALSE;

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		break;
	}

	return bRet;
}

// 2016.05.09 Camera Trigger Mode 설정
BOOL CDalsaLineCamControl::SetCameraTriggerMode(eCamModel eModel, int nMode)		// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
{
	BOOL bRet = FALSE;	
	char strTemp[256];

	switch(eModel)
	{
	case eNUCLi7370AT6:
		sprintf_s(strTemp, "t%d", nMode);
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		break;

	default:
		break;
	}

	return bRet;
}

// 2016.05.10 Camera Analog Gain 값 설정
BOOL CDalsaLineCamControl::SetCameraAnalogGain(eCamModel eModel, double dMode)
{
	BOOL bRet = FALSE;
	//double dMode = 0.0;		// for VA47M5

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		break;
	}

	return bRet;
}

// 2016.05.09 Camera Data Bit 설정
BOOL CDalsaLineCamControl::SetCameraDataBit(eCamModel eModel, int nDataBit)
{
	BOOL bRet = FALSE;

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		break;
	}


	return bRet;
}

// 2016.05.09 Camera Exposure Source 설정
BOOL CDalsaLineCamControl::SetCameraExpSource(eCamModel eModel, int nMode)
{
	BOOL bRet = FALSE;

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		break;
	}

	return bRet;
}

// 2016.05.09 Camera Trigger Source 설정
BOOL CDalsaLineCamControl::SetCameraTrigSource(eCamModel eModel, int nMode)
{
	BOOL bRet = FALSE;

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		break;
	}

	return bRet;
}


int CDalsaLineCamControl::GetCameraSequence(eCamModel eModel)
{
	int nRet = 0;
	CString retmsg, tempstr;		

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		nRet = -1;
		break;

	}
	return nRet;
}

int CDalsaLineCamControl::GetCameraTriggerMode(eCamModel eModel)
{
	int nRet = 0;
	CString retmsg, tempstr;		

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		nRet = -1;
		break;

	}
	// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
	return nRet;
}

int CDalsaLineCamControl::GetCameraAnalogGain(eCamModel eModel)
{
	BOOL bRet = FALSE;
	int nRet;
	CString retmsg, tempstr;		

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		nRet = -1;
		break;

	}
	return nRet;
}

double CDalsaLineCamControl::GetCameraExposureTime(eCamModel eModel)
{
	double dRet;
	CString retmsg, tempstr;		

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		dRet = -1.0;
		break;

	}
	return dRet;
}

void CDalsaLineCamControl::GetAllProperty()
{
	char strTemp[256];
	sprintf_s(strTemp, "sta");
	bool bRet = m_fnSendCommand<bool>(strTemp) ? true : false;
	DalsaLineParam::Instance().SetParams(m_CamSerial->m_strReturnMsg);
	if (m_CamSerial->m_strReturnMsg.Find(_T("OK")) >= 0)
		theApp.SetCameraState(TRUE);
}

void CDalsaLineCamControl::LoadAllProperty()
{
	char strTemp[256];
	sprintf_s(strTemp, "l");
	bool bRet = m_fnSendCommand<bool>(strTemp) ? true : false;
	DalsaLineParam::Instance().SetParams(m_CamSerial->m_strReturnMsg);
	if (m_CamSerial->m_strReturnMsg.Find(_T("OK")) >= 0)
		theApp.SetCameraState(TRUE);
}

void CDalsaLineCamControl::SaveAllProperty()
{
	char strTemp[256];
	sprintf_s(strTemp, "w");
	bool bRet = m_fnSendCommand<bool>(strTemp) ? true : false;
	if (m_CamSerial->m_strReturnMsg.Find(_T("OK")) >= 0)
		theApp.SetCameraState(TRUE);
}

void CDalsaLineCamControl::SetProperty(char* cmd, int value)
{
	char strTemp[256];
	sprintf_s(strTemp, "%s %d", cmd, value);
	bool bRet = m_fnSendCommand<bool>(strTemp) ? true : false;
	if (m_CamSerial->m_strReturnMsg.Find(_T("OK")) >= 0)
		theApp.SetCameraState(TRUE);
}

int CDalsaLineCamControl::GetCameraDataBit(eCamModel eModel)
{
	int nRet;
	CString retmsg, tempstr;		

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		nRet = -1;
		break;

	}
	// DataBit : 8 or 10 or 12
	return nRet;
}

int CDalsaLineCamControl::GetCameraExpSource(eCamModel eModel)
{
	int nRet = 0;			

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		nRet = -1;
		break;

	}
	// Mode 0:Timed 1:PulseWidth
	return nRet;
}

int CDalsaLineCamControl::GetCameraTrigSource(eCamModel eModel)
{
	int nRet = 0;			

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		nRet = -1;
		break;

	}
	// Mode 1:CC1 2:PulseWidth
	return nRet;
}

bool CDalsaLineCamControl::GetCameraFanStatus(eCamModel eModel)
{
	bool bRet = false;			

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		bRet = false;
		break;

	}
	// 0:Fan Off, 1:Fan On
	return bRet;
}

int CDalsaLineCamControl::GetCameraFanRPM(eCamModel eModel)
{
	int nRet = 0;			

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		nRet = 0;
		break;

	}
	// Camera FAN RPM
	return nRet;
}

double CDalsaLineCamControl::GetCameraTemperature(eCamModel eModel)
{
	double dRet = 0;

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		dRet = -1.0;
		break;

	}
	// Current Temperature
	return dRet;
}

double CDalsaLineCamControl::GetCameraCCDTemperature(eCamModel eModel)
{
	double dRet = 0;

	switch(eModel)
	{
	case eNUCLi7370AT6:
	default:
		dRet = -1.0;
		break;

	}
	// Current Temperature
	return dRet;
}