// SerialCtrl.cpp: implementation of the CXMVSerialCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SerialCtrl.h"
#include "CPUTimer.h"
#include "mil.h"
#include "VSCameraTask.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialCtrl::CSerialCtrl()
{
	m_ViewWorksComCtrl = new CVieworksComControl;

	// 카메라 중복 컨트롤 제거
	InitializeCriticalSection(&m_csCameraCtrl);
}

CSerialCtrl::~CSerialCtrl()
{
	if (m_ViewWorksComCtrl != NULL)
	{
		delete m_ViewWorksComCtrl;
		m_ViewWorksComCtrl = NULL;
	}
	// 카메라 중복 컨트롤 제거
	DeleteCriticalSection(&m_csCameraCtrl);
}

BOOL CSerialCtrl::ConnectCameraSerial(int ComPort, int nBaudrate, eCamModel eModel)
{
	CString PortName;

	// 16.03.11 Camera 통신 추가 by CWH
	switch(eModel) {
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		PortName.Format(_T("COM%d"), ComPort);

		if(!m_ViewWorksComCtrl->OpenComPort(nBaudrate,PortName))
		{
			ResetCamera(eModel);
		}
		else
		{
			AfxMessageBox(_T("Error : Open COM-Port"));
			return FALSE;
		}	
		break;

	default:
		break;
 	}
	return TRUE;
}

// 16.03.11 Camera 초기화
BOOL CSerialCtrl::ResetCamera(eCamModel eModel)
{
	BOOL bRet = FALSE;

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		//human : "rnp" 입력...  -> 스테이지 초기화.
		bRet = m_fnSendCommand<bool>("rnp") ? TRUE : FALSE;
		break;

	default:
		break;
	}
	return bRet;
}

BOOL CSerialCtrl::IsMultiShot(eCamModel eModel)
{
	BOOL bRet = FALSE;	
	CString retmsg, tempstr;

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
		//MultiShot Enable or Disable?
		bRet = m_fnSendCommand<bool>("gme", 1) ? TRUE : FALSE;
		break;

	default:
		bRet = -1;
		break;

	}
	return bRet;
}

