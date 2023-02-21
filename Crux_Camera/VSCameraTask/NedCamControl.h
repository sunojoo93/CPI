// SerialCtrl.h: interface for the CXMVSerialCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SerialCtrl_H__961E3893_9011_4043_9607_9264629F3242__INCLUDED_)
#define AFX_SerialCtrl_H__961E3893_9011_4043_9607_9264629F3242__INCLUDED_

//#include "Serial.h"
#include "define.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//enum eCamModel
//{
//	eVIEWWORKS_VN29 = 0,
//	eVIEWWORKS_VH16
//};

// 16.03.11 Camera Comm 추가 by CWH
#include "NedSerial.h"
#include "NedParam.hpp"

template<class T>	struct returnType			{ typedef T type; };
template<>			struct returnType<int>		{ typedef int type; };
template<>			struct returnType<double>	{ typedef double type; };
template<>			struct returnType<bool>		{ typedef bool type; };

class CNedCamControl
{
public:
	CNedCamControl();
	~CNedCamControl();

	BOOL ConnectCameraSerial(int ComPort, int nBaudrate);
	// 16.03.11 Vieworks Camera Control 추가 by CWH
	CNedSerial*		m_CamSerial;
	// Interface 함수
	BOOL	ResetCamera(eCamModel nModel);

	BOOL	SetExpTime(eCamModel nModel, double nExpTime);
	BOOL	SetStitch(eCamModel nModel, int nMode);				// 180809 YSS
	BOOL	SetCameraSequence(eCamModel nModel, int nMode);
	BOOL	SetMultishotEnable(eCamModel nModel, BOOL bFlag);
	BOOL	SetCameraTriggerMode(eCamModel nModel, int nMode);
	BOOL	SetCameraAnalogGain(eCamModel nModel, double dMode);
	BOOL	SetCameraDataBit(eCamModel nModel, int nDataBit);
	BOOL	SetCameraExpSource(eCamModel nModel, int nMode);
	BOOL	SetCameraTrigSource(eCamModel nModel, int nMode);

	BOOL	IsMultiShot(eCamModel nModel);
	int		GetCameraSequence(eCamModel nModel);
	int		GetCameraTriggerMode(eCamModel nModel);
	int		GetCameraAnalogGain(eCamModel nModel);
	double	GetCameraExposureTime(eCamModel nModel);
	int		GetCameraDataBit(eCamModel nModel);
	int		GetCameraExpSource(eCamModel nModel);
	int		GetCameraTrigSource(eCamModel nModel);
	// Camera 상태 모니터링 기능 추가
	bool	GetCameraFanStatus(eCamModel eModel);
	int		GetCameraFanRPM(eCamModel eModel);
	double	GetCameraTemperature(eCamModel eModel);
	double	GetCameraCCDTemperature(eCamModel eModel);
	void	GetAllProperty();
	void	LoadAllProperty();
	void	SaveAllProperty();
	void	SetProperty(char* cmd, int value);

private:
	CRITICAL_SECTION	m_csCameraCtrl;			// 카메라 중복 접근 제거

	// 리턴값 오버라이딩
	//int		m_fnSendCommand(char* data, int nDigit = 0);
	template<class T> typename returnType<T>::type m_fnSendCommand(char* data, int nDigit = 0);
	template<> int m_fnSendCommand<int>(char* data, int nDigit);
	template<> double m_fnSendCommand<double>(char* data, int nDigit);
	template<> bool m_fnSendCommand<bool>(char* data, int nDigit);
};

#endif // !defined(AFX_XMVSerialCtrl_H__961E3893_9011_4043_9607_9264629F3242__INCLUDED_)
