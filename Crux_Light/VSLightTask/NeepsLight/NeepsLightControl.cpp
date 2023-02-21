// Make By jskim

#include "stdafx.h"
#include "NeepsLightControl.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define MAX_BUFF_SIZE					256

#define NIPS_CHANNEL6		6
#define NIPS_CHANNEL10		10
#define NIPS_CHANNEL12		12
#define NIPS_CHANNEL16		16

enum E_LIGHT_CH
{
	E_6CH,
	E_10CH,
	E_12CH,
	E_16CH,
	E_CH_NUMBER
};

BYTE auchCRCHi[] = 
{
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};

BYTE auchCRCLo[] = 
{
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};

#define PTYPE_REQUEST				0x00		// Request Packet
#define PTYPE_RESPONSE				0x01		// Response Packet
#define PTYPE_TRANSMIT				0x02		// Only Transmit (No Require Response) 응답이 필요 없을때 설정

#define PID_SET_CHANNEL				0x01
#define PID_SET_CHANNEL_ALL			0x02
#define PID_GET_STATUS				0x03
#define PID_ERROR_RESET				0x04
#define PID_SET_TEMP_ERROR			0x05

#define PKSTATUS_UNUSED				0x00
#define PKSTATUS_OK					0x01
#define PKSTATUS_INVALID_SIZE		0x02
#define PKSTATUS_INVALID_PID		0x03
#define PKSTATUS_INVALID_INPUT		0x04
#define PKSTATUS_UNKNOWN_ERROR		0x05

#define EC_FAN_ERROR				0x0001
#define EC_TEMP_ERROR				0x0002

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	BYTE					TID;						// Terminal ID
	BYTE					PType;						// Protocol Type
	BYTE					PID;						// Protocol ID
	BYTE					Status;						// Process Result
	WORD					Len;						// Packet Len
}__SHeader;

typedef struct
{
	BYTE					CrcL;
	BYTE					CrcH;
}__SCrc;

typedef struct
{
	__SHeader				SHeader;
	BYTE					Channel;					// PWM Channel
	WORD					Duty;						// PWM Duty (0 ~ 255)
	__SCrc					SCrc;
} __SNLC_M12SetChannelReq;

typedef struct
{
	__SHeader				SHeader;
//	WORD					Duty[12];					// PWM Duty (0 ~ 255)
	WORD					Duty[MAX_LIGHT_CHANNEL_COUNT];					// PWM Duty (0 ~ 255)
	__SCrc					SCrc;
} __SNLC_M12SetChannelAllReq;

typedef struct
{
	__SHeader				SHeader;
	__SCrc					SCrc;
} __SNLC_M12GetStatusReq;

typedef struct
{
	__SHeader				SHeader;
	__SCrc					SCrc;
} __SNLC_M12ErrorResetReq;

typedef struct
{
	__SHeader				SHeader;
	WORD					TempValue;					// Set Error Temperature Value
	__SCrc					SCrc;
} __SNLC_M12SetTempErrorReq;

typedef struct
{
	__SHeader				SHeader;
	__SCrc					SCrc;
} __SNLC_M12SetChannelResp;

typedef struct
{
	__SHeader				SHeader;
	__SCrc					SCrc;
} __SNLC_M12SetChannelAllResp;

typedef struct
{
	__SHeader				SHeader;
	BYTE					Auto;						// Menu : 0, Auto : 1
	WORD					Duty[12];					// PWM Duty (0 ~ 1024)
	WORD					LEDStatus;					// LED On/Off Status
	WORD					Temperature[2];				// LED Temperature
	WORD					SetErrorTemp;				// Error Temperature setting value
	WORD					CM;
	BYTE					Error;
	__SCrc					SCrc;
} __SNLC_M12GetStatusResp;

typedef struct
{
	__SHeader				SHeader;
	__SCrc					SCrc;
} __SNLC_M12ErrorResetResp;

typedef struct
{
	__SHeader				SHeader;
	__SCrc					SCrc;
} __SNLC_M12SetTempErrorResp;




typedef struct
{
	__SHeader				SHeader;
	WORD					Duty[NIPS_CHANNEL6];					// PWM Duty (0 ~ 255)
	__SCrc					SCrc;
} __SLPSetChannel6Req;