///  Send serial command to camera
template<> int CSerialCtrl::m_fnSendCommand<int>(char* data, int nDigit)
{
	CString retmsg, tempstr;	
	int nLength = (int)strlen(data);
	int nResponse = 0;

	char* cCommand = new char[nLength+1];
	memset(cCommand, 0, nLength+1);
	memcpy(cCommand, data, nLength);

	EnterCriticalSection(&m_csCameraCtrl);

	int nRet = m_ViewWorksComCtrl->SetCommand(cCommand, 1000);

	if (nRet == 0 && nDigit > 0)
	{
		retmsg = m_ViewWorksComCtrl->m_strReturnMsg;
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

template<> double CSerialCtrl::m_fnSendCommand<double>(char* data, int nDigit)
{
	CString retmsg, tempstr;	
	int nLength = (int)strlen(data);
	double dResponse = 0;

	char* cCommand = new char[nLength+1];
	memset(cCommand, 0, nLength+1);
	memcpy(cCommand, data, nLength);

	EnterCriticalSection(&m_csCameraCtrl);

	int nRet = m_ViewWorksComCtrl->SetCommand(cCommand, 1000);

	if (nRet == 0 && nDigit > 0)
	{
		retmsg = m_ViewWorksComCtrl->m_strReturnMsg;
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

template<> bool CSerialCtrl::m_fnSendCommand<bool>(char* data, int nDigit)
{
	CString retmsg, tempstr;
	int nLength = (int)strlen(data);
	int nResponse = 0;
	bool bResponse = 0;

	char* cCommand = new char[nLength+1];
	memset(cCommand, 0, nLength+1);
	memcpy(cCommand, data, nLength);
	
	EnterCriticalSection(&m_csCameraCtrl);

	int nRet = m_ViewWorksComCtrl->SetCommand(cCommand, 1000);

	if (nRet == 0 && nDigit == 1)
	{
		retmsg = m_ViewWorksComCtrl->m_strReturnMsg;
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

BOOL CSerialCtrl::SetExpTime(eCamModel eModel, double dExpTime)
{
	BOOL bRet = FALSE;
	char strTemp[256];

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		sprintf_s(strTemp, "set %d", (int)(dExpTime * 1000));		// ms(밀리) -> us(마이크로)
		//str_temp.Format(_T("set %d"), nExpTime * 1000);
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		break;

	default:
		break;
	}

	return bRet;
}

// Camera Tab Stitch Mode 설정		 180809 YSS
BOOL CSerialCtrl::SetStitch(eCamModel eModel, int nMode)
{
	BOOL bRet = FALSE;
	char strTemp[256];

	switch (eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		sprintf_s(strTemp, "ssgs %d", nMode);		// ssgs 0~3 : Stitch 보정 영역 선택(0~3)
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		break;

	default:
		break;
	}

	return bRet;
}

BOOL CSerialCtrl::SetCameraSequence(eCamModel eModel, int nMode)
{
	BOOL bRet = FALSE;
	char strTemp[256];

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
		//str_temp.Format(_T("ssm %d"), nMode);
		sprintf_s(strTemp, "ssm %d", nMode);
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;

// 		bRet = ResetCamera(eModel);
		break;

	default:
		break;
	}

	return bRet;
}

BOOL CSerialCtrl::SetMultishotEnable(eCamModel eModel, BOOL bFlag)
{
	BOOL bRet = FALSE;

	switch(eModel)
	{
	case eVN_Series:
		if (bFlag == TRUE)
		{
			bRet = m_fnSendCommand<bool>("sme 1") ? TRUE : FALSE;
			bRet = m_fnSendCommand<bool>("ssr 1") && bRet ? TRUE : FALSE;
		}
		else
		{
			bRet = m_fnSendCommand<bool>("sme 0") ? TRUE : FALSE;
		}
		ResetCamera(eModel);
		break;		

	default:
		break;
	}

	return bRet;
}

// 2016.05.09 Camera Trigger Mode 설정
BOOL CSerialCtrl::SetCameraTriggerMode(eCamModel eModel, int nMode)		// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
{
	BOOL bRet = FALSE;	
	char strTemp[256];

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		//str_temp.Format(_T("stm %d"), nMode);
		sprintf_s(strTemp, "stm %d", nMode);
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		break;

	default:
		break;
	}

	return bRet;
}

// 2016.05.10 Camera Analog Gain 값 설정
BOOL CSerialCtrl::SetCameraAnalogGain(eCamModel eModel, double dMode)
{
	BOOL bRet = FALSE;
	char strTemp[256];
	//double dMode = 0.0;		// for VA47M5

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVA_Series:
		//str_temp.Format(_T("sag %d"), nMode);
		sprintf_s(strTemp, "sag %d", (int)dMode);
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		break;

	case eVH_Series:
		//str_temp.Format(_T("sag 1 %d"), nMode);
		sprintf_s(strTemp, "sag 1 %d", (int)dMode);
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		//str_temp.Format(_T("sag 2 %d"), nMode);
		sprintf_s(strTemp, "sag 2 %d", (int)dMode);
		bRet = m_fnSendCommand<bool>(strTemp) && bRet ? TRUE : FALSE;
		break;

	case eVA47M5_Series:
		//dMode = (((int)dMode + 1) / 300.0) + 1.0;		// VA47M5 Gain 허용 범위 : 1.0 ~ 4.0
		if (dMode < 1.0)	dMode = 1.0;
		if (dMode > 4.0)	dMode = 4.0;	
		sprintf_s(strTemp, "sg %.1f", dMode);		
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		break;

	default:
		break;
	}

	return bRet;
}

// 2016.05.09 Camera Data Bit 설정
BOOL CSerialCtrl::SetCameraDataBit(eCamModel eModel, int nDataBit)
{
	BOOL bRet = FALSE;
	char strTemp[256];

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		sprintf_s(strTemp, "sdb %d", nDataBit);
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		break;

	default:
		break;
	}


	return bRet;
}

// 2016.05.09 Camera Exposure Source 설정
BOOL CSerialCtrl::SetCameraExpSource(eCamModel eModel, int nMode)
{
	BOOL bRet = FALSE;
	char strTemp[256];

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		sprintf_s(strTemp, "ses %d", nMode);
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		break;

	default:
		break;
	}

	return bRet;
}

// 2016.05.09 Camera Trigger Source 설정
BOOL CSerialCtrl::SetCameraTrigSource(eCamModel eModel, int nMode)
{
	BOOL bRet = FALSE;
	char strTemp[256];

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		sprintf_s(strTemp, "sts %d", nMode);
		bRet = m_fnSendCommand<bool>(strTemp) ? TRUE : FALSE;
		break;

	default:
		break;
	}

	return bRet;
}