typedef struct
{
	__SHeader				SHeader;
	WORD					Duty[NIPS_CHANNEL10];					// PWM Duty (0 ~ 255)
	__SCrc					SCrc;
} __SLPSetChannel10Req;

typedef struct
{
	__SHeader				SHeader;
	WORD					Duty[NIPS_CHANNEL12];					// PWM Duty (0 ~ 255)
	__SCrc					SCrc;
} __SLPSetChannel12Req;

typedef struct
{
	__SHeader				SHeader;
	WORD					Duty[NIPS_CHANNEL16];					// PWM Duty (0 ~ 255)
	__SCrc					SCrc;
} __SLPSetChannel16Req;

#pragma pack(pop)


CNeepsLight::CNeepsLight()
	//: CDialog(CNeepsLight::IDD)
	: m_nPort(0), m_nBaudRate(0), m_nControllerCnt(0)
{
}


CNeepsLight::~CNeepsLight()
{	
}

BOOL CNeepsLight::ConnectLight(int nPortNumber, int nBaudRate)
{
	//if(this->m_Connected )		return TRUE;
	//strPortName.Format(_T("\\\\.\\COM%d"), nPortNumber);
	m_SPort.Open(nPortNumber, DWORD(nBaudRate));
	
	if(!m_SPort.IsOpen()) 
	{
		//Message PanelID,ErrorID,ErrorLevel,ErrorInfo
 		CString strMsg;
 		strMsg.Format(_T("Light Com Port disconnection!! Please Check The Cable or Board (COM%d)"), nPortNumber);
 		AfxMessageBox(strMsg);
		return FALSE;
	}
 	else	m_SPort.m_Connected = TRUE;

	CString				Str;
	Str.Format(_T("%d"), nPortNumber);
	WriteProfileString(_T("NEEPS"), _T("SPort"), Str);
	m_SPort.Set0ReadTimeout();
	Sleep(300);
	return TRUE;	
}

WORD CNeepsLight::GetCrc(BYTE *pBuff, WORD nByte)
{
	BYTE					uchCRCHi = 0xFF;
	BYTE					uchCRCLo = 0xFF;
	WORD					nIndex;

	while (nByte--)
	{
		nIndex = uchCRCLo ^ *pBuff++;
		uchCRCLo = uchCRCHi ^ auchCRCHi[nIndex];
		uchCRCHi = auchCRCLo[nIndex];
	}

	return (uchCRCHi << 8 | uchCRCLo);
}

WORD CNeepsLight::ConvertWord(WORD nValue)
{
	WORD					nResult = ((nValue >> 8) & 0x00FF) | ((nValue << 8) & 0xFF00);

	return nResult;
}

void CNeepsLight::SetPWMChannel(BYTE Terminal, BYTE Channel, WORD Duty)
{
	//EnterCriticalSection(&m_cs);
	//if (m_hThreadWatchLight != NULL) m_hThreadWatchLight->SuspendThread();
	if ( !m_SPort.IsOpen() )
	{
	//	if (m_hThreadWatchLight != NULL) m_hThreadWatchLight->ResumeThread();
	//	LeaveCriticalSection(&m_cs);
		return;
	}	
	__SNLC_M12SetChannelReq		SNLC_M12SetChannelReq;
	WORD					nCrc;

	SNLC_M12SetChannelReq.SHeader.TID			= Terminal;
	SNLC_M12SetChannelReq.SHeader.PType			= PTYPE_REQUEST | PTYPE_TRANSMIT;		// PTYPE_TRANSMIT을 설정했으므로 응답이 없음
	SNLC_M12SetChannelReq.SHeader.PID			= PID_SET_CHANNEL;
	SNLC_M12SetChannelReq.SHeader.Status		= PKSTATUS_OK;
	SNLC_M12SetChannelReq.SHeader.Len			= ConvertWord((WORD)sizeof(__SNLC_M12SetChannelReq));

	SNLC_M12SetChannelReq.Channel = Channel;
	SNLC_M12SetChannelReq.Duty = ConvertWord(Duty);
	nCrc = GetCrc((BYTE *)&SNLC_M12SetChannelReq, sizeof(__SNLC_M12SetChannelReq) - sizeof(__SCrc));
	SNLC_M12SetChannelReq.SCrc.CrcL = nCrc & 0xFF;
	SNLC_M12SetChannelReq.SCrc.CrcH = (nCrc >> 8) & 0xFF;

	//WriteComm((UCHAR *)(LPCTSTR)&SNLC_M12SetChannelReq, sizeof(__SNLC_M12SetChannelReq));
	m_SPort.Write((UCHAR *)(LPCTSTR)&SNLC_M12SetChannelReq, sizeof(__SNLC_M12SetChannelReq));
//	if (m_hThreadWatchLight != NULL) m_hThreadWatchLight->ResumeThread();
//	LeaveCriticalSection(&m_cs);
}
 
void CNeepsLight::SetPWMAll(BYTE Terminal, STRU_SERIAL_INFO* stLight)
{
	if ( !m_SPort.IsOpen() )
	{
		return;
	}
	__SNLC_M12SetChannelAllReq	SNLC_M12SetChannelAllReq;
	WORD					nCrc;
	int	nIntensityValue = 0;

	SNLC_M12SetChannelAllReq.SHeader.TID		= Terminal+1;
	SNLC_M12SetChannelAllReq.SHeader.PType		= PTYPE_REQUEST | PTYPE_TRANSMIT;
	SNLC_M12SetChannelAllReq.SHeader.PID		= PID_SET_CHANNEL_ALL;
	SNLC_M12SetChannelAllReq.SHeader.Status		= PKSTATUS_OK;
	SNLC_M12SetChannelAllReq.SHeader.Len		= ConvertWord((WORD)sizeof(__SNLC_M12SetChannelAllReq));

	if (stLight != NULL)
	{
		//CString msg = _T("[Light]Set Value : ");
		for (UINT i=0; i<MAX_LIGHT_CHANNEL_COUNT; i++ )
		{
			nIntensityValue = stLight->nLightVal[i];
			if (i>=stLight->nChCnt){
				SNLC_M12SetChannelAllReq.Duty[i] =0;
			}else{
				SNLC_M12SetChannelAllReq.Duty[i] = ConvertWord(nIntensityValue);
			}
		}
	}
	else
	{
		for ( int i = 0; i < MAX_LIGHT_CHANNEL_COUNT; i++ )
			SNLC_M12SetChannelAllReq.Duty[i] = 0;
	}

	nCrc = GetCrc((BYTE *)&SNLC_M12SetChannelAllReq, sizeof(__SNLC_M12SetChannelAllReq) - sizeof(__SCrc));
	SNLC_M12SetChannelAllReq.SCrc.CrcL = nCrc & 0xFF;
	SNLC_M12SetChannelAllReq.SCrc.CrcH = (nCrc >> 8) & 0xFF;

	m_SPort.Write((UCHAR *)(LPCTSTR)&SNLC_M12SetChannelAllReq, sizeof(__SNLC_M12SetChannelAllReq));
}

void CNeepsLight::SetPWMAll_Manual(BYTE Terminal, int nValue, int nCh )
{
	if ( !m_SPort.IsOpen() )
		return;
	int* nDuty = new int[nCh];


	for ( int i = 0; i < nCh; i++ )
	{
		nDuty[i] = nValue;
// 		if (i>=pSystem->m_Config.m_nSettingLightChannelCnt) nDuty[i] =0;
	}
	SetChannel16(Terminal, nDuty, nCh);

	delete[] nDuty;
}

// Turn On
void CNeepsLight::MultiTurnOn(STRU_LIGHT_INFO stLightInfo)
{
	UINT i = 0;
	// 17.06.15 UI 에서 보내는 Count 와 상관없이 모든 Controller 를 설정된 값으로 켜도록 수정
	//for (i=0; i<stLightInfo.nControllerCount; i++)
	for (i=0; i<m_nControllerCnt; i++)
		SetPWMAll(i, &stLightInfo.stLight[i]);

}

void CNeepsLight::TurnOn(UINT nController, UINT nChannel, UINT nIntensity)
{
	SetPWMChannel(nController, nChannel,  nIntensity);
}

// Turn Off
void CNeepsLight::MultiTurnOff()
{
	for(int nCtrlNum=0; nCtrlNum<MAX_LIGHT_CONTROLLER_COUNT; nCtrlNum++)
	{
		SetPWMAll(nCtrlNum, NULL);
	}
}

void CNeepsLight::TurnOff(UINT nController, UINT nChannel)
{
	SetPWMChannel(nController, nChannel,  0);
}