int CSerialCtrl::GetCameraSequence(eCamModel eModel)
{
	int nRet = 0;
	CString retmsg, tempstr;		

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
		//MultiShot Enable or Disable?
		nRet = m_fnSendCommand<int>("gsm", 1);
		break;

	case eVA47M5_Series:
		nRet = m_fnSendCommand<int>("gsm", 1);
		break;
	default:
		nRet = -1;
		break;

	}
	return nRet;
}

int CSerialCtrl::GetCameraTriggerMode(eCamModel eModel)
{
	int nRet = 0;
	CString retmsg, tempstr;		

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		// Camera Trigger Mode
		nRet = m_fnSendCommand<int>("gtm", 1);
		break;

	default:
		nRet = -1;
		break;

	}
	// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
	return nRet;
}

double CSerialCtrl::GetCameraAnalogGain(eCamModel eModel)
{
	BOOL bRet = FALSE;
	double nRet;
	CString retmsg, tempstr;		

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVA_Series:
		// Camera Gain
		nRet = m_fnSendCommand<int>("gag", 3);
		break;

	case eVH_Series:
		// Camera Gain
		// VH Camera Gain Channel 2개이므로 2번 Channel 값만 가져와서 표시 - 필요 시 Channel 분리 작업 필요
		nRet = m_fnSendCommand<int>("gag 2", 3);
		break;

	case eVA47M5_Series:	
		// Camera Gain
		nRet = /*(double)*/((m_fnSendCommand<double>("gg", 3)/* - 1*/)/* * 300 - 1*/);
		break;

	default:
		nRet = -1;
		break;

	}
	return nRet;
}

double CSerialCtrl::GetCameraExposureTime(eCamModel eModel)
{
	double dRet;
	CString retmsg, tempstr;		

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		// Camera Exposure Time
		dRet = m_fnSendCommand<double>("get", 7);
		break;

	default:
		dRet = -1.0;
		break;

	}
	return dRet;
}

int CSerialCtrl::GetCameraDataBit(eCamModel eModel)
{
	int nRet;
	CString retmsg, tempstr;		

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		// Camera Trigger Mode
		nRet = m_fnSendCommand<int>("gdb", 1);
		break;

	default:
		nRet = -1;
		break;

	}
	// DataBit : 8 or 10 or 12
	return nRet;
}

int CSerialCtrl::GetCameraExpSource(eCamModel eModel)
{
	int nRet = 0;			

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		nRet = m_fnSendCommand<int>("ges", 1);
		break;

	default:
		nRet = -1;
		break;

	}
	// Mode 0:Timed 1:PulseWidth
	return nRet;
}

int CSerialCtrl::GetCameraTrigSource(eCamModel eModel)
{
	int nRet = 0;			

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		nRet = m_fnSendCommand<int>("gts", 1);
		break;

	default:
		nRet = -1;
		break;

	}
	// Mode 1:CC1 2:PulseWidth
	return nRet;
}

bool CSerialCtrl::GetCameraFanStatus(eCamModel eModel)
{
	bool bRet = false;			

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		bRet = m_fnSendCommand<bool>("gft", 1);
		break;

	default:
		bRet = false;
		break;

	}
	// 0:Fan Off, 1:Fan On
	return bRet;
}

int CSerialCtrl::GetCameraFanRPM(eCamModel eModel)
{
	int nRet = 0;			

	switch(eModel)
	{
	//case eVN_Series:
	//case eVH_Series:
	case eVA47M5_Series:
		nRet = m_fnSendCommand<int>("gfr", 6);
		break;

	default:
		nRet = 0;
		break;

	}
	// Camera FAN RPM
	return nRet;
}

double CSerialCtrl::GetCameraTemperature(eCamModel eModel)
{
	double dRet = 0;

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		dRet = m_fnSendCommand<double>("gct", 6);	// xxx.xx (소수점 둘째자리까지 표현) 가정
		break;

	default:
		dRet = -1.0;
		break;

	}
	// Current Temperature
	return dRet;
}

double CSerialCtrl::GetCameraCCDTemperature(eCamModel eModel)
{
	double dRet = 0;

	switch(eModel)
	{
	case eVN_Series:
	case eVP_Series:
	case eVH_Series:
	case eVA_Series:
	case eVA47M5_Series:
		dRet = m_fnSendCommand<double>("gst", 6);	// xxx.xx (소수점 둘째자리까지 표현) 가정
		break;

	default:
		dRet = -1.0;
		break;

	}
	// Current Temperature
	return dRet;
}