void CNeepsLight::ErrorReset(BYTE Terminal)
{
	if ( !m_SPort.IsOpen() )
	{
		return;
	}

	__SNLC_M12ErrorResetReq		SNLC_M12ErrorResetReq;
	WORD						Crc;

	SNLC_M12ErrorResetReq.SHeader.TID			= Terminal;
	SNLC_M12ErrorResetReq.SHeader.PType			= PTYPE_REQUEST | PTYPE_TRANSMIT;
	SNLC_M12ErrorResetReq.SHeader.PID			= PID_ERROR_RESET;
	SNLC_M12ErrorResetReq.SHeader.Status		= PKSTATUS_OK;
	SNLC_M12ErrorResetReq.SHeader.Len			= ConvertWord((WORD)sizeof(__SNLC_M12ErrorResetReq));

	Crc = GetCrc((BYTE *)&SNLC_M12ErrorResetReq, sizeof(__SNLC_M12ErrorResetReq) - sizeof(__SCrc));
	SNLC_M12ErrorResetReq.SCrc.CrcL = Crc & 0xFF;
	SNLC_M12ErrorResetReq.SCrc.CrcH = (Crc >> 8) & 0xFF;

	//WriteComm((UCHAR *)(LPCTSTR)&SNLC_M12ErrorResetReq, sizeof(__SNLC_M12ErrorResetReq));
	m_SPort.Write((UCHAR *)(LPCTSTR)&SNLC_M12ErrorResetReq, sizeof(__SNLC_M12ErrorResetReq));
}

void CNeepsLight::SetChannel16(BYTE Terminal, int* Duty, int nCh)
{
	if (!m_SPort.IsOpen() )
	{
		return;
	}

	__SHeader	stHeader;

	WORD		nCrc;

	switch(nCh)
	{
	case NIPS_CHANNEL6:
		stHeader.Len = ConvertWord((WORD)sizeof(__SLPSetChannel6Req));
		break;
	case NIPS_CHANNEL10:
		stHeader.Len = ConvertWord((WORD)sizeof(__SLPSetChannel10Req));
		break;
	case NIPS_CHANNEL12:
		stHeader.Len = ConvertWord((WORD)sizeof(__SLPSetChannel12Req));
		break;
	case NIPS_CHANNEL16:
		stHeader.Len = ConvertWord((WORD)sizeof(__SLPSetChannel16Req));
		break;
	default:
		return;
	}

	stHeader.TID	= Terminal;
	stHeader.PType	= PTYPE_REQUEST | PTYPE_TRANSMIT;
	stHeader.PID	= PID_SET_CHANNEL_ALL;
	stHeader.Status	= PKSTATUS_OK;

	switch(nCh)
	{
	case NIPS_CHANNEL6:
		{
			__SLPSetChannel6Req		SLPSetChannelAllReq;

			for ( int i = 0; i < NIPS_CHANNEL6; i++ ) 
				SLPSetChannelAllReq.Duty[i] = ConvertWord(Duty[i]);

			SLPSetChannelAllReq.SHeader = stHeader;
			nCrc = GetCrc((BYTE *) & SLPSetChannelAllReq, sizeof(__SLPSetChannel6Req) - sizeof(__SCrc));
			SLPSetChannelAllReq.SCrc.CrcL = nCrc & 0xFF;
			SLPSetChannelAllReq.SCrc.CrcH = (nCrc >> 8) & 0xFF;
			m_SPort.Write((UCHAR *)(LPCTSTR) & SLPSetChannelAllReq, sizeof(__SLPSetChannel6Req));
		}
		break;
	case NIPS_CHANNEL10:
		{
			__SLPSetChannel10Req		SLPSetChannelAllReq;

			for ( int i = 0; i < NIPS_CHANNEL10; i++ ) 
				SLPSetChannelAllReq.Duty[i] = ConvertWord(Duty[i]);

			SLPSetChannelAllReq.SHeader = stHeader;
			nCrc = GetCrc((BYTE *) & SLPSetChannelAllReq, sizeof(__SLPSetChannel10Req) - sizeof(__SCrc));
			SLPSetChannelAllReq.SCrc.CrcL = nCrc & 0xFF;
			SLPSetChannelAllReq.SCrc.CrcH = (nCrc >> 8) & 0xFF;
			m_SPort.Write((UCHAR *)(LPCTSTR) & SLPSetChannelAllReq, sizeof(__SLPSetChannel10Req));
		}
		break;
	case NIPS_CHANNEL12:
		{
			__SLPSetChannel12Req		SLPSetChannelAllReq;

			for ( int i = 0; i < NIPS_CHANNEL12; i++ ) 
				SLPSetChannelAllReq.Duty[i] = ConvertWord(Duty[i]);

			SLPSetChannelAllReq.SHeader = stHeader;
			nCrc = GetCrc((BYTE *) & SLPSetChannelAllReq, sizeof(__SLPSetChannel12Req) - sizeof(__SCrc));
			SLPSetChannelAllReq.SCrc.CrcL = nCrc & 0xFF;
			SLPSetChannelAllReq.SCrc.CrcH = (nCrc >> 8) & 0xFF;
			m_SPort.Write((UCHAR *)(LPCTSTR) & SLPSetChannelAllReq, sizeof(__SLPSetChannel12Req));
		}
		break;
	case NIPS_CHANNEL16:
		{
			__SLPSetChannel16Req		SLPSetChannelAllReq;

			for ( int i = 0; i < NIPS_CHANNEL16; i++ ) 
				SLPSetChannelAllReq.Duty[i] = ConvertWord(Duty[i]);

			SLPSetChannelAllReq.SHeader = stHeader;
			nCrc = GetCrc((BYTE *) & SLPSetChannelAllReq, sizeof(__SLPSetChannel16Req) - sizeof(__SCrc));
			SLPSetChannelAllReq.SCrc.CrcL = nCrc & 0xFF;
			SLPSetChannelAllReq.SCrc.CrcH = (nCrc >> 8) & 0xFF;
			m_SPort.Write((UCHAR *)(LPCTSTR) & SLPSetChannelAllReq, sizeof(__SLPSetChannel16Req));
		}
		break;
	default:
		return;
	}
}

BOOL CNeepsLight::InitializeLight(CString strInitFilePath)
{
	TCHAR szSimulationFlag[6] = {0,};
	m_nPort		= GetPrivateProfileInt(_T("Settings"), _T("PORT"), 2, strInitFilePath);	
	m_nBaudRate  = GetPrivateProfileInt(_T("Settings"),_T("BAUDRATE"), 19200, strInitFilePath);
	m_nControllerCnt = GetPrivateProfileInt(_T("Settings"),_T("CONTROLLER_COUNT"), 4, strInitFilePath);	// 현재 포트에 연결된 Controller 개수
	m_nChannelCnt		= GetPrivateProfileInt(_T("Settings"),_T("CHANNEL_COUNT "), 12, strInitFilePath);

	m_pThreadCheckLight = AfxBeginThread(ThreadCheckLight, LPVOID(this)); 

	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);
	if( !_tcscmp(szSimulationFlag, _T("TRUE")) )
		return TRUE;
	else	
		return ConnectLight(m_nPort, m_nBaudRate);
}



UINT CNeepsLight::ThreadCheckLight(LPVOID lParam)
{ 
	//theApp.m_pInternalServer->m_fnCmdEditSend()
	CNeepsLight* pThis = (CNeepsLight*)lParam;
	int nControllerNum = 0;
	while(1)
	{
		for(int i = 0 ; i < pThis->GetControllerCount() ; i++)
		{
			Sleep(500);
			nControllerNum = i+1;
			//18.10.18 Light Task 조명 Count를 .ini 파일에서 불러오도록 수정
			theApp.m_pInternalServer->m_fnCmdEditSend(VS_SEND_CHECK_LIGHT,0,(USHORT)sizeof(int),theApp.GetVsTaskNum(),(BYTE*)&nControllerNum,CMD_TYPE_NORES);
		}
		Sleep(30000);
	}


	return 0;
}

BYTE CNeepsLight::CheckProtocol(BYTE *pBuff, WORD nByte)
{
	WORD					nCRC;

	if ( nByte < DSMP_MIN_DATA_LEN )
	{
		return DSMP_NOCOMPLETE;
	}
	nCRC = ((WORD)pBuff[nByte - 1]) << 8 | pBuff[nByte - 2];
	if ( nCRC != GetCrc(pBuff, nByte - 2) )
	{
		return DSMP_CRCERROR;
	}

	return DSMP_NOERROR;
}

void CNeepsLight::CheckLight(int nControllerCount)
{
	if ( !m_SPort )
	{
		return;
	}

	int						i;
	BYTE					InBuf[MAX_BUFF_SIZE] = { 0, };
	WORD					nByte = sizeof(__SNLC_M12GetStatusResp);
	DWORD					nReadByte = 0;
	__SNLC_M12GetStatusReq		SNLC_M12GetStatusReq;
	WORD					Crc;
	//CButton					*pButton;
	
	//WORD					WData[MAX_CHANNEL_SIZE];

	SNLC_M12GetStatusReq.SHeader.TID		= (BYTE)nControllerCount; // 컨트롤러번호
	SNLC_M12GetStatusReq.SHeader.PType		= PTYPE_REQUEST;
	SNLC_M12GetStatusReq.SHeader.PID		= PID_GET_STATUS;
	SNLC_M12GetStatusReq.SHeader.Status		= PKSTATUS_OK;
	SNLC_M12GetStatusReq.SHeader.Len			= ConvertWord((WORD)sizeof(__SNLC_M12GetStatusReq));

	Crc = GetCrc((BYTE *)&SNLC_M12GetStatusReq, sizeof(__SNLC_M12GetStatusReq) - sizeof(__SCrc));
	SNLC_M12GetStatusReq.SCrc.CrcL = Crc & 0xFF;
	SNLC_M12GetStatusReq.SCrc.CrcH = (Crc >> 8) & 0xFF;

	m_SPort.Write(&SNLC_M12GetStatusReq, sizeof(__SNLC_M12GetStatusReq));

	nReadByte = m_SPort.Read(InBuf, nByte);
	__SNLC_M12GetStatusResp	SNLC_M12GetStatusResp;
	CString	strLog, strStatus,strDuty,strTemperature,strTemp;
	if ( CheckProtocol(InBuf, nByte) == DSMP_NOERROR )
	{
		memcpy(&SNLC_M12GetStatusResp, InBuf, nByte);
		SNLC_M12GetStatusResp.LEDStatus = ConvertWord(SNLC_M12GetStatusResp.LEDStatus);
 		strStatus+= SNLC_M12GetStatusResp.LEDStatus==4095?_T("ON"):_T("OFF");

		for (i = 0; i < 2 ; i++ )
		{
			SNLC_M12GetStatusResp.Temperature[i] = ConvertWord(SNLC_M12GetStatusResp.Temperature[i]);

			strTemp.Format(_T("%d"), SNLC_M12GetStatusResp.Temperature[i]);
			strTemp+= i==1?_T(""):_T(",");
			strTemperature += strTemp;
		}
		SNLC_M12GetStatusResp.SetErrorTemp = ConvertWord(SNLC_M12GetStatusResp.SetErrorTemp);
		SNLC_M12GetStatusResp.CM = ConvertWord(SNLC_M12GetStatusResp.CM);

		for ( i = 0; i < GetChannelCount(); i++ )
		{
			SNLC_M12GetStatusResp.Duty[i] = ConvertWord(SNLC_M12GetStatusResp.Duty[i]);
			strTemp.Format(_T("%d"), SNLC_M12GetStatusResp.Duty[i]);
			strTemp+= i==GetChannelCount()-1?_T(""):_T(",");
			strDuty += strTemp;
		}

 		strLog.Format(_T("Controller - %d OK, LED Status - %s, Temperature - %s, LightValue - %s, Error - %d"),nControllerCount,strStatus,strTemperature,strDuty, SNLC_M12GetStatusResp.Error);
	}
	else
	{
		switch(CheckProtocol(InBuf, nByte))
		{
		case DSMP_NOCOMPLETE:
			strLog.Format(_T("Controller - %d NG, DSMP_NOCOMPLETE"),nControllerCount);
			break;
		case DSMP_CRCERROR:
			strLog.Format(_T("Controller - %d NG, DSMP_CRCERROR"),nControllerCount);
			break;
		default:
			break;
		}
		
		nReadByte = m_SPort.Read(InBuf, nByte * 2);
	}

	strLog+= nControllerCount==GetControllerCount()?_T("\n"):_T("");
	theApp.m_pLogWriter->m_fnWriteLog(strLog);
}